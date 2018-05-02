#include "PreInclude.h"

extern G_COLOREF						g_ColorRefData;		
extern IStarApi *						g_StarApi;
extern IConfigFrame *					g_config_api;
extern bool								flagAuthent;
extern IAuthent *						g_pAuthent;
extern CPositionDiffBar					g_PosDiffBar;
extern ICspAccessApi *					g_pCspAccessApi;

#define Max1QtyLimitLen					2
#define Max2QtyLimitLen					3
#define MaxQtyLimit2					100
#define MaxQtyLimit1					50
#define MinQtyLimit						0  

const int	CY_GROUP					= 400;

//#define NUMLINES						((int) (sizeof sysmetrics / sizeof sysmetrics [0]))

LRESULT CALLBACK FillOrderSetEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	CPositonDiffWin *pPositonDiffWin = (CPositonDiffWin*)GetWindowLong(hwnd, GWL_USERDATA);
	if (!pPositonDiffWin)
		return FALSE;
	switch (message)
	{
	//case WM_KEYDOWN:
	//	switch (LOWORD(wParam))
	//	{
	//	case VK_TAB:
	//		HWND hwndnext = GetNextWindow(GetFocus(), GW_HWNDNEXT);
	//		SetFocus(hwndnext);
	//		break;
	//	}
	//	break;
	case WM_CHAR:
		switch (id)
		{
		case PD_Code_Edit:
			if ((wParam >= '0'&&wParam <= '9') || wParam == '\b')
				break;
			else if ((wParam >= 'a'&&wParam <= 'z') && PD_Code_Edit == id)
			{
				wParam = wParam - 32;
				break;
			}
			else
				return FALSE;
		case PD_Max1_Edit:
		{
			if (wParam >= '0'&&wParam <= '9')
			{
				DWORD select;
				select = Edit_GetSel(hwnd);
				int j = LOWORD(select);
				int i = HIWORD(select);
				//没有选定文本
				if (HIWORD(select) == LOWORD(select))
				{
					PosdifIntW sQty = { 0 };
					int nQty = 0;
					GetWindowText(hwnd, sQty, sizeof(PosdifIntW) / sizeof(wchar_t));
					nQty = _wtoi(sQty);
					int unit = wParam - 48;
					if ((nQty * 10 + unit) > 50)
						return FALSE;
					else 
						break;
				}
				//选定文本，不做判断
				else
					break;
			}
			else if (wParam == '\b')
				break;
			else
				return FALSE;
		}
		case PD_Max2_Edit:
		{
			if (wParam >= '0'&&wParam <= '9')
			{
				DWORD select;
				select = Edit_GetSel(hwnd);
				int j = LOWORD(select);
				int i = HIWORD(select);
				//没有选定文本
				if (HIWORD(select) == LOWORD(select))
				{
					PosdifIntW sQty = { 0 };
					int nQty = 0;
					GetWindowText(hwnd, sQty, sizeof(PosdifIntW) / sizeof(wchar_t));
					nQty = _wtoi(sQty);
					int unit = wParam - 48;
					if ((nQty * 10 + unit) > 100)
						return FALSE;
					else
						break;
				}
				//选定文本，不做判断
				else
					break;
			}
			else if (wParam == '\b')
				break;
			else
				return FALSE;
		}
		}
	//case EN_UPDATE:
	//{
	//	switch (id)
	//	{
	//	case PD_Max1_Edit:
	//	{
	//		PosdifIntW sQty = { 0 };
	//		int nQty = 0;
	//		GetWindowText(hwnd, sQty, sizeof(PosdifIntW) / sizeof(wchar_t));
	//		nQty = _wtoi(sQty);
	//		int unit = wParam - 48;
	//		if ((nQty * 10 + unit) > 50)
	//			return FALSE;
	//		else
	//			break;
	//	}
	//	case PD_Max2_Edit:
	//	{
	//		PosdifIntW sQty = { 0 };
	//		int nQty = 0;
	//		GetWindowText(hwnd, sQty, sizeof(PosdifIntW) / sizeof(wchar_t));
	//		nQty = _wtoi(sQty);
	//		int unit = wParam - 48;
	//		if ((nQty * 10 + unit) > 100)
	//			return FALSE;
	//		else
	//			break;
	//	}
	//	}
	//}
	case WM_SETFOCUS:
		if (id == PD_Max1_Edit)
			g_config_api->set_hint(CConvert::LoadResString(IDS_Hint50).c_str());
		if (id == PD_Max2_Edit)
			g_config_api->set_hint(CConvert::LoadResString(IDS_Hint100).c_str());
		break;
	case WM_KILLFOCUS:
		g_config_api->set_hint(L"");
		break;
	default:
		break;
	}
	map<int, WNDPROC>::iterator it = pPositonDiffWin->m_SetEdit.find(id);
	if (it == pPositonDiffWin->m_SetEdit.end())
		return FALSE;
	else
		return CallWindowProc(it->second, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ListViewProcSet(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);

	CPositonDiffWin *pPositonDiffWin = (CPositonDiffWin*)GetWindowLong(hwnd, GWL_USERDATA);

	if (!pPositonDiffWin)
		return false;
	switch (message)
	{
	case WM_DRAWITEM:
	{
		wchar_t		strMoneyThreshold[20];		//仓差阈值wstr
		//获取阈值
		//moneyThreshold = g_StarApi->GetConfigSwitchInfo();
		//static const SConfigSwitchNoType				S_CONFIGNO_EQUITYTHRESHOLD = "PositionDif.EquityThreshold";//仓差监测 权益阈值 （0-3200） 单位万
		int moneyThreshold = 0;
		SConfigSwitchInfo* temp[1];
		g_pCspAccessApi->GetConfigSwitchInfo(S_CONFIGNO_EQUITYTHRESHOLD, temp, 1, false);
		moneyThreshold = temp[0]->ConfigSwitchValue;

		DRAWITEMSTRUCT *pDraw = (DRAWITEMSTRUCT*)(lParam);
		wstring wstr = TEXT("");
		if (id == PD_ListView)
		{
			switch (pDraw->itemID)
			{
			case 0:
				wstr = CConvert::LoadResString(IDS_ContractNo);
				break;
			case 1:
				swprintf_s(strMoneyThreshold, CConvert::LoadResString(IDS_LowLimit).c_str(), moneyThreshold);
				//wstr = CConvert::LoadResString(IDS_LowLimit);
				wstr = strMoneyThreshold;
				break;
			case 2:
				swprintf_s(strMoneyThreshold, CConvert::LoadResString(IDS_OverLimit).c_str(), moneyThreshold);
				//wstr = CConvert::LoadResString(IDS_OverLimit);
				wstr = strMoneyThreshold;
				break;
			}
		}

		RECT rect = pDraw->rcItem;
		FillRect(pDraw->hDC, &rect, g_ColorRefData.GetHbrRGB150());
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
	if (id == PD_ListView)
		return CallWindowProc(pPositonDiffWin->m_ProcListview, hwnd, message, wParam, lParam);
	else
		return false;
}

CPositonDiffWin::CPositonDiffWin()
	:m_bPosDifDataModify(false)
	, m_StopVer(sVersion)
{
	m_rcRiskTip.left = 8;
	m_rcRiskTip.top = 350;
	m_rcRiskTip.right = 504;
	m_rcRiskTip.bottom = m_rcRiskTip.top + 60;
	memset(m_sRiskTip, 0, sizeof(m_sRiskTip));
	Init();
}

CPositonDiffWin ::~CPositonDiffWin()
{
}

//创建窗口
void CPositonDiffWin::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}



static int iVScrollBarPos, cxClient, cyClient;
static int cxChar, cyChar, cxCaps;
static int iDeltaPerLine = 40, iAccumDelta = 0;			//鼠标滑轮
HDC             hDC;
SCROLLINFO		si;
TEXTMETRIC      tm;
ULONG			ulScrollLines;
//窗口处理过程
LRESULT CPositonDiffWin::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	//case WM_SETTINGCHANGE:
	//	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);
	//	if (ulScrollLines)
	//		iDeltaPerLine = WHEEL_DELTA / ulScrollLines;  // ulScrollLines通常为3
	//	else
	//		iDeltaPerLine = 0;
	//	break;
	case WM_CREATE:					OnCreateWindow();
		break;
	case WM_PAINT:					OnPaint(wParam, lParam);
		break;
	case WM_NOTIFY:					OnNotify(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:				OnMouseWheel(wParam, lParam);
		break;
	case WM_SIZE:
		//改变大小时，先回滚到初始位置
		if (si.nPos != 0)
		{
			ScrollWindow(m_Hwnd, 0, si.nPos, NULL, NULL);
			InvalidateRect(m_Hwnd, NULL, FALSE);
			UpdateWindow(m_Hwnd);
		}

		//窗口大小改变时收到此消息
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		//滚动条属性
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMax = CY_GROUP;
		si.nMin = 0;
		si.nPos = 0;
		si.nPage = cyClient;
		//设定滚动条 
		SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE);


		////获取滚动条的新位置，即判断窗口大小改变以后滚动条是否超出了应有的最大范围  
		//iVScrollBarPos = min(cyClient, max(0, iVScrollBarPos));

		////如果滚动条超过了最大范围，则重设滚动条范围并刷新窗口  
		//if (iVScrollBarPos != GetScrollPos(m_Hwnd, SB_VERT))
		//{
		//	SetScrollPos(m_Hwnd, SB_VERT, iVScrollBarPos, SB_VERT);
		//	InvalidateRect(m_Hwnd, NULL, FALSE);
		//}
		break;
	case WM_VSCROLL:				
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(m_Hwnd, SB_VERT, &si);
		iVScrollBarPos = si.nPos;
		//垂直滚动条消息，F12查看消息可以从MSDN获取消息详细参数  
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case SB_LINEUP:
			si.nPos -= 10;
			break;
		case SB_LINEDOWN:
			si.nPos += 10;
			break;
		case SB_PAGEUP:
			//每次翻页都滚动一整个客户区的大小  
			si.nPos -= cyClient;
			break;
		case SB_PAGEDOWN:
			si.nPos += cyClient;
			break;
		case SB_THUMBTRACK:
			//si.nPos = HIWORD(wParam);
			si.nPos = si.nTrackPos;
			break;
		//case SB_THUMBPOSITION: // 客户区内容只有当滑动放开鼠标时才更新  
		//	si.nPos = HIWORD(wParam);
		//	break;
		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo(m_Hwnd, SB_VERT, &si);

		//判断滚动后的滚动条是否超过最大值或最小值，如果超过最大值或者最小值，则取最大值或0，否则等于当前值  
		//iVScrollBarPos = max(0, min(iVScrollBarPos, 100));

		//如果滚动条位置发生变化，则设置滚动条位置和刷新屏幕  
		if (si.nPos != iVScrollBarPos)
		{
			ScrollWindow(m_Hwnd, 0, iVScrollBarPos - si.nPos, NULL, NULL);
			//最后参数设置为FALSE可以大幅度减少屏幕闪烁，可以尝试一下。  
			InvalidateRect(m_Hwnd, NULL, FALSE);//不能
			UpdateWindow(m_Hwnd);
		}
		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LONG)m_hStaticBKBrush;
	}
	case WM_COMMAND:				OnCommond(wParam, lParam);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
			SendMessage(m_Hwnd, WM_VSCROLL, SB_LINEUP, 0);
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		//恢复默认
		si.fMask = SIF_POS;
		si.nPos = 0;
		SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE);

		//保存配置信息
		SaveParam();
		break;

	//case WM_MOUSEHOVER:				OnItemHover(int(wParam));
	//	break;
	//case WM_CHECK_LEAVE:			OnItemLeave(int(wParam));
	//	break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}

