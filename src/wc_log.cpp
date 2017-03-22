/**
	@file wc_log.cpp
   	@brief WCLog 소스
*/

#include "wc_lib.h"

#define INITLIST m_strName(""), m_nLevel(E_LEVEL_WARN|E_LEVEL_ERROR), m_eCycle(E_CYCLE_DAY), m_bDirCreate(true), m_pcOutBuf(NULL), m_nOutBufSz(500), m_bMutex(true)

WCLog::WCLog() : INITLIST
{
	InitMember();
}

WCLog::WCLog(const string& strFullName, const int nMode, const E_CYCLE eCycle) : INITLIST
{
	InitMember();
	Open(strFullName, nMode, eCycle);
}

WCLog::WCLog(const WCLog &Other) : INITLIST
{
	InitCopy(Other);
}

WCLog& WCLog::operator=(const WCLog &Other)
{
	if (this != &Other) {
		InitCopy(Other);
	}
	return *this;
}

WCLog::~WCLog()
{
	Close();
	WC_DELETEL(m_pcOutBuf); 
	pthread_mutex_destroy(&m_mutexLog);
}

inline void WCLog::InitMember()
{
	memset(&m_tmCycle, 0x00, sizeof(m_tmCycle));
	if (!m_pcOutBuf) {
		m_pcOutBuf = new char[m_nOutBufSz];
	}
	pthread_mutex_init(&m_mutexLog, NULL);
}

inline void WCLog::InitCopy(const WCLog &Other)
{
	m_strName = Other.m_strName;
	m_nLevel = Other.m_nLevel;
	m_eCycle = Other.m_eCycle;
	memcpy(&m_tmCycle,&Other.m_tmCycle,sizeof(Other.m_tmCycle));
	m_bDirCreate = Other.m_bDirCreate;
	m_nOutBufSz = Other.m_nOutBufSz;
	if (!m_pcOutBuf) {
		m_pcOutBuf = new char[m_nOutBufSz]; // 깊은 복사
	}
	pthread_mutex_init(&m_mutexLog, NULL);
}

int WCLog::Open(const string& strFullName, const int nLevel, const E_CYCLE eCycle)
{
	int nRet = WC_NOK; 

	if (&strFullName == nullptr) {
		WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Parameter -> strFullName == nullptr") % __FILE__ % __FUNCTION__ % __LINE__));
		Close();
		return nRet;
	}
	else if (strFullName.empty()) {
		//파일명이 빈 것은 표준출력으로 출력한다.
		WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Parameter -> strFullName.empty()") % __FILE__ % __FUNCTION__ % __LINE__));
		Close();
		m_strName = strFullName;
		m_nLevel = nLevel;
		m_eCycle = eCycle;		
		return WC_OK;
	}

	long t;
	struct tm tmBuf;
	time(&t);
	if (!localtime_r(&t, &tmBuf)) {
		WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Function -> localtime_r (%d, %s)") % __FILE__ % __FUNCTION__ % __LINE__ % errno % strerror(errno)));
		Close();
		return nRet;
	}

	string strDir, strFile, strFilnalName;
	size_t szFound, szFound1;
	char cDateTime[20];

	// 값 설정
	Close();	
	m_strName = strFullName;
	m_nLevel = nLevel;
	m_eCycle = eCycle;
	memcpy(&m_tmCycle,&tmBuf,sizeof(m_tmCycle));

	// 파일경로를 디렉토리와 파일명으로 분리
	szFound = strFullName.rfind('/');
	szFound1 = szFound + 1;
	if (szFound != string::npos && szFound1 != string::npos) {
		strDir = strFullName.substr(0, szFound1);
		strFile = strFullName.substr(szFound1, string::npos);
	}
	else {
		strDir = "";
		strFile = strFullName;
	}

	// 디렉토리 자동생성 - strtok_r 사용하지 않음
	if (m_bDirCreate) {
		size_t szTemp = 0;
		string strTemp;
		struct stat sb;
		char cTemp[2048];
		int nTemp;
		for (int i=0; i<1000; ++i) {
			szFound = strDir.find('/', szTemp);
			if (szFound != string::npos) {
				strTemp = strDir.substr(0, szFound+1);
				szTemp = szFound + 1;
				nTemp = sprintf(cTemp, "%s", strTemp.c_str());
				if (nTemp > 2048) {
					WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Parameter -> nTemp > 2048") % __FILE__ % __FUNCTION__ % __LINE__));
					break;
				}
				else {
					cTemp[nTemp] = 0x00;
					if (stat(cTemp, &sb) == 0 && S_ISDIR(sb.st_mode)) {
					}
					else {
						if (mkdir(cTemp, 0777) == -1) {
							WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Function -> mkdir (%d, %s)") % __FILE__ % __FUNCTION__ % __LINE__ % errno % strerror(errno)));
							break;
						}
					}
				}
			}
			else
				break;
		}
	}

	// 로그주기에 따라 파일 생성
	if (m_eCycle == E_CYCLE_DAY) {
		sprintf(cDateTime, "%04d%02d%02d-", tmBuf.tm_year+1900, tmBuf.tm_mon+1, tmBuf.tm_mday);
		cDateTime[9] = 0x00;
		strFilnalName = strDir + cDateTime + strFile;
	}
	else if (m_eCycle == E_CYCLE_MONTH) {
		sprintf(cDateTime, "%04d%02d-", tmBuf.tm_year+1900, tmBuf.tm_mon+1);
		cDateTime[7] = 0x00;
		strFilnalName = strDir + cDateTime + strFile;
	}
	else {
		strFilnalName = strFullName;
	}

	// 파일 열기
	m_ofs.open(strFilnalName, ofstream::out|ofstream::app);
	if (!m_ofs.is_open()) {
		WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Function -> is_open (%d, %s)") % __FILE__ % __FUNCTION__ % __LINE__ % errno % strerror(errno)));
		nRet = WC_NOK;
	}
	else
	{
		nRet = WC_OK;
	}

	return nRet;
}

