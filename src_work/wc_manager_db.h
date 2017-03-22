/**
	@class WCManagerDB

	@brief
		DB 객체 관리자

	@details
		- 설명
			- DB 객체를 멤버로 등록해서 사용하게 한다.
		- 기능
			- DB 객체 관리 : WCDb 객체를 멤버로 등록하여 그룹으로 관리한다. 
			- DBPool 기능 : DB연결 정보를 받아서 내부에서 WCDb 객체를 생성해서 Pooling 기능을 제공한다.

	@warning
		- 복사생성자, 대입연산자 : 금지

	@todo
		- DB 타입을 할까 말까?
		- PoolOpen 함수 구현 - 한꺼번에 오픈, DB 타입을 구분해야 하네...
		- manager 안에서 쿼리 함수 지원. 그런데 select가 문제 

	@author	
		서원철, neo20@hanmail.net

	@date
	    - 2015-10-28
	        -# 최초 작성

	@example sam_manager_db.cpp
*/

#ifndef __WC_MANAGERDB_H__
#define __WC_MANAGERDB_H__

class WCManagerDB : public WCManager<unsigned long long>
{
public:
	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST) 호출
	*/
	WCManagerDB();

	/**
	@brief 생성자
	@details
		- 생성자리스트(INITLIST) 호출
	@param nMax 최대멤버수
	*/
	WCManagerDB(const int nMax);

	/**
	@brief 소멸자
	@details
		-
	*/
	virtual ~WCManagerDB();

public:
	/**
	@brief DB객체 추가 
	@details
		- 맵에 DB객체 삽입, 키는 순차값, DB객체는 외부에서 Open(DB연결)되어 있어야 한다.
	@param pDB DB객체
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
    virtual int Add(WCDb* const pDB);

	/**
	@brief DB객체 삭제
	@details
		- DB맵에서 DB객체 삭제
	@param pDB DB객체
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	virtual int Remove(WCDb* const pDB);

	/**
	@brief DB객체 오픈
	@details
		- 맴버로 등록된 DB 객체를 오픈한다.
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int Open();

	/**
	@brief DB객체 생성 및 오픈
	@details
		- DB정보를 받아서 내부에서 DB객체를 생성하고 맵에 등록한다. 내부에서 생성한 객체는 소멸자에서 해제한다.
		- nCount값이 최대멤버수보다 클 경우는 최대멤버수만큼만 오픈한다.
	@param strIP DB주소
	@param nPort DB포트
	@param strID DB사용자
	@param strPW DB사용자암호
	@param strDB DB명
	@param eCharSet 캐릭터셑
	@param nCount DB연결자 갯수
	@return 성공: WC_OK
	@return 실패: WC_NOK
	*/
	int Open(const std::string &strIP, const int nPort, const std::string &strID, const std::string &strPW, const std::string &strDB, const WCDb::E_CHARSET eCharSet=WCDb::E_CHARSET_UTF8, const int nCount=5);

	/**
	@brief DB객체 반환
	@details
		- 사용가능한 DB객체를 반환한다. 
		- 반환하는 알고리즘은 라운드로빈, 즉 순차적이다. 현재 사용중이지 않거나 최소 사용빈도 등을 계산해서 반환하는 것도 괜찮으나 복장성,속도 등을 고려할 때 순차적도 괜찮음.
	*/
	WCDb* GetDB();

private:
	/**
	@brief 복사생성자
	@details
		- 
	@param Other 복사객체
	*/	
	WCManagerDB(const WCManagerDB &Other);

	/**
	@brief 대입연산자
	@details
		- 
	@param Other 대입객체
	@return 성공: 대입된 객체참조자
	@return 실패: 없음
	*/
	WCManagerDB& operator=(const WCManagerDB &Other);

	/**
	@brief 멤버변수 초기화
	@details
		- 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
		- DB객체반복자 초기화
	@return void
	*/	
	void InitMember();

private:
	map<unsigned long long, WIMember*>::iterator m_itMap;		///< DB객체반복자

};

#endif
