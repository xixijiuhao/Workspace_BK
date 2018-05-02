#pragma once

#ifdef STRUCT_FUNCTION
//trade_struct.h里重新定义了这里的所有结构体，且其名称和内存布局都和这里的一样，只是为了调用方便增加了成员函数
#include "trade_struct.h"
#else

#include "TradeApi_Type.h"


#pragma pack(push, 1)


//交易后台基本信息
struct TRawTradePlugin
{
	char					PluginNo[21];
	char					Version[21];
	char					PluginName[31];
};

//key=Sing
struct TAddress
{
	char					Sign[21];					//表示服务器表示，以ip，port为依据, 在多账户多后台情况下使用						
	char 					IP[40];						//留长一点，以便兼容域名地址格式
	ushort					Port;
	TServerType				ServerType;					//登录的服务器类型
};

//连接、断开			
struct TConnect : public TAddress
{
	char					SiteAddrNo[21];				//站点编号
	double					LoadValue;					//负载值
	//应答字段			
	char					LocalIP[40];
	ushort					LocalPort;
	char					LocalMac[24];
};

//用户修改密码和初始化回调时使用			
struct TLoginBase : public TConnect
{
	char					CompanyNo[11];				//公司编号
	char					BrokerNo[11];				//经纪公司编号
	char					LoginUser[21];				//登录账号
	TPswType				ChangePswType;				//上次修改的密码类型
};

//登录信息
//key=LoginUser+Sign		
struct TLogin : public TLoginBase
{
	char 					LoginPsw[31];
	char 					InitPsw[31];				//强制初始化密码
	char 					CaPsw[31];					//CA密码
	char					LoginClassName[51]; 		//多后台的交易类名

	//应答字段	
	TUserIdentity 			UserType;
	char 					LoginName[21];
	union
	{
		char 				QuoteTempPsw[51];
		struct
		{
			char			GroupNo;					//投资者分组
			double			Principal;					//本金
			double			PriorityFund;				//优先资金
			//曆史上最高的三個單日盈利
			double			HisDayProfit1;				//歷史單日盈利1
			double			HisDayProfit2;				//歷史單日盈利2
			double			HisDayProfit3;				//歷史單日盈利3
		};	
	};
	char 					PreLoginIP[40];
	ushort 					PreLoginProt;
	char 					PreLoginTime[21];
	char 					PreLogoutTime[21];
	char 					PreLoginAddress[201];		//上次登录机器信息
	char 					TradeDate[11];
	char 					LastSettleTime[21];
	char 					StartTime[21];
	char 					InitTime[21];
	char 					CurrDateTime[25];			//系统当前时间(客户端校时)精确到毫秒

	//本地字段			
	TradeInitProgress		InitProgress;				//最新初始化进度
	TBool					IsInitPsw;					//是否需要强制修改密码，bYes:bNo, 登录应答时中使用，修改成功后会自动变为bNo
	TBool 					Logined;					//是否已登录
	uint 					ReadyCount;					//该用户的初始化成功次数
	TradeUserRight			Rights[16];					//用户权限表
	uint					LocalDataVersion;			//本地数据版本号，读写本地文件时使用
	uint					PswValidDay;				//客户密码有效天数
	char					PswDate[21];				//客户密码修改日期
	TBool					ConfirmState;				//是否已经进行过风险确认
	TBool					LastQryed;					//最后一次查询是否完成

	//新增字段
	ushort					PortEx;						//查询端口
	char					LinkType[21];				//链接方式 tcp ssl ...
};

//交易所关键字
//key=Sign+ExchangeNo
struct TExchangeKey
{
	char					Sign[21];
	char					ExchangeNo[11];
};

//商品关键字
//key=Sign+ExchangeNo+CommodityNo+CommodityType
struct TCommodityKey : public TExchangeKey
{
	TCommodityType			CommodityType;
	char					CommodityNo[11];
};

//单腿合约关键字
//key=Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType
struct TContractSingleKey : public TCommodityKey
{
	char					ContractNo[11];				//月份
	char					StrikePrice[11];			//执行价
	TOptionType				OptionType;					//买卖权 TOptionType
};

//合约关键字
//key=Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType+ContractNo2+StrikePrice2+OptionType2
struct TContractKey : public TContractSingleKey
{
	char					ContractNo2[11];			//月份2
	char					StrikePrice2[11];			//执行价2
	TOptionType				OptionType2;				//买卖权2 TOptionType
};

