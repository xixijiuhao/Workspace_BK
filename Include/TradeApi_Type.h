#ifndef _TRADEAPI_TYPE_
#define _TRADEAPI_TYPE_

typedef signed long long		int64;
typedef unsigned char			uchar;
typedef unsigned short			ushort;
typedef unsigned long int		uint;
typedef unsigned long long		uint64;


#pragma pack(push, 1)


//交易回调类型 CurrMaxValue = 58
enum TradeNotifyType
{
	tntNone					= 0,

	tntConnect				= 2,		//连上
	tntDisconnect			= 3,		//断开
	tntAuthenticate			= 56,		//认证
	tntLogin				= 5,		//登录信息 
	tntLoginInfo			= 6,		//登录信息更新 
	tntLogout				= 7,		//登出
	tntIniting				= 8,		//初始化中

	tntAgreementInfo 		= 9,		//协议内容
	tntConfirmState			= 10,		//协议确认状态

	tntUser					= 11,		//客户信息
	tntRight				= 12,		//权限信息

	tntBank					= 13,		//用户银行信息
	tntBankBalance			= 14,		//银行余额
	tntFutureBalance		= 55,
	tntTransfer				= 15,		//转账流水

	tntCfmmcToken			= 16,		//保证金监控中心令牌

	tntUpper				= 17,		//上手信息
	tntCurrency				= 18,		//币种信息
	tntExchange				= 19,		//市场信息
	tntCommodity			= 20,		//品种信息
	tntContract				= 21,		//合约信息
	tntCommodityOrderType	= 22,		//品种定单类型信息
	tntCommodityValidType	= 23,		//品种有效类型信息
	tnt3MExpDate			= 24,		//3M到期日

	tntDeepQuote			= 57,		//深度行情

	tntBill					= 25,		//帐单信息
	tntBillConfirm			= 26,		//账单确认
	tntUserProxy			= 27,		//关系人
	
	tntOrder				= 28,		//定单信息
	tntMatch				= 29,		//成交信息
	tntPosition				= 31,		//持仓明细
	tntPositionSub			= 32,		//持仓小计
	tntPositionTotal		= 33,		//持仓合计
	tntLiquidate			= 34,		//平仓信息
	tntMoney				= 35,		//资金信息
	tntMsg					= 36,		//柜台消息	
	tntEnquiry				= 37,		//询价通知
	tntPositionDeposit		= 38,		//持仓保证金

	tntTradableQty			= 39,		//客下单数量
	tntPreFreeze			= 40,		//下单预冻结资金
	tntChangePsw			= 41,		//修改密码
	tntCheckUserProxy		= 42,		//关系人认证

	tntPswValidDay			= 43,		//密码有效天数
	tntPswDate				= 44,		//密码有效期

	tntHisOrder				= 45,		//历史委托
	tntHisOrderProc			= 46,		//委托流程
	tntHisMatch				= 47,		//历史成交
	tntHisPosition			= 48,		//历史持仓
	tntHisDelivery			= 49,		//历史交割
	tntHisCash				= 50,		//历史出入金 历史资金调整

	tntOrderCheck 			= 51,		//定单审核信息
	tntErrorInfo			= 52,		//错误信息回报

	tntRefAllData			= 53,		//刷新所有数据, 历史数据(45~50)需要重新加载，实时数据只需刷新界面

	tntInventory			= 54,		//现货库存
	tntExchangeState		= 58,		//交易所状态
	tntCalcParam			= 59,		//计算参数
	tntOrderPriceRange		= 60,		//委价偏离范围
	
	tntPositionCommodity	= 61,		//品种持仓
	tntMatchSub				= 62,		//成交信息小计
	tntMatchTotal			= 63		//成交信息合计
};

//数据初始化进度编码
enum TradeInitProgress
{
	tipNone 					= 0,	
	
