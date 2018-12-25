#ifndef STOP_LP_VIEW_H
#define STOP_LP_VIEW_H


struct TStopLPViewData
{
	string sign;
	string UserNo;
	double LastPrice;//当前选中合约的最新价，会根据行情回调变动
	string RawContractID;
	double minChangePrice;
	int		iPrecision;
	int		Qty;//当前选中合约的手数,会根据持仓回调变动
	int		ProductDot;//每手乘数
	double	OpenPrice;//当前选中合约的开仓均价，会根据持仓回调变动
	double	InitDeposit;//初始保证金
	TDirect direct;
	int		iSelPage;
	TStopLPViewData()
	{
		LastPrice = 0.0;
		minChangePrice = 1.00;
		iPrecision = 1;
		Qty = 1;
		ProductDot = 1;
		OpenPrice = 0.0;
		iSelPage = 1;
		InitDeposit = 1.0;
	}
};

class TStopLPView :public TIxFrm, public IColleague
{
public:
	TStopLPView(IMediator* md);
	void Create(HWND hParent, int PageID = Page_StopLoss);
	void SwitchPage(int PageID);
	void MoveWindow(int x, int y, int cx, int cy);
	void SetTitle(string str){ m_TitleName = str; InvalidateRect(m_Hwnd, nullptr, true); }

	void SetData(int Sel, bool notMOD);//将更新的结构体Set到控件上（进阶：将止损单信息Set到Combox上）
	void UpdatePositionPrice();
	void ComboxUpdate(const TStopLPitem* pItem);
protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void OnPaint();
	void OnCreate();
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtrlColor(WPARAM wParam, LPARAM lParam);
	void OnStaticBtnClick(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnCheckBox(LPARAM lParam);
	void DealSpinUD(CxSpinEdit& spinEdit, bool Up);
	void DealSpinQty(CxSpinEdit& spinEdit, bool Up);
	void UpdateViewWidget_CBChange(int index, OrderLPType type);


	void OnStart();
	void OnStop();

	void PageOneSH(bool bShow);
	void PageTwoSH(bool bShow);
	void PageThreeSH(bool bShow);

	void CreateWidget();
	void InitWidget();
	void SetWidetPos();

	void SetComboxList();
	void AddCombox(const TStopLPitem* pitem);
	void FormComboxItemString(wstring& str,const TStopLPitem* pitem);
	void SelComboxItem(int index1, int index2, int index3);

	void GetData();
	void GetComboxData();

	void CloseWnd();
	void ShowStartBtn(bool show);//填flase就显示暂停键

	void RefreshReminder();

	OrderLPType Page2OrderLPType(const int &iPage);
	int OrderLPType2Page(const OrderLPType &Ordertype);
private:
	TStaticButton m_btnStart;
	TStaticButton m_btnPause;

	CxCombox	m_cbStopLoss;
	CxCombox	m_cbStopProfit;
	CxCombox	m_cbBreakEven;

	CxSpinEdit      m_spinSLPrice;
	CxSpinEdit      m_spinSLQty;
	CxSpinEdit      m_spinFloatPrice;
	CxSpinEdit      m_spinSPPrice;
	CxSpinEdit      m_spinSPQty;
	CxSpinEdit      m_spinBEPrice;
	CxSpinEdit      m_spinBEQty;

	CxCheck  m_ckFloat;

	TStatic m_scContract;

	TStatic m_scQty;

	TStatic m_scPrice;
	TStatic m_scFloatPrice;
	TStatic m_scReMinder;

	TStatic m_scInfo;

	enum
	{
		ID_editSLPrice,
		ID_spinSLPrice,
		ID_editSPPrice,
		ID_spinSPPrice,
		ID_editBEPrice,
		ID_spinBEPrice,
		ID_editFloatPrice,
		ID_spinFloatPrice,
		ID_editSLQty,
		ID_spinSLQty,
		ID_editSPQty,
		ID_spinSPQty,
		ID_editBEQty,
		ID_spinBEQty
	};
public:
	enum
	{
		Page_StopLoss = 1,
		Page_StopProfit,
		Page_BreakEven,

		CMD_StopLPView_InsertOrder,
		CMD_StopLPView_ModifyOrder,
		CMD_StopLPView_DeleteOrder,
		CMD_StopLPView_GetData,
		CMD_StopLPView_GetComboxData,
		CMD_StopLPView_UpdatePositionPrice
	};
private:
	int Wnd_Width;
	int Wnd_Height;
	string m_TitleName = "";

	const int CAPTION_HEIGHT = 30;
	const int FRAME_WIDTH = 395;
	const int FRAME_HEIGHT = 320;
	const int CHOOSEFRAME_HEIGHT = 24;

	COLORREF m_colorCaption;
	COLORREF m_colorBk;
	RECT m_rectClose;
	RECT m_rectMini;
	bool m_bhoverclose;
	bool m_bhovermini;
	bool m_bmousetrack;

	RECT m_rectChoice[3];
	int m_iSelPage;
	std::string m_StrCode = "";

	TStopLPViewData* m_pData = nullptr;
	TDataContainer* m_pComboxData;

	vector<const TStopLPitem*> m_vecStopLossItem;
	vector<const TStopLPitem*> m_vecStopProfitItem;
	vector<const TStopLPitem*> m_vecBreakEvenItem;

	int m_DealingKeyID_StopL = -1;
	int m_DealingKeyID_StopP = -1;
	int m_DealingKeyID_BreakE = -1;
};


#endif