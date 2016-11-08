/**
	@file wc_oracle.cpp
   	@brief WCOracle 소스
*/

#include "wc_lib.h"

WCOracle::WCOracle()
{
	InitMember();
}

WCOracle::WCOracle(const WCOracle &Other) : WCDb(Other)
{
	InitCopy(Other);
}

WCOracle& WCOracle::operator=(const WCOracle &Other)
{
	if (this != &Other) {
		WCDb::operator =(Other);
		InitCopy(Other);
	}
	return *this;
}

WCOracle::~WCOracle()
{
	VClose();
	TerminateEnvironment();
	//pthread_mutex_destroy(&m_mutexOpen);
}

inline void WCOracle::InitMember()
{
	//pthread_mutex_init(&m_mutexOpen,NULL); 
	//m_eType = E_CTYPE_ORACLE;
	CreateEnvironment();
}

inline void WCOracle::InitCopy(const WCOracle &Other)
{
	//pthread_mutex_init(&m_mutexOpen,NULL);
	// m_eType = Other.m_eType;
	CreateEnvironment();
	if (m_cIP[0]!=0x00) 
		VOpen(m_cIP,m_cID,m_cPW,NULL);
}

int WCOracle::CreateEnvironment()
{
	int nRet=0;

	try
	{
		// 환경 만들기
		//m_pEnv = Environment::createEnvironment(Environment::DEFAULT);
		m_pEnv = Environment::createEnvironment(Environment::THREADED_MUTEXED);	// THREADED_MUTEXED 옵션으로 하지 않을 경우, 여러 쓰레드에서 동시 연결시 에러 발생
		nRet = 0;
	}
	catch (oracle::occi::SQLException ex)
	{
       WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] createEnvironment (Er:%d,%s)\n",__FUNCTION__,__LINE__,ex.getErrorCode(),ex.what());
       TerminateEnvironment();
       nRet = -1;
	}

	return nRet;
}

void WCOracle::TerminateEnvironment()
{
	if (m_pEnv) {
		Environment::terminateEnvironment(m_pEnv);
	}
	m_pEnv = NULL;
}

int WCOracle::VOpen(const char * const pcIp, const char * const pcId, const char * const pcPw, const char * const pcDb, const int nCharSet)
{
	int nRet;

	// oracle 연결은 dbname이 없다.
	if (!pcIp || !pcId || !pcPw) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] !pcIp(%p) || !pcId(%p) || !pcPw(%p)\n",__FUNCTION__,__LINE__,pcIp,pcId,pcPw);
		return -1;
	}

	if(m_cIP != pcIp) SetIP(pcIp);
	if(m_cID != pcId) SetID(pcId);
	if(m_cPW != pcPw) SetPW(pcPw);
	//if(m_cDB != pcDb) SetDB(pcDb);

	//pthread_mutex_lock(&m_mutexOpen);

	try
	{
		// 연결하기
		if (m_pEnv) {
			m_pConn = m_pEnv->createConnection(pcId,pcPw,pcIp);
			nRet = 0;
			WCTRACE(WCLog::E_MODE_ALL,"[ORA:%s] createConnection (IP:%s)(ID:%s)(PW:%s)\n",__FUNCTION__,pcIp,pcId,pcPw);
		}
		else {
			nRet = -1;
			WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] createConnection (IP:%s)(ID:%s)(PW:%s)\n",__FUNCTION__,__LINE__,pcIp,pcId,pcPw);
		}
	}
	catch (oracle::occi::SQLException ex)
	{
		WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] SQLException (This:%s)(Er:%d,%s)(IP:%s)(ID:%s)(PW:%s)\n",__FUNCTION__,__LINE__,this,ex.getErrorCode(),ex.what(),pcIp,pcId,pcPw);
		VClose();
       	nRet = -1;
	}

	//pthread_mutex_unlock(&m_mutexOpen);

	return nRet;
}

int WCOracle::VClose()
{
    if (m_pEnv && m_pConn) {
    	//pthread_mutex_lock(&m_mutexOpen);	
        m_pEnv->terminateConnection(m_pConn);
        m_pConn = NULL;
        //pthread_mutex_unlock(&m_mutexOpen);
    }

	return 0;
}