	tipAgreementSign			= 1,			//确认状态
	tipAgreementInfoQry 		= 2,			//确认信息查询
	tipAgreementInfoReady		= 3,			//确认信息查询应答完成
	tipSignAgreement			= 4,			//始化信息
	tipPswValidDay				= 5,			//密码有效天数
	tipPswDate					= 6,			//密码修改日期
	tipUser						= 7,			//客户信息
	tipRight					= 8,			//权限信息
	tipBank						= 9,			//用户银行信息
	tipCurrency					= 10,			//币种信息
	tipBillConfirm				= 11,			//结算单确认
	tipUpper					= 12,			//上手信息
	tipExchange					= 13,			//市场信息
	tipCommodity				= 14,			//品种信息
	tipContract					= 15,			//合约信息
	tipCommodityOrderType		= 16,			//品种定单类型信息
	tipCommodityValidType		= 17,			//品种有效类型信息
	tip3MExpDate				= 18,			//3M到期日
	tipOrder					= 19,			//委托信息
	tipMatch					= 20,			//成交信息
	tipPosition					= 21,			//持仓信息
	tipLiquidate				= 22,			//平仓信息
	tipMoney					= 23,			//资金信息
	tipMsg						= 24,			//消息查询
	tipLocalData				= 25,			//本地数据
	tipReady					= 26,			//所有数据初始化完毕
	tipDepositParam				= 27,			//保证金参数
	tipFeeParam					= 28,			//手续费参数
	tipOrderPriceRange			= 29			//委价偏离范围
};	

//客户权限			  		
enum TradeUserRight								
{						
	turNone						= 0,
	turUnDefine					= 1,			//未定义的权限
	turShfeDeepQuote			= 2,			//查上期深度行情

	//单客户权限			
	turQuickOrder				= 60001,		//快速下单
	turIcebergOrder				= 60002,		//冰山单
	turGhostOrder				= 60003,		//幽灵单

	//交易员权限			
	turEOrderUpdate				= 30001,		//电子单修改、删除
	turOrderMove				= 30002,		//定单转移、成交转移
	turSystemRemoveOrder		= 30003,		//系统删单
	turOrderInput				= 30004,		//定单录入
	turOrderCheck				= 30005			//定单审核
};

//内部错误编码定义
enum TradeErrorCode
{
	tecUnHoldAction				= 0xffff		//不支持该操作
};	
	
//数据过滤类型	
enum TFilterType	
{	
	ftNone						= 0,			//不过滤
	ftValid						= 1,			//有效数据，不包括被删除或转移的数据= , 也不包括历史的数据
	ftCanShow					= 2				//需要显示的数据，成交包括GTC历史成交
};

//空结构体
struct TNullStruct{};	

#pragma pack(pop)



//数据容器类
class IDataContainer
{
public:
	virtual uint __cdecl size() = 0;
	virtual const void * __cdecl at(uint i) = 0;
	virtual void __cdecl clear() = 0;
	virtual void __cdecl push_back(const void * data) = 0;			
};


#define EMPTY_STR				""								//空字符串
#define Currency_Base			"Base"							//基币的币种代码，好多计算依赖与此，不能和其他现有币种重复
#define Currency_RMB			"CNY"							//人民币比重
#define Currency_Pledge			"Pledge"						//质押币种
#define _3M						"3M"							//3M合约默认代码
#define INVAID_FLOAT			(-1e+100)						//无效的浮点数
#define FLOAT_SAME_ERRORDIFF	(1e-10)							//浮点数最大误差
#define KEY_LENGTH				(100)							//关键字空间长度
#define KEY_SPLIT				"^"								//关键字中的字段间隔符号
#define LocalVersion			(1)								//本地数据版本号
#define IsValidFloat(v)			((v) > INVAID_FLOAT && (v) < -INVAID_FLOAT)			//是否有效浮点数
#define SpreadCommodityFormat	"%s&%s"							//跨品种套利的商品格式


