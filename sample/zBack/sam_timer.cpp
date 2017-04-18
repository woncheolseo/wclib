/*
<WCTimer 예제>

0. 단위 테스트 
1. 기본 사용법1
2. 기본 사용법2
9. 스트레스 테스트

Usage : sam_timer 모드번호 프로세스수 쓰레드수 타이머수
*/

#include <wc_lib.h>

void SamInit();
void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1();
void Sam_Basic2();
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
		fprintf(stderr, "@ usage\n\tsam_timer ModeNo ProcessCount ThreadCount TimerCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 2-BasicUsage2, 9-StressTest\
			\n@ Example\n\tsam_timer 0\n\tsam_timer 1\n\tsam_timer 2\n\tsam_timer 9 2 10 100\n");
	}

	if (nMode == 0) Sam_Unittest(argc, argv);		// 단위테스트, sam_timer 0
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_timer 1
	else if (nMode == 2) Sam_Basic2();				// 기본사용법2, sam_timer 2
	else if (nMode == 9) Sam_Stress(argc, argv, 1);	// 스트레스테스트, sam_timer 9 2 10 100

	return 0;
}

/****************************************************************************************************************************************************************************************************
* 테스트 전역 클래스,함수,변수
*****************************************************************************************************************************************************************************************************/

// 타이머 테스트 객체, WITimer 클래스를 상속받은 객체는 IF_TimerEvent() 함수를 구현하여 WCTimer 타이머 사용 가능
class WCSam1 : public WCObject, public WITimer
{
public:
	WCSam1(const char *pcTest);
	~WCSam1();
	virtual void IF_TimerEvent(const timer_t tTimer, const int nTimerID, void *pData);
public:
	WCLog m_Log;
	char m_cTest[10];
	int m_nSTmCnt;	// 타이머 생성 카운트
	int m_nRTmCnt; 	// 타이머 이벤트 수신 카운트 
};
WCSam1::WCSam1(const char *pcTest) : m_nSTmCnt(0), m_nRTmCnt(0)
{
	memset(m_cTest, 0x00, sizeof(m_cTest));
	strcpy(m_cTest, pcTest);	
}
WCSam1::~WCSam1()
{
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[~WCSam1] (Name:%s)(%05d)\n\n", m_cTest, m_nRTmCnt);
}
void WCSam1::IF_TimerEvent(const timer_t tTimer, const int nTimerID, void *pData)
{
	m_nRTmCnt++;
	//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[WCSam1] (timer:%lu)(No:%d) (Name:%s)(S:%05d)(R:%05d) I Love You\n\n", tTimer, nTimerID, m_cTest, m_nSTmCnt, m_nRTmCnt);
	m_Log.CoutlnFormat(WCLog::E_LEVEL_ALL, "[WCSam1] (timer:%lu)(No:%d)(Data:%p) (Name:%s)(S:%05d)(R:%05d) I Love You\n", tTimer, nTimerID, pData, m_cTest, m_nSTmCnt, m_nRTmCnt);

	if (nTimerID == 3) {
		if (pData) {
			int *pnNo = (int *)pData;
			cout << "[WCSam1] Data: " << *pnNo << endl;
		}
	}	
}

// 타이머 테스트 객체
class WCSam2 : public WCObject, WITimer
{
public:
	WCSam2(const char *pTest);
	~WCSam2();
	virtual void IF_TimerEvent(const timer_t tTimer, const int nTimerID, void *pData);

public:
	WCLog m_Log;
	char m_cTest[10];
	int m_nRTmCnt;	// 타이머 이벤트 수신 카운트 
};
WCSam2::WCSam2(const char *pcTest) : m_nRTmCnt(0)
{
	memset(m_cTest, 0x00, sizeof(m_cTest));
	strcpy(m_cTest, pcTest);
}
WCSam2::~WCSam2()
{
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[~WCSam2] (Name:%s)(%05d)\n\n", m_cTest, m_nRTmCnt);
}
void WCSam2::IF_TimerEvent(const timer_t tTimer, const int nTimerID, void *pData)
{
	m_nRTmCnt++;
	//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[WCSam2] (timer:%lu)(No:%d) (Name:%s)(%05d) Hello World\n\n", tTimer, nTimerID, m_cTest, m_nRTmCnt);
	m_Log.CoutlnFormat(WCLog::E_LEVEL_ALL, "[WCSam2] (timer:%lu)(No:%d)(Data:%p) (Name:%s)(%05d) Hello World\n\n", tTimer, nTimerID, pData, m_cTest, m_nRTmCnt);
}