void CPositonDiffWin::Init()
{
	
	if (GetModulePathEx())
		LoadParam();
	//	LoadShowInfo();														//提示信息
	//m_log.open(m_strPathLog, false);
	//char cDate[24] = { 0 };
	//TimeFun::GetDateTime(cDate);
	//m_log.add_log(true, "\nOpen:	LocalTime:%s\r\n", cDate);
}


void CPositonDiffWin::OnCreateWindow()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	//if (!flagAuthent)
	//{
	//	UserAuthentEx stAuthent;
	//	memset(&stAuthent, 0, sizeof(UserAuthentEx));
	//	bool bRet = g_pAuthent->GetAuthent("HXQH", "pd2017", stAuthent);

	//	if (stAuthent.serverdate < stAuthent.auth.value[0].date)
	//	{
	//		MessageBox(GetHwnd(), CConvert::LoadResString(IDS_AuthentSuccess).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
	//		flagAuthent = true;
	//		//g_config_api->show(CConvert::LoadResString(IDS_Subtitle).c_str(), cmtExpand);
	//	}
	//	else
	//	{
	//		//MessageBox(GetHwnd(), CConvert::LoadResString(IDS_AuthentFailed).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
	//		HWND hwndAuthent = CreateWindowEx(0, L"static", CConvert::LoadResString(IDS_AuthentFailed).c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, 12 , 26, 92, 20, m_Hwnd, (HMENU)1, hInstance, NULL);
	//		SendMessage(hwndAuthent, WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
	//		return;
	//	}
	//}

	//背景
	m_hStaticBKBrush = g_ColorRefData.GetHbrWhite();//背景色
	HDC hdc = GetDC(m_Hwnd);
	RECT itemRect;
	GetClientRect(m_Hwnd, &itemRect);
	FillRect(hdc, &itemRect, m_hStaticBKBrush);
	ReleaseDC(m_Hwnd, hdc);

	//获取字体
	hDC = GetDC(m_Hwnd);

	GetTextMetrics(hDC, &tm);
	cxChar = tm.tmAveCharWidth;
	cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
	cyChar = tm.tmHeight + tm.tmExternalLeading;

	//滚动条设置
	SetScrollRange(m_Hwnd, SB_VERT, 0, 100, FALSE);
	SetScrollPos(m_Hwnd, SB_VERT, iVScrollBarPos, TRUE);

	//红色提示文字
	swprintf_s(m_sRiskTip, sizeof(m_sRiskTip) / 2, CConvert::LoadResString(IDS_Remarks).c_str(), L"\r\n", L"\r\n");
	//wcsncpy_s(m_sRiskTip, sizeof(m_sRiskTip) / sizeof(wchar_t), CConvert::LoadResString(IDS_Remarks).c_str(), sizeof(m_sRiskTip) / sizeof(wchar_t));

	const int nPitch = 1;
	const int nLeftStart = 12;
	const int nRightStart = 312;

	const int nNormalHeight = 20;		//统一高度
	const int nTextWidthShort = 82;		//统一的静态文本宽度 短 +Combox+Edit
	const int nTextWidthShort1 = 60;		//统一的静态文本宽度 短 +Combox+Edit
	const int nComboxWidth = 80;		//统一的combox宽度
	const int nWidthSpin = 16;			//统一spin宽度

	const int nFirstLine = 26;
	//const int nSecondLine = 104 + 26;
	const int nEditWidth = 33;
	const int nLongWidth = 200;
	const int nShortWidth = 80;

	//合约框
	m_hControl[PD_Code_Edit] = CreateWindowEx(0, L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, nLeftStart, nFirstLine, nLongWidth - nNormalHeight - 1, nNormalHeight, m_Hwnd, (HMENU)PD_Code_Edit, hInstance, NULL);
	//选合约button
	m_hControl[PD_Code_Button] = CreateWindowEx(0, L"button", L"...", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS , nLeftStart + nLongWidth - nNormalHeight, nFirstLine, nNormalHeight, nNormalHeight, m_Hwnd, (HMENU)PD_Code_Button, hInstance, NULL);
	SendMessage(m_hControl[PD_Code_Edit], EM_SETLIMITTEXT, 50, 0);
	//第一档仓差限制
	m_hControl[PD_Max1_Edit] = CreateWindowEx(0, L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, nLeftStart + nLongWidth + 10, nFirstLine, nEditWidth, nNormalHeight, m_Hwnd, (HMENU)PD_Max1_Edit, hInstance, NULL);
	m_hControl[PD_Max1_Spin] = CreateWindowEx(0, L"msctls_updown32", L"", WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_NOTHOUSANDS, nLeftStart + nLongWidth + nEditWidth + 11, nFirstLine, nWidthSpin, nNormalHeight, m_Hwnd, (HMENU)PD_Max1_Spin, hInstance, NULL);
	SendMessage(m_hControl[PD_Max1_Edit], EM_SETLIMITTEXT, Max1QtyLimitLen, 0);
	//第二档仓差限制
	m_hControl[PD_Max2_Edit] = CreateWindowEx(0, L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, nLeftStart + nLongWidth + nShortWidth + 10, nFirstLine, nEditWidth, nNormalHeight, m_Hwnd, (HMENU)PD_Max2_Edit, hInstance, NULL);
	m_hControl[PD_Max2_Spin] = CreateWindowEx(0, L"msctls_updown32", L"", WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_NOTHOUSANDS, nLeftStart + nLongWidth + nShortWidth + nEditWidth + 11, nFirstLine, nWidthSpin, nNormalHeight, m_Hwnd, (HMENU)PD_Max2_Spin, hInstance, NULL);
	SendMessage(m_hControl[PD_Max2_Edit], EM_SETLIMITTEXT, Max2QtyLimitLen, 0);
	//手
	m_hControl[PD_Hands_Static] = CreateWindowEx(0, L"static", CConvert::LoadResString(IDS_Lots).c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, nLeftStart + nLongWidth + nShortWidth * 2, nFirstLine, 23, nNormalHeight, m_Hwnd, (HMENU)PD_Hands_Static, hInstance, NULL);
	//增加删除
	m_hControl[PD_Add_Button] = CreateWindowEx(0, L"button", CConvert::LoadResString(IDS_Add).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, nLeftStart + 386, nFirstLine + 20, 80, nNormalHeight, m_Hwnd, (HMENU)PD_Add_Button, hInstance, NULL);
	m_hControl[PD_Delete_Button] = CreateWindowEx(0, L"button", CConvert::LoadResString(IDS_Delete).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, nLeftStart + 386, nFirstLine + 60, 80, nNormalHeight, m_Hwnd, (HMENU)PD_Delete_Button, hInstance, NULL);

	//表格
	m_hControl[PD_ListView] = CreateWindowEx(0, TEXT("SysListView32"),
		TEXT("List view"), WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT/* | LVS_EDITLABELS*/,
		nLeftStart, nFirstLine + 21, 361, 300,
		m_Hwnd, (HMENU)PD_ListView, hInstance, NULL);
	
	ListView_SetExtendedListViewStyleEx(m_hControl[PD_ListView], NULL, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT/**/);
	ListView_SetSelectedColumn(m_hControl[PD_ListView], 3);
	ListView_SetBkColor(m_hControl[PD_ListView], g_ColorRefData.GetColorWhite());
	ListView_SetTextBkColor(m_hControl[PD_ListView], g_ColorRefData.GetColorWhite());
	LV_COLUMN LvCol;

	memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
	LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
	LvCol.fmt = LVCFMT_CENTER;
	LvCol.cx = nLongWidth;

	LvCol.pszText = (LPWSTR)CConvert::LoadResString(IDS_ContractNo).c_str();
	ListView_InsertColumn(m_hControl[PD_ListView], 0, &LvCol);

	LvCol.cx = nShortWidth;

	LvCol.pszText = (LPWSTR)CConvert::LoadResString(IDS_LowLimit).c_str();
	ListView_InsertColumn(m_hControl[PD_ListView], 1, &LvCol);

	LvCol.pszText = (LPWSTR)CConvert::LoadResString(IDS_OverLimit).c_str();
	ListView_InsertColumn(m_hControl[PD_ListView], 2, &LvCol);

	InitListView();

	m_hControl[PD_Config_Groupbox] = CreateWindowEx(0, L"button", CConvert::LoadResString(IDS_PDGroupBox).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 0/*84 + 26*/, 504, CY_GROUP, m_Hwnd, (HMENU)PD_Config_Groupbox, hInstance, NULL);

	//设置控件字体
	for (int i = PD_START ; i <= PD_END; i++)
	{
		if (m_hControl[i] != 0)
		{
			HWND h = m_hControl[i];
			if (i == PD_Max2_Edit || i == PD_Max1_Edit)
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontNum13(), (LPARAM)TRUE);
			else
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
		}
	}

	SetWindowPos(m_hControl[PD_Max1_Spin], NULL, 0, 0, nWidthSpin, nNormalHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[PD_Max2_Spin], NULL, 0, 0, nWidthSpin, nNormalHeight, SWP_NOMOVE | SWP_NOZORDER);

	m_hwndEdit[0].Init(m_hControl[PD_Code_Edit]);
	m_hwndEdit[1].Init(m_hControl[PD_Max1_Edit]);
	m_hwndEdit[2].Init(m_hControl[PD_Max2_Edit]);

	m_hwndSpin[0].Init(m_hControl[PD_Max1_Spin]);
	m_hwndSpin[1].Init(m_hControl[PD_Max2_Spin]);

	m_hwndBtn[0].Init(m_hControl[PD_Code_Button]);
	m_hwndBtn[1].Init(m_hControl[PD_Add_Button]);
	m_hwndBtn[2].Init(m_hControl[PD_Delete_Button]);

	//编辑框控件 子类化处理查过程 
	m_SetEdit.clear();
	vector<int> vID;
	vID.push_back(PD_Code_Edit);
	vID.push_back(PD_Max1_Edit);
	vID.push_back(PD_Max2_Edit);

	for (vector<int>::iterator iter = vID.begin(); iter != vID.end(); iter++)
	{
		WNDPROC w = (WNDPROC)SetWindowLong(m_hControl[*iter], GWL_WNDPROC, (LONG)FillOrderSetEditProc);	//默认价格
		m_SetEdit.insert(pair<int, WNDPROC>(*iter, w));
		SetWindowLong(m_hControl[*iter], GWL_USERDATA, LONG(this));//用户数据
	}
	LoadParam(true);
}


