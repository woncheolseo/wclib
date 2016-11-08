/**	
	@class WCMd5

	@brief
		MD5 해싱 클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(O), gtest(O), Sample(O), Stress(O), valg(O), prof()

	@remark
		- 64스트링 MD5 해싱값 반환

	@warning

	@bug

	@todo

	@date
	    - 2010-03-20
	        -# 최초 작성

	@example sam_md5.cpp
*/

#ifndef __WCMD5_H__
#define __WCMD5_H__

class WCMd5 : public WCCryptograph
{
public :
	/**
	* @brief 생성자
	*/
	WCMd5();

	/**
	* @brief 복사생성자
	*
	* @param Other 복사객체
	*/
	WCMd5(const WCMd5 &Other);

	/**
	* @brief 대입연산자
	*
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCMd5& operator=(const WCMd5 &Other);

	/**
	* @brief 소멸자
	*/
	virtual ~WCMd5();

public :
	/**
	* @brief 64스트링 MD5 해싱
	*
	* @param pcSrc 암호화 대상
	* @param nSize1 암호화 대상 크기
	* @param pcMd5 MD5암호화된 버퍼
	* @param nSize2 MD5암호화된 버퍼 크기
	* @return 성공: 0
	* @return 실패: -1
	*/
	static int SGetMd5base64(char *pcSrc, int nSize1, char *pcMd5, int nSize2);

private:
	static const int msc_nDigest=16;	///< 이동비트
};

#endif
