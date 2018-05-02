#pragma once


#include "tradeapi_struct.h"

#ifdef STRUCT_FUNCTION
//trade_struct.h里重新定义了这里的所有结构体，且其名称和内存布局都和这里的一样，只是为了调用方便增加了成员函数
#include "trade_struct.h"
#else


#pragma pack(push, 1)

//资金账号对应计算参数查询结构
struct	TFundCalcParamReq : public TContractSingleKey
{
	char					UserNo[21];					//资金账户
};

//资金账号对应计算参数应答结构
struct	TFundCalcParam : public TFundCalcParamReq
{
	char					CurrencyNo[11];				//币种编号

	double					OpenFeeRatio;				//开仓手续费比例
	double					ColseFeeRatio;				//平仓手续费比例
	double					ColseTodayFeeRatio;			//平今手续费比例
	double					OpenFeeAmount;				//开仓手续费数额
	double					CoverFeeAmount;				//平仓手续费数额
	double					CoverTodayFeeAmount;		//平今手续费数额
	
	double					StrikeFeeRatio;				//行权手续费比例	
	double					StrikeFeeAmount;			//行权手续费数额

	double                  SLongDepositRatio;			//多头投机保证金比例
	double                  SLongDepositAmount;			//多头投机保证金数额
	double                  SShortDepositRatio;			//空头投机保证金比例
	double                  SShortDepositAmount;		//空头投机保证金数额
	double                  HLongDepositRatio;			//多头套保保证金比例
	double                  HLongDepositAmount;			//多头套保保证金数额
	double                  HShortDepositRatio;			//空头套保保证金比例
	double                  HShortDepositAmount;		//空头套保保证金数额
	double                  ALongDepositRatio;			//多头套利保证金比例
	double                  ALongDepositAmount;			//多头套利保证金数额
	double                  AShortDepositRatio;			//空头套利保证金比例
	double                  AShortDepositAmount;		//空头套利保证金数额
	double                  MLongDepositRatio;			//多头做市保证金比例
	double                  MLongDepositAmount;			//多头做市保证金数额
	double                  MShortDepositRatio;			//空头做市保证金比例
	double                  MShortDepositAmount;		//空头做市保证金数额

	const TCurrency *		pCurrency;
};

#pragma pack(pop)

#endif