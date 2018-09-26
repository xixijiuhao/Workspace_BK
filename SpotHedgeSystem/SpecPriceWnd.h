#pragma once

class SpecPriceWnd :public TIxFrm
{ 
public:
	SpecPriceWnd();
	~SpecPriceWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddSpecPriceToList(SmsSpecPriceDataRsp& specprice, int index = -1);			//添加到列表

	TListView			m_listSpecPrice;
	CxCombox			m_cbSpecificationNo;
	CxCombox			m_cbSpecNo;
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	enum
	{
		ID_Begin,

		ID_LabBeginDate,
		ID_CbtBeginDate,
		ID_LabEndDate,
		ID_CbtEndDate,
		ID_LabSpec,
		ID_CbSpec,
		ID_BtnQry,

		ID_labSettleDate,
		ID_cbtSettleDate,
		ID_labSpec,
		ID_cbSpec,
		ID_labSpecPirce,
		ID_editSpecPirce,
		ID_btnAdd,
		ID_btnReset,
		
		IDM_DelSpecPrice,
		ID_ListView_SpecPrice,

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

	void DealSpecPriceList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void OnBtnQry();
	void DealDelSpecPrice();
	void OnBtnAdd();
	void OnBtnReset();
	void OnRefresh();

private:
	//AddAgreeementWnd	m_addAgreementWnd;
	HINSTANCE			m_hInstance;
	SystemFrame *		m_System;

	//TListView			m_listSpecPrice;
	HMENU				m_menuSpecPrice;
	HWND				m_beginTimePick;
	HWND				m_endTimePick;
	HWND				m_settleTimePick;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;
	TStatic				m_labEndDate;
	CxComboxTime		m_cbtEndDate;
	TStatic				m_labSpecificationNo;
	//CxCombox			m_cbSpecificationNo;
	TStaticButton		m_btnQry;

	TStatic				m_labSettleDate;
	CxComboxTime		m_cbtSettleDate;
	TStatic				m_labSpecNo;
	//CxCombox			m_cbSpecNo;
	TStatic				m_labSpecPirce;
	CxEdit				m_editSpecPirce;
	TStaticButton		m_btnAdd;
	TStaticButton		m_btnReset;
};