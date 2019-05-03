#pragma once

template<class T>
class TStack
{
public:
    TStack();
    TStack(FREEDATAPROC freedata);
    ~TStack();
    
    void            push(T item);
    T               pop();
    T               get();
    T               get(int pos);
    int             count();
private:
    bool            grow();
    int             m_size;
    int             m_currPos;
    T *             m_array;
    FREEDATAPROC    m_freeData;    
};

template<class T>
TStack<T>::TStack()
{
    m_size      = 10; 
    m_currPos   = 0;
    m_array     = (T *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_size * sizeof(T));
    if (!m_array)
    {
        throw(GetLastError());
    }
    m_freeData = NULL;
}

template<class T>
TStack<T>::TStack(FREEDATAPROC freedata)
{
    m_size      = 10; 
    m_currPos   = 0;
    m_array     = (T *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_size * sizeof(T));
    if (!m_array)
    {
        throw(GetLastError());
    }
    m_freeData = freedata;
}

template<class T>
TStack<T>::~TStack()
{
    m_size = m_currPos = 0;
    if (m_array)
    {
        MyFree(m_array);
    }
}

template<class T>
T TStack<T>::get(int pos)
{
    if(pos > 0 && pos <= m_size)
    {
        return m_array[pos - 1];
    }
    return 0;
    
}

template<class T>
int TStack<T>::count()
{
    return m_count;
}
template<class T>
T TStack<T>::get()
{
    if(m_currPos > 0)
    {
        return m_array[m_currPos - 1];
    }
    return 0;
    
}

template< class T>
void TStack<T>::push(T item)
{
    if (!grow())
    {
        return; // should probably throw
    }
    m_array[m_currPos] = item;
    m_currPos ++;
}

template< class T>
T TStack<T>::pop()
{
    if (m_currPos > 0)
    {
        T ret = m_array[m_currPos - 1];
        //if(m_freeData)
        //{
        //    m_freeData((PVOID)ret);
        //}
        m_currPos --;
        return ret;
    }
    return 0;
}    

template< class T>
bool TStack<T>::grow()
{
    bool b = true;
    if (m_currPos >= m_size)
    {
        // first, try to reallocate it
        T * tmp = (T* )HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_array, (m_size + 10) * sizeof(T)) ;
        if (!tmp)
        {
            // if that fails, try to allocate it
            tmp = (T *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_size * sizeof(T));
            if (!tmp)
            {
                b = false;
            }
            else
            {
                for (int i = 0; i < m_size; i ++)
                {
                    tmp[i] = m_array[i];
                }
                m_array = tmp;
                MyFree(m_array);
                m_size += 10;
            }
        }
        else
        {
            m_array = tmp;
            m_size +=10;
        }
    }
    return b;
}