//市场状态
//key=ExchangeNo+CommodityNo
struct TExchangeState : public TExchangeKey
{
	TCommodityType			CommodityType;
	char					CommodityNo[10];
	char					ExchangeDateTime[21];
	TTradeState				TradeState;
};

//上期所深度行情
struct TDeepQuote : public TContractKey
{
	TDirect					Direct;						//买卖方向
	double					Price;						//价格
	uint					Qty;						//数量
	TBool					IsLast;						//是否最后一笔
};

//定单关联信息查询请求
struct TOrderLinkInfoReq
{
	uint					OrderID;
	TLinkType				LinkType;					//关联信息类型
};

struct TOrder;
//定单审核			
//key=OrderID			
struct TOrderCheck
{
	uint					OrderID;
	TCheckMode				CheckMode;					//审核类型
	TCheckState				CheckState;					//审核状态
	char					CheckInfo[51];				//审核信息
	char					CheckerNo[21];				//审核操作员
	char					CheckTime[21];				//审核时间

	const TOrder *			pOrder;
};

//下单 根据sign和userno可以找到loginUser，根据LoginUser和Sign可以确定连接信息，进而通过此连接发出委托
//是否要发出委托交给ITrans的实例去判断吧
//本地套利时 commodityType表示本地套利第一腿的品种类型，本地套利类型填在策略类型字段中
struct TSendOrder : public TContractKey
{
	char					UserNo[21];
	TOrderType				OrderType;					//定单类型 
	TOrderWay				OrderWay;					//委托来源 
	TValidType				ValidType;					//有效类型 
	char					ValidTime[21];				//有效日期时间(GTD情况下使用)
	TBool					IsRiskOrder;				//风险报单 
	TDirect					Direct;						//买卖方向 
	TOffset					Offset;						//开仓平仓 或 应价买入开平 
	THedge					Hedge;						//投机保值 
	double					OrderPrice;					//委托价格 或 期权应价买入价格
	double					TriggerPrice;				//触发价格
	TTriggerMode			TriggerMode;				//触发模式
	TTriggerCondition		TriggerCondition;			//触发条件
	uint					OrderQty;					//委托数量 或 期权应价数量
	uint					MinMatchQty;				//最小成交量	
	uint					MinOrderQty;				//冰山单最小随机量
	uint					MaxOrderQty;				//冰山单最大随机量

	TOffset					SellOffset;					//应价卖出开平 
	double					SellOrderPrice;				//应价卖出价格
	uint					SellOrderQty;				//应价卖出委托数量 
	THedge					SellHedge;					//应价卖出投保方向
	uint					StayTime;					//应价停留时间（秒）
	char					EnquiryNo[21];				//询价请求号，应价时用

	TStrategyType			StrategyType;				//策略类型

	char					Remark[51];					//下单备注字段，只有下单时生效。如果需要唯一标识一个或一组定单，最好以GUID来标识，否则可能和其他下单途径的ID重复
	TTradeSection			AddOneIsValid;				//T+1时段有效(仅港交所)
	uchar					MarketLevel;				//市价撮合深度,目前只有中金所支持，取值为0、1、5
	
	//增加
	uint					ParentOrderID;				//父单号,可以是TOrder::OrderID也可以是TSpreadOrder::OrderID等
	TBool					FutureAutoCloseFlag;		//行权后期货自对冲标记
};

//定单修改
struct TModifyOrder : public TSendOrder
{
	uint					OrderID;					//报单在客户端的编号，即应答号
	char					ServerFlag;					//服务器标识
};

struct SCommodity;
//委托回报
//key1=orderid		
//key2=orderno+Sign		
struct TOrder : public TModifyOrder
{
	char					OrderNo[21];				//服务器编写的委托唯一编号
	char					LocalNo[21];				//本地号
	char					SystemNo[51];
	char					ExchangeSystemNo[51];		//交易所系统号
	char					TradeNo[11];				//交易编码
	
	char					UpperNo[11];				//使用上手(会员号)
	char					UpperChannelNo[11];			//上手通道号(席位号)

	char					InsertNo[21];				//下单人
	char					InsertDateTime[21];			//下单时间
	char					UpdateNo[21];				//最后一次变更人
	char					UpdateDateTime[21];			//最后一次变更时间

