/**
	@file wc_socket.cpp
   	@brief WCSocket 소스
*/

#include "wc_lib.h"

#define INITLIST m_nFd(WC_NOK), m_eMode(E_MODE_CLIENT), m_usDomain(AF_INET), m_nType(SOCK_STREAM), m_nProtocol(0), m_bConnect(false), m_nRecvIx(0), m_nProcIx(0), m_nDataCnt(0), m_nRecvBuf(4096), m_pcRecvBuf(NULL)

WCSocket::WCSocket() : INITLIST
{
	InitMember();
	InitSocket("", 0, E_MODE_CLIENT, AF_INET, SOCK_STREAM, 0, -1);
	InitPacket();
}

WCSocket::WCSocket(const char *pcIp, const unsigned short usPort, const E_MODE eMode, const unsigned short usDomain, const int nType, const int nProtocol, const int nFd) : INITLIST
{
	InitMember();
	InitSocket(pcIp, usPort, eMode, usDomain, nType, nProtocol, nFd);
	InitPacket();
}

WCSocket::~WCSocket()
{
	// 소켓 닫기
	Close();

	// 멤버 해제
	pthread_mutex_lock(&m_mutexRecvBuf);
	WC_FREE(m_pcRecvBuf);
	pthread_mutex_unlock(&m_mutexRecvBuf);
	pthread_mutex_destroy(&m_mutexRecvBuf);

	/*
	REPACK *pRePack;


	// Queue 해제
	pthread_mutex_lock(&m_mutexRepack);
	while (!m_queuePack.empty()) {
		pRePack = m_queuePack.front();
		if (pRePack) WcXFree(pRePack->pcData);
		WcXDelete(pRePack);
		m_queuePack.pop();
	}
	pthread_mutex_unlock(&m_mutexRepack);
	pthread_mutex_destroy(&m_mutexRepack);

	WCTRACE(WCLog::E_MODE_ALL,"[SOC:~Sock][%05d] (m_nRBuf:%d)(m_pcRBuf:%p)\n", m_nFd,m_nRecvBuf,m_pcRecvBuf);
	*/
}

inline void WCSocket::InitMember()
{
	pthread_mutex_init(&m_mutexRecvBuf, NULL);
	/*[test]
	pthread_mutex_init(&m_mutexRepack,NULL);
	WCTRACE(WCLog::E_MODE_ALL,"[SOC:SOC] 1 (Fd:%05d)\n",m_nFd);
	*/	
}

inline void WCSocket::InitSocket(const char *pcIp, const unsigned short usPort, const E_MODE eMode, const unsigned short usDomain, const int nType, const int nProtocol, const int nFd)
{
	if (m_cIp != pcIp) WcMemcpyChar(m_cIp, pcIp, sizeof(m_cIp)-1);
	m_usPort = usPort;
	m_eMode = eMode;
	m_usDomain = usDomain;
	m_nType = nType;
	m_nProtocol = nProtocol;
	m_nFd = nFd;

	if (m_eMode == E_MODE_ACCEPT) m_bConnect = true;
 	else m_bConnect = false;

	// 소켓 오픈할때마다 수신버퍼 재할당 
	pthread_mutex_lock(&m_mutexRecvBuf);
	m_nRecvBuf = 4096;
	WC_FREE(m_pcRecvBuf);
	m_pcRecvBuf = (char *)malloc(m_nRecvBuf);
	pthread_mutex_unlock(&m_mutexRecvBuf);

	/*[test]
	memset(m_cIpS,0x00,sizeof(m_cIpS));
	m_usPortS = 0;
	m_bLogin = false;

	//[swc,120207] 맴버맵 키값을 Name으로 할 경우, FD를 Name으로 저장 
	//int nLen;
	//char cName[30];
	//nLen = sprintf(cName,"%d",m_nFd);
	//cName[nLen]=0x00;
	//SetName(cName);
	*/

	//WCTRACE(WCLog::E_LEVEL_ALL, "[SOC:InitS][%05d] (SMode:%d)(Ip:%s)(Port:%d)(SDomain:%d)(SType:%d)(SProto:%d)(m_nRBuf:%d)(m_pcRBuf:%p)\n",m_nFd,m_nSockMode,m_cIp,m_usPort,m_usSockDomain,m_nSockType,m_nSockProtocol,m_nRecvBuf,m_pcRecvBuf);[test]
}

void WCSocket::InitPacket()
{
	m_nRecvIx = m_nProcIx = m_nDataCnt = 0;

	/*[test]
	pthread_mutex_lock(&m_mutexRepack);
	while(!m_queuePack.empty())
		m_queuePack.pop();
	pthread_mutex_unlock(&m_mutexRepack);
	WCTRACE(WCLog::E_MODE_ALL,"[SOC:InitP][%05d]\n", m_nFd); 
	*/
}

int WCSocket::Open()
{
	return
		Open(m_cIp, m_usPort, m_eMode, m_usDomain, m_nType, m_nProtocol);
}

