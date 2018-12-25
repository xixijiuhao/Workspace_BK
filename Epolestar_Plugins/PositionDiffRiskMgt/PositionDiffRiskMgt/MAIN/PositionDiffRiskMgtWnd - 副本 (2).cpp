#include "PreInclude.h"

const int N_Group_Width = 400;
#define CONFIG_FILE					"config\\PositionDiffRiskMgt.pri"

#define SECTION_CFG                 "config"
#define NODE_CONTRACT               "contract"
#define NODE_POSITION_RANGE         "position_range"
#define CANCEL_SELECT_TYPE			"cancel_select_type"
#define NEED_LESS_POSI_RAG			"need_less_range"
#define CANCEL_TYPE_SELECT			"cancel_type_select"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT	g_FontData;
extern IConfigFrame	    *g_conFrame;
extern IStarApi		    *g_StarApi;
extern PositionDiffRiskMgtWnd *g_pPositionDiffRiskMgtWnd;
extern ILanguageApi	    *g_pLanguage;		//ȫ�� ������Դ
extern LanguageID		 g_LanguageID;
extern IAuthent          *g_authent;

void PositionDiffRiskMgtWnd::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	g_conFrame->show(LoadResWchar_t(IDS_PositionDiff_Risk_Management), cmtAdvance);
}

PositionDiffRiskMgtWnd::PositionDiffRiskMgtWnd()
{
	m_nInitGroupCfgCnt = 0;
	m_ConfigGroupData_API = nullptr;
	QryConfigFinish();

	//LoadConfig();
}
PositionDiffRiskMgtWnd::~PositionDiffRiskMgtWnd()
{
	delete m_ConfigGroupData_API;
	m_ConfigGroupData_API = nullptr;
}

void PositionDiffRiskMgtWnd::CreateCfgWnd(const wchar_t * title)
{
	UserAuthentEx stAuthent;
	string curUserNO = "";
	int index = 0;
	memset(&stAuthent, 0, sizeof(UserAuthentEx));
	if (g_authent->GetAuthent(AUTHENT_COMPANY, AUTHENT_KEY, stAuthent))
	{
		curUserNO = stAuthent.auth.user;
		while ((index = curUserNO.find(' ', index)) != string::npos)
		{
			curUserNO.erase(index, 1);
		}
		m_CurUserNO = curUserNO;
	}

	CreateFrm(title, NULL, WS_OVERLAPPED, 0);
}

void PositionDiffRiskMgtWnd::SetContractNo(string sCode)
{
	//SetWindowTextA(m_EditContract.GetEditHwnd(), sCode.c_str());
	m_EditContract.SetWindowText(sCode.c_str());
	ShowWindow(m_EditContract.GetEditHwnd(), SW_SHOW);
}

LRESULT PositionDiffRiskMgtWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		break;
	case WM_PAINT:
		OnPaint();
		break;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		break;
	case WM_DESTROY:
		SetGroupConfigToAPI();
		SaveConfig();
		break;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnButtonClickDown(wParam, lParam);
		break;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		break;
	case WM_MEASUREITEM:
		OnMeasureItem(wParam, lParam);
		break;
	case WM_SETFOCUS:
	{
		HWND hwnd = (HWND)wParam;
		if (hwnd == m_EditGroupNO.GetEditHwnd())
			g_conFrame->set_hint(L"1. �Ϸ�������Ϊ���ֺ���ĸ \n2. �����ַ������Ϊ10���ַ�");
		break;
	}
	case WM_KILLFOCUS:
		g_conFrame->set_hint(L"");
		break;
	case WM_ACTIVATE:
	{
		if (WA_ACTIVE == LOWORD(wParam) &&
			m_EditGroupNO.GetEditHwnd() == (HWND)lParam)
		{
			g_conFrame->set_hint(L"1. �Ϸ�������Ϊ���ֺ���ĸ \n2. �����ַ������Ϊ10���ַ�");
		}
	}

	//case WM_MOUSEMOVE:
	//	OnMouseMove(wParam, lParam);
	//	return PROCESSED;
	//case WM_MOUSELEAVE:
	//	OnMouseLeave();
	//	return PROCESSED;
	//case WM_LBUTTONDOWN:
	//	//OnLButtonDown(wParam, lParam);
	//	break;
	//case WM_LBUTTONUP:
	//	OnDealListItem();
	//	//OnLButtonUp(wParam, lParam);
	//	break;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void PositionDiffRiskMgtWnd::OnButtonClickDown(WPARAM wParam, LPARAM lParam)
{
	UINT count = m_ListGroupInfo.GetSelectItem();
	wchar_t sText[51] = { 0 };
	if (count >= 0)
	{
		ListView_GetItemText(m_ListGroupInfo.GetListHwnd(), count, 0, sText, sizeof(sText) / sizeof(sText[0]));
		m_CurGroupNO = LoadRC::unicode_to_ansi(sText).c_str();
	}
	else if (m_ListGroupInfo.GetItemCount() == 0)
	{
		ListView_GetItemText(m_ListGroupInfo.GetListHwnd(), 0, 0, sText, sizeof(sText) / sizeof(sText[0]));
		m_CurGroupNO = LoadRC::unicode_to_ansi(sText).c_str();
	}
	else
	{
		MessageBox(NULL, L"����ѡ��һ������", LoadResWchar_t(IDS_Tips), MB_OK);
	}

	switch (lParam)
	{
	case ID_Select_Contract_Btn:
		OnBtnSelect();
		break;
	case ID_Add_Btn:
		OnBtnAdd();
		break;
	case ID_Delete_Btn:
		OnBtnDelete();
		break;
	case ID_Add_Group_Btn:
		OnBtnGroupAdd();
		break;
	case ID_Delete_Group_Btn:
		OnBtnGroupDelete();
		break;
	default:
		break;
	}
}

void PositionDiffRiskMgtWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_List_Default_Qty:
		m_listDefaultQty.OnDrawItem(lParam);
		break;
	case ID_List_GroupInfo:
		m_ListGroupInfo.OnDrawItem(lParam);
		OnDealListItem();
		break;
	default:
		break;
	}
}

void PositionDiffRiskMgtWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	switch (wParam)
	{
	case ID_List_Default_Qty:
		m_listDefaultQty.OnMeasureItem(p->itemHeight);
		break;
	case ID_List_GroupInfo:
		m_ListGroupInfo.OnMeasureItem(p->itemHeight);
		break;
	default:
		break;
	}
}

void PositionDiffRiskMgtWnd::OnCreate()
{
	CreateRiskConfigWnd();
	CreateSetGroupInfoWnd();
}

void PositionDiffRiskMgtWnd::CreateRiskConfigWnd()
{
	int nLeftStart = 12;
	int nTop = 10;
	int nLabelWidth = (g_LanguageID == ENU) ? 110 : 60;;
	int nEditWidth = 120;
	int nUniformHeight = 25;

	RECT firstColRect = { nLeftStart, nTop, 0, 0 };
	RECT secondColRect = { nLeftStart + nLabelWidth + 10, nTop, 0, 0 };
	RECT thirdColRect = { nLeftStart + nLabelWidth + nEditWidth + 15, nTop, 0, 0 };

	//��غ�Լ����
	//��Լѡ��
	m_labelContractNo.Create(m_Hwnd, LoadResWchar_t(IDS_Contract));
	m_labelContractNo.SetFont(g_FontData.GetFontNum13());
	m_labelContractNo.MoveWindow(firstColRect.left, firstColRect.top, nLabelWidth, nUniformHeight);
	m_EditContract.Create(m_Hwnd, ES_UPPERCASE, ID_Edit_ContractNo);
	m_EditContract.SetFont(g_FontData.GetFontNum13());
	m_EditContract.MoveWindow(secondColRect.left, secondColRect.top, nEditWidth, nUniformHeight);
	m_SelectContractBtn.Create(m_Hwnd, ID_Select_Contract_Btn);
	m_SelectContractBtn.SetButtonText("...");
	m_SelectContractBtn.MoveWindow(thirdColRect.left, secondColRect.top, 25, nUniformHeight);

	firstColRect.top += 30;
	secondColRect.top += 30;
	thirdColRect.top += 30;

	//�ֲ� 
	m_LablePositionRange.Create(m_Hwnd, LoadResWchar_t(IDS_Position_Range));
	m_LablePositionRange.SetFont(g_FontData.GetFontNum13());
	m_LablePositionRange.MoveWindow(firstColRect.left, firstColRect.top, nLabelWidth, nUniformHeight);
	m_EditPositionRange.Create(m_Hwnd);
	m_EditPositionRange.SetFont(g_FontData.GetFontNum13());
	m_EditPositionRange.MoveWindow(secondColRect.left, secondColRect.top, nEditWidth + 30, nUniformHeight);

	firstColRect.top += 30;
	secondColRect.top += 30;
	thirdColRect.top += 30;

	//����/ɾ��
	m_AddBtn.Create(m_Hwnd, ID_Add_Btn);
	m_AddBtn.SetButtonText(LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_Add)).c_str());
	m_AddBtn.SetFont(g_FontData.GetFontNum13());
	m_AddBtn.MoveWindow(firstColRect.left, firstColRect.top, 100, nUniformHeight);
	m_DeleteBtn.Create(m_Hwnd, ID_Delete_Btn);
	m_DeleteBtn.SetButtonText(LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_Remove)).c_str());
	m_DeleteBtn.SetFont(g_FontData.GetFontNum13());
	m_DeleteBtn.MoveWindow(firstColRect.left + 120, firstColRect.top, 100, nUniformHeight);

	firstColRect.top += 30;
	secondColRect.top += 30;
	thirdColRect.top += 30;

	m_listDefaultQty.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_List_Default_Qty);
	m_listDefaultQty.MoveWindow(firstColRect.left, firstColRect.top, 300, 200);
	m_listDefaultQty.AddColumn(LoadResWchar_t(IDS_Contract), 215);
	m_listDefaultQty.AddColumn(LoadResWchar_t(IDS_Position_Range), 85);
	ShowList();

	firstColRect.top += 210;
	secondColRect.top += 210;
	thirdColRect.top += 210;

	//����ѡ��
	m_labelCancelSel.Create(m_Hwnd, LoadResWchar_t(IDS_Cancel_Order_Option));
	m_labelCancelSel.SetFont(g_FontData.GetFontNum13());
	m_labelCancelSel.MoveWindow(firstColRect.left, firstColRect.top, nLabelWidth + 10, nUniformHeight);
	m_ComboxCancelSel.Create(m_Hwnd);
	m_ComboxCancelSel.SetFont(g_FontData.GetFontNum13());
	m_ComboxCancelSel.AddString(LoadResWchar_t(IDS_NotAllow_Cancel));
	m_ComboxCancelSel.AddString(LoadResWchar_t(IDS_Cancel_Earliest_One_Order));
	m_ComboxCancelSel.SetSelect(m_cfgData.m_CancelSelType);
	m_ComboxCancelSel.SetFont(g_FontData.GetFontNum13());
	m_ComboxCancelSel.MoveWindow(secondColRect.left, secondColRect.top, nEditWidth + 110, nUniformHeight * 3);

	firstColRect.top += 30;
	//����
	m_CheckboxHangupOrd.Create(m_Hwnd, LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_Single_Less_Position_Range)).c_str());
	m_CheckboxHangupOrd.SetFont(g_FontData.GetFontNum13());
	m_CheckboxHangupOrd.SetValue(m_cfgData.m_bNeedLessPosiRag);
	m_CheckboxHangupOrd.SetBkColor(g_ColorRefData.g_ColorWhite);
	m_CheckboxHangupOrd.MoveWindow(firstColRect.left, firstColRect.top, 500, nUniformHeight);

	firstColRect.top += 30;
	//����ѡ��
	m_CheckboxCancelType.Create(m_Hwnd, LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_CancelTypeSelect)).c_str());
	m_CheckboxCancelType.SetFont(g_FontData.GetFontNum13());
	m_CheckboxCancelType.SetValue(m_cfgData.m_bIsCommonModel);
	m_CheckboxCancelType.SetBkColor(g_ColorRefData.g_ColorWhite);
	m_CheckboxCancelType.MoveWindow(firstColRect.left, firstColRect.top, 500, nUniformHeight);

	firstColRect.top += 120;
	//��ʾ
	m_labelCancelSelTips.Create(m_Hwnd, LoadResWchar_t(IDS_Tips));
	m_labelCancelSelTips.MoveWindow(firstColRect.left, firstColRect.top, 800, nUniformHeight);

	for (int i = 0; i < 5; ++i)
	{
		firstColRect.top += 20;
		m_lableTips[i].Create(m_Hwnd, LoadResWchar_t(IDS_Remarks + i));
		m_lableTips[i].MoveWindow(firstColRect.left, firstColRect.top, 1000, nUniformHeight);
		HDC hdc = ::GetWindowDC(m_lableTips[i].GetHwnd());
		HDC hdc2 = CreateCompatibleDC(hdc);
		SetTextColor(hdc2, g_ColorRefData.g_ColorTextRed);
	}
}

