#ifndef _TRADE_QUOTE_H_
#define _TRADE_QUOTE_H_

namespace CConvert
{
	TCommodityType TradeTypeToQuoteType(char cType);
	void TradeContractToRawContractIdType(const TContractKey &ContractKey, char(&ContractId)[51]);
	TOffset GetOffset(const char* UserNo, const TContractSingleKey * ContractKey, TDirect direct);
	void DefaultSendOrder(TSendOrder&SendOrder);
	void TOrderState2Char(TOrderState tstate, TStrategyType ttype, char* sztext, int nsize/* = MAX_PATH*/);
	void TDirect2Char(TDirect tdirect, char* sztext, int nsize);
	void TOffset2Char(TOffset toffset, char* sztext, int nsize);

	wstring ANSIToUnicode(const string& str);
	COLORREF SetOrderStateColor(TOrderState state);
}

#endif