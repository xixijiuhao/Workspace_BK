#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
#define cCAPTION_H			45
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体

MatchWnd::MatchWnd()
{
}

MatchWnd::~MatchWnd()
{
}

void MatchWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void MatchWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void MatchWnd::AddMatchToList(SmsMatchDataRsp & match, int index)
{
	int count = 0;
	if (index == -1)
		count = m_listMatch.GetItemCount();
	else
		count = index;
	//交易员编号
	m_listMatch.AddItem(match.UserNo, count, MATCH_UserNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合约编号
	string sCode = match.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	m_listMatch.AddItem(tCode.c_str(), count, MATCH_ContractNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//买卖方向
	if (match.Direct == SMS_DIRECT_BUY)
		m_listMatch.AddItem("买", count, MATCH_Direct, DT_LEFT, g_ColorRefData.GetColorBuyRed());
	else if (match.Direct == SMS_DIRECT_SELL)
		m_listMatch.AddItem("卖", count, MATCH_Direct, DT_RIGHT, g_ColorRefData.GetColorSellBlue());
	//开平
	if (match.Offset == SMS_O_OPEN)
		m_listMatch.AddItem("开仓", count, MATCH_Offset, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	else if (match.Offset == SMS_O_COVER)
		m_listMatch.AddItem("平仓", count, MATCH_Offset, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	//成交价格
	m_listMatch.AddItem((double)match.MatchPrice, count, MATCH_MatchPrice, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//成交数量
	m_listMatch.AddItem((double)match.MatchQty, count, MATCH_MatchQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//手续费
	m_listMatch.AddItem((double)match.MatchFee, count, MATCH_MatchFee, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//成交时间
	m_listMatch.AddItem(match.MatchTime, count, MATCH_MatchTime, DT_CENTER, g_ColorRefData.GetColorBlack());
	//投保类型
	if (match.Hedge == SMS_HEDGE_SPECULATE)
		m_listMatch.AddItem("投机", count, MATCH_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (match.Hedge == SMS_HEDGE_HEDGE)
		m_listMatch.AddItem("套保", count, MATCH_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	else
		m_listMatch.AddItem("无", count, MATCH_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	//结算日期
	m_listMatch.AddItem(match.SettleDate, count, MATCH_SettleDate, DT_CENTER, g_ColorRefData.GetColorBlack());
	//公司编号
	m_listMatch.AddItem(match.CompanyNo, count, MATCH_CompanyNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//序列号
	m_listMatch.AddItem((double)match.SerialId, count, MATCH_SerialId, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//成交编号
	m_listMatch.AddItem(match.MatchNo, count, MATCH_MatchNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//系统号
	m_listMatch.AddItem(match.SystemNo, count, MATCH_SystemNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//委托编号
	m_listMatch.AddItem(match.OrderNo, count, MATCH_OrderNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//本地编号
	m_listMatch.AddItem(match.LocalNo, count, MATCH_LocalNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//操作员
	m_listMatch.AddItem(match.OperatorNo, count, MATCH_OperatorNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//修改时间
	m_listMatch.AddItem(match.OperateTime, count, MATCH_OperateTime, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT MatchWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_MATCH_REFRESH:
		RefreshList();
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM://==7
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_NCDESTROY:
		OnNcDestroy();
		break;
	case WM_DESTROY:
		OnDestroy();
		break;
	default:
		return NOT_PROCESSED;
	}
	return NOT_PROCESSED;
}

void MatchWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	///////////////////////////////Create//////////////////////////////////
	//开始日期
	m_labBeginDate.Create(m_Hwnd, L"开始日期", DT_RIGHT, ID_LabBeginDate);
	m_beginTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtBeginDate, m_hInstance, NULL);
	//结束日期
	m_labEndDate.Create(m_Hwnd, L"结束日期", DT_RIGHT, ID_LabEndDate);
	m_endTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtEndDate, m_hInstance, NULL);
	//合同
	m_labAgreementNo.Create(m_Hwnd, L"合同编号", DT_CENTER, ID_LabAgreementNo);
	m_cbAgreementNo.Create(m_Hwnd, ID_CbAgreementNo);

	//查询
	m_btnQry.Create(m_Hwnd, "查  询", ID_BtnQry);
	//表格
	m_listMatch.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Match);
	//RECT rect;
	//GetClientRect(m_Hwnd, &rect);
	//m_listMatch.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	//////////////////////////////Init////////////////////////////////////
	for (int i = MATCH_UserNo; i < MATCH_OperateTime; i++)
	{
		switch (i)
		{
		case MATCH_SerialId:
			m_listMatch.AddColumn(L"序列号", 80, MATCH_SerialId);
			//m_listCloseMatch.AddColumn(L"平仓成交", 80, MATCH_SerialId);
			break;
		case MATCH_CompanyNo:
			m_listMatch.AddColumn(L"公司编号", 80, MATCH_CompanyNo);
			//m_listCloseMatch.AddColumn(L"公司编号", 80, MATCH_CompanyNo);
			break;
		case MATCH_UserNo:
			m_listMatch.AddColumn(L"账号", 80, MATCH_UserNo);
			//m_listCloseMatch.AddColumn(L"账号", 80, MATCH_UserNo);
			break;
		case MATCH_SettleDate:
			m_listMatch.AddColumn(L"结算日期", 80, MATCH_SettleDate);
			//m_listCloseMatch.AddColumn(L"结算日期", 80, MATCH_SettleDate);
			break;
		case MATCH_ContractNo:
			m_listMatch.AddColumn(L"合约编号", 130, MATCH_ContractNo);
			//m_listCloseMatch.AddColumn(L"合约编号", 80, MATCH_ContractNo);
			break;
		case MATCH_MatchNo:
			m_listMatch.AddColumn(L"成交编号", 150, MATCH_MatchNo);
			//m_listCloseMatch.AddColumn(L"成交编号", 80, MATCH_MatchNo);
			break;
		case MATCH_SystemNo:
			m_listMatch.AddColumn(L"系统号", 150, MATCH_SystemNo);
			//m_listCloseMatch.AddColumn(L"系统号", 80, MATCH_SystemNo);
			break;
		case MATCH_OrderNo:
			m_listMatch.AddColumn(L"委托编号", 150, MATCH_OrderNo);
			//m_listCloseMatch.AddColumn(L"委托编号", 80, MATCH_OrderNo);
			break;
		case MATCH_LocalNo:
			m_listMatch.AddColumn(L"本地编号", 150, MATCH_LocalNo);
			//m_listCloseMatch.AddColumn(L"本地编号", 80, MATCH_LocalNo);
			break;
		case MATCH_Direct:
			m_listMatch.AddColumn(L"买卖", 40, MATCH_Direct);
			//m_listCloseMatch.AddColumn(L"买卖方向", 80, MATCH_Direct);
			break;
		case MATCH_Offset:
			m_listMatch.AddColumn(L"开平", 80, MATCH_Offset);
			//m_listCloseMatch.AddColumn(L"开平", 80, MATCH_Offset);
			break;
		case MATCH_Hedge:
			m_listMatch.AddColumn(L"投保类型", 80, MATCH_Hedge);
			//m_listCloseMatch.AddColumn(L"投保类型", 80, MATCH_Hedge);
			break;
		case MATCH_MatchPrice:
			m_listMatch.AddColumn(L"成交价格", 80, MATCH_MatchPrice);
			//m_listCloseMatch.AddColumn(L"成交价格", 80, MATCH_MatchPrice);
			break;
		case MATCH_MatchQty:
			m_listMatch.AddColumn(L"成交数量", 80, MATCH_MatchQty);
			//m_listCloseMatch.AddColumn(L"成交数量", 80, MATCH_MatchQty);
			break;
		case MATCH_MatchFee:
			m_listMatch.AddColumn(L"手续费", 80, MATCH_MatchFee);
			//m_listCloseMatch.AddColumn(L"手续费", 80, MATCH_MatchFee);
			break;
		case MATCH_MatchTime:
			m_listMatch.AddColumn(L"成交时间", 130, MATCH_MatchTime);
			//m_listCloseMatch.AddColumn(L"成交时间", 80, MATCH_MatchTime);
			break;
		case MATCH_OperatorNo:
			m_listMatch.AddColumn(L"操作员", 80, MATCH_OperatorNo);
			//m_listCloseMatch.AddColumn(L"操作员", 80, MATCH_OperatorNo);
			break;
		case MATCH_OperateTime:
			m_listMatch.AddColumn(L"修改时间", 130, MATCH_OperateTime);
			//m_listCloseMatch.AddColumn(L"修改时间", 130, MATCH_OperateTime);
			break;
		default:
			break;
		}
	}

	m_menuMatch = CreatePopupMenu();

	AppendMenu(m_menuMatch, MF_STRING, IDM_AddMatch, L"录入成交");
	AppendMenu(m_menuMatch, MF_STRING, IDM_ModMatch, L"修改成交");
	AppendMenu(m_menuMatch, MF_STRING, IDM_DelMatch, L"删除成交");
	AppendMenu(m_menuMatch, MF_SEPARATOR, IDM_LineMatch, L"");
	AppendMenu(m_menuMatch, MF_STRING, IDM_ManageManualClose, L"管理指定平仓");

	m_labAgreementNo.SetFont(g_FontData.GetFontWord13());
	m_cbAgreementNo.SetFont(g_FontData.GetFontNum15());

	m_labBeginDate.SetFont(g_FontData.GetFontWord13());
	m_labEndDate.SetFont(g_FontData.GetFontWord13());

	m_cbtBeginDate.Init(m_beginTimePick);
	SendMessage(m_beginTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_beginTimePick, L"yyyy-MM-dd");
	m_cbtEndDate.Init(m_endTimePick);
	SendMessage(m_endTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_endTimePick, L"yyyy-MM-dd");
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtEndDate.GetHwnd(), GDT_VALID, &time);
	time.wDay = 1;
	DateTime_SetSystemtime(m_cbtBeginDate.GetHwnd(), GDT_VALID, &time);
	/////////////////////POS///////////////////////////////////
	int leftstart = 10;
	int topstart = 10;
	int editWidth = 120;	//edit控件宽度
	int labWidth = 60;		//lab控件宽度
	int btnWidth = 80;		//btn控件宽度
	int Height = 25;		//控件高度
	int htap = 10;			//横向间隔

	POINT pt;
	pt.x = leftstart;
	pt.y = topstart;
	m_labBeginDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtBeginDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labEndDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtEndDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_labAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height * 10);
	pt.x += editWidth + htap;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listMatch.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top);
}

void MatchWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void MatchWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
		case IDM_AddMatch:	
			if (!IsWindow(m_addMatchWnd.GetHwnd()))
			{
				m_addMatchWnd.ShowFrm(m_Hwnd);
				m_addMatchWnd.Register(m_System);
			}
			else
			{
				if (!IsWindowVisible(m_addMatchWnd.GetHwnd()))
					ShowWindow(m_addMatchWnd.GetHwnd(), SW_SHOW);
			}
			m_addMatchWnd.InitWnd();
			break;
		case IDM_ModMatch://修改合同
		{
			if (!IsWindow(m_addMatchWnd.GetHwnd()))
			{
				m_addMatchWnd.ShowFrm(m_Hwnd);
				m_addMatchWnd.Register(m_System);
			}
			else
			{
				if (!IsWindowVisible(m_addMatchWnd.GetHwnd()))
					ShowWindow(m_addMatchWnd.GetHwnd(), SW_SHOW);
			}

			char	matchNo[64] = { 0 };
			int index = m_listMatch.GetSelectItem();
			m_listMatch.GetSubItem(index, MATCH_MatchNo, matchNo, sizeof(matchNo));
		
			auto it = m_System->m_mapMatch.find(matchNo);
			if (it != m_System->m_mapMatch.end())
			{
				m_addMatchWnd.InitModifyWnd(it->second);
			}
		}
			break;
		case IDM_DelMatch://删除合同
			DealDelMatch();
			break;
		case IDM_ManageManualClose:
		{
			int index = m_listMatch.GetSelectItem();
			SmsPositionNoType tempMatchNo;
			m_listMatch.GetSubItem(index, MATCH_MatchNo, tempMatchNo, sizeof(SmsPositionNoType));

			auto it_main = m_System->m_mapMatch.find(tempMatchNo);
			if (it_main == m_System->m_mapMatch.end())
				return;
			string closeAgreementNo;
			for (auto it_order : m_System->m_vecOrder)
			{
				if (strcmp(it_main->second.OrderNo, it_order.OrderNo) == 0)
					closeAgreementNo = it_order.AgreementNo;
			}

			vector<SmsMatchDataRsp> tempUnrel;
			tempUnrel.clear();
	
			for (auto it_unrel : m_System->m_mapMatch)
			{
				if (strcmp(it_unrel.second.ContractNo, it_main->second.ContractNo) == 0 && it_unrel.second.Offset == SMS_O_OPEN && strcmp(it_unrel.second.UserNo, it_main->second.UserNo) == 0 && it_unrel.second.Direct != it_main->second.Direct)
					tempUnrel.push_back(it_unrel.second);
			}

			vector<SmsManualCloseDataRsp>	tempRel;
			tempRel.clear();
			string key = m_System->get_curkey();
			for (auto it_rel : m_System->m_mapManualClose[key])
			{
				if (strcmp(it_rel.CloseNo, it_main->second.MatchNo) == 0)
					tempRel.push_back(it_rel);
			}
			if (!IsWindow(m_addManualCloseWnd.GetHwnd()))
			{
				m_addManualCloseWnd.ShowFrm(m_Hwnd);
				m_addManualCloseWnd.Register(m_System);
			}
			else
			{
				if (!IsWindowVisible(m_addManualCloseWnd.GetHwnd()))
					ShowWindow(m_addManualCloseWnd.GetHwnd(), SW_SHOW);
			}
			m_addManualCloseWnd.InitWnd(it_main->second, closeAgreementNo, tempUnrel, tempRel);
		}
			break;
		default:
			break;
		}
	}
	else if (LOWORD(wParam) == ID_CbAgreementNo && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbAgreementNo.GetSelect();
		if (index == 0)
		{
			EnableWindow(m_labBeginDate.GetHwnd(), TRUE);
			EnableWindow(m_cbtBeginDate.GetHwnd(), TRUE);
			EnableWindow(m_labEndDate.GetHwnd(), TRUE);
			EnableWindow(m_cbtEndDate.GetHwnd(), TRUE);
		}
		else
		{
			EnableWindow(m_labBeginDate.GetHwnd(), FALSE);
			EnableWindow(m_cbtBeginDate.GetHwnd(), FALSE);
			EnableWindow(m_labEndDate.GetHwnd(), FALSE);
			EnableWindow(m_cbtEndDate.GetHwnd(), FALSE);
		}
	}
}

void MatchWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Match:
		m_listMatch.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void MatchWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Match:
		DealMatchList(lParam);
		break;
	default:
		break;
	}
}

LRESULT MatchWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void MatchWnd::OnNcDestroy()
{

}

void MatchWnd::OnDestroy()
{
}

void MatchWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listMatch.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void MatchWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listMatch.OnMeasureItem(p->itemHeight);
}

void MatchWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	default:
		break;
	}
}

void MatchWnd::DealMatchList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listMatch.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuMatch, IDM_ModMatch, MF_GRAYED);
			EnableMenuItem(m_menuMatch, IDM_DelMatch, MF_GRAYED);
			EnableMenuItem(m_menuMatch, IDM_ManageManualClose, MF_GRAYED);
		}
		else {
			//平仓才显示
			char	offset[21] = { 0 };
			m_listMatch.GetSubItem(p->iItem, MATCH_Offset, offset, sizeof(offset));
			if (strcmp(offset, "开仓") == 0)
			{
				EnableMenuItem(m_menuMatch, IDM_ManageManualClose, MF_GRAYED);
			}
		}
		ShowMenu(m_menuMatch);
	}
}

void MatchWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void MatchWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_AddMatch; i <= IDM_ManageManualClose; i++)
		EnableMenuItem(m_menuMatch, i, flage);
}

void MatchWnd::OnBtnQry()
{
	if (!m_System)return;

	SmsMatchQryReq matchReq;
	memset(&matchReq, 0, sizeof(SmsMatchQryReq));

	char temp[MAX_PATH] = { 0 };

	if (IsWindowEnabled(m_labBeginDate.GetHwnd()))	//按日期
	{
		//序列号
		matchReq.SerialId = 0;
		//公司
		strncpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
		//用户
		strcpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);
		//开始日期
		wstring tempDate1;
		m_cbtBeginDate.CxGetWindowText(tempDate1);
		strcpy_s(matchReq.BeginDate, LoadRC::unicode_to_ansi(tempDate1).c_str());
		//终止日期
		wstring tempDate2;
		m_cbtEndDate.CxGetWindowText(tempDate2);
		strcpy_s(matchReq.EndDate, LoadRC::unicode_to_ansi(tempDate2).c_str());
	}
	else											//按合同
	{
		//序列号
		matchReq.SerialId = 0;
		//公司
		strncpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
		//用户
		strcpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);
		//合同号
		wchar_t tempAgree[128] = {0};
		m_cbAgreementNo.GetText(tempAgree);
		strcpy_s(matchReq.AgreementNo, LoadRC::unicode_to_ansi(tempAgree).c_str());
	}

	m_listMatch.DeleteAllItems();
	m_System->m_mapMatch.clear();

	g_pSpotSystem->QryMatch(&matchReq);

}

