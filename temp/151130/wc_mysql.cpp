/**
   @file wc_mysql.cpp
   @brief WCMysql 소스
*/

#include "wc_lib.h"

#define INITLIST m_eCharSet(E_CHARSET_UTF8)

WCMysql::WCMysql() : INITLIST
{
	InitMember();
}

WCMysql::WCMysql(const WCMysql &Other) : WCDb(Other), INITLIST
{
	InitCopy(Other);
}

WCMysql& WCMysql::operator=(const WCMysql &Other)
{
	if (this != &Other) {
		WCDb::operator =(Other);
		InitCopy(Other);
	}
	return *this;
}

WCMysql::~WCMysql()
{
	VClose();
}

inline void WCMysql::InitMember()
{
	memset(&m_Mysql,0x00,sizeof(m_Mysql));
}

inline void WCMysql::InitCopy(const WCMysql &Other)
{
	// 연결자를 공유하지 않고 동일한 정보로 연결세션을 생성한다. 연결 정보는 상위클래스인 WCDb에서 복사한다.
	memset(&m_Mysql,0x00,sizeof(m_Mysql));
	if (m_cIP[0]!=0x00) VOpen(m_cIP,m_cID,m_cPW,m_cDB,Other.m_eCharSet);
}

int WCMysql::VOpen(const char * const pcIp, const char * const pcId, const char * const pcPw, const char * const pcDb, const int nCharSet)
{
	if (!pcIp || !pcId || !pcPw || !pcDb) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[MYQ:%s:%d] !Ip(%p) || !Id(%p) || !Pw(%p) || !Db(%p)\n",__FUNCTION__,__LINE__,pcIp,pcId,pcPw,pcDb);
		return -1;
	}

	if(m_cIP != pcIp)
		SetIP(pcIp);
	if(m_cID != pcId)
		SetID(pcId);
	if(m_cPW != pcPw)
		SetPW(pcPw);
	if(m_cDB != pcDb)
		SetDB(pcDb);

	mysql_init(&m_Mysql);

	if (!mysql_real_connect(&m_Mysql,pcIp,pcId,pcPw,pcDb,3306,(char *)NULL,0)) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[MYQ:%s:%d] connect (IP:%s)(ID:%s)(PW:%s)(DB:%s) (Er:%d,%s)\n",__FUNCTION__,__LINE__,pcIp,pcId,pcPw,pcDb,mysql_errno(&m_Mysql),mysql_error(&m_Mysql));
		return -1;
	}

	if (nCharSet > -1)
	{
		// 캐릭터셑 설정
		if (nCharSet==E_CHARSET_UTF8) mysql_query(&m_Mysql, "set names utf8");	// Query("set names utf8") 호출하면 재귀루프에 빠짐
		else if (nCharSet==E_CHARSET_EUCKR) mysql_query(&m_Mysql, "set names euckr");
		else if (nCharSet==E_CHARSET_LATIN1) mysql_query(&m_Mysql, "set names latin1");
		m_eCharSet = (E_CHARSET)nCharSet;
	}
	WCTRACE(WCLog::E_MODE_ALL,"[MYQ:%s] connect (IP:%s)(ID:%s)(PW:%s)(DB:%s)\n",__FUNCTION__,pcIp,pcId,pcPw,pcDb);

	return 0;
}

int WCMysql::VClose()
{
	mysql_close(&m_Mysql);
	memset(&m_Mysql,0x00,sizeof(m_Mysql));
	WCTRACE(WCLog::E_MODE_ALL,"[MYQ:%s] (IP:%s)(ID:%s)(PW:%s)(DB:%s)\n",__FUNCTION__,m_cIP,m_cID,m_cPW,m_cDB);
	return 0;
}

