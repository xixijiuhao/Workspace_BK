#include "PreInclude.h"
#include <Richedit.h>

void CxRichEdit::Create(HWND hParent, UINT style, UINT specialStyle, UINT id)
{
	m_hRichEdit = CreateWindowEx(0, MSFTEDIT_CLASS, 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL| WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL /*| ES_READONLY*/ /*| WS_TABSTOP*/ | style, \
		0,0 , 0, 0, hParent, (HMENU)(id), GetModuleHandle(NULL), 0);
	SendMessage(m_hRichEdit, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
	//SendMessage(m_hRichEdit, EM_SHOWSCROLLBAR, SB_VERT, TRUE);
	//SendMessage(m_hRichEdit, EM_SHOWSCROLLBAR, SB_HORZ, TRUE);
	//SendMessage(m_hRichEdit, EM_SHOWSCROLLBAR, SB_BOTH, TRUE);
	//SendMessage(m_hRichEdit, EM_SETEVENTMASK, 0, ENM_SELCHANGE | ENM_CHANGE | ENM_SCROLL);
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
	SetWindowText(m_hRichEdit, L"");
}
void CxRichEdit::Show(UINT show)
{
	ShowWindow(m_hRichEdit, show);
}
HWND CxRichEdit::GetHwnd()
{
	return m_hRichEdit;
}
//void CxRichEdit::AddString(const char* pStr, int strMaxLength, COLORREF color)
//{
//	//设置字体颜色
//	CHARFORMAT2 cf;
//	memset(&cf, 0, sizeof(CHARFORMAT2));
//	cf.cbSize = sizeof(CHARFORMAT2);
//	cf.dwMask = CFM_COLOR  ;
//	//cf.yHeight = 14 * 14;
//	//lstrcpy(cf.szFaceName, _T("宋体"));
//	cf.crTextColor = color;
//	//SendMessage(m_hRichEdit, WM_SETFONT, WPARAM(m_font), 0);
//	SendMessageA(m_hRichEdit, EM_SETSEL, -1, -1);
//	SendMessage(m_hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
//	//设置完毕
//	int length = strlen(pStr);
//	string temp = pStr;
//	if (length + 3 > strMaxLength)
//		return;
//	//pStr[length] = '\r';
//	//pStr[length + 1] = '\n';
//	//pStr[length + 2] = '\0';
//	temp[length] = '\r';
//	temp[length + 1] = '\n';
//	temp[length + 2] = '\0';
//
//	SendMessageA(m_hRichEdit, EM_REPLACESEL, TRUE, (LPARAM)pStr);
//	SendMessage(m_hRichEdit, WM_VSCROLL, SB_BOTTOM, 0);
//}

void CxRichEdit::AddString(string str, COLORREF color)
{
	//设置字体颜色
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR | CFM_CHARSET| CFM_FACE | CFM_KERNING | CFM_SPACING/* |CFM_SIZE*/ | CFM_OFFSET;
	//cf.yHeight = 16 * 16;
	cf.crTextColor = color;
	cf.bCharSet = ANSI_CHARSET;
	lstrcpy(cf.szFaceName, _T("黑体"));
	cf.wKerning = 0;
	cf.sSpacing = 0;
	cf.yOffset = 0;
	SendMessage(m_hRichEdit, WM_SETFONT, WPARAM(m_font), 0);
	SendMessageA(m_hRichEdit, EM_SETSEL, -1, -1);
	SendMessage(m_hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

	::SendMessage(m_hRichEdit, EM_SETLANGOPTIONS, 0, 0);

	//设置行间距
	PARAFORMAT2 pf2;
	memset(&pf2, 0, sizeof(PARAFORMAT2));
	pf2.cbSize = sizeof(PARAFORMAT2);
	pf2.dwMask = PFM_LINESPACING;
	pf2.bLineSpacingRule = 4; 
	pf2.dyLineSpacing = 210; //bLineSpacingRule 设为3、4、5，dyLineSpacing才有效 
	SendMessage(m_hRichEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf2);

	//设置完毕
	str.append("\n");

	SendMessageA(m_hRichEdit, EM_REPLACESEL, TRUE, (LPARAM)str.c_str());
	SendMessage(m_hRichEdit, WM_VSCROLL, SB_BOTTOM, 0);
}

void CxRichEdit::AddString(wstring wstr, COLORREF color)
{
	//设置字体颜色
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR | CFM_CHARSET | CFM_FACE;
	cf.yHeight = 10 * 10;
	lstrcpy(cf.szFaceName, _T("黑体"));
	cf.crTextColor = color;
	//SendMessage(m_hRichEdit, WM_SETFONT, WPARAM(m_font), 0);
	SendMessageA(m_hRichEdit, EM_SETSEL, -1, -1);
	SendMessage(m_hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	//设置完毕
	wstr.append(L"\n");

	SendMessage(m_hRichEdit, EM_REPLACESEL, TRUE, (LPARAM)wstr.c_str());
	SendMessage(m_hRichEdit, WM_VSCROLL, SB_BOTTOM, 0);
}

void CxRichEdit::AddLineString(char* pStr, int strMaxLength, COLORREF color)
{
	//设置字体颜色
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR;
	cf.yHeight = 14 * 14;
	//lstrcpy(cf.szFaceName, _T("宋体"));
	cf.crTextColor = color;
	//SendMessage(m_hRichEdit, WM_SETFONT, WPARAM(m_font), 0);
	SendMessageA(m_hRichEdit, EM_SETSEL, -1, -1);
	SendMessage(m_hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	//设置完毕
	//int length = strlen(pStr);
	//if (length + 2 > strMaxLength)
	//	return;
	//pStr[length] = '\r';
	//pStr[length + 1] = '\n';
	//pStr[length + 2] = '\0';
	SendMessageA(m_hRichEdit, EM_REPLACESEL, TRUE, (LPARAM)pStr);
	SendMessage(m_hRichEdit, WM_VSCROLL, SB_BOTTOM, 0);
}

wstring CxRichEdit::GetAllText()
{
	wstring txt(L"");
	CHARRANGE cr = { 0 };
	//cr.cpMin = 0;
	//cr.cpMax = -1;

	SendMessageA(m_hRichEdit, EM_SETSEL, 0, -1);
	SendMessageA(m_hRichEdit, EM_EXGETSEL, 0, (LPARAM)&cr);

	if (cr.cpMax <= 0)
		return txt;
	wchar_t* p = new wchar_t[cr.cpMax+1];
	memset(p, 0, sizeof(*p));

	SendMessage(m_hRichEdit, EM_GETSELTEXT, 0, (LPARAM)p);
	txt = p;

	delete[]p;
	return txt;
}
