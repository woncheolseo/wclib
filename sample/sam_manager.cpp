/*
<WCManager 예제>

0. 단위 테스트 
1. 기본 사용법1
9. 스트레스 테스트

Usage : sam_manager 모드번호 프로세스수 쓰레드수 멤버최대수
*/

#include <wc_lib.h>
#include "../work/wc_manager.cpp"

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
		fprintf(stderr, "@ usage\n\tsam_manager ModeNo ProcessCount ThreadCount MemberMaxCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 9-StressTest\
			\n@ Example\n\tsam_manager 0\n\tsam_manager 1\n\tsam_manager 9 2 10 100\n");
	}

	if (argc > 1) {
		nMode = atoi(argv[1]);
	}

	if (nMode == 0) Sam_Unittest(argc, argv);		// 단위테스트, sam_manager 0
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_manager 1
	else if (nMode == 9) Sam_Stress(argc, argv, 1);	// 스트레스테스트, sam_manager 9 2 10 100

	return 0;
}


/****************************************************************************************************************************************************************************************************
* 테스트 전역 클래스,함수,변수
*****************************************************************************************************************************************************************************************************/

// WCManger 테스트 클래스 : WIMember 상속받음, WI는 인터페이스 클래스이므로 WC로 시작하는 객체클래스도 반드시 상속. 인터페이스 다중상속 예제를 위해 WITimer도 상속받음
//class WCSam : public WCObject, WIMember
class WCSam : public WCObject, public WIMember
{
public:
	WCSam(const char *pcTest);
	~WCSam();
	void Test();
public:
	WCLog m_Log;
};

WCSam::WCSam(const char *pcTest) : WCObject(pcTest)
{
}

WCSam::~WCSam()
{
}

void WCSam::Test()
{
	//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "[WCSam1] (Name:%s)", GetName());[test]
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
* WCManager 기본사용법을 보여준다. WCManager는 추상클래스이므로 직접 생성할 일은 없지만 상속받아서 사용예제를 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	WCManager<int> tManager;								// Key를 int로
	WCManager<string> *pManager = new WCManager<string>;	// Key를 string으로
	WCManager<double> tManager2;							// Key를 double로

	WCSam tOb1("tMyOb1"), tOb2("tMyOb2"), tOb3("tMyOb3");
	WCSam *pOb1 = new WCSam("pMyOb1");
	WCSam *pOb2 = new WCSam("pMyOb2");	pOb2->IF_SetInDel(true);	// pOb2는 Manager 내부에서 삭제하게 한다.
	WCSam *pOb3 = new WCSam("pMyOb3"); 

	// 멤버 추가 
	tManager.Add(1,(WIMember *)&tOb1);
	tManager.Add(2,(WIMember *)&tOb2);
	tManager.Add(3,(WIMember *)&tOb3);;
	tManager.Add(4,(WIMember *)pOb1);;
	tManager.Add(5,(WIMember *)pOb2);;
	tManager.Add(6,(WIMember *)pOb3);;
	pManager->Add(tOb1.GetName(),(WIMember *)&tOb1);
	pManager->Add(tOb2.GetName(),(WIMember *)&tOb2);
	pManager->Add(tOb3.GetName(),(WIMember *)&tOb3);
	pManager->Add(pOb1->GetName(),(WIMember *)pOb1);
	pManager->Add(pOb2->GetName(),(WIMember *)pOb2);
	pManager->Add(pOb3->GetName(),(WIMember *)pOb3);

	// 멤버 검색 (NULL 체크는 생략)
	WCSam *pSam1, *pSam2, *pSam3, *pSam4, *pSam5, *pSam6;
	pSam1 = (WCSam *)tManager.Find(1);				
	pSam2 = (WCSam *)tManager.Find(2);
	pSam3 = (WCSam *)tManager.Find(3);
	pSam4 = (WCSam *)tManager.Find(4);
	pSam5 = (WCSam *)tManager.Find(5);
	pSam6 = (WCSam *)tManager.Find(6);
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Mem 1 (Cnt:%d) (%s) (%s) (%s) (%s) (%s) (%s)", tManager.Size(), pSam1->GetName(),pSam2->GetName(),pSam3->GetName(),pSam4->GetName(),pSam5->GetName(),pSam6->GetName());
	pSam1 = (WCSam *)pManager->Find("tMyOb1");
	pSam2 = (WCSam *)pManager->Find("tMyOb2");
	pSam3 = (WCSam *)pManager->Find("tMyOb3");
	pSam4 = (WCSam *)pManager->Find("pMyOb1");
	pSam5 = (WCSam *)pManager->Find("pMyOb2");
	pSam6 = (WCSam *)pManager->Find("pMyOb3");
	if (pSam1 && pSam2 && pSam3 && pSam4 && pSam5 && pSam6)
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Mem 2 (Cnt:%d) (%s) (%s) (%s) (%s) (%s) (%s)", pManager->Size(), pSam1->GetName(),pSam2->GetName(),pSam3->GetName(),pSam4->GetName(),pSam5->GetName(),pSam6->GetName());

	// 멤버 삭제
	tManager.Remove(1);
	tManager.Remove(2);
	tManager.Remove(3);
	tManager.Remove(4);
	tManager.Remove(5);
	tManager.Remove(6);
	/*
	pManager->Remove("tMyOb1");
	pManager->Remove("tMyOb2");
	pManager->Remove("tMyOb3");
	*/
	pManager->Remove("pMyOb1");
	pManager->Remove("pMyOb2");
	pManager->Remove("pMyOb3");

	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Mem 3 (Cnt:%d) (Cnt:%d) (Ob1:%d) (ob2:%d)", tManager.Size(), pManager->Size(), pOb1->IsLive(), pOb2->IsLive());
	WC_DELETEWC(pOb1);
	//WC_DELETEWC(pOb2);		// pOb2는 Manager 내부에서 삭제하기 때문에 여기서 삭제하면 에러난다. 왜냐하면 pOb2 변수 자체는 null로 초기화되지 않아서 또 delete 하기 때문 
	WC_DELETEWC(pOb3);
	WC_DELETEWC(pManager);
	WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "Mem 4 (Cnt:%d)", tManager.Size());
}


