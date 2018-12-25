#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;

CMouseComboEdit::CMouseComboEdit(CMouseComboBox& Combo) :m_Combo(Combo)
{
	
}
CMouseComboEdit::~CMouseComboEdit()
{
	
}
void CMouseComboEdit::InitEdit(HWND hEdit)
{
	Init(hEdit);
}
void CMouseComboEdit::DrawItemFrame(HWND hwnd,bool bHover, bool bFocus)
{
	HDC hDC = ::GetWindowDC(hwnd);
	RECT rect, rectClient;
	::GetWindowRect(hwnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;

	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrLightBlue());
	else
		::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrRGB150());

	::ReleaseDC(hwnd, hDC);
}
LRESULT CALLBACK CMouseComboEdit::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_KEYDOWN == message)
	{
		int key = wParam;
		wchar_t strText[101] = { 0 };
		Edit_GetText(m_hWnd, strText, sizeof(strText) / sizeof(strText[0]));
		if (m_Combo.m_bAutoComplete)
		{
			if (strText[0] != '\0' && (key == VK_DELETE || key == VK_BACK))
				m_Combo.m_bAutoComplete = false;
		}
	}
	else if(WM_NCPAINT == message || WM_PAINT == message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam);
		DrawItemFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus);
		return 1;
	}
	else if (message == WM_MOUSEWHEEL)
	{
		if (m_Combo.IsNeedForbidMosueWheel())
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
			m_bTracking = _TrackMouseEvent(&tme)==TRUE;
		}
	}
	else if (WM_MOUSELEAVE == message)
	{
		m_bTracking = FALSE;
		m_bHover = FALSE;
		DrawItemFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus);
	}
	else if (WM_MOUSEHOVER == message)
	{
		m_bHover = TRUE;
		DrawItemFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus);
	}
	else if (WM_SETFOCUS == message)
	{
		m_bFocus = TRUE;
		DrawItemFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus);
	}
	else if (WM_KILLFOCUS == message)
	{
		m_bFocus = FALSE;
		DrawItemFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus);
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}