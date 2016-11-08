/*
<WCManagerThread 예제>

0. 단위 테스트 
1. 기본 사용법1
9. 스트레스 테스트

Usage : sam_manager_thread 모드번호 프로세스수 쓰레드수 라인수
*/

#include <wc_lib.h>

void SamInit();
void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1();
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
		fprintf(stderr, "@ usage\n\tsam_manager_thread ModeNo ProcessCount ThreadCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 9-StressTest\
			\n@ Example\n\tsam_manager_thread 0\n\tsam_manager_thread 1\n\tsam_manager_thread 9 2 10 100\n");
	}

	if (nMode == 0) Sam_Unittest(argc, argv);		// 단위테스트, sam_manager_thread 0
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_manager_thread 1
	else if (nMode == 9) Sam_Stress(argc, argv, 1);	// 스트레스테스트, sam_manager_thread 9 5 20 10000

	return 0;
}


/****************************************************************************************************************************************************************************************************
* 테스트 전역 클래스,함수,변수
*****************************************************************************************************************************************************************************************************/

// Thread 클래스를 상속받아서 사용
class WCTestT : public WCThread
{
public:
	WCTestT();
	WCTestT(const char *pcName);
	~WCTestT();
	virtual void Run();
	virtual void Stop();
	virtual int RecvEvent(const char *pcData, const int nPackSz);

public:
	bool m_bLoop;
	int m_nLine;
	int m_nThread;
	WCLog mLog;
};

WCTestT::WCTestT() : m_bLoop(true), m_nLine(10), m_nThread(1)
{
	char cFile[50];
	sprintf(cFile, "log/log_%s.txt", GetName());
	mLog.Open(cFile, WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);
}

WCTestT::WCTestT(const char *pcName) : WCThread(pcName), m_bLoop(true), m_nLine(10000), m_nThread(1)
{
	char cFile[50];
	sprintf(cFile, "log/log_%s.txt", GetName());
	mLog.Open(cFile, WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);	
}

WCTestT::~WCTestT()
{
	m_bLoop = false;
	mLog.Close();
}

// 쓰레드 실행 함수
void WCTestT::Run()
{
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL,"[%s] Start (%s)", __FUNCTION__, GetName());

	char cTemp[1000], cNo[11];
	int nLen = 0;

	// 쓰레드 통신 테스트를 위해 수신측 쓰레드명 설정
	
	WcMemcpyChar(cNo, GetName(), sizeof(cNo)-1); 
	WcMemcpyChar(cTemp, &cNo[5], 5); 
	nLen = atoi(cTemp);
	nLen++;
	if (nLen >= m_nThread) nLen = 0;
	sprintf(&cNo[5], "%05d", nLen);

	// 파일에 쓰기
	for(int i=0; i<m_nLine; ++i) 
	{
		if (m_bLoop == false) break;	// Stop() 함수에 의해 종료조건이 만족하면 루프를 빠져나간다.

		nLen = sprintf(cTemp,"[%s][%s][%s] 1ng2ng3ng4ng5ng6ng7ng8ng9ngAngBngCngDngEngFngGngHngIngJngKngLngMngNngOngPngQngRngSngTngUngVngWngXngYngZnga-%10d", __FUNCTION__, GetName(), cNo, i);
		cTemp[nLen] = 0x00;
		//mLog.CoutlnFormat(WCLog::E_LEVEL_ALL, cTemp);
		//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, cTemp);

		// 쓰레드통신 : 다른 쓰레드로 데이터를 보낸다.
		if (m_pManager)	((WCManagerThread *)m_pManager)->WriteIpc(cNo, cTemp, (unsigned short)nLen);
		
		usleep(100);
	}

	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Stop (%s)", __FUNCTION__, GetName());
}

// 쓰레드 실행 함수 종료 조건을 준다.
void WCTestT::Stop()
{
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Stop (%s)", __FUNCTION__, GetName());
	m_bLoop = false;
}

