/*
make

# Загружаем драйвер
  sudo insmod scull_buffer.ko

# Даем права на чтение/запись всем пользователям
 sudo chmod 666 /dev/scull_buffer*

gcc -o process_a process_a.c
gcc -o process_b process_b.c
gcc -o process_c process_c.c
*/ 

//  ./process_a
//  ./process_b
//  ./process_c

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/version.h>

#define DEVICE_NAME "scull_buffer"
#define BUFFER_SIZE 1024
#define NUM_DEVICES 2
#define CONTENT_BUF_SIZE 256

// IOCTL команды
#define SCULL_IOCTL_GET_SIZE 0
#define SCULL_IOCTL_GET_CONTENT 1

struct scull_buffer {
    struct cdev cdev;
    dev_t devno;
    char *buffer;
    int read_index;
    int write_index;
    int data_size;
    struct mutex lock;
    wait_queue_head_t read_queue;
    wait_queue_head_t write_queue;
};

static struct scull_buffer devices[NUM_DEVICES];
static int major_num = 0;
static struct class *scull_class = NULL;

// Прототипы функций
static int scull_open(struct inode *inode, struct file *filp);
static int scull_release(struct inode *inode, struct file *filp);
static ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

static struct file_operations scull_fops = {
    .owner = THIS_MODULE,
    .open = scull_open,
    .release = scull_release,
    .read = scull_read,
    .write = scull_write,
    .unlocked_ioctl = scull_ioctl,
};

static int scull_open(struct inode *inode, struct file *filp)
{
    struct scull_buffer *dev;
    int minor = iminor(inode);

    if (minor >= NUM_DEVICES) {
        return -ENODEV;
    }

    dev = &devices[minor];
    filp->private_data = dev;

    pr_info("scull_buffer: Device %d opened by process %d (%s)\n", 
            minor, current->pid, current->comm);
    return 0;
}

static int scull_release(struct inode *inode, struct file *filp)
{
    pr_info("scull_buffer: Device %d closed by process %d (%s)\n", 
            iminor(inode), current->pid, current->comm);
    return 0;
}

static ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct scull_buffer *dev = filp->private_data;
    ssize_t retval = 0;
    int bytes_to_read;
    int bytes_read_first_part;

    pr_info("scull_buffer: Process %d (%s) attempting to read %zu bytes from device %d\n",
            current->pid, current->comm, count, iminor(filp->f_path.dentry->d_inode));

    if (mutex_lock_interruptible(&dev->lock))
        return -ERESTARTSYS;

    // Ждем, пока в буфере появятся данные для чтения
    while (dev->data_size == 0) {
        mutex_unlock(&dev->lock);

        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;

        pr_info("scull_buffer: Buffer empty, process %d (%s) going to sleep\n",
                current->pid, current->comm);
        pr_info("scull_buffer: Process %d (%s) entering wait queue for reading\n",
                current->pid, current->comm);
        
        if (wait_event_interruptible(dev->read_queue, (dev->data_size > 0))) {
            pr_info("scull_buffer: Process %d (%s) interrupted while waiting to read\n",
                    current->pid, current->comm);
            return -ERESTARTSYS;
        }
        
        pr_info("scull_buffer: Process %d (%s) woke up from read queue, data available: %d\n",
                current->pid, current->comm, dev->data_size);

        if (mutex_lock_interruptible(&dev->lock)) {
            pr_info("scull_buffer: Process %d (%s) interrupted while reacquiring lock after read wait\n",
                    current->pid, current->comm);
            return -ERESTARTSYS;
        }
        
        pr_info("scull_buffer: Process %d (%s) reacquired lock after read wait\n",
                current->pid, current->comm);
    }

    bytes_to_read = min(count, (size_t)dev->data_size);
    bytes_read_first_part = min(bytes_to_read, BUFFER_SIZE - dev->read_index);

    if (copy_to_user(buf, dev->buffer + dev->read_index, bytes_read_first_part)) {
        retval = -EFAULT;
        goto out;
    }

    if (bytes_to_read > bytes_read_first_part) {
        if (copy_to_user(buf + bytes_read_first_part, dev->buffer, 
                        bytes_to_read - bytes_read_first_part)) {
            retval = -EFAULT;
            goto out;
        }
    }

    dev->read_index = (dev->read_index + bytes_to_read) % BUFFER_SIZE;
    dev->data_size -= bytes_to_read;
    retval = bytes_to_read;

    pr_info("scull_buffer: Read %zu bytes from device %d. Data size: %d\n",
            retval, iminor(filp->f_path.dentry->d_inode), dev->data_size);

    // Будим процессы, ждущие в очереди записи
    pr_info("scull_buffer: Waking up write queue after read operation\n");
    wake_up_interruptible(&dev->write_queue);

