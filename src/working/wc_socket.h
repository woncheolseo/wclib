/**
	@class WCSocket

	@brief
		소켓 통신 클래스

	@details
		- 설명
			- 소켓을 이용하여 통신한다.
		- 기능
			- 소켓 통신: TCP, UDP 통신

	@warning
		- 복사생성자, 대입연산자 : 금지

	@bug
		-# Rewrite 함수를 Write 함수에서 호출하는데, 근데 다음번에 보낼것이 없으면 Write 함수로 안 들어가니간 Rewrite를 호출할수가 없네. 수정해야 할듯

	@todo
		-# PacketRef() 함수 재작성
		-# 버퍼최대 크기를 설정
		-# 소켓 Close를 완벽하게 하기위해 이렇게 하라던데... 별 소용없는듯. 나중에 연구
		-# 수신버퍼를 외부버퍼로 설정: 수신버퍼가 현재 내부버퍼이나 외부 버퍼와 연결가능하게끔 설정
		-# 단위테스트 작성
		-# UDP는 코딩 안 했음. 시간이 없어서. 나중에 함.

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2015-10-16
	        -# 최초 작성

	@example sam_socket.cpp
*/

#ifndef __WC_SOCKET_H__
#define __WC_SOCKET_H__

class WCSocket : public WCIpc
{
public:
	/**
	@enum E_MODE
	@brief 소켓 모드
	@details 
		- 서버소켓, 클라이언트 소켓 여부, 서버의 수신대기 소켓은 별도로 뺀다.
	*/	
	enum E_MODE
	{
		E_MODE_SERVER,								///< 서버
		E_MODE_ACCEPT,								///< 서버수신대기
		E_MODE_CLIENT								///< 클라이언트
	};

	/**
	@enum E_PACKET
	@brief 패킷 타입
	@details 
		-
	*/	
	enum E_PACKET
	{
		E_PACKET_BINARY,							///< 패킷 바이너리
		E_PACKET_ASCII								///< 패킷 아스키
	};

	static const int MAX_RECV_SIZE=1000000;			///< 수신데이터최대크기

public:
	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화1(InitMember), 멤버초기화2(InitSocket), 멤버초기화3(InitPacket) 호출
	*/
	WCSocket();

	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화1(InitMember), 멤버초기화2(InitSocket), 멤버초기화3(InitPacket) 호출
	@param pcIp 주소
	@param usPort 포트
	@param eMode 소켓 모드
	@param usDomain 소켓 도메인
	@param nType 소켓 타입
	@param nProtocol 프로토콜 타입
	@param nFd 소켓FD	
	*/
	WCSocket(const char *pcIp, const unsigned short usPort, const E_MODE eMode=E_MODE_CLIENT, const unsigned short usDomain=AF_INET, const int nType=SOCK_STREAM, const int nProtocol=0, const int nFd=-1);

	/**
	@brief 소멸자
	@details
		- 소켓 종료
		- // 타이머 삭제, 메모리 해제
	*/
	virtual ~WCSocket();

public:
	/**
	@brief 소켓 열기
	@details
		- 가상함수 구현때문에 인자없는 Open()함수도 정의부가 있어야 함.
	@return 성공: 양수 - 소켓FD
	@return 실패: WC_NOK
	*/
	virtual int Open();

	/**
	@brief 소켓 열기
	@details
		-
	@param pcIp 주소
	@param usPort 포트
	@param eMode 소켓 모드
	@param usDomain 소켓 도메인 - AF_INET, PF_INET
	@param nType 소켓 타입 - SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, SOCK_RDM, SOCK_PACKET
	@param nProtocol 프로토콜 타입 - TCP, ICMP 등을 구분
	@return 성공: 양수 - 소켓FD
	@return 실패: WC_NOK
	*/
	virtual int Open(const char *pcIp, const unsigned short usPort, const E_MODE eMode=E_MODE_CLIENT, const unsigned short usDomain=AF_INET, const int nType=SOCK_STREAM, const int nProtocol=0);

	/**
	@brief 소켓 닫기
	@details
		-  
	@return 성공: 0
	@return 실패: WC_NOK
	*/
	virtual int Close();

	/**
	@brief 소켓 연결 끊기
	@details
		-  
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int ShutDown();

	/*
	@brief 소켓 연결 허용
	@details
		- 소켓 객체를 생성한후 리턴한다.
	@return 성공: 소켓객체 포인터
	@return 실패: NULL
	*
	WCSocket* Accept();
	*/