void CPositonDiffWin::OnPaint(WPARAM wParam, LPARAM lParam)		//重绘
{
	HDC			hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(m_Hwnd, &ps);
	//RECT itemRect;
	//GetClientRect(m_Hwnd, &itemRect);
	//FillRect(hdc, &itemRect, m_hStaticBKBrush);

	SelectObject(hdc, g_FontData.GetFontWord12());
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, g_ColorRefData.GetColorBuyRed());

	m_rcRiskTip.top = 350;
	m_rcRiskTip.top -= si.nPos;
	m_rcRiskTip.bottom = m_rcRiskTip.top + 60;
	DrawText(hdc, m_sRiskTip, wcslen(m_sRiskTip), &m_rcRiskTip, DT_LEFT | DT_VCENTER/* | DT_SINGLELINE */);

	//hdcSrc = CreateCompatibleDC(hdc);
	//SelectObject(hdcSrc, m_Hwnd);
	//BitBlt(hdc, 0, -iVScrollBarPos, 0, cyClient, hdcSrc, 0, 0, SRCCOPY);

	EndPaint(m_Hwnd, &ps);
}
void CPositonDiffWin::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case PD_ListView:
		OnNMClick(PD_ListView, lParam);
		break;
	case PD_Max1_Spin:
		switch (((LPNMHDR)lParam)->code)
		{
		case UDN_DELTAPOS:
			OnSpinDeltaUpDownNormal(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1, MaxQtyLimit1, MinQtyLimit);
			break;
		}
		break;
	case PD_Max2_Spin:
		switch (((LPNMHDR)lParam)->code)
		{
		case UDN_DELTAPOS:
			OnSpinDeltaUpDownNormal(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1, MaxQtyLimit2, MinQtyLimit);
			break;
		}
		break;
	}
}

