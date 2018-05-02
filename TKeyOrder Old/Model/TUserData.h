#ifndef _T_USER_DATA_H_
#define _T_USER_DATA_H_

class TUserData
{
public:
	TUserData(ISQuoteDataSpi *pQuoteSpi);
	~TUserData();
public:
	bool SplitNotice(string strContent, UINT type,string &strContractID);
	bool DealContractNo(string & strCode , bool bGetPositionQty =false, TDirect direct = 0); //内部默认
	TOrderViewData * GetOrderShowData(){ return &m_stOrderData; }
	TQuoteData * GetQuoteData(){ return &m_stQuoteData; }
	void InsertOrder(TSendOrder *pOrder);
	void DealQuoteData(SContractNoType contractID);
	bool OnQuote(const SContract* quote = nullptr);
	void * GetCommInfo(){ return &m_stCommodityInfo; }
	void SetLockState(bool bLock){ m_bLock = bLock; }
	string GetUserNo(){ return m_strUserNo; }
	void GetOrderShowInfo(TContractKey &key);
	void UpdateOrderData(bool bGetPositionQty =true);

	void SetContractNo(const char* szUserNo, TDirect direct);
	void SetPresenter(IPresenterCallBack *p){ m_pPresenter = p; }

	void UpdateConfig();  //默认手数、超价参数等配置	

	void OnUserChange();
	//获取当前合约对应的持仓
	const TPosition * GetPosition();
private:
	void SubQuote(TContractKey &key);
	int  GetOrderQty(TContractKey &key);
	void SetContractNoText(TContractKey& key);

	void SetInnerOrderBtnText(TContractKey &key, uint iLongCount, uint iShortCount);
	void SetForeignOrderBtnText(uint iLongCount, uint iShortCount);

	bool OrderConfirm(const TSendOrder &stOrder);
	void InsertOrder(TSendOrder & stOrder);
	void BigOrderSplitForOpen(TSendOrder& stOrder);
	void SetAutoOpenStopLP(TSendOrder& stOrder);

	void SetConfig();
private:
	bool				m_bLock;
	string				m_strCode;
	SOFCommodityInf		m_stCommodityInfo;
	SContractNoType		m_strQuoteContractId;
	TContractKey        m_stContractKey;
	ISQuoteDataSpi *		m_pQuoteSpi;
	IPresenterCallBack* m_pPresenter;
	bool				m_bGetPositionQty;
private:
	TOrderViewData      m_stOrderData;
	TQuoteData          m_stQuoteData;
	TDirect             m_DirectFromNotice;
	string				m_strUserNo;
};

#endif