/////////////////////////////////////////////////////////////////////////////////////////////

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
* 타이머를 생성하여 사용한다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	WCSam1 wcSam1("Sam"), *pwcSam1;
	WCTimer tWcTimer, *pwcTimer;
	timer_t timer;
	int nRet, nData1 = 17;

	pwcSam1 = new WCSam1("Son");
	pwcTimer = new WCTimer;

	// 타이머 실행
	nRet = tWcTimer.StartTimer((WITimer *)&wcSam1, 1, 3.5, 0);						// 3.5초 간격으로 5번 발생 
	nRet = tWcTimer.StartTimer((WITimer *)pwcSam1, 2, 5.3, 0);						// 5.3초 간격으로 무한 발생 
	nRet = pwcTimer->StartTimer((WITimer *)pwcSam1, 3, 4.7, 5, &nData1);			// 4.7초 간격으로 5번 발생
	nRet = pwcTimer->StartTimer((WITimer *)&wcSam1, 4, 7.2, 0);						// 7.2초 간격으로 무한 발생

	// 대기1
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Wait 1");
	while(g_bLoop) pause();
	g_bLoop = true;
}

/****************************************************************************************************************************************************************************************************
* 1. 기본 사용법2
*
* 타이머를 여러개 생성하여 객체를 삭제하고 타이머 발생 여부를 확인한다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic2()
{
	WCSam1 wcSam1("Sam1"), *pwcSam1;
	WCSam2 wcSam2("Sam2"), *pwcSam2, *pwcSam3;
	WCTimer tWcTimer2,tWcTimer1;
	WCTimer *pWcTimer3,*pWcTimer4;
	timer_t timer;
	int nRet;

	pwcSam1 = new WCSam1("pSam1");
	pwcSam2 = new WCSam2("pSam2");
	pwcSam3 = new WCSam2("pSam3");
	pWcTimer3 = new WCTimer;
	pWcTimer4 = new WCTimer;

	// 타이머 실행
	nRet = pWcTimer3->StartTimer((WITimer *)&wcSam1, 1, 2.0, 0);		// 2.0초 간격으로 무한발생 
	nRet = pWcTimer3->StartTimer((WITimer *)&wcSam2, 2, 3.0, 20);		// 3.0초 간격으로 20번발생 
	nRet = tWcTimer1.StartTimer((WITimer *)pwcSam1, 3, 3.3, 50);		// 3.3초 간격으로 50번발생 
	nRet = pWcTimer4->StartTimer((WITimer *)&wcSam2, 4, 4.2, 30);		// 4.2초 간격으로 30번발생 
	nRet = tWcTimer2.StartTimer((WITimer *)pwcSam2, 5, 5.5, 0);			// 5.5초 간격으로 무한발생 

	// 대기1
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Wait 1");
	while(g_bLoop) pause();
	g_bLoop = true;

	// 타이머에서 삭제 
	tWcTimer1.DeleteTimer((WITimer *)pwcSam1);
	// 대기2
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Wait 2");
	while(g_bLoop) pause();
	g_bLoop = true;

	// 타이머인터페이스 소멸
	WC_DELETE(pwcSam2);
	// 대기3
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Wait 3");
	while(g_bLoop) pause();
	g_bLoop = true;

	// 타이머객체 소멸
	WC_DELETE(pWcTimer3);
	// 대기4
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Wait 4");
	while(g_bLoop) pause();
	g_bLoop = true;

	// 종료
	WC_DELETE(pwcSam1);
	WC_DELETE(pwcSam3);
	WC_DELETE(pWcTimer4);
}


/****************************************************************************************************************************************************************************************************
* 9. 스트레스 테스트
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
*****************************************************************************************************************************************************************************************************/
static int gnLine = 0;	// 타이머수
static int gnPno = 0;	// 프로세스번호(사용자정의)
pthread_mutex_t g_Mutex;
WCLog gLog1, gLogT;

void * ThreadFunc(void *pArg);

