#pragma once
#include "CxSubClassWnd.h"

class CxCheck :public CxSubClassWnd
{
public:
	CxCheck();
	~CxCheck();
	void Init(HWND hwnd,COLORREF color = RGB(255,255,255)); 
	int  GetCheck() { return m_bChecked ? BST_CHECKED : BST_UNCHECKED; }
	void SetCheck(int check);
	void SetColor(COLORREF color){ m_color = color; InvalidateRect(GetHwnd(), NULL, TRUE); }
	COLORREF GetColor(){ return m_color; };
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	bool m_bChecked;
	bool m_bInRect;

	COLORREF m_color;
	COLORREF m_colorBK;
private:
	const int m_selchecksize = 13;
protected:
	//处理消息
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
};

