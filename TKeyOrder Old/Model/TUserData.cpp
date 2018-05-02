#include "BaseInclude.h"

#define bigOrderQty 1000

inline unsigned __int64 GetCycleCount()
{
	__asm
	{
		_emit 0x0F;
		_emit 0x31;
	}
}

char szChsUM[][20] = { "平", "开", "买","卖", "手", "平仓", "加多", "加空", "锁仓", "买多", "卖空", "优先平今", "先开先平", "卖平", "买平", "锁仓状态", "买入", "卖出" };
char szChtUM[][20] = { "平", "開", "買", "賣","手", "平倉", "加多", "加空", "鎖倉", "買多", "賣空", "優先平今", "先開先平", "賣平", "買平", "鎖倉狀態", "買入", "賣出" };
char szEnuUM[][20] = { " close ", " open ", " buy "," sell ", " lots ", "Close", "AddLong", "AddShort", "Lock", "BuyLong", "SellShort", "CTF", "FOFC", "SellClose", "BuyClose",
"Locked", "Buy", "Sell" };
enum{
	CloseOne_Index,
	OpenOne_Index,
	BuyOne_Index,
	SellOne_Index,
	Lots_Index,
	Close_Index,
	AddLong_Index,
	AddShort_Index,
	Lock_Index,
	BuyLong_Index,
	SellShort_Index,
	CTF_Index,
	FOFC_Index,
	SellClose_Index,
	BuyClose_Index,
	Locked_Index,
	Buy_Index,
	Sell_Index,
};
char* _GetTextUM(int i)
{
	if (g_language == CHT)return szChtUM[i];
	else if (g_language == ENU)return szEnuUM[i];
	return szChsUM[i];
}
TUserData::TUserData(ISQuoteDataSpi *pQuoteSpi)
{
	m_strCode = "";
	m_bLock = false;
	m_pQuoteSpi = pQuoteSpi;

	memset(&m_stQuoteData, 0, sizeof(TQuoteData));

	m_strUserNo = "";
	m_bGetPositionQty = true;
}

TUserData::~TUserData()
{
	if (g_pStarApi)
		g_pStarApi->UnsubAllQuote(m_pQuoteSpi);
}

void TUserData::InsertOrder(TSendOrder *pOrder)
{
	memcpy(pOrder, &m_stContractKey, sizeof(TContractKey));
	strcpy_s(pOrder->UserNo, g_pGlobalModel->m_strUserNo.c_str());
	strcpy_s(pOrder->Sign, g_pGlobalModel->m_strSign.c_str());
	pOrder->OrderType = otLimit;

	if (!m_stOrderData.Contract)
	{
		m_pPresenter->ShowMsgBox(EN_Invalid_ContractNo);
		return;
	}

	int iRet = g_pTradeAction->CheckOrder(*pOrder);
	if (iRet != 0)
	{
		if (!m_pPresenter->ShowMsgBox(iRet))
			return;
	}

	if (g_pTradeApi) 
	{
		if (pOrder->Offset == oCover) //平仓
		{
			pOrder->Direct = dBuy;
			if (m_stOrderData.TCoverDirect == dBuy)
				pOrder->Direct = dSell;
			if (OrderConfirm(*pOrder))
				g_pTradeAction->InsertCloseOrderNoCheck(*pOrder);
		}
		else//开仓
		{
			if (OrderConfirm(*pOrder))
			{
				InsertOrder(*pOrder);
			//	g_pTradeApi->InsertOrder(*pOrder);
			}
		}
	}
}

