#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体

#define cCAPTION_H			45

SpecWnd::SpecWnd()
{

}

SpecWnd::~SpecWnd()
{

}

void SpecWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void SpecWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void SpecWnd::AddSpecToList(SmsSpecDataRsp & agree)
{
	int row = 0/*m_listSpec.GetItemCount()*/;
	//序列号
	m_listSpec.AddItem((double)agree.SerialId, row, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//公司编号
	//m_listSpec.AddItem(agree.CompanyNo, 0, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//规格编号
	m_listSpec.AddItem(agree.SpecificationNo, row, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//规格名称
	wstring specName = LoadRC::ansi_to_unicode(agree.SpecName, CP_UTF8);
	m_listSpec.AddItem(LoadRC::unicode_to_ansi(specName).c_str(), row, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//品种编号
	m_listSpec.AddItem(agree.CommodityNo, row, 3, DT_CENTER, g_ColorRefData.GetColorBlack());
	//操作员
	m_listSpec.AddItem(agree.OperatorNo, row, 4, DT_CENTER, g_ColorRefData.GetColorBlack());
	//修改时间
	m_listSpec.AddItem(agree.OperateTime, row, 5, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT SpecWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
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

void SpecWnd::OnCreate()
{
	//序列号
	//m_labSerialId.Create(m_Hwnd, L"序列号码", DT_RIGHT, ID_labSerialID);
	//m_editSerialId.Create(m_Hwnd, 0, 0, ID_editSerialID);
	//规格编号
	m_labSpecNo.Create(m_Hwnd, L"规格编号", DT_RIGHT, ID_labSpecNo);
	m_editSpecNo.Create(m_Hwnd, 0, 0, ID_editSpecNo);
	//规格名称
	m_labSpecName.Create(m_Hwnd, L"规格名称", DT_RIGHT, ID_labSpecName);
	m_editSpecName.Create(m_Hwnd, 0, 0, ID_editSpecName);
	//品种编号
	m_labCommNo.Create(m_Hwnd, L"品种编号", DT_RIGHT, ID_labCommNo);
	m_editCommNo.Create(m_Hwnd, 0, 0, ID_editCommNo);
	//增加
	m_bthAdd.Create(m_Hwnd, "增  加", ID_btnAdd);
	//修改
	//m_bthModify.Create(m_Hwnd, "修  改", ID_btnModify);
	//重置
	m_bthReset.Create(m_Hwnd, "清  空", ID_btnReset);
	//表格
	m_listSpec.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Spec);

	//============================配置列表=============================
	m_listSpec.AddColumn(L"序列号", 60, -1);
	//m_listSpec.AddColumn(L"公司编号", 120, -1);
	m_listSpec.AddColumn(L"规格编号", 140, -1);
	m_listSpec.AddColumn(L"规格名称", 200, -1);
	m_listSpec.AddColumn(L"品种编号", 200, -1);
	m_listSpec.AddColumn(L"操作员", 200, -1);
	m_listSpec.AddColumn(L"修改时间", 150, -1);
	m_menuSpec = CreatePopupMenu();
	//AppendMenu(m_menuSpec, MF_STRING, IDM_AddSpec, L"录入规格");
	//AppendMenu(m_menuSpec, MF_SEPARATOR, IDM_Line, L"");
	//AppendMenu(m_menuSpec, MF_STRING, IDM_ModSpec, L"修改规格");
	AppendMenu(m_menuSpec, MF_STRING, IDM_DelSpec, L"删除规格");

	//EnableWindow(m_labSerialId.GetHwnd(), FALSE);
	//EnableWindow(m_editSerialId.GetHwnd(), FALSE);

	//m_labSerialId.SetFont(g_FontData.GetFontWord13());
	m_labSpecNo.SetFont(g_FontData.GetFontWord13());
	m_labSpecName.SetFont(g_FontData.GetFontWord13());
	m_labCommNo.SetFont(g_FontData.GetFontWord13());

	//m_editSerialId.SetFont(g_FontData.GetFontNum15());
	m_editSpecNo.SetFont(g_FontData.GetFontNum15());
	m_editSpecName.SetFont(g_FontData.GetFontNum15());
	m_editCommNo.SetFont(g_FontData.GetFontNum15());

	m_bthAdd.SetFont(g_FontData.GetFontWord12());
	//m_bthModify.SetFont(g_FontData.GetFontWord12());
	m_bthReset.SetFont(g_FontData.GetFontWord12());

	////////////////////POS//////////////////////	
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
	//m_labSerialId.MoveWindow(pt.x, pt.y, labWidth, Height);
	//pt.x += labWidth + htap;
	//m_editSerialId.MoveWindow(pt.x, pt.y, editWidth, Height);
	//pt.x += editWidth + htap;
	m_labSpecNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSpecNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labSpecName.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSpecName.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labCommNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCommNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_bthAdd.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	//m_bthModify.MoveWindow(pt.x, pt.y, btnWidth, Height);
	//pt.x += btnWidth + htap * 3;
	m_bthReset.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listSpec.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);

}

void SpecWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void SpecWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
		case IDM_DelSpec://删除合同
			DealDelSpec();
			break;
		default:
			break;
		}
	}


}

void SpecWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Spec:
		m_listSpec.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void SpecWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Spec:
		DealSpecList(lParam);
		break;
	default:
		break;
	}
}

LRESULT SpecWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void SpecWnd::OnNcDestroy()
{
}

