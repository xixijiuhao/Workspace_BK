#pragma once

#include "resource.h"

#include <string>
#include "windef.h"
#include "wingdi.h"

#include "_base.h"
#include "TradeApi_Type.h"

#include "load_rc.h"

using namespace std;

class TradeConstS
{
public:
	static COLORREF GetBkColor(char OrderType, char Direct, char Offset)
	{
		switch (OrderType)
		{
		case otMarketStop	:
		case otLimitStop	:
			{
				if (Offset==oOpen)
					return RGB(255,255,204);
				else if (Offset==oCover || Offset==oCoverT)
					return RGB(184,204,228);
				else if (Direct==dBuy)
					return RGB(255,255,204);
				else
					return RGB(184,204,228);
			}
			break;
		case otLimit		:
		case otMarket		:
		case otOffer		:
			{
				if (Offset==oOpen)
					return RGB(255,199,206);
				else if (Offset==oCover || Offset==oCoverT)
					return RGB(198,239,206);
				else if (Offset==oOpenCover || Offset==oCoverOpen)
					return 0xd0f8ff;
				else if (Direct==dBuy)
					return RGB(255,199,206);
				else
					return RGB(198,239,206);
			}
			break;		
		case otEnquiry		:
			{
					return 0xfff8f8;
			}
			break;
		default:
			return RGB(255,255,255);
			break;
		}
	}
//////////////////////////////////////////////////////////////////////////
public:
	//_stricmp不区分大小写
	static char StrToEnquirer(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_eExchange).c_str(), Str)==0)
			return eExchange;
		else if (_stricmp(LoadRC::load_str(IDS_eUser).c_str(), Str)==0)
			return eUser;
		else
			return 0;
	}
	static char StrToPswType(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_ptTradeLogin).c_str(), Str)==0)
			return ptTrade;
		else if (_stricmp(LoadRC::load_str(IDS_ptQuoteLogin).c_str(), Str)==0)
			return ptQuote;
		else if (_stricmp(LoadRC::load_str(IDS_ptTradeCheck).c_str(), Str)==0)
			return ptAuth;
		else
			return 0;
	}
	static char StrToUserIdentity(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_uiUser).c_str(), Str)==0)
			return uiUser;
		else if (_stricmp(LoadRC::load_str(IDS_uiProxy).c_str(), Str)==0)
			return uiProxy;
		else if (_stricmp(LoadRC::load_str(IDS_uiBroker).c_str(), Str)==0)
			return uiBroker;
		else if (_stricmp(LoadRC::load_str(IDS_uiTrade).c_str(), Str)==0)
			return uiTrader;
		else if (_stricmp(LoadRC::load_str(IDS_uiQUser).c_str(), Str)==0)
			return uiQUser;
		else
			return 0;
	}
	static char StrToDirect(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_dBuy).c_str(), Str)==0 || 
			_stricmp(LoadRC::load_str(IDS_dBuy_Ex).c_str(), Str)==0)
			return dBuy;
		else if (_stricmp(LoadRC::load_str(IDS_dSell).c_str(), Str)==0 || 
			_stricmp(LoadRC::load_str(IDS_dSell_Ex).c_str(), Str)==0)
			return dSell;
		else if (_stricmp(LoadRC::load_str(IDS_dBoth).c_str(), Str)==0 || 
			_stricmp(LoadRC::load_str(IDS_dBoth_Ex).c_str(), Str)==0)
			return dBoth;
		else
			return 0;
	}
	static char StrToOrderState(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_osSended).c_str(), Str)==0)
			return osSended;
		else if (_stricmp(LoadRC::load_str(IDS_osFail).c_str(), Str)==0)
			return osFail;
		else if (_stricmp(LoadRC::load_str(IDS_osAccept).c_str(), Str)==0)
			return osAccept;
		else if (_stricmp(LoadRC::load_str(IDS_osSuspended).c_str(), Str)==0)
			return osSuspended;
		else if (_stricmp(LoadRC::load_str(IDS_osQueued).c_str(), Str)==0)
			return osQueued;
		else if (_stricmp(LoadRC::load_str(IDS_osCanceling).c_str(), Str)==0)
			return osCanceling;
		else if (_stricmp(LoadRC::load_str(IDS_osModifying).c_str(), Str)==0)
			return osModifying;
		else if (_stricmp(LoadRC::load_str(IDS_osPartCanceled).c_str(), Str)==0)
			return osPartCanceled;
		else if (_stricmp(LoadRC::load_str(IDS_osCanceled).c_str(), Str)==0)
			return osCanceled;
		else if (_stricmp(LoadRC::load_str(IDS_osPartFilled).c_str(), Str)==0)
			return osPartFilled;
		else if (_stricmp(LoadRC::load_str(IDS_osFilled).c_str(), Str)==0)
			return osFilled;
		else if (_stricmp(LoadRC::load_str(IDS_osActive).c_str(), Str)==0)
			return osActive;
		else if (_stricmp(LoadRC::load_str(IDS_osTriggering).c_str(), Str)==0)
			return osTriggering;
		else  if (_stricmp(LoadRC::load_str(IDS_osFillTriggered).c_str(), Str)==0)
			return osFillTriggered;
		else if (_stricmp(LoadRC::load_str(IDS_osApply).c_str(), Str)==0)
			return osApply;
		else if (_stricmp(LoadRC::load_str(IDS_osChecking).c_str(), Str)==0)
			return osChecking;
		else if (_stricmp(LoadRC::load_str(IDS_osInvalid).c_str(), Str)==0)
			return osInvalid;
		else
			return 0;
	}
	static char StrToPtlBool(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_bYes).c_str(), Str)==0)
			return bYes;
		else if (_stricmp(LoadRC::load_str(IDS_bNo).c_str(), Str)==0)
			return bNo;
		else
			return 0;
	}	
	static char StrToOrderWay(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_owETrade).c_str(), Str)==0)				
			return owETrade;
		else if (_stricmp(LoadRC::load_str(IDS_owProxyETrade).c_str(), Str)==0)
			return owProxyETrade;
		else if (_stricmp(LoadRC::load_str(IDS_owJTrade).c_str(), Str)==0)
			return owJTrade;
		else if (_stricmp(LoadRC::load_str(IDS_owManual).c_str(), Str)==0)
			return owManual;
		else if (_stricmp(LoadRC::load_str(IDS_owCarry).c_str(), Str)==0)
			return owCarry;
		else if (_stricmp(LoadRC::load_str(IDS_owProgram).c_str(), Str)==0)
			return owProgram;
		else if (_stricmp(LoadRC::load_str(IDS_owDelivery).c_str(), Str)==0)
			return owDelivery;
		else if (_stricmp(LoadRC::load_str(IDS_owRTS).c_str(), Str)==0)
			return owRTS;
		else if (_stricmp(LoadRC::load_str(IDS_owSpreadLocal).c_str(), Str)==0)				
			return owSpreadSyn;
		else if (_stricmp(LoadRC::load_str(IDS_owHorz).c_str(), Str)==0)
			return owHorz;
		else if (_stricmp(LoadRC::load_str(IDS_owVert).c_str(), Str)==0)
			return owVert;
		else if (_stricmp(LoadRC::load_str(IDS_owClickPrice).c_str(), Str)==0)
			return owClickPrice;
		else if (_stricmp(LoadRC::load_str(IDS_owCard).c_str(), Str)==0)
			return owCard;
		else if (_stricmp(LoadRC::load_str(IDS_owBrtch).c_str(), Str)==0)
			return owBrtch;
		else if (_stricmp(LoadRC::load_str(IDS_owFast).c_str(), Str)==0)
			return owFast;
		else if (_stricmp(LoadRC::load_str(IDS_owPingPong).c_str(), Str)==0)
			return owPingPong;
		else if (_stricmp(LoadRC::load_str(IDS_owStopLoss).c_str(), Str)==0)
			return owStopLoss;
		else if (_stricmp(LoadRC::load_str(IDS_owStopProfit).c_str(), Str)==0)
			return owStopProfit;
		else if (_stricmp(LoadRC::load_str(IDS_owFollow).c_str(), Str)==0)
			return owFollow;
		else if (_stricmp(LoadRC::load_str(IDS_owCatch).c_str(), Str)==0)
			return owCatch;
		else 
			return 0;
	}
	//定单类型
	static char StrToOrderType(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_otLimit).c_str(), Str)==0)
			return otLimit;
		else if (_stricmp(LoadRC::load_str(IDS_otMarket).c_str(), Str)==0)
			return otMarket;
		else if (_stricmp(LoadRC::load_str(IDS_otLimitStop).c_str(), Str)==0)
			return otLimitStop;
		else if (_stricmp(LoadRC::load_str(IDS_otMarketStop).c_str(), Str)==0)
			return otMarketStop;
		else if (_stricmp(LoadRC::load_str(IDS_otEnquiry).c_str(), Str)==0)
			return otEnquiry;
		else if (_stricmp(LoadRC::load_str(IDS_otOffer).c_str(), Str)==0)
			return otOffer;
		else if (_stricmp(LoadRC::load_str(IDS_otExecute).c_str(), Str)==0)
			return otExecute;
		else if (_stricmp(LoadRC::load_str(IDS_otAbandon).c_str(), Str)==0)
			return otAbandon;
		else if (_stricmp(LoadRC::load_str(IDS_otIceberg).c_str(), Str)==0)
			return otIceberg;
		else if (_stricmp(LoadRC::load_str(IDS_otGhost).c_str(), Str)==0)
			return otGhost;
		else if (_stricmp(LoadRC::load_str(IDS_otSwap).c_str(), Str)==0)
			return otSwap;
		else 
			return 0;
	}
	static char StrToCommodityType(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_ctGoods).c_str(), Str)==0)
			return ctGoods;
		else if (_stricmp(LoadRC::load_str(IDS_ctDefer).c_str(), Str)==0)
			return ctDefer;
		else if (_stricmp(LoadRC::load_str(IDS_ctFutures).c_str(), Str)==0)
			return ctFutures;
		else if (_stricmp(LoadRC::load_str(IDS_ctOption).c_str(), Str)==0)
			return ctOption;
		else if (_stricmp(LoadRC::load_str(IDS_ctSpreadMonth).c_str(), Str)==0)
			return ctSpreadMonth;
		else if (_stricmp(LoadRC::load_str(IDS_ctSpreadCommodity).c_str(), Str)==0)
			return ctSpreadCommodity;
		else if (_stricmp(LoadRC::load_str(IDS_ctUnDefine).c_str(), Str)==0)
			return ctUnDefine;
		else if (_stricmp(LoadRC::load_str(IDS_ctBUL).c_str(), Str)==0)
			return ctBUL;
		else if (_stricmp(LoadRC::load_str(IDS_ctBER).c_str(), Str)==0)
			return ctBER;
		else if (_stricmp(LoadRC::load_str(IDS_ctBLT).c_str(), Str)==0)
			return ctBLT;
		else if (_stricmp(LoadRC::load_str(IDS_ctBRT).c_str(), Str)==0)
			return ctBRT;
		else if (_stricmp(LoadRC::load_str(IDS_ctSTD).c_str(), Str)==0)
			return ctSTD;
		else if (_stricmp(LoadRC::load_str(IDS_ctSTG).c_str(), Str)==0)
			return ctSTG;
		else
			return 0;
	}
	static char StrToHedge(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_hSpeculate).c_str(), Str)==0)
			return hSpeculate;
		else if (_stricmp(LoadRC::load_str(IDS_hHedge).c_str(), Str)==0)
			return hHedge;
		else if (_stricmp(LoadRC::load_str(IDS_hSpread).c_str(), Str)==0)
			return hSpread;
		else if (_stricmp(LoadRC::load_str(IDS_hMarket).c_str(), Str)==0)
			return hMarket;
		else 
			return hNone;
	}
	static char StrToOffset(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_oOpen).c_str(), Str)==0)
			return oOpen;
		else if (_stricmp(LoadRC::load_str(IDS_oCover).c_str(), Str)==0)
			return oCover;
		else if (_stricmp(LoadRC::load_str(IDS_oCoverT).c_str(), Str)==0)
			return oCoverT;
		else if (_stricmp(LoadRC::load_str(IDS_oOpenCover).c_str(), Str)==0)
			return oOpenCover;
		else if (_stricmp(LoadRC::load_str(IDS_oCoverOpen).c_str(), Str)==0)
			return oCoverOpen;
		else 
			return oNone;
	}
	static char StrToValidType(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_vtFOK).c_str(), Str)==0)
			return vtFOK;
		else if (_stricmp(LoadRC::load_str(IDS_vtIOC).c_str(), Str)==0)
			return vtIOC;
		else if (_stricmp(LoadRC::load_str(IDS_vtGFD).c_str(), Str)==0)
			return vtGFD;
		else if (_stricmp(LoadRC::load_str(IDS_vtGTC).c_str(), Str)==0)
			return vtGTC;
		else if (_stricmp(LoadRC::load_str(IDS_vtGTD).c_str(), Str)==0)
			return vtGTD;
		else 
			return 0;
	}
	static char StrToValidTypeAbb(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_vtFOK_Abb).c_str(), Str)==0)
			return vtFOK;
		else if (_stricmp(LoadRC::load_str(IDS_vtIOC_Abb).c_str(), Str)==0)
			return vtIOC;
		else if (_stricmp(LoadRC::load_str(IDS_vtGFD_Abb).c_str(), Str)==0)
			return vtGFD;
		else if (_stricmp(LoadRC::load_str(IDS_vtGTC_Abb).c_str(), Str)==0)
			return vtGTC;
		else if (_stricmp(LoadRC::load_str(IDS_vtGTD_Abb).c_str(), Str)==0)
			return vtGTD;
		else 
			return 0;
	}
	static char StrToValidTypeEx(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_vtFOK_Ex).c_str(), Str)==0)
			return vtFOK;
		else if (_stricmp(LoadRC::load_str(IDS_vtIOC_Ex).c_str(), Str)==0)
			return vtIOC;
		else if (_stricmp(LoadRC::load_str(IDS_vtGFD_Ex).c_str(), Str)==0)
			return vtGFD;
		else if (_stricmp(LoadRC::load_str(IDS_vtGTC_Ex).c_str(), Str)==0)
			return vtGTC;
		else if (_stricmp(LoadRC::load_str(IDS_vtGTD_Ex).c_str(), Str)==0)
			return vtGTD;
		else 
			return 0;
	}
	static char BoolToPtlBool(bool Value)
	{
		if (Value)
			return bYes;
		else
			return bNo;
	}
	static char StrToCashType(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_ctOut).c_str(), Str)==0)
			return ctOut;
		else if (_stricmp(LoadRC::load_str(IDS_ctIn).c_str(), Str)==0)
			return ctIn;
		else 
			return 0;
	}
	static char StrToCashMode(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_cmAmount).c_str(), Str)==0)
			return cmAmount;
		else if (_stricmp(LoadRC::load_str(IDS_cmCheck).c_str(), Str)==0)
			return cmCheck;
		else if (_stricmp(LoadRC::load_str(IDS_cmCash).c_str(), Str)==0)
			return cmCash;
		else if (_stricmp(LoadRC::load_str(IDS_cmExchange).c_str(), Str)==0)
			return cmExchange;
		else if (_stricmp(LoadRC::load_str(IDS_cmTransfer).c_str(), Str)==0)
			return cmTransfer;
		else 
			return 0;
	}
	static char StrToCheckState(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_csWaiting).c_str(), Str)==0)
			return csWaiting;
		else if (_stricmp(LoadRC::load_str(IDS_csPass).c_str(), Str)==0)
			return csPass;
		else if (_stricmp(LoadRC::load_str(IDS_csFail).c_str(), Str)==0)
			return csFail;
		else 
			return 0;
	}
	static char StrToCardType(const char * Str)
	{
		//if (_stricmp(LoadRC::load_str(IDS_uptTrade).c_str(), Str)==0)
		//	return rutTrade;
		//else if (_stricmp(LoadRC::load_str(IDS_uptCash).c_str(), Str)==0)
		//	return rutCash;
		//else 
			return 0;
	}	
	static char StrToAccountType(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_atCNY).c_str(), Str)==0)
			return bYes;
		else if (_stricmp(LoadRC::load_str(IDS_atForeignSettle).c_str(), Str)==0)
			return bNo;
		else 
			return 0;
	}
	static char StrToBillType(const char * Str)
	{
		if (_stricmp(LoadRC::load_str(IDS_btDay).c_str(), Str)==0)
			return btDay;
		else if (_stricmp(LoadRC::load_str(IDS_btMonth).c_str(), Str)==0)
			return btMonth;
		else 
			return 0;
	}
