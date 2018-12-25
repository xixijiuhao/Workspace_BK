#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;

CxKeyBoardSpin::CxKeyBoardSpin()
{
	m_bTracking = m_bHover = m_bFocus = m_bDown = false;
}
CxKeyBoardSpin::~CxKeyBoardSpin()
{

}
void CxKeyBoardSpin::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth,bool bNormal)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;
	if (bNormal)
	{
		int nMidx = nStartx + nWidth / 2;
		int nMidy = nStarty + nWidth / 2;
		MoveToEx(hdc, nStartx, nStarty, NULL);
		LineTo(hdc, nMidx, nMidy);

		int nEndx = nStartx + nWidth - 1;
		int nEndy = nStarty;
		LineTo(hdc, nEndx + 1, nEndy - 1);
	}
	else
	{
		int nMidx = nStartx + nWidth / 2;
		int nMidy = nStarty - nWidth / 2;
		MoveToEx(hdc, nStartx, nStarty, NULL);
		LineTo(hdc, nMidx, nMidy);

		int nEndx = nStartx + nWidth - 1;
		int nEndy = nStarty;
		LineTo(hdc, nEndx + 1, nEndy + 1);
	}
	
}
void CxKeyBoardSpin::DrawItemFrame(bool bHover, bool bFocus, bool bDown)
{
	HBRUSH hClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorRGB150());
	HBRUSH hNormalBkBrush = ::CreateSolidBrush(g_ColorRefData.GetColorWhite());
	HBRUSH hHotBKBrush = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
	HPEN   hGrayPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
	
	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;

	int nWidth = GetSystemMetrics(SM_CXHTHUMB);
	int nxBorder = GetSystemMetrics(SM_CXBORDER);
	int nyBorder = GetSystemMetrics(SM_CYBORDER);
	

	if (bDown)
	{	
		RECT rectArrow(rectClient);
		InflateRect(&rectArrow, -nxBorder, -nyBorder);
		int nw = rectArrow.right - rectArrow.left;
		int nvline = (rectArrow.bottom - rectArrow.top) / 2+1;

		::SetBkColor(hDC, g_ColorRefData.GetColorWhite());
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);

		SelectObject(hDC, hGrayPen);

		MoveToEx(hDC, rectArrow.left, nvline, NULL);
		LineTo(hDC, rectArrow.right, nvline);

		DrawTriangle(hDC, rectArrow.left + nw / 3, rectArrow.top + rectArrow.bottom / 4 + nw / 6, nw / 6 * 2 + 1, false);

		DrawTriangle(hDC, rectArrow.left + nw / 3, rectArrow.top + rectArrow.bottom * 3 / 4 - nw / 6, nw / 6 * 2 + 1);

		RECT rectDraw(rectArrow);
		if (m_point.y < nvline)
		{
			rectDraw.bottom = nvline;
			::FrameRect(hDC, &rectDraw, hClientBrush);
		}	
		else if (m_point.y == nvline)
		{
		}
		else
		{
			rectDraw.top = nvline+1;
			::FrameRect(hDC, &rectDraw, hClientBrush);
		}	
	}
	else
	{
		RECT rectArrow(rectClient);
		InflateRect(&rectArrow, -nxBorder, -nyBorder);
		int nw = rectArrow.right - rectArrow.left;
		int nvline = (rectArrow.bottom - rectArrow.top) / 2+1;

		if (!IsWindowEnabled(m_hWnd))
			::SetBkColor(hDC, g_ColorRefData.GetColorBackground());
		else
			::SetBkColor(hDC, g_ColorRefData.GetColorWhite());
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);

		SelectObject(hDC, hGrayPen);

		MoveToEx(hDC, rectArrow.left, nvline, NULL);
		LineTo(hDC, rectArrow.right, nvline);

		DrawTriangle(hDC, rectArrow.left + nw / 3, rectArrow.top + rectArrow.bottom / 4 + nw / 6, nw / 6 * 2 + 1, false);

		DrawTriangle(hDC, rectArrow.left + nw / 3, rectArrow.top + rectArrow.bottom * 3 / 4 - nw / 6, nw / 6 * 2 + 1);
	}
	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, hHotBKBrush);
	else
		::FrameRect(hDC, &rectClient, hClientBrush);
	
	::ReleaseDC(m_hWnd, hDC);

	DeleteObject(hClientBrush);
	DeleteObject(hNormalBkBrush);
	DeleteObject(hHotBKBrush);
	DeleteObject(hGrayPen);
}
LRESULT CALLBACK CxKeyBoardSpin::WndProc(UINT message, WPARAM w, LPARAM l)
{
	switch (message)
	{
	case WM_NCPAINT:
	case WM_PAINT:
		{
			POINT pointB;
			pointB.x = LOWORD(l);
			pointB.y = HIWORD(l);
			CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			return 1;
		}
	case WM_MOUSEMOVE:
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 50;
			m_bTracking =_TrackMouseEvent(&tme)==TRUE;
		}
		break;
	case WM_MOUSELEAVE:
		m_bTracking = false;
		m_bHover = false;
		m_bDown = false;
		DrawItemFrame(m_bHover, m_bFocus);
		break;
	case WM_MOUSEHOVER:
		m_bHover = true;
		DrawItemFrame(m_bHover, m_bFocus);
		break;
	case WM_SETFOCUS:
		m_bFocus = true;
		DrawItemFrame(m_bHover, m_bFocus);
		break;
	case WM_KILLFOCUS:
		m_bFocus = false;
		m_bDown = false;
		DrawItemFrame(m_bHover, m_bFocus);
		break;
	case WM_LBUTTONDOWN:
		m_point.x = LOWORD(l);
		m_point.y = HIWORD(l);
		m_bDown = true;
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		break;
	case WM_LBUTTONUP:
		m_point.x = LOWORD(l);
		m_point.y = HIWORD(l);
		m_bDown = false;
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		break;
	case WM_ERASEBKGND:
		return true;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}
