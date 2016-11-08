/*
<WCSocket 예제>

0. 단위 테스트 
1. 기본 사용법1
2. 기본 사용법2
9. 스트레스 테스트

Usage : sam_socket 모드번호 프로세스수 쓰레드수 
*/

#include <wc_lib.h>

void SamInit();
void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1(unsigned short usPort);
void Sam_Basic2(unsigned short usPorta, unsigned short usPortb);
void Sam_Stress(int argc, char *argv[], int nType);

int main(int argc, char *argv[])
{
	int nMode = -1;

	SamInit();

	if (argc > 1) {
		nMode = atoi(argv[1]);
	}
	else {
		// 사용옵션
		fprintf(stderr, "@ usage\n\tsam_socket ModeNo ProcessCount ThreadCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 2-BasicUsage2, 9-StressTest\
			\n@ Example\n\tsam_socket 0\n\tsam_socket 1\n\tsam_socket 2\n\tsam_socket 9 2 10\n");
	}

	if (nMode == 0) Sam_Unittest(argc, argv);		// 단위테스트, sam_socket 0
	else if (nMode == 1) Sam_Basic1(15000);			// 기본사용법1-서버, sam_socket 1
	else if (nMode == 2) Sam_Basic2(15000, 16000);	// 기본사용법2-서버&클라이언트, sam_socket 2
	else if (nMode == 9) Sam_Stress(argc, argv, 1);	// 스트레스테스트, sam_socket 9 3 10

	return 0;
}

/****************************************************************************************************************************************************************************************************
* 테스트 전역 클래스,함수,변수
*****************************************************************************************************************************************************************************************************/
// 프로세스 대기 플래그
static bool g_bLoop = true;

// INT 시그널 핸들링
static void mn_sigint(int signo, siginfo_t *info, void *context)
{
	fprintf(stderr,"[%s] (No:%d)\n", __FUNCTION__, signo);
	g_bLoop = false;
}

// 테스트에 필요한 기능 초기화
void SamInit()
{
	// signal 등록 (종료시키기 위해)
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = mn_sigint;
	if ((sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGINT, &act, NULL) == -1)) {};

	// 로그레벨 설정
	WCLOG_SETLEVEL(WCLog::E_LEVEL_ALL);	
}


/****************************************************************************************************************************************************************************************************
* 0. 단위 테스트
*****************************************************************************************************************************************************************************************************/
void Sam_Unittest(int argc, char *argv[])
{
	int nRet;

	testing::InitGoogleTest(&argc,argv);

	nRet = RUN_ALL_TESTS();

	if (nRet == -1) fprintf(stderr,"[Er] RUN_ALL_TESTS\n");
}

