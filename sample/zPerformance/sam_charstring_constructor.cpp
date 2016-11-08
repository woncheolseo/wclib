/************************************************************************************************************************************
* char / string 속도 테스트 
*
* 객체 멤버로 char, string 문자열 할당 테스트임. string이 객체이므로 생성자 호출 비용이 발생하나 싶어서 
* 
* 0. 할당
* 1. memcpy
* 2. string
*************************************************************************************************************************************/

/************************************************************************************************************************************

횟수		char (초/상대속도)		string (초/상대속도)	
1 			0.000001916 	0.23 	0.000008445 	1.00 
10 			0.000002499 	0.24 	0.000010382 	1.00 
100 		0.000008586 	0.26 	0.000032542 	1.00 
1,000 		0.000072514 	0.28 	0.000256515 	1.00 
10,000 		0.000714051 	0.28 	0.002522444 	1.00 
100,000 	0.007119493 	0.29 	0.024509637 	1.00 
1,000,000 	0.071352472 	0.29 	0.244635061 	1.00 
10,000,000 	0.707776592 	0.29 	2.445197856 	1.00 
100,000,000 6.890589453 	0.28 	24.451895067 	1.00 

< 결론 >
1. string 생성자 호출(?)로 인해 char가 상대적으로 3~4배 정도 더 빠르다

************************************************************************************************************************************/


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>

#include <wc_lib.h>


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
	string strName = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i=0; i<2; ++i)
	{
		sprintf(cTitle, "%02d",i);
		cTitle[sizeof(cTitle)-1] = 0x00;

		// 시작시간
		print_nowtime(cTitle, &g_tspecS);
	
		for (int k=0; k<nCount; ++k)
		{			
			if (i == 0) {
				WCObject wcObj1("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
				//if (k == 0) cout << "Print : " << wcObj1.GetName() << " - " << wcObj1.GetNameStr() << endl;
			}
			else if (i == 1) {
				WCObject wcObj2(strName);	// WCObject 클래스에서 멤버변수로 string을 임시로 추가하고 오직 string만 할당했음. 실제 WCObject에서는 string을 c_str()로 변환시키지만.
				//if (k == 0) cout << "Print : " << wcObj2.GetName() << " - " << wcObj2.GetNameStr() << endl;
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

