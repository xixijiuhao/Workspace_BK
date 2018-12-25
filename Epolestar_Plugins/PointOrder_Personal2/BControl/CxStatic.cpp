#include "stdafx.h"
#include "CxStatic.h"
#include "UIDesign.h"



extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CxStatic::CxStatic()
{
	m_bHover = false;
	m_bTracking = false;
	m_nState = 0;
}
CxStatic::~CxStatic()
{
	
}
void CxStatic::Init(HWND hwnd)
{
	SetWindowLong(GetHwnd(), GWL_STYLE, GetWindowLong(GetHwnd(),GWL_STYLE) | SS_OWNERDRAW);
	CxSubClassWnd::Init(hwnd);
}
LRESULT CALLBACK CxStatic::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			OnPaint();
			return 1;
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
			m_bHover = FALSE;
			InvalidateRect(m_hWnd, NULL, TRUE);
			break;
		case WM_MOUSEHOVER:
			m_bHover = TRUE;
			InvalidateRect(m_hWnd, NULL, TRUE);
			break;
		case WM_ERASEBKGND:
			return true;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CxStatic::UpdateText( wstring wstrText,int nState)
{
	m_strText = wstrText;
	m_nState = nState;
	if (::IsWindow(m_hWnd))
		InvalidateRect(GetHwnd(),NULL,TRUE);
}
void CxStatic::Create(HWND hParent, UINT id, UINT ExStyle)
{
	HWND hwnd= CreateWindowEx(0, WC_STATIC, L"Static", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | WS_TABSTOP,
		0, 0, 0, 0, hParent, HMENU(id), GetModuleHandle(NULL), 0);
	if (hwnd)
		Init(hwnd);
}

void CxStatic::SetPosition(int x, int y, int cx, int cy)
{
	SetWindowPos(m_hWnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_hWnd, NULL, true);
}
void CxStatic::SetFont(HFONT hfont)
{
	SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hfont, TRUE);
}
void CxStatic::OnPaint()
{
	HBRUSH hNormalBkBrush = ::CreateSolidBrush(g_ColorRefData.GetColorWhite());
	
	PAINTSTRUCT paintstruct;
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	
	FillRect(memdc, &rect, hNormalBkBrush);

	SelectObject(memdc, g_FontData.GetFontWord22());
	SetBkMode(memdc,TRANSPARENT);

	if (m_nState == 1)
		SetTextColor(memdc, g_ColorRefData.GetColorBlue());
	else if (m_bHover)
		SetTextColor(memdc, g_ColorRefData.GetColorLightBlue());
	else
		SetTextColor(memdc, g_ColorRefData.GetColorBlack());

	DrawText(memdc, m_strText.c_str(), m_strText.length(), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	
	DeleteDC(memdc);
	DeleteObject(bitmap);
	DeleteObject(hNormalBkBrush);
	::EndPaint(GetHwnd(), &paintstruct);
}