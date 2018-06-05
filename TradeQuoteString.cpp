#include "BaseInclude.h"

using namespace std;

extern ITradeData* g_pTradeData;
extern IStarApi *	g_pQuoteApi;
//品种类型
typedef char               TapCommodityTypeType;
//无
const TapCommodityTypeType		COMMODITY_TYPE_NONE = 'N';
//现货
const TapCommodityTypeType		COMMODITY_TYPE_SPOT = 'P';
//期货
const TapCommodityTypeType		COMMODITY_TYPE_FUTURES = 'F';
//期权
const TapCommodityTypeType		COMMODITY_TYPE_OPTION = 'O';
//跨期套利
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_MONTH = 'S';
//跨品种套利
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_COMMODITY = 'M';
//看涨垂直套利
const TapCommodityTypeType		COMMODITY_TYPE_BUL = 'U';
//看跌垂直套利
const TapCommodityTypeType		COMMODITY_TYPE_BER = 'E';
//跨式套利
const TapCommodityTypeType		COMMODITY_TYPE_STD = 'D';
//宽跨式套利
const TapCommodityTypeType		COMMODITY_TYPE_STG = 'G';
//备兑组合
const TapCommodityTypeType		COMMODITY_TYPE_PRT = 'R';
//看涨水平期权
const TapCommodityTypeType		COMMODITY_TYPE_BLT = 'L';
//看跌水平期权
const TapCommodityTypeType		COMMODITY_TYPE_BRT = 'Q';
//外汇 直接汇率 USD是基础货币
const TapCommodityTypeType		COMMODITY_TYPE_DIRECTFOREX = 'X';
//外汇 直接汇率1 非USD是基础货币
const TapCommodityTypeType		COMMODITY_TYPE_DIRECTFOREXR = 'Y';
//外汇 间接汇率
const TapCommodityTypeType		COMMODITY_TYPE_INDIRECTFOREX = 'I';
//外汇 交叉汇率
const TapCommodityTypeType		COMMODITY_TYPE_CROSSFOREX = 'C';
//指数
const TapCommodityTypeType		COMMODITY_TYPE_INDEX = 'Z';
//股票
const TapCommodityTypeType		COMMODITY_TYPE_STOCK = 'T';
//中金所对锁组合
const TapCommodityTypeType		COMMODITY_TYPE_FUTURE_LOCK = 'J';
//中金所不同月份跨品种套利
const TapCommodityTypeType		COMMODITY_TYPE_SPREAD_C_COMMODITY = 'K';

extern ITradeApi *g_pTradeApi;
extern ITradeData *g_pTradeData;
extern IStarApi *g_pQuoteApi;

void CConvert::DefaultSendOrder(TSendOrder&SendOrder)
{
	memset(&SendOrder, 0, sizeof(TSendOrder));

	SendOrder.CommodityType = ctNone;
	SendOrder.OptionType = otNone;
	SendOrder.OptionType2 = otNone;
	SendOrder.OrderType = otUnDefine;
	SendOrder.OrderWay = owUndefine;
	SendOrder.ValidType = vtNone;
	SendOrder.IsRiskOrder = bNo;
	SendOrder.Direct = dNone;
	SendOrder.Offset = oNone;
	SendOrder.SellOffset = oNone;
	SendOrder.Hedge = hNone;
	SendOrder.TriggerMode = tmNone;
	SendOrder.TriggerCondition = tcNone;
	SendOrder.StrategyType = stNone;
	SendOrder.AddOneIsValid = tsDay;
	SendOrder.OrderType = otLimit;
	SendOrder.ValidType = vtGFD;
}


