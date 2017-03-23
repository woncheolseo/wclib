/*
<WCObject 예제>

0. 단위 테스트 
1. 기본 사용법1
9. 스트레스 테스트

Usage : sam_object 모드번호 프로세스수 쓰레드수
*/

#include "wc_lib.h"

// 프로세스 대기 플래그
static bool g_bLoop = true;

void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1();
void Sam_Stress(int argc, char *argv[], int nType);

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

// 메인 함수
int main(int argc, char *argv[])
{
	int nMode = -1;
	char cUsage[] = "@ usage\n\tsam_object ModeNo ProcessCount ThreadCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage, 9-StressTest\
			\n@ Example\n\tsam_object 0\n\tsam_object 1\n\tsam_object 9 3 10\n";

	SamInit();

	if (argc > 1) {
		nMode = atoi(argv[1]);
		if (nMode == 0) Sam_Unittest(argc, argv);		// 단위테스트 - sam_object 0
		else if (nMode == 1) Sam_Basic1();				// 기본사용법1 - sam_object 1
		else if (nMode == 9) Sam_Stress(argc, argv, 1);	// 스트레스테스트 - sam_object 9 3 10
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

	testing::InitGoogleTest(&argc, argv);

	nRet = RUN_ALL_TESTS();

	if (nRet == -1) fprintf(stderr, "[Er] RUN_ALL_TESTS\n");
}


/****************************************************************************************************************************************************************************************************
* 1. 기본 사용법1
*
* WCObject 객체를 생성하고 복사,대입하고 정보를 출력하는 예제이다.
* WCObject 객체는 최상위 부모 클래스로 직접 선언해서 사용할 일은 거의 없다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	for (int i=0; i<1; i++)	// 여러번 돌려서 키값 중복을 확인하려고
	{

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 1) 기본 사용
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		WCObject wOB1, wOB2("Zinga");
		WCObject *pOB3=NULL, *pOB4=NULL, *pOTemp=NULL;

		// 객체명 설정
		pOB3 = new WCObject("NiceGuyNiceGuyNiceGuyNiceGuyNiceGuyNiceGuyNiceGuyNiceGuyNiceGuy");
		pOB4 = new WCObject();
		if (!pOB3 || !pOB4) {
			fprintf(stderr,"[Er] new\n");
			continue;
		}

		fprintf(stderr, "[OB1] This / KeyPri / KeyGrp / Name (i:%02d)\n", i);
		for (int k=0; k<4; k++)
		{
			if (k == 0) pOTemp = &wOB1;
			else if (k == 1) pOTemp = &wOB2;
			else if (k == 2) pOTemp = pOB3;
			else pOTemp = pOB4;

			if (pOTemp) fprintf(stderr, "(%d : %p / %llu / %llu / %s)\n", k, pOTemp, pOTemp->GetKeyPri(), pOTemp->GetKeyGrp(), pOTemp->GetName());
		}
		fprintf(stderr, "\n");		

		// 그룹 설정
		// wOB1.SetGroup(&wOB1);
		// wOB2.SetGroup(pOB4);
		// pOB3->SetGroup(&wOB2);
		// pOB4->SetGroup(pOB3);

		// Live,Group,객체키,객체명 출력
		// fprintf(stderr,"\n[OB1] This / Key / Name / Type / Group / Live (i:%02d) \n(1: %p / %llu / %s / %d / %p / %d)\n(2: %p / %llu / %s / %d / %p / %d)\n(3: %p / %llu / %s / %d / %p / %d)\n(4: %p / %llu / %s / %d / %p / %d)\n\n",
		//	i,
		//	&wOB1,wOB1.GetKey(),wOB1.GetName(),wOB1.GetType(),wOB1.GetGroup(),wOB1.IsLive(),			
		//	&wOB2,wOB2.GetKey(),wOB2.GetName(),wOB2.GetType(),wOB2.GetGroup(),wOB2.IsLive(),			
		//	pOB3,pOB3->GetKey(),pOB3->GetName(),pOB3->GetType(),pOB3->GetGroup(),pOB3->IsLive(),			
		//	pOB4,pOB4->GetKey(),pOB4->GetName(),pOB4->GetType(),pOB3->GetGroup(),pOB4->IsLive()
		//);
		/*fprintf(stderr,"\n[OB1] This / Key / Name / Type / Live (i:%02d) \n(1: %p / %u / %s / %d / %d)\n(2: %p / %u / %s / %d / %d)\n(3: %p / %u / %s / %d / %d)\n(4: %p / %u / %s / %d / %d)\n\n",
			i,
			&wOB1,wOB1.GetKey(),wOB1.GetName(),wOB1.GetType(),wOB1.IsLive(),
			&wOB2,wOB2.GetKey(),wOB2.GetName(),wOB2.GetType(),wOB2.IsLive(),
			pOB3,pOB3->GetKey(),pOB3->GetName(),pOB3->GetType(),pOB3->IsLive(),
			pOB4,pOB4->GetKey(),pOB4->GetName(),pOB4->GetType(),pOB4->IsLive()
		);*/
		/*fprintf(stderr,"\n[OB1] This / KeyPri / KeyGrp / Name (i:%02d) \n(1: %p / %llu / %llu / %s)\n(2: %p / %llu / %llu / %s)\n(3: %p / %llu / %llu / %s)\n(4: %p / %llu / %llu / %s)\n\n",
				i,
				&wOB1, wOB1.GetKeyPri(), wOB1.GetKeyGrp(), wOB1.GetName(), 
				&wOB2, wOB2.GetKeyPri(), wOB2.GetKeyGrp(), wOB2.GetName(),
				pOB3, pOB3->GetKeyPri(), pOB3->GetKeyGrp(), pOB3->GetName(),
				pOB4, pOB4->GetKeyPri(), pOB3->GetKeyGrp(), pOB4->GetName()
		);*/


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 2) 복사생성자, 대입연산자
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		WCObject tOB1=*pOB3;	// 복사생성자
		WCObject tOB2;
		WCObject *ptOB1 = new WCObject(*pOB3); // 복사생성자 
		WCObject *ptOB2 = new WCObject();

		tOB2 = *pOB3;	// 대입		
		*ptOB2 = *pOB3; // 대입

		fprintf(stderr, "[OB2] This / KeyPri / KeyGrp / Name (i:%02d)\n", i);
		for (int k=0; k<4; k++)
		{
			if (k == 0) pOTemp = &tOB1;
			else if (k == 1) pOTemp = &tOB2;
			else if (k == 2) pOTemp = ptOB1;
			else if (k == 3) pOTemp = ptOB2;

			if (pOTemp) fprintf(stderr, "(%d : %p / %llu / %llu / %s)\n", k, pOTemp, pOTemp->GetKeyPri(), pOTemp->GetKeyGrp(), pOTemp->GetName());
		}
		fprintf(stderr, "\n");

		// 소멸후 IsLive값 확인
		if (pOB3) {
			delete pOB3;
			//pOB3 = NULL;
		}
		if (pOB4) {
			delete pOB4;
			//pOB4 = NULL;
		}
		if (ptOB1) {
			delete ptOB1;
			//pOB4 = NULL;
		}
		if (ptOB2) {
			delete ptOB2;
			//pOB4 = NULL;
		}
		// valgrind로 체크시 아래 2문장은 주석 처리 
		//fprintf(stderr,"[OB3] (i:%02d)(This:%p)(Key:%u)(Name:%s)(Live:%d)\n", i, pOB3,pOB3->GetKey(),pOB3->GetName(),pOB3->IsLive());
		//fprintf(stderr,"[OB3] (i:%02d)(This:%p)(Key:%u)(Name:%s)(Live:%d)\n", i, pOB4,pOB4->GetKey(),pOB4->GetName(),pOB4->IsLive());
	}
}

