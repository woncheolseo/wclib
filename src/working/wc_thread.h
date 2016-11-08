/**
	@class WCThread

	@brief
		쓰레드 추상 클래스

	@details
		- 설명
			- 쓰레드 추상 클래스 : 상속받는 객체들은 Run()함수를 구현하여 쓰레드 기능을 구현할 수 있다.
		- 기능
			- 쓰레드 실행 : 상속받은 자식클래스에서 VRun() 함수를 구현하면 그 함수가 쓰레드로 실행된다.

	@warning
		- 복사생성자, 대입연산자 : 금지

	@todo
		-# 쓰레드풀을 프레임으로 구현해서 관리해야 한다.
		-# 한개의 WCThreadGroup만 속하게 되어있는데 여러 그룹에 속할수도 있잖아.

	@author	
		서원철, neo20@hanmail.net

	@date
	    - 2015-10-22
	        -# 최초 작성

	@example sam_thread.cpp
*/

#ifndef __WC_THREAD_H__
#define __WC_THREAD_H__

class WCThread : public WCProcess
{
public:
	/** 
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
		- 클래스 이름은 WCObject 생성자에서 OIID로 설정한다.
	*/
	WCThread();

	/** 
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
		- 클래스 이름은 반드시 설정해야 한다.
	@param pcName 객체 이름
	*/
	WCThread(const char *pcName);

 	/** 
 	@brief 소멸자
 	@details
 		- Thread 그룹에서 멤버 제거하고 쓰레드를 종료한다.
	*/
 	virtual ~WCThread();

public:
	/** 
	@brief 쓰레드 함수
	@details
		- 상속받는 자식클래스들은 이 함수를 구현해서 쓰레드로 코드를 실행한다.
	@return void
	*/
 	virtual void Run()=0;

	/**
	@brief 쓰레드 시작
	@details
		- 사용자 쓰레드 및 IPC쓰레드도 생성해서 실행한다.
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
 	int Start();

	/** 
	@brief 쓰레드 종료 함수
	@details
		- 상속받는 자식클래스들은 이 함수를 구현해서 쓰레드를 자연스럽게 종료시킨다.
	@return void
	*/
	virtual void Stop()=0;

	/**
	@brief IPC 패킷 처리
	@details
		- ThreadGroup 그룹에 등록된 쓰레드간 통신 중 수신데이터가 있으면 발생하는 이벤트 함수 [swc,120216] 순수가상에서 일반가상으로 변경
	@param pData 수신데이터
	@param nPackSz 수신크기
	@return 성공: WC_OK, 양수 - 수신크기
	@return 실패: WC_NOK
	*/
	virtual int RecvEvent(const void *pData, const int nPackSz);	

	/*
	@brief 쓰레드 실행 플래그 반환
	@details
		- 
	@return 성공: 양수 - 쓰레드 실행 플래그 값
	@return 실패: 없음
	*/
	bool GetRunFlag() { 
		return m_bRun; 
	}

	/**
	@brief 쓰레드 실행 플래그 설정
	@details
		- 쓰레드 실행 여부를 설정한다.
	@param bFlag 실행 플래그 - true 실행중, false 미실행
	@return 성공: 0
	@return 실패: 0 이외의 값
	*/
 	void SetRunFlag(bool bFlag);

	/**
	@brief 매니저객체 포인터 설정
	@details
		- 
	@param pManager 매니저객체 포인터
	@return void
	*/
	void SetManager(WCManager<string> *pManager);

	/**
	@brief 쓰레드값 반환
	@details
		- 
	@return 성공: 쓰레드ID
	@return 실패: 의미없음
	*/
 	pthread_t GetThreadID() { 
 		return m_pthread; 
 	}

private:
	/**
	@brief 복사생성자
	@details
		- 복사생성자 금지
	* @param Other 복사객체
	*/
	WCThread(const WCThread &Other);

	/**
	@brief 대입연산자
	@details
		- 대입연산자 금지
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCThread& operator=(const WCThread &Other);

	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
		- 뮤텍스,조건변수 초기화
	@return void
	*/
	void InitMember();

	/**
	@brief 쓰레드종료 시그널 대기
	@details
		- 쓰레드가 종료할때까지 3초 대기, WThDispatch 함수에서 쓰레드가 종료하면 깨워줌 
	 @return void
	*/
	void Wait();

protected:
 	WCManager<string> *m_pManager;							///< 등록된 매니저[test]

private:
 	pthread_t m_pthread;									///< 쓰레드 변수
 	bool m_bRun;											///< 쓰레드 실행 여부 플래그
	bool m_bWait;											///< 종료조건변수 대기 플래그
 	pthread_mutex_t m_mutexWait;							///< 뮤텍스 - 종료동기화)
	pthread_cond_t m_condWait;								///< 조건변수 - 종료동기화
};

#endif
