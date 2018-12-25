#include "quote_dll.h"
#include "quote_ex.h"


QuoteDll::~QuoteDll()
{	
	LockScope();

	StlFun::DeleteMap(m_results);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//先按合约代码查找最近的计算，如果最近的计算价格和本次相同，则不用重复计算，直接返回查找到的计算结果
//如果没有找到或计算价格与本次不同，则重新计算，并将结果更新到缓冲里
const TCalcResult * QuoteDll::CalcOption(const TContractSingleKey & contract, const double & Price)
{	
	if (contract.OptionType == otNone)
		return nullptr;

	LockScope();

	mpOptionCalc::iterator it = m_results.find(contract.conSingleKey());
	if (it!=m_results.end() && MathFun::SameValue(it->second->CalcPrice, Price, 0.0000001))
		return &it->second->Result;
	
	TQuote quote;
	if (!QuoteFun::get_quote(contract, quote))
		return nullptr;

	OptionCalc * ret = nullptr;
	OptionCalc Calc = {0};
	Calc.CalcPrice = Price;	
	if (g_quote_api->CalcOptionPrice(QuoteFun::to_quote_option_series(contract).c_str(), contract.StrikePrice, *(QOptionPriceRet*)(&Calc.Result))) 
	{
		if (it != m_results.end())
		{
			ret  = it->second;
			*ret = Calc;
		}
		else
			ret = StlFun::SaveToMap(m_results, Calc, contract.conSingleKey());
	}

	if (ret)
		return &ret->Result;
	else
		return nullptr;
}