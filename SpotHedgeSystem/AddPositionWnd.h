#pragma once
class AddPositionWnd :public TIxFrm, public IOperator
{
public:
	AddPositionWnd();
	//~AddPositionWnd();
	void ShowFrm(HWND hParent);
	void Register(SystemFrame* pdata);
	void InitWnd();
	void InitModifyWnd(SmsPositionDataRsp & order);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual void	SetContractNo(string sCode);
	virtual bool	SendOrder(MsgOrderInfo *msg) { return false; };										//设置订单信息 发单
	virtual bool	TableButtonToEdit() { return false; };												//Tab切换
private:
	HINSTANCE	m_hInstance;
	enum
	{
		ID_labCompanyNo,
		ID_editCompanyNo,
		ID_labUserNo,
		ID_editUserNo,
		ID_labAgreementNo,
		ID_cbAgreementNo,
		ID_labContractNo,
		ID_editContractNo,
		ID_btnContractNo,
		ID_labPositionNo,
		ID_editPositionNo,
		ID_labPositionQty,
		ID_editPositionQty,
		ID_labDirect,
		ID_editDirect,
		ID_labOffset,
		ID_editOffset,
		ID_labHedge,
		ID_editHedge,
		ID_labSettleDAate,
		ID_cbtSettleDAate,
		ID_labPositionPrice,
		ID_editPositionPrice,
		ID_labPositionProfit,
		ID_editPositionProfit,

		ID_btnOK,
		ID_btnCancel,
	};

	TStatic     m_labCompanyNo;
	CxEdit      m_editCompanyNo;
	TStatic     m_labUserNo;
	CxEdit      m_editUserNo;
	TStatic		m_labAgreementNo;
	CxCombox	m_cbAgreementNo;
	TStatic     m_labContractNo;
	CxEdit      m_editContractNo;
	TStaticButton m_btnContractNo;
	CPopWinCode			m_popCode;
	TStatic     m_labPositionNo;
	CxEdit      m_editPositionNo;
	TStatic     m_labPositionQty;
	CxEdit      m_editPositionQty;
	TStatic     m_labDirect;
	CxEdit      m_editDirect;
	TStatic     m_labOffset;
	CxEdit      m_editOffset;
	TStatic     m_labHedge;
	CxEdit      m_editHedge;
	TStatic     m_labSettleDate;
	CxComboxTime      m_cbtSettleDate;
	TStatic     m_labPositionPrice;
	CxEdit      m_editPositionPrice;
	TStatic     m_labPositionProfit;
	CxEdit      m_editPositionProfit;

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
	void OnLDoubleClick(WPARAM wParam, LPARAM lParam);
private:
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnBtnOk();
	void OnBtnCancel();
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	void OnBtnCode();

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
