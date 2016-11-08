/**	
	@class WCAes

	@brief
		AES 암호화 클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(O), gtest(O), Sample(O), Stress(O), valg(O), prof()

	@remark
		- AES 암호화

	@warning

	@bug

	@todo

	@date
	    - 2014-06-19
	        -# 최초 작성

	@example sam_aes.cpp
*/

#ifndef __WCAES_H__
#define __WCAES_H__

class WCAes : public WCCryptograph
{
public :
	/**
	* @brief 생성자
	*/
	WCAes();

	/**
	* @brief 복사생성자
	*
	* @param Other 복사객체
	*/
	WCAes(const WCAes &Other);

	/**
	* @brief 대입연산자
	*
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCAes& operator=(const WCAes &Other);

	/**
	* @brief 생성자
	*/
	WCAes(const string& strKey);

	/**
	* @brief 소멸자
	*/
	virtual ~WCAes();

public :

	bool aes_init(const unsigned char *key_data, int key_data_len, unsigned char *salt);

	/**
	* @brief AES 암호화
	*
	* 내부에서 malloc 할당, 리턴받은 포인터는 반드시 해제
	* @param pcPlain 암호화 대상
	* @param nSize 암호화 대상 크기
	* @return 성공:
	* @return 실패:
	*/
	unsigned char * Encrypt(const unsigned char *pcPlain, int *pnSize);

	/**
	* @brief AES 복호화
	*
	* 내부에서 malloc 할당, 리턴받은 포인터는 반드시 해제
	* @param pcCipher 복호화 대상
	* @param nSize 복호화 대상 크기
	* @return 성공:
	* @return 실패:
	*/
	unsigned char * Decrypt(const unsigned char *pcCipher, int *pnSize);

private:
	EVP_CIPHER_CTX m_evpEnc;
	EVP_CIPHER_CTX m_evpDec;
};

#endif
