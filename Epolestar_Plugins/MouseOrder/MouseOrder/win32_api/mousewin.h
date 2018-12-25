#ifndef _MOUSE_WIN_
#define _MOUSE_WIN_

//////////////////////////////////////////////////////////////////////////模块
#define  _MouseOrder				"MouseOrder"
#define  _CommonModule				"CommonModule"
//////////////////////////////////////////////////////////////////////////消息内容类型  
#define  W_CSTYLE_MODIFY			"ORDER_MODIFY"			//修改订单
#define  W_CSTYLE_MODIFY_BATCH		"ORDER_MODIFY_BATCH"	//批量修改
#define  W_BN_CLICK					"Bn_Click"				//单击
#define  W_BN_DBCLICK				"Bn_DbClick"			//双击
#define  W_HOLD_OPTION				"Hold_Option"			//持仓行权
#define  W_ON_MENUITEM              "On_MenuItem"

#define  W_SETCONTRACT				"SetContract"	//行情合约改变
#define	 W_LButtonDown				"LButtonDown"	//左键单击
#define	 W_LButtonDbClk				"LButtonDbClk"	//未使用
#define	 W_VK_UP					"VK_UP"			//向上切行情
#define	 W_VK_DOWN					"VK_DOWN"		//向下切行情

#define  W_SRC_GRID					"grid"			//行情报价
#define  W_SRC_PANEL				"panel"			//盘口
#define  W_SRC_KLINE				"kline"			//K线
//////////////////////////////////////////////////////////////////////////发出消息
#define  W_USER_CHANGED				"UserNo_Update"	//账号联动
#define  W_USER_LOGIN				"TradeLogin"	//账号登录
//////////////////////////////////////////////////////////////////////////通用消息
#define	 W_MODIFY_RECOVER			"MODIFYRECOVER"	//改单界面恢复
//////////////////////////////////////////////////////////////////////////
#define	 _FastCheck					"FastCheck"		//快速勾选
#define	 _AutoCover					"AutoCover"		//自动开平
#define	 _CoverMode					"CoverMode"		//平仓方式
#define	 _AddOne					"AddOne"		//港交所T+1
#define	 _ChaseOrder				"ChaseOrder"	//追单
#define	 _ExchangeSupport			"ExchangeSupport"	//交易所支持
#define	 _MouseHead					"MouseHead"		//列头
#define	 _ChildHead					"ChildHead"		//子项 竖向-点价 | 限价止损-止损-冰山-影子 | 自动单-条件单-埋单-互换-应价-询价-行权-弃权 | 客户端止损-同步套利-先后套利 
#define	 _Advanced					"Advanced"		//套利高级
#define	 _HeadItems					"HeadItems"		//列头保存项
#define	 _RightCancel				"RightCancel"   //保存项 右键撤单
#define  _CoverOpen                 "CoverOpen"     //平开 反手
#define  _BatchCondition            "Condition"     //条件 批量

//期权
#define  _OptionSync				"OptionSync"	//期权策略操作（联动消息的Action）
//操作内容
#define  _BackDirect				"BackDirect"	//反向 "BackDirect&期权序列"
#define  _Reset						"Reset"			//重置 "Reset"
#define  _AddTarget					"AddTarget"		//增加标的 "AddTarget&期权序列&买卖方向"
#define  _DeleteTarget				"DeleteTarget"	//删除标的 "AddTarget&期权序列"

		
//编辑框输入限制
#define MQty_Limit			6			//委托数量 数量倍率 最大最小量
#define MQty_Max			1000000
#define MQty_Min			1

#define MPrice_Limit		10			//委托价 触发价 止损止盈价
#define MPrice_Max			1000000000
#define MPrice_Min			-100000000

#define MPoint_Limit		3			//超价
#define MPoint_Max			999
#define MPoint_Min			-99

#define NNoteLen			50			//备注长度限制

//竖向分类标签
enum MouseHeadInfo
{
	MHI_Normal,		//普通
	MHI_Advanced,	//高级
	MHI_Strategys,	//策略
	MHI_BatchOrder,	//批量
};
//普通标签
enum ChildHeadNormal
{
	CHN_Vert,		//竖向
	CHN_Ladder,		//点价
	CHN_Option,		//期权
};
//高级标签 
enum ChildHeadAdvanced
{
	CHA_Condition,	//条件		启明星 CTP		
	CHA_PreOrder,	//埋单		启明星
	CHA_AutoOrder,	//自动单	启明星
	CHA_Swap,		//互换		内盘
	CHA_LimtStop,	//限价止损	内外盘 
	CHA_Stop,		//止损
	CHA_Iceberg,	//冰山
	CHA_Shadow,		//影子
	CHA_Enquiry,	//询价
	CHA_Offer,		//应价
	CHA_ExecAban,	//行弃权
	CHA_SpreadApply,//套利申请
	CHA_HedgeApply,	//套保申请
    CHA_AutoClose,  //自对冲申请(行权前期权、行权后期货)
	CHA_END,
};
//策略标签
enum ChildHeadStrategys
{
	CHS_PCStop,		//客户端止损
	CHS_SyncSpread,	//本地套利
	CHS_SequSpread,	//先后套利
};
//内外盘标识
enum InnerForeign
{
	IF_Inner,		//内盘
	IF_Foreign		//外盘
};
//行情字段解析 行情分组联动 "contractid=ZCE|F|CF|901;field=113;src=grid" 
enum ID_QUOTE_INFO
{
	ID_CODE,		//合约
	ID_FIELD,		//点击域
    ID_SRC,			//来源
    ID_LEVEL		//深度(盘口有效 src=panel)
};

typedef char MPriceType;
#define mptOrder	'O'		//委托价
#define mptTrig		'T'		//触发价
#define mptLoss		'L'		//止损价
#define mptProfit	'P'		//止盈价

typedef char MErrorType;	//价格错误分类
#define metMinTick	'M'		//最小变动价
#define metEmpty	'E'		//空
#define metNoNegtive 'N'	//不支持负数
#define metZero		'Z'		//0

#define WM_BATCHORDERCHANGE		6000	//批量配置修改
#define WM_ONQUOTE				6001	//行情回调 转消息处理
#define WM_ONPOSITION			6002	//持仓回调 
#define WM_ONCODELISTRETURN		6003	//合约列表处理回车键
#define WM_ONRESETUSERNO		6004	//初始化完成、断线 重置账户信息
#define WM_PRICE_UPDOWN			6005	//价格 上下切换
#define WM_QTY_UPDOWN			6006	//数量 上下切换
#define WM_DEALQUOTECOMMON		6010	//处理行情联动消息
#define WM_DEALPOSCOMMON		6011	//处理持仓联动消息
#define WM_DEALORDERCOMMON		6012	//处理委托联动消息
#define WM_DEALMODIFYRECOVER	6013	//处理改单界面恢复消息
#define WM_DEALTRADABLEQTY		6014	//处理对冲建仓消息F
#define WM_DEALPREFREEZE		6015	//处理预冻结消息F
#define WM_DEALKLINECONTRACT	6016	//K线切换消息
#define WM_DEALOPTIONSTATEGY	6017	//期权组合
#define WM_ONQUOTETARGET		6018	//期权标的行情回调 转消息处理
#define WM_ONEXEOPTION			6019	//期权 行弃权
#define WM_DEALSPREADCOMMON		6020	//处理套利联动消息

