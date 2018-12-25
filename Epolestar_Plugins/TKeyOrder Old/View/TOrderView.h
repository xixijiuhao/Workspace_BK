#ifndef _T_ORDER_VIEW_H_
#define _T_ORDER_VIEW_H_

class TOrderView :public IColleague,public TIxFrm,public IOperator
{
public:
	TOrderView(IMediator *pMediator, HWND hParent);
	~TOrderView();
	void SetShowRect(RECT rect);
	void ShowFrm(int formerIndex, int nowIndex);
	int GetViewBottom(){ return (m_ptShow.y + m_iFrmHeight); }
	int GetViewRight(){ return (m_ptShow.x + m_iFrmWidth); }
	void UpdateShowData();
	void UpdatePrice();

	void UpdateView();

	void SetOrderQty(const char* szText){ m_spinLots.SetText(szText); }
	void SetOrderPrice(const char* szPrice){ m_spinPrice.SetText(szPrice); SetButtonSamePrice(szPrice); }
private:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnOrderBtn(WPARAM wParam, LPARAM lParam);
	void OnClickBtn(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtrlColor(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnPriceType();
	void OnSearchCode();
	void OnbtnUpDownPrice(HWND hwnd);
private:
	virtual void SetPriceType(wstring str);
	virtual void SetContractNo(string sCode);

private:
	void GetPrice(double &dBuyPrice, double &dSellPrice); 
	void SetButtonSamePrice(const char * szPrice);
	void SetBtnFormat(TStaticButton *pBtn);
private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();

	void SetOrderBtnPos(int left, int& top);

	void SetLabSuperPriceText(bool bSetText=true);
private:
	TStatic         m_labContractNo;
	TLockCheck		m_lockContract;
	CxEdit			m_editContractNo;
	TStaticButton	m_btnSearch;

	TStatic			m_labLots;         // 手数
	CxSpinEdit		m_spinLots;

	TStaticButton	m_btnPriceType;
	CxSpinEdit		m_spinPrice;

	TStatic			m_labelUp;         // 涨停
	TStatic			m_labelDown;       // 跌停

	TStaticButton	m_btnUpPrice;
	TStaticButton	m_btnDownPrice;

	CPopWinCode     m_popupContractCode;
	CPopWinCodeList m_popupCodeList;
	CPopWin         m_popupPriceType;

	TStatic         m_labSuperPrice;

	TOrderButton    m_btnLongBuyOpen;  // 多头开仓
	TOrderButton    m_btnShortSellOpen;// 空头开仓
	TOrderButton    m_btnClose;        // 平仓


	TStaticButton   m_btnCancel;       // 撤单
	TStaticButton   m_btnConsideration;// 对价
	TStaticButton   m_btnLineup;       // 排队跟
	TStaticButton   m_btnChase;		   // 排队追
private:
	POINT           m_ptShow;
	int             m_iFrmWidth;
	int             m_iFrmHeight;
	HWND            m_hParent;
	bool            m_bShow;
	POINT			m_ptPopupPriceType;
	POINT			m_ptSearchCode;

	bool            m_bShowCodeList;
	bool			m_bChangeWithQuote;
	UINT			m_iPriceType;
	wstring         m_wstrPriceTypeText;
	int             m_iOrderBtnLeft;
	int				m_iOrderBtnTop;
    TOrderViewData*     m_pOrderData;
	TQuoteData*		m_pQuoteData;
	SOFCommodityInf*m_pCommodityInfo;

public:
	enum{
		ID_LongBuyOpen = 1, ID_ShorSellOpen, ID_Close, 
		ID_CancelOrder, ID_Consideration,ID_LineUp, ID_Chase, 
		ID_BtnUpPrice, ID_BtnDownPrice, ID_BtnSearch, ID_editContractNo, ID_BtnPriceType,
		ID_SpinLots, ID_SpinPrice,ID_editPriceType,
	};
	enum
	{
		CMD_OrderView_InsertOrder,
		CMD_OredrView_SetContractNo,
		CMD_OrderView_LockState,
		CMD_OrderView_ClickBtn,

		CMD_OrderView_GetOrderData,
		CMD_OrderView_GetQuoteData,
		CMD_OrderView_GetCommInfo,
	};
};

#endif