#include "CxEdit.h"
#include "UIDesign.h"

#include <CommCtrl.h>
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
	if ( WM_NCPAINT == message|| WM_PAINT == message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus);
		return 1;
	}
	else if (WM_MOUSEMOVE == message)
	{
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 50;
			m_bTracking = _TrackMouseEvent(&tme) == TRUE;
		}
	}
	else if (WM_MOUSELEAVE == message)
	{
		m_bTracking = FALSE;
		m_bHover = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_MOUSEHOVER == message)
	{
		m_bHover = TRUE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_SETFOCUS == message)
	{
		m_bFocus = TRUE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_KILLFOCUS == message)
	{
		m_bFocus = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);		
	}
	
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}

