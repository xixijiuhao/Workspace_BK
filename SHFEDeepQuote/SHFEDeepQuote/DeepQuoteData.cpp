#include "PreInclude.h"


inline void toLower(char * src)
{
	char *p = src;
	while (*p) 
	{
		if (*p >= 'A' && *p <= 'Z') 
			*p += 32;  
		p++; 
	}
}

TDeepQuoteData::TDeepQuoteData()
{
	m_bNewData = true;
	m_pQuoteUpdate = NULL;
	m_pCommodity = NULL;
}

vector<TDeepQuote> * TDeepQuoteData::GetData(TDirect direct)
{
	if (direct == dBuy)
	{
		return &m_vecBuy;
	}
	else if (direct == dSell)
	{
		return &m_vecSell;
	}
	return nullptr;
}

void TDeepQuoteData::OnSHFEDeepQuote(const TDeepQuote * pDeepQuote)
{
	if (m_bNewData)
	{
		m_vecSell.clear();
		m_vecBuy.clear();
		m_bNewData = false;
	}
	TDeepQuote stQuote = {};
	memcpy(&stQuote, pDeepQuote, sizeof(TDeepQuote));
	if (pDeepQuote->Direct == dBuy)
	{
		m_vecBuy.push_back(stQuote);
	}
	
	else if (pDeepQuote->Direct == dSell)
	{
		m_vecSell.push_back(stQuote);
	}

	if (pDeepQuote->IsLast != 'N')
	{
		if (m_pQuoteUpdate)
		{
			m_pQuoteUpdate->Update();
		}
		m_bNewData = true;
	}
}

void TDeepQuoteData::GetUserInfo()
{
	/*TDataContainer users;
	g_pTradeData->GetAllUser(users);
	uint count = users.size();
	if (users.size())
	{
		const TUserInfo*pUser = (const TUserInfo*)users.at(0);
		strcpy_s(m_szSign, pUser->Sign);
	}*/
	TDataContainer loginUsers;
	g_pTradeData->GetAllLogin(loginUsers);
	uint loginCount = loginUsers.size();
	if (loginUsers.size())
	{
		const TLogin *pLogin = (const TLogin*)loginUsers.at(0);
		strcpy_s(m_szSign, pLogin->Sign);
	}
}

void TDeepQuoteData::QrySHFEDeepQuote(TContractKey & key)
{
	strcpy_s(key.Sign, m_szSign);
	m_pCommodity = g_pTradeData->GetCommodity(&key);

	g_pTradeApi->QrySHFEDeepQuote(key);
}

void TDeepQuoteData::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
	GetUserInfo();
}

const char* TDeepQuoteData::GetPriceFormat()
{
	static char strPriceFormat[100] = {};
	sprintf_s(strPriceFormat, "%%.%df", m_pCommodity->PricePrec);
	return strPriceFormat;
}

int TDeepQuoteData::GetTotalQty(TDirect direct, int index)
{
	int iQty = 0;
	vector<TDeepQuote> * pVec = GetData(direct);
	int iSize = pVec->size();
	for (int i = 0; i < pVec->size(); i++)
	{
		if (index==0 || index > i)
		{
			iQty += pVec->at(iSize - i -1).Qty;
		}
		else
		{
			break;
		}
	}
	return iQty;
}