enum EN_AREATYPE
{
	Area_Head = 0,
	Area_UserNo,					//资金账号			账号 账号锁 简称
	Area_Commodity,					//品种合约			合约 合约锁
    Area_Spread_Code,               //套利合约
	Area_OrderQty,					//委托数量			数量 有效类型 分数比例 点价持仓
	Area_ValidType,					//有效类型			//外盘
	Area_OrderQtyRate,				//数量倍率			//快速
	Area_TrigPrice,					//触发价格			//条件 止损 限价止损
	Area_OrderPrice,				//委托价格			买卖一档行情
	Area_TimePick,					//触发时间			套利生效时间
	Area_SellPrice,					//卖出价格			
	Area_EnquiryNo,					//询价号			
	Area_LocalStop,					//本地止损			//单客户
	Area_MaxQty,					//最大数量			//冰山
	Area_MinQty,					//最小数量
	Area_Note,						//备注				//交易员+配置
	Area_FastOrder,					//快速				//单客户
	Area_AutoDirect,				//自动开平			//内盘
	Area_Force,						//强平				//交易员
	Area_AddOne,					//T+1				//港交所
	Area_CloseDay,					//平今				//上期所
	Area_OpenContinue,				//连续开仓			//内盘
	Area_CmbFlag,					//组合买卖标识		//外盘
	Area_AssistOrder,				//追价				//套利
	Area_Advanced,					//高级				//套利
	Area_TimerCheck,				//时间条件			//套利
    Area_RightCancel,               //右键撤单           //点价
    Area_CoverOpen,                 //平开 反手          //竖向
    Area_Condition,                 //条件 批量
    Area_ExeAutoClose,              //行权 行权后自对冲
    Area_MovePosition,              //移仓
	Area_Picture,					//买卖按钮			四键-三键-二键
	Area_List,						//点价版
	Area_Option,					//期权版
	Area_Ok,						//确定				//
	Area_Modify,					//修改				//修改定单 批量修改
	Area_Cancel,					//取消
	Area_End						//
};

enum ControlID
{
	//选项头
	CI_Head_Button = 0,
	CI_Head_Search,

	//资金账号
	CI_UserNoBatch_Static,
	CI_UserNo_Static,
	CI_UserNo_Static2,
	CI_UserNo_Edit,
	CI_UserNo_Combox,
	CI_UserNo_Lock,
	CI_UserName_Static,
	CI_UserNo_StaticA,
	//套利账号
	
	CI_UserNo_Edit2, 
	CI_UserNo_Edit3,
	CI_UserNo_Edit4,

	CI_UserNo_Combox2,
	CI_UserNo_Sync,	
    CI_UserNo_Combox3,
    CI_UserNo_Combox4,

	//合约
	CI_ContractNo_Static,
	CI_ContractNo_Edit,
	CI_ContractNo_Lock,
	CI_ContractNo_Edit_Win,

    //套利合约 
	CI_Spread_Static,
	CI_Spread_Edit,

	//委托数量
	CI_OrderQty_Static,
	CI_OrderQty_Edit,
	CI_OrderQty_Spin,
    //数量比例 套利
    CI_OrderRatio_Static,

	//多头空头
	CI_OrderLongShort_Static,
    //有效类型
	CI_ValidStyle_Button,

	//数量倍率
	CI_OrderQtyRate_Static,
	CI_OrderQtyRate_Edit,
	CI_OrderQtyRate_Spin,

	//委托价格
	CI_OrderPrice_Static,
	CI_OrderPrice_Edit,
	CI_OrderPrice_Spin,
	CI_OrderPrice_Static_Cal,
	CI_OrderPrice_Edit_Mole,
	CI_OrderPrice_Static_Div,
	CI_OrderPrice_Edit_Dem,
	//超价点数
	CI_OverPoint_Static,
	CI_OverPoint_Edit,
	CI_OverPoint_Spin,
	CI_OverPoint_Edit_Win,

	CI_SpreadSwitch_Static,
	//套利触发
	CI_SpreadTrigMode_Combox,
	CI_SpreadTrigMode_Combox2,
	//套利触发时间
	CI_SpreadBeignTime_Static,
	CI_SpreadBeginTime_Picker,
    CI_SpreadEndTime_Static,
    CI_SpreadEndTime_Picker,

	//卖出价格
	CI_BuyPrice_Static,
	CI_SellPrice_Static,
	CI_SellPrice_Edit,
	CI_SellPrice_Spin,
	CI_SellPrice_Static_Cal,
	CI_SellPrice_Edit_Mole,
	CI_SellPrice_Static_Div,
	CI_SellPrice_Edit_Dem,
	
	//询价号
	CI_EnquriyNo_Static,
	CI_EnquriyNo_Edit,

	//本地止损
	CI_StopLoss_Static,
	CI_StopLoss_Edit,
	CI_StopLoss_Spin,
	CI_StopLoss_Static_Cal,
	CI_StopLoss_Edit_Mole,
	CI_StopLoss_Static_Div,
	CI_StopLoss_Edit_Dem,

	CI_StopProfit_Static,
	CI_StopProfit_Edit,
	CI_StopProfit_Spin,
	CI_StopProfit_Static_Cal,
	CI_StopProfit_Edit_Mole,
	CI_StopProfit_Static_Div,
	CI_StopProfit_Edit_Dem,

	//最大下单量
	CI_MaxQty_Static,
	CI_MaxQty_Edit,
	CI_MaxQty_Spin,

	//最小下单量
	CI_MinQty_Static,
	CI_MinQty_Edit,
	CI_MinQty_Spin,
	
	//买卖行情
	CI_OrderSellPrice_Static,
	CI_OrderBuyPrice_Static,

	//触发价格
	CI_TriggerPrice_Static,
	CI_TriggerPrice_Edit,
	CI_TriggerPrice_Spin,
	CI_TriggerPrice_Static_Cal,
	CI_TriggerPrice_Edit_Mole,
	CI_TriggerPrice_Static_Div,
	CI_TriggerPrice_Edit_Dem,

	//条件单 触发方式
	CI_TrigMode_Combox,
	CI_TrigMode_Combox2,
	CI_TrigMode_Static2,

