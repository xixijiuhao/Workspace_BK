#include "PreInclude.h"

const int N_Group_Width = 500;
#define CONFIG_FILE					"config\\CapitalRiskMgt.pri"

#define SECTION_CFG                 "config"
#define CANCEL_STANDARD_TYPE		"cancel_standard"
#define FIXED_MONEY					"fixed_money"
#define CANCEL_MONEY_TYPE			"cancel_money"
#define PERCENTAGE					"percentage"
#define FIXED_CANCEL				"fixed_cancel"
#define ON_FORBID					"on_forbid"
#define ON_WARNING					"on_warning"
#define MAIN_GROUP_USER_CFG			"main_group_user_cfg"

enum
{
	Index_CANCEL_STANDARD_TYPE = 0,
	Index_FIXED_MONEY,
	Index_CANCEL_MONEY_TYPE,
	Index_PERCENTAGE,
	Index_FIXED_CANCEL,
	Index_ON_FORBID,
	Index_ON_WARNING
};

extern G_COLOREF g_ColorRefData;
extern G_UIFONT	g_FontData;
extern IConfigFrame	    *g_conFrame;
extern IStarApi		    *g_StarApi;
extern CapitalRiskMgtWnd *g_pCapitalRiskMgtWnd;
extern LanguageID		 g_LanguageID;		//全局 语言资源
extern IAuthent          *g_authent;

inline double DealDoubleDot(double d)
{
	int d1 = floor(d * 100 + 0.5);
	return (double)d1 / 100;
}

inline std::string WcharToChar(const wchar_t* wp, size_t m_encode = CP_ACP)
{
	std::string str;
	int len = WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	char *m_char = new char[len + 1];
	WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	str = m_char;
	delete m_char;
	return str;
}

void CapitalRiskMgtWnd::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	g_conFrame->show(LoadResWchar_t(IDS_Capital_Risk_Management), cmtAdvance);
}

CapitalRiskMgtWnd::CapitalRiskMgtWnd()
{
	m_nInitGroupCfgCnt = 0;
	m_ConfigGroupData_API = nullptr;
	QryConfigFinish();
}

void CapitalRiskMgtWnd::QryConfigFinish()
{
	QryUserInfo();
	QryConfigInfo();
}

void CapitalRiskMgtWnd::QryUserInfo()
{
	m_nUserCfgCnt = 0;
	g_authent->GetPlugUser(m_nUserCfgCnt);
	m_UserPluginData_API = new UserPluginRsp[m_nUserCfgCnt];
	memset(m_UserPluginData_API, 0, m_nUserCfgCnt * sizeof(UserPluginRsp));
	g_authent->GetPlugUser(m_nUserCfgCnt, m_UserPluginData_API);
	if (m_nUserCfgCnt > 0)
	{
		GetUserInfoFormAPI();
	}
}

void CapitalRiskMgtWnd::QryConfigInfo()
{
	m_nInitGroupCfgCnt = 0;
	g_authent->GetGroupConfigInfo(m_nInitGroupCfgCnt);
	m_ConfigGroupData_API = new ConfigGroupRsp[m_nInitGroupCfgCnt];
	memset(m_ConfigGroupData_API, 0, m_nInitGroupCfgCnt * sizeof(ConfigGroupRsp));
	g_authent->GetGroupConfigInfo(m_nInitGroupCfgCnt, m_ConfigGroupData_API);

	//数据预处理
	if (m_nInitGroupCfgCnt > 0)
	{
		GetGroupConfigFormAPI();
	}
}

void CapitalRiskMgtWnd::CreateCfgWnd(const wchar_t * title)
{
	CreateFrm(title, NULL, WS_OVERLAPPED, 0);
}

LRESULT CapitalRiskMgtWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		break;
	case WM_CTLCOLORSTATIC:
		return (LONG)m_hStaticBKBrush;
	case WM_PAINT:
		OnPaint();
		break;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		break;
	case WM_DESTROY:
		SetMainUserNOToAPI();
		//SaveConfig();
		break;
	//case SSWM_COMBOX_SELECT:
	//	OnComboxSel(wParam,lParam);
	//	break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void CapitalRiskMgtWnd::OnCreate()
{
	auto iterFind = m_UserNoAndCfgDataMap.find(m_MainGroupUserCfg);
	//TODO 存储当前用户的风控配置
	if (iterFind != m_UserNoAndCfgDataMap.end())
	{
		m_cfgData = iterFind->second;
	}

	CreateRiskConfigWnd();
	if (!m_bCurUserCanModify)
	{
		EnableControls();
	}
	else
	{
		CreateSetUserCfgInfoWnd();
	}
}

