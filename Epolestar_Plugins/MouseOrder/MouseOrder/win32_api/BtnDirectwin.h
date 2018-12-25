#ifndef _BTN_DIRECT_WIN_
#define _BTN_DIRECT_WIN_

//消息
#define WM_ORDER		5004	

class CBtnWin :public TIxFrm
{
public:
	CBtnWin();
	~CBtnWin();

	void	CreatePopWin(IOperator *opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, int nwidth = 0, int nheight = 0, char cDirect = oNone, char cOffset = dNone, char cSellOffset = dNone,COLORREF rbuy=RGB(201,51,39),COLORREF rsell = RGB(26,129,237));	//创建弹出窗口

	void	SetLongData(int nlong);// { m_nLong = nlong; };		//设置多头数量
	int     GetLongData(){ return m_nLong; };
	void	SetPriceData(wchar_t* dprice);// { m_dPrice = dprice; };		//设置多头数量
	wstring  GetPriceData(){ return m_dPrice; };
	void	SetShortData(int nshort);// { m_nShort = nshort; };	//设置空头数量
	int     GetShortData(){ return m_nShort; };

	void    UpdateWindow(RECT &rect);									//更新窗口

	void	SetOffset(TCoverMode cm);						//设置开平

	void    SetPosShowFlag(bool bShow = false);// { m_bShowPos = bShow; }	//设置显示持仓

	void	SetCheck(bool bGetF);										//设置是否获取焦点标记
	bool	GetCheck();													//获取是否拥有焦点标价

	void    SetOffer(bool bOffer){ m_bOffer = bOffer; }					//设置应价标识

	void    SetTextQty(wchar_t *cQty);						//设置开仓信息文本		外盘
	void    SetTextPre(wchar_t *cPre, wchar_t *cPreBase);	//设置预冻结手续费信息	外盘

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:

	HDC		m_hdctemp;		//缓存DC 图片加文字
	HDC		m_HdcMem;		//缓存DC 图片背景
	
	RECT	m_reClient;		//窗口矩形框
	RECT    m_rePos;		//文字显示区域坐标

	int		m_nLeft;		//窗口起点 横坐标
	int     m_nTop;			//窗口起点 纵坐标
	int		m_nWidth;		//窗口宽度
	int		m_nHeight;		//窗口高度

	TDirect m_tDirect;		//买卖方向
	TOffset m_tOffset;		//开平方向
	TOffset m_tSellOffset;	//卖出开平方向
	HBITMAP m_hBitmap;		//蓝色位图
	HBITMAP m_hbmptemp;		//目的大小位图
	BITMAP  m_bBitmapInfo;	//位图大小
	int     m_nState;			//状态
	BOOL    m_bMouseTrack;		//鼠标跟踪

	void    DrawItemText(HDC &hdc, RECT &rect);

	bool    m_bShowPos;			//显示持仓标识
	wchar_t    m_cPosShow[20];		//持仓文本
	//持仓
	int     m_nLong;			//多头 
	int     m_nShort;			//空头
	//价格
	wstring  m_dPrice;			//下单价格
	RECT    m_rePrice;			//价格

	IOperator *m_opr;	//操作
	bool	m_bFocus;	//当前 是否得到焦点
	bool	m_bInFlag;	//是否在窗口区域按下左键

	bool    m_bOffer;	//应价标识

	wstring    m_cCanQtyShow;		//建仓数量
	wstring    m_cPreFreeShow;		//预冻结资金
	wstring    m_cPreFreeBaseShow;	//基币
	RECT    m_reCanQty;				//文字显示区域坐标
	RECT    m_rePreFree;			//文字显示区域坐标
	RECT    m_rePreFreeBase;		//文字显示区域坐标

	COLORREF m_BuyColor;
	COLORREF m_SellColor;
};

class CBtnDirectWin :public TIxFrm
{
public:
	CBtnDirectWin();
	~CBtnDirectWin();

	void CreatePopWin(IOperator *opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, int nwidth = 0, int nheight = 0, char cDirect = dNone, COLORREF rbuy = RGB(201, 51, 39), COLORREF rsell = RGB(26, 129, 237));	//创建弹出窗口

	void	SetLongData(int nlong);							//设置多头持仓
	int     GetLongData(){ return m_nLong; };				//获取多头持仓
	void	SetPriceData(wchar_t* dprice);					//设置多头数量
	wstring  GetPriceData(){ return m_dPrice; };
	void	SetShortData(int nshort);						//设置空头持仓
	int     GetShortData(){ return m_nShort; };				//获取空头持仓
	void	SetCanOpenCover(wchar_t* wCon);

	void    UpdateWindow(RECT &rect);						//刷新窗口 重绘	

	void	SetOffset(TCoverMode cm);

	void    SetPosShowFlag(bool bShow = false);// { m_bShowPos = bShow; }

	HWND    GetHwndOpen();
	HWND    GetHwndClose();

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	vector<string> m_vstring;
	HDC		m_HdcMem;		//缓存DC
	

	UINT	m_nPos;			//鼠标
	
	RECT	m_reUnit;		//单元矩形框
	RECT	m_reClient;		//窗口矩形框

	RECT    m_reText;		//方向
	RECT    m_rePos;		//持仓
	int		m_nLeft;
	int     m_nTop;
	int		m_nWidth;		//窗口宽度
	int		m_nHeight;		//窗口高度

	CBtnWin m_Btnwinleft;		//开仓按钮
	CBtnWin m_Btnwinright;		//平仓按钮

    TDirect m_tDirect;			//方向
	
	HBITMAP m_hBitmap;			//蓝色位图
	BITMAP  m_bitmap;			//位图大小

	wstring m_sDirect;			//方向文本
	wstring m_sDirectOffer;		//应价方向
	bool	m_bShowPos;			//持仓显示标记
	wchar_t m_cPosShow[20];		//持仓数据
	
	int     m_nLong;			//多头 //持仓
	int     m_nShort;			//空头

	//价格
	wstring m_dPrice;			//下单价格
	RECT	m_rePrice;			//价格

	wchar_t m_sCanOpenCover[40];//可开可平
	RECT	m_reCanOpenCover;	//可开可平

	IOperator *m_opr;

	RECT    m_reLeftBtn;		//
	RECT    m_reRightBtn;		//

	COLORREF m_BuyColor;
	COLORREF m_SellColor;
};
#endif