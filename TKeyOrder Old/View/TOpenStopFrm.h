#ifndef _T_OPEN_STOP_FRM_H_
#define _T_OPEN_STOP_FRM_H_

class TOpenStopFrm :public VirtualWnd
{
public:
	TOpenStopFrm();
	~TOpenStopFrm();
	void Update();   //在OrderFrame中的OnDealQuote调
	void UpdateCode();//在OrderFrame中的SetCode中调

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
	void OnCreate();  //创建页面控件
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
	void  SetOrderPrice(); //设置价格、止损、止盈label的text
	void SetLabelText(double Price, UINT label_id); //设置label的text
	void SetDiffPrice(UINT edit_id); //设置止损止盈label的text
	void SetTickText(UINT label_id); //设置止损止盈label的Tick Text
private:
	TStatic  m_labels[4];//手数，价格，止损价，止盈价
	CxSpinEdit m_pEdits[4];//手数，价格，止损价，止盈价
	TStatic  m_labelInfo[6];//最大可开数，价差，损，盈，涨板，跌板，
	TLockCheck m_lcPriceChained;//价格联动
	CxCheck m_ckDynamic;//动态追踪
	CxCheck	m_ckPricePoint;
private:
	RECT          m_rcBuy;
	RECT          m_rcSell;
	RECT          m_rcBtnOrder; 
	bool          m_bIsBuy;             //标价买卖方向
	bool          m_bIsCilckOrderBtn;   //是否点击了下单按钮
	bool          m_bIsPriceLink;       //价格是否联动、
	bool		  m_bIsDynamic;			//是否是跟踪回撤功能
	bool          m_bHasSetUpDownPrice; //标记是否设置了涨停价
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