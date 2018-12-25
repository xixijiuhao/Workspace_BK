#pragma once

class IQuoteUpdate
{
public:
	virtual void Update() = 0;
};

class TDeepQuoteData :public TradeSpiEx
{
public:
	TDeepQuoteData();
	void SetQuoteUpdate(IQuoteUpdate * update) { m_pQuoteUpdate = update; };
	vector<TDeepQuote> * GetData(TDirect  direct);
	void GetUserInfo();
	void QrySHFEDeepQuote(TContractKey & key);
	const char * GetPriceFormat();
	int GetTotalQty(TDirect direct, int index);
	const char * GetSign() { return m_szSign; };
public:
	virtual void __cdecl OnSHFEDeepQuote(const TDeepQuote * pDeepQuote);
	virtual void __cdecl OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase);
private:
	vector<TDeepQuote> m_vecBuy;
	vector<TDeepQuote> m_vecSell;
	bool		   m_bNewData;
private:
	IQuoteUpdate * m_pQuoteUpdate;
	char           m_szSign[21];
	const SCommodity * m_pCommodity;
};