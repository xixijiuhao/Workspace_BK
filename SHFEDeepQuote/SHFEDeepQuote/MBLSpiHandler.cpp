#pragma once
#include "PreInclude.h"

inline bool sortBuyData(const TDeepQuote &first, const  TDeepQuote &second)
{
	return first.Price > second.Price;
}

inline bool sortSellData(const TDeepQuote &first, const  TDeepQuote &second)
{
	return first.Price < second.Price;
}

CMBLSpiHandler::CMBLSpiHandler() : m_Mutex()
{
	m_iPricePrec = 2;
	QuoteTypes = "";
}

CMBLSpiHandler * CMBLSpiHandler::GetMBLQuoteData()
{
	static CMBLSpiHandler * handler = nullptr;
	if(handler == nullptr)
	{
		MBLApi*	tmpMBLApi = MBLApi::CreatMBLApi();
		handler = new CMBLSpiHandler();
		tmpMBLApi->MBLRegisterSpi(handler);
	}
	return handler;
}

void CMBLSpiHandler::Login(const char* userName, const char* pwd, string &scontractType)
{
	QuoteTypes = scontractType;
	strcpy_s(m_stUserData.username, userName);
	strcpy_s(m_stUserData.password, pwd);

	MBLApi * pApi = MBLApi::CreatMBLApi();
	if (QuoteTypes == "SHFE")
	{
		pApi->MBLSetIPInfo(MBL_IP, MBL_SHFE_Port);
	}
	else if(QuoteTypes == "INE")
	{
		pApi->MBLSetIPInfo(MBL_IP, MBL_INE_Port);
	}
	pApi->MBLRegisterSpi(this);
	pApi->MBLInit();
}

void CMBLSpiHandler::OnMBLConnected()
{
	OutputDebugString(L"Entering OnMBLConnected...");
	MBLApi::CreatMBLApi()->MBLReqUserLogin(&m_stUserData);
	
}

void CMBLSpiHandler::OnMBLRspUserLogin(MBLErrorInfo ErrorInfo)
{
	if(SHFEDeepQuoteFrm::bQuoteLoginSuccess != true)
	{
		char cret[100] = { 0 };
		sprintf_s(cret, "Login Over,ErrorInfo = %d", ErrorInfo);
		OutputDebugStringA(cret);
		if (m_pIQuoteLogin != nullptr)
		{
			m_pIQuoteLogin->QuoteLogin(ErrorInfo);
		}
	}
}

void CMBLSpiHandler::OnMBLRspSubcribe(MBLErrorInfo ErrorInfo)
{
	if (ErrorInfo == MBL_NO_ERROR)
	{
		OutputDebugString(L"MBLSubcribe Success!");
		MBLApi::CreatMBLApi()->SetMBLDataPushingFlag(true);
	}
	else
	{
		char cret[100] = { 0 };
		sprintf_s(cret, "MBLSubcribe ErrorInfo = %d", ErrorInfo);
		OutputDebugStringA(cret);
	}
}

void CMBLSpiHandler::AddData(TDeepQuote* quote)
{
	vector<TDeepQuote> * pVec = &m_vecMBLSell;
	if (quote->Direct == dBuy)
	{
		pVec = &m_vecMBLBuy;
	}

	for (auto iter = pVec->begin(); iter != pVec->end(); ++iter)
	{
		if (iter->Price == quote->Price)
		{
			iter->Qty = quote->Qty;
			return;
		}
	}
	pVec->push_back(*quote);

	//对行情进行排序，买价按照从高到底，卖价按照从低到高
	if(quote->Direct == dBuy)
	{
		std::sort(pVec->begin(), pVec->end(), sortBuyData);
	}
	else if(quote->Direct == dSell)
	{
		std::sort(pVec->begin(), pVec->end(), sortSellData);
	}
	//只保留十档行情
	if (pVec->size() > 10)
	{
		pVec->pop_back();
	}
}

