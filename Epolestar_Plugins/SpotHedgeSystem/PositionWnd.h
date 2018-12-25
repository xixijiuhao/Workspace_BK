#pragma once
class PositionWnd :public TIxFrm
{
public:
	PositionWnd();
	~PositionWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddPositionToList(SmsPositionDataRsp& specprice, int index = -1);			//添加到列表
	TListView			m_listPosition;
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

		IDM_AddPosition,
		IDM_Line,
		IDM_ModPosition,
		IDM_DelPosition,
		ID_ListView_Position,

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

	void DealPositionList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void OnBtnQry();
	void DealDelPosition();
	void RefreshList();

private:
	HINSTANCE			m_hInstance;
	SystemFrame *		m_System;

	HMENU				m_menuPosition;
	AddPositionWnd		m_addPositionWnd;


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

