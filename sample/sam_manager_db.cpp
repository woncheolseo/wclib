/*
<WCManagerDB 예제>

0. 단위 테스트 
1. 기본 사용법1
2. 스트레스 테스트

Usage : sam_manager_db 모드번호 프로세스수 쓰레드수 태스크수
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
		fprintf(stderr, "@ usage\n\tsam_manager_db ModeNo ProcessCount ThreadCount TaskCount\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1, 9-StressTest\
			\n@ Example\n\tsam_manager_db 0\n\tsam_manager_db 1\n\tsam_manager_db 9 2 10 1000\n");
	}	

	if (nMode == 0) Sam_Unittest(argc, argv);		// 단위테스트, sam_manager_db 0 
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_manager_db 1
	else if (nMode == 2) Sam_Stress(argc, argv, 1);	// 스트레스테스트, sam_manager_db 9 2 10 1000

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
* WCManagerDB 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	// [PostgreSQL] ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	WCManagerDB wcManDB;
	WCPostgreSQL wcPqsql, *pwcPqsql, *pMy;
	string strQuery;
	pqxx::result pqResult;
	char cNo[15];
	int nRes;

	// DB객체가 외부에서 연결하면 ManagerDB에 등록해야 ManagerDB 사용이 가능하다.
	if (wcPqsql.Open("192.168.1.207", 5432, "nwaf", "nwaf1234", "nwaf") == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Open 1");
		return;
	}
	nRes = wcManDB.Add(&wcPqsql);
	if (nRes == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Add 1");
		return;
	}

	// DB객체가 외부에서 연결하면 ManagerDB에 등록해야 ManagerDB 사용이 가능하다. 그리고, 외부에서 new 할당했으므로 반드시 외부에서 delete 해줘야 한다.
	pwcPqsql = new WCPostgreSQL();
	if (pwcPqsql->Open("192.168.1.207", 5432, "nwaf", "nwaf1234", "nwaf") == -WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Open 2");
		return;
	}
	nRes = wcManDB.Add(pwcPqsql);
	if (nRes == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"Add 2");
		return;
	}	

	// ManagerDB 오픈
	nRes = wcManDB.Open();
	if (nRes == WC_NOK) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR,"Open");
		return;
	}

	// SQL 쿼리 
	for (int i=0; i<10; i++)
	{
		// ManagerDB에 등록된 DB연결자 중 하나를 얻는다. 
		pMy = (WCPostgreSQL *)wcManDB.GetDB();
		if (pMy)
		{
			sprintf(cNo, "%05d%05d", i, i);
			cNo[10] = 0x00;	

			// 테이블 삭제
			strQuery = str(boost::format("DROP TABLE COMPANY_%1%") % cNo);	 //strQuery = "DROP TABLE COMPANY_" + cNo;	
			nRes = pMy->Query(strQuery, pqResult);
			if (nRes == WC_NOK) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[E] Query-drop");
				//break;		
			}

			// 테이블 생성 
			strQuery = str(boost::format("CREATE TABLE COMPANY_%1% (" \
			      "ID INT PRIMARY KEY     NOT NULL," \
			      "NAME           TEXT    NOT NULL," \
			      "AGE            INT     NOT NULL," \
			      "ADDRESS        CHAR(50)," \
			      "SALARY         REAL );") % cNo);
			nRes = pMy->Query(strQuery, pqResult);
			if (nRes == WC_NOK) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Query-create");
				break;		
			}

			// 데이터 삽입
			strQuery = str(boost::format("INSERT INTO COMPANY_%1% (ID,NAME,AGE,ADDRESS,SALARY) VALUES (1, 'Neo', 32, 'California', 20000.00 ); " \
			      "INSERT INTO COMPANY_%2% (ID,NAME,AGE,ADDRESS,SALARY) VALUES (2, 'Nice', 25, 'Texas', 15000.00 ); "     \
			      "INSERT INTO COMPANY_%3% (ID,NAME,AGE,ADDRESS,SALARY) VALUES (3, 'Fall', 23, 'Norway', 20000.00 );" \
			      "INSERT INTO COMPANY_%4% (ID,NAME,AGE,ADDRESS,SALARY) VALUES (4, 'Summer', 25, 'Rich-Mond ', 65000.00 );") % cNo % cNo % cNo % cNo);	
			nRes = pMy->Query(strQuery, pqResult);
			if (nRes == WC_NOK) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Query-insert");
				//break;		
			}

			// 데이터 수정
			strQuery = str(boost::format("UPDATE COMPANY_%1% SET NAME='SEO' where ID=1") % cNo);	
			nRes = pMy->Query(strQuery, pqResult);
			if (nRes == WC_NOK) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Query-update");
				//break;		
			}

			// 데이터 삭제
			strQuery = str(boost::format("DELETE FROM COMPANY_%1% where ID=3") % cNo);	
			nRes = pMy->Query(strQuery, pqResult);
			if (nRes == WC_NOK) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Query-delete");
				//break;		
			}	

			// 테이블 조회 
			strQuery = str(boost::format("SELECT * FROM COMPANY_%1%") % cNo);	
			nRes = pMy->Query(strQuery, pqResult, true);
			if (nRes == WC_NOK) {
				WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "Query-select");
				//break;		
			}	
			for (pqxx::result::const_iterator itRow = pqResult.begin(); itRow != pqResult.end(); ++itRow) {
				for (pqxx::result::tuple::const_iterator itField = itRow->begin(); itField != itRow->end(); ++ itField)	{
					cout << "[s1]" << itField << endl;
				}
				//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "ID = %d", it[0].as<int>());
				//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "NAME = %s", it[1].as<string>().c_str());
				//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "AGE = %d", it[2].as<int>());
				//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "ADDRESS = %s", it[3].as<string>().c_str());
				//WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ALL, "SALARY = %f", it[4].as<float>());
			}
			pqResult.clear();

			strQuery = "commit";
			pMy->Query(strQuery, pqResult);	
		}
	}

	// 해제
	WC_DELETEWC(pwcPqsql);	// 외부할당한 객체는 외부에서 해제해야 함

	/* [MySQL] ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	WCManagerDB wcManDB;
	WCMysql wcMysql, *pwcMysql, *pMy;
	MYSQL_ROW myRow;
	MYSQL_RES *pmyRes;
	char cTemp[10][500];
	char cNo[15];
	int nRes;

	// DB객체가 외부에서 연결하면 ManagerDB에 등록해야 ManagerDB 사용이 가능하다.
	if (wcMysql.VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8) == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"VOpen 1 (%d)\n",wcMysql.GetError());
		return;
	}
	nRes = wcManDB.VAdd(&wcMysql);
	if (nRes == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"VAdd 2\n");
		return;
	}

	// DB객체가 외부에서 연결하면 ManagerDB에 등록해야 ManagerDB 사용이 가능하다. 그리고, 외부에서 new 할당했으므로 반드시 외부에서 delete 해줘야 한다.
	pwcMysql = new WCMysql();
	if (pwcMysql->VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8) == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"VOpen 1 (%d)\n",pwcMysql->GetError());
		return;
	}
	nRes = wcManDB.VAdd(pwcMysql);
	if (nRes == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"VAdd 2\n");
		return;
	}	

	// ManagerDB의 VOpen 함수를 이용하면 내부에서 알아서 new,delete 해주므로 연결정보만 파라미터로 넘겨주면 된다.
	nRes = wcManDB.VOpen(WCObject::E_OTYPE_MYSQL,5,"localhost","root","a","neo21");
	if (nRes == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"OpenDBPool\n");
		return;
	}

	for (int i=0; i<50; i++)
	{
		// ManagerDB에 등록된 DB연결자 중 하나를 얻는다. 
		pMy = (WCMysql *)wcManDB.GetDB();
		if (pMy)
		{
			sprintf(cNo,"%05d%05d",i,i);
			cNo[10]=0x00;			
		
			// mysql 질의 - 삽입 
			pmyRes = pMy->Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('김태희','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'%s','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')",cNo);
			if (pmyRes != NULL)
				WcLogPrintf(WCLog::E_MODE_ALL,"Query 1 : Insert Ok\n");
			else
				WcLogPrintf(WCLog::E_MODE_ERROR,"Query 1 : Insert Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());

			// mysql 질의 - 수정
			pmyRes = wcMysql.Query("update testinno set name='한예슬',age=28,address='경상남도 남해군 철수네집' where test='%s'",cNo);
			if (pmyRes != NULL) 
				WcLogPrintf(WCLog::E_MODE_ALL,"Query 2 : Update Ok\n");
			else
				WcLogPrintf(WCLog::E_MODE_ERROR,"Query 2 : Update Er (%d,%s)\n",pMy->GetErrno(),pMy->GetError());

			// mysql 질의 - 조회
			pmyRes = wcMysql.Query("select * from testinno where test='%s'",cNo);
			if (pmyRes)
			{
				while ((myRow=pMy->FetchRow(pmyRes)))
				{
					for (int xx=0; xx<10; xx++) {
						if (myRow[xx])
							strcpy(&cTemp[xx][0], myRow[xx]);
						else
							strcpy(&cTemp[xx][0], "NULL");
					}
					WcLogPrintf(WCLog::E_MODE_ALL,"Query 3 : Select Ok (%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%.30s)\n",&cTemp[0][0],&cTemp[1][0],&cTemp[2][0],&cTemp[3][0],&cTemp[4][0],&cTemp[5][0],&cTemp[6][0],&cTemp[7][0],&cTemp[8][0],&cTemp[9][0]);
				}
				pMy->FreeResult(pmyRes);
			}
			else 
				WcLogPrintf(WCLog::E_MODE_ERROR,"Query 3 : Select Er (%d,%s)\n",pMy->GetErrno(),pMy->GetError());

			// mysql 질의 - 삭제 
			pmyRes = wcMysql.Query("delete from testinno where test='%s'",cNo);
			if (pmyRes != NULL) 
				WcLogPrintf(WCLog::E_MODE_ALL,"Query 4 : Delete Ok\n");
			else
				WcLogPrintf(WCLog::E_MODE_ERROR,"Query 4 : Delete Er (%d,%s)\n",pMy->GetErrno(),pMy->GetError());
		}
	}

	// 해제
	WcXDeleteW(pwcMysql);	// 외부할당한 객체는 외부에서 해제해야 함
	*/
}


