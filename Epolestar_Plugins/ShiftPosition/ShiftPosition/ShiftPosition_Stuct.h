#pragma once

#include "TradeApi_Type.h"
#include "TradeApi_Struct.h"
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

/* c++ library */
#include <string>
#include <vector>
#include <map>


typedef char SDirect;
#define BuyCoverSellOpen 'B'//买平卖开
#define SellCoverBuyOpen 'S'//卖平买开
#define BuyOpenSellCover 'O'//买开卖平
#define SellOpenBuyCover 'C'//卖开买平


typedef int SLinkCondition;
#define LcMulti 0			//乘
#define LcPlus  1
#define LcSub   2
#define LcDiv   3

typedef int ConditionOrderState;
#define  osRun 0
#define  osPause 1
#define  osTrigger 2

typedef int ShiftPriceCondition;
#define pcLastPrice 0
#define pcCounterPrice 1
#define pcQueuePrice 2

typedef int ShiftTriggerConditon;
#define tcEqualLarger 0
#define tcEqualLess   1



struct ShiftCondition 
{
	int OrderId;
	char ShiftContractNo[51];
	char Code1[51];
	char Code2[51];
	uint OrderQty1;                //第一腿合约数量
	uint OrderQty2;                //第二腿合约数量
	uint TotalQty;				   //总数量（逐笔下单时使用）
	THedge	Hedge;				   //套保类型

	char Sign[21];
	char UserNo[21];
	char Sign2[21];
	char UserNo2[21];

	ShiftPriceCondition iPriceCondition;//最新差价，对价差价，排队差价（根据订单方向判断）
	ShiftTriggerConditon iTriggerCondition;//触发条件
	double OrderPrice;//触发价格
	SDirect iDirect;
	int OrderType1;
	int OrderType2;

	bool Firstclose;
	ConditionOrderState OrderState;
	ShiftCondition()
	{
		OrderId = -1;
		Sign[0] = '\0';
		UserNo[0] = '\0';
		Sign2[0] = '\0';
		UserNo2[0] = '\0';
		ShiftContractNo[0] = '\0';
		OrderPrice = 0.0;
		Code1[0] = '\0';
		Code2[0] = '\0';
		TotalQty = 0;
		Hedge = hSpeculate;
		Firstclose = true;
	}
};


//刷新移仓列表显示 key = ShiftContractNo
struct ShiftListInfo
{	
	char Code1[51];  //第一腿合约
	char Code2[51];  //第二腿合约

	uint OrderQty1;                //第一腿合约数量
	uint OrderQty2;                //第二腿合约数量

	SLinkCondition LinkContract1; //第一腿数量连接条件 * + - / 
	SLinkCondition LinkContract2; //第二腿数量连接条件

	SLinkCondition LinkCondition; //两腿合约连接连接条件


	char ShiftContractNo[51];//可以作为关键字
	double Long_CounterDif;
	double Long_QueueDif;
	double Short_CounterDif;
	double Short_QueueDif;

	double LastDif;
	double AverageDif;
	double OpenDif;
	double Long_MarketDif;
	double Short_MarketDif;
	std::vector<int> OrderId;
	ShiftListInfo()
	{
		Code1[0] = '\0';
		Code2[0] = '\0';
		Long_CounterDif = 0.0;
		Long_QueueDif = 0.0;
		Short_CounterDif = 0.0;
		Short_QueueDif = 0.0;
		LastDif = 0.0;
		AverageDif = 0.0;
		OpenDif = 0.0;
		Long_MarketDif = 0.0;
		Short_MarketDif = 0.0;
	}
};

struct WarningShift
{

	bool FirstColse;	//记录先平还是先开,true为先平

	char ContractCode1[51];
	char ContractCode2[51];

	int NeedOpenQty;				//记录应开仓数量
	int NeedMatchQty;				//记录应平仓数量
	int MatchQty;					//记录平仓成交数量
	int OpenQty;					//记录开仓成交数量
	int TOrderId;					//平今订单号
	int OrderId;					//平/开仓订单号

	bool Warning;

	WarningShift()
	{
		FirstColse = true;
		ContractCode1[0] = '\0';
		ContractCode2[0] = '\0';
		NeedMatchQty = 0;
		NeedOpenQty = 0;
		OpenQty = 0;
		MatchQty = 0;
		TOrderId = -1;
		OrderId = -1;
		Warning = true;
	}
};
//下单信息；
struct ShiftSendOrder
{
	char Sign[21];
	char UserNo[21];
	char Sign2[21];
	char UserNo2[21];

	bool FirstColse;	//记录先平还是先开,true为先平
	bool Warning;		//记录 开-平 第一次不足的警告

