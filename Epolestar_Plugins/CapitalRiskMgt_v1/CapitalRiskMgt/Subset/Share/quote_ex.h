#pragma once

#include <string>
#include "QuoteApi_API.h"
#include "TradeApi_API.h"
#include "global.h"
#include "quote_field.h"
#include "quote_spi.h"


#define Contract_Split_1		'|' 
#define Contract_Split_2		'&' 
#define Contract_Split_3		'^' 

extern IQuoteApi *	g_quote_api;

class TQuote : public QQuoteSnapShot
{
private:
	class Price
	{
	private:
		TQuote & m_quote;
	public:
		Price(TQuote & quote) : m_quote(quote){}
		inline double & operator[](const QFidMeanType ind)
		{
			return m_quote[ind].Price;
		}
	};
	class Qty
	{
	private:
		TQuote & m_quote;
	public:
		Qty(TQuote & quote) : m_quote(quote){}
		inline long long & operator[](const QFidMeanType ind)
		{
			return m_quote[ind].Qty;
		}
	};
public:
	QContractNoType ContractId;
	TQuote() : price(*this), qty(*this)
	{
		memset(this, 0, sizeof(QQuoteSnapShot));
	}
	TQuote(const char * contract_id) : price(*this), qty(*this)
	{
		strncpy_s(ContractId, contract_id);
	}
	TQuote(const QQuoteSnapShot & quote) : price(*this), qty(*this)
	{
		*this = quote;
	}
	void operator=(const QQuoteSnapShot & quote)
	{
		*this = quote;
	}
public:
	Price price;
	Qty qty;
	inline QQuoteField & operator[](const QFidMeanType ind)
	{
		return Data[ind];
	}
	inline QContractNoType & contract_id()
	{
		return ContractId;
	}
	inline QDateTimeType & update_time()
	{
		return Data[qfDateTime].DateTime;
	}
	inline char & state()
	{
		return (*this)[qfTradeState].State;
	}
	inline bool exist_limit_up_down()
	{
		return Data[qfLimitUp].FidAttr != FidAttr_None && Data[qfLimitDown].FidAttr != FidAttr_None;
	}
	inline bool is_limit_up()
	{
		return Data[qfLimitUp].FidAttr != FidAttr_None && MathFun::SameValue(price[qfBidPrice1], price[qfLimitUp]);
	}
	inline bool is_limit_down()
	{
		return Data[qfLimitDown].FidAttr != FidAttr_None && MathFun::SameValue(price[qfAskPrice1], price[qfLimitDown]);
	}
};

