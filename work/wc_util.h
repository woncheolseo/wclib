/**
	@class WCUtil

	@brief
		유틸리티 상위 클래스

	@details
		- 설명
			- 로그, 환경파일, 암호, 타이머 등의 유틸리티 클래스들의 상위 클래스 
		- 기능
			- 유틸리티 추상 클래스 역할

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2015-10-07
	        -# 최초 작성
*/

#ifndef __WC_UTIL_H__
#define __WC_UTIL_H__

class WCUtil : public WCObject
{
public :
	/**
	@brief 생성자
	*/
	WCUtil();

	/**
	@brief 복사생성자
	@details
		-
	@param Other 복사객체
	*/
	WCUtil(const WCUtil &Other);

	/**
	@brief 대입연산자
	@details
		-	
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCUtil& operator=(const WCUtil &Other);

	/**
	@brief 소멸자
	*/
	virtual ~WCUtil();
};

#endif
