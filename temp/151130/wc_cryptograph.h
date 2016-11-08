/**
	@class WCCryptograph

	@brief
		암호 상위 클래스 : MD5, SSL 등의 암호,복호,해싱 클래스들의 상위 클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(O), gtest(O), Sample(O), Stress(O), valg(O), prof()

	@remark
		- 암호 상위 클래스 역할만 한다.

	@warning

	@bug

	@todo

	@date
	    - 2010-03-17
	        -# 최초 작성
*/

#ifndef __WCCRYPTOGRAPH_H__
#define __WCCRYPTOGRAPH_H__

class WCCryptograph : public WCUtil
{
public :
	/**
	* @brief 생성자
	*/
	WCCryptograph();

	/**
	* @brief 복사생성자
	*
	* @param Other 복사객체
	*/
	WCCryptograph(const WCCryptograph &Other);

	/**
	* @brief 대입연산자
	*
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCCryptograph& operator=(const WCCryptograph &Other);

	/**
	* @brief 소멸자
	*/
	virtual ~WCCryptograph();
};

#endif
