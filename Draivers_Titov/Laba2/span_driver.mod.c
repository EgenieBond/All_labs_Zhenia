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
	{ 0x7e85caa5, "dev_get_by_name" },
	{ 0xc609ff70, "strncpy" },
	{ 0x378f72ff, "netdev_rx_handler_register" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x03c877ed, "skb_clone" },
	{ 0xd485a5cd, "__dev_queue_xmit" },
	{ 0xe4de56b4, "__ubsan_handle_load_invalid_value" },
	{ 0x210598c9, "netdev_rx_handler_unregister" },
	{ 0x2d4f4516, "param_ops_charp" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0x30b0617d, "init_net" },
	{ 0xd268ca91, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x7e85caa5,
	0xc609ff70,
	0x378f72ff,
	0xd272d446,
	0x03c877ed,
	0xd485a5cd,
	0xe4de56b4,
	0x210598c9,
	0x2d4f4516,
	0xd272d446,
	0xe8213e80,
	0x30b0617d,
	0xd268ca91,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"dev_get_by_name\0"
	"strncpy\0"
	"netdev_rx_handler_register\0"
	"__x86_return_thunk\0"
	"skb_clone\0"
	"__dev_queue_xmit\0"
	"__ubsan_handle_load_invalid_value\0"
	"netdev_rx_handler_unregister\0"
	"param_ops_charp\0"
	"__fentry__\0"
	"_printk\0"
	"init_net\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C4E67E80A4F971DA1E3A931");
