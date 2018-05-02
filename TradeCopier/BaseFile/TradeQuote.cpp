#include "PreInclude.h"

//品种类型
typedef char               TapCommodityTypeType;
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

TOffset CConvert::GetOffset(const char* UserNo, const TContractSingleKey * ContractKey, TDirect direct)
{
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	const TCommodity *pCommodity = g_pTradeData->GetCommodity(ContractKey);
	if (!pCommodity)
		return oNone;
	if (pCommodity->CoverMode == cmCoverToday)
	{
		if ((dBuy == direct&&DayShortCount > 0) || (dSell == direct&&DayLongCount > 0))
			return oCoverT;
		else if ((dBuy == direct&&ShortCount > 0) || (dSell == direct&&LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else if (pCommodity->CoverMode == cmCover)
	{
		if ((dBuy == direct && ShortCount > 0) || (dSell == direct && LongCount > 0))
			return oCover;
		else
			return oOpen;
	}
	else
		return oNone;
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
	SendOrder.Hedge2 = hNone;
	//2016-2-22 10:32 xzc
	SendOrder.OrderType = otLimit;
	SendOrder.ValidType = vtGFD;
	SendOrder.OrderWay = owETrade;
	SendOrder.Hedge = hSpeculate;
	SendOrder.IsRiskOrder = bNo;
}

char		szorderstatechs[][50] = {
	"无", "已发送", "已受理", "待触发", "埋单",
	"已生效", "已排队", "部分成交", "完全成交", "待撤",
	"待改", "已撤单", "已撤余单", "指令失败", "待审核",
	"已挂起", "已申请", "无效单", "部分触发", "完全触发",
	"余单失败"
};

char* _GetOrderState(int i)
{
	return szorderstatechs[i];
}

void CConvert::TOrderState2Char(TOrderState tstate, TStrategyType ttype, char* sztext, int nsize/* = MAX_PATH*/)
{
	switch (tstate)
	{
	case  osNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;//					'N' //无
	case	osSended: strcpy_s(sztext, nsize, _GetOrderState(1)); break;//				'0'	//已发送
	case	osAccept: strcpy_s(sztext, nsize, _GetOrderState(2)); break;//				'1'	//已受理
	case osTriggering: //			'2'	//待触发
		if (ttype != stPreOrder)
			strcpy_s(sztext, nsize, _GetOrderState(3));
		else
			strcpy_s(sztext, nsize, _GetOrderState(4));
		break;
	case	osActive: strcpy_s(sztext, nsize, _GetOrderState(5)); break;//				'3'	//已生效
	case	osQueued: strcpy_s(sztext, nsize, _GetOrderState(6)); break;//			'4'	//已排队
	case	osPartFilled: strcpy_s(sztext, nsize, _GetOrderState(7)); break;//			'5'	//部分成交
	case	osFilled: strcpy_s(sztext, nsize, _GetOrderState(8)); break;//			'6'	//完全成交
	case	osCanceling: strcpy_s(sztext, nsize, _GetOrderState(9)); break;//				'7'	//待撤
	case	osModifying: strcpy_s(sztext, nsize, _GetOrderState(10)); break;//				'8'	//待改
	case	osCanceled: strcpy_s(sztext, nsize, _GetOrderState(11)); break;//				'9'	//已撤单
	case	osPartCanceled: strcpy_s(sztext, nsize, _GetOrderState(12)); break;//				'A'	//已撤余单
	case	osFail: strcpy_s(sztext, nsize, _GetOrderState(13)); break;//				'B'	//指令失败
	case osChecking: strcpy_s(sztext, nsize, _GetOrderState(14)); break;//				'C' //待审核
	case	osSuspended: strcpy_s(sztext, nsize, _GetOrderState(15)); break;//				'D'	//已挂起
	case	osApply: strcpy_s(sztext, nsize, _GetOrderState(16)); break;//					'E'	//已申请
	case osInvalid: strcpy_s(sztext, nsize, _GetOrderState(17)); break;//				'F' //无效单
	case osPartTriggered:strcpy_s(sztext, nsize, _GetOrderState(18)); break;//'G' //部分触发
	case osFillTriggered:strcpy_s(sztext, nsize, _GetOrderState(19)); break;//			'H' //完全触发
	case osPartFailed:strcpy_s(sztext, nsize, _GetOrderState(20)); break;//'I' //余单失败
	}
}

char szdirectchs[][50] = {
	"买入", "卖出", "双边"
};
char* _GetDirect(int i)
{
	return szdirectchs[i];
}
void CConvert::TDirect2Char(TDirect tdirect, char* sztext, int nsize)
{
	switch (tdirect)
	{
	case dNone: strcpy_s(sztext, nsize, ""); break;// sztext = ""; break;//					'N'
	case	dBuy: strcpy_s(sztext, nsize, _GetDirect(0)); break; //sztext = _GetDirect(0); break;//				'B'	//买入
	case	dSell: strcpy_s(sztext, nsize, _GetDirect(1)); break; //sztext = _GetDirect(1); break;//				'S'	//卖出
	case	dBoth: strcpy_s(sztext, nsize, _GetDirect(2)); break; //sztext = _GetDirect(2); break;//				'A'	//双边
	}
}

char		szoffsetchs[][50] = {
	"开仓", "平仓", "平今",
	"开平", "平开"
};
char* _GetOffset(int i)
{
	return szoffsetchs[i];
}
void CConvert::TOffset2Char(TOffset toffset, char* sztext, int nsize)
{
	switch (toffset)
	{
	case	oNone: strcpy_s(sztext, nsize, _GetOrderState(0)); break;// sztext = _GetOrderState(0); break;//				'N'	//无
	case	oOpen: strcpy_s(sztext, nsize, _GetOffset(0)); break;// sztext = _GetOffset(0); break;//				'O'	//开仓
	case	oCover: strcpy_s(sztext, nsize, _GetOffset(1)); break;// sztext = _GetOffset(1); break;//				'C'	//平仓
	case	oCoverT: strcpy_s(sztext, nsize, _GetOffset(2)); break;//sztext = _GetOffset(2); break;//				'T'	//平今
	case oOpenCover: strcpy_s(sztext, nsize, _GetOffset(3)); break;//sztext = _GetOffset(3); break;//				'1' //开平，应价时有效
	case oCoverOpen: strcpy_s(sztext, nsize, _GetOffset(4)); break;//sztext = _GetOffset(4); break;//				'2'	//平开，应价时有效
	}
}

wstring CConvert::ANSIToUnicode(const string& str)//简体Ansic转Unicode
{
	UINT nPage = 936;
	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(nPage, 0, str.c_str(), -1, nullptr, 0);

	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));

	::MultiByteToWideChar(nPage, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
	wstring  rt = (wchar_t*)pUnicode;
	delete[]  pUnicode;

	return  rt;
}

COLORREF CConvert::SetOrderStateColor(TOrderState state)
{
	switch (state)
	{
	case osFilled:return RGB(0, 128, 0);
	case osQueued:return RGB(0, 0, 255);
	case osPartFilled:return RGB(0, 128, 128);
	case osPartCanceled:
	case osCanceled:return RGB(128, 128, 0); 
	case osSended:return RGB(128, 0, 128);
	case osFail:return RGB(255, 102, 0);
	case osAccept:return RGB(128, 0, 128); 
	default:return RGB(77, 96, 130);
	}
}
