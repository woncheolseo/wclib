/**
	@class WCGroup

	@brief
		객체관리 상위 클래스

	@details
		- 설명
			- 객체관리 템플릿 클래스의 상위 클래스
		- 기능
			- 객체관리 템플릿 추상 클래스 역할

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2010-01-05
			-# 최초 작성
*/

#ifndef __WC_GROUP_H__
#define __WC_GROUP_H__

class WCGroup : public WCObject
{
public :
	/**
	@brief 생성자
	*/
	WCGroup();

	/**
	@brief 복사생성자
	@details
		-
	@param Other 복사객체
	*/
	WCGroup(const WCGroup &Other);

	/**
	@brief 대입연산자
	@details
		-	
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCGroup& operator=(const WCGroup &Other);

	/**
	@brief 소멸자
	*/
	virtual ~WCGroup();
};

#endif
