#include "PreInclude.h"

#define CAPTION_HEIGHT		30
TModalWnd::TModalWnd(HWND hparent,const char* sztitle)
{
	m_parent = hparent;
	m_sztitle = sztitle;
	m_bhoverclose = false;
	m_btrack = false;
	m_nret = 1;
	m_bNeedEscColose = true;
}
TModalWnd::~TModalWnd()
{

}
//
bool TModalWnd::Domodal()
{
	CreateFrm(_T("TModalWnd"), m_parent, WS_POPUP /*| WS_VISIBLE */ | WS_CLIPCHILDREN);
	SetClassLongPtr(m_Hwnd, GCLP_HBRBACKGROUND, (LONG)GetClassLongPtr(m_parent, GCLP_HBRBACKGROUND));

	CreateSubCtrl();

	BecomeModal();
	SetModalRect();
	//消息循环
	MSG msg;
	while (m_bdomodal&&GetMessage(&msg, NULL, 0, 0))
	{
		if (m_bNeedEscColose)
		{
			if ((msg.message == WM_KEYDOWN&&msg.wParam == VK_ESCAPE)
				|| (msg.message == WM_SYSCOMMAND&&msg.wParam == SC_CLOSE&&!msg.lParam))
			{
				m_nret = 0;
				DestroyWindow(m_Hwnd);
				continue;
			}
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	bool bret =false; if (m_nret) bret = true;
	return bret;
}

//MSG
void TModalWnd::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord12);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	HBRUSH hbr;

	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_white);

	hbr = CreateSolidBrush(RGB(70, 70, 70));
	FrameRect(memdc.GetHdc(), &rect, hbr);
	DeleteObject(hbr);
	hbr = CreateSolidBrush(RGB(0, 130, 154));
	FillRect(memdc.GetHdc(), &m_rctitle, hbr);

	m_rcclose = rect;
	m_rcclose.top = CAPTION_HEIGHT / 3;
	m_rcclose.bottom = m_rcclose.top + CAPTION_HEIGHT / 3;
	m_rcclose.left = m_rcclose.right - CAPTION_HEIGHT * 2 / 3;
	m_rcclose.right = m_rcclose.left + CAPTION_HEIGHT / 3;

	HPEN hpen;
	if (m_bhoverclose)
		hpen = CreatePen(PS_SOLID, 1, g_ColorRefData.g_ColorWhite);
	else
		hpen = CreatePen(PS_SOLID, 1, g_ColorRefData.g_ColorGray);
	SelectObject(memdc.GetHdc(), hpen);

	MoveToEx(memdc.GetHdc(), m_rcclose.left, m_rcclose.top, 0);
	LineTo(memdc.GetHdc(), m_rcclose.right + 1, m_rcclose.bottom + 1);
	MoveToEx(memdc.GetHdc(), m_rcclose.left, m_rcclose.bottom, 0);
	LineTo(memdc.GetHdc(), m_rcclose.right + 1, m_rcclose.top - 1);								//关闭按钮
	InflateRect(&m_rcclose, 1, 1);
	DeleteObject(hpen);


	RECT rctxt = m_rctitle;
	rctxt.left = 5;
	SetTextColor(memdc.GetHdc(), g_ColorRefData.g_ColorWhite);
	DrawTextA(memdc.GetHdc(), m_sztitle.c_str(),-1, &rctxt, DT_LEFT | DT_VCENTER|DT_SINGLELINE);
	DeleteObject(hbr);

	DrawMain(&memdc);
}

void TModalWnd::OnSize()
{
	RECT r, rect;
	GetClientRect(m_Hwnd, &rect);
	m_rctitle = r = rect; m_rctitle.bottom = CAPTION_HEIGHT;
}

void TModalWnd::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);

	if (MK_LBUTTON == wParam&& PtInRect(&m_rctitle, pt) && !PtInRect(&m_rcclose, pt))
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam/*MAKELPARAM(point.x, point.y)*/);
	}
}

void TModalWnd::OnMouseMove()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);

	if (!m_btrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
	}

	if (PtInRect(&m_rcclose, pt))
	{
		m_bhoverclose = true;
	}
	else
		m_bhoverclose = false;
	InvalidateRect(m_Hwnd, 0, false);
}

void TModalWnd::OnMouseLeave()
{
	m_btrack = false;
	m_bhoverclose = false;
	InvalidateRect(m_Hwnd, 0, false);
}

LRESULT TModalWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_SIZE:
		OnSize();
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_Hwnd, &pt);

		if (PtInRect(&m_rcclose, pt))
		{
			m_nret = 0;
			DestroyWindow(m_Hwnd);
		}
		break;
	case WM_MOUSEMOVE:
		OnMouseMove();
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}
//Property
void TModalWnd::BecomeModal()
{
	m_bdomodal = true;
	ShowWindow(m_Hwnd, SW_SHOWNA);
	BringWindowToTop(m_Hwnd);

	HWND hParentWnd = m_parent;
	while (hParentWnd != NULL)
	{
		EnableWindow(hParentWnd, FALSE);
		hParentWnd = ::GetParent(hParentWnd);
	}
}
void TModalWnd::EndModal()
{
	m_bdomodal = false;
	HWND hParentWnd = m_parent;
	while (hParentWnd != NULL)
	{
		EnableWindow(hParentWnd, TRUE);
		hParentWnd = ::GetParent(hParentWnd);
	}
	BringWindowToTop(hParentWnd);
	SetForegroundWindow(hParentWnd);
	SetActiveWindow(hParentWnd);
}

BOOL TModalWnd::DestroyWindow(HWND hWnd)
{
	EndModal();
	return ::DestroyWindow(hWnd);
}