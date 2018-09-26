#pragma once
#include "TradeApi_Type.h"//½»Ò×
#include <string>

struct MsgOrderInfo
{
	TDirect Direct;
	TOffset Offset;
	TOffset SellOffset;
	MsgOrderInfo() :Direct(dNone), Offset(oNone), SellOffset(oNone) {}
};

class IOperator
{
public:
	virtual void SetContractNo(string sCode) = 0;
	virtual bool SendOrder(MsgOrderInfo *msg) = 0;
	virtual bool TableButtonToEdit() = 0;
};
