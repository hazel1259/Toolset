#pragma once

class CRect
{
public:
	CRect(); 
	~CRect();
	// builds CRect with the specified dimensions
	CRect(int left, int top, int right, int bottom);
	// builds a CRect with the specified size
	CRect(SIZE * lpSize);
	// builds CRect from the rectangle
	CRect(RECT * rect);
	//casting operator to LPRECT
	operator const LPRECT();
	// casting operator to a RECT
	operator const RECT (); 
	// equivalence relation
	BOOL operator == (RECT * prc);  
	// inequality relation
	BOOL operator != (RECT * prc); 
	// assignment
	void operator = (RECT * prc); 
	// assingment 
// public methods	
public:	
	// initializes the rect to the specified dimensions
	void Init(int left, int top, int right, int bottom);
	// initializes the dimensions by normalizing lpp1, lpp2
	void Init(LPPOINT lpp1, LPPOINT lpp2);
	// initializes the rect object to the specified width and height
	void Init(int w, int h);
	// initializes the rect object to the size
	void Init(SIZE *lpdz);
	// initializes the rect to the internal dimensions (if 0, this clears the rect)
	void Init();
	// initializes the rect using the LPRECT diminsions
	void Init(LPRECT lprc);
	// initliazes the dimensions of the object using GetClientRect()
	void InitFrom(HWND hwnd);
	// intializes the dimensions of the object using GetWindowRect()
	void InitFromWindow(HWND hwnd);
	// recalculates the size
	void CalcSize();
	// copies the CRect object to this object
	void Copy(CRect * prect);
	// copies the RECT object to this object
	void Copy(RECT * prc)	;
	// infates the rect by the given dimensions
	void Inflate(int x, int y);
	// infates the rect by the given size
	void Inflate(SIZE & sz);
	// tests the rect to see if the point is in the rectangle
	BOOL InRect(POINT & pt);
	// tests the rect to see if the point is in the rectangle
	BOOL InRect(LPPOINT lppt);
	// tests the rect to see if the points are in the rectangle
	BOOL InRect(int x, int y);
	// moves the rectangle by the distance given
	void Move(int x, int y);
	// resets the dimensios to 0
	void Clear();
	// creates a new object with exactly the same dimensions
	CRect * Clone();

	// variables
public:
	int Left;
	int Right;
	int Top;
	int Bottom;
	int Width;
	int Height;
	int Area;
private:
  RECT m_rect;
};