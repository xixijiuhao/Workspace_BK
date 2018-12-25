#pragma once

class CxSubClassWnd
{
public:
	CxSubClassWnd();
	~CxSubClassWnd();
	void MoveWindow(int x, int y, int cx, int cy);
	void SetFont(HFONT font);

public:
	BOOL Init(HWND hWnd);
	HWND GetHwnd()	{ return m_hWnd; }
	BOOL CxGetWindowText(wstring &w);
protected:
	static LRESULT WINAPI stdProc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam);
	virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
protected:
	HWND m_hParenthWnd;
	HWND m_hWnd;//��ǰ��BUTTON���
	LONG m_OldProc;//ԭ���Ļص�������ַ
};

