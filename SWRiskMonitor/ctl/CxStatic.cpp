#include "CxStatic.h"
#include "UIDesign.h"

#include <CommCtrl.h>

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CxStatic::CxStatic()
{
	m_bHover = false;
	m_bTracking = false;
	memset(m_strText, 0, sizeof(m_strText));
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
				m_bTracking = _TrackMouseEvent(&tme) == TRUE;
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
void CxStatic::UpdateText(const wchar_t* strText)
{
	wcsncpy_s(m_strText, strText, sizeof(m_strText) / sizeof(m_strText[0]));
	if (::IsWindow(m_hWnd))
		InvalidateRect(GetHwnd(),NULL,TRUE);
}

void CxStatic::OnPaint()
{
	PAINTSTRUCT paintstruct;
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	
	FillRect(memdc, &rect, g_ColorRefData.GetHbrWhite());

	SelectObject(memdc, g_FontData.GetFontWord15());
	SetBkMode(memdc,TRANSPARENT);

	if (m_bHover)
		SetTextColor(memdc, g_ColorRefData.GetColorLightBlue());
	else
		SetTextColor(memdc, g_ColorRefData.GetColorBlack());

	DrawText(memdc, m_strText, wcslen(m_strText), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	
	DeleteDC(memdc);
	DeleteObject(bitmap);
	::EndPaint(GetHwnd(), &paintstruct);
}