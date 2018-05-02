#ifndef _T_RICH_EDIT_H
#define _T_RICH_EDIT_H

class TRichEdit
{
public:
	void Create(HWND hParent, UINT style = 0);
	void AddString(char* pStr, COLORREF color);
	void Clear();
	void Show(UINT show);
	HWND GetHwnd();
	void MoveWindow(int left, int top, int width, int height);
	void SetFont(HFONT font);
private:
	HWND m_hRichEdit;
};

#endif