	/*
	@brief 소켓 연결 허용
	@details
		- 소켓 객체는 생성하지 않고 FD만 리턴한다.
	@param pcAddress - 주소버퍼
	@param nSize - 주소버퍼 길이
	@param pusPort - 포트변수 주소
	@return 성공: Fd
	@return 실패: -1
	*/
	int Accept(char *pcAddress=NULL, int nSize=0, unsigned short *pusPort=NULL);

	/**
	@brief 서버 연결 시도
	@details
		- 멤버변수에 설정된 IP, PORT로 연결 시도
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int Connect();

	/**
	@brief 서버 연결 시도
	@details
		- 
	@param pcIp 연결할 서버 주소
	@param usPort 연결할 서버 포트
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int Connect(const char *pcIp, const unsigned short usPort);

	/**
	@brief 데이터 수신
	@details
		- // 수신버퍼 크기보다 패킷이 크면 수신버퍼를 늘린다.
	@return 성공: 양수 - 수신크기
	@return 실패: WC_OK - 연결 종료, WC_NOK - 실패
	*/
	virtual ssize_t Read();

	/**
	@brief 데이터 송신
	@details
		- 
	@return 성공: 양수 - 송신크기
	@return 실패: 0-연결종료, WC_NOK-실패
	*/
	virtual ssize_t Write();

	/**
	@brief 데이터 송신
	@details
		- 패킷을 송신한다.
	@note 다음 순서를 따른다 : 못보낸 패킷 재송신 -> 본패킷 송신 -> 송신이 실패하면 재전송큐에 패킷을 추가해서 다음번 Write()호출시 보낸다.
	@param pcWriteBuf 송신 데이터
	@param nWriteSz 송신 크기
	@param pcDestIp 목적지 주소. UDP송신은 목적지 주소가 반드시 있어야 함.
	@param usDestPort 목적지 포트. UDP송신은 목적지 포트가 반드시 있어야 함.
	@return 성공: 양수 - 송신크기
	@return 실패: WC_NOK
	*/
	virtual ssize_t Write(const char *pcWriteBuf, const int nWriteSz, const char *pcDestIp=NULL, const unsigned short usDestPort=0);

	/*[test]
	* @brief 재전송
	* 
	* 재송신 큐에서 패킷을 전송한다.
	* @return 성공: 0(송신할거없음), 0< (송신크기)
	* @return 실패: -2(재송신횟수내실패), -1(완전실패)
	*
	virtual ssize_t WriteRe();
	*/

	/**
	@brief 패킷 추출
	@details
		- 수신버퍼에서 패킷을 추출한다. 추출한 패킷은 pcData 버퍼에 복사한다.
	@param pcData 패킷 복사할 버퍼
	@param nHeadIx 패킷에서 패킷 사이즈가 기록된 위치
	@param nszLen 패킷크기를 나타내는 변수의 크기 - short 2바이트, int 4바이트
	@param nAddSz 패킷 추가 크기 - 패킷 사이즈 + 나머지
	@param ePacket 패키크기변수 타입
	@return 성공 : 양수 - 패킷 크기
	@return 성공 : 0 - 패킷 미완성
	@return 실패 : WC_NOK
	*/
	int Packet(char *pcData, const int nHeadIx=0, const int nszLen=0, const int nAddSz=0, E_PACKET ePacket=E_PACKET_BINARY);

	/*
	@brief 패킷 추출
	@details
		- 수신버퍼에서 패킷을 추출한다. 단, 버퍼에 복사하지 않고 수신버퍼의 포인터만 돌려준다.
		- 수신버퍼에서 따로 복사를 하지 않기 때문에 추출이 끝나면 수신/처리인덱스 및 데이터크기는 항상 초기화된다.
	@param pnRecvSz 패킷 크기 - 양수: 패킷 크기, 0 : 패킷 미완성, WC_NOK - 에러
	@param nHeadIx 패킷에서 패킷 사이즈가 기록된 위치
	@param nszLen 패킷크기를 나타내는 변수의 크기 - short 2바이트, int 4바이트
	@param nAddSz 패킷 추가 크기 - 패킷 사이즈 + 나머지
	@param ePacket 패키크기변수 타입
	@return 성공 : 수신버퍼 포인터
	@return 성공 : NULL
	*
	char * PacketRef(int *pnRecvSz, const int nHeadIx=0, const int nszLen=0, const int nAddSz=0, E_PACKET ePacket=E_PACKET_BINARY);
	*/

	/**
	@brief 패킷 추출
	@details
		- 수신버퍼에서 패킷을 추출한다. 추출한 패킷은 pcData 버퍼에 복사한다.
	@param pcData 패킷 복사할 버퍼
	@return 성공 : 양수 - 패킷 크기
	@return 성공 : 0 - 패킷 미완성
	@return 실패 : WC_NOK
	*/
	int PacketJSON(char *pcData);

