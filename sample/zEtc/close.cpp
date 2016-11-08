/****************************
* close 함수 예제 
****************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	int nFildes=10,nMyErrno;

	if (close(nFildes) == -1) {
		fprintf(stderr,"Fail %d, %d,%s\n",nFildes,errno,strerror(errno));
		nMyErrno = errno;
		if (nMyErrno == EBADF) fprintf(stderr,"ER : EBADF\n");
		else if (nMyErrno == EINTR) fprintf(stderr,"ER : EINTR\n");
		else fprintf(stderr,"ER: %d\n",nMyErrno);
	}
	else {
		fprintf(stderr,"Succ %d\n",nFildes);
	}

	return 0;
}


