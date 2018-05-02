#pragma once
#include <string>
#include "TradeApi_Type.h"//½»Ò×
using namespace std;


struct MsgOrderInfo
{
	TDirect Direct;
	TOffset Offset;
	TOffset SellOffset;
	MsgOrderInfo() :Direct(dNone), Offset(oNone), SellOffset(oNone){}
};
class IOperator
{
public:
    virtual void SetContractNo(string sCode) = 0;
};