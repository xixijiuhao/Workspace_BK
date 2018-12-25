#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;

CxKeyBoardCheck::CxKeyBoardCheck()
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	
	m_bChecked = false;

	m_color = g_ColorRefData.GetColorBlack();
}
CxKeyBoardCheck::~CxKeyBoardCheck()
{
	
}
void  CxKeyBoardCheck::InitBtn(HWND hwnd)
{
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | BS_OWNERDRAW);
	Init(hwnd);
}
LRESULT CALLBACK CxKeyBoardCheck::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCPAINT:
	case WM_PAINT:
		OnPaint();
		return 0;
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
		InvalidateRect(GetHwnd(), 0, TRUE); 
		break;
	case WM_MOUSEHOVER:
		m_bHover = TRUE;
		InvalidateRect(GetHwnd(), 0, TRUE);
		break;
	case WM_SETFOCUS:
		m_bFocus = TRUE;
		InvalidateRect(GetHwnd(), 0, TRUE);
		break;
	case WM_KILLFOCUS:
		m_bFocus = FALSE;
		InvalidateRect(GetHwnd(), 0, TRUE);
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_ENABLE:
		InvalidateRect(GetHwnd(), 0, TRUE);
		return TRUE;
	}
	
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程

}
void CxKeyBoardCheck::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdctmp = BeginPaint(GetHwnd(), &ps);

	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc = CreateCompatibleDC(hdctmp);
	HBITMAP bitmap = CreateCompatibleBitmap(hdctmp, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc, bitmap);
	SetBkMode(memdc, TRANSPARENT);
	HBRUSH hBkBrush = CreateSolidBrush(g_ColorRefData.GetColorBackground());
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
	GetWindowTextW(GetHwnd(), strText,80);
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