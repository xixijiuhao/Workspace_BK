#pragma once

#include "trade_struct.h"

//交易和行情的合约数据转换
class StructTrans
{
public:
	//从交易数据转到行情数据
	static inline void ToQuoteExchange(DC_ExchangeInfo & dest, const TExchangeKey & sour)
	{
		strncpy_s(dest.ExchangeNo	, sour.ExchangeNo);
	}
	static inline void ToQuoteCommodity(DC_CommodityInfo & dest, const TCommodityKey & sour)
	{
		strncpy_s(dest.ExchangeNo	, sour.ExchangeNo);

		strncpy_s(dest.CommodityNo	, sour.CommodityNo);
		dest.CommodityType			= sour.CommodityType;
	}
	static inline void ToQuoteContract(DC_ContractInfo & dest, const TContractKey & sour)
	{
		ToQuoteCommodity(dest		, sour);

		strncpy_s(dest.ContractNo1	, sour.ContractNo);
		strncpy_s(dest.StrikePrice1	, sour.StrikePrice);
		dest.CallOrPutFlag1			= sour.OptionType;
	
		strncpy_s(dest.ContractNo2	, sour.ContractNo2);
		strncpy_s(dest.StrikePrice2	, sour.StrikePrice2);
		dest.CallOrPutFlag2			= sour.OptionType2;
	}
	
	//从交易数据转到行情数据
	static inline void ToTradeExchange(TExchangeKey & dest, const DC_ExchangeInfo & sour, const char * sign = "")
	{
		strncpy_s(dest.Sign			, sign);
		strncpy_s(dest.ExchangeNo	, sour.ExchangeNo);
	}
	static inline void ToTradeCommodity(TCommodityKey & dest, const DC_CommodityInfo & sour, const char * sign = "")
	{
		strncpy_s(dest.Sign			, sign);
		strncpy_s(dest.ExchangeNo	, sour.ExchangeNo);

		strncpy_s(dest.CommodityNo	, sour.CommodityNo);
		dest.CommodityType			= sour.CommodityType;
	}
	static inline void ToTradeContract(TContractKey & dest, const DC_ContractInfo & sour, const char * sign = "")
	{
		ToTradeCommodity(dest		, sour, sign);

		strncpy_s(dest.ContractNo	, sour.ContractNo1);
		strncpy_s(dest.StrikePrice	, sour.StrikePrice1);
		dest.OptionType				= sour.CallOrPutFlag1;

		strncpy_s(dest.ContractNo2	, sour.ContractNo2);
		strncpy_s(dest.StrikePrice2	, sour.StrikePrice2);
		dest.OptionType2			= sour.CallOrPutFlag2;
	}

	//单腿交易合约转到完整的交易和约
	static inline void ToTradeContract(TContractKey & dest, const TContractSingleKey & sour)
	{
		memcpy_s(&dest, sizeof(sour), &sour, sizeof(sour));

		dest.OptionType2			= otNone;
	}
};