/****************************************************************************************************************************************************************************************************
* 1. 기본 사용법1
*
* WCSocket 서버 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1(unsigned short usPort)
{
	WCSocket wcTcpSvr;
	struct epoll_event epEventCtl, epEventWait[10];
	sigset_t old;
	sigset_t sigmask;	
	ssize_t szRead = 0, szWrite = 0;
	int nRes, nEp, nEpWait, nszPack, nszSend, nIxPack;
	WCSocket *pwcTcpApp = NULL;
	unsigned long long unTcpAppRead=0, unTcpAppPack=0, unTcpAppWrite=0, unTcpAppCountRead=0;
	char cData[10000], cSend[10000], *pcData = NULL, *pcTemp = NULL;
	cData[0] = 0x00;

	pcTemp = cSend;

	// 소켓 오픈 
	nRes = wcTcpSvr.Open("127.0.0.1", usPort, WCSocket::E_MODE_SERVER, AF_INET, SOCK_STREAM, 0);
	if (nRes == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'Open' function was failed. ---> (Er:%d:%s)", errno, (char *)strerror(errno));
		return;
	}

	// epoll 생성
	nEp = epoll_create(10);
	if (nEp < 0) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'epoll_create' function was failed. ---> (Er:%d:%s)", errno, (char *)strerror(errno));
		return;
	}

	// epoll 등록
	epEventCtl.events = (unsigned int)(EPOLLIN | EPOLLERR);
	epEventCtl.data.fd = wcTcpSvr.GetFd();
	nRes = epoll_ctl(nEp, EPOLL_CTL_ADD, wcTcpSvr.GetFd(), &epEventCtl);
	if (nRes < 0) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'epoll_ctl' function was failed. ---> (Er:%d:%s)", errno, (char *)strerror(errno));
		return;
	}

	// 이벤트 수신
	while (g_bLoop)
	{
		// event 대기
		sigfillset(&sigmask);
		sigdelset(&sigmask, SIGINT);
		sigprocmask(SIG_SETMASK, &sigmask, &old);
		nEpWait = epoll_wait(nEp, epEventWait, 10, 10000);	// 10초(10,000) 대기
		sigprocmask(SIG_SETMASK, &old, 0);

		// event 수신 에러
		if (nEpWait < 0)
		{
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "nEpWait < 0 ---> (Er:%d:%s)", errno, (char *)strerror(errno));
			if (errno == EINTR);
			else {				
			}

			break;
		}
		// event 대기시간 동안 아무 event가 발생하지 않음
		else if (nEpWait == 0)
		{
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "nEpWait == 0");
		}
		// event 발생
		else 
		{
			// event 발생 횟수만큼 loop
			for (int i = 0; i < nEpWait; ++i)
			{
				// 수신 event
				if (epEventWait[i].events==EPOLLIN || epEventWait[i].events==17 || epEventWait[i].events==25)
				{
					// TCP Server 데이터 수신 - 소켓 연결을 받아준다. 
					if (epEventWait[i].data.fd == wcTcpSvr.GetFd())
					{
						int nFdApp;
						char cIpApp[20];
						unsigned short usPortApp;
						memset(cIpApp, 0x00, sizeof(cIpApp));

						// 새로운 소켓과 연결시켜 준다.
						nFdApp = wcTcpSvr.Accept(cIpApp, (int)(sizeof(cIpApp)-1), &usPortApp);
						if (nFdApp > 0) 
						{
							pwcTcpApp = new WCSocket(cIpApp, usPortApp, WCSocket::E_MODE_ACCEPT, AF_INET, SOCK_STREAM, 0, nFdApp);	
							if (pwcTcpApp) {
								nRes = pwcTcpApp->Open();
								if (nRes != WC_NOK) {
									// epoll 등록
									epEventCtl.events = (unsigned int)(EPOLLIN | EPOLLERR);
									epEventCtl.data.fd = pwcTcpApp->GetFd();
									nRes = epoll_ctl(nEp, EPOLL_CTL_ADD, pwcTcpApp->GetFd(), &epEventCtl);
									if (nRes < 0) {
										WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'epoll_ctl' function was failed. ---> TcpApp (Er:%d:%s)", errno, (char *)strerror(errno));								
									}
								}
							}
						}
					}

					// TCP Accept 데이터 수신
					if (pwcTcpApp && epEventWait[i].data.fd == pwcTcpApp->GetFd())
					{
						szRead = pwcTcpApp->Read();
						if (szRead <= 0) {
							WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "Disconneted (%d) ---> 'Read' function was failed.", epEventWait[i].data.fd);								
							pwcTcpApp->Close();
							WC_DELETEWC(pwcTcpApp);
							unTcpAppRead = 0;
							unTcpAppPack = 0;
							unTcpAppWrite = 0;	
							unTcpAppCountRead = 0;	
							cData[0] = 0x00;					
						}
						else {
							unTcpAppRead += szRead;
							++unTcpAppCountRead;
							nIxPack = 0;

							//cout << "[RECV]" << endl << cData << endl;//[test]

							for(int k=0; k<10000; k++) 
							{
								nszPack = pwcTcpApp->Packet(&cData[nIxPack], 0, 0, 0, WCSocket::E_PACKET_BINARY);
								if(nszPack <= 0) break;
								else {
									unTcpAppPack += nszPack;
									nIxPack += nszPack;
													
									//cout << "[PACK]" << endl << cData << endl;//[test]							

									// 데이터 송신
									nszSend = sprintf(cSend, "Recv - (ReadCnt:%llu) (ReadSz:%llu)", unTcpAppCountRead, unTcpAppRead);
									cSend[nszSend] = 0x00;				
									nszSend = 10;//[test]		
									szWrite = pwcTcpApp->Write(cSend, nszSend);
									if (szWrite <= 0) {

									}
									else {
										unTcpAppWrite += szWrite;
									}								

									//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "TcpApp Read (Read:%llu)(Pack:%llu)", unTcpAppRead, unTcpAppPack);
									//WcLogPrintfnByte(cData,nPackSz,WCLog::E_TYPE_CHAR);
								}
							}

							WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "TcpApp Read (Fd:%d)(ReadCnt:%d)(ReadSz:%llu)(PackSz:%llu)(SendSz:%llu)", epEventWait[i].data.fd, unTcpAppCountRead, unTcpAppRead, unTcpAppPack, unTcpAppWrite);
						}
					}
				}
				// event 끊어짐 : // Cliet 소켓 생성후 연결을 하지 않으면 발생하더라. 따라서 생성 후 바로 연결해서 EPOLLHUP 발생안하게 한다.
				else if (epEventWait[i].events == EPOLLHUP)
				{
					WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "EPOLLHUP (Fd:%d)", epEventWait[i].data.fd);
				}
				// event 에러
				else if (epEventWait[i].events == EPOLLERR)
				{
					nEpWait = -1;
					break;
				}
				else
				{
					WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "EPOLL (Fd:%d)", epEventWait[i].data.fd);
				}
				memset(&epEventWait[i], 0x00, sizeof(epEventWait[i]));
			}
		}
    }	

	// 소켓 닫기 
	nRes = wcTcpSvr.Close();
	if (nRes == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'Close' function was failed. ---> (Er:%d:%s)", errno, (char *)strerror(errno));
		return;
	}
}

/****************************************************************************************************************************************************************************************************
* 1. 기본 사용법2
*
* 여러 개의 WCSocket 서버, 클라이언트 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic2(unsigned short usPorta, unsigned short usPortb)
{
	WCSocket wcTcpSvr("127.0.0.1", usPorta, WCSocket::E_MODE_SERVER, AF_INET, SOCK_STREAM);		// TCP Server 소켓 생성 및 오픈
	WCSocket wcTcpCli("192.168.1.223", usPorta, WCSocket::E_MODE_CLIENT, AF_INET, SOCK_STREAM);	// TCP Client 소켓 생성 및 오픈
	WCSocket wcUdpSvr;																			// UDP Server 소켓 생성
	WCSocket wcUdpCli;																			// UDP Client 소켓 생성
	WCSocket *pwcTcpApp=NULL;
	struct epoll_event epEvent[10],epEventR;
	sigset_t old;
	sigset_t sigmask;
	ssize_t szReadSz=0;
	char cData[10000], cSend[10000];
	int nRes,nEp=0,nEpWait,nPackSz,nszSend;
	unsigned long long unTcpAppRead=0,unTcpAppPack=0;
	unsigned long long unTcpCliRead=0,unTcpCliPack=0;
	unsigned long long unUdpSvrRead=0,unUdpSvrPack=0;
	unsigned long long unUdpCliRead=0,unUdpCliPack=0;
	unsigned long long unTcpAppWrite=0;
	memset(&epEvent,0x00,sizeof(epEvent));
	memset(&epEventR,0x00,sizeof(epEventR));

	// 소켓 오픈
	nRes = wcTcpSvr.Open();																		// TCP Server 오픈 
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"Open - TcpSvr");

	nRes = wcTcpCli.Open();																		// TCP Client 오픈 
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"Open - TcpCli");

	nRes = wcUdpSvr.Open("127.0.0.1",usPortb,WCSocket::E_MODE_SERVER,AF_INET,SOCK_DGRAM);		// UDP Server 오픈 
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"Open - UdpSvr");

	nRes = wcUdpCli.Open("192.168.1.202",usPortb,WCSocket::E_MODE_CLIENT,AF_INET,SOCK_DGRAM);	// UDP Client 오픈
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"Open - UdpCli");

	// TCP Client 연결 
	//nRes = wcTcpCli.Connect("192.168.15.10",usPorta);
	//nRes = wcTcpCli.Connect("127.0.0.1",usPorta);
	nRes = wcTcpCli.Connect();
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"Connect");

	// epoll 생성
	nEp = epoll_create(10);
	if (nEp < 0) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"epoll_create");

	// epoll 등록
	epEventR.events = (unsigned int)(EPOLLIN | EPOLLERR);
	epEventR.data.fd = wcTcpSvr.GetFd();
	nRes = epoll_ctl(nEp,EPOLL_CTL_ADD,wcTcpSvr.GetFd(),&epEventR);
	if (nRes < 0) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"epoll_ctl - TcpSvr");

	epEventR.events = (unsigned int)(EPOLLIN | EPOLLERR);
	epEventR.data.fd = wcTcpCli.GetFd();
	nRes = epoll_ctl(nEp,EPOLL_CTL_ADD,wcTcpCli.GetFd(),&epEventR);
	if (nRes < 0) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"epoll_ctl - TcpCli");

	epEventR.events = (unsigned int)(EPOLLIN | EPOLLERR);
	epEventR.data.fd = wcUdpSvr.GetFd();
	nRes = epoll_ctl(nEp,EPOLL_CTL_ADD,wcUdpSvr.GetFd(),&epEventR);
	if (nRes < 0) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"epoll_ctl - UdpSvr");

	epEventR.events = (unsigned int)(EPOLLIN | EPOLLERR);
	epEventR.data.fd = wcUdpCli.GetFd();
	nRes = epoll_ctl(nEp,EPOLL_CTL_ADD,wcUdpCli.GetFd(),&epEventR);
	if (nRes < 0) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"epoll_ctl - UdpCli");

	// event 수신
	while (g_bLoop)
	{
		// event 대기
		sigfillset(&sigmask);
		sigdelset(&sigmask, SIGINT);
		sigprocmask(SIG_SETMASK,&sigmask,&old);
		nEpWait = epoll_wait(nEp,epEvent,10,10000);	// 10초(10,000) 대기
		sigprocmask(SIG_SETMASK,&old,0);

		// event 수신 에러
		if (nEpWait < 0)
		{
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"[%s] nEpWait < 0 (Er:%d,%s)",__FUNCTION__,errno,(char *)strerror(errno));
			if (errno == EINTR);
			else {}

			break;
		}
		// event 대기시간 동안 아무 event가 발생하지 않음
		else if (nEpWait == 0)
		{
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG,"[%s] nEpWait == 0\n",__FUNCTION__);
		}
		// event 발생
		else 
		{
			// event 발생 횟수만큼 loop
			for (int i=0; i<nEpWait; i++)
			{
				// 수신 event
				if (epEvent[i].events==EPOLLIN || epEvent[i].events==17 || epEvent[i].events==25)
				{
					// TCP Server 데이터 수신 - 소켓 연결을 받아준다. 
					if (epEvent[i].data.fd == wcTcpSvr.GetFd())
					{
						int nFd;
						char cIp[20];
						unsigned short usPort;
						memset(cIp,0x00,sizeof(cIp));

						// 새로운 소켓과 연결시켜 준다.
						nFd = wcTcpSvr.Accept(cIp, sizeof(cIp)-1, &usPort);
						if (nFd > 0) {
							pwcTcpApp = new WCSocket(cIp, usPort, WCSocket::E_MODE_ACCEPT, AF_INET, SOCK_STREAM, 0, nFd);	
							if (pwcTcpApp) {
								nRes = pwcTcpApp->Open();
								if (nRes != -1) {
									// epoll 등록
									epEventR.events = (unsigned int)(EPOLLIN | EPOLLERR);
									epEventR.data.fd = pwcTcpApp->GetFd();
									nRes = epoll_ctl(nEp,EPOLL_CTL_ADD,pwcTcpApp->GetFd(),&epEventR);
									if (nRes < 0) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"epoll_ctl - TcpApp");								
								}
							}
						}
					}

					// TCP Client 데이터 수신
					if (epEvent[i].data.fd == wcTcpCli.GetFd())
					{
						szReadSz = wcTcpCli.Read();
						if (szReadSz <= 0) {
							WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN,"[%s] TcpCli Read <= 0",__FUNCTION__);
							wcTcpCli.Close();
							unTcpCliRead = 0;
							unTcpCliPack = 0;
						}
						else {
							unTcpCliRead += szReadSz;

							for(int k=0; k<100; k++) 
							{
								nPackSz = wcTcpCli.Packet(cData, 0, 0, 0, WCSocket::E_PACKET_BINARY);
								if(nPackSz <= 0) break;
								else {
									unTcpCliPack += nPackSz;							
									WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG,"[%s] TcpCli Read (port:%d) (%llu)(%llu)",__FUNCTION__,wcTcpCli.GetPort(),unTcpCliRead,unTcpCliPack);
									//WcLogPrintfnByte(cData,nPackSz,WCLog::E_TYPE_CHAR);
								}
							}
						}
					}

					// TCP Accept 데이터 수신
					if (pwcTcpApp && epEvent[i].data.fd == pwcTcpApp->GetFd())
					{
						szReadSz = pwcTcpApp->Read();
						if (szReadSz <= 0) {
							WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN,"[%s] TcpApp Read <= 0",__FUNCTION__);
							pwcTcpApp->Close();
							WC_DELETEWC(pwcTcpApp);
							unTcpAppRead = 0;
							unTcpAppPack = 0;
						}
						else {
							unTcpAppRead += szReadSz;

							for(int k=0; k<100; k++) 
							{
								nPackSz = pwcTcpApp->Packet(cData, 0, 0, 0, WCSocket::E_PACKET_BINARY);
								if(nPackSz <= 0) break;
								else {
									unTcpAppPack += nPackSz;
									WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] TcpApp Read (port:%d) (%llu)(%llu)",__FUNCTION__,pwcTcpApp->GetPort(),unTcpAppRead,unTcpAppPack);
									//WcLogPrintfnByte(cData,nPackSz,WCLog::E_TYPE_CHAR);

									/*// 데이터 송신
									nszSend = sprintf(cSend, "Recv - (ReadCnt:%llu) (ReadSz:%llu)", unTcpAppCountRead, unTcpAppRead);
									cSend[nszSend] = 0x00;				
									nszSend = 10;//[test]		
									szWrite = pwcTcpApp->Write(cSend, nszSend);
									if (szWrite <= 0) {

									}
									else {
										unTcpAppWrite += szWrite;
									}*/
								}
							}
						}
					}

					// UDP Server 데이터 수신
					if (epEvent[i].data.fd == wcUdpSvr.GetFd())
					{
						szReadSz = wcUdpSvr.Read();
						if (szReadSz <= 0) {
							WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "[%s] UdpSvr Read <= 0",__FUNCTION__);
							wcUdpSvr.Close();
							unUdpSvrRead = 0;
							unUdpSvrPack = 0;									
						}
						else {
							unUdpSvrRead += szReadSz;

							for(int k=0; k<100; k++) 
							{
								nPackSz = wcUdpSvr.Packet(cData, 0, 0, 0, WCSocket::E_PACKET_BINARY);
								if(nPackSz <= 0) break;
								else {
									unUdpSvrPack += nPackSz;
									WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] UdpSvr Read (port:%d) (%llu)(%llu)", __FUNCTION__,wcUdpSvr.GetPort(),unUdpSvrRead,unUdpSvrPack);
									//WcLogPrintfnByte(cData,nPackSz,WCLog::E_TYPE_CHAR);
								}
							}
						}
					}

					// UDP Client 데이터 수신
					if (epEvent[i].data.fd == wcUdpCli.GetFd())
					{
						szReadSz = wcUdpCli.Read();
						if (szReadSz <= 0) {
							WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "[%s] UdpCli Read <= 0", __FUNCTION__);
							wcUdpCli.Close();
							unUdpCliRead = 0;
							unUdpCliPack = 0;									
						}
						else {
							unUdpCliRead += szReadSz;

							for(int k=0; k<100; k++) 
							{
								nPackSz = wcUdpCli.Packet(cData, 0, 0, 0, WCSocket::E_PACKET_BINARY);
								if(nPackSz <= 0) break;
								else {
									unUdpCliPack += nPackSz;
									WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "[%s] UdpCli Read (port:%d) (%llu)(%llu)", __FUNCTION__,wcUdpCli.GetPort(),unUdpCliRead,unUdpCliPack);
									//WcLogPrintfnByte(cData,nPackSz,WCLog::E_TYPE_CHAR);
								}
							}
						}
					}
				}
				// event 끊어짐 : // Cliet 소켓 생성후 연결을 하지 않으면 발생하더라. 따라서 생성 후 바로 연결해서 EPOLLHUP 발생안하게 한다.
				else if (epEvent[i].events==EPOLLHUP)
				{
					WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "[%s] EPOLLHUP (Fd:%d)", __FUNCTION__,epEvent[i].data.fd);
				}
				// event 에러
				else if (epEvent[i].events==EPOLLERR)
				{
					nEpWait = -1;
					break;
				}
				else
				{
					WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "[%s] (EPOLL :%d)", __FUNCTION__, epEvent[i].events);
				}
				memset(&epEvent[i],0x00,sizeof(epEvent[i]));
			}
		}
    }
}


