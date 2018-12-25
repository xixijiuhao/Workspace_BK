#ifndef _CX_SUB_BUTTON_
#define _CX_SUB_BUTTON_

enum BTN_STATE
{
	BTN_S_NORMAL,
	BTN_S_FOCUS,
	BTN_S_DOWN,
	BTN_S_UP,
	BTN_S_OVER,
	BTN_S_LEAVE
};

class CxSubButton :public TIxFrm
{
public:
	CxSubButton();
	~CxSubButton();

	void	CreatePopWin(LPCTSTR classname, HWND parent, DWORD style, RECT rect, bool btriangle = false, COLORREF color = RGB(0, 0, 0), DWORD exstyle = 0);	//������������ ����λͼ���
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseHover(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual void OnCreate(WPARAM wParam, LPARAM lParam);
	virtual void OnSize(WPARAM wParam, LPARAM lParam);
	virtual void OnSetFocus(WPARAM wParam, LPARAM lParam);
	virtual void OnKillFocus(WPARAM wParam, LPARAM lParam);
	virtual void OnDestory(WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(WPARAM wParam, LPARAM lParam);

	virtual void OnDrawItemText(HDC &hdc, RECT rect){};

	bool GetClickCheck(){ return m_bInRect; }
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	HDC		m_hdctemp;					//����DC ͼƬ������
	HDC		m_HdcMemNormal;				//����DC ͼƬ����
	HDC		m_HdcMemSpecial;			//����DC ͼƬ����

	HBITMAP m_hBitmapNormal;
	HBITMAP m_hBitmapSpecial;
	HBITMAP m_hbmptemp;				//Ŀ�Ĵ�Сλͼ
	BITMAP  m_bBitmapInfoNormal;	//����λͼ
	BITMAP  m_bBitmapInfoSpecial;	//����λͼ
	
	HDC		m_HdcMemInvalid;		//����DC ͼƬ����
	HBITMAP m_hBitmapInvalid;		//��Ч״̬λͼ
	
	RECT	m_reClient;		//���ھ��ο�
	int		m_nLeft;		//������� ������
	int     m_nTop;			//������� ������
	int		m_nWidth;		//���ڿ��
	int		m_nHeight;		//���ڸ߶�

	BOOL    m_bMouseTrack;	//������
	
	bool	m_bInRect;		//�Ƿ���������

	COLORREF m_ColorEdge;	//�߿�ˢ

protected:
	BTN_STATE	m_nState;		//��ǰ״̬

	bool	m_bTriangle;//�Ƿ���ʾ ������ͷ
	
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth,bool bFull=false);//�������
};

#endif