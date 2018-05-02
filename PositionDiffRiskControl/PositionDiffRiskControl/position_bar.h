#pragma once

class CPositionDiffBar:public TIxFrm
{
public:
	CPositionDiffBar();
	~CPositionDiffBar();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void IsActive();
private:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//���ڴ������
private:
	void OnCreateWindow();								//���ڴ�����Ϣ��Ӧ
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnPaint(WPARAM wParam, LPARAM lParam);		//�ػ�
	void OnNotify(WPARAM wParam, LPARAM lParam);

private:
	//HBRUSH		m_hStaticBKBrush;	//��̬�ı�������ˢ
	HWND		hwnd;		
	HBRUSH		m_brushBg;
	COLORREF	m_textColor;
	HFONT		m_font;
};