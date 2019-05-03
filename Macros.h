///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Various macros we use to initialize varaibles, classes, and bad guys with
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// these macors define masks to be used in the application to unassemble data with
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define gen_byte_val(__N__) (unsigned short)((0xFF) << __N__)
#define gen_byte_mask(__N__) ~(gen_byte_val(__N__))
#define mask_bits_low(__N__)gen_byte_mask(__N__)
#define mask_bits_high(__N__)gen_byte_mask((8 - __N__))

#define gen_word_val(__N__) (unsigned short)((0xFFFF) << __N__)
#define gen_word_mask(__N__) ~(gen_word_val(__N__))

#define gen_long_val(__N__)(unsigned long)((0xFFFFFFFF) << __N__)
#define gen_long_mask(__N__) ~(gen_long_val(__N__))

#define is_bit_set(__V__, __N__) (__V__ & (1 << (__N__ - 1)))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Heap alloc wrappers, you should not remove the compiler directive as it is important when compiling unix
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// allocate some heap memory
//
#ifdef POSIX
#define MyAlloc(_t_, _s_) (_t_)malloc, _s_)
// Fre the memory, should have been allocated with HeapAlloc
//
#define MyFree(_p_) (_t_)free(_p_); _p_ = NULL
// Re allocate the memory, should have been allocated with HeapAlloc, don't forge the 
// return can trash an existing pointer
//
#define MyRealloc(_t_, _p_, _s_) (_t_)realloc(_p_, _s_) 
#else

#define MyAlloc(_t_, _s_) (_t_)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, _s_)
// Fre the memory, should have been allocated with HeapAlloc
//
#define MyFree(_p_) HeapFree(GetProcessHeap(), 0, _p_); _p_ = NULL
// Re allocate the memory, should have been allocated with HeapAlloc, don't forge the 
// return can trash an existing pointer
//
#define MyRealloc(_t_, _p_, _s_) (_t_)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, _p_, _s_) 
// Get the heap size of a previously allocated chnk
//
#define MyHeapSize(__P__) HeapSize(GetProcessHeap(), 0, __P__)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// defines the length of a string so the compiler does not complain about using int instead of size_t
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define slen(__S__) (int)strlen((LPSTR)__S__)
#define wlen(__S__) (int)wcslen((LPWSTR)__S__)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Buffer management routines, shouldn't remove these, they are also important
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// End of a buffer we read.
//
#define BUFF_END(__p1__,__p2__,__d__) ((DWORD)(__p1__ - __p2__) == __d__)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Initialization macros, these are used to initialize strings
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INIT_STR(__N__, __S__, __I__) \
__N__= MyAlloc(LPTSTR, strlen(__S__) + 1); \
if (__N__) \
{ \
  StringCchCopyA(__N__, __I__, __S__); \
} 

#define FREE_STR(__N__) \
  if (__N__) { MyFree(__N__); }


