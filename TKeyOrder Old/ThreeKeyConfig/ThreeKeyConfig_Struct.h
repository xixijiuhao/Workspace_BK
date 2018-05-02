#ifndef THREEKEY_CONFIG_STRUCT_H
#define THREEKEY_CONFIG_STRUCT_H 

//选项设置

typedef int ClickPositionQty;   //点击持仓列表填单手数

#define DefaultQty     0       //默认手数
#define AllPositionQty 1       //该合约的全部可用持仓

typedef int BackFillOrderMod;         //反手默认下单方式
typedef int ProportionFillOrderMod;   //比例平仓下单方式
#define ptQueuePrice   0                //排队价
#define ptCounterPrice 1                //对价
#define ptSuperPrice   2                //超价
#define ptMarketPrice  3                //市价
#define ptLastPrice    4                //最新价
#define ptSpecifiedPrice 5				//指定价


typedef int CloseAllFillOrderMod;     //账户清仓下单方式

typedef int LogDisplay;               //日志显示内容
#define Log 0					      //日志
#define DishPrice 1                   //盘口价格

struct OptionConfig
{
	//bool bTradeSound;              //交易声音提示
	bool bWndPosition;			   //下单窗口位置记忆
	//bool bProportionOpen;		   //百分比开仓
	//bool bPointOrder;              //点价下单
	bool bDoublePositionConfirm;   //双击持仓和挂单的确认操作
	//bool bSpecialPrice;			   //默认指定价下单

	//持仓列表， 最大可开仓手数填单  暂时不做；

	ClickPositionQty iPositionQty;                   //点击持仓列表填单手数 0默认手数  1 该合约的全部可用持仓  
	BackFillOrderMod iBackFillOrderMod;              //反手默认下单方式 0排队价 1对价 2超价 3市价 4最新价
	ProportionFillOrderMod iProportionFillOrderMod;  //比例平仓下单方式 0排队价 1对价 2超价 3市价 4最新价
	CloseAllFillOrderMod iCloseAllFillOrderMod;      //账户清仓下单方式 0对价 1超价 2市价 

	//三键下单
	bool bOrderConfirm;             //委托确认
	bool bDoubleClickPositionClose; //双击持仓快速平仓；
	bool bAutoOpenClose;            //自动分配开平仓；
	bool bLogDisPlay;               //Log区显示；    0日志 1盘口价格
	bool bBigOrderSplit;	//是否大单拆分

	THedge  Hedge;
	OptionConfig()
	{
		//bTradeSound = true;
		bWndPosition = true;
		//bProportionOpen = false;
		//bPointOrder = false;
		bDoublePositionConfirm = false;
		//bSpecialPrice = false;

		iPositionQty = AllPositionQty;
		iBackFillOrderMod = ptCounterPrice;
		iProportionFillOrderMod = ptCounterPrice;
		iCloseAllFillOrderMod = ptMarketPrice;

		bOrderConfirm = false;
		bDoubleClickPositionClose = true;
		bAutoOpenClose = false;
		bLogDisPlay = true;

		bBigOrderSplit = false;
		
		Hedge = hSpeculate;
	}
};

//默认手数
typedef int iDefaultQty;
struct stDefaultQty
{
	TContractKey stContract;
	iDefaultQty  iQty;
};

//超价参数
struct stSuperPrice
{
	TContractKey stContract;
	int iLongSuperPrice;
	int iShortSuperPrice;
	double dMinChangePrice;
};
//追价参数

//首次下单价格
typedef int FirstOrderPrice;
//追价价格
typedef int ChasePriceCom;

#define ChaseLadderPrice 0
#define ChaseMarketPrice 1


//追价终止
typedef int ChaseOver;
#define NoOver 0
struct stChasePrice
{
	int iSecond;                      //自动连续追价触发条件 n 秒
	ChasePriceCom   iChasePriceCom;   //追价价格    0阶梯追价 1市价
	ChaseOver       iChaseOver;       //追价终止    0不终止，1，2，3，4........

	stChasePrice()
	{
		iSecond = 3;
		iChaseOver = NoOver;
		iChasePriceCom = ChaseLadderPrice;
	}
};

//止损参数
struct stSLContractPriceMargin
{
	TContractKey stContract;	//合约
	int iStopLossPriceGap;	//止损价差倍率（跟踪回撤价差）
	int iStopProfitPriceGap;	//止盈价差倍率（保本价差）（都是最小变动价整数倍）
	double dMinChangePrice;		//最小变动价
};
struct stStopLossConfig
{
	int iContinuity;	//持续性 0交易退出时自动取消 1交易退出时上传服务器
	int iTriggerCount;  //触发需要次数 1以上的整数int>0
	int iOpenAutoStopLossBasicPrice;//开仓自动止损基准价 0第一批成交价 1委托发出时对价
	int iDefaultTactic;	//默认策略 0限价止损+限价止盈 1限价止损+保本 2动态追踪+保本 3动态追踪 4限价止损 5限价止盈

	std::map<std::string, stSLContractPriceMargin> stdmapContract;//合约价差

	//bool bClickListStart;	//点持仓列表的赢损列启用止盈止损
	bool bOpenAutoStopLoss;	//开仓自动止损止盈（条件单、反手开仓、限价单除外）
	//bool bStopLossOrderConfirm;	//止损单委托确认
	//bool bStopLossOrderCloseCancel;	//止损单平仓撤掉原有挂单

	int iStopLossOrderPrice;	//止损单和保本单委托价格
	int iStopProfitOrderPrice;	//止盈委托价格
	stStopLossConfig()
	{
		iContinuity = 0;
		iTriggerCount = 2;
		iOpenAutoStopLossBasicPrice = 0;
		iDefaultTactic = 3;
		bOpenAutoStopLoss = false;
		iStopProfitOrderPrice = ptCounterPrice;
		iStopLossOrderPrice = ptCounterPrice;
	}
};


#endif