#pragma  once

class VirtualWnd : public TIxFrm
{
public:
	VirtualWnd();
	~VirtualWnd();
	void ShowFrm(HWND hParent, TCHAR* WndName, string TitleName);
	void MoveWindow(int x, int y, int cx, int cy);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCreate() = 0;  //创建页面控件
	virtual void OnPaint(); 
	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; }
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; }
	virtual LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam) { return NOT_PROCESSED; }
	virtual void OnBtnClick(WPARAM wParam, LPARAM lParam){}
	virtual void DrawMain(TMemDC& pmemdc){};
	virtual LRESULT OnNcDestory(WPARAM wParam, LPARAM lParam){ return NOT_PROCESSED; };
	void CloseWnd();
	virtual void OnClose(){};

	void SetNeedDestroy(bool bNeed){ m_bNeedDestroyWnd = bNeed; } //是否需要点击关闭按钮DestroyWindow
private:
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

	//new
	bool			m_bNeedDestroyWnd;

protected:
	HWND			m_hParent;
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