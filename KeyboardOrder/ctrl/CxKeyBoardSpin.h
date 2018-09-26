#pragma once

class CxKeyBoardSpin :public CxSubClassWnd
{
public:
	CxKeyBoardSpin();
	~CxKeyBoardSpin();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bDown;
	bool m_bTracking;
	POINT m_point;
	
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth, bool bNormal=true);
	void DrawItemFrame(bool bHover, bool bFocus, bool bDown = false);
};