//内盘的几家交易所
#define exNone					"NONE"							//空
#define exZCE					"ZCE"							//郑商所
#define exSHFE					"SHFE"							//上期所
#define exCFFEX					"CFFEX"							//中金所
#define exDCE					"DCE"							//大商所
#define exSZSE					"SZSE"							//深交所
#define exSSE					"SSE"							//上交所
#define exSGE					"SGE"							//金交所
#define exINE					"INE"							//能交所



/**************************************************************************/
typedef char TBool;
#define	bYes					'Y'	//是
#define	bNo						'N'	//否

typedef char TServerType;
#define svtNone					'N' 
#define svtTradeServer			't' //交易服务器
#define svtFrontServer			'f' //前置机服务
#define svtNameServer			'n' //前端命名服务

//合约类型
typedef char TCommodityType;
#define ctUnDefine				'U' //未定义
#define ctNone				    'N'	//无效

#define	ctGoods					'G'	//现货
#define ctDefer					'Y' //现货延期

#define	ctFutures				'F'	//期货
#define ctSpreadMonth			'm'	//跨期套利
#define ctSpreadCommodity		'c'	//品种套利
#define ctSpreadLocal		    'L'	//本地套利
#define ctIndexNumber			'Z'	//指数
#define	ctOption				'O'	//期权
#define	ctBUL					'u'	//垂直看涨套利
#define	ctBER					'e'	//垂直看跌套利
#define	ctBLT					'l'	//水平看涨套利	
#define ctBRT					'r'	//水平看跌套利	
#define ctSTD					'd'	//跨式套利
#define ctSTG					'g'	//宽跨式套利
#define ctPRT					'P' //备兑组合

#define ctDirect				'D'	//外汇直接汇率
#define ctInDirect				'I' //外汇间接汇率
#define ctCross					'C' //外汇交叉汇率

#define ctStocks				'T'	//股票
#define ctStatics				'0'	//统计


//期权类型
typedef char TOptionType;
#define otNone					'N' //无
#define otCall					'C' //看涨
#define otPut					'P'	//看跌

//程序化操作类型
typedef char TProgramActionType;
#define atBuy					(0)	//买入
#define atBuyToCover			(1)	//买入平仓
#define atSellShort				(2)	//卖出开仓
#define atSell					(3)	//卖出平仓

//合约类型
typedef char TContractType;
#define ctTradeQuote			'1' //交易行情合约
#define ctQuote					'2' //行情合约
//合约类型
#define ctTrade					'1' //交易合约
#define ctQuote					'2' //行情合约
#define ctLocal                 '3' //本地合约

//交易所状态
typedef char TTradeState;
#define tsUnknown				'N' //未知状态
#define tsIniting				'I' //正初始化
#define tsReady					'R' //准备就绪
#define tsSwitchDay             '0' //交易日切换
#define tsBiding                '1' //竞价申报
#define tsMakeMatch             '2' //竞价撮合
#define tsTradeing				'3' //连续交易
#define tsPause					'4' //交易暂停
#define tsClosed				'5' //交易闭市
#define tsBidPause				'6' //竞价暂停
#define tsGatewayDisconnect		'7' //报盘未连
#define tsTradeDisconnect		'8' //交易未连
#define tsCloseDeal				'9' //闭市处理


//价差买卖关键合约 
typedef char TSpreadDirect;
#define dfNone					'0' //无
#define dfFirst					'1' //以第一腿为准
#define dfSecond				'2' //以第二腿为准

//行情数据类型
#define PRD_DYNA				'0' //分时
#define PRD_TICK				'1' //明细
#define PRD_SECOND				'2' //秒
#define PRD_MIN					'3' //分钟
#define PRD_DAY					'5' //日
#define PRD_WEEK				'6' //周
#define PRD_MONTH				'7' //月
#define PRD_YEAR				'8' //年

//登陆类型
typedef char TLoginType;
#define ltPtl					'P' //协议登陆
#define ltApi					'A' //API登陆
#define ltMob					'M'	//手机登陆

