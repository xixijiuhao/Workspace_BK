#ifndef _T_STATIC_H_
#define _T_STATIC_H_

class TStatic
{
public:
	void Create(HWND hParent, const TCHAR* pszText, UINT style = SS_LEFT | SS_CENTERIMAGE, UINT id = 0);
	void MoveWindow(int x,int y,int cx, int cy);
	void SetText(const TCHAR *pszText);
	void SetText(const char* pszText);
	void SetFont(HFONT hfont);
	HWND GetHwnd(){ return m_hStatic; }
private:
	HWND m_hStatic;
};

#endif