/**
	@file wc_test.cpp
   	@brief WCTest 소스
*/

#include "wc_lib.h"
#include "wc_manager.cpp"

static int g_nRTmCnt = 0;	// 전체 타이머 이벤트 수신 횟수

WCTestManager::WCTestManager(const char *pcName)
{
	SetName(pcName);
}

WCTestTimer::WCTestTimer(const char *pcName) : m_nSTmCnt(0), m_nRTmCnt(0)
{
	memset(m_cTest,0x00,sizeof(m_cTest));
	strcpy(m_cTest,pcName);	
}

WCTestTimer::~WCTestTimer()
{
}

void WCTestTimer::VTimer(const timer_t tTimer, const int nTimerID)
{
	m_nRTmCnt++;
	g_nRTmCnt++;
}

WCTestThread::WCTestThread(const char *pcName) : WCThread(pcName), m_bLoop(true), m_nThread(1), m_nCountIn(0), m_nCountOut(0)
{
}

WCTestThread::~WCTestThread()
{
	m_bLoop = false;
}

// 쓰레드 실행 함수
void WCTestThread::VRun()
{
	char cTemp[11],cNo[11];
	int nLen;

	// 쓰레드 통신 테스트를 위해 수신측 쓰레드명 설정
	memcpy(cNo,GetName(),sizeof(cNo)-1); cNo[sizeof(cNo)-1] = 0x00;
	memcpy(cTemp,&cNo[5],5); cTemp[6] = 0x00;
	nLen = atoi(cTemp);
	nLen++;
	if (nLen >= m_nThread) nLen = 0;
	sprintf(&cNo[5],"%05d",nLen);

	for(int i=0; i<100; i++) 
	{
		if (m_bLoop == false) break;	// VStop() 함수에 의해 종료조건이 만족하면 루프를 빠져나간다.

		// 내부카운트 증가
		m_nCountIn++;
		
		// 쓰레드통신 : 다른 쓰레드로 데이터를 보낸다.
		if (m_pManager)	((WCManagerThread *)m_pManager)->WriteIpc("abcde",(unsigned short)5,cNo);
				
		usleep(10000);
	}
}

// 쓰레드 실행 함수 종료 조건을 준다.
void WCTestThread::VStop()
{
	m_bLoop = false;
}

// 쓰레드 내부 통신 : 쓰레드를 통해서 데이터를 수신하면 발생한다.
int WCTestThread::VEvent(const char *pcData, const int nPackSz)
{
	// 외부카운트 증가
	m_nCountOut++;

	return 0;
}

// 단위테스트 (WCObject)
TEST_F(UnitTest_WCObject, UTObject) 
{
	WCObject wcObj1,wcObj2("NiceRobot");

	// 객체 생성
	EXPECT_STREQ(wcObj2.GetName(),"NiceRobot");					// 객체명 확인
	EXPECT_EQ(wcObj2.IsLive(),true);							// 객체존재 확인
	EXPECT_EQ(wcObj2.GetType(),WCObject::E_OTYPE_OBJECT);		// 객체타입 확인
	// wcObj2.SetGroup(&wcObj2);
	// EXPECT_EQ(wcObj2.GetGroup(),&wcObj2);					// 객체 그룹포인터 확인

	// 복사생성자 -> 별개의 객체 생성 확인
	WCObject wcObj3=wcObj2;
	// EXPECT_NE(wcObj3.GetKey(),wcObj2.GetKey());				// 두 객체는 객체키가 다르다.
	// EXPECT_STRNE(wcObj3.GetName(),wcObj2.GetName());			// 두 객체는 객체명이 다르다.
	EXPECT_STREQ(wcObj3.GetName(),wcObj2.GetName());			// 복사했으므로 두 객체의 객체명은 같다.
	EXPECT_EQ(wcObj3.GetType(),wcObj2.GetType());				// 복사했으므로 두 객체의 객체타입은 같다.

	// 대입연산자 -> 별개의 객체 생성 확인
	WCObject wcObj4;
	wcObj4 = wcObj2;
	// EXPECT_NE(wcObj4.GetKey(),wcObj2.GetKey());				// 두 객체는 객체키가 다르다.
	// EXPECT_STRNE(wcObj4.GetName(),wcObj2.GetName());			// 두 객체는 객체명이 다르다.
	EXPECT_STREQ(wcObj4.GetName(),wcObj2.GetName());			// 대입했으므로 두 객체의 객체명은 같다.
	EXPECT_EQ(wcObj4.GetType(),wcObj2.GetType());				// 대입했으므로 두 객체의 객체타입은 같다.

	// 객체 소멸
	WCObject *pObj5 = new WCObject("test");
	delete(pObj5);
	EXPECT_EQ(pObj5->IsLive(),false);							// 객체 존재 확인
}

