#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
#define cCAPTION_H			80
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体

CashInOutWnd::CashInOutWnd()
	:m_System(nullptr), m_serialId(-1)
{
}

CashInOutWnd::~CashInOutWnd()
{
}

void CashInOutWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void CashInOutWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void CashInOutWnd::AddCashInOutToList(SmsCashInoutDataRsp & cash)
{
	//序列号
	m_listCashInOut.AddItem((double)cash.SerialId, 0, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//结算日期
	m_listCashInOut.AddItem(cash.SettleDate, 0, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//公司编号
	//m_listCashInOut.AddItem(cash.CompanyNo, 0, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//交易员编号
	m_listCashInOut.AddItem(cash.UserNo, 0, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//出入金类型
	if (cash.CashType == SMS_CASH_IN)
		m_listCashInOut.AddItem("入金", 0, 3, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	else if (cash.CashType == SMS_CASH_OUT)
		m_listCashInOut.AddItem("出金", 0, 3, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	else
		m_listCashInOut.AddItem("调整", 0, 3, DT_CENTER, g_ColorRefData.GetColorBlack());
	//数额
	m_listCashInOut.AddItem(cash.CashValue, 0, 4, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//操作员
	m_listCashInOut.AddItem(cash.OperatorNo, 0, 5, DT_CENTER, g_ColorRefData.GetColorBlack());
	//修改时间
	m_listCashInOut.AddItem(cash.OperateTime, 0, 6, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT CashInOutWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case SSWM_EDIT_DBLCLK:
		OnLDoubleClick(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
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

void CashInOutWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	///////////////////////////////Create//////////////////////////////////
	//开始日期
	m_labBeginDate.Create(m_Hwnd, L"开始日期", DT_RIGHT, ID_LabBeginDate);
	m_beginTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 120, 25, m_Hwnd, (HMENU)ID_CbtBeginDate, m_hInstance, NULL);
	//结束日期
	m_labEndDate.Create(m_Hwnd, L"结束日期", DT_RIGHT, ID_LabEndDate);
	m_endTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 120, 25, m_Hwnd, (HMENU)ID_CbtEndDate, m_hInstance, NULL);
	//查询
	m_btnQry.Create(m_Hwnd, "查  询", ID_BtnQry);

	//结算日期
	m_labSettleDate.Create(m_Hwnd, L"结算日期", DT_RIGHT, ID_LabSettleDate);
	m_settleTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 120, 25, m_Hwnd, (HMENU)ID_CbtSettleDate, m_hInstance, NULL);
	//账号
	m_labUserno.Create(m_Hwnd, L"账    号", DT_CENTER, ID_LabUserNo);
	m_editUserno.Create(m_Hwnd, 0, ES_READONLY, ID_EditUserNo);
	//出入金
	m_editCashType.Create(m_Hwnd, 0, ES_READONLY, ID_EditCashType);
	m_editCashValue.Create(m_Hwnd, 1, 0, ID_EditCashValue);
	//增加、删除
	m_btnAdd.Create(m_Hwnd, "增  加", ID_BtnAdd);
	//清空
	m_btnReset.Create(m_Hwnd, "清  空", ID_BtnReset);

	//表格
	m_listCashInOut.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_CashInOut);
	//////////////////////////////Init////////////////////////////////////
	m_listCashInOut.AddColumn(L"序列号", 60, -1);
	m_listCashInOut.AddColumn(L"结算日期", 140, -1);
	m_listCashInOut.AddColumn(L"账号", 200, -1);
	m_listCashInOut.AddColumn(L"出入金", 200, -1);
	m_listCashInOut.AddColumn(L"数额", 200, -1);
	m_listCashInOut.AddColumn(L"操作员", 200, -1);
	m_listCashInOut.AddColumn(L"修改时间", 200, -1);

	m_menuCashInOut = CreatePopupMenu();

	AppendMenu(m_menuCashInOut, MF_STRING, IDM_DelCashInOut, L"删除出入金");

	m_labBeginDate.SetFont(g_FontData.GetFontWord13());
	m_labEndDate.SetFont(g_FontData.GetFontWord13());
	m_labSettleDate.SetFont(g_FontData.GetFontWord13());
	m_labUserno.SetFont(g_FontData.GetFontWord13());

	m_editUserno.SetFont(g_FontData.GetFontNum15());
	m_editCashType.SetFont(g_FontData.GetFontWord15());
	m_editCashValue.SetFont(g_FontData.GetFontNum15());
	//EnableWindow(m_editUserno.GetHwnd(), FALSE);
	m_editCashType.SetText("入金");

	m_btnQry.SetFont(g_FontData.GetFontWord12());
	m_btnAdd.SetFont(g_FontData.GetFontWord12());
	m_btnReset.SetFont(g_FontData.GetFontWord12());

	m_cbtBeginDate.Init(m_beginTimePick);
	SendMessage(m_beginTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_beginTimePick, L"yyyy-MM-dd");

	m_cbtEndDate.Init(m_endTimePick);
	SendMessage(m_endTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_endTimePick, L"yyyy-MM-dd");

	m_cbtSettleDate.Init(m_settleTimePick);
	SendMessage(m_settleTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_settleTimePick, L"yyyy-MM-dd");

	SYSTEMTIME time;
	GetLocalTime(&time);	
	DateTime_SetSystemtime(m_cbtEndDate.GetHwnd(), GDT_VALID, &time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);
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
	int vtap = 10;			//纵向间隔

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
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labSettleDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtSettleDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labUserno.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editUserno.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_editCashType.MoveWindow(pt.x, pt.y, labWidth -20, Height);
	pt.x += labWidth - 20 + htap;
	m_editCashValue.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_btnAdd.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnReset.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listCashInOut.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void CashInOutWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void CashInOutWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
		case IDM_DelCashInOut://删除出入金
			DealDelCashInOut();
			break;
		default:
			break;
		}
	}
}

void CashInOutWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_CashInOut:
		m_listCashInOut.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void CashInOutWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_CashInOut:
		DealCashInOutList(lParam);
		break;
	default:
		break;
	}
}

