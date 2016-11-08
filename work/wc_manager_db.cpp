/**
	@file wc_manager_db.cpp
   	@brief WCManagerDB 소스
*/

#include "wc_lib.h"
#include "wc_manager.cpp"

// 생성자
WCManagerDB::WCManagerDB()
{
	InitMember();
}

// 생성자
WCManagerDB::WCManagerDB(const int nMax) : WCManager<unsigned long long>(nMax)
{
	InitMember();
}

// 소멸자
WCManagerDB::~WCManagerDB()
{
}

// 멤버변수 초기화
inline void WCManagerDB::InitMember()
{
	m_itMap = m_mMember.begin();
}

// DB객체 추가
int WCManagerDB::Add(WCDb* const pDB)
{
	if (!pDB) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pDB", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	int nRet;

	nRet = WCManager<unsigned long long>::Add(pDB->GetKeyPri(), (WIMember *)pDB);
	if (nRet == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> WCManager.Add (Key:%llu)", __FILE__, __FUNCTION__, __LINE__, pDB->GetKeyPri());
	}
	else {
		m_itMap = m_mMember.begin();
	}

	return nRet;
}

// DB객체 삭제
int WCManagerDB::Remove(WCDb* const pDB)
{
	if (!pDB) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> !pDB", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	int nRet;

	nRet = WCManager<unsigned long long>::Remove(pDB->GetKeyPri());
	if (nRet == -1) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> WCManager.Remove (Key:%llu)", __FILE__, __FUNCTION__, __LINE__, pDB->GetKeyPri());
	}
	else {
		m_itMap = m_mMember.begin();
	}

	return nRet;
}

// DB객체 오픈
int WCManagerDB::Open()
{
	int nRet = WC_NOK;
	map<unsigned long long, WIMember*>::iterator it_map;
	WIMember *pIMember = NULL;
	WCDb *pDB = NULL;

	for (it_map = m_mMember.begin(); it_map != m_mMember.end(); ++it_map) {
		pIMember = it_map->second;
		if (pIMember && pIMember->IF_IsLive()) {
			pDB = (WCDb *)pIMember;
			if (pDB) {
				nRet = pDB->Open();
				if (nRet == WC_NOK)
					WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> WCDb.Open", __FILE__, __FUNCTION__, __LINE__);
				usleep(100);				
			}
		}
	}	

	return nRet;
}

// DB객체 반환
WCDb * WCManagerDB::GetDB()
{
	WCDb *pDB = NULL;
	size_t szCount;

	szCount = m_mMember.size();
	if (szCount <= 0) {
		pDB = NULL;
	}
	else {
		for (int i=0; i<int(szCount); ++i) {
			pDB = (WCDb *)m_itMap->second;
			m_itMap++;
			if (m_itMap == m_mMember.end()) {
				m_itMap = m_mMember.begin();
			}
			if (pDB->IsOpen()) {
				break;
			}
			else {
				pDB = NULL;
			}
		}
	}

	return pDB;
}

// DB객체 생성 및 오픈
int WCManagerDB::Open(const std::string &strIP, const int nPort, const std::string &strID, const std::string &strPW, const std::string &strDB, const WCDb::E_CHARSET eCharSet, const int nCount)
{
	int nLoop = nCount;

	if (&strIP == nullptr || &strID == nullptr || &strPW == nullptr || &strDB == nullptr) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> Parameter was null)", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}
	else if (strIP.empty() || nPort < 0 || strID.empty() || strPW.empty() || strDB.empty()) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid object ---> Parameter was empty)", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}
	else if (nCount <= 0) {
		nLoop = 1;
	}
	else if (nCount > m_nMaxMember) {
		nLoop = m_nMaxMember;
	}

	int nRet = WC_NOK;
	WCDb *pDB = NULL;

	for (int i=0; i<nLoop; ++i)
	{
		pDB = new WCPostgreSQL();
		if (pDB) 
		{
			nRet = pDB->Open(strIP, nPort, strID, strPW, strDB, eCharSet);
			if (nRet == WC_OK)
			{
				((WIMember *)pDB)->IF_SetInDel(true); 	// Manager 내부에서 삭제한다. 이 부분 때문에 간혹 중복 에러가 발생하기도 하는데...
				nRet = Add(pDB);
				if (nRet == WC_NOK) {
					WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> WCManagerDB.Add (Loop:%d)", __FILE__, __FUNCTION__, __LINE__, i);
					WC_DELETEWC(pDB);					
				}
			}
			else 
			{
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> WCManagerDB.Open (Loop:%d)", __FILE__, __FUNCTION__, __LINE__, i);
				WC_DELETEWC(pDB);
			}
			usleep(100);
		}
	}

	return nRet;
}

//int WCManagerDB::VOpen(const E_OTYPE eType, int nCnt, const char * const pcIp, const char * const pcId, const char * const pcPw, const char * const pcDb, const int nCharSet)
/*int WCManagerDB::PoolOpen(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet, const int nCount)
{
	if (strIP.empty() || nPort <= 0 || strID.empty() || strPW.empty() || strDB.empty()) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> strIp.empty(%d) || nPort(%d) <= 0 || strID.empty(%d) || strPW.empty(%d) || strDB.empty(%d)", 
			__FILE__, __FUNCTION__, __LINE__, strIP.empty(), nPort, strID.empty(), strPW.empty(), strDB.empty());
		return WC_NOK;
	}
	else if (nCnt > m_nMaxMember) {
		nCnt = m_nMaxMember;
	}

	int nRet = WC_NOK;

	for (int i=0; i<nCnt; ++i)
	{
		WCDb *pDB = new WCDb();
		if (pDB) 
		{
			nRet = pDB->Open(strIp, nPort, strID, strPW, strDB, eCharSet);
			if (nRet == WC_OK)
			{
				//((WIMember *)pDB)->SetInnerNew(true); // Manager 내부 할당 설정한다. 그래야 소멸자에서 삭제함.
				nRet = Add(pDB);
				if (nRet == WC_NOK) {}
					WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Add function call fails ---> (IP:%s)(PORT:%d)(ID:%s)(PW:%s)(DB:%s) (Cnt:%d)", __FILE__, __FUNCTION__, __LINE__, strIP, nPort, strID, strPW, strDB, i);
					WC_DELETEWC(pDB);					
				}
			}
			else 
			{
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Open function call fails ---> (IP:%s)(PORT:%d)(ID:%s)(PW:%s)(DB:%s) (Cnt:%d)", __FILE__, __FUNCTION__, __LINE__, strIP, nPort, strID, strPW, strDB, i);
				WC_DELETEWC(pDB);
			}
			usleep(100);
		}
	}

	return nRet;
}
*/