void TUserData::InsertOrder(TSendOrder & stOrder)
{
	if (g_pGlobalModel->m_stOptionConfig.bBigOrderSplit) //大单拆分
	{
		BigOrderSplitForOpen(stOrder);
	}
	else
	{
		if (g_pGlobalModel->m_stStopLossConfig.bOpenAutoStopLoss)
		{
			SetAutoOpenStopLP(stOrder);
		}
		else
		{
			g_pTradeApi->InsertOrder(stOrder);
		}
	}
}
void TUserData::BigOrderSplitForOpen(TSendOrder& stOrder)
{
	int iOrderQty = stOrder.OrderQty;
	if (stOrder.Offset == oOpen)
	{
		int CanOpenQty = 0;
		do
		{
			if (g_pTradeData->GetTradableQty(stOrder, CanOpenQty))
				break;
			if (iOrderQty > CanOpenQty)
				iOrderQty = CanOpenQty;
		} while (false);
		while (iOrderQty > bigOrderQty)
		{
			stOrder.OrderQty = bigOrderQty;
			if (g_pGlobalModel->m_stStopLossConfig.bOpenAutoStopLoss)
				SetAutoOpenStopLP(stOrder);
			else
				g_pTradeApi->InsertOrder(stOrder);
			iOrderQty -= bigOrderQty;
		}
		stOrder.OrderQty = iOrderQty;
		if (g_pGlobalModel->m_stStopLossConfig.bOpenAutoStopLoss)
			SetAutoOpenStopLP(stOrder);
		else
		{
			//__int64 iBegin = GetCycleCount();
			//__int64 iEnd = GetCycleCount();
			g_pTradeApi->InsertOrder(stOrder);
			//iEnd = GetCycleCount();
			//LARGE_INTEGER lFre = { 0 };
			//QueryPerformanceFrequency(&lFre);
			//double iTime = (iEnd - iBegin)*1.0 / lFre.QuadPart;
			//iTime = 0;
		}
	}
}
void TUserData::SetAutoOpenStopLP(TSendOrder& stOrder)
{
	AutoStopInfo Order;
	Order.iOrderType = AutoStopType;
	g_pStopLP->OpenLPOrder(&Order, &stOrder);
}

bool TUserData::OrderConfirm(const TSendOrder &stOrder)
{
	if (!g_pGlobalModel->m_stOptionConfig.bOrderConfirm) return true;
	char strShowInfo[100] = { 0 };
	char strOrderPrice[30] = { 0 };
	DoubleToChar(m_stCommodityInfo.iPrecision, stOrder.OrderPrice, strOrderPrice);

	char strOffset[20] = {};
	strcpy_s(strOffset, _GetTextUM(CloseOne_Index));
	if (stOrder.Offset == oOpen)
		strcpy_s(strOffset, _GetTextUM(OpenOne_Index));
	if (stOrder.Direct == dBuy)
		sprintf_s(strShowInfo, "%s%s,%s,%s%s%d%s?", m_stContractKey.CommodityNo,m_stContractKey.ContractNo, strOrderPrice,
		_GetTextUM(BuyOne_Index), strOffset, stOrder.OrderQty,_GetTextUM(Lots_Index));
	else
		sprintf_s(strShowInfo, "%s%s,%s,%s%s%d%s?", m_stContractKey.CommodityNo, m_stContractKey.ContractNo, strOrderPrice, 
		_GetTextUM(SellOne_Index), strOffset, stOrder.OrderQty, _GetTextUM(Lots_Index));

	return m_pPresenter->ShowMsgBox(Msg_OKOrCancel, strShowInfo);
}
bool TUserData::DealContractNo(string &strCode, bool bGetPositionQty, TDirect direct)
{
	if (m_bLock) return false;

	m_bGetPositionQty = bGetPositionQty;

	if (direct != 0)
		m_DirectFromNotice = direct;

	m_stOrderData.TCoverDirect = dNone;
	memset(&m_stContractKey, 0, sizeof(TContractKey));
	strcpy_s(m_stContractKey.Sign, g_pGlobalModel->m_strSign.c_str());

	if (!CConvert::TContractNoToTContractKey(strCode.c_str(), m_stContractKey) || !CConvert::IsValidTradeCommodity(m_stContractKey))
	{
		m_strCode = "";
		memset(&m_stOrderData, 0, sizeof(TOrderViewData));
		memset(&m_stQuoteData, 0, sizeof(TQuoteData));
		return false;
	}
	CConvert::GetCommodityInf(m_stContractKey, m_stCommodityInfo);
	if (m_stCommodityInfo.bIfFractionPrice)
	{
		m_stCommodityInfo.dMiniChangePrice = m_stCommodityInfo.dMiniChangePrice / m_stCommodityInfo.iPrecision;
		m_stCommodityInfo.iPrecision = 5;
	}

	m_strCode = strCode;
	SubQuote(m_stContractKey);
	
	GetOrderShowInfo(m_stContractKey);

	return true;
}
void TUserData::UpdateOrderData(bool bGetPositionQty)
{
	m_bGetPositionQty = bGetPositionQty;
	GetOrderShowInfo(m_stContractKey);
	m_stOrderData.OrderQty = 0; //0不更新OrderView上手数数量
}