	//备注信息
	CI_NoteInfo_Static,
	CI_NoteInfo_Edit,

	//自动开平 强平 T+1 平今
	CI_AutoDirect_Check,
	CI_ForceClose_Check,
	CI_AddOne_Check,
	CI_CloseDay_Check,
	CI_FastOrder_Check,
	CI_AssistOrder_Check,
	CI_Advanced_Check,
	CI_TimeValid_Check,
	CI_OpenContinue_Check,
    CI_Right_Cancel_Check,
    CI_CoverOpen_Check,
    CI_Condition_Check,
    CI_ExeAutoClose_Check,      //行权后自对冲
    CI_MovePosition_Check,

	CI_CmbFlag_Static,
	//买卖 竖向
	CI_OrderBtn,

	//买卖 点价
	CI_OrderList,

	//修改 取消
	CI_Ok_Button,
	CI_Modify_Button,
	CI_Cancel_Button,

	//期权 虚实值 到期日 显示
	CI_ShowTip_Option,

	//可行权数
	CI_CanExcute_Option,

	//提示信息
	CI_ShowTip_Static,

	//竖向结束标记
	CI_Mouse_End,
};
//区域信息
struct sAreaField
{
	int		nID;		//区域编号 EN_AREATYPE
	bool	bShow;		//显示行 标记
	bool    bSubShow;	//特殊标记
	sAreaField(){
		nID = 0;
		bShow = false;
		bSubShow = false;
	}
};
//用户信息
struct sUserInfor
{
	char sUserNo[21];	//账号
	char sUserName[21];	//简称
	char sSign[21];		//服务器标识
	char cUserType;		//身份类型
	sUserInfor()
	{
		memset(this, 0, sizeof(sUserInfor));
		cUserType = uiUser;
	}
};
//期权虚实值
struct TOptionRealValue
{
	TOptionType		cOptionType;
	double			dStrikePrice;
	wchar_t			cRealValue[21];
	TOptionRealValue()
	{
		memset(this, 0, sizeof(TOptionRealValue));
		cOptionType = otNone;
	}
};
//表头信息
struct  TClickHeadInfo
{
	int nCurHeadType;
	int nCurChildType;
	TClickHeadInfo()
	{
		memset(this, 0, sizeof(TClickHeadInfo));
	}
};
//存储信息
struct THeadCurInfo
{
	int				nSize;
	TClickHeadInfo	tHeadInfo[N_Search_Size];
	THeadCurInfo()
	{
		memset(this, 0, sizeof(THeadCurInfo));
	}
};
//下单相关属性保存字段 保存在框架中 
struct WinProperty
{
	bool bCheckFast;		//快速勾选
	bool bCheckAutoCOver;	//自动开平勾选
	char cCoverMode;		//平仓方式（决定初始布局是内盘布局 还是外盘布局）
	bool bAddOne;			//港交所T+1
	bool bChaseOrder;		//追单
	bool bAdvanced;			//套利高级
	bool bExchangeSup;		//交易所支持 跨式 宽跨式
	int	 nMouseHead;		//MouseHeadInfo
	int	 nChildHead;		//ChildHeadInfo
	char HeadItemsInfo[21];	//列头子项
    bool bRightCancel;      //右键撤单
    bool bCoverOpen;        //平开 反手
    bool bCondition;        //条件 批量
    int nOrderListStyle;    //点价样式

	WinProperty()
	{
		memset(this, 0, sizeof(WinProperty));
		cCoverMode = oCover;//默认区分开平 内盘
		bExchangeSup = true;
        bRightCancel = true;
        bCoverOpen = false;
        bCondition = false;
	}
};
//竖向下单类
class CMousewin : public TIxFrm, public TradeSpiEx, public ISQuoteDataSpi, public IOperator, 
                  public ICommonInteract, public ICommonInteractFast, public IStrategySpi, public IOrderConfig
{
public:
	CMousewin(WinProperty wProprety);
	virtual ~CMousewin();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	//////////////////////////////////////////////////////////////////////////系统通知消息处理函数
	virtual void __cdecl ModifyOrder(const uint OrderID, bool bBatch = false);						//改单 
	virtual void __cdecl SetModifyRecover();														//改单恢复（多个改单界面同步恢复时调用）

	virtual void __cdecl SetCodeFromQuote(const char * sKey, const char* cType);					//双击 单击 行情填单
	virtual void __cdecl SetCodeFromPosTotal(const char * sKey, const char* cType);					//点击持仓合计
	virtual void __cdecl SetCodeFromPosSub(const char * sKey, const char* cType);					//点击持仓明细
	virtual void __cdecl SetCodeFromOrder(const char *sOrderID, const char* cType);					//双击 单击 委托填单	
	virtual void __cdecl SetCodeFromSpread(const char *sOrderID, const char* cType);				//双击 单击 套利填单	
	virtual void __cdecl SetCodeFromQuoteKline(const char *sContent);								//行情K线切换
	virtual void __cdecl SetCodeFromOption(const char *sContent);									//期权组合
	virtual void __cdecl SetExecFromPosTotal(const char * sKey);									//行弃权
public:
	//////////////////////////////////////////////////////////////////////////ITradeSpi 交易回调
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin);//断线
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress
		, const TLoginBase * pLoginBase);															//交易初始化完成
	virtual void __cdecl OnMatch(const TMatch * pMatch);											//成交回报 刷持仓（金仕达 持仓全刷 不能监控持仓）
	virtual void __cdecl OnOrder(const TOrder * pOrder);											//委托回报
	virtual void __cdecl OnPreFrozen(const TPreFreeze * pPreFreeze);								//预冻结回报
	virtual void __cdecl OnTradableQty(const TTradableQty * pTradableQty);							//对冲建仓回报
public:
	//////////////////////////////////////////////////////////////////////////行情回调 	
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);				//普通行情数据	
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont);								//套利行情数据
public:
	//////////////////////////////////////////////////////////////////////////ICommonInteract 通用回调
	virtual void __cdecl OnCommonCfgChanged(CM_SUBID PartID, char* pdata);
	virtual bool __cdecl GetFastInfo(sFastInfo& sInfo);												//快速下单信息获取
	virtual void __cdecl OnFastError(const ErrorReflectInfo* eInfo);								//快速下单错误信息反馈
	virtual void __cdecl OnFastOrderInfo(const TSendOrder* TSOrder, double dRatio);					//快速下单回填订单信息
	virtual void __cdecl OnFastStateInfo(int nstate);												//快速勾选状态	1勾选 0未勾选
