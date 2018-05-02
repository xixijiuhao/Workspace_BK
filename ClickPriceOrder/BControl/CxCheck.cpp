#include "stdafx.h"
#include "CxCheck.h"
#include "UIDesign.h"
extern G_UIFONT g_FontData;
//ɫ��
extern G_COLOREF g_ColorRefData;

CxCheck::CxCheck()
{
	m_bTracking = m_bHover = m_bFocus = m_bInRect= false;
	m_bChecked = false;
}
CxCheck::~CxCheck()
{
	
}
void CxCheck::Create(HWND hParent, UINT id, UINT ExStyle)
{
	m_hWnd = CreateWindowEx(0, WC_BUTTON, L"", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | WS_TABSTOP,
		0, 0, 0,0, hParent, HMENU(100), GetModuleHandle(NULL), 0);
	if (m_hWnd)
		Init(m_hWnd);
}
void CxCheck::SetPosition(int x, int y, int cx, int cy)
{
	SetWindowPos(m_hWnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_hWnd, NULL, true);
}
void  CxCheck::Init(HWND hwnd)
{
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | BS_OWNERDRAW);
	CxSubClassWnd::Init(hwnd);
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
			tme.dwHoverTime = 50;
			m_bTracking = TrackMouseEvent(&tme) == TRUE;
		}
		break;
	case WM_MOUSELEAVE:
		m_bTracking = false;
		m_bHover = false;
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
	}
	
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //Ĭ�ϻص�����

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
	FillRect(memdc, &rect, g_ColorRefData.g_brush_white);

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
	//����checkbox
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

	wstring strText;
	CxSubClassWnd::CxGetWindowText(strText);
	//HFONT hfont = (HFONT)SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	//hfont = (HFONT)GetStockObject(ANSI_FIXED_FONT);\

	SelectObject(memdc, m_hfont);
	
	SetTextColor(memdc, g_ColorRefData.GetColorBlack());
	rcsel.left = rcsel.right + 4; rcsel.right = rect.right;
	rcsel.top = rect.top;
	rcsel.bottom = rect.bottom;
	DrawText(memdc, strText.c_str(), strText.length(),&rcsel,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

	BitBlt(hdctmp, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);

	DeleteObject(pen);
	DeleteObject(bitmap);
	DeleteDC(memdc);
	EndPaint(GetHwnd(), &ps);
}