#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include "hello_world.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thomas Vermeilh <thomas.vermeilh@gmail.com>");
MODULE_DESCRIPTION("Hello World module");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
	int retval = 0;

	printk(KERN_INFO "Hello world !\n");

	retval = ft_create_proc_entry();

	return retval;
}

static void __exit hello_cleanup(void)
{
	printk(KERN_INFO "Cleaning up module.\n");

	ft_delete_proc_entry();
}

module_init(hello_init);
module_exit(hello_cleanup);
