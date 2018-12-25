#pragma once
#include "CxSubClassWnd.h"
#include "UIDesign.h"
#include <CommCtrl.h>

class CxCombox :public CxSubClassWnd
{
public:
	CxCombox();
	~CxCombox();
public:
	void Create(HWND hParent, UINT id = 0);
	void AddString(const TCHAR *pszText, int index = -1);
	void SetSelect(int index);
	bool SetSelect(const TCHAR* pszText);
	int  GetSelect();
	int  GetCount();
	void GetText(TCHAR* pszText);
	void Clear();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
	BOOL m_bTracking;
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);
	void DrawItemFrame(bool bHover, bool bFocus, bool bArrow=false);
	HBRUSH m_hEditBrushNormal;
	HBRUSH m_hEditBrushInvalid;
};

