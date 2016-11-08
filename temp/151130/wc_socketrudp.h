/**
	@class WCSocketRUDP

	@brief
		RUDP 소켓 통신 클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(), gtest(), Sample(), Stress(), valg(), prof()

	@remark
		- RUDP 통신: RealibeUDP 패킷 통신

	@warning
		- 복사생성자,대입연산자 : 금지

	@bug
		-# RUDP : 2개이상의 연속프레임이 한패킷인 경우, 계속 들어오면 타이머시그널 최대치로 인해 문제가 발생한다.

	@todo

	@date
	    - 2009-05-12
	        -# 최초 작성
*/

#ifndef __WCSOCKETRUDP_H__
#define __WCSOCKETRUDP_H__

class WCSocketRUDP : public WCSocket
{
public :


private:
	/// RUDP 패킷 관리
	struct T_RUDP_PACK
	{
		char cIp[20];					///< 주소
		unsigned short usPort;			///< 포트
		timer_t tTimer;					///< 타이머
		int nSendCnt;					///< 송신횟수 (-1 이면 수신패킷, >=0 면 송신패킷)
		T_RUDP_BODY tRudp;				///< 패킷
	};

public :

	/**
	* @brief 생성자
	*
	* 멤버변수 초기화
	*/	
	WCSocketRUDP();

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
	WCSocketRUDP(const int nSockMode, const char *pcIp, const unsigned short usPort, const unsigned short usSockDomain=AF_INET, const int nSockType=SOCK_STREAM, const int nSockProtocol=0, const int nFd=-1);

	/**
	* @brief 소멸자
	* 
	* 소켓종료, 타이머 삭제, 메모리 해제
	*/
	virtual ~WCSocketRUDP();

public:
	/**
	* @brief 타이머 함수 - RUDP 패킷 처리
	* 
	* RUDP 리스트를 검사해서 패킷을 재전송하거나 삭제한다.\n
	* 송신측은 3초 대기, 4회 송신후 실패면 삭제\n
	* 수신측은 6초 대기후 삭제\n
	* @param tTimer 타이머번호
	* @param nTimerID 타이머식별
	* @return 성공: 0
	* @return 실패: -1	
	*/	
	virtual void VTimer(const timer_t tTimer, const int nTimerID);

	/**
	* @brief RUDP Signal 송신
	* 
	* RUDP 중 Ack, FinPush, FinAck 등 신호 관련 패킷만 보낸다
	* @param pPack 송신패킷 구조체
	* @param pcIp 송신할 주소
	* @param usPort 송신할 포트
	* @param nSendCnt 송신횟수
	* @param usSeq 패킷번호
	* @param usAck 패킷Ack번호
	* @param ucFlag 헤더값
	* @return 성공: >0 (송신크기)
	* @return 실패: -1
	*/
	int WriteRudpSignal(T_RUDP_PACK *pPack, const char *pcIp, const unsigned short usPort, const int nSendCnt, const unsigned short usSeq, const unsigned usAck, const unsigned char ucFlag);

	/**
	* @brief RUDP Data 송신
	* 
	* @param pcDestIp 주소
	* @param usDestPort 포트
	* @param pcWriteBuf 데이터
	* @param nWriteSz 크기
	* @return 성공: >0 (송신크기)
	* @return 실패: -1
	*/
	int WriteRudpData(const char *pcWriteBuf, const int nWriteSz, const char *pcDestIp, const unsigned short usDestPort);

	/**
	* @brief RUDP 패킷 추출
	* 
	* 수신한 데이터에서 RUDP 패킷을 추출하여 헤더별로 처리한다.
	* @param pcData 추출한 패킷을 받는 버퍼
	* @param nHeadFirstPos 패킷에서 패킷 사이즈가 기록된 위치
	* @param nLenSz 패킷 사이즈 크기 (short 2바이트, int 4바이트)
	* @param nAddSz 패킷 실제 크기 (패킷 사이즈 + 나머지)
	* @return 성공: 패킷 크기 (>0)
	* @return 성공: 패킷 미완성 (==0) 
	* @return 실패: -1
	*/
	int PacketRudp(char *pcData, const int nHeadFirstPos=0, const int nLenSz=0, const int nAddSz=0);