	TOrderState				OrderState;					//委托状态 

	double					MatchPrice;					//成交价
	uint					MatchQty;					//成交量
	uint					PreWorkingQty;				//上次排队量，动态计算排队数量时使用	

	double					SellMatchPrice;				//应价卖出成交量
	uint					SellMatchQty;				//应价买入成交量

	int						ErrorCode;					//最新信息码				
	char					ErrorText[51];				//最新错误信息

	TBool					IsInput;
	TDeleteType				Deleted;					//是否已删 //可否放在定单状态上
	TBool					AddOne;						//T+1 TBool
	TOrderMark				OrderMark;					//定单标记 omAttention， omIgnore

	uint					StreamID;					//委托流号

	char					Currency[11];				//商品币种
	char					FeeCurrency[11];			//手续费币种
	double					FrozenFee;					//冻结手续费
	double					FrozenDeposit;				//冻结保证金 合并计算的此项为

	//历史委托 需要自己更新pCommodity
	char					SettleDate[11];

	//新增--------------------------------------
	const SCommodity *		pCommodity;					//关联品种指针
	const TOrderCheck *		pCheckInfo;					//定单审核信息
};

//行权录入
struct TExecuteInput :public TOrder
{
	char					FutureContractNo[11];				//标的月份
};

//排队单数量，关键字为UserNo+SingleContractKey
//key=UserNo+Sign+ExchangeNo+CommodityNo+CommodityType+ContractNo+StrikePrice+OptionType
struct TOrderQty :public TContractSingleKey
{
	char					UserNo[21];

	//不区分开平的时候只有下面两个字段
	uint					BuyQty;
	uint					SellQty;

	//区分开平的时候下边字段才有效
	uint					BuyOpenQty;
	uint					BuyCoverQty;
	uint					BuyCoverDayQty;
	uint					SellOpenQty;
	uint					SellCoverQty;
	uint					SellCoverDayQty;
};

//询价通知
//key=EnquiryNo+Sign
struct TEnquiry : public TContractKey
{
	char					EnquiryNo[21];				//请求序号
	TDirect					ReqDirect;					//请求方向 
	TEnquirerType			ReqBy;						//请求方   
	char					EnquiryDateTime[21];		//询价时间
};

//成交回报 关键字需要调整，现在这样不能满足唯一标识的要求 交易所+上手+成交号+买卖
//key=MatchNo+Sign
struct TMatch :public TContractSingleKey
{
	char					MatchNo[21];				//成交序号

	char					UserNo[21];

	TOrderWay				MatchWay;
	TDirect					Direct;
	TOffset					Offset;
	THedge					Hedge;

	uint					StreamID;
	char					ServerFlag;
	char					OrderNo[21];				//关联定单编号
	char					ExchangeMatchNo[51];		//原始成交号
	char					MatchDateTime[21];
	char					UpperMatchDateTime[21];

	double					MatchPrice;
	uint					MatchQty;

	TDeleteType				Deleted;					//是否删除 

	char					Currency[11];				//商品币种
	TBool					ManualFee;					//手工手续费 
	char					FeeCurrency[11];			//手续费币种
	double					MatchFee;					//成交手续费
	double					Premium;					//权利金收支，收入为正，支出为负  = 成交价 * 成交量 * 乘数
	double					CoverProfit;				//平仓盈亏 或 未到期平赢

	char					SettleDate[11];				//历史成交用

	char					UpperNo[11];				//使用上手(会员号)
	TBool					AddOne;						//T+1标记 
	double					CoverPrice;					//平仓时优先平掉该价格的持仓
	//新增------------------------------------------------------------
	const TOrder *			pOrder;
	const SCommodity *		pCommodity;					//关联品种指针
};
//成交录入
struct TInputMatch :public TMatch
{
	TBool					IsCarry;					//是否carry单
	char					SystemNo[51];				//系统号		
};

//风险参数
struct TOptionRiskParam
{
	double					Delta;
	double					Gamma;
	double					Theta;
	double					Vega;
	double					Rho;
	double					IV;
};

//持仓保证金
//key=PositionNo, Sign
struct TPositionDeposit
{
	TDepositVertion			Version;					//版本号	

	char					Sign[21];
	char					PositionNo[51];				//持仓关键字

