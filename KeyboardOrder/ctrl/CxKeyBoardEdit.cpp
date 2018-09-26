#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;

CxKeyBoardEdit::CxKeyBoardEdit(COFInputDlg&ref):m_RefDlg(ref)
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	m_ClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorWhite()/*GetSysColor(COLOR_BTNFACE)*/);
	m_hOrderBrush = ::CreateSolidBrush(/*GetSysColor(COLOR_WINDOWFRAME)*/g_ColorRefData.GetColorRGB150());
	m_HotOrderBrush1 = ::CreateSolidBrush(RGB(96, 200, 253));
	m_HotOrderBrush2 = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
}
CxKeyBoardEdit::~CxKeyBoardEdit()
{
	if (m_ClientBrush)
		DeleteObject(m_ClientBrush);
	if (m_hOrderBrush)
		DeleteObject(m_hOrderBrush);
	if (m_HotOrderBrush1)
		DeleteObject(m_HotOrderBrush1);
	if (m_HotOrderBrush2)
		DeleteObject(m_HotOrderBrush2);
}
LRESULT CALLBACK CxKeyBoardEdit::WndProc(UINT message, WPARAM w, LPARAM l)
{
	if (WM_NCPAINT == message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
		return 1;
	}
	else if (WM_LBUTTONDBLCLK == message)
	{
		if (::IsWindow(m_RefDlg.GetHwnd()))
		{
			if (m_RefDlg.DoWndDbClick(GetHwnd(), l))
				return 0;
		}
	}
	else if (WM_CHAR == message)
	{
		if (::IsWindow(m_RefDlg.GetHwnd()))
		{
			if (m_RefDlg.DoWndKeyDown(::GetFocus(), w))
				return 0;
		}
	}
	else if (WM_KEYDOWN == message)
	{
		if (::IsWindow(m_RefDlg.GetHwnd()) && m_RefDlg.DoLeftRightUpDownKey(w))
			return 0;
		if (VK_RETURN == w&&::IsWindow(m_RefDlg.GetHwnd()) && m_RefDlg.DealEnterSendOrder(w))
			return 1;
		if (VK_TAB == w || VK_RETURN == w)
		{
			bool bShiftDown = (0x8000 & GetKeyState(VK_SHIFT)) != 0;
			if (::IsWindow(m_RefDlg.GetHwnd()))
			{
				if (bShiftDown)
					m_RefDlg.SetIndexFocus(m_RefDlg.GetLastFocusIndex(GetDlgCtrlID(GetHwnd())), true, false);
				else
					m_RefDlg.SetIndexFocus(m_RefDlg.GetNextFocusIndex(GetDlgCtrlID(GetHwnd())), true);
				return 0;
			}
		}
		else if (VK_UP == w||VK_DOWN == w)
		{
			if (m_RefDlg.DoWndKeyDown(::GetFocus(), w))
				return 0;
		}
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
		DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
	}
	else if (WM_MOUSEHOVER == message)
	{
		m_bHover = TRUE;
		DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
	}
	else if (WM_SETFOCUS == message)
	{	
		m_bFocus = TRUE;
		DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
	}
	else if (WM_KILLFOCUS == message)
	{
		m_bFocus = FALSE;
		DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
	}
	
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}

