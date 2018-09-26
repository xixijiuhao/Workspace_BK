#pragma  once

class VirtualWnd : public TIxFrm
{
public:
	VirtualWnd();
	~VirtualWnd();
	void ShowFrm(HWND hParent, TCHAR* WndName, string TitleName);
	void MoveWindow(int x, int y, int cx, int cy);
	void ModifyTitle(string TitleName);
protected:
	virtual void OnCreate() = 0;  //创建页面控件
	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; }
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; }
	virtual LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam) { return NOT_PROCESSED; }
	virtual void OnBtnClick(WPARAM wParam, LPARAM lParam){}
	virtual void DrawMain(TMemDC& pmemdc){};
	virtual LRESULT OnNcDestory(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; };
	void CloseWnd();
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	
private:
	string m_TitleName;
private:
	bool			m_bhoverclose;
	bool            m_bhovermini;
	bool			m_bmousetrack;
private:
	const int CAPTION_HEIGHT = 30;
	RECT m_rectClose;
	RECT m_rectMini;
	COLORREF m_colorBk;
	COLORREF m_colorCaption;
public:
	int Wnd_Width;
	int Wnd_Height;
};