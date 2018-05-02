#pragma once

#define SSWM_STATIC_BUTTON_CLICKDOWN		WM_USER + 10
#define SSWM_TAB_SWITCH_FOCUS				WM_USER + 11
#define SSWM_COMBOX_SELECT					WM_USER + 12
#define SSWM_COMBOX_CHECK_CHANAGE           WM_USER + 13
#define SSWM_PINEDIT_UP                     WM_USER + 14
#define SSWM_PINEDIT_DOWN                   WM_USER + 15
#define SSWM_EDIT_VK_RETURN                 WM_USER + 16
#define SSWM_ORDERBUTTON_UP                 WM_USER + 17

#define AUTHENT_KEY             "123456"
#define AUTHENT_COMPANY         "EPOLESTAR"

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
	ID_Groupbox,

	ID_End,
};

enum CancelStandardEnum
{
	PreEquityEnum,					// ��Ȩ��
	FixedMoneyEnum,				// �̶��ʽ�
};

enum CancelMoneyTypeEnum
{
	PercentageEnum,					// �ٷֱ�
	FixedCancelEnum,				// �̶��س�
};

struct ConfigData
{
	CancelStandardEnum m_nCancelStandardType;      //�س���׼����
	CancelMoneyTypeEnum m_nCancelMoneyType;    //�س��ʽ�����

	double m_nFixedMoney;  //�̶��ʽ�
	double m_nPercentage;  //�ٷֱ�
	double m_nFixedCancelNum;  //�̶��س�

	bool m_bIsWarningOn;
	bool m_bIsForbidOn;

	ConfigData()
	{
		m_nCancelStandardType = PreEquityEnum;
		m_nCancelMoneyType = PercentageEnum;
		m_nFixedMoney = 0;
		m_nPercentage = 0;
		m_nFixedCancelNum = 0;
		m_bIsWarningOn = false;
		m_bIsForbidOn = true;
	}
};

class CapitalRiskMgtWnd : public TIxFrm
{
public:
	CapitalRiskMgtWnd();
	static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);
	void CreateCfgWnd(const wchar_t *title);
	void QryConfigFinish();

protected:
	virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void OnCreate();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnPaint();
	//void OnComboxSel(WPARAM wParam, LPARAM lParam);
	void SaveConfig();
	void LoadConfig();
	bool CheckDoubleInvildData(char *dText, int nPrecision);

	void CreateSetUserCfgInfoWnd();
	void ChangeFrmWithGroup(ConfigData &cfgData);
	void CreateRiskConfigWnd();
	void GetGroupConfigFormAPI();
	void SetMainUserNOToAPI();

	void DealComboxBackMoney();
	void DealEditBackPrecent();
	void DealComboxBackStandard();
private:
	HWND m_hControl[ID_End - ID_Begin + 1];
	HBRUSH m_hStaticBKBrush;		//��̬�ı�������ˢ

	TStatic		m_LableBackStandard;
	CxCombox    m_ComboxBackStandard;
	TEdit		m_EditBackStandardLocal;

	TStatic		m_LableBackMoney;
	CxCombox    m_ComboxBackMoney;
	TEdit		m_EditBackPrecent;
	TEdit		m_EditBackLocal;

	TCheckBox   m_CheckboxWarning;
	TCheckBox   m_CheckboxForbid;

	TStatic		m_LableTips;
	TStatic		m_LableRemarks[5];

	TStatic         m_LableCurUser;
	CxCombox		m_ComboxCurUser;
	HWND			m_hGroupbox;

	bool m_isFirst1;
	bool m_isFirst2;
	
public:
	ConfigData m_cfgData;

	map<string, ConfigData> m_UserNoAndCfgDataMap; //�Լ��õ����ݽṹ�������������һ��
	map<string, vector<ConfigGroupRsp>> m_GroupInfoCompareMap;  //������������ݽ�����ʼ�պͷ���������һ��
	map<string, int> m_GroupNoAndIndexCnt;

	string m_CurUserNO;
	int m_nInitGroupCfgCnt;
	string			m_MainGroupUserCfg;
	string			m_sCurUserInGroupNO;	//��ط���
	bool			m_bCurUserCanModify;   //��ط����Ƿ����޸�Ȩ��
	ConfigGroupRsp *m_ConfigGroupData_API;

};