void TUserData::GetOrderShowInfo(TContractKey &key)
{
	if (!g_pTradeData) return; 	
	//根据Direct来设置CoverText的值
	memset(&m_stOrderData, 0, sizeof(TOrderViewData));
	SetContractNoText(key);

	uint iLongCount = 0;
	uint iShortCount = 0;
	uint iDayLongCount = 0;
	uint iDayShortCount = 0;
//	g_pTradeData->GetPositionVol(g_pGlobalModel->m_strUserNo.c_str(), &key, iLongCount, iShortCount, iDayLongCount, iDayShortCount);
	CConvert::GetPositionVol(g_pGlobalModel->m_strUserNo.c_str(), &m_stContractKey, iLongCount, iShortCount, iDayLongCount, iDayShortCount);

	m_stOrderData.iBuyPosQty = iLongCount;
	m_stOrderData.iSellPosQty = iShortCount;
	memcpy(&m_stOrderData.ContractKey, &m_stContractKey, sizeof(TContractKey));

	SContract * contract[1] = {};
	SContractNoType contractNo = {};
	CConvert::TradeContractToQContractNoType(m_stContractKey, contractNo);
	g_pStarApi->GetContractData("", contractNo, contract, 1, false);
	m_stOrderData.Contract = contract[0];

	if (g_pGlobalModel->m_utUserType == utInner)
	{
		SetInnerOrderBtnText(key,iLongCount,iShortCount);
	}
	else
	{
		SetForeignOrderBtnText(iLongCount,iShortCount);
	}

	//如果有持仓，那么就要获取持仓数量
	if (iLongCount || iShortCount) {
		m_bGetPositionQty = true;
	}

	SetConfig();
}
void TUserData::SetConfig()
{
	if (m_bGetPositionQty)
	{
		if (g_pGlobalModel->m_stOptionConfig.iPositionQty == 0)
		{
			m_stOrderData.OrderQty = GetOrderQty(m_stContractKey);
		}
		else
		{
			if (m_stOrderData.OrderQty == 0)
				m_stOrderData.OrderQty = 1;
		}
	}
	else
	{
		m_stOrderData.OrderQty = GetOrderQty(m_stContractKey);	
	}
	m_stOrderData.iBuyOffset = g_pGlobalModel->GetSuperPriceOffset(&m_stContractKey, dBuy);
	m_stOrderData.iSellOffset = g_pGlobalModel->GetSuperPriceOffset(&m_stContractKey, dSell);
}
void TUserData::SetContractNoText(TContractKey & key)
{
	switch (key.CommodityType)
	{
	case ctFutures:
	case ctSpreadCommodity:
		sprintf_s(m_stOrderData.szContractNo, "%s%s", key.CommodityNo, key.ContractNo);
		break;
	case ctSpreadMonth:
		sprintf_s(m_stOrderData.szContractNo, "%s%s/%s", key.CommodityNo, key.ContractNo,key.ContractNo2);
		break;
	case ctOption:
		sprintf_s(m_stOrderData.szContractNo, "%s%s%c%s", key.CommodityNo, key.ContractNo, key.OptionType, key.StrikePrice);
		break;

	}
}
int TUserData::GetOrderQty(TContractKey & stContract)
{
	int iQty = 1;
	char szKey[100] = { 0 };
	sprintf_s(szKey, "%s%c%s%s%s%c",
		stContract.ExchangeNo, stContract.CommodityType, stContract.CommodityNo,
		stContract.ContractNo, stContract.StrikePrice, stContract.OptionType);

	auto iter = g_pGlobalModel->m_mapDefaultQty.find(szKey);
	if (iter != g_pGlobalModel->m_mapDefaultQty.end())
		iQty = iter->second.iQty;
	return iQty;
}

