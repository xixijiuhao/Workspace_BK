#pragma once
#include <string>
#include "TradeApi_Type.h"
#include "resource.h"
#include <tchar.h>

using namespace std;

extern wstring LoadResString(int strID);

//默认价格
#define	dpLastPrice			0		//最新价  
#define	dpCompetitorPrice	1		//对盘价
#define	dpNoDefaultPrice	2		//不默认
#define	dpQueuePrice		3		//排队价


//一键清仓
#define qcdpLastPrice		0		//最新价 
#define	qcdpMarket			1		//市价
#define	qcdpFanXiang		2		//反向停板
#define	qcdpCompetitorPrice	3		//对盘
#define	qcdpQueuePrice		4		//排队


//触发方式(tm)
#define tmLastPrice			0	//最新价
#define tmBuyPrice			1	//买价
#define tmSellPrice			2	//卖价


//触发条件(tc) 
#define tcStopLossMode		0	//止损模式
#define tcStopProfit		1	//止赢模式

namespace CTradeConst
{
	//默认价格--------------------------------------------------------------
	inline int StrToDefaultPrice(const wchar_t* str)
	{
		if (wcscmp(str, LoadResString(IDS_STRING_DPLastPrice).c_str()) == 0)
			return dpLastPrice;
		else if (wcscmp(str, LoadResString(IDS_STRING_DPOppoPrice).c_str()) == 0)
			return dpCompetitorPrice;
		else if (wcscmp(str, LoadResString(IDS_STRING_DPNoPrice).c_str()) == 0)
			return dpNoDefaultPrice;
		else if (wcscmp(str, LoadResString(IDS_STRING_DPQuenePrice).c_str()) == 0)
			return dpQueuePrice;
		return 0;
	}
	inline void DefaultPriceToStr(int AChar, wchar_t* str, unsigned int nLen)
	{
		switch (AChar)
		{
		case dpLastPrice:			wcscpy_s(str,nLen,LoadResString(IDS_STRING_DPLastPrice).c_str());
			return;
		case dpCompetitorPrice:		wcscpy_s(str, nLen, LoadResString(IDS_STRING_DPOppoPrice).c_str());
			return;
		case dpNoDefaultPrice:		wcscpy_s(str, nLen, LoadResString(IDS_STRING_DPNoPrice).c_str());
			return;
		case dpQueuePrice:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_DPQuenePrice).c_str());
			return;
		default:					memset(str, 0, nLen);
			return;
		}
		
	}

	//投机保值-------------------------------------------------------------------
	inline void HedgeToStr(char AChar, wchar_t* str, unsigned int nLen)
	{
		switch (AChar)
		{
		case hSpeculate:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_Speculate).c_str());
			return;
		case hHedge:				wcscpy_s(str, nLen, LoadResString(IDS_STRING_Hedge).c_str());
			return;
		case hNone:					wcscpy_s(str, nLen, L"");
			return;
		default:					memset(str, 0, nLen);
			return;
		}
	}
	
	inline void TJTriggerModeToStr(unsigned int AChar, wchar_t* str, unsigned int nLen)
	{
		switch (AChar)
		{
		case tmLastPrice:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_TJTriggerModeLast).c_str());
			return;
		case tmBuyPrice:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_TJTriggerModeBuy).c_str());
			return;
		case tmSellPrice:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_TJTriggerModeSell).c_str());
			return;
		default:					memset(str, 0, nLen);
			return;
		}
	}
	
	//方  向-------------------------------------------------------------------
	inline void DirectToStr(char AChar, wchar_t* str, unsigned int nLen)	//买卖方向
	{
		switch (AChar)
		{
		case dBuy:			wcscpy_s(str, nLen, LoadResString(IDS_Direct_Buy).c_str());
			return;
		case dSell:			wcscpy_s(str, nLen, LoadResString(IDS_Direct_Sell).c_str());
			return;
		default:			memset(str, 0, nLen);
			return;
		}
	}
	inline char StrToDirect(const wchar_t *direct)
	{
		if (LoadResString(IDS_Direct_Buy) == direct)
			return dBuy;
		else if (LoadResString(IDS_Direct_Sell) == direct)
			return dSell;
		return dNone;
	}

	//开  平-------------------------------------------------------------------
	inline void OffsetToStr(char AChar, wchar_t* str, unsigned int nLen)
	{
		switch (AChar)
		{
		case oOpen:				wcscpy_s(str, nLen, LoadResString(IDS_Offset_Open).c_str());
			return;
		case oCover:			wcscpy_s(str, nLen, LoadResString(IDS_Offset_Cover).c_str());
			return;
		case oCoverT:			wcscpy_s(str, nLen, LoadResString(IDS_Offset_CoverT).c_str());
			return;
		case oNone:				wcscpy_s(str, nLen, L"");
			return;
		default:				memset(str, 0, nLen);
			return;
		}
	}

	//条  件-------------------------------------------------------------------
	inline void TriggerConditionToStr(char AChar, wchar_t* str, unsigned int nLen)
	{
		if (AChar == tcGreaterEqual)			wcscpy_s(str, nLen, L">=");
		else if (AChar == tcLessEqual)			wcscpy_s(str, nLen, L"<=");
		else
			memset(str, 0, nLen);
	}
	inline char StrToTriggerCondition(const wchar_t* str)
	{
		if (wcscmp(str,L">=")==0)			return tcGreaterEqual;
		else if (wcscmp(str,L"<=")==0)		return tcLessEqual;
		else								return tcNone;
	}

	//触发模式-------------------------------------------------------------------
	inline void TriggerModeToStr(char AChar, wchar_t* str,unsigned int nLen)
	{
		if (AChar == tmLatest)			wcscpy_s(str, nLen,LoadResString(IDS_STR_TirgMode_Last).c_str());
		else if (AChar == tmBid)		wcscpy_s(str, nLen, LoadResString(IDS_STR_TirgMode_Buy).c_str());
		else if (AChar == tmAsk)		wcscpy_s(str, nLen, LoadResString(IDS_STR_TirgMode_Sell).c_str());
		else
			memset(str, 0, nLen);
	}
	inline char StrToTriggerMode(const wchar_t* str)
	{
		if (str == LoadResString(IDS_STR_TirgMode_Last))			return tmLatest;
		else if (str == LoadResString(IDS_STR_TirgMode_Buy))		return tmBid;
		else if (str == LoadResString(IDS_STR_TirgMode_Sell))		return tmAsk;
		else	return tmNone;
	}

	//定单类型-------------------------------------------------------------------
	inline char StrToOrderType(const wchar_t * Str)		
	{
		if (wcscmp(Str, LoadResString(IDS_STRING_OrderTypeLimitKey).c_str()) == 0)
			return otLimit;
		else if (wcscmp(Str, LoadResString(IDS_STRING_OrderTypeMarketKey).c_str()) == 0)
			return otMarket;
		else if (wcscmp(Str, LoadResString(IDS_STRING_OrderTypeLStopKey).c_str()) == 0)
			return otLimitStop;
		else if (wcscmp(Str, LoadResString(IDS_STRING_OrderTypeMStopKey).c_str()) == 0)
			return otMarketStop;
		else if (wcscmp(Str, LoadResString(IDS_STRING_OrderTypeIceKey).c_str()) == 0)
			return otIceberg;
		else if (wcscmp(Str, LoadResString(IDS_STRING_OrderTypeGhostKey).c_str()) == 0)
			return otGhost;
		else
			return 0;
	}
	inline void OrderTypeToStr(char AChar, wchar_t* str, unsigned int nLen)
	{
		switch (AChar)
		{
		case otLimit:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_OrderTypeLimitKey).c_str());
			return;
		case otMarket:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_OrderTypeMarketKey).c_str());
			return;
		case otLimitStop:		wcscpy_s(str, nLen, LoadResString(IDS_STRING_OrderTypeLStopKey).c_str());
			return;
		case otMarketStop:		wcscpy_s(str, nLen, LoadResString(IDS_STRING_OrderTypeMStopKey).c_str());
			return;
		case otIceberg:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_OrderTypeIceKey).c_str());
			return;
		case otGhost:			wcscpy_s(str, nLen, LoadResString(IDS_STRING_OrderTypeGhostKey).c_str());
			return;
		default:				memset(str, 0, nLen);
			return;
		}
	}

	//有效类型-------------------------------------------------------------------
	inline char StrToValidType(const wchar_t * Str)
	{
		if (wcscmp(Str, LoadResString(IDS_STR_ValidType_FOK).c_str()) == 0)
			return vtFOK;
		else if (wcscmp(Str, LoadResString(IDS_STR_ValidType_IOC).c_str()) == 0)
			return vtIOC;
		else if (wcscmp(Str, LoadResString(IDS_STR_ValidType_GFD).c_str()) == 0)
			return vtGFD;
		else if (wcscmp(Str, LoadResString(IDS_STR_ValidType_GTC).c_str()) == 0)
			return vtGTC;
		else if (wcscmp(Str, LoadResString(IDS_STR_ValidType_GTD).c_str()) == 0)
			return vtGTD;
		else if (wcscmp(Str, LoadResString(IDS_STR_ValidType_GFS).c_str()) == 0)
			return vtGFS;
		else
			return 0;
	}
	inline void ValidTypeToStr(char AChar, wchar_t* str, unsigned int nLen)
	{
		switch (AChar)
		{
		case vtFOK:			wcscpy_s(str, nLen, LoadResString(IDS_STR_ValidType_FOK).c_str());
			return;
		case vtIOC:			wcscpy_s(str, nLen, LoadResString(IDS_STR_ValidType_IOC).c_str());
			return;
		case vtGFD:			wcscpy_s(str, nLen, LoadResString(IDS_STR_ValidType_GFD).c_str());
			return;
		case vtGTC:			wcscpy_s(str, nLen, LoadResString(IDS_STR_ValidType_GTC).c_str());
			return;
		case vtGTD:			wcscpy_s(str, nLen, LoadResString(IDS_STR_ValidType_GTD).c_str());
			return;
		case vtGFS:			wcscpy_s(str, nLen, LoadResString(IDS_STR_ValidType_GFS).c_str());
			return;
		default:			memset(str, 0,	nLen);
			return;
		}
	}

}