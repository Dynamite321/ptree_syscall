
/* Project 1, problem 2 */
/* test file by Yanjun Fu, 516030910354 */

#include <stdio.h>
#include <stdlib.h>

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

int depth[5000]; // record the depth of the process in the process tree

int get_depth(struct prinfo *buf, int i) // get the depth of the process in the process tree
{
	if (i == 0)
		return 0;
	int j = i-1;
	while (j >= 0 && buf[j].pid != buf[i].parent_pid) // find its parent process
		--j;
	depth[i] = depth[j] + 1; // its depth is equal to parent's depth plus 1
	return depth[i];
}

int main(int argc, char **argv)
{
	struct prinfo *buf = malloc(1500 * sizeof(struct prinfo));
	int *nr = malloc(sizeof(int));
	syscall(287,buf,nr); // call the new system call pstree
	int n = *nr, deep = 0; // n is the number of processes
	int i = 0, j = 0;
	for (i; i < n; ++i)
		depth[i] = 0; // initialize the depth array
	printf("%d\n", n); // print the number of current processes
	
	for (i = 0; i < n; ++i)
	{
		deep = get_depth(buf, i);
		/* we use the number of tabs to denote the parent-child relation, 
		the depth array is used to determine how many tabs shoul be printed */
		for (j = 0; j < deep; ++j) 
			printf("\t");
		printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[i].comm, buf[i].pid, buf[i].state, buf[i].parent_pid, 
			buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);
	}
	// free the space allocated by malloc
	free(buf);
	free(nr);

	return 0;
}
