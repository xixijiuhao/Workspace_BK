#pragma once

#define BTNBORDERHEIGHT				8
#define BTNBORDERWIDTH				10
//编辑框中字符串能算出来宽度，但要填充到编辑框中还是要多留出点空隙
#define	EDITBORDERWIDTH				8	
#define EDITBORDERHEIGHT			8
#define WM_QUOTCHG					WM_USER+100
#define WM_OFHOTKEY					WM_USER+101
#define WM_BATCHORDER				WM_USER+102
#define WM_POSITIONSHOW				WM_USER+103
#define WM_CONDITION				WM_USER+104
#define WM_DEALTRADABLEQTY			WM_USER+105
#define WM_DEALPREFREEZE			WM_USER+106
#define WM_DEALQUOTECOMMON			WM_USER+107
#define WM_DEALPOSCOMMON			WM_USER+108
#define WM_DEALORDERCOMMON			WM_USER+109
#define	WM_DEALKLINECONTRACT		WM_USER+110

//横向下单热键定义
#define Vk_Condition	'T'
#define Vk_CloseOpen	'L'
#define Vk_Fast			'K'
#define Vk_BatchTimes	'B'
#define Vk_BatchQty		'U'
#define Vk_OK			'Y'
#define Vk_PreOrder		'P'
//买卖
#define KeyDirectBuy	'1'
#define KeyDirectSell	'3'
//开平
#define KeyOffsetOpen	'1'
#define KeyOffsetCover	'2'
#define KeyOffsetCoverT	'3'
//定单类型
#define KeyLimit		'1'
#define KeyMarket		'2'
#define KeyLimitStop	'3'
#define KeyStop			'4'
#define KeyIceberg		'5'
#define KeyGhost		'6'
//条件
#define KeyGreater		'1'
#define KeyLesser		'2'
//触发方式
#define KeyTrigLast		'1'
#define KeyTrigBuy		'2'
#define KeyTrigSell		'3'
//按钮状态
#define SwitchLevel1	0
#define SwitchLevel2	1

typedef char FPriceType;
#define fptOrder	'O'
#define fptTrig		'T'

#define  _KeyBoardOrder				"KeyBoardOrder"
//////////////////////////////////////////////////////////////////////////通用消息
#define  W_CSTYLE_MODIFY			"ORDER_MODIFY"			//订单修改
#define  W_CSTYLE_MODIFY_BATCH		"ORDER_MODIFY_BATCH"	//批量修改
#define  W_CSTYLE0					"LBUTTON_CLICK"    
#define  W_CSTYLE1					"LBUTTON_DBCLICK"  
#define  W_BN_CLICK					"Bn_Click"//"BN_CLICK"
#define  W_BN_DBCLICK				"Bn_DbClick"//"BN_DBCLICK"
#define	 W_USER_CHANGED				"UserNo_Update"	//账号联动
#define  W_USER_LOGIN				"TradeLogin"	//账号登录
#define  W_KLINE_CHANGED			"KLINE_SETCONTRACT"	//k线切换

#define	 WM_MODIFY_RECOVER			"MODIFYRECOVER"	//改单界面恢复

#define  W_SETCONTRACT				"SetContract"
#define	 W_LButtonDown				"LButtonDown"
#define	 W_LButtonDbClk				"LButtonDbClk"
#define	 W_VK_UP					"VK_UP"
#define	 W_VK_DOWN					"VK_DOWN"

#define  W_SRC_GRID					"grid"
#define  W_SRC_PANEL				"panel"
#define  W_SRC_KLINE				"kline"

#define	 ConditionCheck				"ConditionCheck"
#define	 BackHandCheck				"BackHandCheck"
#define	 FastCheck					"FastCheck"
#define	 BatchCountCheck			"BatchCountCheck"
#define  AddOne						"AddOne"

struct WinProperty
{
	bool bConditionCheck;
	bool bBackHandCheck;
	bool bFastCheck;
	bool bBatchCountCheck;
	WinProperty()
	{
		memset(this, 0, sizeof(WinProperty));
	}
};
enum ID_QUOTE_INFO
{
	ID_CODE,
	ID_FIELD,
	ID_SRC,
	ID_LEVEL,
};

#define KPRICELENGTH_LIMIT	10
//价格类型 根据此判断是取行情的排队买价 还是排队卖价
enum ePriceType
{
	BUYUSEBUY = 1,//买价买入
	BUYUSESELL, //卖价买入
	SELLUSEBUY,	//买价卖出
	SELLUSESELL	//卖价卖出
};
//控件类型
enum eCtrlType
{
	EDIT_TYPE,
	CHECK_TYPE,
	COMBO_TYPE,
	BTN_TYPE,
	STATIC1_TYPE,
	STATIC2_TYPE
};