//定单类型
typedef char TOrderType;
#define otUnDefine				'U' //未定义
#define	otMarket				'1'	//市价单
#define	otLimit					'2'	//限价单
#define	otMarketStop			'3'	//市价止损
#define	otLimitStop				'4'	//限价止损
#define otExecute				'5' //行权
#define otAbandon				'6' //弃权
#define otEnquiry				'7' //询价
#define otOffer					'8' //应价
#define otIceberg				'9' //冰山单
#define otGhost					'A' //影子单
#define otSwap					'B' //互换
#define otSpreadApply			'C' //套利申请
#define otHedgApply				'D' //套保申请
#define otOptionAutoClose		'F' //行权前期权自对冲申请
#define otFutureAutoClose		'G' //履约期货自对冲申请
#define otMarketOptionKeep		'H' //做市商留仓

//策略类型
typedef char TStrategyType;
#define stNone					'N' //无
#define stPreOrder				'P' //预备单(埋单)
#define stAutoOrder				'A' //自动单
#define stCondition				'C' //条件单
//这两种定单组合使用可以达到2.0预约单的效果
#define stAttach				'a' //附加定单
#define stOCO					'o' //二择一

//有效类型
typedef char TValidType;
#define vtNone					'N' //无
#define	vtFOK					'4'	//即时全部
#define	vtIOC					'3'	//即时部分
#define vtGFS					'5' //本节有效
#define	vtGFD					'0'	//当日有效
#define	vtGTC					'1'	//长期有效
#define	vtGTD					'2'	//限期有效
//#define	vtNDF					'3'	//下日首节有效(HK)

//委托来源
typedef char TOrderWay;
#define owUndefine				'U' //未定义
#define	owAll					'A'	//所有
#define	owETrade				'E'	//E-Trade
#define	owProxyETrade			'P'	//代理单
#define	owJTrade				'J'	//J-Trade
#define	owManual				'M'	//人工单
#define	owCarry					'C'	//Carry单
#define	owDelivery				'D'	//交割行权
#define	owProgram				'S' //程式单	
#define owExecute				'e' //行权
#define owAbandon				'a' //弃权
#define owChannel				'c' //通道费
#define owRTS					'R' //RTS
#define owAPI					'I' //API

#define owSpreadSyn				'Y' //同步套利
#define owSpreadSequ			'u' //先后套利
#define owSpreadButterfly		'T' //蝶式套利
#define owLocalPrepare			'O'	//本地埋单
#define	owHorz					'H'	//横向下单
#define	owVert					'V'	//竖向下单
#define	owClickPrice			'L'	//点价下单
#define	owCard					'r' //卡片下单	
#define owBrtch					'B' //批量下单
#define owFast					'F' //快速下单
#define owPingPong				'p' //乒乓
#define	owStopLoss				's' //止损
#define	owStopProfit			'o' //止盈
#define owFollow				'f' //跟单
#define owCatch					't' //追单
#define owClear					'l' //一键清仓
#define owDrawLine				'd' //画线下单
#define owMouseCove				'm' //鼠标平仓
#define owQuoteOrder			'q' //行情下单

//标记
typedef char TOrderMark;
#define omNone					'N'
#define omAttention				'A' //关注
#define omIgnore				'I' //忽略
#define omPass					'Y' //审核通过
#define omNopass				'F' //审核未通过

//买卖
typedef char TDirect;
#define dNone					'N'
#define	dBuy					'B'	//买入
#define	dSell					'S'	//卖出
#define	dBoth					'A'	//双边

//开平
typedef char TOffset;
#define	oNone					'N'	//无
#define	oOpen					'O'	//开仓
#define	oCover					'C'	//平仓
#define	oCoverT					'T'	//平今
#define oOpenCover				'1' //开平，应价时有效, 本地套利也可以
#define oCoverOpen				'2'	//平开，应价时有效, 本地套利也可以

//投保标记
typedef char THedge;
#define	hNone					'N'	//无
#define	hSpeculate				'T'	//投机
#define	hHedge					'B'	//套保
#define hSpread					'S' //套利
#define hMarket					'M' //做市

