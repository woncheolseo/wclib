/** 
	@file wc_manager_thread.cpp
   	@brief WCManagerThread 소스
*/

#include "wc_lib.h"
#include "wc_manager.cpp"

WCManagerThread::WCManagerThread()
{
}

WCManagerThread::WCManagerThread(const int nMax) : WCManager<string>(nMax)
{
}

WCManagerThread::~WCManagerThread()
{
}

int WCManagerThread::Add(WCThread *pThread)
{
	if (!pThread) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pThread", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	int nRet;
	char cKey[30];

	WcMemcpyChar(cKey, pThread->GetName(), sizeof(cKey)-1);

	nRet = WCManager<string>::Add(cKey,(WIMember *)pThread);
	if (nRet == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> Add, (Key:%s)(MapCnt:%d)", __FILE__, __FUNCTION__, __LINE__, cKey, m_mMember.size());
	}
	else {
		pThread->SetManager(this);
	}

	return nRet;
}

int WCManagerThread::Remove(WCThread *pThread)
{
	if (!pThread) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pThread", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	int nRet;

	nRet = WCManager<string>::Remove(pThread->GetName());
	if (nRet == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> Remove, (MapCnt:%d)", __FILE__, __FUNCTION__, __LINE__, m_mMember.size());
	}
	else {
		pThread->SetManager(NULL);
	}

	return nRet;
}

int WCManagerThread::Start()
{
	WCThread *pThread;
	map<string,WIMember*>::iterator it_map;
	int nRes = WC_OK;

	pthread_mutex_lock(&m_mutexMember);
	for (it_map = m_mMember.begin(); it_map != m_mMember.end(); ++it_map) 
	{
		pThread = (WCThread *)it_map->second;
		// 쓰레드 생성이 실패하면 1초 대기후 쓰레드를 종료하고 재실행시킨다. 100번 시도한다.
		for (int i=0; i<100; ++i) 
		{
		    if (pThread->Start() != 0) {
	    		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "[%s:%s:%d] Function call fails ---> Start, 1 (i:%d)(Name:%s)", __FILE__, __FUNCTION__, __LINE__, i, pThread->GetName());
	    		sleep(1);
	    		if(i==99) {
	    			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> Start, 2 (i:%d)(Name:%s)", __FILE__, __FUNCTION__, __LINE__, i, pThread->GetName());
	    			nRes = WC_NOK;
	    			break;
	    		}
		    }
		    else
				break;
		    usleep(100);
		}

		if (nRes == WC_NOK) 
			break;
	}
	pthread_mutex_unlock(&m_mutexMember);

	return nRes;
}

int WCManagerThread::Stop()
{
	WCThread *pThread;
	map<string,WIMember*>::iterator it_map;

	pthread_mutex_lock(&m_mutexMember);
	for (it_map = m_mMember.begin(); it_map != m_mMember.end(); ++it_map) 
	{
		pThread = (WCThread *)it_map->second;
		if (pThread) {
			//WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Stop (Object:%p)(Name:%s)(Running:%d)(MapCnt:%d)", pThread, pThread->GetName(), pThread->GetRunning(), m_mMember.size());
			if (pThread->GetRunFlag()) pThread->Stop(); // 쓰레드 중이면 Stop
		}
	}
	pthread_mutex_unlock(&m_mutexMember);
	//WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Stop (MapCnt:%d)", m_mMember.size());

	return WC_OK;
}

int WCManagerThread::WriteIpc(const char *pcName, const void *pData, const int nWriteSz)
{
	if (!pcName || !pData) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pcName(%p) || !pData(%p)", __FILE__, __FUNCTION__, __LINE__, pcName, pData);
		return WC_NOK;
	}

	//T_IPC_HEADER *pHeader;
	WCThread *pThread = NULL;
	map<string,WIMember*>::iterator it_map;
	int nRet = WC_NOK;

	//pthread_mutex_lock(&m_mutexMember);	// 뮤텍스를 걸면 A->WriteIpc 함수안에서 B->WriteIpc를 호출하면서 데드락 걸림
	//pHeader = (T_IPC_HEADER *)pData;
	it_map = m_mMember.find(pcName);
	if (it_map != m_mMember.end()) {
		pThread = (WCThread *)it_map->second;
		// IPC패킷 처리: 헤더부는 떼고 전달한다.
		//WC_TRACEL(WCLog::E_LEVEL_DEBUG, "WriteIpc (From:%s)(To:%s)", this->GetName(), pcName);
		nRet = pThread->RecvEvent(pData, nWriteSz);
	}
	//pthread_mutex_unlock(&m_mutexMember);

	return nRet;
}