void CapitalRiskMgtWnd::EnableControls()
{
	EnableWindow(m_ComboxBackStandard.GetHwnd(), false);
	InvalidateRect(m_ComboxBackStandard.GetHwnd(), 0, true);

	m_EditBackStandardLocal.EnableEdit(false);

	EnableWindow(m_ComboxBackMoney.GetHwnd(), false);
	InvalidateRect(m_ComboxBackMoney.GetHwnd(), 0, true);

	m_EditBackPrecent.EnableEdit(false);

	m_EditBackLocal.EnableEdit(false);

	EnableWindow(m_CheckboxWarning.GetHwnd(), false);
	InvalidateRect(m_CheckboxWarning.GetHwnd(), 0, true);

	EnableWindow(m_CheckboxForbid.GetHwnd(), false);
	InvalidateRect(m_CheckboxForbid.GetHwnd(), 0, true);

	EnableWindow(m_ComboxCurUser.GetHwnd(), false);
	InvalidateRect(m_ComboxCurUser.GetHwnd(), 0, true);
}

void CapitalRiskMgtWnd::CreateRiskConfigWnd()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	m_hStaticBKBrush = g_ColorRefData.GetHbrWhite();//背景色
	char strTemp[11] = "";
	std::string cfg_file = FileFun::GetAbsolutePathForExe(CONFIG_FILE);

	int nLeftStart = 12;
	int nTop = 10;
	int nLabelWidth = (g_LanguageID == ENU) ? 90 : 70;
	int nEditWidth = 120;
	int nComboxWidth = 100;
	int nUniformHeight = 22;

	int nInterval = 8;
	RECT firstColRect = { nLeftStart, nTop, 0, 0 };
	RECT secondColRect = { nLeftStart + nLabelWidth + 10, nTop, 0, 0 };
	RECT thirdColRect = { nLeftStart + nLabelWidth + nComboxWidth + 20, nTop, 0, 0 };

	//回撤基准 
	m_LableBackStandard.Create(m_Hwnd, LoadResWchar_t(IDS_Cancel_Standard), ID_Lable_Back_Standard);
	m_LableBackStandard.SetFont(g_FontData.GetFontNum13());
	m_LableBackStandard.MoveWindow(firstColRect.left, firstColRect.top, nLabelWidth, nUniformHeight);
	m_ComboxBackStandard.Create(m_Hwnd);
	m_ComboxBackStandard.AddString(LoadResWchar_t(IDS_PreEquity));
	m_ComboxBackStandard.AddString(LoadResWchar_t(IDS_Fixed_Money));
	m_ComboxBackStandard.SetSelect(m_cfgData.m_nCancelStandardType);
	m_ComboxBackStandard.SetFont(g_FontData.GetFontNum13());
	m_ComboxBackStandard.MoveWindow(secondColRect.left, secondColRect.top, nComboxWidth, nUniformHeight * 3);
	m_EditBackStandardLocal.Create(m_Hwnd, 0, 0, ID_Edit_Back_Standard_Local);
	m_EditBackStandardLocal.SetFont(g_FontData.GetFontNum13());
	m_EditBackStandardLocal.MoveWindow(thirdColRect.left, thirdColRect.top, nEditWidth, nUniformHeight);
	if (m_cfgData.m_nCancelStandardType == PreEquityEnum)
	{
		m_EditBackStandardLocal.SetText("");
		m_EditBackStandardLocal.EnableEdit(false);
	}
	else if (m_cfgData.m_nCancelStandardType == FixedMoneyEnum)
	{
		m_EditBackStandardLocal.EnableEdit(true);
		m_EditBackStandardLocal.SetDoubleData(m_cfgData.m_nFixedMoney);
	}

	firstColRect.top += 35;
	secondColRect.top += 35;
	thirdColRect.top += 35;
	//回撤资金
	m_LableBackMoney.Create(m_Hwnd, LoadResWchar_t(IDS_Cancel_Money), ID_Lable_Back_Money);
	m_LableBackMoney.SetFont(g_FontData.GetFontNum13());
	m_LableBackMoney.MoveWindow(firstColRect.left, firstColRect.top, nLabelWidth, nUniformHeight);
	m_ComboxBackMoney.Create(m_Hwnd);
	m_ComboxBackMoney.AddString(LoadResWchar_t(IDS_Percentage));
	m_ComboxBackMoney.AddString(LoadResWchar_t(IDS_Fixed_Cancel));
	m_ComboxBackMoney.SetSelect(m_cfgData.m_nCancelMoneyType);
	m_ComboxBackMoney.SetFont(g_FontData.GetFontNum13());
	m_ComboxBackMoney.MoveWindow(secondColRect.left, secondColRect.top, nComboxWidth, nUniformHeight * 3);
	m_EditBackPrecent.Create(m_Hwnd, 0, 0, ID_Edit_Back_Precent);
	m_EditBackPrecent.SetFont(g_FontData.GetFontNum13());
	m_EditBackPrecent.MoveWindow(thirdColRect.left, thirdColRect.top, nEditWidth, nUniformHeight);
	if (m_cfgData.m_nCancelMoneyType == PercentageEnum)
	{
		m_EditBackPrecent.SetDoubleData(m_cfgData.m_nPercentage);
	}
	else if (m_cfgData.m_nCancelMoneyType == FixedCancelEnum)
	{
		m_EditBackPrecent.SetDoubleData(m_cfgData.m_nFixedCancelNum);
	}

	//告警
	firstColRect.top += 35;
	secondColRect.top += 35;
	thirdColRect.top += 35;
	m_CheckboxWarning.Create(m_Hwnd, WcharToChar(LoadResWchar_t(IDS_Warning_Issued)).c_str());
	m_CheckboxWarning.SetFont(g_FontData.GetFontNum13());
	m_CheckboxWarning.SetBkColor(g_ColorRefData.g_ColorWhite);
	m_CheckboxWarning.SetValue(m_cfgData.m_bIsWarningOn);
	m_CheckboxWarning.MoveWindow(firstColRect.left, firstColRect.top, 300, 25);

	//禁止开仓
	firstColRect.top += 35;
	secondColRect.top += 35;
	thirdColRect.top += 35;
	m_CheckboxForbid.Create(m_Hwnd, WcharToChar(LoadResWchar_t(IDS_Forbid_Open_Qty)).c_str());
	m_CheckboxForbid.SetFont(g_FontData.GetFontNum13());
	m_CheckboxForbid.SetBkColor(g_ColorRefData.g_ColorWhite);
	m_CheckboxForbid.SetValue(m_cfgData.m_bIsForbidOn);
	m_CheckboxForbid.MoveWindow(firstColRect.left, firstColRect.top, 300, 25);

	firstColRect.top += 130;
	m_LableTips.Create(m_Hwnd, LoadResWchar_t(IDS_Tips));
	m_LableTips.MoveWindow(firstColRect.left, firstColRect.top, 300, nUniformHeight);

	for (int i = 0; i < 5; ++i)
	{
		firstColRect.top += 20;
		m_LableRemarks[i].Create(m_Hwnd, LoadResWchar_t(IDS_Remarks + i));
		m_LableRemarks[i].MoveWindow(firstColRect.left, firstColRect.top, 800, nUniformHeight);
	}
}

