/**
	@file wc_object.cpp
   	@brief WCObject 소스
*/

#include "wc_lib.h"

#define INITLIST m_unllKeyPri(0), m_unllKeyGrp(0), m_nLiveKey(WC_LIVEKEY)

WCObject::WCObject() : INITLIST
{
	InitMember("");
}

WCObject::WCObject(const char* const pcName) : INITLIST
{
	InitMember(pcName);
}

WCObject::WCObject(const string strName) : INITLIST
{
	InitMember(strName.c_str());
}

WCObject::WCObject(const WCObject &Other) : INITLIST
{
	m_unllKeyPri = WcCreateObjectKey();
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
	m_nLiveKey = 0;
	m_unllKeyPri = m_unllKeyGrp = 0;
}

inline void WCObject::InitMember(const char* const pcName)
{
	if (!pcName) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] 'Invalid parameter ---> !pcName", __FILE__, __FUNCTION__, __LINE__);
		return;
	}

	// 객체키 설정
	m_unllKeyPri = m_unllKeyGrp = WcCreateObjectKey();

	// 객체명 설정 - 객체명이 없으면 키값을 이름으로 등록한다.
	if (strcmp(pcName,"") == 0) {
		int nIx;
		nIx = sprintf(m_cName, "WC_%llu", m_unllKeyPri);
		m_cName[nIx] = 0x00;
 	}
 	else {
		SetName(pcName);
 	}
}

inline void WCObject::InitCopy(const WCObject &Other)
{
	// 멤버 복사
	// m_unllKeyPri = Other.m_unllKeyPri; // 개인키는 복사하지 않는다.
	m_unllKeyGrp = Other.m_unllKeyGrp;
	WcMemcpyChar(m_cName, Other.m_cName, sizeof(m_cName)-1);
}

bool WCObject::IsLive() const
{
	if (m_nLiveKey == WC_LIVEKEY) {
		return true;
	}
	else 
		return false;
}

int WCObject::SetName(const char* const pcName)
{
	if (!pcName) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] 'Invalid parameter ---> !pcName", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	char *pcRet 
		= WcMemcpyChar(m_cName, pcName, sizeof(m_cName)-1);
	if (pcRet) 
		return WC_OK;
	else
		return WC_NOK;
}
