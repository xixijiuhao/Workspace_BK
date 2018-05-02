#include "PreInclude.h"
extern ITradeApi* g_pTradeApi;
FollowOrder::FollowOrder()
{

}
FollowOrder::~FollowOrder()
{

}

void FollowOrder::InsertOrderNoToFollow(const char* OrderNo, TSendOrder stFollow)
{
	std::map<string, TSendOrder>::iterator iter = m_OrderNoToFollow.find(OrderNo);
	if (iter != m_OrderNoToFollow.end())
	{
		m_OrderNoToFollow.erase(iter);
		m_OrderNoToFollow.insert(std::make_pair(OrderNo, stFollow));
	}
	else
		m_OrderNoToFollow.insert(std::make_pair(OrderNo, stFollow));
}
void FollowOrder::DealOrder(const TOrder* pst)
{
	if (pst==NULL)
		return;
	std::map<string, TSendOrder>::iterator iter = m_OrderNoToFollow.find(pst->OrderNo);
	if (iter == m_OrderNoToFollow.end())
		return;
	else
	{
		if (pst->OrderState == osCanceled)
		{
            InsertOrderAssitant::InsertOrder(iter->second);
			m_OrderNoToFollow.erase(iter);
		}
		else if (pst->OrderState == osFilled)
		{
			m_OrderNoToFollow.erase(iter);
		}
		
	}
}