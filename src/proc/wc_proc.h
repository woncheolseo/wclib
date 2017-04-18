/**
	@class WCProc

	@brief
		프로세스 상위 클래스

	@details
		- 설명
			- 프로세스, 쓰레드 등의 프로세스 상위 클래스 역할
		- 기능
			- 프로세스 추상 클래스 역할

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2010-01-05
	        -# 최초 작성
*/

#ifndef __WCPROC_H__
#define __WCPROC_H__

class WCProc : public WCObject, public WIMember
{
public :
	/**
	@brief 생성자
	*/
	WCProc();

	/**
	@brief 생성자
	@details
		- 
	@param pcName 객체명
	*/
	WCProc(const char* const pcName);

	/**
	@brief 복사생성자
	@details
		- 
	@param Other 복사객체
	*/
	WCProc(const WCProc &Other);

	/**
	@brief 대입연산자
	@details
		- 
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCProc& operator=(const WCProc &Other);

	/**
	@brief 소멸자
	*/
	virtual ~WCProc();

public:
	/** 
	@brief 프로세스/쓰레드 실행
	@details
		- 상속받는 자식클래스들은 이 함수를 구현해서 프로세스/쓰레드를 실행한다.
	@return void
	*/
 	virtual void Run()=0;

	/** 
	@brief 프로세스/쓰레드 종료 함수
	@details
		 - 상속받는 자식클래스들은 이 함수를 구현해서 프로세스/쓰레드를 자연스럽게 종료시킨다.
	@return void
	*/
	virtual void Stop()=0;
};

#endif
