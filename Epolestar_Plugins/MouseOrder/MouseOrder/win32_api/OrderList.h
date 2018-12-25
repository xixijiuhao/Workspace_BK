#ifndef _LIST_ORDER_WIN_
#define _LIST_ORDER_WIN_

#define	N_Buy	0
#define	N_Sell	1
#define	N_Size	2
#define N_Scale	20

#define N_WIDTH_LIST		297
#define N_HEIGHT_LIST_MIN	252
#define N_HEIGHT_LIST		1080
#define N_HEIGHT_ITEM_LIST	36
#define N_AVERAGE			5


//黑色系
//#define RGB_Red				RGB(255, 60, 40)
//#define RGB_Green			RGB(0, 220, 0)
#define RGB_Yellow			RGB(220, 220, 0)
#define RGB_Red				RGB(255, 0, 0)
#define RGB_Green			RGB(0, 255, 0)

#define RGB_Black20			RGB(20,20,20)
#define RGB_Black38			RGB(38, 38, 38)
#define RGB_Black70			RGB(70, 70, 70)
//白色系
//#define RGB_Red_W			RGB(255,0,0)
//#define RGB_Green_W		RGB(20, 240, 20)

#define RGB_Red_W			RGB(157,36,12)
#define RGB_Green_W			RGB(54, 134, 50)

#define RGB_Red_B			RGB(246, 226, 226)
#define RGB_Green_B			RGB(224, 242, 223)

#define RGB_WhiteWholeBk	RGB(242,243,247)
#define RGB_WhiteSpecialBk	RGB(204, 206, 219)
#define RGB_Yellow_W		RGB(211,163,91)


//defalut
#define Default_Red                     RGB(157,36,12)          //红色 
#define Default_Green                   RGB(54, 134, 50)        //绿色
#define Default_AskBid_Red              RGB(255, 180, 180)      //红色 买1卖1背景
#define Default_AskBid_Green            RGB(180, 240, 180)      //绿色 买1卖1背景

//Light Red Green
#define Light_RG_Red                    RGB(157,36,12)          //Red
#define Light_RG_Green                  RGB(54, 134, 50)        //Green
#define Light_RG_AskBid_Red             RGB(246, 226, 226)      //红色 买1卖1背景
#define Light_RG_AskBid_Greed           RGB(224, 242, 223)      //绿色 买1卖1背景

//Dark
#define Dark_Red                        RGB(200, 100, 200)      //Red
#define Dark_Green                      RGB(85, 166, 85)        //Green
#define Dark_Black                      RGB(0, 0, 0)            //Black
#define Dark_Bg_Red                     RGB(35, 15, 15)         //Red background 点价列表红色背景
#define Dark_Bg_Green                   RGB(20, 35, 20)         //Green background 点价列表绿色背景
#define Dark_AskBid_Red                 RGB(150, 0, 0)          //红色 买1卖1背景
#define Dark_AskBid_Green               RGB(0, 150, 0)          //绿色 买1卖1背景

//Light Yellow
#define Light_Yellow                    RGB(231,227,207)

//Light Blue
#define Light_Blue                      RGB(192,209,220)

//Light Green
#define Light_Green                     RGB(199,237,204)

//Select Color
#define Select_Color                    RGB(200, 200, 20)

