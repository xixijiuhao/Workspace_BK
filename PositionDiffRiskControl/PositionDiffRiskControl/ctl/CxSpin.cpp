#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;


CxSpin::CxSpin()
{
	m_bTracking = m_bHover = m_bFocus = m_bDown = false;
}
CxSpin::~CxSpin()
{

}
BOOL CxSpin::Init(HWND hWnd)
{
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE)|BS_OWNERDRAW);
	return CxSubClassWnd::Init(hWnd);
}
void CxSpin::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth,bool bNormal)//宽度奇数
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
void CxSpin::DrawItemFrame(bool bHover, bool bFocus, bool bDown)
{
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

		RECT rectDraw(rectArrow);
		if (m_point.y < nvline)
		{
			rectDraw.bottom = nvline;
			::FillRect(hDC, &rectDraw, g_ColorRefData.GetHbrBackground());
		}	
		else if (m_point.y == nvline)
		{}
		else
		{
			rectDraw.top = nvline+1;
			::FillRect(hDC, &rectDraw, g_ColorRefData.GetHbrBackground());
		}

		DrawTriangle(hDC, rectArrow.left + (rectArrow.right - rectArrow.left) / 2 - 2, rectArrow.top + rectArrow.bottom / 4 + nw / 6, 5, false);

		DrawTriangle(hDC, rectArrow.left + (rectArrow.right - rectArrow.left) / 2 - 2, rectArrow.top + rectArrow.bottom * 3 / 4 - nw / 6, 5);
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

		DrawTriangle(hDC, rectArrow.left + (rectArrow.right - rectArrow.left) / 2 - 2, rectArrow.top + rectArrow.bottom / 4 + nw / 6, 5, false);

		DrawTriangle(hDC, rectArrow.left + (rectArrow.right - rectArrow.left) / 2 - 2, rectArrow.top + rectArrow.bottom * 3 / 4 - nw / 6, 5);
	}
	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrLightBlue());
	else
		::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrRGB150());
	
	::ReleaseDC(m_hWnd, hDC);

	DeleteObject(hGrayPen);
}
LRESULT CALLBACK CxSpin::WndProc(UINT message, WPARAM w, LPARAM l)
{
	switch (message)
	{
	case WM_NCPAINT:
	case WM_PAINT:
		{
			CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			return 0;
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
		InvalidateRect(m_hWnd, NULL, TRUE);
		break;
	case WM_MOUSEHOVER:
		m_bHover = true;
		InvalidateRect(m_hWnd, NULL, TRUE);
		break;
	case WM_SETFOCUS:
		m_bFocus = true;
		InvalidateRect(m_hWnd, NULL, TRUE);
		break;
	case WM_KILLFOCUS:
		m_bFocus = false;
		m_bDown = false;
		InvalidateRect(m_hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONDOWN:
		m_point.x = LOWORD(l);
		m_point.y = HIWORD(l);
		m_bDown = true;
		InvalidateRect(m_hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONUP:
		m_point.x = LOWORD(l);
		m_point.y = HIWORD(l);
		m_bDown = false;
		InvalidateRect(m_hWnd, NULL, TRUE);
		break;
	case WM_ERASEBKGND:
		return true;
	case WM_CAPTURECHANGED:
		InvalidateRect(m_hWnd, NULL, TRUE);
		break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程

}
