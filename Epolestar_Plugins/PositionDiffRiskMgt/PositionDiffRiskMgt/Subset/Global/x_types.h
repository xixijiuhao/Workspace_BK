#pragma once


#include <windef.h>

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

class TSize;
class TPoint;
class TRect;

/////////////////////////////////////////////////////////////////////////////
// TSize - An extent, similar to Windows SIZE structure.

class TSize :
	public tagSIZE
{
public:

// Constructors
	// construct an uninitialized size
	TSize() throw();
	// create from two integers
	TSize(
		_In_ int initCX,
		_In_ int initCY) throw();
	// create from another size
	TSize(_In_ SIZE initSize) throw();
	// create from a point
	TSize(_In_ POINT initPt) throw();
	// create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
	TSize(_In_ DWORD dwSize) throw();

// Operations
	BOOL operator==(_In_ SIZE size) const throw();
	BOOL operator!=(_In_ SIZE size) const throw();
	void operator+=(_In_ SIZE size) throw();
	void operator-=(_In_ SIZE size) throw();
	void SetSize(_In_ int CX, _In_ int CY) throw();

// Operators returning TSize values
	TSize operator+(_In_ SIZE size) const throw();
	TSize operator-(_In_ SIZE size) const throw();
	TSize operator-() const throw();

// Operators returning TPoint values
	TPoint operator+(_In_ POINT point) const throw();
	TPoint operator-(_In_ POINT point) const throw();

// Operators returning TRect values
	TRect operator+(_In_ const RECT* lpRect) const throw();
	TRect operator-(_In_ const RECT* lpRect) const throw();
};

/////////////////////////////////////////////////////////////////////////////
// TPoint - A 2-D point, similar to Windows POINT structure.

class TPoint :
	public tagPOINT
{
public:
// Constructors

	// create an uninitialized point
	TPoint() throw();
	// create from two integers
	TPoint(
		_In_ int initX,
		_In_ int initY) throw();
	// create from another point
	TPoint(_In_ POINT initPt) throw();
	// create from a size
	TPoint(_In_ SIZE initSize) throw();
	// create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
	TPoint(_In_ LPARAM dwPoint) throw();


// Operations

// translate the point
	void Offset(
		_In_ int xOffset,
		_In_ int yOffset) throw();
	void Offset(_In_ POINT point) throw();
	void Offset(_In_ SIZE size) throw();
	void SetPoint(
		_In_ int X,
		_In_ int Y) throw();

	BOOL operator==(_In_ POINT point) const throw();
	BOOL operator!=(_In_ POINT point) const throw();
	void operator+=(_In_ SIZE size) throw();
	void operator-=(_In_ SIZE size) throw();
	void operator+=(_In_ POINT point) throw();
	void operator-=(_In_ POINT point) throw();

// Operators returning TPoint values
	TPoint operator+(_In_ SIZE size) const throw();
	TPoint operator-(_In_ SIZE size) const throw();
	TPoint operator-() const throw();
	TPoint operator+(_In_ POINT point) const throw();

// Operators returning TSize values
	TSize operator-(_In_ POINT point) const throw();

// Operators returning TRect values
	TRect operator+(_In_ const RECT* lpRect) const throw();
	TRect operator-(_In_ const RECT* lpRect) const throw();
};

/////////////////////////////////////////////////////////////////////////////
// TRect - A 2-D rectangle, similar to Windows RECT structure.

