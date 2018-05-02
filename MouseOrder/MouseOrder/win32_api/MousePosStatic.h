#pragma once

class CxMousePos :public CxSubClassWnd
{
public:
	CxMousePos();
	~CxMousePos();
	void UpdatePosInfo(int nLong, int nShort, wchar_t* sPriceLong, wchar_t* sPriceShort);
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
private:	
	int m_nLong;			//多头持仓
	int m_nShort;			//空头持仓
	wstring m_wLongPrice;	//多头价格
	wstring m_wShortPrice;	//空头价格
	COLORREF m_ColorBText;	//买入色
	COLORREF m_ColorSText;	//卖出色
public:
	void Init(HWND hwnd, bool bUseCfg = false);
};

