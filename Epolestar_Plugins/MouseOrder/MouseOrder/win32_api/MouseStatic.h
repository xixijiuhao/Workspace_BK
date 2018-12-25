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
	//文本
	wstring	m_strText;
	//状态
	int		m_nState;	//0   1   2

	HWND	 m_hTip;		//提示信息句柄
	TOOLINFO m_tip;			//提示信息结构体
	wstring  m_sToolTips;	//提示文本
	
public:
	void Init(HWND hwnd);
	void UpdateText(string strText);//供外部调用
	void SetToolTips(const wchar_t* sTips){ m_sToolTips = sTips; };
};

