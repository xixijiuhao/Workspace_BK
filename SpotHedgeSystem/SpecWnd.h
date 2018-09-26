#pragma once

class SpecWnd :public TIxFrm
{
public:
	SpecWnd();
	~SpecWnd();

public:
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddSpecToList(SmsSpecDataRsp& agree);								//添加到列表
	TListView			m_listSpec;											//规格列表
protected:
	enum ControlID
	{
		ID_Begin,

		ID_labSerialID,
		ID_editSerialID,
		ID_labSpecNo,
		ID_editSpecNo,
		ID_labSpecName,
		ID_editSpecName,
		ID_labCommNo,
		ID_editCommNo,

		ID_btnAdd,
		//ID_btnModify,
		ID_btnReset,

		ID_ListView_Spec,

		//IDM_AddSpec,
		//IDM_Line,
		//IDM_ModSpec,
		IDM_DelSpec,

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

	void DealSpecList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void DealDelSpec();
	void OnBtnAdd();
	//void OnBtnMod();
	void OnBtnReset();
private:
	//AddSpecementWnd	m_addSpecWnd;
	HMENU				m_menuSpec;
	SystemFrame *		m_System;

	//TStatic				m_labSerialId;
	//CxEdit				m_editSerialId;
	TStatic				m_labSpecNo;
	CxEdit				m_editSpecNo;
	TStatic				m_labSpecName;
	CxEdit				m_editSpecName;
	TStatic				m_labCommNo;
	CxEdit				m_editCommNo;

	TStaticButton		m_bthAdd;
	//TStaticButton		m_bthModify;
	TStaticButton		m_bthReset;

};
