#pragma once

class CLockBtn :public CxSubClassWnd
{
public:
	CLockBtn(UINT IDBitmap);
	~CLockBtn();
	BOOL Init(HWND hwnd);
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
protected:
	//处理消息
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//自绘
	void DrawItem(HDC hdc);
public:
	//创建button
	void CreateButton();
public:
	//设置状态
	void SetLockState(bool bLock) { m_bLock = bLock; InvalidateRect(m_hWnd,NULL,TRUE); }
	//获取状态
	inline bool GetLockState()	{ return m_bLock; }
};

