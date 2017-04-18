/**
	@file wc_util.cpp
   	@brief WCUtil 소스
*/

#include "../wc_lib.h"

WCUtil::WCUtil()
{
}

WCUtil::WCUtil(const WCUtil &Other) : WCObject(Other)
{
}

WCUtil& WCUtil::operator=(const WCUtil &Other)
{
	if (this != &Other) {
		 WCObject::operator =(Other);
	}
	return *this;
}

WCUtil::~WCUtil()
{
}