// 단위테스트 (WCLog) - 파일열기,파일닫기
TEST_F(UnitTest_WCLog, UTLogOpen) 
{
	int nRet;
	
	WCLog wcLog1;

	// 파일오픈 - 경로,이름 공백
	nRet = wcLog1.Open("","");
	EXPECT_GT(nRet,-1);									// Open()함수 결과값 확인
	EXPECT_EQ(wcLog1.GetFd(),STDOUT_FILENO);			// Open()함수 인자 모두가 ""일 경우, 로그파일은 표준출력인지 확인

	// 파일오픈 - 이름만 공백
	nRet = wcLog1.Open("","log_g1.txt");
	EXPECT_GT(nRet,-1);									// Open()함수 결과값 확인
	EXPECT_GT(wcLog1.GetFd(),STDERR_FILENO);			// Open()함수 인자 중 경로만 ""일 경우, 로그파일이 생성되었는가 확인

	// 열기 - 이름이 너무 긴 경우
	nRet = wcLog1.Open("../log","log_g222222222222222222222222222222222222222222222222222222222222222222222222.txt");
	EXPECT_GT(nRet,-1);									// Open()함수 결과값 확인
	EXPECT_GT(wcLog1.GetFd(),STDERR_FILENO);			// Open()함수 인자 중 파일 이름이 너무 긴 경우에도 로그파일이 생성되었는가 확인

	// 열기 - 정상
	nRet = wcLog1.Open("../log","log_gtest01.txt");
	EXPECT_GT(nRet,-1);									// Open()함수 결과값 확인
	EXPECT_GT(wcLog1.GetFd(),STDERR_FILENO);			// Open()함수 인자가 정상일 경우, 로그파일이 생성되었는가 확인

	// 열기 - 정상
	nRet = wcLog1.Open("../log","log_gtest01.txt",WCLog::E_MODE_ALL,WCLog::E_CYCLE_DAY);
	EXPECT_GT(nRet,-1);									// Open()함수 결과값 확인
	EXPECT_GT(wcLog1.GetFd(),STDERR_FILENO);			// Open()함수 인자가 정상일 경우, 로그파일이 생성되었는가 확인
	EXPECT_EQ(wcLog1.GetMode(),WCLog::E_MODE_ALL);		// 로그모드 설정 확인
	EXPECT_EQ(wcLog1.GetCycle(),WCLog::E_CYCLE_DAY);	// 로그주기 설정 확인

	// 닫기
	wcLog1.Close();
	EXPECT_EQ(wcLog1.GetFd(),-1);						// 로그파일 닫힘 확인
}

