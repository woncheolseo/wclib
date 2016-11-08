/**	
	@class WCCfg

	@brief
		설정파일 클래스

	@details
		- 설명
			- 설정 파일에서 옵션값을 읽거나 저장한다.
		- 기능
			- 설정파일 읽기
			- 설정옵션 저장
			- 설정옵션 반환

	@warning
		- 제한: 환경파일은 최대 1,000 라인 (MAX_STR_LINE 변수값)
		- 제한: 환경파일의 최대 200 문자열 (MAX_STR_LEN 변수값)
		- 제한: 섹션이나 환경문자열에 공백은 허용하지 않는다.
		@code 
			< 환경파일 규칙 >

			섹션값 = 환경값
			섹션값 : 환경값
			섹션값 := 환경값 
		@endcode

	@todo
		-# 암호화 저장 및 읽기 
		-# 환경파일 규칙에 공백 허용 추가

	@date
	    - 2015-10-29
	        -# 최초 작성

	@example sam_cfg.cpp
*/

#ifndef __WC_CFG_H__
#define __WC_CFG_H__

class WCCfg : public WCUtil
{
public :
	/**
	@brief 생성자
	@details
		- 멤버초기화(InitMember) 호출
	*/
	WCCfg();

	/**
	@brief 생성자
	@details
		- 멤버초기화(InitMember) 호출
		- 파일 오픈
	@param strFile 파일경로
	*/
	WCCfg(const std::string &strFile);

	/**
	@brief 복사생성자
	@details
		- 멤버복사(InitCopy) 호출
	@param Other 복사객체
	*/
	WCCfg(const WCCfg &Other);

	/**
	@brief 대입연산자
	@details
		- 멤버복사(InitCopy) 호출
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCCfg& operator=(const WCCfg &Other);

	/**
	@brief 소멸자
	@details
	*/
	virtual ~WCCfg();

public :
	/**
	@brief 환경파일 열기
	@details
		-  
	@param strFile 환경파일 경로
	@return 성공: WC_OK, 양수 - 환경변수 갯수
	@return 실패: WC_NOK
	*/
	int Open(const string& strFile);

	/**
	@brief 환경변수값 얻기
	@details
		-  
	@param strSection 섹션
	@param strName 변수명
	@param strDefault 디폴트값, 검색되지 않을 경우
	@return 성공: 변수값
	@return 실패: ""
	*/
	string Get(const string& strSection, const string& strName, const string& strDefault="");

	/**
	@brief 환경변수 갯수 얻기
	@details
		-  
	@return 성공: WC_OK, 양수 - 환경변수 갯수
	@return 실패: 없음 
	*/
	size_t Size() const { 
		return m_mapConf.size(); 
	}

	/**
	@brief 환경변수맵 초기화
	@details
		-
	@return void
	*/
	void Clear() { 
		m_mapConf.clear(); 
	}

private:
	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
		- SetTimer() 함수 호출
	@return void
	*/
	void InitMember();

	/**
	@brief 멤버 복사
	@details
		- m_mapConf 데이터를 새로운 m_mapConf에 깊은 복사한다.
	@param Other 복사객체
	@return void
	*/
	void InitCopy(const WCCfg &Other);

	/**
	@brief 환경변수 설정
	@details
		- 환경변수 파일에서 읽어서 환경변수맵에 저장한다.
	@param fs 파일스트림
	@return 성공: WC_OK, 양수 - 설정된 환경변수갯수
	@return 실패: WC_NOK
	*/
	int Set(std::ifstream& fs);

private:
	static const int MAX_STR_LEN = 200;		///< 문자열 최대길이
	static const int MAX_STR_LINE = 1000;	///< 라인 최대수
    map<string,string> m_mapConf;			///< 환경변수맵
};

#endif
