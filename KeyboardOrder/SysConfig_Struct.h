#pragma once 
#include <WinDef.h>
#include <WinGDI.h>

#pragma	pack(push, 1)

#define  ZCE_NO		"ZCE"
#define  DCE_NO		"DCE"
#define  SHFE_NO	"SHFE"
#define  CFFEX_NO	"CFFEX"


#define LOAD_XML_CFG							0x00000000

#define	KEYORDER_ORDEROPTION_HOTKEY				0x00000002		//交易 ->下单快捷键
#define	KEYORDER_ORDEROPTION_HOTKEYOTHER		0x00000004		//交易->其他快捷键
#define	KEYORDER_ORDEROPTION_ABNORMAL			0x00000008		//交易->异常提醒
#define	KEYORDER_ORDEROPTION_DEFAULTNUM			0x00000010		//交易->下单默认量
#define KEYORDER_ORDEROPTION_CONTRACTHOTKEY		0x00000020		//交易->合约快捷键
#define KEYORDER_ORDEROPTION_FACADESET			0x00000800		//交易->界面配置 与其他模块处理机制不一样 "高级"中的字体、横向下单中德顺序都要考虑
#define SYSGENERAL_ORDER_SETTING				0x00002000		//交易 ->下单参数
#define SYSFRAME_SOUND_SETTING					0x00008000		//交易->声音设置
#define	BATCH_ORDER_SETTING						0x00080000		//交易->常规->批量下单设置
#define FAST_QTY_MULTI							0x00100000		//交易->快速下单中数量倍率
#define GENERAL_RISK_CONTROL					0x00200000		//交易->风险控制
#define CONDITION_ORDER_SET						0x00400000		//交易->条件单
#define KEYORDER_CHANGECODE						0x01000000		//交易->合约切换设置
#define USERNO_FILTER							0x02000000		//交易 ->成交回报 下单人过滤
#define TRADE_ADVANCE							0x04000000		//交易->高级

//配置项ID定义，一个配置项的ID由三项来决定
//szField为所在域，目前包括交易和行情两部分
//szSection为域下所在的节，根据实际情况配置
//szItemID为具体的配置项的ID，包含在某个节下

struct OrderHotKey//下单快捷键的配置结构
{
	unsigned long	kBuyBySellPrice4;			//卖价买进4
	unsigned long	kSellByBuyPrice4;			//买价卖出4
	int				iOverPoint14;				//超价点数4

	unsigned long	kBuyBySellPrice3;			//卖价买进3
	unsigned long	kSellByBuyPrice3;			//买价卖出3
	int				iOverPoint13;				//超价点数3

	unsigned long	kBuyBySellPrice2;			//卖价买进2
	unsigned long	kSellByBuyPrice2;			//买价卖出2
	int				iOverPoint12;				//超价点数2

	unsigned long	kBuyBySellPrice1;			//卖价买进1
	unsigned long	kSellByBuyPrice1;			//买价卖出1
	int				iOverPoint11;				//超价点数1

	unsigned long	kBuyByBuyPrice1;			//买价买入1
	unsigned long	kSellBySellPrice1;			//卖价卖出1
	int				iOverPoint21;				//超价点数1

	unsigned long	kBuyByBuyPrice2;			//买价买入2
	unsigned long	kSellBySellPrice2;			//卖价卖出2
	int				iOverPoint22;				//超价点数2

	unsigned long	kBuyByBuyPrice3;			//买价买入3
	unsigned long	kSellBySellPrice3;			//卖价卖出3
	int				iOverPoint23;				//超价点数3

	unsigned long	kBuyByBuyPrice4;			//买价买入4
	unsigned long	kSellBySellPrice4;			//卖价卖出4
	int				iOverPoint24;				//超价点数4

	int				iPCType;					//平仓方式
	bool			bPCCtrlInvert;				//按Ctrl键快速平仓方式临时取反
	bool			bKCBySpace;					//按空格键连续开仓
	unsigned int	iBySpaceOpenQty;			//按空格键连续开仓的开仓数量取值 0默认开仓量  1反向持仓量

	//数量倍率点数  数量倍率快捷键设置中的
	double			fQtyMulPoint;

