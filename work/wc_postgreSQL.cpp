/**
   @file wc_postgreSQL.cpp
   @brief WCPostgreSQL 소스
*/

#include "wc_lib.h"

#define INITLIST m_pqConn(NULL), m_pqWork(NULL)

WCPostgreSQL::WCPostgreSQL() : INITLIST
{
	InitMember();
}

WCPostgreSQL::WCPostgreSQL(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet) : WCDb(strIP, nPort, strID, strPW, strDB, eCharSet), INITLIST
{
	InitMember();
}

WCPostgreSQL::~WCPostgreSQL()
{
	Close();
}

inline void WCPostgreSQL::InitMember()
{
	Close();
}

int WCPostgreSQL::Open(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const WCDb::E_CHARSET eCharSet)
{
	if (strIP.empty() || nPort <= 0 || strID.empty() || strPW.empty() || strDB.empty()) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> strIp.empty(%d) || nPort(%d) <= 0 || strID.empty(%d) || strPW.empty(%d) || strDB.empty(%d)", 
			__FILE__, __FUNCTION__, __LINE__, strIP.empty(), nPort, strID.empty(), strPW.empty(), strDB.empty());
		return WC_NOK;
	}

	// 초기화
	if (m_strIP != strIP) m_strIP = strIP;
	if (m_nPort != nPort) m_nPort = nPort;
	if (m_strID != strID) m_strID = strID;
	if (m_strPW != strPW) m_strPW = strPW;
	if (m_strDB != strDB) m_strDB = strDB;
	Close();

	// DB 오픈
	string strOpts;
	strOpts += "hostaddr=" + m_strIP;
	strOpts += " port=" + std::to_string(nPort);
	strOpts += " user=" + m_strID;
	strOpts += " password=" + m_strPW;
	strOpts += " dbname=" + m_strDB;

	try
	{
		m_pqConn = new pqxx::connection(strOpts);

		if (eCharSet >= 0)
		{
			m_eCharSet = eCharSet;
		}

		if (m_pqConn) {
			WC_DELETE(m_pqWork);
			m_pqWork = new pqxx::work(*m_pqConn);
		}
	}
	catch (const exception &e){
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Exception ---> pqxx::connection(%s), (Er:%s)", __FILE__, __FUNCTION__, __LINE__, strOpts.c_str(), e.what());
      	return WC_NOK;
  	}

	return WC_OK;
}

int WCPostgreSQL::Close()
{
	if (m_pqConn) 
	{
		WC_DELETE(m_pqWork);
		if (m_pqConn->is_open()) m_pqConn->disconnect();
		WC_DELETE(m_pqConn);
	}
	return 0;
}

bool WCPostgreSQL::IsOpen()
{
	if (m_pqConn && m_pqConn->is_open()) {
		return true;
	}
	else {
		return false;
	}
}

int WCPostgreSQL::Query(std::string& strQuery)
{
	if (!IsOpen())
	{
		 m_pqConn->activate();
	}

	pqxx::result pqResult;
	return 
		Query(strQuery, pqResult, true);
}

//int WCPostgreSQL::Query(std::string &strQuery, pqxx::result &pqResult, bool bCommit, bool bTransaction)
//{
//	if (!IsOpen())
//	{
//		 m_pqConn->activate();
//	}
//	int nRet = WC_NOK;
//	
//	try
//	{
//
//		pqResult = m_pqWork->exec(strQuery);
//
//	}
//	catch (const std::exception &e){
//		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Exception ---> pqxx::exec(%s), (Er:%s)", __FILE__, __FUNCTION__, __LINE__, strQuery.c_str(), e.what());
//      	nRet = WC_NOK;
//  	}
//
//	return nRet;
//}

int WCPostgreSQL::Query(std::string &strQuery, pqxx::result &pqResult, bool bCommit, bool bTransaction)
{
	//if (!IsOpen())
	//{
	//	 m_pqConn->activate();
	//}
	int nRet = WC_NOK;

	if (&strQuery == nullptr || strQuery.empty()) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid object ---> strQuery was empty)", __FILE__, __FUNCTION__, __LINE__);
		return nRet;
	}

	try
	{
		if (m_pqConn) {
			if (bTransaction) {
				//pqxx::work pqWork(*m_pqConn);
				if (&pqResult != nullptr) {
					if (m_pqWork) {
						pqResult = m_pqWork->exec(strQuery);
					}
				}
				else {
					if (m_pqWork) {
						m_pqWork->exec(strQuery);
					}
				}
				if (bCommit) m_pqWork->commit();
			}
			else {
				// nontranscatio은 정상동작하지 않음. m_pqConn을 별도로 열어서 연결시켜야 할거 같음.
				pqxx::nontransaction pqNontrans(*m_pqConn);
				if (&pqResult != nullptr) {
					pqResult = pqNontrans.exec(strQuery);
				}
				else {
					pqNontrans.exec(strQuery);	
				}
			}
			nRet = WC_OK;
		}
		else {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "[%s:%s:%d] Invalid object ---> m_pqConn was NULL)", __FILE__, __FUNCTION__, __LINE__);
			nRet = WC_NOK;
		}
	}
	catch (const std::exception &e){
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Exception ---> pqxx::exec(%s), (Er:%s)", __FILE__, __FUNCTION__, __LINE__, strQuery.c_str(), e.what());
      	nRet = WC_NOK;
  	}

	return nRet;
}
