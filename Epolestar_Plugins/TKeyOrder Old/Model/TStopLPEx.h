#ifndef T_STOP_LOSS_PROFIT_EXPORT_H
#define T_STOP_LOSS_PROFIT_EXPORT_H



//#define结构体
#pragma pack(push,1)
typedef char OrderLPType;
#define otLoss		'0'	//限价止损
#define otProfit	'1'	//限价止盈
#define otFloat		'2' //浮动止损
#define otBreakEven	'3' //保本止盈
#define otVoid		'v' //无
typedef char OrderState;
#define osGeneral	'0'	//未触发
#define osTrig		'1'	//已触发
#define osSuspend	'2'	//已挂起
#define osBreak		'3'	//保本启动
#define osDeleted	'4' //已删除
typedef char TriggerModeBase;
#define tmbLatest	'0'//最新价触发
#define tmbOppo		'1'//对盘价触发

typedef std::multimap<std::string, unsigned int> DATAMAP;
typedef DATAMAP::const_iterator CIT;
typedef pair<CIT, CIT> Range;
typedef pair<std::multimap<std::string, unsigned int>::const_iterator, std::multimap<std::string, unsigned int>::const_iterator> PositionKeyRange;
struct TStopLPInsertItem
{
	char			sign[21];
	char			userNo[21];
	char			code[51];
	TDirect			direct;
	OrderLPType		orderType;			//类型（止损还是止盈）
	int				orderPriceType;		//委托价格类型, 0 排队价 1对手价 2 超价  3市价 4最新价
	double			triggerPrice;		//触发价格
	unsigned int	Qty;				//数量
	unsigned int	MaxCount;			//触发所需次数
	TriggerModeBase trigBase;			//触发模式，止损止盈以指定价格类型变化触发
	double			floatPoint;			//浮动止损 价差
	double			breakEvenPoint;		//保本价差
	int				point;				//超价点数
	char			descri[51];			//描述信息
	double			openPrice;			//开仓均价
	TStopLPInsertItem()
	{
		triggerPrice = 0.0;
		floatPoint = 0.0;
		breakEvenPoint = 0.0;
		openPrice = 0.0;
	}
};
struct TStopLPitem: public TStopLPInsertItem
{
	unsigned int	keyID;				//关键ID 递增 重启重新计算
	OrderState		state;				//状态

	unsigned int	TriggerCount;		//满足条件几次
	bool			LastTirgger;		//上一次是否触发
	char			trigTime[31];		//触发时间
};

#define AutoStopType 0
#define StopOpenType 1
struct AutoStopInfo
{
	int    iOrderType; // AutoStopType开仓自动止损  StopOpen止损开仓  必填

	//止损开仓相关字段；
	bool   bLoss;        //是否发送止损单 必填
	bool   bProfit;      //是否发送止盈单 必填
	bool   bFloat;       //是否发送浮动止损 必填

	double dstopLoss;    //止损 必填
	double dstopProfit;  //止盈 必填
	double dFloatPoint;  //浮动止损 必填
	int    iOrderQty;    //委托数量 必填

	//开仓自动止损保存字段(其他的根据配置来定)
	double dCounterPrice; //委托发出的对价 必填
	double dMatchPrice;   //第一批成交价
	AutoStopInfo()
	{
		iOrderType = AutoStopType;
		bLoss = false;
		bProfit = false;
		bFloat = false;
		dCounterPrice = 0.0;
		dMatchPrice = 0.0;
		dstopProfit = 0.0;
		dstopLoss = 0.0;
		dFloatPoint = 0.0;
		iOrderQty = 0;
	}
};
#pragma pack(pop)
enum DataType
{
	Del_Type,	//删除全部
	Add_Type,	//增加一项
	Chg_Type,	//修改一项
	Trg_Type,	//触发计数变化
	ChO_Type	//修改为其他类型单子
};
class IStopLPListener
{
public:
	virtual void __cdecl Update(DataType type, const TStopLPitem*pdata) = 0;
};

class IStopLP
{
public://公共接口
	//订阅止损止盈数据
	virtual void __cdecl Attach(IStopLPListener *pOb) = 0;
	//取消订阅
	virtual void __cdecl Detach(IStopLPListener *pOb) = 0;
	//下止损单
	virtual unsigned int __cdecl InsertLPOrder(const TStopLPInsertItem *item) = 0;
	//删除止损单
	virtual void __cdecl DeleteLPOrder(const unsigned int KeyID) = 0;
	//修改止损单
	virtual unsigned int __cdecl ModifyLPOrder(const TStopLPitem *item) = 0;
	//触发止损单
	//触发计数加1(参数Trigger为true），或者触发计数清零
	virtual void __cdecl TriggerLPOrder(const unsigned int KeyID, bool Trigger) = 0;
	//开仓止损单
	virtual unsigned int __cdecl OpenLPOrder(const AutoStopInfo *item, const TSendOrder * stSendOrder) = 0;
	//删除所有止损单
	virtual void __cdecl DeleteAllLPOrder() = 0;

	//获得指定客户所有止损单
	//输出参数：
	// Datas：止损单的容器信息，其数据元素类型为 const TStopLPitem*
	//输入参数：
	// UserNo：客户编号，为""或nullptr时不区分客户
	// SCode: 合约代码,为""或nullptr时不区分合约
	// Direct: 方向，为dBoth时不区分买卖方向，dBuy为买，dSell为卖
	// NoTrig：未触发，为正数时返回未触发，为负数时返回已触发,为0时不区分
	virtual void __cdecl GetALLStopLPOrder(IDataContainer &Datas, const char * UserNo = "", const char* SCode = "", TDirect direct = dBoth, int NoTrig = 0) = 0;

	//获得指定keyID的止损单
	//输出参数：
	//pItem：止损单指针，若没找到相应止损单则返回为空指针nullptr
	//输入参数：
	//KeyID：关键字
	virtual void __cdecl GetSingleStopLPOrder(const TStopLPitem* pItem, const unsigned int KeyID) = 0;
};


#endif