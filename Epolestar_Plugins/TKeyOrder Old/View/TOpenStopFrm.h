#ifndef _T_OPEN_STOP_FRM_H_
#define _T_OPEN_STOP_FRM_H_

class TOpenStopFrm :public VirtualWnd
{
public:
	TOpenStopFrm();
	~TOpenStopFrm();
	void Update();   //��OrderFrame�е�OnDealQuote��
	void UpdateCode();//��OrderFrame�е�SetCode�е�

	void SetOrderData(const TOrderViewData* pData){ m_pOrderData = pData; }
	void SetQuoteData(const TQuoteData* pData){ m_pQuoteData = pData; }
	void SetCommInfoData(const SOFCommodityInf* pData){ m_pCommodityInfo = pData; }
private:
	void Init();
	void SetPrice();
	void SetUpDownPrice();
	void SendOrder();
	void SetStopLPOrder(TSendOrder &stOrder);
	void GetLossAndProfitPrice(double& dLoss, double& dProfit);
	void SetStopOrderType(const double& dstopLoss, const double& dstopProfit, AutoStopInfo& stStopInfo);
	bool CheckPrice(const double& dstopLoss, const double& dstopProfit,const double& OrderPrice, TDirect direct);
protected:
	LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnCreate();  //����ҳ��ؼ�
	void DrawMain(TMemDC &pMemDC);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);

	void OnClickCheckBox(WPARAM wParam, LPARAM lParam);

private:
	bool OnClickBuyOrSell(POINT pt);
	void OnClickOrderBtn(POINT pt);
	void DrawBuySellBtn(TMemDC *pMemDC);
	void DrawOrderBtn(TMemDC *pMemDC);
	void DrawPosition(TMemDC *pMemDC);
private:
//	void SetDifferPrice(double dPrice);
	void  SetOrderPrice(); //���ü۸�ֹ��ֹӯlabel��text
	void SetLabelText(double Price, UINT label_id); //����label��text
	void SetDiffPrice(UINT edit_id); //����ֹ��ֹӯlabel��text
	void SetTickText(UINT label_id); //����ֹ��ֹӯlabel��Tick Text
private:
	TStatic  m_labels[4];//�������۸�ֹ��ۣ�ֹӯ��
	CxSpinEdit m_pEdits[4];//�������۸�ֹ��ۣ�ֹӯ��
	TStatic  m_labelInfo[6];//���ɿ������۲��ӯ���ǰ壬���壬
	TLockCheck m_lcPriceChained;//�۸�����
	CxCheck m_ckDynamic;//��̬׷��
	CxCheck	m_ckPricePoint;
private:
	RECT          m_rcBuy;
	RECT          m_rcSell;
	RECT          m_rcBtnOrder; 
	bool          m_bIsBuy;             //�����������
	bool          m_bIsCilckOrderBtn;   //�Ƿ������µ���ť
	bool          m_bIsPriceLink;       //�۸��Ƿ�������
	bool		  m_bIsDynamic;			//�Ƿ��Ǹ��ٻس�����
	bool          m_bHasSetUpDownPrice; //����Ƿ���������ͣ��
private:
	const SOFCommodityInf*  m_pCommodityInfo;
	const TQuoteData *      m_pQuoteData;
	const TOrderViewData*       m_pOrderData;
	enum{id_shand = 1,id_sprice,id_sstoploss,id_sstopprofit,
	/*	id_ehand,id_eprice,id_estroploss,id_estopprofit,*/
		id_scanopen, id_spricegap, id_sloss, id_sprofit, id_spriceup, id_spricedown,
		id_ckDynamic,id_spinLots, id_spinPrice, id_spinLoss, id_spinProfit,id_ckPricePoint,
	};//s=static,e=edit

	enum
	{
		CMD_OLPView_BindOrderData,
		CMD_OLPView_BindQuoteData,
		CMD_OLPView_BindCommInfo,
	};
};


#endif