void CapitalRiskMgtWnd::CreateSetUserCfgInfoWnd()
{
	int nLeftStart = 412;
	int nTop = 10;
	int nLabelWidth = 80;
	int nEditWidth = 80;
	int nFirstColWidth = 90;
	int nUniformHeight = 20;

	RECT firstColRect = { nLeftStart, nTop, 0, 0 };
	RECT secondColRect = { nLeftStart + nFirstColWidth, nTop, 0, 0 };

	firstColRect.top += 25;
	m_LableCurUser.Create(m_Hwnd, LoadResWchar_t(IDS_UserSelect));
	m_LableCurUser.MoveWindow(firstColRect.left, firstColRect.top, 170, nUniformHeight);

	firstColRect.top += 25;
	m_ComboxCurUser.Create(m_Hwnd);
	m_ComboxCurUser.MoveWindow(firstColRect.left, firstColRect.top, 100, nUniformHeight * 10);
	SetUserComboxValue();

	HINSTANCE hInstance = GetModuleHandle(NULL);
	m_hGroupbox = CreateWindowEx(0, L"button", LoadResWchar_t(IDS_UserParameterSettings), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		nLeftStart - 15, nTop, 200, 100, m_Hwnd, (HMENU)ID_Groupbox, hInstance, NULL);
	SendMessage(m_hGroupbox, WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
}

void CapitalRiskMgtWnd::ChangeFrmWithGroup(ConfigData &cfgData)
{
	m_ComboxBackStandard.SetSelect(cfgData.m_nCancelStandardType);
	string value = "";
	if (cfgData.m_nCancelStandardType == PreEquityEnum)
	{
		m_EditBackStandardLocal.EnableEdit(false);
		m_EditBackStandardLocal.SetText("");
	}
	else if (cfgData.m_nCancelStandardType == FixedCancelEnum)
	{
		m_EditBackStandardLocal.EnableEdit(true);
		value = to_string(cfgData.m_nFixedMoney);
		value = value.substr(0, value.size() - 4);
		m_EditBackStandardLocal.SetText(value.c_str());
	}

	m_ComboxBackMoney.SetSelect(cfgData.m_nCancelMoneyType);

	if (cfgData.m_nCancelMoneyType == PercentageEnum)
	{
		value = to_string(cfgData.m_nPercentage);
		value = value.substr(0, value.size() - 4);
		m_EditBackPrecent.SetText(value.c_str());
	}
	else if (cfgData.m_nCancelMoneyType == FixedCancelEnum)
	{
		value = to_string(cfgData.m_nFixedCancelNum);
		value = value.substr(0, value.size() - 4);
		m_EditBackPrecent.SetText(value.c_str());
	}
	m_CheckboxWarning.SetValue(cfgData.m_bIsWarningOn);
	m_CheckboxForbid.SetValue(cfgData.m_bIsForbidOn);
}

void CapitalRiskMgtWnd::SetUserComboxValue()
{
	m_ComboxCurUser.Clear();
	int index = 0;
	int index2 = 0;
	auto Iter = m_UserNoAndUserInfoMap.begin();
	for (; Iter != m_UserNoAndUserInfoMap.end(); ++Iter)
	{
		if (m_UserNoAndUserInfoMap.find(m_MainGroupUserCfg) != m_UserNoAndUserInfoMap.end())
		{
			index2 = index;
		}
		m_ComboxCurUser.AddString(LoadRC::ansi_to_unicode(Iter->first).c_str());
		++index;
	}
	m_ComboxCurUser.SetSelect(index2);
}

void CapitalRiskMgtWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

	HWND hwnd = (HWND)lParam;
	string sCurUserNo = m_ComboxCurUser.GetText();
	auto userIter = m_UserNoAndUserInfoMap.find(sCurUserNo);
	if (userIter != m_UserNoAndUserInfoMap.end())
	{
		m_sCurUserInGroupNO = userIter->second.GroupNo;
	}
	bool isSuccess = false;

	ConfigGroupAdd cnfGroupAdd;
	strcpy_s(cnfGroupAdd.UserNo, sCurUserNo.c_str());
	strcpy_s(cnfGroupAdd.GroupNo, m_sCurUserInGroupNO.c_str());
	//固定资金Edit
	if (hwnd == m_EditBackStandardLocal.GetHwnd())
	{
		if (sCurUserNo == "") return;
		m_UserNoAndCfgDataMap[sCurUserNo].m_nFixedMoney = m_EditBackStandardLocal.GetDoubleData();
		strcpy_s(cnfGroupAdd.Key, FIXED_MONEY);
		cnfGroupAdd.KeyIndex = Index_FIXED_MONEY;
		cnfGroupAdd.ValueDouble = m_EditBackStandardLocal.GetDoubleData();
		isSuccess = g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);
	} 
	//回撤资金Edit
	else if(hwnd == m_EditBackPrecent.GetHwnd())
	{
		DealEditBackPrecent();
	}
	else if (hwnd == m_CheckboxForbid.GetHwnd())
	{
		if (sCurUserNo == "") return;
		m_UserNoAndCfgDataMap[sCurUserNo].m_bIsForbidOn = m_CheckboxForbid.GetValue();
		strcpy_s(cnfGroupAdd.Key, ON_FORBID);
		cnfGroupAdd.KeyIndex = Index_ON_FORBID;
		cnfGroupAdd.ValueInt = (int)m_CheckboxForbid.GetValue();
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);
	}
	else if (hwnd == m_CheckboxWarning.GetHwnd())
	{
		if (sCurUserNo == "") return;
		m_UserNoAndCfgDataMap[sCurUserNo].m_bIsWarningOn = m_CheckboxWarning.GetValue();
		strcpy_s(cnfGroupAdd.Key, ON_WARNING);
		cnfGroupAdd.KeyIndex = Index_ON_WARNING;
		cnfGroupAdd.ValueInt = (int)m_CheckboxWarning.GetValue();
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);
	}
	else if (hwnd == m_ComboxBackStandard.GetHwnd())
	{
		if (sCurUserNo == "") return;
		DealComboxBackStandard();
	}
	else if (hwnd == m_ComboxBackMoney.GetHwnd())
	{
		if (sCurUserNo == "") return;
		DealComboxBackMoney();
	}
	else if (hwnd == m_ComboxCurUser.GetHwnd())
	{
		sCurUserNo = m_ComboxCurUser.GetText();
		//添加切换分组的界面操作
		ConfigData cfgData;
		if (sCurUserNo != "")
			cfgData = m_UserNoAndCfgDataMap[sCurUserNo];
		ChangeFrmWithGroup(cfgData);
	}
}

