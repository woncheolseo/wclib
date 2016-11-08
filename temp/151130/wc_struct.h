/**
	@class WCStruct

	@brief
		자료구조 상위 클래스 : 리스트,스택,큐 등의 자료구조 상위클래스

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(), gtest(), Sample(), Stress(), valg(), prof()

	@remark
		- 자료 구조 상위 클래스 역할만 한다.

	@warning

	@bug

	@todo

	@date
	    - 2009-07-12
	        -# 최초 작성
*/

#ifndef __WCSTRUCT_H__
#define __WCSTRUCT_H__

class WCStruct : public WCObject
{
public :
	/// 생성자
	WCStruct();

	/// 소멸자
	virtual ~WCStruct();

	/**
	* @brief 복사생성자
	*
	* @param Other 복사객체
	*/
	WCStruct(const WCStruct &Other);

	/**
	* @brief 대입연산자
	*
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCStruct& operator=(const WCStruct &Other);

public:
	/*
	* @brief 멤버 추가
	*
	* @param pData 삽입데이터
	* @return 성공: 0<= (삽입인덱스)
	* @return 실패: -1
	*
	//virtual int VInsert(const void * pData)=0;
	virtual int VInsert(const T1 pData)=0;
	*/

	/*
	* @brief 멤버 삭제
	*
	* @param pData 삭제데이터
	* @return 성공: 0<= (삭제인덱스)
	* @return 실패: -1
	*
	//virtual int VErase(const void * pData)=0;
	virtual int VErase(const T1 pData)=0;
	*/
};

#endif
