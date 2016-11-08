/**
	@class WCLinkedList

	@brief
		링크드 리스트 클래스 : STL이 있기 때문에 사용할 일은 거의 없다.

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(O), gtest(), Sample(), Stress(), valg(), prof()

	@remark
		- 링크드 리스트 기능 : 템플릿으로 원형 링크드 리스트 기능 구현

	@warning

	@bug

	@todo

	@date
	    - 2012-08-09
	        -# 최초 작성

	@example sam_linkedlist.cpp
*/

#ifndef __WCLINKEDLIST_H__
#define __WCLINKEDLIST_H__

template <typename T1>
class WCLinkedList : public WCStruct
{
public:
   	/**
	* @brief 생성자
	*
	* 생성자리스트(INITLIST), 멤버초기화(InitMember) 호출
	*/
   	WCLinkedList();

	/**
	* @brief 복사생성자
	*
	* 멤버복사(InitCopy) 호출
	* @param Other 복사객체
	*/
	WCLinkedList(const WCLinkedList &Other);

	/**
	* @brief 대입연산자
	*
	* 멤버복사(InitCopy) 호출
	* @param Other 대입객체
	* @return 성공: 대입된 객체참조자
	* @return 실패: 없음
	*/
	WCLinkedList& operator=(const WCLinkedList &Other);

   	/**
	* @brief 소멸자
	*/
   	virtual ~WCLinkedList();

public:
	/**
	* @brief 멤버 추가
	*
	* @param t1Data 삽입데이터
	* @return 성공: 0<= (삽입인덱스)
	* @return 실패: -1
	*/
	int Insert(const T1 t1Data);

	/**
	* @brief 멤버 삭제
	*
	* @param t1Data 삭제데이터
	* @return 성공: 0<= (삭제인덱스)
	* @return 실패: -1
	*/
	int Erase(const T1 t1Data);

	int PushBack(const T1 t1Data);

	void Show();

private:
	/*
	* @brief 초기화리스트에서 초기화할 수 없는 멤버변수 초기화
	*
	inline void InitMember();
	*/

	/**
	* @brief 복사생성,대입연산 초기화
	* 
	* 멤버를 복사한다.
	* @param Other 복사객체
	*/
	inline void InitCopy(const WCLinkedList<T1> &Other);

private:
    /// 데이터 저장 구조체
    struct T_NODE
    {
        T1 t1Data;			// 저장 데이터
        T_NODE *ptNext;		// 다음 노드를 가리키는 포인터
        T_NODE *ptPrev;		// 이전 노드를 가리키는 포인터
    };

    // 노드갯수
    int m_nCount;			///< 노드 갯수
    T_NODE *m_ptSNode;  	///< 시작노드 포인터 (헤더)
    T_NODE *m_ptENode;  	///< 마지막노드 포인터 (테일)
	T_NODE *m_ptNNode;  	///< 다음노드를 포인트한다.  

    /*
   
    
    T_NODE *m_ptMNode;  	///< 노드 할당을 위해서 사용한다. (필요없다)
    */
};

#endif