//字符串转交易合约
bool CConvert::StrToTradeContract(const char * szCode, TContractKey & ContractKey)//保证传过来的字符串左右两端都去掉空格了
{
	vector<string> vecstr;		//保存所有满足的
	string tempstr(szCode);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), ' ');
		if (tempit != tempstr.end())
		{
			iterator_traits<string::iterator>::difference_type dis = distance(tempstr.begin(), tempit);
			vecstr.push_back(tempstr.substr(0, dis));
			if (++tempit != tempstr.end())
			{
				tempstr.erase(tempstr.begin(), tempit);
				tempit = tempstr.begin();
			}
			else	break;
		}
		else//没找到' '但有内容  类似abc;这是仍然要保存
		{
			iterator_traits<string::iterator>::difference_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	vector<string>::size_type iCount = vecstr.size();
	if (iCount < 2 || iCount>5)	return false;
	//防止连续输入多个空格
	for (vector<string>::size_type i = 0; i < iCount; i++)
	{
		if (vecstr[i].empty())	return false;
	}
	strncpy_s(ContractKey.ExchangeNo, sizeof(ContractKey.ExchangeNo), vecstr[0].c_str(), _TRUNCATE);					//交易所
	strncpy_s(ContractKey.CommodityNo, sizeof(ContractKey.CommodityNo), vecstr[1].c_str(), _TRUNCATE);				//商品
	ContractKey.OptionType = otNone;
	ContractKey.OptionType2 = otNone;
	string str;
	if (iCount == 2)
	{
		ContractKey.CommodityType = ctGoods;
		int iPs = vecstr[1].find('(');
		if (string::npos != iPs)	ContractKey.CommodityType = ctDefer;
	}
	else if (iCount == 3)														//期货或者期货组合
	{
		int iPos2 = vecstr[1].find('&');
		if (string::npos != iPos2)	//跨品种
		{
			ContractKey.CommodityType = ctSpreadCommodity;
			strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
			strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[2].c_str(), _TRUNCATE);
			return true;
		}
		int isubPos = vecstr[2].find('/');
		if (string::npos != isubPos)										//组合
		{
			if (++isubPos == string::npos)		return false;
			ContractKey.CommodityType = ctSpreadMonth;
			str = vecstr[2].substr(0, --isubPos);
			strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), str.c_str(), _TRUNCATE);
			strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[2].substr(++isubPos, vecstr[2].size() - isubPos).c_str(), _TRUNCATE);
		}
		else															//单腿
		{
			ContractKey.CommodityType = ctFutures;
			strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
		}
	}
	else if (4 == iCount)													//期权
	{
		ContractKey.CommodityType = ctOption;
		strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
		if (otCall == vecstr[3].at(0))									//看涨
			ContractKey.OptionType = otCall;
		else if (otPut == vecstr[3].at(0))									//看跌
			ContractKey.OptionType = otPut;
		strncpy_s(ContractKey.StrikePrice, sizeof(ContractKey.StrikePrice), vecstr[3].substr(1).c_str(), _TRUNCATE);
	}
	else if (5 == iCount)													//期权组合 目前为六种 并且第一腿 第二腿有严格的顺序
	{
		strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
		if (otCall == vecstr[3].at(0))									//看涨 第一腿
			ContractKey.OptionType = otCall;
		else if (otPut == vecstr[3].at(0))									//看跌
			ContractKey.OptionType = otPut;
		vecstr[3] = vecstr[3].substr(1);
		int isubPos = vecstr[3].find('/');
		if (string::npos != isubPos)										//组合
		{
			if (++isubPos == string::npos)
				return false;
			str = vecstr[3].substr(0, --isubPos);							//
			strncpy_s(ContractKey.StrikePrice, sizeof(ContractKey.StrikePrice), str.c_str(), _TRUNCATE);				//4300/307
			strncpy_s(ContractKey.ContractNo2, sizeof(ContractKey.ContractNo2), vecstr[3].substr(++isubPos, vecstr[3].size() - isubPos).c_str(), _TRUNCATE);
		}

		if (otCall == vecstr[4].at(0))									//看涨 第二腿
			ContractKey.OptionType2 = otCall;
		else if (otPut == vecstr[4].at(0))									//看跌
			ContractKey.OptionType2 = otPut;
		strncpy_s(ContractKey.StrikePrice2, sizeof(ContractKey.StrikePrice2), vecstr[4].substr(1).c_str(), _TRUNCATE);

		//判断品种类型
		if (0 == strcmp(ContractKey.ContractNo, ContractKey.ContractNo2))	//合约月份相同
		{
			if (ContractKey.OptionType == ContractKey.OptionType2)			//同看涨或同看跌
			{
				if ((strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) > 0) &&
					(otPut == ContractKey.OptionType))					//执行价1>执行价2 同看跌 BER 4-看跌期权垂直价差
				{
					ContractKey.CommodityType = ctBER;
				}
				else if ((strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) < 0) &&
					(otCall == ContractKey.OptionType))			 		///执行价1<执行价2 同看涨 BUL 3-看涨期权垂直价差 
				{
					ContractKey.CommodityType = ctBUL;
				}
				else	return false;
			}
			else														//看涨看跌或者看跌看涨
			{
				if ((otCall == ContractKey.OptionType) &&
					(otPut == ContractKey.OptionType2))					//第一腿看涨 第二腿看跌
				{
					if (0 == strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2))
						ContractKey.CommodityType = ctSTD;				//STD 7-跨式组合
					else if (strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2) > 0)
						ContractKey.CommodityType = ctSTG;				//STG 8-宽跨式组合
				}
				else	return false;
			}
		}
		else															//BLT 5-看涨期权水平价差 BRT 6-看跌期权水平价差
		{
			if ((0 == strcmp(ContractKey.StrikePrice, ContractKey.StrikePrice2)) &&
				(ContractKey.OptionType == ContractKey.OptionType2))		//月份不同、执行价相同、同看涨或者同看跌
			{
				if (otPut == ContractKey.OptionType)						//
					ContractKey.CommodityType = ctBRT;
				else if (otCall == ContractKey.OptionType)
					ContractKey.CommodityType = ctBLT;
				else	return false;
			}
			else		return false;
		}
	}
	return true;
}



