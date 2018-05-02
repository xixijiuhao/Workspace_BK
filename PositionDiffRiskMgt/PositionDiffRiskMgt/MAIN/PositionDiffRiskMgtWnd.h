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
	ID_Add_Btn,
	ID_Delete_Btn,
	ID_Edit_ContractNo,
	ID_Select_Contract_Btn,
	ID_List_Default_Qty,
	ID_Add_Group_Btn,
	ID_Delete_Group_Btn,
	ID_Edit_GroupNo,
	ID_Edit_GroupName,
	ID_List_GroupInfo,
	ID_Groupbox,

	ID_End,
};

enum CancelSelectType
{
	Forbid_Cancel_Order,
	Allow_OneTime_Cancel,
};

//struct ContractConfigInfo
//{
//	CancelSelectType m_CancelSelType;
//	bool m_bNeedLessPosiRag;
//	int m_PositionRange;
//	ContractConfigInfo()
//	{
//		m_bNeedLessPosiRag = false;
//		m_CancelSelType = Forbid_Cancel_Order;
//		m_PositionRange = -1;
//	}
//};

struct ConfigData
{
	CancelSelectType m_CancelSelType;
	bool m_bNeedLessPosiRag;
	std::map<std::string, int> m_ContractPosiRangeMap;   //仓差
	bool m_bIsCommonModel;

	ConfigData()
	{
		m_bNeedLessPosiRag = false;
		m_CancelSelType = Forbid_Cancel_Order;
		m_bIsCommonModel = false;
	}
	void Clear()
	{
		m_ContractPosiRangeMap.clear();
		m_bNeedLessPosiRag = false;
		m_CancelSelType = Forbid_Cancel_Order;
		m_bIsCommonModel = false;
	}
};

class PositionDiffRiskMgtWnd : public TIxFrm, public IOperator
{
public:
	PositionDiffRiskMgtWnd();
	~PositionDiffRiskMgtWnd();
	static void OnMenuClick(const unsigned int MenuIndex, const HWND PastLife);
public:
	virtual void SetContractNo(string sCode);

	void CreateCfgWnd(const wchar_t *title);

	void QryConfigFinish();
	void GetGroupConfigFormAPI();


protected:
	virtual	LRESULT	WndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void OnCreate();
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseLeave();

	void OnButtonClickDown(WPARAM wParam, LPARAM lParam);
	void OnBtnSelect();
	void OnBtnAdd();
	void OnBtnDelete();
	void ShowList(ConfigData &cfgData);
	bool CheckInputValueInvaild(string &sCode, int &nRange);
	void CreateRiskConfigWnd();
	void CreateSetUserCfgInfoWnd();
	void ChangeFrmWithGroup(ConfigData &cfgData);

	void OnBtnGroupAdd();
	void OnBtnGroupDelete();
	void OnDealListItem();
	void SetMainGroupComboxValue();
	void ShowGroupList();
	void EnableContorl(bool isEnable);
	void CreateSetGroupInfoWnd();

private:
	void SaveConfig();
	void LoadConfig();
	//void SaveConfigWithGroup();
	//void LoadConfigWithGroup();
	//void ApiStructToUserStruct(ConfigGroupRsp &apiStruct, ConfigData &userStruct);
	//void UserStructToApiStruct(ConfigData &userStruct, ConfigGroupRsp &apiStruct);
	void SetMainGroupNOToAPI();
	void DealGroupInfoToCompare(ConfigGroupRsp &cfgGroupData);
	void SetCurGroupSigInfoToAPI(string &strKey, int valueInt);
	void DealWithContractDeleteData(string &strContractNO);
	void DealWithContractAddData(string &strContractNO, int nRange);
	void SaveGroupNoAndIndexMap(string &nUserNo, int nKeyIndex);
	void QryConfigInfo();
	void QryUserInfo();
	void GetUserInfoFormAPI();

private:
	HBRUSH m_hStaticBKBrush;		//静态文本背景画刷

	TStatic         m_labelContractNo;
	TEdit			m_EditContract;
	TStatic			m_LablePositionRange;
	TEdit			m_EditPositionRange;
	TStaticButton   m_SelectContractBtn;
	TStaticButton	m_AddBtn;
	TStaticButton	m_DeleteBtn;
	TListView       m_listDefaultQty;

	TStatic         m_labelCancelSel;
	CxCombox		m_ComboxCancelSel;
	TCheckBox		m_CheckboxHangupOrd;
	TCheckBox		m_CheckboxCancelType;
	CPopWinCode		m_popWinCode;

	TStatic			m_labelCancelSelTips;
	TStatic			m_lableTips[5];

	TStatic			m_LableGroupInfo;
	TStatic			m_LableGroupNO;
	TStatic			m_LableGroupName;
	TEdit			m_EditGroupNO;
	TEdit			m_EditGroupName;
	TStaticButton	m_AddGroupBtn;
	TStaticButton	m_DeleteGroupBtn;
	TListView       m_ListGroupInfo;
	TStatic         m_LableCurUserGroup;
	CxCombox		m_ComboxCurUserGroup;

	TStatic         m_LableCurUser;
	CxCombox		m_ComboxCurUser;
	HWND			m_hGroupbox;

public:
	ConfigData m_cfgData; //针对当前用户生效的配置
	map<string, string> m_GroupNoAndNameMap;   
	map<string, ConfigData> m_UserNoAndCfgDataMap; //自己用的数据结构，与服务器保持一致
	map<string, UserPluginRsp> m_UserNoAndUserInfoMap; //自己用的数据结构，与服务器保持一致
	ConfigGroupRsp *m_ConfigGroupData_API;
	UserPluginRsp *m_UserPluginData_API;
	map<string, vector<ConfigGroupRsp>> m_GroupInfoCompareMap;  //与服务器的数据交互，始终和服务器保持一致
	map<string, int> m_GroupNoAndIndexCnt;

	//string m_CurGroupNO;
	string m_CurUserNO;
	int m_nInitGroupCfgCnt;
	int m_nUserCfgCnt;
	string			m_MainGroupUserCfg;
	string			m_sCurUserInGroupNO; //风控分组
	bool			m_bCurUserCanModify;   //风控分组是否有修改权限
};