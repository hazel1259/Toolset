#pragma once

class CSize
{
public:
	//operator const POINT();
	//operator const LPPOINT();
  operator const SIZE();
  operator const LPSIZE();
  CSize() ; 
  ~CSize() ; 
  CSize(int x, int y);
  CSize(SIZE size);
// operators
public:
  BOOL operator == (SIZE size); 
  BOOL operator == (CSize sz); 
  CSize operator + (SIZE sz); 
  CSize operator + (CSize sz); 
  CSize operator - (SIZE sz); 
  CSize operator - (CSize sz); 
  CSize operator = (SIZE sz);
  CSize operator = (LPSIZE lpsz);
// methods
public:
  void Clear(); 
  void Init(int x, int y); 
  void Init(SIZE sz);
  void InitFrom(HWND);
  void InitFromWindow(HWND);
  void Grow(int dx, int dy); 
// variables
public:
  int cx;
  int cy;
  int Width;
  int Height;
private:
	SIZE m_size;
};
