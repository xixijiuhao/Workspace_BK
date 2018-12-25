#include "PreInclude.h"

void  TPointOrderFrmData::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
    if (UpdataQuoteData() && UpdataQuoteDataL2())
        PostMessage(m_hOrderFrm, WM_ONQUOTE, 0, 0);
}

void TPointOrderFrmData::OnOrder(const TOrder *pOrder)
{
    if (UpdataOrderData(pOrder))
        PostMessage(m_hOrderFrm, WM_ONORDER, 0, 0);

    string strCode;
    CConvert::ParOrderToStr(*pOrder, strCode);

    auto iter = m_ParOrderCoverMap.find(strCode);
    if (iter != m_ParOrderCoverMap.end())
    {
        if (pOrder->OrderState == osCanceled || pOrder->OrderState == osPartCanceled)
        {
            iter->second--;
        }
        if (iter->second == 0)
        {
            auto iter1 = m_CoverOrderMap.find(strCode);
            if (iter1 != m_CoverOrderMap.end())
            {
                int iCover = 0, iTCover = 0;
                CConvert::GetCloseNum(m_UserNo, &iter1->second, iter1->second.Direct, iter1->second.OrderQty, iCover, iTCover);
                InsertCloseOrder(iter1->second,iCover, iTCover);
                m_CoverOrderMap.clear();
                m_ParOrderCoverMap.clear();
            }

        }
    }
}
void __cdecl TPointOrderFrmData::OnMatch(const TMatch * pMatch)
{
	PostMessage(m_hOrderFrm, WM_ONMATCH, 0, (LPARAM)pMatch);
}

void TPointOrderFrmData::OnPositionTotal(const TTotalPosition *pPosition)
{
	if (UpdataPositionData(pPosition))
		PostMessage(m_hOrderFrm, WM_POSITION, 0, 0);
}
void __cdecl  TPointOrderFrmData::OnMoney(const TMoney * pMoney)
{
	if (pMoney)
		PostMessage(m_hOrderFrm, WM_ONMONEY, 0, (LPARAM)pMoney);
}

bool  TPointOrderFrmData::UpdataQuoteData()
{
    memset(&m_QuoteData, 0, sizeof(m_QuoteData));
	SContractNoType qNo;
    CConvert::TradeContractToQContractNoType(m_stConKey, qNo);

    if (g_pStarApi && g_pStarApi->GetSnapshot(qNo,&m_QuoteData))
    {
        return true;
    }
    return false;
}

bool  TPointOrderFrmData::UpdataQuoteDataL2()
{
	memset(&m_SellQuoteDataL2, 0, sizeof(m_SellQuoteDataL2));
	memset(&m_BuyQuoteDataL2, 0, sizeof(m_BuyQuoteDataL2));
	SContractNoType qNo;
	CConvert::TradeContractToQContractNoType(m_stConKey, qNo);

	if (g_pStarApi && g_pStarApi->GetSnapshotL2(qNo, &m_BuyQuoteDataL2, &m_SellQuoteDataL2))
	{
		return true;
	}
	return false;
}
bool TPointOrderFrmData::UpdataMoney()
{
	SCurrencyNoType currency = "CNY";
	if (g_pTradeData)
	{
		TCommodityKey pCommodityKey; 
		memcpy(&pCommodityKey, &m_stConKey, sizeof(TCommodityKey));
		memcpy(&pCommodityKey.Sign, &m_Sign, sizeof(pCommodityKey.Sign));
		
		const SCommodity * pCommodity = g_pTradeData->GetCommodity(&pCommodityKey);
		if (pCommodity && pCommodity->Currency)
		{
			if (!strcmp(pCommodity->Currency->CurrencyNo,"")) {
				strcpy_s(pCommodity->Currency->CurrencyNo, sizeof(pCommodity->Currency->CurrencyNo), currency);
			}
			const TCurrency* pCurrency =  g_pTradeData->GetCurrency(pCommodity->Currency->CurrencyNo, m_Sign);
			if (pCurrency)
			{
				const TMoney* pMoney = g_pTradeData->GetMoney(m_UserNo, NULL, pCommodity->Currency->CurrencyNo, m_Sign);
				if (pMoney)
					PostMessage(m_hOrderFrm, WM_ONMONEY, 0, (LPARAM)pMoney);
		
				return true;
			}
		}	
	}
	return false;
}


bool  TPointOrderFrmData::UpdataPositionData(const TContractSingleKey *pPosition)
{
    if (CConvert::Cmp2ContractSingleKey(*pPosition, m_stConKey))
    {
        m_clPosition.clear();
        m_PositionMap.clear();
        g_pTradeData->GetAllPositionSub(m_clPosition, m_Sign);
        for (uint i = 0; i < m_clPosition.size(); i++)
        {
            TPosition *p = (TPosition *)m_clPosition.at(i);		 
			if (CConvert::Cmp2ContractSingleKey(*p, m_stConKey))
			m_PositionMap.insert(make_pair(p->PositionPrice, *p));		
        }
        return true;
    }
    return false;

}

