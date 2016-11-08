/*
임시 예제

Usage : sam_svr 포트번호
*/

#include <wc_lib.h>

void SamInit();
void Sam_Basic1(unsigned short usPorta);

int main(int argc, char *argv[])
{
	char cOption[100];
	int nPort = 30000;
	bool bRun = false;

	SamInit();

	if (argc > 1) {
		if (strncmp(argv[1], "-r", 2) == 0) {
			if (argc > 2) {
				nPort = atoi(argv[2]);
				bRun = true;
				Sam_Basic1(nPort);
			}
		}
		else if (strncmp(argv[1], "-v", 2) == 0) {
			bRun = true;
			fprintf(stderr, "version 1.0.0.1\n");

		}
		else if (strncmp(argv[1], "-d", 2) == 0) {
			bRun = true;
			fprintf(stderr, "release 2015-10-27 10:30:00\n");
		}
	}

 	if (!bRun) {
		// 사용옵션
		fprintf(stderr, "@ usage\n\tsam_svr option port\n@ option\n\t-r : run\n\t-v : version\n\t-d : release date\
			\n@ Example\n\tsam_svr -r 30000\n\tsam_svr -v\n\tsam_svr -d\n");
	}

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
void Sam_Basic1(unsigned short usPorta)
{
	WCSocket wcTcpSvr("127.0.0.1", usPorta, WCSocket::E_MODE_SERVER, AF_INET, SOCK_STREAM);		// TCP Server 소켓 생성 및 오픈
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

										WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "(Fd:%07d) (Recv-%07llu) (Pack-%07llu) (Write-%07llu) / Total (R-%010llu) (P-%010llu) (W-%010llu)", pSock->GetFd(), szReadSz, nPackSz, szWrite, unTcpAppRead, unTcpAppPack, unTcpAppPack, unTcpAppWrite);
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