void SpecWnd::OnDestroy()
{
	//if (IsWindow(m_addSpecWnd.GetHwnd()))
	//	DestroyWindow(m_addSpecWnd.GetHwnd());
}

void SpecWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listSpec.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}


void SpecWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listSpec.OnMeasureItem(p->itemHeight);
}

void SpecWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_btnAdd:
		OnBtnAdd();
		break;
	//case ID_btnModify:
	//	OnBtnMod();
	//	break;
	case ID_btnReset:
		OnBtnReset();
		break;
	default:
		break;
	}
}

void SpecWnd::DealSpecList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listSpec.GetItemCount() || p->iItem == -1)
		{
			//EnableMenuItem(m_menuSpec, IDM_ModSpec, MF_GRAYED);
			EnableMenuItem(m_menuSpec, IDM_DelSpec, MF_GRAYED);
		}
		ShowMenu(m_menuSpec);
	}
	else if (p->hdr.code == NM_DBLCLK)
	{
		if (p->iItem >= m_listSpec.GetItemCount() || p->iItem == -1)
			return;
		//双击回填信息
		int index = m_listSpec.GetSelectItem();

		char temp[MAX_PATH] = { 0 };
		//m_listSpec.GetSubItem(index, 0, temp, sizeof(temp));
		//m_editSerialId.SetText(temp);
		m_listSpec.GetSubItem(index, 1, temp, sizeof(temp));
		m_editSpecNo.SetText(temp);
		m_listSpec.GetSubItem(index, 2, temp, sizeof(temp));
		m_editSpecName.SetText(temp);
		m_listSpec.GetSubItem(index, 3, temp, sizeof(temp));
		m_editCommNo.SetText(temp);

		//规格置灰、按钮转换
		EnableWindow(m_editSpecNo.GetHwnd(), FALSE);
		m_bthAdd.SetButtonText("修  改");
	}
}

void SpecWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void SpecWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_DelSpec; i <= IDM_DelSpec; i++)
		EnableMenuItem(m_menuSpec, i, flage);

}

void SpecWnd::DealDelSpec()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "提示", L"确定删除此规格？", TMB_OKCANCEL))
		return;
	if (!m_System)return;

	SmsSpecDelReq delReq;
	memset(&delReq, 0, sizeof(delReq));

	int row = m_listSpec.GetSelectItem();
	strncpy_s(delReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strncpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);
	m_listSpec.GetSubItem(row, 1, delReq.SpecificationNo, sizeof(SmsSpecificationNoType));
	
	g_pSpotSystem->DeleteSpec(&delReq);
}

void SpecWnd::OnBtnAdd()
{
	if (!m_System)return;

	if (IsWindowEnabled(m_editSpecNo.GetHwnd()))	//增加
	{
		SmsSpecAddReq req;
		memset(&req, 0, sizeof(SmsSpecAddReq));

		char temp[64] = { 0 };
		memset(&temp, 0, sizeof(temp));
		m_editCommNo.GetText(temp, sizeof(temp));
		strncpy_s(req.CommodityNo, temp);

		if (!CConvert::IsValidQuoteCommodity(req.CommodityNo))
		{
			_TMessageBox_Domodal(m_Hwnd, "提示", L"无效品种", TMB_OK);
			return;
		}

		req.SerialId = 0;
		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);


		m_editSpecNo.GetText(temp, sizeof(temp));
		strncpy_s(req.SpecificationNo, temp);

		memset(&temp, 0, sizeof(temp));
		m_editSpecName.GetText(temp, sizeof(temp));
		wstring w_specName = LoadRC::ansi_to_unicode(temp);
		string s_specName = LoadRC::unicode_to_ansi(w_specName, CP_UTF8);
		strncpy_s(req.SpecName, s_specName.c_str());

	

		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);
		g_pSpotSystem->AddSpec(&req);
	}
	else {	//修改
		SmsSpecModReq req;
		memset(&req, 0, sizeof(SmsSpecModReq));

		char temp[64] = { 0 };
		memset(&temp, 0, sizeof(temp));
		m_editCommNo.GetText(temp, sizeof(temp));
		strncpy_s(req.CommodityNo, temp);

		if (!CConvert::IsValidQuoteCommodity(req.CommodityNo))
		{
			_TMessageBox_Domodal(m_Hwnd, "提示", L"无效品种", TMB_OK);
			return;
		}

		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);

		m_editSpecNo.GetText(temp, sizeof(temp));
		strncpy_s(req.SpecificationNo, temp);

		//序列号
		SpecMapType::iterator it = m_System->m_mapSpec.find(req.SpecificationNo);
		if (it != m_System->m_mapSpec.end())
		{
			req.SerialId = it->second.SerialId;
		}

		memset(&temp, 0, sizeof(temp));
		m_editSpecName.GetText(temp, sizeof(temp));
		wstring w_specName = LoadRC::ansi_to_unicode(temp);
		string s_specName = LoadRC::unicode_to_ansi(w_specName, CP_UTF8);
		strncpy_s(req.SpecName, s_specName.c_str());



		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);

		g_pSpotSystem->ModifySpec(&req);
	}


}

void SpecWnd::OnBtnReset()
{
	//m_editSerialId.SetText("");
	m_editSpecNo.SetText("");
	m_editSpecName.SetText("");
	m_editCommNo.SetText("");
	//规格、按钮转换
	EnableWindow(m_editSpecNo.GetHwnd(), TRUE);
	m_bthAdd.SetButtonText("增  加");
}