//行情相关函数
static QuoteDataSpi g_quote_spi;
class QuoteFun
{
private:
	static bool to_trade_contract(const Strings & items, Strings & commoditys, TContractSingleKey & key)
	{
		if (items.size() < 3)
			return false;
		if (!items[1].empty() && items[1][0]==ctSpreadLocal)		//不处理本地套利		
			return false;


		StrFun::SplitString(items[2], commoditys, Contract_Split_2, 2);
		
		strncpy_s(key.ExchangeNo, items[0].c_str());
		key.CommodityType = items[1][0];
		strncpy_s(key.CommodityNo, commoditys[0].c_str());

		if (items.size() < 4)
			return true;

		size_t pos = items[3].find(otCall);
		if (pos == string::npos)
			pos = items[3].find(otPut);

		if (pos != string::npos)
		{
			strncpy_s(key.ContractNo, items[3].substr(0, pos).c_str());
			key.OptionType = items[3][pos];
			strncpy_s(key.StrikePrice, items[3].substr(pos + 1, items[3].length() - pos - 1).c_str());
		}
		else
		{
			strncpy_s(key.ContractNo, items[3].c_str());
			key.OptionType = otNone;
			key.StrikePrice[0] = 0;
		}

		return true;
	}
public:
	//行情合约转到单腿交易合约, Sign需要另外赋值
	static bool to_trade_contract(const char * contract_id, TContractSingleKey & key)
	{
		Strings items, commoditys;
		StrFun::SplitString(contract_id, items, Contract_Split_1);
		return to_trade_contract(items, commoditys, key);
	}
	//行情合约转到套利交易合约, Sign需要另外赋值
	static bool to_trade_contract(const char * contract_id, TContractKey & key)
	{
		memset(&key, 0, sizeof(key));
		key.OptionType2 = otNone;

		Strings items, commoditys;
		StrFun::SplitString(contract_id, items, Contract_Split_1);
		if (!to_trade_contract(items, commoditys, key))
			return false;		

		//组合
		if (commoditys.size() > 1 || items.size() > 4)
		{
			if (items.size() > 4)
			{
				int pos = items[4].find(otCall);
				if (pos == string::npos)
					pos = items[4].find(otPut);
				if (pos != string::npos)
				{
					strncpy_s(key.ContractNo2, items[4].substr(0, pos).c_str());
					key.OptionType2 = items[4][pos];
					strncpy_s(key.StrikePrice2, items[4].substr(pos + 1, items[4].length() - pos - 1).c_str());
				}
				else
				{
					strncpy_s(key.ContractNo2, items[4].c_str());
					key.OptionType2 = otNone;
					key.StrikePrice2[0] = 0;
				}
			}
			else
			{
				strncpy_s(key.ContractNo2, key.ContractNo);
				key.OptionType2 = key.OptionType;
				strncpy_s(key.StrikePrice2, key.StrikePrice);
			}			
		}
		return true;
	}
	static double get_tick(const char * contract_id)
	{
		if (!g_quote_api)
			return 1;

		double tick = 1;
		QContract * contract = nullptr;
		QCommodity * commodity = nullptr;
		if (g_quote_api->GetContractData(nullptr, contract_id, &contract, 1, false))
			if (g_quote_api->GetCommodityData(nullptr, contract->Commodity->CommodityNo, &commodity, 1, false))
				tick = commodity->Nume;

		return tick;
	}
	static int get_precision(const char * contract_id)
	{
		int ret = 0;
		char s_precision[KEY_LENGTH] = {0};
		_snprintf_s(s_precision, "%.10f", get_tick(contract_id));
		for (int i = sizeof(s_precision) - 1; i >= 0; i--)
		{
			if (ret == 0 && s_precision[i] > '0')
				ret = i;
			if (s_precision[i] == '.' && ret > i)
			{
				ret -= i;
				break;
			}
		}

		return ret;
	}
	//交易合约转行情合约
	static string to_quote_contract_id(const TContractSingleKey & conKey)
	{
		string code = conKey.ExchangeNo + string(1,Contract_Split_1) + string(1,conKey.CommodityType) + string(1,Contract_Split_1) + string(conKey.CommodityNo) + string(1,Contract_Split_1) + conKey.ContractNo;
		if (conKey.OptionType==otPut || conKey.OptionType==otCall)
			code.append(string(1,conKey.OptionType) + conKey.StrikePrice);

		return code;
	}
	//交易合约转行情期权系列代码
	static string to_quote_option_series(const TContractSingleKey & conKey)
	{
		return conKey.ExchangeNo + string(1,Contract_Split_1) + string(1,conKey.CommodityType) + string(1,Contract_Split_1) + string(conKey.CommodityNo) + string(1,Contract_Split_1) + conKey.ContractNo;
	}
	//交易合约转行情合约
	static string to_quote_contract_id(const TContractKey & conKey)
	{
		string code = to_quote_contract_id((TContractSingleKey)conKey);
		if (conKey.ContractNo2[0])
			code.append(string(1,Contract_Split_1) + conKey.ContractNo2);
		if (conKey.OptionType2==otPut || conKey.OptionType2==otCall)
			code.append(string(1,conKey.OptionType2) + conKey.StrikePrice2);

		return code;
	}
	static inline bool get_quote(TQuote & quote)
	{		
		if (!g_quote_api)
			return false;
		bool ret = g_quote_api->GetSnapshot(quote.ContractId, &quote);
		if (!ret)
			g_quote_api->SubQuote(&quote.contract_id(), 1, &g_quote_spi);
		return ret;
	}
	static inline bool get_quote(const char * contract_id, TQuote & quote)
	{		
		strncpy_s(quote.contract_id(), contract_id);
		return get_quote(quote);
	}
	static inline bool get_quote(const TContractSingleKey & conKey, TQuote & quote)
	{
		return get_quote(to_quote_contract_id(conKey).c_str(), quote);
	}
	static inline bool get_quote(const TContractKey & conKey, TQuote & quote)
	{		
		return get_quote(to_quote_contract_id(conKey).c_str(), quote);
	}
	static inline double get_out_price(TQuote & quote, TPriceType prc_type, int out_tick, const TDirect direct)
	{
		double prc = 0;
		double prc_dx = QuoteFun::get_tick(quote.contract_id()) * out_tick;

		if (prc_type == ptUnLimit && !quote.exist_limit_up_down() ||
			prc_type == ptLast && quote.qty[qfTotalQty] == 0)
			prc_type = ptMatch;

		switch (prc_type)
		{
			case ptMarket		: prc = 0; break;
			case ptLast			: prc = quote.price[qfLast];	break;
			case ptUnLimit		: prc = (direct == dBuy) ? quote.price[qfLimitUp] : quote.price[qfLimitDown];	break;
			case ptWorking		: prc = (direct == dBuy) ? quote.price[qfBidPrice1] : quote.price[qfAskPrice1]; break;
			case ptMatch		: 
			default				: prc = (direct == dBuy) ? quote.price[qfAskPrice1] : quote.price[qfBidPrice1]; break;
		}

		if (prc_type == ptMatch || prc_type == ptWorking || prc_type == ptLast)
		{
			if (quote.is_limit_down())
				prc = quote.price[qfLimitDown];
			else if (quote.is_limit_up())
				prc = quote.price[qfLimitUp];				
			prc = (direct == dBuy) ? prc + prc_dx : prc - prc_dx;

			if (quote.exist_limit_up_down())
			{
				prc = min(prc, quote.price[qfLimitUp]);
				prc = max(prc, quote.price[qfLimitDown]);
			}
		}

		return prc;
	}
};