bool  TPointOrderFrmData::UpdataOrderData(const TContractSingleKey *pOrder)
{
    if (CConvert::Cmp2ContractSingleKey(*pOrder, m_stConKey))
    {
        m_clParOrder.clear();
        m_ParOrderMap.clear();
        m_ConditionOrderMap.clear();
        g_pTradeData->GetParOrder(m_clParOrder);
        for (uint i = 0; i < m_clParOrder.size(); i++)
        {
            TOrder *p = (TOrder *)m_clParOrder.at(i);
            if (CConvert::Cmp2ContractSingleKey(*p, m_stConKey) && p->StrategyType == stNone)
            {
                m_ParOrderMap.insert(make_pair(p->OrderPrice, *p));
            }
            else if (CConvert::Cmp2ContractSingleKey(*p, m_stConKey) && p->StrategyType == stCondition)
            {
                m_ConditionOrderMap.insert(make_pair(p->OrderPrice, *p));
            }
        }
        return true;
    }
    return false;
}

void TPointOrderFrmData::SetContractNo(string sCode, char* UserNo, int iUserSize)
{
    SContractNoType sQuote = { 0 };
	strcpy_s(m_UserNo, sizeof(m_UserNo), UserNo);
    CConvert::TContractNoToTContractKey(sCode.c_str(), m_stConKey);
    CConvert::TradeContractToQContractNoType(m_stConKey, sQuote);
    UpdataPositionData(&m_stConKey);
    UpdataOrderData(&m_stConKey);
	int iRet = g_pStarApi->UnsubAllQuote(this);
     g_pStarApi->SubQuote(&sQuote,1, this);
    UpdataQuoteData();
	UpdataQuoteDataL2();
	UpdataMoney();
    PostMessage(m_hOrderFrm, WM_ONCONTRACTDATAINIT, 0, 0);
}


bool TPointOrderFrmData::DealAutoOpenCloseOrder(TSendOrder &stSendOrder, char* UserNo)
{
    int iCover = 0, iTCover = 0;
	strcpy_s(m_UserNo, sizeof(m_UserNo), UserNo);

    stSendOrder.Offset = CConvert::GetOffset(UserNo, &stSendOrder, stSendOrder.Direct);
    uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	if (stSendOrder.Offset == oOpen || stSendOrder.Offset == oNone)
    {       
        int iQty = 0;
        if (!g_pTradeData->GetTradableQty(stSendOrder, iQty) && iQty < stSendOrder.OrderQty)
            stSendOrder.OrderQty = iQty;
        InsertOrderAssitant::InsertOrder(stSendOrder);
        return true;
    }
    CConvert::GetCloseNum(UserNo, &stSendOrder, stSendOrder.Direct, stSendOrder.OrderQty, iCover, iTCover);
    int iCoverVol = iCover + iTCover;
    if (stSendOrder.OrderQty <= iCoverVol&&iCoverVol != 0)
    {
        stSendOrder.OrderQty = iCoverVol;
        InsertCloseOrder(stSendOrder, iCover, iTCover);
    }
    else if (stSendOrder.OrderQty > iCoverVol)
    {
        uint count = 0;
        for (auto iter : m_ParOrderMap)
        {           
            if (iter.second.Offset == oCover || iter.second.Offset == oCoverT &&iter.second.Direct == stSendOrder.Direct)
            {
                g_pTradeApi->CancelOrder(iter.second.OrderID);
                string strCode;
                CConvert::ParOrderToStr(stSendOrder, strCode);
                m_ParOrderCoverMap.insert(make_pair(strCode, ++count));
                m_CoverOrderMap.insert(make_pair(strCode, stSendOrder));
            }
        }
    }
    return true;
}

void TPointOrderFrmData::InsertCloseOrder(TSendOrder& stOrder, int iCover, int iTCover)
{
    if (iTCover == 0 && iCover != 0)
    {
        stOrder.Offset = oCover;
        InsertOrderAssitant::InsertOrder(stOrder);
    }
    else
    {
        stOrder.Offset = oCoverT;
        stOrder.OrderQty = iTCover;
        InsertOrderAssitant::InsertOrder(stOrder);
        if (iCover != 0)
        {
            stOrder.OrderQty = iCover;
            stOrder.Offset = oCover;
            InsertOrderAssitant::InsertOrder(stOrder);
        }
    }
}
void TPointOrderFrmData::InsertCancelCover(TSendOrder& stSendOrder)
{
    uint count = 0;
    for (auto iter : m_ParOrderMap)
    {
        if (iter.second.Offset == oCover || iter.second.Offset == oCoverT &&iter.second.Direct == stSendOrder.Direct)
        {
            g_pTradeApi->CancelOrder(iter.second.OrderID);
            string strCode;
            CConvert::ParOrderToStr(stSendOrder, strCode);
            m_ParOrderCoverMap.insert(make_pair(strCode, ++count));
            m_CoverOrderMap.insert(make_pair(strCode, stSendOrder));
        }
    }
}