void TUserData::SubQuote(TContractKey &key)
{
	if (!g_pStarApi) return;

	SContractNoType sQuote = { 0 };
	CConvert::TradeContractToQContractNoType(key, sQuote);
	//if (!m_strQuoteContractId.empty() && m_strQuoteContractId != sQuote)
	if (strlen(m_strQuoteContractId) && sQuote!=m_strQuoteContractId)
	{
		if (g_Presenter->CanUnsubQuote(m_strQuoteContractId))
			g_pStarApi->UnsubQuote(&m_strQuoteContractId,1, m_pQuoteSpi);
		m_strQuoteContractId[0] = '\0';
	}
//	m_strQuoteContractId = sQuote;
	strcpy_s(m_strQuoteContractId, sQuote);
	bool ret = g_pStarApi->SubQuote(&sQuote,1, m_pQuoteSpi);
}

bool TUserData::SplitNotice(string strContent, UINT type, string &strContracID)
{
	vector<string>vecContent;

	m_DirectFromNotice = dNone;
	m_strUserNo = "";

	if (type == TPresenter::fromQuote)
	{
		string::size_type pos = strContent.find('=');
		if (pos != string::npos)
		{
			strContracID = strContent.substr(++pos, string::npos);
			string::size_type bpos = strContracID.find(';');
			if (bpos != string::npos)
			{
				strContracID = strContracID.substr(0, bpos);
				//
				SContractNoType q;
				strcpy_s(q, strContracID.c_str());
				SContract* qRet = g_pStarApi->GetContractUnderlay(q);
				if(qRet)
					strContracID = qRet->ContractNo;
				//
				vecContent = CConvert::SplitString(strContracID, '|');
				if (vecContent.size() >= 4)
				{
					//if (/*vecContent[3] == "NEARBY" || vecContent[3] == "MAIN" ||*/ vecContent[3] == "INDEX")
					//	return false;
					if (vecContent.size() == 4)
					{
						if (vecContent[1] == "O")  //期权
						{
							string::size_type iPosFindC = vecContent[3].find_first_of('C');
							if (iPosFindC != string::npos)
							{
								vecContent[3].insert(iPosFindC,1, ' ');
								strContracID = vecContent[0] + ' ' + vecContent[2] + ' ' + vecContent[3];
							}
							else
							{
								string::size_type iPosFindP = vecContent[3].find_first_of('P');
								if (iPosFindP != string::npos)
								{
									vecContent[3].insert(iPosFindP, 1, ' ');
									strContracID = vecContent[0] + ' ' + vecContent[2] + ' ' + vecContent[3];
								}
								else
								{
									return false;
								}
							}
						}
						else //跨品种，期货，模拟不支持跨品种套利
						{
							strContracID = vecContent[0] + ' ' + vecContent[2] + ' ' + vecContent[3];
						}
					}
					else if (vecContent.size() == 5) //跨期套利
					{
						strContracID = vecContent[0] + ' ' + vecContent[2] + ' ' + vecContent[3] + '/' + vecContent[4];
					}
					else
						return false;
				}
				else
				{
					return false;
				}
			}
		}
	}
	else if (type == TPresenter::fromTradeD)
	{
		string::size_type pos = strContent.find("contractkey=");
		string::size_type rpos = strContent.find(";user=");
		string::size_type Directpos = strContent.find("direct=");
		if (Directpos != string::npos)
		{
			m_DirectFromNotice = strContent[Directpos + strlen("direct=")];
		}
		if (rpos != string::npos)
		{
			string::size_type iFindUserEnd = strContent.find(';', rpos+1);
			if (iFindUserEnd != string::npos)
			{
				string::size_type iBegin = rpos + strlen(";user=");
				m_strUserNo = strContent.substr(iBegin, iFindUserEnd -iBegin);
			}
		}
		if (pos != string::npos)
		{
			pos = pos + strlen("contractkey=");
			strContracID = strContent.substr(pos, rpos - pos);
			vecContent = CConvert::SplitString(strContracID, '|');
			if (vecContent.size() < 6)
			{
				m_DirectFromNotice = dNone;
				m_strUserNo = "";
				return false;
			}
			if (vecContent[2] != "O")
				strContracID = vecContent[0] + ' ' + vecContent[1] + ' ' + vecContent[3];
			else
				strContracID = vecContent[0] + ' ' + vecContent[1] + ' ' + vecContent[3] + ' ' + vecContent[5] + vecContent[4];
		}
	}
	return true;
}

