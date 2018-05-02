#ifndef _STRATEGYTRADE_API_
#define _STRATEGYTRADE_API_

#include "RawTradeApi_Struct.h"

#define PLUG_STRATEGYTRADE_NAME 			L"StrategyTrade"
#define PLUG_STRATEGYTRADE_VERSION 			L"9.3.0.14"

typedef char	TStrategyContract[101];			//合约
typedef char	TStrategyUserNo[21];			//资金账号
typedef char	TStrategySign[21];				//服务器标识
typedef char	TStrategyDateTime[21];			//时间
typedef wchar_t	TStrategyErrorText[51];			//错误信息

typedef int StrategyWorkState;				//运行状态
#define SWS_NONE			0x00000000		//结束 无工作模块
#define SWS_DrawLine		0x00000001		//画线功能
#define SWS_StopTrade		0x00000010		//止损功能
#define SWS_SpreadTrade		0x00000100		//套利功能
#define SWS_Assist			0x00001000		//追价功能

typedef char TTStrategyType;
#define sgtDrawLine 'd' //画线条件单
#define sgtPrice	'p'	//价格条件单
#define sgtTime		't' //时间条件单
#define sgtOpen		'o'	//开盘触发条件单
#define sgtPreOrder	'r'	//预埋单

typedef char TTypeFilter;
#define tfAll		'a'	//指定合约所有的条件单
#define tfActive	'c' //待触发|已挂起 可修改的条件单
#define tfEnd		'u'	//已触发|指令失败|已失效 不可修改的条件单

typedef char TDrawType;
#define tdtBuy		'b'	//买
#define tdtSell		's'	//卖
#define tdtCover	'c'	//平
#define tdtBackHand	'h'	//反
#define tdtErase	'e'	//清除

typedef int TDLPriceMode;
#define tpm_Last		0	//最新价
#define tpm_Match		1	//对盘价
#define tpm_Work		2	//挂单价
#define tpm_Market		3	//市价
#define tpm_DrawLine	4	//画线价
#define tpm_Unlimit		5	//反向停板价

typedef int DrawLineType;
#define DLT_Condition	0		//条件单画线
#define DLT_Stop		1		//止损单画线
#define DLT_Spread		2		//套利单画线

//条件单结构
struct TStrategyInsert 
{
	TStrategyContract		sContractId;		//合约代码*
	TTStrategyType			StrategyType;		//策略类型*
	TStrategyUserNo			sUserNo;			//资金账号*
	TStrategySign			sSign;				//服务器标识*
	TDirect					tDirect;			//买卖方向
	TOffset					tOffset;			//开平
	double					TriggerPrice;		//触发价格
	uint					OrderQty;			//数量

	TTriggerCondition		TriggerCondition;	//触发条件
	TDLPriceMode			OrderType;			//委托价格类型
	double					dOverPrice;			//超价
	bool					bBackHand;			//反手

	TTriggerMode			TriggerMode;		//触发模式
	double					OrderPrice;			//价格
	TStrategyDateTime		sTrigTime;			//时间条件 触发时间
};

//条件单修改 (合约 资金账号 不能修改)
struct TStrategyModify :public TStrategyInsert
{
	uint					StrategyOrderId;	//条件单编号*  //注释 *字段值不可修改
};

//条件单通知结构
struct TStrategyOrder :public TStrategyModify
{
	TOrderState				OrderState;			//条件单状态     待触发|已挂起  已触发|指令失败|无效单
	TStrategyDateTime		sLocalTime;			//本地下单时间
	TStrategyDateTime		sUpdateTime;		//本地更新时间	
	TStrategyErrorText		ErrorText;			//发单反馈信息
	const SCommodity*		pCommodity;			//品种信息
};

//////////////////////////////////////////////////////////////////////////止损止盈
typedef int  StopType;
#define stLoss			0x00000001	//止损
#define stProfit		0x00000010	//止盈
#define stProtectInvest	0x00000100	//保本
#define stFloatLoss		0x00001000	//浮动 

struct HWND__; typedef HWND__* HWND;
//止损关键字
struct TStopKeyinfo
{
	TStrategyUserNo		UserNo;		//资金账号
	TStrategySign		Sign;		//服务器标识
	TStrategyContract	Code;		//合约代码
	char				Direct;		//买卖方向
};
//待触发的赢损单
struct  LossProfitSet :public TStopKeyinfo
{
	StopType	stype;					//赢损类型  止损 止盈 浮动 保本

