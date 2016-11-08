/**
	@file wc_lib.cpp
   	@brief libwc 소스
*/

#define WC_GLOBALS	1

#include "wc_lib.h"

int WcMakeargv(const char *pcSrc, const char *pcDelim, char ***pcArgvp) 
{
	if (!pcSrc || !pcDelim || !pcArgvp) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[%s:%d] !pcSrc(%p) || !pcDelim(%p) || !pcArgvp(%p)\n",__FUNCTION__,__LINE__,pcSrc,pcDelim,pcArgvp);
		return -1;
	}

 	int i,nError,nNumtokens;
   	const char *pcSnew;
   	char *pcDst,*pcLast;

	// 버퍼 할당
   	*pcArgvp = NULL;
   	pcSnew = pcSrc + strspn(pcSrc,pcDelim);
   	pcDst = (char *)malloc(strlen(pcSnew) + 1);
   	if (!pcDst) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[%s:%d] malloc, !pcDst\n",__FUNCTION__,__LINE__);
      	return -1;
   	}
   	strcpy(pcDst, pcSnew);	// 원본 문자열 복사

   	// 토큰 갯수 확인
   	nNumtokens = 0;
   	if (strtok_r(pcDst,pcDelim,&pcLast) != NULL)
    	for (nNumtokens = 1; strtok_r(NULL,pcDelim,&pcLast) != NULL; nNumtokens++) ; 

    // 토큰 받을 버퍼 할당
   	if ((*pcArgvp = (char **)malloc((nNumtokens + 1)*sizeof(char *))) == NULL) {
      	nError = errno;
      	free(pcDst);
      	WcLogPrintf(WCLog::E_MODE_ERROR,"[%s:%d] malloc, !pcArgvp\n",__FUNCTION__,__LINE__);
      	return -1; 
   	} 

    // 토큰 할당
   	if (nNumtokens == 0) 
    	free(pcDst);
   	else {
      	strcpy(pcDst,pcSnew);
      	**pcArgvp = strtok_r(pcDst,pcDelim,&pcLast);
      	for (i = 1; i < nNumtokens; i++)
          	*((*pcArgvp) + i) = strtok_r(NULL,pcDelim,&pcLast);
    } 
    *((*pcArgvp) + nNumtokens) = NULL;

    return nNumtokens;
}     

void WcFreemakeargv(char **pcArgv) {
   	if (!pcArgv) {
   		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcArgv\n",__FUNCTION__,__LINE__);
    	return;
	}
   if (*pcArgv)
      free(*pcArgv);
   free(pcArgv);
}

pid_t wait_r(int *pnStatLoc)
{
	int nRet;
	while (((nRet = wait(pnStatLoc)) == -1) && (errno == EINTR));
	return nRet;
}

int close_r(int nFd) 
{
   int nRet;
   while (nRet = close(nFd), nRet == -1 && errno == EINTR) ;
   return nRet;
}

ssize_t read_r(int nFd, void *pBuf, size_t szSize) 
{
   ssize_t szRet;
   while (szRet = read(nFd, pBuf, szSize), szRet == -1 && errno == EINTR) ;
   return szRet;
}

ssize_t write_r(int nFd, void *pBuf, size_t szSize) 
{
   char *pcBuf;
   size_t szTowrite;
   ssize_t szWritten;
   size_t szTotal;

   for (pcBuf = (char *)pBuf, szTowrite = szSize, szTotal = 0; szTowrite > 0; pcBuf += szWritten, szTowrite -= szWritten) 
   {
      szWritten = write(nFd, pcBuf, szTowrite);
      if ((szWritten == -1) && (errno != EINTR))
         return -1;
      if (szWritten == -1)
         szWritten = 0;
      szTotal += szWritten;
   }
   return szTotal;
}
