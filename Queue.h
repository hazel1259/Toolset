#pragma once

typedef struct _QNODE
{
    _QNODE * Next;
    void*   pData;
}QNODE, *PQNODE;

template<class T>
class TQueue
{
private:

public:
    TQueue();
    TQueue(FREEDATAPROC freeData);
    ~TQueue();
    void    add(T item);
    T       remove();
    //T       get();
    T       head();
    T       tail();
    T       query(int pos);
    bool    promote(int pos, int newpos);
    bool    demote(int pos, int newpos);
    bool    move_to_front(int pos);
    bool    move_to_end(int pos);
    int     count();
    void    release();
private:
    PQNODE   pHead;
    PQNODE   pEnd;
    int     m_count;
    
    PQNODE get_node(int pos)
    {
        if (pos > 0 && pos <= m_count)
        {
            PQNODE p = pHead;
            for (int i = 1; i < pos; i ++)
            {
                p = p->Next;
            }
            return p;
        }
        return 0;
    }
    FREEDATAPROC m_freeData;
};

template<class T>
TQueue<T>::TQueue()
{
    pHead = pEnd = NULL;
    m_count = 0;
    m_freeData = NULL;
}

template<class T>
TQueue<T>::TQueue(FREEDATAPROC fd)
{
    pHead = pEnd = NULL;
    m_count = 0;
    m_freeData = fd;
}
template<class T>
TQueue<T>::~TQueue()
{
}

template<class T>
void TQueue<T>::release()
{
    PQNODE p = pHead;
    while(p)
    {
        PQNODE tmp = p->Next;
        if (m_freeData)
        {
            m_freeData(p->pData);
        }
        MyFree(p);
        p = tmp;
    }
    pHead = NULL;
    m_count = 0;
}
template<class T>
int TQueue<T>::count()
{
    return m_count;
}

template<class T>
T TQueue<T>::head()
{
    if (pHead)
    {
        return (T)pHead->pData;
    }
    return 0;
}

template<class T>
T TQueue<T>::tail()
{
    if (pEnd)
    {
        return (T)pEnd->pData;
    }
    return 0;
}

template<class T>
void TQueue<T>::add(T item)
{
    PQNODE pq = MyAlloc(QNODE *, sizeof(QNODE));
    if(pq)
    {
        pq->pData = (PVOID)item;
        if (!pHead)
        {
            pHead = pq;
            pHead->Next = NULL;
            pEnd = pHead;
        }
        else
        {
            pEnd->Next = pq;
            pEnd = pq;
        }
        m_count ++;
    }
}

template<class T>
T TQueue<T>::remove()
{
    if (pHead)
    {
        PQNODE p = pHead;
        T ret = (T)p->pData;
        pHead = p->Next;
        MyFree(p);
        m_count --;
        return ret;
    }
    return 0;
}

//template<class T>
//T TQueue<T>::get()
//{
//    return (T)pHead->pData;
//}

template<class T>
T TQueue<T>::query(int pos)
{
    if (pos > 0 && pos <= m_count)
    {
        PQNODE p = pHead;
        for (int i = 1; i < pos; i ++)
        {
            p = p->Next;
        }
        if (p)
        {
            return (T)p->pData;
        }
    }
    return 0;
}

template<class T>
bool TQueue<T>::promote(int pos, int newpos)
{
    if (pos == newpos) // make it idempotent/reflexive
    {
        return true;
    }
    
    if(m_count == 1)
    {
        return false; // cannot promote if nowhere to promote
    }
    // invalid replacement node
    if (pos > m_count)
    {
        return false;
    }
    // invalid location
    if (newpos > pos || newpos < 1)
    {
        return false; // cannot demote when promoting
    }
    // see if we fit
    if ((pos > 0) && (pos <= m_count) && (newpos < pos))
    {
        PQNODE pNode    = get_node(pos);
        PQNODE pPrev    = get_node(pos - 1);
        PQNODE pNext    = get_node(pos + 1);
        if (newpos == 1 && pNode)
        {
            pNode->Next = pHead;
            // move to head
            pHead = pNode;
            if (pPrev)
                pPrev->Next = pNext;
            // return true
            return true;
        }
        PQNODE pNewPos  = get_node(newpos - 1);
        // new position node is valid and is the head
        if (pNode && pNewPos)
        { 
            // are we replacing the head of the queue?
            if (pNewPos == pHead)
            {
                pNode->Next = pHead;
                pHead = pNode;
            }
            else
            {
                // just replacing a node
                PQNODE pNext = pNewPos->Next;
                pNewPos->Next = pNode;
                pNode->Next = pNext;
            }
            // previous pointer
            if (pPrev)
            {
                pPrev->Next = pNext;
            }
            return true;
        }        
    }
    return false; // cannot promote to non-existent node
}

template<class T>
bool TQueue<T>::demote(int pos, int newpos)
{
    if (pos == newpos) // make it idempotent/reflexive
    {
        return true;
    }
    
    if(m_count == 1)
    {
        return false; // cannot demote if nowhere to promote
    }
     // invalid replacement node
    if (pos > m_count)
    {
        return false;
    }
    // invalid location
    if (pos > newpos || pos < 1)
    {
        return false; // cannot demote when promoting
    }
    
    if ((pos > 0) && (pos <= m_count) && (pos < newpos))
    {
        PQNODE pNode    = get_node(pos);
        PQNODE pPrev    = get_node(pos - 1);
        PQNODE pNext    = get_node(pos + 1);
        // move node to the end of the queue
        if (newpos == m_count && pNode)
        {
            pEnd->Next = pNode;
            if (pNode == pHead)
            {
                pHead = pHead->Next;
            }
            if (pPrev)
            {
                pPrev->Next = pNext;
            }
            pEnd = pNode;
            pEnd->Next = NULL;
            return true;
        }
        PQNODE pNewPos  = get_node(newpos);
        // new position node is valid and is the head
        if (pNode && pNewPos)
        { 
            // are we replacing the end of the queue?
            if (pNewPos == pEnd)
            {
                pNode->Next = pEnd;
                pEnd = pNode;
            }
            else
            {
                // just replacing a node
                if (pNode == pHead)
                {
                    pHead = pHead->Next;
                    
                }
                PQNODE pNext    = pNewPos->Next;
                pNewPos->Next   = pNode;
                pNode->Next     = pNext;
            }
            // previous pointer
            if (pPrev)
            {
                pPrev->Next = pNext;
            }
            return true;
        }            
    }  
    return false;
}

template<class T>
bool TQueue<T>::move_to_front(int pos)
{
    return promote(pos, 1);
}

template<class T>
bool TQueue<T>::move_to_end(int pos)
{
    return demote(pos, m_count);
}

