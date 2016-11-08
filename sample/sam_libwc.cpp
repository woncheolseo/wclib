/*
<libwc 예제>

0. 단위 테스트 
1. 기본 사용법1
9. 스트레스테 스트

Usage : sam_libwc 모드번호 프로세스수 쓰레드수

* 샘플코드 기준 양식
*/

#include "wc_lib.h"

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
		fprintf(stderr, "@ usage\n\tsam_libwc ModeNo ProcessCount ThreadCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 2-BasicUsage2, 9-StressTest\
			\n@ Example\n\tsam_libwc 0\n\tsam_libwc 1\n\tsam_libwc 9 3 10\n");
	}

	if (nMode == 0) Sam_Unittest(argc, argv);		// 단위테스트 - sam_libwc 0 (libwc 단위테스트만 실행시킬 경우, "--gtest_filter=UnitTest_libwc.*" 인자 추가)
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1 - sam_libwc 1
	else if (nMode == 9) Sam_Stress(argc, argv, 1);	// 스트레스테스트 - sam_libwc 9 3 10

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
	fprintf(stderr, "[%s] (No:%d)\n", __FUNCTION__, signo);
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

	testing::InitGoogleTest(&argc, argv); // Google Test에게 입력 인자를 넘겨준다. 이 부분이 gtest-gbar를 수행하면 gtes-gbar에서 argument로 --gtest_list_tests를 넣어주게 되어 있다. 프로그램 수행 인자로 google test의 동작을 제어하는 부분인가보다.

	nRet = RUN_ALL_TESTS(); // 모든 테스트를 수행한다.

	if (nRet == -1) fprintf(stderr, "[Er] RUN_ALL_TESTS\n");
}


/****************************************************************************************************************************************************************************************************
* 1. 기본사용법
* 
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
}

/****************************************************************************************************************************************************************************************************
* 9. 스트레스 테스트
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
*****************************************************************************************************************************************************************************************************/
//static int gnLine = 0;		// 라인수
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
	pthread_mutex_init(&g_Mutex, NULL);
	gLog1.Open("log/log_g1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);
	gLogT.Open("log/log_t1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);

	if (argc > 3) {
		nFork = atoi(argv[2]);		// 프로세스수
		nThread = atoi(argv[3]);	// 쓰레드수
		//gnLine = atoi(argv[4]);		// 라인수	
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
				fprintf(stderr, "[%s:%d] pthread_create (Pid:%d)(gnPno:%d)(i:%d)(Er:%d,%s)\n", __FUNCTION__, __LINE__, getpid(), gnPno, i, errno, (char *)strerror(errno));
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

		fprintf(stderr, "[%s] Child End (Pid:%d)(gnPno:%d)\n", __FUNCTION__, getpid(), gnPno);
	}
	else 
	{	// 부모 : 자식 프로세스 대기
		while (wait_r(NULL) > 0);
		fprintf(stderr, "[%s] Parend End 1 (Pid:%d)\n", __FUNCTION__, getpid());
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

	Sam_Basic1();

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
TEST(UnitTest_libwc, UTLibWcXMemcpy) 
{
	char cDest[10]={0,}, cSrc[10]="abcde";

	WcMemcpyChar(cDest, cSrc, sizeof(cDest)-1);

	// 단위테스트 - WcXMemcpyChar 호출후 결과
	size_t szLenDest, szLenSrc;

	szLenDest = strlen(cDest);
	szLenSrc = strlen(cSrc);

	EXPECT_TRUE(cDest != NULL);		
	EXPECT_STREQ(cDest, cSrc);
	EXPECT_GT(szLenDest, 0);
	EXPECT_NE(szLenDest, sizeof(cDest)-1);
	EXPECT_EQ(szLenDest, szLenSrc);
}

