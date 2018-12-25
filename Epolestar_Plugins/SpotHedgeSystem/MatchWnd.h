#pragma once

#define	MATCH_UserNo			0
#define	MATCH_ContractNo		1
#define	MATCH_Direct			2
#define	MATCH_Offset			3
#define	MATCH_MatchPrice		4
#define	MATCH_MatchQty			5
#define	MATCH_MatchFee			6
#define	MATCH_MatchTime			7
#define	MATCH_Hedge				8
#define	MATCH_SettleDate		9
#define	MATCH_CompanyNo			10
#define	MATCH_SerialId			11
#define	MATCH_MatchNo			12
#define	MATCH_SystemNo			13
#define	MATCH_OrderNo			14
#define	MATCH_LocalNo			15
#define	MATCH_OperatorNo		16
#define	MATCH_OperateTime		17

class MatchWnd :public TIxFrm
{
public:
	MatchWnd();
	~MatchWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddMatchToList(SmsMatchDataRsp& match, int index = -1);			//添加到列表
	TListView			m_listMatch;
	AddManualCloseWnd	m_addManualCloseWnd;

	CxCombox			m_cbAgreementNo;
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

		ID_LabAgreementNo,
		ID_CbAgreementNo,

		ID_BtnQry,

		IDM_AddMatch,
		IDM_ModMatch,
		IDM_DelMatch,
		IDM_LineMatch,
		IDM_ManageManualClose,
		//IDM_AddManualClose,
		//IDM_ModManualClose,
		//IDM_DelManualClose,

		ID_ListView_Match,

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

	void DealMatchList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void OnBtnQry();
	void DealDelMatch();
	void RefreshList();
private:
	SystemFrame *		m_System;
	HINSTANCE			m_hInstance;
	AddMatchWnd			m_addMatchWnd;


	HMENU				m_menuMatch;

	HWND				m_beginTimePick;
	HWND				m_endTimePick;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;
	TStatic				m_labEndDate;
	CxComboxTime		m_cbtEndDate;

	TStatic				m_labAgreementNo;

	TStaticButton		m_btnQry;

};