// 단위테스트 (WCLog) - 로그출력
TEST_F(UnitTest_WCLog, UTLogPrintf) 
{
	int nRet;
	WCLog wcLog1;

	// 출력
	nRet = wcLog1.Printf(WCLog::E_MODE_ALL,"abcdefghijklmlopqrstuvwxyz - %d\n",13);
	EXPECT_GE(nRet,0);	// 로그 출력 여부 확인
	
	// 출력 - 데이터가 현재 할당된 버퍼보다 크다.
	nRet = wcLog1.Printf(WCLog::E_MODE_WARN,"[%s][%010d] 1ng2ng3ng4ng5ng6ng7ng8ng9ngAngBngCngDngEngFngGngHngIngJngKngLngMngNngOngPngQngRngSngTngUngVngWngXngYngZnga1234512345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890-%03d\n",__FUNCTION__,1237,99);	
	EXPECT_GE(nRet,0);	// 현재 출력 버퍼보다 큰 경우 버퍼 재할당 후 로그 출력 여부 확인

	// 출력 - 모드 변경
	wcLog1.SetMode(WCLog::E_MODE_DEBUG);
	nRet = wcLog1.Printf(WCLog::E_MODE_WARN,"[%s] Mode Change 01\n",__FUNCTION__);	
	EXPECT_EQ(nRet,-2);	// 로그모드가 다른 경우 로그 출력 여부 확인

	// 출력 - hex
	nRet = wcLog1.PrintfnByte("abcde",sizeof("abcee"),WCLog::E_TYPE_HEX); //nRet = wcLog1.PrintfHex("abcde\n",sizeof("abcee"));
	EXPECT_GE(nRet,0);	// hex 출력 여부 확인

	// 출력 - char
	nRet = wcLog1.PrintfnByte("abcde",sizeof("abcee"),WCLog::E_TYPE_CHAR); //nnRet = wcLog1.PrintfChar("abcde\n",sizeof("abcee"));
	EXPECT_GE(nRet,0);	// char 출력 여부 확인
}

// 단위테스트 (WCCfg) - 환경파일 열기,읽기
TEST_F(UnitTest_WCCfg, UTCfg) 
{
	WCCfg wcCfg1("../etc/sam_test.conf"), wcCfg2;
	string str1,str2,str3;
	int nRet;

	// 환경파일 오픈
	nRet = wcCfg2.Open("../etc/sam_db.conf");
	EXPECT_GE(nRet,0);				// 오픈 성공 유무 확인
	EXPECT_GE(wcCfg1.Size(),0);		// 환경파일 필드 갯수 확인
	EXPECT_GE(wcCfg2.Size(),0);		// 환경파일 필드 갯수 확인

	// 데이터 얻기 
	str1 = wcCfg1.Get("sec01","db");
	str2 = wcCfg1.Get("netian","db");
	str3 = wcCfg1.Get("test","my");	
	EXPECT_STREQ(str1.c_str(),"mysql");			// "sec01" 섹션의 "db" 필드값 확인
	EXPECT_STREQ(str2.c_str(),"oracle");		// "netian" 섹션의 "db" 필드값 확인
	// "필드명 : 필드값"의 최대 길이는 변수 msc_nMaxLine(현재 1,000)값에 의한다. "test"섹션의 my" 필드값이 최대값만큼만 추출되는지 확인
	EXPECT_STREQ(str3.c_str(), "a123456789b123456789c123456789d123456789e123456789a123456789b123456789c123456789d123456789e123456789a123456789b123456789c123456789d123456789e123456789a123456789b123456789c123456789d123456789e123");
	str1 = wcCfg2.Get("db-test","name");
	str2 = wcCfg2.Get("db-netian","pw");
	str3 = wcCfg2.Get("db-swc-swc","pw");
	EXPECT_STREQ(str1.c_str(),"test");			// "db-test" 섹션의 "name" 필드값 확인
	EXPECT_STREQ(str2.c_str(),"egarreo01");		// "db-netian" 섹션의 "pw" 필드값 확인
	EXPECT_STREQ(str3.c_str(),"");				// "db-swc-swc" 섹션의 "pw" 필드값 확인. 해당 섹션은 없으므로 ""이어야 한다.
	str3 = wcCfg2.Get("db-swc","low","abcde");	
	EXPECT_STREQ(str3.c_str(),"abcde");			// "db-swc" 섹션의 "low" 필드값 확인. 해당 섹션은 없으므로 기본값 "abcde"가 반환되는지 확인한다.

	// 복사생성자,대입연산자
	WCCfg wcCfg11=wcCfg1;
	WCCfg wcCfg21;
	wcCfg21 = wcCfg2;

	wcCfg1.Clear();	// 원본 초기화
	wcCfg2.Clear();
	EXPECT_EQ(wcCfg1.Size(),0);					// 환경파일 필드 갯수 확인
	EXPECT_EQ(wcCfg1.Size(),0);					// 환경파일 필드 갯수 확인

	str1 = wcCfg11.Get("sec01","db");
	str2 = wcCfg11.Get("netian","db");
	str3 = wcCfg11.Get("test","my");	
	EXPECT_STREQ(str1.c_str(),"mysql");			// "sec01" 섹션의 "db" 필드값 확인
	EXPECT_STREQ(str2.c_str(),"oracle");		// "netian" 섹션의 "db" 필드값 확인
	str1 = wcCfg21.Get("db-test","name");
	str2 = wcCfg21.Get("db-netian","pw");
	str3 = wcCfg21.Get("db-swc-swc","pw");
	EXPECT_STREQ(str1.c_str(),"test");			// "db-test" 섹션의 "name" 필드값 확인
	EXPECT_STREQ(str2.c_str(),"egarreo01");		// "db-netian" 섹션의 "pw" 필드값 확인
	EXPECT_STREQ(str3.c_str(),"");				// "db-swc-swc" 섹션의 "pw" 필드값 확인. 해당 섹션은 없으므로 ""이어야 한다.
	str3 = wcCfg21.Get("db-swc","low","abcde");	
	EXPECT_STREQ(str3.c_str(),"abcde");			// "db-swc" 섹션의 "low" 필드값 확인. 해당 섹션은 없으므로 기본값 "abcde"가 반환되는지 확인한다.

}

