#ifndef _T_CONDITION_ORDER_FRM_H_
#define _T_CONDITION_ORDER_FRM_H_

class TConditionOrderFrm :public VirtualWnd, public IColleague, public IOperator
{
public:
	TConditionOrderFrm(IMediator * pMediator);
	~TConditionOrderFrm();
	void MoveWindow();
	void Update(bool bUpdateAll =true);

	void SetOrderData(const TOrderViewData* pData){ m_pOrderData = pData; }
	void SetQuoteData(const TQuoteData* pData){ m_pQuoteData = pData; }
	void SetCommInfoData(const SOFCommodityInf* pData){ m_pCommodityInfo = pData; }

	void SetMode(bool bModify){ m_bIsModify = bModify; };
	void SetWidgetContent(TConditionOrder* pOrder, HWND hParent);
private:
	void OnCreate();
	void DrawMain(TMemDC &memdc);
	LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	void OnBtnClick(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtrlColor(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnClose();
private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();

	void ResetCheckBox();
	void ShowWidget(UINT type);
	bool OnClickCheckBox(CxCheck *pCheck, UINT type);

private:
	//new
	void InsertOrder();
	void OnPriceType();

//	uint SendPreOrder(TConditionOrder * pOrder);
	bool CheckConditionOrder(TConditionOrder* pOrder);

	void GetPriceType();

	void GetDataFromWidget(TConditionOrder* pOrder); 
protected:
	virtual void SetContractNo(string sCode){};
	virtual void SetPriceType(wstring str);
private:
	void ShowStopLoss();
	void ShowStopLPWidget();
	void HideSecond();
	void ShowFirstSecond();

	void GetStopLPInfo(TConditionOrder*pOrder);

	void SetSpinStopLp(CxSpinEdit* pEdit); //���������Сֵ

	void SetSpinStopLPContent(TConditionOrder*pOrder);
	void SetBtnPos();
private:
	TStatic         m_labelContract;       // ��ʾ��Լ
				    
	CxCheck			m_ckPriceCondition;    // �۸�������
	TStatic         m_labNewPrice;         // ��ʾ���¼�������
	CxCombox        m_cbPriceCondition;    // �۸�����
	CxSpinEdit      m_peditConditionPrice; // �����۸�
				    
	CxCheck			m_ckTimeCondition;     // ʱ��������
	TStatic         m_labelTime;           // ��ʾ ʱ�䵽�
	TDateTime       m_dtTime;              // ʱ��
				    
	CxCheck			m_ckOpenCondition;     // ���̴���������
	CxCheck			m_ckPreOrder;          // Ԥ��
				    
	TStatic         m_labelOrder;          // ��ʾ ����
	CxCombox        m_cbDirect;            // ���� or ����
	CxCombox        m_cbOffset;            // ��ƽor����
	CxSpinEdit      m_peditOrderPrice;     // �����۸�
	TStatic         m_labelYuan;           // Ԫ
	CxSpinEdit      m_peditOrderQty;       // ������
	TStatic         m_labelQty;            // ��

	TStaticButton   m_btnConfirm;
	TStaticButton   m_btnCancel;

	TStatic         m_labelNew;
	TStatic         m_labelNewPrice;
	TStatic         m_labelBuy;
	TStatic         m_labelBuyPrice;
	TStatic         m_labelSell;
	TStatic         m_labelSellPeice;

	CPopWin         m_popupPriceType;

	//16.10.13 New
	TStatic			m_labStopLoss;
	CxCombox		m_cbStopLossMethod;
	TStatic			m_labStopLP[2];
	CxSpinEdit		m_spinStopLP[2];
	TStaticButton	m_btnSimple;

	CxCheck			m_ckIsServerOrder;
private:
	int	                m_iCkSelect;
	TOffset			    m_Offset;            // ����ƽ��
	TDirect			    m_Direct;            // ��������
	TTriggerCondition	m_TriggerCondition;  // ��������
	string              m_strCode;
	POINT				m_ptPopupPriceTypeFrm;
	UINT				m_iPriceType;
	bool				m_bIsModify;

	int					m_iStopLPMethod;
	bool				m_bIsSimple;
	int					m_iInfoTextTop;
private:
	std::map<std::string, stSuperPrice>*  m_pSuperPriceMap;
	//HWND				m_hparent;
	const SOFCommodityInf* m_pCommodityInfo;
	const TQuoteData*      m_pQuoteData;
	const TOrderViewData*  m_pOrderData;
	TConditionOrder       m_stModifyOrder;
	TConditionOrder*      m_pModifyOrder;
	
	enum
	{
		id_btnConfirm,
		id_btnCancel,
		id_ckServerOrder,
		id_ckPrice,
		id_ckTime,
		id_ckOpen,
		id_ckPreOrder,
		id_cbPrice,
		id_editOrderPrice,
		id_cbDirect,
		id_cbOffset,

		id_labelNew,
		id_labelNewPrice,
		id_labelBuy,
		id_labelBuyPrice,
		id_labelSell,
		id_labelSellPrice,

		id_spinPriceCondition,
		id_spinOrderPrice,

		id_spinStopLP0,
		id_spinStopLP1,
		id_btnSimple,
	};
public:
	enum
	{
		CMD_ConView_InsertOrder,
		CMD_ConView_UpdateConOrder,
	};

	enum
	{
		stopNone,
		stopLossProfit,
		stopFloatChase,
		stopLoss,
		stopProfit,
		stopSpeciLossProfit,
	};
};

#endif