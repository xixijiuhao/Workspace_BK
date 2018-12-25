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
	//�ֲ�
	unsigned int m_iPosQty;		
	//Ĭ�������С
	HFONT m_font;		
	//Ĭ��pen
	HPEN m_pen;	
	//�ı�
	wstring	m_strText;
	//Ĭ�ϱ���ɫ
	COLORREF m_rgbBk;		
	//�ı���ɫ
	COLORREF m_rgbText;		
	//brush
	HBRUSH m_brush;
public:
	void InitStatic(HWND hwnd);
	void UpdateText(unsigned int PosQty, wstring strText, LOGFONT &lgFont, COLORREF rgbBk, COLORREF rgbText);//���ⲿ����
	void UpdateText(unsigned int PosQty, wstring strText, COLORREF rgbText, COLORREF rgbBk);
	void UpdateText(unsigned int PosQty, wstring strText, COLORREF rgbText);
	void SetAutoSize();
};

