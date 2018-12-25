#include "PreInclude.h"

using namespace std;
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

extern IStarApi *g_StarApi;


//行情ID转交易关键字
void CConvert::SContractNoTypeToTradeContract(const char *szSign, const string &id, TContractKey &ContractKey)
{
    std::vector<std::string> vecStr;
    memset(&ContractKey, 0, sizeof(TContractKey));

    strcpy_s(ContractKey.Sign, szSign);
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
        /*case COMMODITY_TYPE_DIRECTFOREX:
        case COMMODITY_TYPE_DIRECTFOREXR:		return ctDirect;*/
    case COMMODITY_TYPE_INDIRECTFOREX:		return ctInDirect;
    case COMMODITY_TYPE_CROSSFOREX:			return ctCross;
    case COMMODITY_TYPE_INDEX:				return ctIndexNumber;
    case COMMODITY_TYPE_STOCK:				return ctStocks;
    case 'Y':								return ctDefer;
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

std::wstring CConvert::UTF8ToUnicode(const std::string& str)
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

void CConvert::SContractNoTypeToTradeCommodity(const char *szSign, const std::string &id, TCommodityKey &CommodityKey)
{
    memset(&CommodityKey, 0, sizeof(TCommodityKey));
    strcpy_s(CommodityKey.Sign, szSign);
    std::vector<std::string> vecStr;
    vecStr = SplitString(id.c_str(), '|');
    if (vecStr.size() < 3)
        return;
    strncpy_s(CommodityKey.ExchangeNo, vecStr[0].c_str(), sizeof(CommodityKey.ExchangeNo) - 1);
    CommodityKey.CommodityType = QuoteTypeToTradeType(vecStr[1].c_str()[0]);
    strncpy_s(CommodityKey.CommodityNo, vecStr[2].c_str(), sizeof(CommodityKey.CommodityNo) - 1);
}



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
	SendOrder.SellHedge = hNone;
}
bool CConvert::IsLME(const TExchangeKey &Exchange)
{ 
    return strcmp(Exchange.ExchangeNo, "LME") == 0;
}
//判断是否是港交所期货
bool CConvert::IsHK(const char *szCode)
{
	if (!szCode)	return false;
	TContractKey temp;
	memset(&temp, 0, sizeof(TContractKey));
	TContractNoToTContractKey(szCode, temp);

	if ((0 == strcmp(temp.ExchangeNo, "HKEX") || 0 == strcmp(temp.ExchangeNo, "HKFE") || 0 == strcmp(temp.ExchangeNo,"HKF"))
		/*&& temp.CommodityType == ctFutures*/)
		return true;
	return false;
}

//字符串转交易合约
bool CConvert::TContractNoToTContractKey(const char * szCode, TContractKey & ContractKey)//保证传过来的字符串左右两端都去掉空格了
{
	vector<string> vecstr;		//保存所有满足的
	string tempstr(szCode);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), ' ');
		if (tempit != tempstr.end())
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempstr.begin(), tempit);
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
			iterator_traits<string::iterator>::value_type dis = distance(tempit, tempstr.end());
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


