/**
	@file wc_manager_socket.cpp
   	@brief WCManagerSocket 소스
*/

#include "wc_lib.h"
#include "wc_manager.cpp"

#define INITLIST2 m_nEpoll(0)

WCManagerSocket::WCManagerSocket() : INITLIST2
{
}

WCManagerSocket::WCManagerSocket(const int nMax) : WCManager<int>(nMax), INITLIST2
{
}

WCManagerSocket::~WCManagerSocket()
{
}

int WCManagerSocket::Add(WCSocket *pSock)
{
	if (!pSock) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pSock", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	int nRet,nFd;

	// 소켓이 열리지 않았으면 OPNE한다.
	if (pSock->GetFd() <= 0) {
		if (pSock->Open() == -1) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> Open, (Ip:%s)(Port:%u)", __FILE__, __FUNCTION__, __LINE__, pSock->GetIp(), pSock->GetPort());
			return WC_NOK;
		}
	}
	nFd = pSock->GetFd();

	// epoll 등록
	memset(&m_epEventOne,0x00,sizeof(struct epoll_event));
	// EPOLLET는 ET방식(Edge Trigger)이다. 신호가 변경되는 시점에서만 이벤트가 발생하는데 약간 문제가 있어서 그냥 LT(Lever Trigger) 방식을 사용한다. 
	// 성능은 ET가 LT보다 조금더 뛰어나다고 한다.
	// m_epEventOne.events = (unsigned int)(EPOLLIN | EPOLLET);
	// m_epEventOne.events = (unsigned int)(EPOLLIN | EPOLLOUT);
	// m_epEventOne.events = (unsigned int)(EPOLLIN | EPOLLERR | EPOLLHUP);
	m_epEventOne.events = (unsigned int)(EPOLLIN | EPOLLERR); //EPOLLHUP은 Cliet 소켓 생성후 연결을 하지 않으면 발생하더라. 따라서 생성 후 바로 연결해서 EPOLLHUP 발생 안하게 한다.
	m_epEventOne.data.fd = nFd;
	if (epoll_ctl(m_nEpoll, EPOLL_CTL_ADD, nFd, &m_epEventOne) < 0) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> getsockopt, (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, nFd, errno, (char *)strerror(errno));
		return WC_NOK;
	}

	// 멤버 등록
	nRet = WCManager<int>::Add(nFd, (WIMember *)pSock);
	if (nRet == -1) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> Add, (Fd:%05d)(Ip:%s)(Port:%u)", __FILE__, __FUNCTION__, __LINE__, nFd, pSock->GetIp(), pSock->GetPort());
	}
	else {
		nRet = nFd;
		// WC_TRACELF(WCLog::E_MODE_DEBUG,"Manager add a Socket ---> (Fd:%d)(Ip:%s)(Port:%u)", nFd, pSock->GetIp(), pSock->GetPort());
	}

	return nRet;
}

int WCManagerSocket::Remove(WCSocket *pSock)
{
	int nFd = WC_NOK;

	if (!pSock || (nFd = pSock->GetFd()) <= 0) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pSock(%p) || pSock->Fd(%d) <= 0", __FILE__, __FUNCTION__, __LINE__, pSock, nFd);
		return WC_NOK;
	}

	// epoll 삭제
	if (epoll_ctl(m_nEpoll, EPOLL_CTL_DEL, nFd, &m_epEventOne) < 0 ) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> epoll_ctl, (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, nFd, errno, (char *)strerror(errno));
		return WC_NOK;
	}

	// 소켓 닫기
	pSock->Close();

	int nRet = WCManager<int>::Remove(nFd);
	if (nRet == -1) 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> Remove, (Fd:%05d)(Ip:%s)(Port:%u)(This:%p)", __FILE__, __FUNCTION__, __LINE__, nFd, pSock->GetIp(), pSock->GetPort(), this);	
	//else WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG,"Remove ---> (Fd:%d)(Ip:%s)(Port:%u)(This:%p)\n", nFd, pSock->GetIp(), pSock->GetPort(), this);

	return nRet;
}

