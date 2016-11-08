/**
	@file wc_group.cpp
   	@brief WCGroup 소스
*/

#include "wc_lib.h"

WCGroup::WCGroup()
{
}

WCGroup::WCGroup(const WCGroup &Other) : WCObject(Other)
{
}

WCGroup& WCGroup::operator=(const WCGroup &Other)
{
	if (this != &Other) {
		 WCObject::operator =(Other);
	}
	return *this;
}

WCGroup::~WCGroup()
{
}
