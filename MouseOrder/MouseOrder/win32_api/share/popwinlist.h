#ifndef _POP_WIN_LIST_
#define _POP_WIN_LIST_

class CPopWinList :public TIxFrm
{
public:
	CPopWinList();
	~CPopWinList();

	void CreatePopWin(IOperator *p, HWND parent,int nwidth, int nleft, int ntop, int nRef, TMouPriceTypeStrW sMem[10], int nSize,int nRow, int nCol,bool bCombox = false);	//创建弹出窗口

	void UpdateData(TMouPriceTypeStrW sMem[10], int nSize);

	void UpdateUnit(int nWidth);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void DrawList();
	void DrawCombox();
private:
	vector<wstring> m_vstring;

	HDC		m_HdcMem;		//缓存DC
	
	HBITMAP m_hBKBitmap;	//背景位图
	
	UINT	m_nPos;			//鼠标
	
	RECT	m_reClient;	//窗口矩形框

	int     m_nLeft;
	int     m_nTop;
	int		m_nWidth;		//窗口宽度
	int		m_nHeight;		//窗口高度

	int		m_nUnit;		//窗口宽度
	int		m_nRow;
	int		m_nCol;
private:
	bool	m_bHover;		//激活
	bool	m_bTracking;	//跟踪
	bool	m_bCombox;		//高仿下拉
private:
	IOperator *m_pOper;			//回调接口

	int    m_nRef;
};
#endif