/**
	@class WCOracle

	@brief
		Oracle 클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(O), gtest(), Sample(), Stress(), valg(), prof()

	@remark
		- Oracle DB 연결,끊기
		- Oracle DB 질의 처리

	@warning

	@bug

	@todo
		-# m_mutexOpen 뮤텍스 테스트 : createEnvironment() 함수 호출시 THREADED_MUTEXED 옵션을 줬는데 필요한지 테스트 필요
		-# 쿼리 실패시 재연결 : 쿼리가 실패하면 에러코드에 따라 재연결하고 다시 쿼리를 한다. 현재는 m_pConn가 NULL일때만 자동으로 재연결한다.

	@date
	    - 2009-04-15
	        -# 최초 작성
	    - 2008-05-05
	    	-# 복사생성자, 대입연산자 추가
	    	-# 복사생성,대입연산 초기화 함수인 InitCopy 추가

	@example sam_oracle.cpp
*/

#ifndef __WCORACLE_H__
#define __WCORACLE_H__

class WCOracle : public WCDb
{
public:
	/// 쿼리결과
	struct T_RESULTSET
	{
		Statement *pStmt;		///< 질의 객체
		ResultSet *pResultSet;	///< 쿼리 객체 
	};


public :
	/**
	* @brief 생성자
	*
	* 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	*/
	WCOracle();

	/**
	* @brief 복사생성자
	*
	* 멤버복사(InitCopy) 호출
	* @param Other 복사객체
	*/
	WCOracle(const WCOracle &Other);

	/**
	* @brief 대입연산자
	*
	* 멤버복사(InitCopy) 호출
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCOracle& operator=(const WCOracle &Other);

	/**
	* @brief 소멸자
	*
	* 연결 끊기, 연결자 해제
	*/
	virtual ~WCOracle();

public :
	/**
	* @brief Oracle 연결
	*
	* @param pcIp DB주소
	* @param pcId DB사용자
	* @param pcPw 사용자암호
	* @param pcDb DB명
	* @param nCharSet 캐릭터셑
	* @return 성공: 0
	* @return 실패: -1
	*/
	virtual int VOpen(const char * const pcIp, const char * const pcId, const char * const pcPw, const char * const pcDb=NULL, const int nCharSet=0);

	/**
	* @brief Oracle 연결끊기
	*
	* @return 의미없음
	*/
	virtual int VClose();

	/**
	* @brief Oracle 쿼리
	*
	* Query함수를 호출한 후에 반드시 FreeResult함수를 호출해서 Res를 해제한다. Res에 할당이 되지않고 -1이 리턴되면 Res가 NULL이고 할당되어서 FreeResult에서 해제안한다.
	* @param pRes 결과저장
	* @param pcFmt 쿼리문
	* @return 성공: 0
	* @return 실패: -1
	*/
	int Query(T_RESULTSET *pRes, const char *pcFmt, ...);

	/**
	* @brief 쿼리구조체 해제하기
	*
	* @param pRes 쿼리구조체
	* @return 성공: 0
	* @return 실패: -1
	*/
	int FreeResult(T_RESULTSET *pRes);

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
	inline void InitCopy(const WCOracle &Other);

	/**
	* @brief Oracle 환경 생성
	*
	* @return 성공: 0
	* @return 실패: -1	
	*/
	inline int CreateEnvironment();

	/**
	* @brief Oracle 환경 소멸
	*
	*/
	inline void TerminateEnvironment();

private:
	Environment *m_pEnv;					///< 환경 객체
	Connection *m_pConn;					///< 연결 객체
	//Statement *m_pStmt;					///< 질의 객체
	//pthread_mutex_t m_mutexOpen;			///< 뮤텍스-DB오픈
};

// 단위테스트 : wc_test.h/cpp 파일에 있음

#endif
