/**
	@file wc_db.cpp
	@brief WCDb 소스
*/

#include "../wc_lib.h"

#define INITLIST m_strIP(""), m_nPort(0), m_strID(""), m_strPW(""), m_strDB(""), m_eCharSet(E_CHARSET_UTF8)

WCDb::WCDb() : INITLIST
{
	//InitMember();
}

WCDb::WCDb(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet)
{
	InitMember(strIP, nPort, strID, strPW, strDB, eCharSet);
}

WCDb::WCDb(const WCDb &Other) : WCObject(Other), INITLIST
{
	InitCopy(Other);
}

WCDb& WCDb::operator=(const WCDb &Other)
{
	if (this != &Other) {
		WCObject::operator =(Other);
		InitCopy(Other);
	}
	return *this;
}

WCDb::~WCDb()
{
}

int WCDb::Open()
{
	return
		Open(m_strIP, m_nPort, m_strID, m_strPW, m_strDB, m_eCharSet);
}

inline void WCDb::InitMember(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet)
{
	m_strIP = strIP;
	m_nPort = nPort;
	m_strID = strID;
	m_strPW = strPW;
	m_strDB = strDB;
	m_eCharSet = eCharSet;
}

inline void WCDb::InitCopy(const WCDb &Other)
{
	m_strIP = Other.m_strIP;
	m_nPort = Other.m_nPort;
	m_strID = Other.m_strID;
	m_strPW = Other.m_strPW;
	m_strDB = Other.m_strDB;
	m_eCharSet = Other.m_eCharSet; 
}

void WCDb::Set(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet)
{
	InitMember(strIP, nPort, strID, strPW, strDB, eCharSet);
}
