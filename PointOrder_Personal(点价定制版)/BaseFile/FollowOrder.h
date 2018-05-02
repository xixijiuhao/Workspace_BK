#pragma once

class FollowOrder
{
public:
	FollowOrder();
	~FollowOrder();
public:
	void InsertOrderNoToFollow(const char* OrderNo, TSendOrder stFolloew);

public:
	void DealOrder(const TOrder* pst);

	std::map<string, TSendOrder> m_OrderNoToFollow;//¸úµ¥Ê±ºòÓÃ;
};
