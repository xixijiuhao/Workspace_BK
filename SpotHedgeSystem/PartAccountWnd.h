#pragma once

class PartAccountWnd :public TIxFrm
{
public:
	PartAccountWnd();
	~PartAccountWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	HANDLE						m_hQryFund;
	HANDLE						m_hQryPosition;
protected:
	enum ControlID
	{
		ID_Begin,

		ID_LabDate,
		ID_CbtDate,

		ID_BtnQry,
		ID_BtnRead,

		ID_ListPart,

		ID_End
	};
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
	void OnDestroy();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);

	void OnBtnQry();
	void OnBtnRead();
	void GenerateList();
private:
	SystemFrame * m_System;

	HWND				m_TimePick;

	TStatic				m_labDate;
	CxComboxTime		m_cbtDate;

	TStaticButton		m_btnQry;
	TStaticButton		m_btnRead;

	HINSTANCE			m_hInstance;
	//CxRichEdit			m_richeditAll;									//∂‘’Àµ•
	TListView			m_listPartAccount;
};