//涨跌标识
#define LastPrice_Keep		0
#define LastPrice_Up		1
#define LastPrice_Down		-1
enum ListItemId
{
	LII_Tick_Edit_Win,
	LII_Tick_Edit,
};
enum DataPriceType
{
	DPT_Last=0,	//最新价
	DPT_Bid1,	//买1
	DPT_Ask1,	//卖1
	DPT_VLong,	//多头持仓
	DPT_VShort,	//空头持仓
	DPT_High,	//最高价
	DPT_Low,	//最低价
	DPT_Up,		//涨停价
	DPT_Down,	//跌停价
	DPT_VStart,	//可见起始
	DPT_VBase,	//基准
	DPT_VEnd,	//可见终止
	DPT_Size,
};
enum DataSpecialItem
{
	DSI_MarketBuy,	//市买
	DSI_MarketSell,	//市卖
	DSI_Mid_1,		//最新
	DSI_Mid_2,		//买一
	DSI_Mid_3,		//卖一
	DSI_Mid_4,		//手动
	DSI_CancelB,	//撤买
	DSI_CancelS,	//撤卖
	DSI_CancelA,	//撤所有
	DSI_Size,	
};
enum ColorLevel
{
	CL_HighLight,	//高亮
	CL_LowLight,	//背景
	CL_Size,	
};
enum ColorStyle
{
    CS_Black,       //黑色
	CS_White,		//白色
	CS_BkMid,		//中部背景色
	CS_BKLine,		//边线背景色
	CS_BKSpecial,	//特殊按钮背景色
	CS_FGSpecial,	//特殊色
	CS_LastPrice,	//最新价颜色
	CS_PrcFG,		//普通价格前景色
	CS_Size,
};
enum DataGridCol
{
	DGC_LEFT,		//左
	DGC_MID,		//中
	DGC_RIGHT,		//右
	DGC_Size,
};
enum DataSpecial
{
	DS_LastHover,	//最后激活
	DS_Hover,		//激活
    DS_Select,      //选择区域
	DS_Size,
};
enum ClickType
{
	CT_None,		//无点击
	CT_Special,		//特殊按钮
	CT_BS,			//买卖按钮
};
enum BaseMidType
{
	BMT_Last,		//最新价居中
	BMT_Bid1,		//买一价居中
	BMT_Ask1,		//卖一价居中
	BMT_Hand,		//手动控制
	BMT_Size,		//
};
enum BuySellSubItem
{
	BSI_WHOLE,		//整个
	BSI_LEFT,		//左
	BSI_RIGHT,		//右
};
enum TStopType
{
	Tst_STP,
	Tst_STL,
	Tst_LMT,
	Tst_Size
};
enum PopWinType
{
	PWT_Mid,
	PWT_Stop,
	PWT_Size,
};

struct DataPriceID
{
	double	dPrice;	//价格
	int		nQty;
	int		nId;	//编号
};
struct DataQuoteQty
{
	double	dPrice;	//价格
	int		nQty;	//数量
};
struct DataLocalWorkQty
{
	int		nQtyNormal[N_Sell+1];		//挂单数量
	int		nQtyCondition[N_Sell + 1];	//止损单数量

};
struct ItemRowCol
{
	int		nRow;		//行
	int		nCol;		//列
	int		nSubItem;	//子项
};
struct ItemSpecial
{
	RECT    rect;		//按钮
	wchar_t wtext[20];	//显示内容
};
struct ItemClick
{
	int	nSpecial;		//特殊按钮编号
	int	nCol;			//买卖按钮 列
	int nRow;			//买卖按钮 行
	ClickType nType;	//特殊按钮、买卖按钮
};
struct ListOrderInfo
{
	TDirect			Direct;		//买卖
	TOffset			Offset;		//开平
	double			OrderPrice;	//委托价格
	TOrderType		OrderType;	//市价 限价
	double			TriggerPrice;//止损
};

enum ColorSytle
{
    Style_Defalut = 0,      //默认方案，浅色
    Style_Light_RG,         //浅色背景，红绿买卖背景
    Style_Dark,             //黑色背景
    Style_Light_Yellow,     //浅黄色背景
    Style_Light_Blue,       //浅蓝色背景
    Style_Light_Green,      //浅绿色背景
};

class CMousewin;

class COrderList :public TIxFrm,public IOperator
{
public:
	COrderList();
	~COrderList();
	
	void CreateWin(CMousewin* opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, bool bUseFor = false);					//创建弹出窗口
	
	void UpdateCode(const SContractNoType sCode, SOFCommodityInf& sinfo);	//更新合约信息

	void UpdateUser(const char* UserNo, const char* Sign, char cCoverMode);	//更新用户关联信息

	void UpdatePrice(bool bBaseChange = false);								//更新价格

	void UpdateQty(int nQty) { m_OrderQty = nQty; };						//更新数量

	void UpdateTradeOrder(const TOrder* data);								//委托回报更新

	void UpdateParOrderQty(bool bUpdateUI = true);							//挂单量更新

