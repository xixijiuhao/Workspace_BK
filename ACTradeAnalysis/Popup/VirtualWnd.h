#pragma  once

class VirtualWnd : public TIxFrm
{
public:
	VirtualWnd();
	~VirtualWnd();
	void ShowFrm(HWND hParent, TCHAR* WndName, wstring TitleName);
	void MoveWindow(int x, int y, int cx, int cy);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCreate() = 0;  //创建页面控件
	virtual void OnPaint(); 
	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; }
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; }
	virtual LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam) { return NOT_PROCESSED; }
	virtual void OnBtnClick(WPARAM wParam, LPARAM lParam){}
	virtual void DrawMain(HDC hdc){};
	virtual LRESULT OnNcDestory(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; };
	void CloseWnd();
	virtual void OnClose(){};
private:
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	UINT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	void OnSetCursor(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
	
	void OnSizeing(WPARAM wParam, LPARAM lParam);
private:
	wstring m_TitleName;
private:
	bool			m_bhoverclose;
	bool            m_bhovermini;
	bool			m_bSizeMin;
	bool			m_bmousetrack;
protected:
	HWND			m_hParent;
	const int CAPTION_HEIGHT = 30;
private:
	RECT m_rectClose;
	RECT m_rectMini;
	RECT m_rectSizeMin;
protected:
	int Wnd_Width;
	int Wnd_Height;
	COLORREF m_colorBk;
	COLORREF m_colorCaption;
	int MinWidth;
	int MinHeight;
};