void CPositonDiffWin::OnCommond(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case PD_Code_Button:
		if (HIWORD(wParam) == BN_CLICKED)
			OnButtonCodeSelect();
		break;
	case PD_Add_Button:
		if (HIWORD(wParam) == BN_CLICKED)
			OnButtonAddItem();
		break;
	case PD_Delete_Button:
		if (HIWORD(wParam) == BN_CLICKED)
			OnButtonDeleteItem();
		break;
	default:
		break;
	}
}

void CPositonDiffWin::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int nID = GetWindowLong(GetFocus(), GWL_ID);
	switch (nID)
	{
	case PD_Max1_Edit:
		OnSpinDeltaUpDownNormal(GET_WHEEL_DELTA_WPARAM(wParam), nID, MaxQtyLimit1, MinQtyLimit);
		break;
	case PD_Max2_Edit:
		OnSpinDeltaUpDownNormal(GET_WHEEL_DELTA_WPARAM(wParam), nID, MaxQtyLimit2, MinQtyLimit);
		break;
	default:
		if(iDeltaPerLine == 0)
			break;
		iAccumDelta += (short)HIWORD(wParam);
		while (iAccumDelta >= iDeltaPerLine)
		{
			SendMessage(m_Hwnd, WM_VSCROLL, SB_LINEUP, 0);
			iAccumDelta -= iDeltaPerLine;
		}
		while (iAccumDelta <= -iDeltaPerLine)
		{
			SendMessage(m_Hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			iAccumDelta += iDeltaPerLine;
		}
		break;
	}
}

