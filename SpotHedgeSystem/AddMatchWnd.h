#pragma once
class AddMatchWnd :public TIxFrm, public IOperator
{
public:
	AddMatchWnd();
	//~AddMatchWnd();
	void ShowFrm(HWND hParent);
	void Register(SystemFrame* pdata);
	void InitWnd();
	void InitModifyWnd(SmsMatchDataRsp & match);
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual void	SetContractNo(string sCode);
	virtual bool	SendOrder(MsgOrderInfo *msg) { return false; };										//设置订单信息 发单
	virtual bool	TableButtonToEdit() { return false; };												//Tab切换
private:
	HINSTANCE	m_hInstance;
	HWND		m_settleTimePick;
	HWND		m_matchTimePick;
	enum
	{
		ID_labCompanyNo,
		ID_editCompanyNo,
		ID_labUserNo,
		ID_editUserNo,
		ID_labSettleDAate,
		ID_cbtSettleDAate,
		ID_labContractNo,
		ID_editContractNo,
		ID_btnContractNo,
		ID_labMatchNo,
		ID_editMatchNo,
		ID_labSystemNo,
		ID_editSystemNo,

		ID_labOrderNo,
		ID_editOrderNo,
		ID_labLocalNo,
		ID_editLocalNo,
		ID_labDirect,
		ID_editDirect,
		ID_labOffset,
		ID_editOffset,
		ID_labHedge,
		ID_editHedge,
		ID_labMatchTime,
		ID_cbtMatchTime,
		ID_labMatchPrice,
		ID_editMatchPrice,
		ID_labMatchQty,
		ID_editMatchQty,
		ID_labMatchFee,
		ID_editMatchFee,

		ID_btnOK,
		ID_btnCancel,
	};

	TStatic     m_labCompanyNo;
	CxEdit      m_editCompanyNo;
	TStatic     m_labUserNo;
	CxEdit      m_editUserNo;
	TStatic     m_labSettleDate;
	CxComboxTime      m_cbtSettleDate;
	TStatic     m_labContractNo;
	CxEdit      m_editContractNo;
	TStaticButton m_btnContractNo;
	CPopWinCode			m_popCode;
	TStatic     m_labMatchNo;
	CxEdit      m_editMatchNo;
	TStatic     m_labSystemNo;
	CxEdit      m_editSystemNo;

	TStatic     m_labOrderNo;
	CxEdit      m_editOrderNo;
	TStatic     m_labLocalNo;
	CxEdit      m_editLocalNo;
	TStatic     m_labDirect;
	CxEdit      m_editDirect;
	TStatic     m_labOffset;
	CxEdit      m_editOffset;
	TStatic     m_labHedge;
	CxEdit      m_editHedge;
	TStatic     m_labMatchTime;
	CxComboxTime      m_cbtMatchTime;
	TStatic     m_labMatchPrice;
	CxEdit      m_editMatchPrice;
	TStatic     m_labMatchQty;
	CxEdit      m_editMatchQty;
	TStatic     m_labMatchFee;
	CxEdit      m_editMatchFee;

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
	void OnBtnCode();
	void OnLDoubleClick(WPARAM wParam, LPARAM lParam);
private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();

	bool m_bMouseTrack;
	SystemFrame * m_subSystem;
	string	m_title;

};