// 단위테스트 (WCTimer) - 타이머
TEST_F(UnitTest_WCTimer, UTTimer) 
{
	WCTestTimer wcTest1("Timer1"),wcTest2("Timer2"),wcTest3("Timer3"),wcTest4("Timer4"),wcTest5("Timer5");
	WCTestTimer *pwcTest6,*pwcTest7;
	WCTimer wcTimer;
	timer_t tTimer[8];
	int nTemp[8];
	tTimer[0] = 0x00;
	nTemp[0] = 0x00;

	pwcTest6 = new WCTestTimer("Timer6");
	pwcTest7 = new WCTestTimer("Timer7");

	// 타이머 실행
	tTimer[1] = wcTimer.StartTimer((WITimer *)&wcTest1,4.3, 1, 1);	// 4.3초 간격으로 1번 발생 
	EXPECT_TRUE(tTimer[1]!=NULL);									// 타이머 생성 확인
	tTimer[2] = wcTimer.StartTimer((WITimer *)&wcTest2,2.0, 2, 0);	// 2.0초 간격으로 무한발생 
	EXPECT_TRUE(tTimer[2]!=NULL);									// 타이머 생성 확인
	tTimer[3] = wcTimer.StartTimer((WITimer *)&wcTest3,2.0, 3, 0);	// 2.0초 간격으로 무한발생 
	EXPECT_TRUE(tTimer[3]!=NULL);									// 타이머 생성 확인
	tTimer[4] = wcTimer.StartTimer((WITimer *)&wcTest4,2.0, 4, 0);	// 2.0초 간격으로 무한발생 
	EXPECT_TRUE(tTimer[4]!=NULL);									// 타이머 생성 확인
	tTimer[5] = wcTimer.StartTimer((WITimer *)&wcTest5,2.0, 5, 0);	// 2.0초 간격으로 무한발생 
	EXPECT_TRUE(tTimer[5]!=NULL);									// 타이머 생성 확인
	tTimer[6] = wcTimer.StartTimer((WITimer *)pwcTest6,2.5, 6, 5);	// 2.5초 간격으로 5번 발생 
	pwcTest6->m_nSTmCnt = 5;
	EXPECT_TRUE(tTimer[6]!=NULL);									// 타이머 생성 확인
	tTimer[7] = wcTimer.StartTimer((WITimer *)pwcTest7,1.5, 7, 0);	// 2.7초 간격으로 무한발생 
	EXPECT_TRUE(tTimer[7]!=NULL);									// 타이머 생성 확인

	// 타이머 발생
	WcLogPrintf(WCLog::E_MODE_ALL,"[%s] Wait 1\n",__FUNCTION__);
	for (int i=0; i<1000; i++) {
		sleep(1);
		if (wcTest1.m_nRTmCnt == 1) break;
	}
	EXPECT_EQ(wcTest1.m_nRTmCnt,1);									// 1번타이머는 타임이벤트 1번 수신 확인

	// 타이머 삭제 
	nTemp[2] = wcTest2.m_nRTmCnt;									// 삭제전 타이머이벤트수신횟수 저장
	nTemp[3] = wcTest3.m_nRTmCnt;
	nTemp[4] = wcTest4.m_nRTmCnt;
	nTemp[5] = wcTest5.m_nRTmCnt;
	wcTimer.DeleteTimer((WITimer *)&wcTest2);						// 타이머인터페이스 삭제
	wcTimer.DeleteTimer(tTimer[3]);	 								// 타이머값으로 삭제
	wcTimer.DeleteTimer((WITimer *)&wcTest4);						// 타이머인터페이스 삭제
	wcTimer.DeleteTimer(5);											// 타이머ID로 삭제
	WcLogPrintf(WCLog::E_MODE_ALL,"[%s] Wait 2\n",__FUNCTION__);
	for (int i=0; i<1000; i++) {
		sleep(1);
		if (pwcTest6->m_nRTmCnt == 5) break;
	}
	EXPECT_EQ(wcTest2.m_nRTmCnt,nTemp[2]);							// 2번타이머는 삭제했으므로 타이머이벤트수신횟수 변동이 없다.
	EXPECT_EQ(wcTest3.m_nRTmCnt,nTemp[3]);							// 3번타이머는 삭제했으므로 타이머이벤트수신횟수 변동이 없다.
	EXPECT_EQ(wcTest4.m_nRTmCnt,nTemp[4]);							// 4번타이머는 삭제했으므로 타이머이벤트수신횟수 변동이 없다.
	EXPECT_EQ(wcTest5.m_nRTmCnt,nTemp[5]);							// 5번타이머는 삭제했으므로 타이머이벤트수신횟수 변동이 없다.
	EXPECT_EQ(pwcTest6->m_nSTmCnt,pwcTest6->m_nRTmCnt);				// 6번타이머는 발생횟수와 이벤트수신횟수가 같은가 확인

	// 객체 소멸
	nTemp[7] = g_nRTmCnt;
	WcXDeleteW(pwcTest7);
	WcLogPrintf(WCLog::E_MODE_ALL,"[%s] Wait 3\n",__FUNCTION__);
	for (int i=0; i<5; i++) {
		sleep(1);
	}
	EXPECT_EQ(g_nRTmCnt,nTemp[7]);									// 7번타이머는 발생시킨 객체가 소멸했으므로 5초 대기 후에도 타이머이벤트수신횟수 변동이 없다.

	// 종료
	WcXDeleteW(pwcTest6);
}

