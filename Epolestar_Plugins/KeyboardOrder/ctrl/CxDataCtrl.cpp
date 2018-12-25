#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;

CxDataCtrl::CxDataCtrl(COFInputDlg&ref) :m_ref(ref)
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	m_ClientBrush = ::CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	m_hOrderBrush = ::CreateSolidBrush(/*GetSysColor(COLOR_WINDOWFRAME)*/g_ColorRefData.GetColorRGB150());
	m_HotOrderBrush1 = ::CreateSolidBrush(RGB(96, 200, 253));
	m_HotOrderBrush2 = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
}
CxDataCtrl::~CxDataCtrl()
{
	if (m_ClientBrush)
		DeleteObject(m_ClientBrush);
	if (m_hOrderBrush)
		DeleteObject(m_hOrderBrush);
	if (m_HotOrderBrush1)
		DeleteObject(m_HotOrderBrush1);
	if (m_HotOrderBrush2)
		DeleteObject(m_HotOrderBrush2);
}
BOOL CxDataCtrl::Init(HWND hWnd)
{
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | BS_OWNERDRAW);
	return CxSubClassWnd::Init(hWnd);
}
void CxDataCtrl::DrawItemFrame(bool bHover, bool bFocus, bool bArrow)
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
		rectArrow.left = rectArrow.right - nWidth - 1;
		if (IsWindowEnabled(m_hWnd))
			::SetBkColor(hDC, g_ColorRefData.GetColorWhite());
		else
			::SetBkColor(hDC, g_ColorRefData.GetColorBackground());

		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);

		SelectObject(hDC, hGrayPen);

		int pos = rectArrow.top + (rectArrow.bottom - rectArrow.top) / 2;
		POINT ptLeft;
		ptLeft.x = rectArrow.right - 5 - 8;
		ptLeft.y = pos - 2;
		POINT ptCenter;
		ptCenter.x = rectArrow.right - 5 - 4;
		ptCenter.y = pos + 2;
		POINT ptRight;
		ptRight.x = rectArrow.right - 5;
		ptRight.y = pos - 2;
		HPEN pen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
		HPEN OldPen = (HPEN)SelectObject(hDC, pen);
		MoveToEx(hDC, ptLeft.x, ptLeft.y, NULL);
		LineTo(hDC, ptCenter.x, ptCenter.y);
		LineTo(hDC, ptRight.x + 1, ptRight.y - 1);
		SelectObject(hDC, OldPen);

		DeleteObject(pen);
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
LRESULT CALLBACK CxDataCtrl::WndProc(UINT message, WPARAM w, LPARAM l)
{
	if (WM_NCPAINT == message || WM_PAINT == message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus,true);
		return 1;
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
		DrawItemFrame(m_bHover, m_bFocus, true);
	}
	else if (WM_MOUSEHOVER == message)
	{
		m_bHover = TRUE;
		DrawItemFrame(m_bHover, m_bFocus, true);
	}
	else if (WM_SETFOCUS == message)
	{
		m_bFocus = TRUE;
		DrawItemFrame(m_bHover, m_bFocus, true);
	}
	else if (WM_KILLFOCUS == message)
	{
		m_bFocus = FALSE;
		DrawItemFrame(m_bHover, m_bFocus, true);
	}
	else if (WM_LBUTTONUP == message || WM_LBUTTONDOWN== message)
	{
		DrawItemFrame(m_bHover, m_bFocus, true);
	}
	else if (WM_DRAWITEM == message)
	{
		DRAWITEMSTRUCT *pDraw = (DRAWITEMSTRUCT*)(l);
		int a = 0;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}

