#ifndef T_MODIFY_VIEW_H_
#define T_MODIFY_VIEW_H_

class TModifyConView
{
public:
	void RegisterConOrder(TConditionOrder* pOrder,HWND hParent, IMediator *pMediator);
private:
	TConditionOrderFrm*  m_pFrm;
	TConditionOrder*     m_pOrder;
	TOrderViewData       m_stOrderViewData;
	TQuoteData	         m_stQuoteData;
	SOFCommodityInf      m_stCommodityInfo;
};

#endif