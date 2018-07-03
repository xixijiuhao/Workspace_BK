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

/*=============================请求参数==========================*/

typedef STR50 AC_Service_Type;	
static const AC_Service_Type AC_BigMovement = "bigmovementpatterns";
static const AC_Service_Type AC_ConsecutiveCandles = "consecutivecandlespatterns";

static const AC_Service_Type AC_Trade = "Trading Opportunities";
static const AC_Service_Type AC_Volatility = "Volatility Analysis";
static const AC_Service_Type AC_Performance = "Performance Statistics";
static const AC_Service_Type AC_Community = "Trading Community";

typedef STR50 AC_Username;
static const AC_Username username = "cfddcejk01";

typedef STR50 AC_Password;
static const AC_Password password = "!20180516DceKJ01";

typedef STR10 AC_ExchangeNo;
//内盘
static const AC_ExchangeNo Ex_CZCE = "CZCE";
static const AC_ExchangeNo Ex_SHFE = "SHFE";
static const AC_ExchangeNo Ex_DCE = "DCE";
//外盘
static const AC_ExchangeNo Ex_CFFEX = "CFFEX";

/*=============================回调参数==========================*/

//===============================其他===============================//
struct ACUrlParams
{
	AC_Service_Type		sServiceType;
	AC_Username			sUsername;
	AC_Password			sPassword;
	AC_ExchangeNo		sExchangeNo;
};

struct ACChartPatternParam
{
	STR20		exchange;
	STR50		symbol;
	I32			interval;
	STR20		pattern;
	I32			length;
	C8			direction;
	I32			identified_time;
	STR20		trend;
	I32			quality;
	I32			initialtrend;
	F32			volumeincrease;
	I32			uniformity;
	I32			clarity;
	I32			breakout;
	F32			forecastprice;
};

struct ACFibonacciPatternParam
{
	STR20		exchange;
	STR50		symbol;
	I32			interval;
	STR20		pattern;
	I32			length;
	C8			direction;
	I32			identified_time;
	I32			quality;
	I32			timesymmetry;
	I32			pricesymmetry;
	I32			clarity;
	F32			target0;
	F32			target3;
	F32			target5;
	F32			target6;
	F32			target7;
	F32			target10;
	F32			target12;
	F32			target16;
};

struct ACKeyLevelsPatternParam
{
	STR20		exchange;
	STR50		symbol;
	I32			interval;
	STR20		pattern;
	I32			length;
	C8			direction;
	I32			quality;
	I32			identified_time;
	F32			price;
	F32			forecastprice;
};

struct ACConsecutiveCandlesParam
{
	STR50 m_sCommoditySymbol;

	I32 patternendtime;
	I32 patternstarttime;
	I32	direction;

	ACConsecutiveCandlesParam& operator=(ACConsecutiveCandlesParam& value)
	{
		strcpy_s(m_sCommoditySymbol, value.m_sCommoditySymbol);
		patternendtime = value.patternendtime;
		patternstarttime = value.patternstarttime;
		direction = value.direction;
		return *this;
	}
};

struct ACBigMovementParam
{
	STR50 m_sCommoditySymbol;

	I32 patternendtime;
	I32 patternstarttime;
	I32	direction;

	ACBigMovementParam& operator=(ACBigMovementParam& value)
	{
		strcpy_s(m_sCommoditySymbol, value.m_sCommoditySymbol);
		patternendtime = value.patternendtime;
		patternstarttime = value.patternstarttime;
		direction = value.direction;
		return *this;
	}
};

struct ACPatternParam
{
	STR20		exchange;
	STR50		symbol;
	I32			interval;
	STR50		pattern;
	I32			length;
	I32			direction;
	I32			identified_time;
	I32			quality;
	I32			completed;
	//I32			patternendtime;

	F64	predictionpricefrom;
	F64	predictionpriceto;
	I32	predictiontimeto;
	I32	predictiontimefrom;

	//ChartPatternParam
	STR20		trend;
	I32			initialtrend;
	F64			volumeincrease;
	I32			uniformity;
	I32			clarity_c;
	I32			breakout;