MYSQL_RES * WCMysql::Query(const char *pcFmt, ...)
{
	if (!pcFmt) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[MYQ:%s:%d] !pcFmt\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	va_list ap;
	char ch=0;
	int nRet=0,nLen;

	va_start(ap, pcFmt);
	nLen = vsnprintf(&ch,1,pcFmt,ap) + 1;
	va_end(ap);
	if (nLen <= 0) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[MYQ:%s:%d] va_Len(%d) <= 0\n",__FUNCTION__,__LINE__,nLen);
		return NULL;
	}

	pthread_mutex_lock(&m_mutexBuf);

	// 쿼리버퍼보다 큰 쿼리는 메모리 할당을 다시 한다.
	if (!m_pcBuf || m_nBufSz<nLen)
	{
		WCTRACE(WCLog::E_MODE_WARN,"[MYQ:%s:%d] ReMalloc (BufSz:%d)(Len:%d)(Buf:%p)\n",__FUNCTION__,__LINE__,m_nBufSz,nLen,m_pcBuf);
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
   		WcLogPrintf(WCLog::E_MODE_ERROR,"[MYQ:%s:%d] malloc (Er:%d,%s)\n",__FUNCTION__,__LINE__,errno,(char *)strerror(errno));
  	 	pthread_mutex_unlock(&m_mutexBuf);
		return NULL;
  	}

	va_start(ap,pcFmt);
	vsnprintf(m_pcBuf,nLen,pcFmt,ap);  // nLen가 10이면 9+1(NULL) 이렇게 가져옴
	//vsprintf(m_pcBuf,pcFmt,ap);
	//m_pcBuf[nLen]=0x00;
	va_end(ap);

	for(int i=0; i<2; i++)
	{
		if (mysql_query(&m_Mysql,m_pcBuf))
		{
			int nError = mysql_errno(&m_Mysql);

			// 재연결 시도해서 성공하면 다시 쿼리, 실패하면 리턴.
			// 2006 : MySQL server has gone away (연결 끊어짐)
			// 1213 : Deadlock found when trying to get lock; try restarting transaction
			// 1205 : Lock wait timeout exceeded; try restarting transaction
			// 1062 : 기본키 중복
			// 1044 : Access denied for user 'test'@'%' to database 'test'. 락 걸린 테이블에 접근
			if (nError==2006 || nError==2013) {
				WcLogPrintf(WCLog::E_MODE_WARN,"[MYQ:%s:%d] mysql_query (Er:%d,%s) (Query:%s)\n",__FUNCTION__,__LINE__,nError,mysql_error(&m_Mysql),m_pcBuf);
				VClose();
				if (VOpen(m_cIP, m_cID, m_cPW, m_cDB, m_eCharSet) < 0) {
					nRet = -1;
					break;
				}
			}
			else if (nError==1044 || nError==1213) {
				nRet = 0;
				//WcLogPrintf(WCLog::E_MODE_WARN,"[MYQ:%s:%d] mysql_query (Er:%d,%s) (Query:%s)\n",__FUNCTION__,__LINE__,nError,mysql_error(&m_Mysql),m_pcBuf);
			}
			else {
				WcLogPrintf(WCLog::E_MODE_ERROR,"[MYQ:%s:%d] mysql_query (Er:%d,%s) (Query:%s)\n",__FUNCTION__,__LINE__,nError,mysql_error(&m_Mysql),m_pcBuf);
				nRet = -1;
				break;
			}
		}
		else {
			nRet = 0;
			break;
		}
	}

	MYSQL_RES *pMysqlRes=NULL;

	if (nRet == 0) {
		pMysqlRes = mysql_store_result(&m_Mysql);
		if (!pMysqlRes)
			pMysqlRes = (MYSQL_RES *)0x00000001;
	}

	pthread_mutex_unlock(&m_mutexBuf);

	return pMysqlRes;
}

MYSQL_ROW WCMysql::FetchRow(MYSQL_RES * pMysqlRes)
{
	if (!pMysqlRes) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[MYQ:%s:%d] !pMysqlRes\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	MYSQL_ROW MysqlRow;
	MysqlRow = mysql_fetch_row(pMysqlRes);
	return MysqlRow;
}

my_ulonglong WCMysql::NumRow(MYSQL_RES * pMysqlRes)
{
	if (!pMysqlRes) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[MYQ:%s:%d] !pMysqlRes\n",__FUNCTION__,__LINE__);
		return -1;
	}

	my_ulonglong nRows;
	nRows = mysql_num_rows(pMysqlRes);
	return nRows;
}

void WCMysql::FreeResult(MYSQL_RES * pMysqlRes)
{
	mysql_free_result(pMysqlRes);
}

bool WCMysql::Ping()
{
	if (m_Mysql.thread_id == 0) 
		return false;
	else {
		return
			mysql_ping(&m_Mysql);
	}
}

long unsigned int WCMysql::GetTID()
{
	return
		mysql_thread_id(&m_Mysql);
}
