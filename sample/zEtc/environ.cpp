/*********************
* 전역환경변수 예제 
***********************/

#include <stdio.h>
//#include <string.h>
//#include <unistd.h>
//#include <errno.h>

extern char **environ;

int main()
{
	fprintf(stderr,"The environment list : \n");
	for (int i=0; environ[i] != NULL; i++)
		fprintf(stderr,"environ[%d] : %s\n",i,environ[i]);

	return 0;
}


