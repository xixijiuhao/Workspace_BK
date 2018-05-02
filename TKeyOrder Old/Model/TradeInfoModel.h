#ifndef _T_TRADEINFO_MODEL_H_
#define _T_TRADEINFO_MODEL_H_

class TradeInfoModel :public IStopLPListener
{
public:
	TradeInfoModel();
	~TradeInfoModel();

	void * GetParOrderCon(){ return &m_conParOrder; };
	void * GetPositionTotal(){ return &m_conTotalPosition; }
    void * GetOrder(){ return &m_conOrder; }
    void * GetMatch(){ return &m_conMatch; }
    void * GetLog(){ return &m_stLogData; }
	void * GetStopLPOrder(){ return &m_conStopLPOrder; }

	const TPosition*GetPosition(int index);

	void SetParOrder();
	bool SetPosition(const TPosition* pPosition=nullptr);
    void SetOrder();
    void SetMatch();
	void SetStopLPOrder(bool NOtrig = true);
    void SetContractNo(string strCode);
	void SetLogInfo();
	void OnUserChange();  //选择的用户更换之后，更新数据

	void OnClickBtn(WPARAM wParam, LPARAM lParam);
	void SetOrderStateClickIndex(int index);
	uint GetSelectParOrderID(){ return m_iParOrderID; }
	void OnCommand(TMenuInfo* pMenuInfo);
	void SetPresenter(IPresenterCallBack *p){ m_pPresenter = p; }

	void Cover(UINT priceType);

	void UpdateStopLP(){ SetStopLPOrder(m_StopLPShowNotTrigger); };
	//设置当前选中持仓
	void SetCurrentPosition(const TPosition * pPosition) { m_pNowSelectPosition = pPosition; }
private:
	void ModifyOrderPrice(UINT type,const TOrder* pOrder=nullptr);
	void Cover(const TPosition* pPosition, uint iQty);
private:
	bool CheckPosition(const void* pPosition);
	bool CheckParOrder(const void* pOrder);
	bool CheckStopLP(const void* pStopLP);
	void LockPosition(const TPosition* pPosition);
	void CoverSHFELastPosition(const TPosition* pPosition);

	bool GetCmbContract(const TPosition* pPosition,TContractKey& stContractKey, TDirect& direct);//平组合单
	void SetCmbContract(const TPosition* pPosition);
	bool IsAddOrUpdate(uint iQty, int index);
private:
	TDataContainer m_conTotalPosition;
	TDataContainer m_conOrder;
	TDataContainer m_conParOrder;
	TDataContainer m_conMatch;
	TDataContainer m_conMoney;
	TDataContainer m_conStopLPOrder;

	vector<uint>   m_vecPositionQty;
    TLogData       m_stLogData;
    TContractKey   m_stContract;
	uint		   m_iParOrderID;
	IPresenterCallBack * m_pPresenter;
	const TPosition*  m_pNowSelectPosition; //当前选中的Position

	bool m_StopLPShowNotTrigger = true;//止损显示未触发列表
public:
	enum
	{
		btnFromOrderView,
		btnFromTradeInfo,
	};

public:
	virtual void __cdecl Update(DataType type, const TStopLPitem*pdata);
};

#endif