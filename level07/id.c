#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define STUDENT_LOGIN		"tvermeil\n"
#define STUDENT_LOGIN_LEN	10

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

const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = ft_read,
	.write = ft_write,
};
