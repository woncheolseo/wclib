/**	
	@class WCObject

	@brief
		최상위 클래스

	@details
		- 설명
			- 최상위 클래스
		- 기능
			- 클래스 최상위 부모
			- 객체 식별 - 객체키, 객체명, 객체타입 등을 저장하여 객체를 식별한다.
			- 객체 체크 - 객체 메모리 존재 여부 확인

	@note
		- 객체명 
			- char를 사용한다. - char, string 생성 속도 테스트 해보니 string, vectro 생성자 호출로 인해 char가 대체로 빠르다. sam_charstring_constructor 소스 참조
			- 객체명은 중복가능하다.
			- 객체명은 최대 100 byte
		- 객체키
			- 개인키와 그룹키로 나눈다.
			- 개인키는 유일하다. 단, 유일성을 보장하는 루틴은 없고 long long 타입의 마이크로 시간을 이용해서 객체키를 설정하므로 유일성을 보장한다고 가정한다.
			- 그룹키는 중복 가능하다. 복사 또는 대입시 객체를 그룹하기 위해 필요하다. 

	@author	
		- 서원철, neo20@hanmail.net
	
	@date
	    - 2010-01-05
	        - 최초 작성

	@example sam_object.cpp
*/

#ifndef __WC_OBJECT_H__
#define __WC_OBJECT_H__

class WCObject
{
public:
	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	*/
	WCObject();

	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	@param pcName 객체명
	*/
	WCObject(const char* const pcName);

	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	@param strName 객체명
	*/
	WCObject(const string strName);

	/**
	@brief 복사생성자
	@details
		- 생성자리스트(INITLIST), 멤버복사(InitCopy) 호출
		- 개인키 생성
	@param Other 복사객체
	*/
	WCObject(const WCObject &Other);

	/**
	@brief 대입연산자
	@details
		- 멤버복사(InitCopy) 호출
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCObject& operator=(const WCObject &Other);

	/**
	@brief 소멸자
	*/
	virtual ~WCObject();

public:
 	/**
	@brief 개인키 반환
	@details
		-
	@return 성공: 양수
	@return 실패: 없음
	*/
	unsigned long long GetKeyPrivate() const {
		return m_unllKeyPri;
	}

 	/**
	@brief 그룹키 반환
	@details
		- 
	@return 성공: 양수
	@return 실패: 없음
	*/
	unsigned long long GetKeyGroup() const {
		return m_unllKeyGrp;
	}	

	/**
	@brief 객체 체크
	@details
		- 객체가 힙 메모리에 살아있는지 여부를 확인한다. 
		- Windows 계열에는 IsBadCodePtr, IsBadReadPtr, IsBadWritePtr, IsBadStringPtr 함수들이 있다는데 Unix/Linux 계열에는 없음. 구글에서 하라고 하는 방향으로 했는데 잘 안됨
		- try,catch문보다 적은 비용이 든다.
	@return 성공: true
	@return 실패: false
	*/
	bool IsLive() const;

 	/**
	@brief 객체명 설정
	@details
		- 
	@param pcName 객체명
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
 	int SetName(const char* const pcName);

	/**
	@brief 객체명 반환
	@details
		- 
	@return 성공: 객체명
	@return 실패: 없음
	*/
 	const char* GetName() const { 
 		return m_cName; 
 	};

private:
	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
		- 개인키/그룹키 생성
	@param pcName 객체명
	@return void
	*/
	void InitMember(const char* const pcName);

	/**
	@brief 멤버변수 복사
	@details
		- 개인키를 생성하지 않고 그룹키만 복사
	@param Other 복사객체
	@return void
	*/
	void InitCopy(const WCObject &Other);

private:
	unsigned long long m_unllKeyPri;		///< 개인키 - 객체 유일한 키, 중복 불가
	unsigned long long m_unllKeyGrp;		///< 그룹키 - 객체 복사,대입 등으로 파생된 객체를 구별하기 위한 값, 중복 가능
	int m_nLiveKey;							///< 객체살아있나~키
	char m_cName[100];						///< 객체명 : 사용자가 지정, 중복 가능
};

#endif
