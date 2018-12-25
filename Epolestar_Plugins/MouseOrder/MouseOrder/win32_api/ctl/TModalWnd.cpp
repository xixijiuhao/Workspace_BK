#include "..\PreInclude.h"

extern	IMainFrame			* g_mainframe;
//色彩
extern	G_COLOREF			g_ColorRefData;
extern	G_UIFONT			g_FontData;
#define WM_DRAW				WM_USER+WM_PAINT
#define CAPTION_HEIGHT		25
#define WND_WIDTH			350
#define WND_HEIGHT			150
TModalWnd::TModalWnd(HWND hparent)
	:m_nWidth(WND_WIDTH),
	m_nHeight(WND_HEIGHT)
{
	m_parent = hparent;
	m_sztitle = LoadRC::w_load_str(IDS_ExtraInfo);
	m_szok = LoadRC::w_load_str(IDS_STRING1101);
	m_szcancel = LoadRC::w_load_str(IDS_STRING1103);
	m_bhoverclose = false;
	m_bhoverbtnok = false;
	m_bhoverbtncancel = false;
	m_btrack = false;
	m_nret = 1;
}
TModalWnd::~TModalWnd()
{

}
//
bool TModalWnd::Domodal()
{
	CreateFrm(L"TModalWnd", m_parent, WS_POPUP /*| WS_VISIBLE */ | WS_CLIPCHILDREN);
	SetClassLongPtr(m_Hwnd, GCLP_HBRBACKGROUND, (LONG)GetClassLongPtr(m_parent, GCLP_HBRBACKGROUND));
	HDC			hdc;
	hdc = GetDC(m_Hwnd);
	m_hdctemp = CreateCompatibleDC(hdc);
	ReleaseDC(m_Hwnd, hdc);
	CreateSubCtrl();

	BecomeModal();
	SetModalRect();
	//消息循环
	MSG msg;
	while (m_bdomodal&&GetMessage(&msg, NULL, 0, 0))
	{
		if ((msg.message == WM_KEYDOWN&&msg.wParam == VK_ESCAPE)
			|| (msg.message == WM_SYSCOMMAND&&msg.wParam == SC_CLOSE&&!msg.lParam))
		{
			m_nret = 0;
			DestroyWindow(m_Hwnd);
			continue;
		}
		if (msg.message == WM_PAINT || msg.message > WM_USER || IsWndMessage(m_Hwnd, msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	bool bret =false; if (m_nret) bret = true;
	return bret;
}
bool TModalWnd::IsWndMessage(HWND Hwnd, MSG msg)
{
	if (Hwnd == msg.hwnd)
		return true;
	HWND hparent = ::GetParent(msg.hwnd);
	if (Hwnd == hparent)
		return true;
	/*HWND hparent1 = ::GetParent(hparent);
	if (Hwnd == hparent1)
		return true;
	HWND hparent2 = ::GetParent(hparent1);
	if (Hwnd == hparent1)
		return true;*/
	else
		return false;
}
void TModalWnd::CreateSubCtrl()
{
	m_hEdit = CreateWindowEx(0, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS | ES_AUTOHSCROLL,75, 50, 200,25, m_Hwnd, (HMENU)1, GetModuleHandleW(NULL), NULL);	
	SendMessage(m_hEdit, WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx17(), (LPARAM)TRUE);
	SendMessage(m_hEdit, EM_SETLIMITTEXT,20,0);
}
void TModalWnd::GetRemarkInfo(char(&sRemark)[51])
{
	memcpy(sRemark, m_sRemark, sizeof(m_sRemark));
}
void TModalWnd::SetModalRect()
{
	long scrWidth, scrHeight;
	scrWidth = GetSystemMetrics(SM_CXSCREEN);
	scrHeight = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(m_Hwnd, (scrWidth - WND_WIDTH) / 2, (scrHeight - WND_HEIGHT) / 2, WND_WIDTH, WND_HEIGHT, true);
	//SetWindowPos(m_Hwnd, HWND_TOPMOST, (scrWidth - WND_WIDTH) / 2, (scrHeight - WND_HEIGHT) / 2, WND_WIDTH, WND_HEIGHT,SWP_SHOWWINDOW);
}
//MSG
void TModalWnd::OnPaint()
{
	HBRUSH hWhiteBrush = CreateSolidBrush(g_ColorRefData.GetColorWhite());
	HBRUSH hGrayBrush = CreateSolidBrush(g_ColorRefData.GetColorGrayTitle());
	HDC			hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(m_Hwnd, &ps);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	SelectObject(m_hdctemp, g_FontData.GetFontNum15());
	SetBkMode(m_hdctemp, TRANSPARENT);

	HBRUSH hbr;

	FillRect(m_hdctemp, &rect, hWhiteBrush);

	hbr = CreateSolidBrush(g_ColorRefData.GetColorRGB200());
	FrameRect(m_hdctemp, &rect, hbr);

	if(m_bhoverbtnok)
		FrameRect(m_hdctemp, &m_rcBtnOk, hGrayBrush);
	else
		FrameRect(m_hdctemp, &m_rcBtnOk, hbr);

	if(m_bhoverbtncancel)
		FrameRect(m_hdctemp, &m_rcBtnCancel, hGrayBrush);
	else
		FrameRect(m_hdctemp, &m_rcBtnCancel, hbr);

	DeleteObject(hbr);
	
	FillRect(m_hdctemp, &m_rctitle, hGrayBrush);

	m_rcclose = rect;
	m_rcclose.top = CAPTION_HEIGHT / 4;
	m_rcclose.bottom = m_rcclose.top + CAPTION_HEIGHT / 2;
	m_rcclose.left = m_rcclose.right - CAPTION_HEIGHT * 3/ 4;
	m_rcclose.right = m_rcclose.left + CAPTION_HEIGHT / 2;

	HPEN hpen;
	if (m_bhoverclose)
		hpen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorWhite());
	else
		hpen = CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
	SelectObject(m_hdctemp, hpen);

	MoveToEx(m_hdctemp, m_rcclose.left, m_rcclose.top, 0);
	LineTo(m_hdctemp, m_rcclose.right + 1, m_rcclose.bottom + 1);
	MoveToEx(m_hdctemp, m_rcclose.left, m_rcclose.bottom, 0);
	LineTo(m_hdctemp, m_rcclose.right + 1, m_rcclose.top - 1);								//关闭按钮
	InflateRect(&m_rcclose, 1, 1);
	DeleteObject(hpen);

	
	SetTextColor(m_hdctemp, g_ColorRefData.GetColorWhite());
	DrawText(m_hdctemp, m_sztitle.c_str(), m_sztitle.length(), &m_rctext, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	SetTextColor(m_hdctemp, g_ColorRefData.GetColorGray());
	
	DrawText(m_hdctemp, m_szok.c_str(), m_szok.length(), &m_rcBtnOk, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	DrawText(m_hdctemp, m_szcancel.c_str(), m_szcancel.length(), &m_rcBtnCancel, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_hdctemp, 0, 0, SRCCOPY);

	EndPaint(m_Hwnd, &ps);
	DeleteObject(hWhiteBrush);
	DeleteObject(hGrayBrush);
}

void TModalWnd::OnSize()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_rctitle = rect; 
	m_rctitle.bottom = CAPTION_HEIGHT;
	m_rctext = m_rctitle;
	m_rctext.top = m_rctitle.top + 5;
	m_rctext.bottom = m_rctitle.bottom - 5;
	m_rcBtnOk.left = 85;
	m_rcBtnOk.right = 165;
	m_rcBtnOk.top = 100;
	m_rcBtnOk.bottom = 125;
	m_rcBtnCancel.left = 185;
	m_rcBtnCancel.right = 265;
	m_rcBtnCancel.top = 100;
	m_rcBtnCancel.bottom = 125;

	HDC			hdc;
	hdc = GetDC(m_Hwnd);
	
	m_nWidth = rect.right - rect.left;
	m_nHeight = rect.bottom - rect.top;//获取窗口大小客户区

	if (m_hbmptemp)
		DeleteObject(m_hbmptemp);

	m_hbmptemp = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
	SelectObject(m_hdctemp, m_hbmptemp);

	ReleaseDC(m_Hwnd, hdc);
}

void TModalWnd::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	if (MK_LBUTTON == wParam&& PtInRect(&m_rctitle, pt) && !PtInRect(&m_rcclose, pt))
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam/*MAKELPARAM(point.x, point.y)*/);
	}
}

void TModalWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

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
		m_bhoverclose = true;
	else
		m_bhoverclose = false;

	if (PtInRect(&m_rcBtnOk, pt))
		m_bhoverbtnok = true;
	else
		m_bhoverbtnok = false;

	if (PtInRect(&m_rcBtnCancel, pt))
		m_bhoverbtncancel = true;
	else
		m_bhoverbtncancel = false;

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
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		if (PtInRect(&m_rcclose, pt)||PtInRect(&m_rcBtnCancel,pt))
		{
			m_nret = 0;
			DestroyWindow(m_Hwnd);
		}
		else if (PtInRect(&m_rcBtnOk, pt))
		{
			GetWindowTextA(m_hEdit, m_sRemark,50);
			DestroyWindow(m_Hwnd);
		}
	}
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		break;
	case WM_DESTROY:
		DeleteDC(m_hdctemp);
		DeleteObject(m_hbmptemp);
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
		//EnableWindow(hParentWnd, FALSE);
		hParentWnd = ::GetParent(hParentWnd);
	}
	if (g_mainframe)
		g_mainframe->DisableFrames();
}
void TModalWnd::EndModal()
{
	m_bdomodal = false;
	HWND hParentWnd = m_parent;
	while (hParentWnd != NULL)
	{
		//EnableWindow(hParentWnd, TRUE);
		hParentWnd = ::GetParent(hParentWnd);
	}
	if (g_mainframe)
		g_mainframe->EnableFrames();
	BringWindowToTop(hParentWnd);
	SetForegroundWindow(hParentWnd);
	SetActiveWindow(hParentWnd);
}

BOOL TModalWnd::DestroyWindow(HWND hWnd)
{
	EndModal();
	return ::DestroyWindow(hWnd);
}