/**	
	@class WCObject

	@brief
		최상위 클래스로서 모든 클래스들의 부모이다.

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(O), gtest(O), Sample(O), Stress(O), valg(O), prof()

	@remark
		- 객체정보 저장 : 객체키, 객체명, 객체타입 등을 저장한다.
		- 객체정보 반환

	@warning
		- 객체명은 중복가능하다.
		- // 객체키는 유일하다. 단, 유일성을 보장하는 루틴(따로 객체키값들을 저장해야 하기에)은 없고 long long 타입의 마이크로 시간을 이용해서 객체키를 설정하므로 유일성을 보장한다고 가정한다.

	@bug

	@todo

	@date
	    - 2009-02-02
	        -# 최초 작성
	    - 2009-04-08
	    	-# 복사생성자, 대입연산자, 객체키 추가
	    - 2009-04-10
	    	-# 객체타입 추가
	    - 2009-05-14
	    	-# 객체키를 마이크로초로 변경
	    	-# 객체명의 기본값을 OIID값으로 설정

	@example sam_object.cpp
*/

#ifndef __WCOBJECT_H__
#define __WCOBJECT_H__

class WCObject
{
public :
	// 객체 타입 
	enum E_OTYPE
	{
		E_OTYPE_OBJECT,			///< WCObject 객체
		E_OTYPE_MYSQL,			///< WCMysql 객체
		E_OTYPE_ORACLE,			///< WCOracle 객체
		//E_OTYPE_SOCKET,			///< WCSocket 객체
		//E_OTYPE_THREAD,			///< WCThread 객체
		//E_OTYPE_DB,				///< WCDb 객체
	};

public :
	/**
	* @brief 생성자
	*
	* 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	*/
	WCObject();

	/**
	* @brief 생성자
	*
	* 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	* @param pcName 객체명
	*/
	WCObject(const char* const pcName);

	/**
	* @brief 복사생성자
	*
	* 멤버복사(InitCopy) 호출
	* @param Other 복사객체
	*/
	WCObject(const WCObject &Other);

	/**
	* @brief 대입연산자
	*
	* 멤버복사(InitCopy) 호출
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCObject& operator=(const WCObject &Other);

	/**
	* @brief 소멸자
	* 
	* 객체키 초기화
	*/
	virtual ~WCObject();

public:
 	/**
	* @brief 객체키 설정
	*
	* @param nKey 객체키
	* @return 없음 
	*/
 	void SetKey(const unsigned int nKey);

 	/**
	* @brief 객체키 반환
	*
	* @return 성공: 객체키
	* @return 실패: 없음
	*/
	unsigned int GetKey() { return m_nKey; }

 	/**
	* @brief 객체명 설정
	*
	* @param pcName 객체명
	* @return 성공: 0
	* @return 실패: -1
	*/
 	int SetName(const char* const pcName);

	/**
	* @brief 객체명 반환
	*
	* @return 성공: 객체명
	* @return 실패: 없음
	*/
 	const char* GetName() { return m_cName; };

 	/**
	* @brief 객체타입 설정
	*
	* @param eType 객체타입
	* @return 없음
	*/
 	void SetType(const E_OTYPE eType);
 
	/**
	* @brief 객체타입 반환
	* 
	* @return 성공: 객체타입
	* @return 실패: 없음
	*/
	E_OTYPE GetType() { return m_eType; }

	/**
	* @brief 객체 존재 체크
	*
	* 객체가 메모리에 살아있는지 여부를 확인한다. try,catch문보다 적은 비용이 든다.
	* @return 성공: true
	* @return 실패: false
	*/
	bool IsLive();

private:
	/**
	* @brief 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
	*
	* @param pcName 객체명
	*/
	inline void InitMember(const char* const pcName);

	/**
	* @brief 복사생성,대입연산 초기화
	* 
	* 멤버를 복사한다. // [DLY] 객체키는 유일해야 하므로 객체키만 새로 할당하고 나머지 멤버는 복사한다.
	* @param Other 복사객체
	*/
	inline void InitCopy(const WCObject &Other);

protected:
	E_OTYPE m_eType;						///< 객체타입

private:
	unsigned int m_nKey;					///< 객체키 - 객체를 구별하기 위한 유일한 값이어야 하는데 일단은 중복가능하게 한다. (TickCount 설정시는 unsigned long long)
	char m_cName[30];						///< 객체명 - 사용자가 지정하는 중복가능한 값
};

#endif