int WCSocket::Open(const char *pcIp, const unsigned short usPort, const E_MODE eMode, const unsigned short usDomain, const int nType, const int nProtocol)
{
	int nSize, nBufSz;
	struct sockaddr_in saMain;

	// 변수 초기화
	nSize = sizeof(int); 
	saMain.sin_family = usDomain;
	saMain.sin_addr.s_addr = htonl(INADDR_ANY);	// 서버주소를 찾아서 자동으로 대입
	saMain.sin_port = htons(usPort);

	// 소켓 생성
	if (eMode != E_MODE_ACCEPT) {
		if ((m_nFd = socket((int)usDomain, nType, nProtocol)) == WC_NOK) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> socket (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
			return WC_NOK;
		}
	}

	// 소켓옵션값 얻기, 현재 RCVBUF 값을 얻어온다. SO_SNDBUF 값은 일단 배제
	/*
	if (getsockopt(m_nFd, SOL_SOCKET, SO_RCVBUF, &nBufSz, (socklen_t *)&nSize) < 0) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> getsockopt (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
	  	return (m_nFd = WC_NOK); 
	}
	*/

	// 소켓옵션값 설정
	// bind로 할당된 소켓자원을 프로세스가 재사용할 수 있도록 허락
	nBufSz = 1;
	if (setsockopt(m_nFd, SOL_SOCKET, SO_REUSEADDR, (void *)&nBufSz, (socklen_t)nSize) == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> setsockopt (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
		return (m_nFd = WC_NOK);
	}
	// 커널 레벨에서 약 2시간 동안 수신이 없을 경우 패킷 보내서 연결 여부 확인
	nBufSz = 1;
	if (setsockopt(m_nFd, SOL_SOCKET, SO_KEEPALIVE, (void *)&nBufSz, (socklen_t)nSize) == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> setsockopt (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
		return (m_nFd = WC_NOK);
	}

	/* 
	// 소켓 옵션 처리
	if (getsockopt(m_nFd, SOL_SOCKET, SO_REUSEADDR, &nBufSz, (socklen_t *)&nSize) < 0) 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "SO_REUSEADDR");
	else 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "SO_REUSEADDR %d", nBufSz);

	//[test]
	if (getsockopt(m_nFd, SOL_SOCKET, SO_KEEPALIVE, &nBufSz, (socklen_t *)&nSize) < 0) 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "SO_KEEPALIVE");
	else 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "SO_KEEPALIVE %d", nBufSz);

	if (getsockopt(m_nFd, SOL_SOCKET, SO_RCVTIMEO, &nBufSz, (socklen_t *)&nSize) < 0) 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "SO_RCVTIMEO");
	else 
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "SO_RCVTIMEO %d", nBufSz);
	
	// 소켓 Close를 완벽하게 하기위해 이렇게 하라던데... 별 소용없는듯. 나중에 연구
	 struct linger lng;
	 lng.l_onoff = 1;
	 lng.l_linger = 0;
	 if (setsockopt(m_nFd, SOL_SOCKET, SO_LINGER, (char *)&lng, sizeof(lng)) == WC_NOK) {
	  WcLogPrintf(WCLog::E_MODE_ERROR,"[SOC:Open][%05d] setsockopt (Er:%s)\n", m_nFd,(char *)strerror(errno));
		return (m_nFd = WC_NOK);
	/ }
	*/

	if (eMode == E_MODE_SERVER) {
		// 소켓 bind
		if (bind(m_nFd, (struct sockaddr *)&saMain, sizeof(saMain)) == WC_NOK) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> bind (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
			return (m_nFd = WC_NOK);
		}
		// 소켓 리슨
		if (nType == SOCK_STREAM) {
			if (listen(m_nFd, 10) == WC_NOK) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> listen (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
				return (m_nFd = WC_NOK);
			}
		}
		nSize = sizeof(saMain);
		if (getsockname(m_nFd, (struct sockaddr*)&saMain, (socklen_t *)&nSize) == WC_NOK) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> getsockname (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
			return (m_nFd = WC_NOK);
		}
	}

	// 멤버변수 설정
	if (usPort == 0) m_usPort = ntohs(saMain.sin_port);
	else m_usPort = usPort;
	InitSocket(pcIp, m_usPort, eMode, usDomain, nType, nProtocol, m_nFd);
	InitPacket();
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "Socket open ---> (Fd:%05d)(IP:%s)(Port:%d) (Mode:%d)", m_nFd, pcIp, m_usPort, eMode);	//[test] TRACE로 변경해야 하나 ?

	return m_nFd;
}

int WCSocket::Close()
{
	int nRes = 0;

	if (m_nFd >= 0)
	{
		nRes = close_r(m_nFd);
		if (nRes == -1) {
			// close() 함수가 실패하면 FD를 초기화하지 않는다.
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> close_r (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
		}
		else 
			m_nFd = WC_NOK;
	}

	return nRes;
}

int WCSocket::ShutDown()
{
	int nRet = WC_OK;

	if (m_nFd > 0) 
	{
		if (m_eMode == E_MODE_SERVER) {
			WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Server shutdown (Fd:%05d)", m_nFd);	//[test] TRACE로
			nRet = shutdown(m_nFd, 2);
		}
		else {
			if (m_bConnect) {
				WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Client-Connect shutdown (Fd:%05d)",m_nFd);	//[test] TRACE로
				nRet = shutdown(m_nFd, 0);
			}
			else {
				WC_TRACELF(WCLog::E_LEVEL_DEBUG, "Client-Disconnect shutdown (Fd:%05d)",m_nFd);	//[test] TRACE로
				nRet = shutdown(m_nFd, 1);
			}
		}
	}

	return nRet;
}

int WCSocket::Accept(char *pcAddress, int nSize, unsigned short *pusPort)
{
	/* 파라미터가 널이면 멤버 복사를 하지 않는다.
	if (!pcAddress || nSize <= 0 || !pusPort) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> (Fd:%05d) !pcAddress(%p) || nSize(%d) <= 0 || !pusPort(%p)", __FILE__, __FUNCTION__, __LINE__, m_nFd, pcAddress, nSize, pusPort);
		return WC_NOK;
	}*/

	int nNewFd = WC_NOK;
	struct sockaddr_in saAccept;
	unsigned int unSize;

	unSize = sizeof(saAccept);  // 이걸 꼭 초기화해야 accept 함수에서 Invalide agumnet 에러가 안 남 
	usleep(100);				//지연을 두지 않으니깐 accept()함수가 실패가 날 때가 있네... 정확한 이유는 모름 [swc-?]

	if( (nNewFd = accept(m_nFd, (struct sockaddr *)&saAccept, &unSize)) == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> accept (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
		return WC_NOK;
	}

	// 주소, 포트 복사
	if (pcAddress && nSize > 0 && pusPort) {
		WcStrncpy(pcAddress, inet_ntoa(saAccept.sin_addr), nSize);
		*pusPort = ntohs(saAccept.sin_port);
	}
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "Socket accept ---> (Fd:%05d)(NewFd:%d)(IP:%s)(Port:%d)(Domain:%d)(Type:%d)", m_nFd, nNewFd, inet_ntoa(saAccept.sin_addr), ntohs(saAccept.sin_port), m_usDomain, m_nType);	

	return
		nNewFd;
}

int WCSocket::Connect()
{
	return
		Connect(m_cIp, m_usPort);
}

int WCSocket::Connect(const char *pcIp, const unsigned short usPort)
{
	struct sockaddr_in saDest;

	saDest.sin_family = AF_INET;
	saDest.sin_addr.s_addr = inet_addr(pcIp);
	saDest.sin_port = htons(usPort);
	if(connect(m_nFd, (struct sockaddr *)&saDest, (socklen_t)sizeof(saDest)) == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> connect (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
		m_bConnect = false;
		return WC_NOK;
	}
	if (m_cIp != pcIp) WcMemcpyChar(m_cIp, pcIp, sizeof(m_cIp)-1);
	m_bConnect = true;
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Socket connect ---> (Fd:%05d)(IP:%s)(Port:%d)", m_nFd, pcIp, usPort);

	return WC_OK;
}

ssize_t WCSocket::Read()
{
	struct sockaddr_in saAddr;
	unsigned int nSaLen = sizeof(struct sockaddr);
	int nszRead = 0;
	ssize_t szRecv;

	// 수신크기 얻기
	if (ioctl(m_nFd, FIONREAD, &nszRead) == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> ioctl (Fd:%05d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
		Close();
		return WC_NOK;
	}

	if (nszRead <= 0) {
		// 수신크기가 0보다 작다는 것은 연결 끊어졌다는 신호
		if (m_nType == SOCK_STREAM) 
			szRecv = recv(m_nFd, &m_pcRecvBuf[m_nRecvIx], nszRead, 0);
		else
			szRecv = recvfrom(m_nFd, &m_pcRecvBuf[m_nRecvIx], nszRead, 0, (struct sockaddr*)&saAddr, &nSaLen);
		m_bConnect = false;
		//WC_TRACELF(WCLog::E_LEVEL_WARN, "read <= 0 ---> (Fd:%05d) (ReadSz:%d)(RecvSz:%d)", m_nFd, nReadSz, szRecvSz);
		return szRecv;
	}

	// 수신버퍼 재조정 - 수신버퍼보다 패킷이 크면 버퍼크기를 늘린다.
	int nDummy, nMaxBuf, nszEmpty;
	nDummy = m_nDataCnt + nszRead - m_nRecvBuf;
	nMaxBuf = MAX_RECV_SIZE - m_nRecvBuf;

	if (nDummy > nMaxBuf)
	{	// 최대 수신 용량을 초과하므로 수신 실패 처리하고 연결 끊기
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Dummy >= MaxBuf ---> (Fd:%05d)(Dummy:%d)(MaxBuf:%d)", __FILE__, __FUNCTION__, __LINE__, m_nFd, nDummy, nMaxBuf);
		Close();
		return WC_NOK;
	}
	else if (nDummy > 0 && nDummy <= nMaxBuf) 
	{	// 수신할 데이터가 현재 버퍼보다 크므로 버퍼 재조정
		char *pcTemp;
		int nTempEmpty;

		pthread_mutex_lock(&m_mutexRecvBuf);

		nTempEmpty = m_nRecvBuf - m_nProcIx;
		m_nRecvBuf += nDummy + 100;
		pcTemp = (char *)malloc(m_nRecvBuf);
		if (pcTemp) {
			// 데이터가 있으면 데이터 옮김
			if (m_nProcIx < m_nRecvIx) {
				memcpy(pcTemp, &m_pcRecvBuf[m_nProcIx], m_nDataCnt);
			}
			else if (m_nProcIx > m_nRecvIx) {
				memcpy(pcTemp, &m_pcRecvBuf[m_nProcIx], nTempEmpty);
				memcpy(&pcTemp[nTempEmpty], m_pcRecvBuf, m_nDataCnt-nTempEmpty);
			}
			else {
				// 옮길 데이터가 없음
			}
			WC_FREE(m_pcRecvBuf);
			m_pcRecvBuf = pcTemp;
			m_nRecvIx = m_nDataCnt;
			m_nProcIx = 0;
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "Memory size was changed ---> (Fd:%05d)(Size:%d)", m_nFd, m_nRecvBuf);
		}
		else
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> malloc (Fd:%05d)(Sz:%d)(Er:%d:%s)", __FILE__, __FUNCTION__, __LINE__, m_nFd, m_nRecvBuf, errno, (char *)strerror(errno));

		pthread_mutex_unlock(&m_mutexRecvBuf);
	}

	szRecv = 0;
	nszEmpty = m_nRecvBuf - m_nRecvIx;

	// 데이터 수신 - 수신데이터가 수신버퍼의 빈공간보다 크면 임시버퍼를 만들어서 한꺼번에 수신후, 쪼개서 원래 수신버퍼로 나눠서 담는다.
	if (nszEmpty >= nszRead)
	{
		if (m_nType == SOCK_DGRAM) {
			szRecv = recvfrom(m_nFd, &m_pcRecvBuf[m_nRecvIx], nszRead, 0, (struct sockaddr*)&saAddr, &nSaLen);
		}
		else {
			szRecv = recv(m_nFd, &m_pcRecvBuf[m_nRecvIx], nszRead, 0);
		}
		m_nRecvIx += (int)szRecv;
	}
	else
	{
		char *pcTemp2 = (char *)malloc(nszRead + 1);

		if (m_nType == SOCK_DGRAM) {
			szRecv = recvfrom(m_nFd, pcTemp2, nszRead, 0, (struct sockaddr*)&saAddr, &nSaLen);
		}
		else {
			szRecv = recv(m_nFd, pcTemp2, nszRead, 0);
		}

		if (nszEmpty < szRecv) 
		{
			int nszFirst = (int)szRecv - nszEmpty;
			memcpy(&m_pcRecvBuf[m_nRecvIx], pcTemp2, nszEmpty);
			if (nszFirst > 0) memcpy(&m_pcRecvBuf[0], &pcTemp2[nszEmpty], nszFirst);
			m_nRecvIx = nszFirst;
		}
		else
		{
			memcpy(&m_pcRecvBuf[m_nRecvIx], pcTemp2, szRecv);
			m_nRecvIx += (int)szRecv;
		}
		WC_FREE(pcTemp2);
	}

	// 수신변수 재조정
	if (m_nRecvIx == m_nRecvBuf) {
		m_nRecvIx = 0;
	}
	m_nDataCnt += (int)szRecv;

	// [test] UDP면 송신소켓의 정보를 저장
	/*if (m_nSockType == SOCK_DGRAM)
	{
		m_usPortS = ntohs(saAddr.sin_port);
		memset(m_cIpS,0x00,sizeof(m_cIpS));
		WcXStrcpy(m_cIpS, inet_ntoa(saAddr.sin_addr));
	}*/

	// 마지막 에러 체크 - 0보다 작으면 연결 끊어짐
	if (szRecv <= 0) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] RecvSz <= 0 ---> (Fd:%05d)(RecvSz:%d)", __FILE__, __FUNCTION__, __LINE__, m_nFd, szRecv);
		return szRecv;
	}
	else if (m_nRecvIx >= m_nRecvBuf) {
		// 수신인덱스가 버퍼크기보다 크면 ... 실제로 발생하진 않는다.
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] m_nRecvIx >= m_nRecvBuf ---> (Fd:%05d)(RecvIx:%d)(RecvSz:%d)", __FILE__, __FUNCTION__, __LINE__, m_nFd, m_nRecvIx, m_nRecvBuf);
		InitPacket();
	}

	return szRecv;
}

