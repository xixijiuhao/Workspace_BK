/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//system	: 交易分析客户端9.0
//company	: 郑州易盛信息技术有限公司
//file		: TradeApi_API.h
//brief		: 定义了客户端内部的交易操作，交易事件回调及交易数据访问的接口
//history	: 
//			  20141226	刘明祥	创建
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TradeApi_Struct.h"	


#define PLUG_TRADEAPI_NAME 			L"TradeApi"
#define PLUG_TRADEAPI_VERSION 		L"1.0.0.1"

#define WM_TradeData				(0x0400 + 10011)		//交易数据回调消息, 等同与WM_USER + 10011
struct SCommodity;
//可以删除历史数据通知接口，因为历史数据都是通过OnRefAllData通知的
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//交易事件回调接口，需要用户实现并在ITradeData类对象中调用RegNotify注册子类对象
//	该接口中只有bool IsWnd(uint &)函数有一个输出参数，其他函数的所有参数都是输入参数
//	回调方式可以是消息方式也可以是线程方式，由bool IsWnd(uint &)函数的实现决定，具体参见函数注释
class ITradeSpi
{
public:		
	//是否窗口，以确定是发消息还是用线程执行回调
	//输出参数：
	//	WinHwnd:实现该接口的窗口句柄，如果实现类不是一个窗口，则需要返回零
	//返回值：
	//	true：说明实现该接口的类是一个窗口类；false：实现该接口的类并非一个窗口类
	//注意：
	//	只有当该函数返回值为true且WinHwnd不等于零时才会以消息形式通知(需要自己解析并作消息路由)，不然则以线程形式直接调用通知函数
	virtual bool __cdecl IsWnd(uint & WinHwnd)=0;	

	//连接成功的通知
	//pLogin：返回了本地ip、port，mac等信息
	virtual void __cdecl OnConnect(const TLogin * pLogin)=0;	

