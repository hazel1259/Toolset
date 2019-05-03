#include "comm_inc.h"
#include "rect.h"

CRect::CRect() 
{
	Clear();
}

CRect::~CRect() 
{
}

CRect::CRect(int left, int top, int right, int bottom)
{
	Init(left, top, right, bottom);
}
  
CRect::CRect(RECT * rect)
{
	Init(rect);
}

CRect::CRect(SIZE * lpSize)
{
	RECT rc = {0};
	rc.right = lpSize->cx;
	rc.bottom = lpSize->cy;
	Init(&rc);
}

CRect::operator const RECT()
{
	return (const RECT)m_rect;
}

CRect::operator const LPRECT()
{
	return (const LPRECT)&m_rect;
}

BOOL CRect:: operator == (RECT * prc) 
{	
	return EqualRect(prc, &m_rect); 
}

BOOL CRect:: operator != (RECT * prc) 
{ 
	return !EqualRect(prc, &m_rect); 
}

void CRect:: operator = (RECT * prc) 
{ 
	Init(prc); 
}
void CRect:: Init(int left, int top, int right, int bottom)
{
	RECT rc;
	rc.left = left;
	rc.top = top;
	rc.bottom = bottom;
	rc.right = right;
	Init(&rc);
}

void CRect::Init()
{
	CalcSize();
}

void CRect::Init(int w, int h)
{
	SIZE sz;
	sz.cx = w;
	sz.cy = h;
	Init(&sz);
}

void CRect::Init(SIZE * lpsz)
{
	Init(0, 0, lpsz->cx, lpsz->cy);
}

void CRect::Init(LPPOINT lpp1, LPPOINT lpp2)
{
	int left = min(lpp1->x, lpp2->x);
	int right = max(lpp1->x, lpp2->x);
	int top = min(lpp1->y, lpp2->y);
	int bottom = max(lpp1->y, lpp2->y);
	Init(left, top, right, bottom);
}

void CRect:: CalcSize()
{
	Init(Left, Top, Right, Bottom);
}


void CRect:: InitFrom(HWND hwnd)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc))
	{
		Init(&rc);
	}
}
  
void CRect:: Init(LPRECT lprc)
{
	Clear();
	CopyRect(&m_rect, lprc);
	Left = lprc->left;
	Right = lprc->right;
	Bottom = lprc->bottom;
	Top = lprc->top;
	Width = lprc->right - lprc->left;
	Height = lprc->bottom - lprc->top;
	Area = 0;
}
	
void CRect:: Copy(RECT * prc)
{
	Init(prc);
}

void CRect:: Copy(CRect * prect)
{
	const RECT rc = (const RECT)*this;
	Copy((LPRECT)&rc);
}
	
void CRect:: Inflate(int x, int y)
{
	RECT rc;
	rc.left		= Left;
	rc.right	= Right;
	rc.top		= Top;
	rc.bottom	= Bottom;

	InflateRect(&rc, x, y);
	Init(&rc);
}
	
BOOL CRect:: InRect(POINT & pt)
{
	return PtInRect(&m_rect, pt);
}
 
BOOL CRect::InRect(LPPOINT lppt)
{
	if (lppt)
	{
		return ::PtInRect(&m_rect, *lppt);
	}
	return FALSE;
}

BOOL CRect::InRect(int x, int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	return InRect(pt);
}
void CRect:: Move(int x, int y)
{
	RECT rc;
	rc.left = Left + x;
	rc.right = Right + x;
	rc.top = Top + y;
	rc.bottom = Bottom + y;
	Init(&rc);
}
	
void CRect:: Inflate(SIZE & sz)
{
	Inflate(sz.cx, sz.cy);
}

void CRect:: Clear()
{
	Left = 0;
	Right = 0;
	Top = 0;
	Bottom = 0;
	Width = 0;
	Height = 0;
	Area = 0;
	SetRectEmpty(&m_rect);
	//Init(0, 0, 0, 0);
}
  

void CRect:: InitFromWindow(HWND hwnd)
{
	RECT rc;
	if (GetWindowRect(hwnd, &rc))
	{
		Init(&rc);
	}
}

CRect * CRect:: Clone()
{
	return new CRect(&m_rect);
}