/****************************************************************************************************************************************************************************************************
* 9. 스트레스 테스트
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
*****************************************************************************************************************************************************************************************************/
//static int gnLine = 0;	// 라인수
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
		while (WC_Wait_r(NULL) > 0);
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
	WCLOG_WRITEFORMAT(WCLog::E_LEVEL_ALL, "[%s] Start (Pid:%d)(Pno:%d)", __FUNCTION__, getpid(), nPno);
	// 시작시간 기록
	gettimeofday(&tv, NULL);
	gLogT.WriteFormat(WCLog::E_LEVEL_ALL, "[%s] Time Start (Pid:%d)(Pno:%d) (Time:%ld-%ld)", __FUNCTION__, getpid(), nPno, tv.tv_sec, tv.tv_usec);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 시작 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Sam_Basic1();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 종료
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// 종료시간 기록
	gettimeofday(&tv, NULL);
	gLogT.WriteFormat(WCLog::E_LEVEL_ALL, "[%s] Time End (Pid:%d)(Pno:%d) (Time:%ld-%ld)", __FUNCTION__, getpid(), nPno, tv.tv_sec, tv.tv_usec);
	// 쓰레드 종료
	WCLOG_WRITEFORMAT(WCLog::E_LEVEL_ALL, "[%s] End (Pid:%d)(Pno:%d)", __FUNCTION__, getpid(), nPno);

	return NULL;
}