void PositionDiffRiskMgtWnd::CreateSetGroupInfoWnd()
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
	secondColRect.top += 25;
	//����� 
	m_LableGroupNO.Create(m_Hwnd, L"���");
	//m_LableGroupNO.SetFont(g_FontData.GetFontNum9());
	m_LableGroupNO.MoveWindow(firstColRect.left, firstColRect.top, nLabelWidth, nUniformHeight);
	
	//������ 
	m_LableGroupName.Create(m_Hwnd, L"����");
	//m_LableGroupName.SetFont(g_FontData.GetFontNum9());
	m_LableGroupName.MoveWindow(secondColRect.left, secondColRect.top, nLabelWidth, nUniformHeight);

	firstColRect.top += 25;
	secondColRect.top += 25;
	m_EditGroupNO.Create(m_Hwnd);
	//m_EditGroupNO.SetFont(g_FontData.GetFontNum9());
	m_EditGroupNO.MoveWindow(firstColRect.left, firstColRect.top, nEditWidth, nUniformHeight);

	m_EditGroupName.Create(m_Hwnd);
	//m_EditGroupName.SetFont(g_FontData.GetFontNum9());
	m_EditGroupName.MoveWindow(secondColRect.left, secondColRect.top, nEditWidth, nUniformHeight);

	firstColRect.top += 25;
	secondColRect.top += 25;
	//������/ɾ��
	m_AddGroupBtn.Create(m_Hwnd, ID_Add_Group_Btn);
	m_AddGroupBtn.SetButtonText(LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_Add)).c_str());
	m_AddGroupBtn.SetFont(g_FontData.GetFontWord12());
	m_AddGroupBtn.MoveWindow(firstColRect.left, firstColRect.top, nEditWidth, nUniformHeight);
	m_DeleteGroupBtn.Create(m_Hwnd, ID_Delete_Group_Btn);
	m_DeleteGroupBtn.SetButtonText(LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_Remove)).c_str());
	m_DeleteGroupBtn.SetFont(g_FontData.GetFontWord12());
	m_DeleteGroupBtn.MoveWindow(secondColRect.left, secondColRect.top, nEditWidth, nUniformHeight);

	firstColRect.top += 25;
	secondColRect.top += 25;
	//List
	m_ListGroupInfo.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_List_GroupInfo);
	m_ListGroupInfo.MoveWindow(firstColRect.left, firstColRect.top, 170, 200);
	m_ListGroupInfo.AddColumn(L"���", 60);
	m_ListGroupInfo.AddColumn(L"����", 110);
	ShowGroupList();

	HINSTANCE hInstance = GetModuleHandle(NULL);
	m_hGroupbox = CreateWindowEx(0, L"button", L"������Ϣ����", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		nLeftStart - 15, nTop, 200, 320, m_Hwnd, (HMENU)ID_Groupbox, hInstance, NULL);
	SendMessage(m_hGroupbox, WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
}

void PositionDiffRiskMgtWnd::ShowList()
{
	m_listDefaultQty.DeleteAllItems();
	int iRow = 0;
	int iCol = 0;
	for (auto iter = m_cfgData.m_ContractPosiRangeMap.begin(); iter != m_cfgData.m_ContractPosiRangeMap.end(); iter++)
	{
		iCol = 0;
		m_listDefaultQty.AddItem(iter->first.c_str(), iRow, iCol++);
		m_listDefaultQty.AddItem(iter->second, iRow, iCol++);
		iRow++;
	}
}

void PositionDiffRiskMgtWnd::ShowGroupList()
{
	m_ListGroupInfo.DeleteAllItems();
	int iRow = 0;
	int iCol = 0;
	for (auto iter = m_GroupNoAndNameMap.begin(); iter != m_GroupNoAndNameMap.end(); iter++)
	{
		iCol = 0;
		m_ListGroupInfo.AddItem(iter->first.c_str(), iRow, iCol++);
		m_ListGroupInfo.AddItem(iter->second.c_str(), iRow, iCol++);
		iRow++;
	}
}


void PositionDiffRiskMgtWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	if ((int)wParam == ID_List_GroupInfo)
	{
		OnDealListItem();
	}
}

void PositionDiffRiskMgtWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;

	UINT count = m_ListGroupInfo.GetSelectItem();
	if (count >= 0)
	{
		wchar_t sText[51] = { 0 };
		ListView_GetItemText(m_ListGroupInfo.GetListHwnd(), count, 0, sText, sizeof(sText) / sizeof(sText[0]));
		m_CurGroupNO = LoadRC::unicode_to_ansi(sText).c_str();
	}
	else
	{
		MessageBox(NULL, L"����ѡ��һ������", LoadResWchar_t(IDS_Tips), MB_OK);
	}

	if (m_EditGroupNO.GetEditHwnd() == (HWND)lParam)
	{
		g_conFrame->set_hint(L"1. �Ϸ�������Ϊ���ֺ���ĸ \n2. �����ַ������Ϊ10���ַ�");
	}


	if (hwnd == m_ComboxCancelSel.GetHwnd())
	{
		m_GroupNoAndCfgDataMap[m_CurGroupNO].m_CancelSelType = (CancelSelectType)m_ComboxCancelSel.GetSelect();
	}
	else if (hwnd == m_CheckboxHangupOrd.GetHwnd())
	{
		m_GroupNoAndCfgDataMap[m_CurGroupNO].m_bNeedLessPosiRag = (int)m_CheckboxHangupOrd.GetValue();
	}
	else if (hwnd == m_CheckboxCancelType.GetHwnd())
	{
		m_GroupNoAndCfgDataMap[m_CurGroupNO].m_bIsCommonModel = (int)m_CheckboxCancelType.GetValue();
	}
}

void PositionDiffRiskMgtWnd::OnPaint()
{
	HDC			hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(m_Hwnd, &ps);
	RECT item;
	GetClientRect(m_Hwnd, &item);
	FillRect(hdc, &item, g_ColorRefData.GetHbrWhite());
	EndPaint(m_Hwnd, &ps);
}

void PositionDiffRiskMgtWnd::OnBtnSelect()
{
	RECT rectvalid;
	GetWindowRect(m_SelectContractBtn.GetHwnd(), &rectvalid);
	if (!IsWindow(m_popWinCode.GetHwnd()))
	{
		m_popWinCode.CreatePopWin(this, TEXT("m_pPopWinCode"), m_Hwnd, WS_CLIPCHILDREN, 0, rectvalid.left - 64 + 3, rectvalid.bottom);
		m_popWinCode.UpdateData();
		::ShowWindow(m_popWinCode.GetHwnd(), SW_SHOWNORMAL);
	}
	else
	{
		m_popWinCode.UpdateData();
		SetWindowPos(m_popWinCode.GetHwnd(), NULL, rectvalid.left - 64 + 3, rectvalid.bottom, 0, 0, SWP_NOSIZE);
		::ShowWindow(m_popWinCode.GetHwnd(), SW_SHOWNORMAL);
	}
}

void PositionDiffRiskMgtWnd::OnBtnAdd()
{
	//У��Ϸ���
	char sText[51] = { 0 };
	GetWindowTextA(m_EditContract.GetEditHwnd(), sText, 51);
	string sCode = sText;
	CConvert::TrimString(sCode);
	//�ж��Ƿ��ǺϷ��Ľ��׺�Լ
	TContractKey tempContract;
	memset(&tempContract, 0, sizeof(tempContract));

	if (!CConvert::TContractNoToTContractKey(sCode.c_str(), tempContract))
	{
		wchar_t msg[MAX_PATH];
		swprintf_s(msg, LoadResWchar_t(IDS_Invalid_Contract), CConvert::UTF8ToUnicode(sCode).c_str());
		MessageBox(m_Hwnd, msg, LoadResWchar_t(IDS_Tips), NULL);
		return ;
	}

	GetWindowTextA(m_EditPositionRange.GetEditHwnd(), sText, 51);
	int nRange = atoi(sText);
	if (nRange <= 0)
	{
		MessageBox(m_Hwnd, LoadResWchar_t(IDS_Invalid_Position), LoadResWchar_t(IDS_Tips), NULL);
		return;
	}

	if (g_StarApi)
	{
		SContractNoType typeId;
		typeId[0] = '\0';
		CConvert::TradeContractToSContractNoType(tempContract, typeId);
		//��Լ��Ʒ��
		if (tempContract.CommodityType == ctGoods)
		{
			if (!CConvert::IsValidQuoteCommodity(tempContract.ExchangeNo, tempContract.CommodityNo))
			{
				wchar_t msg[MAX_PATH];
				swprintf_s(msg, LoadResWchar_t(IDS_Invalid_Contract), CConvert::UTF8ToUnicode(sCode).c_str());
				MessageBox(m_Hwnd, msg, LoadResWchar_t(IDS_Tips), NULL);
				return;
			}
		}
		//����Լ
		else
		{
			if (!CConvert::IsValidQuotContract(typeId))
			{
				wchar_t msg[MAX_PATH];
				swprintf_s(msg, LoadResWchar_t(IDS_Invalid_Contract), CConvert::UTF8ToUnicode(sCode).c_str());
				MessageBox(m_Hwnd, msg, LoadResWchar_t(IDS_Tips), NULL);
				return;
			}
		}
		m_GroupNoAndCfgDataMap[m_CurGroupNO].m_ContractPosiRangeMap[sCode] = nRange;
		m_cfgData = m_GroupNoAndCfgDataMap[m_CurGroupNO];
		ShowList();

		//���ӵ�������
		ConfigGroupAdd cnfGroupAdd;
		
		strcpy_s(cnfGroupAdd.UserNo, m_CurUserNO.c_str());
		strcpy_s(cnfGroupAdd.GroupNo, m_CurGroupNO.c_str());
		//ADD ��Լ��
		m_GroupNoAndIndexCnt[m_CurGroupNO]++;
		strcpy_s(cnfGroupAdd.Key, NODE_CONTRACT);
		cnfGroupAdd.KeyIndex = m_GroupNoAndIndexCnt[m_CurGroupNO];
		strcpy_s(cnfGroupAdd.ValueStr, sCode.c_str());
		g_authent->AddGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, cnfGroupAdd);
		m_GroupInfoCompareMap[m_CurGroupNO].push_back(cnfGroupAdd);
		
		//ADD �ֲ�
		strcpy_s(cnfGroupAdd.ValueStr, "");
		strcpy_s(cnfGroupAdd.Key, NODE_POSITION_RANGE);
		cnfGroupAdd.KeyIndex = m_GroupNoAndIndexCnt[m_CurGroupNO];
		cnfGroupAdd.ValueInt = nRange;
		g_authent->AddGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, cnfGroupAdd);
		m_GroupInfoCompareMap[m_CurGroupNO].push_back(cnfGroupAdd);
	}	
}

