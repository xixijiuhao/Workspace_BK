#pragma once
class  ShiftTradeApi
{
public:
	ShiftTradeApi();
public:
	bool SendShiftOrder(ShiftSendOrder& stShift);				//移仓
	void ModifyOrder(int OrderId, TSendOrder& stOrder);			//对价跟，排队跟，市价跟 （还可以利用追价（开仓时用））
	void CanCelAll(TContainer ParOrder);						//撤所有挂单;
	void CloseAll(TContainer stPosition, TContainer stOrder);	//清仓;
	void CloseOrder(TPosition* pst, double OrderPrice);			//对价平，超价平，市价平;
	void OnDealOrder(const TOrder* pst);
	void OnDealTimer();
	void Register(std::map<int, ShiftCondition>* pShiftCondition);
private:
	bool InsertShiftOrder(ShiftSendOrder& stShift);				//移仓默认策略
	bool SingleInsetShiftOrder(ShiftSendOrder& stShift);		//移仓逐笔下单
	int InsertOrder(TSendOrder& stOrder);	
	int InsertChaseOrder(TSendOrder& stOrder, ShiftSendOrder stShift, int iChaseOrderId = -1);
	void addConditionOrder(int orderId, int ConditionOrderId);
private:
	void OnDealShift(const TOrder* pst);
	void OnDealWarning(const TOrder* pst);
	void OnDealCanCelAll(const TOrder* pst);
	void OnDealCloseAll(const TOrder* pst);
	void OnDealModify(const TOrder* pst);
	void OnDealChasePrice(const TOrder* pst);
	void OnDealCondition(const TOrder* pst);
	void OnDealChaseQueue(const TOrder* pst);
	void OnDeleteConditionOrder(const TOrder* pst);
	double GetPrice(const TOrder* pOrder, TChaseOrder *pChase);
	uint GetPriceOffset(const TSendOrder* pOrder, double dFirstOrderPrice, double OrderPrice );
private:
	std::map<int, TChaseOrder> m_mapChaseOrder;			//记录追价相关结构体
	std::map<int, TConditionOrder> m_mapConditionOrder;	//记录条件单
	std::map<int, ShiftCondition> * m_mapShiftCondition;  //条件单
	std::map<int, ShiftSendOrder> m_mapShiftOrder;		//记录移仓的相关结构体
	std::map<int, TSendOrder> m_mapModifyOrder;			//改单操作（TOrder记录应该发单的信息）;
	std::vector<int> m_vcCancelOrderId;					//记录全撤的OrderId;
	std::vector<int> m_vcCoverOrderId;					//记录清仓时撤单的Orderid;
	std::vector<TPosition*> m_vcPosition;				//记录清仓时需要平调的持仓;
	std::map<int, WarningShift> m_mapWaringOrder;		//记录提前终止移仓单;
private:
	bool m_bCancelAll;		//记录是否全撤
	bool m_bCoverAll;		//记录是否全平
	bool m_bModify;			//记录是否改单
	bool m_bShift;			//记录是否移仓
	int iCoverNum, iTCoverNum;
private:
	double GetMinChangePrice(const TCommodityKey *pKey);
	void StoreShiftOrderId(int OrderId, ShiftSendOrder stShift);
	bool DefaultSendOrder(ShiftSendOrder stShift, TSendOrder& stOrder, bool bFirst = true);
	bool GetSecondShiftQty(int OrderId, ShiftSendOrder& stShift, int iMatch, uint& OrderQty);
private:
	bool CloseNum(char* UserNo, TSendOrder stOrder, int qty, bool firstclose);		//计算可用持仓
	void SingleOpen(ShiftSendOrder stShift,const TOrder* pst, TSendOrder stOrder, bool FirstOrSecond);
	void SingleClose(ShiftSendOrder stShift,const TOrder* pst, TSendOrder stOrder, bool FirstOrSecond);
	bool FirstorSecond(const TOrder* pst, bool firstclose);
	void GetOrderPrice(TSendOrder& stOrder, ShiftSendOrder stShift,bool firOrsec);
	void RecordChaseOrder(ShiftSendOrder stShift, TChaseOrder stChaseOrder, int OrderId, TOffset offset);
	void StopWarning(bool firstclose,char *ContractCode1, char *ContractCode2,int Qty1, int Qty2, int OpentQty, int CloseQty);
	void DefaultWarningShift(WarningShift &waShift, ShiftSendOrder stShift);
	void IsTriggerByCondition(int ConditionId, int newOrderId);
	
private:
	HWND m_hParent;
};