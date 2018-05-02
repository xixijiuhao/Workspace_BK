#include "PreInclude.h"
extern ITradeApi*      g_pTradeApi;
extern ITradeData*     g_pTradeData;
void TCanCelAllOrder::RegistNotice(OnCencelNotice pclNotice)
{
	m_pNotice = pclNotice;
}
void TCanCelAllOrder::CanCelOrder(TContractSingleKey* pstContract)
{
	if (!pstContract)
		return;
	GetCancelOrderId(pstContract);
	CanCelAllParOrder();
}
void TCanCelAllOrder::OnOrder(const TOrder * pOrder)
{
	std::map<const uint, TOrder*>::iterator iter = m_mapOrderId.find(pOrder->OrderID);
	if (iter != m_mapOrderId.end())
	{
		if (pOrder->OrderState == osCanceled || pOrder->OrderState == osPartCanceled)
			m_mapOrderId.erase(iter);
		else if (pOrder->OrderState == osPartFailed || pOrder->OrderState == osFail)
			g_pTradeApi->CancelOrder(pOrder->OrderID);
		if (!m_mapOrderId.size())
			m_pNotice(true);
	}
}
void TCanCelAllOrder::GetCancelOrderId(TContractSingleKey* pstContract)
{
	if (!g_pTradeData)
		return;
	TDataContainer ParCtn;
	if (!pstContract)
		return;
	g_pTradeData->GetParOrder(ParCtn);

	for (int i = 0; i < ParCtn.size(); i++)
	{
		TOrder* pst = (TOrder*)ParCtn.at(i);
		if (CConvert::Cmp2ContractSingleKey(*pst, *pstContract))
			m_mapOrderId.insert(make_pair(pst->OrderID, pst));
	}
	if (m_mapOrderId.size() == 0)
		m_pNotice(true);
}
void TCanCelAllOrder::CanCelAllParOrder()
{

	std::map<const uint, TOrder*>::iterator iter = m_mapOrderId.begin();
	while (iter != m_mapOrderId.end())
	{
		g_pTradeApi->CancelOrder(iter->first);
		iter++;
	}
		
}