	double					PreSettle;					//前结算价	ver3
	double 					Deposit;					//保证金		ver3
	double					KeepDeposit;				//维持保证金	ver3

	double					LastPrice;					//最新价
	double					FloatProfit;				//浮盈		ver9
	double					FloatProfitTBT;				//逐笔浮赢 trade by trade
	double					OptionMarketValue;			//期权市值	ver9

	char					UpdateTime[9];				//更新时间
	double					UnderlyingPrice;			//标的最新价，取不到可以不填
};

//持仓明细
//key=PositionNo+Sign
struct TPosition : public TContractSingleKey
{
	char					PositionNo[51];

	char					UserNo[21];

	TDirect					Direct;
	THedge					Hedge;						//投保 

	uint					StreamID;
	char					ServerFlag;
	TBool					IsCmb;						//是否组合持仓
	char					MatchNo[21];				//成交关键字
	char					MatchDateTime[21];

	double					PositionPrice;				//价格
	uint					PositionQty;				//总持仓量	
	
	char					UpperNo[11];				//使用上手(会员号)
	char					CurrencyNo[11];

	double					ProfitCalcPrice;			//浮盈计算价
	double					FloatProfit;				//浮盈
	double					FloatProfitTBT;				//逐笔浮赢 trade by trade

	double					DepositCalcPrice;			//老仓保证金计算价（昨结算），新仓为成交价
	double					Deposit;					//客户初始保证金
	double					KeepDeposit;				//客户维持保证金			
	double					MarketValue;				//期权市值 = 最新价 * 持仓量 * 乘数
	char					DepositUpdateTime[9];		//保证金更新时间					

	uint					CanCoverQty;				//可平量(暂未使用)
	uint					ParOrderQty;				//平仓挂单量(暂未使用)

	char					SettleDate[11];				//历史持仓用	
	//add--------------------------------------		
	double					OptionValue;				//期权虚实值
	uint					PrePositionQty;				//昨仓数量
	double					StrikeProfit;				//期权执行盈亏 = (标的最新价 - 执行价) * 持仓量 * 乘数;
	double					Delta;
	double					Gamma;
	TPositionType			SourcePositionType;			//持仓来源的合计类型
	const SCommodity *		pCommodity;					//关联品种指针
	const TMatch *			pMatch;						//昨仓没有成交信息
};

typedef TPosition TSubPosition;
//typedef TPosition TTotalPosition;
struct TTotalPosition:TPosition{};

//所有合约总持仓的多头、空头
struct TAllPosition
{
	uint					Long;
	uint					Short;
};

//平仓回报
//key=LiquidateNo+Sign
struct TLiquidate : public TContractSingleKey
{
	char					LiquidateNo[21];			//平仓编号

	char					OpenMatchNo[21];			//开仓成交号
	char					CoverMatchNo[21];			//平仓成交号

	char					UserNo[21];					//资金帐号

	TDirect					CoverDirect;				//买卖 
	THedge					Hedge;						//投保 
	double					OpenPrice;					//开仓价
	double					CoverPrice;					//平仓价
	uint					CoverQty;					//数量

	uint					StreamID;
	char					OpenServerFlag;
	char					CloseServerFlag;

	char					Currency[11];				//品种币种	
	double 					Premium;					//权利金
	double					CloseProfit;				//平仓盈亏   盯市
	double					PreSettle;					//昨结算
	double					CloseProfitTBT;				//平仓总盈亏 开仓价 - 平仓价 逐笔
	double					UnExpProfit;				//未到期平盈
	//新增--------------------------
	const TMatch *			pOpenMatch;
	const TMatch *			pCoverMatch;
};

//用户信息
//key=UserNo+Sign		
struct TUserInfo
{
	char					Sign[21];
	char					LoginUser[21];				//登录账号         //查询多账户多后台数据的时候找到登陆用户再找具体数据

	char					UserNo[21];
	char					UserName[21];
	TUserType				UserType;					//用户类型，单客户、机构户
	TBool					IsPermittee;				//是否持证人
	TBool					IsMarketMaker;				//是否做市商

	char					FullName[51];				//全名
	char					UserEngName[51];
	char					Phone[51];					//电话
	char					IdentigyNo[21];				//证件号
	char					Address[101];				//地址
	char					Remark[182];				//备注
	char					SettleGroup[11];			//结算分组
	double					DepositThreshold;			//保证金阈值