	unsigned long	kCancelKC;					//撤买入快捷键
	unsigned long	kCancelPC;					//撤卖出快捷键
	int				iCancelType;				//撤单选项 0==撤全部 1 撤选中

	unsigned long	kCancelOpen;				//撤开仓快捷键
	unsigned long	kCancelCover;				//撤平仓快捷键
	unsigned long	kCancelLast;				//撤最近一笔
	unsigned long	kCancelOld;					//撤最早一笔
};

struct OtherHotKey//下单其他快捷键
{
	unsigned long	kBuyOfNormal;				//普通下单-买进
	unsigned long	kSellOfNormal;				//普通下单-卖出
	unsigned long	kKCOfNormal;				//普通下单-开仓
	unsigned long	kPCOfNormal;				//普通下单-平仓
	unsigned long	kPJOfNormal;				//普通下单-平今

	unsigned long	kZXJOfCondition;			//条件下单-最新价
	unsigned long	kBuyJOfCondition;			//条件下单-买价
	unsigned long	kSellJOfCondition;			//条件下单-卖价
	unsigned long	kDYOfCondition;				//条件下单->=
	unsigned long	kXYOfCondition;				//条件下单-<=

	unsigned long	kCancelBySysNO;				//按系统号撤单

	unsigned long	kYJQC;						//一键清仓
	int				iQCPriceType;				//价格类型
	int				iQCOverPoint;				//超价点数
	bool			bQCCancelOrder;				//清仓时自动撤单
	int				iQCType;					//清仓类型 平今 、平昨、还是全部

	//2015/1/4
	unsigned long	kOTLimit;					//订单类型 限价
	unsigned long	kOTMarket;					//订单类型 市价
	unsigned long	kOTLimitStop;				//订单类型 限价止损
	unsigned long	kOTMarketStop;				//订单类型 止损
	unsigned long	kOTIce;						//订单类型 冰山
	unsigned long	kOTGhost;					//订单类型 影子

	//2015/6/11
	unsigned long   kLockPos;					//一键锁单
	int				iLockPosBuyPriceType;		//买入价格类型  '0'lpOppo   '1'lpQuene  '2'lpLast
	int             iLockPosBuyOverPoint;		//买入超价点数
	int				iLockPosSellPriceType;		//卖出价格类型
	int             iLockPosSellOverPoint;		//卖出超价点数
	char			code[71];					//合约
	int				iOpenQty;					//开仓数量
};

//快速下单合约默认量
struct CommodityVol								
{
	char			commodity[70];				//格式化之后的 固定格式的合约字符串 ZCE SR 305 ; ZCE SR 305 P4400  ;ZCE SR 305/307; ZCE SR 305 P4400/307 C4400
	unsigned int	vol;						//开仓默认量
	unsigned int	Covervol;					//平仓默认量
};

//数量倍率
struct FastQtyMultip								
{
	unsigned long	kFastKey;			//快捷键
	double	fMultip;			//数量倍率
};

//快速下单合约快捷键
struct ContractHotKey
{
	char			szKey[11];					//合约快捷键
	char			commodity[70];				//格式化之后的 固定格式的合约字符串 ZCE SR 305 ; ZCE SR 305 P4400  ;ZCE SR 305/307; ZCE SR 305 P4400/307 C4400
};

struct AbnormalTradeLimit//异常交易配置信息
{
	char			ZCEExchangeNo[11];
	//char			ZCEsign[21];
	char			DCEExchangeNo[11];
	//char			DCEsign[21];
	char			SHFEExchangeNo[11];
	//char			SHFEsign[21];
	char			CFFEXExchangeNo[11];
	//char			CFFEXsign[21];

	bool			bVerifySelfMatchZCE;		//验证郑商所自成交
	int				nSelfMatchTimesZCE;			//郑商所自成交限制数目
	bool			bVerifyCancelZCE;			//验证郑商所撤单次数
	int				nCancelTimesZCE;			//郑商所撤单限制次数
	int				nBigCancelNumZCE;			//郑商所大额撤单手数
	bool			bVerifyBigCancelZCE;		//验证郑商所大单撤单次数
	int				nBigCancelTimesZCE;			//郑商所大额撤单警戒次数

