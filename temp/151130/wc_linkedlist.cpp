/**
	@file wc_linkedlist.cpp
   	@brief WCLinkedList 소스
*/

#include "wc_lib.h"

#define INITLIST m_nCount(0), m_ptSNode(NULL), m_ptENode(NULL), m_ptNNode(NULL)

template <typename T1>
WCLinkedList<T1>::WCLinkedList() : INITLIST
{
	//InitMember();
}

template <typename T1>
WCLinkedList<T1>::WCLinkedList(const WCLinkedList<T1> &Other) : INITLIST
{
	InitCopy(Other);
}

template <typename T1>
WCLinkedList<T1>& WCLinkedList<T1>::operator=(const WCLinkedList<T1> &Other)
{
	if (this != &Other) {
		InitCopy(Other);
	}
	return *this;
}

template <typename T1>
WCLinkedList<T1>::~WCLinkedList()
{
}

/*
inline void WCLinkedList::InitMember()
{
}
*/

template <typename T1>
inline void WCLinkedList<T1>::InitCopy(const WCLinkedList<T1> &Other)
{
	m_nCount = Other.m_nCount;
}

template <typename T1>
int WCLinkedList<T1>::Insert(const T1 t1Data)
{
	return 0;	
}

template <typename T1>
int WCLinkedList<T1>::Erase(const T1 t1Data)
{
	return 0;
}

template <typename T1>
int WCLinkedList<T1>::PushBack(const T1 t1Data)
{
	T_NODE *ptNode = new T_NODE;

	if (!ptNode) {
		WcLogPrintf(WCLog::E_MODE_ERROR,"[LNK:%s:%d] !ptNode\n",__FUNCTION__,__LINE__);
		return -1;
	}
	ptNode->t1Data = t1Data;
	ptNode->ptNext = NULL;
	ptNode->ptPrev = NULL;

    if (m_nCount == 0)
    {
    	ptNode->ptNext = ptNode;
    	ptNode->ptPrev = ptNode;

        m_ptSNode = ptNode;
        m_ptENode = ptNode;
        
        fprintf(stderr,"[S1] %d, (%p,%p,%p) (%p,%p,%p) (%p,%p,%p)\n", m_nCount, ptNode,ptNode->ptNext,ptNode->ptPrev, m_ptSNode,m_ptSNode->ptNext,m_ptSNode->ptPrev, m_ptENode,m_ptENode->ptNext,m_ptENode->ptPrev);
    }
    else
    {
    	m_ptENode->ptNext = ptNode;

    	ptNode->ptNext = m_ptSNode;
    	ptNode->ptPrev = m_ptENode;
        m_ptENode = ptNode;

    	if (m_nCount == 1) {
    		m_ptSNode->ptNext = ptNode;
    	}
    	m_ptSNode->ptPrev = m_ptENode;

        fprintf(stderr,"[S2] %d, (%p,%p,%p) (%p,%p,%p) (%p,%p,%p)\n", m_nCount, ptNode,ptNode->ptNext,ptNode->ptPrev, m_ptSNode,m_ptSNode->ptNext,m_ptSNode->ptPrev, m_ptENode,m_ptENode->ptNext,m_ptENode->ptPrev);
    }
    m_nCount++;
 
	return 0;
}


template <typename T1>
void WCLinkedList<T1>::Show()
{
	int nTemp=0;

    m_ptNNode = m_ptSNode;

	// 마지막 전까지 출력 
    while (m_ptNNode != m_ptENode) {

        fprintf(stderr,"[R1] %d, (%p,%p,%p) (%p,%p,%p) (%p,%p,%p)\n", m_nCount, m_ptNNode,m_ptNNode->ptNext,m_ptNNode->ptPrev, m_ptSNode,m_ptSNode->ptNext,m_ptSNode->ptPrev, m_ptENode,m_ptENode->ptNext,m_ptENode->ptPrev);

        cout << m_ptNNode->t1Data << endl;
        m_ptNNode = m_ptNNode->ptNext;
        
        fprintf(stderr,"[R2] %d, (%p,%p,%p) (%p,%p,%p) (%p,%p,%p)\n", m_nCount, m_ptNNode,m_ptNNode->ptNext,m_ptNNode->ptPrev, m_ptSNode,m_ptSNode->ptNext,m_ptSNode->ptPrev, m_ptENode,m_ptENode->ptNext,m_ptENode->ptPrev);
        nTemp++;
        if (nTemp >= 5) break;
        
    }
    
    // 마지막 데이터 출력 
    if (m_ptENode) { 
    	cout << m_ptENode->t1Data << endl;
    }
    
}