class COFInputDlg :public IxDlg, public TradeSpiEx, public ISQuoteDataSpi, public IOperator, public ICommonInteract,public IKeyOrderCfg, public IStrategySpi,public ICommonInteractFast
{
public:
	COFInputDlg(HWND hPrent,WinProperty wProperty);
	~COFInputDlg();
	//创建非模态对话框
	HWND ShowDlg(HINSTANCE hInstance, int idDlg, HWND hWndParent);
	//初始化
	void Init();
public:
	//IQuoteSpi
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont){};

	//TraderSpi
	virtual void __cdecl OnOrder(const TOrder * pOrder);
	virtual void __cdecl OnDisconnect(const int ErrorCode, const TLogin * pLogin);
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);
	virtual void __cdecl OnPosition(const TPosition * pPosition);						//持仓变化通知
	virtual void __cdecl OnLogin(const int ErrorCode, const TLogin * pLogin);
	virtual void __cdecl OnPreFrozen(const TPreFreeze * pPreFreeze);
	virtual void __cdecl OnTradableQty(const TTradableQty * pTradableQty);

	//ICommonCfg
	virtual void __cdecl OnCommonCfgChanged(CM_SUBID PartID, char* cdata);
	virtual bool __cdecl GetFastInfo(sFastInfo& sInfo);
	virtual void __cdecl OnFastError(const ErrorReflectInfo* eInfo);
	virtual void __cdecl OnFastOrderInfo(const TSendOrder* TSOrder, double dRatio);
	virtual void __cdecl OnFastStateInfo(int nstate);
	//Ioperator
	virtual void SetPriceType(wstring sPType){};
	virtual void SetValidType(wstring sVData){};
	virtual void SetMaxMinQty(string sQty){};
	virtual void SendOrder(MsgOrderInfo *msg){};
	virtual void SetContractNo(string sCode, bool brepaint = true, bool bnutton = false, bool bgetfreeze = false);
	virtual void SetPriceData(string sdata){};
	//IKeyBoardCfg
	virtual void __cdecl OnKeyBoardCfgChange(const C_KeyBoardOrderCfg* kbcfg);
	virtual void __cdecl OnKeyBoardCfgFontChange(const C_KeyBoardFontInfoCfg* kbcfg);
	virtual void __cdecl OnKeyBoardSeqChange(const C_KeyBoardSeqCfg* kbcfg);
	//IStrategySpi
	virtual void __cdecl OnStrategyNotice(const TStrategyOrder* pdata) {};
	virtual void __cdecl OnPositionNotice(const TPosition* pdata) {};
	virtual void __cdecl OnStrategyWorkState(const StrategyWorkState state) {};
	virtual void __cdecl OnStopInfoToPosition(const TPosition* p) {};
	virtual void __cdecl OnStopInfoToOrder() {};
	virtual void __cdecl OnStopInfoUpdate(bool bShwoUpdate) {};
	virtual void __cdecl OnSpreadInfoNotice(const TSpreadOrder* pspread){};
	virtual void __cdecl OnAssistInfoRef(uint OrderIdOld, uint OrderIdNew);
	//配置更新回调
	virtual void __cdecl OnStrategyParamModify(ST_PARAM type) {};
	//
	void SetHotKeyActive(bool bActive);
	bool GetHotKeyActive(){ return  m_bHotKeyAltActive; };

	//
	virtual void __cdecl SetCodeFromQuote(const char * sKey, const char* cType);	//双击 单击 行情填单
	virtual void __cdecl SetCodeFromPosTotal(const char * sKey, const char* cType);						//点击持仓合计
	virtual void __cdecl SetCodeFromPosSub(const char * sKey, const char* cType);						//点击持仓明细
	virtual void __cdecl SetCodeFromOrder(const char *sOrderID, const char* cType);						//双击 单击 委托填单	

	void ModifyOrderOper(const TOrder *pOrder, bool bBatch = false);//改单
	void OnDealModifyRecover();										//改单恢复
private:
	//处理消息
	INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OFSendOrder(TSendOrder* order);
