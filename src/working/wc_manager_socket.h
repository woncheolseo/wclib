/**
	@class WCManagerSocket

	@brief
		Socket 객체 관리자

	@details
		- 설명
			- 소켓 객체 관리 : WCSocket 객체를 멤버로 등록하여 그룹으로 관리한다. 
		- 기능
			- Epoll 생성,대기 : 이벤트 수신을 위한 epoll 을 생성하고 이벤트를 대기한다.
			- WCSocket 객체를 멤버로 관리한다.

	@warning
		- 복사생성자, 대입연산자 : 금지

	@todo
		- 단위테스트 작성

	@author	
		서원철, neo20@hanmail.net

	@date
	    - 2015-10-22
	        -# 최초 작성

	@example sam_manager_socket.cpp
*/

#ifndef __WC_MANAGERSOCKET_H__
#define __WC_MANAGERSOCKET_H__

class WCManagerSocket  : public WCManager<int>
{
public:
	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST) 호출
	*/
	WCManagerSocket();

	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST) 호출
	@param nMax 최대멤버수
	*/
	WCManagerSocket(const int nMax);

	/**
	@brief 소멸자
	@details
		-
	*/
	virtual ~WCManagerSocket();

public:
	/**
	@brief 소켓객체 추가 
	@details
		- 생성된 소켓을 그룹에 추가한다. 소켓이 생성되지 않았으면 생성한다.
	@param pSock 소켓객체
	@return 성공 : WC_OK
	@return 실패 : WC_NOK
	*/
    virtual int Add(WCSocket *pSock);

	/**
	@brief 소켓객체 삭제
	@details
		- 소켓맵에서 소켓객체 삭제
	@param pSock 소켓객체
	@return 성공 : WC_OK
	@return 실패 : WC_NOK
	*/
	virtual int Remove(WCSocket *pSock);

	/**
	@brief 모든 소켓 닫기
	@details
		- epoll에서 제거하고 소켓을 닫는다. 
	@return void
	*/
	void Close();

	/**
	@brief 특정 소켓 닫기
	@details
		- epoll에서 제거하고 소켓을 닫는다.
	@return 성공: WC_OK - 의미없음
	*/
	int Close(int nFd);

	/**
	@brief 소켓 shutdown
	@details
		- 등록된 모든 소켓을 shutdown 한다.
	@return void
	*/
	void ShutDown();

	/**
	@brief 소켓 shutdown
	@details
		- 특정 소켓을 shutdown 한다.
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int ShutDown(int nFd);

	/**
	@brief 소켓객체 찾기
	@details
		-
	@param nFd 객체키
	@return 성공: 소켓
	@return 실패: NULL
	*/
	WCSocket* Find(int nFd);

	/**
	@brief epoll 생성
	@details
		-  
	@return 성공: 양수 - epoll 번호
	@return 실패: WC_NOK
	*/
	int CreateEpoll();

	/**
	@brief epoll 이벤트 대기
	@details
		- 이벤트 대기2, Recv를 함수 밖에서 한다.
	@param pepEvent 이벤트 구조체
	@return 성공: 양수 - 이벤트갯수
	@return 실패: 0, -1
	*/
	int WaitEpoll(epoll_event *pepEvent);

private:
	/**
	@brief 복사생성자
	@details
		- private 선언은 복사생성자를 막겠다는 의미 
	@param Other 복사객체
	*/
	WCManagerSocket(const WCManagerSocket &Other);

	/**
	@brief 대입연산자
	@details
		- 
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCManagerSocket& operator=(const WCManagerSocket &Other);


public:
	static const int MAX_EPOLLEVENT_SIZE = 10;		///< epoll 동시이벤트 최대수 - epoll 등록 최대수가 아님


private:
	int m_nEpoll;									///< epoll FD
	struct epoll_event m_epEventOne;				///< epoll구조
};

#endif
