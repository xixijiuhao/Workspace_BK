#include "PreInclude.h"

const int N_Group_Width = 500;
#define CONFIG_FILE					"config\\SWRiskMonitor.pri"

#define SECTION_CFG                 "config"
#define NODE_MAX_OPEN_QTY           "max_open_qty"
#define NODE_CANCEL_THRESHOLD       "cancel_threshold"
#define NODE_CONTRACT               "contract"
#define NODE_POSITION_RANGE         "position_range"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT	g_FontData;
extern IConfigFrame	    *g_conFrame;
extern IStarApi		    *g_StarApi;
extern CapitalRiskMgtWnd *g_pCapitalRiskMgtWnd;

LRESULT CALLBACK ContractListViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	CapitalRiskMgtWnd *pWnd = (CapitalRiskMgtWnd*)GetWindowLong(hwnd, GWL_USERDATA);
	if (!pWnd)
		return false;
	switch (message)
	{
	case WM_DRAWITEM:
	{
		DRAWITEMSTRUCT *pDraw = (DRAWITEMSTRUCT*)(lParam);
		std::wstring wstr = TEXT("");
		if (id == ID_ListView_Contract)
		{
			switch (pDraw->itemID)
			{
			case 0:
				wstr = LoadResString(IDS_Contract);
				break;
			case 1:
				wstr = LoadResString(IDS_Position_Range);
				break;
			}
		}

		RECT rect = pDraw->rcItem;
		InflateRect(&rect, 1, 1);
		FillRect(pDraw->hDC, &rect, g_ColorRefData.GetHbrRGB150());
		FrameRect(pDraw->hDC, &rect, g_ColorRefData.GetHbrBackground());
		SetTextColor(pDraw->hDC, g_ColorRefData.GetColorWhite());
		SetBkMode(pDraw->hDC, TRANSPARENT);
		DrawText(pDraw->hDC, wstr.c_str(), wstr.size(), &pDraw->rcItem, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		return true;
	}
	break;
	case WM_NOTIFY:
	{
		NMHDR *pNMHDR = (NMHDR *)lParam;
		LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		switch (pNMListView->hdr.code)
		{
		case HDN_BEGINTRACKW:
		case HDN_BEGINTRACKA:
			return true;
		case HDN_DIVIDERDBLCLICK:
			return true;
		}
	}
	break;
	}
	if (id == ID_ListView_Contract)
		return CallWindowProc(pWnd->m_procListview, hwnd, message, wParam, lParam);
	else
		return false;
}


void CapitalRiskMgtWnd::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	//g_conFrame->reg_config_dlg(LoadResString(IDS_SW_Monitor).c_str(), (dlg_create_func)GetCfgWnd, cmtAdvance, 15, "esunny.epolestar.configframe");

	g_conFrame->show(L"资金风控", cmtAdvance);
}


CapitalRiskMgtWnd::CapitalRiskMgtWnd()
	: m_bCheckedPwd(false)
{
	LoadConfig();

}

void CapitalRiskMgtWnd::CreateCfgWnd(const wchar_t * title)
{
	CreateFrm(title, NULL, WS_OVERLAPPED, 0);
}

