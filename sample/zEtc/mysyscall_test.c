#include <stdio.h>
#include <linux/unistd.h>

int mysyscall()
{
	int i;
	//i = syscall(338);
	i = syscall(300);
	return i;
}

int main()
{
	int i;
	i = mysyscall();
	printf("return = %d\n",i);
	return 0;
}


