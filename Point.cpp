#include "comm_inc.h"
#include "Point.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//					CPointW Point class starts here for short members
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPointW::CPointW() 
{
	X = 0; 
	Y = 0;
}
	
CPointW::CPointW(POINT pt)
{
	Init(&pt);
}
	
CPointW::CPointW (WORD  x, WORD y)
{
	Init(x, y);	
}
	
CPointW::CPointW (LPARAM lp)
{
	Init(lp);
}

CPointW::operator const LPPOINT()
{
	return (const LPPOINT)&m_point;
}

CPointW::operator const POINT()
{
	return (const POINT) m_point;
}

BOOL	CPointW::operator == (CPointW pt) 
{ 
	return pt.X == X && pt.Y == Y; 
}

BOOL	CPointW::operator == (POINT pt) 
{ 
	return pt.x == X && pt.y== Y; 
}

BOOL	CPointW::operator != (CPointW pt) 
{ 
	return pt.X != X || pt.Y != Y;
}

BOOL	CPointW::operator != (POINT pt) 
{ 
	return pt.x != X || pt.y != Y; 
}

CPointW	CPointW::operator + (CPointW pt) 
{
	pt.X += X; pt.Y += Y; return *this;
}

CPointW	CPointW::operator - (CPointW pt) 
{
	pt.X -= X; pt.Y -= Y; return *this;
}

CPointW	CPointW::operator + (POINT pt) 
{
	pt.x += X; pt.y += Y; return *this;
}

CPointW	CPointW::operator - (POINT pt) 
{ 
	pt.x -= X; pt.y -= Y; return *this;
}

void	CPointW::operator = (POINT pt) 
{ 
	Init(&pt);
}

void CPointW::Init(LONG_PTR lp)
{
	WORD x = GET_X_LPARAM(lp);
	WORD y = GET_Y_LPARAM(lp);
	Init(x, y);
}

void CPointW::Init()
{
	m_point.x = X;
	m_point.y = Y;
}

void CPointW::Init(WORD x, WORD y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	Init(&pt);
}

void CPointW::Init(POINT & pt)
{
	Init(&pt);
}

void CPointW::Init(LPPOINT lppt)
{
	memcpy_s(&m_point, sizeof(POINT), lppt, sizeof(POINT));
	X = LOWORD(lppt->x);
	Y = LOWORD(lppt->y);
}