/****************************************************************************************************************************************************************************************************
* 2. 스트레스 테스트
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
	pthread_mutex_init(&g_Mutex,NULL);
	gLog1.Open("log/log_g1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);
	gLogT.Open("log/log_t1.txt", WCLog::E_LEVEL_ALL, WCLog::E_CYCLE_DAY);

	if (argc > 3) {
		nFork = atoi(argv[2]);		// 프로세스수
		nThread = atoi(argv[3]);	// 쓰레드수
		gnLine = atoi(argv[4]);		// 태스크수	
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

	/*[test]
	WCManagerDB wcManDB;
	WCMysql wcMysql, *pwcMysql, *pMy;
	MYSQL_ROW myRow;
	MYSQL_RES *pmyRes;
	char cTemp[10][500];
	char cNo[15];
	int nRes;

	// DB객체가 외부에서 연결하면 ManagerDB에 등록해야 ManagerDB 사용이 가능하다.
	if (wcMysql.VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8) == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"VOpen 1 (%d)\n",wcMysql.GetError());
		return NULL;
	}
	nRes = wcManDB.VAdd(&wcMysql);
	if (nRes == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"VAdd 2\n");
		return NULL;
	}

	// DB객체가 외부에서 연결하면 ManagerDB에 등록해야 ManagerDB 사용이 가능하다. 그리고, 외부에서 new 할당했으므로 반드시 외부에서 delete 해줘야 한다.
	pwcMysql = new WCMysql();
	if (pwcMysql->VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8) == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"VOpen 1 (%d)\n",pwcMysql->GetError());
		return NULL;
	}
	nRes = wcManDB.VAdd(pwcMysql);
	if (nRes == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"VAdd 2\n");
		return NULL;
	}	

	// ManagerDB의 VOpen 함수를 이용하면 내부에서 알아서 new,delete 해주므로 연결정보만 파라미터로 넘겨주면 된다.
	nRes = wcManDB.VOpen(WCObject::E_OTYPE_MYSQL,3,"localhost","root","a","neo21");
	if (nRes == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"OpenDBPool\n");
		return NULL;
	}

	for (int i=0; i<gnLine; i++)
	{
		// ManagerDB에 등록된 DB연결자 중 하나를 얻는다. 
		pMy = (WCMysql *)wcManDB.GetDB();
		if (pMy)
		{
			sprintf(cNo,"%05d%05d",nPno,i);
			cNo[10]=0x00;			

			// mysql 질의 - 삽입 
			pmyRes = pMy->Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('김태희','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'%s','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')",cNo);
			if (pmyRes == NULL)
				WcLogPrintf(WCLog::E_MODE_ERROR,"Query 1 : Insert Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());
			//else
				//WcLogPrintf(WCLog::E_MODE_ALL,"Query 1 : Insert Ok\n");

			// mysql 질의 - 수정
			pmyRes = wcMysql.Query("update testinno set name='한예슬',age=28,address='경상남도 남해군 철수네집' where test='%s'",cNo);
			if (pmyRes == NULL) 
				WcLogPrintf(WCLog::E_MODE_ERROR,"Query 2 : Update Er (%d,%s)\n",pMy->GetErrno(),pMy->GetError());
			//else
				//WcLogPrintf(WCLog::E_MODE_ALL,"Query 2 : Update Ok\n");

			// mysql 질의 - 조회
			pmyRes = wcMysql.Query("select * from testinno where test='%s'",cNo);
			if (pmyRes)
			{
				while ((myRow=pMy->FetchRow(pmyRes)))
				{
					for (int xx=0; xx<10; xx++) {
						if (myRow[xx])
							strcpy(&cTemp[xx][0], myRow[xx]);
						else
							strcpy(&cTemp[xx][0], "NULL");
					}
					// WcLogPrintf(WCLog::E_MODE_ALL,"Query 3 : Select Ok (%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%.30s)\n",&cTemp[0][0],&cTemp[1][0],&cTemp[2][0],&cTemp[3][0],&cTemp[4][0],&cTemp[5][0],&cTemp[6][0],&cTemp[7][0],&cTemp[8][0],&cTemp[9][0]);
				}
				pMy->FreeResult(pmyRes);
			}
			else 
				WcLogPrintf(WCLog::E_MODE_ERROR,"Query 3 : Select Er (%d,%s)\n",pMy->GetErrno(),pMy->GetError());

			// mysql 질의 - 삭제 
			//pmyRes = wcMysql.Query("delete from testinno where test='%s'",cNo);
			//if (pmyRes == NULL) 
			//	WcLogPrintf(WCLog::E_MODE_ERROR,"Query 4 : Delete Er (%d,%s)\n",pMy->GetErrno(),pMy->GetError());
			//else
			//	WcLogPrintf(WCLog::E_MODE_ALL,"Query 4 : Delete Ok\n");
		}
		else
			WcLogPrintf(WCLog::E_MODE_ERROR,"!pMy (%d)\n",i);
	}
	
	// 해제
	WcXDeleteW(pwcMysql);	// 외부할당한 객체는 외부에서 해제해야 함

	*/

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

