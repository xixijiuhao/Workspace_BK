#ifndef _TCONORDER_MODEL_H_
#define _TCONORDER_MODEL_H_

class TConOrderModel
{
public:
	TConOrderModel();
	~TConOrderModel();

	bool InsertConOrder(TConditionOrder* pOrder);
	void OnQuote(const SContract* quote);
	void OnTimer();
	void OnOrder(const TOrder* pOrder);

	void * GetConOrder(){ return &m_vecCondtionOrder; }
	void*  GetTrigConOrder(){ return &m_vecTriggeredConOrder; }

	void OnConMenuClick(TMenuInfo* pMenuInfo);
	void OnUserChange();
	bool CanUnsubQuote(const char* pszContractId);

	void ModifyPreAutoOrder(TConditionOrder* pOrder);

	void SaveOrder(); //保存本地监控的
	void ReadOrder();//读取本地监控和未成交的条件单
private:
	bool CheckPrice(TConditionOrder* pOrder, double dPrice);
	bool SendOrder(TConditionOrder* pOrder);
	void DealData();
	bool DealData(TConditionOrder* pOrder, bool &bErase);
	void DeleteAll(bool bShowTrig);
	void DeleteOne(TConditionOrder*pOrder, bool bShowTrig);

	void CancelOrder(TConditionOrder *pOrder);
	void SendAllPreOrder();

	void SendStopLPOrder(TConditionOrder*pOrder, const TOrder* pOrderBack);
	void CheckNeedStopLP(const TOrder* pOrder);

	void GetOrderFromParOrder(const TOrder* pParOrder);

	void GetOrderFromOrder(const TOrder * pOrde);

	TConditionOrder * DealTradeOrder(const TOrder * pOrder);

	bool OnParOrder(const TOrder* pParOrde);
private:
	vector<TConditionOrder*> m_vecCondtionOrder;
	vector<TConditionOrder*> m_vecTriggeredConOrder;
	int						m_iOrderId;
	multimap<string, TConditionOrder*>  m_mapContractId;
};

#endif