ssize_t WCSocket::Write()
{
	return 0;
}

ssize_t WCSocket::Write(const char *pcWriteBuf, const int nWriteSz, const char *pcDestIp, const unsigned short usDestPort)
{
	// 인자 검사 (생략해야 하나?)
	if (m_nFd <= 0 || !pcWriteBuf || nWriteSz <= 0) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Paramter was wrong ---> (Fd:%05d) m_nFd <= 0 || !pcWriteBuf(%p) || nWriteSz(%d) <= 0", __FILE__, __FUNCTION__, __LINE__, m_nFd, pcWriteBuf, nWriteSz);
		return WC_NOK;
	}

	/*[test]
	// 못 보낸 패킷 송신
	if (VWriteRe() == WC_NOK) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[SOC:%s:%d] (Fd:%05d) VWriteRe() == WC_NOK\n",__FUNCTION__,__LINE__,m_nFd);
		VClose();
		return WC_NOK;
	}
	*/

	int nError; //int nRet, nEr;
	ssize_t szRet;

	// 못 보낸 패킷은 리스트에 저장한다.
	if (m_nType == SOCK_STREAM) {
		szRet = send(m_nFd, &pcWriteBuf[0], nWriteSz, 0);
		//WcLogPrintf(WCLog::E_MODE_ALL,"[SOC:Write][%05d] (Ret:%d)(%s)\n", m_nFd,szRet,&pcWriteBuf[0]);
	}
	else {
		if (!pcDestIp) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Paramter was null ---> (Fd:%05d) !pcDestIp", __FILE__, __FUNCTION__, __LINE__, m_nFd);
			return 0;
		}
		struct sockaddr_in saDest;
		saDest.sin_addr.s_addr = inet_addr(pcDestIp);
		saDest.sin_family = AF_INET;
		saDest.sin_port = htons(usDestPort);
		szRet = sendto(m_nFd, &pcWriteBuf[0], nWriteSz, 0, (struct sockaddr *)&saDest, sizeof(struct sockaddr));
	}

	// 에러 처리: WSAEWOULDBLOCK, EAGIN일 경우는 재패킷
	if (szRet == -1) 
	{
		nError = errno;
		if (nError == EAGAIN || nError == EWOULDBLOCK || nError == ENOBUFS) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "Resend ---> (Fd:%05d)", m_nFd);

			/* [test]
			pthread_mutex_lock(&m_mutexRepack);
			REPACK *pRePack = new REPACK;
			if (pRePack) {
				pRePack->pcData = (char *)malloc(nWriteSz+1);
				if (pRePack->pcData) {
					memcpy(pRePack->pcData, pcWriteBuf, nWriteSz);
					pRePack->pcData[nWriteSz] = 0x00;
					pRePack->nFailCnt = 1;
					pRePack->nWriteSz = nWriteSz;
					if (pcDestIp) strcpy(pRePack->cDestIp,pcDestIp);
					pRePack->usDestPort = usDestPort;
					m_queuePack.push(pRePack);
				}
			}
			pthread_mutex_unlock(&m_mutexRepack);
			*/
		}
		else {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> send (Fd:%05d)(Er:%d,%s) !pcDestIp", __FILE__, __FUNCTION__, __LINE__, m_nFd, errno, (char *)strerror(errno));
			if (errno == 32) ShutDown(); // Broken pipe 일 경우다. 소켓을 닫는다. 콜백으로 처리해도 되고...
		}
	}

	return szRet;
}