int WCLog::Close()
{
	int nRes = WC_NOK;
	if (m_ofs.is_open()) m_ofs.close();
	m_strName = "";
	return nRes;
}

int WCLog::WriteConsole(const E_LEVEL eLevel, const string& strLog)
{
	long t;
	struct tm tmBuf;

	time(&t);
	if (!localtime_r(&t, &tmBuf)) {
		cerr << "[E-00:00:00] " << strLog << endl;
		return WC_NOK;
	}

	if (eLevel == E_LEVEL_ERROR) {
		cerr << "[E-" << tmBuf.tm_hour << ":" << tmBuf.tm_min << ":" << tmBuf.tm_sec << "] " << strLog << endl;
	}
	else {
		cout << "[E-" << tmBuf.tm_hour << ":" << tmBuf.tm_min << ":" << tmBuf.tm_sec << "] " << strLog << endl;
	}

	return WC_OK;
}

int WCLog::Write(const E_LEVEL eLevel, const string& strLog)
{
	int nRet = WC_NOK;

	if (&strLog == nullptr || strLog.empty()) {
		WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Parameter -> &strLog == nullptr || strLog.empty()") % __FILE__ % __FUNCTION__ % __LINE__));
		return nRet;
	}

	// 레벨 체크
	if (!CheckLevel(eLevel)) {
		WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Function -> Loglevel was inconsistency. ---> (File:%d != Log:%d) (strLog:%s)") 
			% __FILE__ % __FUNCTION__ % __LINE__ % m_nLevel % eLevel % strLog));
   		return -2;
	}

	long t;
	struct tm tmBuf;
	char ch, cBuf[20];
	int nLen;

	// 동기화
	if (m_bMutex) pthread_mutex_lock(&m_mutexLog);

	// 주기 체크
	time(&t);
	if (!localtime_r(&t, &tmBuf)) {
		WriteConsole(E_LEVEL_ERROR, "Function call fails ---> localtime_r");
	}
	else {
		if (m_ofs.is_open()) {
			CheckCycle(&tmBuf);
		}
	}

	// 문자열 설정
	if(eLevel == E_LEVEL_DEBUG) ch = 'D';
	else if(eLevel==E_LEVEL_WARN) ch = 'W';
	else if(eLevel==E_LEVEL_ERROR) ch = 'E';
	else if(eLevel==E_LEVEL_NORMAL) ch = 'N';
	else ch = 'A';

	nLen = sprintf(cBuf, "[%c-%02d:%02d:%02d] ", ch, tmBuf.tm_hour, tmBuf.tm_min, tmBuf.tm_sec);
	cBuf[nLen] = 0x00;	

	// 파일 쓰기
	if (m_ofs.is_open()) {
		m_ofs << cBuf << strLog << endl;
	}
	else {
		cout << cBuf << strLog << endl;
	}

	nRet = nLen;

	pthread_mutex_unlock(&m_mutexLog);

	return nRet;
}

