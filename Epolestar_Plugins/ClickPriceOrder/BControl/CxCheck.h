#pragma once
#include "CxSubClassWnd.h"


class CxCheck :public CxSubClassWnd
{
public:
	CxCheck();
	~CxCheck();
	void Create(HWND hParent, UINT id,  UINT ExStyle = 0);
	void SetPosition(int x, int y, int cx, int cy);
	void Init(HWND hwnd);
	void SetFont(HFONT hfont){ m_hfont = hfont; }
	int  GetCheck() { return m_bChecked ? BST_CHECKED : BST_UNCHECKED; }
	void SetCheck(int check) { m_bChecked = (check == BST_CHECKED) ? true : false; InvalidateRect(GetHwnd(), NULL, TRUE); }
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	bool m_bChecked;
	bool m_bInRect;
	HFONT m_hfont;
private:
	const int m_selchecksize = 13;
protected:
	//处理消息
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
};

