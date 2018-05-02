#include "stdafx.h"
#include "CxCombox.h"
#include "UIDesign.h"

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
			::SetBkColor(hDC, g_ColorRefData.GetColorWhite());
		else
			::SetBkColor(hDC, g_ColorRefData.GetColorBackground());

		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rectArrow, NULL, 0, NULL);

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

	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}
