#pragma once
#include "TradeApi_Type.h"//交易
#define LEN_RET 21

typedef long  CodeSetType;
#define cstChange		0x00000001	//代码变化
#define cstSelect		0x00000010	//合约选择
#define cstQuote		0x00000100	//行情合约变化
#define cstQtyFreeze	0x00001000	//更新可开可平 冻结保证金

enum EN_PriceType
{
	Type_IceBerg,
	Type_Shadow,
	Type_Swap,
	Type_Option,
	Type_Reply,
	Type_End
};

//列头分段
enum MH_TYPE
{
	MH_Order,	//普通
	MH_Advanced,//高级
	MH_Strategy,//策略
};

struct MsgOrderInfo
{
	TDirect Direct;
	TOffset Offset;
	TOffset SellOffset;
	MsgOrderInfo() :Direct(dNone), Offset(oNone), SellOffset(oNone){}
};
struct sMousewinCfg
{
	bool bShowNoteInfo;
	bool bUseCfgColor;
	bool bShowLadder;
	bool bShowBatchOrder;
	COLORREF  BuyRgb;
	COLORREF  SellRgb;
	char      cFlag[Type_End + 1];
	sMousewinCfg()
	{
		memset(this, 0, sizeof(sMousewinCfg));
	}
};
class IOperator
{
public:
	virtual void SetValidType(const wchar_t* sPType) = 0;
	virtual void SetMaxMinQty(const char* sQty) = 0;
	virtual bool SendOrder(MsgOrderInfo *msg) = 0;
	virtual void SetContractNo(const char* sCode, CodeSetType csType = cstChange) = 0;
	virtual void SetPriceData(const char* sdata) = 0;
	virtual bool SetHeadData(const wchar_t* sPType, bool bHead = true) = 0;
	virtual bool SetSelect(const wchar_t* sStrategy,int nRef) = 0;
	virtual bool TableButtonToEdit() = 0;
	virtual bool CxGetWindowText(MH_TYPE m_type,int& nSize, wchar_t* wstr, int nMaxLen = LEN_RET) = 0;
};

class IOrderConfig
{
public:
    virtual void OnLadderStyleChanged(int style) = 0;
};