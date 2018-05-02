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
#define BuyCoverSellOpen 'B'//��ƽ����
#define SellCoverBuyOpen 'S'//��ƽ��
#define BuyOpenSellCover 'O'//����ƽ
#define SellOpenBuyCover 'C'//������ƽ


typedef int SLinkCondition;
#define LcMulti 0			//��
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
	uint OrderQty1;                //��һ�Ⱥ�Լ����
	uint OrderQty2;                //�ڶ��Ⱥ�Լ����
	uint TotalQty;				   //������������µ�ʱʹ�ã�
	THedge	Hedge;				   //�ױ�����

	char Sign[21];
	char UserNo[21];
	char Sign2[21];
	char UserNo2[21];

	ShiftPriceCondition iPriceCondition;//���²�ۣ��Լ۲�ۣ��ŶӲ�ۣ����ݶ��������жϣ�
	ShiftTriggerConditon iTriggerCondition;//��������
	double OrderPrice;//�����۸�
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


//ˢ���Ʋ��б���ʾ key = ShiftContractNo
struct ShiftListInfo
{	
	char Code1[51];  //��һ�Ⱥ�Լ
	char Code2[51];  //�ڶ��Ⱥ�Լ

	uint OrderQty1;                //��һ�Ⱥ�Լ����
	uint OrderQty2;                //�ڶ��Ⱥ�Լ����

	SLinkCondition LinkContract1; //��һ�������������� * + - / 
	SLinkCondition LinkContract2; //�ڶ���������������

	SLinkCondition LinkCondition; //���Ⱥ�Լ������������


	char ShiftContractNo[51];//������Ϊ�ؼ���
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

	bool FirstColse;	//��¼��ƽ�����ȿ�,trueΪ��ƽ

	char ContractCode1[51];
	char ContractCode2[51];

	int NeedOpenQty;				//��¼Ӧ��������
	int NeedMatchQty;				//��¼Ӧƽ������
	int MatchQty;					//��¼ƽ�ֳɽ�����
	int OpenQty;					//��¼���ֳɽ�����
	int TOrderId;					//ƽ�񶩵���
	int OrderId;					//ƽ/���ֶ�����

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
//�µ���Ϣ��
struct ShiftSendOrder
{
	char Sign[21];
	char UserNo[21];
	char Sign2[21];
	char UserNo2[21];

	bool FirstColse;	//��¼��ƽ�����ȿ�,trueΪ��ƽ
	bool Warning;		//��¼ ��-ƽ ��һ�β���ľ���

	char ContractCode1[51];
	char ContractCode2[51];
	
	int OrderQty1;//��¼ԭʼ���ݵ�һ��
	int OrderQty2;//��¼ԭʼ���ݵڶ���
	int TestQty;

	int OrderPriceType1;
	int OrderPriceType2;

	int OrderPriceSuperPot1;//��¼���۵���
	int OrderPriceSuperPot2;//��¼���۵���

	TDirect Direct1;
	TDirect Direct2;

	THedge Hedge;	//Ͷ������

	int TCoverMatchQty;				//��¼ƽ��ɽ�����
	int CoverMatchQty;				//��¼ƽ�ֳɽ�����
	int OpenQty;					//��¼���ֳɽ�����
	int TotalQty;					//�����µ�������
	int RecordTotal;				//��¼������
	int TOrderId;					//ƽ�񶩵���
	int OrderId;					//ƽ/���ֶ�����
	int ConditionOrderId;			//����������

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

//�״��µ��۸�
typedef int FirstOrderPrice;
//׷�ۼ۸�
typedef int ChasePriceCom;
#define ChaseLadderPrice 0
#define ChaseMarketPrice 1

//׷����ֹ
typedef int ChaseOver;
#define NoOver 0

//Ͷ������
typedef int CXHedge;
#define  cxSpeculate	0
#define  cxHedge		1

struct stChasePrice
{
	int iSecond;                      //�Զ�����׷�۴������� n ��
	int iConditionSecond;			  //������n����Զ�����
	ChasePriceCom   iChasePriceCom;   //׷�ۼ۸�    1����׷�� 2�м�
	ChaseOver       iChaseOver;       //׷����ֹ    0����ֹ��1��2��3��4........
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
	int		iOrderId;			//������Ҫ׷�۵�Orderid
	int		iChaseCount;		//׷�۴���
	int		iTime;				//��¼û��ʱ��
	bool	bCancel;			//��¼�Ƿ��Լ�����
	double  dFirstOrderPrice;	//�״��µ��۸�
};

struct TConditionOrder
{
	int		iOrderId;				//��¼��Ӧ������
	int		ConditionId;			//��¼��Ӧ������
	int		iTime;					//��¼��һ�ȶ��û�ɽ�
	int		iMatchQty;				//��¼��һ����һ�γɽ���
	bool	bCancel;				//��¼�Ƿ���Ϊ����
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