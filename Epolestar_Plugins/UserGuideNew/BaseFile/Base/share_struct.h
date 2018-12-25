
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

//�����ֶ�����
enum TradeFieldType
{
	TFT_STRING,
	TFT_NUMERIC,
	TFT_DATE
};

//�ڻ��µ������ֶ� //������ֻ������� ���ܴ��м���� ����ᵼ�����Ͽͻ��汾������ �м���뵼��IAF_ACCOUNT�������ļ������ֵ����Ӧ
enum FutureInputArea
{
	IAF_TJCHECK,			//����ѡ���� ���ܲ�������
	IAF_ACCOUNT,			//�ͻ��˺�
	IAF_COMMODITY,			//��Լ����
	IAF_DIRECT,				//����
	IAF_OFFSET,				//��ƽ
	IAF_PRICE,				//ί�м۸�  ������� + ���� /��ĸ �ĸ� ���ĸ�Ҫ��ʾ�������ί�м۸�֮��
	IAF_PRICE_ADD,
	IAF_PRICE_FENZI,
	IAF_PRICE_CHUHAO,
	IAF_PRICE_FENMU,
	IAF_VOL,				//ί������
	IAF_TRIGGERMODE,		//������ʽ
	IAF_TRIGGERCONDITION,	//��������
	IAF_TRIGGERPRICE,		//�����۸�
	IAF_VOLBL,				//��������
	//IAF_STOPPRICE,			//ֹ��۸�
	//IAF_MATCHCONDITION,		//�ɽ�����
	IAF_VALIDTIME,			//��Ч����

	IAF_SELECODE,			//ѡ���Լ ʼ����ʾ
	IAF_TRIGGERPRICE_ADD,
	IAF_TRIGGERPRICE_FENZI,
	IAF_TRIGGERPRICE_CHUHAO,
	IAF_TRIGGERPRICE_FENMU,
	//IAF_ORDERPRICE_

	IAF_VOLCOUNT,		//�µ�����  ����
	IAF_INCREVOL,		//��������

	IAF_INCREPRICE,			//�۸����� 
	IAF_INCRERPRICE_CHUHAO,	//�۸���������  ��ʾ������ڼ۸�����֮��
	IAF_INCREPRICE_FENMU,	//�۸�������ĸ  ��ʾ������ڼ۸���������֮��

	IAF_ORDERTYPE,			//��������
	IAF_VALIDTYPE,			//��Ч����

	IAF_MAXQTY,				//����� ��ɽ
	IAF_MINQTY,				//��С�� ��ɽ

	//IAF_BUTTON,				//���ð�ť��������
	//IAF_SUM 				//����������ӵ��ֶ��������ǰ��
};
struct  FacadeSetting
{
	LOGFONT			lgBtnFont;			//��ť����
	LOGFONT			lgCheckFont;		//CheckBox����
	LOGFONT			lgStaticFont1;		//��̬�ı�����
	LOGFONT			lgStaticFont2;		//״̬��ʾ����
	LOGFONT			lgComboFont;		//�ɱ༭����
	LOGFONT			lgPositionFont;		//�ֲ�����

	int				iLeftMargin;		//�ؼ���߾�
	int				iTopMargin;			//�ؼ����߾�

	int				iHorCtrlInterval;	//ˮƽ�ؼ����
	int				iVerCtrlInterval1;	//��ֱ�ؼ����1 ��̬�ı���ˮƽ�ؼ��ļ��
	int				iVerCtrlInterval2;	//��ֱ�ؼ����2 ˮƽ�ؼ���״̬��ʾ�ļ��
	int				iVerCtrlInverval3;	//��ֱ�ؼ����2 ��ť֮��Ĵ�ֱ���

	COLORREF		rgbBkPosition;		//�ֲֵı���ɫ
	COLORREF		rgbBuy;				//��ͷ�ĳֲ���ɫ
	COLORREF		rgbSell;			//��ͷ�ĳֲ���ɫ

	// �ֶνṹ����
	struct OrderField
	{
		int		id;						// �ֶα�ţ���Ӧ��TradeOrderField������� FutureInputArea
		int		width;					// ��ʾ���(������) �Զ���� �������޹�
		int		height;					// ��ʾ���(������) �Զ���� �������޹�
		//char	text[32];				//�ֶεı���  
		int		align;					//���뷽ʽ��1������룬2�����У�3���Ҷ���  ��ʱδ��
		int		type;					//�ֶε�����Ҳ���������ϣ��������ַ�������������ֵ���������ڵȵ�  TradeFieldType    ��ʱδ��
		bool	bShow;					//�Ƿ���ʾ
	};

	int				iOrderFieldCount;	//�ֶνṹ����	
	OrderField		*pOrderField;		//�ֶνṹָ��

	unsigned int	iDefaultKbCfg;		//�����µ����ý���  //0==С���� 1==������ 2==������

};


//new
typedef char TKUserType;
#define utForeign  'F'
#define utInner    'I'


struct TOrderViewData
{
	int     OrderQty;
	uint    iBuyPosQty;   //��ֲ���
	uint    iSellPosQty;  //���ֲ���
	const   TContractKey*  pContractKey;
};

struct TQuoteData
{
	double BuyPrice;      //���
	double SellPrice;     //����
	double LastPrice;     //���¼�
	double UpPrice;       //��ͣ��
	double DownPrice;     //��ͣ��

	uint   BuyQty;
	uint   SellQty;
	uint   LastQty;
};



struct TMenuInfo
{
	uint Index;
	uint MenuID;
	const void *pData;
};

typedef char TRunState;
#define rsRun  'R'       //����
#define rsStop 'S'       //ֹͣ
#define rsTriggered 'T'  //�Ѵ���

typedef char  TConOrderType;
#define cotPrice	'P'
#define cotTime		'T'
#define cotPreOrder 'p'
#define cotAutoOrder 'O'

struct TConditionOrder
{
	char				InsertTime[20];
	char				UserNo[21];
	TRunState			RunState;
	TConOrderType		OrderType;
	TOrderType			RealOrderType;
	bool            	Triggered;
	const TContract*	pContract;
	double      		ConPrice;      //�۸�����
	int					ConPriceType;
	TDirect     		Direct;
	TOffset     		Offset;
	uint        		iQty;
	uint        		PriceType;     //�µ��۸�����
	double        		Price;         //�۸�����
	int					OrderPriceOffset;
	int                 OrderID;
	int					OrderID2;
	TTriggerCondition   TriggerCondition;
	char				ContractId[51];
	double				dMinChangePrice;
	uint				iPrecision;
};




#define ptQueuePrice   0                //�ŶӼ�
#define ptCounterPrice 1                //�Լ�
#define ptSuperPrice   2                //����
#define ptMarketPrice  3                //�м�
#define ptLastPrice    4                //���¼�
#define ptSpecifiedPrice 5				//ָ����
#define ptTrigPrice    6


struct TrigPriceInfo
{
	char BuyPriceType;
	char SellPriceType;
	char OrderPriceType;

	int BuyOffset;
	int SellOffset;
	int OrderOffset;
	char Key[21];
};

