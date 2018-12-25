#ifndef T_ADD_MOD_STOPLP_DATA_H
#define T_ADD_MOD_STOPLP_DATA_H



//止损止盈新建下单以及修改界面的数据类
//需要获取合约的最小变动价，更新最新价消息
class TAddModStopLPData :public ISQuoteDataSpi,public IStopLPListener
{
public:
	TAddModStopLPData();
	~TAddModStopLPData();
	virtual void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	virtual void __stdcall OnSpreadQuote(const SSpreadContract* scont) {}
	//套利行情数据
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
	TDataContainer m_ComBoxData;//所有该合约止损单子

	IPresenterCallBack * m_pPresenter;
};


#endif