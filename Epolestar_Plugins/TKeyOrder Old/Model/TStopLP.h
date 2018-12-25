#ifndef T_STOP_LOSS_PROFIT_H
#define T_STOP_LOSS_PROFIT_H

#include "TStopLPEx.h"
#include "PreInclude.h"
//*******************************************************
//止损模块，行情内部监控，止损自动进行，有数据回调
//*********************************************************
class AbstractOrder;
class TStopLP :public TIxFrm, public TradeSpiEx, public IStopLP,public ISQuoteDataSpi
{
public:
	TStopLP();
	~TStopLP();
public:
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {}

	virtual void __cdecl Attach(IStopLPListener *pOb);
	//取消订阅
	virtual void __cdecl Detach(IStopLPListener *pOb);
	//下止损单
	virtual unsigned int __cdecl InsertLPOrder(const TStopLPInsertItem *item);
	//删除止损单
	virtual void __cdecl DeleteLPOrder(const unsigned int KeyID);
	//修改止损单
	virtual unsigned int __cdecl ModifyLPOrder(const TStopLPitem *item);
	//触发止损单
	//触发计数加1(参数Trigger为true），或者触发计数清零
	virtual void __cdecl TriggerLPOrder(const unsigned int KeyID, bool Trigger);
	//开仓止损单
	virtual unsigned int __cdecl OpenLPOrder(const AutoStopInfo *item, const TSendOrder * stSendOrder);
	//删除所有止损单
	virtual void __cdecl DeleteAllLPOrder();

	//获得指定客户所有止损单
	//输出参数：
	// Datas：止损单的容器信息，其数据元素类型为 const TStopLPitem*
	//输入参数：
	// UserNo：客户编号，为""或nullptr时不区分客户
	// Scode: 合约代码, 为""或nullptr时不区分合约
	// Direct: 方向，为dBoth时不区分买卖方向，dBuy为买，dSell为卖
	// NoTrig：未触发，为正数时返回未触发，为负数时返回已触发,为0时不区分
	virtual void __cdecl GetALLStopLPOrder(IDataContainer &Datas, const char * UserNo = "", const char* SCode = "", TDirect direct = dBoth, int NoTrig = 0);

	//获得指定keyID的止损单
	//输出参数：
	//pItem：止损单指针，若没找到相应止损单则返回为空指针nullptr
	//输入参数：
	//KeyID：关键字
	virtual void __cdecl GetSingleStopLPOrder(const TStopLPitem* pItem, const unsigned int KeyID);

protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnDataChange(DataType type, TStopLPitem*pdata);

private:
	void DealQuote(LPARAM lParam);
	int  DealAttach(WPARAM wParam,LPARAM lParam);
	void DealInsertOrder(WPARAM wParam, LPARAM lParam);
	void DealDeleteOrder(WPARAM wParam, LPARAM lParam);
	void DealModifyOrder(WPARAM wParam, LPARAM lParam);
	void DealTrigOrder(WPARAM wParam, LPARAM lParam);
	void DealOnOrderCallBack(WPARAM wParam);
	void DealOnLiquidateCallBack(WPARAM wParam);

	bool DealSingleOrder(char(&contractID)[101], unsigned int i);//处理单个止损单
	void SingleOrderQtyandParOrder(TStopLPitem& item, TContractKey& ContractKey);//持仓量以及挂单量判断
	void SendOrder(const TStopLPitem &item);//实时发单，持仓不足平的，将剩余仓位平掉
	bool InsertLinkedKeyIDs(unsigned int ID1, unsigned int ID2);//关联的止损单处理
	void DealLinkedKeyIDs(unsigned int keyID);//关联的止损单处理
private:
	void AddSub(char(&contractID)[101], unsigned int i);
	void DelSub(char(&contractID)[101], unsigned int i);	//删除某条监视
	void DelSub();										//删除所有监视
private:
	//交易回调
	virtual void __cdecl OnLiquidate(const TLiquidate * pLiquidate);
	virtual void __cdecl OnOrder(const TOrder * pOrder);
	void GetContractKey(char* ContractKey, const TContractSingleKey* pst, int iSize);
	void GetPositionKey(char* PositionKey, TContractSingleKey* pContract, TDirect direct, int iSize);
	void GetPositionKey(char* PositionKey, const char* scode, TDirect direct, int iSize);
	bool IsStopOpen(const AutoStopInfo* stInfo);
	bool IsAutoStop(char* ContractKey);
	void InsertRemarkToStopInfo(char* cRemark,const AutoStopInfo* stStopOrder);
	bool DeleteRemarkToStopInfo(const char* cRemark);
	//收到回调发止损单
	void InsertOnOrderCallBackOrder(AutoStopInfo& stStop, const TOrder* pst);
	void SendAutoStop(AutoStopInfo& stStop, const TOrder* pst, TStopLPInsertItem& stStopOrder);
	void SendOpenStop(AutoStopInfo& stStop, const TOrder* pst, TStopLPInsertItem& stStopOrder);

