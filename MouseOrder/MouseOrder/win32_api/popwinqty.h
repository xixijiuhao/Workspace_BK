#ifndef _POP_WIN_QTY_
#define _POP_WIN_QTY_

#define NQ_UNIT_WIDTH	37
#define NQ_UNIT_HEIGHT	22
#define WM_ORDERQTY_CHANGED  5003
#define NQ_BORDER		2
class CPopWinQty :public TIxFrm
{
public:
	CPopWinQty();
	~CPopWinQty();

	void CreatePopWin(IOperator*opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, UINT ncol = 3, UINT nrow = 3);	//创建弹出窗口

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<string> m_vstring;
	HDC		 m_HdcMem;		//缓存DC
	
	HBITMAP  m_hBKBitmap;	//背景位图
	
	UINT	 m_nPos;			//鼠标
	
	RECT	 m_reClient;	//窗口矩形框

	int      m_nLeft;
	int      m_nTop;
	UINT     m_nCol;
	UINT     m_nRow;
	int		 m_nWidth;		//窗口宽度
	int		 m_nHeight;		//窗口高度

	IOperator* m_opr;
};
#endif