	bool					QryMoneyed;					//资金是否定制成功
	bool					QryDeliveryed;				//是否已查过交割数据

	const TLogin *			pLogin;						//所属登录账号
};

//资金回报
//key=UserNo+CurrencyNo+Sign
struct TMoney
{
	char					Sign[21];
	char					UserNo[21];
	char					CurrencyGroupNo[11];		//币种组编号
	char					CurrencyNo[11];				//币种号(Currency_Base表示币种组基币资金)

	double					ExchangeRate;				//币种汇率

	TFuturesAlg				FutureAlg;					//期货算法
	TOptionAlg				OptionAlg;					//期权算法

	double					PreAvailable;				//昨可用
	double					PreBalance;					//昨账面
	double					PreEquity;					//昨权益
	double					PreMarketEquity;			//昨市值权益
	double					PreUnExpProfit;				//昨未结平盈
	double					PreLmeFloatExpProfit;		//昨lme浮盈

	double					Adjust;						//资金调整
	double					CashIn;						//入金
	double					CashOut;					//出金

	double					FrozenFee;					//冻结手续费20
	double					FrozenDeposit;				//冻结保证金19

	double					Fee;						//手续费(包含交割手续费)
	double					TradeFee;					//交易手续费17
	double					DeliveryFee;				//交割手续费17
	double					ExchangeFee;				//汇兑手续费

	double					PremiumIncome;				//权利金收取
	double					PremiumPay;					//权利金支付
	double					CoverProfit;				//平盈 盯市
	double					CoverProfitTBT;				//逐笔平盈
	double					DeliveryProfit;				//交割盈亏
	double					UnExpProfit;				//未结平盈
	double					ExpProfit;					//到期平仓盈亏

	double					FloatProfit;				//不含LME持仓盈亏,盯市 market to market
	double					LmeFloatProfit;				//LME持仓盈亏
	double					FloatProfitTBT;				//逐笔浮赢 trade by trade

	double					MarketValue;				//期权市值23

	double					Premium;					//权利金
	double					Deposit;					//保证金
	double					KeepDeposit;				//维持保证金
	double					Discount;					//LME贴现金额15

	double					AuthMoney;					//授信资金

	double					Balance;					//今资金
	double					Equity;						//今权益
	double					Available;					//今可用
	double					CanCashOut;					//今可提	
	double					AccoutMarketValue;			//账户市值			
	double					OriCash;					//币种原始出入金24(非自动汇兑资金)
	double					FrozenRisk;					//风险冻结

	//特殊产品------------------------
	double					PreFundPledgedIn;			//昨货币质入
	double					PreFundPledgedOut;			//昨货币质出
	double					FundPledgedIn;				//今货币质入
	double					FundPledgedOut;				//今货币质出
	double					FundCashPledged;			//货币质押余额
	double					PledgedableFund;			//可质押货币
	double					PositionPledged;			//仓单质押
	double					SpecDeposit;				//特殊产品保证金
	double					SpecFrozenDeposit;			//特殊产品冻结保证金
	double					SpecFee;					//特殊产品手续费
	double					SpecFrozenFee;				//特殊产品冻结手续费
	double					SpecFloatProfit;			//特殊产品浮盈
	double					SpecCoverProfit;			//特殊产品平盈

	//增加----------------------------
	double					RiskRate;					//风险率
	double					RiskRateDown;				//风险率倒数
	double					SelfRiskRate;				//自有风险率
	char					UpdateTime[11];				//更新时间
	double					NetProfit;					//净盈利
	double					ProfitRate;					//盈利率
	const TUserInfo *		pUser;
};

//历史出入金
struct TCash
{
	char					Sign[21];
	char					UserNo[21];
	TCashType				CashType;
	TCashMode				CashMode;
	char					CurrencyNo[11];
	double					OperAmount;
	char					Remark[101];

	char					UserBank[3];				//银行标识
	char					UserAccount[51];			//银行账户
	TBool					UsertAccountType;			//是否本币 本外币账户标识 
	char					CompanyBank[3];				//银行标识
	char					CompanyAccount[51];			//银行账户
	TBool					CompanyAccountType;			//是否本币 本外币账户标识 