/*
ssize_t WCSocket::VWriteRe()
{
	if (m_nFd <= 0) {
		WcLogPrintf(WCLog::E_MODE_ERROR, "[SOC:VWriteRe][%05d] m_nFd<=0\n");
		return WC_NOK;
	}

	int nEr; //nRet=0,nEr;//,nCount;
	REPACK *pRePack;
	ssize_t szRet = 0;

	// 재송신이 일정횟수 이내 실패일때만 바로 루프를 빠져나오고 나머지 경우는 삭제한다.
	
	pthread_mutex_lock(&m_mutexRepack);	
	
	while (!m_queuePack.empty()) {
		pRePack = m_queuePack.front();

		if (pRePack && pRePack->pcData) 
		{
			WcLogPrintf(WCLog::E_MODE_WARN, "[SOC:VWriteRe][%05d] START (Wsz:%d)\n", m_nFd,pRePack->nWriteSz);
			usleep(100);
			if(m_nSockType == SOCK_STREAM) {
				szRet = send(m_nFd, pRePack->pcData, pRePack->nWriteSz, 0);
			}
			else {
				struct sockaddr_in saDest;
				saDest.sin_addr.s_addr = inet_addr(pRePack->cDestIp);
				saDest.sin_family = AF_INET;
				saDest.sin_port = htons(pRePack->usDestPort);
				szRet = sendto(m_nFd, pRePack->pcData, pRePack->nWriteSz, 0, (struct sockaddr *)&saDest, sizeof(struct sockaddr));
			}
			WcLogPrintf(WCLog::E_MODE_WARN, "[SOC:VWriteRe][%05d] END (Wsz:%d)(Ret:%d)\n", m_nFd,pRePack->nWriteSz,szRet);

			if (szRet == WC_NOK) {
				nEr = errno;
				if (nEr==EAGAIN || nEr==EWOULDBLOCK || nEr==ENOBUFS) {
					pRePack->nFailCnt++;
					if (pRePack->nFailCnt > 10) {
						WcLogPrintf(WCLog::E_MODE_ERROR,"[SOC:VWriteRe][%05d] FailCnt(%d) > 100\n", m_nFd,pRePack->nFailCnt);
					}
					else {
						szRet = -2;
						break;
					}
				}
				else {
					WcLogPrintf(WCLog::E_MODE_ERROR,"[SOC:VWriteRe][%05d] send (Er:%s)\n", m_nFd,(char *)strerror(errno));
				}
			}
		}

		WcLogPrintf(WCLog::E_MODE_WARN,"[SOC:VWriteRe][%05d] send (Wsz:%d)(Ret:%d)\n", m_nFd,pRePack->nWriteSz,szRet);
		m_queuePack.pop();
		WcXFree(pRePack->pcData);
		WcXDelete(pRePack);
	}
	
	pthread_mutex_unlock(&m_mutexRepack);
*/	
	/*nCount = m_queuePack.size();
	if (nCount > 0) 
	{
		pthread_mutex_lock(&m_mutexRepack);	

		for(int i=0; i<nCount; i++)
		{
			pRePack = m_queuePack.front();
			if (pRePack && pRePack->pcData) 
			{
				WcLogPrintf(WCLog::E_MODE_WARN, "[SOC:VWriteRe][%05d] START (Wsz:%d)\n", m_nFd,pRePack->nWriteSz);
				usleep(100);
				if(m_nSockType == SOCK_STREAM) {
					nRet = send(m_nFd, pRePack->pcData, pRePack->nWriteSz, 0);
				}
				else {
					struct sockaddr_in saDest;
					saDest.sin_addr.s_addr = inet_addr(pRePack->cDestIp);
					saDest.sin_family = AF_INET;
					saDest.sin_port = htons(pRePack->usDestPort);
					nRet = sendto(m_nFd, pRePack->pcData, pRePack->nWriteSz, 0, (struct sockaddr *)&saDest, sizeof(struct sockaddr));
				}
				WcLogPrintf(WCLog::E_MODE_WARN, "[SOC:VWriteRe][%05d] END (Wsz:%d)(Ret:%d)\n", m_nFd,pRePack->nWriteSz,nRet);

				if (nRet == WC_NOK) {
					nEr = errno;
					if (nEr==EAGAIN || nEr==EWOULDBLOCK || nEr==ENOBUFS) {
						pRePack->nFailCnt++;
						if (pRePack->nFailCnt > 10) {
							WcLogPrintf(WCLog::E_MODE_ERROR,"[SOC:VWriteRe][%05d] FailCnt(%d) > 100\n", m_nFd,pRePack->nFailCnt);
						}
						else {
							nRet = -2;
							break;
						}
					}
					else {
						WcLogPrintf(WCLog::E_MODE_ERROR,"[SOC:VWriteRe][%05d] send (Er:%s)\n", m_nFd,(char *)strerror(errno));
					}
				}
			}

			WcLogPrintf(WCLog::E_MODE_WARN,"[SOC:VWriteRe][%05d] send (Wsz:%d)(Ret:%d)\n", m_nFd,pRePack->nWriteSz,nRet);
			m_queuePack.pop();
			WcXFree(pRePack->pcData);
			WcXDelete(pRePack);
		}

		pthread_mutex_unlock(&m_mutexRepack);
	}*/