	double		dBasePrice;				//基准价格//初始比较价格  //浮动时会随着最新价（成交价）变化
	double		dStopLossPrice;			//止损触发价   			买 dBasePrice-LossPoint*MinChangePrice			卖 dBasePrice+LossPoint*MinChangePrice
	double		dStopProfitPrice;		//止盈触发价			买 dBasePrice+ProfitPoint*MinChangePrice		卖 dBasePrice-ProfitPoint*MinChangePrice
	double		dFloatPrice;			//浮动价格				dLastPrice-dBasePrice>=dFloatPrice				//dStopLossTrigPrice+=dFloatPrice;
	double		dStopKeepPrice;			//保本价格
};
//委托止损信息
struct TOrderStopInfo
{
	LossProfitSet	item;
	uint			uCount;				//数量
	int				OrderID[100];		//定单号
};
//持仓上显示数据
struct TPositionShow
{
	uint		 uStopQty;				//止损止盈量
	double       dStopLossPrice;		//止损价
	double       dStopFloatPrice;		//浮动价
	double       dStopProfitPrice;		//止盈价
	double       dStopKeepPrice;		//保本价
};
//委托显示数据
struct TOrderShow :public TPositionShow
{
	bool		bTriggered;				//待触发
};
//行情画线 持仓数据 (仅支持 展示 修改 止损止盈价)
struct TPositionDraw 
{
	uint		 uLocalId;				//本地编号
	StopType	 nType;					//止损止盈类型
	uint		 uStopQty;				//止损止盈量
	double       dStopLossPrice;		//止损价
	double       dStopProfitPrice;		//止盈价
};
//画线 增加止损止盈单
struct TAddStopDraw 
{
	const TPosition*	pPosition;		//持仓指针
	double				dPrice;			//触发价格
};
//画线 修改止损止盈单
struct TModifyStopDraw
{
	const TPosition*	pPosition;		//持仓指针
	uint				uLocalId;		//本地编号
	StopType			nType;			//止损止盈类型
	double				dPrice;			//修改的触发价格
	uint				uQty;			//修改数量
};
//画线 删除止损止盈单
struct TDeleteStopDraw
{
	const TPosition*	pPosition;		//持仓指针
	uint				uLocalId;		//本地编号
};
//////////////////////////////////////////////////////////////////////////
//套利类型
typedef char TSpreadType;
#define stSpreadSyn		'S' //同步套利(本地套利)
#define stSpreadSequ	'E' //先后套利

//本地套利配对状态
typedef char TPairState;
#define psNone			'N'//空
#define psPaired		'P'//已配对
#define psPairing		'I'//配对中
#define psUnpaired		'U'//有瘸腿

//本地套利配对状态
typedef char TActiveState;
#define tvsNone			'N'//未生效
#define tvsActive		'A'//生效
#define tvsEnd			'E'//结束

//套利 下单请求结构
struct TSpreadSendOrder 
{
	SContractNoType			ContractNo;					//行情合约
	TContractSingleKey		Contract[4];				//交易合约关键字
	TStrategyUserNo			UserNo[4];					//本地套利 用户账号2
	TStrategySign			Sign[4];					//本地套利 服务器标识
	TOffset					Offset[4];					//本地套利 第二腿的开平，移仓时使用
	TBool					IsRecove[4];				//本地套利 是否追价

	TSpreadType				SpreadType;					//套利类型
	TDirect					Direct[4];					//买卖方向 
	double					OrderPrice;					//价差
	uint					OrderQty;					//数量
	SSpreadRateModeType		RateMode[3];				//价差 价比
	TTriggerCondition		TrigCondition;				//触发条件	大于等于 小于等于
	TDLPriceMode			TrigPriceMode;				//对盘 挂单
	bool					bTimeWaitForTrig;			//时间有效性
	TStrategyDateTime		BeginTime;					//开始时间
	TStrategyDateTime		EndTime;					//结束时间
};
//套利 数据应答结构
struct TSpreadOrder
{
	TSpreadSendOrder		sendorder;					//下单信息