private:
	//是否初始化完成
	bool m_nInitReady;
	//是否激活界面快捷键
	bool m_bHotKeyAltActive;
	//倍率上次的值
	string m_LastBLStr;	
	//数量上次的值
	string m_LastVolStr;						
	//刷子
	HBRUSH m_ctlBrush;
	//快速背景刷子
	HBRUSH m_ctlBrushKS;
	//背景刷子
	HBRUSH m_BkBrushDlg;
	//初始化静态空间文本垂直居中
	void IniStaticTextVer();
	//设置spin
	void SetNumSpin(int ID, int IDSpin, bool bCanNat);
	//初始化对话框
	BOOL OnInitDialog();
	//处理命令消息
	BOOL OnCommandCOFInputDlg(WPARAM wParam, LPARAM lParam);
	//处理通告消息
	BOOL OnNotifyCOFInputDlg(WPARAM wParam, LPARAM lParam);
	//处理条件check
	void OnBnClickedCheckTJ();
	//处理平开check
	void OnBnClickedCheckPK();
	//处理快速check
	void OnBnClickedCheckKS();
	//处理批次check
	void OnBnClickedCheckBatchCount();
	//处理批量check
	void OnBnClickedCheckBatchOrder();
	//处理T1check
	void OnBnClickedCheckT1();
	//处理强平check
	void OnBnClickedCheckForCover();

	//zyp 新增处理投机check
	void OnBnClickedCheckSP();
	//zyp 新增处理保值check
	void OnBnClickedCheckHG();

	//处理ctrlcolor
	HBRUSH OnCtlColor(WPARAM wParam, LPARAM lParam);
	//处理确定按钮
	void OnBnClickedButtonDoOrder();
	//处理埋单按钮
	void OnBnClickedButtonPayOrder();
	//处理自动弹按钮
	void OnBnClickedButtonAutoOrder();
	//处理顺序按钮
	void OnBnClickedButtonSeq();
	//处理买入按钮
	void OnBnClickedBtnBuy();
	//处理卖出按钮
	void OnBnClickedBtnSell();
	//处理点击静态控件方向
	void OnStnClickedStaticDirect();
	//处理点击静态控件开平
	void OnStnClickedStaticOffset();
	//买卖框焦点
	void OnEnSetfocusEditDirect();
	//开平焦点
	void OnEnSetfocusEditOffset();
	//设置组框条件
	void OnCbnSetfocusComboTirgMode();
	//委托价格焦点
	void OnEnSetfocusEditOrderPrice();
	//委托数量焦点
	void OnEnSetfocusEditOrderQty();
	//触发价格焦点
	void OnEnSetfocusEditTrigPrice();
	//触发条件焦点
	void OnEnSetfocusEditTrigType();
	//数量倍率焦点
	void OnEnSetfocusEditQtyMulti();
	//账户组框选择
	void OnCbnSelChangeUserNo();
	//定单类型组框选择
	void OnCbnSelchangeComboOrdertype();
	//设置定单类型组框选择
	void OnCbnSetfocusComboOrderType();
	//有效类型组框选择
	void OnCbnSelchangeComboValidtype();
	//合约代码内容改变
	void OnEnChangeEditCode();
	//合约代码输入完毕失去焦点
	void OnEnKillFocusEditCode();
	//买卖方向内容改变
	void OnEnChangeEditDirect();
	//委托价格spin
	void OnDeltaposSpinOrderPrice(int nDelta);
	//委托数量spin
	void OnDeltaposSpinOrderQty(int nDelta);
	//数量倍率spin
	void OnDeltaposSpinQtyMulti(int nDelta);
	//触发价格spin
	void OnDeltaposSpinTrigPrice(int nDelta);
	//下单次数spin
	void OnDeltaposSpinQtycount(int nDelta);
	//数量增加spin
	void OnDeltaposSpinIncreqty(int nDelta);
	//价格增量spin
	void OnDeltaposSpinIncreprice(int nDelta);
	//最大量spin
	void OnDeltaposSpinMaxqty(int nDelta);
	//最小量spin
	void OnDeltaposSpinMinqty(int nDelta);
	//账户edit更改
	void OnCbnEditchangeComuserno();
	//倍率更新
	void OnEnUpdateEditQtyMulti();
	//数量更新
	void OnEnUpdateEditOrderQty();
	//价格增量更新
	void OnEnUpdateEditIncreprice();
	//下单次数增量
	void OnEnUpdateEditIncreqty();
	//选合约按钮
	void OnBnClickedBtnSelcode();
	//跟盘价格
	void OnBnClickedCheckAutoprice();
	//滚轮消息 编辑框（价格 数量 合约代码列表）
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	//热键响应
	void OnDealHotKey(WPARAM wParam, LPARAM lParam);
	//批量配置修改
	void OnBatchOrderInfoCHG();
private:
	//保存与合约相关的信息
	SOFCommodityInf	m_CommodityInf;	
	//填单信息
	C_FillOrderInfo m_FillOrderInfo;
private:
	//账户数量
	vector<CStringsVec> m_vecUserNo;
	//账户控件
	CMultiColComboBox m_UserNO;
	//按钮字体
	HFONT m_fontBtn;
	//check字体
	HFONT m_fontCheck;
	//静态控件1字体
	HFONT m_fontStatic1;
	//静态控件2字体
	HFONT m_fontStatic2;
	//组框字体
	HFONT m_fontCombo;
	//edit字体
	HFONT m_fontEdit;
	//持仓显示字体
	HFONT m_fontPosition;