void CapitalRiskMgtWnd::DealComboxBackMoney()
{
	string sCurUserNo = m_ComboxCurUser.GetText();
	ConfigGroupAdd cnfGroupAdd;
	strcpy_s(cnfGroupAdd.UserNo, sCurUserNo.c_str());
	strcpy_s(cnfGroupAdd.GroupNo, m_sCurUserInGroupNO.c_str());

	CancelMoneyTypeEnum tmp = m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelMoneyType;
	string tmpWstr = WcharToChar(LoadResWchar_t(IDS_Fixed_Cancel));

	if (strcmp(m_ComboxBackMoney.GetText(), tmpWstr.c_str()) == 0)
	{
		m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelMoneyType = FixedCancelEnum;
	}
	else
	{
		m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelMoneyType = PercentageEnum;
	}

	if (tmp != m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelMoneyType)
	{
		m_EditBackPrecent.SetText("");

		m_UserNoAndCfgDataMap[sCurUserNo].m_nPercentage = 0.0f;
		strcpy_s(cnfGroupAdd.Key, PERCENTAGE);
		cnfGroupAdd.KeyIndex = Index_PERCENTAGE;
		cnfGroupAdd.ValueDouble = 0.0f;
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);

		m_UserNoAndCfgDataMap[sCurUserNo].m_nFixedCancelNum = 0.0f;
		strcpy_s(cnfGroupAdd.Key, FIXED_CANCEL);
		cnfGroupAdd.KeyIndex = Index_FIXED_CANCEL;
		cnfGroupAdd.ValueDouble = 0.0f;
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);
	}

	strcpy_s(cnfGroupAdd.Key, CANCEL_MONEY_TYPE);
	cnfGroupAdd.KeyIndex = Index_CANCEL_MONEY_TYPE;
	cnfGroupAdd.ValueInt = (int)m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelMoneyType;
	g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);
}