	void UpdatePosition(bool bUpdateUI = true);								//持仓线更新

	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax = 999, int nMin = -99);

	virtual void SetPriceType(const wchar_t* sPType){};
	virtual void SetValidType(const wchar_t* sPType){};
	virtual void SetMaxMinQty(const char* sQty){};
	virtual bool SendOrder(MsgOrderInfo *msg) { return false; };
	virtual void SetContractNo(const char* sCode, CodeSetType csType = cstChange){};
	virtual void SetPriceData(const char* sdata){};
	virtual bool GetMouseWinCfg(sMousewinCfg & cfg){ return false; };
	virtual bool SetHeadData(const wchar_t* sPType, bool bHead = true){ return false; };
	virtual bool SetSelect(const wchar_t* sStrategy, int nRef);
	virtual bool TableButtonToEdit(){ return false; };
	virtual bool CxGetWindowText(MH_TYPE m_type,int& nSize, wchar_t* wstr, int nMaxLen = LEN_RET) { return false; };

    void SetRightCancel(bool b);                                             //右键撤单

	WNDPROC			m_SetEdit;

	void ListSendOrder(ListOrderInfo& order);

    void UpdateColorStyle(bool bUseFor);

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	bool IsActive();

	void OnCreate(WPARAM wParam, LPARAM lParam);

	void OnCommond(WPARAM wParam, LPARAM lParam);

	void OnDestory(WPARAM wParam, LPARAM lParam);

	void OnPaint(WPARAM wParam, LPARAM lParam);

	void OnSize(WPARAM wParam, LPARAM lParam);

	LONG OnCtlColorStatic(WPARAM wParam, LPARAM lParam);

	void OnLButtonDown(WPARAM wParam, LPARAM lParam);

	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void OnRButtonDown(WPARAM wParam, LPARAM lParam);

	void OnRButtonUp(WPARAM wParam, LPARAM lParam);

	void OnMouseMove(WPARAM wParam, LPARAM lParam);

	void OnMouseHover(WPARAM wParam, LPARAM lParam);

	void OnMouseLeave(WPARAM wParam, LPARAM lParam);

	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
		
	void OnKillFocus(WPARAM wParam, LPARAM lParam);

	void OnKeyDown(WPARAM wParam, LPARAM lParam);

	void DrawItem(HDC hdc);							//绘制列表单元

	void DrawItemLeft(HDC hdc, int n, int m);						//绘制列表单元

	void DrawItemMid(HDC hdc, int n, int m);						//绘制列表单元

	void DrawItemRight(HDC hdc, int n, int m);						//绘制列表单元

	void DrawItemQty(HDC hdc);										//绘制行情挂单 客户挂单

	void DrawItemSpecial(HDC hdc);									//绘制特殊单元

	void ResetHoverState(bool bUpdateWin = false);					//
	
	void DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth);	//绘制三角
	
	void DrawArrow(HDC &hdc, int nStartx, int nStarty, int nWidth, int nHeight);	//绘制箭头

	void DrawDiv(HDC &hdc, int nStartx, int nStarty, int nWidth);	//绘制分割线

	void DrawHighLow(HDC& hdc, RECT& rect,bool bHigh = true);

	void UpdateRect(ItemRowCol &item);								//更新区域
private:
	bool GetPriceStrByLineId(TMouFormatPriceStrW &wtext, int nID);

	int GetSTLTick();

	void SetColorStyle(bool bUseFor = false, bool bBlack = true);

	void SetChildRectContent();

	void UpdateMidHead();													//更新列头中部显示

	void UpdateLastPriceData();												//更新最新价信息
private:
	void OnClickSpecial(int nItem);

	void OnClickBuySell(int nCol,int nRow);

	void OnRCancelOrder(int nCol, int nRow);

	bool  OnCancelOrder(int nOrderId,TDirect direct,bool bShowMsg = false);

private:
	void ClearData();	
    void CreateBrushObject();
    void DeleteBrushObject();
    void ResetColorStyle(bool bUseFor, int style);