void PositionDiffRiskMgtWnd::OnBtnDelete()
{
	UINT count = m_listDefaultQty.GetSelectItem();
	//��������޸ĳ�ɾ�����һ��
	if (count < 0)
	{
		count = m_listDefaultQty.GetItemCount() - 1;
	}
			
	wchar_t cc[101] = { 0 };
	ListView_GetItemText(m_listDefaultQty.GetListHwnd(), count, 0, cc, sizeof(cc) / sizeof(cc[0]));
	//m_cfgData.m_ContractPosiRangeMap.erase(LoadRC::unicode_to_ansi(cc).c_str());
	string strContractNO = LoadRC::unicode_to_ansi(cc);
	m_GroupNoAndCfgDataMap[m_CurGroupNO].m_ContractPosiRangeMap.erase(strContractNO);
	
	m_cfgData = m_GroupNoAndCfgDataMap[m_CurGroupNO];
	ShowList();

	//ɾ���������ϵ�����
	auto iterCompare = m_GroupInfoCompareMap.find(m_CurGroupNO);
	//�������������������飬��Ҫȫ��ɾ���÷��������
	if (iterCompare != m_GroupInfoCompareMap.end())
	{
		int curKeyIndex = -1;
		vector<ConfigGroupRsp> &vecIndexGroupInfo = iterCompare->second;
		for (auto iterGroupInfo = vecIndexGroupInfo.begin(); iterGroupInfo != vecIndexGroupInfo.end(); ++iterGroupInfo)
		{
			ConfigGroupRsp &cfgGroupRsp = *iterGroupInfo;
			if (strcmp(cfgGroupRsp.Key, NODE_CONTRACT) == 0 &&
				strcmp(cfgGroupRsp.ValueStr, strContractNO.c_str()))
			{
				ConfigGroupDel tcfgReq;
				//ɾ���÷������һ m_CurGroupNO ��Լ
				strcpy_s(tcfgReq.GroupNo, cfgGroupRsp.GroupNo);
				strcpy_s(tcfgReq.UserNo, cfgGroupRsp.UserNo);
				strcpy_s(tcfgReq.Key, cfgGroupRsp.Key);
				tcfgReq.KeyIndex = cfgGroupRsp.KeyIndex;
				g_authent->DelGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, tcfgReq);

				//ɾ���÷�����һ m_CurGroupNO ��Լ��Ӧ�Ĳֲ�
				strcpy_s(tcfgReq.GroupNo, cfgGroupRsp.GroupNo);
				strcpy_s(tcfgReq.UserNo, cfgGroupRsp.UserNo);
				strcpy_s(tcfgReq.Key, NODE_POSITION_RANGE);
				tcfgReq.KeyIndex = cfgGroupRsp.KeyIndex;
				g_authent->DelGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, tcfgReq);

				curKeyIndex = cfgGroupRsp.KeyIndex;
				vecIndexGroupInfo.erase(iterGroupInfo);
				break;
			}
		}

		//��map��ɾ����Ӧ�Ĳֲʼ�պͷ���������һ��
		for (auto iterGroupInfo2 = vecIndexGroupInfo.begin(); iterGroupInfo2 != vecIndexGroupInfo.end(); ++iterGroupInfo2)
		{
			ConfigGroupRsp &cfgGroupRsp = *iterGroupInfo2;
			if (strcmp(cfgGroupRsp.Key, NODE_POSITION_RANGE) == 0 &&
				cfgGroupRsp.KeyIndex == curKeyIndex)
			{
				vecIndexGroupInfo.erase(iterGroupInfo2);
				break;
			}
		}
	}
}

void PositionDiffRiskMgtWnd::OnBtnGroupAdd()
{
	char sText[51] = { 0 };
	GetWindowTextA(m_EditGroupNO.GetEditHwnd(), sText, 51);
	string nGroupNO = sText;
	if (nGroupNO.empty())
	{
		MessageBox(m_Hwnd, L"����Ų���Ϊ��", LoadResWchar_t(IDS_Tips), MB_OK);
		return;
	}
	if (m_GroupNoAndNameMap.find(nGroupNO) != m_GroupNoAndNameMap.end())
	{
		MessageBox(m_Hwnd, L"������ظ������������", LoadResWchar_t(IDS_Tips), MB_OK);
		return;
	}
	m_CurGroupNO = nGroupNO;

	GetWindowTextA(m_EditGroupName.GetEditHwnd(), sText, 51);
	string nGroupName =sText;
	nGroupName += "Group";
	
	m_GroupNoAndNameMap.emplace(make_pair(nGroupNO, nGroupName));
	ShowGroupList();

	ConfigData cfgData;
	m_GroupNoAndCfgDataMap.emplace(make_pair(m_CurGroupNO, cfgData));
	ChangeFrmWithGroup();
}

