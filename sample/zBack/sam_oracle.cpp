/*
<WCOracle 예제>

0. 단위 테스트 
1. 기본 사용법1
2. 기본 사용법2
3. 스트레스 테스트

Usage : sam_oracle 모드번호 프로세스수 쓰레드수 라인수
*/

#include <wc_lib.h>

void SamInit();
void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1();
void Sam_Basic2();
//void Sam_Stress(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int nMode = -1;

	SamInit();

	if (argc > 1) {
		nMode = atoi(argv[1]);
	}

	if (nMode == 0) Sam_Unittest(argc,argv);		// 단위테스트, sam_oracle 0 (WCOracle 단위테스트만 실행시킬 경우, "--gtest_filter=UnitTest_WCOracle.*" 인자 추가)
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_oracle 1
	else if (nMode == 2) Sam_Basic2();				// 기본사용법2, sam_oracle 2
	//else if (nMode == 3) Sam_Stress(argc,argv);		// 스트레스테스트, sam_oracle 3 3 100 1000

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
}


/****************************************************************************************************************************************************************************************************
* 0. 단위 테스트
*****************************************************************************************************************************************************************************************************/
void Sam_Unittest(int argc, char *argv[])
{
	int nRet;

	testing::InitGoogleTest(&argc,argv);

	nRet = RUN_ALL_TESTS(); // 모든 테스트를 수행한다.
}


