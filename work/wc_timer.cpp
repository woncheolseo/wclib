/**
	@file wc_timer.cpp
   	@brief WCTimer 소스
*/

#include "wc_lib.h"

bool WCTimer::m_bSetTimer = false;
boost::object_pool<WCTimer::T_TIMER> WCTimer::m_btpoolTimer;

/**
@brief SIGALRM 시그널 콜백 함수

SIGALRM 시그널을 받으면 작동하는 함수로 호출한 객체의 IF_TimerEvent() 가상함수를 호출하여 타어머처럼 동작하게 한다.
@param nSigno 시그널번호
@param pInfo 시그널정보
@param pContext 기타정보
*/
static void gSigInterrupt(int nSigno, siginfo_t *pInfo, void *pContext) 
{
	if (!pInfo) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pInfo", __FILE__, __FUNCTION__, __LINE__);
		return;
	}

	WCTimer::T_TIMER *ptTimer;
	ptTimer = (WCTimer::T_TIMER *)(pInfo->si_value.sival_ptr);
	if (!ptTimer || !ptTimer->pWiTimer || !ptTimer->pWcTimer) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !Timer(%p) || !Timer->pWiTimer || !Timer->pWcTimer", __FILE__, __FUNCTION__, __LINE__, ptTimer);
		return;
	}

	timer_t timer;
	int nTimerNO;
	WITimer *pWiTimer = NULL;
	WCTimer *pWcTimer = NULL;
	void *pData = NULL;

	timer = ptTimer->timer;
	nTimerNO = ptTimer->nTimerNO;
	pWiTimer = (WITimer *)ptTimer->pWiTimer;
	pWcTimer = (WCTimer *)ptTimer->pWcTimer;
	pData = ptTimer->pData;
	//WC_TRACELF(WCLog::E_LEVEL_ALL, "Event (timer:%lu)(No:%d)(Cnt:%d) (pTM:%p)(pWI:%p)(pWC:%p)", ptTimer->timer, ptTimer->nTimerNO, ptTimer->nTimerCnt, ptTimer, ptTimer->pWiTimer, ptTimer->pWcTimer);

	// 타이머 발생수만큼 타이머를 발생시키고 삭제함 (0:무한발생, 0<:횟수만큼 발생)
	if (ptTimer->nTimerCnt >= 0) ptTimer->nTimerCnt--;									// -1은 무한이라는 의미이므로 -1보다 큰 경우만 감소
	if (pWcTimer->IsLive())
	{
		if (ptTimer->nTimerCnt == 0) pWcTimer->DeleteTimer(timer);						// 카운터가 0이 되었으므로 타이머 삭제
		if (pWiTimer->IF_IsLive()) pWiTimer->IF_TimerEvent(timer, nTimerNO, pData);		// 타이머 이벤트
	}

	// 객체 소멸에 대한 정리 
	if (!pWcTimer->IsLive()) {
		timer_delete(timer);
	}
	if (!pWiTimer->IF_IsLive()) pWcTimer->DeleteTimer(pWiTimer);
}

WCTimer::WCTimer()
{
	InitMember();
}

WCTimer::WCTimer(const WCTimer &Other) : WCUtil(Other)
{
	InitCopy(Other);
}

WCTimer& WCTimer::operator=(const WCTimer &Other)
{
	if (this != &Other) {
		InitCopy(Other);
	}
	return *this;
}

WCTimer::~WCTimer()
{
	DeleteTimer();
	pthread_mutex_destroy(&m_mutexTimer);
}

inline void WCTimer::InitMember()
{
	// 타이머 시그널 등록
	if (!m_bSetTimer) {
		m_bSetTimer = true;
		for (int i=0; i<10; ++i) {
			if (SetTimer() == WC_OK) 
				break;
			else if (i >= 9)
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> SetTimer", __FILE__, __FUNCTION__, __LINE__); 
		}
	}

	// 멤버변수 초기화
	m_mapTimer.clear();
	pthread_mutex_init(&m_mutexTimer, NULL);
}

inline void WCTimer::InitCopy(const WCTimer &Other)
{
	m_mapTimer.clear();
	pthread_mutex_init(&m_mutexTimer, NULL);
}