public:
	//////////////////////////////////////////////////////////////////////////IStrategyTrade 策略回调
	virtual void __cdecl OnStopInfoToPosition(const TPosition *p){};								//赢损信息更新到持仓合计上 显示
	virtual void __cdecl OnStopInfoToOrder(){};														//赢损信息更新到委托上 显示
	virtual void __cdecl OnStopInfoUpdate(bool bShwoUpdate);										//本地加载数据 有待确认信息
	virtual void __cdecl OnStrategyNotice(const TStrategyOrder *pdata){};							//条件单状态变化通知 NULL时需要重新获取
	virtual void __cdecl OnPositionNotice(const TPosition *pdata){};								//持仓变化通知 NULL时需要重新获取
	virtual void __cdecl OnStrategyWorkState(const StrategyWorkState stste){};						//条件单 运行状态变化通知
	virtual void __cdecl OnSpreadInfoNotice(const TSpreadOrder *pspread){};							//套利数据更新
	virtual void __cdecl OnAssistInfoRef(uint OrderIdOld, uint OrderIdNew) {};
public:
	//////////////////////////////////////////////////////////////////////////IOperator
	virtual void	SetValidType(const wchar_t* sVData);								//设置有效类型
	virtual void	SetMaxMinQty(const char* sQty);										//设置最大最小下单量
	virtual bool	SendOrder(MsgOrderInfo *msg);										//设置订单信息 发单
	virtual void	SetContractNo(const char* sCode, CodeSetType csType = cstChange);	//设置合约
	virtual void	SetPriceData(const char* sdata);									//设置价格 委托 触发 卖出
	virtual bool	SetHeadData(const wchar_t* sPType,bool bHead=true);					//设置竖向列头信息
	virtual bool	SetSelect(const wchar_t* sSelect, int nRef);						//设置策略（条件 自动 埋单）
	virtual bool	TableButtonToEdit();												//Tab切换
	virtual bool	CxGetWindowText(MH_TYPE m_type, int& nSize, wchar_t* wstr, int nMaxLen = LEN_RET);
public:
    /////IOrderConfig 
    virtual void OnLadderStyleChanged();			//点价下单配置改变
public:
	//////////////////////////////////////////////////////////////////////////Tab Enter
	BOOL DealTabKey(HWND hwnd, int nID);			//Tab键处理
	BOOL DealReturnKey(HWND hwnd, int nID);			//Enter键处理
public:
	bool TablesEditToButton();						//tab切换到下单按钮

	bool IsLadderLayOut();							//是否点价
	bool IsCheckOpenContinues();					//是否勾选连续开仓
	bool IsOptionStrategyLayOut();					//是否期权策略
    bool IsAutoCloseLayOut();                       //是否自对冲申请

	bool LadderSendOrder(ListOrderInfo &OrderInfo);	//点价发单

	bool OptionSendOrder(TSendOrder &OrderInfo);	//期权策略发单

	bool TradeLoginLinkage();						//联动消息 弹出登录窗口

	void GetSign(char(&sign)[21]) { memcpy(sign, m_sSign[0].c_str(),sizeof(sign)); };			//获取服务器标识

	void UpdateSelectInfo(TMouPriceTypeStrW str[N_Search_Size], int nSize, bool bClearSpecial);	//更新列头

	void OnSizeHeadWidth();							//固定列宽度变化反馈

	void GetPopTopLeftAxis(const int nWidth,int &left, int &nTop);//获取竖向左上角坐标

	void UpdateExchangeSupport(bool bCheck);		//更新交易所支持勾选项

	void GetProperty(char(&cProperty)[MAX_PATH]);

    bool GetOrderUser(TMouUserNoStrA(&UserNo), bool bShowMsg = true);   //获取资金账户  期权计算保证金需要获取userno
public:
	map<int, WNDPROC> m_gTabs;			//Table切换窗口处理过程

	SOFCommodityInf m_CommodityInfo;	//品种信息
	
	C_FillOrderInfo m_FillOrderInfo;	//填单信息 通用返回

	string			m_sOrderInfo;		//填单信息 委托使用

	string			m_sSpreadInfo;		//填单信息 套利使用
private:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//窗口处理过程

	wstring		GetModulePathEx();						//配置路径
private:
	//////////////////////////////////////////////////////////////////////////窗口消息处理
	void OnCreateWindow();								//窗口创建消息响应

	void OnNotify(WPARAM wParam, LPARAM lParam);		//通知消息处理 spin控制 

	void OnCommond(WPARAM wParam, LPARAM lParam);		//事件处理

	void OnSize(WPARAM wParam, LPARAM lParam);			//拖拽处理

	void OnPaint(WPARAM wParam, LPARAM lParam);			//自绘

	void OnDestroy();									//窗口销毁 数据清空

	void OnNcDestory();									//释放

	void OnDrawItem(WPARAM wParam, LPARAM lParam);		//Combox重绘

	void OnMeasureItem(WPARAM wParam, LPARAM lParam);	//Combox重绘

	void OnMouseWheel(WPARAM wParam, LPARAM lParam);	//滚轮消息 编辑框（价格 数量 合约代码列表）

	void OnMouseMove();									//鼠标移动

	void OnMouseLeave();								//鼠标移出

	void OnMouseHover();								//激活

	void OnLButtonDown(WPARAM wParam, LPARAM lParam);	//左键按下

