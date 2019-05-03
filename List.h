#pragma once
typedef struct _NODE
{
    _NODE *  Next;
    _NODE *  Prev;
    void *   data;
}NODE, *PNODE;

template<class T> 
class TList
{
private:
    bool IsValidPos(int pos);

    PNODE   pHead;
    PNODE   pCurr;
    int     m_count;
public:        
    TList();
    TList(FREEDATAPROC freeProc);
    ~TList();
	void Initialize();
    T operator[] (int pos);
    int     Count();
    void    MoveToBegin();
    void    MoveToEnd();
    void    AddItem(T item);
    T       RemoveItem();
    void    AddAtPos(T item, int pos);
    void    InsertAfter(T item, int pos);
    void    InsertBefore(T item, int pos);
    T       RemoveAtPos(int pos);
    T       GetAtPos(int pos);
    bool    MoveToPos(int pos);
    void    Release();
    T       CurrItem();
    T       GetFirst();
    T       GetNext();
    T       GetLast();
    T       GetPrev();
    virtual 
    void    FreeNode(T item);
protected:
    PNODE   GetNodeAtPos(int pos);
    void    AddNode(PNODE pNode);
    PNODE   NewNode(T item);
    FREEDATAPROC m_freeData;
};


template<class T>
TList<T>::TList()
{
    pCurr = pHead = NULL;
    m_count = 0;
    m_freeData = NULL;
}
//    TList(FREEDATAPROC freeProc);
template<class T>
TList<T>::TList(FREEDATAPROC freeProc)
{
    pCurr = pHead = NULL;
    m_count = 0;
    m_freeData = freeProc;
}

template<class T>
TList<T>::~TList()
{
}

template <class T>
T TList<T>::operator [](int pos)
{
    return GetAtPos(pos);
}
//template <class T>
//T TList<>::
//{
//    return GetAtPos(pos);
//}
//int& IntVector::operator[]( int nSubscript ) {
template<class T>
void TList<T>::Initialize()
{
   pCurr = pHead = NULL;
    m_count = 0;
   // m_freeData = freeProc;
}
template<class T>
bool TList<T>::IsValidPos(int pos)
{
    return (pos > 0 && pos <= m_count);
}

template<class T>
int TList<T>::Count() 
{
    return m_count;
}
    
template<class T>
void TList<T>::MoveToBegin()
{
    pCurr = pHead;
}

template<class T>
void TList<T>::MoveToEnd()
{
    if (pHead)
    {
        PNODE p = pHead;
        
        while(p->Next)
        {
            p = p->Next;
        }
        pCurr = p;
    }
}

template<class T>
void TList<T>::AddItem(T item)
{
    PNODE node = NewNode(item);
    if (node)
    {
        AddNode(node);
    }
}

template<class T>
T TList<T>::RemoveItem()
{
    if (pCurr)
    {
        PNODE pPrev = pCurr->Prev;
        PNODE pNext = pCurr->Next;
        pPrev->Next = pNext;
        PVOID pData = pCurr->data;
        if (pPrev)
        {
            pPrev->Next = pNext;
        }
        if (pNext)
        {
            pNext->Prev = pPrev;
        }
        free(pCurr);
        return (T)pData;
    }
    return 0;
}

template<class T>
void TList<T>::AddAtPos(T item, int pos)
{

    if (pos == 0 && !pHead)
    {
        AddItem(item);
    }
    else
    {
        if (IsValidPos(pos))
        {
            PNODE pNew = NewNode(item);
            pCurr = GetNodeAtPos(pos);
            if (pCurr)
            {
                AddNode(pNew);
            }    
        }
    }
}

template<class T>
void TList<T>::InsertAfter(T item, int pos)
{
    AddAtPos(item, pos);
}

