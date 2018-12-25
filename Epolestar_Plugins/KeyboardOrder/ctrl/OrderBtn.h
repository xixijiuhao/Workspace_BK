#pragma once
#include "CxSubClassWnd.h"
#include <string>

using namespace std;
class COFInputDlg;
class COrderBtn :public CxSubClassWnd
{
public:
	COrderBtn(COFInputDlg&ref);
	~COrderBtn();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	COFInputDlg &m_RefDlg;
	bool m_bEnterBtn;
	bool m_bFocus;
	//小数时的精度 默认显示两位
	int	m_iPrecision;
	int	m_BitmapIDNormal;
	int	m_BitmapIDPress;
	int	m_BitmapIDEnter;
	wstring m_strTitle;
	int	m_iTotalNum;
	int	m_iCoverNum;
	int	m_iOpenNum;
private:
	//是否显示价格
	bool m_bShowPrice;
	wstring m_strPrice;
public:
	void InitButton(HWND hwnd);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void SetBitmapID(const wchar_t *pszTitle);
public:
	void SetShowPrice(bool bShow)	{ m_bShowPrice = bShow;  InvalidateRect(GetHwnd(),NULL,TRUE); }
	void SetPrice(const wstring&strPrice) { m_strPrice = strPrice; InvalidateRect(GetHwnd(), NULL, TRUE); }
};