// 단위테스트 (WCMd5)
TEST_F(UnitTest_WCMd5, UTMd5) 
{
	char cPw[100]={"1234"};
	char cMd5[100];
	int nRes;

	nRes = WCMd5::SGetMd5base64(cPw,(int)strlen(cPw),cMd5,(int)sizeof(cMd5));
	EXPECT_EQ(nRes,0);									// 결과값 확인
	EXPECT_STREQ(cMd5,"gdyb21LQTcIANtvYMT7QVQ=="); 		// 암호값 확인

	memcpy(cPw,"niceguy0728139",sizeof(cPw)-1);
	nRes = WCMd5::SGetMd5base64(cPw,(int)strlen(cPw),cMd5,(int)sizeof(cMd5));
	EXPECT_EQ(nRes,0);									// 결과값 확인
	EXPECT_STREQ(cMd5,"fmPBubaxEWeHUmkiNTV+bw=="); 		// 암호값 확인
}

// 단위테스트 (WCMysql) - 아래 DB는 생성되어 있어야 한다.
TEST_F(UnitTest_WCMysql, UTMysql) 
{
	MYSQL_RES *pmyRes;
	WCMysql wcMysql;
	int nRes;

	// mysql 초기화,연결
	nRes = wcMysql.VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8);
	EXPECT_EQ(nRes,0);	// DB연결 확인

	// mysql 질의 - 삽입 
	pmyRes = wcMysql.Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('김태희','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'B11114452','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')");
	EXPECT_TRUE(pmyRes!=NULL);	// DB질의 확인

	// mysql 질의 - 수정
	pmyRes = wcMysql.Query("update testinno set name='한예슬', age=28, address='경상남도 남해군 철수네집'");
	EXPECT_TRUE(pmyRes!=NULL);	// DB질의 확인

	// mysql 질의 - 조회
	pmyRes = wcMysql.Query("select * from testinno");
	EXPECT_TRUE(pmyRes!=NULL);	// DB질의 확인

	// mysql 질의 - 삭제 
	pmyRes = wcMysql.Query("delete from testinno");
	EXPECT_TRUE(pmyRes!=NULL);	// DB질의 확인

	// mysql 닫기
	wcMysql.VClose();
	EXPECT_EQ(wcMysql.GetTID(),0);	// DB닫기 확인
}