template<class T>
void TList<T>::InsertBefore(T item, int pos)
{
    if (pos == 0)
    {
        return;
    }
    if(pos == 1)
    {
        PNODE pNew = NewNode(item);
        pNew->Next = pHead;
        pNew->Prev = NULL;
        if (pHead)
        {
            pHead->Prev = pNew;
        }
        pHead = pNew;
        m_count ++;
    }
    else
    {
        AddAtPos(item, pos - 1);
    }        
}

template<class T>
T TList<T>::RemoveAtPos(int pos)
{
    if (IsValidPos(pos))
    {
        int c = 0;
        PNODE pNode = GetNodeAtPos(pos);
        
        if (pNode)
        {
            PVOID pData = pNode->data;
            PNODE pPrev = pNode->Prev;
            PNODE pNext = pNode->Next;
            if (pPrev)
            {
                pPrev->Next = pNext;
            }
            if (pNext)
            {
                pNext->Prev = pPrev;
            }
            if (pNode == pHead)
            {
                pHead = pNext;
            }
            free(pNode);
            m_count --;
            return (T)pData;
        }
    }
    return 0;
}

template<class T>
T TList<T>::GetAtPos(int pos)
{
    if (IsValidPos(pos))
    {
        int c = 0;
        PNODE pNode = pHead;
        while(pNode && c < pos)
        {
            pNode = pNode->Next;
            c ++;
        }
        if (pNode)
        {
            return (T)pNode->data;
        }
    }
    return 0;
}

template<class T>
bool TList<T>::MoveToPos(int pos)
{
    if (GetAtPos(pos) != 0)
    {
        return true;
    }
    return false;
}

template<class T>
void TList<T>::Release()
{
    PNODE p = pHead;
    while(p)
    {
        PNODE tmp = p->Next;
        if (p->data && m_freeData)
        {
            m_freeData(p->data);
            p->data = NULL;
        }
        HeapFree(GetProcessHeap(), 0, p);
        //free(p);
        p = tmp;
    }
    pCurr = pHead = NULL;
    m_count = 0;
}

template<class T>
T TList<T>::CurrItem()
{
    if (pCurr)
    {
        return (T)pCurr->data;
    } 
    return 0;
}

template<class T>
T TList<T>::GetFirst()
{
    MoveToBegin();
    return CurrItem();
}

template<class T>
T TList<T>::GetNext()
{
    if (pCurr && pCurr->Next)
    {
        pCurr = pCurr->Next;
        return CurrItem();
    }
    else
    {
        pCurr = pHead;
    }
    return 0;
}

template<class T>
T TList<T>::GetLast()
{
    MoveToEnd();
    return CurrItem();
}

template<class T>
T TList<T>::GetPrev()
{
    if (pCurr && pCurr->Prev)
    {
        pCurr = pCurr->Prev;
        return CurrItem();        
    }
    else
    {
        pCurr = pHead;
    }
    return 0;
}

template<class T>
PNODE TList<T>::GetNodeAtPos(int pos)
{
    if (IsValidPos(pos))
    {
        int c = 1;
        PNODE pNode = pHead;
        PNODE pPrev = pHead;
        while(c <= pos - 1)
        {
            pPrev = pNode;
            pNode = pNode->Next;
            c ++;
        }
        return pNode;
    }
    return 0;
}
    
template<class T>
void TList<T>::AddNode(PNODE pNode)
{
    if (pCurr)
    {
        PNODE pNext = pCurr->Next;
        if(pNext)
        {
            pNext->Prev = pNode;
        }
        pCurr->Next = pNode;
        pNode->Prev = pCurr;
        pNode->Next = pNext;
    }
    else
    {
        pHead = pNode;
    }
    pCurr = pNode;
    m_count ++;
}

template<class T>
PNODE TList<T>::NewNode(T item)
{
    PNODE node = (PNODE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NODE));
    if (node)
    {
        node->data = item;
        node->Prev = node->Next = NULL;
    }
    else
    {
        //OutputDebugString("Error allocating memory. Make sure you have enough memory free");
    }
    return node;
}


template<class T>
void TList<T>::FreeNode(T item)
{
    if (m_freeData)
    {
        m_freeData(item);
    }

}