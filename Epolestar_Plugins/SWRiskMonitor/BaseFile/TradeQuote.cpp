#include "PreInclude.h"

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


//交易合约转字符串 bOnlyContract为true时只需要格式化合约部分 不需要交易所和商品
bool CConvert::TradeContractToStr(const TContractKey & ContractKey, string & strCode, bool bOnlyContract, bool bName, bool bOnlyName)//
{
	char strText[100] = "";
	const SContract* sContract = g_pTradeData->GetContract(&ContractKey);
	const TCommodity* pCommodity = NULL;
	if (pContract)	
		pCommodity = pContract->pCommodity;
	const TExchange* pExchange = g_pTradeData->GetExchange(&ContractKey);
	if (bOnlyName)
	{
		if (!(pContract &&pCommodity &&pExchange))	return false;
		if (pCommodity->CommodityType)
		{
			//组合
			if (ctSpreadMonth == pCommodity->CommodityType || ctSpreadCommodity == pCommodity->CommodityType || ctBUL == pCommodity->CommodityType || ctBER == pCommodity->CommodityType || ctBLT == pCommodity->CommodityType || ctBRT == pCommodity->CommodityType || ctSTD == pCommodity->CommodityType || ctSTG == pCommodity->CommodityType)
				sprintf_s(strText, "%s\\%s\\%s %s", pExchange->ExchangeName, pCommodity->CommodityName, pContract->ContractName, "价差");
			else
				sprintf_s(strText, "%s\\%s\\%s", pExchange->ExchangeName, pCommodity->CommodityName, pContract->ContractName);
		}
		strCode = strText;
		return true;
	}
	if (ContractKey.CommodityType == ctFutures || ContractKey.CommodityType == ctGoods || ContractKey.CommodityType == ctDefer)//期货,现货、现货延期
	{
		if (!bOnlyContract)
		{
			if (!bName)
				sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
			else
				sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);//?
		}
		else
		{
			if (!bName)
				sprintf_s(strText, "%s", ContractKey.ContractNo);
			else
				sprintf_s(strText, "%s%c%s", ContractKey.ContractNo, (1), pContract->ContractName);
		}
	}
	else if (ContractKey.CommodityType == ctOption)//期权
	{
		if (!bOnlyContract)
		{
			if (!bName)
				sprintf_s(strText, "%s %s %s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
			else
				sprintf_s(strText, "%s %s %s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
		}
		else
		{
			if (!bName)
				sprintf_s(strText, "%s %c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice);
			else
				sprintf_s(strText, "%s %c%s%c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, (1), pContract->ContractName);
		}
	}
	else if (ContractKey.CommodityType == ctSpreadMonth)//期货的 跨期套利
	{
		if (!bOnlyContract)
		{
			if (!bName)
				sprintf_s(strText, "%s %s %s/%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
			else
				sprintf_s(strText, "%s %s %s/%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.ContractNo2);
		}
		else
		{
			if (!bName)
				sprintf_s(strText, "%s/%s", ContractKey.ContractNo, ContractKey.ContractNo2);
			else
				sprintf_s(strText, "%s/%s%c%s", ContractKey.ContractNo, ContractKey.ContractNo2, (1), pContract->ContractName);
		}
	}
	else if (ContractKey.CommodityType == ctSpreadCommodity)//跨商品套利
	{
		if (!bOnlyContract)
		{
			if (!bName)
				sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
			else
				sprintf_s(strText, "%s %s %s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo);
		}
		else
		{
			if (!bName)
				sprintf_s(strText, "%s", ContractKey.ContractNo);
			else
				sprintf_s(strText, "%s%c%s", ContractKey.ContractNo, (1), pContract->ContractName);
		}
	}
	else if (ContractKey.CommodityType == ctBUL || ContractKey.CommodityType == ctBER || ContractKey.CommodityType == ctBLT || ContractKey.CommodityType == ctBRT || ContractKey.CommodityType == ctSTD || ContractKey.CommodityType == ctSTG)//期权的 看涨期权垂直价差，看跌期权垂直价差，跨式组合，宽跨式组合，水平看涨套利，水平看跌套利	
	{
		if (!bOnlyContract)
		{
			if (!bName)
				sprintf_s(strText, "%s %s %s %c%s/%s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
			else
				sprintf_s(strText, "%s %s %s %c%s/%s %c%s", ContractKey.ExchangeNo, ContractKey.CommodityNo, ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2, (1), pContract->ContractName);
		}
		else
		{
			if (!bName)
				sprintf_s(strText, "%s %c%s/%s %c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
			else
				sprintf_s(strText, "%s %c%s/%s %c%s%c%s", ContractKey.ContractNo, ContractKey.OptionType, ContractKey.StrikePrice, ContractKey.ContractNo2, ContractKey.OptionType2, ContractKey.StrikePrice2);
		}
	}
	strCode = strText;
	return true;
}
bool CConvert::IsLME(TExchange&Exchange)
{
	return true;
}

bool CConvert::StrToTradeSingleContract(const char* szCode, TContractSingleKey& ContractKey)
{
	vector<string> vecstr;		//保存所有满足的
	string tempstr(szCode);
	string::iterator tempit = tempstr.begin();
	memset(&ContractKey, 0, sizeof(TContractSingleKey));
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), ' ');
		if (tempit != tempstr.end())
		{
			int dis = distance(tempstr.begin(), tempit);
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
			int dis = distance(tempit, tempstr.end());
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
	string str;
	if (iCount == 2)
	{
		ContractKey.CommodityType = ctGoods;
		int iPs = vecstr[1].find('(');
		if (string::npos != iPs)	ContractKey.CommodityType = ctDefer;
	}
	else if (iCount == 3)														//期货或者期货组合
	{
		ContractKey.CommodityType = ctFutures;
		strncpy_s(ContractKey.ContractNo, sizeof(ContractKey.ContractNo), vecstr[2].c_str(), _TRUNCATE);
		ContractKey.OptionType = otNone;
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
			int dis = distance(tempstr.begin(), tempit);
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
			int dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}

void CConvert::TradeSingleContractToRawId(TContractSingleKey* ContractSingleKey, char(&ContractId)[51])
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
			int dis = distance(tempstr.begin(), tempit);
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
			int dis = distance(tempit, tempstr.end());
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

void CConvert::DefaultSendOrder(TSendOrder&SendOrder)
{
	memset(&SendOrder, 0, sizeof(TSendOrder));

	SendOrder.CommodityType = ctNone;
	SendOrder.OptionType = otNone;
	SendOrder.OptionType2 = otNone;
	//SendOrder.OrderType = otUnDefine;
	//SendOrder.OrderWay = owUndefine;
	//SendOrder.ValidType = vtNone;
	/*SendOrder.IsRiskOrder = bNo;*/
	SendOrder.Direct = dNone;
	SendOrder.Offset = oNone;
	SendOrder.SellOffset = oNone;
	/*SendOrder.Hedge = hNone;*/
	SendOrder.TriggerMode = tmNone;
	SendOrder.TriggerCondition = tcNone;
	SendOrder.StrategyType = stNone;
	SendOrder.AddOneIsValid = tsDay;
	//2016-2-22 10:32 xzc
	SendOrder.OrderType = otLimit;
	SendOrder.ValidType = vtGFD;
	SendOrder.OrderWay = owETrade;
	SendOrder.Hedge = hSpeculate;
	SendOrder.IsRiskOrder = bNo;
}
void CConvert::GetPositionVol(const char * UserNo, const TContractKey * ContractKey
	, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount)
{
	if (!g_pTradeData)
		return;
	if (UserNo == NULL || ContractKey == NULL)
	{
		g_pTradeData->GetPositionVol(NULL, NULL, LongCount, ShortCount, DayLongCount, DayShortCount);
		return;
	}

	if (!CConvert::IsCombContract(ContractKey->CommodityType))
		g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	else
	{
		TContractSingleKey SigCont1, SigCont2;
		CConvert::CombContractToSingleContract(*ContractKey, SigCont1, SigCont2);
		uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
		uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
		g_pTradeData->GetPositionVol(UserNo, &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
		g_pTradeData->GetPositionVol(UserNo, &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);
		//多头
		LongCount = LongCount1 > ShortCount2 ? ShortCount2 : LongCount1;
		DayLongCount = DayLongCount1 > DayShortCount2 ? DayShortCount2 : DayLongCount1;

		//空头
		ShortCount = ShortCount1 > LongCount2 ? LongCount2 : ShortCount1;
		DayShortCount = DayShortCount1 > DayLongCount2 ? DayLongCount2 : DayShortCount1;
	}
}
void CConvert::CombContractToSingleContract(const TContractKey &ContractKey, TContractSingleKey &SigCont1, TContractSingleKey &SigCont2)
{
	const TCommodity *pCommodity = g_pTradeData->GetCommodity(&ContractKey);
	if (!pCommodity)	return;
	bool bFirstDirect = CombContractIsFirst(ContractKey);
	if (bFirstDirect)
	{
		//1
		strcpy_s(SigCont1.Sign, ContractKey.Sign);
		strcpy_s(SigCont1.ExchangeNo, pCommodity->RelateExchangeNo);
		SigCont1.CommodityType = pCommodity->RelateCommodityType;
		strcpy_s(SigCont1.CommodityNo, pCommodity->RelateCommodityNo);
		strcpy_s(SigCont1.ContractNo, ContractKey.ContractNo);
		strcpy_s(SigCont1.StrikePrice, ContractKey.StrikePrice);
		SigCont1.OptionType = ContractKey.OptionType;
		//2
		strcpy_s(SigCont2.Sign, ContractKey.Sign);
		strcpy_s(SigCont2.ExchangeNo, pCommodity->RelateExchangeNo2);
		SigCont2.CommodityType = pCommodity->RelateCommodityType2;
		strcpy_s(SigCont2.CommodityNo, pCommodity->RelateCommodityNo2);
		strcpy_s(SigCont2.ContractNo, ContractKey.ContractNo2);
		strcpy_s(SigCont2.StrikePrice, ContractKey.StrikePrice2);
		SigCont2.OptionType = ContractKey.OptionType2;
	}
	else//以第二腿为准
	{
		//1
		strcpy_s(SigCont1.Sign, ContractKey.Sign);
		strcpy_s(SigCont1.ExchangeNo, pCommodity->RelateExchangeNo2);
		SigCont1.CommodityType = pCommodity->RelateCommodityType2;
		strcpy_s(SigCont1.CommodityNo, pCommodity->RelateCommodityNo2);
		strcpy_s(SigCont1.ContractNo, ContractKey.ContractNo2);
		strcpy_s(SigCont1.StrikePrice, ContractKey.StrikePrice2);
		SigCont1.OptionType = ContractKey.OptionType2;
		//2
		strcpy_s(SigCont2.Sign, ContractKey.Sign);
		strcpy_s(SigCont2.ExchangeNo, pCommodity->RelateExchangeNo);
		SigCont2.CommodityType = pCommodity->RelateCommodityType;
		strcpy_s(SigCont2.CommodityNo, pCommodity->RelateCommodityNo);
		strcpy_s(SigCont2.ContractNo, ContractKey.ContractNo);
		strcpy_s(SigCont2.StrikePrice, ContractKey.StrikePrice);
		SigCont2.OptionType = ContractKey.OptionType;
	}
}

bool CConvert::IsCombContract(const char CommodityType)
{
	if (ctSpreadMonth == CommodityType || ctSpreadCommodity == CommodityType || ctSpreadLocal == CommodityType ||
		ctBUL == CommodityType || ctBER == CommodityType || ctBLT == CommodityType || ctBRT == CommodityType ||
		ctSTD == CommodityType || ctSTG == CommodityType)
		return true;
	return false;
}
TOffset CConvert::GetOffset(const char* UserNo, const TContractKey * ContractKey, TDirect direct)
{
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	SOFCommodityInf sCommodityInfo;
	CConvert::GetCommodityInf(*ContractKey, sCommodityInfo);
	if (sCommodityInfo.cCoverMode == cmCoverToday)
	{
		if ((dBuy == direct&&DayShortCount > 0) || (dSell == direct&&DayLongCount > 0))
			return oCoverT;
		else if ((dBuy == direct&&ShortCount > 0) || (dSell == direct&&LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else if (sCommodityInfo.cCoverMode == cmCover)
	{
		if ((dBuy == direct && ShortCount > 0) || (dSell == direct && LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else
		return oNone;
}
bool CConvert::GetCommodityInf(const TCommodityKey &sCommodityKey, SOFCommodityInf & sCommodityInfo)
{
	if (!(NULL != g_pTradeData && NULL != g_pTradeData->GetCommodity(&sCommodityKey)))
	{
		sCommodityInfo.bCanNegative = false;
		sCommodityInfo.dMiniChangePrice = 1;
		sCommodityInfo.iPrecision = 0;
		sCommodityInfo.bIfFractionPrice = false;
		return false;
	}
	const TCommodity * pCommodityInfo = g_pTradeData->GetCommodity(&sCommodityKey);//由参数传递给的
	if (0 == pCommodityInfo->LowerTick)
	{
		sCommodityInfo.bCanNegative = false;
		sCommodityInfo.dMiniChangePrice = 1;
		sCommodityInfo.iPrecision = 0;
		return true;
	}
	strncpy_s(sCommodityInfo.Sign, sCommodityKey.Sign, _TRUNCATE);
	char CommdityType = pCommodityInfo->CommodityType;
	if (ctGoods == CommdityType || ctFutures == CommdityType || ctOption == CommdityType)
		sCommodityInfo.bCanNegative = false;
	else if (ctSpreadMonth == CommdityType || ctSpreadCommodity == CommdityType ||
		ctBUL == CommdityType || ctBER == CommdityType || ctBLT == CommdityType || ctBRT == CommdityType || ctSTD == CommdityType || ctSTG == CommdityType)
		sCommodityInfo.bCanNegative = true;

	//开平信息
	sCommodityInfo.cCoverMode = pCommodityInfo->CoverMode;
	switch (pCommodityInfo->LowerTick)
	{
	case 1:	//对应最小变动价是小数
	{
		sCommodityInfo.bIfFractionPrice = false;
		sCommodityInfo.dMiniChangePrice = pCommodityInfo->UpperTick;//最小变动价
		sCommodityInfo.iPrecision = pCommodityInfo->Precision;
		return true;
	}
	default: //对应最小变动价是分数 形如10/320 分数没有精度 精度没意义 等于多少不知道
	{
		sCommodityInfo.bIfFractionPrice = true;
		sCommodityInfo.dMiniChangePrice = pCommodityInfo->UpperTick;//分子变动单位 当整数用
		sCommodityInfo.iPrecision = pCommodityInfo->LowerTick;	//分母
		break;
	}
	}
	return true;
}
void CConvert::GetCloseNum(char* UserNo, TContractSingleKey* ContractKey, TDirect direct, uint &iCoverQty, int& iCoverNum, int& iTCoverNum)
{
	if (iCoverQty == 0)
		return;
	iCoverNum = 0;
	iTCoverNum = 0;
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	const TOrderQty* stOrderQty = g_pTradeData->GetParOrderQty(UserNo, ContractKey);
	int iAvaQty = 0;
	int iTAvaQty = 0;
    int iPositionQty = 0;

	if (stOrderQty == NULL)
	{
		if (!CConvert::IsDistinguishCoverType(ContractKey))
			iCoverNum = iCoverQty;
		else
		{
			//获取可用持仓量
			if (direct == dBuy)
			{
				iAvaQty = ShortCount;
				iTAvaQty = DayShortCount;
			}
			else if (direct == dSell)
			{
				iAvaQty = LongCount;
				iTAvaQty = DayLongCount;
			}
			if (iCoverQty <= iTAvaQty)
			{
				iCoverNum = 0;
				iTCoverNum = iCoverQty;
			}
			else if (iCoverQty > iTAvaQty && iCoverQty < iAvaQty)
			{
				iTCoverNum = iTAvaQty;
				iCoverNum = iCoverQty - iTAvaQty;
			}
			else if (iCoverQty >= iAvaQty)
			{
				iTCoverNum = iTAvaQty;
				iCoverNum = iAvaQty - iTAvaQty;
			}
		}
	}
	else if (!CConvert::IsDistinguishCoverType(ContractKey))
	{
		//获取可用持仓量
        if (direct == dBuy)
        {
            iPositionQty = ShortCount;
            iAvaQty = ShortCount - stOrderQty->BuyCoverQty;
        }
        else if (direct == dSell)    
        {
            iPositionQty = LongCount;
            iAvaQty = LongCount - stOrderQty->SellCoverQty;        }

        if (iCoverQty > iAvaQty)
        {
            iCoverQty = min(iPositionQty, iCoverQty);
            iCoverNum = iAvaQty;
        }
		else
			iCoverNum = iCoverQty;
	}
	else
	{
		//获取可用持仓量
		if (direct == dBuy)
		{
            iPositionQty = ShortCount;
			iAvaQty = ShortCount - stOrderQty->BuyCoverDayQty - stOrderQty->BuyCoverQty;
			iTAvaQty = DayShortCount - stOrderQty->BuyCoverDayQty;
		}
		else if (direct == dSell)
		{
            iPositionQty = LongCount;
			iAvaQty = LongCount - stOrderQty->SellCoverDayQty - stOrderQty->SellCoverQty;
			iTAvaQty = DayLongCount - stOrderQty->SellCoverDayQty;
		}
		if (iCoverQty <= iTAvaQty)
		{
			iCoverNum = 0;
			iTCoverNum = iCoverQty;
		}
		else if (iCoverQty > iTAvaQty && iCoverQty < iAvaQty)
		{
			iTCoverNum = iTAvaQty;
			iCoverNum = iCoverQty - iTAvaQty;
		}
		else if (iCoverQty >= iAvaQty)
		{
            iCoverQty = min(iPositionQty, iCoverQty);
			iTCoverNum = iTAvaQty;
			iCoverNum = iAvaQty - iTAvaQty;
		}
	}
}
bool CConvert::IsDistinguishCoverType(const TCommodityKey*pCommoditykey)
{
	if (strcmp(pCommoditykey->ExchangeNo, "SHFE") == 0)
		return true;
	else
		return false;
// 	const TCommodity * pCommodity = g_pTradeData->GetCommodity(pCommoditykey);
// 	if (!pCommodity)		
// 		return false;
// 	if (cmCoverToday == pCommodity->CoverMode)	
// 		return true;
// 	return false;
}
void  CConvert::TStopOrderPrice2Char(int iPriceType, char* szText, int Size)
{
	if (iPriceType == QueuePrice)
		strcpy_s(szText, Size, "排队价");
	else if (iPriceType == CounterPrice)
		strcpy_s(szText, Size, "对手价");
	else if (iPriceType == SuperPrice)
		strcpy_s(szText, Size, "超价");
	else if (iPriceType == MarketPrice)
		strcpy_s(szText, Size, "市价");
	else if (iPriceType == LastPrice)
		strcpy_s(szText, Size, "最新价");
	else
		strcpy_s(szText, Size, "");
}
bool CConvert::CombContractIsFirst(const TCommodityKey & CommodityKey)
{
	const TCommodity *pCommodity = g_pTradeData->GetCommodity(&CommodityKey);
	if (!pCommodity)	return true;
	if (dfNone == pCommodity->CmbDirectFlag){}
	else if (dfFirst == pCommodity->CmbDirectFlag)
		return true;
	else if (dfSecond == pCommodity->CmbDirectFlag)
		return false;
	return true;
}
bool CConvert::newGUID(char* buf)
{
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		sprintf_s(buf, 51, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2,
			guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		return true;
	}
	return false;
}

void CConvert::GetFilePath(char* filePath, int length, char* fileName)
{
	GetModuleFileNameA(0, filePath, length);
	char* cp_pos = strrchr(filePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = '\0';
	strcat_s(filePath, length, fileName);
}

void CConvert::GetFilePath(TCHAR* filePath, int length, TCHAR* fileName)
{
	GetModuleFileName(0, filePath, length);
	WCHAR* cp_pos = wcsrchr(filePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = '\0';
	wcscat_s(filePath, length, fileName);
}



bool CConvert::Cmp2ContractSingleKey(TContractSingleKey key1, TContractSingleKey key2)
{
	if (strcmp(key1.ExchangeNo, key2.ExchangeNo) || strcmp(key1.ContractNo, key2.ContractNo) || strcmp(key1.CommodityNo, key2.CommodityNo) || key1.CommodityType != key2.CommodityType)
	{
		return false;
	}
	else
		return true;
}
void CConvert::DefaultDealClose(TPosition *stPosition, TSendOrder &stOrder)
{
	DefaultSendOrder(stOrder);
	if (stPosition->Direct == dBuy)
		stOrder.Direct = dSell;
	else if (stPosition->Direct == dSell)
		stOrder.Direct = dBuy;
	strcpy_s(stOrder.UserNo, stPosition->UserNo);
	strcpy_s(stOrder.Sign, stPosition->Sign);
	stOrder.Offset = oCover;
	stOrder.OrderQty = stPosition->PositionQty;
	strcpy_s(stOrder.ExchangeNo, stPosition->ExchangeNo);
	strcpy_s(stOrder.CommodityNo, stPosition->CommodityNo);
	stOrder.CommodityType = stPosition->CommodityType;
	strcpy_s(stOrder.ContractNo, stPosition->ContractNo);
	strcpy_s(stOrder.StrikePrice, stPosition->StrikePrice);
	stOrder.OptionType = stPosition->OptionType;
}
bool CConvert::ParOrderToStr(const TSendOrder & stOrder, string & strCode)
{
    char strText[100] = "";
    sprintf_s(strText, "%s%s%s%c%c", stOrder.ExchangeNo, stOrder.CommodityNo, stOrder.ContractNo, stOrder.Direct, stOrder.Offset);
    strCode = strText;
    return true;
}

bool CConvert::SplitNotice(const std::string &strContent, char cSplit, std::vector<NoticeContent> &vecstr)
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
