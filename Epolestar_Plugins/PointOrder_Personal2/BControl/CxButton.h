#pragma once
#include "CxSubClassWnd.h"
#include <xstring>
using namespace std;

class CxButton :public CxSubClassWnd
{
public:
	CxButton();
	~CxButton();
	void Create(HWND hParent, UINT id, UINT ExStyle = 0);
	void MoveWindow(int x, int y, int cx, int cy);
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
public:
	void SetButtonText(wstring wText);
	void SetButtonText(string lpString);
	wstring GetButtonText();
	BOOL Init(HWND hWnd,bool bArrow=false);
};

