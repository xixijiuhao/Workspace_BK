#include "BaseInclude.h"

#define maxPriceJump  3 

TChase::TChase(HWND hTimer, uint timerID)
{
	m_hwndTimer = hTimer;
	m_iTimerID = timerID;
	m_pstChaseConfig = &g_pGlobalModel->m_stChaseConfig;
}
TChase::~TChase()
{

}

void TChase::InsertChaseOrder(const TOrder* pOrder)
{
	SetTimer(m_hwndTimer, m_iTimerID, 1000, 0);   //要保证timerID不重复，timerID应放在外边管理
	auto iter = m_mapChaseOrder.find(pOrder->OrderID);
	if (iter == m_mapChaseOrder.end())
	{
		TChaseOrder stTemp;
		memset(&stTemp, 0, sizeof(TChaseOrder));
		stTemp.iChaseCount = 0;
		stTemp.iTime = -1;
		stTemp.bCancel = true;
		stTemp.pOrder = pOrder;
		m_mapChaseOrder.insert(make_pair(pOrder->OrderID, stTemp));
	}
	else
	{
		iter->second.iChaseCount = 0;
		iter->second.pOrder = pOrder;
		iter->second.bCancel = true;
	}
	g_pTradeApi->CancelOrder(pOrder->OrderID);
}

void TChase::OnTimer(WPARAM wParam)
{
	if (wParam != m_iTimerID) return;
	for (auto iter = m_mapChaseOrder.begin(); iter != m_mapChaseOrder.end(); iter++)
	{
		if (iter->second.iTime == 0)
		{
			iter->second.bCancel = true;
			g_pTradeApi->CancelOrder(iter->first);
		}
		else
		{
			iter->second.iTime--;
		}
	}
}

void TChase::OnOrder(const TOrder * pOrder)
{
	switch (pOrder->OrderState)
	{
	case osCanceled:
	case osPartCanceled:
		OnCancelOrder(pOrder);
		break;
	case osFilled:
	case osFail:
	case osPartFailed:
		m_mapChaseOrder.erase(pOrder->OrderID);
		break;
	default:
		break;
	}
}
void TChase::OnCancelOrder(const TOrder* pOrder)
{
	auto iter = m_mapChaseOrder.find(pOrder->OrderID);
	if (iter != m_mapChaseOrder.end())
	{
		if (!iter->second.bCancel)   //不是追单模块撤的单，那就不再追
		{
			m_mapChaseOrder.erase(iter);
			return;
		}
		TSendOrder stOrder;
		memcpy(&stOrder, pOrder, sizeof(TSendOrder));
		iter->second.iChaseCount++;
		iter->second.iTime = m_pstChaseConfig->iSecond;
		stOrder.OrderPrice = GetPrice(pOrder, &(iter->second));
		iter->second.bCancel = false;
		uint iOrderID = g_pTradeApi->InsertOrder(stOrder);
		stOrder.OrderQty = pOrder->PreWorkingQty;

		if (m_pstChaseConfig->iChaseOver&&GetPriceOffset(iter->second.pOrder, stOrder.OrderPrice) >= m_pstChaseConfig->iChaseOver)//追价次数达到
		{
			m_mapChaseOrder.erase(iter);
			return;
		}
		m_mapChaseOrder.insert(make_pair(iOrderID, iter->second));
		m_mapChaseOrder.erase(iter);
	}
}

double TChase::GetMinChangePrice(const TContractKey *pKey)
{
	const TCommodity* pCommodity = g_pTradeData->GetCommodity(pKey);
	if (!pCommodity) return 0;
	return pCommodity->LowerTick == 1 ? pCommodity->UpperTick : pCommodity->UpperTick / pCommodity->LowerTick;
}

double TChase::GetPrice(const TOrder* pOrder, TChaseOrder *pChase)
{
	double dPrice = 0.0;
	CConvert::GetPrice(const_cast<TOrder*>(pOrder), pOrder->Direct, ptCounterPrice, dPrice);

	double dMarketPrice = 0.0;
	CConvert::GetPrice(const_cast<TOrder*>(pOrder), pOrder->Direct, ptMarketPrice, dMarketPrice);

	if (m_pstChaseConfig->iChasePriceCom == ChaseLadderPrice)
	{
		double MinPrice = GetMinChangePrice(pOrder);
		uint iChaseCount = pChase->iChaseCount < 3 ? pChase->iChaseCount : 3;

		if (pOrder->Direct == dBuy)
			dPrice += iChaseCount * MinPrice;
		else if (pOrder->Direct == dSell)
			dPrice -= iChaseCount * MinPrice;
		if ((dPrice > dMarketPrice && pOrder->Direct == dBuy) || (dPrice < dMarketPrice && pOrder->Direct == dSell))
			dPrice = dMarketPrice;
	}
	else if (m_pstChaseConfig->iChasePriceCom == ChaseMarketPrice)
	{
		dPrice = dMarketPrice;
	}
	return dPrice;
}

uint TChase::GetPriceOffset(const TOrder* pOrder, double dOrderPrice)
{
	double dMinChangePrice = GetMinChangePrice(pOrder);
	if (dMinChangePrice != 0)
		return abs((dOrderPrice - pOrder->OrderPrice) / dMinChangePrice);
	return 0;
}