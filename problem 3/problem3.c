
/* Project 1, problem 3 */
/* problem 3 file programmed by Yanjun Fu, 516030910354 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	pid_t parent = getpid();
	pid_t pid = fork();
	pid_t child;

	if (pid != 0)
	{
		printf("516030910354_Parent, %d\n", parent);
		wait(1000);
	}
	else
	{
		child = getpid();
		printf("516030910354_Child, %d\n", child);
		execl("./testARM","testARM",NULL); // call the pstree system call
	}
	return 0;
}