	bool			bVerifySelfMatchDCE;		//验证大商所自成交
	int				nSelfMatchTimesDCE;			//大商所自成交限制数目
	bool			bVerifyCancelDCE;			//验证大商所撤单次数
	int				nCancelTimesDCE;			//大商所撤单限制次数
	int				nBigCancelNumDCE;			//大商所大额撤单手数
	bool			bVerifyBigCancelDCE;		//验证大商所大单撤单次数
	int				nBigCancelTimesDCE;			//大商所大额撤单警戒次数

	bool			bVerifySelfMatchSHFE;		//验证上期所自成交
	int				nSelfMatchTimesSHFE;		//上期所自成交限制数目
	bool			bVerifyCancelSHFE;			//验证上期所撤单次数
	int				nCancelTimesSHFE;			//上期所撤单限制次数
	int				nBigCancelNumSHFE;			//上期所大额撤单手数
	bool			bVerifyBigCancelSHFE;		//验证上期所大单撤单次数
	int				nBigCancelTimesSHFE;		//上期所大额撤单警戒次数

	bool			bVerifySelfMatchCFFEX;		//验证中金所自成交
	int				nSelfMatchTimesCFFEX;		//中金所自成交限制数目
	bool			bVerifyCancelCFFEX;			//验证中金所撤单次数
	int				nCancelTimesCFFEX;			//中金所撤单限制次数
	bool			bVerifyOpenCFFEX;			//验证中金所开仓成交量
	int				nOpenMatchNumCFFEX;			//中金所开仓成交量限制数

	bool			bInsertOrderCheckSelfMatch;	//下单时判断自成交，并在有自成交可能时提示
	bool			bIgnoreOrder;				//判断自成交时忽略套利关联成交和市价单
	bool			bMaxWarnCount;				//最大警戒次数
	int				nMaxWarnCount;				//最大警戒次数
	bool			bForbidOper;				//达到警戒值时强制禁止同类操作

	bool            bShowExchangeCancelCount;	//显示按交易所统计的撤单次数总和
};
//快速下单 界面配置

//交易字段类型
enum TradeFieldType
{
	TFT_STRING,
	TFT_NUMERIC,
	TFT_DATE
};

//期货下单区域字段 //新增的只能往后加 不能从中间插入 否则会导致与老客户版本不兼容 中间插入导致IAF_ACCOUNT与配置文件保存的值不对应
enum FutureInputArea
{
	IAF_TJCHECK,			//条件选择区 不能参与排序
	IAF_ACCOUNT,			//客户账号
	IAF_COMMODITY,			//合约代码
	IAF_DIRECT,				//买卖
	IAF_OFFSET,				//开平
	IAF_PRICE,				//委托价格  后面加上 + 分子 /分母 四个 这四个要显示必须跟在委托价格之后
	IAF_PRICE_ADD,
	IAF_PRICE_FENZI,
	IAF_PRICE_CHUHAO,
	IAF_PRICE_FENMU,
	IAF_VOL,				//委托数量
	IAF_TRIGGERMODE,		//触发方式
	IAF_TRIGGERCONDITION,	//触发条件
	IAF_TRIGGERPRICE,		//触发价格
	IAF_VOLBL,				//数量倍率
	//IAF_STOPPRICE,			//止损价格
	//IAF_MATCHCONDITION,		//成交属性
	IAF_VALIDTIME,			//有效日期

	IAF_SELECODE,			//选择合约 始终显示
	IAF_TRIGGERPRICE_ADD,
	IAF_TRIGGERPRICE_FENZI,
	IAF_TRIGGERPRICE_CHUHAO,
	IAF_TRIGGERPRICE_FENMU,
	//IAF_ORDERPRICE_

	IAF_VOLCOUNT,		//下单次数  批次
	IAF_INCREVOL,		//数量增量

	IAF_INCREPRICE,			//价格增量 
	IAF_INCRERPRICE_CHUHAO,	//价格增量除号  显示必须跟在价格增量之后
	IAF_INCREPRICE_FENMU,	//价格增量分母  显示必须跟在价格增量除号之后

	IAF_ORDERTYPE,			//定单类型
	IAF_VALIDTYPE,			//有效类型

	IAF_MAXQTY,				//最大量 冰山
	IAF_MINQTY,				//最小量 冰山

