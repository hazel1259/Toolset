#pragma once
#include "tarray.h"

template<class T> 
class TCollection
{
public: 
    TCollection(void)
    {
      Init();
    }
    ~TCollection(void)
    {
    }
    // retrieve the first item in the collection
    T GetFirst()
    {
      if (m_currCount == 0)
      {
        return 0;
      }
      m_currItem = 0;
      return GetNext();
    }
    void Reset()
    {
      m_currCount = 0;
    }
    // retrieves the next item in the collection
    T  GetNext()
    {
      if (m_currItem < m_currCount)
      {
        m_currItem ++;
        return m_collection.get(m_currItem - 1);
      }
      //m_currItem = 0;
      return NULL;
    }
    
    int Count() { return m_currCount; }
    int Position() { return m_currItem; }
    T  operator [] (int pos)
    {
      if (pos < m_currCount)
      {
        return m_collection.get(pos);
      }
      return 0;
    }

    void ClearItem(int pos)
    {
      if (pos < m_currCount)
      {
        m_collection.remove(pos);
      }
    }
    // adds a new item to the collection, assumes the item was already allocated with the 
    // new operator. The collection destroys this copy when it is completed
    void AddNewItem(T  item)
    {
      if (m_currCount >= m_collection.size())
      {
        m_collection.grow(10);
      }
      m_collection.add((const T)item, m_currCount);
      m_currCount++;
    }
private:
    TArray<T, 10> m_collection;
    void Init()
    {
      m_currCount = 0;
      m_currItem =0;
    }
    int m_currCount ; // 0;
    int m_currItem ; //0;
};
