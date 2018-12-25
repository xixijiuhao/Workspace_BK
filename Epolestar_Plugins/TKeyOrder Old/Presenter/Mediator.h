#ifndef T_MEDIATOR_H
#define T_MEDIATOR_H





class TMediator :public IMediator
{
public:
	void SetThreeKeyView(TViewThreeKey* colleagueView)
	{
		m_TreeKeyView = colleagueView;
	}
	void SetColleaguePre(TPresenter* colleaguePre)
	{
		m_colleaguePresenter = colleaguePre;
	}
	void SetConfigView(TConfigFrm *pView)
	{
		m_ConfigView = pView;
		m_ConfigView->SetShowRect(m_TreeKeyView->GetShowRect());
	}
	void SetOrderView(TOrderView *pOrderView)
	{
		m_OrderView = pOrderView;
		m_OrderView->SetShowRect(m_TreeKeyView->GetShowRect());
	}
	void SetLogView(TLogView *pView)
	{
		m_LogView = pView;
		m_LogView->SetShowRect(m_TreeKeyView->GetShowRect());
		m_pOpenStopView = nullptr;
		m_pConditionOrderView = nullptr;
	}
	void SetTradeView(TTradeInfo *pView)
	{
		m_TradeView = pView;
		m_TradeView->SetShowRect(m_TreeKeyView->GetShowRect());
	}
	void SetStopLPView(TStopLPView *pView)
	{
		m_StopLPView = pView;
	}
public:
	virtual void Send(unsigned int CommandID, const void* content, IColleague* colleague);
	virtual void Get(unsigned int CommandID,void *& pVoid, IColleague *colleague);
private:
	TViewThreeKey* m_TreeKeyView;
	TConfigFrm *   m_ConfigView;
	TOrderView*    m_OrderView;
	TLogView*      m_LogView;
	TTradeInfo*    m_TradeView;
	TStopLPView*	m_StopLPView;

	TOpenStopFrm*  m_pOpenStopView;   //止损开仓
	TConditionOrderFrm * m_pConditionOrderView;
	//三键那块的界面View
	//TViewOrderBtn* m_OrderBtnView;

	//三键下方的那块界面View
	//TViewLogAndBtn * m_LogAndBtnView;

	//右边列表View
	//TViewTradeInfoList* m_TradeInfoListView;


	TPresenter* m_colleaguePresenter;



private:
	void DealPresenter(unsigned int CommandID, const void* content);
	void DealThreeKey(unsigned int CommandID, const void* content);
    void DealLog(unsigned int CommandID, const void* content);
	void DealOrderView(unsigned int CommandID, const void* content);
	void DealTradeView(unsigned int CommandID, const void* content);
	void DealStopLPView(unsigned int CommandID, const void* content);
	void DealConView(unsigned int CommandID, const void* content);
private:
    void DealLogGet(unsigned int CommandID, void*& content);
    void DealTradeGet(unsigned int CommandID, void*& content);
	void DealOrderGet(unsigned int CommandID, void*& content);
	void DealTViewGet(unsigned int CommandID, void*&content);
	void DealTStopLPGet(unsigned int CommandID, void*&content);

    
};



#endif