class TRect :
	public tagRECT
{
// Constructors
public:
	// uninitialized rectangle
	TRect() throw();
	// from left, top, right, and bottom
	TRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw();
	// copy constructor
	TRect(_In_ const RECT& srcRect) throw();

	// from a pointer to another rect
	TRect(_In_ LPCRECT lpSrcRect) throw();
	// from a point and size
	TRect(
		_In_ POINT point,
		_In_ SIZE size) throw();
	// from two points
	TRect(
		_In_ POINT topLeft,
		_In_ POINT bottomRight) throw();

// Attributes (in addition to RECT members)

	// retrieves the width
	int Width() const throw();
	// returns the height
	int Height() const throw();
	// returns the size
	TSize Size() const throw();
	// reference to the top-left point
	TPoint& TopLeft() throw();
	// reference to the bottom-right point
	TPoint& BottomRight() throw();
	// const reference to the top-left point
	const TPoint& TopLeft() const throw();
	// const reference to the bottom-right point
	const TPoint& BottomRight() const throw();
	// the geometric center point of the rectangle
	TPoint CenterPoint() const throw();
	// swap the left and right
	void SwapLeftRight() throw();
	static void WINAPI SwapLeftRight(_Inout_ LPRECT lpRect) throw();

	// convert between TRect and LPRECT/LPCRECT (no need for &)
	operator LPRECT() throw();
	operator LPCRECT() const throw();

	// returns TRUE if rectangle has no area
	BOOL IsRectEmpty() const throw();
	// returns TRUE if rectangle is at (0,0) and has no area
	BOOL IsRectNull() const throw();
	// returns TRUE if point is within rectangle
	BOOL PtInRect(_In_ POINT point) const throw();

// Operations

	// set rectangle from left, top, right, and bottom
	void SetRect(
		_In_ int x1,
		_In_ int y1,
		_In_ int x2,
		_In_ int y2) throw();
	void SetRect(
		_In_ POINT topLeft,
		_In_ POINT bottomRight) throw();
	// empty the rectangle
	void SetRectEmpty() throw();
	// copy from another rectangle
	void CopyRect(_In_ LPCRECT lpSrcRect) throw();
	// TRUE if exactly the same as another rectangle
	BOOL EqualRect(_In_ LPCRECT lpRect) const throw();

	// Inflate rectangle's width and height by
	// x units to the left and right ends of the rectangle
	// and y units to the top and bottom.
	void InflateRect(
		_In_ int x,
		_In_ int y) throw();
	// Inflate rectangle's width and height by
	// size.cx units to the left and right ends of the rectangle
	// and size.cy units to the top and bottom.
	void InflateRect(_In_ SIZE size) throw();
	// Inflate rectangle's width and height by moving individual sides.
	// Left side is moved to the left, right side is moved to the right,
	// top is moved up and bottom is moved down.
	void InflateRect(_In_ LPCRECT lpRect) throw();
	void InflateRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw();

	// deflate the rectangle's width and height without
	// moving its top or left
	void DeflateRect(
		_In_ int x,
		_In_ int y) throw();
	void DeflateRect(_In_ SIZE size) throw();
	void DeflateRect(_In_ LPCRECT lpRect) throw();
	void DeflateRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw();

	// translate the rectangle by moving its top and left
	void OffsetRect(
		_In_ int x,
		_In_ int y) throw();
	void OffsetRect(_In_ SIZE size) throw();
	void OffsetRect(_In_ POINT point) throw();
	void NormalizeRect() throw();

	// absolute position of rectangle
	void MoveToY(_In_ int y) throw();
	void MoveToX(_In_ int x) throw();
	void MoveToXY(
		_In_ int x,
		_In_ int y) throw();
	void MoveToXY(_In_ POINT point) throw();

	// set this rectangle to intersection of two others
	BOOL IntersectRect(
		_In_ LPCRECT lpRect1,
		_In_ LPCRECT lpRect2) throw();

	// set this rectangle to bounding union of two others
	BOOL UnionRect(
		_In_ LPCRECT lpRect1,
		_In_ LPCRECT lpRect2) throw();

	// set this rectangle to minimum of two others
	BOOL SubtractRect(
		_In_ LPCRECT lpRectSrc1,
		_In_ LPCRECT lpRectSrc2) throw();

// Additional Operations
	void operator=(_In_ const RECT& srcRect) throw();
	BOOL operator==(_In_ const RECT& rect) const throw();
	BOOL operator!=(_In_ const RECT& rect) const throw();
	void operator+=(_In_ POINT point) throw();
	void operator+=(_In_ SIZE size) throw();
	void operator+=(_In_ LPCRECT lpRect) throw();
	void operator-=(_In_ POINT point) throw();
	void operator-=(_In_ SIZE size) throw();
	void operator-=(_In_ LPCRECT lpRect) throw();
	void operator&=(_In_ const RECT& rect) throw();
	void operator|=(_In_ const RECT& rect) throw();

// Operators returning TRect values
	TRect operator+(_In_ POINT point) const throw();
	TRect operator-(_In_ POINT point) const throw();
	TRect operator+(_In_ LPCRECT lpRect) const throw();
	TRect operator+(_In_ SIZE size) const throw();
	TRect operator-(_In_ SIZE size) const throw();
	TRect operator-(_In_ LPCRECT lpRect) const throw();
	TRect operator&(_In_ const RECT& rect2) const throw();
	TRect operator|(_In_ const RECT& rect2) const throw();
	TRect MulDiv(
		_In_ int nMultiplier,
		_In_ int nDivisor) const throw();
};

