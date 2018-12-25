#include "PreInclude.h"


CxCombox::CxCombox()
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	m_hEditBrushNormal = ::CreateSolidBrush(g_ColorRefData.GetColorWhite());
	m_hEditBrushInvalid = ::CreateSolidBrush(g_ColorRefData.GetColorBackground());
}
CxCombox::~CxCombox()
{
	DeleteObject(m_hEditBrushNormal);
	DeleteObject(m_hEditBrushInvalid);
}
void CxCombox::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;
	
	int nMidx = nStartx + nWidth / 2;
	int nMidy = nStarty + nWidth / 2;
	MoveToEx(hdc, nStartx, nStarty, NULL);
	LineTo(hdc, nMidx, nMidy);

	int nEndx = nStartx + nWidth - 1;
	int nEndy = nStarty;
	LineTo(hdc, nEndx + 1, nEndy - 1);
}
void CxCombox::DrawItemFrame(bool bHover, bool bFocus,bool bArrow)
{
	HBRUSH hClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorBtnFace());
	HBRUSH hNormalBKBrush = ::CreateSolidBrush(g_ColorRefData.GetColorRGB150());
	HBRUSH hHotOrderBrush2 = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
	HPEN   hGrayPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());

	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;
	if (bArrow)
	{
		int nWidth = GetSystemMetrics(SM_CXHTHUMB);
		int nxBorder = GetSystemMetrics(SM_CXBORDER);
		int nyBorder = GetSystemMetrics(SM_CYBORDER);
		RECT rectArrow(rectClient);
		InflateRect(&rectArrow, -nxBorder, -nyBorder);
		rectArrow.left = rectArrow.right - nWidth-1;
		
		if (IsWindowEnabled(m_hWnd))
		{
			::SetBkColor(hDC, g_ColorRefData.GetColorWhite());
			::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);
		}			
		else
		{
			//禁用状态TEXT重绘
			HFONT hfont = (HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0);
			HFONT hOldFont = (HFONT)SelectObject(hDC, hfont);
			SetBkMode(hDC, TRANSPARENT);
			wchar_t wText[21] = { 0 };
			GetWindowText(m_hWnd, wText, 21);
			::SetBkColor(hDC, g_ColorRefData.GetColorBackground());
			::SetTextColor(hDC, g_ColorRefData.GetColorRGB150());
			RECT rectText(rectClient);
			InflateRect(&rectText, -nxBorder * 3, -nyBorder * 3);
			rectText.right = rectArrow.left;
			::ExtTextOut(hDC, 3, 3, ETO_OPAQUE, &rectText, wText, wcslen(wText), NULL);
			::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);
			SelectObject(hDC, hOldFont);
		}
			
		SelectObject(hDC, hGrayPen);

		MoveToEx(hDC, rectArrow.left, rectArrow.top, NULL);
		LineTo(hDC, rectArrow.left, rectArrow.bottom);

		DrawTriangle(hDC, rectArrow.left + nWidth / 3, rectArrow.bottom/2 - nWidth / 6+1 , nWidth / 6 * 2 + 3);
	}
	if (bHover || bFocus)
	{
		::FrameRect(hDC, &rectClient, hClientBrush);
		InflateRect(&rectClient, -1, -1);
		::FrameRect(hDC, &rectClient, hHotOrderBrush2);
	}
	else
	{
		::FrameRect(hDC, &rectClient, hClientBrush);
		InflateRect(&rectClient, -1, -1);
		::FrameRect(hDC, &rectClient, hNormalBKBrush);
	}
	::ReleaseDC(m_hWnd, hDC);

	DeleteObject(hClientBrush);
	DeleteObject(hNormalBKBrush);
	DeleteObject(hHotOrderBrush2);
	DeleteObject(hGrayPen);
}
LRESULT CALLBACK CxCombox::WndProc(UINT message, WPARAM w, LPARAM l)
{
	if (WM_NCPAINT == message||WM_PAINT==message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus,true);
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


void CxCombox::AddString(const TCHAR *pszText, int index)
{
	SendMessage(m_hWnd, CB_INSERTSTRING, (WPARAM)index, (LPARAM)pszText);
}
int CxCombox::DeleteString(int index)
{
	return SendMessage(m_hWnd, CB_DELETESTRING, (WPARAM)index, 0);
}
void CxCombox::SetSelect(int index)
{
	SendMessage(m_hWnd, CB_SETCURSEL, index, 0);
}
void CxCombox::Create(HWND hParent, UINT id)
{
	HWND hcb = CreateWindowEx(0, WC_COMBOBOX, TEXT(""), WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL | WS_CLIPSIBLINGS | WS_TABSTOP,
		0, 0, 0, 0, hParent, (HMENU)(id), GetModuleHandle(NULL), NULL);
	Init(hcb);
	SendMessage(hcb, WM_SETFONT, (WPARAM)g_FontData.g_FontWord12, 0);
}
int CxCombox::GetSelect()
{
	return SendMessage(m_hWnd, CB_GETCURSEL, 0, 0);
}
void CxCombox::SetSelect(const TCHAR *pszText)
{
	int index = SendMessage(m_hWnd, CB_FINDSTRING, 0, (LPARAM)pszText);
	SetSelect(index);
}

const wchar_t* CxCombox::GetText()
{
	GetWindowText(m_hWnd, m_szContent, ARRAYSIZE(m_szContent));
	return m_szContent;
}