void CPositonDiffWin::OnVScroll(WPARAM wParam, LPARAM lParam)
{

}

void CPositonDiffWin::OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax, int nMin) //数量增减处理
{
	if (nDelta > 0)
	{
		PosdifIntW sInt = { 0 };
		GetWindowTextW(m_hControl[nID], sInt, sizeof(PosdifIntW) / sizeof(wchar_t));
		int a = _wtoi(sInt);
		a++;
		a = a > nMax ? nMax : a;
		SetWindowInt(m_hControl[nID], a);
	}
	else
	{
		PosdifIntW sInt = { 0 };
		GetWindowTextW(m_hControl[nID], sInt, sizeof(PosdifIntW) / sizeof(wchar_t));
		int a = _wtoi(sInt);
		a--;
		if (PD_Max1_Edit == nID || PD_Max2_Edit == nID)
			a = a < 1 ? 0 : a;
		else
			a = a < nMin ? nMin : a;
		SetWindowInt(m_hControl[nID], a);
	}
}

void CPositonDiffWin::InitListView()
{
	SetWindowLong(ListView_GetHeader(m_hControl[PD_ListView]), GWL_STYLE, HDS_FLAT);
	for (int i = 0; i < 3; i++)
	{
		HDITEM hditem;
		hditem.mask = HDI_FORMAT;
		Header_GetItem(ListView_GetHeader(m_hControl[PD_ListView]), i, &hditem);
		hditem.fmt = hditem.fmt | HDF_OWNERDRAW;
		Header_SetItem(ListView_GetHeader(m_hControl[PD_ListView]), i, &hditem);
	}
	m_ProcListview = (WNDPROC)SetWindowLong(m_hControl[PD_ListView], GWL_WNDPROC, (LONG)ListViewProcSet);
	SetWindowLong(m_hControl[PD_ListView], GWL_USERDATA, LONG(this));
}