private:
	//////////////////////////////////////////////////////////////////////////改单 行权 弃权 询价
	void OnOkSendOrder();								//改单发单
	
	void OnModifySendOrder();							//改单发单

	void OnCancel();									//取消

	void OnOK();										//确定
	//////////////////////////////////////////////////////////////////////////子控件触发事件
	void OnButtonSearch();								//搜索
	void OnButtonUserLock();							//账号锁
	void OnButtonUserSync();							//账号联动
	void OnButtonValidStyle();							//有效类型
	void UpdatePriceStyleControl();						//价格类型 控件状态更新
	void OnButtonContractLock();						//合约锁
	void OnButtonCalFlag(int nID);						//运算符

	void OnButtonAutoDirect();							//自动开平处理
	void OnButtonFastOrder();							//快速
	void OnButtonSpreadAdvanced();						//高级 套利
	void OnButtonSpreadTimer();							//高级 套利 时间
	void OnButtonQuotePrice(int nID);					//点击界面 合约行情信息 填写委托价格
    void OnCheckRightCancel();                          //右键撤单

	void OnSpinDeltaUpDownEx(int nDelta, int nID, int nMoleID);		//价格增减处理
	void OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax = MQty_Max, int nMin = MQty_Min);				//数量增减处理
	void OnSpinDeltaUpDownRate(int nDelta, int nID);				//数量倍率增减处理

	void OnWheelCodeList(int nID, int nDelta);			//合约列表响应滚轮响应
	void OnCodeReturn(int nID);							//合约列表响应回车按钮

	void OnClickUserStatic();							//账号同步消息

	void OnKillFocusUserNo(int nID);					//资金账户编辑框失去焦点处理
	void OnSelChangeUserNo(int nID);					//资金账户编辑框信息改变处理
	void OnEditChangeUserNo(int nID,bool bSync = false);//资金账户编辑框信息改变处理 bSync是否联动引起变化

	void OnKillFocusCode(int nID = CI_ContractNo_Edit);	//合约代码失去焦点
	void OnSelChangeCode(int nID = CI_ContractNo_Edit);	//合约代码发生变化
	void UpdateCodeEdit(int nID = CI_ContractNo_Edit);	//合约显示变化
    void UpdateSpreadCodeEdit(SSpreadContract &sQuote); //套利合约变化
	
	void OnSelChangeEditPrice(int nID);					//价格变动处理
	void OnSetFocusEditPrice(int nID);					//价格编辑框得到焦点处理

	void OnSelChangeEditPoint();						//超价点数变动处理

	void OnKillFocusOrderQty();							//委托数量失去焦点
	void OnSetFocusOrderQty();							//委托数量得到焦点
	void OnUpdateOrderQty();							//委托数量更新

	void OnEditChangeNoteInfo();						//备注信息
	//////////////////////////////////////////////////////////////////////////
	void CreateControlWnd();							//窗口控件创建

	void SubClassWindow();								//窗口控件子类化

	inline void SubClass(int nID);						//子类化函数

	void EnableWndStaticOwnerDraw(int nID, BOOL bEnable);//控件状态 变化 刷新

	void CreateStaticControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);
	void CreateEditControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);
	void CreateComboxControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);
	void CreateButtonControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);
	void CreateSpinControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight);

	void SetIdWithWnd();								//绑定 Id 窗口
	void SetSpinSize();									//设置 spin控件大小
	void SetEditLen();									//设置编辑框内容长度
	void SetControlFont();								//设置控件字体
	void SetRedrawWnd();								//设置控件重绘
	void SetPopListContent();							//设置弹出窗口内容
public:
	//////////////////////////////////////////////////////////////////////////弹出选择框
	void OnPopCodeSelectWnd(int nID = CI_ContractNo_Edit);								//合约代码得到焦点

	void OnPopPriceSelectWnd(int nID);						//价格

	void OnPopQtySelectWnd(int nID);						//委托数量 最大最小数量