	int						StreamID;
	int						RemoteStreamID;
	TCheckState				CheckState;
	char					OperDateTime[21];
	char					OperatorNo[11];
	char					CheckDateTime[21];
	char					CheckerNo[11];
	char					SettleDate[11];

	const TUserInfo *		pUserInfo;
};

//交割数据
//key=DeliveryNo+Sign
struct TDelivery : public TContractSingleKey
{
	char					DeliveryNo[21];

	char					OpenDate[11];				//开仓日期
	char					DelivDate[11];				//交割日期
	char					UserNo[21];					//客户
	char					OpenNo[51];					//结算用成交编号，日期 + 成交流号

	TOrderWay				MatchWay;					//成交来源 
	TDirect					OpenDirect;					//开仓方向 
	uint					DelivQty;					//交割数量
	uint					FrozenQty;					//冻结数量
	double					OpenPrice;					//开仓价格
	double					DelivPrice;					//交割价格
	double					Fee;						//手续费
	double					UpperFee;					//上手手续费
	double					DelivProfit;				//盈亏
	double					Deposit;					//保证金
	double					KeepDeposit;				//维持保证金
	double					UpperDeposit;				//上手保证金
	double					UpperKeepDeposit;			//上手维持保证金
	char					UpperNo[11];				//上手
	char					FeeCurrencyNo[11];			//手续费币种
	char					CurrencyNo[11];				//品种币种
	TDeliveryMode			DeliveryMode;				//交割方式 
	char					UnderlyingContractNo[11];	//期权标的合约（月份）
	//增加----------------------------
	const SCommodity *		pCommodity;					//关联品种指针
};

//账单查询
struct TBillQry
{
	char					UserNo[21];
	TBillType				BillType;
	char					BillDate[11];				//月账单和日账单显示的长度不一样
	TBillFormatType			FormatType;					//账单格式
};

//帐单回报
//key=UserNo+BillDate+Sign
struct TBill :public TBillQry
{
	char					Sign[21];
	int						ContentSize;				//正文大小
	char					Content[1];					//帐单正文
};

//key=LoginUser+Sign
struct TAgreementInfo
{
	char					Sign[21];
	char					LoginUser[21];
	int						ContentSize;				//正文大小
	char 					Content[1];					//帐单正文
};

//key=UserNo+BankNo+BankAccount+Currency+Sign
struct TBankBase
{
	char					Sign[21];
	char					UserNo[21];					//期货资金账号
	char					BankNo[11];					//银行标志
	char					BankAccount[51];			//银行卡号
	char					Currency[11];				//币种编号
};

//账户转账信息
struct TUserBank : public TBankBase
{
	char					BankName[51];				//银行名称
	TBool					SwapState;      			//转账换汇状态
	TBool					TransState;  				//转账状态
};

//银行余额查詢
struct TBankBalanceQry : public TBankBase
{
	char					TransAuth[21];				//银期转账认证密码
	char					BankPsw[21];				//银行密码
};

//银行余额查詢應答
struct TBankBalance : public TBankBase
{
	double					Balance;                    //可转资金
	uint					ErrorCode;					//错误码
	char	 				ErrorText[51];				//错误信息
};

//期货可转资金查询
struct TFutureBalanceQry
{
	char					Sign[21];
	char					UserNo[21];
	char					Currency[11];
};

//期货可转资金应答
struct TFutureBalance : public TFutureBalanceQry
{
	double					Balance;
};

//銀行轉賬請求
struct  TTransferReq : public TBankBalanceQry
{
	TTransDirect			TransDirect;				//转账方向
	double					TransFund;					//转账金额
};

//银期转账应答
//key=FutureSerialID+Sign
struct  TTransfer : public TBankBase
{
	TTransDirect			TransDirect;				//转账方向
	double					TransFund;					//转账金额
	TTransState				TransState;					//转账状态

	uint					BankSerialID;				//银行流水号
	uint					FutureSerialID;				//期货流水号
	char					TransTime[21];				//转账时间

	double					BankFee;					//手续费
	double					FutureFee;					//公司手续费

	uint					ErrorCode;					//错误码
	char	 				ErrorText[51];				//错误信息

	TTransInitiator 		Initiator;					//转账发起方
};

//保证金监控中心登录令牌
struct TCfmmcToken
{
	char					Sign[21];

	char					CompanyNo[11];				//期货公司代码(会员码)
	char					UserNo[21];					//资金账号
	uint					TokenID;					//令牌ＩＤ
	char					Token[21];					//令牌认证码

