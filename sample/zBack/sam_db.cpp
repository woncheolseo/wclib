/*
<WCDb 예제>

0. 단위 테스트 
1. 기본 사용법1

Usage : sam_db 모드번호
*/

#include <wc_lib.h>

void Sam_Unittest(int argc, char *argv[]);
void Sam_Basic1();

int main(int argc, char *argv[])
{
	int nMode = -1;

	if (argc > 1) {
		nMode = atoi(argv[1]);
	}

	if (nMode == 0) Sam_Unittest(argc,argv);		// 단위테스트, sam_db 0 (WCDb 단위테스트만 실행시킬 경우, "--gtest_filter=UnitTest_WCDb.*" 인자 추가)
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_db 1

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
	/// DB 열기
	virtual int VOpen(const char *pcIP, const char *pcID, const char *pcPW, const char *pcDB, const int nChar=-1);
	/// DB 닫기
	virtual int VClose();
};

WCTestDb::WCTestDb()
{
}

WCTestDb::~WCTestDb()
{
}

int WCTestDb::VOpen(const char *pcIP, const char *pcID, const char *pcPW, const char *pcDB, const int nChar)
{
	return 0;
}

int WCTestDb::VClose()
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

	nRet = RUN_ALL_TESTS(); // 모든 테스트를 수행한다.
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
		wDB1.SetIP("211.238.138.235");
		wDB1.SetID("admin");
		wDB1.SetPW("arreo123");
		wDB1.SetDB("MyDB");
		if (pDB2) {
			pDB2->SetIP("211.238.138.235.AAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBB");
			pDB2->SetID("adminAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBB");
			pDB2->SetPW("arreo123AAAAAAAAAAAAAABBBBBBBBBBBBBBAAAAAAAAAAAAA");
			pDB2->SetDB("MyDBAAAAAAAAAAAABBBBBBBBBBBBBBBAAAAAAAAAAAAAAA");
		}
		WcLogPrintf(WCLog::E_MODE_ALL,"[DB1] (%s)(%s)(%s)(%s)\n",wDB1.GetIP(),wDB1.GetID(),wDB1.GetPW(),wDB1.GetDB());
		WcLogPrintf(WCLog::E_MODE_ALL,"[DB2] (%s)(%s)(%s)(%s)\n",pDB2->GetIP(),pDB2->GetID(),pDB2->GetPW(),pDB2->GetDB());

		// 복사생성, 대입연산
		WCTestDb wDBS,wDBR=wDB1;
		wDBS = wDB1;

		WcLogPrintf(WCLog::E_MODE_ALL,"[DBR] (%s)(%s)(%s)(%s)\n",wDBR.GetIP(),wDBR.GetID(),wDBR.GetPW(),wDBR.GetDB());
		WcLogPrintf(WCLog::E_MODE_ALL,"[DBS] (%s)(%s)(%s)(%s)\n",wDBS.GetIP(),wDBS.GetID(),wDBS.GetPW(),wDBS.GetDB());

		WcXDeleteW(pDB2);
	}
}