private:
	typedef vector<FacadeSetting::OrderField> vecOrderField;
	//输入区域的配置
	vector<C_OrderField> m_InputCfgArray;
	//是否能够layout
	bool m_bCanLayout;
	//Btn原始宽度
	int	m_iOrginBtnWidth;	
	//Btn内原始字体的宽度
	int	m_iOrginBtnFontWidth;
	//合约代码编辑框长度字符串
	string	m_CodeLengthStr;					
	//获取下单按钮原始大小
	void SetOrginBtnWidth();
	//设置可见
	void SetItemVisible(int id, BOOL bShow);
	//读取进来列表的配置信息
	void ReadFieldConfig();		
	//根据字体计算控件大小
	bool UseFontSetCtrlSize(eCtrlType eType, HFONT &font, SIZE & sz, wstring& str);
	//设置编辑框参数
	bool SetEditCtrlParam(FutureInputArea eInputArea, wstring str, int iEditID, SIZE &sz, bool bExtraWidth = true);		
	//设置组框框参数
	bool SetComboCtrlParam(FutureInputArea eInputArea, wstring str, int iComboID, SIZE &sz);		
	//针对静态文本
	bool SetStaticCtrlParam(HFONT &font, wstring str, int iStaticID, SIZE &sz, int *pDefaultMaxWidht = NULL, bool bExtraWidth = true);
	//针对按钮 按钮大小需要不同,类型0是确定 1是其他
	bool SetBtnCtrlParam(wstring str, int iBtnID, SIZE &sz, int *pwidth = NULL);							
	//设置check框参数
	bool SetCheckCtrlParam(FutureInputArea eInputArea, wstring str, int iCheckID, SIZE &sz);
	bool SetCheckCtrlParam2(wstring str, int iCheckID, SIZE &sz);
	//设置日期控件参数
	bool SetDateTimeCtrlParam();
	//修改控件宽度
	void ModifyCtrlWidth(int id, int iNewWidth);
	//设置字体
	bool SetCtrlFont();
	//设置控制窗口layout
	inline void SetCanLayout(bool bCan) { m_bCanLayout = bCan; }
	//获取控制窗口layout
	inline bool GetCanLayout()			 { return m_bCanLayout; }
	//check布局
	void CheckLayout(int id, bool bShow, int height, RECT &ctrlRect);
	//条件区布局
	void TJCheck(C_OrderField &of, RECT &ctrlRect);
	//更新状态
	void UpdateItemStates();
	//移动静态文本
	void MoveStaticCtrl(bool bShow, int idStatic, RECT &ctrlRect);
	//移动控件
	void MoveCtrl(bool bShow, int id, int iVerInterval1, int height, RECT &ctrlRect, HWND &LastWnd);
	//移动spin
	void MoveSpinCtrl(bool bShow, int idspin,int idStatic, const RECT &InputRect, RECT &ctrlRect, int &iExtraSpinWidht);
	//移动分数报价部分
	void MoveFractionCtrl(bool bShow, int idStatic, int idFraction, int interval, int iVerInterval1, int height, RECT &InputRect, RECT &ctrlRect, HWND &LastWnd);
	//移动按钮、持仓 布局
	void MoveBtnCtrl(RECT &rectBtn, RECT &InputRect, int interval, HWND&LastWnd, int iStateBarLeft,int &iSeqBtnLeft);
	//移动状态提示
	void MoveStatusTip(int iStateBarLeft, int iTempHeight, int iVerInterval2, int iDepoRight);
	//移动持仓
	void MovePosCtrl(int interval, RECT &ctrlRect);
	//布局
	int  Layout();
	//获取控件高度
	int  FindCtrlHeight(int id);
	//获取对话框大小
	SIZE GetKbDlgSize();
	//转化屏幕坐标
	void ScreenToClient(RECT&rt);
	//重绘子窗口
	void ReInvalidateCtrl();
	//修改按钮名字
	void ModifyBtnName();
	//修改静态文本内容
	void ModifyStaticUserName();
	//改单控制ctrl可用
	void EnableModifyOrder(bool bModify);
	//T1
	void EnableT1(const char *szCode);
	//初始化跟盘价check
	void IniAutoPriceCheck();
private:
	//版本类型 0内盘  1外盘 2内外盘
	int	m_iVersionType;
	//判断是内外盘类型
	void LoadVersionType();
private:
	//获取check是否选中
	bool IsCheck(int check);
	//是否单客户
	inline bool IsOnlyOneUser()	{ return m_vecUserNo.size() <= 1; }
	//是否显示买卖按钮
	inline bool IsShowBuySellBtn() { return m_KeyBoardSet.uiDoOrderStyle == 1; }
	//是否显示 快速
	inline bool IsShowKS() { return !CConvert::IsTraderLogined(); }
	//是否显示 条件
	inline bool IsShowTJ() { return  (0 == VersionType() || 2 == VersionType()); }
	//是否显示 平开
	inline bool IsShowPK(){ return  (0 == VersionType() || 2 == VersionType()); }
	//是否显示 批次
	inline bool IsShowPC(){ return !CConvert::IsTraderLogined(); }
	//是否显示 批量
	inline bool IsShowPL(){ return (!CConvert::IsTraderLogined() && m_vecUserNo.size() > 1); }
	//是否显示 T+1
	inline bool IsShowT1(){ return /*GetTraderLoginCount() > 0 &&*/ (1 == VersionType() || 2 == VersionType()); }
	//是否显示 强平
	inline bool IsShowForceCover(){ return CConvert::IsTraderLogined(); }
	//0表示不显示 1表示显示
	inline int  IsShowComboType() { return (VersionType() == 1 || VersionType() == 2) || m_KeyBoardSet.bShowOrderValidType; }
	//是否显示 触发价格
	bool IsShowTrigPrice();
	//是否显示顺序 埋单 自动单按钮 根据配置和xml文件 与ordercfg.ini 和配置文件 相关  优先级ordercfg.ini较高级
	inline bool IsShowBtn(){ return (VersionType() == 0 || VersionType() == 2) && 2==m_KeyBoardSet.uiDoOrderStyle; }

	//zyp 新增套利保值
	inline bool IsShowSPAndHG() { return !CConvert::IsTraderLogined(); }
	inline bool IsShowHG() { return !CConvert::IsTraderLogined(); }