	//连接断开的通知。pSender为断开连接的类。
	//	ErrorCode：错误代码
	//	pLogin: 被断开的登陆账号信息
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin)=0;			

	//用户登录回调。登录成功时通过pLogin返回登录信息。登录失败时pLogin为nullptr。
	//	ErrorCode: 错误代码，0:成功
	//	pLogin：登陆信息
	virtual void __cdecl OnLogin(const int ErrorCode, const TLogin * pLogin)=0;	

	//登录账户关联信息更新、如权限、密码有效期、风险确认状态等。
	//	pLogin: 登陆信息
	virtual void __cdecl OnLoginInfo(const TLogin * pLogin)=0;	

	//用户登出回调。
	//	pLogin：登出的账户信息
	virtual void __cdecl OnLogout(const int ErrorCode, const TLogin * pLogin)=0;	

	//初始化进度通知，初始化会在用户登录成功后进行。初始化完毕之前，用户不能进行交易。
	//	ErrorCode：错误号，0:成功
	//	CurrProgress: 当前回调发出时的初始化进度
	//	TLoginBase：登录信息者的基本信息，可据此查到TLogin
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)=0;

	//定单信息通知，定单的任何改变都会通过该函数通知用户
	virtual void __cdecl OnOrder(const TOrder * pOrder)=0;	

	//成交信息通知，成交的任何改变都会通过该函数通知用户
	virtual void __cdecl OnMatch(const TMatch * pMatch) = 0;

	//成交信息通知，成交小计的改变都会通过该函数通知用户
	virtual void __cdecl OnMatchSub(const TMatch * pMatch) = 0;

	//成交信息通知，成交合计的改变都会通过该函数通知用户
	virtual void __cdecl OnMatchTotal(const TMatch * pMatch) = 0;

	//持仓明细通知，持仓明细的任何改变都会通过该函数通知用户
	virtual void __cdecl OnPosition(const TPosition * pPosition) = 0;

	//持仓小计通知，持仓小计的任何改变都会通过该函数通知用户
	//持仓小计也叫价位持仓，是将持仓明细按资金账号相同、合约相同、价格相同的原则进行统计，显示合计数量
	virtual void __cdecl OnPositionSub(const TSubPosition * pPosition) = 0;

	//持仓合计通知，持仓合计的任何改变都会通过该函数通知用户
	//持仓合计也叫合约持仓，是将持仓明细按资金账号相同、合约相同的原则进行统计，显示均价和合计数量
	virtual void __cdecl OnPositionTotal(const TTotalPosition * pPosition) = 0;

	//平仓信息通知，平仓信息的任何改变都会通过该函数通知用户，包括增、删、改
	virtual void __cdecl OnLiquidate(const TLiquidate * pLiquidate)=0;	

	//资金信息通知，这个信息需要分资金账户订阅才能收到
	//	单客户初始化时已经自动订阅了，交易员需要在外部显示的通过ITradeApi::QryMoney()去订阅
	virtual void __cdecl OnMoney(const TMoney * pMoney)=0;	

	//柜台消息应答通知
	virtual void __cdecl OnMsg(const TMessage * pMsg)=0;

	//密码修改应答
	//	ErrorCode：错误代码，0表示成功
	//	pLoginBase：账户密码修改信息
	virtual void __cdecl OnChgPsw(const int ErrorCode, const TLoginBase * pLoginBase)=0;		

	//帐单信息查询应答
	//	pBill：结算单内容
	virtual void __cdecl OnBill(const TBill * pBill)=0;	

	//历史交割数据查询应答
	virtual void __cdecl OnHisDelivery(const TDelivery * pDelivery)=0;

	//历史委托查询应答，历史数据按结算日期查询
	virtual void __cdecl OnHisOrder(const TOrder * pHisOrder)=0;	

	//历史委托流程查询应答，包括当天委托的流程和历史委托的流程，历史数据按结算日期查询
	virtual void __cdecl OnHisOrderProc(const TOrder * pHisOrderProc)=0;

	//历史成交查询应答，历史数据按结算日期查询
	virtual void __cdecl OnHisMatch(const TMatch * pHisMatch)=0;	

	//历史出入金查询应答，包括当天历史的出入金和资金调整信息，历史数据按结算日期查询
	virtual void __cdecl OnHisCash(const TCash * pHisCash)=0;

	//询价通知，做市商使用
	//	pEnquiry：投资者发起的询价请求，做市商会据此信息进行应价处理
	virtual void __cdecl OnEnquiry(const TEnquiry * pEnquiry)=0;	

	//关系人信息查询应答，和某一资金账号相关的其他人员（如：资金调拨人、下单人等等）的信息
	virtual void __cdecl OnUserProxy(const TUserProxy * pUserProxy)=0;	

	//预收保证金数量查询应答
	//	pPreFreeze：包含了预收保证金的查询信息和应答内容
	//		TPreFreeze::Deposit 本币保证金
	virtual void __cdecl OnPreFrozen(const TPreFreeze * pPreFreeze) = 0;

	//可下单数量查询应答
	//	pTradableQty：包含了可下单数量的查询信息和应答内容
	//		TTradableQty::OpenQty	可开
	//		TTradableQty::CoverQty	可平
	virtual void __cdecl OnTradableQty(const TTradableQty * pTradableQty) = 0;		

	//错误回报
	//	pErrorInfo：包括错误ID和错误信息即原始数据通知类型
	virtual void __cdecl OnErrorInfo(const TErrorInfo * pErrorInfo) = 0;	

	//按类型刷新所有数据的通知，批量更新数据时使用
	//	DataType：要刷新的数据类型
	//注意：收到该通知，表示该类数据在ITradeData里已经有多条更新，但并没有逐笔通知前台，前台需要重新获取所有数据来更新本地
	virtual void __cdecl OnRefAllData(const TradeNotifyType DataType) = 0;	

	//银行可转资金查询应答
	virtual void __cdecl OnBankBalance(const TBankBalance * pBankBalance) = 0;

	//银期转账应答
	virtual void __cdecl OnTransfer(const TTransfer * pTransfer) = 0;

	//保证金监控中心认证信息查询应答
	virtual void __cdecl OnCfmmcToken(const TCfmmcToken * pCffmmcToken) = 0;

	//历史持仓查询应答
	virtual void __cdecl OnHisPosition(const TPosition * pPosition) = 0;

	//查询期货可转资金应答
	virtual void __cdecl OnFutureBalance(const TFutureBalance * pFutureBalance) = 0;

	//上期所深度行情查询回报
	virtual void __cdecl OnSHFEDeepQuote(const TDeepQuote * pDeepQuote) = 0;

	//交易所状态回报
	virtual void __cdecl OnExchangeState(const TExchangeState * pExchangeState) = 0;

	virtual void __cdecl OnOther(const uchar DataType, const void * pData) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//交易数据访问接口
