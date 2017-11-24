#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include "hello_world.h"

static struct {
	void	*page;
	size_t	len;
} foo_struct = {
	.page = NULL,
	.len = 0,
};

static ssize_t ft_read(struct file *filep,
			char __user *buffer,
			size_t len,
			loff_t *offset)
{
	int retval = 0;
	size_t to_copy;
	size_t copied;

	(void)filep;

	printk(KERN_INFO "debugfs_hello_world: read was called, current len is %zu\n", foo_struct.len);
	/* TODO Get the lock */
	if (*offset >= foo_struct.len)
	{
		/* TODO Realease the lock */
		return 0;
	}
	to_copy = foo_struct.len - *offset;
	to_copy = min(to_copy, len);
	retval = copy_to_user(buffer, foo_struct.page + *offset, to_copy);
	/* TODO realease the lock */
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

	(void)filep;
	(void)offset;

	if (*offset >= PAGE_SIZE)
		return -EINVAL;
	if (foo_struct.page == NULL)
		return -EINVAL;
	len = min(PAGE_SIZE - foo_struct.len, len);

	/* TODO Get the lock ! */
	retval = copy_from_user(foo_struct.page, buffer, len);
	if (retval == 0)
		foo_struct.len = *offset + len;
	/* TODO Realease the lock ! */
	if (retval != 0)
		return -EINVAL;
	*offset += len;

	return len;
}

const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = ft_read,
	.write = ft_write,
};

void create_foo_file(struct ft_debugfs_entries_struct *entries)
{
	foo_struct.page = (void *)get_zeroed_page(GFP_KERNEL);
	if (foo_struct.page == NULL)
	{
		printk(KERN_ALERT "debugfs_hello_world: failed to allocate one page for foo\n");
		return;
	}

	entries->foo_file = debugfs_create_file("foo", 0644, entries->dir, NULL, &foo_fops);
	if (IS_ERR(entries->foo_file) || entries->foo_file == NULL)
	{
		entries->foo_file = NULL;
		printk(KERN_ALERT "debugfs_hello_world: failed to create file foo\n");
		delete_foo_file(entries);
	}
}

void delete_foo_file(struct ft_debugfs_entries_struct *entries)
{
	if (foo_struct.page) {
		/* TODO Get the lock ! */
		free_page((unsigned long)foo_struct.page);
		foo_struct.page = NULL;
		/* TODO Realease the lock ! */
	}

	if (entries->foo_file)
		debugfs_remove(entries->foo_file);
}
