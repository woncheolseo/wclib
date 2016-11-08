/**
	@class WCStack

	@brief
		스택 클래스 : STL이 있기 때문에 사용할 일은 거의 없다.

	@author	
		서원철, neo20@hanmail.net

	@version 
		1.00 / doxy(O), gtest(), Sample(), Stress(), valg(), prof()

	@remark
		- 스택 기능 : 템플릿으로 스택 기능 구현

	@warning

	@bug

	@todo

	@date
	    - 2009-07-12
	        -# 최초 작성
*/

#ifndef __WCSTACK_H__
#define __WCSTACK_H__

template <typename T>
class WCStack : public WCStruct
{
public:
   	/**
	* @brief 생성자
	*
	* 디폴트 인자로 스택의 용량크기를 받아들인다. 용량크기 만큼 메모리 할당하고 몇가지 값을 초기화 한다.
	* @param nAsz 스택 크기
	*/
   	WCStack(int nAsz=12)
	{
		nSize = nAsz;
		nCount = 0;
		tSizeof = sizeof(T);
		ptContainer = (T *)malloc(tSizeof * nSize);
    }

    /**
	* @brief 소멸자
	*
	* 스택을 free 한다.
	*/
    ~WCStack()
    {
    	free(ptContainer);
    }

    /**
	* @brief 멤버 추가
	*
	* @param pData 추가할 데이터
	* @return 성공: 0
	*/
    virtual int VAdd(const void * const pData) { return 0; }

    /**
	* @brief 멤버 삭제
	*
	* @param pData 삭제할 데이터
	* @return 성공: 0
	*/
	virtual int VRemove(const void * const pData) { return 0; }

    /**
	* @brief 데이터 삽입
	*
	* 데이타를 스택에 입력한다. 만약 스택사이즈가 꽉차있다면 realloc 를 호출해서 스택사이즈를 (현재 스택사이즈 * 2) 만큼 늘려준다.
	* @param data 삽입할 데이터
	*/
    void PushBack(T data)
    {
	    if (nCount == (nSize - 1))
	    {
	        nSize *= 2;
	        ptContainer = (T *)realloc(ptContainer, tSizeof * nSize);
	    }
	    *(ptContainer + nCount) = data;
	    nCount++;
    }

    /**
	* @brief 데이터 삭제
	*
	* 스택에서 데이타를 꺼낸다.
	* @return 성공: T형 데이터
	*/
    T PopBack()
    {
		nCount --;
		return *(ptContainer+nCount);
    }

    /**
	* @brief 빈 스택 확인
	*
	* @return 성공: true
	* @return 실패: false
	*/
    bool IsEmpty()
    {
    	return nCount == 0;
    }

    /**
	* @brief 데이타 갯수 반환
	*
	* @return 성공: 0<= (데이터갯수)
	*/
	int GetSize()
    {
    	return nCount;
    }

    /**
	* @brief 스택 용량 확인
	*
	* @return 성공: 0<= (스택용량)
	*/
    int GetCapacity()
    {
    	return nSize;
    }


private:
    T *ptContainer;    	///< 저장소
    int nCount;			///< 스택에 저장된 원소의 갯수
    int nSize;  		///< 스택의 용량크기
    int tSizeof;		///< 타입 T 의 sizeof
};

#endif
