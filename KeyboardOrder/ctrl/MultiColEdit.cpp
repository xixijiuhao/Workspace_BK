#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;

CMultiColEdit::CMultiColEdit(CMultiColComboBox& Combo, COFInputDlg&ref) :m_Combo(Combo)
, m_ref(ref)
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	m_ClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorWhite()/*GetSysColor(COLOR_BTNFACE)*/);
	m_hOrderBrush = ::CreateSolidBrush(/*GetSysColor(COLOR_WINDOWFRAME)*/ g_ColorRefData.GetColorRGB150());
	m_HotOrderBrush1 = ::CreateSolidBrush(RGB(96, 200, 253));
	m_HotOrderBrush2 = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
}
CMultiColEdit::~CMultiColEdit()
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
void CMultiColEdit::InitEdit(HWND hEdit)
{
	Init(hEdit);
}
LRESULT CALLBACK CMultiColEdit::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KEYDOWN)
	{
		if (::IsWindow(m_ref.GetHwnd()) && m_ref.DoLeftRightUpDownKey(wParam, true))
			return 1;
		bool bShiftDown = (0x8000 & GetKeyState(VK_SHIFT))!=0;
		if (VK_TAB == wParam || VK_RETURN == wParam)
		{
			if (::IsWindow(m_ref.GetHwnd()))
			{
				if (bShiftDown)
					m_ref.SetIndexFocus(m_ref.GetLastFocusIndex(GetDlgCtrlID(GetParent(GetHwnd()))), true, false);
				else
					m_ref.SetIndexFocus(m_ref.GetNextFocusIndex(GetDlgCtrlID(GetParent(GetHwnd()))), true);
				return 1;
			}
		}
		int key = wParam;
		wchar_t strText[101] = { 0 };
		Edit_GetText(m_hWnd, strText, sizeof(strText) / sizeof(strText[0]));
		if (m_Combo.m_bAutoComplete)
		{
			if (strText[0]!='\0' && (key == VK_DELETE || key == VK_BACK))
				m_Combo.m_bAutoComplete = false;
		}
	}
	else if(WM_NCPAINT == message || WM_PAINT == message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam);
		DrawHelper::DrawFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
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
		DrawHelper::DrawFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
	}
	else if (WM_MOUSEHOVER == message)
	{
		m_bHover = TRUE;
		DrawHelper::DrawFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
	}
	else if (WM_SETFOCUS == message)
	{
		m_bFocus = TRUE;
		DrawHelper::DrawFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
	}
	else if (WM_KILLFOCUS == message)
	{
		m_bFocus = FALSE;
		DrawHelper::DrawFrame(m_Combo.GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}