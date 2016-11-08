/**
	@class WCManagerThreadTH

	@brief
		Thread 객체 관리자 : WCManagerThread를 상속받았다.

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(), gtest(), Sample(), Stress(), valg(), prof()

	@remark
		- 쓰레드 통신: 내부쓰레드를 통해 뮤텍스, 조건변수를 사용하여 쓰레드 객체간 통신하게 한다.

	@warning
		- 복사생성자,대입연산자 : 금지

	@bug
		-# 버퍼크기에 따라, 송신자가 전혀 delay없이(usleep없이) 쓰면 수신자가 읽을 스위칭이 일어나지 않는데 송신자가 버퍼 크기를 넘어가서 계속 쓴다. 대비책은?

	@todo
		-# 동적버퍼로 변경
		-# 현재 SIGNAL 말고 다른 방법은 없나 ?

	@date
	    - 2009-09-25
	        -# 최초 작성
*/


#ifndef __WCMANAGERTHREADTH_H__
#define __WCMANAGERTHREADTH_H__

class WCManagerThreadTH  : public WCThreadGroup
{
public:
 	/// 생성자
 	WCManagerThreadTH();

 	/**
 	* @brief 생성자
 	*
 	* @param nMax 멤버 최대수
 	*/
 	WCManagerThreadTH(const int nMax);

 	/**
 	* @brief 소멸자
 	*
	* IPC쓰레드를 중지시키고 맵멤버의 쓰레드들 중지, 맵에서 삭제하고 뮤텍스,조건변수를 해제한다. delete는 외부에서 한다.
	*/
 	virtual ~WCManagerThreadTH();

	/**
	* @brief IPC 쓰레드 함수
	* 
	* 1. IPC로 수신된 데이터를 추출해서 헤더별로 처리한다.\n
	* 2. Mutex, 조건변수를 이용해서 동기화를 시킨다.
	*/	
	void ThreadIpc();

	/** 
	* @brief IPC 쓰레드 시작
	* 
	* @return 성공: 0
	* @return 실패: -1
	*/	
	int StartIPC();

	/** 
	* @brief IPC 쓰레드 종료
	* 
	* @return 성공: 0
	* @return 실패: -1
	*/
	int StopIPC();

	/**
	* @brief IPC 쓰기
	* 
	* WCThread 객체를 찾아서 데이터를 쓴다.
	* @param pcData 데이터
	* @param nWriteSz 데이터 크기
	* @param pcObjName 객체 이름
	* @return 성공: 송신 데이터 크기
	* @return 실패: -1
	*/	
	int WriteIpc(const char *pcData, const int nWriteSz, char *pcObjName);

	/**
	* @brief IPC 패킷 추출
	* 
	* IPC버퍼에서 패킷을 추출한다.
	* @param pcData 패킷을 받는 버퍼
	* @param nHeadFirstPos 패킷에서 패킷 사이즈가 기록된 위치
	* @param nLenSz 패킷크기를 나타내는 변수의 크기(short 2바이트, int 4바이트)
	* @param nAddSz 패킷 추가 크기 (패킷 사이즈 + 나머지)
	* @return 성공: 패킷 크기 (>0)
	* @return 성공: 패킷 미완성 (==0) 
	* @return 실패: -1
	*/
	int PacketIpc(char *pcData, const int nHeadFirstPos, const int nLenSz, const int nAddSz);

	/** 
	* @brief 쓰레드ID 설정(초기화)
	*
	* WThDispatch 함수에서 종료시 TID를 0으로 설정해야 소멸자에서 Stop함수안으로 들어가지 않는다.
	* @param nFlag 설정값 (0으로만 하시요)
	*/	
	void SetIPCTid(int nFlag);


private:
	/**
	* @brief 복사생성자
	*
	* @param Other 복사객체
	*/
	WCManagerThreadTH(const WCManagerThreadTH &Other);

	/**
	* @brief 대입연산자
	*
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCManagerThreadTH& operator=(const WCManagerThreadTH &Other);

	/**
	* @brief IPC버퍼 변수 초기화
	*/
	inline void InitValPack();


private:
	static const int m_ncIpcBufSz=30000;					///< IPC버퍼크기, 동적할당하기 전에는 크기를 일단 늘리자
	pthread_t m_pthreadIPC;									///< IPC 쓰레드 변수
	int m_nRecvIx;											///< 수신인덱스
	int m_nProcIx;											///< 처리인덱스
	int m_nDataCnt;											///< 데이터갯수
	char m_cIpcBuf[m_ncIpcBufSz];							///< IPC버퍼
	pthread_mutex_t m_MutexS, m_MutexR, m_MutexD;			///< 뮤텍스 (동기화)
	pthread_cond_t m_CondR, m_CondD;						///< 조건변수 (동기화)
	bool m_bLoop;											///< IPC루프 플래그
};

#endif