/////////////////////////////////////////////////////////////////////////////////////////////
// TSize
inline TSize::TSize() throw()
{
	cx = 0;
	cy = 0;
}

inline TSize::TSize(
	_In_ int initCX,
	_In_ int initCY) throw()
{
	cx = initCX;
	cy = initCY;
}

inline TSize::TSize(_In_ SIZE initSize) throw()
{
	*(SIZE*)this = initSize;
}

inline TSize::TSize(_In_ POINT initPt) throw()
{
	*(POINT*)this = initPt;
}

inline TSize::TSize(_In_ DWORD dwSize) throw()
{
		cx = (short)LOWORD(dwSize);
		cy = (short)HIWORD(dwSize);
}

inline BOOL TSize::operator==(_In_ SIZE size) const throw()
{
	return (cx == size.cx && cy == size.cy);
}

inline BOOL TSize::operator!=(_In_ SIZE size) const throw()
{
	return (cx != size.cx || cy != size.cy);
}

inline void TSize::operator+=(_In_ SIZE size) throw()
{
	cx += size.cx;
	cy += size.cy;
}

inline void TSize::operator-=(_In_ SIZE size) throw()
{
	cx -= size.cx;
	cy -= size.cy;
}

inline void TSize::SetSize(
	_In_ int CX,
	_In_ int CY) throw()
{
	cx = CX;
	cy = CY;
}

inline TSize TSize::operator+(_In_ SIZE size) const throw()
{
	return TSize(cx + size.cx, cy + size.cy);
}

inline TSize TSize::operator-(_In_ SIZE size) const throw()
{
	return TSize(cx - size.cx, cy - size.cy);
}

inline TSize TSize::operator-() const throw()
{
	return TSize(-cx, -cy);
}

inline TPoint TSize::operator+(_In_ POINT point) const throw()
{
	return TPoint(cx + point.x, cy + point.y);
}

inline TPoint TSize::operator-(_In_ POINT point) const throw()
{
	return TPoint(cx - point.x, cy - point.y);
}

inline TRect TSize::operator+(_In_ const RECT* lpRect) const throw()
{
	return TRect(lpRect) + *this;
}

