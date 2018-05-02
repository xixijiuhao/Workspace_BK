
#include "BaseInclude.h"

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
			m_bTracking =TrackMouseEvent(&tme)==TRUE;
		}
		return true;//break;
	case WM_MOUSELEAVE:
		m_bTracking = false;
		m_bHover = false;
		m_bDown = false;
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
		DrawItemFrame(m_bHover, m_bFocus);
		return true;//
	case WM_MOUSEHOVER:
		m_bHover = true;
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
		DrawItemFrame(m_bHover, m_bFocus);
		return true;//break;
	case WM_SETFOCUS:
		m_bFocus = true;
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
		DrawItemFrame(m_bHover, m_bFocus);
		return true;//break;
	case WM_KILLFOCUS:
		m_bFocus = false;
		m_bDown = false;
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
		DrawItemFrame(m_bHover, m_bFocus);
		return true;//break;
	case WM_LBUTTONDOWN:
		m_point.x = LOWORD(l);
		m_point.y = HIWORD(l);
		m_bDown = true;
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		return true;
	case WM_LBUTTONUP:
		m_point.x = LOWORD(l);
		m_point.y = HIWORD(l);
		m_bDown = false;
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		return true;
	case WM_ERASEBKGND:
		return true;
	case WM_CAPTURECHANGED:
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		return 0;
	case WM_TIMER:
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		return true;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}

void CxSpin::Create(HWND hParent, UINT id, UINT style)
{
	HWND hSpin = CreateWindowEx(0, UPDOWN_CLASS, TEXT(""), WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | WS_CLIPSIBLINGS | style,
		0, 0, 0, 0, hParent, (HMENU)(id), GetModuleHandle(NULL), NULL);
	Init(hSpin);
	SendMessage(m_hWnd, UDM_SETBASE, 10, 0);
}

void CxSpin::SetBuddy(HWND hedit)
{
	SendMessage(m_hWnd, UDM_SETBUDDY, (WPARAM)hedit, 0);
}
void CxSpin::SetRange(int lower, int upper)
{
	SendMessage(m_hWnd, UDM_SETRANGE32, lower, upper);
}