/*	return szRet;
}
*/

int WCSocket::Packet(char *pcData, const int nHeadIx, const int nszLen, const int nszAdd, E_PACKET ePacket)
{
	WC_TRACEB
	(
		if (!pcData || nHeadIx < 0 || nHeadIx > 50 || nszLen > 16 || nszAdd < 0) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> (Fd:%05d) !pcData(%p) || nHeadIx(%d) > 50 || nszLen(%d) > 32", __FILE__, __FUNCTION__, __LINE__, m_nFd, pcData, nHeadIx, nszLen);
			return WC_NOK;
		}
	);

	// 패킷 미완성 - 수신데이터가 없거나 헤더위치+패킷크기변수 사이즈보다 작으면
	if (m_nDataCnt <= 0 || m_nDataCnt < nHeadIx + nszLen) {
		//WCTRACE(WCLog::E_MODE_WARN,"[SOC:Packet][%05d] DCNT is short (HFirst:%d)(LenSz:%d) (RBufSz:%d)(Rix:%d)(Pix:%d)(DCnt:%d)\n", m_nFd,nHeadFix,nLenSz,m_nRecvBuf,m_nRecvIx,m_nProcIx,m_nDataCnt);
		return WC_OK;
	}

	unsigned short uszData;
	int nszData, nszAll;
	char cTemp[50];

	// 수신버퍼의 남은 자리수를 고려하여 패킷 크기를 버퍼에 담는다.
	if (nszLen <= 0) {
		cTemp[0] = 0x00;
		nszData = 0;
	}
	else 
	{
		int nHeadFPos, nHeadLPos;

		nHeadFPos = m_nProcIx + nHeadIx;
		nHeadLPos = nHeadFPos + nszLen;

		if (nHeadFPos >= m_nRecvBuf) {
			nHeadFPos -= m_nRecvBuf;
			memcpy(cTemp, &m_pcRecvBuf[nHeadFPos], nszLen);
		}
		else {
			if (nHeadLPos > m_nRecvBuf) {
				int nszTempH = m_nRecvBuf - nHeadFPos;
				memcpy(cTemp, &m_pcRecvBuf[nHeadFPos], nszTempH);
				memcpy(&cTemp[nszTempH], &m_pcRecvBuf[0], nszLen - (nszTempH));
			}
			else {
				memcpy(cTemp, &m_pcRecvBuf[nHeadFPos], nszLen);
			}
		}
		cTemp[nszLen] = 0x00;

		// 바이너리 모드냐 아스키모드냐에 따라 패킷 크기를 계산
		if (ePacket == E_PACKET_BINARY)
		{
			if(nszLen == 2) {
				memcpy(&uszData, cTemp, nszLen);
				uszData = ntohs(uszData);
				nszData = uszData;
			}
			else if(nszLen == 4) {
				memcpy(&nszData, cTemp, nszLen);
				nszData = ntohl(nszData);
			}
			else 
				nszData = 0;
		}
		else 
			nszData = atoi(cTemp);
	}

	// 패킷 크기가 0보다 작거나 같으면 수신한 크기가 패킷 크기
	if (nszData <= 0 && nszAdd <= 0) {
		nszAll = m_nDataCnt;
	}
	else {
		nszAll = nszData + nszAdd;
	}

	// 패킷 추출
	if (nszAll > MAX_RECV_SIZE) 
	{	// 최대패킷크기보다 클수 없다.
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN,"[%s:%s:%d] Packet size was wrong ---> (Fd:%05d) (AllSz:%d)(DataSz:%d)(AddSz:%d) (RBufSz:%d)(Rix:%d)(Pix:%d)(DCnt:%d)", __FILE__, __FUNCTION__, __LINE__, m_nFd, nszAll, nszData, nszAdd, m_nRecvBuf, m_nRecvIx, m_nProcIx, m_nDataCnt);
		nszAll = WC_NOK;
		InitPacket();
	}
	else if (nszAll > m_nDataCnt)
	{ 	// 한 패킷이 수신된 Data보다 크다면 패킷미완성
		//WCTRACE(WCLog::E_MODE_WARN,"[SOC:Packet][%05d] COMPARE (nAllSz > m_nDataCnt) (AllSz:%d)(DataSz:%d)(AddSz:%d) (RBufSz:%d)(Rix:%d)(Pix:%d)(DCnt:%d)\n", m_nFd,nAllSz,nDataSz,nszAdd,m_nRecvBuf,m_nRecvIx,m_nProcIx,m_nDataCnt);
		nszAll = WC_OK;
		// InitPacket() 초기화하면 안 됨		
	}
	else if (nszAll > 0)
	{	// 패킷 정상
		int nszEmpty, nszFirst;

		nszEmpty = m_nRecvBuf - m_nProcIx;
		if (nszEmpty < nszAll) {
			nszFirst = nszAll - nszEmpty;
			memcpy(pcData, &m_pcRecvBuf[m_nProcIx], nszEmpty);
			if (nszFirst > 0) memcpy(&pcData[nszEmpty], &m_pcRecvBuf[0], nszFirst);
			m_nProcIx = nszFirst;
		}
		else {
			memcpy(pcData, &m_pcRecvBuf[m_nProcIx], nszAll);
			m_nProcIx += nszAll;
			if (m_nProcIx == m_nRecvBuf) m_nProcIx = 0;
		}
		m_nDataCnt -= nszAll;

		// 마지막 처리
		if (m_nDataCnt < 0 || m_nProcIx >= m_nRecvBuf) {
			// 에러다 
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"[%s:%s:%d] Packet was wrong. ---> (Fd:%05d) (AllSz:%d) (RBufSz:%d)(Rix:%d)(Pix:%d)(DCnt:%d)\n", __FILE__, __FUNCTION__, __LINE__, m_nFd, nszAll, m_nRecvBuf, m_nRecvIx, m_nProcIx, m_nDataCnt);
			nszAll = WC_NOK;
			InitPacket();
		}
		else {
			pcData[nszAll] = 0x00;
			// 패킷 처리하고 남은 데이터 크기가 0이면 수신변수들 초기화
			if (m_nDataCnt == 0) InitPacket();
		}
	}

	return nszAll;
}

