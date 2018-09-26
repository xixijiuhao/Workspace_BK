#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CxKeyBoardButton::CxKeyBoardButton(COFInputDlg *ref) :m_Ref(ref)
{

}
CxKeyBoardButton::CxKeyBoardButton() :m_Ref(NULL)
{

}
BOOL CxKeyBoardButton::Init(HWND hWnd)
{
	m_bTracking = m_bHover = m_bFocus = m_bDown =FALSE;
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | BS_OWNERDRAW);
	return CxSubClassWnd::Init(hWnd);
}
CxKeyBoardButton::~CxKeyBoardButton()
{

}
void CxKeyBoardButton::SetButtonText(wstring wText)
{
	m_sText = wText;
	InvalidateRect(m_hWnd, NULL, FALSE);
}
void CxKeyBoardButton::DrawItemFrame(bool bHover, bool bFocus, bool bDown)
{
	HBRUSH hClientBrush = g_ColorRefData.GetHbrRGB150();
	HBRUSH hNormalBkBrush = g_ColorRefData.GetHbrWhite();
	HBRUSH hDisableBkBrush = g_ColorRefData.GetHbrBackground();
	HBRUSH hHotBKBrush = g_ColorRefData.GetHbrLightBlue();
	

	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;

	HDC		m_HdcMemNormal;
	HBITMAP  m_hbitmapSwitchF;
	HFONT   hFont = CreateFont((rect.top - rect.bottom) / 2, 0, 0, 0, 500, FALSE, FALSE, FALSE, 0, 0, 0, 0, 0, g_FontData.GetFontNumName().c_str());//
	m_HdcMemNormal = CreateCompatibleDC(hDC);
	m_hbitmapSwitchF = CreateCompatibleBitmap(hDC, rectClient.right, rectClient.bottom);
	SelectObject(m_HdcMemNormal, m_hbitmapSwitchF);
	SetBkMode(m_HdcMemNormal, TRANSPARENT);

	SelectObject(m_HdcMemNormal, hFont);
	if (bDown)
		FillRect(m_HdcMemNormal, &rectClient, hHotBKBrush);
	else
	{
		if (!IsWindowEnabled(m_hWnd))
			FillRect(m_HdcMemNormal, &rectClient, hDisableBkBrush);
		else
			FillRect(m_HdcMemNormal, &rectClient, hNormalBkBrush);
	}

	SetTextColor(m_HdcMemNormal, g_ColorRefData.GetColorBlack());
	DrawText(m_HdcMemNormal, m_sText.c_str(), m_sText.size(), &rectClient, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	BitBlt(hDC, 0, 0, rectClient.right, rectClient.bottom, m_HdcMemNormal, 0, 0, SRCCOPY);

	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, hHotBKBrush);
	else
		::FrameRect(hDC, &rectClient, hClientBrush);

	::ReleaseDC(m_hWnd, hDC);
	DeleteObject(m_HdcMemNormal);
	DeleteObject(m_hbitmapSwitchF);
	DeleteObject(hFont);
}
LRESULT CALLBACK CxKeyBoardButton::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
			if (m_Ref)
			{
				if (::IsWindow(m_Ref->GetHwnd()) && m_Ref->DoLeftRightUpDownKey(wParam))
					return 0;
				if (VK_TAB == wParam)
				{
					bool bShiftDown = (0x8000 & GetKeyState(VK_SHIFT))!=0;
					if (::IsWindow(m_Ref->GetHwnd()))
					{
						if (bShiftDown)
							m_Ref->SetIndexFocus(m_Ref->GetLastFocusIndex(GetDlgCtrlID(GetHwnd())), true, false);
						else
							m_Ref->SetIndexFocus(m_Ref->GetNextFocusIndex(GetDlgCtrlID(GetHwnd())), true);
						return 0;
					}
				}
				else if (VK_RETURN == wParam)
				{
					if (::IsWindow(m_Ref->GetHwnd()))
						m_Ref->DisposeEnterMsg(wParam);
					return 0;
				}
			}
			break;	
		case WM_NCPAINT:
		case WM_PAINT:
		{
			CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam);
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			return 1;
		}
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
		{
			m_bTracking = false;
			m_bHover = false;
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			break;
		}
		case WM_MOUSEHOVER:
		{
			m_bHover = true;
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			break;
		}
		case WM_SETFOCUS:
		{
			m_bFocus = true;
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			break;
		}
		case WM_KILLFOCUS:
		{
			m_bFocus = false;
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			m_bDown = true;
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			break;
		}
		case WM_LBUTTONUP:
		{
			m_bDown = false;
			DrawItemFrame(m_bHover, m_bFocus, m_bDown);
			break;
		}
		case WM_ERASEBKGND:
			return true;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程

}
