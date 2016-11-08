/**
	@file wc_process.cpp
   	@brief WCProcess 소스
*/

#include "wc_lib.h"

WCProcess::WCProcess()
{
}

WCProcess::WCProcess(const char *pcName) : WCObject(pcName)
{
}

WCProcess::WCProcess(const WCProcess &Other) : WCObject(Other)
{
}

WCProcess& WCProcess::operator=(const WCProcess &Other)
{
	if (this != &Other) {
		 WCObject::operator =(Other);
	}
	return *this;
}

WCProcess::~WCProcess()
{
}
