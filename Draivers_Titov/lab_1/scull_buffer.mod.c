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
	{ 0xcb8b6ec6, "kfree" },
	{ 0x690fb1ed, "class_destroy" },
	{ 0x0bc5fb0d, "unregister_chrdev_region" },
	{ 0x357aaab3, "mutex_lock_interruptible" },
	{ 0xf46d5bf3, "mutex_unlock" },
	{ 0x7851be11, "__SCT__might_resched" },
	{ 0x7a5ffe84, "init_wait_entry" },
	{ 0xd272d446, "schedule" },
	{ 0x0db8d68d, "prepare_to_wait_event" },
	{ 0xc87f4bab, "finish_wait" },
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x16ab4215, "__wake_up" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0x9f222e1e, "alloc_chrdev_region" },
	{ 0x3cd2d66b, "class_create" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0xfc961df9, "kmalloc_caches" },
	{ 0xe18fddbc, "__kmalloc_cache_noprof" },
	{ 0xc1e6c71e, "__mutex_init" },
	{ 0x5403c125, "__init_waitqueue_head" },
	{ 0xc130cdce, "cdev_init" },
	{ 0xa5782ab4, "cdev_add" },
	{ 0x2035aec5, "device_create" },
	{ 0xa53f4e29, "memcpy" },
	{ 0xe54e0a6b, "__fortify_panic" },
	{ 0xf64ac983, "__copy_overflow" },
	{ 0xd272d446, "__fentry__" },
	{ 0xc9b02247, "const_pcpu_hot" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x5d1ebb98, "device_destroy" },
	{ 0x0537a61b, "cdev_del" },
	{ 0xd268ca91, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xcb8b6ec6,
	0x690fb1ed,
	0x0bc5fb0d,
	0x357aaab3,
	0xf46d5bf3,
	0x7851be11,
	0x7a5ffe84,
	0xd272d446,
	0x0db8d68d,
	0xc87f4bab,
	0xa61fd7aa,
	0x092a35a2,
	0x16ab4215,
	0xd272d446,
	0x092a35a2,
	0x9f222e1e,
	0x3cd2d66b,
	0xbd03ed67,
	0xfc961df9,
	0xe18fddbc,
	0xc1e6c71e,
	0x5403c125,
	0xc130cdce,
	0xa5782ab4,
	0x2035aec5,
	0xa53f4e29,
	0xe54e0a6b,
	0xf64ac983,
	0xd272d446,
	0xc9b02247,
	0xe8213e80,
	0xd272d446,
	0x90a48d82,
	0x5d1ebb98,
	0x0537a61b,
	0xd268ca91,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"kfree\0"
	"class_destroy\0"
	"unregister_chrdev_region\0"
	"mutex_lock_interruptible\0"
	"mutex_unlock\0"
	"__SCT__might_resched\0"
	"init_wait_entry\0"
	"schedule\0"
	"prepare_to_wait_event\0"
	"finish_wait\0"
	"__check_object_size\0"
	"_copy_to_user\0"
	"__wake_up\0"
	"__stack_chk_fail\0"
	"_copy_from_user\0"
	"alloc_chrdev_region\0"
	"class_create\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"__mutex_init\0"
	"__init_waitqueue_head\0"
	"cdev_init\0"
	"cdev_add\0"
	"device_create\0"
	"memcpy\0"
	"__fortify_panic\0"
	"__copy_overflow\0"
	"__fentry__\0"
	"const_pcpu_hot\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"__ubsan_handle_out_of_bounds\0"
	"device_destroy\0"
	"cdev_del\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "800355566F8B76F951A83BA");