public:
	//获取快速状态
	inline bool IsKS()	{ return IsCheck(IDC_CHECK_KS); }
	//处理上下键、左右键等于tab键
	bool DoLeftRightUpDownKey(char key,bool Combo=false);
	//处理ener
	BOOL DisposeEnterMsg(DWORD key);
	//处理enter发单
	bool DealEnterSendOrder(DWORD key);
private:
	//0表示内盘 1表示外盘 2=内外盘均有
	inline int  VersionType(){ return m_iVersionType; }
public:
	//滚轮滚动
	BOOL DoWndWheel(int nID, BOOL bUP);
	//双击
	BOOL DoWndDbClick(HWND hWnd, LPARAM lparam);
	//按键
	BOOL DoWndKeyDown(HWND hWnd, WPARAM wg);
private:
	//设置分数报价和普通报价滚轮滚动增减
	void SetSpinCtrlBuddy(int IDSpin, int ID, int IDFenZi, int IDFenMu);
	//设置价格框的自动选中
	void SetPriceSel();			
	//格式化double
	string FormatDoubleValue(int precision, double value, BOOL bZeroEmpty);
	//格式化整形
	string FormatIntValue(int value, BOOL bZeroEmpty);
	//bDoubleType是否是带小数 像委托数量就不能带小数  数量倍率可以带小数
	void UpdateNumEdit(BOOL bUp, int ID, double iChangePoint, bool bCanNat, bool bDoubleType = false);
	//针对价格
	void SpinUpOrDown(int ID, int IDFenzi, BOOL bUp);
	//针对价格增量
	void SpinUpOrDown(int IDIncre, BOOL bUp);

private:
	//委托价格是否能用
	BOOL OrderPriceIsCanAble();
	//触发价格是否能用
	BOOL TriggerPriceIsCanAble();
	//委托价格 触发价格 enable 
	void EnablePrice(int ID, int IDAdd, int IDFenzi, int IDChu, int IDFenmu, int IDSpin,BOOL bEanble);
private:
	//买卖下单
	COrderBtn m_BuyBtn;
	COrderBtn m_SellBtn;
	//设置按钮风格 是只显示买入卖出 还是有价格
	void SetBuySellBtnStyle(bool bShowPrice);
public:
	struct ItemPos
	{
		//位置索引
		int iPosIndex;
		//索引控件ID
		int hID;
	public:
		ItemPos() :iPosIndex(-1), hID(-1){}
	};
	typedef vector<ItemPos> vecItemPos;
	//保存所有item
	vecItemPos m_vecItemPos;
	//初始化控件顺序和ID
	void InitItemPos();
	//设置索引焦点
	void SetIndexFocus(int index,bool btabling = false,bool bAdd = true);
	//获取上一个焦点位置索引
	int GetLastFocusIndex(int ID);
	//获取下一个焦点位置索引
	int GetNextFocusIndex(int ID);
public:
	//子类化
	void InitCtrl();
	//委托价格子类化
	CSubClassEdit m_OrderPriceEdit;	
	//委托价格子类化
	CxKeyBoardEdit m_OrderPriceEditFZ;
	//触发价格子类化
	CSubClassEdit m_TriggerPriceEdit;
	//触发价格子类化
	CxKeyBoardEdit m_TriggerPriceEditFZ;
	//合约代码子类化
	CxKeyBoardEdit m_CodeEdit;
	//买卖方向子类化
	CxKeyBoardEdit m_DirectEdit;
	//开平方向子类化
	CxKeyBoardEdit m_OffsetEdit;
	//委托数量
	CxKeyBoardEdit m_OrderQtyEdit;
	//数量倍率
	CxKeyBoardEdit m_QtyMultiEdit;
	//条件
	CxKeyBoardEdit m_TrigCondiEdit;
	//最大量
	CxKeyBoardEdit m_MaxQtyEdit;
	//最小量
	CxKeyBoardEdit m_MinQtyEdit;
	//下单次数
	CxKeyBoardEdit m_QtyCountEdit;
	//数量增量
	CxKeyBoardEdit m_IncreQtyEdit;
	//价格增量
	CxKeyBoardEdit m_IncrePriceEdit;
	//下单按钮
	CxKeyBoardButton m_OrderButton;
	//埋单按钮
	CxKeyBoardButton m_PayOrderButton;
	//自动弹按钮
	CxKeyBoardButton m_AutoOrderButton;
	//顺序按钮
	CxKeyBoardButton m_SeqButton;
	//选合约按钮
	CxKeyBoardButton m_SeleCodeButton;
	//是价格edit吗
	bool IsPriceEdit(HWND hwnd);