//	因为会有多后台的情况，所以很多函数的输入参数需要Sign字段，用以区分不同的连接，
//	ITradeSpi::OnConnect会首次返回该连接的Sign
//	输出一组数据的的函数用到了IDataContainer接口，函数会将对应的数据填入其中返回个调用者
class ITradeData
{
public:	
	//注册交易数据通知组件，注册后这个数据通知组件就可以收到实时的交易数据变化了
	virtual void __cdecl Register(ITradeSpi * TradeSpi) = 0;

	//注消交易数据通知组件，注消后交易数据的实时变化就不会再通知到这个数据通知组件了
	virtual void __cdecl UnRegister(ITradeSpi * TradeSpi) = 0;


	//获得所有登陆用户的信息
	//输出参数：
	//	Datas	数据容器, 其数据元素的类型为 const TLogin *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllLogin(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得单个客户的登陆信息
	//输入参数：
	//	LoginUser: 登录账号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	登录信息，包括IP、权限、初始化状态、后台时间、用户类型等等
	virtual const TLogin * __cdecl GetLoginInfo(const char * LoginUser, const char * Sign) = 0;

	//让系统分配一个全局唯一的委托号
	virtual uint __cdecl NewOrderID() = 0;

	//获得所有满足条件的委托数据
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TOrder *
	//输入参数：
	//	filter: 数据过滤参数，参考TFilterType类型定义
	//	Sign: 交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllOrder(IDataContainer & Datas, const TFilterType filter = ftValid, const char * Sign = "") = 0;	

	//按定单号获得委托
	//输入参数：
	//	OrderID	定单号
	virtual const TOrder * __cdecl GetOrder(const uint OrderID) = 0;

	//按委托号查询定单
	//输入参数：
	//	OrderNo: 委托号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	委托信息
	virtual const TOrder * __cdecl GetOrder(const char * OrderNo, const char * Sign) = 0;	

	//获得满足条件的所有挂单
	//输出参数：
	//	Datas	输出数据的容器, 其数据元素的类型为 const TOrder *
	//输入参数：
	//	filter	数据过滤参数，参考TFilterType类型定义
	//	Sign: 交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetParOrder(IDataContainer & Datas, const TFilterType filter = ftValid, const char * Sign = "") = 0;	


	//按定单号或的父单
	//输入参数：
	//	OrderID: 定单号，冰山单、影子单会有父定单
	//返回值：
	//	定单信息
	virtual const TOrder * __cdecl GetParentOrder(const uint OrderID) = 0;

	//获得获得指定客户、合约、买卖的挂单数量, 只统计有效的委托
	//输入参数：
	//	UserNo:资金账号
	//	ContractKey:合约关键字
	//返回值：
	//	排队单的几种数量统计
	virtual const TOrderQty * __cdecl GetParOrderQty(const char * UserNo, const TContractSingleKey * ContractKey) = 0;	


	//获得满足过滤条件的所有成交信息
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TMatch *
	//输入参数：
	//	filter: 数据过滤参数，参考TFilterType类型定义
	//	Sign: 交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllMatch(IDataContainer & Datas, const TFilterType filter = ftValid, const char * Sign = "") = 0;	

