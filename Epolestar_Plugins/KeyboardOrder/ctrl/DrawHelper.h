#pragma once

namespace DrawHelper
{
	//类似于MFC中的FillSolidRect
	void FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF clr);
	//类似于MFC中的FillSolidRect
	void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);
	//类似于MFC中的Draw3dRect
	void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
	//类似于MFC中的Draw3dRect
	void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
	//画边框
	void DrawFrame(HWND hWnd, bool bHover, bool bFocus, HBRUSH &HotOrderBrush1, HBRUSH &HotOrderBrush2, HBRUSH &ClientBrush, HBRUSH &OrderBrush);
	//内存DC
	class CMemDC
	{
	public:
		CMemDC(HWND hWnd);
		~CMemDC();
		CMemDC* operator->() { return this; }
		operator CMemDC*() { return this; }
		HDC &GetHdc()	{ return m_MemDC; }
	protected:
		PAINTSTRUCT m_paintStruct;
	private:
		HWND m_hWnd;
		HBITMAP m_bitmap;     
		HBITMAP m_hOldBitmap;  
		HDC m_hDC;        
		HDC m_MemDC;	
		RECT m_rect;    
	};
}
