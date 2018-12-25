#pragma once
class SettlePriceWnd :public TIxFrm, public IOperator
{ 
public:
	SettlePriceWnd();
	~SettlePriceWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddSettlePriceToList(SmsSettlePriceDataRsp& specprice);			//添加到列表
	TListView			m_listSettlePrice;
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual void	SetContractNo(string sCode);
	virtual bool	SendOrder(MsgOrderInfo *msg) { return false; };										//设置订单信息 发单
	virtual bool	TableButtonToEdit() { return false; };												//Tab切换
private:
	enum
	{
		ID_Begin,

		ID_LabBeginDate,
		ID_CbtBeginDate,
		ID_LabEndDate,
		ID_CbtEndDate,

		ID_LabContractno,
		ID_EditContractno,
		ID_BtnContractno,

		ID_BtnQry,

		ID_LabSettleDate,
		ID_CbtSettleDate,

		ID_LabContractno2,
		ID_EditContractno2,
		ID_BtnContractno2,

		ID_LabSettlePrice,
		ID_EditSettlePrice,

		ID_BtnAdd,
		ID_BtnReset,


		IDM_DelSettlePrice,

		ID_ListView_SettlePrice,

		ID_End
	};

private:
	void OnCreate();
	void OnPaint();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
	void OnDestroy();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);


	void DealSettlePriceList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void OnBtnQry();
	void OnBtnCode();
	void OnBtnCode2();
	void DealDelSettlePrice();
	void OnBtnAdd();
	void OnBtnReset();
private:
	HINSTANCE			m_hInstance;
	SystemFrame *		m_System;


	HMENU				m_menuSettlePrice;

	HWND				m_beginTimePick;
	HWND				m_endTimePick;
	HWND				m_settleTimePick;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;
	TStatic				m_labEndDate;
	CxComboxTime		m_cbtEndDate;
	TStatic				m_labContractNo;
	CxEdit				m_EditContractNo;
	TStaticButton		m_btnConntractNo;
	CPopWinCode			m_popCode;
	TStaticButton		m_btnQry;

	TStatic				m_labSettleDate;
	CxComboxTime		m_cbtSettleDate;
	TStatic				m_labContractNo2;
	CxEdit				m_EditContractNo2;
	TStaticButton		m_btnConntractNo2;
	CPopWinCode			m_popCode2;
	TStatic				m_labSettlePrice;
	CxEdit				m_EditSettlePrice;
	TStaticButton		m_btnAdd;
	TStaticButton		m_btnReset;
};