void CapitalRiskMgtWnd::SetContractNo(string sCode)
{
	//SetWindowTextA(m_hControl[ID_Edit_Contract], sCode.c_str());
	SetWindowTextA(m_EditContract.GetEditHwnd(),sCode.c_str());
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
		SaveConfig();
		break;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void CapitalRiskMgtWnd::OnCreate()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	m_hStaticBKBrush = g_ColorRefData.GetHbrWhite();//背景色

	int nLeftStart = 12;
	int nStaticWidth = 80;
	int nEditWidth = 100;
	int nEditHeight = 22;
	int nButtonHeight = 22;
	int nButtonWidth = 50;
	int nTop = 10;
	int nInterval = 8;

	nLeftStart = 10;
	nTop = 3;
	RECT rect = { 50,50,50,50 };
	m_LableBackStandard.Create(m_Hwnd, LoadResString(IDS_Cancel_Standard).c_str(),ID_Lable_Back_Standard);
	m_LableBackStandard.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);


	//m_ComboxBackStandard.Create(m_Hwnd);
	//m_ComboxBackStandard.AddString("昨权益");
	//m_ComboxBackStandard.AddString("固定资金");
	//m_LableBackStandard.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_EditBackStandardLocal.Create(m_Hwnd);
	//m_LableBackStandard.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_LableBackMoney.Create(m_Hwnd, L"回撤资金",ID_Lable_Back_Money);
	//m_LableBackMoney.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_ComboxBackMoney.Create(m_Hwnd);
	//m_ComboxBackMoney.AddString("百分比");
	//m_ComboxBackMoney.AddString("固定回撤");
	//m_ComboxBackMoney.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_EditBackPrecent.Create(m_Hwnd);
	//m_EditBackPrecent.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_CheckboxWarning.Create(m_Hwnd, "到达回撤后发出警报");
	//m_CheckboxWarning.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_CheckboxForbid.Create(m_Hwnd, "到达回撤后禁止开仓");
	//m_CheckboxForbid.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);



	////风控合约设置
	//nLeftStart = 32;
	//nTop = 140 + 45;

	//m_EditContract.Create(m_Hwnd);
	//m_EditContract.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_SelectContractBtn.Create(m_Hwnd, ID_Select_Contract_Btn);
	//m_SelectContractBtn.SetButtonText("...");
	//m_EditContract.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_AddBtn.Create(m_Hwnd, ID_Add_Btn);
	//m_AddBtn.SetButtonText("增加");
	//m_AddBtn.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	//m_DeleteBtn.Create(m_Hwnd, ID_Delete_Btn);
	//m_DeleteBtn.SetButtonText("删除");
	//m_DeleteBtn.MoveWindow(rect.top, rect.left, rect.right - rect.left, rect.bottom - rect.top);

	////create List
	//m_hControl[ID_ListView_Contract] = CreateWindowEx(0, TEXT("SysListView32"),
	//	TEXT("List view"), WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT,
	//	nLeftStart, nTop, N_Group_Width - 48, 240,
	//	m_Hwnd, (HMENU)ID_ListView_Contract, hInstance, NULL);

	////设置List的相关属性 风格、列数、BkColor、TextBkColor
	//ListView_SetExtendedListViewStyleEx(m_hControl[ID_ListView_Contract], NULL, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT/**/);
	//ListView_SetSelectedColumn(m_hControl[ID_ListView_Contract], 2);
	//ListView_SetBkColor(m_hControl[ID_ListView_Contract], g_ColorRefData.GetColorWhite());
	//ListView_SetTextBkColor(m_hControl[ID_ListView_Contract], g_ColorRefData.GetColorWhite());
	//LV_COLUMN LvCol;

	//memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
	//LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
	//LvCol.fmt = LVCFMT_CENTER;
	//LvCol.cx = N_Group_Width - 148;

	////设置列名
	//LvCol.pszText = (LPWSTR)LoadResString(IDS_Contract).c_str();
	//ListView_InsertColumn(m_hControl[ID_ListView_Contract], 0, &LvCol);
	//LvCol.cx = 100;
	//LvCol.pszText = (LPWSTR)LoadResString(IDS_Position_Range).c_str();
	//ListView_InsertColumn(m_hControl[ID_ListView_Contract], 1, &LvCol);

	////初始化并显示表格
	//InitListView();
	//UpdateListView();

	////设置控件字体
	//for (int i = ID_Begin + 1; i < ID_End; i++)
	//{
	//	if (m_hControl[i] != 0)
	//	{
	//		SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
	//	}
	//}

}

void CapitalRiskMgtWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_Select_Contract_Btn:
		if (HIWORD(wParam) == BN_CLICKED)
			OnBtnSelect();
		break;
	case ID_Add_Btn:
		if (HIWORD(wParam) == BN_CLICKED)
			OnBtnAdd();
		break;
	case ID_Delete_Btn:
		if (HIWORD(wParam) == BN_CLICKED)
			OnBtnDelete();
		break;
	default:
		break;
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

void CapitalRiskMgtWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	if ((int)wParam == ID_ListView_Contract)
	{
		NMHDR *pNMHDR = (NMHDR *)lParam;
		LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		switch (pNMHDR->code)
		{
		case NM_DBLCLK:
		{
			LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
			wchar_t wtext[100] = { 0 };
			LVITEM lvitem;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = lpnmitem->iItem;
			lvitem.iSubItem = 0;
			lvitem.cchTextMax = sizeof(wtext) / sizeof(wtext[0]);
			lvitem.pszText = wtext;
			ListView_GetItem(m_hControl[ID_ListView_Contract], &lvitem);
			SetWindowText(m_EditContract.GetEditHwnd(), wtext);

			lvitem.iSubItem = 1;
			ListView_GetItem(m_hControl[ID_ListView_Contract], &lvitem);
			//SetWindowText(m_hControl[ID_Edit_Position_Minus], wtext);
		}
		break;
		}
	}
}