//交易时段
typedef char TTradeSection;
#define tsDay					'D' //白天交易时段
#define tsNight					'N' //晚上（T+1）交易时段
#define tsAll					'A' //全交易时段

//报单状态
typedef char TOrderState;
#define osNone					'N' //无
#define	osSended				'0'	//已发送
#define	osAccept				'1'	//已受理
#define osTriggering			'2'	//待触发
#define	osActive				'3'	//已生效
#define	osQueued				'4'	//已排队
#define	osPartFilled			'5'	//部分成交
#define	osFilled				'6'	//完全成交
#define	osCanceling				'7'	//待撤
#define	osModifying				'8'	//待改
#define	osCanceled				'9'	//已撤单
#define	osPartCanceled			'A'	//已撤余单
#define	osFail					'B'	//指令失败
#define osChecking				'C' //待审核
#define	osSuspended				'D'	//已挂起
#define	osApply					'E'	//已申请
#define osInvalid				'F' //无效单
#define osPartTriggered			'G' //部分触发
#define osFillTriggered			'H' //完全触发
#define osPartFailed			'I' //余单失败


//本地套利定单状态
#define osPaired				'J'//已配对
#define osPairing				'K'//配对中
#define osUnpaired				'L'//有瘸腿

//帐单类型
typedef char TBillType;
#define btDay					'D'	//日结单
#define btMonth					'M'	//月结单

//账单格式类型
typedef char TBillFormatType;
#define bftText					'T' //纯文本
#define bftHtml					'H'	//网页
#define bftPdf					'P'	//Pdf
#define bftExcel				'E'	//Excel
#define bftRtf					'R'	//rtf
#define bftLink					'L'	//链接
#define bftDoc					'D'	//doc
#define bftCsv					'C'	//Csv
#define bftDsv					'd'	//Dsv
#define bftPng					'p'	//Png

//用户身份
typedef char TUserIdentity;
#define uiNone					'n'
#define uiUnDefine				'u' //未定义
#define	uiUser					'c'	//单客户
#define uiProxy					'd' //下单人
#define	uiBroker				'b'	//经纪人
#define	uiTrader				't'	//交易员
#define	uiQUser					'q'	//行情客户

//平仓方式
typedef char TCoverMode;
#define	cmNone					'N'	//不区分开平
#define	cmUnfinish				'U'	//平仓未了解
#define	cmCover					'C'	//开仓、平仓
#define	cmCoverToday			'T'	//开仓、平仓、平今

//行权类型
typedef char TExcuteType;
#define etAMERICAN				'1' //美式期权
#define etEUROPEAN				'2' //欧式期权

//交割方式
typedef char TDeliveryMode;
#define	dmGoods					'G'	//实物交割
#define	dmCash					'C'	//现金交割
#define	dmExecute				'E'	//期权行权
#define	dmAbandon				'A'	//期权弃权

//持仓保证金计算方式
typedef char TDepositMode;
#define	dmNormal				'N'	//正常
#define	dmClean					'C'	//合约净持仓
#define	dmLock					'L'	//品种锁仓

//密码类型
typedef char TPswType;
#define	ptTrade					'T'	//交易密码
#define	ptQuote					'Q'	//行情密码
#define	ptAuth					'A'	//代理下单密码
#define	ptMoney					'M'	//资金密码

//成交方式
typedef char TMatchMode;
#define	mmNormal				'N'	//正常
#define	mmUpdate				'U'	//更新委托
#define	mmOther					'O'	//其它

//交易账号类型
typedef char TTradeNoTyep;
#define	ttNormal				'N'	//常规
#define	ttSpeculation			'T'	//投机
#define	ttHedging				'B'	//保值
#define	ttSpread				'S'	//套利