/****************************************************************************************************************************************************************************************************
* 1. 기본 사용법1
*
* OCCI 함수 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	Environment *pEnv = 0;
	Connection *pConn = 0;
	Statement *pStmt1 =0, *pStmt2 = 0;
	ResultSet *pResultSet = 0;

	try
	{
		// OCCI 환경객체 만들기
		pEnv = Environment::createEnvironment(Environment::DEFAULT);
		//pEnv = Environment::createEnvironment(Environment::THREADED_MUTEXED);

		// OCCI 연결객체 만들기 - 연결 하기 (ID,PW,TIG)
		pConn = pEnv->createConnection("neo21","a","TIG_NEO21");

		// OCCI 질의객체 생성
		pStmt1 = pConn->createStatement();

		// OCCI 질의 - insert
		pResultSet = pStmt1->executeQuery("insert into testinno (id,name,nick,age,score,address,code,regd,regt,test,extra) values (TESTSEQ.NEXTVAL,'kimtaehee','god?',31,1451278,'seoul',123456789,SYSDATE,SYSDATE,'00006','long statment')");
		pStmt1->closeResultSet(pResultSet);

		// OCCI 질의 - select
		pResultSet = pStmt1->executeQuery("select * from TESTINNO where ROWNUM <= 10");
	    while (pResultSet->next())
	    {
	    	int n1;
			string s1,s2;
			n1 = pResultSet->getInt(1);
			s1 = pResultSet->getString(2);
			s2 = pResultSet->getString(3);
			cout << "[Select 01] " << n1 << ", " << s1 << ", " << s2 << endl;
	    }
		pStmt1->closeResultSet(pResultSet); 

		// OCCI 질의 - update
		pResultSet = pStmt1->executeQuery("update testinno set nick='BATMAN' where id=1");
		pStmt1->closeResultSet(pResultSet);

		// OCCI 질의 - select 바인딩 (stmt2는 질의객체를 생성안했으로 생성해야 한다)
		pStmt2 = pConn->createStatement("select * from TESTINNO where AGE=:1 and TEST=:2");
		pStmt2->setInt(1,20);
		pStmt2->setString(1,"00005");
		pResultSet = pStmt2->executeQuery();
	    while (pResultSet->next())
	    {
	    	int n1;
			string s1,s2;
			n1 = pResultSet->getInt(1);
			s1 = pResultSet->getString(2);
			s2 = pResultSet->getString(3);
			cout << "[Select 02] " << n1 << ", " << s1 << ", " << s2 << endl;
	    }
		pStmt2->closeResultSet(pResultSet);

		// OCCI 질의객체 반환
 		pConn->terminateStatement(pStmt1);
 		pConn->terminateStatement(pStmt2);

		// OCCI 연결객체 반환 - 연결 끊기
		pEnv->terminateConnection(pConn);

		// OCCI 환경객체 반환
		Environment::terminateEnvironment(pEnv);
	}
	catch (oracle::occi::SQLException ex)
	{
        WcLogPrintf(WCLog::E_MODE_ALL,"SQLException (Er:%d,%s)\n",ex.getErrorCode(),ex.what());

        if (pResultSet)
            pStmt1->closeResultSet(pResultSet);

        if (pStmt1)
            pConn->terminateStatement(pStmt1);

        if (pConn)
            pEnv->terminateConnection(pConn);

        if (pEnv)
            Environment::terminateEnvironment(pEnv);
	}
}

/****************************************************************************************************************************************************************************************************
* 2. 기본 사용법2
*
* WCOracle 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic2()
{
	int nRet, n1;
	string s1, s2, s3;
	WCOracle::T_RESULTSET tOraRes1;
	WCOracle::T_RESULTSET tOraRes2;
	WCOracle::T_RESULTSET tOraRes3;
	WCOracle wcOracle,wcOracleS;

	memset(&tOraRes1,0x00,sizeof(tOraRes1));
	memset(&tOraRes2,0x00,sizeof(tOraRes2));
	memset(&tOraRes3,0x00,sizeof(tOraRes3));

	// Oracle 열기
	if (wcOracle.VOpen("TIG_NEO21","neo21","a") == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"Open\n");
		return;
	}
	else WcLogPrintf(WCLog::E_MODE_ALL,"Open (IP:TIG_NEO21)(ID:neo21)(PW:a)\n");

	// 복사생성,대입연산
	WCOracle wcOracleR = wcOracle;
	wcOracleS = wcOracle;

	// Oracle 질의 - insert
	nRet = wcOracle.Query(&tOraRes1, "insert into testinno (id, name, nick, age, score, address, code, regd, regt, test, extra) values (TESTSEQ.NEXTVAL,'kimtaehee','god?',31,1451278,'seoul',123456789,SYSDATE,SYSDATE,'00006','long statment')");
	wcOracle.FreeResult(&tOraRes1);
	if (nRet != -1)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 1 : Insert Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 1 : Insert Er\n");
	
	// Oracle 질의 - insert-fail
	nRet = wcOracle.Query(&tOraRes1, "insert into testinno (PRC_NM) values ('SWC-TEST5')");
	wcOracle.FreeResult(&tOraRes1);
	if (nRet != -1)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 2 : Insert Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 2 : Insert Er\n");

	// Oracle 질의 - update
	nRet = wcOracle.Query(&tOraRes1, "update testinno set name='%s' where nick='%s'", "HanYelsul","god?");
	wcOracle.FreeResult(&tOraRes1);
	if (nRet != -1)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 3 : Update Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 3 : Update Er\n");

	// Oracle 질의 - update-FAIL
	nRet = wcOracle.Query(&tOraRes1, "update testinno set PRC_NM='%s' where TBL_NM='%s'", "Min-001222","TABLE-TEST222");
	wcOracle.FreeResult(&tOraRes1);
	if (nRet != -1)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 4 : Update Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 4 : Update Er\n");

	// Oracle 질의 - delete
	nRet = wcOracle.Query(&tOraRes1, "delete testinno where nick='%s'", "nice");
	wcOracle.FreeResult(&tOraRes1);
	if (nRet != -1)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 5 : Delete Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 5 : Delete Er\n");

	// Oracle 질의 - delete-FaIL
	nRet = wcOracle.Query(&tOraRes1, "delete testinno where nick='%s'", "nice");
	wcOracle.FreeResult(&tOraRes1);
	if (nRet != -1)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 6 : Delete Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 6 : Delete Er\n");
			
	// Oracle 질의 - select
	nRet = wcOracle.Query(&tOraRes1, "select NAME, NICK, AGE, REGD from TESTINNO where ROWNUM <= 2 order by AGE desc");
	if (nRet != -1)
	{
	    while(tOraRes1.pResultSet->next())
	    {
			s1 = tOraRes1.pResultSet->getString(1);
			s2 = tOraRes1.pResultSet->getString(2);
			n1 = tOraRes1.pResultSet->getInt(3);
			s3 = tOraRes1.pResultSet->getString(4);
			cout << "[Res01] " << s1 << ", " << s2 << ", " << n1 << ", " << s3 << endl;

			nRet = wcOracle.Query(&tOraRes2, "select ADDRESS, TEST, SCORE, REGT from TESTINNO where ROWNUM <= 20 order by REGT desc");
			if (nRet != -1)
			{
				while(tOraRes2.pResultSet->next())
				{
					s1 = tOraRes2.pResultSet->getString(1);
					s2 = tOraRes2.pResultSet->getString(2);
					n1 = tOraRes2.pResultSet->getInt(3);
					s3 = tOraRes2.pResultSet->getString(4);
					cout << "[Res02] " << s1 << ", " << s2 << ", " << n1 << ", " << s3 << endl;

					// Oracle 질의 - Insert
					nRet = wcOracleR.Query(&tOraRes3, "insert into SP_MMS_FAIL_LOG (PRC_NM, TBL_NM, TBL_CNT, UPT_DATE) values ('SWC-TEST2','서원철',2,SYSDATE)");
					wcOracleR.FreeResult(&tOraRes3);

					// Oracle 질의 - update
					nRet = wcOracleS.Query(&tOraRes3, "update SP_MMS_FAIL_LOG set PRC_NM='%s' where TBL_NM='%s'", "Min-002","TABLE-TEST2");
					wcOracleS.FreeResult(&tOraRes3);

					// delete
					//nRet = wcOracle.Query(&tOraRes3, "delete SP_MMS_FAIL_LOG where PRC_NM='%s'", "SWC-TEST1");
					//wcOracle.FreeResult(&tOraRes3);
				}
				wcOracle.FreeResult(&tOraRes2);
			}
		}
		wcOracle.FreeResult(&tOraRes1);
	}

	// Oracle 질의 - commit (Oracle은 기본값이 autocommit 아니기 때문에 commit 해줘야 한다.)
	if (nRet != -1) {
		nRet = wcOracle.Query(&tOraRes1,"commit");
	}
	else {
		nRet = wcOracle.Query(&tOraRes1,"rollback");
	}
	wcOracle.FreeResult(&tOraRes1);

	// Oracle 닫기
	wcOracle.VClose();
	wcOracleR.VClose();
	wcOracleS.VClose();
}


/****************************************************************************************************************************************************************************************************
* 3. 스트레스 테스트
*
* 멀티프로세스, 멀티쓰레드 스트레드 테스트
*****************************************************************************************************************************************************************************************************/
static int gnLine=0;	// 라인수
static int gnPno=0;		// 프로세스번호(사용자정의)
pthread_mutex_t g_Mutex;
WCLog gLogT;

