#ifndef T_PRESENTER_H
#define T_PRESENTER_H

class TUserData;
class TPresenter :public IColleague, public TradeSpiEx, public TIxFrm, public ISQuoteDataSpi, public IPresenterCallBack
{
public:
	TPresenter(IMediator* mediator);
public:
	static void OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source);
	static void OnReceiveNotice(HWND From, HWND To, const char* sender, const char* action, const char* content);
	static void __cdecl OnMenuClk(const unsigned int MenuIndex, const HWND PastLife);

	bool ShowMsgBox(int type, char *pszReason = NULL);
	void UpdateConList();
	static void Show(int iShow =0); //0,忽略该参数，1，显示，2隐藏

private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnNotice(WPARAM wParam);
	void OnQuoteBack(SContract* quote);
	void OnResetUserNo(bool bNeedSetUser = false);
	void OnOrderBack(const TOrder* pOrder );
	void OnMatchBack(const TMatch* pMatch);

public:
	//取数据数据 OrderView Bind Data
	void* GetQuoteData(){ return m_clUserData.GetQuoteData(); };
	void* GetOrderData(){ return m_clUserData.GetOrderShowData(); }
	void * GetCommInfo(){ return m_clUserData.GetCommInfo(); }
	//TKeyView Bind Data
	void * GetUserComboxData(){ return m_clTKeyData.GetComboxContent(); }
	void * GetCurrencyNo(){ return m_clTKeyData.GetCurrencyNo(); }
	void * GetMoney();
	//TradeInfoView Bind Data
	void * GetParOrder(){ return m_clTradeInfoData.GetParOrderCon(); }
	void * GetPositionTotal(){ return m_clTradeInfoData.GetPositionTotal(); }
	void * GetOrder(){ return m_clTradeInfoData.GetOrder(); }
	void * GetMatch(){ return m_clTradeInfoData.GetMatch(); }
	void * GetStopLPOrder(){ return m_clTradeInfoData.GetStopLPOrder(); }
	//LogView
	void * GetLog(){ return m_clTradeInfoData.GetLog(); }

	//处理数据 TOrderView
	void DealContractCode(string sCode, TDirect direct = dNone);
	void SetLockState(bool bLock){ m_clUserData.SetLockState(bLock); };
	void OnClickBtn(WPARAM wParam, LPARAM lParam);
	//TKeyView
	void UserChange(int index);
	void UpdateConfig();
	//TradeInfoView
	void SetContract(int index);
	void SetOrderStateClcikIndex(int index){ m_clTradeInfoData.SetOrderStateClickIndex(index); }
	void OnCommand(TMenuInfo * pMenuInfo){ m_clTradeInfoData.OnCommand(pMenuInfo); };
	void CoverAll();
	void UpdateMoney();
	void CoverOne(int index);
	void UpdateStopLP(){ m_clTradeInfoData.UpdateStopLP(); };
	//ConView
	bool InsertConOrder(TConditionOrder* pOrder);
	void OnConMenuClick(TMenuInfo * pMenuInfo){ m_clConOrderData.OnConMenuClick(pMenuInfo); };
	void* GetConOrder(){ return m_clConOrderData.GetConOrder(); };
	void* GetTrigConOrder(){ return m_clConOrderData.GetTrigConOrder(); };
	bool CanUnsubQuote(const char* pszContractID){ return m_clConOrderData.CanUnsubQuote(pszContractID); }
	void ModifyPreAutoOrder(TConditionOrder* pOrder){ m_clConOrderData.ModifyPreAutoOrder(pOrder); };
	//TKeyView
	HWND GetTKeyHwnd(){ return m_hTKeyOrder; }
	//发送消息
	void Send(unsigned int CommandID, const void* content);
	void Init();
	//交易回调
	virtual void __cdecl OnOrder(const TOrder * pOrder);
	virtual void __cdecl OnMatch(const TMatch * pMatch);
	virtual void __cdecl OnMoney(const TMoney * pMoney);
	virtual void __cdecl OnPositionTotal(const TTotalPosition * pPosition);
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);
	virtual void __cdecl OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin);
	//StopLPView related func
	void InsertStopLPOrder(TStopLPInsertItem* TStop);
	void* GetStopLPData(){ return m_clStopLPData.GetViewData(); }
	void* GetStopLPComboxData(){ return m_clStopLPData.GetComboxDataContainer();}
	void UpdatePositionPrice();
	void DeleteStopLPOrder(const unsigned int* content);
	void RClkMenuStopLPOrder(const void* content, int SelPageOrKey, bool Mod = false);
	void ModifyStopLPOrder(const TStopLPitem* TStop);
private:
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {}
public:
	void InsertOrder(TSendOrder * pOrder);
	void OnUserChange();
	void OnTotalPosition(const TPosition* pPosition);

	void UpdatePrice();

	//2017-3-13  11:03 xzc
	void ShowDetail();
private:
	string        m_strNoticeContent;
	TUserData     m_clUserData;
	TThreeKeyData m_clTKeyData;
	TradeInfoModel m_clTradeInfoData;
	TConOrderModel m_clConOrderData;     //条件单数据类
	HWND		  m_hTKeyOrder;
	TAddModStopLPData m_clStopLPData;

	bool          m_bCanShowDetail;
	int			  m_iShow;
public:
	enum
	{
		CMD_Presenter_ShowHide,
		CMD_Presenter_Notice_OrderView,

        CMD_Presenter_OnQuote,
		CMD_Presenter_UserLogin,
		CMD_Presenter_OnMoney,

        CMD_Presenter_OnParOrder,
        CMD_Presenter_OnOrder,

		CMD_Presenter_OnPositionTotal,
        CMD_Presenter_OnMatch,

		CMD_Presenter_SetStopLPView,
		CMD_Presenter_SetStopLPView_FindKeyID,
		CMD_Presenter_ComboxUpdate,
	    CMD_Presenter_OnStopLPOrderList,
		CMD_Presenter_UpdateTViewMoney,
		CMD_Presenter_UpdateOneConOrder,
		CMD_Presenter_UpdateConOrder,
		CMD_Presenter_TViewUpdate,

		CMD_Presenter_Show,
		CMD_Presenter_Hide,

		CMD_Presenter_UpdateDeposit,
		CMD_Presenter_SetSelectedPosition,
	};
	enum
	{
		fromQuote = 1,
		fromTradeD,
	};

	enum
	{
		Msg_OrderQty_Zero,
	};
};

#endif