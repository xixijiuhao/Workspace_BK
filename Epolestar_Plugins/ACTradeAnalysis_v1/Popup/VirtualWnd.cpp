#include "BaseInclude.h"

VirtualWnd::VirtualWnd()
{
	m_TitleName = L"";
	m_rectClose = { 0 };
	m_rectMini = { 0 };
	m_colorCaption = RGB(76,112,177);
	m_colorBk = g_ColorRefData.g_ColorBlack;
	//m_colorBk = RGB(39, 38, 46);
	Wnd_Width = 0;
	Wnd_Height = 0;
	m_bhoverclose = false;
	m_bhovermini = false;
	m_bmousetrack = false;

	MinWidth = 360;
	MinHeight = 360;
}
VirtualWnd::~VirtualWnd()
{
	m_TitleName = L"";
}
void VirtualWnd::ShowFrm(HWND hParent, TCHAR* WndName, wstring TitleName)
{
	
	if (m_TitleName == TitleName)
		return;
	m_TitleName = TitleName;
	CreateFrm(WndName, NULL, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW  | WS_EX_APPWINDOW);

	m_hParent = hParent;

	m_bhoverclose = false;
	m_bhovermini = false;
	m_bmousetrack = false;
}
void VirtualWnd::MoveWindow(int x, int y, int cx, int cy)
{
	Wnd_Width = cx;
	Wnd_Height = cy;
	SetWindowPos(m_Hwnd, HWND_TOP, x, y, cx, cy, SWP_NOZORDER|SWP_SHOWWINDOW);
	//InvalidateRect(m_Hwnd, NULL, true);
	UpdateWindow(m_Hwnd);
}

//这里会根据消息机制，在ACAnalysisPresenter窗体中画K线图
LRESULT VirtualWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		OnCreate();
		break;
	case WM_PAINT:
		OnPaint();
		break;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClick(wParam, lParam);
		break;
	case WM_COMMAND:
		return OnCommand(wParam, lParam); 
	case WM_NOTIFY:
		return OnNotify(wParam, lParam);
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		break;
	case WM_NCDESTROY:
		return OnNcDestory(wParam, lParam);
	case WM_NCHITTEST: //鼠标的所有消息都有这个产生
		//OnNcHitTest(wParam, lParam); //OnNcHiTest处理鼠标所在区域，返回相应消息
		return NOT_PROCESSED;
	case WM_SETCURSOR:
		//OnSetCursor(wParam, lParam); //设置鼠标形态，根据鼠标所在边框
		return PROCESSED;
	case WM_NCLBUTTONDOWN:          //发送消息，实现窗口的缩放，消息由DefWindowProc处理
		//OnNcLButtonDown(wParam, lParam);
		return NOT_PROCESSED;
	case WM_SIZING:
		//OnSizeing(wParam, lParam);
		return NOT_PROCESSED;
	default:
		 break;
	}
	return OnDealMsg(message, wParam, lParam);
}

void VirtualWnd::OnSizeing(WPARAM wParam, LPARAM lParam)
{
	LPRECT pRect = (LPRECT)lParam;
	if (pRect->bottom - pRect->top < MinHeight)
	{
		if (wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT)
		{
			pRect->bottom = pRect->top + MinHeight;
		}
		else
		{
			pRect->top = pRect->bottom - MinHeight;
		}
	}
	if (pRect->right - pRect->left < MinWidth)
	{
		if (wParam == WMSZ_RIGHT || wParam == WMSZ_TOPRIGHT || wParam == WMSZ_BOTTOMRIGHT)
		{
			pRect->right = pRect->left + MinWidth;
		}
		else
		{
			pRect->left = pRect->right - MinWidth;
		}
	}
}