void PositionDiffRiskMgtWnd::OnBtnGroupDelete()
{
	auto Iter = m_GroupNoAndCfgDataMap.find(m_CurGroupNO);
	if (Iter != m_GroupNoAndCfgDataMap.end())
	{
		m_GroupNoAndCfgDataMap.erase(m_CurGroupNO);
	}

	auto iterCompare = m_GroupInfoCompareMap.find(m_CurGroupNO);
	//�������������������飬��Ҫȫ��ɾ���÷��������
	if (iterCompare != m_GroupInfoCompareMap.end())
	{
		vector<ConfigGroupRsp> vecIndexGroupInfo = iterCompare->second;
		for (auto iterGroupInfo = vecIndexGroupInfo.begin(); iterGroupInfo != vecIndexGroupInfo.end(); ++iterGroupInfo)
		{
			ConfigGroupRsp &cfgGroupRsp = *iterGroupInfo;
			ConfigGroupDel tcfgReq;
			strcpy_s(tcfgReq.GroupNo, cfgGroupRsp.GroupNo);
			strcpy_s(tcfgReq.UserNo, cfgGroupRsp.UserNo);
			strcpy_s(tcfgReq.Key, cfgGroupRsp.Key);
			tcfgReq.KeyIndex = cfgGroupRsp.KeyIndex;
			g_authent->DelGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, tcfgReq);
		}
		m_GroupInfoCompareMap.erase(iterCompare);
	}

	if (m_GroupNoAndNameMap.find(m_CurGroupNO) != m_GroupNoAndNameMap.end())
	{
		m_GroupNoAndNameMap.erase(m_CurGroupNO);
	}
	ShowGroupList();

	m_cfgData.Clear();
	ChangeFrmWithGroup();
}

void PositionDiffRiskMgtWnd::OnDealListItem()
{
	int count = m_ListGroupInfo.GetSelectItem();
	if (count >= 0)
	{
		wchar_t sText[51] = { 0 };
		ListView_GetItemText(m_ListGroupInfo.GetListHwnd(), count, 0, sText, sizeof(sText) / sizeof(sText[0]));
		if (sText == L"")
			return;
		m_CurGroupNO = LoadRC::unicode_to_ansi(sText).c_str();
	}
	//TODO ����л�����Ľ������
	m_cfgData = m_GroupNoAndCfgDataMap[m_CurGroupNO];
	ChangeFrmWithGroup();
}

void PositionDiffRiskMgtWnd::ChangeFrmWithGroup()
{
	m_cfgData = m_GroupNoAndCfgDataMap[m_CurGroupNO];
	ShowList();
	m_ComboxCancelSel.SetSelect(m_cfgData.m_CancelSelType);
	m_CheckboxHangupOrd.SetValue(m_cfgData.m_bNeedLessPosiRag);
	m_CheckboxCancelType.SetValue(m_cfgData.m_bIsCommonModel);
}

bool PositionDiffRiskMgtWnd::CheckInputValueInvaild()
{
	char sText[51] = { 0 };
	GetWindowTextA(m_EditContract.GetEditHwnd(), sText, 51);
	string sCode = sText;
	CConvert::TrimString(sCode);
	//�ж��Ƿ��ǺϷ��Ľ��׺�Լ
	TContractKey tempContract;
	memset(&tempContract, 0, sizeof(tempContract));

	if (!CConvert::TContractNoToTContractKey(sCode.c_str(), tempContract))
	{
		wchar_t msg[MAX_PATH];
		swprintf_s(msg, LoadResWchar_t(IDS_Invalid_Contract), CConvert::UTF8ToUnicode(sCode).c_str());
		MessageBox(m_Hwnd, msg, LoadResWchar_t(IDS_Tips), NULL);
		return false;
	}

	GetWindowTextA(m_EditPositionRange.GetEditHwnd(), sText, 51);
	int nRange = atoi(sText);
	if (nRange <= 0)
	{
		MessageBox(m_Hwnd, LoadResWchar_t(IDS_Invalid_Position), LoadResWchar_t(IDS_Tips), NULL);
		return false;
	}
	return true;
}

