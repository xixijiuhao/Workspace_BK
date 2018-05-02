#pragma once


class CxCheck :public CxSubClassWnd
{
public:
	CxCheck();
	~CxCheck();
	void Create(HWND hParent, UINT id, TCHAR *szText = NULL,  UINT style = 0);
	void SetFont(HFONT hfont){ m_hfont = hfont; }
	int  GetCheck() { return m_bChecked ? BST_CHECKED : BST_UNCHECKED; }
	void SetCheck(int check) { m_bChecked = (check == BST_CHECKED) ? true : false; InvalidateRect(GetHwnd(), NULL, TRUE); }
private:
	void Init(HWND hwnd);
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
	//������Ϣ
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
};

