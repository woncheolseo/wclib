/**	
	@class WITimer

	@brief
		WCTimer 인터페이스

	@details
		- 설명
			- WCTimer 클래스의 타이머 이벤트 객체로 사용하기 위한 인터페이스
		- 기능
			- WCTimer 타이머 이벤트 인터페이스 

	@author	
		- 서원철, neo20@hanmail.net

	@date
		- 2015-10-14
			-# 최초 작성
*/

#ifndef __WI_TIMER_H__
#define __WI_TIMER_H__

class WITimer : public WIObject
{
public:	
	/**
	@brief 타이머 이벤트
	@details
		- 상속받는 자식클래스들이 IF_TimerEvent 함수를 구현하여 타이머가 발생하면 이벤트를 처리한다.
		- 타어머데이터는 포인터만 넘긴다. 따라서 해당 데이터의 유효 여부는 호출하는 개발자가 책임을 져야한다. 타이머는 데이터 포인터만 전달할 뿐이다.
	@param tTimer 타이머핸들
	@param nTimerNO 타이머번호
	@param pData 타이머데이터
	@return void
	*/
	virtual void IF_TimerEvent(const timer_t tTimer, const int nTimerNO, void *pData)=0;
};

#endif
