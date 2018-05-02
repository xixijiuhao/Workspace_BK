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



/*=============================请求参数==========================*/
typedef STR20 TCType_product;										//产品类型
static const TCType_product TC_typenull = "null";
static const TCType_product TC_stock = "stock";						//股票
static const TCType_product TC_index = "index";						//指数
static const TCType_product TC_commodities = "commodities";			//商品
static const TCType_product TC_sector = "sector";					//
static const TCType_product TC_bond = "bond";						//债券
static const TCType_product TC_forex = "forex";						//外汇

typedef STR20 TCProduct;											//代码类型
static const TCProduct TC_ric = "RIC";								//路透社代码
static const TCProduct TC_ticker = "ticker";						//股票代码

typedef STR10 TCPartner;											//客户号
static const TCPartner TC_esunnyNo = "784";

typedef STR500 TCToken;												//客户号对应密钥
static const TCToken TC_esunnyToken = "A78c%2bScgCnaGc3ChIgS%2fiA%3d%3d";

typedef STR10 TCCulture;											//语言(en_US英文 zh_CN中文 zh_TW繁体)
static const TCCulture TC_en_US = "en-US";
static const TCCulture TC_zh_CN = "zh_CN";
static const TCCulture TC_zh_TW = "zh_TW";

typedef STR10 TCTerm;												//周期（高频15min 日间Intraday 短期ST, 中期）
static const TCTerm TC_termAll = "";
static const TCTerm TC_15Min = "15min";
static const TCTerm TC_intraday = "INTRADAY";
static const TCTerm TC_shortTerm= "ST";
static const TCTerm TC_midTerm = "MT";
typedef STR10 TCDay;													//分析展示周期。默认为7  1,2……7
typedef STR10 TCLast_ta;												//是否显示最新结果 0显示所有 1显示最新结果
typedef STR10 TCToday;												//可选参数  仅限股票,其值是1，则仅显示当天结果


/*=============================回调参数==========================*/
typedef I32 TCOPinion;											//短期视图趋势
static const TCOPinion TC_opinion_up = 2;						//上涨
static const TCOPinion TC_opinion_limitUp = 1;					//抑制上涨
static const TCOPinion TC_opinion_wave = 0;						//震荡
static const TCOPinion TC_opinion_limitDown = -1;				//抑制下跌
static const TCOPinion TC_opinion_down = -2;					//下跌

typedef I32 TCDeltaST;												//短期视图趋势(可能需要根据OpinionST来判断具体含义，取值范围-4到4)

typedef I32 TCOPinionMT;											//中期视图趋势
//static const TCOPinionMT TC_opinion_up = 2;						//看涨
//static const TCOPinionMT TC_opinion_wave = 0;						//震荡
//static const TCOPinionMT TC_opinionMT_limitDown = -1;				//看跌

typedef I32 TCDeltaMT;												//中期视图趋势(可能需要根据OpinionMT来判断具体含义，取值范围-2到2)

typedef F64 TCResistance;											//压力线
typedef F64 TCLastPrice;											//收盘价
typedef F64 TCPivot;												//策略转折点
typedef F64 TCSupport;												//支撑线
//================================K线周期===============================//
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

//文件读取内容
typedef wchar_t KPlateName[51]; \
typedef wchar_t KContractCodeType[101];
typedef wchar_t KContractNameType[101];
typedef wchar_t KShowContractName[256];
//typedef char	KShowContractName[256];					//合约名称
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