out:
    mutex_unlock(&dev->lock);
    return retval;
}

static ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    struct scull_buffer *dev = filp->private_data;
    ssize_t retval = 0;
    int space_available;
    int bytes_to_write;
    int bytes_write_first_part;

    pr_info("scull_buffer: Process %d (%s) attempting to write %zu bytes to device %d\n",
            current->pid, current->comm, count, iminor(filp->f_path.dentry->d_inode));

    if (mutex_lock_interruptible(&dev->lock))
        return -ERESTARTSYS;

    space_available = BUFFER_SIZE - dev->data_size;

    // Ждем, пока в буфере появится свободное место для записи
    while (space_available == 0) {
        mutex_unlock(&dev->lock);

        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        
        pr_info("scull_buffer: Buffer full, process %d (%s) going to sleep\n",
                current->pid, current->comm);
        pr_info("scull_buffer: Process %d (%s) entering wait queue for writing\n",
                current->pid, current->comm);
        
        if (wait_event_interruptible(dev->write_queue, 
            (space_available = (BUFFER_SIZE - dev->data_size)) > 0)) {
            pr_info("scull_buffer: Process %d (%s) interrupted while waiting to write\n",
                    current->pid, current->comm);
            return -ERESTARTSYS;
        }
        
        pr_info("scull_buffer: Process %d (%s) woke up from write queue, available space: %d\n",
                current->pid, current->comm, space_available);

        if (mutex_lock_interruptible(&dev->lock)) {
            pr_info("scull_buffer: Process %d (%s) interrupted while reacquiring lock after write wait\n",
                    current->pid, current->comm);
            return -ERESTARTSYS;
        }
        
        pr_info("scull_buffer: Process %d (%s) reacquired lock after write wait\n",
                current->pid, current->comm);
    }

    bytes_to_write = min(count, (size_t)space_available);
    bytes_write_first_part = min(bytes_to_write, BUFFER_SIZE - dev->write_index);

    if (copy_from_user(dev->buffer + dev->write_index, buf, bytes_write_first_part)) {
        retval = -EFAULT;
        goto out;
    }

    if (bytes_to_write > bytes_write_first_part) {
        if (copy_from_user(dev->buffer, buf + bytes_write_first_part, 
                          bytes_to_write - bytes_write_first_part)) {
            retval = -EFAULT;
            goto out;
        }
    }

    dev->write_index = (dev->write_index + bytes_to_write) % BUFFER_SIZE;
    dev->data_size += bytes_to_write;
    retval = bytes_to_write;

    pr_info("scull_buffer: Wrote %zu bytes to device %d. Data size: %d\n",
            retval, iminor(filp->f_path.dentry->d_inode), dev->data_size);

    // Будим процессы, ждущие в очереди чтения
    pr_info("scull_buffer: Waking up read queue after write operation\n");
    wake_up_interruptible(&dev->read_queue);

out:
    mutex_unlock(&dev->lock);
    return retval;
}

