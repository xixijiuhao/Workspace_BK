#include "PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;

CxEdit::CxEdit()
{
	m_bTracking = m_bHover = m_bFocus = FALSE;	
}
CxEdit::~CxEdit()
{
	
}
void CxEdit::DrawItemFrame(bool bHover, bool bFocus)
{
	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;

	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrLightBlue());
	else
		::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrRGB150());

	::ReleaseDC(m_hWnd, hDC);
}
LRESULT CALLBACK CxEdit::WndProc(UINT message, WPARAM w, LPARAM l)
{
	switch (message)
	{
	case WM_NCPAINT:
	case WM_PAINT:
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus);
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
		m_bTracking = FALSE;
		m_bHover = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);
		break;
	case WM_MOUSEHOVER:
		m_bHover = TRUE;
		DrawItemFrame(m_bHover, m_bFocus);
		break;
	case WM_SETFOCUS:
		m_bFocus = TRUE;
		DrawItemFrame(m_bHover, m_bFocus);
		break;
	case WM_KILLFOCUS:
		m_bFocus = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);
		break;
	}

	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}

