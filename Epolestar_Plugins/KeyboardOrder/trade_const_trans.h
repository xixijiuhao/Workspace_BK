#pragma once
#include <string>
#include "TradeApi_Type.h"
#include "resource.h"
#include <tchar.h>

using namespace std;

extern wstring LoadResString(int strID);

//Ĭ�ϼ۸�
#define	dpLastPrice			0		//���¼�  
#define	dpCompetitorPrice	1		//���̼�
#define	dpNoDefaultPrice	2		//��Ĭ��
#define	dpQueuePrice		3		//�ŶӼ�


//һ�����
#define qcdpLastPrice		0		//���¼� 
#define	qcdpMarket			1		//�м�
#define	qcdpFanXiang		2		//����ͣ��
#define	qcdpCompetitorPrice	3		//����
#define	qcdpQueuePrice		4		//�Ŷ�


//������ʽ(tm)
#define tmLastPrice			0	//���¼�
#define tmBuyPrice			1	//���
#define tmSellPrice			2	//����


//��������(tc) 
#define tcStopLossMode		0	//ֹ��ģʽ
#define tcStopProfit		1	//ֹӮģʽ

namespace CTradeConst
{
	//Ĭ�ϼ۸�--------------------------------------------------------------
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

	//Ͷ����ֵ-------------------------------------------------------------------
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
	
	//��  ��-------------------------------------------------------------------
	inline void DirectToStr(char AChar, wchar_t* str, unsigned int nLen)	//��������
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

	//��  ƽ-------------------------------------------------------------------
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

	//��  ��-------------------------------------------------------------------
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

	//����ģʽ-------------------------------------------------------------------
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

	//��������-------------------------------------------------------------------
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

	//��Ч����-------------------------------------------------------------------
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