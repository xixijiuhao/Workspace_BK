#pragma once

class CLockBtn :public CxSubClassWnd
{
public:
	CLockBtn(UINT IDBitmap);
	~CLockBtn();
	BOOL Init(HWND hwnd,int nInflate = 1);
	void SetToolTips(const wchar_t* sTips){ m_sToolTips = sTips; };
private:
	bool m_bMouseOnButton;
	HIMAGELIST m_bitmapList;
	UINT m_IDBitmap;
	bool m_bLock;
	bool m_bDown;

	HWND	 m_hTip;		//提示信息句柄
	TOOLINFO m_tip;			//提示信息结构体
	wstring  m_sToolTips;	//提示文本
	BOOL    m_bMouseTrack;	//鼠标跟踪
	int		m_nInflate;
protected:
	//处理消息
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//自绘
	void DrawItem(HDC hdc);
	//类似于MFC中的Draw3dRect
	void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
	//类似于MFC中的Draw3dRect
	void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
	//
	void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);
public:
	//设置状态
	void SetLockState(bool bLock) { m_bLock = bLock; InvalidateRect(m_hWnd,NULL,TRUE); }
	//获取状态
	inline bool GetLockState()	{ return m_bLock; }
};

