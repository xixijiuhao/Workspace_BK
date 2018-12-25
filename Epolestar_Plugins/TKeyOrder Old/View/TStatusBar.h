#ifndef _T_STATUS_BAR_H
#define _T_STATUS_BAR_H

class TStatusBar :public TIxFrm
{
public:
	TStatusBar();
	~TStatusBar();
	void ShowFrm();
	void UserLogin(bool bLogin =true);
	static void OnReceiveNotice(HWND From, HWND To, const char* Sender, const char* Action, const char* Content);
protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnLBtnDown(WPARAM wParam, LPARAM lParam);
protected:
	void OnPaint();
private:
	COLORREF  m_textColor;
};


#endif