void PositionDiffRiskMgtWnd::SaveConfig()
{
	std::string cfg_file = FileFun::GetAbsolutePathForExe(CONFIG_FILE);
	//�������
	std::ofstream ofs;
	ofs.open(cfg_file, std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	int i = 0;
	std::string strKey = "";
	for (auto it : m_cfgData.m_ContractPosiRangeMap)
	{
		strKey = NODE_CONTRACT + to_string(i);
		WritePrivateProfileStringA(SECTION_CFG, strKey.c_str(), it.first.c_str(), cfg_file.c_str());

		strKey = NODE_POSITION_RANGE + to_string(i);
		WritePrivateProfileStringA(SECTION_CFG, strKey.c_str(), to_string(it.second).c_str(), cfg_file.c_str());
		i++;
	}
	CancelSelectType tmpType = (CancelSelectType)m_ComboxCancelSel.GetSelect();
	m_cfgData.m_CancelSelType = tmpType;
	strKey = CANCEL_SELECT_TYPE;
	WritePrivateProfileStringA(SECTION_CFG, strKey.c_str(), to_string(tmpType).c_str(), cfg_file.c_str());

	bool bNeedLessRage = (bool)m_CheckboxHangupOrd.GetValue();
	m_cfgData.m_bNeedLessPosiRag = bNeedLessRage;
	strKey = NEED_LESS_POSI_RAG;
	WritePrivateProfileStringA(SECTION_CFG, strKey.c_str(), to_string(bNeedLessRage).c_str(), cfg_file.c_str());

	bool bIsCommonModel = (bool)m_CheckboxCancelType.GetValue();
	m_cfgData.m_bIsCommonModel = bIsCommonModel;
	strKey = CANCEL_TYPE_SELECT;
	WritePrivateProfileStringA(SECTION_CFG, strKey.c_str(), to_string(bIsCommonModel).c_str(), cfg_file.c_str());
}

void PositionDiffRiskMgtWnd::LoadConfig()
{
	std::string cfg_file = FileFun::GetAbsolutePathForExe(CONFIG_FILE);
	std::string strKey = "";
	char contract[101];
	char value[20];
	for (int i = 0; ; i++)
	{
		strKey = NODE_CONTRACT + to_string(i);
		GetPrivateProfileStringA(SECTION_CFG, strKey.c_str(), "", contract, 101, cfg_file.c_str());
		if (strcmp(contract, "") == 0)
		{
			break;
		}
		strKey = NODE_POSITION_RANGE + to_string(i);
		GetPrivateProfileStringA(SECTION_CFG, strKey.c_str(), "", value, 20, cfg_file.c_str());
		if (strcmp(value, "") == 0)
		{
			break;
		}
		m_cfgData.m_ContractPosiRangeMap[std::string(contract)] = atoi(value);
	}

	strKey = CANCEL_SELECT_TYPE;
	GetPrivateProfileStringA(SECTION_CFG, strKey.c_str(), "", value, 20, cfg_file.c_str());
	m_cfgData.m_CancelSelType = (CancelSelectType)atoi(value);

	strKey = NEED_LESS_POSI_RAG;
	GetPrivateProfileStringA(SECTION_CFG, strKey.c_str(), "", value, 20, cfg_file.c_str());
	m_cfgData.m_bNeedLessPosiRag = (bool)atoi(value);

	strKey = CANCEL_TYPE_SELECT;
	GetPrivateProfileStringA(SECTION_CFG, strKey.c_str(), "", value, 20, cfg_file.c_str());
	m_cfgData.m_bIsCommonModel = (bool)atoi(value);
}

void PositionDiffRiskMgtWnd::SaveConfigWithGroup()
{
	
}

void PositionDiffRiskMgtWnd::LoadConfigWithGroup()
{
	
}

void PositionDiffRiskMgtWnd::ApiStructToUserStruct(ConfigGroupRsp &apiStruct, ConfigData &userStruct)
{

}

void PositionDiffRiskMgtWnd::UserStructToApiStruct(ConfigData &userStruct, ConfigGroupRsp &apiStruct)
{
	
}

void PositionDiffRiskMgtWnd::QryConfigFinish()
{
	m_nInitGroupCfgCnt = 0;
	g_authent->GetGroupConfigInfo(m_nInitGroupCfgCnt);
	m_ConfigGroupData_API = new ConfigGroupRsp[m_nInitGroupCfgCnt];
	memset(m_ConfigGroupData_API, 0, m_nInitGroupCfgCnt * sizeof(ConfigGroupRsp));
	g_authent->GetGroupConfigInfo(m_nInitGroupCfgCnt, m_ConfigGroupData_API);

	//����Ԥ����
	if (m_nInitGroupCfgCnt > 0)
	{
		GetGroupConfigFormAPI();
		auto IterBegin = m_GroupNoAndCfgDataMap.begin();
		//TODO �洢��ǰ�û��ķ������
		if (IterBegin != m_GroupNoAndCfgDataMap.end())
			m_cfgData = IterBegin->second;
	}
}

void PositionDiffRiskMgtWnd::GetGroupConfigFormAPI()
{
	string nGroupNo = "";
	string sKeyIndexOnly = "";
	ConfigData tCfgData;
	map<string, string> mapIndexContract;
	map<string, int> mapIndexPosiRange;
	for (int i = 0; i < m_nInitGroupCfgCnt; ++i)
	{
		//����һ�ݷ������ϵĽṹ���ڲ����������ĶԱ�
		DealGroupInfoToCompare(m_ConfigGroupData_API[i]);

		nGroupNo = m_ConfigGroupData_API[i].GroupNo;
		string sGroupName = "";
		int cpos = nGroupNo.find('|');
		if (cpos != std::string::npos)
		{
			sGroupName = nGroupNo.substr(0, cpos);
		}

		m_GroupNoAndNameMap.emplace(make_pair(nGroupNo, sGroupName));

		auto iter = m_GroupNoAndCfgDataMap.find(nGroupNo);
		if (iter == m_GroupNoAndCfgDataMap.end())
		{
			m_GroupNoAndCfgDataMap.emplace(make_pair(nGroupNo, tCfgData));
		}

		if (strcmp(m_ConfigGroupData_API[i].Key, NODE_CONTRACT) == 0)
		{
			int nRange = 0;
			string sContractNo = m_ConfigGroupData_API[i].ValueStr;
			sKeyIndexOnly = nGroupNo + to_string(m_ConfigGroupData_API[i].KeyIndex);
			mapIndexContract[sKeyIndexOnly] = sContractNo;
			auto iter = mapIndexPosiRange.find(sKeyIndexOnly);
			if (iter != mapIndexPosiRange.end())
			{
				nRange = iter->second;
			}
			m_GroupNoAndCfgDataMap[nGroupNo].m_ContractPosiRangeMap[sContractNo] = nRange;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, NODE_POSITION_RANGE) == 0)
		{
			int nRange = m_ConfigGroupData_API[i].ValueInt;
			string sContractNo = "";
			sKeyIndexOnly = nGroupNo + to_string(m_ConfigGroupData_API[i].KeyIndex);
			mapIndexPosiRange[sKeyIndexOnly] = nRange;
			auto iter = mapIndexContract.find(sKeyIndexOnly);
			if (iter != mapIndexContract.end())
			{
				sContractNo = iter->second;
				m_GroupNoAndCfgDataMap[nGroupNo].m_ContractPosiRangeMap[sContractNo] = nRange;
			}
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, CANCEL_SELECT_TYPE) == 0)
		{
			m_GroupNoAndCfgDataMap[nGroupNo].m_CancelSelType = (CancelSelectType)m_ConfigGroupData_API[i].ValueInt;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, NEED_LESS_POSI_RAG) == 0)
		{
			m_GroupNoAndCfgDataMap[nGroupNo].m_bNeedLessPosiRag = (bool)m_ConfigGroupData_API[i].ValueInt;
		}
		else if (strcmp(m_ConfigGroupData_API[i].Key, CANCEL_TYPE_SELECT) == 0)
		{
			m_GroupNoAndCfgDataMap[nGroupNo].m_bIsCommonModel = (bool)m_ConfigGroupData_API[i].ValueInt;
		}
		else
		{
			continue;
		}
	}
}

