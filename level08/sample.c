#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/rwsem.h>

/* Dont have a license, LOL */

MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user,
		size_t size, loff_t *offs);

static ssize_t myfd_write(struct file *fp, const char __user *user,
		size_t size, loff_t *offs);

static const struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

char str[PAGE_SIZE];
struct rw_semaphore sem;

static int __init myfd_init(void)
{
	int retval;

	str[0] = '\0';
	init_rwsem(&sem);
	retval = misc_register(&myfd_device);
	return 0;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp,
			char __user *user,
			size_t size,
			loff_t *offs)
{
	ssize_t t, i, ret;
	char *alloc;

	/*
	 * Malloc like a boss
	 */
	alloc = kmalloc(sizeof(str), GFP_KERNEL);
	down_read(&sem);
	for (t = strnlen(str, sizeof(str)) - 1, i = 0; t >= 0; t--, i++)
		alloc[i] = str[t];
	alloc[i] = 0x0;
	up_read(&sem);
	ret = simple_read_from_buffer(user, size, offs, alloc, i);
	kfree(alloc);
	return ret;
}

ssize_t myfd_write(struct file *fp,
			const char __user *user,
			size_t size,
			loff_t *offs)
{
	ssize_t res;

	res = 0;
	down_write(&sem);
	res = simple_write_to_buffer(str, size, offs, user, sizeof(str));
	if (res > 0) {
		/* 0x0 = ’\0’ */
		str[res] = 0x0;
		res++;
	} else if (res == 0)
		res = -EINVAL;
	up_write(&sem);
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
