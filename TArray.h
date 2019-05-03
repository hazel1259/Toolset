
#pragma once

/*************************************************************************************************************************

  Class declaration for a simple template array.

  Author: Hazel LLoyd
  Created: 05-09-06
  History:

  05-09-06 - created the base template array and added the basic methods for adding and cleaning the array

**************************************************************************************************************************/
template<class T, int size> 
class TArray
{
    int sizeofarray;
    T * array;
public: 
    TArray( void );
    ~TArray( void );
    //////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////
    void  add(const T item, int position);
    void  remove(int position);
    int   grow(int size);
    int   count() const { return sizeofarray; }
    T     get(int pos);
    int   size() { return sizeofarray; }
    void  clearitem(int pos);
};

/*************************************************************************************************************************

  TArray()

  Type: Constructor
  Base: template
  Scope: Public
  Parameters: 
  class T - template which can be a struct, type, or class.
  int   i - size of the initial array.

  returns: None, this is a constructor
**************************************************************************************************************************/
template< class T, int i > TArray< T, i >::TArray( void )
{
  sizeofarray = i + 1;
  array = (T *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(T) * sizeofarray);
}

/*************************************************************************************************************************

  ~TArray()

  Type: Destructor
  Base: template
  Scope: Public
  Parameters: 
  
  class T - template which can be a struct, type, or class.
  int   i - size of the initial array.

  returns: None, this is a Destructor. In this function we clean all allocated memory and let the destructor 
  for each object be called.

**************************************************************************************************************************/
template< class T, int i > TArray< T, i >::~TArray( void )
{
  HeapFree(GetProcessHeap(), 0, array);  
}

/*************************************************************************************************************************

  add()

  Type:   method
  Base:   template
  Scope:  Public
  
  Parameters: 
  
  const T item - object to add to the array, it must be of the type that created this template
  int   pos - position to add the item. 

  returns: None, if the position is within the bounds of the array, it is added, otherwse, it is not. No exceptions
  are thrown on error.

**************************************************************************************************************************/
template< class T, int i > 
void TArray< T, i >::add( const T item, int pos )
{
  if (pos >= 0 && pos < sizeofarray)
  {
    array[pos] = item;
  }
}


/*************************************************************************************************************************

  remove()

  Type:   method
  Base:   template
  Scope:  Public
  
  Parameters: 
  
  const T item - object to add to the array, it must be of the type that created this template
  int   pos - position to add the item. 

  returns: None, if the position is within the bounds of the array, T is removed, otherwse, it is not. No exceptions
  are thrown on error.

**************************************************************************************************************************/
template< class T, int i > 
void TArray< T, i >::remove(int pos )
{
  if (pos >= 0 && pos < sizeofarray)
  {
    array[pos] = 0;
  }
}

/*************************************************************************************************************************

  grow()

  Type:   method
  Base:   template
  Scope:  Public
  
  Parameters: 
  
  int   size - number of elements of object size to grow by. 

  returns: TRUE if we grew the array, FALSE otherwise.

**************************************************************************************************************************/
template< class T, int i > 
int TArray< T, i >::grow( int size)
{
  LPVOID ptmp = (LPVOID)array;
  int size_tmp = size + sizeofarray + 1;
  array = (T *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (LPVOID)ptmp,  sizeof(T) * size_tmp);
  if (!array)
  {
    size = size_tmp;
    array = (T *)ptmp;
    return FALSE;
  }
  else
  {
    sizeofarray = size_tmp;
  }
  return TRUE;
}

/*************************************************************************************************************************

  get()

  Type:   method
  Base:   template
  Scope:  Public
  
  Parameters: 
  
  int   pos - index of the object to retrieve from the array, An invalid paranmeter exceotuin kis thrown when the
  index is not in the array range. 

  returns: object of type T used to create the template, an exceptoin is thrownon erropr.

**************************************************************************************************************************/
template< class T, int i > 
T TArray< T, i >::get( int pos)
{
  if (pos >= 0 && pos < sizeofarray)
  {
    return array[pos];
  }
  throw ERROR_INVALID_PARAMETER;
}
