#pragma once

#define	AGREE_UserNo			0
#define	AGREE_AgreementNo		1
#define	AGREE_AgreementType		2
#define	AGREE_SpecificationNo	3
#define	AGREE_SpotQty			4
#define	AGREE_SpotRemainQty		5
#define	AGREE_PositionQty		6
#define	AGREE_CanOpenQty		7
#define	AGREE_SpotPrice			8
#define	AGREE_AgreementState	9
#define	AGREE_SignDate			10
#define	AGREE_InsertTime		11
#define	AGREE_CompanyNo			12
#define	AGREE_SerialId			13
#define	AGREE_OperatorNo		14
#define	AGREE_OperateTime		15

class AgreementWnd :public TIxFrm
{
public:
	AgreementWnd();
	~AgreementWnd(); 
	void Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle = 0);
	void Register(SystemFrame* pdata);
	void AddAgreementToList(SmsAgreementDataRsp& agree,int index = -1);			//添加到列表1	
	void ReloadList();											//刷新列表(不重查)
	void RefreshList();											//刷新列表(重查)
	TListView			m_listAgree;							//开仓合同
	CxCombox			m_cbAgree;
	AddAgreementRelWnd	m_addAgreementRelWnd;
	CxCombox			m_cbUserno;
protected:
	enum ControlID
	{
		ID_Begin,

		ID_LabBeginDate,
		ID_CbtBeginDate,
		ID_LabEndDate,
		ID_CbtEndDate,

		ID_LabAgree,
		ID_CbAgree,

		//ID_CheckAgree,

		ID_LabUserno,
		ID_CbUserno,

		ID_ListView_Agree,
		//ID_ListView_CloseAgree,

		//IDM_AllAgreement,
		//IDM_Line1,
		IDM_AddAgreement,
		IDM_ModAgreement,
		IDM_DelAgreement,
		IDM_Line2,
		IDM_ManageAgreementRel,
		//IDM_ModAgreementRel,
		//IDM_DelAgreementRel,

		ID_BtnQry,
		ID_BtnClear,
		//ID_BtnAll,
		//ID_BtnPart,

		ID_End
	};
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void OnCreate();
	void OnPaint();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnBtnClickDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	void OnNcDestroy();
	void OnDestroy();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);

	void DealAgreeList(LPARAM lParam);
	//void DealCloseAgreeList(LPARAM lParam);
	void ShowMenu(HMENU hmenu);
	void EnableMenu(uint id, bool bEnable);
	void DealDelAgreement();
	void OnBtnQry();
	void OnBtnClear();

	std::vector<std::string> SplitString(const std::string & str, char cSplit);
	int	DateStrToInt(const std::string & str);
private:
	SystemFrame *		m_System;
	HINSTANCE			m_hInstance;
	AddAgreementWnd		m_addAgreementWnd;

	HMENU				m_menuAgree;

	HWND				m_beginTimePick;
	HWND				m_endTimePick;
	HWND				m_hwnd_checkAgree;

	TStatic				m_labBeginDate;
	CxComboxTime		m_cbtBeginDate;
	TStatic				m_labEndDate;
	CxComboxTime		m_cbtEndDate;

	TStatic				m_labAgree;
	//CxCheck				m_checkboxAgree;
	

	TStatic				m_labUserno;

	TStaticButton		m_btnQry;
	TStaticButton		m_btnClear;
	//TStaticButton		m_btn3M;
	//TStaticButton		m_btnAll;
	//TStaticButton		m_btnPart;

	//TStatic				m_labOpenAgree;				
	//TStatic				m_labCloseAgree;			
};