void TUserData::DealQuoteData(SContractNoType contractId)
{
	SQuoteSnapShot stQuoteData;
	memset(&m_stQuoteData, 0, sizeof(TQuoteData));
	if (g_pStarApi && g_pStarApi->GetSnapshot(contractId, &stQuoteData))
	{
		m_stQuoteData.BuyPrice = stQuoteData.Data[S_FID_BESTBIDPRICE].Price;
		m_stQuoteData.SellPrice = stQuoteData.Data[S_FID_BESTASKPRICE].Price;
		m_stQuoteData.LastPrice = stQuoteData.Data[S_FID_LASTPRICE].Price;
		m_stQuoteData.UpPrice = stQuoteData.Data[S_FID_LIMITUPPRICE].Price;
		m_stQuoteData.DownPrice = stQuoteData.Data[S_FID_LIMITDOWNPRICE].Price;

		m_stQuoteData.BuyQty = stQuoteData.Data[S_FID_BESTBIDQTY].Qty;
		m_stQuoteData.SellQty = stQuoteData.Data[S_FID_BESTASKQTY].Qty;
		m_stQuoteData.LastQty = stQuoteData.Data[S_FID_LASTQTY].Qty;

		m_stQuoteData.iPrecision = m_stCommodityInfo.iPrecision;
	}

}