private:
	//////////////////////////////////////////////////////////////////////////布局
	void LayOut();															//重新布局

	void UpdateAreaInfo();													//更新区域信息

	void SetAreaVisible(int nID, bool bshow,bool bshowsub = false);			//设置区域显示信息
	
	void InitOrderTypeData();												//初始化区域信息
	
	void MoveArea_Head(int &nCurHeight, bool bshow);
	void MoveArea_UserNo(int &nCurHeight, bool bshow, bool bshowsub = false);
	void MoveArea_Commodity(int &nCurHeight, bool bshow, bool bshowsub = false);
    void MoveArea_SpreadCode(int &nCurHeight, bool bshow);
	void MoveArea_OrderQty(int &nCurHeight, bool bshow);
	void MoveArea_ValidType(bool bshow);
	void MoveArea_CmbFlag(int &nCurHeight,bool bshow, bool &bdiv);
	void MoveArea_OrderQtyRate(int &nCurHeight, bool bshow);
	void MoveArea_OrderPrice(int &nCurHeight, bool bshow, bool bshowsub = false);
	void MoveArea_TimePicker(int &nCurHeight, bool bshow);
	void MoveArea_SellPrice(int &nCurHeight, bool bshow);
	void MoveArea_Enquriy(int &nCurHeight, bool bshow);
	void MoveArea_LocalStop(int &nCurHeight, bool bshow);
	void MoveArea_MaxQty(int &nCurHeight, bool bshow);
	void MoveArea_MinQty(int &nCurHeight, bool bshow);
	void MoveArea_TrigPrice(int &nCurHeight, bool bshow, bool bshowsub = false);
	void MoveArea_Note(int &nCurHeight, bool bshow);
	void MoveArea_Check(int nID,int &nCurHeight, int &nCurWidth, bool bshow, bool &bdiv);
	void MoveArea_Picture(int &nCurHeight, bool bshow, bool bdiv);
	void MoveArea_List(int &nCurHeight, bool bshow, bool bdiv);
	void MoveArea_Option(int &nCurHeight, bool bshow, bool bdiv);
	void MoveArea_Ok(int &nCurHeight, bool bshow);
	void MoveArea_Modify(int &nCurHeight,int &nCurWidth, bool bshow);
	void MoveArea_Cancel(int &nCurHeight, bool bshow);
	
	void UpdateAreaInfoNormal();		//更新普通列
	void UpdateAreaInfoAdvanced();		//更新高级列
	void UpdateAreaInfoStartegy();		//更新策略列

	void InitTotalInfo();				//初始化 列头数据
	void UpdateSearchLayOut();			//显示项更新（登陆类型 策略状态）
	void UpdatePropertyToHeadItem();	//更新保存属性 固定列
	void UpdatePropertyCurHead();		//更新保存属性 选中列
	void UpdateCurHeadStr();			//更新选中项

	bool GetShowItemByStr(TClickHeadInfo& item, const TMouPriceTypeStrW str);
	bool GetShowStrByItem(const TClickHeadInfo& item, TMouPriceTypeStrW str);
	bool UpdateShowItemByOrderType(char cOrderType, char cStrategyType = stNone);
	
	bool ResizeCtrlWindowWidth(HWND hwnd, int nWidth);						//横向变化窗口

	RECT MoveCtrlWindow(HWND hwnd, int &nHeightStart);						//横向移动基准控件窗口
		
	RECT MoveCtrlWindow(HWND hwnd, int &nHeightStart, int &nHeightCenter);	//横向移动基准控件窗口

	RECT MoveCtrlRowWindow(HWND hwnd, int nHeightCenter);					//纵向移动随同变动控件窗口

	RECT MoveCtrlColWindow(HWND hwnd, int &nWidthStart);					//横向移动随同变动控件窗口

	RECT MoveCtrlColWindowEx(HWND hwnd, const int nWidthMove);				//横向移动随同变动控件窗口

	void SetWindowCtrlEnableOrDisable(bool bEnable = true);					//控件无效性控制

	void UpdateCheckState();												//更新快速勾选

	void UpdateModifyRecover();												//更新改单恢复

	void UpdateFocus(char cDirect, char cOffset);							//更新焦点		

	void UpdatePropertyControl();											//更新保存属性 控件状态

	void UpdatePropertyCfg();												//快速 自动勾选属性保存

	void SetFastControlState();												//设置快速状态

	void SetPriceControlState();											//设置价格控件状态

	void ResetFastCheck();													//快速勾选恢复

	void ResetPriceLevel();													//委托价格恢复

	void ClearPosition();													//清空持仓显示

	OrderButtonType GetModeType();											//获取内外盘风格

	bool IsCodeOfDCE();														//是否套利布局

	bool IsSpreadLayOut();													//是否套利布局

	bool IsSwapLayOut();													//是否互换布局

	bool IsOfferLayOut();													//是否应价布局

	bool IsStopLayOut();													//是否止损布局

	bool IsOptionLayOut();													//期权界面
	
	bool IsOptionExAbnLayOut();												//期权 行权弃权
	
	bool IsBatchLayOut();													//批量界面

	bool IsConditionLayout();												//条件界面

	bool IsSpreadApplyLayOut();												//套利申请布局
	
	bool IsHedgeApplyLayOut();												//套保申请布局

	bool IsInnerCommodity();												//是否内盘合约

	bool IsUserNoSync();													//账号联动

	bool IsUserNoLock();													//账号锁定

	bool IsSpreadCodeAdd(const char* str);									//套利合约设置

	bool IsSpreadTriggered();												//同步套利 （市价）触发发单

	bool CheckSameCode(const char* sCode,bool bSpread = false);				//检测设置合约是否相同				

	void SetCreateFlag(bool bWinCreated){ m_bCreateComplete = bWinCreated; };//创建窗口完成标识

	bool GetCreateFlag(){ return m_bCreateComplete; };						//获取窗口创建标识

	//////////////////////////////////////////////////////////////////////////配置更新	
	void OnDealAssistFill(const C_FillOrderAssistInfo* pdata);		//辅助填单

	void OnDealOrderDeal(const C_OrderDealInfo* pdata);				//定单处理

	void OnDealOrderReturn(const C_OrderReturnInfo* pdata);			//下单扩展

	void OnDealFastOrderInfo(const C_FastOrderInfo* pdata);			//快速下单

	void OnDealBatchGroupInfo(const C_BatchGroupInfo* pdata);		//批量下单

	void OnDealBatchOrderChange();									//批量分组修改

	////////////////////////////////////////////////////////////////////////////行情 数据刷新回调处理 ||交易 持仓更新回调处理	
	void OnDealQuote();												//处理行情更新

	void OnDealQuoteOptionTarget();									//处理期权标的行情更新 期权虚实值

	void OnDealPosition();											//处理持仓更新

	void OnDealPreFreeze(int nDirect);								//处理预冻结

	void OnDealTradableQty(int nDirect);							//处理可开可冲

	//////////////////////////////////////////////////////////////////////////窗口联动
	void OnDealQuoteCommon();										//处理行情数据

	void OnDealPositionCommon();									//处理委托 持仓 数据

	void OnDealOrderCommon();										//处理委托 委托 数据

	void OnDealSpreadCommon();										//处理套利 委托 数据

	void OnDealModifyRecover();										//处理改单界面恢复

	void OnDealKLineContract();										//K线切换处理

	void OnDealOptionStartegy();									//期权组合处理

	void OnDealOptionExe();											//期权行弃权

	void FillBaseDataToDlg(const TSendOrder * pSendOrder);			//改单界面填单

	//////////////////////////////////////////////////////////////////////////交易 行情 数据更新
	bool UpdateUserInfo(string sUser, int index = 0);				//更新用户相关信息
	//bool UpdateUserInfo2(string sUser);								//更新用户相关信息

	void CodeUpdateCommodityInfo();									//更新合约品种信息 交易切换
	//void UpdateSpreadCommodityInfo(SSpreadRateModeType RateMode, SOFCommodityInf &sinf,	SOFCommodityInf &sinf2);//更新套利品种信息

	void UpdateQuoteContract(const SContractNoType sQuote,CodeSetType csType);					//更新行情合约
	
	bool UpdateOptionTarget();										//更新期权标的合约

	void UpdatePositionQty();										//更新持仓显示
	void UpdatePositionQtySpread();									//更新套利持仓显示
	void UpdatePositionQtySwap();									//更新互换持仓显示
	void UpdatePositionQtyNormal();									//更新普通持仓显示

	void UpdateBtnOpenCover(const TContractKey& key, uint nLong, uint nShort);
	void UpdateCanExecuteQty(char cCommodityType,uint nPos);		//更新可行权数	交易员不显示（遍历挂单效率低）
	void UpdateStaticPosInfo(const TContractKey &key,uint nLong,uint nShort);	//更新//行权 期权 应价 点价 //持仓数量 价格

	void UpdateBtnCanOpen(const TContractKey* key = NULL);			//按钮 可开可平显示更新

	void UpdateConditionData(char cDirect);							//更新条件

	void UpdatePriceRange();										//更新价格选择范围 涨跌停 最新
	
	void UpdateShowText(int nEditID,bool setfocus);										//更新价格选择范围 涨跌停 最新
	
	bool GetTradeCommodityInf(TContractKey &key, SOFCommodityInf &CommodityInfo);							//关键字更新品种信息

	void GetPositionVol(const char * UserNo, const TContractKey * ContractKey
		, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount);						//获取持仓

	uint GetParOrderExcute();																				//获取行权挂单量

	bool GetConditionInfo(char &cMode, char &cCondition, double &dPrice, bool bShowMsg = true);				//获取条件单信息

	bool GetSpreadPriceInfo(TDLPriceMode &cMode, char &cCondition, double &dPrice, bool bShowMsg = true);	//获取套利价格信息

	bool GetDefaultTrigPrice(const SContractNoType ContractId, TDirect tdirect, double& dPrice);			//获取条件触发价格

	//////////////////////////////////////////////////////////////////////////对冲建仓 可开可平 概要信息
	void ShowCanOrderVol(const TTradableQty * pTradableQty);		//显示建仓

	void ShowOrderFrozen(const TPreFreeze * pPreFreeze);			//显示预冻结保证金

	bool GetAvailableMoney(double &dAvailabe);						//得到可用资金

	bool IsVaild(const TSendOrder *pOrder, double dPrice);			//已取得的数据是否有效

	void FormatDisplayQtyAndFreeze(bool bQty, bool bFreeze);		//格式化可开可平 和 预冻结保证金 //仅合约1显示有效

	wstring FormatUserSummary(const TUserInfo* tInfo);				//格式化用户概要信息

	////////////////////////////////////////////////////////////////////// 界面到数据
	void FillAndSendOrder(TSendOrder& sendorder, bool bGetPrice = true,bool bGetCode = true);			//界面信息填写订单 发单  （点价不获取价格  期权不获取价格 不获取合约）
	
	bool FillOrder(TSendOrder& sOrder, bool bGetPrice = true, bool bGetCode = true);					//界面信息填写订单
	bool FillSpreadOrder(TSpreadSendOrder& sOrder);														//界面信息填写套利单

	bool ReSetUserNos();															//重置显示的资金账户信息 竖向下单创建 交易断开 交易初始化成功

	//bool GetOrderUser(TMouUserNoStrA(&UserNo), bool bShowMsg = true);														//获取					资金账户
	bool GetOrderUser2(TMouUserNoStrA(&UserNo), int index, bool bShowMsg = true);														//获取					资金账户

	bool GetOrderCode(TContractKey& key, string &sCode, int nID, bool bShowMsg = true);										//获取					交易合约
	bool GetSpreadOrderCode(SSpreadContract &sQuote, TSpreadSendOrder& sOrder, bool bShowMsg = true);					        //获取					套利合约

	bool GetDoublePrice(int nID, int nMoleID, double &dPrice, bool bShowMsg = false, MPriceType type = mptOrder);			//获取价格
	bool GetSpreadTrigPrice(double &dPrice, bool bShowMsg = false);			//获取价格
	
	bool SetOrderPrice(int nID, int nMoleID, double dPrice);																//价格显示到界面
	bool GetOrderPrice(int nID, int nMoleID, double &dPrice, TDirect td, bool bShowMsg = true, MPriceType type = mptOrder);	//获取					委托价格
	bool GetDoubleOrderPrice(double& dPriceBuy, double& dPriceSell);														//获取					买卖委托价格

	bool CheckOrderPrice(double dPrice, bool bShowMsg = true, MPriceType type = mptOrder);									//判断价格有效性
	void PopPriceCheckMsg(MPriceType ptype = mptOrder, MErrorType etype = metMinTick);										//价格信息提示弹窗
	void GetErrorText(TMouPriceExceed &ErrorText, int nID, const SOFCommodityInf &sInfo, double dp1, double dp2);			//格式化错误信息
	void GetSpeExchangeText(TMouPriceExceed &ErrorText, int nID, const SOFCommodityInf &sInfo, const wchar_t *wExchange, double dp);			//格式化错误信息

	void SetOrderQty(int nID, int nQty);																					//数量显示到界面
	bool GetOrderQty(int nID, uint &nQty, bool bShowMsg = true);															//获取					委托数量	

	bool GetOrderValidStyle(TSendOrder &sOrder);																			//获取					有效类型

	bool GetOrderType(TSendOrder &sOrder);																					//获取					订单类型

	bool CheckOrderTypeSupport(TSendOrder &sOrder);																			//检测 定单支持

	bool CheckIsOption(char ctype, bool bShowMsg = true);																	//判断是否期权合约

	bool CorrectOrderPrice(TSendOrder &sOrder);																				//校正价格

	bool CheckSpreadOrderPriceTrigState(TSpreadSendOrder &Sspread);															//套利价格 触发检测

	void SetShowTips(const wchar_t* wtips);																					//设置提示信息

	void SetStopPreHangOrder(const TSendOrder &TOrderInfo, TVecInt& vOrderID);												//设置委托止损单

	bool DoubleSameOrExceed(double dFirst, double dSecond, bool bFirst = true);												//大于等于
	
	bool DoubleExceed(double dFirst, double dSecond, bool bFirst = true);													//大于 小于

	bool GetStopData(TDirect direct, double dprice);																		//获取止损数据

	void ResetStopContent();																								//清空止损价格	

	bool GetActiveTime(TSpreadSendOrder &Sspread);																			//获取生效时间

    bool UpdateOffsetOfMovePosition(TSpreadSendOrder& sOrder);                                                              //移仓更新开平

    bool IsOneOfComboCode(std::string strCode, std::string strCombCode);            //是否是组合合约的组成合约
	//////////////////////////////////////////////////////////////////////////自定义窗口
	HINSTANCE			m_hInstance;

	vector<sAreaField>	m_AreaInfo;			//区域信息容器

	HWND				m_hControl[CI_Mouse_End - CI_Head_Button + 1];				//控件句柄数组

	CMouseHeadWin		m_cHeadWin;			//竖向头
	CPopWinSearch		m_cPopSearch;

	CxStaticEx			m_StaticPriceSwitch;//价格-限价 市价 对盘 挂单 最新
	CxStaticEx			m_StaticSpCtSwitch;//价格-触发价 立即触发

	CxMousePos			m_StaticPosInfo;	//持仓信息

	CxStatic			m_StaticUserNo;		//账号 联动
    //CxStatic			m_StaticUserNo2;	//账号 联动
	CMouseComboBox		m_ComboxUserNo;		//账号 组号
	CMouseComboBox		m_ComboxUserNo2;	//账号
    CMouseComboBox		m_ComboxUserNo3;	//账号
    CMouseComboBox		m_ComboxUserNo4;	//账号
	CMouseStatic		m_StaticUserName;	//名称
	CLockBtn			m_LockUserNoBtn;	//账户锁定
	CLockBtn			m_SyncUserNoBtn;	//账户联动
	CLockBtn			m_SearchBtn;		//搜索

	CxStatic			m_StaticCode;		//合约
	CxStatic			m_StaticCode2;		//合约
	CxEditEx			m_EditExContract;	//合约编辑框
	CxEditEx			m_EditExOverPoint;	//超价编辑框
	CPopWinCode			m_cCodeWin;			//主合约代码 30k
	CPopWinCodeList		m_CodeList;			//主合约列表 16k child
	//CPopWinCodeQuote	m_cCodeSpreadWin;	//套利合约代码
	CLockBtn			m_LockCodeBtn;		//合约锁定

	CxDrawButton		m_BtnValidType;		//有效类型
	CPopWinDate			m_cValidTypeWin;	//有效类型 4k

	CPopWinQty			m_cOrderQtyWin;		//委托数量 5k child

	CPopWinPrice		m_cOrderPriceWin;	//价格 5k

	COrderButton		m_cOrderBtn;		//下单按钮

	COrderList			m_OrderList;		//点价列表

	COrderOption		m_OptionView;		//期权

	CPopWinTip			m_cStopTip;			//止损止盈确认信息

	CxComboxEx			m_ComboxMode;		//触发方式
	CxComboxEx			m_ComboxCompare;	//触发条件

	CxComboxEx			m_ComboxSpreadMode;	//触发方式
	CxComboxEx			m_ComboxSpreadCompare;	//触发条件

	CxComboxTime		m_CombBeginTime;	//开始时间
    CxComboxTime        m_CombEndTime;      //结束时间

	CxDrawButton		m_BtnOk;			//确定按钮
	CxDrawButton		m_BtnModify;		//修改按钮
	CxDrawButton		m_BtnCancel;		//取消按钮

	//////////////////////////////////////////////////////////////////////////控件重绘
	CxSpin m_SpinQty;	//3k
	CxSpin m_SpinQtyRate;
	CxSpin m_SpinPrice;
	CxSpin m_SpinSellPrice;
	CxSpin m_LossPrice;
	CxSpin m_ProfitPrice;
	CxSpin m_SpinMaxQty;
	CxSpin m_SpinMinQty;
	CxSpin m_SpinTrigPrice;
	CxSpin m_SpinOverPoint;
	
	CxEdit m_EditContractNo2;
	CxEdit m_EditOrderQty;
	CxEdit m_EditOrderQtyRate;
	CxEdit m_EditMaxQty;
	CxEdit m_EditMinQty;
	CxEdit m_EditEnquiry;
	CxEdit m_EditNote;
	CxEdit m_EditOrderPriceMole;
	CxEdit m_EditSellPriceMole;
	CxEdit m_EditTrigPriceMole;
	CxEdit m_EditOverPoint;
	CxEdit m_EditLossMole;
	CxEdit m_EditProfitMole;

	CxCheck m_CheckAutoDirect;
	CxCheck m_CheckForceClose;
	CxCheck m_CheckAddOne;
	CxCheck m_CheckCloseDay;
	CxCheck m_CheckFastOrder;
	CxCheck m_CheckAssistOrder;
	CxCheck m_CheckAdvanced;
	CxCheck m_CheckTimePicker;
	CxCheck m_CheckOpenContinue;
    CxCheck m_CheckRightCancel;
    CxCheck m_CheckCoverOpen;                       //平开 反手
    CxCheck m_CheckCondition;                       //条件 批量
    CxCheck m_CheckAutoClose;                       //行权 行权后自对冲
    CxCheck m_CheckMovePosition;                    //移仓

	CMousePriceEdit m_EditOrderPrice;
	CMousePriceEdit m_EditSellPrice;
	CMousePriceEdit m_EditTrigPrice;
	CMousePriceEdit m_EditLoss;
	CMousePriceEdit m_EditProfit;

	//////////////////////////////////////////////////////////////////////////配置
	wstring					m_strPath;				//路径

	sMousewinCfg			m_MouseCfg;				//配置信息

	C_FillOrderAssistInfo	m_FillOrderAssist;		//填单配置 条件单配置

	C_OrderDealInfo			m_OrderDealInfo;		//下单处理

	C_OrderReturnInfo		m_OrderReturn;			//下单后处理 	

	C_FastOrderInfo			m_fastOrderInfo;		//快速

	WinProperty				m_wProperty;			//窗口属性

	//SSpreadContract			m_SpreadParam;			//套利参数		

	//////////////////////////////////////////////////////////////////////////数据
	TClickHeadInfo		m_nCurClickItem;		//当前选中项

	//画刷
	HBRUSH		m_hStaticBKBrush;				//静态文本背景画刷
	//数据
	wstring		m_sValidtype;					//有效类型字符串

	TMouPriceTypeStrW	m_sPricetype;			//价格信息
	TMouPriceTypeStrW	m_sPriceSpreadtype;		//价格信息

	string		m_sUserNo[4];				    //资金账号
	//string		m_sUserNo2;						//资金账号
	string		m_sUserName;					//姓名
	string		m_sSign[4];						//服务器标识
	//string		m_sSign2;						//服务器标识
	char		m_cUserType;					//登陆类型 交易员、单客户等 用来多账户时切换账户变换界面显示强平信息

	string		m_sQuoteContractId;				//合约代码 （行情）
	string		m_sOptionTarget;				//期权标的 （行情）
	string      m_sCode;						//合约编码 （交易）
    string      m_sSpreadCode;                  //套利合约编码 (交易)
	string		m_sQuoteKline;					//K线合约  （行情）
	string		m_sQuoteOption;					//期权  （行情）
	string		m_sExeOption;					//期权  （行弃权）

	TOptionRealValue m_OptionInfo;				//期权信息

	double		m_sPriceLast;					//最新价
	double		m_sPriceMax;					//涨停价
	double		m_sPriceMin;					//跌停价
	double		m_sPriceBid;					//买价
	double		m_sPriceAsk;					//卖价

	//坐标
	int         m_nQtySelectID;					//数量变化 区分
	bool		m_bCanShowPop;					//得到焦点是否显示弹窗

	int			m_nPriceSelectID;				//价格变化区分
	//标记信息
	bool		m_bAutoCover;					//自动开平

	bool		m_bModifyOrderOper;				//订单修改标记

	bool		m_bBatchModifyOrderOper;		//批量修改标记

	bool		m_bBatchInfoModify;				//批量设置修改 切到批量选项卡 需要更新

	bool		m_bTradeLogined;				//是否交易员登录

	bool		m_bCreateComplete;				//窗口创建完成标识

	bool        m_bShowFuzzyQuery;				//合约变化刷新
	void		SetFuzzyQuery(bool bShow){ m_bShowFuzzyQuery = bShow; }

	bool		m_bCanStrategyTrade;				//是否可以止损

	bool		m_bCanBatchOrder;				//是否可以批量

	bool		m_bStopPreHangOrder;			//本地止损标记

	bool		m_bShowStopSubmit;				//止损信息确认 交易初始化完成 需要加载数据确认

	int			m_LoginType;
	//界面
	bool		m_bTracking;					//鼠标跟踪标识

	bool		m_bExecute;
    bool        m_bOptionAutoClose;              //期权行权前自对冲