/*
char * WCSocket::PacketRef(int *pnRecvSz, const int nHeadIx, const int nszLen, const int nszAdd, E_PACKET ePacket)
{
}
*/

int WCSocket::PacketJSON(char *pcData)
{
	WC_TRACEB
	(
		if (!pcData) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> (Fd:%05d)(!pcData)", __FILE__, __FUNCTION__, __LINE__, m_nFd);
			return WC_NOK;
		}
	);

	int nIx, nMatch, nRet;
	nRet = WC_OK;
	nMatch = 0;
	nIx = m_nProcIx;

	// 패킷 미완성 - 수신데이터가 0보다 작으면
	if (m_nDataCnt <= 0) {
		//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] DataCnt <= 0 ---> (Fd:%05d)", __FILE__, __FUNCTION__, __LINE__, m_nFd);
		//InitPacket();
		return WC_OK;
	}

	// JSON 패킷 완성 확인
	for (int i=0; i<m_nDataCnt; ++i)
	{
		if (m_pcRecvBuf[nIx] == '{') {
			++nMatch;
		}
		else if (m_pcRecvBuf[nIx] == '}') {
			--nMatch;
		}
		else  {
			// 인덱스 증가
			++nIx;
			if (nIx >= m_nRecvBuf) {
				nIx = 0;
			}
			continue;
		}

		// 매칭 확인
		if (nMatch < 0 || nMatch > 5) {
			// 매칭 오류 - depth는 5로 제한. {{{{{ 문자가 5 이상 넘어가면 오류
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Matching was wrong ---> (Fd:%05d)(Match:%d)", __FILE__, __FUNCTION__, __LINE__, m_nFd, nMatch);
			nRet = WC_NOK;
			InitPacket();
			break;
		}
		else if (nMatch == 0) {
			// 패킷 완성 
			int nszEmpty, nszFirst;

			nRet = ++i;
			nszEmpty = m_nRecvBuf - m_nProcIx;
			if (nszEmpty < nRet) {
				nszFirst = nRet - nszEmpty;
				memcpy(pcData, &m_pcRecvBuf[m_nProcIx], nszEmpty);
				if (nszFirst > 0) memcpy(&pcData[nszEmpty], &m_pcRecvBuf[0], nszFirst);
				m_nProcIx = nszFirst;
			}
			else {
				memcpy(pcData, &m_pcRecvBuf[m_nProcIx], nRet);
				m_nProcIx += nRet;
				if (m_nProcIx == m_nRecvBuf) m_nProcIx = 0;
			}
			m_nDataCnt -= nRet;

			// 마지막 처리
			if (m_nDataCnt < 0 || m_nProcIx >= m_nRecvBuf) {
				// 에러다 
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"[%s:%s:%d] Packet was wrong. ---> (Fd:%05d) (AllSz:%d) (RBufSz:%d)(Rix:%d)(Pix:%d)(DCnt:%d)\n", __FILE__, __FUNCTION__, __LINE__, m_nFd, nRet, m_nRecvBuf, m_nRecvIx, m_nProcIx, m_nDataCnt);
				InitPacket();
				nRet = 0;	// 에러지만 이전 패킷은 복사했기에 
			}
			else {
				pcData[nRet] = 0x00;

				// 패킷 처리하고 남은 데이터 크기가 0이면 수신변수들 초기화
				if (m_nDataCnt == 0) InitPacket();
			}

			break;
		}
		else {
			// 인덱스 증가
			++nIx;
			if (nIx >= m_nRecvBuf) {
				nIx = 0;
			}
		}
	}

	return nRet;
}

