/*
<WCLog 예제>

0. 단위 테스트 
1. 기본 사용법1
2. 기본 사용법2
3. 기본 사용법3
90. 스트레스 테스트1
91. 스트레스 테스트2

Usage : sam_log 모드번호 프로세스수 쓰레드수 라인수
*/

#include <wc_lib.h>

// 프로세스 대기 플래그
static bool g_bLoop = true;

void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1();
void Sam_Basic2();
void Sam_Basic3();
void Sam_Stress(int argc, char *argv[], int nType);

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
}

// 메인 함수
int main(int argc, char *argv[])
{
	int nMode = -1;
	char cUsage[] = "@ usage\n\tsam_log ModeNo ProcessCount ThreadCount LineCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 2-BasicUsage2, 3-BasicUsage3, 90-StressTest, 91-StressTest\
			\n@ Example\n\tsam_log 0\n\tsam_log 1\n\tsam_log 2\n\tsam_log 3\n\tsam_log 90 2 10 100\n\tsam_log 91 2 10 100\n";

	SamInit();

	if (argc > 1) {
		nMode = atoi(argv[1]);
		if (nMode == 0) Sam_Unittest(argc, argv);			// 단위테스트, sam_log 0
		else if (nMode == 1) Sam_Basic1();					// 기본사용법1, sam_log 1
		else if (nMode == 2) Sam_Basic2();					// 기본사용법2, sam_log 2
		else if (nMode == 3) Sam_Basic3();					// 기본사용법3, sam_log 3
		else if (nMode == 90) Sam_Stress(argc, argv, 0);	// 스트레스테스트-각각의 파일에 쓰기, sam_log 90 2 10 100
		else if (nMode == 91) Sam_Stress(argc, argv, 1);	// 스트레스테스트-하나의 파일에 쓰기, sam_log 91 2 10 100
		else fprintf(stderr, cUsage);
	}
	else {
		fprintf(stderr, cUsage);
	}

	return 0;
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
* 로그파일을 생성하고 사용하는 기본 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	WCLog cLog;

	// 로그파일 오픈
	cLog.Open("zz01.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);				// 일별주기로 파일을 생성한다.

	// 로그파일 쓰기 
	cLog.WriteConsole(WCLog::E_LEVEL_ALL, str(boost::format("1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg") % 1));
	cLog.Write(WCLog::E_LEVEL_ALL, str(boost::format("1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg") % 2));
	cLog.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", 3);

	// 로그파일 닫기 
	cLog.Close();
}


/****************************************************************************************************************************************************************************************************
* 2. 기본 사용법2
*
* 로그파일을 생성하고 사용하는 기본 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic2()
{
	WCLog cLog1, cLog2, cLog3, cLog4, cLog5, cLog6;
	char *pcTemp, cTemp[20], cTems[2];
	memset(cTemp,0x00,sizeof(cTemp));
	memset(cTems,0x00,sizeof(cTems));

	strcpy(cTemp,"zt05.log");
	strcpy(cTems,"zt06.log");
	pcTemp = cTems;

	// 로그파일 오픈
	cLog1.Open("zt01.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);				// 일별주기로 파일을 생성한다.
	cLog2.Open("zt02.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_MONTH);			// 월별주기로 파일을 생성한다.
	cLog3.Open("zlog/zt03.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);		// 디렉토리가 없으면 자동생성한다.
	cLog4.SetDirCreate(false);
	cLog4.Open("../zlog/zt04.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);		// 디렉토리가 없어도 자동생성하지 않는다.
	cLog5.Open(cTemp, WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);
	cLog6.Open(pcTemp, WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);

	// 로그파일 쓰기 
	for (int i=0; i<3; i++)
	{
		cLog1.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog2.Write(WCLog::E_LEVEL_ALL, str(boost::format("1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg") % i));
		cLog3.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);	// 파일이 생성되지 않아서 표준출력으로 표시된다.
		cLog4.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog5.Write(WCLog::E_LEVEL_ALL, str(boost::format("1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg") % i));
		cLog6.WriteConsole(WCLog::E_LEVEL_ALL, str(boost::format("1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg") % i));
	}

	// 로그파일 닫기 
	cLog1.Close();
	cLog2.Close();
	cLog3.Close();
	cLog4.Close();
	cLog5.Close();
	cLog6.Close();
}


/****************************************************************************************************************************************************************************************************
* 3. 기본 사용법3
*
* 로그 모드에 따라 출력 여부를 확인한다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic3()
{
	WCLog cLog1, cLog11, cLog12, cLog2, cLog3, cLog4, cLog5, cLog6, cLog7;

	// 로그파일 오픈
	cLog1.Open("zt01.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);
	cLog11.Open("zlog/zt01.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);
	cLog12.Open("../zlog/zt01.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);
	cLog2.Open("zt02dd.log", WCLog::E_LEVEL_DEBUG, WCLog::E_CYCLE_DAY);
	cLog3.Open("zt03wm.log", WCLog::E_LEVEL_WARN, WCLog::E_CYCLE_MONTH);
	cLog4.Open("zt04wd.log", WCLog::E_LEVEL_ERROR, WCLog::E_CYCLE_DAY);
	cLog5.Open("zt05ad.log", WCLog::E_LEVEL_NORMAL, WCLog::E_CYCLE_MONTH);	
	cLog6.Open("zt06ad.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_MONTH);	
	cLog7.Open("zt07wed.log", WCLog::E_LEVEL_WARN| WCLog::E_LEVEL_ERROR, WCLog::E_CYCLE_DAY);	

	// 로그파일 쓰기 - 1로 시작하는 문자열은 쓰여져야하고 0으로 시작하는 문자열은 쓰여지면 안 됨 
	for (int i=0; i<3; i++)
	{
		cLog1.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog11.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog12.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);

		cLog2.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog2.WriteFormat(WCLog::E_LEVEL_DEBUG, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog2.WriteFormat(WCLog::E_LEVEL_WARN, "0 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);

		cLog3.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog3.WriteFormat(WCLog::E_LEVEL_WARN, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog3.WriteFormat(WCLog::E_LEVEL_ERROR, "0 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);

		cLog4.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog4.WriteFormat(WCLog::E_LEVEL_ERROR, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog4.WriteFormat(WCLog::E_LEVEL_NORMAL, "0 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);

		cLog5.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog5.WriteFormat(WCLog::E_LEVEL_NORMAL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog5.WriteFormat(WCLog::E_LEVEL_WARN, "0 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);

		cLog6.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog6.WriteFormat(WCLog::E_LEVEL_NORMAL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog6.WriteFormat(WCLog::E_LEVEL_WARN, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);

		cLog7.WriteFormat(WCLog::E_LEVEL_ALL, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog7.WriteFormat(WCLog::E_LEVEL_WARN, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog7.WriteFormat(WCLog::E_LEVEL_ERROR, "1 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);
		cLog7.WriteFormat(WCLog::E_LEVEL_NORMAL, "0 %02d-abcdefg 11abcdefg 21abcdefg 31abcdefg 41abcdefg 51abcdefg", i);

		cLog1.Close();								// 닫으면 이후는 표준출력으로 보여준다.
	}

	// 로그파일 닫기 
	cLog1.Close();
	cLog11.Close();
	cLog12.Close();
	cLog2.Close();
	cLog3.Close();
	cLog4.Close();
	cLog5.Close();
	cLog6.Close();
	cLog7.Close();
}


/****************************************************************************************************************************************************************************************************
* 90. 스트레스 테스트 - 각가의 파일에 쓰기
* 91. 스트레스 테스트 - 하나의 파일에 쓰기
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
*****************************************************************************************************************************************************************************************************/
static int gnLine = 0;		// 라인수
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
		gnLine = atoi(argv[4]);		// 라인수	
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
				//fprintf(stderr,"[%s:%d] (Pid:%d)(gnPno:%d)(i:%d)\n",__FUNCTION__,__LINE__,getpid(),gnPno,i);
				pthread_detach(t_pthread[i]);
			}
			usleep(1000);
		}

		while(g_bLoop) pause();

		if (t_pthread) delete[] t_pthread;
		if (t_pthreadattr) delete[] t_pthreadattr;

		fprintf(stderr,"[%s] Child End (Pid:%d)(gnPno:%d)\n", __FUNCTION__, getpid(),gnPno);
	}
	else 
	{	// 부모 : 자식 프로세스 대기
		while (WC_Wait(NULL) > 0);
		fprintf(stderr,"[%s] Parend End 1 (Pid:%d)\n", __FUNCTION__, getpid());
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
	fprintf(stderr,"[%s] Start (Pid:%d)(Pno:%d)\n", __FUNCTION__, getpid(), nPno);
	// 시작시간 기록
	gettimeofday(&tv,NULL);
	gLogT.WriteFormat(WCLog::E_LEVEL_ALL, "[%s] Time Start (Pid:%d)(Pno:%d) (Time:%ld-%ld)", __FUNCTION__, getpid(), nPno, tv.tv_sec, tv.tv_usec);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 시작 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	char cFile[50], cTemp[1000];
	int nLen;

	if (nType == 0)
	{	// 각각의 파일에 쓰기
		WCLog tLog;
		sprintf(cFile, "log/log_%010d.txt", nPno);
		tLog.Open(cFile, WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);
		for(int i=0; i<gnLine; i++) {
			nLen = sprintf(cTemp, "[%s][%010d] 1ng2ng3ng4ng5ng6ng7ng8ng9ngAngBngCngDngEngFngGngHngIngJngKngLngMngNngOngPngQngRngSngTngUngVngWngXngYngZnga1234512345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890-%03d\n", __FUNCTION__, nPno, i);
			cTemp[nLen] = 0x00;
			tLog.WriteFormat(WCLog::E_LEVEL_ALL, cTemp);
			usleep(1);
		}
		tLog.Close();
	}
	else
	{	
		// 하나의 파일에 쓰기 (모든 쓰레드가 하나의 파일에 쓴다.)
		for(int i=0; i<gnLine; i++)
		{
			gLog1.WriteFormat(WCLog::E_LEVEL_ALL, "[%s][%010d] 1ng2ng3ng4ng5ng6ng7ng8ng9ngAngBngCngDngEngFngGngHngIngJngKngLngMngNngOngPngQngRngSngTngUngVngWngXngYngZnga123451234567890123456789012345678901234567890123456789012345678901234567890-%03d\n",__FUNCTION__,nPno,i);
			//WC_COUTLN(WCLog::E_LEVEL_ALL,"[%s][%010d] 1ng2ng3ng4ng5ng6ng7ng8ng9ngAngBngCngDngEngFngGngHngIngJngKngLngMngNngOngPngQngRngSngTngUngVngWngXngYngZnga123451234567890123456789012345678901234567890123456789012345678901234567890-%03d\n",__FUNCTION__,nPno,i);
			usleep(1);
		}
		WCLOG_WRITEFORMAT(WCLog::E_LEVEL_ALL,"[%s] End 1 (%d)\n",__FUNCTION__,nPno);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 종료
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// 종료시간 기록
	gettimeofday(&tv, NULL);
	gLogT.WriteFormat(WCLog::E_LEVEL_ALL, "[%s] Time End (Pid:%d)(Pno:%d) (Time:%ld-%ld)", __FUNCTION__, getpid(), nPno, tv.tv_sec, tv.tv_usec);
	// 쓰레드 종료
	fprintf(stderr, "[%s] End (Pid:%d)(Pno:%d)\n", __FUNCTION__, getpid(), nPno);

	return NULL;
}


// 단위테스트 
TEST(UnitTest_WCLog, UTLogBasic1) 
{
	int nRet1;
	WCLog cLog1;

	// 로그 오픈 
	nRet1 = cLog1.Open("zt01.log", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_ONE);
	EXPECT_EQ(nRet1, WC_OK);
	EXPECT_EQ(cLog1.IsOpen(), true);

	// 로그 쓰기 
	nRet1 = cLog1.WriteFormat(WCLog::E_LEVEL_ALL, "ABCDE");
	EXPECT_EQ(nRet1, 6);

	// 로그 닫기
	cLog1.Close();
	EXPECT_EQ(cLog1.IsOpen(), false);
}

TEST(UnitTest_WCLog, UTLogBasic2) 
{

}