// 단위테스트 (WCOracle) - 아래 DB는 생성되어 있어야 한다.
TEST_F(UnitTest_WCOracle, UTOracl) 
{
	/*
	MYSQL_RES *pmyRes;
	WCMysql wcMysql;
	int nRes;

	// mysql 초기화,연결
	nRes = wcMysql.VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8);
	EXPECT_EQ(nRes,0);

	// mysql 질의 - 삽입 
	pmyRes = wcMysql.Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('김태희','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'B11114452','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')");
	EXPECT_TRUE(pmyRes!=NULL);

	// mysql 질의 - 수정
	pmyRes = wcMysql.Query("update testinno set name='한예슬', age=28, address='경상남도 남해군 철수네집'");
	EXPECT_TRUE(pmyRes!=NULL);

	// mysql 질의 - 조회
	pmyRes = wcMysql.Query("select * from testinno");
	EXPECT_TRUE(pmyRes!=NULL);

	// mysql 질의 - 삭제 
	pmyRes = wcMysql.Query("delete from testinno");
	EXPECT_TRUE(pmyRes!=NULL);

	// mysql 닫기
	wcMysql.VClose();
	EXPECT_EQ(wcMysql.GetTID(),0);
	*/
}

// 단위테스트 (WCManager)
TEST_F(UnitTest_WCManager, UTManager) 
{
	WCManager<int> tManager;								// Key를 int로
	WCManager<string> *pManager = new WCManager<string>;	// Key를 string으로
	WCTestManager *pMem1,*pMem2;
	int nRes;
	char g_cTestKey[][10] = {"Nice01","Girl02"};

	WCTestManager tOb1(g_cTestKey[0]);
	WCTestManager *pOb1 = new WCTestManager(g_cTestKey[1]);

	((WIMember *)pOb1)->SetInnerNew(true); // Manager 내부 할당으로 임시 설정. 소멸자에서 삭제됨

	// 멤버 추가 
	nRes = tManager.VAdd(1,(WIMember *)&tOb1);
	EXPECT_NE(nRes,-1);	// 멤버추가 확인
	nRes = tManager.VAdd(2,(WIMember *)pOb1);;
	EXPECT_NE(nRes,-1);	// 멤버추가 확인
	nRes = pManager->VAdd(tOb1.GetName(),(WIMember *)&tOb1);
	EXPECT_NE(nRes,-1);	// 멤버추가 확인
	nRes = pManager->VAdd(pOb1->GetName(),(WIMember *)pOb1);
	EXPECT_NE(nRes,-1);	// 멤버추가 확인

	// 멤버 검색
	pMem1 = (WCTestManager *)tManager.Find(1);
	EXPECT_STREQ(g_cTestKey[0],pMem1->GetName());			// 멤버검색 확인
	pMem2 = (WCTestManager *)tManager.Find(2);
	EXPECT_STREQ(g_cTestKey[1],pMem2->GetName());			// 멤버검색 확인
	pMem1 = (WCTestManager *)pManager->Find(g_cTestKey[0]);
	EXPECT_TRUE(pMem1!=NULL);								// 멤버검색 확인
	pMem2 = (WCTestManager *)pManager->Find(g_cTestKey[1]);
	EXPECT_TRUE(pMem2!=NULL);								// 멤버검색 확인

	// 멤버 삭제
	tManager.VRemove(1);
	tManager.VRemove(2);
	EXPECT_EQ(tManager.Size(),0);	// 멤버삭제 확인
	// pManager 소멸자에서 멤버 삭제하기 위해 삭제하지 않는다.
	// pManager->VRemove(g_cTestKey[0]);
	// pManager->VRemove(g_cTestKey[1]);
	EXPECT_EQ(pManager->Size(),2);	// 멤버삭제 확인

	//WcXDeleteW(pOb1);		// 내부할당으로 간주했으니 삭제하면 안 됨.
	WcXDeleteW(pManager);	// 소멸자에서 멤버 삭제 및 내부할당 객체는 메모리 해제된다.
}

