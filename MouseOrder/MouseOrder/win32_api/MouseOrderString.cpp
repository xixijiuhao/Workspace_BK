#include "PreInclude.h"

//字符串转定单类型
char CMouseString::WStrToOrderType(const wchar_t *Str)
{
	if (wcscmp(Str, LoadResWchar_t(IDS_LIMIT_PRICE))==0)
		return otLimit;
	else if (wcscmp(Str, LoadResWchar_t(IDS_MARKET_PRICE)) == 0)
		return otMarket;
	else if (wcscmp(Str, LoadResWchar_t(IDS_LIMIT_STOP_PRICE)) == 0)
		return otLimitStop;
	else if (wcscmp(Str, LoadResWchar_t(IDS_STOP_PRICE)) == 0)
		return otMarketStop;
	else if (wcscmp(Str, LoadResWchar_t(IDS_ASK_PRICE)) == 0)
		return otEnquiry;
	else if (wcscmp(Str, LoadResWchar_t(IDS_REPLY_PRICE)) == 0)
		return otOffer;
	else if (wcscmp(Str, LoadResWchar_t(IDS_EXECUTE)) == 0)
		return otExecute;
	else if (wcscmp(Str, LoadResWchar_t(IDS_ABANDON)) == 0)
		return otAbandon;
	else if (wcscmp(Str, LoadResWchar_t(IDS_ICEBERG)) == 0)
		return otIceberg;
	else if (wcscmp(Str, LoadResWchar_t(IDS_SHADOW)) == 0)
		return otGhost;
	else if (wcscmp(Str, LoadResWchar_t(IDS_SWAP)) == 0)
		return otSwap;
	else if (wcscmp(Str, LoadResWchar_t(IDS_ARBAPP)) == 0)
		return otSpreadApply;
	else
		return otLimit;
}
//字符串转有效类型
char CMouseString::WStrToValidType(const wchar_t *Str)
{
	if (wcscmp(Str, LoadResWchar_t(IDS_IMMEDIATE_ALL)) == 0)
		return vtFOK;
	else if (wcscmp(Str, LoadResWchar_t(IDS_IMMEDIATE_PART)) == 0)
		return vtIOC;
	else if (wcscmp(Str, LoadResWchar_t(IDS_EFFECTIVE_THEDAY)) == 0)
		return vtGFD;
	else if (wcscmp(Str, LoadResWchar_t(IDS_EFFECTIVE_LONGTERM)) == 0)
		return vtGTC;
	else
		return vtGTD;	
}

wstring CMouseString::ValidTypeToWStr(char ctype)
{
	switch (ctype)
	{
	case vtFOK:
		return LoadResWchar_t(IDS_IMMEDIATE_ALL);
	case vtIOC:
		return LoadResWchar_t(IDS_IMMEDIATE_PART);	
	case vtGTC:
		return LoadResWchar_t(IDS_EFFECTIVE_LONGTERM);
	case vtGTD:
		return LoadResWchar_t(IDS_EFFECTIVE_DEADLINE);
	case vtGFD:
	default:
		return LoadResWchar_t(IDS_EFFECTIVE_THEDAY);
	}
}
