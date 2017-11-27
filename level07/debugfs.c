#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include "hello_world.h"

static struct ft_debugfs_entries_struct ft_debugfs_entries = { 0 };

extern const struct file_operations id_fops;
extern const struct file_operations foo_fops;

void ft_create_debugfs_entries(void)
{
	ft_debugfs_entries.dir = debugfs_create_dir("fortytwo", NULL);
	if (IS_ERR(ft_debugfs_entries.dir) || ft_debugfs_entries.dir == NULL) {
		ft_debugfs_entries.dir = NULL;
		printk(KERN_ALERT "debugfs_hello_world: failed to create directory fortytwo\n");
		return;
	}
	ft_debugfs_entries.id_file = debugfs_create_file("id", 0666, ft_debugfs_entries.dir, NULL, &id_fops);
	if (IS_ERR(ft_debugfs_entries.id_file) || ft_debugfs_entries.id_file == NULL) {
		ft_debugfs_entries.id_file = NULL;
		printk(KERN_ALERT "debugfs_hello_world: failed to create file id\n");
	}

	ft_debugfs_entries.jiffies_file = debugfs_create_u64("jiffies", 0444, ft_debugfs_entries.dir, (u64 *)(&jiffies));
	if (IS_ERR(ft_debugfs_entries.jiffies_file) || ft_debugfs_entries.jiffies_file == NULL) {
		ft_debugfs_entries.jiffies_file = NULL;
		printk(KERN_ALERT "debugfs_hello_world: failed to create file jiffies\n");
	}

	create_foo_file(&ft_debugfs_entries);
}

void ft_delete_debugfs_entries(void)
{
	delete_foo_file(&ft_debugfs_entries);

	debugfs_remove(ft_debugfs_entries.jiffies_file);
	debugfs_remove(ft_debugfs_entries.id_file);
	debugfs_remove(ft_debugfs_entries.dir);
}
