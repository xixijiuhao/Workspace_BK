#pragma once

class CSwitchBtn :public CxSubClassWnd
{
public:
	CSwitchBtn();
	~CSwitchBtn();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	wstring		m_sTextlevel1;
	wstring		m_sTextlevel2;
	int			m_nState;
	int			m_nSwitchLevel;

	HWND	 m_hTip;		//提示信息句柄
	TOOLINFO m_tip;			//提示信息结构体
	wstring  m_sToolTips;	//提示文本
public:
	void DrawItem(HDC hdc);
public:
	void InitButton(HWND hwnd, const wchar_t* w1, const wchar_t* w2,const wchar_t *wtip=TEXT(""));

	void SetSwitchLevel(int nlevel){ m_nSwitchLevel = nlevel; InvalidateRect(m_hWnd, NULL, TRUE); };

	int GetSwitchLevel(){ return m_nSwitchLevel; };
};

