#include "CxCheck.h"
#include "..\common\UIDesign.h"
//色彩
extern G_COLOREF g_ColorRefData;

CxCheck::CxCheck()
{
	m_bTracking = m_bHover = m_bFocus = m_bInRect= false;
	m_bChecked = false;
}
CxCheck::~CxCheck()
{
	
}
void  CxCheck::Init(HWND hwnd, COLORREF color)
{
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | BS_OWNERDRAW);
	m_colorBK = color;
	CxSubClassWnd::Init(hwnd);
	//改变窗口大小
	HDC hDC = GetDC(hwnd);
	wchar_t strText[80] = {0};
	CxSubClassWnd::CxGetWindowText(strText,80);
	HFONT hfont = (HFONT)SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	SelectObject(hDC, hfont);
	SIZE sz;
	::GetTextExtentPoint32(hDC, strText, (int)wcslen(strText), &sz);
	DeleteDC(hDC);
	RECT rectCheck;
	GetWindowRect(m_hWnd, &rectCheck);
	rectCheck.right = rectCheck.left + sz.cx + 20 + 1;
	SetWindowPos(m_hWnd, NULL, 0, 0, rectCheck.right - rectCheck.left, rectCheck.bottom - rectCheck.top, SWP_NOMOVE);
}
LRESULT CALLBACK CxCheck::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCPAINT:
	case WM_PAINT:
		//CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam);
		OnPaint();
		return 0;
	case WM_MOUSEMOVE:
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 10;
			m_bTracking = _TrackMouseEvent(&tme) == TRUE;
		}
		break;
	case WM_MOUSELEAVE:
		m_bTracking = false;
		m_bHover = false;
		m_bInRect = false;
		InvalidateRect(GetHwnd(), 0, TRUE); 
		//m_bInRect = false;
		break;
	case WM_MOUSEHOVER:
		m_bHover = true;
		InvalidateRect(GetHwnd(), 0, TRUE);
		break;
	case WM_SETFOCUS:
		m_bFocus = true;
		InvalidateRect(GetHwnd(), 0, TRUE);
		break;
	case WM_KILLFOCUS:
		m_bFocus = false;
		InvalidateRect(GetHwnd(), 0, TRUE);
		break;
	case WM_ERASEBKGND:
		return true;
	case WM_ENABLE:
		InvalidateRect(GetHwnd(), 0, TRUE);
		break;
	case WM_LBUTTONDOWN:
		m_bInRect = true;
		break;
	case WM_LBUTTONDBLCLK:
		m_bInRect = false;
		{
			long x = LOWORD(lParam);
			long y = HIWORD(lParam);
			RECT rect;
			GetClientRect(m_hWnd, &rect);
			if (x >= rect.left&&x <= rect.right&&y >= rect.top&&y <= rect.bottom)
				SetCheck(GetCheck() == BST_CHECKED ? false : true);
		}
		break;
	case WM_LBUTTONUP:		
		if (m_bInRect)
		{
			long x = LOWORD(lParam);
			long y = HIWORD(lParam);
			RECT rect;
			GetClientRect(m_hWnd, &rect);
			if (x >= rect.left&&x <= rect.right&&y >= rect.top&&y <= rect.bottom)
				SetCheck(GetCheck() == BST_CHECKED ? false : true);
		}
		break;
	}
	
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程

}
void CxCheck::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdctmp = BeginPaint(GetHwnd(), &ps);

	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc = CreateCompatibleDC(hdctmp);
	HBITMAP bitmap = CreateCompatibleBitmap(hdctmp, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc, bitmap);
	SetBkMode(memdc, TRANSPARENT);
	HBRUSH hBkBrush = CreateSolidBrush(m_colorBK/*g_ColorRefData.GetColorBackground()*/);
	FillRect(memdc, &rect, hBkBrush);

	RECT rcsel;
	rcsel.left = 0; 
	rcsel.top = (rect.bottom - rect.top - m_selchecksize) / 2;
	rcsel.right = rcsel.left + m_selchecksize; 
	rcsel.bottom = rcsel.top + m_selchecksize;
	HPEN pen = CreatePen(PS_USERSTYLE, 1, g_ColorRefData.GetColorRGB150());
	SelectObject(memdc, pen);
	Rectangle(memdc, rcsel.left, rcsel.top, rcsel.right, rcsel.bottom);

	if (!IsWindowEnabled(m_hWnd))
	{
		HBRUSH hDisBrush = CreateSolidBrush(g_ColorRefData.GetColorRGB200());
		InflateRect(&rcsel, -1, -1);
		FillRect(memdc, &rcsel, hDisBrush);
		DeleteObject(hDisBrush);
	}
	//绘制checkbox
	else if (m_bChecked)
	{
		HPEN hpen = CreatePen(PS_SOLID, 2, g_ColorRefData.GetColorRGB150());
		SelectObject(memdc, hpen);
		InflateRect(&rcsel, -2, -2);

		POINT pp[3];
		pp[0].x = rcsel.left + 1;
		pp[0].y = rcsel.top + (rcsel.bottom - rcsel.top) / 2;
		pp[1].x = pp[0].x + 2;
		pp[1].y = rcsel.bottom - 2;
		pp[2].x = rcsel.right - 1;
		pp[2].y = rcsel.top + 2;

		MoveToEx(memdc, pp[0].x, pp[0].y, NULL);
		LineTo(memdc, pp[1].x, pp[1].y);
		MoveToEx(memdc, pp[1].x, pp[1].y, NULL);
		LineTo(memdc, pp[2].x, pp[2].y);

		InflateRect(&rcsel, 2, 2);
		DeleteObject(hpen);
	}

	wchar_t strText[80] = { 0 };
	CxSubClassWnd::CxGetWindowText(strText,80);
	HFONT hfont = (HFONT)SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	SelectObject(memdc, hfont);
	
	SetTextColor(memdc, m_color);
	rcsel.left = rcsel.right + 4; rcsel.right = rect.right;
	rcsel.top = rect.top;
	rcsel.bottom = rect.bottom;
	DrawText(memdc, strText, wcslen(strText),&rcsel,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

	BitBlt(hdctmp, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);

	DeleteObject(hBkBrush);
	DeleteObject(pen);
	DeleteObject(bitmap);
	DeleteDC(memdc);
	EndPaint(GetHwnd(), &ps);
}

void CxCheck::SetCheck(int check) 
{ 
	m_bChecked = (check == BST_CHECKED) ? true : false; 
	InvalidateRect(GetHwnd(), NULL, TRUE); 
	int id = GetWindowLong(m_hWnd, GWL_ID);
	HWND h = ::GetParent(m_hWnd);
	SendMessage(h, WM_COMMAND, MAKEWPARAM(id, BN_CLICKED), 0);
}