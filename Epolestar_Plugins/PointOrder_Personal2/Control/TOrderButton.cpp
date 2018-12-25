#include "PreInclude.h"

TOrderButton::TOrderButton()
{
	m_bMouseTrack = false; 
	m_bkColor = RGB(215, 227, 241);
	m_borderColor = RGB(114, 166, 217);
	m_textColor = RGB(255, 0, 0);
	m_focusBorderColor = RGB(240, 0, 0);
	m_bMouseClicked = false;
	m_bHasFocus = false;
	strcpy_s(m_strPrice, "0.0");
	strcpy_s(m_strAction, "¿ª¶à");
}
void TOrderButton::Create(HWND hParent, int id)
{
	CreateFrm(L"TOrderButton", hParent, WS_VISIBLE | WS_CHILD  | WS_CHILDWINDOW);
	SetWindowLongPtr(m_Hwnd, GWL_ID, id);
}
void TOrderButton::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER | SWP_SHOWWINDOW);
}
void TOrderButton::SetPrice(const char* pszPrice)
{
	strcpy_s(m_strPrice, pszPrice);
	InvalidateRect(m_Hwnd, 0, false);
}
void TOrderButton::SetActionText(const char* pszText)
{
	strcpy_s(m_strAction, pszText);
	InvalidateRect(m_Hwnd, 0, false);
}
void TOrderButton::SetColor(COLORREF bkColor, COLORREF textColor, COLORREF borderColor, COLORREF focusBorderColor)
{
	m_bkColor = bkColor;
	m_textColor = textColor;
	m_borderColor = borderColor;
	m_focusBorderColor = focusBorderColor;
}
LRESULT TOrderButton::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;		
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_SETFOCUS:
		OnSetFocus(); 
		return PROCESSED;
	case WM_KILLFOCUS:
		OnKillFocus();
		return PROCESSED;
	case WM_LBUTTONUP:
		OnLButtonUp();
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
void TOrderButton::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam == MK_LBUTTON)
	{
		m_bMouseClicked = true;
		SetFocus(m_Hwnd);
		InvalidateRect(m_Hwnd, 0, false);
	}
}
void TOrderButton::OnLButtonUp()
{
	if (m_bMouseClicked)
		SendMessage(GetParent(), SSWM_ORDERBUTTON_UP, (WPARAM)m_Hwnd, (LPARAM)GetWindowLongPtr(m_Hwnd, GWL_ID));
	m_bMouseClicked = false;
	InvalidateRect(m_Hwnd, 0, false);
}
void TOrderButton::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	GetClientRect(m_Hwnd, &rect);
	HBRUSH hbrBk = CreateSolidBrush(m_bkColor);
	FillRect(memdc.GetHdc(), &rect, hbrBk);
	COLORREF borderColor = m_borderColor;
	if (m_bMouseTrack)
		borderColor = m_focusBorderColor;
	HBRUSH hbrBorder = CreateSolidBrush(borderColor);
	FrameRect(memdc.GetHdc(), &rect, hbrBorder);
	
	if (m_bHasFocus)
	{
		InflateRect(&rect, -2, -2);
		DrawFocusRect(memdc.GetHdc(), &rect);
		InflateRect(&rect, 2, 2);
	}
	RECT rcText = { 0, 0, rect.right, 25 };
	int iLineTop = 22;
	if (m_bMouseClicked)
	{
		rcText.top = 2;
		rcText.bottom += 2;
		iLineTop += 2;
	}
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(148, 150, 139));
	HPEN oldPen = (HPEN)SelectObject(memdc.GetHdc(), pen);
	MoveToEx(memdc.GetHdc(), 5, iLineTop, 0);
	LineTo(memdc.GetHdc(), rect.right - 5, iLineTop);
	SelectObject(memdc.GetHdc(), oldPen);
	SetTextColor(memdc.GetHdc(), m_textColor);
	SelectObject(memdc.GetHdc(), g_FontData.g_FontNum17);
	DrawTextA(memdc.GetHdc(), m_strPrice, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	
	rcText.left = 0;
	rcText.top = iLineTop;
	rcText.bottom = rect.bottom;
	rcText.right = rect.right;
	if (m_bMouseClicked)
	{
		rcText.top += 2;
		rcText.bottom += 2;
	}
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord18_2);
	DrawTextA(memdc.GetHdc(), m_strAction, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(pen);
	DeleteObject(hbrBk);
	DeleteObject(hbrBorder);
}
void TOrderButton::OnSetFocus()
{
	m_bHasFocus = true;
	InvalidateRect(m_Hwnd, 0, false);
}
void TOrderButton::OnKillFocus()
{
	m_bHasFocus = false;
	InvalidateRect(m_Hwnd, 0, false);
}
void TOrderButton::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMouseTrack = true;
		InvalidateRect(m_Hwnd, 0, false);
	}
}
void TOrderButton::OnMouseLeave()
{
	m_bMouseTrack = false;
	InvalidateRect(m_Hwnd, 0, false);
}