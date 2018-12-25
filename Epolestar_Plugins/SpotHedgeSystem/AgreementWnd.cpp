#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
#define V_TITLE				45							//抬头高度	

AgreementWnd::AgreementWnd()
	:m_System(nullptr)
{
}

AgreementWnd::~AgreementWnd()
{

}

void AgreementWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void AgreementWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

LRESULT AgreementWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_AGREEEMENT_REFRESH:
		RefreshList();
		return PROCESSED;	
	//case WM_LIST_LBUTTONDBCLK:
	//	OnListDoubleClick(wParam, lParam);
	//	return PROCESSED;
	//case WM_MOUSEMOVE:
	//	OnMouseMove(wParam, lParam);
	//	return PROCESSED;
	//case WM_MOUSELEAVE:
	//	OnMouseLeave();
	//	return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	//case WM_SIZING:
	//	OnSizing(wParam, lParam);
	//	return PROCESSED;
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
	//case WM_LBUTTONDOWN:
	//	OnLButtonDown(wParam, lParam);
	//	return PROCESSED;
	//case WM_LBUTTONUP:
	//	OnLButtonUp(wParam, lParam);
	//	return PROCESSED;
	//case WM_LCONTENT:				//联动
	//	OnLContent(wParam, lParam);
	//	return PROCESSED;
	//case WM_REFRESHLIST:			//刷新列表
	//{
	//	int iSoID = wParam;
	//	auto it = m_mapListOrder.find(iSoID);
	//	if (it != m_mapListOrder.end())
	//		RefreshOrderList(it->second);
	//}
	//return PROCESSED;
	//case WM_NCHITTEST:						//判断是否到达边框附近
	//	return OnNcHitTest(wParam, lParam);
	//case WM_SETCURSOR:						//改变光标形状
	//	OnSetCursor(wParam, lParam);
	//	return PROCESSED;
	//case WM_NCLBUTTONDOWN:					//拖动边框
	//	OnNcLButtonDown(wParam, lParam);
	//	return NOT_PROCESSED;
	//case WM_ONRESETUSERNO://==12
	//	ReSetUserNos();
	//	return PROCESSED;
	//case WM_TIMER:
	//	OnTimer();
	//	return PROCESSED;
	//case WM_SYSCOMMAND:						//拖动边框转系统命令
	//	if (wParam == SC_CLOSE)
	//	{
	//		ShowWindow(g_SeparateOrderWnd->GetHwnd(), SW_HIDE);
	//		return PROCESSED;
	//	}
	//	return NOT_PROCESSED;
	//case WM_MOUSEWHEEL:				//=========滚轮。合约EDIT
	//	OnMouseWheel(wParam, lParam);
	//	return PROCESSED;
		//popwincodelist=======================================================
	//case SSWM_EDIT_PRESS_RIGHT:
	//case SSWM_EDIT_PRESS_ENTER:
	//	if ((HWND)lParam == m_editContract.GetHwnd())
	//	{
	//		m_popContractList.OnReturn();
	//	}
	//	return PROCESSED;
	//case SSWM_EDIT_PRESS_UP:
	//	if (!IsWindowVisible(m_popContractList.GetHwnd()))
	//		return PROCESSED;
	//	m_popContractList.DelNum();
	//	return PROCESSED;
	//case SSWM_EDIT_PRESS_DOWN:
	//	if (!IsWindowVisible(m_popContractList.GetHwnd()))
	//		return PROCESSED;
	//	m_popContractList.AddNum();
	//	return PROCESSED;
	//case SSWM_TAB_SWITCH_FOCUS:
	//	OnTapSwitchFocus(wParam, lParam);
	//	return PROCESSED;
		//=====================================================================
	//case WM_KEYDOWN:			//合约自动索引按键			
	//	OnKeyDown(wParam, wParam);
	//	break;
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

