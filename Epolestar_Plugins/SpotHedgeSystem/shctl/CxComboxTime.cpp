#include "PreInclude.h"

//色彩
extern G_COLOREF g_ColorRefData;


CxComboxTime::CxComboxTime()
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	m_hEditBrushNormal = ::CreateSolidBrush(g_ColorRefData.GetColorWhite());
	m_hEditBrushInvalid = ::CreateSolidBrush(g_ColorRefData.GetColorBackground());
}
CxComboxTime::~CxComboxTime()
{
	DeleteObject(m_hEditBrushNormal);
	DeleteObject(m_hEditBrushInvalid);
}
BOOL CxComboxTime::Init(HWND hWnd)
{
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | BS_OWNERDRAW);
	return CxSubClassWnd::Init(hWnd);
}
void CxComboxTime::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;
	
	int nMidx = nStartx + nWidth / 2;
	int nMidy = nStarty + nWidth / 2;
	MoveToEx(hdc, nStartx, nStarty, NULL);
	LineTo(hdc, nMidx, nMidy);

	int nEndx = nStartx + nWidth - 1;
	int nEndy = nStarty;
	LineTo(hdc, nEndx + 1, nEndy - 1);
}
void CxComboxTime::DrawItemFrame(bool bHover, bool bFocus,bool bArrow)
{
	HBRUSH hClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorBtnFace());
	HBRUSH hNormalBKBrush = ::CreateSolidBrush(g_ColorRefData.GetColorRGB150());
	HBRUSH hHotOrderBrush2 = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
	HPEN   hGrayPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());

	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;
	if (bArrow)
	{
		int nWidth = GetSystemMetrics(SM_CXHTHUMB);
		int nxBorder = GetSystemMetrics(SM_CXBORDER);
		int nyBorder = GetSystemMetrics(SM_CYBORDER);
		RECT rectArrow(rectClient);
		InflateRect(&rectArrow, -nxBorder, -nyBorder);
		rectArrow.left = rectArrow.right - nWidth-1;
		
		if (IsWindowEnabled(m_hWnd))
		{
			::SetBkColor(hDC, g_ColorRefData.GetColorWhite());
			::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);
		}			
		else
		{
			//禁用状态TEXT重绘
			HFONT hfont = (HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0);
			HFONT hOldFont = (HFONT)SelectObject(hDC, hfont);
			SetBkMode(hDC, TRANSPARENT);
			wchar_t wText[21] = { 0 };
			GetWindowText(m_hWnd, wText, 21);
			::SetBkColor(hDC, g_ColorRefData.GetColorBackground());
			::SetTextColor(hDC, g_ColorRefData.GetColorRGB150());
			RECT rectText(rectClient);
			InflateRect(&rectText, -nxBorder * 3, -nyBorder * 3);
			rectText.right = rectArrow.left;
			::ExtTextOut(hDC, 3, 3, ETO_OPAQUE, &rectText, wText, wcslen(wText), NULL);
			::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);
			SelectObject(hDC, hOldFont);
		}
			
		SelectObject(hDC, hGrayPen);

		MoveToEx(hDC, rectArrow.left, rectArrow.top, NULL);
		LineTo(hDC, rectArrow.left, rectArrow.bottom);

		DrawTriangle(hDC, rectArrow.left + nWidth / 3, rectArrow.bottom/2 - nWidth / 6+1 , nWidth / 6 * 2 + 3);
	}
	if (bHover || bFocus)
	{
		::FrameRect(hDC, &rectClient, hClientBrush);
		InflateRect(&rectClient, -1, -1);
		::FrameRect(hDC, &rectClient, hHotOrderBrush2);
	}
	else
	{
		::FrameRect(hDC, &rectClient, hClientBrush);
		InflateRect(&rectClient, -1, -1);
		::FrameRect(hDC, &rectClient, hNormalBKBrush);
	}
	::ReleaseDC(m_hWnd, hDC);

	DeleteObject(hClientBrush);
	DeleteObject(hNormalBKBrush);
	DeleteObject(hHotOrderBrush2);
	DeleteObject(hGrayPen);
}
LRESULT CALLBACK CxComboxTime::WndProc(UINT message, WPARAM w, LPARAM l)
{
	if (WM_NCPAINT == message||WM_PAINT==message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus,true);
		return 0;
	}
	else if (WM_MOUSEMOVE == message)
	{
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 50;
			m_bTracking = _TrackMouseEvent(&tme) == TRUE;
		}
	}
	else if (WM_MOUSELEAVE == message)
	{
		m_bTracking = FALSE;
		m_bHover = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_MOUSEHOVER == message)
	{
		m_bHover = TRUE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_SETFOCUS == message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
		m_bFocus = TRUE;
		DrawItemFrame(m_bHover, m_bFocus);
		return true;
	}
	else if (WM_KILLFOCUS == message)
	{
		m_bFocus = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_LBUTTONDOWN == message)
	{
		POINT pointB;
		pointB.x = GET_X_LPARAM(l);
		pointB.y = GET_Y_LPARAM(l);
		m_bFocus = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);
		RECT rect, rectClient;
		::GetWindowRect(m_hWnd, &rect);
		rectClient.left = rect.right - rect.left-22;
		rectClient.top = 0;
		rectClient.right = rect.right - rect.left;
		rectClient.bottom = rect.bottom - rect.top;
		if (PtInRect(&rectClient, pointB))
		{
			SendMessage(m_hWnd, WM_SYSKEYDOWN, VK_DOWN, 0);
			return true;
		}
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}
