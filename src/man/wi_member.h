/**	
	@class WIMember

	@brief
		WCManager 인터페이스

	@details
		- 설명
			- WCManager 객체의 컨테이너 멤버(멤버변수가 아님)로 등록되기 위한 인터페이스
		- 기능
			- WCManager 컨테이너 멤버 인터페이스 

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2010-01-05
	        -# 최초 작성
*/

#ifndef __WI_MEMBER_H__
#define __WI_MEMBER_H__

class WIMember : public WIObject
{
public:
	/**
	@brief 생성자
	*/
	WIMember();

	/**
	@brief 소멸자
	*/
	virtual ~WIMember();

	/**
	@brief 내부 삭제 플래그 설정
	@details
		- Manager 내부에서 객체 삭제 여부룰 설정한다.
	@param bDel 삭제여부
	@return void
	*/
	void IF_SetInDel(const bool bDel) {
		m_bInDel = bDel;
	};

	/**
	@brief 내부 삭제 플래그 반환
	@details
		- Manager 내부에서 객체 삭제 여부 플래그를 반환한다.
	@return 성공: true
	@return 실패: false
	*/
	bool IF_GetInDel() const { 
		return m_bInDel; 
	};

private:
	bool m_bInDel;		///< 내부 객체 삭제 여부 - 추가되는 멤버가 Manager 내부에서 삭제되어야 하는지 여부

};

#endif