	/**
	@brief 패킷 처리 가상함수
	@details
		- 파싱을 사용자가 구현하고 싶을 때 
	@param pcData 패킷 버퍼
	@param nPackSz 패킷 크기
	@return 성공 : 양수 - 패킷 크기
	@return 성공 : 0 - 패킷 미완성
	@return 실패 : WC_NOK
	*/
	virtual int PacketVirtual(char *pcData, const int nPackSz) { 
		return 0; 
	}

	/**
	@brief 소켓핸들 반환
	@details
		-
	@return 성공: 양수 - FD
	@return 실패: WC_NOK
	*/
	int GetFd() { 
		return m_nFd; 
	}

	/**
	@brief 주소 읽기
	@details
		0
	@return 성공: 주소 배열 포인터
	@return 실패: 없음
	*/
	const char *GetIp() { 
		return m_cIp; 
	}

	/*
	@brief 포트 읽기
	@details
		 - 
	@return 성공: 포트
	@return 실패: 없음 
	*/
	unsigned short GetPort() { 
		return m_usPort; 
	}

	/**
	@brief 소켓모드 반환
	@details
		-
	@return 성공: 소켓모드
	@return 실패: 없음
	*/
 	E_MODE GetMode() {
 		return m_eMode; 
 	}

 	/**
	@brief 소켓모드 설정
	@details
		- 
	@param eMode 소켓모드
	@return void
	*/
 	void SetMode(const E_MODE eMode) { 
 		m_eMode = eMode; 
 	}

	/**
	@brief 소켓타입 반환
	@details
		- 
	@return 성공: 소켓타입
	@return 실패: 없다
	*/
	int GetType() { 
		return m_nType; 
	}

	/**
	@brief 소켓타입 설정
	@details
		-
	@param nType 소켓타입
	@return void
	*/
 	void SetType(const int nType) { 
 		m_nType = nType; 
 	};

protected:
	/**
	@brief 멤버변수(소켓) 초기화
	@details
		- 멤버 변수 중 소켓 변수만 초기화
	@param pcIp 주소
	@param usPort 포트
	@param eMode 소켓 모드
	@param usDomain 소켓 도메인
	@param nType 소켓 타입
	@param nProtocol 프로토콜 타입
	@param nFd 소켓FD	
	*/
	void InitSocket(const char *pcIp, const unsigned short usPort, const E_MODE eMode=E_MODE_CLIENT, const unsigned short usDomain=AF_INET, const int nType=SOCK_STREAM, const int nProtocol=0, const int nFd=-1);

	/**
	@brief 멤버변수(패킷) 초기화
	@details
		- 멤버 변수 중 패킷 관련 변수(수신버퍼,수신인덱스 등)만 초기화
	*/
	void InitPacket();

private:
	/**
	@brief 복사생성자
	@details
		- private 선언은 복사생성자 금지
	@param Other 복사객체
	*/
	WCSocket(const WCSocket &Other);

	/**
	@brief 대입연산자
	@details
		- private 선언은 대입연산자 금지
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCSocket& operator=(const WCSocket &Other);

	/**
	@brief 멤버변수 초기화
	@details
		- 
	@return void
	*/
	void InitMember();	

protected:
	/*[test]
	/// 재전송 패킷
	typedef struct _REPACK
	{
		int nFailCnt;						///< 송신실패수
		int nWriteSz;						///< 송신크기
		char *pcData;						///< 송신데이터
		char cDestIp[15];					///< 송신주소
		unsigned short usDestPort;			///< 송신포트
	} REPACK;
	*/

	int m_nFd;								///< 소켓번호 
	E_MODE m_eMode;							///< 소켓모드 - E_MODE_SERVER, E_MODE_CLIENT, E_MODE_ACCEPT
	unsigned short m_usDomain;				///< 도메인 - AF_INET, PF_INET
	int m_nType;							///< 소켓타입 - SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, SOCK_RDM, SOCK_PACKET
	int m_nProtocol;						///< 0, IPPROTO_ICMP
	char m_cIp[20];							///< 주소
	unsigned short m_usPort;				///< 포트

	bool m_bConnect;						///< 연결 여부

	int m_nRecvIx;							///< 수신인덱스
	int m_nProcIx;							///< 처리인덱스
	int m_nDataCnt;							///< 데이터갯수
	int m_nRecvBuf;							///< 수신버퍼크기
	char *m_pcRecvBuf; 						///< 수신버퍼
	pthread_mutex_t m_mutexRecvBuf;			///< 수신버퍼 변경 동기화

	/*[test]	
	queue<REPACK *> m_queuePack;			///< 재전송큐
	pthread_mutex_t m_mutexRepack;			///< REPACK 동기화
	*/
};

#endif
