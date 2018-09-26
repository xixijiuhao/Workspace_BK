#pragma once
class AddAgreementWnd :public TIxFrm
{
public:
	AddAgreementWnd();
	//~AddAgreementWnd();
	void ShowFrm(HWND hParent);
	void Register(SystemFrame* pdata);
	void InitWnd();
	void InitModifyWnd(SmsAgreementDataRsp &rsp);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	bool			m_bMouseTrack;
	SystemFrame *	m_subSystem;
	string			m_title;
	string			m_insertTime;
private:
	HINSTANCE	m_hInstance;
	TStatic     m_labAgreementNo;
	CxEdit      m_editAgreementNo;

	TStatic     m_labUserNo;
	CxCombox    m_cbUserNo;

	TStatic     m_labAgreementType;
	CxCombox	m_cbAgreementType;

	TStatic     m_labSpotPrice;
	CxEdit      m_editSpotPrice;

	TStatic     m_labSpotQty;
	CxSpinEdit  m_spineditSpotQty;

	TStatic     m_labSignDate;
	HWND		m_timepick;
	CxComboxTime m_cbtSignDate;

	TStatic     m_labSpecificationNo;
	CxCombox	m_cbSpecificationNo;

	//TStatic   m_labPositionOffset;
	//CxCombox	m_cbPositionOffset;

	//TStatic     m_labAgreementState;
	//CxCombox	m_cbAgreementState;

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
	enum
	{
		ID_labAgreementNo,
		ID_editAgreementNo,
		ID_labUserNo,
		ID_editUserNo,
		ID_labAgreementType,
		ID_cbAgreementType,
		ID_labSpotPrice,
		ID_editSpotPrice,
		ID_labSpotQty,
		ID_editSpotQty,
		ID_spinSpotQty,
		ID_labSignDate,
		//ID_dateSignDate,
		ID_labSpecificationNo,
		ID_cbSpecificationNo,
		//ID_labPositionOffset,
		//ID_cbPositionOffset,
		ID_labAgreementState,
		ID_cbAgreementState,

		ID_btnOK,
		ID_btnCancel,
	};

};
