/**
	@class	WCLog

	@brief 
		로그 클래스

	@details
		- 설명
			- 로그 파일을 생성, 저장한다.
		- 기능
			- 로그 저장 
				- 로그 파일을 생성해서 내용을 저장한다.
				- 로그 파일에 저장할 경우 기본적으로 동기화를 지원한다. 동기회를 false 시키면 속도는 조금더 빨라진다.
			- 로그레벨 선택
				- enum E_LEVEL 참조
			- 로그주기 선택
				- enum E_CYCLE 참조
			- 로그 포맷 - [로그레벨-시간] 내용
				- ex) [E-19:15:23] logout
	
	@note
		- OutBuf 할당을 new/delete로 필요한 경우에만 함. boost::pool을 이용하려고 했으나 로그는 직접 제어하는 것이 비용이 덜 발생.

	@todo
		-# 가변인자 받는거 외에 string을 인자로 받아서 출력하는 함수 만들기 
		-# 단독 프로세스 구현 - IPC를 통해 다른 프로세스에서 로그를 남기기 위한 로그 단독 프로세스 구현 (memcached 처럼)

	@author	
		- 서원철, neo20@hanmail.net

	@date	
		- 2012-01-05
			-# 최초 작성

	@example sam_log.cpp
*/

#ifndef __WC_LOG_H__
#define __WC_LOG_H__

class WCLog
{
public:
	/**
	@enum E_LEVEL
	@brief 로그 레벨
	@details 
		- 로그파일 및 로그출력시 설정되는 값으로 출력 여부를 제한한다.
		- 로그파일의 레벨은 레벨값의 bit 연산값을 설정할 수 있다. 
			- E_LEVEL_WARN | E_LEVEL_EROOR -> WARN 로그와 ERROR 로그를 출력할 수 있다.
		- 로그출력시 레벨은 하나만 지정 가능하다.
	*/
	enum E_LEVEL
	{
		E_LEVEL_NORMAL = 1,	///< 일반
		E_LEVEL_DEBUG = 2, 	///< 디버그
		E_LEVEL_WARN = 4, 	///< 경고
		E_LEVEL_ERROR = 8, 	///< 에러
		E_LEVEL_ALL = 16	///< 모두출력
	};

	/**
	@enum E_CYCLE
	@brief 로그 주기
	@details 
		- 로그파일 생성되는 주기를 설정하는 값
	*/
	enum E_CYCLE
	{
		E_CYCLE_ONE,		///< 한번만 파일 생성 - 입력받은 파일명 그대로 파일 생성 ex) test.log
		E_CYCLE_MONTH,		///< 월별 파일 생성 - 입력받은 파일명 앞에 '년월-'을 붙여서 파일 생성  ex) 201510-test.log
		E_CYCLE_DAY			///< 일일 파일 생성 - 입력받은 파일명 앞에 '년월일-'를 붙여서 파일 생성 ex) 20151023-test.log
	};

	static const int MAX_LOG_LENGTH = 100000;	///< 최대 로그 크기 - 아웃버퍼 크기

public:
	/**
	@brief 생성자
	@details 
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
		- 테스트
	*/
 	WCLog();

	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
		- 로그파일 오픈
	@param strFullName 파일전체명
	@param nLevel 로그레벨
	@param eCycle 로그주기
	*/
	WCLog(const string& strFullName, const int nLevel, const E_CYCLE eCycle);

	/**
	@brief 복사생성자
	@details
		- 생성자리스트(INITLIST), 멤버복사(InitCopy) 호출
		- 테스트
	@param Other 복사객체
	*/
	WCLog(const WCLog &Other);

	/**
	@brief 대입연산자
	@details
		- 멤버복사(InitCopy) 호출
	@param Other 대입객체
	@return 성공 : 대입된 객체참조자
	@return 실패 : 없음
	*/
	WCLog& operator=(const WCLog &Other);

	/**
	@brief 소멸자
	@details
		- 파일 닫기
	*/
	virtual ~WCLog();

public:
	/**
	@brief 로그레벨 설정
	@details
		-
	@param nLevel 로그레벨
	@return void
	*/
	void SetLevel(const int nLevel) {
		m_nLevel = nLevel;
	}

	/**
	@brief 로그레벨 반환
	@details
		-
	@return 성공: E_LEVEL
	@return 실패: 없음
	*/
	int GetLevel() { 
		return m_nLevel;
	}

	/**
	@brief 로그주기 설정
	@details
		-
	@param eCycle 로그주기
	@return void
	*/
	void SetCycle(const E_CYCLE eCycle) {
		m_eCycle = eCycle;
	}

	/**
	@brief 로그주기 반환
	@details
		-
	@return 성공: E_CYCLE
	@return 실패: 없음
	*/
	E_CYCLE GetCycle() const { 
		return m_eCycle; 
	}

	/**
	@brief 디렉토리 자동생성값 설정
	@details
		- 파일명에서 디렉토리명을 추출해서 해당 디렉토리가 없으면 자동 생성할지 여부를 설정한다.
	@param bFlag 자동생성 플래그 - true : 자동생성, false : 자동생성하지 않음
	@return void
	*/
	void SetDirCreate(bool bFlag) {
		m_bDirCreate = bFlag;
	}

