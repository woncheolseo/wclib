/**	
	@class WCTimer

	@brief
		타이머 클래스

	@details
		- 설명
			- 실시간 타이머를 발생시킨다.
		- 기능
			- 타이머 시작 및 삭제 - 실시간 시그널 Sigalram을 이용해서 타이머 구현, WITimer 인터페이스를 상속받은 객체만 사용 가능

	@bug
		-# 타이머 누락 발생 : 동시 발생수가 어떤 값을 넘어가면 실시간 시그널 발생 횟수의 제한 때문인지 (정확한 수치 측정은 아직 못함, 약 300개) 시그널이 발생하지 않는 경우가 있다. ulimit, kernel 등을 확인해서 수정해야 함.
		-# 버그라기 보다는 , m_map으로 객체를 관리하는데 좀더 나은 방법은? 객체가 힙에서 delete 되었을때 객체 유효성 체크하는 방법은? 현재는 IsLive 함수 사용 

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2015-10-14
	        -# 최초 작성

	@example sam_timer.cpp
*/

#ifndef __WC_TIMER_H__
#define __WC_TIMER_H__

class WCTimer : public WCUtil
{
public:
	/**
	@struct T_TIMER
	@brief 타이머 정보	
	@details 
		- 타이머 정보에 관련된 데이터 구조체
	*/
	struct T_TIMER
	{
		WITimer *pWiTimer;		///< 타이머인터페이스
		WCTimer *pWcTimer;		///< 타이머객체
		timer_t timer;			///< 타이머핸들	- timer_t 는 void *
		int nTimerNO;			///< 타이머번호
		int nTimerCnt;			///< 타이머횟수
		void *pData;			///< 타이머데이터 
	};

public:
	/**
	@brief 생성자
	@details
		- 멤버초기화(InitMember) 호출
	*/
	WCTimer();

	/**
	@brief 복사생성자
	@details
		- 멤버복사(InitCopy) 호출
	@param Other 복사객체
	*/
	WCTimer(const WCTimer &Other);

	/**
	@brief 대입연산자
	@details
		- 멤버복사(InitCopy) 호출
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCTimer& operator=(const WCTimer &Other);

	/**
	@brief 소멸자
	@details
		- 타이머 삭제, 타이머뮤텍스 해제
	*/
	virtual ~WCTimer();

public:
	/**
	@brief 타이머 시작
	@details
		- 타이머를 실행하고 타이머맵에 등록한다. 타이머객체를 m_mapTimer에 설정해줘야 외부에서 삭제가 가능\n
	@note
		- timer 구조체가 void* 이다. 그런데 처음 timer_create() 호출해서 발생하는 timer값은 0이다. 0이나 nullptr 같다. 따라서 결과값을 따로 받고 타이머값은 포인터로 받자.
	@param pWi 타이머 인터페이스 객체
	@param nTimerNO 타이머번호 - 0보다 크거나 같다.
	@param dSec 대기 시간 - 소수점 초 단위. 0보다 크거나 같다.
	@param nTimerCnt 타이머 발생횟수 - 0:무한, 양수: 횟수만큼
	@param pData 타이머 데이터 - 타이머에서 처리해야 할 데이터 주소값. 데이터 유효성은 호출하는 쪽에서 책임져야 함.
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int StartTimer(WITimer *pWi, const int nTimerNO, const double dSec=60.0, const int nTimerCnt=1, void *pData=NULL);

	/**
	@brief 타이머 삭제
	@details
		- 타이머 중지, 맵멤버 삭제, 맴버객체 삭제 (등록된 모든 타이머 삭제)
	@return void
	*/
	void DeleteTimer();

	/**
	@brief 타이머 삭제
	@details
		- 타이머 중지, 맵멤버 삭제, 맴버객체 삭제
	@note
		- timer값은 0도 있어서 파라미터는 검사하지 않는다. 타이머 발생함수에서 정상적인 경우만 타이머를 동작하게 한다.
	@param timer 타이머핸들
	@return void
	*/
	void DeleteTimer(const timer_t timer);

	/*&
	@brief 타이머 삭제
	@details
		- 타이머 중지, 맵멤버 삭제, 맴버객체 삭제
	@param nTimerNO 타이머번호
	@return void
	*/
	void DeleteTimer(const int nTimerNO); 

	/**
	@brief 타이머 삭제
	@details
		- 타이머 중지, 맵멤버 삭제, 맴버객체 삭제
	@param pWiTimer 타이머객체
	@return void
	*/
	void DeleteTimer(const WITimer* const pWiTimer);

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
		- 타이머맵은 복사하지 않는다. 뮤텍스만 초기화한다.
	@param Other 복사객체
	@return void
	*/
	void InitCopy(const WCTimer &Other);

	/**
	@brief 타이머 시그널 설정
	@details
		- SIGALRM 시그널과 인터럽트 함수를 등록한다. 타이머 기능을 사용하려면 반드시 한번은 호출해야 한다.
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
 	static int SetTimer();

private:
	static bool m_bSetTimer;							///< 타이머 시그널 설정 플래그 - SetTimer() 함수를 한번만 호출하기 위해서
	static boost::object_pool<T_TIMER> m_btpoolTimer;	///< 타이머 객체풀
	map<timer_t, T_TIMER *> m_mapTimer;					///< 타이머 객체맵 - timer 핸들이 키값
	pthread_mutex_t m_mutexTimer;						///< 타이머 객체맵 뮤텍스
};

#endif