void AgreementWnd::OnCreate()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	//开始日期
	m_labBeginDate.Create(m_Hwnd, L"开始日期", DT_RIGHT, ID_LabBeginDate);
	m_beginTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 120, 25, m_Hwnd, (HMENU)ID_CbtBeginDate, m_hInstance, NULL);
	//结束日期
	m_labEndDate.Create(m_Hwnd, L"结束日期", DT_RIGHT, ID_LabEndDate);
	m_endTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 120, 25, m_Hwnd, (HMENU)ID_CbtEndDate, m_hInstance, NULL);

	m_labBeginDate.SetFont(g_FontData.GetFontWord13());
	m_labEndDate.SetFont(g_FontData.GetFontWord13());
	m_cbtBeginDate.Init(m_beginTimePick);
	m_cbtEndDate.Init(m_endTimePick);

	SYSTEMTIME time;
	GetLocalTime(&time);
	SendMessage(m_endTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_endTimePick, L"yyyy-MM-dd");
	DateTime_SetSystemtime(m_cbtEndDate.GetHwnd(), GDT_VALID, &time);
	time.wDay = 1;
	SendMessage(m_beginTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_beginTimePick, L"yyyy-MM-dd");
	DateTime_SetSystemtime(m_cbtBeginDate.GetHwnd(), GDT_VALID, &time);

	//合同
	m_labAgree.Create(m_Hwnd, L"合同编号", DT_RIGHT, ID_LabAgree);
	m_cbAgree.Create(m_Hwnd, ID_CbAgree);
	m_cbAgree.SetFont(g_FontData.GetFontNum15());

	//用户账号
	m_labUserno.Create(m_Hwnd, L"用户账号", DT_CENTER, ID_LabUserno);
	m_cbUserno.Create(m_Hwnd, ID_CbUserno);
	m_labUserno.SetFont(g_FontData.GetFontWord13());
	m_cbUserno.SetFont(g_FontData.GetFontNum15());

	//查询
	m_btnQry.Create(m_Hwnd, "查  询", ID_BtnQry);
	m_btnQry.SetFont(g_FontData.GetFontWord12());
	m_btnClear.Create(m_Hwnd, "刷  新", ID_BtnClear);
	m_btnClear.SetFont(g_FontData.GetFontWord12());

	///////////////////////POS///////////////////
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
	pt.x += editWidth + htap;
	m_labAgree.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbAgree.MoveWindow(pt.x, pt.y, editWidth, Height * 10);
	pt.x += editWidth + htap;
	m_labUserno.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbUserno.MoveWindow(pt.x, pt.y, editWidth, Height * 8);

	pt.x += editWidth + htap * 3;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnClear.MoveWindow(pt.x, pt.y, btnWidth, Height);
	//pt.x += btnWidth + htap * 3;
	//m_btnAll.MoveWindow(pt.x, pt.y, btnWidth, Height);
	//pt.x += btnWidth + htap * 3;
	//m_btnPart.MoveWindow(pt.x, pt.y, btnWidth, Height);

	//表格
	m_listAgree.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Agree);
	////m_listCloseAgree.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_CloseAgree);
	m_listAgree.MoveWindow(rect.left, rect.top + V_TITLE, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
	////m_listCloseAgree.MoveWindow((rect.left + rect.right) / 2, rect.top + V_TITLE, (rect.right - rect.left) / 2, rect.bottom - rect.top - V_TITLE);
	//m_listAgree.SetColor(RGB(25, 121, 202), RGB(243, 243, 243), RGB(0, 0, 0));

	//============================配置列表=============================
	for (int i = AGREE_UserNo; i < AGREE_OperateTime; i++)
	{
		switch (i)
		{
		case AGREE_SerialId:
			m_listAgree.AddColumn(L"序列号", 80, AGREE_SerialId);
			break;
		case AGREE_CompanyNo:
			m_listAgree.AddColumn(L"公司编号", 80, AGREE_CompanyNo);
			break;
		case AGREE_AgreementNo:
			m_listAgree.AddColumn(L"合同编号", 120, AGREE_AgreementNo);
			break;
		case AGREE_UserNo:
			m_listAgree.AddColumn(L"用户编号", 80, AGREE_UserNo);
			break;
		case AGREE_AgreementType:
			m_listAgree.AddColumn(L"合同类型", 80, AGREE_AgreementType);
			break;
		case AGREE_SpotQty:
			m_listAgree.AddColumn(L"现货数量", 80, AGREE_SpotQty);
			break;
		case AGREE_SpotPrice:
			m_listAgree.AddColumn(L"合同价格", 80, AGREE_SpotPrice);
			break;
		case AGREE_SignDate:
			m_listAgree.AddColumn(L"合同日期", 80, AGREE_SignDate);
			break;
		case AGREE_InsertTime:
			m_listAgree.AddColumn(L"录入时间", 130, AGREE_InsertTime);
			break;
		case AGREE_SpecificationNo:
			m_listAgree.AddColumn(L"规格名称", 80, AGREE_SpecificationNo);
			break;
		case AGREE_AgreementState:
			m_listAgree.AddColumn(L"合同状态", 80, AGREE_AgreementState);
			break;
		case AGREE_OperatorNo:
			m_listAgree.AddColumn(L"操作员", 80, AGREE_OperatorNo);
			break;
		case AGREE_OperateTime:
			m_listAgree.AddColumn(L"修改时间", 130, AGREE_OperateTime);
			break;
		case AGREE_SpotRemainQty:
			m_listAgree.AddColumn(L"可配量", 80, AGREE_SpotRemainQty);
			break;
		case AGREE_PositionQty:
			m_listAgree.AddColumn(L"持仓量", 80, AGREE_PositionQty);
			break;
		case AGREE_CanOpenQty:
			m_listAgree.AddColumn(L"可开量", 80, AGREE_CanOpenQty);
			break;
		default:
			break;
		}
	}
	m_menuAgree = CreatePopupMenu();

	//AppendMenu(m_menuAgree, MF_STRING, IDM_AllAgreement, L"显示全部合同");
	//AppendMenu(m_menuAgree, MF_SEPARATOR, IDM_Line1, L"");
	AppendMenu(m_menuAgree, MF_STRING, IDM_AddAgreement, L"录入合同");
	AppendMenu(m_menuAgree, MF_STRING, IDM_ModAgreement, L"修改合同");
	AppendMenu(m_menuAgree, MF_STRING, IDM_DelAgreement, L"删除合同");
	AppendMenu(m_menuAgree, MF_SEPARATOR, IDM_Line2, L"");
	AppendMenu(m_menuAgree, MF_STRING, IDM_ManageAgreementRel, L"管理合同关系");
	//AppendMenu(m_menuAgree, MF_STRING, IDM_ModAgreementRel, L"修改合同关系");
	//AppendMenu(m_menuAgree, MF_STRING, IDM_DelAgreementRel, L"删除合同关系");
}

void AgreementWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void AgreementWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
		case IDM_AddAgreement:	//录入合同
			if (!IsWindow(m_addAgreementWnd.GetHwnd()))
			{
				m_addAgreementWnd.ShowFrm(m_Hwnd);
				m_addAgreementWnd.Register(m_System);
				m_addAgreementWnd.InitWnd();
			}
			else
			{
				if (!IsWindowVisible(m_addAgreementWnd.GetHwnd()))
					ShowWindow(m_addAgreementWnd.GetHwnd(), SW_SHOW);
				m_addAgreementWnd.InitWnd();
			}
			break;
		case IDM_ModAgreement://修改合同
		{
			if (!IsWindow(m_addAgreementWnd.GetHwnd()))
			{
				m_addAgreementWnd.ShowFrm(m_Hwnd);
				m_addAgreementWnd.Register(m_System);
				//m_addAgreementWnd.InitModifyWnd();
			}
			else
			{
				if (!IsWindowVisible(m_addAgreementWnd.GetHwnd()))
					ShowWindow(m_addAgreementWnd.GetHwnd(), SW_SHOW);
				//m_addAgreementWnd.InitModifyWnd();
			}

			int index = m_listAgree.GetSelectItem();
			char temp[MAX_PATH] = { 0 };
			m_listAgree.GetSubItem(index, AGREE_AgreementNo, temp, sizeof(temp));
			string key = m_System->get_curkey();
			m_addAgreementWnd.InitModifyWnd(m_System->m_map_mapAgreement[key][temp]);
			//for (auto it : m_System->m_mapAgreement[key])
			//{
			//	if (strcmp(it.AgreementNo,temp) == 0)
			//	{
			//		m_addAgreementWnd.InitModifyWnd(it);
			//		break;
			//	}
			//}
		}
			break;
		case IDM_DelAgreement://删除合同
			DealDelAgreement();
			break;
		case IDM_ManageAgreementRel:
		{
			int index = m_listAgree.GetSelectItem();
			SmsAgreementNoType tempAgreeNo;
			m_listAgree.GetSubItem(index, AGREE_AgreementNo, tempAgreeNo, sizeof(SmsAgreementNoType));

			if (!m_System)return;
			string key = m_System->get_curkey();

			SmsAgreementDataRsp	tempAgreement;
			memcpy(&tempAgreement, &(m_System->m_map_mapAgreement[key][tempAgreeNo]), sizeof(SmsAgreementDataRsp));

			//for (auto it : m_System->m_mapAgreement[key])
			//{
			//	if (strcmp(it.AgreementNo, tempAgreeNo) == 0)
			//	{
			//		memcpy(&tempAgreement, &it, sizeof(SmsAgreementDataRsp));
			//	}		
			//}

			//char offset[21] = { 0 };
			//m_listAgree.GetSubItem(index, AGREE_PositionOffset, offset, sizeof(offset));
			char agreetype[21] = { 0 };
			m_listAgree.GetSubItem(index, AGREE_AgreementType, agreetype, sizeof(agreetype));
			char specNo[31] = { 0 };
			m_listAgree.GetSubItem(index, AGREE_SpecificationNo, specNo, sizeof(specNo));
			char userno[21] = { 0 };
			m_listAgree.GetSubItem(index, AGREE_UserNo, userno, sizeof(userno));

			bool isBuy = tempAgreement.AgreementType == SMS_AGREEMENT_PURCHASE ? true : false;
			//char c_offset = tempAgreement.PositionOffset;
			char c_agreetype =  tempAgreement.AgreementType;

			vector<SmsAgreementDataRsp> tempUnrel;
			tempUnrel.clear();
			for (auto it_unrel : m_System->m_map_mapAgreement[key])
			{
				if (/*it_unrel.PositionOffset != c_offset &&*/ it_unrel.second.AgreementType != c_agreetype && strcmp(it_unrel.second.SpecificationNo, specNo) == 0 && strcmp(it_unrel.second.UserNo,userno)==0)
					tempUnrel.push_back(it_unrel.second);
			}

			vector<string> tempRel;
			tempRel.clear();
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (isBuy)
				{
					if (strcmp(it_rel.BuyAgreementNo, tempAgreeNo)==0 )
						tempRel.push_back(it_rel.SellAgreementNo);
				}
				else
				{
					if (strcmp(it_rel.SellAgreementNo, tempAgreeNo) == 0)
						tempRel.push_back(it_rel.BuyAgreementNo);
				}
			}

			if (!IsWindow(m_addAgreementRelWnd.GetHwnd()))
			{
				m_addAgreementRelWnd.ShowFrm(m_Hwnd);
				m_addAgreementRelWnd.Register(m_System);
			}
			else
			{
				if (!IsWindowVisible(m_addAgreementRelWnd.GetHwnd()))
					ShowWindow(m_addAgreementRelWnd.GetHwnd(), SW_SHOW);
			}
			m_addAgreementRelWnd.InitWnd(tempAgreement, isBuy, tempUnrel, tempRel);
		}
			break;
		default:
			break;
		}
	}
	else if (LOWORD(wParam) == ID_CbAgree && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbAgree.GetSelect();
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

void AgreementWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Agree:
		m_listAgree.OnDrawItem(lParam);
		break;
	//case ID_ListView_CloseAgree:
	//	//m_listCloseAgree.OnDrawItem(lParam);
	//	break;
	default:
		break;
	}
}

void AgreementWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Agree:
		DealAgreeList(lParam);
		break;
	//case ID_ListView_CloseAgree:
	//	DealCloseAgreeList(lParam);
	//	break;
	default:
		break;
	}
}

void AgreementWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	case ID_BtnClear:
		OnBtnClear();
		break;
	default:
		break;
	}
}

LRESULT AgreementWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void AgreementWnd::OnNcDestroy()
{
}

void AgreementWnd::OnDestroy()
{
	if (IsWindow(m_addAgreementWnd.GetHwnd()))
		DestroyWindow(m_addAgreementWnd.GetHwnd());
}

void AgreementWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listAgree.MoveWindow(rect.left, rect.top + V_TITLE, rect.right - rect.left, rect.bottom - rect.top- V_TITLE);
	//m_listCloseAgree.MoveWindow((rect.left + rect.right) / 2, rect.top + V_TITLE, (rect.right - rect.left)/2, rect.bottom - rect.top-V_TITLE);
	//m_labOpenAgree.MoveWindow(15, 0, 150, V_TITLE);
	//m_labCloseAgree.MoveWindow((rect.left + rect.right) / 2+15, 0, 150, V_TITLE);
}

void AgreementWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listAgree.OnMeasureItem(p->itemHeight);
	//m_listCloseAgree.OnMeasureItem(p->itemHeight);
}

