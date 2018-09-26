#pragma once
enum ControlID
{
	ID_Begin,
	//合约
	ID_Static_Contract,
	ID_Edit_Contract,
	ID_Button_Contract,
	//合同
	ID_Static_Agree,
	ID_Combox_Agree,
	//数量
	ID_Static_Qty,
	ID_Edit_Qty,
	ID_Spin_Qty,
	ID_Static_Lot,
	//价格
	ID_Static_Price,
	ID_Combox_Price,
	ID_Static_Plus,
	ID_Edit_Price,
	ID_Edit_Dot,
	ID_Static_Dot,
	//提示
	ID_Static_Tips,
	//买卖一价
	ID_Static_SellPrice,
	ID_Static_BuyPrice,
	//成交委托回报
	ID_Static_MatchTips,

	ID_End
};

class SendOrderWnd : public TIxFrm, public IOperator, public ISQuoteDataSpi, public TradeSpiEx
{
public:
	SendOrderWnd();
	~SendOrderWnd();
	HANDLE										m_hQryMatch;
public:
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	HWND GetQtyHwnd();
	void JumpOrderWnd(string agreement, string contractno, int qty);
	CxCombox		m_cbAgree;
public:
public:
	//////////////////////////////////////////////////////////////////////////ITradeSpi 交易回调
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin);//断线
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);	//交易初始化完成
	virtual void __cdecl OnMatch(const TMatch * pMatch);											//成交回报 刷持仓（金仕达 持仓全刷 不能监控持仓）
	virtual void __cdecl OnOrder(const TOrder * pOrder);											//委托回报

	//////////////////////////////////////////////////////////////////////////行情回调 	
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);				//普通行情数据	
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {};								//套利行情数据
protected:
	virtual void	SetContractNo(string sCode);
	virtual bool	SendOrder(MsgOrderInfo *msg);										//设置订单信息 发单
	virtual bool	TableButtonToEdit();												//Tab切换
private:
	//////////////////////////////////////////////////////////////////////////////
	HINSTANCE			m_hInstance;

	//////////////////////////////////////////////////////////////////////////////
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//窗口处理过程
	void OnCreate();
	void OnPaint();
	void OnSize();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(const int x, const int y);
	void OnLButtonUp(const int x, const int y);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
private:
	void OnSearchCode();
	void OnDealContractNo(WPARAM wParam, LPARAM lParam);
	void OnDealQty(WPARAM wParam, LPARAM lParam);
	void OnDealSwitch(WPARAM wParam, LPARAM lParam);
	void OnDealPrice();
	void OnDealAgree();
	void OnBtnPrice(int nID);
	void OnDealQuote();
	void OnKillFocusCode();								//合约代码失去焦点
public:
	map<int, WNDPROC>		m_SetEdit;					//子类化窗口过程
private:
	SystemFrame * m_System;
	//合约
	TStatic			m_labContract;
	CxEdit			m_editContract;
	TStaticButton   m_btnContract;
	//合同
	TStatic			m_labAgree;

	//数量
	TStatic			m_labQty;
	CxSpinEdit		m_spineditQty;
	TStatic			m_labLot;

	//价格
	TStatic		m_labPrice;
	CxCombox	m_cbPrice;
	TStatic		m_labPlus;
	CxEdit		m_editPrice;
	CxEdit		m_editDot;
	TStatic		m_labDot;
	//提示
	TStatic		m_labTips;
	//买卖一价
	TStatic		m_labSellPrice;
	TStatic		m_labBuyPrice;
	//成交委托回报
	TStatic		m_labMatchTips;

	CPopWinCode     m_popCode;
	//TAgreementPopWnd m_popAgree;

	//下单按钮
	COrderButton		m_cOrderBtn;	


	string				m_curTcode;		//当前交易合约
	string				m_curScode;
	SOFCommodityInf		m_CommodityInfo;	//品种信息

	//已成交数量合计
	int					m_finishedMatchQty;
	//当前合同
	SmsAgreementDataRsp	m_curAgree;
	//每手乘数
	double				m_tradeDots;
	//当前合同可下最大手数
	int					m_canSendLot;
	//当前OrderID
	int					m_curOrderID;
};

