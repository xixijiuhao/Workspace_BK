#include "PreInclude.h"

void TStatic::Create(HWND hParent, const TCHAR* pszText, int id, UINT style)
{
	style |= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTERIMAGE | SS_NOTIFY;
	m_hStatic = CreateWindowEx(0, WC_STATIC, pszText, style, 0, 0, 0, 0,
		hParent, HMENU(id), GetModuleHandle(NULL), 0);
	SetFont(g_FontData.g_FontWord12);

}
void TStatic::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_hStatic, 0, x, y, cx, cy, SWP_NOZORDER);
}
void TStatic::SetText(const TCHAR *pszText)
{
	SendMessage(m_hStatic, WM_SETTEXT, 0, (LPARAM)pszText);
}
void TStatic::SetText(const char *pszText)
{
	SetWindowTextA(m_hStatic, pszText);
}
void TStatic::SetFont(HFONT hfont)
{
	SendMessage(m_hStatic, WM_SETFONT, (WPARAM)hfont, TRUE);
}