private:
	//切换买卖方向
	BOOL SwitchDirect();
	//切换开平
	BOOL SwitchOffset();
	//切换条件
	BOOL SwitchTJ();
	//获取中间位置
	int  GetMiddlePos(int ID);
private:
	//修改控件属性
	void ModifyStyle(HWND hCtrl, DWORD dwRemove, DWORD dwAdd);
	//修改控件属性
	void ModifyCtrlStyle();
	//显示提示
	void ShowTip(const wstring &msg);
private:
	//条件单状态
	void UpdateTJState();
	//填充条件状态根据配置
	void FillTConditionByDirect();
	//条件单触发价格更新
	void UpdateTrigPriceFill(string sCode,char cDirect);
private:
	//当前选中的账户
	string m_CurUserNo;
	//当前登录账户sign 保存
	string m_CurLoginNo;
	//更新账户和sign
	void UpdateLoginSign(const char *szUserNo, const char *sign);
	//获取sign
	void SetLoginSign(char(&Sign)[21]);
	//获取sign
	const char * GetLoginSign();
private:
	//账户锁定
	CLockBtn m_LockUserNoBtn;
	//合约锁定
	CLockBtn m_LockCodeBtn;
	//处理锁定按钮
	void OnBnClickedBtnLockuserno();
	//处理锁定按钮
	void OnBnClickedBtnLockcode();
private:
	//处理行情回调
	LRESULT	OnQuotInfo(WPARAM wparam, LPARAM lparam);
	//bDefaultPrice为双击深度行情、快速下单、和双击买卖价、买卖量之外的行情所用所用 bDefaultPrice=true直接用 =false时并且direct==dNone时不用 但=false时有可能是双击买卖价、买卖量 此时应该根据是否有direct来决定价格
	//bDbClickQuot是双击行情还是双击交易列表
	void FillKeyBoardOrderDlg(const TSendOrder *pSendOrder, bool bFastOrder, bool bQuot, bool bTradeData, bool bModifyOrder);
	//是否LME
	//bool IsLME(SExchangeKey&ExchangeKey);
	//填充改单
	void FillModifyOrder(const TSendOrder &SendOrder);
	//设置价格到界面
	void SetPriceToDlg(int ID, int IDMolecules, int IDDenominator, double dInputPriceParma);
	//设置价格到买卖按钮上
	void SetPriceToBtn(int IDBtn, double dPrice);
	//清空价格
	void EmptyPrice(int ID, int IDMolecules);
	//有效的价格
	inline bool IsValidPrice(char commodityType, double price);
private:
	//订阅过得行情
	string m_regQuotId;
	//获取行情信息但并不订阅 取到就取到 取不到就算了
	bool GetQuotInfo(const char *szCode, SQuoteSnapShot&QuotInfo);
	//获取行情信息但并不订阅 取到就取到 取不到就算了 返回true表示获取成功 false 表示失败
	bool GetQuotInfo(const char *szCode, SQuoteSnapShot &, int iTypePrice, int nDefaultPriceOverStep, char Direct, double &dPrice, double dDefaultPrice = 0);
private:
	//触发方式
	CxComboBox m_TrigModeCmb;
	//定单类型
	CxComboBox m_OrderTypeCmb;
	//有效类型
	CxComboBox m_ValidTypeCmb;
	//设置定单类型组框
	void SetOrderTypeCmbAtrribute();
	//设置有效类型类型组框
	void SetValidTypeCmbAtrribute();
	//下单后取消强平
	void CancelForceCover();
	//下单后定单类型改为限价 前提是定单类型显示出来
	void ModifyOrderType(char OrderType);
	//下单后定单类型改为限价 前提是定单类型显示出来
	void ModifyValidType(const char *szTime);
	//下单后有效类型改为当日有效 前提是有效类型显示出来
	void ModifyValidType(char ValidType);
	//下单后取消T1
	void CancelT1();
private:
	//上一次填充触发价格的合约 只有当前的与此值不同才需要填充触发价格
	string m_strLastTriggerFillCode;
	//触发价格是否已经填充过了 若填充过了 得到焦点时就不需要再填充了
	bool m_bValidFillTrigger;
	//是否需要填充条件单
	bool IsNeedFillTrigger();
	//设置有效性
	void SetValidFillTrigger(bool b){ m_bValidFillTrigger = b; }
	//获取有效性
	bool GetValidFillTrigger(){ return m_bValidFillTrigger; }
	//设置合约
	void SetLastTriggerFillCode(const string & s){ m_strLastTriggerFillCode = s; }
	//获取合约
	string &GetLastTriggerFillCode(){ return m_strLastTriggerFillCode; }
	//更新
	void UpdateFillTriggerValid(const string &strCur);
private:
	//批量组号是否有效 也可以当m_vecUserNo是否有效
	bool m_bValidGroup;		
	//获取账户索引
	bool GetIndexUserNo(const vector<CStringsVec> &vecStr, const string &str, int &index);
	//填充账户组框
	void SetUser(bool bValidData = false);	
