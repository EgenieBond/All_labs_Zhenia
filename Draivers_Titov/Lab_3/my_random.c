/*
make
sudo insmod my_random.ko

sudo ./random_user
sudo ./entropier

ctrl+c   для остановки терминала

*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/version.h>
#include <linux/spinlock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Евгения Бондаренко");
MODULE_DESCRIPTION("Random Driver with Mouse and Keyboard Entropy");

#define DEVICE_NAME "my_rand"
#define MAX_ENTROPY_VALUES 256  // Увеличили буфер

struct node {
    long value;
    struct node* next;
};

struct result {
    long value;
    long seed;
};

struct rand{
    struct cdev cdev;
    dev_t devno;
    struct node *entropy;
    struct node *entropy_tail;  // Указатель на хвост для быстрого добавления
    struct mutex lock;
    spinlock_t spinlock;
    int entropy_count;
};

static struct rand device;
static int major_num = 0;
static struct class *rand_class = NULL;
static struct input_handler rand_input_handler;

static int rand_open(struct inode *inode, struct file *filp);
static int rand_release(struct inode *inode, struct file *filp);
static long rand_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static ssize_t rand_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){return -ENOSYS;}
static ssize_t rand_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){return -ENOSYS;}

static void random_number(struct result* res, long seed)
{
    static long last_seed = 123;
    static long last_value = 123;

    if(last_seed != seed) {
        last_value = last_seed = seed;
    }

    last_value = last_value * 1103515245 + 12345;
    res->seed = last_seed;
    res->value = (last_value >> 16) & 0x7FFFFFFF;
}

static struct file_operations rand_fops = {
    .owner = THIS_MODULE,
    .open = rand_open,
    .release = rand_release,
    .read = rand_read,
    .write = rand_write,
    .unlocked_ioctl = rand_ioctl
};

static void add_entropy_safe(long entropy_value)
{
    struct rand *dev = &device;
    unsigned long flags;
    struct node *new_node, *old_head;

    new_node = kmalloc(sizeof(struct node), GFP_ATOMIC);
    if (!new_node) {
        return;
    }

    new_node->value = entropy_value;
    new_node->next = NULL;

    spin_lock_irqsave(&dev->spinlock, flags);
    
    if (dev->entropy_count < MAX_ENTROPY_VALUES) {
        // Добавляем новый элемент в конец
        if (!dev->entropy) {
            dev->entropy = new_node;
            dev->entropy_tail = new_node;
        } else {
            dev->entropy_tail->next = new_node;
            dev->entropy_tail = new_node;
        }
        dev->entropy_count++;
    } else {
        // Циклический буфер: удаляем старый элемент из начала, добавляем новый в конец
        if (dev->entropy != NULL) {
            // Удаляем первый элемент
            old_head = dev->entropy;
            dev->entropy = dev->entropy->next;
            
            // Обновляем хвост если нужно
            if (old_head == dev->entropy_tail) {
                dev->entropy_tail = dev->entropy;
            }
            
            kfree(old_head);
            
            // Добавляем новый элемент в конец
            if (dev->entropy_tail) {
                dev->entropy_tail->next = new_node;
            } else {
                dev->entropy = new_node;
            }
            dev->entropy_tail = new_node;
        }
        // dev->entropy_count остается тем же
    }
    
    spin_unlock_irqrestore(&dev->spinlock, flags);
}

static void rand_input_event(struct input_handle *handle,
                           unsigned int type, unsigned int code, int value)
{
    long entropy_value;
    
    // Собираем энтропию от движений мыши и нажатий клавиш
    if (type == EV_REL || (type == EV_KEY && value == 1)) {
        entropy_value = (long)(jiffies ^ type ^ code ^ value);
        add_entropy_safe(entropy_value);
        
        printk(KERN_DEBUG "rand: Added entropy: %ld (device: %s)\n", 
               entropy_value, handle->dev->name);
    }
}

static int rand_input_connect(struct input_handler *handler,
                             struct input_dev *dev,
                             const struct input_device_id *id)
{
    struct input_handle *handle;
    int error;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "rand_input";

    error = input_register_handle(handle);
    if (error) {
        pr_err("rand: Failed to register input handle\n");
        kfree(handle);
        return error;
    }

    error = input_open_device(handle);
    if (error) {
        pr_err("rand: Failed to open input device\n");
        input_unregister_handle(handle);
        kfree(handle);
        return error;
    }

    pr_info("rand: Connected to input device: %s\n", dev->name);
    return 0;
}

static void rand_input_disconnect(struct input_handle *handle)
{
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id rand_input_ids[] = {
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_KEY) },
    },
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT, 
        .evbit = { BIT_MASK(EV_REL) },
    },
    { }
};

static int rand_open(struct inode *inode, struct file *filp)
{
    struct rand *dev;
    int minor = iminor(inode);

    if (minor >= 1) {
        return -ENODEV;
    }

    dev = &device;
    filp->private_data = dev;
    return 0;
}

static int rand_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static int __init rand_init(void)
{
    int err;
    dev_t dev_num = 0;

    err = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (err < 0) {
        pr_err("rand: Failed to allocate device numbers\n");
        return err;
    }
    major_num = MAJOR(dev_num);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    rand_class = class_create(DEVICE_NAME);
#else
    rand_class = class_create(THIS_MODULE, DEVICE_NAME);
#endif

    if (IS_ERR(rand_class)) {
        pr_err("rand: Failed to create device class\n");
        err = PTR_ERR(rand_class);
        goto fail_class;
    }

    struct rand *dev = &device;
    mutex_init(&dev->lock);
    spin_lock_init(&dev->spinlock);
    dev->entropy_count = 0;
    dev->entropy = NULL;
    dev->entropy_tail = NULL;
    dev->devno = MKDEV(major_num, 0);

    cdev_init(&dev->cdev, &rand_fops);
    dev->cdev.owner = THIS_MODULE;

    err = cdev_add(&dev->cdev, dev->devno, 1);
    if (err) {
        pr_err("rand: Error %d adding device\n", err);
        goto fail_device;
    }

    device_create(rand_class, NULL, dev->devno, NULL, "rand");

    // Инициализируем обработчик событий ввода
    rand_input_handler.event = rand_input_event;
    rand_input_handler.connect = rand_input_connect;
    rand_input_handler.disconnect = rand_input_disconnect;
    rand_input_handler.name = "rand_input";
    rand_input_handler.id_table = rand_input_ids;

    err = input_register_handler(&rand_input_handler);
    if (err) {
        pr_err("rand: Failed to register input handler\n");
        goto fail_input;
    }

    pr_info("rand: Device /dev/rand created\n");
    pr_info("rand: Module loaded successfully (major number = %d)\n", major_num);
    pr_info("rand: Mouse and keyboard entropy collection enabled\n");
    return 0;

fail_input:
    device_destroy(rand_class, device.devno);
fail_device:
    device_destroy(rand_class, device.devno);
    cdev_del(&device.cdev);
    
    // Освобождаем память при ошибке инициализации
    struct node *curr = device.entropy;
    while (curr != NULL) {
        struct node *next_node = curr->next;
        kfree(curr);
        curr = next_node;
    }
    
    class_destroy(rand_class);
fail_class:
    unregister_chrdev_region(dev_num, 1);
    return err;
}

static void __exit rand_exit(void)
{
    input_unregister_handler(&rand_input_handler);
    device_destroy(rand_class, device.devno);
    cdev_del(&device.cdev);
    
    // Освобождаем память энтропии
    struct node *curr = device.entropy;
    while (curr != NULL) {
        struct node *next_node = curr->next;
        kfree(curr);
        curr = next_node;
    }
    
    class_destroy(rand_class);
    unregister_chrdev_region(MKDEV(major_num, 0), 1);
    pr_info("rand: Module unloaded\n");
}

static long rand_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    void __user *user_arg = (void __user *)arg;
    struct rand *dev = filp->private_data;
    int retval = 0;

    if (mutex_lock_interruptible(&dev->lock)) {
        return -ERESTARTSYS;
    }

    switch (cmd) {
    case 0:
    {
        long seed;
        if(!dev->entropy)
        {
            seed = 123;
            goto _random;
        }

        // Берем ПОСЛЕДНИЙ элемент (самые свежие данные)
        struct node* next = dev->entropy;
        long last_value = 0;
        
        while(next != NULL) 
        {
            last_value = next->value;
            next = next->next;
        }
        seed = last_value;

_random:
        struct result value;
        random_number(&value, seed);

        if (copy_to_user(user_arg, &value, sizeof(struct result))) {
            retval = -EFAULT;
        }
        break;
    }
    case 1:
    {
        long value;
        if (copy_from_user(&value, user_arg, sizeof(long))) {
            retval = -EFAULT;
            break;
        }

        add_entropy_safe(value);
        break;
    }
    default:
        retval = -ENOTTY;
    }

    mutex_unlock(&dev->lock);
    return retval;
}

module_init(rand_init);
module_exit(rand_exit);