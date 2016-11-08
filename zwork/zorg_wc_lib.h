/**	
	@mainpage Project : WcLibSvr

	@brief
		WcLibSvr 라이브러리는 linux/unix 서버 개발의 생산성을 높이기 위해 POSIX C/C++ API를 클래스화한 라이브러리다.

	@author 
		서원철, neo20@hanmail.net

	@version
		2.00

	@remark
		-# WCObject 클래스와 자식 클래스, WIInterface 클래스와 자식 인터페이스로 구성된다.
		-# 2015년 6월 NWAF 개발을 위해 C++ 11 기반으로 재작성한다.
		-# C11 (ISO/IEC 9899:2011)
		-# C++11 (ISO/IEC 14882:2011)
		-# 전역 매크로 : 메모리 할당/해제, 난수 설정 등을 매크로로 정의한다.
		-# 전역 로그 : 라이브러리 로그를 저장한다.
		-# 전역 함수 : 표준함수 랩핑하거나 공용으로 필요한 함수를 정의한다. 

	@warning 

	@bug

	@todo
		-# 서버 프레임 만들기 (ThreadPooling을 이용한 accept 기능)
		-# auto_ptr,shared_ptr,malloc->new 변경 고려 : smart포인터는 메모리 릭 방지를 위해, malloc -> new 변경은 c++ 연산자 형으로 변경을 고려해라. 단, 속도는 malloc가 대체로 빠르다.

	@date
		- 2009-02-03
			-# 최초 작성
		- 2009-03-03
			-# 메모리 매크로 추가
			-# 라이브러리 로그 함수 추가
			-# 토큰 함수 추가
		- 2009-03-10
			-# 실시간 타이머 추가
		- 2009-07-19
			-# WcLogPrintf 및 WcTimerStart 등의 함수를 define으로 변경
		- 2009-10-12
			-# Config 파일 관련 함수 및 변수 모두 삭제 : WCCfg 클래스로 대체함
		- 2009-11-01
			-# GetTickCount 함수 등을 명시적 static inline 함수로 변경

	@example sam_libwc.cpp
*/


#ifndef __WCLIB_H__
#define __WCLIB_H__


/**********************************************************************************************************************************************************
* 헤더파일 선언 
**********************************************************************************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fstream>
#include <dirent.h>
#include <map>
#include <queue>
#include <boost/pool/pool.hpp>
#include <mysql.h>
#include <occi.h>
#include <openssl/md5.h>
#include <gtest/gtest.h>

/*#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include <iostream>
#include <list>
#include <boost/shared_ptr.hpp>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/md5.h>
*/

using namespace oracle::occi;
using namespace std;

#include "wi_interface.h"
#include "wi_timer.h"
#include "wi_member.h"
#include "wc_protocol.h"
#include "wc_object.h"
#include "wc_log.h"
#include "wc_util.h"
#include "wc_cfg.h"
#include "wc_timer.h"
#include "wc_cryptograph.h"
#include "wc_md5.h"
#include "wc_db.h"
#include "wc_mysql.h"
#include "wc_oracle.h"
#include "wc_group.h"
#include "wc_manager.h"
#include "wc_manager_db.h"
#include "wc_proc.h"
#include "wc_thread.h"
#include "wc_manager_thread.h"
#include "wc_ipc.h"
#include "wc_socket.h"
#include "wc_manager_socket.h"
#include "wc_struct.h"
#include "wc_linkedlist.h"
#include "wc_test.h"

/*
#include "wc_threadgroupth.h"
#include "wc_stack.h"
#include "wc_socketrudp.h"
#include "wc_socketssl.h"
*/


/**********************************************************************************************************************************************************
* 전역변수 선언 
**********************************************************************************************************************************************************/

/*
/// 초 단위
enum GE_TICK
{
	GE_TICK_MILLI,		///< 밀리세컨
	GE_TICK_MICRO		///< 마이크로세컨
};
*/

/// 전역변수를 한번만 참조
#ifdef WC_GLOBALS
WCLog gg_Log;			///< Lib 로그파일
#else
extern WCLog gg_Log;	///< Lib 로그파일
#endif