	uint					ErrorCode;					//错误码
	char	 				ErrorText[51];				//错误信息
};

//客户关系人
//key=UserNo+UserProxyNo+UserProxyType+Sign
struct TUserProxy
{
	char					Sign[21];
	char					UserNo[21];						//所属客户
	char					UserProxyNo[21];				//关系人
	char					UserProxyName[51];				//关系人名称
	TUserProxyType			UserProxyType;					//关系人类型
	char					IdentityType[21];				//证件类型
	char					IdentityId[51];					//证件ID
	char					IdExpireDate[11];				//证件到期日
	char					Cert[21];						//认证串
	char					CertExpireDate[11];				//CA到期日
	char					Country[11];					//地址的国家
	char					Province[21];					//地址的省/自治区/直辖市
	char					City[51];						//地址的市/县/区
	char					Address[101];					//地址
	char					PostNo[21];						//邮政编码
	char					TelPhoneNo[101];				//电话
	char					Birthday[11];					//生日
		
	//修订8
	char					CertificatesIssuedAddress[41];	//证件发行地
	TFATCAState				FATCAStatus;					//FATCA状态
	char					Nationality[41];				//国籍
	char					CountryOfResidence[41];			//居住地
	char					IsUSBeneficiary;				//是否有美国受益人
	char					IsUSTaxPayers;					//是否有美国缴税义务
	char					TINTaxNo[33];					//TIN税务号
	char					IsWaiver;						//是否取得Waiver
	TTaxType				TaxFormType;					//税表种类
	char					SignedTaxFromDate[11];			//签署税表日期
	char					MarginStandard[11];				//保证金水平
	TTradePlatFrom			TradePlatFrom;					//交易平台
	char					Fax[101];						//传真
};

//币种信息
//key=CurrencyNo+Sign
struct TCurrency
{
	char					Sign[21];

	char					CurrencyNo[11];
	char					CurrencyName[21];
	char					CurrencyGroupNo[11];		//币种组编号 
	TBool					IsShare;					//是否与其他币种共享计算 
	double					ExchangeRate;				//交易汇率
};

//可下单数量
struct TTradableQty : public TSendOrder
{
	uint					OpenQty;					//可开
	uint					CoverQty;					//可平

	int						MaxOrdID;					//原始定单号	判断是否重新下单
	double					Available;					//原始可用资金	判断可用资金是否发生变化
};

struct TTradableQtySell : public TTradableQty
{
};

//下单冻结资金
struct TPreFreeze : public TSendOrder
{
	double					Deposit;					//本币保证金

	int						MaxOrdID;					//原始定单号	判断是否重新下单
	double					Available;					//原始可用资金	判断可用资金是否发生变化
	//add	
	const TCurrency *		pCurrency;
};

struct TPreFreezeSell : public TPreFreeze
{
};

//上手
//key=UpperNo+Sign
struct TUpper
{
	char					Sign[21];

	char					UpperNo[11];
	char					UpperName[21];
};

//消息
//Key2=MsgID+Sign
struct TMessage
{
	char					Sign[21];
	uint					MsgID;
	char					UserNo[21];					//接收者
	TMessageType			Type;						//消息类型  包括错误
	TMessageLevel			Level;						//消息级别
	char					SendNo[11];					//发送者
	char					SendDateTime[21];			//发送时间
	char					ValidDateTime[21];			//有效时间
	char					Title[101];					//标题
	uint					ContentSize;
	char					Content[1];					//正文
};

//3M到期日,key=Sign
struct T3MEndDate
{
	char					Sign[21];
	char					ContractNo[11];
};

//历史交易查询: 历史委托、历史成交、历史出入金、历史资金调整
struct THisTradeQry
{
	char					UserNo[21];
	char					BeginDate[11];
	char					EndDate[11];
};

//错误信息
struct TErrorInfo
{
	char					Sign[21];					
	char					LoginUser[21];				
	char					module[21];
	TradeNotifyType			ErrorSource;	//错误来源
	int						error_id;
	char					error_text[51];	//错误信息
};

//委价偏离范围
struct TOrderPriceRange: public TCommodityKey
{	
	TRangeType				RangeType;	
	double					Range;
	TActionType				ActionType;
};

#pragma pack(pop)


#endif