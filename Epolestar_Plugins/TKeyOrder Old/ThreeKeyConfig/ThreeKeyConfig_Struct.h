#ifndef THREEKEY_CONFIG_STRUCT_H
#define THREEKEY_CONFIG_STRUCT_H 

//ѡ������

typedef int ClickPositionQty;   //����ֲ��б������

#define DefaultQty     0       //Ĭ������
#define AllPositionQty 1       //�ú�Լ��ȫ�����óֲ�

typedef int BackFillOrderMod;         //����Ĭ���µ���ʽ
typedef int ProportionFillOrderMod;   //����ƽ���µ���ʽ
#define ptQueuePrice   0                //�ŶӼ�
#define ptCounterPrice 1                //�Լ�
#define ptSuperPrice   2                //����
#define ptMarketPrice  3                //�м�
#define ptLastPrice    4                //���¼�
#define ptSpecifiedPrice 5				//ָ����


typedef int CloseAllFillOrderMod;     //�˻�����µ���ʽ

typedef int LogDisplay;               //��־��ʾ����
#define Log 0					      //��־
#define DishPrice 1                   //�̿ڼ۸�

struct OptionConfig
{
	//bool bTradeSound;              //����������ʾ
	bool bWndPosition;			   //�µ�����λ�ü���
	//bool bProportionOpen;		   //�ٷֱȿ���
	//bool bPointOrder;              //����µ�
	bool bDoublePositionConfirm;   //˫���ֲֺ͹ҵ���ȷ�ϲ���
	//bool bSpecialPrice;			   //Ĭ��ָ�����µ�

	//�ֲ��б� ���ɿ��������  ��ʱ������

	ClickPositionQty iPositionQty;                   //����ֲ��б������ 0Ĭ������  1 �ú�Լ��ȫ�����óֲ�  
	BackFillOrderMod iBackFillOrderMod;              //����Ĭ���µ���ʽ 0�ŶӼ� 1�Լ� 2���� 3�м� 4���¼�
	ProportionFillOrderMod iProportionFillOrderMod;  //����ƽ���µ���ʽ 0�ŶӼ� 1�Լ� 2���� 3�м� 4���¼�
	CloseAllFillOrderMod iCloseAllFillOrderMod;      //�˻�����µ���ʽ 0�Լ� 1���� 2�м� 

	//�����µ�
	bool bOrderConfirm;             //ί��ȷ��
	bool bDoubleClickPositionClose; //˫���ֲֿ���ƽ�֣�
	bool bAutoOpenClose;            //�Զ����俪ƽ�֣�
	bool bLogDisPlay;               //Log����ʾ��    0��־ 1�̿ڼ۸�
	bool bBigOrderSplit;	//�Ƿ�󵥲��

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

//Ĭ������
typedef int iDefaultQty;
struct stDefaultQty
{
	TContractKey stContract;
	iDefaultQty  iQty;
};

//���۲���
struct stSuperPrice
{
	TContractKey stContract;
	int iLongSuperPrice;
	int iShortSuperPrice;
	double dMinChangePrice;
};
//׷�۲���

//�״��µ��۸�
typedef int FirstOrderPrice;
//׷�ۼ۸�
typedef int ChasePriceCom;

#define ChaseLadderPrice 0
#define ChaseMarketPrice 1


//׷����ֹ
typedef int ChaseOver;
#define NoOver 0
struct stChasePrice
{
	int iSecond;                      //�Զ�����׷�۴������� n ��
	ChasePriceCom   iChasePriceCom;   //׷�ۼ۸�    0����׷�� 1�м�
	ChaseOver       iChaseOver;       //׷����ֹ    0����ֹ��1��2��3��4........

	stChasePrice()
	{
		iSecond = 3;
		iChaseOver = NoOver;
		iChasePriceCom = ChaseLadderPrice;
	}
};

//ֹ�����
struct stSLContractPriceMargin
{
	TContractKey stContract;	//��Լ
	int iStopLossPriceGap;	//ֹ��۲�ʣ����ٻس��۲
	int iStopProfitPriceGap;	//ֹӯ�۲�ʣ������۲��������С�䶯����������
	double dMinChangePrice;		//��С�䶯��
};
struct stStopLossConfig
{
	int iContinuity;	//������ 0�����˳�ʱ�Զ�ȡ�� 1�����˳�ʱ�ϴ�������
	int iTriggerCount;  //������Ҫ���� 1���ϵ�����int>0
	int iOpenAutoStopLossBasicPrice;//�����Զ�ֹ���׼�� 0��һ���ɽ��� 1ί�з���ʱ�Լ�
	int iDefaultTactic;	//Ĭ�ϲ��� 0�޼�ֹ��+�޼�ֹӯ 1�޼�ֹ��+���� 2��̬׷��+���� 3��̬׷�� 4�޼�ֹ�� 5�޼�ֹӯ

	std::map<std::string, stSLContractPriceMargin> stdmapContract;//��Լ�۲�

	//bool bClickListStart;	//��ֲ��б��Ӯ��������ֹӯֹ��
	bool bOpenAutoStopLoss;	//�����Զ�ֹ��ֹӯ�������������ֿ��֡��޼۵����⣩
	//bool bStopLossOrderConfirm;	//ֹ��ί��ȷ��
	//bool bStopLossOrderCloseCancel;	//ֹ��ƽ�ֳ���ԭ�йҵ�

	int iStopLossOrderPrice;	//ֹ�𵥺ͱ�����ί�м۸�
	int iStopProfitOrderPrice;	//ֹӯί�м۸�
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