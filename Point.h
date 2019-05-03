#pragma once

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

class CPointW
{
	CPointW() ;
	CPointW(POINT pt);
	CPointW(WORD x, WORD y);
	CPointW (LONG_PTR lp);
	WORD X;
	WORD Y;
// operators
public:
	operator const POINT();
	operator const LPPOINT();
	BOOL	operator == (CPointW pt);
	BOOL	operator == (POINT pt) ;
	BOOL	operator != (CPointW pt) ; 
	BOOL	operator != (POINT pt) ; 
	CPointW	operator + (CPointW pt) ; 
	CPointW	operator - (CPointW pt) ; 
	CPointW	operator + (POINT pt) ; 
	CPointW	operator - (POINT pt) ; 
	void	operator = (POINT pt) ; 
// methods
public:
	void Init();
	void Init(WORD x, WORD y);
	// initializes using a POINT but values are cast to short, be careful
	void Init(POINT & pt);
	// initializes using a POINT but values are cast to short, be careful
	void Init(LPPOINT lppt);
	void Init(LPARAM lp);
	CPointW * Clone();
private:
	POINT m_point;
};

class CPoint 
{
public:
	CPoint() ;
	CPoint(POINT pt);
	CPoint (int x, int y);
	CPoint (LPARAM lp);
// operators
public:
	operator const POINT();
	operator const LPPOINT();
	BOOL	operator == (CPoint pt);
	BOOL	operator == (POINT pt) ;
	BOOL	operator != (CPoint pt) ; 
	BOOL	operator != (POINT pt) ; 
	CPoint	operator + (CPoint pt) ; 
	CPoint	operator - (CPoint pt) ; 
	CPoint	operator + (POINT pt) ; 
	CPoint	operator - (POINT pt) ; 
	void	operator = (POINT pt) ; 
// methods
public:
	void Init();
	void Init(int x, int y);
	void Init(POINT & pt);
	void Init(LPPOINT lppt);
	void Init(LPARAM lp);
	CPoint * Clone();
public:
	int X;
	int Y;
private:
	POINT m_point;
};

class CPointF
{
	CPointF() ;
	CPointF(POINT pt);
	CPointF (FLOAT x, FLOAT y);
	CPointF (LPARAM lp);
	FLOAT X;
	FLOAT Y;
	operator const POINT();
	operator const LPPOINT();
	BOOL	operator == (CPointF pt);
	BOOL	operator == (POINT pt) ;
	BOOL	operator != (CPointF pt) ; 
	BOOL	operator != (POINT pt) ; 
	CPointF	operator + (CPointF pt) ; 
	CPointF	operator - (CPointF pt) ; 
	CPointF	operator + (POINT pt) ; 
	CPointF	operator - (POINT pt) ; 
	void	operator = (POINT pt) ; 
// methods
public:
	void Init();
	void Init(FLOAT x, FLOAT y);
	void Init(POINT & pt);
	void Init(LPPOINT lppt);
	void Init(LPARAM lp);
	CPointF * Clone();
private:
	POINT m_point;
};

