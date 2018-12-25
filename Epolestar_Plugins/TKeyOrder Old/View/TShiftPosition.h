#pragma  once
class TShiftPosition : public VirtualWnd
{
public:
	TShiftPosition();
	~TShiftPosition();
public:
	void SetOrderInfo(const TPosition* pPosition);
	void Create(HWND hParent);
protected:
	virtual void OnCreate();
	virtual void OnBtnClick(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DrawMain(TMemDC& pmemdc);
	virtual void OnDestroy() { delete this; }

private:
	void OnBtnOk();
	void OnBtnCancle();
	void InitCombox();
	void PositionToSendOrder(TSendOrder& stOrder, const TPosition* p);
private:
	CxCombox m_comContract;
	CxSpinEdit m_spinQty;
	TStaticButton m_btnOK;
	TStaticButton m_btnCancle;
	TStatic m_labContract;
	TStatic m_labQty;
	int  m_iIndex;
private:
	const TPosition* m_pstPosition;
	enum { ID_BTNOK = 0, ID_BTNCANCLE };
};