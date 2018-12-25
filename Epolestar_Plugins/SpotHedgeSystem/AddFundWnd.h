#pragma once
class AddFundWnd :public TIxFrm
{
public:
	AddFundWnd();
	//~AddFundWnd();
	void ShowFrm(HWND hParent);
	void Register(SystemFrame* pdata);
	void InitWnd();
	void InitModifyWnd(SmsFundDataRsp & order);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE	m_hInstance;
	enum
	{
		ID_labSettleDAate,
		ID_cbtSettleDAate,
		ID_labCompanyNo,
		ID_editCompanyNo,
		ID_labUserNo,
		ID_editUserNo,
		ID_labFee,
		ID_editFee,
		ID_labEquity,
		ID_editEquity,
		ID_labPreEquity,
		ID_editPreEquity,
		ID_labAvailable,
		ID_editAvailable,
		ID_labPreAvailable,
		ID_editPreAvailable,
		ID_labMargin,
		ID_editMargin,
		ID_labPositionProfit,
		ID_editPositionProfit,
		ID_labCloseProfit,
		ID_editCloseProfit,
		ID_labDeliveryProfit,
		ID_editDeliveryProfit,
		ID_labCashIn,
		ID_editCashIn,
		ID_labCashOut,
		ID_editCashOut,

		ID_btnOK,
		ID_btnCancel,
	};

	TStatic     m_labSettleDate;
	CxComboxTime      m_cbtSettleDate;
	TStatic     m_labCompanyNo;
	CxEdit      m_editCompanyNo;
	TStatic     m_labUserNo;
	CxEdit      m_editUserNo;
	TStatic     m_labFee;
	CxEdit      m_editFee;
	TStatic     m_labEquity;
	CxEdit      m_editEquity;
	TStatic     m_labPreEquity;
	CxEdit      m_editPreEquity;
	TStatic     m_labAvailable;
	CxEdit      m_editAvailable;
	TStatic     m_labPreAvailable;
	CxEdit      m_editPreAvailable;
	TStatic     m_labMargin;
	CxEdit      m_editMargin;
	TStatic     m_labPositionProfit;
	CxEdit      m_editPositionProfit;
	TStatic     m_labCloseProfit;
	CxEdit      m_editCloseProfit;
	TStatic     m_labDeliveryProfit;
	CxEdit      m_editDeliveryProfit;
	TStatic     m_labCashIn;
	CxEdit      m_editCashIn;
	TStatic     m_labCashOut;
	CxEdit      m_editCashOut;

	TStaticButton   m_btnOk;
	TStaticButton   m_btnCancel;

private:
	void OnCreate();
	void OnPaint();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
	//void OnSearchCode(LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void KeyDown(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
private:
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnBtnOk();
	void OnBtnCancel();
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();

	bool m_bMouseTrack;
	SystemFrame * m_subSystem;
	HWND		m_settleTimePick;


	string	m_title;
	int		m_serialId;

};