// 쓰레드 내부 통신 : 쓰레드를 통해서 데이터를 수신하면 발생한다.
int WCTestT::RecvEvent(const char *pcData, const int nPackSz)
{
	//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[Right][%s] [%3d] [%s]", GetName(), nPackSz, pcData);
	mLog.CoutlnFormat(WCLog::E_LEVEL_ALL, pcData);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// 프로세스 대기 플래그
static bool g_bLoop = true;

// INT 시그널 핸들링
static void mn_sigint(int signo, siginfo_t *info, void *context)
{
	fprintf(stderr,"[%s] (No:%d)\n",__FUNCTION__,signo);
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
* WCManagerThread 사용법을 보여준다. WCThread는 자체로는 생성할 수 없고 상속받아서 사용해야 한다. 아래는 WCTest는 WCThread 자식 클래스이다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	WCManagerThread *pMan = new WCManagerThread();
	WCTestT *pTh1,*pTh2;
	WCTestT tTh1("WCSwc00000"), tTh2("WCSwc00001");
	int nRes;

	pTh1 = new WCTestT("WCSwc00002");
	pTh2 = new WCTestT("WCSwc00003");
	tTh1.m_nThread = 4;
	tTh2.m_nThread = 4;
	pTh1->m_nThread = 4;
	pTh2->m_nThread = 4;

	// 쓰레드매니저에 추가 
	nRes = pMan->Add(&tTh1);
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s] Add (%s)", __FUNCTION__, tTh1.GetName());

	nRes = pMan->Add(&tTh2);
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"[%s] Add (%s)", __FUNCTION__, tTh2.GetName());

	nRes = pMan->Add(pTh1);
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"[%s] Add (%s)", __FUNCTION__, pTh1->GetName());

	nRes = pMan->Add(pTh2);
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"[%s] Add (%s)", __FUNCTION__, pTh2->GetName());
	
	// 쓰레드매니저를 통해 쓰레드 실행
	if (pMan->Start() == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s] Start\n", __FUNCTION__);

	// 대기
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Wait 1", __FUNCTION__);
	while (g_bLoop) pause();

	// 쓰레드매니저를 통해 쓰레드 종료
	pMan->Stop();
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Stop\n", __FUNCTION__);

	// 해제
	WC_DELETEWC(pTh1);
	WC_DELETEWC(pTh2);
	WC_DELETEWC(pMan);
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] End\n", __FUNCTION__);
}


/****************************************************************************************************************************************************************************************************
* 9. 스트레스 테스트
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
* 다른 예제와 다르게 WCThread,WCManagerThread를 이용해서 멀티쓰레드를 만들어 WCThread,WCManagerThread 부하 테스트를 한다.
*****************************************************************************************************************************************************************************************************/
static int gnLine=0;	// 라인수
static int gnPno=0;		// 프로세스번호(사용자정의)
pthread_mutex_t g_Mutex;
WCLog gLog1, gLogT;

void Sam_Stress(int argc, char *argv[], int nType)
{
	WCTestT *pWcTestT[1000];
	WCManagerThread tMan;
	int i=0, nFork=1, nThread=1, nPidChild=0, nRes;
	char cNo[11];

	// 변수 초기
	pthread_mutex_init(&g_Mutex,NULL);
	gLog1.Open("log/log_g1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);
	gLogT.Open("log/log_t1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);

	if (argc > 3) {
		nFork = atoi(argv[2]);		// 프로세스수
		nThread = atoi(argv[3]);	// 쓰레드수
		gnLine = atoi(argv[4]);		// 라인수	
	}

	// 프로세스 복사
	for(i=0; i<nFork; i++) {
		if ((nPidChild = fork()) <= 0) 
			break;
	}
	gnPno = i * nThread;

	if (nPidChild == -1) {
		fprintf(stderr, "[%s:%d] nPidChid == -1 (Pid:%d)(gnPno:%d)\n",__FUNCTION__,__LINE__,getpid(),gnPno);
		exit(0);
	}

	// 실행
	if (nPidChild == 0) 
	{
		for (i=0; i<nThread; i++)
		{
			// 쓰레드명
			sprintf(cNo, "%05d%05d", gnPno, i);
			cNo[10]=0x00;

			// 쓰레드 생성
			pWcTestT[i] =  new WCTestT(cNo);
			if (pWcTestT[i]) {
				pWcTestT[i]->m_nLine = gnLine;
				pWcTestT[i]->m_nThread = nThread;
				// 쓰레드매니저에 추가 
				nRes = tMan.Add(pWcTestT[i]);
				if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s] VAdd (%s)", __FUNCTION__, pWcTestT[i]->GetName());
			}

			usleep(1000);
		}

		// 쓰레드매니저를 통해 쓰레드 실행
		if (tMan.Start() == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s] Start", __FUNCTION__);

		while(g_bLoop) pause();

		for (i=0; i<nThread; i++) WC_DELETEWC(pWcTestT[i]);

		fprintf(stderr,"[%s] Child End (Pid:%d)(gnPno:%d)\n", __FUNCTION__, getpid(), gnPno);
	}  
	else 
	{	// 부모 : 자식 프로세스 대기
		while (wait_r(NULL) > 0);
		fprintf(stderr,"[%s] Parend End 1 (Pid:%d)\n", __FUNCTION__, getpid());
	}
}
