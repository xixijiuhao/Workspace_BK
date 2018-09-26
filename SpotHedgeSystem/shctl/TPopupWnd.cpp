#include "Preinclude.h"
extern G_COLOREF			g_ColorRefData;				//É«²Ê
TPopupWnd::TPopupWnd()
{
	m_bauto = true;
}

TPopupWnd::~TPopupWnd()
{
}

bool TPopupWnd::Create(HWND hparent)
{
	m_parent = hparent;
	CreateFrm(_T("TPopupWnd"), hparent, WS_POPUP /*| WS_VISIBLE */| WS_CLIPCHILDREN);
	return true;
}

//action
void TPopupWnd::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, 0, true);
}
void TPopupWnd::ShowWindow(int nCmdShow)
{
	::ShowWindow(m_Hwnd, nCmdShow);
}
//MSG
LRESULT TPopupWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_ACTIVATE:
		OnActivate(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void TPopupWnd::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.GetHbrWhite());
	FrameRect(memdc.GetHdc(), &rect, g_ColorRefData.GetHbrGray());

	DrawMain(&memdc);
}

void TPopupWnd::DrawMain(TMemDC* pmemdc)
{

}
void TPopupWnd::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if (!PtInRect(&rect, pt) && m_bauto)
		ShowWindow(SW_HIDE);

}

void TPopupWnd::OnActivate(WPARAM wParam, LPARAM lParam)
{
	int nactive = LOWORD(wParam);   // activation flag 
	if (WA_INACTIVE == nactive&&m_bauto)
		ShowWindow(SW_HIDE);
}