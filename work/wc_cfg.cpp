/**
	@file wc_cfg.cpp
   	@brief WCCfg 소스
*/

#include "wc_lib.h"

WCCfg::WCCfg()
{
	InitMember();
}

WCCfg::WCCfg(const std::string &strFile)
{
	if (&strFile == nullptr || strFile.empty()) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> strFile was nullptr or empty)", __FILE__, __FUNCTION__, __LINE__);
		return;
	}

	InitMember();
	Open(strFile);
}

WCCfg::WCCfg(const WCCfg &Other) : WCUtil(Other)
{
	InitCopy(Other);
}

WCCfg& WCCfg::operator=(const WCCfg &Other)
{
	if (this != &Other) {
		WCUtil::operator =(Other);
		InitCopy(Other);
	}
	return *this;
}

WCCfg::~WCCfg() 
{
	m_mapConf.clear();
}

inline void WCCfg::InitMember()
{
	m_mapConf.clear();
}

inline void WCCfg::InitCopy(const WCCfg &Other)
{
	m_mapConf.clear();
	m_mapConf = Other.m_mapConf; // 얉은 복사가 아니고 깊은 복사를 한다. 즉 Othre.m_mapConf와 this.m_mapConf는 멤버만 복사하고 다른 객체임
}

int WCCfg::Open(const string& strFile)
{
	if (&strFile == nullptr || strFile.empty()) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> strFile was nullptr or empty.)", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	int nRet = WC_NOK;
	ifstream fs;

	fs.open(strFile, ios_base::in);
    //if (fs.fail()) {
    if (!fs.is_open()) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> is_open (%s)", __FILE__, __FUNCTION__, __LINE__, strFile.c_str());
		nRet = WC_NOK;
	}
	else {
		nRet = Set(fs);
		fs.close();
	}

	return nRet;
}

inline int WCCfg::Set(ifstream& fs) 
{
	if (&fs == nullptr) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> fs was nullptr or empty.)", __FILE__, __FUNCTION__, __LINE__);
		return WC_NOK;
	}

	int nScnt=0, nRcnt=0;
	char cLine[MAX_STR_LEN+1], cSection[MAX_STR_LEN+1];
	char *pcName, *pcValue, *pcSave;
	string strKey;	// 섹션@@이름
	string strValue;// 값
	pair<map<string,string>::iterator,bool> pr;

	// 환경파일 최대 라인까지만 읽음
	for (int i=0; i<MAX_STR_LINE; ++i)
	{
		// getline 호출시 sizeof(cLine) 크기보다 라인이 크면 eof를 체크하지 못함
		// if (fs.eof() == true) {
		// WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_DEBUG, "eof)");
		//	break;
		//}

		cLine[0] = 0x00;
		fs.getline(cLine, sizeof(cLine)-1);

		nRcnt = (int)fs.gcount();
		if (nRcnt <= 0) {
			break;
		}

		if (cLine[0] == '#') {
			// 주석
		}
		else if ((pcName=strchr(cLine,'['))) {
			cSection[0]=0x00;
			if ((pcValue=strchr(pcName,']'))) {
				*pcValue = 0x00;
				memcpy(cSection, ++pcName, sizeof(cSection)-1);
			}
		}
		else {
			pcName = strtok_r(cLine," \t:=\r\n",&pcSave);
			pcValue = strtok_r(NULL," \t:=\r\n",&pcSave);
			if (pcName && pcValue) {
				strKey = cSection;
				strKey += "@@";
				strKey += pcName;
				strValue = pcValue;
				pr = m_mapConf.insert(pair<string, string>(strKey, strValue));
				if (pr.second == false) {
					nScnt = -1;
					WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Function call fails ---> map.insert)", __FILE__, __FUNCTION__, __LINE__);
					break;
				}
				else {
					nScnt++;
					//WC_TRACELF(WCLog::E_LEVEL_ALL,"[CFG:%s] M (Key:%s)(Value:%s)(Cnt:%d)", __FUNCTION__, strKey.c_str(), strValue.c_str(), m_mapConf.size());
				}
			}
		}
	}

	return nScnt;
}

string WCCfg::Get(const string& strSection, const string& strName, const string& strDefault)
{
	if (&strSection == nullptr || &strName == nullptr || &strDefault == nullptr) {
		WCLOG_COUTLNFORMAT(WCLog::E_LEVEL_ERROR, "[%s:%s:%d] Invalid parameter ---> strSection == nullptr || strName == nullptr || strDefault == nullptr)", __FILE__, __FUNCTION__, __LINE__);
		return "";
	}

	string strKey="", strValue="", strReturn;

	strKey = strSection + "@@" + strName;

	map<string, string>::iterator it_map; 
	it_map = m_mapConf.find(strKey);
	if (it_map != m_mapConf.end()) {
		return m_mapConf[strKey];
	}
	else {
		if (strDefault.empty()) {
			strReturn = "";					// 검색되지 않았는데 기본값이 없을 경우, "" 반환
		}
		else {
			strReturn =  strDefault;		// 검색되지 않았는데 기본값이 있을 경우, 기본값 반환
		}
		return 
			strReturn;
	}	
}
