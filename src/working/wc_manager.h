/**
	@class WCManager

	@brief
		객체 관리 템플릿 클래스

	@details
		- 설명
			- 객체관리 템플릿 : 객체를 멤버로 관리하는 클래스의 템플릿으로 인터페이스 역할을 하며 순수가상함수가 선언되어서 직접 사용할수 없다. 
		- 기능
			- 객체(소켓, 쓰레드 등)를 멤버로 등록하여 관리한다.
			- 최대 멤버수 제한

	@warning
		- 복사생성자, 대입연산자 : 금지

	@bug
		-# 멤버를 입력,수정,삭제할 때 뮤텍스를 거는데 안 걸수 있는 방법은 ?

	@todo
		-# 현재는 Key가 유일한 단일맵인데 다중키가 가능한 멀티맵도 가능하게 하라.

	@author	
		- 서원철, neo20@hanmail.net

	@date
	    - 2015-10-16
	        -# 최초 작성

	@example sam_manager.cpp
*/

#ifndef __WC_MANAGER_H__
#define __WC_MANAGER_H__

template <typename T1> 
class WCManager : public WCGroup
{
public :
	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	*/
	WCManager();

	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	@param nMax 멤버최대수
	*/
	WCManager(const int nMax);

	/**
	@brief 소멸자
	@details
		- 맴버맵 해제, 뮤텍스 해제
	*/
	virtual ~WCManager();

public :
	/**
	@brief 멤버 추가
	@details
		- 
	@param tKey 객체키
	@param pIManager 추가할 객체인터페이스
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	virtual int Add(T1 tKey, WIMember* pIManager);

	/**
	@brief 멤버 삭제
	@details
		- 맵에서 멤버 삭제
		- 멤버 자체는 삭제하지 않는다. 
		- 단, 멤버가 내부에서 삭제되어야 할 경우는 삭제
	@param tKey 삭제할 객체키
	@return 성공: WC_OK
	@return 실패: 없음
	*/
	virtual int Remove(T1 tKey);
	
	/**
   	@brief 모든 멤버 삭제
   	@details
   		- 맵의 모든 멤버를 맵에서 삭제
   		- 멤버가 내부에서 할당된 경우는 삭제
   		- 단, 멤버가 내부에서 삭제되어야 할 경우는 삭제
   	@return void
	*/
	void RemoveAll();

	/**
	@brief 객체 찾기
	@details
		- 
	@param tKey 객체키
	@return 성공: 객체인터페이스포인터
	@return 실패: NULL
	*/
	WIMember* Find(T1 tKey);

	/**
	@brief 멤버 최대수 반환 
	@details
		-  
	@return 성공: 0,양수
	@return 실패: 없음
	*/
	int GetMaxMember() {
		return m_nMaxMember;
	}	

	/**
	@brief 멤버 최대수 설정
	@details
		- 
	@param nMax 최대수
	@return void
	*/
	void SetMaxMember(const int nMax) {
		m_nMaxMember = nMax;
	}	

	/**
	@brief 맴버맵 맴버 갯수 반환
	@details
		- 
	@return 성공: 맴버맵 개수
	@return 실패: 없음
	*/
	size_t Size() {
		return m_mMember.size();
	}	

	/**
	@brief 맴버맵 반환
	@details
		-  
	@return 성공: 맴버맵 포인터
	@return 실패: 없음
	*/
	map<T1, WIMember*>* GetMap() { 
		return &m_mMember; 
	};

	/**
	@brief 맴버맵 뮤텍스 얻기
	@details
		- 
	@return 성공: 멤버맵 뮤텍스 포인터
	@return 실패: 없음
	*/
	pthread_mutex_t* GetMutex() { 
		return &m_mutexMember; 
	}

private:
	/**
	@brief 복사생성자
	@details
		- 복사 금지
	@param Other 복사객체
	*/
	WCManager(const WCManager &Other);

	/**
	@brief 대입연산자
	@details
		- 대입 금지
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCManager& operator=(const WCManager &Other);

	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
	@return void
	*/
	void InitMember();

protected:
	int m_nMaxMember;						///< 멤버최대수 - 무제한은 없다.
	map<T1, WIMember*> m_mMember;			///< 맴버맵
	pthread_mutex_t m_mutexMember;			///< 맴버맵 뮤텍스
};

#endif
