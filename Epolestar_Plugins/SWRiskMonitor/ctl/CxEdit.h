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
	HBRUSH m_ClientBrush;
	HBRUSH m_hOrderBrush;
	HBRUSH m_HotOrderBrush1;
	HBRUSH m_HotOrderBrush2;
	void DrawItemFrame(bool bHover, bool bFocus);
};

