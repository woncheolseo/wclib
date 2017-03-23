/*
<WCThread 예제>

0. 단위 테스트 
1. 기본 사용법1
9. 스트레스 테스트

Usage : sam_thread 모드번호 프로세스수 쓰레드수 라인수
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
		fprintf(stderr, "@ usage\n\tsam_thread ModeNo ProcessCount ThreadCount LienCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 9-StressTest\
			\n@ Example\n\tsam_thread 0\n\tsam_thread 1\n\tsam_thread 9 2 10 100\n");
	}

	if (nMode == 0) Sam_Unittest(argc, argv);		// 단위테스트, sam_thread 0
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_thread 1
	else if (nMode == 9) Sam_Stress(argc, argv, 1);	// 스트레스테스트, sam_thread 9 2 10 100

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
	// virtual int RecvEvent(const char *pcData, const int nPackSz) { return 0; }

public:
	bool m_bLoop;
	int m_nLine;	
};

WCTestT::WCTestT() : m_bLoop(true), m_nLine(10)
{
}

WCTestT::WCTestT(const char *pcName) : WCThread(pcName), m_bLoop(true), m_nLine(10000)
{
}

WCTestT::~WCTestT()
{
	m_bLoop = false;
}

// 쓰레드 실행 함수
void WCTestT::Run()
{
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL,"[%s] Start (%s)", __FUNCTION__, GetName());			

	char cFile[50], cTemp[1000];
	int nLen;

	// 각각의 파일에 쓰기
	WCLog tLog;
	sprintf(cFile, "log/log_%s.txt", GetName());
	tLog.Open(cFile, WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);
	for(int i=0; i<m_nLine; i++) 
	{
		if (m_bLoop == false) break;	// Stop() 함수에 의해 종료조건이 만족하면 루프를 빠져나간다.

		nLen = sprintf(cTemp,"[%s][%s] 1ng2ng3ng4ng5ng6ng7ng8ng9ngAngBngCngDngEngFngGngHngIngJngKngLngMngNngOngPngQngRngSngTngUngVngWngXngYngZnga-%03d", __FUNCTION__, GetName(), i);
		cTemp[nLen] = 0x00;
		tLog.CoutlnFormat(WCLog::E_LEVEL_ALL, cTemp);
		//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, cTemp);
		usleep(100);
	}
	tLog.Close();

	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Stop (%s)", __FUNCTION__, GetName());
}

// 쓰레드 실행 함수 종료 조건을 준다.
void WCTestT::Stop()
{
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Stop (%s)", __FUNCTION__, GetName());
	m_bLoop = false;
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

	if (nRet == -1) fprintf(stderr, "[Er] RUN_ALL_TESTS\n");
}

/****************************************************************************************************************************************************************************************************
* 1. 기본 사용법1
*
* WCThread 기본 사용법을 보여준다. WCThread는 자체로는 생성할 수 없고 상속받아서 사용해야 한다. 아래는 WCTest는 WCThread 자식 클래스이다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	WCTestT tTest("Test01");
	int nRes;

	// Thread 실행
	nRes = tTest.Start();
	if (nRes == -1) WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s] Add", __FUNCTION__);
	
	// Thread 대기
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Wait", __FUNCTION__);
	while(g_bLoop) pause();

	// Thread 종료
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Stop", __FUNCTION__);
	tTest.Stop();

	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] End", __FUNCTION__);
}

/****************************************************************************************************************************************************************************************************
* 9. 스트레스 테스트
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
* 다른 예제와 다르게 WCThread 객체를 이용해서 멀티쓰레드를 만들어 WCThread 부하 테스트를 한다.
*****************************************************************************************************************************************************************************************************/
static int gnLine = 0;		// 라인수
static int gnPno = 0;			// 프로세스번호(사용자정의)
pthread_mutex_t g_Mutex;
WCLog gLog1, gLogT;

void Sam_Stress(int argc, char *argv[], int nType)
{
	WCTestT *pWcTestT[1000];
	int i=0, nFork=1, nThread=1, nPidChild=0;
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
	for(i = 0; i < nFork; ++i) {
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
		for (i = 0; i < nThread; ++i)
		{
			// 쓰레드명
			sprintf(cNo, "%05d%05d", gnPno, i);
			cNo[10] = 0x00;

			// 쓰레드 생성
			pWcTestT[i] =  new WCTestT(cNo);
			if (pWcTestT[i]) {
				pWcTestT[i]->m_nLine = gnLine;
				pWcTestT[i]->Start(); // 쓰레드 시작
			}

			usleep(100);
		}

		while (g_bLoop) pause();

		for (i=0; i<nThread; i++) WC_DELETEWC(pWcTestT[i]);

		fprintf(stderr, "[%s] Child End (Pid:%d)(gnPno:%d)\n", __FUNCTION__, getpid(), gnPno);
	}
	else 
	{	// 부모 : 자식 프로세스 대기
		while (wait_r(NULL) > 0);
		fprintf(stderr,"[%s] Parend End 1 (Pid:%d)\n", __FUNCTION__, getpid());
	}
}

// 단위테스트 
TEST(UnitTest_WCThread, UTThreadBasic1) 
{

}
