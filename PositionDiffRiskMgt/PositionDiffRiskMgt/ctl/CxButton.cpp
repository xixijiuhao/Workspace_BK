#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CxButton::CxButton()
{
	m_bTracking = m_bHover = m_bFocus = m_bDown = FALSE;
}
CxButton::~CxButton()
{

}
BOOL CxButton::Init(HWND hWnd, bool bArrow, bool bBorder)
{
	m_bArrow = bArrow;
	m_bBorder = bBorder;
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | BS_OWNERDRAW);
	return CxSubClassWnd::Init(hWnd);
}
void CxButton::SetButtonText(const wchar_t* wText)
{
	SetWindowText(m_hWnd, wText);
	InvalidateRect(m_hWnd, NULL, FALSE); 
}
int CxButton::GetButtonText(wchar_t * text, unsigned int nLen)
{ 
	return CxSubClassWnd::CxGetWindowText(text, nLen);
	
}
void CxButton::DrawTriangle(HDC hdc, int nStartx, int nStarty, int nWidth)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;
	MoveToEx(hdc, nStartx, nStarty, NULL);
	LineTo(hdc, nStartx + nWidth / 2, nStarty + nWidth / 2);
	LineTo(hdc, nStartx + nWidth, nStarty - 1);
}
void CxButton::DrawItemFrame(bool bHover, bool bFocus, bool bDown)
{
	HBRUSH hClientBrush = g_ColorRefData.GetHbrRGB150();
	HBRUSH hNormalBkBrush = g_ColorRefData.GetHbrWhite();
	HBRUSH hDisableBkBrush = g_ColorRefData.GetHbrBackground();
	HBRUSH hHotBKBrush = g_ColorRefData.GetHbrLightBlue();
	HPEN   hGrayPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
	
	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;
	
	HDC		m_HdcMemNormal;
	HBITMAP  m_hbitmapSwitchF;
	HFONT   hFont = CreateFont((rect.top - rect.bottom) * 1/2-1, 0, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, g_FontData.GetFontNumName().c_str());//
	m_HdcMemNormal = CreateCompatibleDC(hDC);
	m_hbitmapSwitchF = CreateCompatibleBitmap(hDC, rectClient.right, rectClient.bottom);
	SelectObject(m_HdcMemNormal, m_hbitmapSwitchF);
	SetBkMode(m_HdcMemNormal, TRANSPARENT);
		
	SelectObject(m_HdcMemNormal, hFont);
	if (bDown)
		FillRect(m_HdcMemNormal, &rectClient, hHotBKBrush);
	else
	{
		if (!IsWindowEnabled(m_hWnd))
			FillRect(m_HdcMemNormal, &rectClient, hDisableBkBrush);
		else
			FillRect(m_HdcMemNormal, &rectClient, hNormalBkBrush);	
	}

	if (m_bArrow)
	{
		SelectObject(m_HdcMemNormal, hGrayPen);
		DrawTriangle(m_HdcMemNormal, rectClient.right * 4 / 5, (rectClient.bottom / 2)-3, 11);
	}	

	wchar_t sText[20] = {0};
	GetButtonText(sText,20);
	SetTextColor(m_HdcMemNormal, g_ColorRefData.GetColorBlack());
	DrawText(m_HdcMemNormal, sText, wcslen(sText), &rectClient, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	BitBlt(hDC, 0, 0, rectClient.right, rectClient.bottom, m_HdcMemNormal, 0, 0, SRCCOPY);

	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, hHotBKBrush);
	else
	{
		if (m_bBorder)
		{
			::FrameRect(hDC, &rectClient, hClientBrush);
			if (IsWindowEnabled(m_hWnd))
			{
				InflateRect(&rectClient, -1, -1);
				::FrameRect(hDC, &rectClient, hNormalBkBrush);
			}
		}		
	}
	::ReleaseDC(m_hWnd, hDC);
	DeleteObject(m_HdcMemNormal);
	DeleteObject(m_hbitmapSwitchF);
	DeleteObject(hFont);
	DeleteObject(hGrayPen);
}
LRESULT CALLBACK CxButton::WndProc(UINT message, WPARAM w, LPARAM l)
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
			m_bTracking = _TrackMouseEvent(&tme) == TRUE;
		}
		break;
	case WM_MOUSELEAVE:
		m_bTracking = FALSE;
		m_bHover = FALSE;
		m_bDown = FALSE;
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		break;
	case WM_MOUSEHOVER:
		m_bHover = TRUE;
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		break;
	case WM_SETFOCUS:
		m_bFocus = TRUE;
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		break;
	case WM_KILLFOCUS:
		m_bFocus = FALSE;
		m_bDown = FALSE;
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		break;
	case WM_LBUTTONDOWN:
		m_bDown = TRUE;
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		break;
	case WM_LBUTTONUP:
		m_bDown = FALSE;
		DrawItemFrame(m_bHover, m_bFocus, m_bDown);
		break;
	case WM_ERASEBKGND:
		return true;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}
