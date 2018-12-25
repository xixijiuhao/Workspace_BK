#include "..\PreInclude.h"

extern G_UIFONT g_FontData;

CxDrawButton::CxDrawButton()
{
}
CxDrawButton::~CxDrawButton()
{
}

void CxDrawButton::CreateWin(LPCTSTR classname, HWND parent, DWORD style, int nleft, int ntop, int nWidth, int nHeight, const wchar_t *cText,  bool btriangle, COLORREF color, DWORD exstyle)
{
	m_sText = cText;
	RECT rect;
	rect.left = nleft;
	rect.top = ntop;
	rect.right = nleft + nWidth;
	rect.bottom = ntop + nHeight;
	CreatePopWin(classname, parent, style, rect, btriangle, color);
}
void CxDrawButton::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	CxSubButton::OnLButtonDown(wParam, lParam);
}
void CxDrawButton::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	CxSubButton::OnLButtonUp(wParam, lParam);
	if (GetClickCheck())
	{
		int id = GetWindowLong(m_Hwnd, GWL_ID);
		SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(id, BN_CLICKED), 0);
	}
}

void CxDrawButton::OnDrawItemText(HDC &hdc, RECT rect)
{
	SelectObject(hdc, m_font);
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, m_sText.c_str(), m_sText.size(), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
}

void CxDrawButton::SetText(const wchar_t *sText)
{
	m_sText = sText;
	InvalidateRect(m_Hwnd, NULL, TRUE);
}

wstring CxDrawButton::GetText()
{
	return m_sText;
}

void CxDrawButton::OnDestory(WPARAM wParam, LPARAM lParam)
{
	CxSubButton::OnDestory(wParam, lParam);
}