#pragma once

#include <map>
#include "wintools.h"
#include "OptionCalc_API.h"
#include "trade_struct.h"
#include "Singlton.h"
#ifndef _WINDOWS_
#include <windows.h>
#endif


using namespace std;

//dll����
class QuoteDll: public Singleton<QuoteDll>
{
public:		
	virtual ~QuoteDll();
protected:
	friend class Singleton<QuoteDll>;
	ScopeLock;
private:
	//�ֲַ��ղ���������������
	struct OptionCalc
	{
		double		CalcPrice;
		TCalcResult Result;
	};
	typedef map<string, OptionCalc *> mpOptionCalc;
	mpOptionCalc m_results;
public:
	const TCalcResult * CalcOption(const TContractSingleKey & Contract, const double & Price);
};