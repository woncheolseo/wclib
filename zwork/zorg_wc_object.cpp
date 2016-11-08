/**
	@file wc_object.cpp
   	@brief WCObject 소스
*/

#include "wc_lib.h"

#define INITLIST m_eType(E_OTYPE_OBJECT),m_nKey(74780403)

WCObject::WCObject() : INITLIST
{
	InitMember(NULL);
}

WCObject::WCObject(const char* const pcName) : INITLIST
{
	InitMember(pcName);
}

WCObject::WCObject(const WCObject &Other) : INITLIST
{
	InitCopy(Other);
}

WCObject& WCObject::operator=(const WCObject &Other)
{
	if (this != &Other) {
		InitCopy(Other);
	}
	return *this;
}

WCObject::~WCObject()
{
	m_nKey = 0;
}

inline void WCObject::InitMember(const char* const pcName)
{
	/* 객체키를 시간으로 설정
	if (WcGetTickGount(&m_nKey,GE_TICK_MICRO) == -1) {
		fprintf(stderr,"E00:00:00 [OBJ:%s:%d] WcGetTickGount == -1 (This:%p)(Name:%s)\n",__FUNCTION__,__LINE__,this,m_cName);
		m_nKey = 747804030521;
	}*/

	// 객체명 설정
	if (!pcName) m_cName[0] = 0x00; // sprintf(m_cName,"WC%llu",m_nKey); [DLY] 지정된 객체명이 없으면 객체키를 객체명으로 설정
	else {
		memcpy(m_cName,pcName,sizeof(m_cName)-1); m_cName[sizeof(m_cName)-1] = 0x00;
	}
}

inline void WCObject::InitCopy(const WCObject &Other)
{
	/* 객체키를 시간으로 설정
	if (WcGetTickGount(&m_nKey,GE_TICK_MICRO) == -1) {
		fprintf(stderr,"E00:00:00 [OBJ:%s:%d] WcGetTickGount == -1 (This:%p)(Name:%s)\n",__FUNCTION__,__LINE__,this,m_cName);
		m_nKey = 747804030521;
	}*/

	// 멤버 복사
	m_nKey = Other.m_nKey;
	memcpy(m_cName,Other.m_cName,sizeof(m_cName)-1); m_cName[sizeof(m_cName)-1] = 0x00;
	m_eType = Other.m_eType;
}

void WCObject::SetKey(const unsigned int nKey)
{
	m_nKey = nKey;
}

int WCObject::SetName(const char* const pcName)
{
	if (!pcName) {
		fprintf(stderr,"E00:00:00 [OBJ:%s:%d] !pcName (This:%p)(Name:%s)\n",__FUNCTION__,__LINE__,this,m_cName);
		return -1;
	}
	memcpy(m_cName,pcName,sizeof(m_cName)-1); m_cName[sizeof(m_cName)-1] = 0x00;

	return 0;
}

void WCObject::SetType(const E_OTYPE eType)
{
	m_eType = eType;
}

bool WCObject::IsLive()
{
	if (m_nKey != 0) 
		return true;
	else 
		return false;
}
