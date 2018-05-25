
/* Poject 1, problem 1 */
/* pstree module programmed by Yanjun Fu, 516030910354 */

#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/unistd.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/syscalls.h>
#include<linux/uaccess.h>
#include<linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");
#define __NR_ptree 287

static int (*oldcall)(void);

struct prinfo
{
    pid_t parent_pid; 		    // process id of parent
    pid_t pid;			        // process id
    pid_t first_child_pid;	    // pid of youngest child
    pid_t next_sibling_pid; 	// pid of older sibling
    long state;			        // current state of process
    long uid;			        // user id of process owner
    char comm[64];		        // name of program executed
};

void copy_process(struct task_struct *ts, struct prinfo *p, int *d) // copy data from task_struct to struct prinfo
{
    struct list_head *lh;
    struct task_struct *tmp;
    p[*d].parent_pid = ts->parent->pid;
    p[*d].pid = ts->pid;
    p[*d].state = ts->state;
    p[*d].uid = ts->cred->uid;
    strcpy(p[*d].comm, ts->comm);
    /* copy first_children_pid from tast_struct */
    if(list_empty(&ts->children)) // judge if ts has children
		p[*d].first_child_pid = 0;
    else
    {
        tmp = list_entry((&ts->children)->next, struct task_struct, sibling); // get the first child of ts
        if (tmp == NULL)
        {
            p[*d].first_child_pid = 0;
        }
        else
        {
            p[*d].first_child_pid = tmp->pid;
        }
    }

    /* copy next_sibling_pid from task_struct */
    if(list_empty(&ts->sibling)) // judge if ts has sibling
        p[*d].next_sibling_pid = 0;
    else
    {
		list_for_each(lh, &(ts->parent->children)) // traverse the children of its parent
		{
		    tmp = list_entry(lh, struct task_struct, sibling);
			if (tmp->pid == ts->pid) // now we find ts
			{
				p[*d].next_sibling_pid = list_entry(lh->next, struct task_struct, sibling)->pid; // get its next sibling
				break;  // leave the for-loop and end the traverse
            }
		}
    }
}

void dfs_process(struct task_struct *t, struct prinfo *p, int *d) // use DFS to traverse the tree
{
    struct task_struct *tmp;
    struct list_head *l;
    if (t == NULL)  // the basic condition: there is no more process to visit;
        return;
    copy_process(t, p, d); // copy process information into struct prinfo and store it in *p
    *d = *d + 1;
    if (!list_empty(&t->children))
    {
        list_for_each(l, &t->children)
        {
            tmp = list_entry(l, struct task_struct, sibling);
            dfs_process(tmp, p, d); // recursively visit its children, increase the depth of recursion
        }
    }
}

int ptree (struct prinfo *buf, int *nr)
{
    struct prinfo *tmpBuf = kmalloc_array(500,sizeof(*buf),GFP_KERNEL);
    int *tmpNr = kmalloc(sizeof(*nr),GFP_KERNEL);
    // for temporarily storation of information of processes
    *tmpNr=0;
    read_lock(&tasklist_lock);
    dfs_process(&init_task,tmpBuf,tmpNr); // DFS and store the information of process into tmpBuf and tmpNr
    read_unlock(&tasklist_lock);
    copy_to_user(buf, tmpBuf, 500 * sizeof(*tmpBuf)); // buf get data copied from tmpBuf
    copy_to_user(nr, tmpNr, sizeof(*tmpNr));  // nr get data copied from tmpNr
    kfree(tmpBuf);
    kfree(tmpNr);
    // free the space allocated by kmalloc
    return 0;
}

static int addsyscall_init(void)
{
    long *syscall = (long*)0xc000d8c4;
    oldcall = (int(*)(void))(syscall[__NR_ptree]);
    syscall[__NR_ptree] = (unsigned long)ptree;
    printk(KERN_INFO "module load!\n");
    return 0;
}

static void addsyscall_exit(void)
{
    long *syscall = (long*)0xc000d8c4;
    syscall[__NR_ptree] = (unsigned long)oldcall;
    printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