int WCLog::WriteFormat(const E_LEVEL eLevel, const char *pcFmt, ...)
{
	int nRet = WC_NOK;

	if (!pcFmt) {
		WriteConsole(E_LEVEL_ERROR, "Null parameter ---> pcFmt");
		return nRet;
	}

	// 레벨 체크
	if (!CheckLevel(eLevel)) {
		WriteConsole(E_LEVEL_ERROR, str(boost::format("[%s:%s:%d] Function -> Loglevel was inconsistency. ---> (File:%d != Log:%d) (strLog:)") 
			% __FILE__ % __FUNCTION__ % __LINE__ % m_nLevel % eLevel));
   		return -2;
	}

	long t;
	struct tm tmBuf;
	va_list ap;
	char ch;
	int nLen, nLenBuf;

	// 동기화
	if (m_bMutex) pthread_mutex_lock(&m_mutexLog);

	// 주기 체크
	time(&t);
	if (!localtime_r(&t, &tmBuf)) {
		WriteConsole(E_LEVEL_ERROR, "Function call fails ---> localtime_r");
	}	
	else {
		if (m_ofs.is_open()) {
			CheckCycle(&tmBuf);
		}
	}

	// 문자열 설정
	va_start(ap, pcFmt);
	nLen = vsnprintf(&ch, 1, pcFmt, ap) + 1;	// 문자열수만큼 리턴됨. 가령 "abcde"이면 5가 리턴됨.
	va_end(ap);
	if (nLen <= 0) {
		WriteConsole(E_LEVEL_ERROR, "Function call fails ---> vsnprintf, nLen <= 0");
		nRet = WC_NOK;
	}
	else if (nLen > MAX_LOG_LENGTH) {
		WriteConsole(E_LEVEL_ERROR, "Function call fails ---> vsnprintf, nLen > MAX_LOG_LENGTH");
	}
	else { 
		// 버퍼 재할당
		nLenBuf = nLen + 13;	// NULL 1byte + Date 13byte
		if (!m_pcOutBuf) {
			m_nOutBufSz = nLenBuf + 100; // 여분 100byte
			m_pcOutBuf = new char[m_nOutBufSz];
		}
		else {
			if (m_nOutBufSz < nLenBuf) {
				WC_DELETEL(m_pcOutBuf);
				m_nOutBufSz = nLenBuf + 100; // 여분 100byte
				m_pcOutBuf = new char[m_nOutBufSz];
			}
		}

		if (m_pcOutBuf) {
			if(eLevel == E_LEVEL_DEBUG) ch = 'D';
			else if(eLevel==E_LEVEL_WARN) ch = 'W';
			else if(eLevel==E_LEVEL_ERROR) ch = 'E';
			else if(eLevel==E_LEVEL_NORMAL) ch = 'N';
			else ch = 'A';

			va_start(ap, pcFmt);
			sprintf(m_pcOutBuf, "[%c-%02d:%02d:%02d] ", ch, tmBuf.tm_hour, tmBuf.tm_min, tmBuf.tm_sec);
			vsnprintf(m_pcOutBuf+13, nLen, pcFmt, ap);
			va_end(ap);

			// 파일 쓰기
			if (m_ofs.is_open()) {
				m_ofs << m_pcOutBuf << endl;
			}
			else {
				cout << m_pcOutBuf << endl;
			}

			nRet = nLen;
		}
		else {
			WriteConsole(E_LEVEL_ERROR, "Function call fails ---> new");
			nRet = WC_NOK;
		}
	}

	pthread_mutex_unlock(&m_mutexLog);

	return nRet;
}

bool WCLog::CheckLevel(const E_LEVEL eLevel)
{
	int nFlag1, nFlag2;

	if (m_nLevel == E_LEVEL_ALL || eLevel == E_LEVEL_ALL)
		return true;

	nFlag1 = m_nLevel & E_LEVEL_NORMAL; 
	nFlag2 = eLevel & E_LEVEL_NORMAL;
	if (nFlag1 > 0 && nFlag1 == nFlag2) 
		return true;

	nFlag1 = m_nLevel & E_LEVEL_DEBUG; 
	nFlag2 = eLevel & E_LEVEL_DEBUG;
	if (nFlag1 > 0 && nFlag1 == nFlag2) 
		return true;
	
	nFlag1 = m_nLevel & E_LEVEL_WARN; 
	nFlag2 = eLevel & E_LEVEL_WARN;
	if (nFlag1 > 0 && nFlag1 == nFlag2) 
		return true;
	
	nFlag1 = m_nLevel & E_LEVEL_ERROR; 
	nFlag2 = eLevel & E_LEVEL_ERROR;
	if (nFlag1 > 0 && nFlag1 == nFlag2) 
		return true;

	return false;
}

bool WCLog::CheckCycle(struct tm* const ptmBuf)
{
 	if (!ptmBuf) {
		WriteConsole(E_LEVEL_ERROR, "Null parameter ---> ptmBuf");
		return false;
	}
	else if (!m_ofs.is_open()) {
		// 파일이 열리지 않으면 생성할 필요가 없기 때문에 바로 true 리턴
		return true;
	}

   	bool bChange = false;

	// 주기 체크
	if (m_eCycle==E_CYCLE_DAY) {
		if (m_tmCycle.tm_mday != ptmBuf->tm_mday)
			bChange = true;
	}
	else if (m_eCycle==E_CYCLE_MONTH) {
		if (m_tmCycle.tm_mon != ptmBuf->tm_mon) 
			bChange = true;
	}

	// 신규 파일 생성
	if (bChange)
	{	
		Close();
		if (Open(m_strName, m_nLevel, m_eCycle) == -1) {
			WriteConsole(E_LEVEL_ERROR, "Function call fails ---> Open");
			return false;
		}
	}

	return true;
}