void PositionDiffRiskMgtWnd::SetGroupConfigToAPI()
{
	ConfigGroupAdd cnfGroupAdd;
	bool isSuccess = false;
	string curGroupNO = "";

	for (auto iter = m_GroupNoAndCfgDataMap.begin(); iter != m_GroupNoAndCfgDataMap.end(); ++iter)
	{
		curGroupNO = iter->first;
		if (curGroupNO.empty())
			continue;

		auto iterCompare = m_GroupInfoCompareMap.find(curGroupNO);
		if (iterCompare != m_GroupInfoCompareMap.end())
		{

			for (auto iterGroupAdd = iterCompare->second.begin(); iterGroupAdd != iterCompare->second.end(); )
			{
				//ɾ�����еķǺ�Լ�ͷǲֲ����������
				if (strcmp(cnfGroupAdd.Key, CANCEL_SELECT_TYPE) == 0 ||
					strcmp(cnfGroupAdd.Key, NEED_LESS_POSI_RAG) == 0 ||
					strcmp(cnfGroupAdd.Key, CANCEL_TYPE_SELECT) == 0)
				{
					ConfigGroupRsp &cfgGroupRsp = *iterGroupAdd;
					ConfigGroupDel tcfgReq;
					strcpy_s(tcfgReq.GroupNo, cfgGroupRsp.GroupNo);
					strcpy_s(tcfgReq.UserNo, cfgGroupRsp.UserNo);
					strcpy_s(tcfgReq.Key, cfgGroupRsp.Key);
					tcfgReq.KeyIndex = cfgGroupRsp.KeyIndex;
					g_authent->DelGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, tcfgReq);
					iterGroupAdd = iterCompare->second.erase(iterGroupAdd);
				}
				else
				{
					++iterGroupAdd;
				}

			}
			//���²������зǺ�Լ�ͷǲֲ����������
			ConfigData &cfgData = iter->second;

			strcpy_s(cnfGroupAdd.UserNo, m_CurGroupNO.c_str());
			strcpy_s(cnfGroupAdd.GroupNo, iter->first.c_str());

			++m_GroupNoAndIndexCnt[m_CurGroupNO];
			strcpy_s(cnfGroupAdd.Key, CANCEL_SELECT_TYPE);
			cnfGroupAdd.KeyIndex = m_GroupNoAndIndexCnt[m_CurGroupNO];
			cnfGroupAdd.ValueInt = iter->second.m_CancelSelType;
			isSuccess = g_authent->AddGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, cnfGroupAdd);

			++m_GroupNoAndIndexCnt[m_CurGroupNO];;
			strcpy_s(cnfGroupAdd.Key, NEED_LESS_POSI_RAG);
			cnfGroupAdd.ValueInt = iter->second.m_bNeedLessPosiRag;
			cnfGroupAdd.KeyIndex = m_GroupNoAndIndexCnt[m_CurGroupNO];;
			isSuccess = g_authent->AddGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, cnfGroupAdd);

			++m_GroupNoAndIndexCnt[m_CurGroupNO];;
			strcpy_s(cnfGroupAdd.Key, CANCEL_TYPE_SELECT);
			cnfGroupAdd.KeyIndex = m_GroupNoAndIndexCnt[m_CurGroupNO];;
			cnfGroupAdd.ValueInt = iter->second.m_bIsCommonModel;
			isSuccess = g_authent->AddGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, cnfGroupAdd);
		}
	}
		//else
		//{
		//	ConfigData &cfgData = iter->second;
		//	vector<ConfigGroupRsp> mapIndexGroupInfo = iterCompare->second;
		//	//�����ϵķ���ṹ
		//	for (auto iterGroupInfo = mapIndexGroupInfo.begin(); iterGroupInfo != mapIndexGroupInfo.end(); ++iterGroupInfo)
		//	{
		//		ConfigGroupRsp &cfgGroupRsp = *iterGroupInfo;
		//		
		//		//��Լ�Ĵ���
		//		if (strcmp(cfgGroupRsp.Key, NODE_CONTRACT) == 0)
		//		{
		//			if (cfgData.m_ContractPosiRangeMap.find(cfgGroupRsp.ValueStr) == cfgData.m_ContractPosiRangeMap.end())
		//			{
		//				//����ϵĺ�Լ���µ�Map���Ҳ�����˵���Ѿ����û�ɾ������Ӧ��������ҲҪɾ��
		//				ConfigGroupDel tcfgReq;
		//				strcpy_s(tcfgReq.GroupNo, cfgGroupRsp.GroupNo);
		//				strcpy_s(tcfgReq.UserNo, cfgGroupRsp.UserNo);
		//				strcpy_s(tcfgReq.Key, cfgGroupRsp.Key);
		//				tcfgReq.KeyIndex = cfgGroupRsp.KeyIndex;
		//				g_authent->DelGroupConfigInfo(AUTHENT_COMPANY, AUTHENT_KEY, tcfgReq);
		//			}
		//			else
		//			{
		//				//����ҵ��������

		//			}
		//		}
		//	}
		//}
}


void PositionDiffRiskMgtWnd::DealGroupInfoToCompare(ConfigGroupRsp &cfgGroupData)
{
	int nKeyIndex = cfgGroupData.KeyIndex;
	string nGroupNo = cfgGroupData.GroupNo;
	auto iterFind = m_GroupInfoCompareMap.find(nGroupNo);
	if (iterFind == m_GroupInfoCompareMap.end())
	{
		vector<ConfigGroupRsp> vecIndexGroupInfo;
		vecIndexGroupInfo.push_back( cfgGroupData);
		m_GroupInfoCompareMap.insert(make_pair(nGroupNo, vecIndexGroupInfo));
	}
	else
	{
		m_GroupInfoCompareMap[nGroupNo].push_back(cfgGroupData);
	}

	//�洢���������е�KeyIndex�����ֵ
	if (m_GroupNoAndIndexCnt.find(nGroupNo) != m_GroupNoAndIndexCnt.end())
	{
		if (nKeyIndex > m_GroupNoAndIndexCnt[nGroupNo])
		{
			m_GroupNoAndIndexCnt[nGroupNo] = nKeyIndex;
		}
	}
	else
	{
		m_GroupNoAndIndexCnt[nGroupNo] = nKeyIndex;
	}
}