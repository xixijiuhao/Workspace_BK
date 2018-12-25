#pragma once

class ExposureWnd :public TIxFrm
{
public:
	ExposureWnd();
	~ExposureWnd();
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddExposureToList(string agreeno, double spotRemain, int futureCanOpen,  int index = -1);			//添加到列表
	int	 CalculateFutureCanOpen(string agreeno);									//计算可开
	TListView			m_listExposure;
	//CxCombox			m_cbOpenAgreementNo;

	HANDLE										m_hQryMatch;

protected:
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	enum
	{
		ID_Begin,

		//ID_LabOpenAgreementNo,
		//ID_CbOpenAgreementNo,

		ID_BtnQry,

		ID_ListView_Exposure,

		ID_End
	};

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

	void DealMatchList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void OnBtnQry();

private:
	SystemFrame * m_System;
	HINSTANCE			m_hInstance;


	//TStatic				m_labOpenAgreementNo;

	TStaticButton		m_btnQry;

};