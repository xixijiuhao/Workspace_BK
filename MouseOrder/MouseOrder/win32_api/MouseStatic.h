#pragma once

class CMouseStatic :public CxSubClassWnd
{
public:
	CMouseStatic();
	~CMouseStatic();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
private:
	
	bool	m_bHover;

	bool	m_bTracking;
	//�ı�
	wstring	m_strText;
	//״̬
	int		m_nState;	//0   1   2

	HWND	 m_hTip;		//��ʾ��Ϣ���
	TOOLINFO m_tip;			//��ʾ��Ϣ�ṹ��
	wstring  m_sToolTips;	//��ʾ�ı�
	
public:
	void Init(HWND hwnd);
	void UpdateText(string strText);//���ⲿ����
	void SetToolTips(const wchar_t* sTips){ m_sToolTips = sTips; };
};

