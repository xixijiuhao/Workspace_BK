#ifndef _CX_DRAW_BUTTON_
#define _CX_DRAW_BUTTON_

#include "CxSubButton.h"

class CxDrawButton:public CxSubButton
{
public:
	CxDrawButton();
	~CxDrawButton();

	void CreateWin(LPCTSTR classname, HWND parent, DWORD style, int nleft, int ntop, int nWidth, int nHeight, const wchar_t *cText, bool btriangle = false, COLORREF color = RGB(150,150,150), DWORD exstyle = 0);	//创建弹出窗口

	void OnLButtonDown(WPARAM wParam, LPARAM lParam);

	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void OnDrawItemText(HDC &hdc, RECT rect);

	void OnDestory(WPARAM wParam, LPARAM lParam);

	void SetText(const wchar_t *sText);

	void SetFont(HFONT hfont) { m_font = hfont; };

	wstring GetText();

private:
	wstring  m_sText;

	RECT m_WinRect;

	HFONT m_font;
};

#endif