/*
<WCMysql 예제>

0. 단위 테스트 
1. 기본 사용법1
2. 기본 사용법2
3. 스트레스 테스트

Usage : sam_mysql 모드번호 프로세스수 쓰레드수 라인수
*/

#include <wc_lib.h>

void SamInit();
void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1();
void Sam_Basic2();
void Sam_Stress(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int nMode = -1;

	// mysql_library_init(0,NULL,NULL); 	// 라이브러리 초기화 

	SamInit();

	if (argc > 1) {
		nMode = atoi(argv[1]);
	}

	if (nMode == 0) Sam_Unittest(argc,argv);		// 단위테스트, sam_mysql 0 (WCMysql 단위테스트만 실행시킬 경우, "--gtest_filter=UnitTest_WCMysql.*" 인자 추가)
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_mysql 1
	else if (nMode == 2) Sam_Basic2();				// 기본사용법2, sam_mysql 2
	else if (nMode == 3) Sam_Stress(argc,argv);		// 스트레스테스트, sam_mysql 3 3 100 1000

	// mysql_library_end(); 	// 라이브러리 종료

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
* MYSQL 함수 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	MYSQL_ROW myRow;
	MYSQL_RES *pmyRes;
	MYSQL *pmySql=NULL;
	int nRes;
	char cTemp[10][500];

	// mysql 초기화
	pmySql = mysql_init(NULL);

	// mysql 연결
	if (!mysql_real_connect(pmySql,"localhost","root","a","neo21", 3306, (char *)NULL, 0)) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"mysql_real_connect (Er:%d,%s)\n",mysql_errno(pmySql),mysql_error(pmySql));
		return;
	}
	mysql_query(pmySql, "set names utf8");

	// mysql 질의 - 삽입
	nRes = mysql_query(pmySql,"insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('김태희','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'B11114452','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')");
	if (nRes == 0) WcLogPrintf(WCLog::E_MODE_ALL,"mysql_query 1 : Insert Ok\n",nRes);
	else WcLogPrintf(WCLog::E_MODE_ERROR,"mysql_query 1 : Insert Er (%d,%s)\n",mysql_errno(pmySql),mysql_error(pmySql));

	// mysql 질의 - 조회 
	nRes = mysql_query(pmySql,"select * from testinno limit 10");
	if (nRes == 0) {
		pmyRes = mysql_store_result(pmySql);
		if (pmyRes) {
			while ((myRow = mysql_fetch_row(pmyRes))) {
				for (int i=0; i<10; i++) {
					if (myRow[i])
						strcpy(&cTemp[i][0], myRow[i]);
					else
						strcpy(&cTemp[i][0], "NULL");
				}
				WcLogPrintf(WCLog::E_MODE_ALL,"mysql_query 2 : Select Ok (%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%.30s)\n",&cTemp[0][0],&cTemp[1][0],&cTemp[2][0],&cTemp[3][0],&cTemp[4][0],&cTemp[5][0],&cTemp[6][0],&cTemp[7][0],&cTemp[8][0],&cTemp[9][0]);
			}
			mysql_free_result(pmyRes);
		}
		else 
			WcLogPrintf(WCLog::E_MODE_ERROR,"mysql_query 2 : Select Er (%d,%s)\n",mysql_errno(pmySql),mysql_error(pmySql));
	}

	// mysql 질의 - 삭제
	nRes = mysql_query(pmySql,"delete from testinno");
	if (nRes == 0) WcLogPrintf(WCLog::E_MODE_ALL,"mysql_query 3 : Delete Ok\n",nRes);
	else WcLogPrintf(WCLog::E_MODE_ERROR,"mysql_query 3 : Delete Er (%d,%s)\n",mysql_errno(pmySql),mysql_error(pmySql));

	// mysql 닫기 
	mysql_close(pmySql);
}


