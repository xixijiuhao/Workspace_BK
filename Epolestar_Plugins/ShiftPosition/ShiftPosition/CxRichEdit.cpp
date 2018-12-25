#include "BaseInclude.h"

void CxRichEdit::Create(HWND hParent, UINT style, UINT specialStyle, UINT id)
{
	m_hRichEdit = CreateWindowEx(0, MSFTEDIT_CLASS, 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL /*| ES_READONLY*/ /*| WS_TABSTOP*/ | style, \
		0,0 , 0, 0, hParent, (HMENU)(id), GetModuleHandle(NULL), 0);
	SendMessage(m_hRichEdit, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
}
void CxRichEdit::SetFont(HFONT font)
{
	m_font = font;
	SendMessage(m_hRichEdit, WM_SETFONT, WPARAM(m_font), 0);
}
void CxRichEdit::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_hRichEdit, 0, x, y, cx, cy, SWP_NOZORDER);
}
void CxRichEdit::Clear()
{
	SendMessage(m_hRichEdit, EM_SETSEL, 0, -1);
	SendMessage(m_hRichEdit, WM_CLEAR, 0, 0);
}
void CxRichEdit::Show(UINT show)
{
	ShowWindow(m_hRichEdit, show);
}
HWND CxRichEdit::GetHwnd()
{
	return m_hRichEdit;
}
void CxRichEdit::AddString(char* pStr, int strMaxLength, COLORREF color)
{
	//设置字体颜色
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR  ;
	//cf.yHeight = 14 * 14;
	//lstrcpy(cf.szFaceName, _T("宋体"));
	cf.crTextColor = color;
	//SendMessage(m_hRichEdit, WM_SETFONT, WPARAM(m_font), 0);
	SendMessageA(m_hRichEdit, EM_SETSEL, -1, -1);
	SendMessage(m_hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	//设置完毕
	int length = strlen(pStr);
	if (length + 2 > strMaxLength)
		return;
	pStr[length] = '\r';
	pStr[length + 1] = '\n';
	pStr[length + 2] = '\0';
	SendMessageA(m_hRichEdit, EM_REPLACESEL, TRUE, (LPARAM)pStr);
	SendMessage(m_hRichEdit, WM_VSCROLL, SB_BOTTOM, 0);
}