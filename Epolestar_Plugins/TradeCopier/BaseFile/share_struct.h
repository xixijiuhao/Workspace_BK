
#pragma once
#include <windows.h>
#include <string>
#include <vector>
using namespace std;

typedef vector<string> CStringsVec;

struct SOFCommodityInf
{
	char		Sign[21];
	bool		bCanNegative;					//С��ʱ�Ƿ��ܹ�Ϊ���� 
	bool		bIfFractionPrice;				//�Ƿ��������
	char		cCoverMode;						//���ݴ��������Ƿ���ʾ��ƽ����
	double		dMiniChangePrice;				//bIfFractionPrice��Ϊ1��������ķ���
	int			iPrecision;						//bIfFractionPrice��Ϊ1��������ķ�ĸ
	uint		cUserIdentity;					//���׵�½�û���ݣ����ڵ�½�ص������ͳ�ʼ�������и�ֵ,Ĭ��Ϊ���ͻ�(uiUser) �µ���(uiProxy) ������(uiBroker) ����Ա(uiTrader) ����ͻ�(uiQUser)
	bool		bAddOne;
	char		CmbDirectFlag;					//����������
public:
	SOFCommodityInf()
	{
		Default();
	}
	void Default()
	{
		memset(this, 0, sizeof(SOFCommodityInf));
		//Ĭ��
		dMiniChangePrice = 1.00;
		iPrecision = 1;
		cCoverMode = cmCover;
	}
};
struct SFractionPrice
{
	int	iInteger;			//��������
	int	iMolecules;			//���Ӳ���
	int	idenominator;		//��ĸ����
public:
	SFractionPrice() :iInteger(0), iMolecules(0), idenominator(0){}
};
struct PosCount
{
	//��һ�ȳ������ �ڶ��ȳ������
	uint BuyOpenCount;	//���뿪�� �ֲ���(��һ�ȳ���) ��һ���� �ڶ�����ƽ 
	uint BuyCoverCount;	//����ƽ�� �ֲ���(�ڶ��ȳ���) ��һ��ƽ�� �ڶ�������

	uint SellOpenCount;	//�������� �ֲ���(��һ�ȳ���) ��һ������ �ڶ�����ƽ	
	uint SellCoverCount;//����ƽ�� �ֲ���(�ڶ��ȳ���)  ��һ����ƽ �ڶ�����
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

//��ƽ �ǻ���
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

//����
class UserNoOrder
{
public:
	bool operator()(CStringsVec &vecStr1, CStringsVec &vecStr2)
	{
		return vecStr1[0] < vecStr2[0];
	}
};


#define ptQueuePrice   0                //�ŶӼ�
#define ptCounterPrice 1                //�Լ�
#define ptSuperPrice   2                //����
#define ptMarketPrice  3                //�м�
#define ptLastPrice    4                //���¼�
#define ptSpecifiedPrice 5				//ָ����
#define ptTrigPrice    6




struct TUser
{
	char UserNo[21];
	char Sign[21];
	unsigned char State;//��¼״̬
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
	double           QtyRate;            //��������
	int              FollowCoverTime;    //׷ƽ��ʱ��
	bool             DirectSameToMain;   //����
	TFollowPriceType FollowPriceType;    //�����۸�����
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
#define  RightNow     'R'   //����׷ƽ��
#define  WaitMain     'W'   //�����˺ųɽ���
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