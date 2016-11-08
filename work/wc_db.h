/**	
	@class WCDb

	@brief
		DB 상위 클래스

	@details
		- 설명
			- DB클래스(Oracle, Mysql, Mssql, PostgreSQL 등)의 인터페이스 역할을 하며 직접 사용할 수 없다.
		- 기능
			- DB 추상 클래스 역할
			- DB 열기,닫기 : 순수가상함수로 선언. 상속받는 클래스에서 반드시 구현해야 한다.
			- DB 정보 설정 : DB주소,계정,패스워드,DB이름 저장
			- DB 정버 반환

	@todo
		-# Query 함수를 공통 함수로 올리는 방안 연구 : Mysql, Oracle 클래스의 Query 함수가 각각 만들어져 있는데 pcFmt, ... 인자를 넘기는 방법이 없어서임. __VA_ARGS__는 C99 문법에서 적용되는데..

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2015-10-16
	        -# 최초 작성
*/

#ifndef __WC_DB_H__
#define __WC_DB_H__

class WCDb : public WCObject, public WIMember
{
public :
	/**
	@enum E_CHARSET
	@brief 캐릭터셋
	@details 
		- 
	*/
	enum E_CHARSET
	{
		E_CHARSET_UTF8,		///< utf8
		E_CHARSET_EUCKR,	///< euckr
		E_CHARSET_LATIN1 	///< latin1
	};

public :
	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	*/
	WCDb();

	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	@param strIP DB주소
	@param nPort DB포트
	@param strID DB사용자
	@param strPW 사용자암호
	@param strDB DB명
	@param eCharSet 캐릭터셑
	*/
	WCDb(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet=E_CHARSET_UTF8);	

	/**
	@brief 복사생성자
	@details
		- 멤버복사(InitCopy) 호출
	@param Other 복사객체
	*/
	WCDb(const WCDb &Other);

	/**
	@brief 대입연산자
	@details
		- 멤버복사(InitCopy) 호출
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCDb& operator=(const WCDb &Other);

	/**
	@brief 소멸자
	@details
		- DB연결 끊기, 버퍼해제, 뮤텍스해제
	*/
	virtual ~WCDb();

public :
	/**
	@brief DB 연결
	@details
		- 	 
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	virtual int Open();

	/**
	@brief DB 연결
	@details
		- 	 
	@param strIP DB주소
	@param nPort DB포트
	@param strID DB사용자
	@param strPW 사용자암호
	@param strDB DB명
	@param eCharSet 캐릭터셑
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	virtual int Open(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet=E_CHARSET_UTF8)=0;

	/**
	@brief DB 연결 끊기
	@details
		- 
	@return 의미없음
	*/
	virtual int Close()=0;

	/**
	@brief DB 연결 여부 반환
	@details
		-
	@return 성공 : true
	@return 실패 : false
	*/
	virtual bool IsOpen()=0;

	/**
	@brief 쿼리하기
	@details
		-
	@param strQuery 쿼리문
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	virtual int Query(string& strQuery)=0;

	/**
	@brief DB 정보 설정
	@details
		- 	 
	@param strIP DB주소
	@param nPort DB포트
	@param strID DB사용자
	@param strPW 사용자암호
	@param strDB DB명
	@param eCharSet 캐릭터셑
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	void Set(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet=E_CHARSET_UTF8);	

private:
	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
	@param strIP DB주소
	@param nPort DB포트
	@param strID DB사용자
	@param strPW 사용자암호
	@param strDB DB명
	@param eCharSet 캐릭터셑		
	@return void
	*/	
	void InitMember(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet=E_CHARSET_UTF8);

	/**
	@brief 멤버변수 복사
	@details
		- 
	@param Other 복사객체
	@return void
	*/
	void InitCopy(const WCDb &Other);

protected:
	string m_strIP;				///< DB주소
	int m_nPort;				///< DB포트
	string m_strID;				///< DB사용자
	string m_strPW;				///< DB패스워드
	string m_strDB;				///< DB이름
	E_CHARSET m_eCharSet;		///< 캐릭터셑
};

#endif
