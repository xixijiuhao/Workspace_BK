#ifndef _BTN_ORDER_WIN_
#define _BTN_ORDER_WIN_

#define N_TEXT_LEN 21
#define N_LONGTEXT_LEN	51

typedef int OrderButtonType;
#define	obInner		0
#define obForeign	1
#define	obInnerSwap	2
#define	obInnerOffer	3

typedef int OrderButtonCount;
#define	obc_2		2
#define obc_3		3
#define obc_4		4

enum OrderButtonDirectP //买 平空 平多 卖
{
	OBP_Buy,
	OBP_CoverS,
	OBP_CoverL,
	OBP_Sell
};
enum OrderButtonDirect //买 平 卖
{
	OBD_Buy,
	OBD_Cover,
	OBD_Sell
};
enum OrderButtonDirectF	//买 卖
{
	OBD_BuyF,
	OBD_SellF
};
enum OrderButtonItem	//价格 方向
{
	OB_Price,
	OB_Direct
};
enum OrderShowInfo //数量 保证金 币种
{
	OSI_Qty,
	OSI_Money,
	OSI_Currency
};
enum ItemText			//
{
	IT_NoOffset,
	IT_NoPosition,
	IT_HasPosition,
	IT_HasMatchPosition,
	IT_Offer,
	IT_Offer2,
	IT_Price,
    IT_ShiftPosition,
    IT_ShiftPosition2,
};
enum CanOpenCover	//可开 可平
{
	COC_OpenB,		
	COC_CoverB,
	COC_OpenS,
	COC_CoverS,
};
enum CanOpenCoverF	//外盘 对冲建仓 冻结 
{
	COC_QtyB,
	COC_MoneyB,
	COC_CurrencyB,
	COC_QtyS,
	COC_MoneyS,
	COC_CurrencyS,
};
struct ItemInfo
{
	RECT	rItem;
	RECT	rTextUp[OB_Direct+1];
	RECT	rTextDown[OB_Direct+1];
	bool	bInRect;
	int		nIt;
	wchar_t wContent[IT_ShiftPosition2 + 1][N_LONGTEXT_LEN];
	ItemInfo()
	{
		memset(this, 0, sizeof(ItemInfo));
	}
};
struct ItemQty 
{
	RECT	rItem;
	wchar_t wContent[N_LONGTEXT_LEN];
	ItemQty()
	{
		memset(this, 0, sizeof(ItemQty));
	}
};
struct PositionInfo 
{
	OrderButtonType		type;
	OrderButtonCount	nButton;
	int					nLong;
	int					nShort;
	int					nEditQty;
};
class COrderButton :public TIxFrm
{
public:
	COrderButton();
	~COrderButton();

	void CreateWin(IOperator* opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0, int nleft = 0, int ntop = 0, COLORREF rbuy = RGB(201, 51, 39), COLORREF rsell = RGB(26, 129, 237));					//创建弹出窗口
	
	void UpdateData(const PositionInfo& info);

	void UpdateCanOpen(int nBuy, int nSell);

	void UpdatePrice(wchar_t *wBuy, wchar_t *wSell);

	void UpdateEditQty(int nQty);

	void UpdateForeignQty(wchar_t* wcon,TDirect direct);

	void UpdateForeignPre(wchar_t* wMoney, wchar_t* wCurrency, TDirect direct);

	void OnSetFocus(int nID = 0);

	void OnSetFocus(char cDirect, char cOffset);
	
	void ButtonSendOrder(MsgOrderInfo &Msg);

    void SetBShitPosition(bool bShiftPosition = false);
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OnCreate(WPARAM wParam, LPARAM lParam);

	void OnCommond(WPARAM wParam, LPARAM lParam);

	void OnDestory(WPARAM wParam, LPARAM lParam);

	void OnPaint(WPARAM wParam, LPARAM lParam);

	LONG OnCtlColorStatic(WPARAM wParam, LPARAM lParam);

	void OnLButtonDown(WPARAM wParam, LPARAM lParam);

	void OnLButtonUp(WPARAM wParam, LPARAM lParam);

	void OnMouseMove(WPARAM wParam, LPARAM lParam);

	void OnMouseLeave(WPARAM wParam, LPARAM lParam);

	void OnKillFocus(WPARAM wParam, LPARAM lParam);

	void OnKeyDown(WPARAM wParam, LPARAM lParam);

	void paint_obc_2();		//两键重绘

	void paint_obc_3();		//三键重绘

	void paint_obc_4();		//四键重绘

	bool IsInnerType();		//内盘

	bool IsInnerSwap() { return m_PosInfo.type == obInnerSwap; };		//互换

	bool IsInnerOffer() { return m_PosInfo.type == obInnerOffer; };		//应价
private:
	IOperator		*m_opr;								//操作
	COLORREF		m_ColorBuy;							//买入色
	COLORREF		m_ColorSell;						//卖出色
	ItemInfo		m_InnerItemP[OBP_Sell + 1];			//4键坐标

	ItemInfo		m_InnerItem[OBD_Sell + 1];			//3键坐标
	ItemQty			m_CanOpenItem[COC_CoverS + 1];		//内盘可开可平

	ItemInfo		m_ForeignItem[OBD_SellF + 1];		//2键坐标
	ItemQty			m_CanOpenItemF[COC_CurrencyS + 1];	//外盘对冲建仓

	HDC				m_HdcMem;						//缓存DC
	HBITMAP			m_hBitmap;						//图布
	HBRUSH			m_BKBrush;						//背景画刷
	HBRUSH			m_BuyBrush;						//买入画刷
	HBRUSH			m_SellBrush;					//卖出画刷

	bool			m_bTracking;					//鼠标跟踪
	
	PositionInfo	m_PosInfo;						//持仓信息
private:
	void			UpdateLayout();					//更新坐标信息

	void			OnClickItemInner(int item);		//内盘三键
	void			OnClickItemInnerP(int item);	//内盘四键
	void			OnClickItemForeign(int item);	//外盘两键

	//及时更新
	int				m_nInnerClickIndex;				//内盘点击索引
	int				m_nForeignClickIndex;			//外盘点击索引
	bool			m_bClickDown;					//左键按下
	int				m_nFocusId;						//焦点ID

    bool            m_bShiftPosition;               //是否移仓
};


#endif // !1