// 단위테스트 : 객체 생성 (스택, 힙), 객체 복사, 객체 대입, 객체 소멸
TEST(UnitTest_WCObject, UTObjectBasic1) 
{
	string strName = "Jane";
	WCObject wcObj1, wcObj2("NiceRobot"), wcObj3(strName);	// 생성 - 스택
	WCObject *pwcObj1, *pwcObj2, *pwcObj3;
	WCObject wcObjCp3, wcObjCp3p, *pwcObjCp3, *pwcObjCp3p;

	// 생성 - 힙
	pwcObj1 = new WCObject();
	pwcObj2 = new WCObject("Good");
	pwcObj3 = new WCObject(strName);

	// 복사 
	WCObject wcObjCp2 = wcObj2;
	WCObject wcObjCp2p = *pwcObj2;
	WCObject *pwcObjCp2 = new WCObject(wcObj2);
	WCObject *pwcObjCp2p = new WCObject(*pwcObj2);

	// 대입
	wcObjCp3 = wcObj3;
	wcObjCp3p = *pwcObj3;
	pwcObjCp3 = new WCObject();
	*pwcObjCp3 = wcObj3;
	pwcObjCp3p = new WCObject();
	*pwcObjCp3p = *pwcObj3;


	// 단위테스트 - 객체가 스택에 제대로 생성되었나 
	EXPECT_GT(wcObj1.GetKeyPri(), 0);
	EXPECT_GT(wcObj1.GetKeyGrp(), 0);
	EXPECT_STRNE(wcObj1.GetName(), "");

	EXPECT_GT(wcObj2.GetKeyPri(), 0);
	EXPECT_GT(wcObj2.GetKeyGrp(), 0);
	EXPECT_STREQ(wcObj2.GetName(), "NiceRobot");	

	EXPECT_GT(wcObj3.GetKeyPri(), 0);
	EXPECT_GT(wcObj3.GetKeyGrp(), 0);
	EXPECT_STREQ(wcObj3.GetName(), "Jane");		

	EXPECT_TRUE(pwcObj1 != NULL);
	if (pwcObj1) {
		EXPECT_GT(pwcObj1->GetKeyPri(), 0);
		EXPECT_GT(pwcObj1->GetKeyGrp(), 0);
		EXPECT_STRNE(pwcObj1->GetName(), "");
	}

	// 단위테스트 - 객체가 힙에 제대로 생성되었나 
	EXPECT_TRUE(pwcObj2 != NULL);
	if (pwcObj2) {
		EXPECT_GT(pwcObj2->GetKeyPri(), 0);
		EXPECT_GT(pwcObj2->GetKeyGrp(), 0);
		EXPECT_STREQ(pwcObj2->GetName(), "Good");
	}

	EXPECT_TRUE(pwcObj3 != NULL);
	if (pwcObj3) {
		EXPECT_GT(pwcObj3->GetKeyPri(), 0);
		EXPECT_GT(pwcObj3->GetKeyGrp(), 0);
		EXPECT_STREQ(pwcObj3->GetName(), "Jane");
	}

	// 단위테스트 - 복사생성자는 제대로 동작하나 
	EXPECT_NE(wcObj2.GetKeyPri(), wcObjCp2.GetKeyPri());			// CObject wcObjCp2 = wcObj2;
	EXPECT_EQ(wcObj2.GetKeyGrp(), wcObjCp2.GetKeyGrp());
	EXPECT_STREQ(wcObj2.GetName(), wcObjCp2.GetName());

	if (pwcObj2) {													// WCObject wcObjCp2p = *pwcObj2;
		EXPECT_NE(pwcObj2->GetKeyPri(), wcObjCp2p.GetKeyPri());
		EXPECT_EQ(pwcObj2->GetKeyGrp(), wcObjCp2p.GetKeyGrp());
		EXPECT_STREQ(pwcObj2->GetName(), wcObjCp2p.GetName());
	}

	EXPECT_TRUE(pwcObjCp2 != NULL);									// WCObject *pwcObjCp2 = new WCObject(wcObj2);
	if (pwcObjCp2) {
		EXPECT_NE(wcObj2.GetKeyPri(), pwcObjCp2->GetKeyPri());
		EXPECT_EQ(wcObj2.GetKeyGrp(), pwcObjCp2->GetKeyGrp());
		EXPECT_STREQ(wcObj2.GetName(), pwcObjCp2->GetName());
	}

	EXPECT_TRUE(pwcObjCp2p != NULL);								// WCObject *pwcObjCp2p = new WCObject(*pwcObj2);
	if (pwcObjCp2p && pwcObj2) {
		EXPECT_NE(pwcObj2->GetKeyPri(), pwcObjCp2p->GetKeyPri());
		EXPECT_EQ(pwcObj2->GetKeyGrp(), pwcObjCp2p->GetKeyGrp());
		EXPECT_STREQ(pwcObj2->GetName(), pwcObjCp2p->GetName());
	}

	// 단위테스트 - 대입연산자는 제대로 동작하나 
	EXPECT_NE(wcObj3.GetKeyPri(), wcObjCp3.GetKeyPri());			// wcObjCp3 = wcObj3;
	EXPECT_EQ(wcObj3.GetKeyGrp(), wcObjCp3.GetKeyGrp());
	EXPECT_STREQ(wcObj3.GetName(), wcObjCp3.GetName());

	if (pwcObj3) {													// wcObjCp3p = *pwcObj3;
		EXPECT_NE(pwcObj3->GetKeyPri(), wcObjCp3p.GetKeyPri());
		EXPECT_EQ(pwcObj3->GetKeyGrp(), wcObjCp3p.GetKeyGrp());
		EXPECT_STREQ(pwcObj3->GetName(), wcObjCp3p.GetName());
	}

	EXPECT_TRUE(pwcObjCp3 != NULL);									// *pwcObjCp3 = wcObj3;
	if (pwcObjCp3) {
		EXPECT_NE(wcObj3.GetKeyPri(), pwcObjCp3->GetKeyPri());
		EXPECT_EQ(wcObj3.GetKeyGrp(), pwcObjCp3->GetKeyGrp());
		EXPECT_STREQ(wcObj3.GetName(), pwcObjCp3->GetName());
	}

	EXPECT_TRUE(pwcObjCp3p != NULL);								// *pwcObjCp3p = *pwcObj3;
	if (pwcObjCp3p && pwcObj3) {
		EXPECT_NE(pwcObj3->GetKeyPri(), pwcObjCp3p->GetKeyPri());
		EXPECT_EQ(pwcObj3->GetKeyGrp(), pwcObjCp3p->GetKeyGrp());
		EXPECT_STREQ(pwcObj3->GetName(), pwcObjCp3p->GetName());
	}	

	// 소멸
	WC_DELETE(pwcObj1);
	WC_DELETE(pwcObj2);
	WC_DELETE(pwcObj3);
	WC_DELETE(pwcObjCp2);
	WC_DELETE(pwcObjCp2p);
	WC_DELETE(pwcObjCp3);
	WC_DELETE(pwcObjCp3p);

	// 단위테스트 - 소멸은 제대로 되었나 
	EXPECT_TRUE(pwcObj1 == NULL);
	EXPECT_TRUE(pwcObj2 == NULL);
	EXPECT_TRUE(pwcObj3 == NULL);
	EXPECT_TRUE(pwcObjCp2 == NULL);
	EXPECT_TRUE(pwcObjCp2p == NULL);
	EXPECT_TRUE(pwcObjCp3 == NULL);
	EXPECT_TRUE(pwcObjCp3p == NULL);	
}
