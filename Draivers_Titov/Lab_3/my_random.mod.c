#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x3cd2d66b, "class_create" },
	{ 0xc1e6c71e, "__mutex_init" },
	{ 0xc130cdce, "cdev_init" },
	{ 0xa5782ab4, "cdev_add" },
	{ 0x2035aec5, "device_create" },
	{ 0x66f023d0, "input_register_handler" },
	{ 0xe8213e80, "_printk" },
	{ 0x5d1ebb98, "device_destroy" },
	{ 0x0537a61b, "cdev_del" },
	{ 0x690fb1ed, "class_destroy" },
	{ 0x0bc5fb0d, "unregister_chrdev_region" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x357aaab3, "mutex_lock_interruptible" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xf46d5bf3, "mutex_unlock" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0xd0cba107, "input_unregister_handler" },
	{ 0x058c185a, "jiffies" },
	{ 0x8880cbe8, "input_register_handle" },
	{ 0x8880cbe8, "input_open_device" },
	{ 0xd272d446, "__fentry__" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x17c1ce46, "input_close_device" },
	{ 0x17c1ce46, "input_unregister_handle" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0xfc961df9, "kmalloc_caches" },
	{ 0xe18fddbc, "__kmalloc_cache_noprof" },
	{ 0xe1e1f979, "_raw_spin_lock_irqsave" },
	{ 0x81a1a811, "_raw_spin_unlock_irqrestore" },
	{ 0x9f222e1e, "alloc_chrdev_region" },
	{ 0xd268ca91, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x3cd2d66b,
	0xc1e6c71e,
	0xc130cdce,
	0xa5782ab4,
	0x2035aec5,
	0x66f023d0,
	0xe8213e80,
	0x5d1ebb98,
	0x0537a61b,
	0x690fb1ed,
	0x0bc5fb0d,
	0xd272d446,
	0x357aaab3,
	0x092a35a2,
	0xf46d5bf3,
	0x092a35a2,
	0xd0cba107,
	0x058c185a,
	0x8880cbe8,
	0x8880cbe8,
	0xd272d446,
	0xd272d446,
	0x17c1ce46,
	0x17c1ce46,
	0xcb8b6ec6,
	0xbd03ed67,
	0xfc961df9,
	0xe18fddbc,
	0xe1e1f979,
	0x81a1a811,
	0x9f222e1e,
	0xd268ca91,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"class_create\0"
	"__mutex_init\0"
	"cdev_init\0"
	"cdev_add\0"
	"device_create\0"
	"input_register_handler\0"
	"_printk\0"
	"device_destroy\0"
	"cdev_del\0"
	"class_destroy\0"
	"unregister_chrdev_region\0"
	"__stack_chk_fail\0"
	"mutex_lock_interruptible\0"
	"_copy_from_user\0"
	"mutex_unlock\0"
	"_copy_to_user\0"
	"input_unregister_handler\0"
	"jiffies\0"
	"input_register_handle\0"
	"input_open_device\0"
	"__fentry__\0"
	"__x86_return_thunk\0"
	"input_close_device\0"
	"input_unregister_handle\0"
	"kfree\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"_raw_spin_lock_irqsave\0"
	"_raw_spin_unlock_irqrestore\0"
	"alloc_chrdev_region\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D32EAF06C25F48B7E2DD652");