	unsigned int AutoStopLoss(TStopLPInsertItem &item, const double &stopLoss, const double &basePrice = 0);
	unsigned int AutoStopProfit(TStopLPInsertItem &item, const double &stopProfit, const double &basePrice = 0);
	unsigned int AutoFloat(TStopLPInsertItem &item, const double &floatPrice, const double &basePrice = 0);
	unsigned int AutoBreakEven(TStopLPInsertItem &item, const double &BreakEvenPrice, const double &basePrice);

	void DefaultStopOrder(const TOrder* pst, const AutoStopInfo& stStop, TStopLPInsertItem& stStopOrder);
	void GetLossProfitPrice(char* ContractKey, double& dLoss, double& dProfit);


	void InsertPositionToKeyId(char* PositionKey, unsigned int KeyId);
	void GetDelKeyId(TLiquidate* pst, vector<unsigned int>& vecKeyId);

private:
	//数据存储
	std::vector<AbstractOrder*>	m_Data;
	static unsigned int staticKeyID;

	//关联止损单KeyID的map
	std::map<unsigned int, unsigned int> m_stlLinkedKeyIDs;

	//行情监视multimap，合约ID对应Item的KeyID
	std::multimap<std::string, unsigned int> m_quoteData;//ContractID->KeyID
	int m_iPositionQty;									//当前处理止损单合约的持仓量
	int m_iOrderQty;									//当前处理止损单挂单量
	int m_iDayPositionQty;
	int m_iDayOrderQty;
	SQuoteUpdate m_strRawQuoteData;
	SContract    m_quoteContract;
	double m_dLastPrice;								//当前处理合约的最新价

	//注册回调容器
	std::set<IStopLPListener*> m_setpObs;
	
	//标记发开仓自动止损单的单子和止损信息的map
	std::map<std::string, AutoStopInfo> m_RemarkToStopInfo;
	//标记所有止损单和持仓之间的联系的multimap
	std::multimap<string, unsigned int> m_PositionToKeyId;
};

//************************************************************************************
//止损单类，用了策略模式，这几个类主要是止损判断的函数，以后如果有其他用处还可以拓展
//***********************************************************************************
class AbstractOrder
{
public:
	AbstractOrder(const TStopLPitem *i){ memcpy_s(&m_order, sizeof(TStopLPitem), i, sizeof(TStopLPitem)); }
	virtual bool TriggerJudge(const double &Price) = 0;
	TStopLPitem* GetOrder(){ return &m_order; }
	unsigned int GetKeyID(){ return m_order.keyID; };
protected:
	TStopLPitem m_order;
};

class TStopLossOrder :public AbstractOrder
{
public:
	TStopLossOrder(const TStopLPitem* i) :AbstractOrder(i){};
	virtual bool TriggerJudge(const double &Price);
};

class TStopProfitOrder :public AbstractOrder
{
public:
	TStopProfitOrder(const TStopLPitem* i) :AbstractOrder(i){};
	virtual bool TriggerJudge(const double &Price);
};

class TFloatStopLOrder :public AbstractOrder
{
public:
	TFloatStopLOrder(const TStopLPitem* i) :AbstractOrder(i){};
	virtual bool TriggerJudge(const double &Price);
};

class TBreakEvenOrder :public AbstractOrder
{
public:
	TBreakEvenOrder(const TStopLPitem* i) :AbstractOrder(i){};
	virtual bool TriggerJudge(const double &Price);
};
#endif