private:
	//显示代码
	CCodeNameStatic	m_StaticCodeName;
	//显示可开可平
	CCodeNameStatic	m_StaticCanOpenCover;
	//显示保证金占用
	CCodeNameStatic	m_StaticDeposit;
	//多头持仓 两种显示方式 根据账号显示或者根据合约显示持仓信息
	CDrawStatic	m_BuyPositionStatic;					
	//空头持仓 两种显示方式 根据账号显示或者根据合约显示持仓信息
	CDrawStatic	m_SellPositionStatic;					
	//检查合约有效性
	void CheckCodeInput();
	//iType=0代表显示合约持仓，1代表账户持仓
	void UpdatePositionInf();
	//注册
	bool RequestCommodity(const char *szCode);
	//更新品种信息
	void UpdateCommodityTick(const char *szCode);
	//更新交易数据并重新布局
	bool UpdateTradeLayOut();
private:
	//错误信息
	wstring g_FastError;
	//获取参数信息
	bool GetDlgCtrlInput(TSendOrder &SendOrder, bool bShowTip = false, bool bShowMsgBox = false, bool bFastOrder = false, char key = '\0', const char * ContractID = NULL);		
	//得到用户并判断有效性 合法返回ture  bShowTip是否显示提示信息 为ture时是弹出提示还是在界面上显示
	bool GetCtrlUser(char(&UserNo)[21], bool bShowTip = false, bool bShowMsgBox = false);		
	//SContractKey输出
	bool GetKeyCode(TContractKey &key, bool bShowTip = false, bool bShowMsgBox = false);	
	//获取定单类型
	char GetOrderType(char *pType = NULL);
	//获取定单类型 针对selchange
	char GetOrderTypeEx();
	//获取有效类型 
	char GetValidType();
	//获取有效类型 针对selchange
	char GetValidTypeEx();
	//获取有效日期
	string GetValidTime();
	//获取买卖方向
	bool GetDirect(char &Direct, bool bShowMsgBox = false, bool bSetFocus = true);
	//获取开平
	bool GetOffset(char &Offset, bool bShowMsgBox = false, bool bSetFoucus = true);
	//获取数量
	bool GetCtrlOrderNum(uint & Qty, bool bShowMsgBox = false, bool bSetFocus = true);
	//获取冰山单最大数量
	bool GetCtrlMaxNum(uint & Qty, bool bShowMsgBox = false, bool bSetFocus = true);
	//获取冰山单最小数量
	bool GetCtrlMinNum(uint & Qty, bool bShowMsgBox = false, bool bSetFocus = true);
	//获取所有值，默认的值都是字符串的 ?
	bool GetCtrlPrice(int id, int idMolecular, double &outPrice, bool bShowMsgBox = false, bool bSetFocus = true, FPriceType type = fptOrder);
	//获取条件单参数
	bool GetTJParam(TSendOrder &SendOrder, bool bShowTip = false, bool bShowMsgBox = false, bool bSetFocus = true);
	//获取批次次数
	bool GetPCQtyCount(uint &QtyCount, bool bShowMsgBox = false, bool bSetFocus = true);
	//获取批次数量增量
	bool GetPCQtyIncre(int &QtyIncre, bool bShowMsgBox = false, bool bSetFocus = true);
	//获取批次价格增量
	bool GetPCPriceIncre(double &PriceIncre, bool bShowMsgBox = false, bool bSetFocus = true);
private:
	//原始定单号
	int m_MaxOrdID;	
	//保存上次的可开可平 防止重复查询
	TTradableQty m_LastBuyQty;
	//卖出
	TTradableQty m_LastSellQty;
	//保存上次的占用保证金
	TPreFreeze m_LastBuyFreeze;	
	//卖出
	TPreFreeze m_LastSellFreeze;
	CMyLock					m_lock;
	TTradableQty			m_BuyTradableQtytemp;
	TTradableQty			m_SellTradableQtytemp;
	TPreFreeze				m_BuyPreFreezetemp;
	TPreFreeze				m_SellPreFreezetemp;
private:
	//处理预冻结
	void OnDealPreFreeze(char cDirect);		
	//处理可开可冲
	void OnDealTradableQty(char cDirect);				
	//获取最大ID 初始为0，在委托中更新
	inline int  GetMaxOrderID()  {return m_MaxOrdID; }
	//显示冻结保证金
	void ShowOrderFrozen(const TPreFreeze * pPreFreeze);
	//显示可开可平
	void ShowCanOrderVol(const TTradableQty * pTradableQty, bool bAutoCover = false);
	//清空提示框
	void ClearTipBar();
	//得到可用资金
	bool GetAvailableMoney(double &dAvailabe);
	//格式化保证金
	void FormatStaticDepositDisplay();
	//格式化可开可平
	void FormatStaticNumDisplay();
	//格式化合约
	void FormatStaticCodeDisplay();
	//可开可平、保证金占用是否还有效，是不是需要重新请求 ifpOrder->OrderQty有效为可开可平
	bool IsVaild(TSendOrder * pOrder, int AMaxOrdID, double AAvailable);