	char ContractCode1[51];
	char ContractCode2[51];
	
	int OrderQty1;//记录原始数据第一腿
	int OrderQty2;//记录原始数据第二腿
	int TestQty;

	int OrderPriceType1;
	int OrderPriceType2;

	int OrderPriceSuperPot1;//记录超价点数
	int OrderPriceSuperPot2;//记录超价点数

	TDirect Direct1;
	TDirect Direct2;

	THedge Hedge;	//投保类型

	int TCoverMatchQty;				//记录平今成交数量
	int CoverMatchQty;				//记录平仓成交数量
	int OpenQty;					//记录开仓成交数量
	int TotalQty;					//比例下单总手数
	int RecordTotal;				//记录总手数
	int TOrderId;					//平今订单号
	int OrderId;					//平/开仓订单号
	int ConditionOrderId;			//关联条件单

	bool bChasePrice;

	ShiftSendOrder()
	{
		Sign[0] = '\0';
		UserNo[0] = '\0';
		Sign2[0] = '\0';
		UserNo2[0] = '\0';
		FirstColse = true;
		Warning = false;
		ContractCode1[0] = '\0';
		ContractCode2[0] = '\0';
		OrderQty1 = 0;
		OrderQty2 = 0;
		TestQty = 150;
		TotalQty = 0;
		RecordTotal = 0;
		OrderPriceType1 = 0;
		OrderPriceType2 = 0;
		OpenQty = 0;
		TCoverMatchQty = 0;
		CoverMatchQty = 0;
		TOrderId = -1;
		OrderId = -1;
		ConditionOrderId = -1;
		bChasePrice = false;
		Hedge = hSpeculate;
	}
};

//首次下单价格
typedef int FirstOrderPrice;
//追价价格
typedef int ChasePriceCom;
#define ChaseLadderPrice 0
#define ChaseMarketPrice 1

//追价终止
typedef int ChaseOver;
#define NoOver 0

//投保类型
typedef int CXHedge;
#define  cxSpeculate	0
#define  cxHedge		1

struct stChasePrice
{
	int iSecond;                      //自动连续追价触发条件 n 秒
	int iConditionSecond;			  //条件单n秒后自动撤单
	ChasePriceCom   iChasePriceCom;   //追价价格    1阶梯追价 2市价
	ChaseOver       iChaseOver;       //追价终止    0不终止，1，2，3，4........
	CXHedge			Hedge;

	stChasePrice()
	{
		iSecond = 3;
		iConditionSecond = 3;
		iChaseOver = NoOver;
		iChasePriceCom = ChaseLadderPrice;
		Hedge = cxSpeculate;
	}
};
struct TChaseOrder
{
	int		iOrderId;			//本次需要追价的Orderid
	int		iChaseCount;		//追价次数
	int		iTime;				//记录没有时间
	bool	bCancel;			//记录是否自己撤单
	double  dFirstOrderPrice;	//首次下单价格；
};

struct TConditionOrder
{
	int		iOrderId;				//记录对应条件单
	int		ConditionId;			//记录对应条件单
	int		iTime;					//记录第一腿多久没成交
	int		iMatchQty;				//记录第一腿上一次成交量
	bool	bCancel;				//记录是否认为撤单
};

struct ShiftOrderConfig
{
	stChasePrice	m_stChasePrice;
	bool			m_bSingle = false;
	bool			m_bCondition = false;
};


class TContainer : public IDataContainer
{
public:
	virtual uint __cdecl size()
	{
		return m_stldata.size();
	}
	virtual const void * __cdecl at(uint i)
	{
		return m_stldata.at(i);
	}
	virtual void __cdecl clear()
	{
		m_stldata.clear();
	}
	virtual void __cdecl del(const void * data)
	{
		std::vector<const void*>::iterator itor;
		itor = std::find(m_stldata.begin(), m_stldata.end(), data);
		if (itor != m_stldata.end())
			m_stldata.erase(itor);
	}
	virtual void __cdecl push_back(const void * data)
	{
		if (std::find(m_stldata.begin(), m_stldata.end(), data) == m_stldata.end())
			m_stldata.push_back((const void*)data);
	}
	void FindItem(const void* data, int& iRow)
	{
		int i = 0;
		for (; i < m_stldata.size();)
		{
			if (at(i) == data)
				break;
			else
				i++;
		}
		iRow = i;
	}
	void __cdecl pushbackex(const void* data)
	{
		m_stldata.push_back((const void*)data);
	}
	std::vector<const void*>* GetBaseData(){ return &m_stldata; }
private:
	std::vector<const void*> m_stldata;
};

void DoubleToChar(int iPrecision, double Price, char* Dest);