void CPositonDiffWin::OnNMClick(int nID, LPARAM lParam)
{
	NMHDR *pNMHDR = (NMHDR *)lParam;
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	switch (pNMHDR->code)
	{
	case NM_DBLCLK:
	{
		LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;

		if (lpnmitem->iItem != -1)
		{
			TPosdifListEditStr wtext = { 0 };
			LVITEM lvitem;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = lpnmitem->iItem;
			lvitem.iSubItem = 0;
			lvitem.cchTextMax = sizeof(wtext) / sizeof(wtext[0]);
			lvitem.pszText = wtext;
			ListView_GetItem(m_hControl[nID], &lvitem);
			SetWindowText(m_hControl[PD_Code_Edit], wtext);
			lvitem.iSubItem = 1;
			ListView_GetItem(m_hControl[nID], &lvitem);
			SetWindowText(m_hControl[PD_Max1_Edit], wtext);
			lvitem.iSubItem = 2;
			ListView_GetItem(m_hControl[nID], &lvitem);
			SetWindowText(m_hControl[PD_Max2_Edit], wtext);
		}
	}
	break;
	}
}


void CPositonDiffWin::SetWindowInt(HWND hWnd, int nInt)
{
	PosdifIntW cQty = { 0 };
	swprintf_s(cQty, L"%d", nInt);
	SetWindowTextW(hWnd, cQty);
}

void CPositonDiffWin::OnButtonCodeSelect()
{
	RECT rectvalid;
	GetWindowRect(m_hControl[PD_Code_Button], &rectvalid);
	if (!IsWindow(m_pPopWinCode.GetHwnd()))
	{
		m_pPopWinCode.CreatePopWin(this, TEXT("m_pPopWinCode"), m_Hwnd, WS_CLIPCHILDREN, 0, rectvalid.left - 64 + 3, rectvalid.bottom);
		m_pPopWinCode.UpdateData();
		::ShowWindow(m_pPopWinCode.GetHwnd(), SW_SHOWNORMAL);
	}
	else
	{
		m_pPopWinCode.UpdateData();
		SetWindowPos(m_pPopWinCode.GetHwnd(), NULL, rectvalid.left - 64 + 3, rectvalid.bottom, 0, 0, SWP_NOSIZE);
		::ShowWindow(m_pPopWinCode.GetHwnd(), SW_SHOWNORMAL);
	}
}