void MatchWnd::DealDelMatch()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "提示", L"确定删除此成交么？", TMB_OKCANCEL))
		return;
	if (!m_System)return;

	SmsMatchDelRsp delReq;
	memset(&delReq, 0, sizeof(SmsMatchDelRsp));

	char	temp[MAX_PATH] = { 0 };
	int row = m_listMatch.GetSelectItem();
	//公司编号
	strncpy_s(delReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	//交易员编号
	memset(&temp, 0, sizeof(temp));
	m_listMatch.GetSubItem(row, MATCH_UserNo, temp, sizeof(temp));
	strncpy_s(delReq.UserNo, temp);
	//结算日期
	memset(&temp, 0, sizeof(temp));
	m_listMatch.GetSubItem(row, MATCH_SettleDate, temp, sizeof(temp));
	strncpy_s(delReq.SettleDate, temp);
	//合约编号
	memset(&temp, 0, sizeof(temp));
	m_listMatch.GetSubItem(row, MATCH_ContractNo, temp, sizeof(temp));
	TContractKey conKey;
	memset(&conKey, 0, sizeof(TContractKey));
	CConvert::TContractNoToTContractKey(temp, conKey);
	char sCode[101] = { 0 };
	CConvert::TContractKeyToSContractNo(conKey, sCode);
	strncpy_s(delReq.ContractNo, sCode);
	//成交编号
	memset(&temp, 0, sizeof(temp));
	m_listMatch.GetSubItem(row, MATCH_MatchNo, temp, sizeof(temp));
	strncpy_s(delReq.MatchNo, temp);
	//系统号
	memset(&temp, 0, sizeof(temp));
	m_listMatch.GetSubItem(row, MATCH_SystemNo, temp, sizeof(temp));
	strncpy_s(delReq.SystemNo, temp);
	//操作员
	strncpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);

	g_pSpotSystem->DeleteMatch(&delReq);

}

void MatchWnd::RefreshList()
{
	if (!m_System)return;

	for (auto it : m_System->m_mapMatch)
	{
		AddMatchToList(it.second);
	}

}
