#pragma once

class CxCombox :public CxSubClassWnd
{
public:
	CxCombox();
	~CxCombox();
public:
	void Create(HWND hParent, UINT id = 0);
	void AddString(const TCHAR *pszText, int index = -1);
	int DeleteString(int index = 0);//返回值为combox剩余项数目，如果为CB_ERR（-1）则为所取index越界
	void SetSelect(int index);
	void SetSelect(const TCHAR *pszText);
	int  GetSelect();
	void Clear(){ SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0); };
	const wchar_t* GetText();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
	BOOL m_bTracking;
	TCHAR m_szContent[50];
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);
	void DrawItemFrame(bool bHover, bool bFocus, bool bArrow=false);
	HBRUSH m_hEditBrushNormal;
	HBRUSH m_hEditBrushInvalid;
};