void CapitalRiskMgtWnd::DealComboxBackStandard()
{
	string sCurUserNo = m_ComboxCurUser.GetText();
	ConfigGroupAdd cnfGroupAdd;
	strcpy_s(cnfGroupAdd.UserNo, sCurUserNo.c_str());
	strcpy_s(cnfGroupAdd.GroupNo, m_sCurUserInGroupNO.c_str());

	string tmpWstr = WcharToChar(LoadResWchar_t(IDS_Fixed_Money));
	if (strcmp(m_ComboxBackStandard.GetText(), tmpWstr.c_str()) == 0)
	{
		m_EditBackStandardLocal.EnableEdit(true);
		m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelStandardType = FixedMoneyEnum;
	}
	else
	{
		m_EditBackStandardLocal.SetText("");
		m_EditBackStandardLocal.EnableEdit(false);
		m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelStandardType = PreEquityEnum;

		m_UserNoAndCfgDataMap[sCurUserNo].m_nFixedCancelNum = 0.0f;
		strcpy_s(cnfGroupAdd.Key, FIXED_CANCEL);
		cnfGroupAdd.KeyIndex = Index_FIXED_CANCEL;
		cnfGroupAdd.ValueDouble = 0.0f;
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);
	}

	strcpy_s(cnfGroupAdd.Key, CANCEL_STANDARD_TYPE);
	cnfGroupAdd.KeyIndex = Index_CANCEL_STANDARD_TYPE;
	cnfGroupAdd.ValueInt = (int)m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelStandardType;
	g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);
}