/****************************************************************************************************************************************************************************************************
* 9. 스트레스 테스트
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
*****************************************************************************************************************************************************************************************************/
static int gnLine = 0;		// 멤버최대수
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
		gnLine = atoi(argv[4]);		// 멤버최대수
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
	int nPno = 0;
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

	WCManager<int> tManager;	// Key를 int로
	WCSam *pSam;
	int nRes;
	char cFile[50];
	
	// 멤버최대수 설정
	tManager.SetMaxMember(gnLine);

	// 각각의 파일에 쓰기
	WCLog tLog;
	sprintf(cFile,"log/log_%010d.txt",nPno);
	tLog.Open(cFile);

	// 삽입
	for(int i=0; i<gnLine; i++) 
	{
		WCSam tOb1("tMyOb1");
		nRes = tManager.Add(i,(WIMember *)&tOb1);
		if (nRes == -1) 
			tLog.CoutlnFormat(WCLog::E_LEVEL_ERROR, "[Add] (i:%d)", i);
	}
	tLog.CoutlnFormat(WCLog::E_LEVEL_ALL,"[Add] (Size:%d)", tManager.Size());

	// 검색
	for(int i=0; i<gnLine; i++) 
	{
		pSam = (WCSam *)tManager.Find(i);
		if (pSam == NULL) 
			tLog.CoutlnFormat(WCLog::E_LEVEL_ERROR,"[Find] (i:%d)", i);
	}

	// 삭제
	for(int i=0; i<gnLine; i++) 
	{
		tManager.Remove(i);
	}
	if (tManager.Size() > 0) 
		tLog.CoutlnFormat(WCLog::E_LEVEL_ERROR, "[Remove]");
	else
		tLog.CoutlnFormat(WCLog::E_LEVEL_ALL,"[Add] (Size:%d)", tManager.Size());


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
TEST(UnitTest_WCManager, UTManagerBasic1) 
{
}