void TUserData::SetInnerOrderBtnText(TContractKey &key, uint iLongCount , uint iShortCount)
{
	m_stOrderData.SetCoverPrice = true;

	strcpy_s(m_stOrderData.szCoverBtnText, _GetTextUM(Close_Index));
	if (iLongCount&&iShortCount)
	{
		strcpy_s(m_stOrderData.szBuyBtnText, _GetTextUM(AddLong_Index));
		strcpy_s(m_stOrderData.szSellBtnText, _GetTextUM(AddShort_Index));
		m_stOrderData.TCoverDirect = dBoth;
		m_stOrderData.SetCoverPrice = false;
	}
	else if (iLongCount&&!iShortCount)
	{
		strcpy_s(m_stOrderData.szBuyBtnText, _GetTextUM(AddLong_Index));
		strcpy_s(m_stOrderData.szSellBtnText, _GetTextUM(Lock_Index));
		m_stOrderData.TCoverDirect = dBuy;
	}
	else if (!iLongCount&&iShortCount)
	{
		strcpy_s(m_stOrderData.szBuyBtnText, _GetTextUM(Lock_Index));
		strcpy_s(m_stOrderData.szSellBtnText, _GetTextUM(AddShort_Index));
		m_stOrderData.TCoverDirect = dSell;
	}
	else if (!iLongCount && !iShortCount)
	{
		strcpy_s(m_stOrderData.szBuyBtnText, _GetTextUM(BuyLong_Index));
		strcpy_s(m_stOrderData.szSellBtnText, _GetTextUM(SellShort_Index));
		if (CConvert::IsSHFE(key.ExchangeNo))
			strcpy_s(m_stOrderData.szCoverPrice, _GetTextUM(CTF_Index));
		else
			strcpy_s(m_stOrderData.szCoverPrice, _GetTextUM(FOFC_Index));

		m_stOrderData.SetCoverPrice = false;
		m_stOrderData.TCoverDirect = dNone;
	}
	if (m_DirectFromNotice != dNone)
	{
		m_stOrderData.TCoverDirect = m_DirectFromNotice;
		m_stOrderData.SetCoverPrice = true;
	}

	if (m_stOrderData.TCoverDirect == dBuy)
	{
		if (m_bGetPositionQty)
			m_stOrderData.OrderQty = iLongCount;
		strcpy_s(m_stOrderData.szCoverBtnText, _GetTextUM(SellClose_Index));
	}
	else if (m_stOrderData.TCoverDirect == dSell)
	{
		if (m_bGetPositionQty)
			m_stOrderData.OrderQty = iShortCount;
		strcpy_s(m_stOrderData.szCoverBtnText, _GetTextUM(BuyClose_Index));
	}
	else if (m_stOrderData.TCoverDirect == dBoth)
	{
		strcpy_s(m_stOrderData.szCoverPrice, _GetTextUM(Locked_Index));
	}
}
void TUserData::SetForeignOrderBtnText(uint iLongCount, uint iShortCount)
{
	strcpy_s(m_stOrderData.szBuyBtnText, _GetTextUM(Buy_Index));
	strcpy_s(m_stOrderData.szSellBtnText, _GetTextUM(Sell_Index));
	if (iLongCount)
	{
	//	strcpy_s(m_stOrderData.szSellBtnText, _GetTextUM(Close_Index));
		if (m_bGetPositionQty)
			m_stOrderData.OrderQty = iLongCount;
	}
	else if (iShortCount)
	{
	//	strcpy_s(m_stOrderData.szBuyBtnText, _GetTextUM(Close_Index));
		if (m_bGetPositionQty)
			m_stOrderData.OrderQty = iShortCount;
	}
}

void TUserData::SetContractNo(const char* szUserNo, TDirect direct)
{
	m_DirectFromNotice = direct;
	m_strUserNo = szUserNo;
}

void TUserData::UpdateConfig()
{
	SetConfig();
}

//用户变了之后，检查合约是否有效，无效的话重置数据
void TUserData::OnUserChange()
{
	strcpy_s(m_stContractKey.Sign, g_pGlobalModel->m_strSign.c_str());
	if (!CConvert::IsValidTradeCommodity(m_stContractKey))
	{
		m_strCode = "";
		memset(&m_stOrderData, 0, sizeof(TOrderViewData));
		memset(&m_stQuoteData, 0, sizeof(TQuoteData));
		g_pStarApi->UnsubQuote(&m_strQuoteContractId,1, m_pQuoteSpi);
		m_strQuoteContractId[0] = '\0';
	}
}

bool TUserData::OnQuote(const SContract* quote)
{
	if (quote)
	{
		if (strcmp(quote->ContractNo, m_strQuoteContractId))
			return false;
	}
	DealQuoteData(m_strQuoteContractId);
	return true;
}

const TPosition * TUserData::GetPosition()
{
	TDirect direct = dNone;
	if (m_stOrderData.iBuyPosQty)
	{
		direct = dBuy;
	}else if (m_stOrderData.iSellPosQty)
	{
		direct = dSell;
	}
	else {
		return nullptr;
	}
	const TPosition * pPosition = g_pTradeData->GetPositionTotal(g_pGlobalModel->m_strUserNo.c_str(), &m_stContractKey, direct);
	return pPosition;
}