void CPositonDiffWin::OnButtonAddItem()
{
	TPosDifContract sText = { 0 };
	GetWindowTextA(m_hControl[PD_Code_Edit], sText, sizeof(TPosDifContract));

	string sCode = sText;
	CConvert::TrimString(sCode);
	//判断是否是合法的交易合约
	TContractKey tempContract;
	memset(&tempContract, 0, sizeof(tempContract));
	if (!CConvert::TContractNoToTContractKey(sCode.c_str(), tempContract))
	{
		MessageBox(m_Hwnd, CConvert::LoadResString(IDS_InvalidCode).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
		return;
	}
	else if (CConvert::IsCombContract(tempContract.CommodityType))
	{
		MessageBox(m_Hwnd, CConvert::LoadResString(IDS_CmbUnSupport).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
		return;
	}
	SContractNoType typeId;
	if (g_StarApi)
	{
		typeId[0] = '\0';
		CConvert::TContractKeyToSContractNo(tempContract, typeId);
		if (!CConvert::IsValidQuotContract(typeId))
		{
			MessageBox(m_Hwnd, CConvert::LoadResString(IDS_InvalidQuoteCode).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
			return;
		}
	}
	else
	{
		MessageBox(m_Hwnd, CConvert::LoadResString(IDS_InvalidQuoteCode).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
		return;
	}

	TPostionDifInfo cv = { 0 };
	strcpy_s(cv.Code, sCode.c_str());
	//数量合法
	PosdifIntW sQty = { 0 };
	int nQty = 0;
	GetWindowText(m_hControl[PD_Max1_Edit], sQty, sizeof(PosdifIntW) / sizeof(wchar_t));
	nQty = _wtoi(sQty);
	cv.nMax1Dif = nQty > 0 ? nQty : 0;
	GetWindowText(m_hControl[PD_Max2_Edit], sQty, sizeof(PosdifIntW) / sizeof(wchar_t));
	nQty = _wtoi(sQty);
	cv.nMax2Dif = nQty > 0 ? nQty : 0;

	if (CConvert::TContractNoToTContractKey(sCode.c_str(), tempContract))
	{
		//焦煤焦炭动煤不可设置
		//if (exDCE == tempContract.ExchangeNo && ("J" == tempContract.CommodityNo || "JM" == tempContract.CommodityNo))
		if (strcmp(tempContract.ExchangeNo, exDCE) == 0 && (strcmp(tempContract.CommodityNo, "J") == 0 || strcmp(tempContract.CommodityNo, "JM") == 0))
		{
			//if (cv.nMax1Dif > 5 || cv.nMax2Dif > 10)
			//{
			MessageBox(m_Hwnd, CConvert::LoadResString(IDS_NotSetCommodity).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
			return;
			//}
		}

		if (strcmp(tempContract.ExchangeNo, exZCE) == 0 && strcmp(tempContract.CommodityNo, "ZC") == 0 )
		{
			//if (cv.nMax1Dif > 5 || cv.nMax2Dif > 10)
			//{
			MessageBox(m_Hwnd, CConvert::LoadResString(IDS_NotSetCommodity).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
			return;
			//}
		}
	}

	if (cv.nMax1Dif == 0 || cv.nMax1Dif > 50 || cv.nMax2Dif == 0 || cv.nMax2Dif > 100)
	{
		MessageBox(m_Hwnd, CConvert::LoadResString(IDS_InvalidLots).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), NULL);
		return;
	}
	//vector<TPostionDifInfo> vecTmp;
	GetListViewVec(vecTmp);
	bool found = false;
	for (int i = 0; i < (int)vecTmp.size(); i++)
	{
		TPostionDifInfo &temp = vecTmp[i];
		if (strcmp(temp.Code, cv.Code) == 0)//找到了
		{
			temp.nMax1Dif = cv.nMax1Dif;
			temp.nMax2Dif = cv.nMax2Dif;
			found = true;
		}
	}
	if (!found)
		vecTmp.push_back(cv);
	FillDefaultNumListView(vecTmp);
	m_bPosDifDataModify = true;

	//状态栏改变颜色
	if (vecTmp.size() == 1)
	{
		g_PosDiffBar.IsActive();
		//InvalidateRect(g_PosDiffBar.GetHwnd(), NULL, true);
		SendMessage(g_PosDiffBar.GetHwnd(), WM_PAINT, 0, 0);
	}
}

void CPositonDiffWin::OnButtonDeleteItem()
{
	UINT count = ListView_GetSelectedCount(m_hControl[PD_ListView]);
	if (count <= 0)	return;
	while (ListView_GetNextItem(m_hControl[PD_ListView], -1, LVNI_ALL | LVNI_SELECTED) != -1)
	{
		int nItem = ListView_GetNextItem(m_hControl[PD_ListView], -1, LVNI_ALL | LVNI_SELECTED);
		ListView_DeleteItem(m_hControl[PD_ListView], nItem);
	}
	//获取原始
	//vector<TPostionDifInfo> vectmp;
	GetListViewVec(vecTmp);
	FillDefaultNumListView(vecTmp);
	m_bPosDifDataModify = true;

	//状态栏改变颜色
	if (vecTmp.size() == 0)
	{
		g_PosDiffBar.IsActive();
		//InvalidateRect(g_PosDiffBar.GetHwnd(), NULL, true);
		SendMessage(g_PosDiffBar.GetHwnd(), WM_PAINT, 0, 0);
	}

}


void CPositonDiffWin::SetContractNo(string sCode, SCurrency* sCurrency)
{
	SetWindowTextA(m_hControl[PD_Code_Edit], sCode.c_str());
}


void CPositonDiffWin::GetListViewVec(vPosDifCfg&vec)
{
	vec.clear();
	int count = ListView_GetItemCount(m_hControl[PD_ListView]);
	TPosdifListEditStr cc = { 0 };
	TPostionDifInfo tmp = { 0 };
	for (int i = 0; i < count; i++)
	{
		ListView_GetItemText(m_hControl[PD_ListView], i, 0, cc, sizeof(cc) / sizeof(cc[0]));
		strcpy_s(tmp.Code, LoadRC::unicode_to_ansi(cc).c_str());

		ListView_GetItemText(m_hControl[PD_ListView], i, 1, cc, sizeof(cc) / sizeof(cc[0]));
		tmp.nMax1Dif = _wtoi(cc);

		ListView_GetItemText(m_hControl[PD_ListView], i, 2, cc, sizeof(cc) / sizeof(cc[0]));
		tmp.nMax2Dif = _wtoi(cc);
		
		vec.push_back(tmp);
	}
}
void  CPositonDiffWin::FillDefaultNumListView(vPosDifCfg&vec)
{
	PosdifIntW cc = { 0 };
	ListView_DeleteAllItems(m_hControl[PD_ListView]);
	for (int i = 0; i < (int)vec.size(); i++)
	{
		TPosdifListEditStr text = { 0 };
		wcscpy_s(text, LoadRC::ansi_to_unicode(vec[i].Code).c_str());
		LV_ITEM ltm;
		ltm.mask = LVIF_TEXT;
		ltm.pszText = text;
		ltm.iItem = i;
		ltm.iSubItem = 0;
		ListView_InsertItem(m_hControl[PD_ListView], &ltm);

		swprintf_s(cc, TEXT("%d"), vec[i].nMax1Dif);
		ListView_SetItemText(m_hControl[PD_ListView], i, 1, cc);

		swprintf_s(cc, TEXT("%d"), vec[i].nMax2Dif);
		ListView_SetItemText(m_hControl[PD_ListView], i, 2, cc);

	}
}


bool CPositonDiffWin::LoadParam(bool bUpdateToUI)
{
	vecTmp.clear();
	bool bValidFile = true;
	FILE *File = NULL;
	errno_t err = fopen_s(&File, m_strPathCodeData, "rb");

	//vector<TPostionDifInfo> vectmp;

	//无效文件
	if (!err)
	{
		fseek(File, 0, SEEK_END);
		int fileSize = ftell(File);
		fseek(File, 0, SEEK_SET);

		int FileVer = 0, nCount = 0;
		fread(&FileVer, sizeof(FileVer), 1, File);//读取文件头	
		if (sVersion == FileVer&&fileSize != ftell(File))
		{
			fread(&nCount, sizeof(nCount), 1, File);
			for (int i = 0; i < nCount; i++)
			{
				TPostionDifInfo sinfo = { 0 };
				fread(&sinfo, sizeof(sinfo), 1, File);
				vecTmp.push_back(sinfo);
			}
		}
		fclose(File);
	}

	if (bUpdateToUI)
	{
		if (!vecTmp.empty())
			FillDefaultNumListView(vecTmp);
	}
	return true;
}

bool   CPositonDiffWin::SaveParam()
{
	//数据
	//vector<TPostionDifInfo> vectmp;
	GetListViewVec(vecTmp);

	FILE *File = NULL;
	errno_t err = fopen_s(&File, m_strPathCodeData, "wb");
	if (!err)
	{
		fwrite((void*)&m_StopVer, sizeof(m_StopVer), 1, File);
		int nsize = vecTmp.size();
		fwrite((void*)&nsize, sizeof(nsize), 1, File);
		for (vector<TPostionDifInfo>::iterator iter = vecTmp.begin(); iter != vecTmp.end(); iter++)
			fwrite(&(TPostionDifInfo(*iter)), sizeof(TPostionDifInfo), 1, File);
		fclose(File);
	}

	if (m_bPosDifDataModify)
	{
		char cDate[24] = { 0 };
		TimeFun::GetDateTime(cDate);
		//m_log.add_log(true, "DataUpdate:	LocalTime:%s\r\n", cDate);
		//for (vector<TPostionDifInfo>::iterator iter = vecTmp.begin(); iter != vecTmp.end(); iter++)
		//	m_log.add_log(true, "  PositionDiffData:  Code:%-30s LowLimit:%d HighLimit:%d\r\n", iter->Code, iter->nMax1Dif, iter->nMax2Dif);
		m_bPosDifDataModify = false;
	}

	return true;
}


bool CPositonDiffWin::GetModulePathEx()
{
	char str[_MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL/*hInst*/, str, sizeof(str));
	string str1 = str;
	int npos = str1.find_last_of(TEXT('\\'));
	str1 = str1.substr(0, npos);
	string str2 = str1 + ("\\config\\PositonDiff.RiskCtrlData.pri");
	strcpy_s(m_strPathCodeData, sizeof(m_strPathCodeData), str2.c_str());

	char cDate[24] = { 0 };
	TimeFun::GetDate(cDate);
	str1 = str;
	npos = str1.find_last_of(TEXT('\\'));
	str1 = str1.substr(0, npos);
	str2 = str1 + ("\\log\\positiondiff\\") + cDate + ".set.log";
	strcpy_s(m_strPathLog, sizeof(m_strPathLog), str2.c_str());

	return true;
}


void CPositonDiffWin::OnItemHover(int nID)
{
	if(nID == PD_Max1_Edit || nID == PD_Max1_Spin)
		g_config_api->set_hint(CConvert::LoadResString(IDS_Hint50).c_str());
}