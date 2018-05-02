#pragma once

class CMousePriceEdit :public CxSubClassWnd
{
public:
	CMousePriceEdit(unsigned int &);
	~CMousePriceEdit();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void DrawItemFrame(bool bHover, bool bFocus);
private:
	//价格光标位于第几项 引用
	unsigned int &m_nCursorPosRef;	

	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	
public:
	void Init(HWND hwnd);

};