void CapitalRiskMgtWnd::DealEditBackPrecent()
{
	string sCurUserNo = m_ComboxCurUser.GetText();
	if (sCurUserNo == "") return;
	ConfigGroupAdd cnfGroupAdd;
	strcpy_s(cnfGroupAdd.UserNo, sCurUserNo.c_str());
	strcpy_s(cnfGroupAdd.GroupNo, m_sCurUserInGroupNO.c_str());

	if (m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelMoneyType == PercentageEnum)
	{
		m_UserNoAndCfgDataMap[sCurUserNo].m_nPercentage = m_EditBackPrecent.GetDoubleData();
		strcpy_s(cnfGroupAdd.Key, PERCENTAGE);
		cnfGroupAdd.KeyIndex = Index_PERCENTAGE;
		cnfGroupAdd.ValueDouble = m_EditBackPrecent.GetDoubleData();
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);

		m_UserNoAndCfgDataMap[sCurUserNo].m_nFixedCancelNum = 0.0f;
		strcpy_s(cnfGroupAdd.Key, FIXED_CANCEL);
		cnfGroupAdd.KeyIndex = Index_FIXED_CANCEL;
		cnfGroupAdd.ValueDouble = 0.0f;
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);

	}
	else if (m_UserNoAndCfgDataMap[sCurUserNo].m_nCancelMoneyType == FixedCancelEnum)
	{
		m_UserNoAndCfgDataMap[sCurUserNo].m_nFixedCancelNum = m_EditBackPrecent.GetDoubleData();
		strcpy_s(cnfGroupAdd.Key, FIXED_CANCEL);
		cnfGroupAdd.KeyIndex = Index_FIXED_CANCEL;
		cnfGroupAdd.ValueDouble = m_EditBackPrecent.GetDoubleData();
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);

		m_UserNoAndCfgDataMap[sCurUserNo].m_nPercentage = 0.0f;
		strcpy_s(cnfGroupAdd.Key, PERCENTAGE);
		cnfGroupAdd.KeyIndex = Index_PERCENTAGE;
		cnfGroupAdd.ValueDouble = 0.0f;
		g_authent->AddGroupConfigInfo(PLUGIN_NAME, cnfGroupAdd);
	}
}

void CapitalRiskMgtWnd::OnPaint()
{
	HDC			hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(m_Hwnd, &ps);
	RECT item;
	GetClientRect(m_Hwnd, &item);

	FillRect(hdc, &item, g_ColorRefData.GetHbrWhite());
	EndPaint(m_Hwnd, &ps);
}

//void CapitalRiskMgtWnd::OnComboxSel(WPARAM wParam, LPARAM lParam)
//{
//	HWND hwnd = (HWND)lParam;
//	if (hwnd == m_ComboxBackStandard.GetHwnd())
//	{
//		string tmpWstr = WcharToChar(LoadResWchar_t(IDS_Fixed_Money));
//		//const char *tcStandardStr = tmpWstr.c_str();
//		//const char *tcCurStr = m_ComboxBackStandard.GetText();
//		if (strcmp(m_ComboxBackStandard.GetText(), tmpWstr.c_str()) == 0)
//		{
//			m_EditBackStandardLocal.EnableEdit(true);
//			m_cfgData.m_nCancelStandardType = FixedMoneyEnum;
//		}
//		else
//		{
//			m_EditBackStandardLocal.EnableEdit(false);
//		}
//	}
//	else if (hwnd == m_ComboxBackMoney.GetHwnd())
//	{
//		string tmpWstr = WcharToChar(LoadResWchar_t(IDS_Fixed_Cancel));
//		//const char *tcStandardStr = tmpWstr.c_str();
//		//const char *tcCurStr = m_ComboxBackStandard.GetText();
//		if (strcmp(m_ComboxBackMoney.GetText(), tmpWstr.c_str()) == 0)
//		{
//			m_cfgData.m_nCancelMoneyType = FixedCancelEnum;
//		}
//	}
//}

