/**
	@file wc_thread.cpp
   	@brief WCThread 소스
*/

#include "wc_lib.h"

#define INITLIST m_pManager(NULL), m_pthread(0), m_bRun(false), m_bWait(false)

/**
@brief 쓰레드 함수
@details
	- WCThread 객체를 쓰레드로 실행시킨다.
@param arg this객체
@return void
*/
extern "C" void *WThDispatch(void *arg)
{
	if (!arg) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !arg", __FILE__, __FUNCTION__, __LINE__);
		return (void *)0;
	}

    WCThread *pTask = reinterpret_cast<WCThread *>(arg);
    if (pTask) {
  		pTask->SetRunFlag(true);
	    pTask->Run();
	    pTask->SetRunFlag(false);
	}

    return (void *)0;
}

WCThread::WCThread() : WCProcess(), INITLIST
{
	InitMember();
}

WCThread::WCThread(const char *pcName) : WCProcess(pcName), INITLIST
{
	InitMember();
}

WCThread::~WCThread()
{
	if (m_bRun) Wait();	// 쓰레드 실행중이면 잠시 대기
	pthread_mutex_destroy(&m_mutexWait);
	pthread_cond_destroy(&m_condWait);
}

inline void WCThread::InitMember()
{
	pthread_mutex_init(&m_mutexWait, NULL);
	pthread_cond_init(&m_condWait, NULL);
}

int WCThread::Start()
{
	int nRet = WC_OK;
	pthread_attr_t attr;
	//int nStackSz = 1024000;

	if (pthread_attr_init(&attr) != 0) {
		nRet = WC_NOK;
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> pthread_attr_init (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
	}

	if (nRet == WC_OK && pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
		nRet = WC_NOK;
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> pthread_attr_setdetachstate (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
  	}

	/* 
	if (nRet==0 && pthread_attr_setstacksize(&attr, nStackSz)!=0) {
		nRet = WC_NOK;
	 	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> pthread_attr_setstacksize (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
  	} */

  	if (nRet == WC_OK && pthread_create(&m_pthread, &attr,WThDispatch, (void *)this) != 0) {
		nRet = WC_NOK;
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> pthread_create (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
	}

	if (pthread_attr_destroy(&attr) != 0) {
		nRet = WC_NOK;
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> pthread_attr_destroy (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
	}

	return nRet;
}

int WCThread::RecvEvent(const void *pData, const int nPackSz)
{ 
	return 0; 
}

void WCThread::SetRunFlag(bool bFlag)
{
	m_bRun = bFlag;

	// 소멸자에서 대기중이면 깨운다.	
	if (m_bRun == false && m_bWait == true) 
		pthread_cond_signal(&m_condWait);
}

inline void WCThread::Wait()
{
	int nRet;
	struct timespec tSpec;
	struct timeval now;

	m_bWait = true;
	pthread_mutex_lock(&m_mutexWait);

	gettimeofday(&now, NULL);
	tSpec.tv_sec = now.tv_sec + 3;
	tSpec.tv_nsec = now.tv_usec * 1000;
	nRet = pthread_cond_timedwait(&m_condWait, &m_mutexWait, &tSpec);
	if (nRet != 0 && nRet != 110) 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> pthread_cond_timedwait (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));

	pthread_mutex_unlock(&m_mutexWait);
	m_bWait = false;
}

void WCThread::SetManager(WCManager<string> *pManager)
{
	m_pManager = pManager;
}

/*
extern "C" void *WThDispatch(void *arg)
{
	if (!arg) {
		fprintf(stderr, "[%s] !arg\n",__FUNCTION__);
		return (void *)0;
	}

	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	//pthread_cleanup_push(clean_up, (void *)NULL);

    WCThread *task = reinterpret_cast<WCThread *>(arg);

    if (task)
    {
pthread_attr_t attr;
size_t sizeofstack;
int chk,rc;
rc = pthread_attr_init(&attr);
///fprintf(stderr,"[ssssssss ] thread_attr_init: %d\n",rc);
rc = pthread_attr_getdetachstate(&attr, &chk);
//fprintf(stderr,"[ssssssss ] thread_attr_getdetachedstate: %d\n",rc);
//if(chk == PTHREAD_CREATE_DETACHED ) fprintf(stderr,"[ssssssss ] Detached\n");
//else if (chk == PTHREAD_CREATE_JOINABLE) fprintf(stderr,"[ssssssss ] Joinable\n");
pthread_attr_getstacksize(&attr,&sizeofstack);
//fprintf(stderr,"[ssssssss ] stacksize %d\n", sizeofstack);
  	
  		task->SetRunning(true);
	    task->VRun();
	    task->SetRunning(false);

		WCTRACEM
		(
			void *pThis;
			char cName[100];
		   	memcpy(cName,task->GetName(),sizeof(cName)-1);
	    	pThis 8= task;
	    	WcLogPrintf(WCLog::E_MODE_ALL,"[%s] pthread_exit (Pid:%d)(Name:%s)(TID:%lu)(This:%p)\n",__FUNCTION__,getpid(),cName,tThread,pThis);
	    )
	}

    return (void *)0;
}

	int nRet;

	//pthread_attr_init(&m_pthreattr);
	//nRet = pthread_create(&m_pthread, &m_pthreattr, WThDispatch, this);
	nRet = pthread_create(&m_pthread, NULL, WThDispatch, this);
	if(nRet != 0)
		WcLogPrintf(WCLog::E_MODE_ERROR,"[THD:Start] pthread_create (Name:%s)(TID:%lu)(This:%p)(Er:%s)\n", GetName(),m_pthread,this,(char *)strerror(errno));
	else {
		nRet = pthread_detach(m_pthread);
		if (nRet != 0) 
			WcLogPrintf(WCLog::E_MODE_WARN,"[THD:Start] pthread_detach (Name:%s)(TID:%lu)(This:%p)(Er:%s)\n", GetName(),m_pthread,this,(char *)strerror(errno));
		else 
			WCTRACE(WCLog::E_MODE_ALL,"[THD:Start] pthread_detach (Name:%s)(TID:%lu)(This:%p)(Pid:%d)\n", GetName(),m_pthread,this,getpid());
	}
	return nRet;
*/
