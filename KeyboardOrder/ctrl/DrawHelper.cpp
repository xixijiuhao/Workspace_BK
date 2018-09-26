#include "..\PreInclude.h"

namespace DrawHelper
{
	//类似于MFC中的FillSolidRect
	void FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF clr)
	{
		::SetBkColor(hdc, clr);
		::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	}
	//类似于MFC中的FillSolidRect
	void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr)
	{
		::SetBkColor(hdc, clr);
		RECT rect;
		rect.left = x;
		rect.top = y;
		rect.right = x + cx;
		rect.bottom = y + cy;
		::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	}
	//类似于MFC中的Draw3dRect
	void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
	{
		Draw3dRect(hdc,lpRect->left, lpRect->top, lpRect->right - lpRect->left,
			lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
	}
	//类似于MFC中的Draw3dRect
	void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
	{
		FillSolidRect(hdc,x, y, cx - 1, 1, clrTopLeft);
		FillSolidRect(hdc,x, y, 1, cy - 1, clrTopLeft);
		FillSolidRect(hdc,x + cx, y, -1, cy, clrBottomRight);
		FillSolidRect(hdc,x, y + cy, cx, -1, clrBottomRight);
	}
	void DrawFrame(HWND hWnd, bool bHover, bool bFocus, HBRUSH &HotOrderBrush1, HBRUSH &HotOrderBrush2, HBRUSH &ClientBrush, HBRUSH &OrderBrush)
	{
		HDC hDC = ::GetWindowDC(hWnd);
		RECT rect, rectClient;
		::GetWindowRect(hWnd, &rect);
		rectClient.left = 0;
		rectClient.top = 0;
		rectClient.right = rect.right - rect.left;
		rectClient.bottom = rect.bottom - rect.top;
		/*if (bHover)
		{
			::FrameRect(hDC, &rectClient, HotOrderBrush1);
			InflateRect(&rectClient, -1, -1);
			::FrameRect(hDC, &rectClient, HotOrderBrush2);
		}
		else */if (bHover||bFocus)
		{
			::FrameRect(hDC, &rectClient, HotOrderBrush2);
			InflateRect(&rectClient, -1, -1);
			::FrameRect(hDC, &rectClient, ClientBrush);
		}
		else
		{
			::FrameRect(hDC, &rectClient, OrderBrush);
			InflateRect(&rectClient, -1, -1);
			::FrameRect(hDC, &rectClient, ClientBrush);	
		}
		::ReleaseDC(hWnd, hDC);
	}
}
DrawHelper::CMemDC::CMemDC(HWND hWnd)
{
	memset(&m_paintStruct, 0, sizeof(PAINTSTRUCT));
	m_hWnd = hWnd;
	RECT rect;
	GetWindowRect(hWnd, &rect);
	HDC hdc = ::BeginPaint(hWnd, &m_paintStruct);

	HDC memdc = CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	m_hOldBitmap = (HBITMAP)SelectObject(memdc, bitmap);
	m_hDC = hdc;
	m_MemDC = memdc;
	::ReleaseDC(hWnd, hdc);
}

DrawHelper::CMemDC::~CMemDC()
{
	if (m_hDC)
	{
		BitBlt(m_hDC, m_rect.left, m_rect.top, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top,
			m_MemDC, m_rect.left, m_rect.top, SRCCOPY);
		HBITMAP bitmap = (HBITMAP)SelectObject(m_MemDC, m_hOldBitmap);
		DeleteObject(bitmap);
		DeleteDC(m_MemDC);
		::ReleaseDC(m_hWnd, m_hDC);
	}
	::EndPaint(m_hWnd, &m_paintStruct);
}
