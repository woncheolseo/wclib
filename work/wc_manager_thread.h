/**
	@class WCManagerThread

	@brief
		Thread 객체 관리자

	@details
		- 설명
			- Thread 객체 관리: WCThread 객체를 멤버로 등록하여 그룹으로 관리한다.
		- 기능
			- 쓰레드 통신: 등록된 쓰레드 통신 기능을 제공한다.

	@warning
		- 복사생성자, 대입연산자 : 금지
		- WriteIpc 함수 내에서 뮤텍스를 걸면 다음 상황에 데드락 발생함
			- A->WriteIpc 함수안에서 B->WriteIpc를 호출

	@author	
		서원철, neo20@hanmail.net

	@date
		- 2015-10-23
			-# 최초 작성

	@example sam_manager_thread.cpp
*/

#ifndef __WC_MANAGERTHREAD_H__
#define __WC_MANAGERTHREAD_H__

class WCManagerThread  : public WCManager<string>
{
public:
	/**
	@brief 생성자
	*/
 	WCManagerThread();

 	/**
 	@brief 생성자
 	@details
 		- 멤버수 최대값을 설정한다.
	@param nMax
	*/
 	WCManagerThread(const int nMax);

 	/**
 	@brief 소멸자
	*/
 	virtual ~WCManagerThread();

public:
   	/**
   	@brief 쓰레드객체 추가
   	@details
   		- 맵에 Thread 객체 삽입, 키는 쓰레드명
   	@param pThread 쓰레드객체
	@return 성공 : WC_OK
	@return 실패 : WC_NOK
	*/
    virtual int Add(WCThread *pThread);

   	/** 
   	@brief 쓰레드객체 삭제
	@param pThread 쓰레드객체
	@return 성공 : WC_OK
	@return 실패 : WC_NOK
	*/
	virtual int Remove(WCThread *pThread);

	/*
	@brief 쓰레드 시작
	@details
		- 맴버맵에 등록된 쓰레드 객체를 실행시킨다.
	@return 성공:WC_OK
	@return 실패: WCWC_NOKOK
	*/
	int Start();

	/**
	@brief 쓰레드 종료
	@details
		- 맴버맵에 등록된 쓰레드 객체를 VStop 가상함수를 실행시켜서 쓰레드를 종료시킨다.
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int Stop();

	/*
	@brief 쓰레드간 데이터 쓰기 
	@details
		- WCThread 객체를 찾아서 데이터를 처리한다.
	@param pcName 객체 이름
	@param pData 데이터
	@param nWriteSz 데이터 크기
	@return 성공: 양수 - 송신 데이터 크기
	@return 실패: WC_NOK
	*/
	int WriteIpc(const char *pcName, const void *pData, const int nWriteSz);

private:
	/**
	@brief 복사생성자
	@details
		- 
	@param Other 복사객체
	*/
	WCManagerThread(const WCManagerThread &Other);

	/**
	@brief 대입연산자
	@details
		- 
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCManagerThread& operator=(const WCManagerThread &Other);
};

#endif