// 단위테스트 (WCManagerDB) - 아래 DB는 생성되어 있어야 한다.
TEST_F(UnitTest_WCManagerDB, UTManagerDB) 
{
	WCManagerDB wcManDB;
	WCMysql wcMysql, *pwcMysql, *pMy;
	MYSQL_RES *pmyRes;
	char cNo[15];
	int nRes;

	// DB객체가 외부에서 연결하면 ManagerDB에 등록해야 ManagerDB 사용이 가능하다.
	nRes = wcMysql.VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8);
	nRes = wcManDB.VAdd(&wcMysql);
	EXPECT_EQ(nRes,0);	// 멤버추가 확인

	// DB객체가 외부에서 연결하면 ManagerDB에 등록해야 ManagerDB 사용이 가능하다. 그리고, 외부에서 new 할당했으므로 반드시 외부에서 delete 해줘야 한다.
	pwcMysql = new WCMysql();
	nRes = pwcMysql->VOpen("localhost","root","a","neo21",WCMysql::E_CHARSET_UTF8);
	nRes = wcManDB.VAdd(pwcMysql);
	EXPECT_EQ(nRes,0);	// 멤버추가 확인

	// ManagerDB의 VOpen 함수를 이용하면 내부에서 알아서 new,delete 해주므로 연결정보만 파라미터로 넘겨주면 된다.
	nRes = wcManDB.VOpen(WCObject::E_OTYPE_MYSQL,5,"localhost","root","a","neo21");
	EXPECT_EQ(nRes,0);	// 멤버오픈,추가 확인

	for (int i=0; i<10; i++)
	{
		// ManagerDB에 등록된 DB연결자 중 하나를 얻는다. 
		pMy = (WCMysql *)wcManDB.GetDB();
		if (pMy)
		{
			sprintf(cNo,"%05d%05d",i,i);
			cNo[10]=0x00;			
		
			// mysql 질의 - 삽입 
			pmyRes = pMy->Query("insert into testinno (name,nick,age,score,address,code,regd,regt,test,extra) values ('김태희','여신일까 아닐까?',31,1451278,'서울시 강남구 청담동 선녀아파트',123456789,'2011-07-06',now(),'%s','이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 이건정말긴글을 입력해도 되나 될꺼야 아마도 된다 하하하 그래 된다고 된다고 하하하')",cNo);
			EXPECT_TRUE(pmyRes!=NULL);	// DB질의 확인

			// mysql 질의 - 수정
			pmyRes = wcMysql.Query("update testinno set name='한예슬',age=28,address='경상남도 남해군 철수네집' where test='%s'",cNo);
			EXPECT_TRUE(pmyRes!=NULL);	// DB질의 확인

			// mysql 질의 - 조회
			pmyRes = wcMysql.Query("select * from testinno where test='%s'",cNo);
			EXPECT_TRUE(pmyRes!=NULL);	// DB질의 확인

			// mysql 질의 - 삭제 
			pmyRes = wcMysql.Query("delete from testinno where test='%s'",cNo);
			EXPECT_TRUE(pmyRes!=NULL);	// DB질의 확인		
		}
	}
	
	// 해제
	WcXDeleteW(pwcMysql);	// 외부할당한 객체는 외부에서 해제해야 함
}

