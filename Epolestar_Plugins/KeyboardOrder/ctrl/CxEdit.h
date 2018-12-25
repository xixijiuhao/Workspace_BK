#pragma once
#include "CxSubClassWnd.h"

class CxEdit :public CxSubClassWnd
{
public:
	CxEdit();
	~CxEdit();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	void DrawItemFrame(bool bHover, bool bFocus);
};

