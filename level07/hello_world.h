#ifndef HELLO_WORLD_H
# define HELLO_WORLD_H

struct ft_debugfs_entries_struct {
	struct dentry *dir;
	struct dentry *id_file;
	struct dentry *jiffies_file;
	struct dentry *foo_file;
};


/* debugfs.c */
void ft_delete_debugfs_entries(void);
void ft_create_debugfs_entries(void);

/* foo.c */
void create_foo_file(struct ft_debugfs_entries_struct *entries);
void delete_foo_file(struct ft_debugfs_entries_struct *entries);

#endif
