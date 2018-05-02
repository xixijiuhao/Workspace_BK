#ifndef _T_CHASE_ORDER_H_
#define _T_CHASE_ORDER_H_

class TChase
{
public:
	TChase(HWND timerHwnd, uint timerID);
	~TChase();
public:
	void InsertChaseOrder(const TOrder* pOrder);
	void OnOrder(const TOrder * pOrder);
	void OnTimer(WPARAM wParam);
private:
	void OnCancelOrder(const TOrder* pOrder);
	double GetMinChangePrice(const TContractKey *pKey);
	double GetPrice(const TOrder* pOrder, TChaseOrder *pChase);
	uint GetPriceOffset(const TOrder* pOrder, double dOrderPrice);
private:
	map<uint, TChaseOrder>  m_mapChaseOrder;
	stChasePrice *          m_pstChaseConfig;
	HWND					m_hwndTimer;
	uint					m_iTimerID;
};

#endif