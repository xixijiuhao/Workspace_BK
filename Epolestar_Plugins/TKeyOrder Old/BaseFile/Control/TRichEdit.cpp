#include "PreInclude.h"

//RichEdit�򵥷�װ
void TRichEdit::Create(HWND hParent, UINT style)
{
	m_hRichEdit = CreateWindowEx(0, MSFTEDIT_CLASS, 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL /*| WS_TABSTOP*/ | style, \
		0, 0, 0, 0, hParent, 0, GetModuleHandle(NULL), 0);
    SendMessage(m_hRichEdit, EM_SETBKGNDCOLOR, 0, g_ColorRefData.g_ColorBackground);
}
void TRichEdit::SetFont(HFONT font)
{
	SendMessage(m_hRichEdit, WM_SETFONT, (WPARAM)font, 0);
}
void TRichEdit::AddString(char* pStr, COLORREF color)
{
	//����������ɫ
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = color;
	SendMessage(m_hRichEdit, EM_SETSEL, -1, -1);
	SendMessage(m_hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	//�������
	TCHAR szText[255] = { 0 };
	lstrcpy(szText,LoadRC::ansi_to_unicode(pStr).c_str());
	int length = lstrlen(szText);
	szText[length] = '\r';
	szText[length + 1] = '\n';
	szText[length + 2] = '\0';
	SendMessage(m_hRichEdit, EM_REPLACESEL, TRUE, (LPARAM)szText);
	SendMessage(m_hRichEdit, WM_VSCROLL, SB_BOTTOM, 0);

}
//���
void TRichEdit::Clear()
{
	HWND hwnd = GetFocus();
	SetFocus(m_hRichEdit);
	SendMessage(m_hRichEdit, EM_SETSEL, 0, -1);
	SendMessage(m_hRichEdit, WM_CLEAR, 0, 0);
	SetFocus(hwnd);
}
void TRichEdit::Show(UINT show)
{
	ShowWindow(m_hRichEdit, show);
}
HWND TRichEdit::GetHwnd()
{
	return m_hRichEdit;
}
void TRichEdit::MoveWindow(int left, int top, int width, int height)
{
	SetWindowPos(m_hRichEdit, 0, left, top, width, height, SWP_NOOWNERZORDER);
	InvalidateRect(m_hRichEdit, 0, true);
}