public:
	//热键回调
	void OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
	//转换
	bool TransKey(HWND hwnd, WPARAM wParam, LPARAM lParam);
	//获取倍率
	double GetMultiQty();
	//委托价格为0下市价单
	bool IsOrderTypeToMarket(const TSendOrder &SendOrder);
	//上期所
	inline bool IsSHFE(const char *szExchange);
	//修改上期所订单类型
	void ModifySHFEOrderTyep(TSendOrder &SendOrder);
	//CommitMode应该是为埋单和自动单使用的
	void DoOrder(char CommitMode, char Direct = dNone);	
	//下单之后操作 比如 取消强平 定单改为限价 有效类型等
	void RecoverState();
private:
	//是否改单
	bool m_bModifyOrderOper;
	//是否批量改单
	bool m_bModifyOrderOperBatch;
	//是否是改单
	inline bool GetModifyOrderOper() { return m_bModifyOrderOper; }
	//设置改单
	void SetModifyOrderOper(bool ModifyOrderOper){ m_bModifyOrderOper = ModifyOrderOper; }
	//改单结构
	TOrder m_ModifyOrder;
	//改单时布局前重置条件选择区域
	void ResetTJCheckBox(TBool IsRiskOrder, TTradeSection AddOneIsValid);
private:
	//spin子类化
	CxKeyBoardSpin m_OrderPriceSpin;
	CxKeyBoardSpin m_OrderQtySpin;
	CxKeyBoardSpin m_TrigPriceSpin;
	CxKeyBoardSpin m_MaxQtySpin;
	CxKeyBoardSpin m_MinQtySpin;
	CxKeyBoardSpin m_QtyMultiSpin;
	CxKeyBoardSpin m_QtyCountSpin;
	CxKeyBoardSpin m_IncreQtySpin;
	CxKeyBoardSpin m_IncrePriceSpin;
	CxDataCtrl m_ValidDataCtrl;
private:
	//check
	CxKeyBoardCheck m_CheckTJ;
	CxKeyBoardCheck m_CheckPK;
	CxKeyBoardCheck m_CheckKS;
	CxKeyBoardCheck m_CheckPC;
	CxKeyBoardCheck m_CheckPL;
	CxKeyBoardCheck m_CheckT1;
	CxKeyBoardCheck m_CheckQP;
	//zyp 新增 投机/保值
	CxKeyBoardCheck m_CheckHG;  //保值
	CxKeyBoardCheck m_CheckSP;  //投机

	CSwitchBtn	m_SwitchFollow;			//跟盘
private:
	CPopWinCode m_cCodeWin;				//主合约代码
private:
	//线程
	CThread m_thread;
public:
	//简化合约转化
	void GetContractStr(string &str);
	//修改通过双击行情持仓等方式填充code的内容 比如把ZCE SR 501转为SR1501||SR501
	void ModifyContractStr(string&str);
private:
	//通用
	void OnDealQuoteKLine();
	void OnDealQuoteCommon();
	void OnDealPositionOrderCommon();

	bool GetSignByUserNo(string sUser, int& nIndex);

	C_KeyBoardOrderCfg		m_KeyBoardSet;			//横向下单配置

	C_KeyBoardFontInfoCfg	m_OrderFont;			//字体配置

	C_KeyBoardSeqCfg		m_OrderFiled;			//界面控件顺序

	C_FillOrderAssistInfo	m_FillOrderAssist;		//填单配置 条件单配置

	C_OrderDealInfo			m_OrderDealInfo;		//下单处理

	C_OrderReturnInfo		m_OrderReturn;			//下单后处理 	

	C_FastOrderInfo			m_fastOrderInfo;		//数量倍率浮动点数

	map<string, BatchGroupInfo> m_batchGroupVec;	//批量信息

	void OnDealAssistFill(const C_FillOrderAssistInfo* pdata);
	void OnDealOrderDeal(const C_OrderDealInfo* pdata);
	void OnDealOrderReturn(const C_OrderReturnInfo* pdata);
	void OnDealFastOrderInfo(const C_FastOrderInfo* pdata);
	void OnDealBatchGroupInfo(const C_BatchGroupInfo* pdata);
private:
	//快速
	void SetFastControlState();

	void UpdateAltHotKeyShowState();

	bool TradeLoginLinkage();
private:
	//行情K线
	string m_sQuoteKline;												//K线合约
	virtual void __cdecl SetCodeFromQuoteKline(const char *sContent);	//行情K线切换
private:
	//鼠标table输入法控制
	bool m_bTabling;

	WinProperty		m_wProperty;

	void UpdatePropertyCfg();

	void UpdatePropertyControl();

	bool m_bLoginTrader;
private:
	//Add by xzc 2018.6.12下午
	//上下键切换合约
	std::vector<std::string> m_vecUpDownSwitchContract;
	//是否是下一条
	std::string GetOneSwitchContract(bool bNext);
	//响应上下键，如果上下键作为Tab用，那么就不切换合约
	bool OnKeyUpDown(bool isDown, HWND hFocus);
	//处理用户没有登录时的切换，GetContractStr可能转换不成功，AP810->ZEC AP 810
	void DealUserNotLoginSwitch(std::string & sCode);
	//end
};

 