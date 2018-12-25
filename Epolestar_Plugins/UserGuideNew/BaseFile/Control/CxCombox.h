#pragma once

class CxCombox :public CxSubClassWnd
{
public:
	CxCombox();
	~CxCombox();
public:
	void Create(HWND hParent, UINT id = 0);
	void AddString(TCHAR *pszText, int index = -1);
	void SetSelect(int index);
	void SetSelect(TCHAR *pszText);
	int  GetSelect();
	void Clear();
	void SetHeight(int x, int i =-1);
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

