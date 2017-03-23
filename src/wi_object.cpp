/**
	@file wi_object.cpp
   	@brief WIObject 소스
*/

#include "wc_lib.h"

#define INITLIST m_unllKeyPri(0), m_nLiveKey(WC_LIVEKEY)

// 생성자
WIObject::WIObject() : INITLIST
{
	IF_InitMember();
}

// 복사생성자
WIObject::WIObject(const WIObject &Other) : INITLIST
{
	m_unllKeyPri = WC_CreateObjectKey();	
	IF_InitCopy(Other);
}

// 대입연산자
WIObject& WIObject::operator=(const WIObject &Other)
{
	if (this != &Other) {
		IF_InitCopy(Other);
	}
	return *this;
}

// 소멸자
WIObject::~WIObject()
{
	m_unllKeyPri = 0;
	m_nLiveKey = 0;	// LiveKey 초기화 -> 죽었다~
}

// 멤버변수 초기화 
inline void WIObject::IF_InitMember()
{
	m_unllKeyPri = WC_CreateObjectKey();
}

// 멤버변수 복사
inline void WIObject::IF_InitCopy(const WIObject &Other)
{
	//m_unllKeyPri = Other.m_unllKeyPri;	// 개인키는 복사하지 않는다.
	m_nLiveKey = Other.m_nLiveKey; 
}

// 객체 살이있나 ?
bool WIObject::IF_IsLive() const 
{
	if (m_nLiveKey == WC_LIVEKEY) 
		return true;
	else 
		return false;
}
