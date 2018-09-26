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

	HWND	 m_hTip;		//��ʾ��Ϣ���
	TOOLINFO m_tip;			//��ʾ��Ϣ�ṹ��
	wstring  m_sToolTips;	//��ʾ�ı�
	BOOL    m_bMouseTrack;	//������
protected:
	//������Ϣ
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//�Ի�
	void DrawItem(HDC hdc);
public:
	//����button
	void CreateButton();
public:
	//����״̬
	void SetLockState(bool bLock) { m_bLock = bLock; InvalidateRect(m_hWnd,NULL,TRUE); }
	//��ȡ״̬
	inline bool GetLockState()	{ return m_bLock; }
};

