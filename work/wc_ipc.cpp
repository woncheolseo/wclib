/**
	@file wc_ipc.cpp
   	@brief WCIpc 소스
*/

#include "wc_lib.h"

WCIpc::WCIpc()
{
}

WCIpc::WCIpc(const std::string &strName) : WCObject(strName)
{
}

WCIpc::WCIpc(const WCIpc &Other) : WCObject(Other)
{
}

WCIpc& WCIpc::operator=(const WCIpc &Other)
{
	if (this != &Other) {
		WCObject::operator =(Other);
	}
	return *this;
}

WCIpc::~WCIpc()
{
}
