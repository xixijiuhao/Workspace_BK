#pragma once
#include <windows.h>
#include <string>
#include <iostream>

#include "TradeApi_API.h"
using namespace std;

#define QueuePrice   0                //排队价
#define CounterPrice 1                //对价
#define SuperPrice   2                //超价
#define MarketPrice  3                //市价
#define LastPrice    4                //最新价
typedef vector<string> CStringsVec;

struct SOFCommodityInf
{
	char		Sign[21];
	bool		bCanNegative;					//小数时是否能够为负数 
	bool		bIfFractionPrice;				//是否分数报价
	char		cCoverMode;						//根据此来决定是否显示开平界面
	double		dMiniChangePrice;				//bIfFractionPrice若为1代表分数的分子
	int			iPrecision;						//bIfFractionPrice若为1代表分数的分母
	uint		cUserIdentity;					//交易登陆用户身份，是在登陆回调函数和初始化窗口中赋值,默认为单客户(uiUser) 下单人(uiProxy) 经纪人(uiBroker) 交易员(uiTrader) 行情客户(uiQUser)
	bool		bAddOne;
	char		CmbDirectFlag;					//组合买卖标记
public:
	SOFCommodityInf()
	{
		Default();
	}
	void Default()
	{
		memset(this, 0, sizeof(SOFCommodityInf));
		//默认
		dMiniChangePrice = 1.00;
		iPrecision = 1;
		cCoverMode = cmCover;
	}
};

struct SFractionPrice
{
	int	iInteger;			//整数部分
	int	iMolecules;			//分子部分
	int	idenominator;		//分母部分
public:
	SFractionPrice() :iInteger(0), iMolecules(0), idenominator(0){}
};
struct PosCount
{
	//第一腿持买持卖 第二腿持买持卖
	uint BuyOpenCount;	//买入开仓 持仓量(第一腿持买) 第一腿买开 第二腿卖平 
	uint BuyCoverCount;	//买入平仓 持仓量(第二腿持卖) 第一腿平开 第二腿卖开

	uint SellOpenCount;	//卖出开仓 持仓量(第一腿持卖) 第一腿卖开 第二腿买平	
	uint SellCoverCount;//卖出平仓 持仓量(第二腿持买)  第一腿卖平 第二腿买开
public:
	PosCount()
	{
		memset(this, 0, sizeof(PosCount));
	}
};
struct NoticeContent
{
	string strKey;
	string strContent;
};

//可平 非互换
struct CanCoverQty
{
	uint uCanCoverQty;
	uint uCanCoverTQty;
public:
	CanCoverQty()
	{
		memset(this, 0, sizeof(CanCoverQty));
	}
};
struct GridHeaderInfo
{
	int iBuyPositionQty;
	int iBuyConditionQty;
	int iBuyParOrderQty;
	int iBuyQuoteQty;

	int iSellQuoteQty;
	int iSellPositionQty;
	int iSellParOrderQty;
	int iSellConditionQty;
};

struct OrderInfo
{
	char          	szUserNo[21];
	char          	szSign1[21];
	char          	szSign2[21];
	string          strCode;
	OrderInfo()
	{
		szUserNo[0] = '\0';
		szSign1[0] = '\0';
		szSign2[0] = '\0';
		strCode = "";
	}
	OrderInfo& operator= (const OrderInfo& Info)
	{
		memcpy(&this->szUserNo, &Info.szUserNo, sizeof(szUserNo));
		memcpy(&this->szSign1, &Info.szSign1, sizeof(szSign1));
		memcpy(&this->szSign2, &Info.szSign2, sizeof(szSign2));
		this->strCode = Info.strCode;
		return *this;
	}
};



struct PingPangConfig
{
	bool bPingPang;
	uint iBuyQty;  //买档位
	uint iSellQty; //卖档位
	int  iQueueTime;//设定排队时间自动撤单;最小是10， 最大为60;
};

struct HotKey
{
	bool bHotKey;
	int  iHotKeyDirct;  //0 正向  1 反向
};
//点价配置
struct PointOrderConfig
{
	bool           bLastPriceCenter;
	bool           bDeepQuote;
	uint           iDefaultQty;
	int            iCoverAllPriceType;//0为对手价 1为市价;
	int            iMessageBox;//0为不弹出,1为弹出
	int            iComparePre;//0为对比昨结算,1为对比最新价
	int			   iBuySellDirection;//0为买左、1为卖左

	HotKey         stHotKey;
	PingPangConfig stPingPang;
	PointOrderConfig()
	{
		bLastPriceCenter = false;
		bDeepQuote = false;
		iDefaultQty = 1;
		stHotKey.bHotKey = false;
		stHotKey.iHotKeyDirct = 0;
		stPingPang.bPingPang = false;
		stPingPang.iBuyQty = 1;
		stPingPang.iSellQty = 1;
		stPingPang.iQueueTime = 10;

		iCoverAllPriceType = 0;
		iMessageBox = 0;
		iComparePre = 0;
		iBuySellDirection = 0;
	}
};