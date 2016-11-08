/*
<WCManagerSocket 예제>

0. 단위 테스트 
1. 기본 사용법1
9. 스트레스 테스트

Usage : sam_manager_socket 모드번호 프로세스수 쓰레드수
*/

#include <wc_lib.h>

void SamInit();
void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1(unsigned short usPorta, unsigned short usPortb);
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
		fprintf(stderr, "@ usage\n\tsam_manager_socket ModeNo ProcessCount ThreadCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 9-StressTest\
			\n@ Example\n\tsam_manager_socket 0\n\tsam_manager_socket 1\n\tsam_manager_socket 9 2 10\n");
	}

	if (argc > 1) {
		nMode = atoi(argv[1]);
	}

	if (nMode == 0) Sam_Unittest(argc,argv);		// 단위테스트, sam_manager_socket 0
	else if (nMode == 1) Sam_Basic1(30000, 30000);	// 기본사용법1, sam_manager_socket 1
	else if (nMode == 9) Sam_Stress(argc, argv, 1);	// 스트레스테스트, sam_manager_socket 9 2 10

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
* WCManagerSocket 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1(unsigned short usPorta, unsigned short usPortb)
{
	WCSocket wcTcpSvr("127.0.0.1", usPorta, WCSocket::E_MODE_SERVER, AF_INET, SOCK_STREAM);		// TCP Server 소켓 생성 및 오픈
	WCSocket wcTcpCli("192.168.1.223", usPorta, WCSocket::E_MODE_CLIENT, AF_INET, SOCK_STREAM);	// TCP Client 소켓 생성 및 오픈
	WCSocket wcUdpSvr("127.0.0.1", usPortb, WCSocket::E_MODE_SERVER, AF_INET, SOCK_DGRAM);		// UDP Server 소켓 생성
	WCSocket wcUdpCli("192.168.1.223", usPortb, WCSocket::E_MODE_CLIENT, AF_INET, SOCK_DGRAM);	// UDP Client 소켓 생성
	WCSocket *pwcTcpApp=NULL, *pSock=NULL;
	WCManagerSocket wcManSock;
	struct epoll_event epEvent[10];
	int nRes,nWait,nPackSz;
	ssize_t szReadSz = 0, szWrite = 0;
	char cData[10000]; //, cSend[10000];
	unsigned long long unTcpAppRead=0, unTcpAppPack=0, unTcpAppWrite=0;
	//unsigned long long unTcpCliRead=0,unTcpCliPack=0;
	//unsigned long long unUdpSvrRead=0,unUdpSvrPack=0;
	//unsigned long long unUdpCliRead=0,unUdpCliPack=0;	
	memset(&epEvent,0x00,sizeof(epEvent));

	// epoll 생성
	nRes = wcManSock.CreateEpoll();
	if (nRes == -1) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'CreateEpoll' function was failed. ---> (Er:%d:%s)", errno, (char *)strerror(errno));
		return;
	}

	// 소켓매니저에 소켓 등록 
	nRes = wcManSock.Add(&wcTcpSvr);
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'Add' function was failed. ---> TcpSvr");
	nRes = wcManSock.Add(&wcTcpCli);
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'Add' function was failed. ---> TcpCli");
	nRes = wcManSock.Add(&wcUdpSvr);
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'Add' function was failed. ---> UdpSvr");
	nRes = wcManSock.Add(&wcUdpCli);
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'Add' function was failed. ---> UdpCli");

	// TCP Client 연결 
	//nRes = wcTcpCli.Connect("192.168.15.10",usPorta);
	//nRes = wcTcpCli.Connect("127.0.0.1",usPorta);

	nRes = wcTcpCli.Connect();
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "'Connect' function was failed. ---> TcpCli");

	// event 수신
	while (g_bLoop)
	{
		nWait = wcManSock.WaitEpoll(epEvent);
		if (nWait == -1) {
			if (errno==EINTR) continue;
			else {
				wcManSock.Close();			// 모든 멤버 소켓 닫기 
				wcManSock.RemoveAll();		// 모든 멤버 그룹에서 삭제
				g_bLoop = false;
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"[%s] nWait == -1 (MapCnt:%d)", __FUNCTION__, wcManSock.Size());
				break;
			}
		}
		else if(nWait == 0) {
			WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG,"[%s] nEpWait == 0 (MapCnt:%d)", __FUNCTION__, wcManSock.Size());
			continue;
		}
		else
		{	
			for (int i=0; i<nWait; i++) 
			{
				pSock = (WCSocket *)wcManSock.Find(epEvent[i].data.fd);
				if (pSock)
				{
					if (epEvent[i].events == EPOLLIN || epEvent[i].events == 17 || epEvent[i].events == 25)
					{	
						if (pSock->GetMode() == WCSocket::E_MODE_SERVER && pSock->GetType() == SOCK_STREAM)
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
									nRes = wcManSock.Add(pwcTcpApp);
									if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Add - TcpApp");
								}
							}
						}
						else 
						{	// 데이터 수신
							szReadSz = pSock->Read();
							if (szReadSz <= 0) {
								WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "[%s] Read <= 0 (Fd:%d)", __FUNCTION__, pSock->GetFd());

								unTcpAppRead = 0;
								unTcpAppPack = 0;
								unTcpAppWrite = 0;

								wcManSock.Remove(pSock);
								if (pSock->GetMode() == WCSocket::E_MODE_ACCEPT) {
									WC_DELETEWC(pSock); /// ACCEPT 소켓은 new로 할당해줬으므로 메모리 삭제한다.
									pwcTcpApp = NULL;
								}
								if (wcManSock.Size() <= 0) {
									g_bLoop = false;
									break;
								}
							}
							else {
								//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "[%s] Recv (Fd:%d)(Read:%d)", __FUNCTION__, pSock->GetFd(), szReadSz);
								unTcpAppRead += szReadSz;
	
								// 데이터 가져오기
								for(int k=0; k<100; k++) 
								{
									nPackSz = pSock->Packet(cData, 0, 0, szReadSz, WCSocket::E_PACKET_BINARY);
									if(nPackSz <= 0) break;
									else {
										unTcpAppPack += nPackSz;							
										//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "[%s] Pack (Fd:%d)(Pack:%d)", __FUNCTION__, pSock->GetFd(), nPackSz);
										//WcLogPrintfnByte(cData,nPackSz,WCLog::E_TYPE_CHAR);										

										// 데이터 송신
										//nszSend = sprintf(cSend, "Recv - (ReadCnt:%llu) (ReadSz:%llu)", unTcpAppCountRead, unTcpAppRead);
										//cSend[nszSend] = 0x00;				
										//nszSend = 10;//[test]		
										//szWrite = pSock->Write(cSend, nszSend);
										szWrite = pSock->Write(cData, nPackSz);
										if (szWrite <= 0) {

										}
										else {
											unTcpAppWrite += szWrite;
										}

										WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "[%s] Pack (Fd:%d)(R:%llu)(P:%llu)(W:%llu)", __FUNCTION__, pSock->GetFd(), unTcpAppRead, unTcpAppPack, unTcpAppPack, unTcpAppWrite);
									}
								}
							}
						}
					}
					else if (epEvent[i].events==EPOLLHUP)
					{
						WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "[%s] EPOLLHUP (Fd:%d)", __FUNCTION__, epEvent[i].data.fd);
					}						
					else if (epEvent[i].events==EPOLLERR)
					{
						WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_WARN, "[%s] EPOLLERR (Fd:%d)", __FUNCTION__, epEvent[i].data.fd);
						wcManSock.Close();	
						wcManSock.RemoveAll();
						g_bLoop=false;
						break;
					}
					else
					{
						WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "[%s] EPOLLETC(%d) (Fd:%d)", __FUNCTION__, epEvent[i].events, epEvent[i].data.fd);
					}
				}
				epEvent[i].data.fd = -1;
			}
		}
	}
	
	WC_DELETEWC(pwcTcpApp);

	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "[%s] End", __FUNCTION__);
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

	Sam_Basic1((unsigned short)(30000+nPno),(unsigned short)(30001+nPno));

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
TEST(UnitTest_WCManagerSocket, UTManagerSocketBasic1) 
{

}