	/**
	* @brief 패킷 처리 가상함수
	* 
	* @param pcData 패킷 버퍼
	* @param nPackSz 패킷 크기
	* @return 성공: 패킷 크기 (>0)
	* @return 실패: 0,-1
	*/
	virtual int VPacketProc(char *pcData, const int nPackSz) { return 0; }

protected:

	/**
	* @brief 멤버변수(기타) 초기화
	* 
	* 멤버 변수 중 기타 변수만 초기화
	*/	
	inline void InitVal();

	/** 
	* @brief RUDP 패킷 - Push 
	* 
	* RUDP Push 헤더 처리
	* @param pRudpH RUDP Header
	* @param pcData RUDP Data
	* @return 성공: 패킷 미완성 (==0) 
	* @return 실패: -1
	*/
	int PacketRudpPush(T_RUDP_HEADER * pRudpH, char *pcData);

	/**
	* @brief RUDP 패킷 - PushFin
	* 
	* RUDP PushFin 헤더 처리
	* @param pRudpH RUDP Header
	* @param pcData RUDP Data
	* @return 성공: 패킷 크기 (>0)
	* @return 실패: -1
	*/
	int PacketRudpPushFin(T_RUDP_HEADER * pRudpH, char *pcData);

	/**
	* @brief RUDP 패킷 - Ack
	* 
	* RUDP Ack 헤더 처리
	* @param pRudpH RUDP Header
	* @return 성공: 패킷 크기 (>0)
	* @return 실패: -1
	*/
	int PacketRudpAck(T_RUDP_HEADER * pRudpH);

	/**
	* @brief RUDP 패킷 - AckFin
	* 
	* RUDP AckFin 헤더 처리
	* @param pRudpH RUDP Header
	* @return 성공: 패킷 크기 (>0)
	* @return 실패: -1
	*/
	int PacketRudpAckFin(T_RUDP_HEADER * pRudpH);

	/**
	* @brief RUDP 패킷 체크
	* 
	* RUDP로 추출한 패킷의 데이터의 유무를 체크한다. \n
	* 현재 presence_proxy는 프로토콜에 데이터 사이즈가 없기 때문에 이 함수를 호출하지 않아도 되지만, 만약 있다면... 그래서 사용자 데이터를 덜 수신했다면... 아래는 좀 복잡해지지...
	* @param pcData 추출한 패킷을 받는 버퍼
	* @param nHeadFirstPos 패킷에서 패킷 사이즈가 기록된 위치
	* @param nLenSz 패킷 사이즈 크기 (short 2바이트, int 4바이트)
	* @param nAddSz 패킷 추가 크기 (패킷 사이즈 + 나머지)
	* @param nDataCnt 수신 데이터 크기
	* @return 성공: 패킷 크기 (>0) 
	* @return 성공: 패킷 미완성 (==0) 
	* @return 실패: -1
	*/
	int PacketRudpUserCheck(char *pcData, const int nHeadFirstPos, const int nLenSz, const int nAddSz,  const int nDataCnt);

	/**
	* @brief checksum 계산
	* 
	* 데이터의 체크섬을 계산해서 돌려준다.
	* @param pcData 체크섬을 계산할 데이터
	* @param nDataSz 데이터 크기
	* @return 성공: checksum값
	*/
	uint32_t RudpChecksum(const char *pcData, const int nDataSz);

private:
	/**
	* @brief 복사생성자
	*
	* private 선언은 복사생성자를 막겠다는 의미 
	* @param Other 복사객체
	*/
	WCSocketRUDP(const WCSocketRUDP &Other);

	/**
	* @brief 대입연산자
	*
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCSocketRUDP& operator=(const WCSocketRUDP &Other);

protected:
	static const double m_dwcAckTimeS=3.0;			///< RUDP 송신측 Ack 대기 시간
	static const double m_dwcAckTimeR=6.0;			///< RUDP 수신측 Ack 대기 시간
	static const int m_ncLimitSendCnt=4;			///< RUDP 송신최대횟수
	list<T_RUDP_PACK *> m_listRudpR;				///< RUDP 수신패킷 리스트
	list<T_RUDP_PACK *> m_listRudpS;				///< RUDP 송신패킷 리스트
	unsigned short m_usRudpAck;						///< 패킷ACK번호
	pthread_mutex_t m_MutexRudp;					///< RUDP패킷 동기화
};

#endif
