/**
	@class WCMysql

	@brief
		MySQL 클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(O), gtest(O), Sample(O), Stress(O), valg(O), prof()

	@remark
		- Mysql DB 연결,끊기
		- Mysql DB 질의 처리

	@warning

	@bug
		-# 여러 프로세스(쓰레드가 아님)가 하나의 연결자를 쓸때 mysql_query 함수에서 블럭되는 경우가 생긴다. 이유는 ?

	@todo

	@date
	    - 2009-03-28
	        -# 최초 작성
	    - 2009-04-26
			-# 메모리 단편화 문제 때문에 로그버퍼를 Prinft()안에서 매번 할당하는 것이 아니라 기본 쿼리버퍼보다 큰 내용이면 메모리 재할당해서 쿼리하기 추가함
			-# 뮤텍스 포인터 추가 : 여러 쓰레드에서 한 테이블에 동시에 접속할때 dead rock 에러가 발생. 그래서 외부 뮤텍스를 이용해서 쿼리 접근시 잠금할 수 있게 수정함. LOCK TABLE로 잠그고 써도 됨.
	    - 2009-05-11
			-# 캐릭터셑 설정하는 루틴 추가 : set names utf8. Query 함수에서 연결끊어져서 실패시 자동으로 재연결하는데 캐릭터셑이 달라져서 글자가 깨짐
	    - 2009-07-29
			-# MYSQL m_Mysql 식별자로 수정. 포인터를 썼는데 valgrind에서 메모리 손실 에러가 발생해서
			-# 복사생성자, 대입연산자 추가
	    - 2010-04-24
	    	-# 최종에러코드 반환 추가 : GetErrno(),GetError() 추가

	@example sam_mysql.cpp
*/

#ifndef __WCMYSQL_H__
#define __WCMYSQL_H__

class WCMysql : public WCDb
{
public :
	/// 캐릭터셑
	enum E_CHARSET
	{
		E_CHARSET_UTF8,		///< utf8
		E_CHARSET_EUCKR,	///< euckr
		E_CHARSET_LATIN1 	///< latin1
	};

public :
	/**
	* @brief 생성자
	*
	* 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	*/
	WCMysql();

	/**
	* @brief 복사생성자
	*
	* 멤버복사(InitCopy) 호출
	* @param Other 복사객체
	*/
	WCMysql(const WCMysql &Other);

	/**
	* @brief 대입연산자
	*
	* 멤버복사(InitCopy) 호출
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCMysql& operator=(const WCMysql &Other);

	/**
	* @brief 소멸자
	*
	* mysql 연결자 해제
	*/
	virtual ~WCMysql();

public :
	/**
	* @brief MySQL 연결
	*
	* @param pcIp DB주소
	* @param pcId DB사용자
	* @param pcPw 사용자암호
	* @param pcDb DB명
	* @param nCharSet 캐릭터셑
	* @return 성공: 0
	* @return 실패: -1
	*/
	virtual int VOpen(const char * const pcIp, const char * const pcId, const char * const pcPw, const char * const pcDb, const int nCharSet=0);

	/**
	* @brief Mysql 연결 끊기
	*
	* @return 의미없음
	*/
	virtual int VClose();

	/**
	* @brief 쿼리하기
	*
	* @param pcFmt 쿼리문
	* @return 성공: MYSQL_RES (select 레코드)
	* @return 성공: 0x00000001 (Update,Insert,Delete 결과)
	* @return 실패: NULL
	*/
	MYSQL_RES * Query(const char *pcFmt, ...);

	/**
	* @brief Row 반환
	*
	* @param pMysqlRes Mysql 레코드
	* @return 성공: mysql_row
	* @return 실패: NULL
	*/
	MYSQL_ROW FetchRow(MYSQL_RES * pMysqlRes);

	/**
	* @brief Row수 반환
	*
	* @param pMysqlRes Mysql 레코드
	* @return 성공: 0>= (레코드수)
	* @return 실패: -1
	*/
	my_ulonglong NumRow(MYSQL_RES * pMysqlRes);
	
	/**
	* @brief Row 해제
	*
	* c-mysql 라이브러리는 내부에서 메모리를 할당한다. 따라서 반드시 해제
	* @param pMysqlRes Mysql 레코드
	*/
	void FreeResult(MYSQL_RES * pMysqlRes);

	/**
	* @brief DB 연결 여부
	*
	* Mysql 서버와 연결 상태를 체크한다.
	* @return 성공: true
	* @return 실패: false
	*/
	bool Ping();

	/**
	* @brief DB 연결 TID
	*
	* Mysql 서버 연결 쓰레드의 ID를 반환한다.
	* @return 성공: 쓰레드 ID
	* @return 실패: 0
	*/
	long unsigned int GetTID();

	/**
	* @brief mysql 에러코드 반환 
	*
	* 에러코드를 반환한다.
	* @return 성공: 에러코드
	* @return 실패: 없음
	*/
	int GetErrno() { return mysql_errno(&m_Mysql); }

	/**
	* @brief mysql 에러문자열 반환 
	*
	* 에러문자열을 반환한다.
	* @return 성공: 에러문자열
	* @return 실패: 없음
	*/
	const char* GetError() { return mysql_error(&m_Mysql); }

private:
	/**
	* @brief 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
	*
	* 연결자 초기화
	*/
	inline void InitMember();
	
	/**
	* @brief 복사생성,대입연산 초기화
	* 
	* 연결자를 공유하지 않고 동일한 정보로 새로운 연결세션을 연다.
	* @param Other 복사객체
	*/
	inline void InitCopy(const WCMysql &Other);

private:
	MYSQL m_Mysql;					///< Mysql식별자
	E_CHARSET m_eCharSet;			///< 캐릭터셑
};

// 단위테스트 : wc_test.h/cpp 파일에 있음

#endif