//交易合约转字符串 bOnlyContract为true时只需要格式化合约部分 不需要交易所和商品
bool CConvert::TContractKeyToTContractNo(const TContractKey & ContractKey, string & strCode)//
{
    char strText[100] = "";
    if (ContractKey.CommodityType == ctFutures || ContractKey.CommodityType == ctGoods || ContractKey.CommodityType == ctDefer)//期货,现货、现货延期
    {
        sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
    }
    else if (ContractKey.CommodityType == ctOption)//期权
    {
        sprintf_s(strText, "%s %s %s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
    }
    else if (ContractKey.CommodityType == ctSpreadMonth)//期货的 跨期套利
    {
        sprintf_s(strText, "%s %s %s/%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
    }
    else if (ContractKey.CommodityType == ctSpreadCommodity)//跨商品套利
    {
        sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
    }
    else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//期权的 看涨期权垂直价差，看跌期权垂直价差，跨式组合，宽跨式组合，水平看涨套利，水平看跌套利	
    {
        sprintf_s(strText, "%s %s %s %c%s/%s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
    }
    strCode = strText;
    return true;
}

//期权交易合约转期权序列
bool CConvert::TradeContractToOptionTarget(const TContractSingleKey &ContractKey, char(&ContractId)[101])
{
	ContractId[0] = '\0';
	switch (ContractKey.CommodityType)
	{
		//期权	<Exg>|<Type>|<Root>|<YEAR><MONTH>[DAY]<OFlag><STRIKEPRICE>
	case ctOption:
		sprintf_s(ContractId, "%s|%c|%s|%s",
			ContractKey.ExchangeNo,
			CConvert::TradeTypeToQuoteType(ContractKey.CommodityType),
			ContractKey.CommodityNo,
			ContractKey.ContractNo);
		return true;
	default:
		return false;
	}
}
//单腿交易合约转字符串 默认不带交易所
bool CConvert::SingTradeContractToStr(string & strCode, const TContractSingleKey & SingContractKey, bool bHaveExchange,bool bHaveSign)//
{
	char str[80] = "";
	if (SingContractKey.CommodityType == ctFutures || SingContractKey.CommodityType == ctGoods || SingContractKey.CommodityType == ctDefer)//期货,现货
	{
		if (bHaveSign)
		{
			if (bHaveExchange)
				sprintf_s(str, "%s %s %s %s", SingContractKey.Sign, SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo);
			else
				sprintf_s(str, "%s %s %s", SingContractKey.Sign, SingContractKey.CommodityNo, SingContractKey.ContractNo);
		}
		else
		{
			if (bHaveExchange)
				sprintf_s(str, "%s %s %s", SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo);
			else
				sprintf_s(str, "%s %s", SingContractKey.CommodityNo, SingContractKey.ContractNo);
		}	
	}
	else if (SingContractKey.CommodityType == ctOption)//期权
	{
		if (bHaveSign)
		{
			if (bHaveExchange)
				sprintf_s(str, "%s %s %s %s %c%s", SingContractKey.Sign, SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
			else
				sprintf_s(str, "%s %s %s %c%s", SingContractKey.Sign, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
		}
		else
		{
			if (bHaveExchange)
				sprintf_s(str, "%s %s %s %c%s",SingContractKey.ExchangeNo, SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
			else
				sprintf_s(str, "%s %s %c%s",SingContractKey.CommodityNo, SingContractKey.ContractNo, SingContractKey.OptionType, SingContractKey.StrikePrice);
		}
	}
	strCode = str;
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

void CConvert::FormatTradeContract(const TContractKey & ContractKey, string &str)
{
	char cText[110];
	sprintf_s(cText,"%s&%s&%c&%s&%s&%s&%d&%s&%s&%d", ContractKey.Sign, ContractKey.ExchangeNo, ContractKey.CommodityType, ContractKey.CommodityNo, \
		ContractKey.ContractNo, ContractKey.StrikePrice, (unsigned char)ContractKey.OptionType, \
		ContractKey.ContractNo2, ContractKey.StrikePrice2, (unsigned char)ContractKey.OptionType2);
	str = cText;
	return;
}

bool CConvert::IsInnerDiskExchange(const char *szExchange)
{
	if (!strcmp(exZCE, szExchange) || !strcmp(exCFFEX, szExchange) || !strcmp(exDCE, szExchange) || !strcmp(exSHFE, szExchange))
		return true;
	else
		return false;
}
N_ExchangeType CConvert::GetExchangeType(const char* sExchangeNo)
{
	if (strcmp(exZCE, sExchangeNo) == 0)
		return N_ZCE;
	else if (strcmp(exCFFEX, sExchangeNo) == 0)
		return N_CFFEX;
	else if (strcmp(exDCE, sExchangeNo) == 0)
		return N_DCE;
	else if (strcmp(exSHFE, sExchangeNo) == 0)
		return N_SHFE;
	else
		return N_NONE;
}

bool CConvert::IsCombContract(const char CommodityType)
{
	if (ctSpreadMonth == CommodityType || ctSpreadCommodity == CommodityType || ctSpreadLocal == CommodityType ||
		ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
		ctSTD == CommodityType || ctSTG == CommodityType)
		return true;
	return false;
}

bool CConvert::IsOptionContract(const char CommodityType)
{
	return (ctOption == CommodityType ||
		ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
		ctSTD == CommodityType || ctSTG == CommodityType);
}

long CConvert::Round(double Value)
{
	if (Value > 0)
		return (long)(Value + 0.5);
	else
		return (long)(Value - 0.5);
};


//是否整除
bool CConvert::IsDiv(const double Value, const double SubValue, const double ErrorDiff)
{
	double n = 0;
	double m = modf(abs(Value / SubValue), &n);
	return m < ErrorDiff || 1 - m < ErrorDiff; //误差使值变大或误差使值变小两钟情况处理
}

//正则表达式匹配
bool CConvert::MatchPattern(const char *pattern, const char *target)//正则匹配
{
	std::tr1::cmatch res;
	std::tr1::regex rx(pattern);
	std::tr1::regex_search(target, res, rx);
	return !res.empty();
}
//判断是否是合法的数字输入
bool CConvert::IsValidNumber(const char * sValue, bool bOnlyInt, bool bCanNegative)//是否有效的数字
{
	string pattern = "^(\\d+)?(\\.)?(\\d+)?$";	//整数、小数   
	if (bOnlyInt)
		pattern = "^(\\d+)?$";					//只允许为整数    
	if (bCanNegative)
		pattern.insert(1, "(-)?");				//允许为负  "^[-+]?(\\d+)?(\\.)?(\\d+)?$"

	return MatchPattern(pattern.c_str(), sValue);
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
			iterator_traits<string::iterator>::value_type dis = distance(tempstr.begin(), tempit);
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
			iterator_traits<string::iterator>::value_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}


bool CConvert::SCommodityNoToTCommodityKey(SCommodityNoType commodityno, TCommodityKey & CommodityKey)
{
    memset(&CommodityKey, 0, sizeof(TCommodityKey));

    char* pos = strchr(commodityno, '|');
    if (NULL == pos)
        return false;
    strncpy_s(CommodityKey.ExchangeNo, commodityno, pos - commodityno);
    CommodityKey.CommodityType = QuoteTypeToTradeType(pos[1]);

    //找品种
    char* pos2 = strchr(&pos[3], '|');
    if (NULL == pos2)
        strncpy_s(CommodityKey.CommodityNo, &pos[3], sizeof(SCommodityNoType) - (&pos[3] - commodityno));
    else
        strncpy_s(CommodityKey.CommodityNo, &pos[3], pos2 - &pos[3]);

    return true;
}


void CConvert::TradeContractToRelateContractNoType(const TContractKey &ContractKey, char(&ContractId)[101])
{
    SOptionSeriesNoType type = { 0 };
	sprintf_s(type, "%s|%c|%s|%s",
		ContractKey.ExchangeNo,
		COMMODITY_TYPE_FUTURES,
		ContractKey.CommodityNo,
        ContractKey.ContractNo);
    SContract *contract = 0;
    if (g_StarApi->GetOptionSeriesTarget(type, "", &contract, 1, false))
    {
        strcpy_s(ContractId, contract->ContractNo);
    }
    else
    {
        strcpy_s(ContractId, type);
    }
}
//交易合约转为行情ID
void CConvert::TradeContractToSContractNoType(const TContractKey &ContractKey, char(&ContractId)[101])
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
	case ctDefer:
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

//填充期权字段
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

//行情id到交易字符串
void CConvert::SContractNoTypeToStr(string &sQuoteCode, string &str)
{
	TContractKey	TradeContract;	//交易合约
	memset(&TradeContract, 0, sizeof(TradeContract));
	SContractNoTypeToTradeContract("", sQuoteCode, TradeContract);
	TContractKeyToTContractNo(TradeContract, str);
}

//是否有效行情合约
bool CConvert::IsValidQuotContract(const  char * ContractId)
{
	SContract *SContract = NULL;
	if (g_StarApi&&g_StarApi->GetContractData("", ContractId, &SContract, 1, false))
		return true;
	return false;
}
//是否是有效的行情品种
bool CConvert::IsValidQuoteCommodity(const char * ExchangeNo, const char * CommodityNo)
{
	int ncount = g_StarApi->GetCommodityCount(ExchangeNo);
	if (ncount == 0)
		return false;
	SCommodity **tcommodity = new SCommodity*[ncount];
	if (g_StarApi->GetCommodityData(ExchangeNo, "", tcommodity, ncount,true))
	{
		bool bFind = false;
		for (int i = 0; i < ncount; i++)
		{
			SCommodity **p = tcommodity + i;
			string sCommodity = (*p)->CommodityNo;
			int npos = sCommodity.find_last_of('|');
			if (npos != -1 && npos + 1 < (int)sCommodity.length())
				sCommodity = sCommodity.substr(npos + 1, sCommodity.length());
			else
				continue;
			if (p&&!strcmp(sCommodity.c_str(), CommodityNo))
			{
				bFind = true;
				break;
			}
		}
		delete[]tcommodity;
		return bFind;
	}
	else
	{
		delete[]tcommodity;;
		return false;
	}

}
//交易字符串到行情id
bool CConvert::StrToSContractNoType(const char *code, char(&ContractId)[101])
{
	TContractKey	TradeContract;	//交易合约
	memset(&TradeContract, 0, sizeof(TradeContract));
	TContractNoToTContractKey(code, TradeContract);
	TradeContractToSContractNoType(TradeContract, ContractId);	
	return  IsValidQuotContract(ContractId);	
}
//套利合约字符串分解
bool CConvert::SpreadStrToSpreadInfo(const char *code, char& cRateMode, char(&sFirst)[101], char(&sSecond)[101], int N_SwitchType)
{
	string strCode = code;
	cRateMode = S_SPREADMODE_DIFF;
	size_t npos = strCode.find(cRateMode);
	if (string::npos == npos)
	{
		cRateMode = S_SPREADMODE_RATIO;
		npos = strCode.find(cRateMode);
		if (string::npos == npos)
		{
			cRateMode = S_SPREADMODE_SUM;
			npos = strCode.find(cRateMode);
		}
	}
	if (npos != string::npos)
	{
		//现货延期

		memcpy(sFirst, (strCode.substr(0, npos)).c_str(), sizeof(sFirst));
		memcpy(sSecond, (strCode.substr(npos + 1, string::npos)).c_str(), sizeof(sSecond));

		switch (N_SwitchType)
		{
		case NST_T2Q:
		{
			SContractNoType sQuoteSub[2] = { 0 };
			StrToSContractNoType(sFirst, sQuoteSub[0]);
			StrToSContractNoType(sSecond, sQuoteSub[1]);
			memcpy(sFirst, sQuoteSub[0], sizeof(sFirst));
			memcpy(sSecond, sQuoteSub[1], sizeof(sSecond));
		}
		break;
		case NST_Q2T:
		{
			string sCode = sFirst;
			string sCode2 = sSecond;
			string sQuote1 = "";
			string sQuote2 = "";
			SContractNoTypeToStr(sCode, sQuote1);
			SContractNoTypeToStr(sCode2, sQuote2);
			memcpy(sFirst, sQuote1.c_str(), sizeof(sFirst));
			memcpy(sSecond, sQuote2.c_str(), sizeof(sSecond));
		}
		break;
		default:
			break;
		}
		return true;
	}
	else
		return false;
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
		str = str.substr(pos, pos2 - pos + 1);
		return;
	}
	str = str.substr(pos);
	return;
}

SPriceType CConvert::GetPriceByContract(const SContract* SContract, SFidMeanType nField, int nlevel)//（含隐含）
{
	if (!SContract || !g_StarApi)
		return 0;
	if (nlevel==0 && (nField == S_FID_BESTBIDPRICE || nField == S_FID_BESTASKPRICE))
	{
		SQuoteField qInfo;
		if (g_StarApi->GetImpliedField(SContract, nField, &qInfo))
			return qInfo.FidAttr ? qInfo.Price : 0;
		else
			return GetPriceBySnapShot(SContract->SnapShot, nField);
	}
	else
	{
		SQuoteSnapShotL2 QuoteData[2];
		memset(&QuoteData, 0, sizeof(QuoteData));
		SImpliedDepthL2 implied = { 0 };
		if (g_StarApi->GetImpliedSnapshot(SContract, &QuoteData[0], &QuoteData[1], implied))
			return GetPriceBySnapShot(QuoteData, nField, nlevel);
		else
		{
			memcpy_s(&QuoteData[0], sizeof(SQuoteSnapShotL2), SContract->BidL2, sizeof(SQuoteSnapShotL2));
			memcpy_s(&QuoteData[1], sizeof(SQuoteSnapShotL2), SContract->AskL2, sizeof(SQuoteSnapShotL2));
			return GetPriceBySnapShot(QuoteData, nField, nlevel);
		}
	}
}
SQtyType CConvert::GetQtyByContract(const SContract* SContract, SFidMeanType nField, int nlevel)//（含隐含）
{
	if (!SContract || !g_StarApi)
		return 0;
	if (nlevel == 0 && nField == S_FID_BESTBIDQTY || nField == S_FID_BESTASKQTY)
	{
		SQuoteField qInfo;
		if (g_StarApi->GetImpliedField(SContract, nField, &qInfo))
			return qInfo.FidAttr ? qInfo.Qty : 0;
		else
			return GetQtyBySnapShot(SContract->SnapShot, nField);
	}
	else
	{
		SQuoteSnapShotL2 QuoteData[2];
		memset(&QuoteData, 0, sizeof(QuoteData));
		SImpliedDepthL2 implied = { 0 };
		if (g_StarApi->GetImpliedSnapshot(SContract, &QuoteData[0], &QuoteData[1], implied))
			return GetQtyBySnapShot(QuoteData, nField, nlevel);
		else
		{
			memcpy_s(&QuoteData[0], sizeof(SQuoteSnapShotL2), SContract->BidL2, sizeof(SQuoteSnapShotL2));
			memcpy_s(&QuoteData[1], sizeof(SQuoteSnapShotL2), SContract->AskL2, sizeof(SQuoteSnapShotL2));
			return GetQtyBySnapShot(QuoteData, nField, nlevel);
		}
	}
}
SPriceType CConvert::GetPriceBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField)
{
	if (QuoteSnap)
		return QuoteSnap->Data[nField].FidAttr ? QuoteSnap->Data[nField].Price : 0;
	else
		return 0;
}

SPriceType CConvert::GetPriceBySnapShot(const SQuoteSnapShotL2 QuoteSnap[2], SFidMeanType nField, int nlevel)//
{
	if (nlevel < 0 || nlevel > 9)
		return 0;
	switch (nField)
	{
	case S_FID_BESTBIDPRICE:
		return QuoteSnap[0].Data[nlevel].Qty>0?QuoteSnap[0].Data[nlevel].Price:0;
	case S_FID_BESTASKPRICE:
		return QuoteSnap[1].Data[nlevel].Qty>0?QuoteSnap[1].Data[nlevel].Price:0;
	default:
		return 0;
	}
}

SQtyType CConvert::GetQtyBySnapShot(const SQuoteSnapShot* QuoteSnap, SFidMeanType nField)
{
	if (QuoteSnap)
		return QuoteSnap->Data[nField].FidAttr ? QuoteSnap->Data[nField].Qty : 0;
	else
		return 0;
}

SQtyType CConvert::GetQtyBySnapShot(const SQuoteSnapShotL2 QuoteSnap[2], SFidMeanType nField, int nlevel)//
{
	if (nlevel < 0 || nlevel > 9)
		return 0;
	switch (nField)
	{
	case S_FID_BESTBIDPRICE:
		return QuoteSnap[0].Data[nlevel].Qty;
	case S_FID_BESTASKPRICE:
		return QuoteSnap[1].Data[nlevel].Qty;
	default:
		return 0;
	}
}