void Sam_Stress(int argc, char *argv[], int nType)
{
	pthread_t *t_pthread;
	pthread_attr_t *t_pthreadattr;	
	int i=0, nFork=1, nThread=1, nRet=0, nPidChild=0;

	// 변수 초기
	pthread_mutex_init(&g_Mutex, NULL);
	gLog1.Open("log/log_g1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);
	gLogT.Open("log/log_t1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);

	if (argc > 3) {
		nFork = atoi(argv[2]);		// 프로세스수
		nThread = atoi(argv[3]);	// 쓰레드수
		gnLine = atoi(argv[4]);		// 타이머수
	}

	// 프로세스 복사
	for(i=0; i<nFork; i++) {
		if ((nPidChild = fork()) <= 0) 
			break;
	}
	gnPno = i * nThread;

	if (nPidChild == -1) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%d] nPidChid == -1 (Pid:%d)(gnPno:%d)\n", __FUNCTION__, __LINE__, getpid(), gnPno);
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
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%d] pthread_create (Pid:%d)(gnPno:%d)(i:%d)(Er:%d,%s)\n", __FUNCTION__, __LINE__, getpid(), gnPno, i, errno, (char *)strerror(errno));
				sleep(1);
				i--;
			}
			else {
				//WcLogPrintf(WCLog::E_MODE_ALL,"[%s:%d] (Pid:%d)(gnPno:%d)(i:%d)\n",__FUNCTION__,__LINE__,getpid(),gnPno,i);
				pthread_detach(t_pthread[i]);
			}
			usleep(1000);
		}

		while(g_bLoop) pause();

		if (t_pthread) delete[] t_pthread;
		if (t_pthreadattr) delete[] t_pthreadattr;

		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Child End (Pid:%d)(gnPno:%d)\n", __FUNCTION__, getpid(), gnPno);
	}
	else 
	{	// 부모 : 자식 프로세스 대기
		while (wait_r(NULL) > 0);
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] Parend End 1 (Pid:%d)\n", __FUNCTION__, getpid());
	}
}

void * ThreadFunc(void *pArg)
{
	int nPno = 0;
	int nType = 0;

	if (pArg) nType = *((int*)pArg);

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

	WCSam1 wcSam1("swc");
	WCSam1 *pwcSam1 = new WCSam1("kmk");
	WCTimer tTimer1, tTimer2;
	//timer_t timer[100];
	char cTemp[50];
	int nRet;
	memset(cTemp, 0x00, sizeof(cTemp));

	// 로그 생성
	sprintf(cTemp, "log/Alog_%05d.txt", nPno);
	wcSam1.m_Log.Open(cTemp);
	sprintf(cTemp, "log/Blog_%05d.txt", nPno);
	pwcSam1->m_Log.Open(cTemp);

	for(int i=0; i<gnLine; i++)
	{
		nRet = tTimer1.StartTimer((WITimer *)&wcSam1, 1, 0.3, 10); 	// 10번 발생
		nRet = tTimer1.StartTimer((WITimer *)&wcSam1, 2, 2.5, 10);	// 10번 발생
		nRet = tTimer1.StartTimer((WITimer *)&wcSam1, 3, 1.3, 10);	// 10번 발생
		nRet = tTimer1.StartTimer((WITimer *)&wcSam1, 4, 3.2, 10);	// 10번 발생
		nRet = tTimer1.StartTimer((WITimer *)&wcSam1, 5, 1.7, 10);	// 10번 발생
		wcSam1.m_nSTmCnt += 50; // 타이머생성카운트

		nRet = tTimer2.StartTimer((WITimer *)pwcSam1, 1, 2.3, 10);	// 10번 발생
		nRet = tTimer2.StartTimer((WITimer *)pwcSam1, 2, 0.7, 10);	// 10번 발생
		nRet = tTimer2.StartTimer((WITimer *)pwcSam1, 3, 3.2, 10);	// 10번 발생
		nRet = tTimer2.StartTimer((WITimer *)pwcSam1, 4, 1.1, 10);	// 10번 발생
		nRet = tTimer2.StartTimer((WITimer *)pwcSam1, 5, 2.0, 10);	// 10번 발생
		pwcSam1->m_nSTmCnt += 50; // 타이머생성카운트

		usleep(1000);
	}

	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Wait (Pid:%d)(Pno:%d)\n", getpid(), nPno);
	while(1)
	{
		// 생성숫자와 수신숫자가 같으면 종료
		if (wcSam1.m_nSTmCnt == wcSam1.m_nRTmCnt && pwcSam1->m_nSTmCnt == pwcSam1->m_nRTmCnt) 
			break;
		sleep(1);
	}

	// 종료
	WC_DELETE(pwcSam1);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 종료
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 종료시간 기록
	gettimeofday(&tv,NULL);
	gLogT.CoutlnFormat(WCLog::E_LEVEL_ALL, "[%s] Time End (Pid:%d)(Pno:%d) (Time:%ld-%ld)", __FUNCTION__, getpid(), nPno, tv.tv_sec, tv.tv_usec);
	// 쓰레드 종료
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[%s] End (Pid:%d)(Pno:%d)", __FUNCTION__, getpid(), nPno);

	return NULL;
}

// 단위테스트 
TEST(UnitTest_WCTimer, UTTimerBasic1) 
{

}