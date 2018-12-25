#pragma once
#include "CxSubClassWnd.h"

class CxRichEdit
{
public:
	void Create(HWND hParent, UINT style = 0, UINT specialStyle = 0, UINT id = 0);
	void SetFont(HFONT font);
	void MoveWindow(int x, int y, int cx, int cy);
	void Clear();
	void Show(UINT show);
	HWND GetHwnd();
	//void AddString(const char* pstr, int length = 512, COLORREF color = RGB(0, 0, 0));
	void AddString(string str, COLORREF color = RGB(0, 0, 0));
	void AddString(wstring wstr, COLORREF color = RGB(0, 0, 0));
	void AddLineString(char * pStr, int strMaxLength, COLORREF color = RGB(0, 0, 0));
	wstring GetAllText();
private:
	HWND m_hRichEdit;
	HFONT m_font;
};