bool CConvert::PosContractToStr(string & strCode, const TContractSingleKey & SingContractKey)//
{
	char str[80] = "";
	if (SingContractKey.CommodityType == ctFutures || SingContractKey.CommodityType == ctGoods || SingContractKey.CommodityType == ctDefer)//期货,现货
		sprintf_s(str, "%s %s %s", SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo);
	else if (SingContractKey.CommodityType == ctOption)//期权
		sprintf_s(str, "%s %s %s %c%s",SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
	strCode = str;
	return true;
}


bool CConvert::IsCombContract(const char CommodityType)
{
	if (ctSpreadMonth == CommodityType || ctSpreadCommodity == CommodityType || ctSpreadLocal == CommodityType ||
		ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
		ctSTD == CommodityType || ctSTG == CommodityType)
		return true;
	return false;
}

bool CConvert::DecimalsToFractions(double inPrice, int denominator, SFractionPrice & out)
{
	char cMolecular[11] = { 0 };
	int   iInteger = 0;
	double fDecimal = 0;							//分数的整数部分和小数部分
	iInteger = (int)inPrice;
	fDecimal = inPrice - (float)iInteger;
	double fPrice = fDecimal*denominator;
	sprintf_s(cMolecular, "%0.*lf", 0, fPrice);	//四舍五入分子部分 ,分母部分就是Precision

	out.iInteger = iInteger;						//整数部分
	out.iMolecules = (int)atof(cMolecular);		//分子 分母就是Precision MiniChangePrice是每次要变动的单位
	out.idenominator = denominator;
	return true;
}

string CConvert::FormatPriceToStr(double inPrice, SOFCommodityInf &mInfo)
{
	char sPrice[50];
	if (mInfo.bIfFractionPrice)
	{
		SFractionPrice sfPrice;
		DecimalsToFractions(inPrice, mInfo.iPrecision, sfPrice);
		sprintf_s(sPrice, "%d+%d/%d", sfPrice.iInteger, sfPrice.iMolecules, sfPrice.idenominator);
	}
	else
	{
		char format[50];
		sprintf_s(format, "%%0.%df", mInfo.iPrecision);//卖一价
		sprintf_s(sPrice, format, inPrice);
	}
	return sPrice;
}
//是否整除
bool CConvert::IsDiv(const double Value, const double SubValue, const double ErrorDiff)
{
	double n = 0;
	double m = modf(abs(Value / SubValue), &n);
	return m < ErrorDiff || 1 - m < ErrorDiff; //误差使值变大或误差使值变小两钟情况处理
}

void CConvert::GetSwapPositionVol(const char * UserNo, TContractSingleKey &SigCont1, TContractSingleKey&SigCont2,
	PosCount&posCount)
{
	if (UserNo == NULL)
		return;
	uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
	uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
	g_pTradeData->GetPositionVol(UserNo, &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
	g_pTradeData->GetPositionVol(UserNo, &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);

	//买入开仓 第一腿买入开仓(持第一腿买) 
	posCount.BuyOpenCount = LongCount1;
	//买入平仓 第一腿买入平仓 第二腿卖出开仓(持第二腿卖)
	posCount.BuyCoverCount = ShortCount2;
	//卖出开仓 第一腿卖出开仓(持第一腿卖) 
	posCount.SellOpenCount = ShortCount1;
	//卖出平仓 第一腿卖出平仓 第二腿买入开仓(持第二腿买)
	posCount.SellCoverCount = LongCount2;
}


//正则表达式匹配
bool CConvert::MatchPattern(const char *pattern, const char *target)//正则匹配
{
	cmatch res;
	regex rx(pattern);
	regex_search(target, res, rx);
	return !res.empty();
}

bool CConvert::DoubleSameValue(const double d1, const double d2, double Epsilon)
{
	if (Epsilon == 0)
		Epsilon = max(min(abs(d1), abs(d2)) * (1e-15) * 1000, (1e-15) * 1000);
	if (d1 > d2)
		return (d1 - d2) <= Epsilon;
	else
		return (d2 - d1) <= Epsilon;
};

bool CConvert::SplitNotice(const string &strContent, char cSplit, vector<NoticeContent> &vecstr)
{
	vecstr.clear();
	string sContent = strContent;
	string::size_type isplit = sContent.find(cSplit);
	NoticeContent tmp;
	if (string::npos == isplit)
	{
		string::size_type iIdent = strContent.find('=');
		tmp.strKey = strContent.substr(0, iIdent);
		tmp.strContent = strContent.substr(++iIdent, string::npos);
		vecstr.push_back(tmp);
		return true;
	}
	while (string::npos != isplit)
	{
		string strtmp = sContent.substr(0, isplit++);
		string::size_type iIdent = strtmp.find('=');
		if (string::npos != iIdent)
		{
			tmp.strKey = strtmp.substr(0, iIdent);
			tmp.strContent = strtmp.substr(++iIdent, string::npos);
			vecstr.push_back(tmp);
		}

		sContent = sContent.substr(isplit, string::npos);
		isplit = sContent.find(cSplit);
		if (isplit == string::npos)
		{
			iIdent = sContent.find('=');
			if (string::npos != iIdent)
			{
				tmp.strKey = sContent.substr(0, iIdent);
				tmp.strContent = sContent.substr(++iIdent, string::npos);
				vecstr.push_back(tmp);
			}
		}
	}
	return true;
}
bool CConvert::SplitNoticePos(const string &strContent, char cSplit, vector<NoticeContent> &vecstr,int nContNum)
{
	string sContent = strContent;
	string::size_type npos = sContent.find(';');
	if (string::npos != npos)
	{
		sContent = sContent.substr(++npos, string::npos);
		npos = sContent.find(';');
		NoticeContent tmp;
		for (int i = 0; i < nContNum-2; i++)
		{
			if (string::npos != npos)
			{
				string strtmp = sContent.substr(0, npos++);
				string::size_type iIdent = strtmp.find('=');
				if (string::npos != iIdent)
				{
					tmp.strKey = strtmp.substr(0, iIdent);
					tmp.strContent = strtmp.substr(++iIdent, string::npos);
					vecstr.push_back(tmp);
				}
				else
					return false;
				sContent = sContent.substr(npos, string::npos);
				npos = sContent.find(';');
			}
			else
				return false;
		}
		string::size_type iIdent = sContent.find('=');
		if (string::npos != iIdent)
		{
			tmp.strKey = sContent.substr(0, iIdent);
			tmp.strContent = sContent.substr(++iIdent, string::npos);
			vecstr.push_back(tmp);
		}
		else
			return false;
	}
	else
		return false;

	return true;

}

vector<string> CConvert::SplitString(const string & str, char cSplit)
{
	vector<string> vecstr;
	string tempstr(str);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), cSplit);
		if (tempit != tempstr.end())
		{
			iterator_traits<string::iterator>::difference_type dis = distance(tempstr.begin(), tempit);
			vecstr.push_back(tempstr.substr(0, dis));
			if (++tempit != tempstr.end())
			{
				tempstr.erase(tempstr.begin(), tempit);
				tempit = tempstr.begin();
			}
			else
				break;
		}
		else//没找到' '但有内容  类似abc;这是仍然要保存
		{
			iterator_traits<string::iterator>::difference_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}
TCommodityType CConvert::QuoteTypeToTradeType(char cType)
{
	switch (cType)
	{
	case COMMODITY_TYPE_NONE:				return ctNone;
	case COMMODITY_TYPE_SPOT:				return ctGoods;
	case COMMODITY_TYPE_FUTURES:			return ctFutures;
	case COMMODITY_TYPE_OPTION:				return ctOption;
	case COMMODITY_TYPE_SPREAD_MONTH:		return ctSpreadMonth;
	case COMMODITY_TYPE_SPREAD_COMMODITY:	return ctSpreadCommodity;
	case COMMODITY_TYPE_BUL:				return ctBUL;
	case COMMODITY_TYPE_BER:				return ctBER;
	case COMMODITY_TYPE_STD:				return ctSTD;
	case COMMODITY_TYPE_STG:				return ctSTG;
	case COMMODITY_TYPE_PRT:				return ctPRT;
	case COMMODITY_TYPE_BLT:				return ctBLT;
	case COMMODITY_TYPE_BRT:				return ctBRT;
	case COMMODITY_TYPE_DIRECTFOREX:
	case COMMODITY_TYPE_DIRECTFOREXR:		return ctDirect;
	case COMMODITY_TYPE_INDIRECTFOREX:		return ctInDirect;
	case COMMODITY_TYPE_CROSSFOREX:			return ctCross;
	case COMMODITY_TYPE_INDEX:				return ctIndexNumber;
	case COMMODITY_TYPE_STOCK:				return ctStocks;
	default:
		return cType;
	}
}
//交易品种类型转行情
TCommodityType CConvert::TradeTypeToQuoteType(char cType)
{
	switch (cType)
	{
	case ctNone:				return COMMODITY_TYPE_NONE;
	case ctGoods:				return COMMODITY_TYPE_SPOT;
	case ctFutures:				return COMMODITY_TYPE_FUTURES;
	case ctOption:				return COMMODITY_TYPE_OPTION;
	case ctSpreadMonth:			return COMMODITY_TYPE_SPREAD_MONTH;
	case ctSpreadCommodity:		return COMMODITY_TYPE_SPREAD_COMMODITY;
	case ctBUL:					return COMMODITY_TYPE_BUL;
	case ctBER:					return COMMODITY_TYPE_BER;
	case ctSTD:					return COMMODITY_TYPE_STD;
	case ctSTG:					return COMMODITY_TYPE_STG;
	case ctPRT:					return COMMODITY_TYPE_PRT;
	case ctBLT:					return COMMODITY_TYPE_BLT;
	case ctBRT:					return COMMODITY_TYPE_BRT;
	case ctDirect:				return COMMODITY_TYPE_DIRECTFOREX;//COMMODITY_TYPE_DIRECTFOREXR
	case ctInDirect:			return COMMODITY_TYPE_INDIRECTFOREX;
	case ctCross:				return COMMODITY_TYPE_CROSSFOREX;
	case ctIndexNumber:			return COMMODITY_TYPE_INDEX;
	case ctStocks:				return COMMODITY_TYPE_STOCK;
	default:
		return cType;
	}
}
//行情ID转交易关键字
void CConvert::RawContractIdTypeToTradeContract( const string &id, TContractKey &ContractKey)
{
	std::vector<std::string> vecStr;
	memset(&ContractKey, 0, sizeof(TContractKey));

	ContractKey.ExchangeNo;
	ContractKey.CommodityType = ctNone;
	ContractKey.CommodityNo;
	ContractKey.ContractNo;
	ContractKey.StrikePrice;
	ContractKey.OptionType = otNone;
	ContractKey.ContractNo2;
	ContractKey.StrikePrice2;
	ContractKey.OptionType2 = otNone;

	vecStr = SplitString(id.c_str(), '|');
	if (vecStr.size() < 3)
		return;

	strncpy_s(ContractKey.ExchangeNo, vecStr[0].c_str(), sizeof(ContractKey.ExchangeNo) - 1);
	ContractKey.CommodityType = QuoteTypeToTradeType(vecStr[1].c_str()[0]);
	strncpy_s(ContractKey.CommodityNo, vecStr[2].c_str(), sizeof(ContractKey.CommodityNo) - 1);
	//按类型填充字段
	switch (ContractKey.CommodityType)
	{
		//期货 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]
	case ctFutures:
		//指数
	case ctIndexNumber:
		if (vecStr.size() > 3)
			strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
		break;
		//期权	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
	case ctOption:
		if (vecStr.size() > 3)
			ParseOptionContractStr(vecStr[3], ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
		break;
		//跨期套利 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]|<YEAR><MONTH>[DAY]
	case ctSpreadMonth:
		if (vecStr.size() > 4)
		{
			strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
			strncpy_s(ContractKey.ContractNo2, vecStr[4].c_str(), sizeof(ContractKey.ContractNo2) - 1);
		}
		break;
		//跨品种套利 <Exg>|<Type>|<Root&Root>|<YEAR><MONTH>[DAY]
	case ctSpreadCommodity:
		if (vecStr.size() > 3)
		{
			strncpy_s(ContractKey.ContractNo, vecStr[3].c_str(), sizeof(ContractKey.ContractNo) - 1);
			strncpy_s(ContractKey.ContractNo2, vecStr[3].c_str(), sizeof(ContractKey.ContractNo2) - 1);
		}
		break;
		//看涨垂直套利
	case ctBUL:
		//看跌垂直套利
	case ctBER:
		//跨式套利
	case ctSTD:
		//宽跨式套利
	case ctSTG:
		//期权套利	<Exg>|<Type>|<Root[&Root]>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
		if (vecStr.size() > 4)
		{
			ParseOptionContractStr(vecStr[3], ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
			ParseOptionContractStr(vecStr[4], ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
		}
		break;
		//备兑组合
	case ctPRT:
		//现货
	case ctGoods:
		//外汇——直接汇率
	case ctDirect:
		//外汇——间接汇率
	case ctInDirect:
		//外汇——交叉汇率
	case ctCross:
		//股票
	case ctStocks:
	case ctStatics:
		break;
	default:
		break;
	}
}
//交易合约转为行情ID
void CConvert::TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51])
{
	ContractId[0] = '\0';
	switch (ContractKey.CommodityType)
	{
		//期货 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]
	case ctFutures:
		//指数
	case ctIndexNumber:
		sprintf_s(ContractId, "%s|%c|%s|%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo);
		break;
		//期权	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
	case ctOption:
		sprintf_s(ContractId, "%s|%c|%s|%s%c%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo,
			ContractKey.OptionType,
			ContractKey.StrikePrice);
		break;
		//跨期套利 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]|<YEAR><MONTH>[DAY]
	case ctSpreadMonth:
		sprintf_s(ContractId, "%s|%c|%s|%s|%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo,
			ContractKey.ContractNo2);
		break;
		//跨品种套利 <Exg>|<Type>|<Root&Root>|<YEAR><MONTH>[DAY]
	case ctSpreadCommodity:
		sprintf_s(ContractId, "%s|%c|%s|%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo);
		break;
		//看涨垂直套利
	case ctBUL:
		//看跌垂直套利
	case ctBER:
		//跨式套利
	case ctSTD:
		//宽跨式套利
	case ctSTG:
		//期权套利	<Exg>|<Type>|<Root[&Root]>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
		sprintf_s(ContractId, "%s|%c|%s|%s%c%s|%s%c%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo,
			ContractKey.OptionType,
			ContractKey.StrikePrice,
			ContractKey.ContractNo2,
			ContractKey.OptionType2,
			ContractKey.StrikePrice2);
		break;
		//备兑组合
	case ctPRT:
		break;
		//现货
	case ctGoods:
		//外汇——直接汇率
	case ctDirect:
		//外汇——间接汇率
	case ctInDirect:
		//外汇——交叉汇率
	case ctCross:
		//股票
	case ctStocks:
	case ctStatics:
		sprintf_s(ContractId, "%s|%c|%s|",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo);
		break;
	default:
		sprintf_s(ContractId, "Unknown Contract:%c", CConvert::TradeTypeToQuoteType(ContractKey.CommodityType));
		break;
	}
}
//交易合约转为行情ID
void CConvert::SingleTradeContractToRawContractIdType(const TContractSingleKey &ContractKey, char(&ContractId)[51])
{
	ContractId[0] = '\0';
	switch (ContractKey.CommodityType)
	{
		//期货 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]
	case ctFutures:
		//指数
	case ctIndexNumber:
		sprintf_s(ContractId, "%s|%c|%s|%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo);
		break;
		//期权	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
	case ctOption:
		sprintf_s(ContractId, "%s|%c|%s|%s%c%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo,
			ContractKey.OptionType,
			ContractKey.StrikePrice);
		break;
		//备兑组合
	case ctPRT:
		break;
		//现货
	case ctGoods:
		//外汇——直接汇率
	case ctDirect:
		//外汇——间接汇率
	case ctInDirect:
		//外汇——交叉汇率
	case ctCross:
		//股票
	case ctStocks:
	case ctStatics:
		sprintf_s(ContractId, "%s|%c|%s|",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo);
		break;
	default:
		sprintf_s(ContractId, "Unknown Contract:%c", CConvert::TradeTypeToQuoteType(ContractKey.CommodityType));
		break;
	}
}
//获取最小变动价格
double CConvert::GetMinChangePrice(const SOFCommodityInf & sCommodityInfo)
{
	double MiniChangePrice = sCommodityInfo.dMiniChangePrice;
	if (sCommodityInfo.bIfFractionPrice)//分数报价
		MiniChangePrice = (sCommodityInfo.dMiniChangePrice / sCommodityInfo.iPrecision);
	return MiniChangePrice;
}
//API 与配置无关 返回0成功 返回负号失败  查询参数失败
int CConvert::QrySingleContractCanOpen(const TSendOrder &SendOrder)
{
	//应该根据Direct来计算可开数量
	int canqty = 0;
	if (g_pTradeData)
	{
		int iret = g_pTradeData->GetTradableQty(SendOrder, canqty);
		if (0 != iret)
			canqty = -1;
	}

	return canqty;
}

bool CConvert::ParseOptionContractStr(const std::string& strValue, char(&ContractNo)[11], TOptionType &OptionType, char(&StrikePrice)[11])
{
	bool bRet = true;
	do{
		string::size_type posFlag = std::string::npos;
		if (std::string::npos != (posFlag = strValue.find(otCall)))
			OptionType = otCall;
		else if (std::string::npos != (posFlag = strValue.find(otPut)))
			OptionType = otPut;
		else
		{
			bRet = false;
			break;
		}
		strncpy_s(ContractNo, strValue.substr(0, posFlag).c_str(), sizeof(ContractNo) - 1);
		strncpy_s(StrikePrice, strValue.substr(posFlag + 1).c_str(), sizeof(StrikePrice) - 1);
	} while (false);
	return bRet;
}


//获取资金账户信息
bool CConvert::GetAllUserNo(vector<CStringsVec>&vecUserNo)
{
	//获取登陆
	if (!g_pTradeData) return false;
	TDataContainer tempDataCon;
	g_pTradeData->GetAllUser(tempDataCon);
	CStringsVec vecStr;
	uint num = tempDataCon.size();
	for (uint i = 0; i < num; i++)   //对每一个客户处理
	{
		vecStr.clear();
		TUserInfo * Ref = ((TUserInfo*)(tempDataCon.at(i)));
		if (!(Ref->pLogin && (bYes == Ref->pLogin->Logined)))	continue;
		//1
		vecStr.push_back(Ref->UserNo);
		//2
		if (Ref->UserName[0] == '\0')
			vecStr.push_back(Ref->UserNo);
		else
			vecStr.push_back(Ref->UserName);
		//3 保存sign但并不需要显示
		vecStr.push_back(Ref->pLogin->Sign);
		vecUserNo.push_back(vecStr);
	}
	return true;
}
//是否有效行情合约
bool CConvert::IsValidQuotContract(const  char * ContractId)
{
	return true;
}
//交易字符串到行情id
bool CConvert::StrToRawContractIdType(const char *code, char(&ContractId)[51])
{
	TContractKey	TradeContract;	//交易合约
	memset(&TradeContract, 0, sizeof(TradeContract));
	StrToTradeContract(code, TradeContract);
	TradeContractToRawContractIdType(TradeContract, ContractId);	
	return  IsValidQuotContract(ContractId);
	
}
void CConvert::TrimString(string &str)
{
	string::size_type pos = str.find_first_not_of(' ');
	if (pos == string::npos)
	{
		return;
	}
	string::size_type pos2 = str.find_last_not_of(' ');
	if (pos2 != string::npos)
	{
		str.substr(pos, pos2 - pos + 1);
		return;
	}
	str.substr(pos);
	return;
}

//检查是否有交易员登录过
bool CConvert::IsTraderLogined()
{
	TDataContainer tempUser;
	g_pTradeData->GetAllLogin(tempUser);
	for (uint i = 0; i < tempUser.size(); i++)
	{
		const TLogin *pLogin = ((TLogin*)(tempUser.at(i)));
		if (pLogin && (pLogin->Logined == bYes) && (uiTrader == pLogin->UserType) && pLogin->ReadyCount>0)
			return true;
	}
	return false;
}

void CConvert::SplitString(string strContent, char split, vector<string> &content)
{
	content.clear();
	string::size_type iSplit = strContent.find(split);
	string strTemp;
	while (iSplit != string::npos)
	{
		strTemp = strContent.substr(0, iSplit);
		strContent = strContent.substr(++iSplit, string::npos);
		content.push_back(strTemp);
		iSplit = strContent.find(split);
	}
	content.push_back(strContent);
}

bool CConvert::IsSHFE(const char * exNo)
{
	return strcmp(exNo, S_SHFE) == 0;
}



void CConvert::TradeSingleContractToRawId(const TContractSingleKey* ContractSingleKey, char(&ContractId)[51])
{
	ContractId[0] = '\0';
	switch (ContractSingleKey->CommodityType)
	{
		//期货 <Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]
	case ctFutures:
		//指数
	case ctIndexNumber:
		sprintf_s(ContractId, "%s|%c|%s|%s",
			ContractSingleKey->ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractSingleKey->CommodityType),
			ContractSingleKey->CommodityNo,
			ContractSingleKey->ContractNo);
		break;
		//期权	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
	case ctOption:
		sprintf_s(ContractId, "%s|%c|%s|%s%c%s",
			ContractSingleKey->ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractSingleKey->CommodityType),
			ContractSingleKey->CommodityNo,
			ContractSingleKey->ContractNo,
			ContractSingleKey->OptionType,
			ContractSingleKey->StrikePrice);
		break;
	default:
		break;
	}
}

bool CConvert::newGUID(char* buf)
{
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		sprintf_s(buf, 55, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2,
			guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		return true;
	}
	return false;
}



//void CConvert::GetCloseNum(const char* UserNo, TContractSingleKey* ContractKey, TDirect direct, const int iCoverQty, int& iCoverNum, int& iTCoverNum)
//{
//	if (iCoverQty == 0)
//		return;
//	iCoverNum = 0;
//	iTCoverNum = 0;
//
//	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
//	g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
//	//new
//	if (g_pGlobalModel->m_utUserType == utForeign)
//	{
//		int iParCount = g_pGlobalModel->GetForeignCloseNum(*ContractKey, direct);
//		if (direct == dSell)
//		{
//			if (LongCount > iParCount)
//				iCoverNum = LongCount - iParCount;
//		}
//		else
//		{
//			if (ShortCount > iParCount)
//				iCoverNum = ShortCount - iParCount;
//		}
//		return;
//	}
//	//new
//	const TOrderQty* stOrderQty = g_pTradeData->GetParOrderQty(UserNo, ContractKey);
//	int iAvaQty = 0;
//	int iTAvaQty = 0;
//	if (stOrderQty == NULL)
//	{
//		if (!CConvert::IsDistinguishCoverType(ContractKey))
//			iCoverNum = iCoverQty;
//		else
//		{
//			//获取可用持仓量
//			if (direct == dBuy)
//			{
//				iAvaQty = ShortCount;
//				iTAvaQty = DayShortCount;
//			}
//			else if (direct == dSell)
//			{
//				iAvaQty = LongCount;
//				iTAvaQty = DayLongCount;
//			}
//			if (iCoverQty <= iTAvaQty)
//			{
//				iCoverNum = 0;
//				iTCoverNum = iCoverQty;
//			}
//			else if (iCoverQty > iTAvaQty && iCoverQty < iAvaQty)
//			{
//				iTCoverNum = iTAvaQty;
//				iCoverNum = iCoverQty - iTAvaQty;
//			}
//			else if (iCoverQty >= iAvaQty)
//			{
//				iTCoverNum = iTAvaQty;
//				iCoverNum = iAvaQty - iTAvaQty;
//			}
//		}
//	}
//	else if (!CConvert::IsDistinguishCoverType(ContractKey))
//	{
//		//获取可用持仓量
//		if (direct == dBuy)
//			iAvaQty = ShortCount - stOrderQty->BuyCoverQty;
//		else if (direct == dSell)
//			iAvaQty = LongCount - stOrderQty->SellCoverQty;
//		if (iCoverQty >= iAvaQty)
//			iCoverNum = iAvaQty;
//		else
//			iCoverNum = iCoverQty;
//	}
//	else
//	{
//		//获取可用持仓量
//		if (direct == dBuy)
//		{
//			iAvaQty = ShortCount - stOrderQty->BuyCoverDayQty - stOrderQty->BuyCoverQty;
//			iTAvaQty = DayShortCount - stOrderQty->BuyCoverDayQty;
//		}
//		else if (direct == dSell)
//		{
//			iAvaQty = LongCount - stOrderQty->SellCoverDayQty - stOrderQty->SellCoverQty;
//			iTAvaQty = DayLongCount - stOrderQty->SellCoverDayQty;
//		}
//		if (iCoverQty <= iTAvaQty)
//		{
//			iCoverNum = 0;
//			iTCoverNum = iCoverQty;
//		}
//		else if (iCoverQty > iTAvaQty && iCoverQty < iAvaQty)
//		{
//			iTCoverNum = iTAvaQty;
//			iCoverNum = iCoverQty - iTAvaQty;
//		}
//		else if (iCoverQty >= iAvaQty)
//		{
//			iTCoverNum = iTAvaQty;
//			iCoverNum = iAvaQty - iTAvaQty;
//		}
//	}
//}



bool CConvert::IsEsunnyUser()
{
	TDataContainer stUser;
	g_pTradeData->GetAllUser(stUser);

	for (uint i = 0; i < stUser.size(); i++)
	{
		const TUserInfo* pUser = (const TUserInfo*)stUser.at(i);
		if (pUser->pLogin)
		{
			if (strcmp(pUser->pLogin->LoginClassName, UnicodeToANSI(PLUG_DAYSTARTRADEAPI_NAME).c_str()) == 0
				|| strcmp(pUser->pLogin->LoginClassName, UnicodeToANSI(PLUG_DIPPERTRADEAPI_NAME).c_str()) == 0
				|| strcmp(pUser->pLogin->LoginClassName, UnicodeToANSI(PLUG_POLESTARTRADEAPI_NAME).c_str()) == 0
				|| strcmp(pUser->pLogin->LoginClassName, UnicodeToANSI(PLUG_ESUNNYTRADEAPI_NAME).c_str()) == 0
				|| strcmp(pUser->pLogin->LoginClassName, UnicodeToANSI(PLUG_FOREIGNTRADEAPI_NAME).c_str()) == 0
				)
				return true;
		}
	}
	return false;
}

char* CConvert::WcharToChar(const wchar_t* wc)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), NULL, 0, NULL, NULL);
	char *m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

wstring CConvert::UTF8ToUnicode(const string& str)
{
	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));

	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
	wstring   rt = (wchar_t*)pUnicode;
	delete[]  pUnicode;

	return  rt;
}