/****************************************************************************************************************************************************************************************************
* 9. 스트레스 테스트
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
*****************************************************************************************************************************************************************************************************/
//static int gnLine = 0;
static int gnPno = 0;		// 프로세스번호(사용자정의)
pthread_mutex_t g_Mutex;
WCLog gLog1, gLogT;

void * ThreadFunc(void *pArg);

void Sam_Stress(int argc, char *argv[], int nType)
{
	pthread_t *t_pthread;
	pthread_attr_t *t_pthreadattr;	
	int i=0, nFork=1, nThread=1, nRet=0, nPidChild=0;

	// 변수 초기
	pthread_mutex_init(&g_Mutex,NULL);
	gLog1.Open("log/log_g1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);
	gLogT.Open("log/log_t1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);

	if (argc > 3) {
		nFork = atoi(argv[2]);		// 프로세스수
		nThread = atoi(argv[3]);	// 쓰레드수
		//gnLine = atoi(argv[4]);
	}

	// 프로세스 복사
	for(i=0; i<nFork; i++) {
		if ((nPidChild = fork()) <= 0) 
			break;
	}
	gnPno = i * nThread;

	if (nPidChild == -1) {
		fprintf(stderr, "[%s:%d] nPidChid == -1 (Pid:%d)(gnPno:%d)\n", __FUNCTION__, __LINE__, getpid(), gnPno);
		exit(0);
	}

	// 실행
	if (nPidChild == 0) 
	{
		// 자식 : 쓰레드 생성
		t_pthread = new pthread_t[nThread];
		t_pthreadattr = new pthread_attr_t[nThread];

		for (i=0; i<nThread; i++)
		{
			pthread_attr_init(&t_pthreadattr[i]);
			if ((nRet = pthread_create(&t_pthread[i], NULL, ThreadFunc, (void *)&nType)) != 0) {
				fprintf(stderr,"[%s:%d] pthread_create (Pid:%d)(gnPno:%d)(i:%d)(Er:%d,%s)\n", __FUNCTION__, __LINE__, getpid(), gnPno, i, errno, (char *)strerror(errno));
				sleep(1);
				i--;
			}
			else {
				//fprintf(stderr,"[%s:%d] (Pid:%d)(gnPno:%d)(i:%d)\n", __FUNCTION__, __LINE__, getpid(), gnPno, i);
				pthread_detach(t_pthread[i]);
			}
			usleep(1000);
		}

		while(g_bLoop) pause();

		if (t_pthread) delete[] t_pthread;
		if (t_pthreadattr) delete[] t_pthreadattr;

		fprintf(stderr,"[%s] Child End (Pid:%d)(gnPno:%d)\n", __FUNCTION__, getpid(), gnPno);
	}
	else 
	{	// 부모 : 자식 프로세스 대기
		while (wait_r(NULL) > 0);
		fprintf(stderr,"[%s] Parend End 1 (Pid:%d)\n", __FUNCTION__, getpid());
	}
}

void * ThreadFunc(void *pArg)
{
	int nPno=0;
	//int nType = 0;

	//if (pArg) nType = *((int*)pArg);

	pthread_mutex_lock(&g_Mutex);
	nPno = gnPno++;
	pthread_mutex_unlock(&g_Mutex); 
	struct timeval tv;

	// 쓰레드 시작
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Start (Pid:%d)(Pno:%d)", __FUNCTION__, getpid(), nPno);
	// 시작시간 기록
	gettimeofday(&tv, NULL);
	gLogT.CoutlnFormat(WCLog::E_LEVEL_ALL, "[%s] Time Start (Pid:%d)(Pno:%d) (Time:%ld-%ld)", __FUNCTION__, getpid(), nPno, tv.tv_sec, tv.tv_usec);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 시작 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Sam_Basic1((unsigned short)(30000 + nPno));
	//Sam_Basic2((unsigned short)(30001+nPno),(unsigned short)(40001+nPno));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 종료
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// 종료시간 기록
	gettimeofday(&tv, NULL);
	gLogT.CoutlnFormat(WCLog::E_LEVEL_ALL, "[%s] Time End (Pid:%d)(Pno:%d) (Time:%ld-%ld)", __FUNCTION__, getpid(), nPno, tv.tv_sec, tv.tv_usec);
	// 쓰레드 종료
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] End (Pid:%d)(Pno:%d)", __FUNCTION__, getpid(), nPno);

	return NULL;
}

// 단위테스트 
TEST(UnitTest_WCSocket, UTSocketBasic1) 
{

}