	uint					OrderID;					//委托号
	uint					SendQty[4];					//本地套利 发送数量
	TBool					LoginState[4];
	uint					MatchQty[4];				//成交量

	TOrderState				OrderState;					//委托状态 
	TPairState				PairState;					//配对状态
	uint					PairQty;					//已配对数

	int						ErrorCode;					//最新信息码				
	TStrategyErrorText		ErrorText;					//最新错误信息
	TStrategyDateTime		UpdateDateTime;

	SSpreadPriorityLeg		PriorityLeg;				//套利优先腿
	TActiveState			ActiveState;				//有效状态

	const SCommodity *		pCommodity[4];				//本地套利 关联品种
};

class IStrategySpi;
//追单设置参数
struct TAssistInfo 
{
	uint	OrderID;			//追单 定单号	
	const	IStrategySpi* p;	//追单注册对象
};

//状态变更通知
class IStrategySpi
{
public:
	//////////////////////////////////////////////////////////////////////////
	//条件单状态变化通知 NULL时需要重新获取
	virtual void __cdecl OnStrategyNotice(const TStrategyOrder* pdata) = 0;
	//持仓变化通知 NULL时需要重新获取
	virtual void __cdecl OnPositionNotice(const TPosition* pdata) = 0;
	//条件单 运行状态变化通知
	virtual void __cdecl OnStrategyWorkState(const StrategyWorkState state) = 0;
	//////////////////////////////////////////////////////////////////////////
	//赢损信息更新到持仓合计上 显示
	virtual void __cdecl OnStopInfoToPosition(const TPosition* p) = 0;	
	//赢损信息更新到委托上 显示
	virtual void __cdecl OnStopInfoToOrder() = 0;						
	//本地加载数据 有待确认信息
	virtual void __cdecl OnStopInfoUpdate(bool bShwoUpdate) = 0;	
	//////////////////////////////////////////////////////////////////////////
	//本地套利数据更新
	virtual void __cdecl OnSpreadInfoNotice(const TSpreadOrder* pspread) = 0;
	//////////////////////////////////////////////////////////////////////////
	//追价回调
	virtual void __cdecl OnAssistInfoRef(uint OrderIdOld,uint OrderIdNew) = 0;
};

//条件单接口
class IStrategyTradeAPI
{
public:
	//注册/注销回调
	virtual	bool __cdecl RegistStrategyInfo(IStrategySpi* p) = 0;
	virtual	bool __cdecl UnRegistStrategyInfo(IStrategySpi* p) = 0;

	//获取运行状态（交易员登录则退出）
	virtual StrategyWorkState __cdecl GetStartegyWorkState() = 0;

	//画线操作
	virtual void __cdecl PopupDrawlineWnd() = 0;																		//打开画线下单窗口
	virtual void __cdecl CancelDrawLineOperation() = 0;																	//取消画线下单操作
	virtual int  __cdecl AddDrawLineStrategyOrder(const TStrategyContract sContractId,double TriggerPrice, DrawLineType nDrawLineType) = 0;	//增加条件单、套利单 0成功 
	virtual void __cdecl ModifyDrawLinePice(uint StrategyOrderId, double TriggerPrice, DrawLineType nDrawLineType) = 0;						//修改条件单、套利单价格
	virtual void __cdecl SyncModifyWnd(uint StrategyOrderId, double TriggerPrice, DrawLineType nDrawLineType, bool bDrag = false) = 0;		//同步打开画线窗口
	
	virtual int  __cdecl AddDrawStopRecord(const TAddStopDraw& tadd) = 0;												//增加止损止盈单(画线)
	virtual bool __cdecl DeleteDrawStopRecord(const TDeleteStopDraw& tdelete) = 0;										//删除止损止盈单(画线)
	virtual int  __cdecl ModifyDrawStopRecord(const TModifyStopDraw& tmodify) = 0;										//修改止损止盈单(画线)
	virtual void __cdecl SyncStopModifyWnd(const TModifyStopDraw& tmodify, bool bDrag = false) = 0;						//修改止损窗口
	
