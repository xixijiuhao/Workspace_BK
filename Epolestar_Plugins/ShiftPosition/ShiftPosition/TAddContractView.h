#pragma once

class TAddContractView : public TIxFrm, public IOperator
{
public:
	TAddContractView();
	void ShowFrm(HWND hParent);
	void Regist(char* sign, char* sign2);
	void InitWnd();
protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnBtnOk();
	void OnBtnCancel();
private:
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);

private:
	RECT m_rcClose;
	bool m_bMouseTrack;
	void OnPaint();
	void DrawClose(TMemDC *pMemDC);
	void OnCreate();
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
private:
	void OnSize(WPARAM wParam, LPARAM lParam);
private:
	void CreateWidget();
	void InitWidget();
	void SetWidgetPos();

private:
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnNcLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnSearchCode(LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void KeyDown(WPARAM wParam, LPARAM lParam);


private:
	TStatic     m_labFirstContract;
	TStatic     m_labSecondContract;

	CxEdit      m_editFirstContract;
	CxEdit      m_editSecondContract;

	TStaticButton   m_btnFirstContract;
	TStaticButton   m_btnSecondContract;
	CPopWinCode     m_popContractCode;
	CPopWinCodeList m_popContractList;
	bool			m_bShowCodeList;



	TStatic         m_labLine;
	TStatic         m_labSet;

	//第一腿
	TStatic			m_labFContract;
	CxCombox		m_cbFirstCondition;
	CxSpinEdit		m_spinEditFirstQty;
	//条件
	TStatic			m_labCondition;
	CxCombox        m_cbCondition;
	//第二腿
	TStatic			m_labSContract;
	CxCombox		m_cbSecondCondition;
	CxSpinEdit		m_spinEditSecondQty;

	TStaticButton   m_btnOk;
	TStaticButton   m_btnCancel;


	enum
	{
		//合约显示
		ID_LAB_FIRSTORDER = 1, ID_LAB_SECONDORDER,
		ID_EDIT_FIRSTCONTRACT, ID_EDIT_SECONDCONTRACT,
		ID_BTN_FIRSTCONTRACT, ID_BTN_SECONDCONTRACT,

		//属性设置控件;
		ID_LABLINE, ID_LABSET,
		ID_LABFIRSTCONTRACT, ID_CBFIRSTCONDITION, ID_SPINEDITFIRSTEDIT, ID_SPINEDITFIRSTSPIN,
		ID_LABCONDITION, ID_CBCONDITION,
		ID_LABSECONDCONTRACT, ID_CBSECONDCONDITION, ID_SPINEDITSECOND, ID_SPINEDITSECONDEDIT, ID_SPINEDITSECONDSPIN,
		ID_BTNOK, ID_BTNCANCEL
	};

private:
	bool m_bFirstContract;
	char m_Sign[21];
	char m_Sign2[21];
private:
	virtual void SetPriceType(wstring str){}
	virtual void SetContractNo(string sCode);
};
