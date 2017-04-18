/**
	@file wc_proc.cpp
   	@brief WCProc 소스
*/

#include "../wc_lib.h"

WCProc::WCProc()
{
}

WCProc::WCProc(const char *pcName) : WCObject(pcName)
{
}

WCProc::WCProc(const WCProc &Other) : WCObject(Other)
{
}

WCProc& WCProc::operator=(const WCProc &Other)
{
	if (this != &Other) {
		 WCObject::operator =(Other);
	}
	return *this;
}

WCProc::~WCProc()
{
}