int WCOracle::Query(T_RESULTSET *pRes, const char *pcFmt, ...)
{
	if (!pRes || !pcFmt) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] !pRes(%p) || !pcFmt(%p)\n",__FUNCTION__,__LINE__,pRes,pcFmt);
		return -1;
	}

	va_list ap;
	int nLen;
	char ch;

	va_start(ap, pcFmt);
	nLen = vsnprintf(&ch, 1, pcFmt, ap) + 1;
	va_end(ap);
	if (nLen <= 0) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] va_start, Len(%d) <= 0\n",__FUNCTION__,__LINE__,nLen);
		return -1;
	}

	pthread_mutex_lock(&m_mutexBuf);

	// 쿼리버퍼보다 큰 쿼리는 메모리 할당을 다시 한다.
	if (!m_pcBuf || m_nBufSz<nLen)
	{
		WcLogPrintf(WCLog::E_MODE_WARN,"[ORA:%s:%d] ReMalloc (Sz:%d)(Len:%d)(%p)\n",__FUNCTION__,__LINE__,m_nBufSz,nLen,m_pcBuf);
		if (nLen > msc_nMaxQuerySz) {
			nLen = msc_nMaxQuerySz;
		}
		if (m_nBufSz < msc_nMaxQuerySz)
		{
			WcXFree(m_pcBuf);
			usleep(1);
			m_nBufSz = nLen;
			m_pcBuf = (char *)malloc(m_nBufSz);
		}
	}
 	if (!m_pcBuf) {
   		WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA::%s:%d] malloc (Er:%d,%s)\n",__FUNCTION__,__LINE__,errno,(char *)strerror(errno));
  	 	pthread_mutex_unlock(&m_mutexBuf);
		return -1;
  	}

	va_start(ap, pcFmt);
	vsnprintf(m_pcBuf, nLen, pcFmt, ap);  // nLen가 10이면 9+1(NULL) 이렇게 가져옴
	//vsprintf(m_pcBuf, pcFmt, ap);
	//m_pcBuf[nLen] = 0x00;
	va_end(ap);

	try
	{
		pRes->pStmt=NULL;
		
		for (int i=0; i<2; i++)
		{
			if (m_pConn)
			{
				// 질의, 실행하기
				pRes->pStmt = m_pConn->createStatement();
				if (pRes->pStmt) {
					pRes->pResultSet = pRes->pStmt->executeQuery(m_pcBuf);
					nLen = 0;
					break;	// for 루프로 돌기때문에 성공하면 바로 빠져나가야 한다.
				}
				else {
					WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] createStatement\n",__FUNCTION__,__LINE__);
				}
			}
			else {
				WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] !m_pConn\n",__FUNCTION__,__LINE__);
				if (i==0) {
					VClose();
					if (VOpen(m_cIP,m_cID,m_cPW,NULL,-1) < 0) {
						nLen = -1;
						break;
					}
				}
				else {
					nLen = -1;
				}
			}
			usleep(10);
		}
	}
	catch (oracle::occi::SQLException ex)
	{
		// 쿼리가 실패하면 ResutlSet 주소가 바뀌지 않는다. 이 상태에서 FreeResult() 함수에서 closeResultSet(pRes->pResultSet) 호출하면 프로그램 종료함. 따라서 NULL로만 대입시킨다.
		pRes->pResultSet = NULL;
		nLen = -1;
		WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] executeQuery (Er:%d,%s)(Query:%s)\n",__FUNCTION__,__LINE__,ex.getErrorCode(),ex.what(),m_pcBuf);

		if (ex.getErrorCode()==18) {
			sleep(1);
		}
    }

	pthread_mutex_unlock(&m_mutexBuf);

	return nLen;
}

int WCOracle::FreeResult(T_RESULTSET *pRes)
{
	if (!pRes) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] !pRes\n",__FUNCTION__,__LINE__);
		return -1;
	}

	try
	{
		if(pRes->pStmt)	
		{
			// 실행반납
			if(pRes->pResultSet)
				pRes->pStmt->closeResultSet(pRes->pResultSet);
			if(m_pConn)
				m_pConn->terminateStatement(pRes->pStmt);
			pRes->pStmt = NULL;
			pRes->pResultSet = NULL;
			return 0;
		}
		else
			return -1;
	}
	catch (oracle::occi::SQLException ex)
	{
        WcLogPrintf(WCLog::E_MODE_ERROR,"[ORA:%s:%d] terminateStatement (Er:%d,%s)\n",__FUNCTION__,__LINE__,ex.getErrorCode(),ex.what());
        return -1;
	}
}
