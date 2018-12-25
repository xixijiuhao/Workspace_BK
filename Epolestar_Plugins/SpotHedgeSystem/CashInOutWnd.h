#pragma once
class CashInOutWnd :public TIxFrm
{
public:
	CashInOutWnd();
	~CashInOutWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddCashInOutToList(SmsCashInoutDataRsp& specprice);			//添加到列表
	TListView			m_listCashInOut;
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
		ID_BtnQry,

		ID_LabSettleDate,
		ID_CbtSettleDate,
		ID_LabUserNo,
		ID_EditUserNo,
		ID_EditCashType,
		ID_EditCashValue,
		ID_BtnAdd,
		ID_BtnReset,

		IDM_AddCashInOut,
		IDM_Line,
		IDM_ModCashInOut,
		IDM_DelCashInOut,
		ID_ListView_CashInOut,

		ID_End
	};

private:
	void OnCreate();
	void OnPaint();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
	void OnDestroy();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	void OnLDoubleClick(WPARAM wParam, LPARAM lParam);

	void DealCashInOutList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void OnBtnQry();
	void DealDelCashInOut();
	void OnBtnAdd();
	void OnBtnReset();
	
private:
	HINSTANCE			m_hInstance;


	HMENU				m_menuCashInOut;
	SystemFrame *		m_System;

	HWND				m_beginTimePick;
	HWND				m_endTimePick;
	HWND				m_settleTimePick;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;
	TStatic				m_labEndDate;
	CxComboxTime		m_cbtEndDate;
	TStaticButton		m_btnQry;

	TStatic				m_labSettleDate;
	CxComboxTime		m_cbtSettleDate;
	TStatic				m_labUserno;
	CxEdit				m_editUserno;
	CxEdit				m_editCashType;
	CxEdit				m_editCashValue;
	TStaticButton		m_btnAdd;
	TStaticButton		m_btnReset;

	int					m_serialId;
};

