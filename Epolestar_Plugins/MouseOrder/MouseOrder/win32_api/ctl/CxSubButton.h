#ifndef _CX_SUB_BUTTON_
#define _CX_SUB_BUTTON_

enum BTN_STATE
{
	BTN_S_NORMAL,
	BTN_S_FOCUS,
	BTN_S_DOWN,
	BTN_S_UP,
	BTN_S_OVER,
	BTN_S_LEAVE
};

class CxSubButton :public TIxFrm
{
public:
	CxSubButton();
	~CxSubButton();

	void	CreatePopWin(LPCTSTR classname, HWND parent, DWORD style, RECT rect, bool btriangle = false, COLORREF color = RGB(0, 0, 0), DWORD exstyle = 0);	//创建弹出窗口 传递位图句柄
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseHover(WPARAM wParam, LPARAM lParam);
	virtual void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual void OnCreate(WPARAM wParam, LPARAM lParam);
	virtual void OnSize(WPARAM wParam, LPARAM lParam);
	virtual void OnSetFocus(WPARAM wParam, LPARAM lParam);
	virtual void OnKillFocus(WPARAM wParam, LPARAM lParam);
	virtual void OnDestory(WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(WPARAM wParam, LPARAM lParam);

	virtual void OnDrawItemText(HDC &hdc, RECT rect){};

	bool GetClickCheck(){ return m_bInRect; }
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	HDC		m_hdctemp;					//缓存DC 图片加文字
	HDC		m_HdcMemNormal;				//缓存DC 图片背景
	HDC		m_HdcMemSpecial;			//缓存DC 图片背景

	HBITMAP m_hBitmapNormal;
	HBITMAP m_hBitmapSpecial;
	HBITMAP m_hbmptemp;				//目的大小位图
	BITMAP  m_bBitmapInfoNormal;	//正常位图
	BITMAP  m_bBitmapInfoSpecial;	//特殊位图
	
	HDC		m_HdcMemInvalid;		//缓存DC 图片背景
	HBITMAP m_hBitmapInvalid;		//无效状态位图
	
	RECT	m_reClient;		//窗口矩形框
	int		m_nLeft;		//窗口起点 横坐标
	int     m_nTop;			//窗口起点 纵坐标
	int		m_nWidth;		//窗口宽度
	int		m_nHeight;		//窗口高度

	BOOL    m_bMouseTrack;	//鼠标跟踪
	
	bool	m_bInRect;		//是否在区域内

	COLORREF m_ColorEdge;	//边框画刷

protected:
	BTN_STATE	m_nState;		//当前状态

	bool	m_bTriangle;//是否显示 下拉箭头
	
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth,bool bFull=false);//宽度奇数
};

#endif