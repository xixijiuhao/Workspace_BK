#pragma once
#include "TradeApi_API.h"


extern ITradeData * g_pTradeData;

class TraderSpi : public ITradeSpi
{
private:
	bool m_bAutoReg;
	bool m_bRegisted;
public:
	//AutoReg:是否在创建时自动注册
	TraderSpi(bool AutoReg = true) : m_bAutoReg(AutoReg), m_bRegisted(false)
	{
		if (m_bAutoReg && g_pTradeData)
			RegistSpi();
	};
	virtual ~TraderSpi()
	{
		if (m_bAutoReg && g_pTradeData)
			UnRegistSpi();
	};
	void RegistSpi()
	{
		if (m_bRegisted || !g_pTradeData)
			return;
		m_bRegisted = true;
		g_pTradeData->Register(this);
	};
	void UnRegistSpi()
	{
		if (m_bRegisted&&g_pTradeData)
			g_pTradeData->UnRegister(this);
	}
public:	
	virtual bool __cdecl IsWnd(uint & WinHwnd){return false;};										
	virtual void __cdecl OnConnect(const TLogin * pLogin){};
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin){};			
	virtual void __cdecl OnLoadTest(const TConnect * pConnect){};						
	virtual void __cdecl OnLogin(const int ErrorCode, const TLogin * pLogin){};				
	virtual void __cdecl OnLoginInfo(const TLogin * pLogin){};							
	virtual void __cdecl OnLogout(const int ErrorCode, const TLogin * pLogin){};				
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase){};
	virtual void __cdecl OnOrder(const TOrder * pOrder){};								
	virtual void __cdecl OnMatch(const TMatch * pMatch){};								
	virtual void __cdecl OnPosition(const TPosition * pPosition){};
	virtual void __cdecl OnPositionSub(const TSubPosition * pPosition){};
	virtual void __cdecl OnPositionTotal(const TTotalPosition * pPosition){};
	virtual void __cdecl OnLiquidate(const TLiquidate * pLiquidate){};					
	virtual void __cdecl OnMoney(const TMoney * pMoney){};								
	virtual void __cdecl OnMsg(const TMessage * pMsg){};								
	virtual void __cdecl OnChgPsw(const int ErrorCode, const TLoginBase * pLoginBase){};		
	virtual void __cdecl OnBill(const TBill * pBill){};			
	virtual void __cdecl OnHisDelivery(const TDelivery * pDelivery){};
	virtual void __cdecl OnHisOrder(const TOrder * pHisOrder){};
	virtual void __cdecl OnHisOrderProc(const TOrder * pHisOrderProc){};
	virtual void __cdecl OnHisMatch(const TMatch * pHisMatch){};
	virtual void __cdecl OnHisCash(const TCash * pHisCash){};
	virtual void __cdecl OnEnquiry(const TEnquiry * pEnquiry){};
	virtual void __cdecl OnUserProxy(const TUserProxy * pUserProxy){};
	virtual void __cdecl OnPreFrozen(const TPreFreeze * pPreFreeze){};	
	virtual void __cdecl OnTradableQty(const TTradableQty * pTradableQty){};	
	virtual void __cdecl OnErrorInfo(const TErrorInfo * pErrorInfo){};
	virtual void __cdecl OnRefAllData(const TradeNotifyType DataType){};
	virtual void __cdecl OnBankBalance(const TBankBalance * pBankBalance){};
	virtual void __cdecl OnTransfer(const TTransfer * pTransfer){};
	virtual void __cdecl OnCfmmcToken(const TCfmmcToken * pCffmmcToken){};
	virtual void __cdecl OnHisPosition(const TPosition * pPosition){};
	virtual void __cdecl OnFutureBalance(const TFutureBalance * pFutureBalance){};
	virtual void __cdecl OnSHFEDeepQuote(const TDeepQuote * pDeepQuote){};
	virtual void __cdecl OnExchangeState(const TExchangeState * pExchangeState){};
	virtual void __cdecl OnOther(const uchar DataType, const void * pData){};
    virtual void __cdecl OnMatchSub(const TMatch * pMatch) {};
    virtual void __cdecl OnMatchTotal(const TMatch * pMatch) {};
};