/**********************************************************************************************************************************************************
* 매크로 선언 
**********************************************************************************************************************************************************/

///  TRACE모듈 :_DEBUG 모드일때만 TRACE 안의 루틴이 실행되거나 출력됨
#ifdef _DEBUG
#define WCTRACEH(x) x
#define WCTRACEM(x) {x} 
#define WCTRACE WcLogPrintf
#else
#define WCTRACEH(x) 
#define WCTRACEM(x) 
#define WCTRACE(...)  
#endif

/// malloc 해제
#define WcXFree(x) { \
	if ((x) != NULL) free((x)); \
    (x) = NULL; \
}

/// new 해제
#define WcXDelete(x) { \
    if ((x) != NULL) delete((x)); \
    (x) = NULL; \
}

/// new[] 해제
#define WcXDeleteL(x) { \
    if ((x) != NULL) delete[] ((x)); \
	(x) = NULL; \
}

/// boost::pool 메모리 해제
#define WcXFreeBs(x, y) { \
        if ((y) != NULL) (x).free((y));\
        (y) = NULL; \
}

/// new WCObject 해제
#define WcXDeleteW(x) { \
        if ((x)!=NULL && x->IsLive()) delete((x)); \
        (x) = NULL; \
}

/*
/// obstack 메모리 해제
#define WcXFreeObs(x, y) { \
        if ((y) != NULL) obstack_free((x), (y));\
        (y) = NULL; \
}
*/

/// 난수 초기화
#define WcRandomize() srand((unsigned)time(NULL))

/// 난수 발생
#define WcRandom(n) (rand() % (n))

/// Lib로그 쓰기
#define WcLogPrintf gg_Log.Printf

/// Lib로그 열기
#define WcLogOpen gg_Log.Open

/// Lib로그모드 설정
#define WcLogSetMode gg_Log.SetMode

/// Lib로그 쓰기
#define WcLogPrintfnByte gg_Log.PrintfnByte

/// Lib로그 닫기
#define WcLogClose gg_Log.Close

/**********************************************************************************************************************************************************
* 전역함수 선언 
**********************************************************************************************************************************************************/

/**
* @brief memcpy 랩퍼
*
* 인자의 NULL 유무를 검사하고 NULL이 아니면 memcpy 실행 
* @param pcDest 대상버퍼
* @param pcSrc 원본버퍼
* @param tSz 복사크기
* @return 성공: void* (대상버퍼 포인터)
* @return 실패: NULL
*/
static inline void* WcXMemcpy(void *pcDest, const void *pcSrc, const size_t tSz)
{
	if (!pcDest || !pcSrc || tSz<=0) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcDest(%p) || !pcSrc(%p) || tSz(%d)<=0\n",__FUNCTION__,__LINE__,pcDest,pcSrc,tSz);
		return NULL;
	}
	return (memcpy(pcDest,pcSrc,tSz));
}

/**
* @brief strcmp 랩퍼
* 
* 인자의 NULL 유무를 검사해서 NULL이 아니면 strcmp 실행 
* @param pcDest 문자열1
* @param pcSrc 문자열2
* @return 성공: 0(문자열1과 문자열2가 같다),  0>(문자열1이 크다), 0<(문자열1이 작다)
* @return 실패: -99 (인자값 NULL)
*/
static inline int WcXStrcmp(const char *pcDest, const char *pcSrc)
{
	if (!pcDest || !pcSrc) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcDest(%p) || !pcSrc(%p)\n",__FUNCTION__,__LINE__,pcDest,pcSrc);
		return -99;
	}
	return (strcmp(pcDest,pcSrc));
}

/**
* @brief strcmp 랩퍼
* 
* 인자의 NULL 유무를 검사해서 NULL이 아니면 strncmp 실행 
* @param pcDest 문자열1
* @param pcSrc 문자열2
* @param tSz 비교길이
* @return 성공: 0(문자열1과 문자열2가 같다),  0>(문자열1이 크다), 0<(문자열1이 작다)
* @return 실패: -99 (인자값 NULL)
*/
static inline int WcXStrncmp(const char *pcDest, const char *pcSrc, const size_t tSz)
{
	if (!pcDest || !pcSrc || tSz<=0) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcDest(%p) || !pcSrc(%p) || tSz(%d)<=0\n",__FUNCTION__,__LINE__,pcDest,pcSrc,tSz);
		return -99;
	}
	return (strncmp(pcDest,pcSrc,tSz));
}

