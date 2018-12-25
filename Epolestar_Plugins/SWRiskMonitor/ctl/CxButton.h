#pragma once
#include "CxSubClassWnd.h"

class CxButton :public CxSubClassWnd
{
public:
	CxButton();
	~CxButton();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bDown;
	bool m_bTracking;
	bool m_bArrow;
	bool m_bBorder;
	void DrawItemFrame(bool bHover, bool bFocus, bool bDown = false);
	void DrawTriangle(HDC hdc, int nStartx, int nStarty, int nWidth);
public:
	void SetButtonText(const wchar_t* wText);
	int GetButtonText(wchar_t * text,unsigned int nLen);
	BOOL Init(HWND hWnd,bool bArrow=false,bool bBorder = true);
};

