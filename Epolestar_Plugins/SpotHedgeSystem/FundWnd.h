#pragma once
class FundWnd :public TIxFrm
{
public:
	FundWnd();
	~FundWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddFundToList(SmsFundDataRsp& specprice, int index = -1);			//添加到列表
	TListView			m_listFund;
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

		ID_LabUserno,
		ID_CbUserno,

		ID_BtnQry,

		IDM_AddFund,
		IDM_Line,
		IDM_ModFund,
		IDM_DelFund,
		ID_ListView_Fund,

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

	void DealFundList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void OnBtnQry();
	void DealDelFund();
	void RefreshList();

private:
	HINSTANCE			m_hInstance;
	SystemFrame *		m_System;
	AddFundWnd			m_addFundWnd;

	HMENU				m_menuFund;

	HWND				m_beginTimePick;
	HWND				m_endTimePick;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;
	TStatic				m_labEndDate;
	CxComboxTime		m_cbtEndDate;

	//TStatic				m_labUserno;
	//CxCombox			m_cbUserno;

	TStaticButton		m_btnQry;

};

