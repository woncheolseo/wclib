/*************************************
* 
**************************************/

/*

*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int nLimit=1000,nVal1=3,nVal2=5;
	int nCnt1=0,nCnt2=0,nCntDup=0;
	int nSum1=0,nSum2=0,nSumDup=0;
	int nTemp1,nTemp2;
	
	// 입력
	if (argc > 3) {
		nLimit = atoi(argv[1]);
		nVal1 = atoi(argv[2]);
		nVal2 = atoi(argv[3]);
	}
	
	for (int i=1; i<nLimit; i++)
	{
		nTemp1 = i%3;
		nTemp2 = i%5;
		
		if (nTemp1==0) {
			nCnt1++;
			nSum1 += i;
		}

		if (nTemp2==0) {
			nCnt2++;
			nSum2 += i;
		}

		if (nTemp1==0 && nTemp2==0) {
			nCntDup++;
			nSumDup += i;
		}
	}
	
	fprintf(stderr," %d 보다 작은 자연수 중 \n %d 배수의 갯수는 %d, 합은 %d 이고, %d 배수의 갯수는 %d, 합은 %d 이다.\n 이중 %d과 %d 배수 중복 갯수는 %d, 합은 %d이다.\n 따라서 %d, %d 배수의 모든 합은 %d 이다.\n", 
		nLimit,nVal1,nCnt1,nSum1,nVal2,nCnt2,nSum2,nVal1,nVal2,nCntDup,nSumDup,nVal1,nVal2,nSum1+nSum2-nSumDup);

	return 0;
}
