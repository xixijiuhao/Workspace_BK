#ifndef _POP_WIN_TIP_
#define _POP_WIN_TIP_

class CPopWinTip :public TIxFrm
{
public:
	CPopWinTip();
	~CPopWinTip();

	void CreatePopWin(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);	//创建弹出窗口
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	wstring m_string;
	HDC		m_HdcMem;		//缓存DC
	
	HBITMAP m_hBKBitmap;	//背景位图
	RECT	 m_reClient;	//窗口矩形框

	int		 m_nWidth;		//窗口宽度
	int		 m_nHeight;		//窗口高度

	BOOL    m_bMouseTrack;	//鼠标跟踪
	bool    m_bHover;
};
#endif