	//IAF_BUTTON,				//设置按钮焦点所用
	//IAF_SUM 				//总数量，添加的字段请放置在前面
};
struct  FacadeSetting
{
	LOGFONT			lgBtnFont;			//按钮字体
	LOGFONT			lgCheckFont;		//CheckBox字体
	LOGFONT			lgStaticFont1;		//静态文本字体
	LOGFONT			lgStaticFont2;		//状态提示字体
	LOGFONT			lgComboFont;		//可编辑字体
	LOGFONT			lgPositionFont;		//持仓字体

	int				iLeftMargin;		//控件左边距
	int				iTopMargin;			//控件顶边距

	int				iHorCtrlInterval;	//水平控件间距
	int				iVerCtrlInterval1;	//垂直控件间距1 静态文本与水平控件的间距
	int				iVerCtrlInterval2;	//垂直控件间距2 水平控件与状态提示的间距
	int				iVerCtrlInverval3;	//垂直控件间距2 按钮之间的垂直间距

	COLORREF		rgbBkPosition;		//持仓的背景色
	COLORREF		rgbBuy;				//多头的持仓颜色
	COLORREF		rgbSell;			//空头的持仓颜色

	// 字段结构配置
	struct OrderField 
	{
		int		id;						// 字段编号，对应的TradeOrderField里的内容 FutureInputArea
		int		width;					// 显示宽度(像素数) 自动算出 跟配置无关
		int		height;					// 显示宽度(像素数) 自动算出 跟配置无关
		bool	bShow;					//是否显示
	};

	int				iOrderFieldCount;	//字段结构总数	
	OrderField		*pOrderField;		//字段结构指针

	unsigned int	iDefaultKbCfg;		//横向下单配置界面  //0==小字体 1==中字体 2==大字体
};


//交易->下单设置结构
struct GeneralOrderSetting
{
	bool			bAutoSelectPJPZ;			//平今平昨自适应

	bool			bLiquidateAutoCancel;		//平仓自动撤单
	unsigned int	nPCCDType;					//平仓撤单类型

	bool			bPKAutoKC;					//平开时超出平仓几个价位自动开仓
	int				nOverPCStep;				//超出平仓几个价位

	unsigned int	nCursorPos;					//价格光标位置(右起)
	bool			bReturnWT;					//下单后返回委托界面
	bool			bDbClickOrderCancel;		//双击排队单则撤销该笔委托
	bool			bUpDownEqualTab;			//上下键=Tab键
	bool			bLeftRightEqualTab;			//左右键=Tab键
	unsigned int	nWTOrderType;				//委托下单类型
	bool			bClickQuoteOrCCOrder;		//单击行情或持仓自动填单
	bool			bCommodityChangeAutoSelect;	//合约代码改变自动选中行情

	bool			bDbClickQuoteGetVol;		//双击行情获取买卖量
	unsigned int	nDefaultPriceType;			//默认价格
	int				nDefaultPriceOverStep;		//默认价格超价点数

	//2014/4/9
	bool			bOpenMaxAutoCancel;			//开满自动撤单

	//2014/9/9
	bool			bT1;//T+1
	//2014/10/21
	bool			bMatchTip;//收到新成交时弹出窗口提示

	bool			bPriceDynaChange;			//委托价格与行情同步刷新
	bool			bCommodityChangeGetPrice;	//合约代码改变获取价格  先屏蔽掉，与8.0合约切换设置配合使用
	bool			bSetWTFocus;				//设置委托窗口默认焦点
	unsigned int	nWTDefaultFocusCol;			//委托窗口默认焦点位于第几项
	bool			bDbClickQuoteOpen;			//双击行情是否填开平  iClickQuotOpen=0默认开仓 iClickQuotOpen=1自动开平
	unsigned int	iClickQuotOpen;				//配合bDbClickQuoteOpen使用
	bool			bAutoMarketPrice;			//委托价格为零自动下市价单
	bool			bOnlyFillActiveOrder;		//仅仅自动填单最后激活下单窗口 ?
	bool			bKuaiSu;					//是否快速下单

	bool			bMsgTip;//收到消息都弹出窗口提示
	bool			bNewMsgTip;//仅弹出最新消息
	bool			bOrderErrorTip;				//委托失败弹出提示