	// OrderId==0时 操作UserNo对应的所有条件单;   UserNo=="" 操作所有用户的条件单 
	virtual	int  __cdecl MoveStrategyOrder(uint StrategyOrderId, DrawLineType nDrawLineType, const char* UserNo = "") = 0;					//删除/取消
	virtual int  __cdecl SuspendStrategyOrder(uint StrategyOrderId, DrawLineType nDrawLineType, const char* UserNo = "") = 0;				//暂停 
	virtual int  __cdecl ResumeStrategyOrder(uint StrategyOrderId, DrawLineType nDrawLineType, const char* UserNo = "") = 0;				//启动 
	virtual int  __cdecl TrigStrategyOrder(uint StrategyOrderId, DrawLineType nDrawLineType) = 0;											//立即触发
				 
	virtual int  __cdecl SuspendDrawStopRecord(const TPosition* pPosition) = 0;											//暂停 合约对应的所有止损止盈单(画线) 
	virtual int  __cdecl ResumeDrawStopRecord(const TPosition* pPosition) = 0;											//启动 合约对应的所有止损止盈单(画线)

	//获取指定合约的持仓
	virtual int  __cdecl GetPositionTotal(SContractNoType ContractId, TPosition* data[], int count) = 0;
	
	//条件单
	virtual int  __cdecl GetAllStrategyOrder(uint beginId, TStrategyOrder* data[], int count) = 0;						//获取所有条件单
	virtual int  __cdecl GetStrategyOrdersByContractId(SContractNoType ContractId, uint beginId, TStrategyOrder* data[],
													  int count, TTypeFilter nTypeFilter = tfActive) = 0;				//获取指定合约的条件单
	virtual const TStrategyOrder* __cdecl GetStrategyOrder(uint OrderId) = 0;											//获取具体条件单信息

	//止损止盈	
	virtual	void __cdecl AddOrderStopRecord(const TOrderStopInfo& item) = 0;											//委托止损(下单面板)	
	virtual bool __cdecl GetPositionStopInfo(IDataContainer& Datas, const TPosition* p) = 0;							//获取持仓显示信息(列表)
	virtual int  __cdecl GetPositionStopInfo(uint beginId, TPositionDraw data[], int count, const TPosition* p) = 0;	//获取持仓显示信息(画线)
	virtual bool __cdecl GetOrderStopInfo(TOrderShow& Datas, const TOrder* p) = 0;										//获取委托显示信息(列表)
	virtual void __cdecl StopLossModify(const TPosition* pPosition, HWND hwnd) = 0;										//设置止损止盈(列表)
	virtual void __cdecl StopLossMakeSure(HWND hwnd) = 0;																//止损确认(下单面板)

	//本地套利
	virtual int  __cdecl AddSpreadOrder(const TSpreadSendOrder& data) = 0;												//增加(下单面板)
	virtual int  __cdecl CancelSpreadOrder(uint order_id) = 0;															//撤销 (交易列表、画线) order_id==0 撤全部
	virtual int  __cdecl GetAllSpreadOrder(uint beginId, TSpreadOrder* data[], int count) = 0;							//获取所有套利单(交易列表)
	virtual int  __cdecl GetTriggeringSpreadOrders(uint beginId, TSpreadOrder* data[], int count,
												   SContractNoType ContractId) = 0;										//获取待触发套利单(画线)
	virtual const TSpreadOrder* __cdecl GetSpreadOrderInfo(uint order_id) = 0;											//获取套利单笔信息
	virtual int  __cdecl GetSpreadSubOrderIDCounts(uint order_id) = 0;													//获取套利关联定单数量（交易列表）
	virtual int  __cdecl GetSpreadSubOrderIDs(uint order_id,uint beginId, uint data[], 
											  int count,bool bPosIndex = false) = 0;									//获取套利关联定单（交易列表）
	virtual bool __cdecl GetSSpreadContractNo(SContractNoType &ContractId,const TSpreadOrder* data) = 0;				//获取套利合约代码

	//追单接口
	virtual bool __cdecl AddAssistOrder(const TAssistInfo& passist) = 0;												//增加追价单 Param为空 使用追单助手配置信息
	virtual bool __cdecl StopAssistOrder(const TAssistInfo& passist) = 0;												//停止追加单 order_id为0 停止对应所有追单

	//套利关联接口 返回0表示失败
	virtual int  __cdecl GetSpreadIdbyOrderId(uint order_id) = 0;														//通过OrderId得到关联的套利ID(交易列表)
};


#endif