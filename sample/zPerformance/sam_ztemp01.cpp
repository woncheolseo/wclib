/************************************************************************************************************************************
* 메모리 할당/해제 속도 테스트 
* 
* 0. new/delete
* 1. boost::pool
* 2. boost::object_pool
* 3. boost::singleton_pool
************************************************************************************************************************************/

/************************************************************************************************************************************

횟수		char (초/상대속도)		string (초/상대속도)	



< 결론 >
1. 

************************************************************************************************************************************/


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/time.h>
#include <sys/wait.h>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/singleton_pool.hpp>

using namespace std;

class CTest
{
public:
	CTest();
	~CTest();
	void SetTemp(char *pcTemp);
	char* GetTemp() { return cTemp; }
private:
	char cTemp[1000000];
};

CTest::CTest()
{
	cTemp[0] = 0x00;
}

CTest::~CTest()
{
	cTemp[0] = 0x00;	
}

void CTest::SetTemp(char *pcTemp)
{
	memcpy(cTemp, pcTemp, 100);
	cTemp[100] = 0x00;
}

struct stSingTag{};
typedef boost::singleton_pool<stSingTag, sizeof(CTest)> btSing;

void print_nowtime(const char *pcTitle);
void test_assign(int nCount);


const long long NANOOS = 1000000000LL;
const int g_nBufSz = 100000000;	


int main(int argc, char *argv[])
{
	int nCount = 0;

	if (argc == 2) {
		nCount = atoi(argv[1]);
		test_assign(nCount);		
	}
	
	return 0;
}

// 시간 출력
void print_nowtime(const char *pcTitle, struct timespec *pTspec)
{
	if (clock_gettime(CLOCK_REALTIME, pTspec) == -1) {
		fprintf(stderr, "ER - clock_gettime\n");
	}
	//fprintf(stdout,"%s - %ld:%ld\n", pcTitle, pTspec->tv_sec, pTspec->tv_nsec);
}

// char, string 할당 테스트
void test_assign(int nCount)
{
	struct timespec g_tspecS, g_tspecE;
	long long g_nDifNano, g_nDifMicro, g_nDifMilli;
	char cTitle[3];
	//string strName = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	boost::pool<> btPool(sizeof(CTest));
	//boost::pool<CTest> btPool;
	boost::object_pool<CTest> btObject;
	//boost::singleton_pool<stSingTag, sizeof(CTest)> btSing;

	for (int i=0; i<4; ++i)
	{
		sprintf(cTitle, "%02d",i);
		cTitle[sizeof(cTitle)-1] = 0x00;

		// 시작시간
		print_nowtime(cTitle, &g_tspecS);
	
		for (int k=0; k<nCount; ++k)
		{			
			if (i == 0) {
				CTest *pcT = new CTest();
				//pcT->SetTemp("abcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcde");
				//cout << pcT->GetTemp() << endl;
				delete pcT;
			}
			else if (i == 1) {				
				CTest *pcT = (CTest *)btPool.malloc();
				//pcT->SetTemp("abcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcde");
				//cout << pcT->GetTemp() << endl;
				btPool.free(pcT);
			}
			else if (i == 2) {
				CTest *pcT = btObject.malloc();
				//CTest *pcT = btObject.construct();				
				//pcT->SetTemp("abcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcde");
				//cout << pcT->GetTemp() << endl;
				btObject.free(pcT);
				//btObject.destory(pcT);
			}			
			else if (i == 3) {
				CTest *pcT = (CTest*)btSing::malloc();
				//pcT->SetTemp("abcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcde");
				//cout << pcT->GetTemp() << endl;
				btSing::free(pcT);
			}
		}

		// 종료시간
		print_nowtime(cTitle, &g_tspecE);
		g_nDifNano = NANOOS * (g_tspecE.tv_sec - g_tspecS.tv_sec) + g_tspecE.tv_nsec - g_tspecS.tv_nsec;	
		g_nDifMicro = g_nDifNano / 1000;
		g_nDifMilli = g_nDifNano / 1000000;

		fprintf(stdout,"%s - [%d] [%lld %lld %lld]\n", cTitle, nCount, g_nDifNano, g_nDifMicro, g_nDifMilli);
	}
}

