#pragma once


class CxRichEdit
{
public:
	void Create(HWND hParent, UINT style = 0, UINT specialStyle = 0, UINT id = 0);
	void SetFont(HFONT font);
	void MoveWindow(int x, int y, int cx, int cy);
	void Clear();
	void Show(UINT show);
	HWND GetHwnd();
	void AddString(char* pstr, int length, COLORREF color);
private:
	HWND m_hRichEdit;
	HFONT m_font;
};