//保证金计算方式
typedef char TDepositMode;
#define	dmRatio					'1'	//比例
#define	dmQuota					'2'	//定额
#define	dmDiffRatio				'3'	//差值比例
#define	dmDiffQuota				'4'	//差值定额
#define	dmDisCount				'5'	//折扣

//上手状态
typedef char TUpperType;
#define usUnInit				(0)	//未初始化
#define usInited				'0'	//已就绪
#define usLogined				'1'	//已登录
#define usConnected				'2'	//已连接
#define usDisconnected			'3'	//已断开

//出入金类型
typedef char TCashType;
#define ctOut					'O' //出金
#define ctIn					'I' //入金
#define ctAdjust				'A' //资金调整

//出入金方式
typedef char TCashMode;
#define cmNone					'N'
#define cmAmount				'A' //转帐
#define cmCheck					'B' //支票
#define cmCash					'C' //现金
#define cmExchange				'E' //换汇
#define cmTransfer				'T' //银期

#define cmFeeAdjust				'F' //佣金调整
#define cmProfitAdjust			'P' //盈亏调整
#define cmPledge				'L' //质押资金
#define cmInterestre			'I' //利息收入

//审核状态
typedef char TCheckState;
#define csNone					'N'
#define csWaiting				'W' //待审核
#define csPass					'Y' //已通过
#define csFail					'F' //未通过
#define csTransing				'T' //已在途

//定单审核模式
typedef char TCheckMode;
#define cmNone					'N' //不审核
#define cmAutoCheck				'A' //自动审核
#define cmManualCheck			'M' //人工审核

//成交删除，隐藏标记
typedef char TDeleteType;
#define	dtYes					'Y'	//是
#define	dtNo					'N'	//否
#define dtHide					'D'	//过期的成交单

//消息查询类型
typedef char TMsgQryType;
#define mqtAll					'A'	//全部
#define mqtOnlyValid			'E' //仅有效

//消息类型
typedef char TMessageType;
#define mtQuote					'Q'	//行情消息
#define mtTrade					'T' //交易消息

//消息级别
typedef char TMessageLevel;
#define mlInfo					'I'	//提示
#define mlWarning				'W' //警告
#define mlError					'E' //错误
#define mlVital					'V' //重要
#define mlUrgent				'U' //紧急

//价格精度取类型
typedef char TPrecisionType;
#define ptDefault				'D'	//默认
#define ptSpread				'S'	//价差
#define ptCarry					'C'	//Carry
#define ptAverage				'A'	//均价
#define ptOptionRiskParam		'O' //期权风险参数

//询价者
typedef char TEnquirerType;
#define eExchange				'M' //交易所请求
#define eUser					'P' //会员客户请求

//客户类型
typedef char TUserType;
#define utPersonal				'P' //个人
#define utOrgan					'O' //机构
#define utMarketMaker			'M' //做市商

//用户关系人类型
typedef char TUserProxyType;
#define uptNone					'N'
#define uptTrade				'T' //指定下单人
#define uptCash					'C' //资金调拨人
#define uptBillVerify			'S' //结算单确认人
#define uptOpenAccount			'O' //开户授权人
#define uptAgencyTrade			'A' //委托交易人
#define uptAssetsManagement		'M' //资产管理人
#define uptUnionAccount			'U' //联名账户
#define uptBeneficiary			'B' //最终受益人
#define uptUSAuthperson			'P' //美国授权人
#define uptUSBeneficiary		'E' //美国受益人
#define uptAuthSign				'I' //授权签字人

//期货资金算法
typedef char TFuturesAlg;
#define faTradeByTrade			'1'	//逐笔
#define faMarketByMarket  		'2'	//盯市

//期权资金算法	
typedef char TOptionAlg;
#define oaFutures				'1'	//期货方式
#define oaOption 				'2'	//期权方式

