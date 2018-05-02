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
	int m_nLong;			//��ͷ�ֲ�
	int m_nShort;			//��ͷ�ֲ�
	wstring m_wLongPrice;	//��ͷ�۸�
	wstring m_wShortPrice;	//��ͷ�۸�
	COLORREF m_ColorBText;	//����ɫ
	COLORREF m_ColorSText;	//����ɫ
public:
	void Init(HWND hwnd, bool bUseCfg = false);
};