	/**
	@brief 디렉토리 자동생성값 반환
	@details
		-
	@return 성공 : true
	@return 실패 : false
	*/
	bool GetDirCreate() const {
		return m_bDirCreate;
	}

	/**
	@brief 로그쓰기 동기화 설정
	@details
		-
	@param bFlag 동기화 플래그
	@return void
	*/
	void SetMutex(bool bFlag) {
		m_bMutex = bFlag;
	}

	/**
	@brief 로그쓰기 동기화 반환
	@details
		-
	@return 성공: true
	@return 실패: false
	*/
	bool GetMutex() const {
		return m_bMutex;
	}
	
	/**
	@brief 파일 열기
	@details
		- 
	@param strFullName 파일전체명
	@param nLevel 로그레벨
	@param eCycle 로그주기
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/	
	int Open(const string& strFullName, const int nLevel=E_LEVEL_WARN|E_LEVEL_ERROR, const E_CYCLE eCycle=E_CYCLE_DAY);

	/**
	@brief 파일 오픈 확인
	@details
		-
	@return 성공: true
	@return 실패: false
	*/	
	bool IsOpen()
	{
		return
			m_ofs.is_open();
	}
	
	/**
	@brief 파일 닫기
	@details
		- 로그파일을 닫고 ofstream을 NULL로 초기화한다. 단, 표준입,출력,에러는 닫지 않는다.
	@return 성공: WC_OK
	@return 성공: WC_NOK
	*/
	int Close();

	/**
	@brief 로그 출력 
	@details
		- 로그 레벨에 따라 E(에러)는 표준에러로, 나머지는 표준출력으로 출력한다.
		- 시간을 문장 첫머리에 덧붙여 출력한다.
	@param eLevel 로그레벨 - E_LEVEL 값만 받는다.
	@param strLog 로그데이터
	@return 성공: 0, 양수 - 출력된 문자열 길이. 날짜 등의 길이는 빠짐
	@return 실패: WC_NOK, -2 - 로그레벨 불일치
	*/
	int WriteConsole(const E_LEVEL eLevel, const string& strLog);

	/**
	@brief 로그 출력
	@details
		- 로그 레벨에 따라 E(에러), W(경고), D(디버그), S(정상) 접두어를 붙인다.
		- 시간을 문장 첫머리에 덧붙여 출력한다.
		- string은 format 서식을 지원하지 않는다.
	@param eLevel 로그레벨 - E_LEVEL 값만 받는다.
	@param strLog 로그데이터
	@return 성공: 0, 양수 - 출력된 문자열 길이. 날짜 등의 길이는 빠짐
	@return 실패: WC_NOK, -2 - 로그레벨 불일치
	*/
	int Write(const E_LEVEL eLevel, const string& strLog);

	/**
	@brief 로그 출력
	@details
		- 로그 레벨에 따라 E(에러), W(경고), D(디버그), S(정상) 접두어를 붙인다.
		- 시간을 문장 첫머리에 덧붙여 출력한다.
	@param eLevel 로그레벨 - E_LEVEL 값만 받는다.
	@param pcFmt 출력양식
	@return 성공: 0, 양수 - 출력된 문자열 길이. 날짜 등의 길이는 빠짐
	@return 실패: WC_NOK, -2 - 로그레벨 불일치
	*/
	int WriteFormat(const E_LEVEL eLevel, const char *pcFmt, ...);

private:
	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
	@return void
	*/
	void InitMember();

	/**
	@brief 멤버 복사
	@details
		- 로그파일을 복사공유하지 않는다. cout으로 출력한다.
	@param Other 복사객체
	@return void
	*/
	void InitCopy(const WCLog &Other);

	/** 
	@brief 로그레벨 확인
	@details 
		- 인자값과 설정된 로그레벨을 검사해서 일치 여부를 반환한다.
	@param eLevel 로그레벨
	@return 성공: true
	@return 실패: false
	*/
	bool CheckLevel(const E_LEVEL eLevel);

	/**
	@brief 로그주기 확인
	@details
		- 로그주기를 체크해서 새로운 파일을 만들거나 현재 파일이 존재하는지 체크해서 없으면 만든다.
	@param ptmBuf 날짜시간
	@return 성공: true
	@return 실패: false
	*/
	bool CheckCycle(struct tm* const ptmBuf);

private:
	ofstream m_ofs;								///< 로그파일
	string m_strName;							///< 로프파일이름	
	int m_nLevel;								///< 로그레벨 - 모드값의 OR 비트연산을 위해 E_MODE형이 아닌 Int형
	E_CYCLE m_eCycle;							///< 로그주기
	struct tm m_tmCycle;						///< 로그주기 일시
	bool m_bDirCreate;							///< 디렉토리 자동생성 플래그
	char *m_pcOutBuf;							///< 로그출력버퍼 - format 서식 때문에
	int m_nOutBufSz;							///< 로그출력버퍼 크기
	bool m_bMutex;								///< 로그쓰기 동기화 플래그
	pthread_mutex_t m_mutexLog;					///< 로그파일 뮤텍스

};

#endif
