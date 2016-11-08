/**
	@file wc_md5.cpp
   	@brief WCMd5 소스
*/

#include "wc_lib.h"

static char g_cBase64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

WCMd5::WCMd5()
{
}

WCMd5::WCMd5(const WCMd5 &Other) : WCCryptograph(Other)
{
}

WCMd5& WCMd5::operator=(const WCMd5 &Other)
{
	if (this != &Other) {
		WCCryptograph::operator =(Other);
	}
	return *this;
}

WCMd5::~WCMd5() 
{
}

int WCMd5::SGetMd5base64(char *pcSrc, int nSize1, char *pcMd5, int nSize2)
{ 
	if (!pcSrc || !pcMd5) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[MD5:%s:%d] !pcSrc(%p) || !pcMd5(%p)\n",__FUNCTION__,__LINE__,pcSrc,pcMd5);
		return -1;
	}
	else if (nSize1 <= 0 || nSize2 < msc_nDigest*2) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[MD5:%s:%d] nSize1(%d) <= 0 || nSize2(%d) < msc_nDigest*2\n",__FUNCTION__,__LINE__,nSize1,nSize2);
		return -1;
	}

    MD5_CTX md5CTX; 
    unsigned char cDigest[msc_nDigest+2]; 
	unsigned long unTriplet;
	int nIx1,nIx2;
 
 	memset(cDigest,0x00,sizeof(cDigest));

    MD5_Init(&md5CTX); 
    MD5_Update(&md5CTX, pcSrc, nSize1); 
    MD5_Final(cDigest, &md5CTX); 

	for (int i=0; i<msc_nDigest; i+= 3)
	{
		unTriplet = 0;
		unTriplet = ((cDigest[i] & 0xFF) << 16) | ((cDigest[i + 1] & 0xFF) << 8 ) | (cDigest[i + 2] & 0xFF);
		for (int j = 0; j < 4; j++)
		{
			nIx1 = i / 3 * 4 + j;
			nIx2 = (unTriplet >> (6 * (3 - j))) & 0x3F;

			if (i*8 + j*6 > msc_nDigest*8) pcMd5[nIx1] = '=';
			else pcMd5[nIx1] = g_cBase64Table[nIx2];
		}
	}

	if (nSize2 > (msc_nDigest + 2) / 3 * 4)
		pcMd5[(msc_nDigest + 2) / 3 * 4] = '\0';

    return 0; 
}