void CapitalRiskMgtWnd::LoadConfig()
{
	char strTemp[11] = "";
	std::string cfg_file = FileFun::GetAbsolutePathForExe(CONFIG_FILE);
	m_cfgData.m_nCancelStandardType = (CancelStandardEnum)GetPrivateProfileIntA(SECTION_CFG, CANCEL_STANDARD_TYPE, -1, cfg_file.c_str());

	GetPrivateProfileStringA(SECTION_CFG, FIXED_MONEY, "", strTemp, 10, cfg_file.c_str());
	m_cfgData.m_nFixedMoney = atof(strTemp);

	m_cfgData.m_nCancelMoneyType = (CancelMoneyTypeEnum)GetPrivateProfileIntA(SECTION_CFG, CANCEL_MONEY_TYPE, -1, cfg_file.c_str());

	GetPrivateProfileStringA(SECTION_CFG, PERCENTAGE, "", strTemp, 10, cfg_file.c_str());
	m_cfgData.m_nPercentage = atof(strTemp);

	GetPrivateProfileStringA(SECTION_CFG, FIXED_CANCEL, "", strTemp, 10, cfg_file.c_str());
	m_cfgData.m_nFixedCancelNum = atof(strTemp);

	m_cfgData.m_bIsForbidOn = (bool)GetPrivateProfileIntA(SECTION_CFG, ON_FORBID, -1, cfg_file.c_str());

	m_cfgData.m_bIsWarningOn = (bool)GetPrivateProfileIntA(SECTION_CFG, ON_WARNING, -1, cfg_file.c_str());
}