void * ThreadFunc(void *pArg);

void Sam_Stress(int argc, char *argv[])
{
	pthread_t *t_pthread;
	pthread_attr_t *t_pthreadattr;	
	int i=0,nFork=1,nThread=1,nRet=0,nPidChild=0;

	// 변수 초기
	pthread_mutex_init(&g_Mutex,NULL);
	gLogT.Open("../log/","log_t1.txt",WCLog::E_MODE_ALL,WCLog::E_CYCLE_DAY);

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
		// 자식 : 쓰레드 생성
		t_pthread = new pthread_t[nThread];
		t_pthreadattr = new pthread_attr_t[nThread];

		for (i=0; i<nThread; i++)
		{
			pthread_attr_init(&t_pthreadattr[i]);
			if ((nRet = pthread_create(&t_pthread[i], NULL, ThreadFunc, NULL)) != 0) {
				fprintf(stderr,"[%s:%d] pthread_create (Pid:%d)(gnPno:%d)(i:%d)(Er:%d,%s)\n",__FUNCTION__,__LINE__,getpid(),gnPno,i,errno,(char *)strerror(errno));
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

		fprintf(stderr,"[%s] Child End (Pid:%d)(gnPno:%d)\n",__FUNCTION__,getpid(),gnPno);
	}
	else 
	{	// 부모 : 자식 프로세스 대기
		while (wait_r(NULL) > 0);
		fprintf(stderr,"[%s] Parend End 1 (Pid:%d)\n",__FUNCTION__,getpid());
	}
}

void * ThreadFunc(void *pArg)
{
	int nPno=0;

	pthread_mutex_lock(&g_Mutex);
	nPno = gnPno++;
	pthread_mutex_unlock(&g_Mutex); 
	struct timeval tv;

	// 쓰레드 시작
	fprintf(stderr,"[%s] Start (Pid:%d)(Pno:%d)\n",__FUNCTION__,getpid(),nPno);
	// 시작시간 기록
	gettimeofday(&tv,NULL);
	gLogT.Printf(WCLog::E_MODE_ALL,"[%s] Time Start (Pid:%d)(Pno:%d) (Time:%ld-%ld)\n",__FUNCTION__,getpid(),nPno,tv.tv_sec,tv.tv_usec);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 시작 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int nRet=-1, n1;
	string s1, s2, s3;
	WCOracle::T_RESULTSET tOraRes1;
	WCOracle wcOracle;
	char cNo[15];

	// Oracle 열기
	if (wcOracle.VOpen("TIG_NEO21","neo21","a") == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"Open\n");
		return NULL;
	}
	else WcLogPrintf(WCLog::E_MODE_ALL,"Open (IP:TIG_NEO21)(ID:neo21)(PW:a)\n");
			
	for(int i=0; i<gnLine; i++) 
	{
		sprintf(cNo,"%05d%05d",nPno,i);
		cNo[10]=0x00;

		// Oracle 질의 - insert 
		nRet = wcOracle.Query(&tOraRes1, "insert into testinno (id, name, nick, age, score, address, code, regd, regt, test, extra) values (TESTSEQ.NEXTVAL,'kimtaehee','god?',31,1451278,'seoul',123456789,SYSDATE,SYSDATE,'%s','long statment')",cNo);
		wcOracle.FreeResult(&tOraRes1);
		if (nRet == -1)
			WcLogPrintf(WCLog::E_MODE_ERROR,"Query : Insert Er\n");
		//else
			//WcLogPrintf(WCLog::E_MODE_ALL,"Query : Insert Ok\n");

		// Oracle 질의 - update
		nRet = wcOracle.Query(&tOraRes1, "update testinno set name='송혜교' where test='%s'",cNo);
		wcOracle.FreeResult(&tOraRes1);
		if (nRet == -1)
			WcLogPrintf(WCLog::E_MODE_ERROR,"Query : Update Er\n");
		//else
			//WcLogPrintf(WCLog::E_MODE_ALL,"Query : Update Ok\n");

		// Oracle 질의 - select
		nRet = wcOracle.Query(&tOraRes1,"select NAME,NICK,AGE,REGD from TESTINNO where test='%s'",cNo);
		if (nRet != -1)
		{
		    while(tOraRes1.pResultSet->next())
		    {
				s1 = tOraRes1.pResultSet->getString(1);
				s2 = tOraRes1.pResultSet->getString(2);
				n1 = tOraRes1.pResultSet->getInt(3);
				s3 = tOraRes1.pResultSet->getString(4);
				cout << "[Res01] " << s1 << ", " << s2 << ", " << n1 << ", " << s3 << endl;
			}
			wcOracle.FreeResult(&tOraRes1);
		}
		else
			WcLogPrintf(WCLog::E_MODE_ERROR,"Query : Select Er\n");

		// Oracle 질의 - delete
		nRet = wcOracle.Query(&tOraRes1,"delete testinno where test='%s'",cNo);
		wcOracle.FreeResult(&tOraRes1);
		if (nRet == -1)
			WcLogPrintf(WCLog::E_MODE_ERROR,"Query 5 : Delete Er\n");
		//else
			//WcLogPrintf(WCLog::E_MODE_ALL,"Query 5 : Delete Ok\n");
	}
	
	// Oracle 질의 - commit
	if (nRet != -1) {
		nRet = wcOracle.Query(&tOraRes1,"commit");
	}
	else {
		nRet = wcOracle.Query(&tOraRes1,"rollback");
	}
	wcOracle.FreeResult(&tOraRes1);

	// Oracle 닫기
	wcOracle.VClose();


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스트레스 테스트 종료
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// 종료시간 기록
	gettimeofday(&tv,NULL);
	gLogT.Printf(WCLog::E_MODE_ALL,"[%s] Time End (Pid:%d)(Pno:%d) (Time:%ld-%ld)\n",__FUNCTION__,getpid(),nPno,tv.tv_sec,tv.tv_usec);
	// 쓰레드 종료
	fprintf(stderr,"[%s] End (Pid:%d)(Pno:%d)\n",__FUNCTION__,getpid(),nPno);

	return NULL;
}