int WCTimer::SetTimer()
{
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = gSigInterrupt;
	if ((sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGALRM, &act, NULL) == -1)) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> sigaction (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
		return WC_NOK;
	}
	else 
		return WC_OK;
}

int WCTimer::StartTimer(WITimer *pWi, const int nTimerNO, const double dSec, const int nTimerCnt, void *pData)
{
	if (!pWi || nTimerNO < 0 || dSec < 0 || nTimerCnt < 0) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pWi(%p) || TimerNO(%d) < 0 || Sec(%d) < 0 || TimerCnt(%d) < 0", __FILE__, __FUNCTION__, __LINE__, pWi, dSec, nTimerNO, nTimerCnt);
		return WC_NOK;
	}

	// 타이머구조체 할당
	T_TIMER *ptTimer = m_btpoolTimer.malloc();
	if (!ptTimer) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> boost::malloc (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
		return WC_NOK;
	}
	ptTimer->timer = NULL;
	ptTimer->pWiTimer = pWi;
	ptTimer->pWcTimer = this;
	ptTimer->nTimerNO = nTimerNO;
	ptTimer->nTimerCnt = nTimerCnt;
	ptTimer->pData = pData;

	struct sigevent ev;
	int nRet = WC_NOK;

	// 알람 시그널 설정
	memset(&ev,0x00,sizeof(ev));
	ev.sigev_value.sival_int = 0;
	ev.sigev_notify = SIGEV_SIGNAL;
	ev.sigev_signo = SIGALRM;
	ev.sigev_value.sival_ptr = ptTimer;

	// 타이머 생성
	nRet = timer_create(CLOCK_REALTIME, &ev, &(ptTimer->timer));
	if (nRet == -1) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> timer_create (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
		return nRet;
	}

	// 타이머 시작
	if (nRet == 0) {
		const long long lnnBillon = 1000000000LL;
		struct itimerspec value;
		pair<map<timer_t, T_TIMER *>::iterator, bool> pr;

		// 타이머 실행
		value.it_interval.tv_sec = (long long)dSec; 
	   	value.it_interval.tv_nsec = (long long)((dSec - (double)value.it_interval.tv_sec) * lnnBillon);
	   	if (value.it_interval.tv_nsec >= lnnBillon) {
	    	value.it_interval.tv_sec++;
	      	value.it_interval.tv_nsec -= lnnBillon;
	   	}
		value.it_value = value.it_interval;

		if (timer_settime(ptTimer->timer, 0, &value, NULL) == 0) {
			pthread_mutex_lock(&m_mutexTimer);
			pr = m_mapTimer.insert(pair<timer_t, T_TIMER *>(ptTimer->timer, ptTimer));
			if (pr.second == false) {
				nRet = WC_NOK;
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> map.insert (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
			}
			nRet = WC_OK;
			pthread_mutex_unlock(&m_mutexTimer);
		}
		else {
			nRet = WC_NOK;
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> timer_settime (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
		}
	}
	else {
		nRet = WC_NOK;
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> timer_create (Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, errno, (char *)strerror(errno));
	}

	if (nRet == WC_NOK) {
		WC_FREEBTPOOL(m_btpoolTimer, ptTimer);
	}
	else { 
		//WC_TRACELF(WCLog::E_LEVEL_ALL, "Start (Tm:%lu)(No:%d)(Cnt:%d) (pTM:%p)(pWI:%p)(pWC:%p)", ptTimer->timer, ptTimer->nTimerNO, ptTimer->nTimerCnt, ptTimer, ptTimer->pWiTimer, ptTimer->pWcTimer);
	}

	return 
		nRet;
}

void WCTimer::DeleteTimer()
{
	T_TIMER *ptTimer;
	map<timer_t, T_TIMER *>::iterator it_map;

	pthread_mutex_lock(&m_mutexTimer);	
	for (it_map = m_mapTimer.begin(); it_map != m_mapTimer.end(); ++it_map) {
		ptTimer = (T_TIMER *)it_map->second;
		if (ptTimer) {
			timer_delete(ptTimer->timer);		// 타이머 중지
			//WC_TRACELF(WCLog::E_LEVEL_ALL, "Delete (Tm:%lu)(No:%d)(Cnt:%d) (pTM:%p)(pWI:%p)(pWC:%p) (MCnt:%d)", ptTimer->timer, ptTimer->nTimerNO, ptTimer->nTimerCnt, ptTimer, ptTimer->pWiTimer, ptTimer->pWcTimer, m_mapTimer.size());
			WC_FREEBTPOOL(m_btpoolTimer, ptTimer);	// 메모리 삭제
		}
	}
	m_mapTimer.clear();							// 맵멤버 삭제
	pthread_mutex_unlock(&m_mutexTimer);
}

void WCTimer::DeleteTimer(const timer_t timer)
{
	map<timer_t, T_TIMER *>::iterator it_map;
	
	pthread_mutex_lock(&m_mutexTimer);
	timer_delete(timer);
	it_map=m_mapTimer.find(timer);
	if (it_map != m_mapTimer.end()) {
		T_TIMER *ptTimer = (T_TIMER *)it_map->second;
		if (ptTimer) {
			m_mapTimer.erase(it_map);
			//WC_TRACELF(WCLog::E_LEVEL_ALL, "Delete-timer (Tm:%lu)(No:%d)(Cnt:%d) (pTM:%p)(pWI:%p)(pWC:%p) (MCnt:%d)", ptTimer->timer, ptTimer->nTimerNO, ptTimer->nTimerCnt, ptTimer, ptTimer->pWiTimer, ptTimer->pWcTimer, m_mapTimer.size());
			WC_FREEBTPOOL(m_btpoolTimer, ptTimer);
		}
	}
	pthread_mutex_unlock(&m_mutexTimer);
}

void WCTimer::DeleteTimer(const int nTimerNO)
{
	T_TIMER *ptTimer;
	map<timer_t, T_TIMER *>::iterator it_map;
	
	pthread_mutex_lock(&m_mutexTimer);	
	for (it_map = m_mapTimer.begin(); it_map != m_mapTimer.end(); ++it_map) 
	{
		ptTimer = (T_TIMER *)it_map->second;
		if (ptTimer && ptTimer->nTimerNO == nTimerNO) {
			timer_delete(ptTimer->timer);
			m_mapTimer.erase(it_map++);	
			//WC_TRACELF(WCLog::E_LEVEL_ALL, "Delete-No (Tm:%lu)(No:%d)(Cnt:%d) (pTM:%p)(pWI:%p)(pWC:%p) (MCnt:%d)", ptTimer->timer, ptTimer->nTimerNO, ptTimer->nTimerCnt, ptTimer, ptTimer->pWiTimer, ptTimer->pWcTimer, m_mapTimer.size());
			WC_FREEBTPOOL(m_btpoolTimer, ptTimer);
			break;
		}
	}
	pthread_mutex_unlock(&m_mutexTimer);
}

void WCTimer::DeleteTimer(const WITimer* const pWiTimer)
{
	if (pWiTimer) 
	{
		T_TIMER *ptTimer;
		map<timer_t, T_TIMER *>::iterator it_map;

		pthread_mutex_lock(&m_mutexTimer);	
		for (it_map = m_mapTimer.begin(); it_map != m_mapTimer.end(); ) 
		{
			ptTimer = (T_TIMER *)it_map->second;
			if (ptTimer && ptTimer->pWiTimer == pWiTimer) {
				timer_delete(ptTimer->timer);
				m_mapTimer.erase(it_map++);
				//WC_TRACELF(WCLog::E_LEVEL_ALL, "Delete-Interface (Tm:%lu)(No:%d)(Cnt:%d) (pTM:%p)(pWI:%p)(pWC:%p) (MCnt:%d)", ptTimer->timer, ptTimer->nTimerNO, ptTimer->nTimerCnt, ptTimer, ptTimer->pWiTimer, ptTimer->pWcTimer, m_mapTimer.size());
				WC_FREEBTPOOL(m_btpoolTimer, ptTimer);
			}
			else
				++it_map;
		}
		pthread_mutex_unlock(&m_mutexTimer);
	}
	else
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> pWiTimer", __FILE__, __FUNCTION__, __LINE__);
}

