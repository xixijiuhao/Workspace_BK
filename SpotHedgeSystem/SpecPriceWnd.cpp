#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
#define cCAPTION_H			80
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern string				g_specNoPrice;

SpecPriceWnd::SpecPriceWnd()
	:m_System(nullptr)
{
}

SpecPriceWnd::~SpecPriceWnd()
{
}

void SpecPriceWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void SpecPriceWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void SpecPriceWnd::AddSpecPriceToList(SmsSpecPriceDataRsp & specprice, int index)
{
	int count = 0;
	if (index == -1)
		count = m_listSpecPrice.GetItemCount();
	else
		count = index;
	//序列号
	m_listSpecPrice.AddItem((double)specprice.SerialId, count, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//结算日期
	m_listSpecPrice.AddItem(specprice.SettleDate, count, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//规格编号
	m_listSpecPrice.AddItem(specprice.SpecificationNo, count, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//规格价格
	m_listSpecPrice.AddItem(specprice.SpecificationPirce, count, 3, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT SpecPriceWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_SPECPRICE_REFRESH:
		OnRefresh();
		break;
	case WM_DESTROY:
		OnDestroy();
		break;
	default:
		return NOT_PROCESSED;
	}
	return NOT_PROCESSED;
}

void SpecPriceWnd::OnCreate()
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
	//规格
	m_labSpecificationNo.Create(m_Hwnd, L"规格编号", DT_RIGHT, ID_LabSpec);
	m_cbSpecificationNo.Create(m_Hwnd, ID_CbSpec);
	//查询
	m_btnQry.Create(m_Hwnd, "查  询",ID_BtnQry);

	//结算日期
	m_labSettleDate.Create(m_Hwnd, L"结算日期", DT_RIGHT, ID_labSettleDate);
	m_settleTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 120, 25, m_Hwnd, (HMENU)ID_cbtSettleDate, m_hInstance, NULL);
	//规格编号
	m_labSpecNo.Create(m_Hwnd, L"规格编号", DT_RIGHT, ID_labSpec);
	m_cbSpecNo.Create(m_Hwnd, ID_cbSpec);
	//现货价格
	m_labSpecPirce.Create(m_Hwnd, L"现货价格", DT_RIGHT, ID_labSpecPirce);
	m_editSpecPirce.Create(m_Hwnd, 0, 0, ID_editSpecPirce);
	//增加删除
	m_btnAdd.Create(m_Hwnd, "增  加", ID_btnAdd);
	m_btnReset.Create(m_Hwnd, "清  空", ID_btnReset);

	//表格
	m_listSpecPrice.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_SpecPrice);

	//////////////////////////////Init////////////////////////////////////
	m_listSpecPrice.AddColumn(L"序列号", 80, -1);
	m_listSpecPrice.AddColumn(L"结算日期", 200, -1);
	m_listSpecPrice.AddColumn(L"规格编号", 200, -1);
	m_listSpecPrice.AddColumn(L"现货价格", 200, -1);

	m_menuSpecPrice = CreatePopupMenu();
	AppendMenu(m_menuSpecPrice, MF_STRING, IDM_DelSpecPrice, L"删除现货价格");

	m_labBeginDate.SetFont(g_FontData.GetFontWord13());
	m_labEndDate.SetFont(g_FontData.GetFontWord13());
	m_labSpecificationNo.SetFont(g_FontData.GetFontWord13());

	m_labSettleDate.SetFont(g_FontData.GetFontWord13());
	m_labSpecNo.SetFont(g_FontData.GetFontWord13());
	m_labSpecPirce.SetFont(g_FontData.GetFontWord13());

	m_cbSpecificationNo.SetFont(g_FontData.GetFontNum15());
	m_cbSpecNo.SetFont(g_FontData.GetFontNum15());

	m_editSpecPirce.SetFont(g_FontData.GetFontNum15());

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
	pt.x += editWidth + htap;
	m_labSpecificationNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbSpecificationNo.MoveWindow(pt.x, pt.y, editWidth, Height*10);
	pt.x += editWidth + htap * 3;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labSettleDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtSettleDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labSpecNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbSpecNo.MoveWindow(pt.x, pt.y, editWidth, Height*10);
	pt.x += editWidth + htap;
	m_labSpecPirce.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSpecPirce.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_btnAdd.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnReset.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listSpecPrice.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void SpecPriceWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void SpecPriceWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
		case IDM_DelSpecPrice://删除现货价格
			DealDelSpecPrice();
			break;
		default:
			break;
		}
	}
}

void SpecPriceWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_SpecPrice:
		m_listSpecPrice.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void SpecPriceWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_SpecPrice:
		DealSpecPriceList(lParam);
		break;
	default:
		break;
	}
}

LRESULT SpecPriceWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void SpecPriceWnd::OnNcDestroy()
{
}

void SpecPriceWnd::OnDestroy()
{
}

void SpecPriceWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listSpecPrice.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void SpecPriceWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listSpecPrice.OnMeasureItem(p->itemHeight);
}

void SpecPriceWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	case ID_btnAdd:
		OnBtnAdd();
		break;
	case ID_btnReset:
		OnBtnReset();
		break;
	default:
		break;
	}
}

