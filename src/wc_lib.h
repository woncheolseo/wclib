/**	
	@mainpage Project : wclib

	@brief
		linux/unix Application Development Library

	@details
		- 
		- 설명
			- linux/unix application 개발의 생산성을 높이기 위해 C/C++ API를 클래스화한 라이브러리
			- Shell에서 실행되는 형태의 Application을 개발하기 위한 라이브러리 
			- 주로 Server 개발을 위한 라이브러리
		- 언어 
			- C11 (ISO/IEC 9899:2011)
			- C++11 (ISO/IEC 14882:2011)
		- 구조 
			- WCObject - 최상위 클래스
			- WIObject - 최상위 인터페이스
			- WCLog - 별도 클래스
		- 단위테스트 
			- googletest 1.8.0

	@note
		- 원칙은 C11/C++11 표준으로 작성하는 것이었나 익숙하지 않은 관계로 실제 구현은 11 스럽지않을거 같다. ^^;
		- WCLog 클래스는 WCObject의 모든 클래스에서 사용하기 위해 외부로 뺐다.

	@version
		- 0.9.0.0

	@warning 

	@bug

	@todo
		-# ?? WCTimer에서 객체 할당을 위해 boost::pool을 사용중인데 WCManager를 상속받아서 메모리풀 관리하는 클래스를 따로 만들어 ? boost 굳히 사용할 필요가 ...
		-# ?? boost::format을 다른 것으로 교체 ?
		-# ?? 모든 클래스의 스트레스 테스트 및 단위테스트를 상세하게 작성해야 함, 그리고 예외처리도 확실하게 
		-# ?? char 배열을 string으로 변경, WCObject 키만 빼고. boost::format 이용
		-# ?? WCTimer, WITimer는 모두 상속받는데... 상속 관계에서 생성사,소멸자 호출 비용이 발생하는데... 최소화시켜려면 상속관계가 아닌 독립적?

	@author 
		- 서원철, neo20@hanmail.net

	@date
		- 2010-01-05
			-# 최초 작성

	@example sam_libwc.cpp
*/

#ifndef __WC_LIB_H__
#define __WC_LIB_H__

/**********************************************************************************************************************************************************
* 헤더파일 선언 
**********************************************************************************************************************************************************/
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include <fstream>
/*
#include <ctime>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
*/
#include <boost/pool/object_pool.hpp>
#include <boost/format.hpp>
/*
#include <pqxx/pqxx> 
*/
//#include <json/json.h>
//#include <openssl/sha.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;

/* ??
#include "wc_protocol.h"
*/
#include "wc_log.h"
#include "wi_object.h"
/* ??
#include "wi_timer.h"
#include "wi_member.h"
*/
#include "wc_object.h"
/* ??
#include "wc_util.h"
#include "wc_timer.h"
#include "wc_cfg.h"
#include "wc_group.h"
#include "wc_manager.h"
#include "wc_ipc.h"
#include "wc_process.h"
#include "wc_socket.h"
#include "wc_thread.h"
#include "wc_db.h"
#include "wc_postgreSQL.h"
#include "wc_manager_socket.h"
#include "wc_manager_thread.h"
#include "wc_manager_db.h"
*/

/**********************************************************************************************************************************************************
* extern 선언
**********************************************************************************************************************************************************/
// ?? extern WCLog ge_Log;

/**********************************************************************************************************************************************************
* 매크로 선언 
**********************************************************************************************************************************************************/

#ifdef _DEBUG
/// 디버깅
#define WC_TRACE(x) x
/// 디버깅 블럭
#define WC_TRACEB(x) {x} 
/// 디버깅 로그
#define WC_TRACEL WCLOG_COUTLN
/// 디버깅 로그
#define WC_TRACELF WCLOG_COUTLNFORMAT
#else
/// 디버깅
#define WC_TRACE(x) 
/// 디버깅 블럭
#define WC_TRACEB(x) 
/// 디버깅 로그
#define WC_TRACEL(...)  
/// 디버깅 로그
#define WC_TRACELF(...)  
#endif

/// Lib로그 열기
#define WCLOG_OPEN ge_Log.Open

/// Lib로그모드 설정
#define WCLOG_SETLEVEL ge_Log.SetLevel

/// Lib로그 쓰기
#define WCLOG_WRITE ge_Log.Coutln

/// Lib로그 쓰기
#define WCLOG_WRITEFORMAT ge_Log.CoutlnFormat

/// Lib로그 닫기
#define WCLOG_CLOSE ge_Log.Close

/// malloc 해제
#define WC_FREE(x) { \
	if ((x) != NULL) free((x)); \
    (x) = NULL; \
}

/// new 해제
#define WC_DELETE(x) { \
    if ((x) != NULL) delete((x)); \
    (x) = NULL; \
}

/// new[] 해제
#define WC_DELETEL(x) { \
    if ((x) != NULL) delete[] ((x)); \
	(x) = NULL; \
}

