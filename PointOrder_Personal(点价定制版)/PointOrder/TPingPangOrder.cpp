#include "PreInclude.h"
int PingPangOrder::m_iNum = 0;
void PingPangOrder::RegistConfig(PointOrderConfig* pstConfig)
{
	if (!pstConfig)
		return;
	m_pstConfig = pstConfig;
}
void PingPangOrder::RegistHwnd(HWND hParent)
{
	m_hParent = hParent;
}
void PingPangOrder::InsertPingPangOrder(TSendOrder stOrder)
{
	if (m_pstConfig->stPingPang.bPingPang)
	{
        int iQty = 0;
        if (stOrder.Offset == oOpen&&stOrder.StrategyType == stNone)
        {
            if (!g_pTradeData->GetTradableQty(stOrder, iQty) && iQty > 0 && iQty < stOrder.OrderQty)
                stOrder.OrderQty = iQty;
        }
		int iSplitQty = stOrder.OrderQty;
		if (iSplitQty >= 1000)
        {
			for (int i = 0; i < (iSplitQty / 1000); i++)
            {
                stOrder.OrderQty = 1000;
                GetOrderRemark(stOrder.Remark);
                PingPangInfo stInfo;
                InsertRemarkToInfo(stOrder.Remark, stInfo);
                g_pTradeApi->InsertOrder(stOrder);
            }
        }
		if (iSplitQty % 1000)
        {
			stOrder.OrderQty = iSplitQty % 1000;
            GetOrderRemark(stOrder.Remark);
            PingPangInfo stInfo;
            InsertRemarkToInfo(stOrder.Remark, stInfo);
            g_pTradeApi->InsertOrder(stOrder);
        }
		SetTimer(m_hParent, 0, 1000, NULL);
	}
}
void PingPangOrder::OnDealOrder(const TOrder* pstOrder)
{
	if (!pstOrder)
		return;
	std::map<std::string, PingPangInfo>::iterator iter = m_mapRemarkToInfo.find(pstOrder->Remark);
	if (iter != m_mapRemarkToInfo.end())
	{
		if (pstOrder->OrderState == osFail || pstOrder->OrderState == osPartCanceled || pstOrder->OrderState == osCanceled)
			m_mapRemarkToInfo.erase(iter);
		if (pstOrder->OrderState == osSended || pstOrder->OrderState == osAccept||
			pstOrder->OrderState == osQueued || pstOrder->OrderState == osPartFilled || pstOrder->OrderState == osFilled)
		{
			//����OrderId;
			PingPangInfo stInfo;
			iter->second.iOrderId = pstOrder->OrderID;
			strcpy_s(iter->second.cOrderNo, sizeof(pstOrder->OrderNo), pstOrder->OrderNo);
		}
	}
}
void PingPangOrder::OnDealMatch(const TMatch* pstMatch)
{
	//���ͷ���;
	//�ж��Ƿ��ǵ�۽����µĵ�;
	const TOrder* pstOrder = g_pTradeData->GetOrder(pstMatch->OrderNo, pstMatch->Sign);
	if (!pstOrder)
		return;
	std::map<std::string, PingPangInfo>::iterator iter = m_mapRemarkToInfo.find(pstOrder->Remark);
	if (iter == m_mapRemarkToInfo.end())
		return;
	else
	{
		TSendOrder stOrder;
		InitTSendOrderFromTMatch(stOrder, pstMatch);
		InsertPingPangOrder(stOrder);
		//���·�������
		iter->second.iSendQty += stOrder.OrderQty;

		if (pstOrder->OrderState == osFilled && iter->second.iSendQty == pstOrder->OrderQty)
			m_mapRemarkToInfo.erase(iter);
	}			
}
void PingPangOrder::OnTimer()
{
	//��������ʱ��,�������ʱ��齫�൥����
	std::map<std::string, PingPangInfo>::iterator iter = m_mapRemarkToInfo.begin();
	const TOrder* pstOrder = NULL;
	for (; iter != m_mapRemarkToInfo.end(); iter++)
	{
		if (iter->second.iTime >= m_pstConfig->stPingPang.iQueueTime)
		{
			const TOrder* pstOrder = g_pTradeData->GetOrder(iter->second.iOrderId);
			if (pstOrder && pstOrder->OrderState == osPartFilled)
				g_pTradeApi->CancelOrder(iter->second.iOrderId);			
		}
		else
		{
			iter->second.iTime++;
		}
	}
}
void PingPangOrder::InsertRemarkToInfo(char* Remark, PingPangInfo stInfo)
{
	m_mapRemarkToInfo.insert(make_pair(Remark, stInfo));
}
void PingPangOrder::GetOrderRemark(char* OrderRemark, int iSize)
{
	const char* cPrefix = "���ƹ�ҵ����:";
	++m_iNum;
	sprintf_s(OrderRemark, iSize, "%s%d", cPrefix, m_iNum);
}
void PingPangOrder::InitTSendOrderFromTMatch(TSendOrder& stOrder, const TMatch* pMatch)
{
	CConvert::DefaultSendOrder(stOrder);
	memcpy(&stOrder, pMatch, sizeof(TContractSingleKey));
	memcpy(&stOrder.UserNo, pMatch->UserNo, sizeof(pMatch->UserNo));

	//����
	stOrder.OrderQty = pMatch->MatchQty;
	//����
	if (pMatch->Direct == dBuy)
		stOrder.Direct = dSell;
	else
		stOrder.Direct = dBuy;
	//��ƽ
	if (pMatch->Offset == oCover || pMatch->Offset == oCoverT)
	{
		stOrder.Offset = oOpen;
	}	
	else if (pMatch->Offset == oOpen)
	{
		if (strcmp(pMatch->ExchangeNo, "SHFE") == 0)
			stOrder.Offset = oCoverT;
		else
			stOrder.Offset = oCover;
	}	
	//�۸�
	double dminPrice = 0.0;
	if (!GetMinPrice(&stOrder, dminPrice))
		return;
	if (stOrder.Direct == dBuy)
		stOrder.OrderPrice = pMatch->MatchPrice - dminPrice * m_pstConfig->stPingPang.iSellQty;
	else if (stOrder.Direct == dSell)
		stOrder.OrderPrice = pMatch->MatchPrice + dminPrice * m_pstConfig->stPingPang.iBuyQty;
}
bool PingPangOrder::GetMinPrice(TContractKey* pContract, double& dminPrice)
{
	dminPrice = 0.0;
	const SCommodity * pCommodity = g_pTradeData->GetCommodity(pContract);
	if (pCommodity)
	{
		dminPrice = pCommodity->PriceTick;
		return true;
	}
	return false;
}