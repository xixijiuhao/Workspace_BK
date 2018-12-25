#include "BaseInclude.h"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;

TStatusBar::TStatusBar()
{
	m_textColor = RGB(255, 0, 0);
}
TStatusBar::~TStatusBar()
{
	 
}
void TStatusBar::UserLogin(bool bLogin)
{
	if (bLogin)
		m_textColor = RGB(0, 220, 0);
	else
		m_textColor = RGB(255, 0, 0);
	InvalidateRect(m_Hwnd, 0, false);
}
void TStatusBar::ShowFrm()
{
	CreateFrm(L"T3StatusBar", NULL, WS_VISIBLE | WS_POPUP, WS_EX_TOOLWINDOW);
	SetWindowPos(m_Hwnd, NULL, 0, 0, 80, 0, SWP_NOZORDER | SWP_SHOWWINDOW);

	g_pMainFrm->AddLinkage(m_Hwnd, 0, TStatusBar::OnReceiveNotice);
}
void TStatusBar::OnReceiveNotice(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{

}
LRESULT TStatusBar::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLBtnDown(wParam,lParam);
		return PROCESSED;
	case WM_NCDESTROY:
		delete this;
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
void TStatusBar::OnLBtnDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect = { 0 };
	GetClientRect(m_Hwnd, &rect);
	rect.top = 1;
	rect.left = 3;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (!PtInRect(&rect, pt)) return;

	TPresenter::Show();
}
void TStatusBar::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	HBRUSH hbr = CreateSolidBrush(RGB(30, 30, 30));
	RECT rect = { 0 };
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, hbr);
	DeleteObject(hbr);
	rect.top = 1;
	rect.bottom -= 1;
	hbr = CreateSolidBrush(RGB(60, 60, 60));
	rect.left = 3;
	FillRect(memdc.GetHdc(), &rect, hbr);
	DeleteObject(hbr);
	SelectObject(memdc.GetHdc(), g_FontData.g_FontBoldWord13);
	SetTextColor(memdc.GetHdc(), m_textColor);
	rect.top += 1;

	if (g_language==CHT)
		DrawText(memdc.GetHdc(), L"通用下單", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	else if (g_language==ENU)
		DrawText(memdc.GetHdc(), L"Usual Order", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	else
		DrawText(memdc.GetHdc(), L"通用下单", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}