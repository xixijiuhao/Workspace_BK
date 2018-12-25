#pragma once

class CLockBtn:public TIxFrm
{
public:
	CLockBtn(UINT IDBitmap);
	~CLockBtn();
	void CreateLockBtn(HWND hParent, int index = 0);
	void MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
	void OnLbuttonDown(WPARAM wParam, LPARAM lParam);

private:
	bool m_bMouseOnButton;
	HIMAGELIST m_bitmapList;
	UINT m_IDBitmap;
	bool m_bLock;
	bool m_bFocus;
	int  m_nindex;
protected:

	virtual		LRESULT		WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//自绘
	void DrawItem(HDC hdc);
public:
	//设置状态
	void SetLockState(bool bLock) { m_bLock = bLock; InvalidateRect(m_Hwnd,NULL,TRUE); }
	//获取状态
	inline bool GetLockState()	{ return m_bLock; }
private:
	void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
	void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
	void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);

};

