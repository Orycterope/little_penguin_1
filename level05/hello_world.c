#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#define STUDENT_LOGIN		"tvermeil\n"
#define STUDENT_LOGIN_LEN	10
#define DEVICE_NAME		"fortytwo"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thomas Vermeilh <thomas.vermeilh@gmail.com>");
MODULE_DESCRIPTION("Hello World module");
MODULE_VERSION("0.1");

static ssize_t ft_read(struct file *filep,
			char __user *buffer,
			size_t len,
			loff_t *offset)
{
	int retval = 0;
	int to_copy;
	int copied;

	(void)filep;

	if (*offset >= STUDENT_LOGIN_LEN)
		return 0;

	to_copy = STUDENT_LOGIN_LEN - *offset;
	retval = copy_to_user(buffer, STUDENT_LOGIN + *offset, to_copy);
	if (retval < 0)
		return retval;
	copied = to_copy - retval;
	*offset += copied;
	return copied;
}

static ssize_t ft_write(struct file *filep,
			const char __user *buffer,
			size_t len,
			loff_t *offset)
{
	int retval;
	char local_buf[STUDENT_LOGIN_LEN];

	(void)filep;
	(void)offset;

	/* We don't care about the \0 at the end */
	if (len < STUDENT_LOGIN_LEN - 1)
		return -EINVAL;
	retval = copy_from_user(local_buf, buffer, STUDENT_LOGIN_LEN - 1);
	if (retval != 0)
		return -EINVAL;
	retval = strncmp(STUDENT_LOGIN, local_buf, STUDENT_LOGIN_LEN - 1);
	if (retval != 0)
		return -EINVAL;

	return len;
}

static const struct file_operations ft_file_ops = {
	.owner = THIS_MODULE,
	.read = ft_read,
	.write = ft_write,
};

static struct miscdevice ft_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &ft_file_ops,
};

static int __init hello_init(void)
{
	int retval = 0;

	INIT_LIST_HEAD(&ft_device.list);

	printk(KERN_INFO "Hello world !\n");

	retval = misc_register(&ft_device);
	if (retval)
		goto register_err;

register_err:
	return retval;
}

static void __exit hello_cleanup(void)
{
	printk(KERN_INFO "Cleaning up module.\n");

	misc_deregister(&ft_device);
}


module_init(hello_init);
module_exit(hello_cleanup);