/**
* @brief strcpy 랩퍼

* 인자의 NULL 유무를 검사해서 NULL이 아니면 strncpy 실행 
* @param pcDest 대상버퍼
* @param pcSrc 원본버퍼
* @return 성공: char* (대상버퍼 포인터)
* @return 실패: NULL
*/
static inline char * WcXStrcpy(char *pcDest, const char *pcSrc)
{
	if (!pcDest || !pcSrc) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcDest(%p) || !pcSrc(%p)\n",__FUNCTION__,__LINE__,pcDest,pcSrc);
		return NULL;
	}
	return (strcpy(pcDest,pcSrc));
}

/**
* @brief strncpy 랩퍼
* 
* 인자의 NULL 유무를 검사해서 NULL이 아니면 strncpy 실행 
* @param pcDest 대상버퍼
* @param pcSrc 원본버퍼
* @param tSz 복사크기
* @return 성공: char* (대상버퍼 포인터)
* @return 실패: NULL
*/
static inline char * WcXStrncpy(char *pcDest, const char *pcSrc, const size_t tSz)
{
	if (!pcDest || !pcSrc || tSz<=0) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcDest(%p) || !pcSrc(%p) || tSz(%d)<=0\n",__FUNCTION__,__LINE__,pcDest,pcSrc,tSz);
		return NULL;
	}
	return (strncpy(pcDest,pcSrc,tSz));
}

/**
* @brief strdup 랩퍼
* 
* 인자의 NULL 유무를 검사해서 NULL이 아니면 strdup 실행, 성공하면 내부적으로 malloc으로 메모리가 할당되니 반드시 free로 해제해야한다.
* @param pcSrc 원본버퍼
* @return 성공: char* (내부적으로 malloc 할당된 메모리 포인터)
* @return 실패: NULL
*/
static inline char * WcXStrdup(char *pcSrc)
{
	if (!pcSrc) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcSrc(%p)\n",__FUNCTION__,__LINE__,pcSrc);
		return NULL;
	}
	return strdup(pcSrc);
}

/**
* @brief atoi 랩퍼
* 
* 인자의 NULL 유무를 검사해서 NULL이 아니면 atoi 실행 
* @param pcSrc 원본버퍼
* @return 성공: int값 (0이 성공일수도 있다)
* @return 실패: 0
*/
inline int WcXAtoi(const char *pcSrc)
{
	if (!pcSrc) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcSrc(%p)\n",__FUNCTION__,__LINE__,pcSrc);
		return 0;
	}
	return atoi(pcSrc);
}

/**
* @brief strlen 랩퍼
* 
* 인자의 NULL 유무를 검사해서 NULL이 아니면 strlen 실행 
* @param pcSrc 원본버퍼
* @return 성공: 0<
* @return 실패: -1
*/
inline size_t WcXStrlen(const char *pcSrc)
{
	if (!pcSrc) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pcSrc(%p)\n",__FUNCTION__,__LINE__,pcSrc);
		return -1;
	}
	return strlen(pcSrc);
}

/**
* @brief 토큰 배열 만들기
* 
* 문자열과 구분자를 입력받아서 토큰으로 분리한다. 내부에서 pcArgvp 메모리를 할당하므로 반드시 WcFreemakeargv(pcArgvp)를 호출하여 메모리를 해제한다.
* @param pcSrc 문자열
* @param pcDelimiters 구분자
* @param pcArgvp 토큰 배열 포인터
* @return 성공: 0=<
* @return 실패: -1
*/
int WcMakeargv(const char *pcSrc, const char *pcDelimiters, char ***pcArgvp);