//本地套利相关函数
class LocalSpdFun
{
	//本地套利行情代码格式：ZCE&SHFE^F&O|L|SR&CU|1511|1519C25300、 ZCE&SHFE^F|L|SR&CU|1511、ZCE^F|L|SR&CF|1511|1519、ZCE^F|L|SR&CF|1511、ZCE^F|L|SR|1511|1519
public:
	//从本地套利代码获取两个单腿合约
	static bool get_single_contracts(string & code1, string & code2, const char * local_spd_contract_id)
	{		
		Strings items;
		StrFun::SplitString(local_spd_contract_id, items, Contract_Split_1);
		if (items.size() < 4)
			return false;
		else if (!items[1].empty() && items[1][0]!=ctSpreadLocal)
			return false;
		else if (items.size() == 4)
			items.push_back(items[3]);		

		Strings ex;
		StrFun::SplitString(items[0], ex, Contract_Split_3, 2);
		if (ex.size()!=2)
			return false;			
		Strings exchanges;
		StrFun::SplitString(ex[0], exchanges, Contract_Split_2, 2);
		if (exchanges.size()==0)
			return false;
		else if (exchanges.size()==1)
			exchanges.push_back(exchanges[0]);
		Strings types;
		StrFun::SplitString(ex[1], types, Contract_Split_2, 2);
		if (types.size()==0)
			return false;
		else if (types.size()==1)
			types.push_back(types[0]);

		Strings commoditys;
		StrFun::SplitString(items[2], commoditys, Contract_Split_2, 2);
		if (commoditys.size()==0)
			return false;
		else if (commoditys.size()==1)
			commoditys.push_back(commoditys[0]);

		code1 = exchanges[0] + string(1, Contract_Split_1) + types[0] + string(1, Contract_Split_1) +  commoditys[0] + string(1, Contract_Split_1) +  items[3];
		code1 = exchanges[1] + string(1, Contract_Split_1) + types[1] + string(1, Contract_Split_1) +  commoditys[1] + string(1, Contract_Split_1) +  items[4];

		return true;
	}
	//从交易所套利合约获取两个单腿合约
	static bool get_single_contracts(const char * exchange_spd_contract_id, string & code1, string & code2)
	{
		Strings items;
		StrFun::SplitString(exchange_spd_contract_id, items, Contract_Split_1);
		if (items.size() < 4)
			return false;
		if (!items[1].empty() && (items[1][0] == ctSpreadLocal || items[1][0] != ctPRT))
			return false;

		if (items.size()==4)
			items.push_back(items[3]);
		
		Strings commoditys;
		StrFun::SplitString(items[2], commoditys, Contract_Split_2, 2);
		if (commoditys.size() == 1)
			commoditys.push_back(commoditys[0]);

		code1 = items[0] + string(1, Contract_Split_1) + items[1] + string(1, Contract_Split_1) +  commoditys[0] + string(1, Contract_Split_1) +  items[3];
		code1 = items[0] + string(1, Contract_Split_1) + items[1] + string(1, Contract_Split_1) +  commoditys[1] + string(1, Contract_Split_1) +  items[4];

		return true;
	}
	//从交易所套利委托获取两个单腿合约
	static bool get_single_contracts(string & code1, string & code2, const TOrder & order)
	{		
		const TCommodity * pdata = order.pContract->pCommodity;
		//if (!pdata)
		//	pdata = g_pTradeData->GetCommodity(&order);
		if (!pdata)
			return false;

		code1 = order.ExchangeNo  + string(1, Contract_Split_1) + string(1, pdata->RelateCommodityType)  + string(1, Contract_Split_1) + pdata->RelateCommodityNo  + string(1, Contract_Split_1) +  order.ContractNo;
		code2 = order.ExchangeNo + string(1, Contract_Split_1) + string(1, pdata->RelateCommodityType2) + string(1, Contract_Split_1) + pdata->RelateCommodityNo2 + string(1, Contract_Split_1) +  order.ContractNo2;
		if (order.OptionType==otPut || order.OptionType==otCall)
			code1.append(string(1, order.OptionType) + order.StrikePrice);
		if (order.OptionType2==otPut || order.OptionType2==otCall)
			code2.append(string(1, order.OptionType2) + order.StrikePrice2);

		return true;
	}
	//从两个单腿交易合约获得本地套利行情代码
	static string get_local_spd_id(const TContractSingleKey & key1, const TContractSingleKey & key2)
	{
		string code = key1.ExchangeNo;
		if (strcmp(key1.ExchangeNo, key2.ExchangeNo))
			code.append(string(1, Contract_Split_2) + key2.ExchangeNo);

		code.append(string(1, Contract_Split_3) + string(1, key1.CommodityType));
		if (key1.CommodityType !=  key2.CommodityType)
			code.append(string(1, Contract_Split_2) + string(1, key2.CommodityType));

		code.append(string(1, Contract_Split_1) + string(1, ctSpreadLocal) + string(1, Contract_Split_1) + key1.CommodityNo);
		if (strcmp(key1.CommodityNo, key2.CommodityNo))
			code.append(string(1, Contract_Split_2) + key2.CommodityNo);	

		string con1(key1.ContractNo);
		if (key1.OptionType==otPut || key1.OptionType==otCall)
			con1.append(string(1, key1.OptionType) + key1.StrikePrice);
		code.append(string(1, Contract_Split_1) + con1);

		string con2(key2.ContractNo);
		if (key2.OptionType==otPut || key2.OptionType==otCall)
			con2.append(string(1, key2.OptionType) + key2.StrikePrice);
		if (con1 != con2)
			code.append(string(1, Contract_Split_1) + con2);

		return code;
	}
	//从两个单腿行情代码获得本地套利行情代码
	static string get_local_spd_id(const char * contract_id1, const char * contract_id2)
	{
		TContractSingleKey key1, key2;
		QuoteFun::to_trade_contract(contract_id1, key1);
		QuoteFun::to_trade_contract(contract_id2, key2);
		return get_local_spd_id(key1, key2);
	}
};