/****************************
* 
****************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

int readline(int fd, char *buf, int nbytes) {
   int numread = 0;
   ssize_t returnval;

   while (numread < nbytes - 1) {
      returnval = read(fd, buf + numread, 1);
      if ((returnval == -1) && (errno == EINTR))
         continue;
      if ( (returnval == 0) && (numread == 0) )
         return 0;
      if (returnval == 0)
         break;
      if (returnval == -1)
         return -1;
      numread++;  
      if (buf[numread-1] == '\n') {
         buf[numread] = '\0';
         return numread; 
      }  
   }    
   errno = EINVAL;
   return -1;
}

int func1()
{
	int i = 0;

	int nii[10]= {0,1,2,3,4,5,6,7,8,9};
	char cTemp[] = {"I love you."};
	char *pcTemp = (char *)malloc(30);


	strcpy(pcTemp,"You love me.");

	for (i=0; i<1000; i++)
	{
		i++;
		sleep(1);
		fprintf(stderr,"Func1 Loop %d, %d, %s, %s\n", i,nii[7],cTemp,pcTemp);
	}

	fprintf(stderr,"Func1 End %d, %d, %s, %s\n", i,nii[7],cTemp,pcTemp);

	return i;
}

int main(int argc, char *argv[])
{
	int nn = 13;
	char cc[100];
	int f;
	
	f = open("close.cpp",O_RDONLY);
	
	//nn = readline(STDIN_FILENO,cc,sizeof(cc));
	
	while ( readline(f,cc,sizeof(cc)) > 0 ) fprintf(stderr, "%d-%s",nn,cc);;
	
	/*nn = readline(f,cc,sizeof(cc)); 
	fprintf(stderr, "%d-%s",nn,cc);
	nn = readline(f,cc,sizeof(cc)); 
	fprintf(stderr, "%d-%s",nn,cc);*/

	int zz = func1();
	fprintf(stderr,"[zz] %d\n",zz);

	for (int i=0; i<10000; i++) {
		int k = 0;
		k++;
	}

   return 0;

}
