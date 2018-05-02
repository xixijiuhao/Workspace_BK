#pragma once

#include <string.h>

#pragma pack(push) 
#pragma pack(1) 

namespace OptionCalc
{
	//合约类型(默认期货期权）
	typedef char TContractTypeType;
	const TContractTypeType		ctFuture			= 'F';		//期货期权
	const TContractTypeType		ctStock				= 'S';		//股票期权
	const TContractTypeType		ctIndex				= 'I';		//指数期权
	const TContractTypeType		ctRate				= 'R';		//利率期权
	const TContractTypeType		ctCurrency			= 'C';		//货币期权

	typedef unsigned int TCalcModelType;
	const TCalcModelType		cmBlackScholesMerton= 0;
	const TCalcModelType		cmAmerBinomial		= 1;
	const TCalcModelType		cmEuroBinomial		= 2;
	const TCalcModelType		cmAmerBaw			= 3;

	typedef unsigned int TCalcTypeType;			
	const TCalcTypeType			ctPrice				= 1;		//理论价
	const TCalcTypeType			ctIV 				= 1<<1;		//隐含波动率
	const TCalcTypeType         ctDelta             = 1<<2;     //Delta
	const TCalcTypeType         ctGamma             = 1<<3;     //Gamma
	const TCalcTypeType         ctTheta             = 1<<4;     //Theta
	const TCalcTypeType         ctVega              = 1<<5;     //Vega
	const TCalcTypeType         ctRho               = 1<<6;     //Rho
	//派息周期
	typedef char TDividendPeriodType;
	const TDividendPeriodType	dpMonthly			= 'M';		//一月
	const TDividendPeriodType	dpQuarterly			= 'Q';		//季度
	const TDividendPeriodType	dpSemiAnnually		= 'S';		//半年
	const TDividendPeriodType	dpAnnually			= 'A';		//一年

	//计算器输入参数
	struct TCalcParam
	{
		TContractTypeType	ContractType		;//合约类型
		TCalcModelType		CalcModel			;//计算模型
		TCalcTypeType		CalcyType			;//计算类型


		double				UnderlyingPrice		;//标的物价格			between(PRICE_MIN, PRICE_MAX)
		double				StrikerPrice		;//执行价				between(PRICE_MIN, PRICE_MAX)
		int					ExpirationDays		;//到期天数			between(DAY_MIN, DAY_MAX)
		double				InterestRate		;//利率(%)			between(INTEREST_RATE_MIN, INTEREST_RATE_MAX)
		double				Volatility			;//波动率(%)			between(VOLATILITY_MIN, VOLATILITY_MAX)
		int					Steps				;//步长			    （默认500）

		double				DividendDate		;//派息日期 (yyyy-mm-dd):
		double				DividendAmount		;//股息金额
		TDividendPeriodType	DividendPeriod		;//派息周期

		//计算隐含波动率的附加参数，为零时不计算隐含波动率
		double				CallPrice			;//买权实际价格		between(0, PRICE_MAX)
		double				PutPrice			;//卖权实际价格		between(0, PRICE_MAX)
		double				Accuracy			;//计算隐含波动率误差	（默认1e-5）
		int					MaxIteration		;//最大迭代次数		（默认35）       
	public:
		TCalcParam(){memset(this, 0, sizeof(TCalcParam));}
	};

	//计算器输出结果
	struct TCalcResult
	{
		double	CallPrice	;
		double	PutPrice	;

		double	CallDelta	;
		double	PutDelta 	;

		double	CallGamma	;
		double	PutGamma	;

		double	CallTheta	;
		double	PutTheta	;

		double	CallVega	;
		double	PutVega		;

		double	CallRho		;
		double	PutRho		;

		double	CallIV		;
		double	PutIV		;
	public:
		TCalcResult(){memset(this, 0, sizeof(TCalcResult));}
	};
}

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
using namespace OptionCalc;

#ifdef OPTIONCALC_EXPORTS
#define OPTIONCALC_API extern "C" __declspec(dllexport)
#else
#define OPTIONCALC_API extern "C" __declspec(dllimport)
#endif

OPTIONCALC_API bool _stdcall OptionCalculate(TCalcParam &input,  TCalcResult &output);