void WCManagerSocket::Close()
{
	WCSocket *pSock;
	map<int,WIMember*>::iterator it_map;

	pthread_mutex_lock(&m_mutexMember);
	for (it_map = m_mMember.begin(); it_map != m_mMember.end(); it_map++) {
		pSock = (WCSocket *)it_map->second;
		if (pSock && pSock->GetFd() > 0) {
			//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG,"Close ---> Ready (Fd:%d)(MapCnt:%d)", pSock->GetFd(), m_mMember.size());
			if (epoll_ctl(m_nEpoll, EPOLL_CTL_DEL, pSock->GetFd(), &m_epEventOne) < 0) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> epoll_ctl, (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, pSock->GetFd(), errno, (char *)strerror(errno));
			}
			pSock->Close();
		}
	}
	m_mMember.clear();
	pthread_mutex_unlock(&m_mutexMember);
	//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG,"Close ---> (MapCnt:%d)", m_mMember.size());
}

int WCManagerSocket::Close(int nFd)
{
	WCSocket *pSock=NULL;
	map<int,WIMember*>::iterator it_map;
	int nRet = -1;

	pthread_mutex_lock(&m_mutexMember);
	it_map = m_mMember.find(nFd);
	if(it_map != m_mMember.end()) {
		pSock = (WCSocket *)it_map->second;
		if (pSock && pSock->GetFd() > 0) {
			//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG,"Close ---> Ready (Fd:%d)(MapCnt:%d)", pSock->GetFd(), m_mMember.size());
			if (epoll_ctl(m_nEpoll,EPOLL_CTL_DEL,pSock->GetFd(), &m_epEventOne) < 0) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> epoll_ctl, (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, pSock->GetFd(), errno, (char *)strerror(errno));
			}
			nRet = pSock->Close();
		}
	}
	pthread_mutex_unlock(&m_mutexMember);
	//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG,"Close ---> (MapCnt:%d)", m_mMember.size());
		
	return nRet;
}

void WCManagerSocket::ShutDown()
{
	WCSocket *pSock = NULL;
	map<int, WIMember*>::iterator it_map;

	pthread_mutex_lock(&m_mutexMember);
	for (it_map=m_mMember.begin(); it_map != m_mMember.end(); ++it_map) {
		pSock = (WCSocket *)it_map->second;
		if (pSock && pSock->GetFd() > 0) {
			pSock->ShutDown();
		}
	}
	pthread_mutex_unlock(&m_mutexMember);
}

int WCManagerSocket::ShutDown(int nFd)
{
	WCSocket *pSock = NULL;
	map<int, WIMember*>::iterator it_map;
	int nRet = WC_NOK;

	pthread_mutex_lock(&m_mutexMember);
	it_map = m_mMember.find(nFd);
	if(it_map != m_mMember.end()) {
		pSock = (WCSocket *)it_map->second;
		if (pSock && pSock->GetFd() > 0) {
			nRet = pSock->ShutDown();
		}
	}
	pthread_mutex_unlock(&m_mutexMember);
	
	return nRet;
}

WCSocket* WCManagerSocket::Find(int nFd)
{
	WCSocket *pSock=NULL;
	map<int,WIMember*>::iterator it_map;

	pthread_mutex_lock(&m_mutexMember);
	it_map = m_mMember.find(nFd);
	if(it_map != m_mMember.end()) {
		pSock = (WCSocket *)it_map->second;
		WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Find ---> Fd, (Fd:%05d)", nFd);
	}
	pthread_mutex_unlock(&m_mutexMember);
	return pSock;
}

int WCManagerSocket::CreateEpoll()
{
	if ((m_nEpoll = epoll_create(MAX_EPOLLEVENT_SIZE)) == -1) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> epoll_create, (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
	}
	return m_nEpoll;
}

int WCManagerSocket::WaitEpoll(epoll_event *pepEvent)
{
	int nEpWait;
	sigset_t old;
	sigset_t sigmask;

	if (m_mMember.size() <= 0) {
		errno = 0;
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> m_mMember.size(%d) <= 0", __FILE__, __FUNCTION__, __LINE__, m_mMember.size());
		return WC_NOK;
	}

	sigfillset(&sigmask);
	sigdelset(&sigmask, SIGINT);
	//sigdelset(&sigmask, SIGALRM);
	sigprocmask(SIG_SETMASK, &sigmask, &old);
	nEpWait = epoll_wait(m_nEpoll, pepEvent, MAX_EPOLLEVENT_SIZE, 60000);
	sigprocmask(SIG_SETMASK,&old,0);

	if (nEpWait == -1 && errno != EINTR) 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> epoll_wait, (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));

	return nEpWait;
}