void AgreementWnd::DealAgreeList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listAgree.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuAgree, IDM_ModAgreement, MF_GRAYED);
			EnableMenuItem(m_menuAgree, IDM_DelAgreement, MF_GRAYED);
			EnableMenuItem(m_menuAgree, IDM_ManageAgreementRel, MF_GRAYED);
		}
		ShowMenu(m_menuAgree);
	}
	//else if (p->hdr.code == NM_CLICK)
	//{
	//	if (p->iItem >= m_listAgree.GetItemCount() || p->iItem == -1)
	//		return;

	//	SmsAgreementNoType tempOpen;
	//	m_listAgree.GetSubItem(p->iItem, AGREE_AgreementNo, tempOpen, sizeof(SmsAgreementNoType));
	//	if (!m_System)return;
	//	string key = m_System->get_curkey();

	//	//对应平仓合同
	//	//m_listCloseAgree.DeleteAllItems();
	//	vector<string> vAgree;
	//	vAgree.clear();

	//	for (auto it : m_System->m_mapAgreementRel[key])
	//	{
	//		if (strcmp(it.BuyAgreementNo, tempOpen) == 0)
	//		{
	//			vAgree.push_back(it.SellAgreementNo);
	//		}
	//	}

	//	for (auto it_c : vAgree)
	//	{
	//		for (auto& it_r : m_System->m_mapAgreement[key])
	//		{
	//			if (strcmp(it_r.AgreementNo, it_c.c_str()) == 0 && it_r.PositionOffset == SMS_O_COVER)
	//			{
	//				AddAgreementToList2(it_r);
	//			}
	//		}
	//	}

	//}
}

//void AgreementWnd::DealCloseAgreeList(LPARAM lParam)
//{
//	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
//	if (p->hdr.code == NM_RCLICK)
//	{
//		EnableMenu(p->hdr.idFrom, true);
//		if (p->iItem >= m_listCloseAgree.GetItemCount() || p->iItem == -1)
//		{
//			EnableMenuItem(m_menuAgree, IDM_ModAgreement, MF_GRAYED);
//			EnableMenuItem(m_menuAgree, IDM_DelAgreement, MF_GRAYED);
//
//			EnableMenuItem(m_menuAgree, IDM_ModAgreementRel, MF_GRAYED);
//			EnableMenuItem(m_menuAgree, IDM_DelAgreementRel, MF_GRAYED);
//		}
//
//		ShowMenu(m_menuAgree);
//	}
//	else if (p->hdr.code == NM_CLICK)
//	{
//		if (p->iItem >= //m_listCloseAgree.GetItemCount() || p->iItem == -1)
//			return;
//
//		SmsAgreementNoType tempClose;
//		//m_listCloseAgree.GetSubItem(p->iItem, AGREE_AgreementNo, tempClose, sizeof(SmsAgreementNoType));
//		if (!m_System)return;
//		string key = m_System->get_curkey();
//
//		//对应平仓合同
//		m_listAgree.DeleteAllItems();
//		vector<string> vAgree;
//		vAgree.clear();
//
//		for (auto it : m_System->m_mapAgreementRel[key])
//		{
//			if (strcmp(it.SellAgreementNo, tempClose) == 0)
//			{
//				vAgree.push_back(it.BuyAgreementNo);
//			}
//		}
//
//		for (auto it_c : vAgree)
//		{
//			for (auto& it_r : m_System->m_mapAgreement[key])
//			{
//				if (strcmp(it_r.AgreementNo, it_c.c_str()) == 0 && it_r.PositionOffset == SMS_O_OPEN)
//				{
//					AddAgreementToList1(it_r);
//				}
//			}
//		}
//
//	}
//}

void AgreementWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void AgreementWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_AddAgreement; i <= IDM_ManageAgreementRel; i++)
		EnableMenuItem(m_menuAgree, i, flage);

}

void AgreementWnd::DealDelAgreement()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "提示", L"确定删除此合同？", TMB_OKCANCEL))
		return;
	if (!m_System)return;
	SmsAgreementDelReq delReq;

	memset(&delReq, 0, sizeof(delReq));
	int row = m_listAgree.GetSelectItem();
	m_listAgree.GetSubItem(row, AGREE_AgreementNo, delReq.AgreementNo, sizeof(SmsAgreementNoType));
	strcpy_s(delReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strcpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);

	g_pSpotSystem->DeleteAgreement(&delReq);
}

void AgreementWnd::OnBtnQry()
{
	string key = m_System->get_curkey();
	m_listAgree.DeleteAllItems();

	//用户账号
	wchar_t	userno[MAX_PATH] = { 0 };
	m_cbUserno.GetText(userno);
	bool bPart = false;
	if (m_cbUserno.GetSelect() != 0 && m_System->m_curLogin.UserType == SMS_USER_MANAGER)
	{
		bPart = true;
	}

	wstring	beginDate;
	wstring	endDate;
	m_cbtBeginDate.CxGetWindowText(beginDate);
	m_cbtEndDate.CxGetWindowText(endDate);
	int begin = DateStrToInt(LoadRC::unicode_to_ansi(beginDate));
	int end = DateStrToInt(LoadRC::unicode_to_ansi(endDate));
	wchar_t	select[128] = { 0 };
	m_cbAgree.GetText(select);
	if (strcmp("指定日期", LoadRC::unicode_to_ansi(select).c_str()) == 0)
	{
		for (auto it : m_System->m_map_mapAgreement[key])
		{
			if (DateStrToInt(it.second.SignDate) >= begin && DateStrToInt(it.second.SignDate) <= end)
			{
				if (bPart)
				{
					if (strcmp(it.second.UserNo,LoadRC::unicode_to_ansi(userno).c_str()) == 0)
					{
						AddAgreementToList(it.second);
					}
				}
				else
					AddAgreementToList(it.second);

			}
		}
	}
	else if (strcmp("全部合同", LoadRC::unicode_to_ansi(select).c_str()) == 0 )
	{
		for (auto it : m_System->m_map_mapAgreement[key])
		{
			if (bPart)
			{
				if (strcmp(it.second.UserNo, LoadRC::unicode_to_ansi(userno).c_str()) == 0)
				{
					AddAgreementToList(it.second);
				}
			}
			else
				AddAgreementToList(it.second);
		}
	}
	else if (strcmp("未关联合同", LoadRC::unicode_to_ansi(select).c_str()) == 0)
	{
		set<string> agreeno;
		agreeno.clear();
		for (auto itrel : m_System->m_mapAgreementRel[key])
		{
			agreeno.insert(itrel.SellAgreementNo);
			agreeno.insert(itrel.BuyAgreementNo);
		}

		for (auto it : m_System->m_map_mapAgreement[key])
		{
			set<string>::iterator find = agreeno.find(it.second.AgreementNo);
			if (find == agreeno.end())
			{
				if (bPart)
				{
					if (strcmp(it.second.UserNo, LoadRC::unicode_to_ansi(userno).c_str()) == 0)
					{
						AddAgreementToList(it.second);
					}
				}
				else
					AddAgreementToList(it.second);
			}
		}
	}
	else
	{
		set<string> agreeno;
		agreeno.clear();
		for (auto itrel : m_System->m_mapAgreementRel[key])
		{
			if (strcmp(itrel.SellAgreementNo, LoadRC::unicode_to_ansi(select).c_str()) == 0 || strcmp(itrel.BuyAgreementNo, LoadRC::unicode_to_ansi(select).c_str()) == 0)
			{
				agreeno.insert(itrel.SellAgreementNo);
				agreeno.insert(itrel.BuyAgreementNo);
			}
		}
		for (auto it : m_System->m_map_mapAgreement[key])
		{
			set<string>::iterator find = agreeno.find(it.second.AgreementNo);
			if (find != agreeno.end())
			{
				AddAgreementToList(it.second);
			}
		}
	}

}

