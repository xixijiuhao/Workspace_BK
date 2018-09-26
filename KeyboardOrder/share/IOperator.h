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
	virtual void SetPriceType(wstring sPType)= 0;
	virtual void SetValidType(wstring sVData)= 0;
	virtual void SetMaxMinQty(string sQty) = 0;
	virtual void SendOrder(MsgOrderInfo *msg) = 0;
	virtual void SetContractNo(string sCode, bool brepaint = true,bool bnutton=false,bool bgetfreeze = false) = 0;
	virtual void SetPriceData(string sdata) = 0;
};