/**
	@class WIObject

	@brief
		최상위 인터페이스

	@details
		- 설명
			- 인터페이스 클래스들의 최상위 클래스
			- C/C++은 인터페이스가 없다. 다중상속을 이용하여 인터페이스처럼 구현한다. 
		- 기능
			- 최상위 인터페이스
			- 객체 체크 - 객체 메모리 존재 여부 확인
	
	@note
		- 인터페이스 클래들의 멤버함수명은 IF_ 로 시작한다.
		- 함수명에 IF_ 를 붙이는 이유는 인터페이스는 다중상속을 해야하는데 같은 이름의 함수는 ambiguous 문제가 발생하기 때문이다.

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2015-10-06
	        -# 최초 작성
*/

#ifndef __WI_OBJECT_H__
#define __WI_OBJECT_H__

class WIObject
{
public:
	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(IF_InitMember) 호출
	*/
	WIObject();

	/**
	@brief 복사생성자
	@details
		- 생성자리스트(INITLIST), 멤버복사(IF_InitCopy) 호출
		- 개인키 생성
	@param Other 복사객체
	*/
	WIObject(const WIObject &Other);

	/**
	@brief 대입연산자
	@details
		- 생성자리스트(INITLIST), 멤버복사(IF_InitCopy) 호출
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WIObject& operator=(const WIObject &Other);

	/**
	@brief 소멸자
	@details
		- 멤버변수 초기화
	*/
	virtual ~WIObject();

	/**
	@brief 객체 체크
	@details
		- 객체가 힙 메모리에 살아있는지 여부를 확인한다. 
		- Windows 계열에는 IsBadCodePtr, IsBadReadPtr, IsBadWritePtr, IsBadStringPtr 함수들이 있다는데 Unix/Linux 계열에는 없음. 구글에서 하라고 하는 방향으로 했는데 잘 안됨
		- try,catch문보다 적은 비용이 든다.
	@return 성공: true, 살아있네 살아있어~
	@return 실패: false
	*/
	bool IF_IsLive() const;

private:
	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
	@return void
	*/
	void IF_InitMember();

	/**
	@brief 멤버변수 복사
	@details
		-
	@param Other 복사객체
	@return void
	*/
	void IF_InitCopy(const WIObject &Other);

private:
	unsigned long long m_unllKeyPri;	///< 개인키 - 객체 유일한 키, 중복 불가
	int m_nLiveKey;						///< 객체살아있나~키
};

#endif
