#ifndef _POP_WIN_PRICE
#define _POP_WIN_PRICE

#define N_PRICE_WIDTH	80
#define N_PRICE_HEIGHT	20
#define WM_PRICEDATA_CHANGED  5006

class CPopWinPrice :public TIxFrm
{
public:
	CPopWinPrice();
	~CPopWinPrice();

	void CreatePopWin(IOperator*opr,LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ncenter = 0, UINT ncol = 1, UINT nrow = 9);	//创建弹出窗口

	bool UpdateData(SOFCommodityInf CommkodityInfo, double dMidprice, double dMaxprice, double dMinprice);

	void GetWinSize(int &nwidth,int &nheight);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<string> m_vstring;
	HDC		m_HdcMem;		//缓存DC
	HDC		m_HdcUnit;		//单元DC
	
	HBITMAP m_hBKBitmap;	//背景位图
	HBITMAP m_hFGBitmap;	//前景位图 单元
	UINT	m_nPos;			//鼠标
	
	int		m_nUintx;		//单元宽度
	int		m_nUinty;		//单元高度
	RECT	m_reUnit;		//单元矩形框
	RECT	 m_reClient;	//窗口矩形框

	int      m_nLeft;		//起始坐标
	int      m_nCenter;
	UINT     m_nCol;		//行列数
	UINT     m_nRow;
	int		 m_nWidth;		//窗口宽度
	int		 m_nHeight;		//窗口高度

	bool	IsValidPrice(double dPrice);
	void	GetStrPriceByLineNo(int row, string &strPrice);
	double	GetPriceByLineNo(int row);
	bool	GetFractionPriceByLineNo(int row, SFractionPrice &tempPrice);
	void	DrawLineText(int nLine);
	SOFCommodityInf m_CommkodityInfo;
	double	m_MidPrice;	//最新价
	double	m_MaxPrice;	//涨停价
	double	m_MinPrice;	//跌停价
	int		m_iIndex;	//索引
	string	m_sPrice;	//价格字符串

	IOperator* m_opr;
};
#endif