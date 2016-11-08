/**
	@file wc_cryptograph.cpp
   	@brief WCCryptograph 소스
*/

#include "wc_lib.h"

WCCryptograph::WCCryptograph()
{
}

WCCryptograph::WCCryptograph(const WCCryptograph &Other) : WCUtil(Other)
{
}

WCCryptograph& WCCryptograph::operator=(const WCCryptograph &Other)
{
	if (this != &Other) {
		 WCUtil::operator =(Other);
	}
	return *this;
}

WCCryptograph::~WCCryptograph()
{
}
