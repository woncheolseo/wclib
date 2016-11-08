/************************************************************************************************************************************
* 문자열 복사 속도 테스트 
* 
* 0. 할당
* 1. strcpy
* 2. strncpy
* 3. memcpy
* 4. std::copy
* 5. string
************************************************************************************************************************************/

/************************************************************************************************************************************

횟수	assign (초/상대속도)			strcpy							strncpy							memcpy							std::copy						string	
1 		0.192896334 	11,839.94 		0.061989165 	3,804.88 		0.061871119 	3,797.64 		0.051076456 	3,135.06 		0.051079007 	3,135.22 		0.000016292 	1.00 
10 		1.635355192 	95,651.59 		0.617466324 	36,115.48 		0.617454295 	36,114.77 		0.507340825 	29,674.26 		0.509299478 	29,788.82 		0.000017097 	1.00 
100 	15.974710736 	897,556.51 		6.196559230 	348,160.42 		6.191352579 	347,867.88 		5.076907657 	285,251.58 		5.084987245 	285,705.54 		0.000017798 	1.00 
1,000 	159.262472736 	5,961,314.30 	61.948116932 	2,318,764.67 	61.899611909 	2,316,949.09 	50.780196092 	1,900,740.98 	50.852691589 	1,903,454.54 	0.000026716 	1.00 
10,000 	1592.640003194 	16,926,232.59 	619.255700307 	6,581,315.30 	619.136123706 	6,580,044.46 	507.396572530 	5,392,500.74 	508.107355211 	5,400,054.79 	0.000094093 	1.00 

< 결론 >
1. assign이 가장 늦다.
2. strcpy, strncpy는 비슷한 속도를 보인다. 
3. memcpy, std::copy는 비슷한 속도를 보인다.
4. memcpy는 strcpy보다 더 빠르다.
5. string이 왜이리 빠르지? 뭔가 잘못된거 같음.

************************************************************************************************************************************/


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>


void print_nowtime(const char *pcTitle);
void test_copy(int nCount);


const long long NANOOS = 1000000000LL;
const int g_nBufSz = 100000000;	
//const char g_cTemp[] = "abcdefghijklmnopqrstuvwxyz123456789ABCDEFGHIJKLMNO";	// 50 bytes
const char g_cTemp[] = "abcdefghijklmnopqrstuvwxyz123456789ABCDEFGHIJKLMNOabcdefghijklmnopqrstuvwxyz123456789ABCDEFGHIJKLMNO";	// 100 bytes
//const char g_cTemp[] = "abcdevwxyz";	// 10 bytes

using namespace std;


int main(int argc, char *argv[])
{
	int nCount = 0;

	if (argc == 2) {
		nCount = atoi(argv[1]);
		test_copy(nCount);		
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

// 문자열 복사 테스트
void test_copy(int nCount)
{
	struct timespec g_tspecS, g_tspecE;
	long long g_nDifNano, g_nDifMicro, g_nDifMilli;
	char cTitle[3];
	string sSrc, sDsc;
	char *pcSrc = new char[g_nBufSz];
	char *pcDsc = new char[g_nBufSz];
	
	//sSrc.resize(g_nBufSz);
	//sDsc.resize(g_nBufSz);
	
	// strcpy는 중간에 널이 들어가면 복사를 멈추므로 초기화해야 한다.
	for (int i=0; i<g_nBufSz-1; ++i) {
		pcSrc[i] = 'a';
		sSrc.append("a");
	}
	pcSrc[g_nBufSz-1] = 'b';
	sSrc.append("b");
	pcDsc[g_nBufSz] = 0x00;
			
	// 복사
	for (int i=0; i<6; ++i)
	{
		sprintf(cTitle, "%02d",i);
		cTitle[sizeof(cTitle)-1] = 0x00;
		sDsc = "";

		// 시작시간
		print_nowtime(cTitle, &g_tspecS);
	
		for (int k=0; k<nCount; ++k)
		{			
			if (i == 0) {
				for (int x=0; x<g_nBufSz; ++x) {
					pcDsc[x] = pcSrc[x];
				}
			}
			else if (i == 1) {
				strcpy(pcDsc, pcSrc);
			}
			else if (i == 2) {
				strncpy(pcDsc, pcSrc, g_nBufSz);
			}
			else if (i == 3) {
				memcpy(pcDsc, pcSrc, g_nBufSz);
			}
			else if (i == 4) {
				std::copy(pcSrc, pcSrc+g_nBufSz, pcDsc);
			}
			else if (i == 5) {
				sDsc = sSrc;
			}
		}
		
		// 종료시간
		print_nowtime(cTitle, &g_tspecE);
		g_nDifNano = NANOOS * (g_tspecE.tv_sec - g_tspecS.tv_sec) + g_tspecE.tv_nsec - g_tspecS.tv_nsec;	
		g_nDifMicro = g_nDifNano / 1000;
		g_nDifMilli = g_nDifNano / 1000000;

		if ((int)sDsc.length() < g_nBufSz) 			
			fprintf(stdout,"%s - [%d] [%lld %lld %lld] [%d %c %c] [%d]\n", cTitle, nCount, g_nDifNano, g_nDifMicro, g_nDifMilli, 
				(int)strlen(pcDsc), pcDsc[100000], pcDsc[g_nBufSz-1], (int)sDsc.length());
		else 
			fprintf(stdout,"%s - [%d] [%lld %lld %lld] [%d %c %c] [%d %c %c]\n", cTitle, nCount, g_nDifNano, g_nDifMicro, g_nDifMilli, 
				(int)strlen(pcDsc), pcDsc[100000], pcDsc[g_nBufSz-1], (int)sDsc.length(), sDsc.at(100000), sDsc.at(g_nBufSz-1));
			
	}

	delete[] pcSrc;
	delete[] pcDsc;	
}

