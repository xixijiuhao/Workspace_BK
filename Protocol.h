#pragma once

typedef int		I32;
typedef float   F32;
typedef double  F64;
typedef char	C8;
typedef C8 STR1[2];
typedef C8 STR10[11];
typedef C8 STR20[21];
typedef C8 STR50[51];
typedef C8 STR300[301];
typedef C8 STR500[501];



/*=============================�������==========================*/
typedef STR20 TCType_product;										//��Ʒ����
static const TCType_product TC_typenull = "null";
static const TCType_product TC_stock = "stock";						//��Ʊ
static const TCType_product TC_index = "index";						//ָ��
static const TCType_product TC_commodities = "commodities";			//��Ʒ
static const TCType_product TC_sector = "sector";					//
static const TCType_product TC_bond = "bond";						//ծȯ
static const TCType_product TC_forex = "forex";						//���

typedef STR20 TCProduct;											//��������
static const TCProduct TC_ric = "RIC";								//·͸�����
static const TCProduct TC_ticker = "ticker";						//��Ʊ����

typedef STR10 TCPartner;											//�ͻ���
static const TCPartner TC_esunnyNo = "784";

typedef STR500 TCToken;												//�ͻ��Ŷ�Ӧ��Կ
static const TCToken TC_esunnyToken = "A78c%2bScgCnaGc3ChIgS%2fiA%3d%3d";

typedef STR10 TCCulture;											//����(en_USӢ�� zh_CN���� zh_TW����)
static const TCCulture TC_en_US = "en-US";
static const TCCulture TC_zh_CN = "zh_CN";
static const TCCulture TC_zh_TW = "zh_TW";

typedef STR10 TCTerm;												//���ڣ���Ƶ15min �ռ�Intraday ����ST, ���ڣ�
static const TCTerm TC_termAll = "";
static const TCTerm TC_15Min = "15min";
static const TCTerm TC_intraday = "INTRADAY";
static const TCTerm TC_shortTerm= "ST";
static const TCTerm TC_midTerm = "MT";
typedef STR10 TCDay;													//����չʾ���ڡ�Ĭ��Ϊ7  1,2����7
typedef STR10 TCLast_ta;												//�Ƿ���ʾ���½�� 0��ʾ���� 1��ʾ���½��
typedef STR10 TCToday;												//��ѡ����  ���޹�Ʊ,��ֵ��1�������ʾ������


/*=============================�ص�����==========================*/
typedef I32 TCOPinion;											//������ͼ����
static const TCOPinion TC_opinion_up = 2;						//����
static const TCOPinion TC_opinion_limitUp = 1;					//��������
static const TCOPinion TC_opinion_wave = 0;						//��
static const TCOPinion TC_opinion_limitDown = -1;				//�����µ�
static const TCOPinion TC_opinion_down = -2;					//�µ�

typedef I32 TCDeltaST;												//������ͼ����(������Ҫ����OpinionST���жϾ��庬�壬ȡֵ��Χ-4��4)

typedef I32 TCOPinionMT;											//������ͼ����
//static const TCOPinionMT TC_opinion_up = 2;						//����
//static const TCOPinionMT TC_opinion_wave = 0;						//��
//static const TCOPinionMT TC_opinionMT_limitDown = -1;				//����

typedef I32 TCDeltaMT;												//������ͼ����(������Ҫ����OpinionMT���жϾ��庬�壬ȡֵ��Χ-2��2)

typedef F64 TCResistance;											//ѹ����
typedef F64 TCLastPrice;											//���̼�
typedef F64 TCPivot;												//����ת�۵�
typedef F64 TCSupport;												//֧����
//================================K������===============================//
typedef I32 KLineSlice;
static const KLineSlice KLine_Min1 = 1;
static const KLineSlice KLine_Min5 = 5;
static const KLineSlice KLine_Min10 = 10;
static const KLineSlice KLine_Min15 = 15;
static const KLineSlice KLine_Min30 = 30;
static const KLineSlice KLine_Min60 = 60;
static const KLineSlice KLine_Day = 1;
static const KLineSlice KLine_Day7 = 7;
static const KLineSlice KLine_Day30 = 30;

struct TCParams
{
	TCType_product		product_type;
	TCProduct			product;
	TCPartner			partnerNo;
	TCToken				token;
	TCCulture			culture;
	TCTerm				term;
	TCDay				day_slice;
	TCLast_ta           last;
};

struct TCParamSpiData
{
	bool			valid;
	I32				articleID;
	STR20			date;
	STR20			hour;
	SDateTimeType   dateTime;
	STR300			media;
	STR50			ISIN;
	STR50			ticker;
	STR50			RIC;
	STR50			name;
	STR50			product;
	TCTerm			term;
	TCOPinion		optionST;
	TCDeltaST		deltaSt;
	TCOPinion		optionMT;
	TCDeltaMT		deltaMt;
	TCResistance	res1;
	TCResistance    res2;
	TCResistance	res3;
	TCResistance    pivot;
	TCSupport		sup1;
	TCSupport		sup2;
	TCSupport		sup3;
	STR300			error_message;
	
};
typedef std::vector<std::string> IntroductText;
struct TCSpiData
{
	TCParamSpiData	data;
	IntroductText	text;
	TCSpiData& operator=(TCSpiData& value)
	{
		memcpy(&data, &value.data, sizeof(TCParamSpiData));
		text = value.text;
		return *this;
	}
	void InitData()
	{
		memset(&this->data, 0, sizeof(this->data));
		this->text.clear();
	}
};

//�ļ���ȡ����
typedef wchar_t KPlateName[51]; \
typedef wchar_t KContractCodeType[101];
typedef wchar_t KContractNameType[101];
typedef wchar_t KShowContractName[256];
//typedef char	KShowContractName[256];					//��Լ����
struct KContract
{
	KPlateName			plate;
	SContractNoType		SubContractNo;
	SContract*			pContract;
	KContractCodeType	contractCode;
	SContractNameType	contractName;
	KContractNameType	contractWName;
	KShowContractName	contractShowName;
	STR20				TCTicker;
};