/// new WCObject 해제
#define WC_DELETEWC(x) { \
        if ((x)!=NULL && x->IsLive()) delete((x)); \
        (x) = NULL; \
}

/* ??
/// boost::---pool 해제
#define WC_FREEBTPOOL(x, y) { \
	if ((y) != NULL) (x).free((y));\
    (y) = NULL; \
}
*/

/**********************************************************************************************************************************************************
* 전역변수 선언 
**********************************************************************************************************************************************************/
const long WC_LIVEKEY = 2147483641;		///< 객체 살아있나 ~ 키값 - INT형 최대값 2147483647 에서 조금 뺐음
const int WC_OK = 0;					///< 정상값 
const int WC_NOK = -1;					///< 실패값
const string WC_NOKSTR = "-1";			///< 실패문자열

/**********************************************************************************************************************************************************
* 전역함수 선언 
**********************************************************************************************************************************************************/
/* ??
@brief wait 함수 재정의 
@details 
	- 인터럽트 시그널에 의해 wait 함수가 종료하면 자동으로 재시작하게 한다.
@param pnStatLoc 자식 프로세스 종료 상태값 
@return 성공: 0, 양수 - 자식프로세스ID
@return 실패: WC_NOK
*
inline pid_t wait_r(int *pnStatLoc)
{
	int nRet;
	while (((nRet = wait(pnStatLoc)) == -1) && (errno == EINTR));
	return nRet;
}
*/

/* ??
@brief close 함수 재정의 
@details
	- 인터럽트 시그널에 의해 close 함수가 종료하면 자동으로 재시작하게 한다.
@param nFd 파일 디스크립터
@return 성공: 0, 양수 
@return 실패: WC_NOK
*
inline int close_r(int nFd) 
{
   int nRet;
   while (nRet = close(nFd), nRet == -1 && errno == EINTR) ;
   return nRet;
}
*/

/**
@brief 객체키 생성
@details
		- 
@return 성공: 양수
@return 실패: 0	
*/
inline unsigned long long WcCreateObjectKey()
{
	// 객체키 설정
	struct timespec tspecKey;
	if (clock_gettime(CLOCK_REALTIME, &tspecKey) == -1) {
		// ?? WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] 'clock_gettime' function has failed. ---> (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
		return WC_OK;
	}
	else {
		return tspecKey.tv_nsec;
	}
}

/**
@brief memcpy
@details 
	- 인자의 NULL 유무를 검사하고 NULL이 아니면 memcpy 실행하고 대상 버퍼의 마지막 배열 널 할당
@note 3번째 인자인 size값은 함수 내에서 범위 체크가 불가하므로 호출하는 쪽에서 버퍼오버플로우가 발생하지 않게 확실한 값을 넘겨야 한다.
@param pcDest 대상버퍼
@param pcSrc 원본버퍼
@param tSz 복사크기
@return 성공: void* - 대상버퍼 포인터
@return 실패: NULL
*/
inline char* WcMemcpyChar(char *pcDest, const char *pcSrc, const size_t tSz)
{
	if (!pcDest || !pcSrc || tSz <= 0) {
		// ?? WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] parameter was wrong. ---> !pcDest(%p) || !pcSrc(%p) || tSz(%d) <= 0", __FILE__, __FUNCTION__, __LINE__, pcDest, pcSrc, tSz);
		return NULL;
	}

	char *pcRet;
	pcRet = (char *)memcpy(pcDest, pcSrc, tSz);
	if (pcRet) *(pcDest+tSz) = 0x00;

	return pcRet;
}

/** ??
@brief strcpy
@details 
	- 
@param pcDest 대상버퍼
@param pcSrc 원본버퍼
@return 성공: char* - 대상버퍼 포인터
@return 실패: NULL
*
inline char* WcStrcpy(char *pcDest, const char *pcSrc)
{
	if (!pcDest || !pcSrc) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] parameter was wrong. ---> !pcDest(%p) || !pcSrc(%p)", __FILE__, __FUNCTION__, __LINE__, pcDest, pcSrc);
		return NULL;
	}

	return 
		(strcpy(pcDest,pcSrc));;
}
*/

/** ??
@brief strncpy
@details 
	- 
@param pcDest 대상버퍼
@param pcSrc 원본버퍼
@param tSz 복사크기
@return 성공: char* - 대상버퍼 포인터
@return 실패: NULL
*
inline char* WcStrncpy(char *pcDest, const char *pcSrc, const size_t tSz)
{
	if (!pcDest || !pcSrc) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] parameter was wrong. ---> !pcDest(%p) || !pcSrc(%p)", __FILE__, __FUNCTION__, __LINE__, pcDest, pcSrc);
		return NULL;
	}

	return 
		(strncpy(pcDest,pcSrc,tSz));;
}
*/

#endif