void AgreementWnd::OnBtnClear()
{
	m_cbAgree.SetSelect(1);
	RefreshList();
}

std::vector<std::string> AgreementWnd::SplitString(const std::string & str, char cSplit)
{
	vector<string> vecstr;
	string tempstr(str);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), cSplit);
		if (tempit != tempstr.end())
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempstr.begin(), tempit);
			vecstr.push_back(tempstr.substr(0, dis));
			if (++tempit != tempstr.end())
			{
				tempstr.erase(tempstr.begin(), tempit);
				tempit = tempstr.begin();
			}
			else
				break;
		}
		else//没找到' '但有内容  类似abc;这是仍然要保存
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}

int AgreementWnd::DateStrToInt(const std::string & str)
{
	vector<string> tempvec = SplitString(str, '-');
	if (tempvec.size() < 3)
		return 0;
	int year = atoi(tempvec.at(0).c_str());
	int month = atoi(tempvec.at(1).c_str());
	int day = atoi(tempvec.at(2).c_str());
	return year * 10000 + month * 100 + day;
}

void AgreementWnd::AddAgreementToList(SmsAgreementDataRsp & agree,int index)
{
	int count = 0;
	if (index == -1)
		count = m_listAgree.GetItemCount();
	else
		count = index;
	//用户编号
	m_listAgree.AddItem(agree.UserNo, count, AGREE_UserNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合同编号
	if (agree.AgreementType == SMS_AGREEMENT_PURCHASE)
		m_listAgree.AddItem(agree.AgreementNo, count, AGREE_AgreementNo, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	else if (agree.AgreementType == SMS_AGREEMENT_SALE)
		m_listAgree.AddItem(agree.AgreementNo, count, AGREE_AgreementNo, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	//合同类型
	if (agree.AgreementType == SMS_AGREEMENT_PURCHASE)
		m_listAgree.AddItem("采购合同", count, AGREE_AgreementType, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (agree.AgreementType == SMS_AGREEMENT_SALE)
		m_listAgree.AddItem("销售合同", count, AGREE_AgreementType, DT_CENTER, g_ColorRefData.GetColorBlack());
	//规格编号
	m_listAgree.AddItem(agree.SpecificationNo, count, AGREE_SpecificationNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//现货数量
	m_listAgree.AddItem((double)agree.SpotQty, count, AGREE_SpotQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//现货可配量
	m_listAgree.AddItem((double)agree.SpotRemainQty, count, AGREE_SpotRemainQty, 0, DT_CENTER, agree.SpotRemainQty > 0 ? g_ColorRefData.GetColorBuyRed() : g_ColorRefData.GetColorBlack());
	//持仓量
	m_listAgree.AddItem((double)agree.PositionQty, count, AGREE_PositionQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//可开量
	m_listAgree.AddItem((double)agree.CanOpenQty, count, AGREE_CanOpenQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合同价格
	m_listAgree.AddItem((double)agree.SpotPrice, count, AGREE_SpotPrice, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合同状态 
	if (agree.AgreementState == SMS_AGREEMENT_ACTIVE)
		m_listAgree.AddItem("合同有效", count, AGREE_AgreementState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (agree.AgreementState == SMS_AGREEMENT_FINISH)
		m_listAgree.AddItem("合同结束", count, AGREE_AgreementState, DT_CENTER, g_ColorRefData.GetColorBlack());
	//录入日期
	m_listAgree.AddItem(agree.SignDate, count, AGREE_SignDate, DT_CENTER, g_ColorRefData.GetColorBlack());
	//修改时间
	m_listAgree.AddItem(agree.InsertTime, count, AGREE_InsertTime, DT_CENTER, g_ColorRefData.GetColorBlack());
	//公司编号
	m_listAgree.AddItem(agree.CompanyNo, count, AGREE_CompanyNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//序列号
	m_listAgree.AddItem((double)agree.SerialId, count, AGREE_SerialId, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//操作员
	m_listAgree.AddItem(agree.OperatorNo, count, AGREE_OperatorNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//修改时间
	m_listAgree.AddItem(agree.OperateTime, count, AGREE_OperateTime, DT_CENTER, g_ColorRefData.GetColorBlack());
}

void AgreementWnd::ReloadList()
{
	string key = m_System->get_curkey();
	m_listAgree.DeleteAllItems();

	for (auto it : m_System->m_map_mapAgreement[key])
	{
		AddAgreementToList(it.second);
	}
}

void AgreementWnd::RefreshList()
{
	//========================可配量、持仓量、可开量
	string key = m_System->get_curkey();
	for (auto & it_extra : m_System->m_map_mapAgreement[key])
	{
		//可配量
		int spotremain = m_System->m_instantPositionWnd->CalculatSpotRemain(it_extra.second.AgreementNo);
		it_extra.second.SpotRemainQty = spotremain;
		//持仓量
		vector<InstantPosInfo> v_posinfo;
		v_posinfo.clear();
		int position = m_System->m_instantPositionWnd->CalculatePosition(it_extra.second.AgreementNo, v_posinfo);
		it_extra.second.PositionQty = position;
		//可开量
		int canopen = m_System->m_exposureWnd->CalculateFutureCanOpen(it_extra.second.AgreementNo);
		it_extra.second.CanOpenQty = canopen;
	}
	//==================================================

	m_listAgree.DeleteAllItems();

	for (auto it : m_System->m_map_mapAgreement[key])
	{
		AddAgreementToList(it.second);
	}
}

//void AgreementWnd::AddAgreementToList2(SmsAgreementDataRsp & agree)
//{
	//用户编号
	//m_listCloseAgree.AddItem(agree.UserNo, 0, AGREE_UserNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//序列号
	//m_listCloseAgree.AddItem((double)agree.SerialId, 0, AGREE_SerialId, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//公司编号
	//m_listCloseAgree.AddItem(agree.CompanyNo, 0, AGREE_CompanyNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合同编号
	//m_listCloseAgree.AddItem(agree.AgreementNo, 0, AGREE_AgreementNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合同类型
	//if (agree.AgreementType == SMS_AGREEMENT_PURCHASE)
		//m_listCloseAgree.AddItem("采购合同", 0, AGREE_AgreementType, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	//else if (agree.AgreementType == SMS_AGREEMENT_SALE)
		//m_listCloseAgree.AddItem("销售合同", 0, AGREE_AgreementType, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	//现货数量
	//m_listCloseAgree.AddItem((int)agree.SpotQty, 0, AGREE_SpotQty);
	//合同价格
	//m_listCloseAgree.AddItem((double)agree.SpotPrice, 0, AGREE_SpotPrice, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//录入日期
	//m_listCloseAgree.AddItem(agree.SignDate, 0, AGREE_SignDate, DT_CENTER, g_ColorRefData.GetColorBlack());
	//修改时间
	//m_listCloseAgree.AddItem(agree.InsertTime, 0, AGREE_InsertTime, DT_CENTER, g_ColorRefData.GetColorBlack());
	//规格编号
	//m_listCloseAgree.AddItem(agree.SpecificationNo, 0, AGREE_SpecificationNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//开平仓类型
	//if (agree.PositionOffset == SMS_O_OPEN)
		//m_listCloseAgree.AddItem("开仓", 0, AGREE_PositionOffset, DT_CENTER, g_ColorRefData.GetColorBlack());
	//else if (agree.PositionOffset == SMS_O_COVER)
		//m_listCloseAgree.AddItem("平仓", 0, AGREE_PositionOffset, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合同状态 
	//if (agree.AgreementState == SMS_AGREEMENT_ACTIVE)
		//m_listCloseAgree.AddItem("合同有效", 0, AGREE_AgreementState, DT_CENTER, g_ColorRefData.GetColorBlack());
	//else if (agree.AgreementState == SMS_AGREEMENT_FINISH)
		//m_listCloseAgree.AddItem("合同结束", 0, AGREE_AgreementState, DT_CENTER, g_ColorRefData.GetColorBlack());
	//操作员
	//m_listCloseAgree.AddItem(agree.OperatorNo, 0, AGREE_OperatorNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//修改时间
	//m_listCloseAgree.AddItem(agree.OperateTime, 0, AGREE_OperateTime, DT_CENTER, g_ColorRefData.GetColorBlack());

//}

