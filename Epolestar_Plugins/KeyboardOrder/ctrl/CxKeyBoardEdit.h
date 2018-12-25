#pragma once

class COFInputDlg;
class CxKeyBoardEdit :public CxSubClassWnd
{
public:
	CxKeyBoardEdit(COFInputDlg&ref);
	~CxKeyBoardEdit();
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	COFInputDlg &m_RefDlg;
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	HBRUSH m_ClientBrush;
	HBRUSH m_hOrderBrush;
	HBRUSH m_HotOrderBrush1;
	HBRUSH m_HotOrderBrush2;
};