void CapitalRiskMgtWnd::OnBtnSelect()
{
	RECT rectvalid;
	//GetWindowRect(m_hControl[ID_Button_Select_Contract], &rectvalid);
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

void CapitalRiskMgtWnd::OnBtnAdd()
{
	char sText[51] = { 0 };
	//GetWindowTextA(m_hControl[ID_Edit_Position_Minus], sText, 51);
	int nRange = atoi(sText);
	if (nRange <= 0)
	{
		MessageBox(m_Hwnd, LoadResString(IDS_Invalid_Position).c_str(), LoadResString(IDS_Tips).c_str(), NULL);
		return;
	}

	//GetWindowTextA(m_hControl[ID_Edit_Contract], sText, 51);
	GetWindowTextA(m_EditContract.GetEditHwnd(), sText, 51);

	string sCode = sText;
	CConvert::TrimString(sCode);
	//判断是否是合法的交易合约
	TContractKey tempContract;
	memset(&tempContract, 0, sizeof(tempContract));
	if (!CConvert::TContractNoToTContractKey(sCode.c_str(), tempContract))
	{
		wchar_t msg[MAX_PATH];
		swprintf_s(msg, LoadResString(IDS_Invalid_Contract).c_str(), CConvert::UTF8ToUnicode(sCode).c_str());
		MessageBox(m_Hwnd, msg, LoadResString(IDS_Tips).c_str(), NULL);
		return;
	}
	if (g_StarApi)
	{
		SContractNoType typeId;
		typeId[0] = '\0';
		CConvert::TradeContractToSContractNoType(tempContract, typeId);

		if (tempContract.CommodityType == ctGoods)//合约到品种
		{
			if (!CConvert::IsValidQuoteCommodity(tempContract.ExchangeNo, tempContract.CommodityNo))
			{
				wchar_t msg[MAX_PATH];
				swprintf_s(msg, LoadResString(IDS_Invalid_Contract).c_str(), CConvert::UTF8ToUnicode(sCode).c_str());
				MessageBox(m_Hwnd, msg, LoadResString(IDS_Tips).c_str(), NULL);
				return;
			}
		}
		else//到合约
		{
			if (!CConvert::IsValidQuotContract(typeId))
			{
				wchar_t msg[MAX_PATH];
				swprintf_s(msg, LoadResString(IDS_Invalid_Contract).c_str(), CConvert::UTF8ToUnicode(sCode).c_str());
				MessageBox(m_Hwnd, msg, LoadResString(IDS_Tips).c_str(), NULL);
				return;
			}
		}

		m_cfgData.m_mapPositonRange[sCode] = nRange;

		UpdateListView();
	}
}

void CapitalRiskMgtWnd::OnBtnDelete()
{
	UINT count = ListView_GetSelectedCount(m_hControl[ID_ListView_Contract]);
	if (count <= 0)
		return;

	wchar_t cc[101] = { 0 };
	while (ListView_GetNextItem(m_hControl[ID_ListView_Contract], -1, LVNI_ALL | LVNI_SELECTED) != -1)
	{
		int nItem = ListView_GetNextItem(m_hControl[ID_ListView_Contract], -1, LVNI_ALL | LVNI_SELECTED);
		ListView_GetItemText(m_hControl[ID_ListView_Contract], nItem, 0, cc, sizeof(cc) / sizeof(cc[0]));
		m_cfgData.m_mapPositonRange.erase(LoadRC::unicode_to_ansi(cc).c_str());
		ListView_DeleteItem(m_hControl[ID_ListView_Contract], nItem);
	}
}


void CapitalRiskMgtWnd::InitListView()
{
	SetWindowLong(ListView_GetHeader(m_hControl[ID_ListView_Contract]), GWL_STYLE, HDS_FLAT);

	for (int i = 0; i < 2; i++)
	{
		HDITEM hditem;
		hditem.mask = HDI_FORMAT;
		Header_GetItem(ListView_GetHeader(m_hControl[ID_ListView_Contract]), i, &hditem);
		hditem.fmt = hditem.fmt | HDF_OWNERDRAW | HDF_FIXEDWIDTH;
		Header_SetItem(ListView_GetHeader(m_hControl[ID_ListView_Contract]), i, &hditem);
	}

	m_procListview = (WNDPROC)SetWindowLong(m_hControl[ID_ListView_Contract], GWL_WNDPROC, (LONG)ContractListViewProc);
	SetWindowLong(m_hControl[ID_ListView_Contract], GWL_USERDATA, LONG(this));
}

void CapitalRiskMgtWnd::SaveConfig()
{


}

void CapitalRiskMgtWnd::LoadConfig()
{
	
}

void CapitalRiskMgtWnd::UpdateListView()
{
	wchar_t cc[101] = { 0 };
	ListView_DeleteAllItems(m_hControl[ID_ListView_Contract]);
	int i = 0;
	for (auto it : m_cfgData.m_mapPositonRange)
	{
		wchar_t text[70] = { 0 };
		wcscpy_s(text, LoadRC::ansi_to_unicode(it.first).c_str());
		LV_ITEM ltm;
		ltm.mask = LVIF_TEXT | LVIF_IMAGE;
		ltm.pszText = text;
		ltm.iItem = i;
		ltm.iSubItem = 0;
		ListView_InsertItem(m_hControl[ID_ListView_Contract], &ltm);
		swprintf_s(cc, TEXT("%d"), it.second);
		ListView_SetItemText(m_hControl[ID_ListView_Contract], i, 1, cc);
		i++;
	}
}

void CapitalRiskMgtWnd::EnableCtrl(bool bEnable)
{
	//EnableWindow(m_EditCancel.GetHwnd(), bEnable);
}