//////////////////////////////////////////////////////////////////////////
public:
	static string EnquireroToStr(char AChar)
	{
		switch(AChar)
		{
		case eExchange		:			return LoadRC::load_str(IDS_eExchange);
		case eUser			:			return LoadRC::load_str(IDS_eUser);
		default:			
			string str;
			str = AChar;
			return str;
		}
	}
	static string PswTypeToStr(char AChar)
	{
		switch(AChar)
		{
		case ptTrade		:			return LoadRC::load_str(IDS_ptTradeLogin);
		case ptQuote		:			return LoadRC::load_str(IDS_ptQuoteLogin);
		case ptAuth			:			return LoadRC::load_str(IDS_ptTradeCheck);
		default:			
			string str;
			str= AChar;
			return str;
		}
	}
	static string UserIdentityToStr(char AChar)
	{
		switch(AChar)
		{
		case uiUser			:			return LoadRC::load_str(IDS_uiUser);
		case uiProxy		:			return LoadRC::load_str(IDS_uiProxy);
		case uiBroker		:			return LoadRC::load_str(IDS_uiBroker);
		case uiTrader		:			return LoadRC::load_str(IDS_uiTrade);
		case uiQUser		:			return LoadRC::load_str(IDS_uiQUser);
		default:						
			string str;
			str= AChar;
			return str;
		}
	}
	static string OrderStateToStr(char AChar)
	{
		switch(AChar)
		{
		case osSended		:			return LoadRC::load_str(IDS_osSended);
		case osFail			:			return LoadRC::load_str(IDS_osFail);
		case osAccept		:			return LoadRC::load_str(IDS_osAccept);
		case osSuspended	:			return LoadRC::load_str(IDS_osSuspended);
		case osQueued		:			return LoadRC::load_str(IDS_osQueued);
		case osCanceling	:			return LoadRC::load_str(IDS_osCanceling);
		case osModifying	:			return LoadRC::load_str(IDS_osModifying);
		case osPartCanceled	:			return LoadRC::load_str(IDS_osPartCanceled);
		case osCanceled		:			return LoadRC::load_str(IDS_osCanceled);
		case osPartFilled	:			return LoadRC::load_str(IDS_osPartFilled);
		case osFilled		:			return LoadRC::load_str(IDS_osFilled);
		case osActive		:			return LoadRC::load_str(IDS_osActive);
		case osTriggering	:			return LoadRC::load_str(IDS_osTriggering);
		case osFillTriggered:			return LoadRC::load_str(IDS_osFillTriggered);
		case osApply		:			return LoadRC::load_str(IDS_osApply);
		case osChecking		:			return LoadRC::load_str(IDS_osChecking);
		case osInvalid		:			return LoadRC::load_str(IDS_osInvalid);
		default:						
			string str;
			str= AChar;
			return str;
		}
	}
	static string OrderWayToStr(char AChar)		//定单来源
	{
		switch (AChar)
		{
		case owAll			:			return LoadRC::load_str(IDS_owAll);
		case owDelivery		:			return LoadRC::load_str(IDS_owDelivery);
		case owETrade		:			return LoadRC::load_str(IDS_owETrade);
		case owProxyETrade	:			return LoadRC::load_str(IDS_owProxyETrade);
		case owJTrade		:			return LoadRC::load_str(IDS_owJTrade);
		case owManual		:			return LoadRC::load_str(IDS_owManual);
		case owCarry		:			return LoadRC::load_str(IDS_owCarry);
		case owProgram		:			return LoadRC::load_str(IDS_owProgram);
		case owExecute		:			return LoadRC::load_str(IDS_owExecute);
		case owAbandon		:			return LoadRC::load_str(IDS_owAbandon);
		case owChannel		:			return LoadRC::load_str(IDS_owChannel);
		case owRTS			:			return LoadRC::load_str(IDS_owRTS);			
		case owSpreadSequ	:
		case owSpreadSyn	:			return LoadRC::load_str(IDS_owSpreadLocal	);
		case owHorz			:			return LoadRC::load_str(IDS_owHorz			);
		case owVert			:			return LoadRC::load_str(IDS_owVert			);
		case owClickPrice	:			return LoadRC::load_str(IDS_owClickPrice	);
		case owCard			:			return LoadRC::load_str(IDS_owCard			);
		case owBrtch		:			return LoadRC::load_str(IDS_owBrtch			);
		case owFast			:			return LoadRC::load_str(IDS_owFast			);
		case owPingPong		:			return LoadRC::load_str(IDS_owPingPong		);
		case owStopProfit	:			return LoadRC::load_str(IDS_owStopProfit	);
		case owStopLoss		:			return LoadRC::load_str(IDS_owStopLoss		);
		case owFollow		:			return LoadRC::load_str(IDS_owFollow		);
		case owCatch		:			return LoadRC::load_str(IDS_owCatch			);

		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string StrategyTypeToStr(char AChar)		//定单来源
	{
		switch (AChar)
		{
		case stNone			:			return LoadRC::load_str(IDS_stNone		);
		case stPreOrder		:			return LoadRC::load_str(IDS_stPreOrder	);
		case stAutoOrder	:			return LoadRC::load_str(IDS_stAutoOrder	);
		case stCondition	:			return LoadRC::load_str(IDS_stCondition	);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string TriggerConditionToStr(char AChar)	//触发条件
	{
		switch (AChar)
		{
		case tcNone			:			return LoadRC::load_str(IDS_tcNone		);
		case tcGreater		:			return LoadRC::load_str(IDS_tcGreater	);
		case tcLess			:			return LoadRC::load_str(IDS_tcLess		);
		case tcGreaterEqual	:			return LoadRC::load_str(IDS_tcGreaterEqual);
		case tcLessEqual	:			return LoadRC::load_str(IDS_tcLessEqual	 );
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string TriggerModeToStr(char AChar)		//触发方式
	{
		switch (AChar)
		{
		case tmNone			:			return LoadRC::load_str(IDS_tmNone	);
		case tmLatest		:			return LoadRC::load_str(IDS_tmLast	);
		case tmBid			:			return LoadRC::load_str(IDS_tmBid	);
		case tmAsk			:			return LoadRC::load_str(IDS_tmAsk	);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string OrderTypeToStr(char AChar)		//定单类型
	{
		switch (AChar)
		{
		case otLimit		:			return LoadRC::load_str(IDS_otLimit);
		case otMarket		:			return LoadRC::load_str(IDS_otMarket);
		case otLimitStop	:			return LoadRC::load_str(IDS_otLimitStop);
		case otMarketStop	:			return LoadRC::load_str(IDS_otMarketStop);
		case otEnquiry		:			return LoadRC::load_str(IDS_otEnquiry);
		case otOffer		:			return LoadRC::load_str(IDS_otOffer);
		case otExecute		:			return LoadRC::load_str(IDS_otExecute);
		case otAbandon		:			return LoadRC::load_str(IDS_otAbandon);
		case otIceberg		:			return LoadRC::load_str(IDS_otIceberg);
		case otGhost		:			return LoadRC::load_str(IDS_otGhost);
		case otSwap			:			return LoadRC::load_str(IDS_otSwap);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string PtlBoolToStr(char AChar)		//是否风险报单
	{
		switch (AChar)
		{
		case bYes			:			return LoadRC::load_str(IDS_bYes);
		case bNo			:			return LoadRC::load_str(IDS_bNo);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string DirectToStr(char AChar)		//买卖方向
	{
		switch (AChar)
		{
		case dBuy			:			return LoadRC::load_str(IDS_dBuy);
		case dSell			:			return LoadRC::load_str(IDS_dSell);
		case dBoth			:			return LoadRC::load_str(IDS_dBoth);
		case dNone			:			return LoadRC::load_str(IDS_NONE);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string DirectToStrEx(char AChar)	//买卖方向
	{
		switch (AChar)
		{
		case dBuy			:			return LoadRC::load_str(IDS_dBuy_Ex);
		case dSell			:			return LoadRC::load_str(IDS_dSell_Ex);
		case dBoth			:			return LoadRC::load_str(IDS_dBoth_Ex);
		case dNone			:			return LoadRC::load_str(IDS_NONE);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string CommodityTypeToStr(char AChar)
	{
		switch (AChar)
		{
		case ctUnDefine		:			return LoadRC::load_str(IDS_ctUnDefine);
		case ctGoods		:			return LoadRC::load_str(IDS_ctGoods);
		case ctDefer		:			return LoadRC::load_str(IDS_ctDefer);
		case ctFutures		:			return LoadRC::load_str(IDS_ctFutures);
		case ctOption		:			return LoadRC::load_str(IDS_ctOption);
		case ctSpreadMonth	:			return LoadRC::load_str(IDS_ctSpreadMonth);
		case ctSpreadCommodity:			return LoadRC::load_str(IDS_ctSpreadCommodity);		
		case ctBUL			:			return LoadRC::load_str(IDS_ctBUL);
		case ctBER			:			return LoadRC::load_str(IDS_ctBER);
		case ctBLT			:			return LoadRC::load_str(IDS_ctBLT);	
		case ctBRT			:			return LoadRC::load_str(IDS_ctBRT);	
		case ctSTD			:			return LoadRC::load_str(IDS_ctSTD);
		case ctSTG			:			return LoadRC::load_str(IDS_ctSTG);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string HedgeToStr(char AChar)
	{
		switch (AChar)
		{
		case hSpeculate		:			return LoadRC::load_str(IDS_hSpeculate);
		case hHedge			:			return LoadRC::load_str(IDS_hHedge);
		case hSpread		:			return LoadRC::load_str(IDS_hSpread);
		case hMarket		:			return LoadRC::load_str(IDS_hMarket);
		case hNone			:			return LoadRC::load_str(IDS_hNone);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string OffsetToStr(char AChar)
	{
		switch (AChar)
		{
		case oOpen			:			return LoadRC::load_str(IDS_oOpen);
		case oCover			:			return LoadRC::load_str(IDS_oCover);
		case oCoverT		:			return LoadRC::load_str(IDS_oCoverT);
		case oOpenCover		:			return LoadRC::load_str(IDS_oOpenCover);
		case oCoverOpen		:			return LoadRC::load_str(IDS_oCoverOpen);
		case oNone			:			return LoadRC::load_str(IDS_oNone);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string OffsetToStrAbb(char AChar)
	{
		switch (AChar)
		{
		case oOpen			:			return LoadRC::load_str(IDS_oOpen_Abb);
		case oCover			:			return LoadRC::load_str(IDS_oCover_Abb);
		case oCoverT		:			return LoadRC::load_str(IDS_oCoverT_Abb);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string ValidTypeToStr(char AChar)
	{
		switch (AChar)
		{
		case vtFOK			:			return LoadRC::load_str(IDS_vtFOK); 
		case vtIOC			:			return LoadRC::load_str(IDS_vtIOC);
		case vtGFD			:			return LoadRC::load_str(IDS_vtGFD);
		case vtGTC			:			return LoadRC::load_str(IDS_vtGTC);
		case vtGTD			:			return LoadRC::load_str(IDS_vtGTD);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string ValidTypeToStrAbb(char AChar)
	{
		switch (AChar)
		{
		case vtFOK			:			return LoadRC::load_str(IDS_vtFOK_Abb); 
		case vtIOC			:			return LoadRC::load_str(IDS_vtIOC_Abb);
		case vtGFD			:			return LoadRC::load_str(IDS_vtGFD_Abb);
		case vtGTC			:			return LoadRC::load_str(IDS_vtGTC_Abb);
		case vtGTD			:			return LoadRC::load_str(IDS_vtGTD_Abb);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string ValidTypeToStrEx(char AChar)
	{
		switch (AChar)
		{
		case vtFOK			:			return LoadRC::load_str(IDS_vtFOK_Ex); 
		case vtIOC			:			return LoadRC::load_str(IDS_vtIOC_Ex);
		case vtGFD			:			return LoadRC::load_str(IDS_vtGFD_Ex);
		case vtGTC			:			return LoadRC::load_str(IDS_vtGTC_Ex);
		case vtGTD			:			return LoadRC::load_str(IDS_vtGTD_Ex);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static bool PtlBoolToBool(char AChar)
	{
		return (bYes==AChar);
	}
	static string CashTypeToStr(char AChar)
	{
		switch (AChar)
		{
		case ctOut			:			return LoadRC::load_str(IDS_ctOut);
		case ctIn			:			return LoadRC::load_str(IDS_ctIn);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string CashModeToStr(char AChar)
	{
		switch (AChar)
		{
		case cmAmount		:			return LoadRC::load_str(IDS_cmAmount);
		case cmCheck		:			return LoadRC::load_str(IDS_cmCheck);
		case cmCash			:			return LoadRC::load_str(IDS_cmCash);
		case cmExchange		:			return LoadRC::load_str(IDS_cmExchange);
		case cmTransfer		:			return LoadRC::load_str(IDS_cmTransfer);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string CheckModeToStr(char AChar)
	{
		switch (AChar)
		{
		case cmNone			:			return LoadRC::load_str(IDS_cmNone);
		case cmAutoCheck	:			return LoadRC::load_str(IDS_cmAutoCheck);
		case cmManualCheck	:			return LoadRC::load_str(IDS_cmManualCheck);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string CheckStateToStr(char AChar)
	{
		switch (AChar)
		{
		case csNone			:			return LoadRC::load_str(IDS_csNone);
		case csWaiting		:			return LoadRC::load_str(IDS_csWaiting);
		case csPass			:			return LoadRC::load_str(IDS_csPass);
		case csFail			:			return LoadRC::load_str(IDS_csFail);
		default:
			string str;
			str= AChar;
			return str;
		}
	}	
	static string AdjustTypeToStr(char AChar)
	{
		switch (AChar)
		{
		case csWaiting		:			return LoadRC::load_str(IDS_csWaiting);
		case csPass			:			return LoadRC::load_str(IDS_csPass);
		case csFail			:			return LoadRC::load_str(IDS_csFail);
		case csTransing		:			return LoadRC::load_str(IDS_csTransing);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string RelateTypeToStr(char AChar)
	{
		switch (AChar)
		{
		case uptTrade		:			return LoadRC::load_str(IDS_uptTrade);
		case uptCash		:			return LoadRC::load_str(IDS_uptCash);
		case uptBillVerify	:			return LoadRC::load_str(IDS_uptBillVerify);
		case uptOpenAccount	:			return LoadRC::load_str(IDS_uptAccountHold);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string CardTypeToStr(char AChar)
	{
		//switch (AChar)
		//{
		//case rutTrade			:		return LoadRC::load_str(IDS_rutTrade);
		//case rutCash			:		return LoadRC::load_str(IDS_rutCash);
		//default:
			string str;
			str= AChar;
			return str;
		//}
	}
	static string DeleteToStr(char AChar)
	{
		if (AChar==dtYes)
			return LoadRC::load_str(IDS_htDelete);
		else
			return LoadRC::load_str(IDS_htNone);
	}
	static string AccountTypeToStr(char AChar)
	{
		switch (AChar)
		{
		case bYes			:			return LoadRC::load_str(IDS_atCNY);
		case bNo			:			return LoadRC::load_str(IDS_atForeignSettle);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
	static string TradeSectionToStr(char AChar)
	{
		switch (AChar)
		{
		case tsDay			:			return LoadRC::load_str(IDS_tsDay);
		case tsNight		:			return LoadRC::load_str(IDS_tsNight);
		case tsAll			:			return LoadRC::load_str(IDS_tsAll);
		default:
			string str;
			str= AChar;
			return str;
		}

	}
	static string FATCAStateToStr(char AChar)
	{
		switch (AChar)
		{
		case fsAmerican		:			return LoadRC::load_str(IDS_fsAmerican);			
		case fsUnAmerican	:			return LoadRC::load_str(IDS_fsUnAmerican);		
		case fsNoIntention	:			return LoadRC::load_str(IDS_fsNoIntention);		
		case fsWilling		:			return LoadRC::load_str(IDS_fsWilling);			
		case fsTempUser		:			return LoadRC::load_str(IDS_fsTempUser);			
		case fsNoApplicable	:			return LoadRC::load_str(IDS_fsNoApplicable);		
		case fsPFFI			:			return LoadRC::load_str(IDS_fsPFFI);				
		case fsDCFFI		:			return LoadRC::load_str(IDS_fsDCFFI);			
		case fsODFFI		:			return LoadRC::load_str(IDS_fsODFFI);			
		case fsUSE			:			return LoadRC::load_str(IDS_fsUSE);				
		case fsUSEBO		:			return LoadRC::load_str(IDS_fsUSEBO);			
		case fsPNFFE		:			return LoadRC::load_str(IDS_fsPNFFE);			
		case fsEE			:			return LoadRC::load_str(IDS_fsEE);				
		case fsNPFFI		:			return LoadRC::load_str(IDS_fsNPFFI);			
		default:							   
			string str;
			str= AChar;
			return str;
		}
	}	
	static string TaxTypeToStr(char AChar)
	{
		switch (AChar)
		{
		case ttW8			:			return LoadRC::load_str(IDS_ttW8);					
		case ttW9			:			return LoadRC::load_str(IDS_ttW9);					
		default:						
			string str;
			str= AChar;
			return str;
		}
	}
	static string TradePlatFormToStr(char AChar)
	{
		switch (AChar)
		{
		case tpEsunny		:			return LoadRC::load_str(IDS_tpEsunny);				
		case tpJTrade		:			return LoadRC::load_str(IDS_tpJTrade);
		case tpPats			:			return LoadRC::load_str(IDS_tpPATS);				
		case tpSP			:			return LoadRC::load_str(IDS_tpSP);				
		default:						
			string str;
			str= AChar;
			return str;
		}
	}
	//正在初始化的进度
	static string InitingToStr(TradeInitProgress Progress)
	{		
		switch (Progress)
		{
		case tipNone				:		return LoadRC::load_str(IDS_tipNone				);
		case tipAgreementSign		:		return LoadRC::load_str(IDS_tipAgreementSign		);
		case tipAgreementInfoQry	:		return LoadRC::load_str(IDS_tipAgreementInfo 	);
		case tipSignAgreement		:		return LoadRC::load_str(IDS_tipSignAgreement		);
		case tipPswValidDay			:		return LoadRC::load_str(IDS_tipPswValidDay		);
		case tipPswDate				:		return LoadRC::load_str(IDS_tipPswDate			);
		case tipUser				:		return LoadRC::load_str(IDS_tipUser				);
		case tipRight				:		return LoadRC::load_str(IDS_tipRight				);
		case tipBank				:		return LoadRC::load_str(IDS_tipBank				);
		case tipUpper				:		return LoadRC::load_str(IDS_tipUpper				);
		case tipExchange			:		return LoadRC::load_str(IDS_tipExchange			);
		case tipCommodity			:		return LoadRC::load_str(IDS_tipCommodity			);
		case tipContract			:		return LoadRC::load_str(IDS_tipContract			);
		case tipCommodityOrderType	:		return LoadRC::load_str(IDS_tipCommodityOrderType);
		case tipCommodityValidType	:		return LoadRC::load_str(IDS_tipCommodityValidType);
		case tip3MExpDate			:		return LoadRC::load_str(IDS_tip3MExpDate			);
		case tipOrder				:		return LoadRC::load_str(IDS_tipOrder				);
		case tipMatch				:		return LoadRC::load_str(IDS_tipMatch				);
		case tipPosition			:		return LoadRC::load_str(IDS_tipPosition			);
		case tipLiquidate			:		return LoadRC::load_str(IDS_tipLiquidate			);
		case tipMoney				:		return LoadRC::load_str(IDS_tipMoney				);
		case tipMsg					:		return LoadRC::load_str(IDS_tipMsg				);
		case tipLocalData			:		return LoadRC::load_str(IDS_tipLocalData			);
		case tipReady				:		return LoadRC::load_str(IDS_tipReady				);
		default:
			char str[50];
			_itoa_s(Progress, str, 10);
			return str;
		}
	}
	//初始化完成的进度
	static string InitedToStr(TradeInitProgress Progress)
	{		
		switch (Progress)
		{
		case tipNone				:		return LoadRC::load_str(IDS_tipNone					);
		case tipAgreementSign		:		return LoadRC::load_str(IDS_tipAgreementSign_ed		);
		case tipAgreementInfoQry	:		return LoadRC::load_str(IDS_tipAgreementInfo_ed 		);
		case tipSignAgreement		:		return LoadRC::load_str(IDS_tipSignAgreement_ed		);
		case tipPswValidDay			:		return LoadRC::load_str(IDS_tipPswValidDay_ed		);
		case tipPswDate				:		return LoadRC::load_str(IDS_tipPswDate_ed			);
		case tipUser				:		return LoadRC::load_str(IDS_tipUser_ed				);
		case tipRight				:		return LoadRC::load_str(IDS_tipRight_ed				);
		case tipBank				:		return LoadRC::load_str(IDS_tipBank_ed				);
		case tipUpper				:		return LoadRC::load_str(IDS_tipUpper_ed				);
		case tipExchange			:		return LoadRC::load_str(IDS_tipExchange_ed			);
		case tipCommodity			:		return LoadRC::load_str(IDS_tipCommodity_ed			);
		case tipContract			:		return LoadRC::load_str(IDS_tipContract_ed			);
		case tipCommodityOrderType	:		return LoadRC::load_str(IDS_tipCommodityOrderType_ed	);
		case tipCommodityValidType	:		return LoadRC::load_str(IDS_tipCommodityValidType_ed	);
		case tip3MExpDate			:		return LoadRC::load_str(IDS_tip3MExpDate_ed			);
		case tipOrder				:		return LoadRC::load_str(IDS_tipOrder_ed				);
		case tipMatch				:		return LoadRC::load_str(IDS_tipMatch_ed				);
		case tipPosition			:		return LoadRC::load_str(IDS_tipPosition_ed			);
		case tipLiquidate			:		return LoadRC::load_str(IDS_tipLiquidate_ed			);
		case tipMoney				:		return LoadRC::load_str(IDS_tipMoney_ed				);
		case tipMsg					:		return LoadRC::load_str(IDS_tipMsg_ed				);
		case tipLocalData			:		return LoadRC::load_str(IDS_tipLocalData_ed			);
		case tipReady				:		return LoadRC::load_str(IDS_tipReady					);
		default:
			char str[50];
			_itoa_s(Progress, str, 10);
			return str;
		}
	}
	static string BillTypeToStr(char AChar)
	{
		switch(AChar)
		{
		case btDay					:		return LoadRC::load_str(IDS_btDay					);
		case btMonth				:		return LoadRC::load_str(IDS_btMonth					);
		default:
			string str;
			str= AChar;
			return str;
		}
	}
};