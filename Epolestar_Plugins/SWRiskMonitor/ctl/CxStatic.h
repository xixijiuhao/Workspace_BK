#pragma once
#include "CxSubClassWnd.h"

class CxStatic : public CxSubClassWnd
{
public:
	CxStatic();
	~CxStatic();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
private:
	
	bool	m_bHover;

	bool	m_bTracking;
	//文本
	wchar_t	m_strText[20];

public:
	void Init(HWND hwnd);
	void UpdateText(const wchar_t* strText);//供外部调用
};

