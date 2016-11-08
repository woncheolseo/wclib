/**
	@file wc_manager.cpp
   	@brief WCManager 소스
*/

#include "wc_lib.h"

#define INITLIST m_nMaxMember(1000)

template <typename T1>
WCManager<T1>::WCManager() : INITLIST
{
	InitMember();
}

template <typename T1>
WCManager<T1>::WCManager(const int nMax) : m_nMaxMember(nMax)
{
	InitMember();
}

template <typename T1>
WCManager<T1>::~WCManager()
{
	RemoveAll();
	pthread_mutex_destroy(&m_mutexMember);
}

template <typename T1>
inline void WCManager<T1>::InitMember()
{
	m_mMember.clear();	
	pthread_mutex_init(&m_mutexMember, NULL);
}

template <typename T1>
int WCManager<T1>::Add(T1 tKey, WIMember *pIMember)
{
	if (!pIMember) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pIMember", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	int nRet;

	// 멤버 최대수 체크
	nRet = (int)m_mMember.size();
	if (nRet > m_nMaxMember) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%s] Member size was overed. ---> Member(%d) > Max(%d)", __FILE__, __FUNCTION__, __LINE__, nRet, m_nMaxMember);
		return WC_NOK;
	}

	typename map<T1,WIMember*>::iterator it_map;
	pair <typename map<T1,WIMember*>::iterator,bool> pr;

	pthread_mutex_lock(&m_mutexMember);

	// Key 중복 체크 (멀티맵이면 불필요)
	it_map = m_mMember.find(tKey);
	if (it_map != m_mMember.end()) {
		pthread_mutex_unlock(&m_mutexMember);
		WCLOG_COUTLN(WCLog::E_LEVEL_ERROR, str(boost::format("[%1%:%2%:%3%] Key is duplicated. ---> %4%") % __FILE__ % __FUNCTION__ % __LINE__ % tKey));
		//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Key is duplicated.", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	// 멤버 삽입
	pr = m_mMember.insert(pair<T1,WIMember*>(tKey, pIMember));
	if (pr.second == false) {
		nRet = WC_NOK;
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> map.insert", __FILE__, __FUNCTION__, __LINE__);
	}
	else {
		nRet = WC_OK;
		//WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Funcsion call success ---> map.insert (key:%p)(Member:%p)(Count:%d)", &tKey, pIMember, m_mMember.size());
	}	

	pthread_mutex_unlock(&m_mutexMember);

	return nRet;
}

template <typename T1>
int WCManager<T1>::Remove(T1 tKey)
{
	WCObject *pObject = NULL;
	WIMember *pIMember = NULL;

	pthread_mutex_lock(&m_mutexMember);
	typename map<T1, WIMember*>::iterator it_map;
	it_map = m_mMember.find(tKey);
	if(it_map != m_mMember.end()) {
		pIMember = it_map->second;
		if (pIMember && pIMember->IF_IsLive() && pIMember->IF_GetInDel()) {
			pObject = (WCObject *)pIMember;
			//WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Member was find. ---> (Key:%p)(IMember:%p)", &tKey, pObject);
			WC_DELETEWC(pObject);
		}
	}
	m_mMember.erase(tKey);
	pthread_mutex_unlock(&m_mutexMember);
	//WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Funcsion call success ---> map.remove (key:%p)(Count:%d)", &tKey, m_mMember.size());

	return WC_OK;
}

template <typename T1>
void WCManager<T1>::RemoveAll()
{
	WCObject *pObject = NULL;
	WIMember *pIMember = NULL;
	typename map<T1, WIMember*>::iterator it_map;

	pthread_mutex_lock(&m_mutexMember);
	for (it_map = m_mMember.begin(); it_map != m_mMember.end(); ++it_map) {
		pIMember = it_map->second;
		if (pIMember && pIMember->IF_IsLive() && pIMember->IF_GetInDel()) {
			pObject = (WCObject *)pIMember;
			//WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Member was deleted. ---> (Object:%p)(Name:%s)(Count:%d)", pObject, pObject->GetName(), m_mMember.size());
			WC_DELETEWC(pObject);
		}
	}
	m_mMember.clear();
	pthread_mutex_unlock(&m_mutexMember);
	//WC_TRACELF(WCLog::E_LEVEL_DEBUG, "RemoveAll was end. ---> (Count:%d)", m_mMember.size());
}

template <typename T1>
WIMember* WCManager<T1>::Find(T1 tKey)
{
	WIMember *pIMember=NULL;
	typename map<T1, WIMember*>::iterator it_map;

	pthread_mutex_lock(&m_mutexMember);
	it_map = m_mMember.find(tKey);
	if(it_map != m_mMember.end()) {
		pIMember = it_map->second;
		//WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Member was find. ---> (Key:%p)(IMember:%p)", &tKey, pIMember);
	}
	pthread_mutex_unlock(&m_mutexMember);
	return pIMember;
}
