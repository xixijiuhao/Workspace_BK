#pragma once
#include <windows.h>
#include <string>
#include <iostream>

#include "TradeApi_API.h"
using namespace std;

#define QueuePrice   0                //�ŶӼ�
#define CounterPrice 1                //�Լ�
#define SuperPrice   2                //����
#define MarketPrice  3                //�м�
#define LastPrice    4                //���¼�
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
	uint iBuyQty;  //��λ
	uint iSellQty; //����λ
	int  iQueueTime;//�趨�Ŷ�ʱ���Զ�����;��С��10�� ���Ϊ60;
};

struct HotKey
{
	bool bHotKey;
	int  iHotKeyDirct;  //0 ����  1 ����
};
//�������
struct PointOrderConfig
{
	bool           bLastPriceCenter;
	bool           bDeepQuote;
	uint           iDefaultQty;
	int            iCoverAllPriceType;//0Ϊ���ּ� 1Ϊ�м�;
	int            iMessageBox;//0Ϊ������,1Ϊ����
	int            iComparePre;//0Ϊ�Ա������,1Ϊ�Ա����¼�
	int			   iBuySellDirection;//0Ϊ����1Ϊ����

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