// 단위테스트 (WCThread)
TEST_F(UnitTest_WCThread, UTThread) 
{
	int nRes;
	WCTestThread tTh1("TestThread-01");

	// Thread 실행
	nRes = tTh1.Start();
	EXPECT_EQ(nRes,0);				// 실행 확인

	// Thread 대기
	for (int i=0; i<2; i++)
		sleep(1);

	// Thread 실행결과 확인
	EXPECT_EQ(tTh1.m_nCountIn,100);	// 1초 동안 m_nCountIn 값을 100 증가시킨다. 실행후 값 확인. 
}

// 단위테스트 (WCManagerThread)
TEST_F(UnitTest_WCManagerThread, UTManagerThread) 
{
	WCManagerThread *pMan = new WCManagerThread();
	WCTestThread *pTh1,*pTh2;
	WCTestThread tTh1("WCTes00000"),tTh2("WCTes00001");
	int nRes;

	pTh1 = new WCTestThread("WCTes00002");
	pTh2 = new WCTestThread("WCTes00003");
	
	tTh1.m_nThread = 4;
	tTh2.m_nThread = 4;
	pTh1->m_nThread = 4;
	pTh2->m_nThread = 4;

	// 쓰레드매니저에 추가 
	nRes = pMan->VAdd(&tTh1);
	EXPECT_EQ(nRes,0);				// 멤버 추가 확인

	nRes = pMan->VAdd(&tTh2);
	EXPECT_EQ(nRes,0);				// 멤버 추가 확인

	nRes = pMan->VAdd(pTh1);
	EXPECT_EQ(nRes,0);				// 멤버 추가 확인

	nRes = pMan->VAdd(pTh2);
	EXPECT_EQ(nRes,0);				// 멤버 추가 확인
	
	// 쓰레드매니저를 통해 쓰레드 실행
	nRes = pMan->Start();
	EXPECT_EQ(nRes,0);				// 쓰레드 실행 확인

	// Thread 대기
	for (int i=0; i<3; i++)
		sleep(1);

	// Thread 실행결과 및 쓰레드통신 결과 확인
	EXPECT_EQ(tTh1.m_nCountIn,100);	// 쓰레드 실행 : 1초 동안 m_nCountIn 값을 100 증가시킨다. 실행후 값 확인. 
	EXPECT_EQ(tTh1.m_nCountOut,100);	// 쓰레드 통신 : 1초 동안 m_nCountOut 값을 100 증가시킨다. 실행후 값 확인. 

	// 쓰레드매니저를 통해 쓰레드 종료
	pMan->Stop();

	// 해제
	WcXDeleteW(pTh1);
	WcXDeleteW(pTh2);
	WcXDeleteW(pMan);
}

// 단위테스트 (WCSocket)	// [TEST]
TEST_F(UnitTest_WCSocket, UTSocket) 
{
	
}

// 단위테스트 (UnitTest_WCManagerSocket)	// [TEST]
TEST_F(UnitTest_WCManagerSocket, UTManagerSocket) 
{
	
}

