#pragma once
#pragma comment(lib, "MBLdll.lib")
#include <mutex>

using namespace std;

class IQuoteLogin
{
public:
	virtual void QuoteLogin(MBLErrorInfo errprInfo) = 0;
};

class CMBLSpiHandler : public MBLSpi
{
public:
	CMBLSpiHandler();
	~CMBLSpiHandler() {}
//public method
public:
	static CMBLSpiHandler * GetMBLQuoteData();
	void Login(const char * userName, const char * pwd, string &scontractType);
	void SubSHFEDeepQuote(TContractKey & key);
//quote call back

public:
	virtual void OnMBLConnected();
	virtual void OnMBLRspUserLogin(MBLErrorInfo ErrorInfo);
	virtual void OnMBLRspSubcribe(MBLErrorInfo ErrorInfo);
	virtual void OnMBLRspMarketData(MBLData *pMBLMarketData);

	vector<TDeepQuote> * GetMBLData(TDirect  direct);
	//const char * GetPriceFormat();
	int GetTotalMBLQty(TDirect direct, int index);
	void SetQuoteUpdate(IQuoteUpdate * update) { m_pQuoteUpdate = update; };
	void SetQuoteLogin(IQuoteLogin * login) { m_pIQuoteLogin = login; };
	void GetUserInfo();
	const char * GetPriceFormat();
	//const char * GetSign() { return m_szSign; };

	void GetQuoteData(vector<TDeepQuote> & vecBuyData, vector<TDeepQuote> & vecSellData);
	void ClearQuoteData();

private:
	void TransMBLDataToDeepQuote(MBLData *pMBLMarketData, TDeepQuote & tDeepQuote);
	void SetPricePrec(TContractKey & key);
	void AddData(TDeepQuote * quote);

	vector<TDeepQuote> m_vecMBLBuy;
	vector<TDeepQuote> m_vecMBLSell;
	IQuoteUpdate*   m_pQuoteUpdate;
	IQuoteLogin*    m_pIQuoteLogin;
private:
	MBLUserData     m_stUserData;
	//char           m_szSign[21];
	unsigned int   m_iPricePrec;

	mutable std::mutex m_Mutex;

public:
	string QuoteTypes;
};