#ifndef _POP_WIN_
#define _POP_WIN_

#define N_UNIT_WIDTH	80
#define N_UNIT_HEIGHT	40
#define WM_PRICETYPE_CHANGED  5001
class CPopWin :public TIxFrm
{
public:
	CPopWin();
	~CPopWin();

	void CreatePopWin(IOperator*opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT ncol = 3, UINT nrow = 3);	//创建弹出窗口

	void UpdateData(bool bBase = false);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<wstring> m_vstring;
	HDC		m_HdcMem;		//缓存DC
	HDC		m_HdcUnit;		//单元DC
	
	HBITMAP m_hBKBitmap;	//背景位图
	HBITMAP m_hFGBitmap;	//前景位图 单元
	
	UINT	m_nPos;			//鼠标
	
	int		m_nUintx;		//单元宽度
	int		m_nUinty;		//单元高度
	RECT	m_reUnit;		//单元矩形框

	RECT	 m_reClient;	//窗口矩形框

	int      m_nLeft;
	int      m_nTop;
	UINT     m_nCol;
	UINT     m_nRow;
	int		 m_nWidth;		//窗口宽度
	int		 m_nHeight;		//窗口高度

	IOperator* m_opr;

	sMousewinCfg m_MouseCfg;

	bool	m_bBase;
};
#endif