private:
	CMousewin		*m_opr;	//操作

	bool			m_bCreate;
	COLORREF		m_Color[N_Sell + 1][CL_Size];	//三级颜色
	HBRUSH			m_Brush[N_Sell + 1][CL_Size];	//三级画刷

	COLORREF		m_ExColor[CS_Size];
	HBRUSH			m_ExBrush[CS_Size];	

    COLORREF        m_ColorBackground;              //整体背景色
    COLORREF        m_ColorTextHover;               //hover状态字体颜色
    COLORREF        m_ColorOrderQty;                //委托数量字体颜色
    COLORREF        m_ColorTextBuy;                 //点价列表买字体颜色
    COLORREF        m_ColorTextSell;                //点价列表sell字体颜色
    COLORREF        m_ColorTextNormal;              //普通字体颜色,没有挂单量的价格等
    COLORREF        m_ColorTextLmt;                 //LMT、STP等颜色
    COLORREF        m_ColorBuy;                     //buy
    COLORREF        m_ColorSell;                    //sell
    COLORREF        m_ColorHighLow;                 //最高价、最低价颜色
    COLORREF        m_ColorLine;                    //表格线的颜色
    COLORREF        m_ColorHoverFrame;              //hover边框颜色
    COLORREF        m_ColorBgSpecial;               //特殊区域的背景色
    COLORREF        m_ColorLastPrice;               //最新价颜色
    COLORREF        m_ColorBgMid;                   //表头中间区域背景色
    COLORREF        m_ColorBlack;                   //black
    COLORREF        m_ColorWhite;                   //white
    COLORREF        m_ColorBgAsk1;                  //卖1背景色
    COLORREF        m_ColorBgBid1;                  //买1背景色
    COLORREF        m_ColorBgBid;                   //买区间背景色
    COLORREF        m_ColorBgAsk;                   //卖区间背景色
    COLORREF        m_ColorBgBuy;                   //buy背景色
    COLORREF        m_ColorBgSell;                  //sell背景色

    HBRUSH          m_BrushBackground;              //整体背景色
    HBRUSH          m_BrushTextHover;               //hover状态字体颜色
    HBRUSH          m_BrushTextQty;                 //数量字体颜色
    HBRUSH          m_BrushTextBuy;                 //点价列表买字体颜色
    HBRUSH          m_BrushTextSell;                //点价列表sell字体颜色
    HBRUSH          m_BrushTextNormal;              //普通字体画刷
    HBRUSH          m_BrushBuy;                     //buy
    HBRUSH          m_BrushSell;                    //sell
    HBRUSH          m_BrushBgBuy;                   //buy背景色
    HBRUSH          m_BrushBgSell;                  //sell背景色
    HBRUSH          m_BrushHighLow;                 //最高价、最低价颜色
    HBRUSH          m_BrushLine;                    //表格线的颜色
    HBRUSH          m_BrushHoverFrame;              //hover边框
    HBRUSH          m_BrushLastPrice;               //最新价颜色
    HBRUSH          m_BrushBgMid;                   //表头中间区域背景色
    HBRUSH          m_BrushBlack;                   //black
    HBRUSH          m_BrushWhite;                   //white
    HBRUSH          m_BrushBgAsk1;                  //卖1背景
    HBRUSH          m_BrushBgBid1;                  //买1背景
    HBRUSH          m_BrushBgAsk;                   //卖区间背景
    HBRUSH          m_BrushBgBid;                   //买区间背景
    HBRUSH          m_BrushBgSpecial;               //特殊区域背景
    HBRUSH          m_BrushSelect;                  //当前选中背景色

	HDC				m_HdcMem;						//缓存DC
	HBITMAP			m_hBitmap;						//图布

	bool			m_bTracking;					//鼠标跟踪

    bool            m_bRightCancel;                 //右键撤单

	CxEditEx		m_EditExPoint;					//合约编辑框
private:
	DataPriceID		m_DataAxis[DPT_Size];			//坐标

	RECT			m_ItemRect[DGC_Size][N_Scale * 2 + 1];	//列表3纵列 单元坐标

	ItemSpecial		m_ItemRectSpecial[DSI_Size];	//特殊按钮

	ItemRowCol		m_SpecialItem[DS_Size];		//高亮按钮 （新旧）

	int				m_nLine;						//行数

	int				m_nListHeight;					//高度 25整数倍

	RECT			m_HeadRect;						//列表头区域
	RECT			m_ListRect;						//买卖列表区域
	RECT			m_FooterRect;					//列表撤单区域

	int				m_nScrollOffset;				//滚动条偏移

	ItemClick		m_ItemClickL;					//左键点击信息
	ItemClick		m_ItemClickR;					//右键点击信息

	int				m_BaseMidType;					//居中类型

	int				m_nStopType;					//止损类型

	bool			m_bInner;						//内外盘标记

	CPopWinList		m_PopWinList[PWT_Size];			//弹出窗口

	TMouPriceTypeStrW	m_PopWinMidStr[2][BMT_Size];	//居中字符串

	TMouPriceTypeStrW	m_PopWinStopStr[2][Tst_Size];	//止损字符串
private:
	map<int, DataQuoteQty> m_WorkData;				//行情挂单

	map<int, DataLocalWorkQty> m_LocalWorkData;		//客户挂单

	list<DataPriceID>			m_listLastPriceId;	//最新价缓存
	double						m_OldLast;			//前次最新价
	int							m_LastUpdown;		//涨跌标识

	SOFCommodityInf	m_CommodityInfo;				//品种信息

	SContractNoType m_sQuoteCode;					//行情合约

	string			m_UserNo;						//账号

	string			m_Sign;							//服务器标识

	map<int, const TOrder*>	m_ParOrderInfo;			//挂单信息

	int				m_nCanCancelQty[N_Sell + 1];	//挂单量

	int				m_OrderQty;						//数量
private:
	int  DivResult(const double Value, const double SubValue, const double ErrorDiff = 1e-8);
};


#endif // !1
