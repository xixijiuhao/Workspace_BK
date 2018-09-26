#pragma once
class TaxWnd :public TIxFrm
{
public:
	TaxWnd();
	~TaxWnd();

public:
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddTaxToList(SmsTaxDataRsp& agree);								//添加到列表
	TListView			m_listTax;											//规格列表
protected:
	enum ControlID
	{
		ID_Begin,

		ID_labBeginDate,
		ID_cbtBeginDate,
		ID_labTax,
		ID_editTax,

		ID_btnAdd,
		ID_btnReset,

		ID_ListView_Tax,

	/*	IDM_AddTax,
		IDM_Line,
		IDM_ModTax,*/
		IDM_DelTax,

		ID_End
	};
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);	//窗口处理过程
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

	void DealTaxList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void DealDelTax();
	void OnBtnAdd();
	void OnBtnReset();

private:
	HMENU				m_menuTax;
	HINSTANCE			m_hInstance;
	SystemFrame *		m_System;

	HWND				m_beginTimePick;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;

	TStatic				m_labTax;
	CxEdit				m_editTax;

	TStaticButton		m_btnAdd;
	TStaticButton		m_btnReset;

};