void VirtualWnd::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	int iSavaDc = SaveDC(memdc.GetHdc());
	HBRUSH hbrCaption = CreateSolidBrush(RGB(39, 38, 46));
	HBRUSH hbrFrameBk = CreateSolidBrush(m_colorBk);
	HBRUSH hbrRed = CreateSolidBrush(RGB(201, 51, 39));
	HPEN   penRed = CreatePen(PS_SOLID, 1, RGB(201, 51, 39));
	HPEN   penBlack = CreatePen(PS_SOLID, 1, RGB(0 ,0 ,0));
	HPEN   penWhite = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_black);
	Wnd_Width = rect.right;
	Wnd_Height = rect.bottom;
	RECT rc = rect;
	rc.bottom = CAPTION_HEIGHT;
	FillRect(memdc.GetHdc(), &rc, hbrCaption);
	rc.top = rc.bottom-2; 
	rc.bottom = rect.bottom;
	FillRect(memdc.GetHdc(), &rc, hbrFrameBk);
	FrameRect(memdc.GetHdc(), &rc, hbrCaption);
	InflateRect(&rc, -1, -1);
	FrameRect(memdc.GetHdc(), &rc, hbrCaption);
	

	//写标题
	SetTextColor(memdc.GetHdc(), RGB(240,240,240));
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord13);

	rc.left = 0; rc.right = Wnd_Width ; 
	rc.top = rect.top;  rc.bottom = rect.top + CAPTION_HEIGHT;
	DrawText(memdc.GetHdc(), m_TitleName.c_str(), -1, &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	m_rectClose = rect;
	m_rectClose.bottom = CAPTION_HEIGHT; 
	m_rectClose.left = m_rectClose.right - CAPTION_HEIGHT;
	m_rectMini.left = m_rectClose.left + 10; m_rectMini.right = m_rectClose.right - 10;
	m_rectMini.top = m_rectClose.top + 10; m_rectMini.bottom = m_rectClose.bottom - 10;
	////画关闭按钮
	//if (m_bhoverclose)
	//{
	//	rc = m_rectClose; rc.bottom = m_rectClose.bottom + 2;
	//	SelectObject(memdc.GetHdc(), penRed);
	//	FillRect(memdc.GetHdc(), &rc, hbrRed);
	//	SelectObject(memdc.GetHdc(), penWhite);
	//}
	//else
	//{
	//	SelectObject(memdc.GetHdc(), penWhite);
	//}
	//rc = m_rectMini;
	//MoveToEx(memdc.GetHdc(), rc.left, rc.top, 0);
	//LineTo(memdc.GetHdc(), rc.right + 1, rc.bottom + 1);
	//MoveToEx(memdc.GetHdc(), rc.right, rc.top, 0);
	//LineTo(memdc.GetHdc(), rc.left - 1, rc.bottom + 1);

	//
	////画最小化按钮
	//m_rectSizeMin.right = m_rectClose.left;
	//m_rectSizeMin.left = m_rectSizeMin.right - CAPTION_HEIGHT;
	//m_rectSizeMin.top = 0;
	//m_rectSizeMin.bottom = CAPTION_HEIGHT;
	//if (m_bSizeMin) {
	//	FillRect(memdc.GetHdc(), &m_rectSizeMin, hbrRed);
	//}
	//MoveToEx(memdc.GetHdc(), m_rectSizeMin.left + 10, 20, NULL);
	//LineTo(memdc.GetHdc(), m_rectSizeMin.right - 10, 20);



	SelectObject(memdc.GetHdc(), penBlack);
	SetTextColor(memdc.GetHdc(), RGB(10, 10, 10));
	DrawMain(memdc.GetHdc());	
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
	HWND hParent = ::GetParent(m_Hwnd);
	EnableWindow(hParent, true);
	//DestroyWindow(m_Hwnd);
	ShowWindow(m_Hwnd, SW_HIDE);
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

	m_bSizeMin = false;
	if (PtInRect(&m_rectSizeMin, pt))
		m_bSizeMin = true;

	InvalidateRect(m_Hwnd, 0, false);
}
void VirtualWnd::OnMouseLeave()
{
	m_bmousetrack = false;
	m_bhoverclose = false;
	m_bhovermini = false;
	m_bSizeMin = false;
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
	if (PtInRect(&rect, pt) && !PtInRect(&m_rectClose, pt) && !PtInRect(&m_rectSizeMin, pt))
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
		OnClose();
		CloseWnd();
	}
	else if (PtInRect(&m_rectSizeMin, pt)) {
		ShowWindow(m_Hwnd, SW_MINIMIZE);
	}
	
}


UINT VirtualWnd::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = GET_X_LPARAM(lParam);
	point.y = GET_Y_LPARAM(lParam);
	RECT rect;
	GetWindowRect(m_Hwnd, &rect);
	if (point.x <= rect.left + 10 && point.y <= rect.top + 10)
	{
		return HTTOPLEFT;
	}
	else if (point.x >= rect.right - 10 && point.y <= rect.top + 10)
	{
		return HTTOPRIGHT;
	}
	else if (point.x <= rect.left + 10 && point.y >= rect.bottom - 10)
	{
		return HTBOTTOMLEFT;
	}
	else if (point.x >= rect.right - 10 && point.y >= rect.bottom - 10)
	{
		return HTBOTTOMRIGHT;
	}
	else if (point.x <= rect.left + 5)
	{
		return HTLEFT;
	}
	else if (point.x >= rect.right - 5)
	{
		return HTRIGHT;
	}
	else if (point.y <= rect.top + 5)
	{
		return HTTOP;
	}
	else if (point.y >= rect.bottom - 5)
	{
		return HTBOTTOM;
	}

	return NOT_PROCESSED;
}
void VirtualWnd::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam != m_Hwnd)
		return;
	UINT nHitTest = LOWORD(lParam);
	switch (nHitTest)
	{
	case HTTOP:
	case HTBOTTOM:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		break;
	case HTLEFT:
	case HTRIGHT:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
		break;
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
		break;
	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
		break;
	case HTCAPTION:
	case HTSYSMENU:
	case HTMENU:
	case HTCLIENT:
	default:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		break;
	}
}
void VirtualWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case HTTOP:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, lParam);
		break;
	case HTBOTTOM:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, lParam);
		break;
	case HTLEFT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, lParam);
		break;
	case HTRIGHT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, lParam);
		break;
	case HTTOPLEFT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, lParam);
		break;
	case HTTOPRIGHT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, lParam);
		break;
	case HTBOTTOMLEFT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, lParam);
		break;
	case HTBOTTOMRIGHT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, lParam);
		break;
	default:
		break;
	}
}