/****************************************************************************************************************************************************************************************************
* 2. 기본 사용법2
*
* WCMysql 사용법을 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic2()
{
	MYSQL_ROW myRow;
	MYSQL_RES *pmyRes;
	WCMysql wcMysql;
	char cTemp[10][500];

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 일반 객체
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// mysql 초기화,연결
	if (wcMysql.VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8) == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"Open (%d)\n",wcMysql.GetError());
		return;
	}
	else WcLogPrintf(WCLog::E_MODE_ALL,"Open (IP:localhost)(ID:neo21)(PW:a)(DB:neo21)\n");

	// mysql 질의 - 삽입 
	pmyRes = wcMysql.Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('김태희','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'B11114452','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')");
	if (pmyRes != NULL)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 1 : Insert Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 1 : Insert Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());

	// mysql 질의 - 수정
	pmyRes = wcMysql.Query("update testinno set name='한예슬', age=28, address='경상남도 남해군 철수네집'");
	if (pmyRes != NULL) 
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 2 : Update Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 2 : Update Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());

	// mysql 질의 - 조회
	pmyRes = wcMysql.Query("select * from testinno");
	if (pmyRes)
	{
		while ((myRow=wcMysql.FetchRow(pmyRes)))
		{
			for (int xx=0; xx<10; xx++) {
				if (myRow[xx])
					strcpy(&cTemp[xx][0], myRow[xx]);
				else
					strcpy(&cTemp[xx][0], "NULL");
			}
			WcLogPrintf(WCLog::E_MODE_ALL,"Query 3 : Select Ok (%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%.30s)\n",&cTemp[0][0],&cTemp[1][0],&cTemp[2][0],&cTemp[3][0],&cTemp[4][0],&cTemp[5][0],&cTemp[6][0],&cTemp[7][0],&cTemp[8][0],&cTemp[9][0]);
		}
		wcMysql.FreeResult(pmyRes);
	}
	else 
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 3 : Select Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());

	// mysql 질의 - 삭제 
	pmyRes = wcMysql.Query("delete from testinno");
	if (pmyRes != NULL) 
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 4 : Delete Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 4 : Delete Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());

	// mysql 닫기
	wcMysql.VClose();


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 복사생성자, 대입연산자
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	WCMysql wcMysqlS, wcMysqlR = wcMysql;
	wcMysqlS = wcMysql;

	// mysql 질의 - 삽입 
	pmyRes = wcMysqlR.Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('수지','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'B11114452','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')");
	if (pmyRes != NULL)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 1 : Insert Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 1 : Insert Er (%d,%s)\n",wcMysql.GetErrno(),wcMysqlR.GetError());

	// mysql 질의 - 삽입 
	pmyRes = wcMysqlS.Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('한가인','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'B11114452','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')");
	if (pmyRes != NULL)
		WcLogPrintf(WCLog::E_MODE_ALL,"Query 1 : Insert Ok\n");
	else
		WcLogPrintf(WCLog::E_MODE_ERROR,"Query 1 : Insert Er (%d,%s)\n",wcMysql.GetErrno(),wcMysqlS.GetError());

	// mysql 닫기
	wcMysqlR.VClose();
	wcMysqlS.VClose();
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

	MYSQL_ROW myRow;
	MYSQL_RES *pmyRes;
	WCMysql wcMysql;
	char cTemp[10][500];
	char cNo[15];

	// mysql 초기화,연결
	if (wcMysql.VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8) == -1) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"Open (%d)\n",wcMysql.GetError());
		return NULL;
	}
	else WcLogPrintf(WCLog::E_MODE_ALL,"Open (IP:localhost)(ID:neo21)(PW:a)(DB:neo21)\n");

	for(int i=0; i<gnLine; i++) 
	{
		sprintf(cNo,"%05d%05d",nPno,i);
		cNo[10]=0x00;

		// mysql 질의 - 삽입 
		pmyRes = wcMysql.Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('김태희','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'%s','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')",cNo);
		if (pmyRes == NULL) 
			WcLogPrintf(WCLog::E_MODE_ERROR,"Query 1 : Insert Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());
		//else
			//WcLogPrintf(WCLog::E_MODE_ALL,"Query 1 : Insert Ok\n");

		// mysql 질의 - 수정
		pmyRes = wcMysql.Query("update testinno set name='한예슬',age=28,address='경상남도 남해군 철수네집' where test='%s'",cNo);
		if (pmyRes == NULL) 
			WcLogPrintf(WCLog::E_MODE_ERROR,"Query 2 : Update Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());
		//else
			//WcLogPrintf(WCLog::E_MODE_ALL,"Query 2 : Update Ok\n");

		// mysql 질의 - 조회
		pmyRes = wcMysql.Query("select * from testinno where test='%s'",cNo);
		if (pmyRes)
		{
			while ((myRow=wcMysql.FetchRow(pmyRes)))
			{
				for (int xx=0; xx<10; xx++) {
					if (myRow[xx])
						strcpy(&cTemp[xx][0], myRow[xx]);
					else
						strcpy(&cTemp[xx][0], "NULL");
				}
				WcLogPrintf(WCLog::E_MODE_ALL,"Query 3 : Select Ok (%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%s)(%.30s)\n",&cTemp[0][0],&cTemp[1][0],&cTemp[2][0],&cTemp[3][0],&cTemp[4][0],&cTemp[5][0],&cTemp[6][0],&cTemp[7][0],&cTemp[8][0],&cTemp[9][0]);
			}
			wcMysql.FreeResult(pmyRes);
		}
		else 
			WcLogPrintf(WCLog::E_MODE_ERROR,"Query 3 : Select Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());	

		/* mysql 질의 - 삭제 
		pmyRes = wcMysql.Query("delete from testinno where test='%s'",cNo);
		if (pmyRes == NULL) 
			WcLogPrintf(WCLog::E_MODE_ERROR,"Query 4 : Delete Er (%d,%s)\n",wcMysql.GetErrno(),wcMysql.GetError());
		//else
			//WcLogPrintf(WCLog::E_MODE_ALL,"Query 4 : Delete Ok\n");
		*/
	}

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


