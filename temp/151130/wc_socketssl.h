/**
	@class WCSocketSSL

	@brief
		SSL 소켓 클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(), gtest(), Sample(), Stress(), valg(), prof()

	@remark
		- SSL 보안이 적용된 소켓 통신

	@warning
		- 복사생성자,대입연산자 : 금지

	@bug

	@todo

	@date
	    - 2011-04-12
	        -# 최초 작성
*/

#ifndef __WCSOCKETSSL_H__
#define __WCSOCKETSSL_H__

class WCSocketSSL : public WCSocket
{
public:
	struct T_VERIDATA
	{
   		int nVerboseMode;
   		int nVerifyDepth;
   		int nAlwaysContinue;
 	};
 	//int mydata_index;
 
public :

	/**
	* @brief 생성자
	*
	* 멤버변수 초기화
	*/	
	WCSocketSSL();

	/**
	* @brief 생성자
	* 
	* @param nSockMode SOCK_모드
	* @param pcIp 주소
	* @param usPort 포트
	* @param usSockDomain 도메인 (AF_INET, PF_INET) 
	* @param nSockType 타입 (SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, SOCK_RDM, SOCK_PACKET)
	* @param nSockProtocol 프로토콜 타입 (TCP, ICMP 등 구분)
	* @param nFd 소켓FD
	*/
	WCSocketSSL(const int nSockMode, const char *pcIp, const unsigned short usPort, const unsigned short usSockDomain=AF_INET, const int nSockType=SOCK_STREAM, const int nSockProtocol=0, const int nFd=-1);

	/**
	* @brief 소멸자
	* 
	* 소켓종료, 타이머 삭제, 메모리 해제
	*/
	virtual ~WCSocketSSL();


	static void InitLibrary();

	int InitObject();

	static int VerifyCallback(int nPreVerifyOk, X509_STORE_CTX *ctx);

	//void  ssl_info_callback(const SSL *s, int where, int ret);

	int LoadCertiKey(char *pcCert, char *pcKey);
	
	int HandShake(int nFd);
	
	virtual int Read();
	
	virtual int Write(const char *pcWriteBuf, const int nWriteSz, const char *pcDestIp=NULL, const unsigned short usDestPort=0);

	virtual int VWriteRe();


private:
	/**
	* @brief 복사생성자
	*
	* private 선언은 복사생성자를 막겠다는 의미 
	* @param Other 복사객체
	*/
	WCSocketSSL(const WCSocketSSL &Other);

	/**
	* @brief 대입연산자
	*
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCSocketSSL& operator=(const WCSocketSSL &Other);

	
private:
	SSL_METHOD *m_pMeth;
	SSL_CTX* m_pCtx;
	SSL* m_pSSL;
	
};

#endif
