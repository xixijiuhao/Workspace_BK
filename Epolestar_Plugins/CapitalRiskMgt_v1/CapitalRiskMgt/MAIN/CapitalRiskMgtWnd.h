#pragma once

#define SSWM_STATIC_BUTTON_CLICKDOWN		WM_USER + 10
#define SSWM_TAB_SWITCH_FOCUS				WM_USER + 11
#define SSWM_COMBOX_SELECT					WM_USER + 12
#define SSWM_COMBOX_CHECK_CHANAGE           WM_USER + 13
#define SSWM_PINEDIT_UP                     WM_USER + 14
#define SSWM_PINEDIT_DOWN                   WM_USER + 15
#define SSWM_EDIT_VK_RETURN                 WM_USER + 16
#define SSWM_ORDERBUTTON_UP                 WM_USER + 17

enum ControlID
{
	ID_Begin,
	ID_Static_,
	ID_Lable_Back_Standard,
	ID_Combox_Back_Standard,
	ID_Edit_Back_Standard_Local,
	ID_Lable_Back_Money,
	ID_Edit_Back_Precent,
	ID_Edit_Back_Local,
	ID_Checkbox_Warning,
	ID_Checkbox_Forbid,
	ID_Add_Btn,
	ID_Delete_Btn,
	ID_Select_Contract_Btn,
	ID_ListView_Contract,

	ID_End,
};

struct ConfigData
{
	int m_nMaxOpenQty;      //最大开仓量
	int m_nThreshold;       //撤单阈值
	std::map<std::string, int> m_mapPositonRange;   //仓差

	ConfigData()
	{
		m_nMaxOpenQty = -1;
		m_nThreshold = -1;
	}
};

class CapitalRiskMgtWnd : public TIxFrm, public IOperator
{
public:
	CapitalRiskMgtWnd();
	static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);
public:
	virtual void SetContractNo(string sCode);

	void CreateCfgWnd(const wchar_t *title);

public:
	WNDPROC m_procListview;

protected:
	virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void OnCreate();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void OnNotify(WPARAM wParam, LPARAM lParam);

	void OnBtnSelect();
	void OnBtnAdd();
	void OnBtnDelete();

private:
	void InitListView();
	void SaveConfig();
	void LoadConfig();
	void UpdateListView();
	void EnableCtrl(bool bEnable);

private:
	HWND m_hControl[ID_End - ID_Begin + 1];
	HBRUSH m_hStaticBKBrush;		//静态文本背景画刷

	//CxButton	m_AddBtn;
	//CxButton	m_DeleteBtn;
	//CxButton    m_SelectBtn;
	//CxButton    m_CheckPwdBtn;

	//CxEdit		m_EditPosition;
	//CxEdit      m_EditOpen;
	//CxEdit      m_EditCancel;
	//CxEdit      m_EditContract;
	//CxEdit      m_EditPassword;

	TStatic		m_LableBackStandard;
	TCombox     m_ComboxBackStandard;
	TEdit		m_EditBackStandardLocal;

	TStatic		m_LableBackMoney;
	TCombox     m_ComboxBackMoney;
	TEdit		m_EditBackPrecent;
	TEdit		m_EditBackLocal;

	TCheckBox   m_CheckboxWarning;
	TCheckBox   m_CheckboxForbid;

	TEdit			m_EditContract;
	TStaticButton   m_SelectContractBtn;
	TStaticButton	m_AddBtn;
	TStaticButton	m_DeleteBtn;
	
public:
	ConfigData m_cfgData;

	CPopWinCode m_popWinCode;

	bool m_bCheckedPwd;
	std::string m_password;
};