//socket断开原因 reason
typedef int TSocketDisconnectReason;
#define sdrInitiative			(1) //主动断开
#define sdrPassive				(2) //被动断开
#define sdrReadError			(3) //读错误
#define sdrWriteError			(4) //写错误
#define sdrBufFill				(5) //缓冲区满
#define sdrAsynError			(6) //异步操作错误
#define sdrParseError			(7) //解析数据错误
#define sdrConnectOuttime		(8) //连接超时

//数据订阅类型
typedef char TDataSubscibeType;
#define dstSubscibe				'S' //订阅
#define dstCancel				'C' //取消
#define dstQuery				'Q' //查询

//转账方向
typedef char TTransDirect;
#define tdNone					'N' //未定义
#define tdToFutures				'I' //银行->期货(转入)
#define tdToBank				'O' //期货->银行(转出)

//转账状态
typedef char TTransState;
#define tsNone					'N' //未定义
#define tsSend					'R' //已发出
#define tsTransing				'T' //转账中
#define tsTransed				'S' //转账成功
#define tsTransFail				'F' //转账失败
#define tsReversing				'r' //冲正中
#define tsReversed				's' //冲正成功
#define tsReversFail			'f' //冲正失败,此状态多余

//转账发起方
typedef char TTransInitiator;
#define tiNone					'N' //未定义
#define tiFutures				'F' //期货
#define tiBank					'B' //银行

//触发模式
typedef char TTriggerMode;
#define tmNone					'N' //无
#define tmLatest				'L' //最新价
#define tmBid					'B' //买价
#define tmAsk					'A' //卖价

//触发条件
typedef char TTriggerCondition;
#define tcNone					'N' //无
#define tcGreater				'g' //大于
#define tcGreaterEqual			'G' //大于等于
#define tcLess					'l' //小于
#define tcLessEqual				'L' //小于等于


//数据合计类型
typedef char TTotalType;
#define ttDetail				'D' //明细
#define ttPrice					'P' //按价格合计
#define ttContract				'C' //按合约合计

//持仓类型
typedef char TPositionType;
#define ptTotal					'T' //持仓合计
#define ptSub					'S'	//持仓小计
#define ptDetail				'D'	//持仓明细

//持仓保证金版本
typedef char TDepositVertion;
#define dvEsunny3				(3) //3.0
#define dvEsunny9				(9) //9.0
#define dvDaystar				'D' //Daystar
#define dvCTP					(1) //1.0

//定单关联信息
typedef char TLinkType;
#define ltCheckInfo				'C' //定单审核信息

//FATCA状态
typedef char TFATCAState;
#define fsAmerican				'0' //美国人
#define fsUnAmerican			'1' //非美国人
#define fsNoIntention 			'2' //不配合-无迹象
#define fsWilling				'3' //不配合-有迹象
#define fsTempUser				'4' //临时户
#define fsNoApplicable			'5' //不适用
#define fsPFFI					'A' //PFFI	
#define fsDCFFI					'B' //DCFFI	
#define fsODFFI					'C' //ODFFI	
#define fsUSE					'D' //USE	
#define fsUSEBO					'E' //USEBO	
#define fsPNFFE					'F' //PNFFE	
#define fsEE					'G' //EE	
#define fsNPFFI					'H' //NPFFI	

//交易平台
typedef char TTradePlatFrom;
#define tpEsunny				'E' //esunny
#define tpJTrade				'J' //jtrade
#define tpPats					'P' //PATS
#define tpSP					'S' //SP

//税表种类
typedef char TTaxType;
#define ttW8					'8' //W-8
#define ttW9					'9' //W-9

//委价偏离范围类型
typedef char TRangeType;
#define rtRate					'R' //百分比
#define trValue					'V' //数值

//委价偏离范围处理类型
typedef char TActionType;
#define atWarn					'W' //警告
#define atReject				'R' //拒绝

//价格类型
typedef char TPriceType;
#define ptNone					'n'
#define ptMatch					'm' //对盘超价
#define ptWorking				'w' //挂单超价
#define ptLast					'l' //最新超价
#define ptUnLimit				'u' //反向停板
#define ptMarket				'a' //市价


#endif