static long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct scull_buffer *dev = filp->private_data;
    int retval = 0;

    if (mutex_lock_interruptible(&dev->lock))
        return -ERESTARTSYS;

    switch (cmd) {
    case SCULL_IOCTL_GET_SIZE:
        if (copy_to_user((int __user *)arg, &dev->data_size, sizeof(dev->data_size))) {
            retval = -EFAULT;
        }
        break;
        
    case SCULL_IOCTL_GET_CONTENT:
        if (dev->data_size > 0) {
            char kernel_buf[CONTENT_BUF_SIZE];
            int bytes_to_copy = min(dev->data_size, CONTENT_BUF_SIZE - 1);
            int bytes_first_part, bytes_second_part;
            
            // Копируем данные из кольцевого буфера в линейный
            bytes_first_part = min(bytes_to_copy, BUFFER_SIZE - dev->read_index);
            memcpy(kernel_buf, dev->buffer + dev->read_index, bytes_first_part);
            
            if (bytes_to_copy > bytes_first_part) {
                bytes_second_part = bytes_to_copy - bytes_first_part;
                memcpy(kernel_buf + bytes_first_part, dev->buffer, bytes_second_part);
            }
            
            kernel_buf[bytes_to_copy] = '\0'; // Добавляем нуль-терминатор
            
            if (copy_to_user((char __user *)arg, kernel_buf, bytes_to_copy + 1)) {
                retval = -EFAULT;
            }
        } else {
            char empty_buf[1] = "";
            if (copy_to_user((char __user *)arg, empty_buf, 1)) {
                retval = -EFAULT;
            }
        }
        break;
        
    default:
        retval = -ENOTTY;
    }

    mutex_unlock(&dev->lock);
    return retval;
}

static int __init scull_init(void)
{
    int i, err;
    dev_t dev_num = 0;

    err = alloc_chrdev_region(&dev_num, 0, NUM_DEVICES, DEVICE_NAME);
    if (err < 0) {
        pr_err("scull_buffer: Failed to allocate device numbers\n");
        return err;
    }
    major_num = MAJOR(dev_num);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    scull_class = class_create(DEVICE_NAME);
#else
    scull_class = class_create(THIS_MODULE, DEVICE_NAME);
#endif

    if (IS_ERR(scull_class)) {
        pr_err("scull_buffer: Failed to create device class\n");
        err = PTR_ERR(scull_class);
        goto fail_class;
    }

    for (i = 0; i < NUM_DEVICES; i++) {
        struct scull_buffer *dev = &devices[i];

        dev->buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
        if (!dev->buffer) {
            pr_err("scull_buffer: Failed to allocate buffer for device %d\n", i);
            err = -ENOMEM;
            goto fail_device;
        }

        mutex_init(&dev->lock);
        init_waitqueue_head(&dev->read_queue);
        init_waitqueue_head(&dev->write_queue);

        dev->read_index = 0;
        dev->write_index = 0;
        dev->data_size = 0;

        dev->devno = MKDEV(major_num, i);

        cdev_init(&dev->cdev, &scull_fops);
        dev->cdev.owner = THIS_MODULE;

        err = cdev_add(&dev->cdev, dev->devno, 1);
        if (err) {
            pr_err("scull_buffer: Error %d adding device %d\n", err, i);
            kfree(dev->buffer);
            goto fail_device;
        }

        device_create(scull_class, NULL, dev->devno, NULL, "scull_buffer%d", i);
        pr_info("scull_buffer: Device /dev/scull_buffer%d created\n", i);
    }

    pr_info("scull_buffer: Module loaded successfully (major number = %d)\n", major_num);
    return 0;

fail_device:
    while (--i >= 0) {
        device_destroy(scull_class, devices[i].devno);
        cdev_del(&devices[i].cdev);
        kfree(devices[i].buffer);
    }
    class_destroy(scull_class);

fail_class:
    unregister_chrdev_region(dev_num, NUM_DEVICES);
    return err;
}

static void __exit scull_exit(void)
{
    int i;

    for (i = 0; i < NUM_DEVICES; i++) {
        device_destroy(scull_class, devices[i].devno);
        cdev_del(&devices[i].cdev);
        kfree(devices[i].buffer);
    }

    class_destroy(scull_class);
    unregister_chrdev_region(MKDEV(major_num, 0), NUM_DEVICES);

    pr_info("scull_buffer: Module unloaded\n");
}

module_init(scull_init);
module_exit(scull_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Евгения Бондаренко");
MODULE_DESCRIPTION("Scull Driver with Circular Buffer and Locking");