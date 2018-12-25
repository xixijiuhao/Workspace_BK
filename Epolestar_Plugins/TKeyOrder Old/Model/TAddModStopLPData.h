#ifndef T_ADD_MOD_STOPLP_DATA_H
#define T_ADD_MOD_STOPLP_DATA_H



//ֹ��ֹӯ�½��µ��Լ��޸Ľ����������
//��Ҫ��ȡ��Լ����С�䶯�ۣ��������¼���Ϣ
class TAddModStopLPData :public ISQuoteDataSpi,public IStopLPListener
{
public:
	TAddModStopLPData();
	~TAddModStopLPData();
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {}
	//������������
	virtual void __cdecl Update(DataType type, const TStopLPitem*pdata);
public:
	void RefreshContract(const TPosition* Tkey);
	void UnSubQuote();
	void * GetViewData(){ return &m_Data; }
	void * GetComboxDataContainer() {return &m_ComBoxData;}
	void RefreshComBoxContainer();
	void UpdatePositionPrice();

	void DealModifyOrder(const TStopLPitem* TStop);
	void DealInsertOrder(const TStopLPInsertItem* TStop);

	bool CheckOrder(const TStopLPInsertItem* TStop);

	void SetPresenter(IPresenterCallBack *p){ m_pPresenter = p; }
private:
	TStopLPViewData m_Data;
	TDataContainer m_ComBoxData;//���иú�Լֹ����

	IPresenterCallBack * m_pPresenter;
};


#endif