void CapitalRiskMgtWnd::SaveConfig()
{
	std::string cfg_file = FileFun::GetAbsolutePathForExe(CONFIG_FILE);
	//清空内容
	std::ofstream ofs;
	ofs.open(cfg_file, std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	char sText[10] = { 0 };
	std::string value = to_string(m_cfgData.m_nCancelStandardType);
	//value = value.substr(0, value.size() - 4);
	WritePrivateProfileStringA(SECTION_CFG, CANCEL_STANDARD_TYPE, value.c_str(), cfg_file.c_str());

	value = to_string(m_cfgData.m_nFixedMoney);
	value = value.substr(0, value.size() - 4);
	WritePrivateProfileStringA(SECTION_CFG, FIXED_MONEY, value.c_str(), cfg_file.c_str());

	value = to_string(m_cfgData.m_nCancelMoneyType);
	value = value.substr(0, value.size() - 4);
	WritePrivateProfileStringA(SECTION_CFG, CANCEL_MONEY_TYPE, value.c_str(), cfg_file.c_str());

	if (m_cfgData.m_nCancelMoneyType == PercentageEnum)
	{
		value = to_string(m_cfgData.m_nPercentage);
		value = value.substr(0, value.size() - 4);
		WritePrivateProfileStringA(SECTION_CFG, PERCENTAGE, value.c_str(), cfg_file.c_str());

		value = "0";
		WritePrivateProfileStringA(SECTION_CFG, FIXED_CANCEL, value.c_str(), cfg_file.c_str());
	}
	else
	{
		value = "0";
		WritePrivateProfileStringA(SECTION_CFG, PERCENTAGE, value.c_str(), cfg_file.c_str());

		value = to_string(m_cfgData.m_nFixedCancelNum);
		value = value.substr(0, value.size() - 4);
		WritePrivateProfileStringA(SECTION_CFG, FIXED_CANCEL, value.c_str(), cfg_file.c_str());
	}
	
	value = to_string(m_cfgData.m_bIsForbidOn);
	//value = value.substr(0, value.size() - 4);
	WritePrivateProfileStringA(SECTION_CFG, ON_FORBID, value.c_str(), cfg_file.c_str());

	value = to_string(m_cfgData.m_bIsWarningOn);
	//value = value.substr(0, value.size() - 4);
	WritePrivateProfileStringA(SECTION_CFG, ON_WARNING, value.c_str(), cfg_file.c_str());
}

bool CapitalRiskMgtWnd::CheckDoubleInvildData(char *dText, int nPrecision)
{
	string tmpStr = dText;
	int dotPos = tmpStr.find('.');
	if (dotPos >= 0 && tmpStr.size() - 1 - dotPos > nPrecision)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CapitalRiskMgtWnd::GetUserInfoFormAPI()
{
	for (int i = 0; i < m_nUserCfgCnt; ++i)
	{
		if (strcmp(m_UserPluginData_API[i].PluginNo, "CapitalRiskMgt") == 0)
		{
			m_UserNoAndUserInfoMap[m_UserPluginData_API[i].UserNo] = m_UserPluginData_API[i];
		}
	}
}

void CapitalRiskMgtWnd::GetGroupConfigFormAPI()
{
	string sUserNo = "";
	ConfigData tCfgData;
	for (int i = 0; i < m_nInitGroupCfgCnt; ++i)
	{
#if 0
		//清空数据，测试用
		ConfigGroupDel tcfgReq;
		strcpy_s(tcfgReq.GroupNo, m_ConfigGroupData_API[i].GroupNo);
		strcpy_s(tcfgReq.UserNo, m_ConfigGroupData_API[i].UserNo);
		strcpy_s(tcfgReq.Key, m_ConfigGroupData_API[i].Key);
		tcfgReq.KeyIndex = m_ConfigGroupData_API[i].KeyIndex;
		g_authent->DelGroupConfigInfo(PLUGIN_NAME, tcfgReq);
		continue;
#endif
		sUserNo = m_ConfigGroupData_API[i].UserNo;
		if (sUserNo.empty() && m_UserNoAndUserInfoMap.find(sUserNo) != m_UserNoAndUserInfoMap.end())
			continue;
		//保存一份服务器上的结构用于操作服务器的对比
		//DealGroupInfoToCompare(m_ConfigGroupData_API[i]);

		auto iter = m_UserNoAndCfgDataMap.find(sUserNo);
		if (iter == m_UserNoAndCfgDataMap.end())
		{
			m_UserNoAndCfgDataMap.emplace(make_pair(sUserNo, tCfgData));
		}

		if (strcmp(m_ConfigGroupData_API[i].Key, CANCEL_STANDARD_TYPE) == 0)
		{
			m_UserNoAndCfgDataMap[sUserNo].m_nCancelStandardType = (CancelStandardEnum)m_ConfigGroupData_API[i].ValueInt;;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, FIXED_MONEY) == 0)
		{
			m_UserNoAndCfgDataMap[sUserNo].m_nFixedMoney= m_ConfigGroupData_API[i].ValueDouble;;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, CANCEL_MONEY_TYPE) == 0)
		{
			m_UserNoAndCfgDataMap[sUserNo].m_nCancelMoneyType = (CancelMoneyTypeEnum)m_ConfigGroupData_API[i].ValueInt;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, PERCENTAGE) == 0)
		{
			m_UserNoAndCfgDataMap[sUserNo].m_nPercentage = m_ConfigGroupData_API[i].ValueDouble;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, FIXED_CANCEL) == 0)
		{
			m_UserNoAndCfgDataMap[sUserNo].m_nFixedCancelNum = m_ConfigGroupData_API[i].ValueDouble;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, ON_WARNING) == 0)
		{
			m_UserNoAndCfgDataMap[sUserNo].m_bIsWarningOn = (bool)m_ConfigGroupData_API[i].ValueInt;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, ON_FORBID) == 0)
		{
			m_UserNoAndCfgDataMap[sUserNo].m_bIsForbidOn = (bool)m_ConfigGroupData_API[i].ValueInt;
		}
		else
		{
			continue;
		}
	}
}

void CapitalRiskMgtWnd::SetMainUserNOToAPI()
{
	//TODO 更新当前的分组的数据
	auto iter = m_UserNoAndCfgDataMap.find(m_MainGroupUserCfg);
	if (iter != m_UserNoAndCfgDataMap.end())
	{
		m_cfgData = iter->second;
	}
}