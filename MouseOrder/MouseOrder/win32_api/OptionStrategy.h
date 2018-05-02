#ifndef _ORDER_OPTION_WIN_
#define _ORDER_OPTION_WIN_

#define Nref_Pricetype 0
#define Nref_Validtype 1
#define Nref_OCtype	   2

#define WM_DELOPTIONQUOTE	7000

#define N_OPTION_NONE	 0
#define N_OPTION_STD	 1
#define N_OPTION_STG	 2

enum StrategyGroup
{
	T_Bullish,			//大涨
	T_NotRise,			//不涨
	T_DullRiseCall,		//盘涨Call
	T_DullRisePut,		//盘涨Put
	T_Bearish,			//大跌
	T_NotFall,			//不跌
	T_DullFallCall,		//盘跌Call
	T_DullFallPut,		//盘跌Put
	T_Dull,				//盘整
	T_DullStrangle,		//盘整（宽）
	T_Break,			//突破	
	T_BreakStrangle,	//突破（宽）	
	T_AutoGroup,		//自组
	T_GroupType,		//情境
	T_END,
};
//行 索引
enum ERowIndex
{
	N_Head_row,
	N_1_row,
	N_2_row,
	N_3_row,
	N_4_row,
	N_5_row,
	N_Target_row,//标的
	N_Max_Row	
}; 

//列索引
enum EColItem
{
	COO_Code,
	COO_Direct,	
	COO_Qty,
	COO_PriceType,
	COO_Price,
	COO_Delta,
	COO_Gamma,
	COO_END
};

//特殊控件ID
enum OptinControl
{
	OC_PriceSync_Btn,
	OC_QtySync_Btn,
	OC_Price_Edit,
	OC_Price_Spin,
	OC_Qty_Edit,
	OC_Qty_Spin,
	OC_Support_Check,
	OC_AddOne_Check,
	QC_END,
};

//自绘控件ID
enum ESpecItem
{
	SPI_Type,
	SPI_Option,
	SPI_BackDirect,
	SPI_AddTarget,
	SPI_Reset,
	SPI_OC,
	SPI_Valid,
	SPI_Order,
    SPI_Info,   /// PreFrozen or Error message
	SPI_END
};

//方向
enum EDirectColor
{
	EDC_Buy,
	EDC_Sell,
	EDC_END
};

//行情 交易
enum EQuoteTrade
{
	EQT_Quote,
	EQT_Trade,
	EQT_End,
};

//单元属性
struct OCClickItem
{
	int nRow;
	int nCol;
};

//期权填充信息
struct TOptionInfo 
{
	SContractNoType	Contract;
	TDirect			Direct;
};
//标的信息
struct TOptionTarget
{
	bool			bAdd;					//是否添加
	TDirect			Direct;					//买卖方向
	SContractNoType	Code[EQT_End];		//行情代码//交易代码
	TOptionTarget()
	{
		memset(this, 0, sizeof(TOptionTarget));
		Direct = dBuy;
	}
};

//期权组合面板
class COrderOption :public TIxFrm, public IOperator, public ISQuoteDataSpi
{
public:
	COrderOption();
	~COrderOption();

	void CreateWin(CMousewin* opr, HWND parent, int nleft = 0, int ntop = 0, bool bUseFor = false);

	void UpdateOptionInfo(int nSize, TOptionInfo info[N_5_row], int nType = 0);

	void UpdatePrice(int nIndex = 0);				//更新价格

	void SetExchangeSupport(bool bCheck);
private:
	void SendOptionStrategy();

	bool IsActive();
public:
	//////////////////////////////////////////////////////////////////////////行情回调 	
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote) ;				//普通行情数据	
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {};							//套利行情数据
public:
	//////////////////////////////////////////////////////////////////////////
	virtual void SetPriceType(const wchar_t* sPType) {};
	virtual void SetValidType(const wchar_t* sPType) {};
	virtual void SetMaxMinQty(const char* sQty) {};
	virtual bool SendOrder(MsgOrderInfo *msg) { return false; };
	virtual void SetContractNo(const char* sCode, CodeSetType csType = cstChange) {};
	virtual void SetPriceData(const char* sdata) {};
	virtual bool GetMouseWinCfg(sMousewinCfg & cfg) { return false; };
	virtual bool SetHeadData(const wchar_t* sPType, bool bHead = true) { return false; };
	virtual bool SetSelect(const wchar_t* sStrategy, int nRef);
	virtual bool TableButtonToEdit() { return false; };
	virtual bool CxGetWindowText(MH_TYPE m_type, int& nSize, wchar_t* wstr, int nMaxLen = LEN_RET) { return false; };
public:
	WNDPROC				m_EditProc;		//价格处理过程
	WNDPROC				m_EditProc2;	//数量处理过程
	SOFCommodityInf		m_OptionCommodityInfo;	//品种信息
	SOFCommodityInf		m_TargetCommodityInfo;	//品种信息
