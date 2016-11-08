/**
`  @file wc_struct.cpp
   @brief WCStruct 소스
*/

#include "wc_lib.h"

WCStruct::WCStruct()
{
}

WCStruct::WCStruct(const WCStruct &Other) : WCObject(Other)
{
}

WCStruct& WCStruct::operator=(const WCStruct &Other)
{
	if (this != &Other) {
		 WCObject::operator =(Other);
	}
	return *this;
}

WCStruct::~WCStruct()
{
}
