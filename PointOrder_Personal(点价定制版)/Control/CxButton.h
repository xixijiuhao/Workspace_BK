#pragma once
using namespace std;

class CxButton :public CxSubClassWnd
{
public:
	CxButton();
	~CxButton();

	void Create(HWND hParent, TCHAR* name, UINT style = 0, UINT id = 0);
	void SetArrow(bool bArrow);
	void SetButtonText(wstring wText);
	void SetButtonText(string lpString);
	wstring GetButtonText();

protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bDown;
	bool m_bTracking;
	bool m_bArrow;
	void DrawItemFrame(bool bHover, bool bFocus, bool bDown = false);
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);
	BOOL Init(HWND hWnd);
};

