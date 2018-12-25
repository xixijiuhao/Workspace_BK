#pragma once

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
	bool m_bChecked;
	bool m_bInRect;

	COLORREF m_color;		//�ı���ɫ
	COLORREF m_colorBK;		//����ɫ
private:
	const int m_selchecksize = 13;	//��ѡ����
protected:
	//������Ϣ
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OnPaint();

	void OnLButtonDown(WPARAM wParam, LPARAM lParam);

	void OnLButtonDbClick(WPARAM wParam, LPARAM lParam);

	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void Resize();
};