void SpecPriceWnd::DealSpecPriceList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listSpecPrice.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuSpecPrice, IDM_DelSpecPrice, MF_GRAYED);
		}

		ShowMenu(m_menuSpecPrice);
	}
	else if (p->hdr.code == NM_DBLCLK)
	{
		if (p->iItem >= m_listSpecPrice.GetItemCount() || p->iItem == -1)
			return;
		if (!m_System)return;

		//双击回填信息
		int index = m_listSpecPrice.GetSelectItem();

		//结算日期
		char temp[MAX_PATH] = { 0 };
		m_listSpecPrice.GetSubItem(index, 1, temp, sizeof(temp));
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
		//规格编号
		memset(&temp, 0, sizeof(temp));
		m_listSpecPrice.GetSubItem(index, 2, temp, sizeof(temp));
		bool bSetCbSpec = m_cbSpecNo.SetSelect(LoadRC::ansi_to_unicode(temp).c_str());
		if(!bSetCbSpec)
			_TMessageBox_Domodal(m_Hwnd, "提示", L"无对应规格，请重新录入！", TMB_OK);
		//现货价格
		memset(&temp, 0, sizeof(temp));
		m_listSpecPrice.GetSubItem(index, 3, temp, sizeof(temp));
		m_editSpecPirce.SetText(temp);

		//规格置灰、按钮转换
		EnableWindow(m_cbtSettleDate.GetHwnd(), FALSE);
		EnableWindow(m_cbSpecNo.GetHwnd(), FALSE);
		m_btnAdd.SetButtonText("修  改");
	}
}

void SpecPriceWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void SpecPriceWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_DelSpecPrice; i <= IDM_DelSpecPrice; i++)
		EnableMenuItem(m_menuSpecPrice, i, flage);
}

void SpecPriceWnd::OnBtnQry()
{
	if (!m_System)	return;

	SmsSpecPriceQryReq qryReq;
	memset(&qryReq, 0, sizeof(SmsSpecPriceQryReq));
	char temp[MAX_PATH] = { 0 };
	//序列号
	qryReq.SerialId = 0;
	//公司
	strcpy_s(qryReq.CompanyNo, m_System->m_curLogin.CompanyNo);

	//规格编号
	wchar_t spec[MAX_PATH] = { 0 };
	m_cbSpecificationNo.GetText(spec);
	strcpy_s(qryReq.SpecificationNo, LoadRC::unicode_to_ansi(spec).c_str());
	//开始日期
	wstring tempDate1;
	m_cbtBeginDate.CxGetWindowText(tempDate1);
	strcpy_s(qryReq.BeginDate, LoadRC::unicode_to_ansi(tempDate1).c_str());
	//终止日期
	wstring tempDate2;
	m_cbtEndDate.CxGetWindowText(tempDate2);
	strcpy_s(qryReq.EndDate, LoadRC::unicode_to_ansi(tempDate2).c_str());

	m_listSpecPrice.DeleteAllItems();

	//m_System->m_vecSpecPrice.clear();
	g_pSpotSystem->QrySpecPrice(&qryReq);
}

void SpecPriceWnd::DealDelSpecPrice()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "提示", L"确定删除此现货价格？", TMB_OKCANCEL))
		return;
	SmsSpecPriceDelReq delReq;
	memset(&delReq, 0, sizeof(delReq));

	strncpy_s(delReq.CompanyNo, m_System->m_curLogin.CompanyNo);

	int row = m_listSpecPrice.GetSelectItem();
	m_listSpecPrice.GetSubItem(row, 1, delReq.SettleDate, sizeof(SmsDateType));
	m_listSpecPrice.GetSubItem(row, 2, delReq.SpecificationNo, sizeof(SmsSpecificationNoType));

	strncpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);

	g_pSpotSystem->DeleteSpecPrice(&delReq);
}

void SpecPriceWnd::OnBtnAdd()
{
	if (!m_System)return;
	if (IsWindowEnabled(m_cbtSettleDate.GetHwnd()))	//增加
	{
		SmsSpecPriceAddReq req;
		memset(&req, 0, sizeof(SmsSpecPriceAddReq));

		req.SerialId = 0;
		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);

		wstring tempDate;
		m_cbtSettleDate.CxGetWindowText(tempDate);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(tempDate).c_str());

		wchar_t		spec[128] = { 0 };
		m_cbSpecNo.GetText(spec);
		strncpy_s(req.SpecificationNo, LoadRC::unicode_to_ansi(spec).c_str());

		char temp[64] = { 0 };
		m_editSpecPirce.GetText(temp, sizeof(temp));
		req.SpecificationPirce = atof(temp);

		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);

		g_pSpotSystem->AddSpecPrice(&req);
	}
	else//修改
	{
		SmsSpecPriceModReq req;
		memset(&req, 0, sizeof(SmsSpecPriceModReq));

		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);

		wstring tempDate;
		m_cbtSettleDate.CxGetWindowText(tempDate);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(tempDate).c_str());

		wchar_t		spec[128] = { 0 };
		m_cbSpecNo.GetText(spec);
		strncpy_s(req.SpecificationNo, LoadRC::unicode_to_ansi(spec).c_str());

		char temp[64] = { 0 };
		m_editSpecPirce.GetText(temp, sizeof(temp));
		req.SpecificationPirce = atof(temp);

		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);

		g_pSpotSystem->ModifySpecPrice(&req);
	}
}

void SpecPriceWnd::OnBtnReset()
{
	m_editSpecPirce.SetText("");

	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);

	EnableWindow(m_cbtSettleDate.GetHwnd(), TRUE);
	EnableWindow(m_cbSpecNo.GetHwnd(), TRUE);
	m_btnAdd.SetButtonText("增  加");
}

void SpecPriceWnd::OnRefresh()
{
	m_listSpecPrice.DeleteAllItems();

	for (auto it : m_System->m_mapSpecPrice[g_specNoPrice])
	{
		AddSpecPriceToList(it);
	}
}