CPointW * CPointW::Clone()
{
	return new CPointW(X, Y);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//					CPoint Point class starts here for integer members
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPoint::CPoint() 
{
	X = 0; 
	Y = 0;
}
	
CPoint::CPoint(POINT pt)
{
	Init(&pt);
}
	
CPoint::CPoint (int x, int y)
{
	Init(x, y);	
}
	
CPoint::CPoint (LPARAM lp)
{
	Init(lp);
}

CPoint::operator const LPPOINT()
{
	return (const LPPOINT)&m_point;
}

CPoint::operator const POINT()
{
	return (const POINT) m_point;
}

BOOL	CPoint::operator == (CPoint pt) 
{ 
	return pt.X == X && pt.Y == Y; 
}

BOOL	CPoint::operator == (POINT pt) 
{ 
	return pt.x == X && pt.y== Y; 
}

BOOL	CPoint::operator != (CPoint pt) 
{ 
	return pt.X != X || pt.Y != Y;
}

BOOL	CPoint::operator != (POINT pt) 
{ 
	return pt.x != X || pt.y != Y; 
}

CPoint	CPoint::operator + (CPoint pt) 
{
	pt.X += X; pt.Y += Y; return *this;
}

CPoint	CPoint::operator - (CPoint pt) 
{
	pt.X -= X; pt.Y -= Y; return *this;
}

CPoint	CPoint::operator + (POINT pt) 
{
	pt.x += X; pt.y += Y; return *this;
}

CPoint	CPoint::operator - (POINT pt) 
{ 
	pt.x -= X; pt.y -= Y; return *this;
}

void	CPoint::operator = (POINT pt) 
{ 
	Init(&pt);
}

void CPoint::Init()
{
	m_point.x = X;
	m_point.y = Y;
}

void CPoint::Init(int x, int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	Init(&pt);
}

void CPoint::Init(POINT & pt)
{
	Init(&pt);
}

void CPoint::Init(LPPOINT lppt)
{
	memcpy_s(&m_point, sizeof(POINT), lppt, sizeof(POINT));
	X = lppt->x;
	Y = lppt->y;
}

void CPoint::Init(LPARAM lp)
{
	int x = GET_X_LPARAM(lp);
	int y = GET_Y_LPARAM(lp);
	Init(x, y);
}

CPoint * CPoint::Clone()
{
	return new CPoint(X, Y);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//					CPointF Point class starts here for FLOAT members
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPointF::CPointF() 
{
	X = 0; 
	Y = 0;
}
	
CPointF::CPointF(POINT pt)
{
	Init(&pt);
}
	
CPointF::CPointF (FLOAT x, FLOAT y)
{
	Init(x, y);	
}
	
CPointF::CPointF (LPARAM lp)
{
	Init(lp);
}

CPointF::operator const LPPOINT()
{
	return (const LPPOINT)&m_point;
}

CPointF::operator const POINT()
{
	return (const POINT) m_point;
}

BOOL CPointF::operator == (CPointF pt) 
{ 
	return pt.X == X && pt.Y == Y; 
}

BOOL	CPointF::operator == (POINT pt) 
{ 
	return pt.x == X && pt.y== Y; 
}

BOOL	CPointF::operator != (CPointF pt) 
{ 
	return pt.X != X || pt.Y != Y;
}

BOOL	CPointF::operator != (POINT pt) 
{ 
	return pt.x != X || pt.y != Y; 
}


CPointF	CPointF::operator + (CPointF pt) 
{
	LONG x_l, y_l;
	x_l = (LONG)X;
	y_l = (LONG)Y;
	pt.X += x_l; pt.Y += y_l; return *this;
}

CPointF	CPointF::operator - (CPointF pt) 
{
	LONG x_l, y_l;
	x_l = (LONG)X;
	y_l = (LONG)Y;
	pt.X -= x_l; pt.Y -= y_l; return *this;
}

CPointF	CPointF::operator + (POINT pt) 
{
	LONG x_l, y_l;
	x_l = (LONG)X;
	y_l = (LONG)Y;
	pt.x += x_l; pt.y += y_l; return *this;

}

CPointF	CPointF::operator - (POINT pt) 
{ 
	LONG x_l, y_l;
	x_l = (LONG)X;
	y_l = (LONG)Y;
	pt.x -= x_l; pt.y -= y_l; return *this;
}

void	CPointF::operator = (POINT pt) 
{ 
	Init(&pt);
}

void CPointF::Init()
{
	LONG x_l, y_l;
	x_l = (LONG)X;
	y_l = (LONG)Y;
	m_point.x = x_l;
	m_point.y = y_l;
}

void CPointF::Init(FLOAT x, FLOAT y)
{
	int x_l = (LONG)x;
	int y_l = (LONG)y;
	m_point.x = x_l;
	m_point.y = y_l;
	X = x;
	Y = y;
}

void CPointF::Init(POINT & pt)
{
	FLOAT x_f = (FLOAT)pt.x;
	FLOAT y_f = (FLOAT)pt.y;
	Init(x_f, y_f);
}

void CPointF::Init(LPPOINT lppt)
{
	memcpy_s(&m_point, sizeof(POINT), lppt, sizeof(POINT));
	// prevents strange behavior when copying directly from int to float
	FLOAT x_f = (FLOAT)lppt->x;
	FLOAT y_f = (FLOAT)lppt->y;
	X = x_f;
	Y = y_f;
}

void CPointF::Init(LPARAM lp)
{
	int x = GET_X_LPARAM(lp);
	int y = GET_Y_LPARAM(lp);
	FLOAT f_x = (FLOAT)x;
	FLOAT f_y = (FLOAT)y;
	Init(f_x, f_y);

}

CPointF * CPointF::Clone()
{
	return new CPointF(X, Y);
}
