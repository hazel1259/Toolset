#include "comm_inc.h"
#include "size.h"

CSize::CSize() 
{ 
	Init(0, 0);
}

CSize::~CSize() 
{
}

CSize::CSize(int x, int y) 
{ 
	Init(x, y);
}

CSize::CSize(SIZE size) 
{ 
	Init(size);
}

CSize::operator const LPSIZE()
{
	return (const LPSIZE)&m_size;
}

CSize::operator const SIZE()
{
	return (const SIZE)m_size;
}

BOOL CSize::operator == (SIZE size) 
{ 
	return cx ==  size.cx && cy == size.cy; 
}

BOOL CSize::operator == (CSize sz)   
{ 
	return cx ==  sz.cx && cy == sz.cy; 
}

CSize CSize::operator + (SIZE sz)
{ 
	cx += sz.cx; 
	cy += sz.cy; 
	Width = cx; 
	Height = cy; 
	m_size.cx = cx;
	m_size.cy = cy;
	return *this;
}

CSize CSize::operator + (CSize sz)
{ 
	cx += sz.cx; 
	cy += sz.cy; 
	Width = cx; 
	Height = cy; 
	m_size.cx = cx;
	m_size.cy = cy;
	return *this;
}

CSize CSize::operator - (SIZE sz)
{ 
	cx -= sz.cx; cy -= sz.cy; 
	Width = cx; 
	Height = cy; 
	m_size.cx = cx;
	m_size.cy = cy;
	return *this;
}

CSize CSize::operator - (CSize sz)
{ 
	cx -= sz.cx; 
	cy -= sz.cy; 
	Width = cx; 
	Height = cy; 
	m_size.cx = cx;
	m_size.cy = cy;
	return *this;
}

CSize CSize::operator = (SIZE sz)
{
	Init(sz);
	return *this;
}

CSize CSize::operator = (LPSIZE lpsz)
{
	Width = lpsz->cx;
	Height = lpsz->cy;
	return *this;
}

void CSize::Clear() 
{
	Init(0, 0);
}

void CSize::Grow(int dx, int dy) 
{ 
	Init(cx + dx, cy + dy);
}

void CSize::Init(int x, int y) 
{
	SIZE sz;
	sz.cx = x;
	sz.cy = y;
	Init(sz);
}

void CSize::Init(SIZE sz)
{
	memcpy_s(&m_size, sizeof(SIZE), &sz, sizeof(SIZE));
	cx = Width = sz.cx; 
	cy = Height = sz.cy; 
}

void CSize::InitFrom(HWND hwnd)
{
    RECT rc = { 0 };
    if (IsWindow(hwnd) && GetClientRect(hwnd, &rc))
    {
        Init(rc.right - rc.left, rc.bottom - rc.top);
    }
}

void CSize::InitFromWindow(HWND hwnd)
{
    RECT rc = { 0 };
    if (IsWindow(hwnd) && GetWindowRect(hwnd, &rc))
    {
        Init(rc.right - rc.left, rc.bottom - rc.top);
    }
}
