#pragma once

class CxSubClassWnd
{
public:
	CxSubClassWnd();
	~CxSubClassWnd();
public:
	BOOL Init(HWND hWnd);
	HWND GetHwnd()	{ return m_hWnd; }
	int CxGetWindowText(wchar_t *w,unsigned int nLen);
protected:
	static LRESULT WINAPI stdProc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam);
	virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
protected:
	HWND m_hParenthWnd;
	HWND m_hWnd;//当前的BUTTON句柄
	LONG m_OldProc;//原来的回调函数地址
};

