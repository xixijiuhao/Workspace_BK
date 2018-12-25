#pragma once

class COFInputDlg;
class CxKeyBoardButton :public CxSubClassWnd
{
public:
	CxKeyBoardButton(COFInputDlg *);
	CxKeyBoardButton();
	~CxKeyBoardButton();
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	bool m_bDown;
protected:
	//处理消息
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	COFInputDlg *m_Ref;
	
private:
	void DrawItemFrame(bool bHover, bool bFocus, bool bDown = false);
	wstring m_sText;
public:
	BOOL Init(HWND hWnd);
	void SetButtonText(wstring wText);
};

