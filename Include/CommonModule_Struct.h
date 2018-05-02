#pragma once

//下单类型

typedef int		CM_TYPE;
#define	CM_NORMAL		0x00000000	//普通下单
#define	CM_BATCHQTY		0x00000001	//批量下单
#define	CM_BATCHCOUNT	0x00000002	//批次下单
#define	CM_DRAWORDER	0x00000004	//画线下单
#define	CM_STOPORDER	0x00000008	//止损下单
#define	CM_ENQUIRY		0x00000010	//询价下单

typedef int		CT_SUBPARAM;
#define	CT_NONE			0x00000000	//空
#define	CT_COVEROPEN	0x00000001	//平开
#define	CT_RECOVE		0x00000002	//追价

//模块ID
typedef int		CM_SUBID;
#define	AssistFillID	0			//辅助填单
#define	DealOrderID		1			//下单处理
#define	ExpandInfoID	2			//扩展操作
#define	FastInfoID		3			//快速下单
#define	BatchInfoID		4			//批量

//辅助填单 价格方式
typedef int		CM_FillOrderPriceMode;
#define fopm_Last				0	//最新超价
#define fopm_Follow				1	//对盘超价
#define fopm_Work				2	//挂单超价
#define fopm_Nodefault			3	//不默认

//条件填单 触发条件
typedef int		CM_FillOrderTrigCondition;
#define foc_Loss				0	//止损模式
#define foc_Profit				1	//止盈模式

//条件填单 触发方式
typedef int		CM_FillOrderTrigMode;
#define fot_Last				0	//最新价
#define fot_Buy					1	//买价
#define fot_Sell				2	//卖价

//持仓显示 显示方式
typedef int    CM_FillOrderShowPosType;
#define fosp_Single				0	//单笔
#define fosp_All				1	//全部

//持仓显示 显示方式
typedef int    CM_ChaseType;
#define ct_Match				0	//对价跟
#define ct_Work					1	//排队跟
#define ct_Chase				2	//连续追

//批次信息
struct BatchCountsInfo
{
	unsigned long int	uiCounts;	//下单次数
	double				dPriceInc;	//价格增量
	int					nQtyInc;	//数量增量
};

//界面参数
struct CM_PARAM
{
	CM_TYPE			cmParam;		//类型
	CT_SUBPARAM		ctParam;		//参数
	BatchCountsInfo bcinfo;			//批次
};

//数据容器类
class IVecInt
{
public:
	virtual unsigned int __cdecl size() = 0;
	virtual int	 __cdecl at(unsigned int) = 0;
	virtual void __cdecl clear() = 0;
	virtual void __cdecl push_back(int data) = 0;
};

enum ErrorLevelCommon
{
	EL_None,		//正确
	EL_Error,		//错误
	EL_Warn			//警告
};

struct ErrorReflectInfo
{
	ErrorLevelCommon		nErrorLevel;	//错误级别 正确0 错误1 警告2
	wchar_t					error_text[101];//错误信息
};

struct sFastInfo	//快速下单信息
{
	bool bcondition;			//是否选中条件
	bool bFastOrder;			//是否选中快速
	bool bBatchQuantities;		//是否选中批量
	bool bBackHand;				//是否选中平反
	bool bBatchTimes;			//是否选中批次
	bool bAddOne;				//是否选中T+1
	bool bRecove;				//是否选中追价

	char sFastUserNo[21];		//快速资金账号 批量分组账号
	char sFastCode[51];			//快速交易合约
	char sFastSign[21];			//快速服务器标识
	double  dQtyRatio;			//数量倍率

	//条件
	char cTrigMode;				//触发方式
	char cTrigCondition;		//触发条件
	double dTrigPrice;			//触发价格

	//批次
	int		uiQtyInc;			//数量增量
	double  dPriceInc;			//价格增量
	unsigned long int  uiOrderTimes;	//下单次数

	TValidType	ValidType;		//有效类型
	char		ValidTime[21];	//有效日期时间(GTD情况下使用)
};

//辅助填单信息
struct C_FillOrderAssistInfo
{
	unsigned int				uSize;						//结构大小

	//条件单信息
	CM_FillOrderTrigMode		uiTrigMode;					//条件填单 触发方式 0最新 1买价 2卖价
	CM_FillOrderTrigCondition	uiTrigCondition;			//触发条件	0止损模式	1止盈模式
	CM_FillOrderPriceMode		uiTrigPriceMode;			//触发价格	0最新超价 1对盘超价 2挂单超价 3不默认
	int							uiTrigOverPoint;			//触发价格超价点数
	bool						bTrigSynQuote;				//触发价格行情同步刷新

	bool						bBackMatchUserNo;			//资金账号反向匹配
	unsigned int				uiPricePos;					//价格光标位置
	bool						bShowPosition;				//显示持买持卖
	CM_FillOrderShowPosType		uiShowPosType;				//持仓显示类型

	bool						bShowAccountNo;				//状态栏资金账号信息显示
};
//下单处理信息
struct C_OrderDealInfo
{
	unsigned int				uSize;							//结构大小

	bool						bOpenAutoCancel;				//开满自动撤单
	bool						bCoverAutoCancel;				//平仓自动撤单 标识

	bool						bShowTipSpreadTrig;				//双击排队单撤销该笔委托 修改为 套利单下单达到触发条件弹出提示0418
	bool						bShowTipCancelOrder;			//撤单弹出确认信息

	bool						bPositionCoverTip;				//持仓列表平仓操作需确认
};
//下单后处理信息
struct C_OrderReturnInfo
{
	unsigned int				uSize;							//结构大小

	bool						bClearDirect;					//下单后清空买卖
	bool						bValidTypeGFD;					//下单后有效类型设为当日有效
	bool						bOrderTypeLimit;				//下单后定单类型设为限价
	bool						bCancelForceCover;				//下单后取消强平
	bool						bCancelAddOne;					//下单后取消港交所T+1
};
//快速信息
struct C_FastOrderInfo
{
	unsigned int				uSize;							//结构大小
	double						dChangePoint;					//倍率变动点数
};
//批量信息
struct BatchGroupInfo
{
	unsigned int				uSize;							//结构大小

	char						BatchGroupNo[11];				//批量分组号	
	char						BatchGroupName[21];				//批量分组名
	char						BaseUser[21];					//比例基准账号 / 首个子帐号 
};
struct C_BatchGroupInfo
{
	unsigned int				uMember;						//分组个数

	BatchGroupInfo*				pBatchGroupInfo;				//分组信息
};
//填单信息
struct C_FillOrderInfo
{
	char	sUserNo[21];
	char	sCode[101];
	int		nQtyType;	//0 使用nQty   1保留上次委托数量  2默认下单量
	int		nQty;
	double	dPrice;
	char	cDirect;
	char	cOffset;
	char	cStrategy;
	int		nOpenQty;
	int		nCoverQty;
	int		nClickType;//0 单击		1 双击
};
//填单超价信息
struct C_ExcInfo 
{
	CM_FillOrderPriceMode	PriceMode;	//超价方式 
	int						nOverPoint;	//超价点数
};