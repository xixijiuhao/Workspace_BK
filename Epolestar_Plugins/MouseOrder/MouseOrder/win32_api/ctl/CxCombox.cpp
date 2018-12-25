#include "..\PreInclude.h"

//色彩
extern G_COLOREF g_ColorRefData;


CxCombox::CxCombox()
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
}
CxCombox::~CxCombox()
{
	
}
void CxCombox::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;

	HPEN   hGrayPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
	SelectObject(hdc, hGrayPen);

	int nMidx = nStartx + nWidth / 2;
	int nMidy = nStarty + nWidth / 2;
	MoveToEx(hdc, nStartx, nStarty, NULL);
	LineTo(hdc, nMidx, nMidy);

	int nEndx = nStartx + nWidth - 1;
	int nEndy = nStarty;
	LineTo(hdc, nEndx + 1, nEndy - 1);

	DeleteObject(hGrayPen);
}
void CxCombox::DrawItemFrame(bool bHover, bool bFocus,bool bArrow)
{
	HBRUSH hClientBrush = g_ColorRefData.GetHbrBtnFace();
	HBRUSH hNormalBKBrush = g_ColorRefData.GetHbrRGB150();
	HBRUSH hHotOrderBrush2 = g_ColorRefData.GetHbrLightBlue();
	
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
			
			wchar_t wText[21] = { 0 };
			GetWindowText(m_hWnd, wText, 21);
			SetBkMode(hDC, TRANSPARENT);
			SetBkColor(hDC, g_ColorRefData.GetColorBackground());
			SetTextColor(hDC, g_ColorRefData.GetColorRGB150());
			RECT rectText(rectClient);
			InflateRect(&rectText, -nxBorder * 3, -nyBorder * 3);
			rectText.right = rectArrow.left;
			::ExtTextOut(hDC, 3, 3, ETO_OPAQUE, &rectText, wText, wcslen(wText), NULL);
			::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);
			SelectObject(hDC, hOldFont);
		}
				
		DrawTriangle(hDC, rectArrow.left + nWidth / 3-1, rectArrow.bottom/2 - nWidth / 6 , nWidth / 6 * 2 + 5);
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
}
LRESULT CALLBACK CxCombox::WndProc(UINT message, WPARAM w, LPARAM l)
{
	switch (message)
	{
	case WM_PAINT:
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus, true);
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
	default:
		break;
	}

	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}