/**
* @brief 토큰 배열 해제 
* 
* WcMakeargv함수 호출로 만들어진 인자 배열을 해제한다. WcMakeargv 함수 호출후에는 반드시 호출한다.
* @param pcArgv 
* @return 없음
*/
void WcFreemakeargv(char **pcArgv);

/**
* @brief wait 함수 재정의 
* 
* 인터럽트 시그널에 의해 wait 함수가 종료하면 자동으로 재시작하게 한다.
* @param pnStatLoc 자식 프로세스 종료 상태값 
* @return 성공: 0< (자식프로세스ID)
* @return 실패: -1
*/
pid_t wait_r(int *pnStatLoc);

/* @brief close 함수 재정의 
* 
* 인터럽트 시그널에 의해 close 함수가 종료하면 자동으로 재시작하게 한다.
* @param nFd 파일 디스크립터
* @return 성공: 0<= 
* @return 실패: -1
*/
int close_r(int nFd);

/**
* @brief read 함수 재정의 
* 
* 인터럽트 시그널에 의해 read 함수가 종료하면 자동으로 재시작하게 한다.
* @param nFd 파일 디스크립터
* @param pBuf 읽을 버퍼
* @param szSize 읽을 크기
* @return 성공: 0<= (읽은 크기)
* @return 실패: -1
*/
ssize_t read_r(int nFd, void *pBuf, size_t szSize);

/**
* @brief write 함수 재정의 
* 
* 인터럽트 시그널에 의해 write 함수가 종료하면 자동으로 재시작하게 한다.
* @param nFd 파일 디스크립터
* @param pBuf 쓰기 버퍼
* @param szSize 쓸 크기
* @return 성공: 0<= (쓴 크기)
* @return 실패: -1
*/
ssize_t write_r(int nFd, void *pBuf, size_t szSize);

/*
* @brief TickCount 계산
* 
* TickCount (1970년01월01부터 현재까지 시간)을 인자로 넘긴다. long -> long long 형으로 변경 
* @param pnSecond TickCount
* @param enMode 시간모드
* @return 성공: 0
* @return 실패: -1
*
static inline int WcGetTickGount(unsigned long long *pnSecond, const GE_TICK enMode)
{
	if (!pnSecond) {
		//WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pnSecond\n",__FUNCTION__,__LINE__);
		fprintf(stderr,"[Wn] [%s:%d] !pnSecond\n",__FUNCTION__,__LINE__);
		return -1;
	}

	int nRet;
    struct timeval tv;
    nRet = gettimeofday(&tv,NULL);

    if (nRet != -1) {
    	if (enMode==GE_TICK_MILLI) *pnSecond = tv.tv_sec*1000LL + tv.tv_usec/1000LL;
    	else  *pnSecond = tv.tv_sec*1000000LL + tv.tv_usec;
    }

	return nRet;
}
*/

/*
* @brief 조건변수 대기
* 
* 쓰레드 동기화를 위해 조건변수 대기
* @param pCond 조건변수
* @param pMutex 뮤텍스
* @param nTime 대기시간
* @return 성공: 0, 110(시간초과)
* @return 실패: 0 이외의 값
*
static inline int WcCondTimedWait(pthread_cond_t *pCond, pthread_mutex_t *pMutex, const int nTime)
{
	if (!pCond || !pMutex || nTime<=0) {
		WcLogPrintf(WCLog::E_MODE_WARN,"[%s:%d] !pCond(%p) || !pMutex(%p) || nTime(%d)<=0\n",__FUNCTION__,__LINE__,pCond,pMutex,nTime);
		return -1;
	}

	int nRet;
	struct timespec tSpec;
	struct timeval now;

	gettimeofday(&now, NULL);
	tSpec.tv_sec = now.tv_sec + nTime;
	tSpec.tv_nsec = now.tv_usec * 1000;
	nRet = pthread_cond_timedwait(pCond, pMutex, &tSpec);
	if (nRet != 0 && nRet != 110) WcLogPrintf(WCLog::E_MODE_ERROR,"[%s:%d] pthread_cond_timewait(%d)\n",__FUNCTION__,__LINE__,nRet);

	return nRet;
}
*/

#endif