private:
	CMousewin		*m_opr;				//竖向操作

	CPopWinList		m_PopListPriceType;	//价格选择
	CPopWinList		m_PopListValidType;	//有效类型
	CPopWinList		m_PopListOCType;	//开平

	CLockBtn		m_SyncPriceTypeBtn;	//价格类型
	CLockBtn		m_SyncQtyBtn;		//数量联动
	
	bool			m_bSyncPrice;		//价格类型联动选中
	bool			m_bSyncQty;			//数量联动选中
	
	CxSpin			m_SpinPrice;		//价格 微调
	CxSpin			m_SpinQty;			//数量 微调

	CxCheck			m_ExchangeSupport;
	CxCheck			m_AddOne;

	HWND			m_hControl[QC_END];	//控件句柄数组

	RECT			m_rcItem[N_Max_Row][COO_END];		//list区域坐标
	RECT			m_rcSpecialItem[SPI_END];			//button区域坐标
			
	TMouPriceTypeStrW	m_strItem[N_Max_Row][COO_END];	//list文本
	TMouPriceTypeStrW m_strSpecialItem[SPI_END];		//button文本
	int				m_nCurHoverID;						//高亮显示索引
	int				m_CurType;							//当前类型
	int				m_nSTDSTG;							//是否STD STG

	OCClickItem		m_LClickItem;		//左键按下坐标
	OCClickItem		m_CurShowItem;		//当前激活坐标

	HDC				m_HdcMem;			//缓存DC
	HBITMAP			m_hBitmap;			//图布

	COLORREF		m_ExColor[EDC_END];	//颜色	
	HBRUSH			m_ExBrush[EDC_END];	//画刷

	int				m_nleft;			//左坐标
	int				m_ntop;				//顶坐标
	int				m_nWidth;			//窗口宽度
	int				m_nHeight;			//窗口高度
	int				m_nOptionRows;	    //当前有效合约数
	bool			m_bTracking;		//鼠标跟踪
    bool            m_bInfoError;       /// 当前信息是错误信息还是保证金
    wchar_t         m_strInfo[MAX_PATH]; /// 状态信息，显示保证金或错误信息
private:
	char			m_Sign[21];			//账户服务器标识

	SContractNoType m_QuoteTradeCode[EQT_End][N_5_row];//行情合约
	
	TOptionTarget	m_OptionTarget;

	SOptionSeriesNoType	m_SeriesNo;
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	void LayOut();						//区域布局更新
	
	void OnCreate(WPARAM wParam, LPARAM lParam);	
	void OnNotify(WPARAM wParam, LPARAM lParam);//通知消息处理 spin控制 		
	void OnCommond(WPARAM wParam, LPARAM lParam);
	void OnPaint(WPARAM wParam, LPARAM lParam);
	void OnSize(WPARAM wParam, LPARAM lParam);
	LONG OnCtlColorStatic(WPARAM wParam, LPARAM lParam);
	void OnDestory(WPARAM wParam, LPARAM lParam);
	
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);

	void OnKillFocusPriceEdit();		//价格编辑框失去焦点
	void OnKillFocusQtyEdit();			//数量编辑框失去焦点
	
	void MoveEdit(int x, int y, int width, int height, bool bPrice = true);	//移动Edit并显示
	
	void OnButtonPriceSync();			//联动价格类型
	void OnButtonQtySync();				//联动数量
	void ResetSyncState();				//重置联动

	void OnBackDirect();
	void OnAddOptionTarget();
	void OnReset();

	void OnCheckSupport();				//交易所支持组合
	
	void OnSpinDeltaUpDownEx(int nDelta, int nID);										//价格增减处理
	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax = 1000, int nMin = 1);	//数量增减处理

	//////////////////////////////////////////////////////////////////////////缓存区域绘制
	void DrawBackColor();		//背景
	void DrawGridLine();		//网格	
	void DrawListContent();		//列表
	void DrawRow(int i);		//行
	void DrawSpecial();			//特殊区域
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth, COLORREF color = RGB(200, 200, 200));//宽度奇数

	void Redraw(RECT* rc = NULL);
	//////////////////////////////////////////////////////////////////////////信息更新
	void UpdateSyncQty();							//更新数量联动	
	void UpdateSyncType(int nIndex = 0);			//更新价格类型联动	
	void UpdateCommodityInfo(const char* Code,bool bOption = true);		//更新品种信息
	void UpdateDelta(int i);
	void UpdateTargetDelta();
	void UpdateTotal();
	int GetTypeStrId(int nType);
	bool GetSeriNo(const char* sQuote);
	//////////////////////////////////////////////////////////////////////////
	bool GetOrderInfo(int i, TSendOrder &sOrder, bool bShowMessage = true);    //获取定单信息
	bool GetOrderPrice(int nRow, double &dprice, bool bShowMessage = true);	//获取价格	
	bool GetOrderQty(int nRow, uint &uQty, bool bShowMessage = true);			//获取数量	
	bool GetOffset(int nRow, TOffset &Offset);		//获取开平	
	void GetValidType(TValidType &type);			//获取有效类型
    void UpdatePreErrorMsg(int nErrCode, std::string strContractCode);           /// 更新保证金错误信息

    void UpdatePreFreeze(); /// 更新预冻结资金

};

#endif
