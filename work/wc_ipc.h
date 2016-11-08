/**	
	@class WCIpc

	@brief
		IPC 상위 클래스

	@details
		- 설명
			- 쓰레드, 프로세스 통신을 위한 IPC 클래스(Socket, Pipe 등)의 상위 클래스
		- 기능
			- IPC 추상 클래스 역할
			- IPC 열기, 닫기, 읽기, 쓰기 : 순수가상함수로 선언

	@warning
		- 복사생성자, 대입연산자 : 금지

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2015-10-16
	        -# 최초 작성
*/

#ifndef __WC_IPC_H__
#define __WC_IPC_H__

class WCIpc : public WCObject, public WIMember
{
public:
	/**
	@brief 생성자
	*/
	WCIpc();

	/**
	@brief 생성자
	@details
		-
	@param strName 객체명
	*/
	WCIpc(const std::string &strName);

	/**
	@brief 소멸자
	*/
	virtual ~WCIpc();

public:
	/**
	@brief IPC 열기
	@details
		-
	@return 성공: 0,양수 - FD
	@return 실패: WC_NOK
	*/
	virtual int Open()=0;

	/**
	@brief IPC 닫기
	@details
		-
	@return 성공: 0,양수
	@return 실패: WC_NOK
	*/
	virtual int Close()=0;

	/**
	@brief IPC 읽기
	@details
		-
	@return 성공: 양수 - 수신 데이터수
	@return 실패: 0-연결종료, WC_NOK-실패
	*/
	virtual ssize_t Read()=0;

	/**
	@brief IPC 쓰기
	@details
		-
	@return 성공: 양수 - 송신 데이터수
	@return 실패: 0-연결종료, WC_NOK-실패
	*/
	virtual ssize_t Write()=0;

private:
	/**
	@brief 복사생성자
	@details
		-
	@param Other 복사객체
	*/
	WCIpc(const WCIpc &Other);

	/**
	@brief 대입연산자
	@details
		-
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCIpc& operator=(const WCIpc &Other);
	
};

#endif
