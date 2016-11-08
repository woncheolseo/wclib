/*
<WCMd5 예제>

0. 단위 테스트 
1. 기본 사용법1

Usage : sam_md5 모드번호
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

	if (nMode == 0) Sam_Unittest(argc,argv);		// 단위테스트, sam_md5 0 (WCMd5 단위테스트만 실행시킬 경우, "--gtest_filter=UnitTest_WCMd5.*" 인자 추가)
	else if (nMode == 1) Sam_Basic1();				// 기본사용법1, sam_md5 1

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
}


/****************************************************************************************************************************************************************************************************
* 1. 기본 사용법1
*
* MD5 사용법을 보여준다. "1234"를 암호화하면 "gdyb21LQTcIANtvYMT7QVQ=="
*****************************************************************************************************************************************************************************************************/
void Sam_Basic1()
{
	char cPw[100]={"1234"};
	char cMd5[100];

	WCMd5::SGetMd5base64(cPw,(int)strlen(cPw),cMd5,(int)sizeof(cMd5));
	WcLogPrintf(WCLog::E_MODE_ALL,"1 : (Pw:%s)(Md5:%s)\n",cPw,cMd5);

	memcpy(cPw,"niceguy0728139",sizeof(cPw)-1);
	WCMd5::SGetMd5base64(cPw,(int)strlen(cPw),cMd5,(int)sizeof(cMd5));
	WcLogPrintf(WCLog::E_MODE_ALL,"2 : (Pw:%s)(Md5:%s)\n",cPw,cMd5);
}