private:
	map<wstring, TClickHeadInfo> m_TotalItemInfo;

	map<string, sUserInfor>		m_UserInfo;			//资金账户信息容器

	map<string, char>			m_UserCoverModeMap;	//登陆账户绑定平仓方式 内外盘布局使用

	map<string, BatchGroupInfo> m_batchGroupVec;	//批量信息

	TOrder			m_OrderModifyParam;				//改单参数

	PositionInfo	m_posinfo;						//持仓信息

	double			m_dLossPrice;					//止损价

	double			m_dProfitPrice;					//止盈价

	StopType		m_StopType;						//止损类型

	TMouPriceTypeStrW m_SearchString[CSP_END][N_MAX_CHILD];

	int				m_nSearchSize[CSP_END];
		
	THeadCurInfo	m_HeadCurInfo;					//当前列头项
	//////////////////////////////////////////////////////////////////////////对冲 建仓信息
	CMyLock					m_lock;
	TTradableQty			m_BuyTradableQtytemp;
	TTradableQty			m_SellTradableQtytemp;
	TPreFreeze				m_BuyPreFreezetemp;
	TPreFreeze				m_SellPreFreezetemp;

	TTradableQty			m_LastBuyQty;		//保存上次的可开可平 防止重复查询
	TTradableQty			m_LastSellQty;
	TPreFreeze				m_LastBuyFreeze;	//保存上次的占用保证金
	TPreFreeze				m_LastSellFreeze;
	int						m_MaxOrdID;			//原始定单号	
    int                     m_ValidTypeCfgValue;//接入服务控制有效类型
};

#endif // _MOUSE_WIN_