inline TRect TSize::operator-(_In_ const RECT* lpRect) const throw()
{
	return TRect(lpRect) - *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// TPoint
inline TPoint::TPoint() throw()
{
	x = 0;
	y = 0;
}

inline TPoint::TPoint(
	_In_ int initX,
	_In_ int initY) throw()
{
	x = initX;
	y = initY;
}

inline TPoint::TPoint(_In_ POINT initPt) throw()
{
	*(POINT*)this = initPt;
}

inline TPoint::TPoint(_In_ SIZE initSize) throw()
{
	*(SIZE*)this = initSize;
}

inline TPoint::TPoint(_In_ LPARAM dwPoint) throw()
{
	x = (short)LOWORD(dwPoint);
	y = (short)HIWORD(dwPoint);
}

inline void TPoint::Offset(
	_In_ int xOffset,
	_In_ int yOffset) throw()
{
	x += xOffset;
	y += yOffset;
}

inline void TPoint::Offset(_In_ POINT point) throw()
{
	x += point.x;
	y += point.y;
}

inline void TPoint::Offset(_In_ SIZE size) throw()
{
	x += size.cx;
	y += size.cy;
}

inline void TPoint::SetPoint(
	_In_ int X,
	_In_ int Y) throw()
{
	x = X;
	y = Y;
}

inline BOOL TPoint::operator==(_In_ POINT point) const throw()
{
	return (x == point.x && y == point.y);
}

inline BOOL TPoint::operator!=(_In_ POINT point) const throw()
{
	return (x != point.x || y != point.y);
}

inline void TPoint::operator+=(_In_ SIZE size) throw()
{
	x += size.cx;
	y += size.cy;
}

inline void TPoint::operator-=(_In_ SIZE size) throw()
{
	x -= size.cx;
	y -= size.cy;
}

inline void TPoint::operator+=(_In_ POINT point) throw()
{
	x += point.x;
	y += point.y;
}

inline void TPoint::operator-=(_In_ POINT point) throw()
{
	x -= point.x;
	y -= point.y;
}

inline TPoint TPoint::operator+(_In_ SIZE size) const throw()
{
	return TPoint(x + size.cx, y + size.cy);
}

inline TPoint TPoint::operator-(_In_ SIZE size) const throw()
{
	return TPoint(x - size.cx, y - size.cy);
}

inline TPoint TPoint::operator-() const throw()
{
	return TPoint(-x, -y);
}

inline TPoint TPoint::operator+(_In_ POINT point) const throw()
{
	return TPoint(x + point.x, y + point.y);
}

inline TSize TPoint::operator-(_In_ POINT point) const throw()
{
	return TSize(x - point.x, y - point.y);
}

inline TRect TPoint::operator+(_In_ const RECT* lpRect) const throw()
{
	return TRect(lpRect) + *this;
}

inline TRect TPoint::operator-(_In_ const RECT* lpRect) const throw()
{
	return TRect(lpRect) - *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// TRect
inline TRect::TRect() throw()
{
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
}

inline TRect::TRect(
	_In_ int l,
	_In_ int t,
	_In_ int r,
	_In_ int b) throw()
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

inline TRect::TRect(_In_ const RECT& srcRect) throw()
{
	::CopyRect(this, &srcRect);
}

inline TRect::TRect(_In_ LPCRECT lpSrcRect) throw()
{
	::CopyRect(this, lpSrcRect);
}

inline TRect::TRect(
	_In_ POINT point,
	_In_ SIZE size) throw()
{
	right = (left = point.x) + size.cx;
	bottom = (top = point.y) + size.cy;
}

inline TRect::TRect(
	_In_ POINT topLeft,
	_In_ POINT bottomRight) throw()
{
	left = topLeft.x;
	top = topLeft.y;
	right = bottomRight.x;
	bottom = bottomRight.y;
}

inline int TRect::Width() const throw()
{
	return right - left;
}

inline int TRect::Height() const throw()
{
	return bottom - top;
}

inline TSize TRect::Size() const throw()
{
	return TSize(right - left, bottom - top);
}

inline TPoint& TRect::TopLeft() throw()
{
	return *((TPoint*)this);
}

inline TPoint& TRect::BottomRight() throw()
{
	return *((TPoint*)this+1);
}

inline const TPoint& TRect::TopLeft() const throw()
{
	return *((TPoint*)this);
}

inline const TPoint& TRect::BottomRight() const throw()
{
	return *((TPoint*)this+1);
}

inline TPoint TRect::CenterPoint() const throw()
{
	return TPoint((left+right)/2, (top+bottom)/2);
}

inline void TRect::SwapLeftRight() throw()
{
	SwapLeftRight(LPRECT(this));
}

inline void WINAPI TRect::SwapLeftRight(_Inout_ LPRECT lpRect) throw()
{
	LONG temp = lpRect->left;
	lpRect->left = lpRect->right;
	lpRect->right = temp;
}

inline TRect::operator LPRECT() throw()
{
	return this;
}

inline TRect::operator LPCRECT() const throw()
{
	return this;
}

inline BOOL TRect::IsRectEmpty() const throw()
{
	return ::IsRectEmpty(this);
}

inline BOOL TRect::IsRectNull() const throw()
{
	return (left == 0 && right == 0 && top == 0 && bottom == 0);
}

inline BOOL TRect::PtInRect(_In_ POINT point) const throw()
{
	return ::PtInRect(this, point);
}

inline void TRect::SetRect(
	_In_ int x1,
	_In_ int y1,
	_In_ int x2,
	_In_ int y2) throw()
{
	::SetRect(this, x1, y1, x2, y2);
}

inline void TRect::SetRect(
	_In_ POINT topLeft,
	_In_ POINT bottomRight) throw()
{
	::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

inline void TRect::SetRectEmpty() throw()
{
	::SetRectEmpty(this);
}

inline void TRect::CopyRect(_In_ LPCRECT lpSrcRect) throw()
{
	::CopyRect(this, lpSrcRect);
}

inline BOOL TRect::EqualRect(_In_ LPCRECT lpRect) const throw()
{
	return ::EqualRect(this, lpRect);
}

inline void TRect::InflateRect(
	_In_ int x,
	_In_ int y) throw()
{
	::InflateRect(this, x, y);
}

inline void TRect::InflateRect(_In_ SIZE size) throw()
{
	::InflateRect(this, size.cx, size.cy);
}

inline void TRect::DeflateRect(
	_In_ int x,
	_In_ int y) throw()
{
	::InflateRect(this, -x, -y);
}

inline void TRect::DeflateRect(_In_ SIZE size) throw()
{
	::InflateRect(this, -size.cx, -size.cy);
}

inline void TRect::OffsetRect(
	_In_ int x,
	_In_ int y) throw()
{
	::OffsetRect(this, x, y);
}

inline void TRect::OffsetRect(_In_ POINT point) throw()
{
	::OffsetRect(this, point.x, point.y);
}

inline void TRect::OffsetRect(_In_ SIZE size) throw()
{
	::OffsetRect(this, size.cx, size.cy);
}

inline void TRect::MoveToY(_In_ int y) throw()
{
	bottom = Height() + y;
	top = y;
}

inline void TRect::MoveToX(_In_ int x) throw()
{
	right = Width() + x;
	left = x;
}

inline void TRect::MoveToXY(
	_In_ int x,
	_In_ int y) throw()
{
	MoveToX(x);
	MoveToY(y);
}

inline void TRect::MoveToXY(_In_ POINT pt) throw()
{
	MoveToX(pt.x);
	MoveToY(pt.y);
}

inline BOOL TRect::IntersectRect(
	_In_ LPCRECT lpRect1,
	_In_ LPCRECT lpRect2) throw()
{
	return ::IntersectRect(this, lpRect1, lpRect2);
}

inline BOOL TRect::UnionRect(
	_In_ LPCRECT lpRect1,
	_In_ LPCRECT lpRect2) throw()
{
	return ::UnionRect(this, lpRect1, lpRect2);
}

inline void TRect::operator=(_In_ const RECT& srcRect) throw()
{
	::CopyRect(this, &srcRect);
}

inline BOOL TRect::operator==(_In_ const RECT& rect) const throw()
{
	return ::EqualRect(this, &rect);
}

inline BOOL TRect::operator!=(_In_ const RECT& rect) const throw()
{
	return !::EqualRect(this, &rect);
}

inline void TRect::operator+=(_In_ POINT point) throw()
{
	::OffsetRect(this, point.x, point.y);
}

inline void TRect::operator+=(_In_ SIZE size) throw()
{
	::OffsetRect(this, size.cx, size.cy);
}

inline void TRect::operator+=(_In_ LPCRECT lpRect) throw()
{
	InflateRect(lpRect);
}

inline void TRect::operator-=(_In_ POINT point) throw()
{
	::OffsetRect(this, -point.x, -point.y);
}

inline void TRect::operator-=(_In_ SIZE size) throw()
{
	::OffsetRect(this, -size.cx, -size.cy);
}

inline void TRect::operator-=(_In_ LPCRECT lpRect) throw()
{
	DeflateRect(lpRect);
}

inline void TRect::operator&=(_In_ const RECT& rect) throw()
{
	::IntersectRect(this, this, &rect);
}

inline void TRect::operator|=(_In_ const RECT& rect) throw()
{
	::UnionRect(this, this, &rect);
}

inline TRect TRect::operator+(_In_ POINT pt) const throw()
{
	TRect rect(*this);
	::OffsetRect(&rect, pt.x, pt.y);
	return rect;
}

inline TRect TRect::operator-(_In_ POINT pt) const throw()
{
	TRect rect(*this);
	::OffsetRect(&rect, -pt.x, -pt.y);
	return rect;
}

inline TRect TRect::operator+(_In_ SIZE size) const throw()
{
	TRect rect(*this);
	::OffsetRect(&rect, size.cx, size.cy);
	return rect;
}

inline TRect TRect::operator-(_In_ SIZE size) const throw()
{
	TRect rect(*this);
	::OffsetRect(&rect, -size.cx, -size.cy);
	return rect;
}

inline TRect TRect::operator+(_In_ LPCRECT lpRect) const throw()
{
	TRect rect(this);
	rect.InflateRect(lpRect);
	return rect;
}

inline TRect TRect::operator-(_In_ LPCRECT lpRect) const throw()
{
	TRect rect(this);
	rect.DeflateRect(lpRect);
	return rect;
}

inline TRect TRect::operator&(_In_ const RECT& rect2) const throw()
{
	TRect rect;
	::IntersectRect(&rect, this, &rect2);
	return rect;
}

inline TRect TRect::operator|(_In_ const RECT& rect2) const throw()
{
	TRect rect;
	::UnionRect(&rect, this, &rect2);
	return rect;
}

inline BOOL TRect::SubtractRect(
	_In_ LPCRECT lpRectSrc1,
	_In_ LPCRECT lpRectSrc2) throw()
{
	return ::SubtractRect(this, lpRectSrc1, lpRectSrc2);
}

inline void TRect::NormalizeRect() throw()
{
	int nTemp;
	if (left > right)
	{
		nTemp = left;
		left = right;
		right = nTemp;
	}
	if (top > bottom)
	{
		nTemp = top;
		top = bottom;
		bottom = nTemp;
	}
}

inline void TRect::InflateRect(_In_ LPCRECT lpRect) throw()
{
	left -= lpRect->left;
	top -= lpRect->top;
	right += lpRect->right;
	bottom += lpRect->bottom;
}

inline void TRect::InflateRect(
	_In_ int l,
	_In_ int t,
	_In_ int r,
	_In_ int b) throw()
{
	left -= l;
	top -= t;
	right += r;
	bottom += b;
}

inline void TRect::DeflateRect(_In_ LPCRECT lpRect) throw()
{
	left += lpRect->left;
	top += lpRect->top;
	right -= lpRect->right;
	bottom -= lpRect->bottom;
}

inline void TRect::DeflateRect(
	_In_ int l,
	_In_ int t,
	_In_ int r,
	_In_ int b) throw()
{
	left += l;
	top += t;
	right -= r;
	bottom -= b;
}

inline TRect TRect::MulDiv(
	_In_ int nMultiplier,
	_In_ int nDivisor) const throw()
{
	return TRect(
		::MulDiv(left, nMultiplier, nDivisor),
		::MulDiv(top, nMultiplier, nDivisor),
		::MulDiv(right, nMultiplier, nDivisor),
		::MulDiv(bottom, nMultiplier, nDivisor));
}
