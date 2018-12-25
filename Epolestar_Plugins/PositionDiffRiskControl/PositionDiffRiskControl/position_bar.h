#pragma once

class CPositionDiffBar:public TIxFrm
{
public:
	CPositionDiffBar();
	~CPositionDiffBar();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void IsActive();
private:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//窗口处理过程
private:
	void OnCreateWindow();								//窗口创建消息响应
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnPaint(WPARAM wParam, LPARAM lParam);		//重绘
	void OnNotify(WPARAM wParam, LPARAM lParam);

private:
	//HBRUSH		m_hStaticBKBrush;	//静态文本背景画刷
	HWND		hwnd;		
	HBRUSH		m_brushBg;
	COLORREF	m_textColor;
	HFONT		m_font;
};