void CMBLSpiHandler::OnMBLRspMarketData(MBLData *pMBLMarketData)
{
	//数据存和取是多线程的，需要加锁
	std::lock_guard<std::mutex> lock(m_Mutex);
	
	TDeepQuote stQuote = {};
	TransMBLDataToDeepQuote(pMBLMarketData, stQuote);
	AddData(&stQuote);
	TCHAR log[100] = {};
	wsprintf(log, L"direct: %d, %c\n", pMBLMarketData->direction, stQuote.Direct);
	OutputDebugString(log);

	if (m_pQuoteUpdate != nullptr)
	{
		m_pQuoteUpdate->Update();
	}
}

void CMBLSpiHandler::SubSHFEDeepQuote(TContractKey & key)
{
	m_vecMBLSell.clear();
	m_vecMBLBuy.clear();
	SetPricePrec(key);
	string commodityNo = key.CommodityNo;
	string contractNo = key.ContractNo;
	string comContractNo = commodityNo + contractNo;
	transform(comContractNo.begin(), comContractNo.end(), comContractNo.begin(), ::tolower);
	MBLApi::CreatMBLApi()->MBLReqSubcribe(const_cast<char *>(comContractNo.c_str()));
}

void CMBLSpiHandler::SetPricePrec(TContractKey & key)
{
	SCommodityNoType commodityNo = {};
	sprintf_s(commodityNo, "%s|%c|%s", key.ExchangeNo, key.CommodityType, key.CommodityNo);
	SCommodity  * out[1] = {};
	SRetType count = g_pStarApi->GetCommodityData(key.ExchangeNo, commodityNo, out, 1, false);
	if(count)
	{
		m_iPricePrec = out[0]->PricePrec;
	}
}

vector<TDeepQuote> * CMBLSpiHandler::GetMBLData(TDirect direct)
{
	//数据存和取是多线程的，需要加锁
	std::unique_lock<std::mutex> lock(m_Mutex);

	if (direct == dBuy)
	{
		return &m_vecMBLBuy;
	}
	else if (direct == dSell)
	{
		return &m_vecMBLSell;
	}
	return nullptr;
}

void CMBLSpiHandler::GetQuoteData(vector<TDeepQuote> & vecBuyData, vector<TDeepQuote> & vecSellData)
{
	//数据存和取是多线程的，需要加锁
	std::unique_lock<std::mutex> lock(m_Mutex);

	vecBuyData = m_vecMBLBuy;
	vecSellData = m_vecMBLSell;
}
void CMBLSpiHandler::ClearQuoteData()
{
	//数据存和取是多线程的，需要加锁
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_vecMBLBuy.clear();
	m_vecMBLSell.clear();
}


void CMBLSpiHandler::TransMBLDataToDeepQuote(MBLData *pMBLMarketData, TDeepQuote & tDeepQuote)
{
	if(pMBLMarketData->direction == '0')
	{
		tDeepQuote.Direct = dBuy;
	}
	else
	{
		tDeepQuote.Direct = dSell;
	}
	
	tDeepQuote.Price = pMBLMarketData->price;
	tDeepQuote.Qty = pMBLMarketData->volume;
	tDeepQuote.IsLast = 'N';
}

int CMBLSpiHandler::GetTotalMBLQty(TDirect direct, int index)
{
	int iQty = 0;
	vector<TDeepQuote> * pVec = GetMBLData(direct);
	int iSize = pVec->size();
	for (int i = 0; i < pVec->size(); ++i)
	{
		if (index == 0 || index > i)
		{
			iQty += pVec->at(iSize - i - 1).Qty;
		}
		else
		{
			break;
		}
	}
	return iQty;
}

void CMBLSpiHandler::GetUserInfo()
{
	//TDataContainer loginUsers;
	//g_pTradeData->GetAllLogin(loginUsers);
	//if (loginUsers.size())
	//{
	//	const TLogin *pLogin = (const TLogin*)loginUsers.at(0);
	//	strcpy_s(m_szSign, pLogin->Sign);
	//}
}

const char* CMBLSpiHandler::GetPriceFormat()
{
	static char strPriceFormat[100] = {};
	sprintf_s(strPriceFormat, "%%.%df", m_iPricePrec);
	return strPriceFormat;
}
