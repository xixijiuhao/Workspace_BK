#pragma once

class CxKeyBoardCheck :public CxSubClassWnd
{
public:
	CxKeyBoardCheck();
	~CxKeyBoardCheck();
	void InitBtn(HWND hwnd);
	int  GetCheck() { return m_bChecked ? BST_CHECKED : BST_UNCHECKED; }
	void SetCheck(int check) { m_bChecked = (check == BST_CHECKED) ? true : false; InvalidateRect(GetHwnd(), NULL, TRUE); }
	void SetColor(COLORREF color){ m_color = color; InvalidateRect(GetHwnd(), NULL, TRUE); }
	COLORREF GetColor(){ return m_color; };
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	bool m_bChecked;
	COLORREF m_color;
private:
	const int m_selchecksize = 13;
protected:
	//处理消息
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
};

