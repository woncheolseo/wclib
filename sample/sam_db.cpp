/*
<WCDb 예제>

0. 단위 테스트 
1. 기본 사용법1

Usage : sam_db 모드번호
*/

#include <wc_lib.h>

void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1();

// 메인 함수
int main(int argc, char *argv[])
{
	int nMode = -1;
	char cUsage[] = "@ usage\n\tsam_db ModeNo\n@ option\n\tModeNo : 0-UnitTest, 1-BasicUsage1\
			\n@ Example\n\tsam_db 0\n\tsam_db 1\n";

	//SamInit();

	if (argc > 1) {
		nMode = atoi(argv[1]);
		if (nMode == 0) Sam_Unittest(argc, argv);			// 단위테스트, sam_db 0
		else if (nMode == 1) Sam_Basic1();					// 기본사용법1, sam_db 1
		else fprintf(stderr, cUsage);
	}
	else {
		fprintf(stderr, cUsage);
	}

	return 0;
}


/****************************************************************************************************************************************************************************************************
* 테스트 전역 클래스,함수,변수
*****************************************************************************************************************************************************************************************************/

// WCDb는 순수가상함수가 포함된 추상클래스이므로 반드시 상속받아서 사용, 실제로는 WCDb 객체를 바로 쓸 일은 없다.
class WCTestDb : public WCDb
{
public:
	/// 생성자
	WCTestDb();
	/// 소멸자
	virtual ~WCTestDb();

	virtual int Open();
	/// DB 열기
	virtual int Open(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet=E_CHARSET_UTF8);
	/// DB 닫기
	virtual int Close();

	virtual bool IsOpen();
	int Query(string& strQuery);
};

WCTestDb::WCTestDb()
{
}

WCTestDb::~WCTestDb()
{
}

int WCTestDb::Open()
{
	return 0;
}

int WCTestDb::Open(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet)
{
	return 0;
}

int WCTestDb::Close()
{
	return 0;
}

bool WCTestDb::IsOpen()
{
	return true;
}

int WCTestDb::Query(string& strQuery)
{
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
* WCDb는 추상클래스이므로 직접 사용되지는 않는다. 정보를 저장하는 것만 보여준다.
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	WCTestDb wDB1,*pDB2;

	for (int i=0; i<10; i++)
	{
		pDB2 = new WCTestDb();

		// DB정보를 설정하고 출력한다.
		wDB1.Set("211.238.138.235", 9090, "admin", "admin1973$", "mydb");

		if (pDB2) {
			pDB2->Set("211.238.138.235.AAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBB", 7718, "adminAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBB", "arreo123AAAAAAAAAAAAAABBBBBBBBBBBBBBAAAAAAAAAAAAA$", "MyDBAAAAAAAAAAAABBBBBBBBBBBBBBBAAAAAAAAAAAAAAA");
		}
		WCLOG_WRITEFORMAT(WCLog::E_LEVEL_ALL,"[DB1] (%s)(%s)(%s)(%s)\n",wDB1.GetIP().c_str(),wDB1.GetID().c_str(),wDB1.GetPW().c_str(),wDB1.GetDB().c_str());
		WCLOG_WRITEFORMAT(WCLog::E_LEVEL_ALL,"[DB2] (%s)(%s)(%s)(%s)\n",pDB2->GetIP().c_str(),pDB2->GetID().c_str(),pDB2->GetPW().c_str(),pDB2->GetDB().c_str());

		// 복사생성, 대입연산
		WCTestDb wDBS,wDBR=wDB1;
		wDBS = wDB1;

		WCLOG_WRITEFORMAT(WCLog::E_LEVEL_ALL,"[DBR] (%s)(%s)(%s)(%s)\n",wDBR.GetIP().c_str(),wDBR.GetID().c_str(),wDBR.GetPW().c_str(),wDBR.GetDB().c_str());
		WCLOG_WRITEFORMAT(WCLog::E_LEVEL_ALL,"[DBS] (%s)(%s)(%s)(%s)\n",wDBS.GetIP().c_str(),wDBS.GetID().c_str(),wDBS.GetPW().c_str(),wDBS.GetDB().c_str());

		WC_DELETE_WC(pDB2);
	}
}