	I32 resistancex0;
	I32	resistancex1;
	F64	resistancey0;
	F64	resistancey1;
	I32	supportx0;
	I32	supportx1;
	F64	supporty0;
	F64	supporty1;

	//FibonacciPatternParam
	I32			timesymmetry;
	I32			pricesymmetry;
	I32			clarity_f;
	F64			target0;
	F64			target3;
	F64			target5;
	F64			target6;
	F64			target7;
	F64			target10;
	F64			target12;
	F64			target16;

	F64			priceA;
	F64			priceB;
	F64			priceC;
	F64			priceD;
	F64			priceX;
	I32			timeA;
	I32			timeB;
	I32			timeC;
	I32			timeD;
	I32			timeX;

	//KeyLevelsPatternParam
	F64			price;
	//F64			predictionpricefrom_k;    //预计价格
	I32			point1;
};

struct ACSpiData
{
	//ACChartPatternParam	ChartPatternParam;
	//ACFibonacciPatternParam  FibonacciPatternParam;
	//ACKeyLevelsPatternParam KeyLevelsPatternParam;

	bool m_bValid;

	ACPatternParam				m_PatternParam;
	std::vector<ACConsecutiveCandlesParam>	m_ConsecutiveParamVec;
	std::vector<ACBigMovementParam>			m_BigMovementParamVec;

	I32		m_nResultuid;
	STR50	m_sCommoditySymbol;
	STR20	m_sACPattern;
	STR500  m_sPatternImageUrl;
	STR20	m_sPlateName;
	ACSpiData()
	{
		m_bValid = true;
		memset(&this->m_PatternParam, 0, sizeof(ACPatternParam));
		m_nResultuid = 0;
		strcpy_s(this->m_sCommoditySymbol, "");
		strcpy_s(this->m_sACPattern, "");
		strcpy_s(this->m_sPatternImageUrl, "");
	}

	ACSpiData& operator=(ACSpiData& value)
	{
		m_bValid = value.m_bValid;
		memcpy(&m_PatternParam, &value.m_PatternParam, sizeof(ACPatternParam));
		memcpy(&m_PatternParam, &value.m_ConsecutiveParamVec, value.m_ConsecutiveParamVec.size() * sizeof(ACConsecutiveCandlesParam));
		memcpy(&m_PatternParam, &value.m_BigMovementParamVec, value.m_BigMovementParamVec.size() * sizeof(ACBigMovementParam));
		m_nResultuid = value.m_nResultuid;
		strcpy_s(m_sCommoditySymbol, value.m_sCommoditySymbol);
		strcpy_s(m_sACPattern, value.m_sACPattern);
		strcpy_s(m_sPatternImageUrl, value.m_sPatternImageUrl);
		return *this;
	}

	void InitData()
	{
		m_bValid = true;
		memset(&this->m_PatternParam, 0, sizeof(ACPatternParam));
		m_ConsecutiveParamVec.clear();
		m_BigMovementParamVec.clear();
		m_nResultuid = 0;
		strcpy_s(this->m_sCommoditySymbol, "");
		strcpy_s(this->m_sACPattern, "");
		strcpy_s(this->m_sPatternImageUrl, "");
	}
};

//文件读取内容
typedef wchar_t KPlateName[51]; \
typedef wchar_t KContractCodeType[101];
typedef wchar_t KContractNameType[101];
typedef wchar_t KShowContractName[256];
//typedef char	KShowContractName[256];					//合约名称
struct ACContract
{
	I32					resultuid;
	STR50				commoditySymbol;
	STR50				pattern;
	SContractNoType		SubContractNo;
	SContract*			pContract;
};

struct ContractTransInfo
{
	SContractNoType		SubContractNo;
	std::string				years;
	std::string				months;
};

//std::string MONTHSTR[] =
//{
//	"Jan",
//	"Feb",
//	"Mar",
//	"Apr",
//	"May",
//	"Jun",
//	"Jul",
//	"Aug",
//	"Sep",
//	"Oct",
//	"Nov",
//	"Dec"
//};