/*
void WCSocket::SetId(const char *pcUser) 
{
	memset(m_cId,0x00,sizeof(m_cId));
	memcpy(m_cId,pcUser,sizeof(m_cId)-1);
	m_cId[sizeof(m_cId)-1] = 0x00;
}

void WCSocket::SetPw(const char *pcPw)
{
	memset(m_cPw,0x00,sizeof(m_cPw));
	memcpy(m_cPw,pcPw,sizeof(m_cPw)-1);
	m_cPw[sizeof(m_cPw)-1] = 0x00;
}

int WCSocket::GetLocalIp(char *pcDevice, char *pcIp,int nLen)
{
	if (!pcDevice || !pcIp) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[SOC:%s:%d] !pcDevice(%p) || !pcIp(%p)\n",__FUNCTION__,__LINE__,pcDevice,pcIp);
		return WC_NOK;
	}

	struct ifaddrs *pIfa=NULL, *pIfp=NULL;

	if (getifaddrs(&pIfp) < 0)
	{
		WcLogPrintf(WCLog::E_MODE_ERROR,"[SOC:%s:%d] getifaddrs\n",__FUNCTION__,__LINE__);
		return WC_NOK;
	}

	for (pIfa=pIfp; pIfa; pIfa=pIfa->ifa_next)
	{
		char cIp[30];
		socklen_t salen;

		if (pIfa->ifa_addr->sa_family == AF_INET) salen = sizeof(struct sockaddr_in);
		else if (pIfa->ifa_addr->sa_family == AF_INET6) salen = sizeof(struct sockaddr_in6);	
		else continue;

		if (getnameinfo(pIfa->ifa_addr, salen, cIp, sizeof(cIp), NULL, 0, NI_NUMERICHOST) < 0) {
			WcLogPrintf(WCLog::E_MODE_WARN,"[SOC:%s:%d] getnameinfo\n",__FUNCTION__,__LINE__);
			continue;
		}
		WCTRACE(WCLog::E_MODE_WARN,"[SOC:%s:%d] [%s][%s]\n",__FUNCTION__,__LINE__,pIfa->ifa_name,cIp);

		if (strcmp(pIfa->ifa_name,pcDevice)==0 && strchr(cIp,'.')) memcpy(pcIp,cIp,nLen);
	}

	freeifaddrs(pIfp);

	return 0;
}
*/

