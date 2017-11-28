#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/nsproxy.h>
#include <../fs/mount.h>
#include "hello_world.h"

#define PROC_FILENAME "mymounts"

static struct proc_dir_entry *proc_file;

static void *seq_start(struct seq_file *s, loff_t *pos)
{
	struct mount *mnt;
	loff_t i;

	if (*pos >= current->nsproxy->mnt_ns->mounts)
		return NULL;
	mnt = list_first_entry(&current->nsproxy->mnt_ns->list,
				struct mount,
				mnt_list);
	for (i = 0; i < *pos; i++)
		mnt = list_next_entry(mnt, mnt_list);
	return mnt;
}

static void *seq_next(struct seq_file *sfile, void *v, loff_t *pos)
{
	struct mount *mnt;

	(*pos)++;
	if (*pos >= current->nsproxy->mnt_ns->mounts)
		return NULL;
	mnt = (struct mount *)v;
	mnt = list_next_entry(mnt, mnt_list);
	return mnt;
}

static void seq_stop(struct seq_file *sfile, void *v)
{
	(void)sfile;
	(void)v;
}

static int seq_show(struct seq_file *sfile, void *v)
{
	struct mount	*mnt;
	char		buf[512];
	struct path	path;
	char		*pathstring;


	mnt = (struct mount *)v;
	path.mnt = &mnt->mnt_parent->mnt;
	path.dentry = mnt->mnt_mountpoint;
	pathstring = d_path(&path, buf, 512);
	seq_printf(sfile, "%-20s\t%s\n", mnt->mnt_devname, pathstring);

	return 0;
}

static const struct seq_operations ft_seq_ops = {
	.start = seq_start,
	.next = seq_next,
	.stop = seq_stop,
	.show = seq_show,
};

static int ft_proc_open(struct inode *inodep, struct file *file)
{
	(void)inodep;
	return seq_open(file, &ft_seq_ops);
}

static const struct file_operations ft_fops = {
	.owner		= THIS_MODULE,
	.open		= ft_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

int	ft_create_proc_entry(void)
{
	proc_file = proc_create(PROC_FILENAME, 0, NULL, &ft_fops);
	if (!proc_file)
		return -ENOMEM;
	return 0;
}

void	ft_delete_proc_entry(void)
{
	remove_proc_entry(PROC_FILENAME, NULL);
}
