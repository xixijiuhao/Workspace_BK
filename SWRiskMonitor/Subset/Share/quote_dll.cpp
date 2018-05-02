#include "quote_dll.h"
#include "quote_ex.h"


QuoteDll::~QuoteDll()
{	
	LockScope();

	StlFun::DeleteMap(m_results);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//�Ȱ���Լ�����������ļ��㣬�������ļ���۸�ͱ�����ͬ�������ظ����㣬ֱ�ӷ��ز��ҵ��ļ�����
//���û���ҵ������۸��뱾�β�ͬ�������¼��㣬����������µ�������
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