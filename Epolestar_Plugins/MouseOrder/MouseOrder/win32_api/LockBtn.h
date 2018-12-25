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

	HWND	 m_hTip;		//��ʾ��Ϣ���
	TOOLINFO m_tip;			//��ʾ��Ϣ�ṹ��
	wstring  m_sToolTips;	//��ʾ�ı�
	BOOL    m_bMouseTrack;	//������
	int		m_nInflate;
protected:
	//������Ϣ
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//�Ի�
	void DrawItem(HDC hdc);
	//������MFC�е�Draw3dRect
	void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
	//������MFC�е�Draw3dRect
	void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
	//
	void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);
public:
	//����״̬
	void SetLockState(bool bLock) { m_bLock = bLock; InvalidateRect(m_hWnd,NULL,TRUE); }
	//��ȡ״̬
	inline bool GetLockState()	{ return m_bLock; }
};

