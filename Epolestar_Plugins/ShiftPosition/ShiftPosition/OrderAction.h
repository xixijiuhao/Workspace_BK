#pragma once
class  ShiftTradeApi
{
public:
	ShiftTradeApi();
public:
	bool SendShiftOrder(ShiftSendOrder& stShift);				//�Ʋ�
	void ModifyOrder(int OrderId, TSendOrder& stOrder);			//�Լ۸����ŶӸ����м۸� ������������׷�ۣ�����ʱ�ã���
	void CanCelAll(TContainer ParOrder);						//�����йҵ�;
	void CloseAll(TContainer stPosition, TContainer stOrder);	//���;
	void CloseOrder(TPosition* pst, double OrderPrice);			//�Լ�ƽ������ƽ���м�ƽ;
	void OnDealOrder(const TOrder* pst);
	void OnDealTimer();
	void Register(std::map<int, ShiftCondition>* pShiftCondition);
private:
	bool InsertShiftOrder(ShiftSendOrder& stShift);				//�Ʋ�Ĭ�ϲ���
	bool SingleInsetShiftOrder(ShiftSendOrder& stShift);		//�Ʋ�����µ�
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
	std::map<int, TChaseOrder> m_mapChaseOrder;			//��¼׷����ؽṹ��
	std::map<int, TConditionOrder> m_mapConditionOrder;	//��¼������
	std::map<int, ShiftCondition> * m_mapShiftCondition;  //������
	std::map<int, ShiftSendOrder> m_mapShiftOrder;		//��¼�Ʋֵ���ؽṹ��
	std::map<int, TSendOrder> m_mapModifyOrder;			//�ĵ�������TOrder��¼Ӧ�÷�������Ϣ��;
	std::vector<int> m_vcCancelOrderId;					//��¼ȫ����OrderId;
	std::vector<int> m_vcCoverOrderId;					//��¼���ʱ������Orderid;
	std::vector<TPosition*> m_vcPosition;				//��¼���ʱ��Ҫƽ���ĳֲ�;
	std::map<int, WarningShift> m_mapWaringOrder;		//��¼��ǰ��ֹ�Ʋֵ�;
private:
	bool m_bCancelAll;		//��¼�Ƿ�ȫ��
	bool m_bCoverAll;		//��¼�Ƿ�ȫƽ
	bool m_bModify;			//��¼�Ƿ�ĵ�
	bool m_bShift;			//��¼�Ƿ��Ʋ�
	int iCoverNum, iTCoverNum;
private:
	double GetMinChangePrice(const TCommodityKey *pKey);
	void StoreShiftOrderId(int OrderId, ShiftSendOrder stShift);
	bool DefaultSendOrder(ShiftSendOrder stShift, TSendOrder& stOrder, bool bFirst = true);
	bool GetSecondShiftQty(int OrderId, ShiftSendOrder& stShift, int iMatch, uint& OrderQty);
private:
	bool CloseNum(char* UserNo, TSendOrder stOrder, int qty, bool firstclose);		//������óֲ�
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