	unsigned int	iDBQuotDirect;//【买卖】填充方式 0(双击买入填卖出，双击卖出填买入) 1(双击买入填买入 双击卖出填卖出)
};
// 成交回报 下单人过滤 保存的TMatchSaveInfo 数量
struct TMatchTipQty
{
	struct TMatchSaveInfo
	{
		char UserNo[21];
	};

	unsigned int	iQty;
	TMatchSaveInfo	*pMatchSaveInfo;
};
//个性化->声音设置
struct SoundSetting
{
	bool	bQueue;
	bool	bPartFilled;
	bool	bFilled;
	bool	bCanceled;

	bool	bFail;
	bool	bTradeDisConnect;
	bool	bVipDisconnect;
	bool	bCounterMsg;
};

//交易->风险控制
struct GeneralRiskControl
{
	bool			bCancelConfir;				//撤单时弹出确认信息
	unsigned int	nOpenPriceScope;			//开仓时委价与最新价差范围点数
	unsigned int	nCoverPriceScope;			//平仓时委价与最新价差范围点数
	unsigned int	nSingleMaxOrderQty;			//单笔最大下单量

	//2014/10/24
	bool			bSaveLastQty;				//自动填单时保留上次的委托数量
	bool			bOrderTypeToLimit;			//下单后定单类型设为限价
	bool			bValidTypeToDay;			//下单后有效类型设为当日有效
	bool			bCancelFCover;				//下单后取消强平选项

	int				iOnlyCoverDay;				//下单合约临近只可平仓日弹出提示
	bool			bEmptyDirect;				//下单后清空买卖
	bool			bFastOrderConfir;			//快速下单需确认
	bool			bSuperPriceProtect;			//挂单超价保护
	unsigned int	nKSOpenPriceScope;			//快速开仓买卖价差范围点数
	unsigned int	nKSCoverPriceScope;			//快速平仓买卖价差范围点数
	bool			bKSDBclickSeleCode;			//快速下单双击切换合约

	bool			bOrderConfir;				//普通下单弹出确认窗口

	bool			bCancelT1;					//下单后取消T+1
};

//交易->条件单设置
struct ConditionOrderSet
{
	unsigned int	nTriggerMode;			//触发方式 最新价、买价、卖价
	unsigned int	nTriggerCondition;		//触发条件 止损模式、止赢模式

	unsigned int	nTriggerPrice;			//最新超价、对盘超价、挂单超价、不默认
	int				iTriggerPriceOverStep;	//超价点数
	bool			bTriggerPirctDyn;		//触发价格与行情同步刷新
};
//快速下单 和合约切换
struct CommodityInfo
{
	char			commodity[70];				//格式化之后的 固定格式的合约字符串 ZCE SR 305 ; ZCE SR 305 P4400  ;ZCE SR 305/307; ZCE SR 305 P4400/307 C4400
};
struct CommodityChange								
{
	bool			bChangeCode;//上下键切换合约

	int				iCommodityInfoCount;	//字段结构总数	
	CommodityInfo	*pCommodityInfo;
};

//交易 ->高级
struct TradeAdvance
{
	bool			bMoneyShortageAdjustCanOpen;//资金不足时开仓量自动调整为可开数量
	bool			bSingleOrderSplit;			//单笔委托大于 nSingleOrderSplitNum 时自动拆分
	unsigned int	nSingleOrderSplitNum;		

	bool			bCoverAutoNum;				//平仓数大于可平数时自动拆分为多笔
	bool			bIsShowPosition;			//是否显示持仓
	unsigned int	nPostionShowType;			//持仓显示类型 单笔持仓0 全部持仓1

	bool			bCancelBtn;					//隐藏下单窗口中买单、自动单、顺序按钮

	bool			bLightedShow;				//委托栏【买卖】【开平】高亮显示
	unsigned int	iCFFEXMarket;				//中金所市价撮合深度

	//2014/1/14
	bool			bOnlyOneOrderBtn;			//横向下单只显示一个按钮
	//2015/2/10
	bool			bReverFilter;				//IDC_CHECK_ReverFilter

	bool			bShowCmb;					//是否显示定单类型和有效类型

	bool            bShowFilter;				//交易列表显示过滤条件  默认true
};

#pragma	pack(pop)