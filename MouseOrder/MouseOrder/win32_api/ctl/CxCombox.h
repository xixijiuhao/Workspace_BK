#pragma once

class CxCombox :public CxSubClassWnd
{
public:
	CxCombox();
	~CxCombox();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
	BOOL m_bTracking;
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);
	void DrawItemFrame(bool bHover, bool bFocus, bool bArrow=false);
};

