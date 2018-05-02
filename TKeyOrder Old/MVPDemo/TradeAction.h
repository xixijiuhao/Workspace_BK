#ifndef _T_TRADE_ACTION_H_
#define _T_TRADE_ACTION_H_

class TradeAction:public TIxFrm,public ISQuoteDataSpi,public TradeSpiEx
{
public:
	TradeAction();
	~TradeAction();
public:
	void InsertChaseOrder(const TOrder* pOrder);
	int	CheckOrder(TSendOrder & stOrder);
	int	InsertCloseOrder(TSendOrder & stOrder, vector<uint>*pOrderID = nullptr);
	void InsertCloseOrderNoCheck(TSendOrder & stOrder, vector<uint>*pOrderID = nullptr);
	void InsertBackOrder(const TPosition* pPosition, int priceType);
	void InsertShiftOrder(TSendOrder &stOrder, const char * pszContractNo);

	bool CoverAll(IDataContainer*pConParOrder, IDataContainer*pConPosition);

	bool Cover(TSendOrder &stOrder,bool bShowMsg =false, vector<uint>*pVecOrderID =nullptr); //默认不报错
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnTimer();
	void OnCreate();
	void OnBackOrderQuote(const SContractNoType quoteID);
private:
	void InsertCoverOrder(TSendOrder & stOrder, vector<uint>*pOrderID);
private:
	virtual void __cdecl OnOrder(const TOrder * pOrder);
private:
	void OnCancelChaseOrder(const TOrder* pOrder);
	double GetPrice(const TOrder* pOrder, TChaseOrder *pChase);
	uint GetPriceOffset(const TOrder* pOrder, double dOrderPrice);
private:
	void OnFillBackOrder(const TOrder *pOrder);
private:
	void OnFillShiftOrder(const TOrder*pOrder);
private:
	void BigOrderForClose(TSendOrder& stOrder,vector<uint>*pOrderID);
	void InsertOrder(TSendOrder & stOrder, vector<uint>*pOrderID);
private:
	void __stdcall OnQuote(const SContract* cont, const SQuoteUpdate* quote);
	void __stdcall OnSpreadQuote(const SSpreadContract* scont) {};
	void SubQuoteAndInsert(TSendOrder &stOrder);
private:
	void CoverAllPosition(IDataContainer* pConPosition);
	void OnCancelCoverOrder(const TOrder* pOrder);
private:
	map<uint, TChaseOrder>  m_mapChaseOrder;
	map<string, TBackOrder> m_mapBackOrder;
	map<uint, string>		m_mapShiftOrder;
	map<string, uint>       m_mapSubQuote;
	multimap<string, TSendOrder> m_mapBackOrderQuote; //名字起错了似乎，用于没有get到price的合约，订阅行情，行情回调中发单
	IDataContainer *        m_pConPosition;
	set<const TOrder*>m_setCoverAllParOrder;
	stChasePrice *          m_pstChaseConfig;
};

#endif