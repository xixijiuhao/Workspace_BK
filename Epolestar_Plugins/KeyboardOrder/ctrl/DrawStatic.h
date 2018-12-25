#pragma once

class CDrawStatic :public CxSubClassWnd
{
public:
	CDrawStatic();
	~CDrawStatic();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
private:
	//持仓
	unsigned int m_iPosQty;		
	//默认字体大小
	HFONT m_font;		
	//默认pen
	HPEN m_pen;	
	//文本
	wstring	m_strText;
	//默认背景色
	COLORREF m_rgbBk;		
	//文本颜色
	COLORREF m_rgbText;		
	//brush
	HBRUSH m_brush;
public:
	void InitStatic(HWND hwnd);
	void UpdateText(unsigned int PosQty, wstring strText, LOGFONT &lgFont, COLORREF rgbBk, COLORREF rgbText);//供外部调用
	void UpdateText(unsigned int PosQty, wstring strText, COLORREF rgbText, COLORREF rgbBk);
	void UpdateText(unsigned int PosQty, wstring strText, COLORREF rgbText);
	void SetAutoSize();
};

