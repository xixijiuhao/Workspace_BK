#pragma once
class ModifyOrderWnd :public TIxFrm
{
public:
	ModifyOrderWnd();
	//~ModifyOrderWnd();
	void ShowFrm(HWND hParent);
	void Register(SystemFrame* pdata);
	void InitWnd(SmsOrderDataRsp & order);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE	m_hInstance;
	enum
	{
		ID_labSerialId,
		ID_editSerialId,
		ID_labCompanyNo,
		ID_editCompanyNo,
		ID_labUserNo,
		ID_editUserNo,
		ID_labContractNo,
		ID_editContractNo,
		ID_labOrderNo,
		ID_editOrderNo,
		ID_labSystemNo,
		ID_editSystemNo,
		ID_labDirect,
		ID_editDirect,
		ID_labOffset,
		ID_editOffset,
		ID_labHedge,
		ID_editHedge,
		ID_labSettleDAate,
		ID_editSettleDAate,

		ID_labAgreementNo,
		ID_cbAgreementNo,
		ID_labOrderState,
		ID_cbOrderState,
		ID_labOrderPrice,
		ID_editOrderPrice,
		ID_labOrderQty,
		ID_editOrderQty,
		ID_labMatchPrice,
		ID_editMatchPrice,
		ID_labMatchQty,
		ID_editMatchQty,
		ID_labLocalNo,
		ID_editLocalNo,
		ID_labOrderTime,
		ID_editOrderTime,
		ID_labErrorCode,
		ID_editErrorCode,
		ID_labErrorText,
		ID_editErrorText,

		ID_btnOK,
		ID_btnCancel,
	};
	TStatic     m_labSerialId;
	CxEdit      m_editSerialId;
	TStatic     m_labCompanyNo;
	CxEdit      m_editCompanyNo;
	TStatic     m_labUserNo;
	CxEdit      m_editUserNo;
	TStatic     m_labContractNo;
	CxEdit      m_editContractNo;
	TStatic     m_labOrderNo;
	CxEdit      m_editOrderNo;
	TStatic     m_labSystemNo;
	CxEdit      m_editSystemNo;
	TStatic     m_labDirect;
	CxEdit      m_editDirect;
	TStatic     m_labOffset;
	CxEdit      m_editOffset;
	TStatic     m_labHedge;
	CxEdit      m_editHedge;
	TStatic     m_labSettleDate;
	CxEdit      m_editSettleDate;

	TStatic     m_labAgreementNo;
	CxCombox    m_cbAgreementNo;
	TStatic     m_labOrderState;
	CxCombox    m_cbOrderState;
	TStatic     m_labOrderPrice;
	CxEdit      m_editOrderPrice;
	TStatic     m_labOrderQty;
	CxEdit      m_editOrderQty;
	TStatic     m_labMatchPrice;
	CxEdit      m_editMatchPrice;
	TStatic     m_labMatchQty;
	CxEdit      m_editMatchQty;
	TStatic     m_labOrderTime;
	CxEdit      m_editOrderTime;
	TStatic     m_labLocalNo;
	CxEdit      m_editLocalNo;
	TStatic     m_labErrorCode;
	CxEdit      m_editErrorCode;
	TStatic     m_labErrorText;
	CxEdit      m_editErrorText;

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


};
