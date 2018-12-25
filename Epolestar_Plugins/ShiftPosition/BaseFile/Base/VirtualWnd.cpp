#include "BaseInclude.h"

VirtualWnd::VirtualWnd()
{
	m_TitleName = "";
	m_rectClose = { 0 };
	m_rectMini = { 0 };
	m_colorCaption = RGB(100,100,100);
	m_colorBk = RGB(255 ,255, 255);
	Wnd_Width = 0;
	Wnd_Height = 0;
	m_bhoverclose = false;
	m_bhovermini = false;
	m_bmousetrack = false;

}
VirtualWnd::~VirtualWnd()
{
	m_TitleName = "";
}
void VirtualWnd::ShowFrm(HWND hParent, TCHAR* WndName, string TitleName)
{
	if (m_TitleName == TitleName)
		return;
	m_TitleName = TitleName;
	CreateFrm(WndName, hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);

	m_bhoverclose = false;
	m_bhovermini = false;
	m_bmousetrack = false;
}
void VirtualWnd::MoveWindow(int x, int y, int cx, int cy)
{
	Wnd_Width = cx;
	Wnd_Height = cy;
	SetWindowPos(m_Hwnd, HWND_TOP, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, NULL, true);
}
void VirtualWnd::ModifyTitle(string TitleName)
{
	m_TitleName = TitleName;
	InvalidateRect(m_Hwnd, NULL, true);
}
LRESULT VirtualWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClick(wParam, lParam);
		return PROCESSED;
	case WM_COMMAND:
		return OnCommand(wParam, lParam); 
	case WM_NOTIFY:
		return OnNotify(wParam, lParam);
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		break;
	case WM_NCDESTROY:
		return OnNcDestory(wParam, lParam);
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)g_ColorRefData.g_brush_white;
	default:
		 break;
	}
	return OnDealMsg(message, wParam, lParam);
}
void VirtualWnd::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	int iSavaDc = SaveDC(memdc.GetHdc());
	HBRUSH hbrCaption = CreateSolidBrush(m_colorCaption);
	HBRUSH hbrFrameBk = CreateSolidBrush(m_colorBk);
	HBRUSH hbrRed = CreateSolidBrush(RGB(201, 51, 39));
	HPEN   penRed = CreatePen(PS_SOLID, 1, RGB(201, 51, 39));
	HPEN   penBlack = CreatePen(PS_SOLID, 1, RGB(0 ,0 ,0));
	HPEN   penWhite = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	


	RECT rc = rect;
	rc.top = CAPTION_HEIGHT; rc.bottom = rect.bottom;//背景
	FillRect(memdc.GetHdc(), &rc, hbrFrameBk);
	rc = rect;
	FrameRect(memdc.GetHdc(), &rc, hbrCaption);
	InflateRect(&rc, -1, -1);
	FrameRect(memdc.GetHdc(), &rc, hbrCaption);

	rc = rect;
	rc.bottom = CAPTION_HEIGHT;
	FillRect(memdc.GetHdc(), &rc, hbrCaption);//标题

	
	//写标题
	SetTextColor(memdc.GetHdc(), RGB(240,240,240));
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord13());
	rc.left = 8 ; rc.right = Wnd_Width ; rc.top = rect.top;  rc.bottom = rect.top + CAPTION_HEIGHT;
	DrawTextA(memdc.GetHdc(), m_TitleName.c_str(), -1, &rc, DT_VCENTER |DT_LEFT|DT_SINGLELINE);

	m_rectClose = rect;
	m_rectClose.bottom = CAPTION_HEIGHT; m_rectClose.left = m_rectClose.right - CAPTION_HEIGHT;
	m_rectMini.left = m_rectClose.left + 10; m_rectMini.right = m_rectClose.right - 10;
	m_rectMini.top = m_rectClose.top + 10; m_rectMini.bottom = m_rectClose.bottom - 10;
	//画关闭按钮
	if (m_bhoverclose)
	{
		rc = m_rectClose; rc.bottom = m_rectClose.bottom;
		SelectObject(memdc.GetHdc(), penRed);
		FillRect(memdc.GetHdc(), &rc, hbrRed);
		SelectObject(memdc.GetHdc(), penWhite);
	}
	else
	{
		SelectObject(memdc.GetHdc(), penWhite);
	}

	rc = m_rectMini;

	MoveToEx(memdc.GetHdc(), rc.left, rc.top, 0);
	LineTo(memdc.GetHdc(), rc.right + 1, rc.bottom + 1);
	MoveToEx(memdc.GetHdc(), rc.right, rc.top, 0);
	LineTo(memdc.GetHdc(), rc.left - 1, rc.bottom + 1);


	SelectObject(memdc.GetHdc(), penBlack);
	SetTextColor(memdc.GetHdc(), RGB(10, 10, 10));
	DrawMain(memdc);	
	RestoreDC(memdc.GetHdc(), iSavaDc);

	DeleteObject(hbrCaption);
	DeleteObject(hbrFrameBk);
	DeleteObject(hbrRed);
	DeleteObject(penRed);
	DeleteObject(penBlack);
	DeleteObject(penWhite);
	
}
void VirtualWnd::CloseWnd()
{
	if (IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_HIDE);
	else
		ShowWindow(m_Hwnd, SW_SHOW);
}
void VirtualWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bmousetrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);

		m_bmousetrack = true;
	}
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	m_bhoverclose = false;
	if (PtInRect(&m_rectClose, pt))
		m_bhoverclose = true;

	m_bhovermini = false;
	if (PtInRect(&m_rectMini, pt))
		m_bhovermini = true;

	InvalidateRect(m_Hwnd, 0, false);
}
void VirtualWnd::OnMouseLeave()
{
	m_bmousetrack = false;
	m_bhoverclose = false;
	m_bhovermini = false;
	InvalidateRect(m_Hwnd, 0, false);
}
void VirtualWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	rect.bottom = rect.top +  CAPTION_HEIGHT;
	if (PtInRect(&rect, pt) && !PtInRect(&m_rectClose, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
	}
}
void VirtualWnd::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if (PtInRect(&m_rectClose, pt))
	{
		CloseWnd();
	}
}

