#pragma once

#define	ORDER_UserNo			0
#define	ORDER_AgreementNo		1
#define	ORDER_ContractNo		2
#define	ORDER_Direct			3
#define	ORDER_Offset			4
#define	ORDER_OrderPrice		5
#define	ORDER_OrderQty			6
#define	ORDER_MatchPrice		7
#define	ORDER_MatchQty			8
#define	ORDER_OrderState		9
#define	ORDER_Hedge				10
#define	ORDER_SettleDate		11
#define	ORDER_CompanyNo			12
#define	ORDER_SerialId			13
#define	ORDER_OrderNo			14
#define	ORDER_SystemNo			15
#define	ORDER_LocalNo			16
#define	ORDER_ErrorCode			17
#define	ORDER_ErrorText			18
#define	ORDER_OrderTime			19
#define	ORDER_OperatorNo		20
#define	ORDER_OperateTime		21

class OrderWnd :public TIxFrm
{
public:
	OrderWnd();
	~OrderWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddOrderToList(SmsOrderDataRsp& specprice,int index = -1);			//添加到列表
	TListView			m_listOrder;
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

		//IDM_AddOrder,
		IDM_ModOrder, 
		IDM_Line,
		IDM_DelOrder,
		ID_ListView_Order,

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

	void DealOrderList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void OnBtnQry();
	void DealDelOrder();
	void RefreshList();

private:
	ModifyOrderWnd		m_modifyOdrerWnd;

	HINSTANCE			m_hInstance;
	SystemFrame *		m_System;

	HMENU				m_menuOrder;

	HWND				m_beginTimePick;
	HWND				m_endTimePick;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;
	TStatic				m_labEndDate;
	CxComboxTime		m_cbtEndDate;

	TStaticButton		m_btnQry;

};

