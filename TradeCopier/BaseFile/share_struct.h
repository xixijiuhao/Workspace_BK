
#pragma once
#include <windows.h>
#include <string>
#include <vector>
using namespace std;

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

//排序
class UserNoOrder
{
public:
	bool operator()(CStringsVec &vecStr1, CStringsVec &vecStr2)
	{
		return vecStr1[0] < vecStr2[0];
	}
};


#define ptQueuePrice   0                //排队价
#define ptCounterPrice 1                //对价
#define ptSuperPrice   2                //超价
#define ptMarketPrice  3                //市价
#define ptLastPrice    4                //最新价
#define ptSpecifiedPrice 5				//指定价
#define ptTrigPrice    6




struct TUser
{
	char UserNo[21];
	char Sign[21];
	unsigned char State;//登录状态
	bool operator==(const TUser& tUser)
	{
		return strcmp(UserNo, tUser.UserNo) == 0;
	}
	bool operator==(const char* szUserNo)
	{
		return strcmp(UserNo, szUserNo) == 0;
	}
};

typedef char TFollowPriceType;
#define fptFollowMain     'F'
#define fptQueuePrice     'Q'
#define fptConsideration  'C'
#define fptMarket         'M'
struct TFollowConfigData
{
	double           QtyRate;            //数量比率
	int              FollowCoverTime;    //追平仓时间
	bool             DirectSameToMain;   //方向
	TFollowPriceType FollowPriceType;    //跟单价格类型
	int              ChaseCount;
	int              CheckPositionTime;
	bool			 bUseCheckPosition;
	TFollowConfigData(){
		FollowPriceType = fptFollowMain;
		QtyRate = 1.0;
		FollowCoverTime = 2;
		DirectSameToMain = true;
		ChaseCount = 5;

		CheckPositionTime = 10;
		bUseCheckPosition = true;
	}
};
struct TFollowUser :TUser, TFollowConfigData
{
	int iTimerCount;
	int iCheckPositionCount;
};
struct TOrderKey
{
	uint   mainOrderID;
	uint   followOrderID;
	const TOrder* pMainOrder;
	const TOrder* pFollowOrder;
	int		followQty;
	int     mainTotalQty;
	int     mainMatchQty;
	char   mainOrderState;
	char   followOrderState;
	bool   mainFirstCallBack;

};
#define  RightNow     'R'   //立即追平仓
#define  WaitMain     'W'   //等主账号成交后
struct TParOrderKey
{
	const TOrder* pMainOrder;
	string Remark;
	uint FollowOrderID;
};
struct TCancelOrderKey
{
	const TOrder *pMainOrder;
	uint FollowOrderID;
};

struct TCancelQty
{
	char UserNo[21];
	int iQty;
	bool operator==(const char* szUserNo)
	{
		return strcmp(UserNo, szUserNo) == 0;
	}
};
struct TChaseInfo{
	UINT OrderID;
	UINT ChaseCount;
	TChaseInfo(UINT oi)
	{
		OrderID = oi;
		ChaseCount = 0;
	}
	bool operator==(const TChaseInfo& tUser)
	{
		return OrderID == tUser.OrderID;
	}
};

struct PositionInfo
{
	wchar_t     CommodityName[21];
	//char		CommodityNo[11];
	//char		ContractNo[11];
	char		Key[21];
	TDirect		Direct;
	uint		Qty;
	double      Price;
	double      Margin;
	double      Profit;
	vector		<const TPosition*> postions;
	bool operator==(const char *pszKey)
	{
		return strcmp(Key, pszKey) == 0;
	}

};

struct TradeTime
{
	uint BeginTime[3];
	uint EndTime[3];

	TradeTime()
	{
		for (int i = 0; i < ARRAYSIZE(BeginTime); i++)
		{
			BeginTime[i] = 0;
			EndTime[i] = 0;
		}
	}
};

struct TNightTradeTime
{
	uint BeginTime;
	uint EndTime;

	TNightTradeTime()
	{
		BeginTime = 0;
		EndTime = 0;
	}
};