LRESULT CashInOutWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void CashInOutWnd::OnNcDestroy()
{
}

void CashInOutWnd::OnDestroy()
{
}

void CashInOutWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listCashInOut.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void CashInOutWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listCashInOut.OnMeasureItem(p->itemHeight);
}

void CashInOutWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	case ID_BtnAdd:
		OnBtnAdd();
		break;
	case ID_BtnReset:
		OnBtnReset();
		break;
	default:
		break;
	}
}

void CashInOutWnd::OnLDoubleClick(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_EditCashType://双击切换
	{
		char	temp[21] = { 0 };
		m_editCashType.GetText(temp, sizeof(temp));
		if (strcmp(temp, "出金") == 0)
			m_editCashType.SetText("入金");
		else
			m_editCashType.SetText("出金");
		break;
	}
	default:
		break;
	}
}

void CashInOutWnd::DealCashInOutList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listCashInOut.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuCashInOut, IDM_ModCashInOut, MF_GRAYED);
			EnableMenuItem(m_menuCashInOut, IDM_DelCashInOut, MF_GRAYED);
		}

		ShowMenu(m_menuCashInOut);
	}
	else if (p->hdr.code == NM_CLICK)
	{
		//账号
		char temp[32] = { 0 };
		int index = m_listCashInOut.GetSelectItem();

		m_listCashInOut.GetSubItem(index, 2, temp, sizeof(temp));
		m_editUserno.SetText(LoadRC::ansi_to_unicode(temp).c_str());
	}
	else if (p->hdr.code == NM_DBLCLK)
	{
		if (p->iItem >= m_listCashInOut.GetItemCount() || p->iItem == -1)
			return;
		if (!m_System)return;

		//双击回填信息
		int index = m_listCashInOut.GetSelectItem();
		char temp[MAX_PATH] = { 0 };
		//序列号
		m_listCashInOut.GetSubItem(index, 0, temp, sizeof(temp));
		m_serialId = atoi(temp);
		//结算日期
		memset(&temp, 0, sizeof(temp));
		m_listCashInOut.GetSubItem(index, 1, temp, sizeof(temp));
		vector<string> vdate;
		vdate = m_System->SplitString(temp, '-');
		if (vdate.size() < 3)
			return;
		SYSTEMTIME time;
		memset(&time, 0, sizeof(SYSTEMTIME));
		time.wYear = atoi(vdate.at(0).c_str());
		time.wMonth = atoi(vdate.at(1).c_str());
		time.wDay = atoi(vdate.at(2).c_str());
		DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);
		//账号
		memset(&temp, 0, sizeof(temp));
		m_listCashInOut.GetSubItem(index, 2, temp, sizeof(temp));
		m_editUserno.SetText(LoadRC::ansi_to_unicode(temp).c_str());
		//出入金
		memset(&temp, 0, sizeof(temp));
		m_listCashInOut.GetSubItem(index, 3, temp, sizeof(temp));
		m_editCashType.SetText(LoadRC::ansi_to_unicode(temp).c_str());
		//结算价
		memset(&temp, 0, sizeof(temp));
		m_listCashInOut.GetSubItem(index, 4, temp, sizeof(temp));
		m_editCashValue.SetText(temp);

		//规格置灰、按钮转换
		EnableWindow(m_editUserno.GetHwnd(), FALSE);
		m_btnAdd.SetButtonText("修  改");
	}
}

void CashInOutWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void CashInOutWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_AddCashInOut; i <= IDM_DelCashInOut; i++)
		EnableMenuItem(m_menuCashInOut, i, flage);
}

void CashInOutWnd::OnBtnQry()
{
	SmsCashInoutQryReq qryReq;
	memset(&qryReq, 0, sizeof(SmsCashInoutQryReq));
	char temp[MAX_PATH] = { 0 };
	//序列号
	qryReq.SerialId = 0;
	//公司
	strcpy_s(qryReq.CompanyNo, m_System->m_curLogin.CompanyNo);

	//规格编号
	//wchar_t userno[MAX_PATH] = { 0 };
	//m_cbUserno.GetText(userno);
	strcpy_s(qryReq.UserNo, m_System->m_curLogin.UserNo);
	//开始日期
	wstring tempDate1;
	m_cbtBeginDate.CxGetWindowText(tempDate1);
	strcpy_s(qryReq.BeginDate, LoadRC::unicode_to_ansi(tempDate1).c_str());
	//终止日期
	wstring tempDate2;
	m_cbtEndDate.CxGetWindowText(tempDate2);
	strcpy_s(qryReq.EndDate, LoadRC::unicode_to_ansi(tempDate2).c_str());
	//清空列表
	m_listCashInOut.DeleteAllItems();

	m_System->m_vecCashInout.clear();
	g_pSpotSystem->QryCashInout(&qryReq);
}

void CashInOutWnd::DealDelCashInOut()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "提示", L"确定删除此出入金？", TMB_OKCANCEL))
		return;
	if (!m_System)return;

	SmsCashInoutDelReq delReq;
	memset(&delReq, 0, sizeof(delReq));

	int row = m_listCashInOut.GetSelectItem();
	//序列号
	char	serial[24] = { 0 };
	m_listCashInOut.GetSubItem(row, 0, serial, sizeof(serial));
	delReq.SerialId = atoi(serial);

	//操作员
	strncpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);

	g_pSpotSystem->DeleteCashInout(&delReq);
}

void CashInOutWnd::OnBtnAdd()
{
	if (!m_System)return;
	if (IsWindowEnabled(m_editUserno.GetHwnd()))	//增加
	{
		SmsCashInoutAddReq req;
		memset(&req, 0, sizeof(SmsCashInoutAddReq));
		//序列号
		req.SerialId = 0;
		//结算日期
		wstring tempDate;
		m_cbtSettleDate.CxGetWindowText(tempDate);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(tempDate).c_str());
		//公司
		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);
		//交易员
		char	temp[64] = { 0 };
		m_editUserno.GetText(temp, sizeof(temp));
		strncpy_s(req.UserNo, temp);
		//出入金
		memset(&temp, 0, sizeof(temp));
		m_editCashType.GetText(temp, sizeof(temp));
		if (strcmp(temp, "出金") == 0)
			req.CashType = SMS_CASH_OUT;
		else
			req.CashType = SMS_CASH_IN;
		//数额
		memset(&temp, 0, sizeof(temp));
		m_editCashValue.GetText(temp, sizeof(temp));
		req.CashValue = atof(temp);
		//操作员
		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);

		g_pSpotSystem->AddCashInout(&req);
	}
	else//修改
	{
		SmsCashInoutModReq req;
		memset(&req, 0, sizeof(SmsCashInoutModReq));

		//ID
		req.SerialId = m_serialId;
		//结算日期
		wstring tempDate;
		m_cbtSettleDate.CxGetWindowText(tempDate);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(tempDate).c_str());
		//公司
		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);
		//交易员
		char	temp[64] = { 0 };
		m_editUserno.GetText(temp, sizeof(temp));
		strncpy_s(req.UserNo, temp);
		//出入金
		memset(&temp, 0, sizeof(temp));
		m_editCashType.GetText(temp, sizeof(temp));
		if (strcmp(temp, "出金") == 0)
			req.CashType = SMS_CASH_OUT;
		else
			req.CashType = SMS_CASH_IN;
		//数额
		memset(&temp, 0, sizeof(temp));
		m_editCashValue.GetText(temp, sizeof(temp));
		req.CashValue = atof(temp);
		//操作员
		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);

		g_pSpotSystem->ModifyCashInout(&req);
	}
}

void CashInOutWnd::OnBtnReset()
{
	m_editCashValue.SetText("");

	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);

	//规格置灰、按钮转换
	EnableWindow(m_editUserno.GetHwnd(), TRUE);
	m_btnAdd.SetButtonText("增  加");
}
