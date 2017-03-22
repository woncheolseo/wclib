/**
	@class WCPostgreSQL

	@brief
		postgreSQL 클래스

	@details
		- 설명
			- postgreSQL 클래스 
		- 기능
			- DB 열고 닫기
			- SQL 쿼리 실행 - Select, Insert, Update, Delete

	@warning
		- 복사생성자, 대입연산자 : 금지

	@todo
		-# 단위테스트 작성

	@author	
		서원철, neo20@hanmail.net

	@date
	    - 2015-10-26
	        -# 최초 작성

	@example sam_postgreSQL.cpp
*/

#ifndef __WC_POSTGRESQL_H__
#define __WC_POSTGRESQL_H__

class WCPostgreSQL : public WCDb
{
public :
	/**
	@brief 생성자
	@details
		-생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	*/
	WCPostgreSQL();

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
	WCPostgreSQL(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const E_CHARSET eCharSet=E_CHARSET_UTF8);	

	/**
	@brief 소멸자
	@details
		- DB 연결자 해제
	*/
	virtual ~WCPostgreSQL();

public :
	/**
	@brief postgreSQL 연결
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
	virtual int Open(const string& strIP, const int nPort, const string& strID, const string& strPW, const string& strDB, const WCDb::E_CHARSET eCharSet=WCDb::E_CHARSET_UTF8);

	/**
	@brief postgreSQL 연결 끊기
	@details
		- 
	@return 의미없음
	*/
	virtual int Close();

	/**
	@brief DB 연결 여부 반환
	@details
		-
	@return 성공: true
	@return 실패: false
	*/
	virtual bool IsOpen();	

	/**
	@brief 쿼리하기
	@details
		-
	@param strQuery 쿼리문
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int Query(std::string& strQuery);

	/**
	@brief 쿼리하기
	@details
		-
	@param strQuery 쿼리문
	@param pqResult 결과조회 레코드
	@param bCommit 즉시 커밋 여부
	@param bTransaction 트랜잭션 플래그 - 현재 사용하지 않음. 나중에 재연구해서 사용.
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int Query(std::string &strQuery, pqxx::result &pqResult, bool bCommit=false, bool bTransaction=true);

private:
	/**
	@brief 복사생성자
	@details
		- 생성자리스트(INITLIST), 멤버복사(InitCopy) 호출
	@param Other 복사객체
	*/
	WCPostgreSQL(const WCPostgreSQL &Other);

	/**
	@brief 대입연산자
	@details
		- 멤버복사(InitCopy) 호출
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCPostgreSQL& operator=(const WCPostgreSQL &Other);

	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
		- 연결자 초기화
	@return void
	*/	
	void InitMember();

private:
	pqxx::connection* m_pqConn;			///< postgreSQL 핸들
	pqxx::work *m_pqWork;				///< postgreSQL trnasaction
};

#endif