	//获得满足过滤条件的所有成交小计
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TMatch *
	virtual void __cdecl GetAllSubMatch(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得满足过滤条件的所有成交合计
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TMatch *
	virtual void __cdecl GetAllTotalMatch(IDataContainer & Datas, const char * Sign = "") = 0;

	//根据成交号获得单笔成交信息
	//输入参数：
	//	MatchNo: 本地成交编号，不同于交易所成交号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	成交信息
	virtual const TMatch * __cdecl GetMatch(const char * MatchNo, const char * Sign) = 0;	

	//获得同一委托的所有成交信息
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TMatch *
	//输入参数：
	//	OrderID: 定单号
	//	filter: 数据过滤参数，参考TFilterType类型定义
	//	Sign: 交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetMatchs(const uint OrderID, IDataContainer & Datas, const TFilterType filter = ftValid) = 0;		


	//获得所有持仓明细，持仓明细与成交明细之间是正向1:1的关系
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TPosition *
	//	Sign: 交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllPosition(IDataContainer & Datas, const char * Sign = "") = 0;

	//根据持仓编号获得单笔持仓明细，持仓明细与成交明细之间是正向1:1的关系
	//输入参数：
	//	PositionNo: 持仓编号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	持仓明细
	virtual const TPosition * __cdecl GetPosition(const char * PositionNo, const char * Sign) = 0;	


	//获得所有的持仓小计，持仓小计是将持仓数量按同客户同合约同买卖同价格进行合计
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TPosition *
	//	Sign: 交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllPositionSub(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得单笔持仓小计，持仓小计是将持仓数量按同客户同合约同买卖同价格进行合计
	//输入参数：
	//	UserNo: 资金账号
	//	ContractKey: 单腿合约关键字
	//	Direct: 买卖方向
	//	Price: 持仓价格
	//返回值：
	//	持仓小计
	virtual const TPosition * __cdecl GetPositionSub(const char * UserNo, const TContractSingleKey * ContractKey, const TDirect Direct, const char * Price) = 0;


	//获得所有的持仓合计，持仓合计是将持仓价格和持仓数量按同客户同合约同买卖进行加权平均和合计
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TPosition *	
	//	Sign: 交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllPositionTotal(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得单笔持仓合计，持仓合计是将持仓价格和持仓数量按同客户同合约同买卖进行加权平均和合计
	//输入参数：
	//	UserNo: 资金账号
	//	ContractKey: 单腿合约关键字
	//	Direct: 买卖方向
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	持仓合计
	virtual const TPosition * __cdecl GetPositionTotal(const char * UserNo, const TContractSingleKey * ContractKey, const TDirect Direct) = 0;		


	//获得所有平仓信息
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TLiquidate *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllLiquidate(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得单笔平仓信息
	//输入参数：
	//	LiquidateNo：平仓编号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	平仓信息
	virtual const TLiquidate * __cdecl GetLiquidate(const char * LiquidateNo, const char * Sign) = 0;	


	//获得所有资金信息	
	//输出参数：		
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TMoney *	
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllMoney(IDataContainer & Datas, const char * Sign = "") = 0;																															

	//获得资金信息
	//输入参数：
	//	UserNo：资金账号
	//	CurrencyGrp：币种组，目前没用，可以填nullptr
	//	CurrencyNo：币种编号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	资金信息
	virtual const TMoney * __cdecl GetMoney(const char * UserNo, const char * CurrencyGrp, const char * CurrencyNo, const char * Sign) = 0;		


	//获得所有询价通知	
	//输出参数：		
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TEnquiry *	
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllEnquiry(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得询价通知
	//输入参数：
	//	EnquiryNo：询价号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	询价通知信息
	virtual const TEnquiry * __cdecl GetEnquiry(const char * EnquiryNo, const char * Sign) = 0;	


	//获得某客户指定合约的持仓数量
	//输入参数：
	//	UserNo		：资金账号
	//	ContractKey	：合约关键字
	//输出参数：
	//	LongCount	：多头总持仓数量
	//	ShortCount	：空头总持仓数量
	//	DayLongCount	：多头今仓数量
	//	DayShortCount	：空头今仓数量
	virtual void __cdecl GetPositionVol(const char * UserNo, const TContractSingleKey * ContractKey, uint & LongCount, uint & ShortCount, uint & DayLongCount, uint & DayShortCount) = 0;		
									

	//获得所有用户信息
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TUserInfo *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllUser(IDataContainer & Datas, const char * Sign = "") = 0;	
	//获得单个用户的信息
	//输出参数：
	//	UserNo		：资金账号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	用户信息
	virtual const TUserInfo * __cdecl GetUser(const char * UserNo, const char * Sign) = 0;	


	//获得所有关联客户
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TUserProxy *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllUserProxy(IDataContainer & Datas, const char * Sign = "") = 0;		

	//获得历史委托
	//输入参数：
	//	OrderNo: 委托号
	//	Date：交割日期，格式为yyyy-mm-dd
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	单笔历史委托
	virtual const TOrder * __cdecl GetHisOrder(const char * OrderNo, const char * Date, const char * Sign) = 0;	

	//获得所有历史委托
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TOrder *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllHisOrder(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得所有历史委托流程，包括当日委托流程
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TOrder *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllHisOrderProc(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得所有历史成交
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TMatch *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllHisMatch(IDataContainer & Datas, const char * Sign = "") = 0;	
	
	//获得所有历史持仓
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TPosition *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllHisPosition(IDataContainer & Datas, const char * Sign = "") = 0;	

	//获得所有历史交割
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TDelivery *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllHisDelivery(IDataContainer & Datas, const char * Sign = "") = 0;	

	//获得所有历史出入金, 包括资金调整信息
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TCash *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllHisCash(IDataContainer & Datas, const char * Sign = "") = 0;			

	//获得客户帐单
	//输入参数：
	//	UserNo:资金账号
	//	BillDate:交割日期，格式为yyyy-mm-dd
	//	BillType:账单类型
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	结算单信息
	virtual const TBill * __cdecl GetBill(const char * UserNo, const char * Sign, const char * BillDate, const TBillType BillType = btDay) = 0;	

	//获得所有交易所
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 char*
	//输入参数：
	//	Sing：交易后台表示符，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllExchange(IDataContainer & Datas, const char * Sign = "") = 0;																													
	//获得指定交易所下的所有品种
	//输入参数：
	//	ExchangeKey：交易所关键字
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 char*
	virtual void __cdecl GetCommoditys(IDataContainer & Datas, const TExchangeKey* ExchangeKey) = 0;

	//获得指定交易品种的品种信息
	//输入参数：
	//	CommodityKey：品种关键字
	//输出参数：
	//	SCommodity* Commodity
	virtual const SCommodity * __cdecl GetCommodity(const TCommodityKey * CommodityKey) = 0;

	//获得指定合约的标的合约
	//输入参数：
	//	key
	//输出参数：
	//	key
	virtual void __cdecl GetOptionTarget(TContractSingleKey& key) = 0;

	//获得3M代表的具体日期（LME的委托与成交显示3M合约，持仓中需要显示为3M对应的具体日期）
	//输入参数：
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	3M交易日期
	virtual const char * __cdecl Get3MEndDate(const char * Sign) = 0;		

	//获得下单预收保证金数量, 通过Value返回
	//函数返回值：	0 :成功
	//				-1:不支持的定单类型
	//				-2:计算参数不存在
	//				-3:品种不存在
	//				-4:行情不存在
	//				-6:无效的买卖
	virtual int __cdecl GetPreFrozen(const TSendOrder & Order, double & Value) = 0;				

	//获得可下单数量, 通过Value返回	
	//函数返回值：	0 :成功
	//				-1:不支持的定单类型
	//				-2:计算参数不存在
	//				-3:品种不存在
	//				-4:行情不存在
	//				-5:资金不存在
	//				-6:无效的买卖
	virtual int __cdecl GetTradableQty(const TSendOrder & Order, int & Value) = 0;	

	//获得单个币种
	//输入参数：
	//	CurrencyNo：币种号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	币种信息
	virtual const TCurrency * __cdecl GetCurrency(const char * CurrencyNo, const char * Sign) = 0;	

	//获得所有币种
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TCurrency *
	//输入参数：
	//	Sing：交易后台表示符，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllCurrency(IDataContainer & Datas, const char * Sign = "") = 0;		


	//获得单个上手
	//输入参数：
	//	UpperNo：上手编号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	上手信息
	virtual const TUpper * __cdecl GetUpper(const char * UpperNo, const char * Sign) = 0;	

	//获得所有上手
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TUpper *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllUpper(IDataContainer & Datas, const char * Sign = "") = 0;		


	//获得单个消息
	//输入参数：
	//	MsgID：消息ID
	//返回值：
	//	交易消息的信息
	virtual const TMessage * __cdecl GetMsg(const uint MsgID) = 0;	

	//获得所有消息
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TMessage *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllMsg(IDataContainer & Datas, const char * Sign = "") = 0;	


	//获得客户确认信息
	//输入参数：
	//	LoginUser：登录账号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//注意：
	//	目前不允许登录账号重复，Sgin可以不填
	//返回值：
	//	确认信息的文本内容
	virtual const char * __cdecl GetConfirmInfo(const char * LoginUser, const char * Sign) = 0;

	//合约代码格式化
	//输入参数：
	//	ContractKey: 合约关键字结构
	//	sCodeLen: 是Code参数的长度
	//	Format:	合约编码格式化掩码字符串, 掩码定义如下
	//		%e%		交易所
	//		%e1%	关联交易所1
	//		%e2%	关联交易所2
	//		%ct%	品种类型
	//		%ct1%	关联品种类型1
	//		%ct2%	关联品种类型2
	//		%c%		品种代码
	//		%c1%	关联品种代码1
	//		%c2%	关联品种代码2
	//		%m1%	合约月份或日期1 
	//		%m2%	合约月份或日期2
	//		%p1%	执行价格1
	//		%p2%	执行价格2
	//		%ot1%	期权涨跌类型1
	//		%ot2%	期权涨跌类型2	
	//		%oe%	后台系统原始交易所
	//		%oc%	后台系统原始品种代码
	//		%oct%	后台系统原始品种类型
	//		举例:
	//			期货单腿 %c%%m1%				期货组合 %c1%%m1%/%c2%%m2%
	//			期权单腿 %c%%m1%%ot1%%p1%	期权组合 %c1%%m1%%ot1%%p1%/%c2%%m2%%ot2%%p2%
	//		注意：如果要输出%则需要以%%的形式转意
	//输出参数：
	//	sCode: 输出字符串
	//返回值：
	//	输出的有效字符串长度
	virtual uint __cdecl FormatCode(const TContractKey * ContractKey, char * sCode, const uint sCodeLen, const char * Format = nullptr) = 0;

	//获得所有账户银行信息
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TUserBank *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllUserBank(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得所有转账流水
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TTransfer *
	//输入参数：
	//	Sing：交易后台标识串，为""或nullptr时不区分交易后台
	virtual void __cdecl GetAllTransfer(IDataContainer & Datas, const char * Sign = "") = 0;

	//获得单笔转账流水
	//输入参数：
	//	FutureSerialID: 期货端转账流水号
	//	Sign: 后台服务器标识串，以OnConnect返回的TConnect中的Sign为准
	//返回值：
	//	转账信息
	virtual const TTransfer * __cdecl GetTransfer(const uint FutureSerialID, const char * Sign) = 0;

	//获得委价偏离范围查询应答结构
	//输入参数：
	//	CommodityKey：商品关键字
	//返回值：
	//	委价偏离范围应答结构
	virtual const TOrderPriceRange * __cdecl GetOrderPriceRange(const TCommodityKey * CommodityKey) = 0;	
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//交易操作接口
class ITradeApi
{
public:	
	//注册可支持的底层交易插件的信息，底层库收到OnPluginInit的时候注册
	virtual void __cdecl RegisterRawTradeInfo(const char * PluginNo, const char * Version, const char * Name) = 0;
	//获得支持的所有交易后台的插件信息
	//输出参数：
	//	Datas: 输出数据的容器, 其数据元素的类型为 const TRawTradePlugin *
	virtual void __cdecl GetAllRawTradeInfo(IDataContainer & Datas) = 0;
	//获得交易数据访问接口
	virtual ITradeData * __cdecl GetTradeData() = 0;
	//判断某登录账户是否连接成功
	//注意：
	//	目前不允许登录账号重复
	virtual bool __cdecl IsConnected(const char * LoginUser, const char * Sign) = 0;	
	//判断某登录账户是否登录成功
	//注意：
	//	目前不允许登录账号重复)
	virtual bool __cdecl IsLogined(const char * LoginUser, const char * Sign) = 0;
	//判断某登录账户是否初始化成功(已就绪)
	//注意：
	//	目前不允许登录账号重复
	virtual bool __cdecl IsReady(const char * LoginUser, const char * Sign) = 0;	
			
	//打开(连接、登录并初始化)
	//	如果LoginUser已经登录，则直接返回1，否则返回0;
	//	CompanyNo: 会员公司编号
	//	SiteAddrNo: 接入站点编号
	//	LoginUser: 登录账户
	//	LoginPsw: 登录密码
	//	NewPsw: 用户初始化密码
	//返回值:
	//	 0: 操作成功
	//	-1: 不能同时登录不同类型的帐号
	//	-2: 只有单客户可以同时登录多个帐号
	//	-3: 同名帐号已经登录
	//  -4: 无效的接入地址获取API
	//  -5: 无效的公司或站点
	virtual int __cdecl Open(const char * CompanyNo, const char * SiteAddrNo, const char * LoginUser, const char * LoginPsw, const char * NewPsw = nullptr) = 0;	
	//关闭，登出和断开
	//注意：
	//	目前不允许登录账号重复
	virtual int __cdecl Close(const char * LoginUser, const char * Sign) = 0;																			
	//重刷数据, 内部会自动删除现有的交易数据，并从服务器重新请求交易数据，最基本的合约信息、账户、参数等数据不会清空重刷
	//注意：
	//	目前不允许登录账号重复
	virtual int __cdecl RefData(const char * LoginUser, const char * Sign) = 0;
	//下单
	virtual int __cdecl InsertOrder(const TSendOrder & Data) = 0;
	//改单
	virtual int __cdecl ModifyOrder(const TModifyOrder & Data) = 0;
	//撤单
	virtual int __cdecl CancelOrder(const uint OrderID) = 0;
	//标记定单, Remark备注信息
	virtual int __cdecl MarkOrder(const uint OrderID, const TOrderMark Mark, const char * Remark = "") = 0;																
	//挂起定单
	virtual int __cdecl SuspendOrder(const uint OrderID) = 0;																		
	//激活定单
	virtual int __cdecl ResumeOrder(const uint OrderID) = 0;	
																			
	//人工录单、改单、移单
	virtual int __cdecl InputOrder(const TOrder & Order) = 0;																		
	//移除定单
	virtual int __cdecl RemoveOrder(const uint OrderID) = 0;																		
	//成交录入
	virtual int __cdecl InputMatch(const TInputMatch & Match) = 0;																		
	//移除成交
	virtual int __cdecl RemoveMatch(const char * MatchNo, const char * Sign) = 0;
	//行权录入
	virtual int __cdecl InputExecute(const TExecuteInput & Order) = 0;

	//资金的查询兼订阅		
	virtual int __cdecl QryMoney(const char * UserNo, const char * Sign) = 0;																
	//可交易数量查询
	virtual int __cdecl QryTradableQty(const TSendOrder & Data) = 0;																
	//预冻结资金查询
	virtual int __cdecl QryPreFreeze(const TSendOrder & Data) = 0;																
	//修改密码
	//注意：
	//	目前不允许登录账号重复
	virtual int __cdecl ChangePsw(const char * LoginUser, const char * CurrPsw, const char * NewPsw, const TPswType PswType, const char * Sign) = 0;	
	//关系人身份认证
	//注意：
	//	目前不允许登录账号重复
	virtual int __cdecl CheckUserProxy(const char * LoginUser, const char * UserProxy, const char * CheckPsw, const char * Sign) = 0;				
	//历史委托查询
	//	BeginSettleDate: 起始结算日期， yyyy-mm-dd 格式
	//	EndSettleDate  : 结束结算日期， yyyy-mm-dd 格式
	//	Sign: 后台服务器标识串，以ITradeData::OnConnect返回的TConnect中的Sign为准
	//注意：
	//	查询结果是包括BeginSettleDate和EndSettleDate在内这段时间的所有委托，但结果中不包括盘中的委托
	virtual int __cdecl QryHisOrder(const char * UserNo, const char * BeginSettleDate, const char * EndSettleDate, const char * Sign) = 0;									
	//历史委托流程查询, 查询某笔委托(历史委托、盘中委托)的委托流程，
	virtual int __cdecl QryHisOrderProc(const TOrder & Order) = 0;												
	//历史成交查询,
	//	BeginSettleDate: 起始结算日期， yyyy-mm-dd 格式
	//	EndSettleDate  : 结束结算日期， yyyy-mm-dd 格式
	//	Sign: 后台服务器标识串，以ITradeData::OnConnect返回的TConnect中的Sign为准
	//注意：
	//	查询结果是包括BeginSettleDate和EndSettleDate在内这段时间的所有成交，但结果中不包括盘中的成交
	virtual int __cdecl QryHisMatch(const char * UserNo, const char * BeginSettleDate, const char * EndSettleDate, const char * Sign) = 0;		
	//历史持仓查询
	//	BeginSettleDate: 起始结算日期， yyyy-mm-dd 格式
	//	EndSettleDate  : 结束结算日期， yyyy-mm-dd 格式， 目前没有用到
	//	Sign: 后台服务器标识串，以ITradeData::OnConnect返回的TConnect中的Sign为准
	//注意：
	//	查询结果是包括BeginSettleDate和EndSettleDate在内这段时间的所有持仓，但结果中不包括盘中的持仓
	virtual int __cdecl QryHisPosition(const char * UserNo, const char *  BeginSettleDate, const char *  EndSettleDate, const char * Sign) = 0;						
	//历史交割查询
	//	BeginSettleDate: 起始结算日期， yyyy-mm-dd 格式
	//	EndSettleDate  : 结束结算日期， yyyy-mm-dd 格式
	//	Sign: 后台服务器标识串，以ITradeData::OnConnect返回的TConnect中的Sign为准
	//注意：
	//	查询结果是包括BeginSettleDate和EndSettleDate在内这段时间的所有交割
	virtual int __cdecl QryHisDelivery(const char * UserNo, const char *  BeginSettleDate, const char *  EndSettleDate, const char * Sign) = 0;						
	//历史出入金查询
	//	BeginSettleDate: 起始结算日期， yyyy-mm-dd 格式
	//	EndSettleDate  : 结束结算日期， yyyy-mm-dd 格式
	//	Sign: 后台服务器标识串，以ITradeData::OnConnect返回的TConnect中的Sign为准
	//注意：
	//	查询结果是包括BeginSettleDate和EndSettleDate在内这段时间的所有出入金
	virtual int __cdecl QryHisCash(const char * UserNo, const char * BeginSettleDate, const char * EndSettleDate, const char * Sign) = 0;																							
	//结算单查询
	//	BillDate: 结算日期， yyyy-mm-dd 格式
	//  BillTyep: 账单类型
	//	Format:	  账单格式
	virtual int __cdecl QryBill(const char * UserNo, const char * Sign, const char * BillDate, const TBillType BillType = btDay, const TBillFormatType Format = bftText) = 0;						
	//客户关系人查询
	virtual int __cdecl QryUserProxy(const char * UserNo, const char * Sign) = 0;																			

	//订阅持仓保证金数据
	//Subscibe:订阅类型
	//取消的次数>=订阅的次数时才会真正的取消
	virtual int __cdecl SubscibePositionDeposit(const TDataSubscibeType SubscibeType = dstSubscibe) = 0;

	//查询银行可转资金
	//	TBankBalanceQry: 银行余额查询信息
	virtual int __cdecl QryBankBalance(const TBankBalanceQry & Data) = 0;

	//用户银期转账
	//	TransferReq： 银期转账请求
	virtual int __cdecl BankTransfer(const TTransferReq & Data) = 0;

	//查询银期转账流水
	//	UserNo: 资金账号
	//	BeginDate: 起始日期， yyyy-mm-dd 格式
	//	EndDate: 结束日期， yyyy-mm-dd 格式， 目前没有用到
	//	Sign: 后台服务器标识串，以ITradeData::OnConnect返回的TConnect中的Sign为准
	//注意：
	//	查询结果是包括BeginSettleDate和EndSettleDate在内这段时间的所有转账记录，但结果中不包括盘中的持仓
	virtual int __cdecl QryBankTransfer(const char * UserNo, const char * BeginDate, const char * EndDate,  const char * Sign) = 0;
	
	//查询保证金监控中心登录认证信息
	//	UserNo: 资金账号
	//	Sign: 后台服务器标识串，以ITradeData::OnConnect返回的TConnect中的Sign为准
	virtual int __cdecl QryCfmmcToken(const char * UserNo, const char * Sign) = 0;		

	//刷新定单关联信息，	
	// OrderID: 要刷新的定单ID，为0表示刷新说有定单
	// Link: 要刷新的关联数据类型
	virtual int __cdecl QryOrderLinkInfo(const uint OrderID = 0, const TLinkType Link = ltCheckInfo) = 0;	

	//查询期货可转资金
	//	TFutureBalanceQry: 银行余额查询信息
	virtual int __cdecl QryFutureBalance(const TFutureBalanceQry & Data) = 0;

	//上期所深度行情查询
	virtual int __cdecl QrySHFEDeepQuote(const TContractKey & Data) = 0;
	
	//交易所状态查询
	virtual int __cdecl QryExchangeState(const char * LoginUser, const char * Sign) = 0;	

	//请求验证码
	virtual int __cdecl QryVerificationCode(const char * LoginUser, const char * Sign) = 0;

	//验证验证码
	virtual int __cdecl CheckVerificationCode(const char * LoginUser, const char * Sign,const char* VerificationCode) = 0;
};