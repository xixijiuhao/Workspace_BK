#pragma once
extern G_UIFONT g_FontData;

#define LEN_RET		10
class CxStatic :public CxSubClassWnd
{
public:
	CxStatic();
	~CxStatic();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void DrawTriangle2(HDC &hdc, int nStartx, int nStarty, int nWidth);
private:	
	bool	m_bHover;

	bool	m_bTracking;

	bool	m_bTriangle;
	//ÎÄ±¾
	wchar_t	m_strText[LEN_RET];

	//×ÖÌå
	HFONT	m_hfont;
public:
	void Init(HWND hwnd,const wchar_t *wtext,bool btriangle = false, HFONT hfont = g_FontData.GetFontWord13());	
};

