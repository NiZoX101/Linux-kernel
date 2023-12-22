#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#define procfs_name "tsu"
static struct proc_dir_entry* our_proc_file = NULL;

static ssize_t procfile_read(struct file* file_pointer, char __user* buffer, size_t buffer_length, loff_t* offset)
{
	int len;
	char s[32]; 
	len = snprintf(s, sizeof(s), "Number of CPUs: %d\n", num_online_cpus());

	if (*offset > 0 || buffer_length < len)
		return 0;
	if (copy_to_user(buffer, s, len))
		return -EFAULT;
	*offset += len;
	pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
	return len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_fops =
{
	.proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops =
{
	.read = procfile_read,
};
#endif

static int __init procfs1_init(void)
{
	our_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops);
	if (!our_proc_file)
	{
		pr_err("Error creating proc file\n");
		return -ENOMEM;
	}
	pr_info("/proc/%s created\n", procfs_name);
	return 0;
}

static void __exit procfs1_exit(void)
{
	proc_remove(our_proc_file);
	pr_info("/proc/%s removed\n", procfs_name);
}

module_init(procfs1_init);
module_exit(procfs1_exit);

MODULE_LICENSE("GPL");
