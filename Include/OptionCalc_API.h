#pragma once

#pragma pack(push, 1)

namespace OptionCalc
{
	//��Լ����(Ĭ���ڻ���Ȩ��
	typedef char TContractTypeType;
	const TContractTypeType		ctFuture			= 'F';		//�ڻ���Ȩ
	const TContractTypeType		ctStock				= 'S';		//��Ʊ��Ȩ
	const TContractTypeType		ctIndex				= 'I';		//ָ����Ȩ
	const TContractTypeType		ctRate				= 'R';		//������Ȩ
	const TContractTypeType		ctCurrency			= 'C';		//������Ȩ

	//����ģʽ
	typedef unsigned int TCalcModelType;
	const TCalcModelType		cmBlackScholesMerton= 0;
	const TCalcModelType		cmAmerBinomial		= 1;
	const TCalcModelType		cmEuroBinomial		= 2;
	const TCalcModelType		cmAmerBaw			= 3;

	//���ֵ�������������ʹ��
	typedef unsigned int TCalcTypeType;			
	const TCalcTypeType			ctPrice				= 1;		//���ۼ�
	const TCalcTypeType			ctIV 				= 1<<1;		//����������
	const TCalcTypeType         ctDelta             = 1<<2;     //Delta
	const TCalcTypeType         ctGamma             = 1<<3;     //Gamma
	const TCalcTypeType         ctTheta             = 1<<4;     //Theta
	const TCalcTypeType         ctVega              = 1<<5;     //Vega
	const TCalcTypeType         ctRho               = 1<<6;     //Rho

	//��Ϣ����
	typedef char TDividendPeriodType;
	const TDividendPeriodType	dpMonthly			= 'M';		//һ��
	const TDividendPeriodType	dpQuarterly			= 'Q';		//����
	const TDividendPeriodType	dpSemiAnnually		= 'S';		//����
	const TDividendPeriodType	dpAnnually			= 'A';		//һ��

	//�������������
	struct TCalcParam
	{
		TContractTypeType	ContractType		;//��Լ����
		TCalcModelType		CalcModel			;//����ģ��
		TCalcTypeType		CalcyType			;//��������

		double				UnderlyingPrice		;//�����۸�		between(PRICE_MIN, PRICE_MAX)
		double				StrikerPrice		;//ִ�м�			between(PRICE_MIN, PRICE_MAX)
		int					ExpirationDays		;//��������			between(DAY_MIN, DAY_MAX)
		double				InterestRate		;//����				between(INTEREST_RATE_MIN, INTEREST_RATE_MAX)
		double				Volatility			;//������			between(VOLATILITY_MIN, VOLATILITY_MAX)
		int					Steps				;//����			    ��Ĭ��500��

		//��Ʊ��Ȩ���������Ӳ���
		double				DividendDate		;//��Ϣ���� (yyyy-mm-dd):
		double				DividendAmount		;//��Ϣ���
		TDividendPeriodType	DividendPeriod		;//��Ϣ����

		//�������������ʵĸ��Ӳ�����Ϊ��ʱ����������������
		double				CallPrice			;//��Ȩʵ�ʼ۸�		between(0, PRICE_MAX)
		double				PutPrice			;//��Ȩʵ�ʼ۸�		between(0, PRICE_MAX)
		double				Accuracy			;//����������������Ĭ��1e-5��
		int					MaxIteration		;//����������		��Ĭ��35��       
	};

	//������������
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

OPTIONCALC_API bool _stdcall OptionCalculate(OptionCalc::TCalcParam &input, OptionCalc::TCalcResult &output);

