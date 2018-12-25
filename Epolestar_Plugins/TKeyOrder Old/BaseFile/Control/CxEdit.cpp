#include "BaseInclude.h"

CxEdit::CxEdit()
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	
}
CxEdit::~CxEdit()
{
	
}
void CxEdit::DrawItemFrame(bool bHover, bool bFocus)
{
	HBRUSH hClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorRGB150());
	HBRUSH hHotBKBrush = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
	

	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;

	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, hHotBKBrush);
	else
	{
		::FrameRect(hDC, &rectClient, hClientBrush);
	}
	::ReleaseDC(m_hWnd, hDC);

	DeleteObject(hClientBrush);
	DeleteObject(hHotBKBrush);
}
LRESULT CALLBACK CxEdit::WndProc(UINT message, WPARAM w, LPARAM l)
{
	if (WM_NCPAINT == message || WM_PAINT == message)
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
			m_bTracking = TrackMouseEvent(&tme) == TRUE;
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
	else if (WM_CHAR == message)
	{
		if (m_iExStyle == floatStyle &&!DealFloat(w, l))
			return 0;
	}
	else if (WM_KEYDOWN == message )
	{
		if (w==VK_RETURN)
			SendMessage(GetParent(m_hWnd), SSWM_EDIT_PRESS_ENTER, GetWindowLongPtr(m_hWnd,GWL_ID), LPARAM(m_hWnd));
		else if (w==VK_TAB)
			SendMessage(GetParent(m_hWnd), SSWM_TAB_SWITCH_FOCUS, GetWindowLongPtr(m_hWnd, GWL_ID), LPARAM(m_hWnd));
		else if (VK_UP == w)
			SendMessage(GetParent(m_hWnd), WM_MOUSEWHEEL, 1 << 16, (LPARAM)m_hWnd);
		else if (VK_DOWN == w)
			SendMessage(GetParent(m_hWnd), WM_MOUSEWHEEL, -1 << 16, (LPARAM)m_hWnd);
		//return 0;
	}
	
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}

void CxEdit::Create(HWND hParent, UINT style,UINT specialStyle, UINT id)
{
	m_iExStyle = style;
	UINT editStyle = ES_AUTOVSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS | specialStyle;
	if (style == intStyle)
		editStyle |= ES_NUMBER;
	HWND hedit = CreateWindowEx(0, WC_EDIT, TEXT(""), editStyle, 0, 0, 0, 0, hParent, (HMENU)(id), GetModuleHandle(NULL), NULL);
	Init(hedit);

}
void CxEdit::SetLimitText(UINT length)
{
	SendMessage(m_hWnd, EM_SETLIMITTEXT, length, 0);
}
void CxEdit::SetWindowNumber(int number)
{
	TCHAR szText[100] = { 0 };
	wsprintf(szText, L"%d", number);
	SetWindowText(m_hWnd, szText);
}
void CxEdit::SetDoubleData(double data)
{
	char szText[100] = { 0 };
	sprintf_s(szText, "%.2lf", data);
	SetWindowTextA(m_hWnd, szText);
}
int CxEdit::GetWindowNumber()
{
	TCHAR szText[100] = { 0 };
	GetWindowText(m_hWnd, szText, sizeof(szText) / sizeof(szText[0]));

	return _wtoi(szText);
}
void CxEdit::GetText(TCHAR *pText, int length)
{
	GetWindowText(m_hWnd, pText, length);
}
void CxEdit::SetText(const TCHAR *pText)
{
	SetWindowText(m_hWnd, pText);
}
void CxEdit::SetText(const char* pText)
{
	SetWindowTextA(m_hWnd, pText);
}
void CxEdit::GetText(char*pText, int length)
{
	GetWindowTextA(m_hWnd, pText, length);
}
double CxEdit::GetDoubleData()
{
	TCHAR szText[100] = { 0 };
	GetWindowText(m_hWnd, szText, sizeof(szText) / sizeof(szText[0]));
	return _wtof(szText);
}
bool CxEdit::DealFloat(WPARAM wParam, LPARAM lParam)
{
	static bool dotFlag = true;
	TCHAR szText[50] = { 0 };
	GetWindowText(m_hWnd, szText, sizeof(szText));
	if (lstrlen(szText) == 0)
		dotFlag = true;
	TCHAR * pC = wcsrchr(szText, L'.');
	if (pC == NULL)
		dotFlag = true;
	if (wParam == '.')
	{
		if (!dotFlag)
			return false;
		dotFlag = false;
	}
	else if (wParam == '+' || wParam == '-')
	{
		if (lstrlen(szText) != 0)
			return false;
	}
	else if ((wParam<'0' || wParam>'9') && wParam != VK_BACK)
		return false;
	return true;
}