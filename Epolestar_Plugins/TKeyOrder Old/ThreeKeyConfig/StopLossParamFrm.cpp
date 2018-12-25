#include"PreInclude.h"

//@String
static TCHAR szCHS[][50] = {
	L"第一批成交价", L"委托发出时对价", L"限价止损 + 限价止盈", L"限价止损 + 保本", L"动态追踪 + 保本",
	L"动态追踪", L"限价止损", L"限价止盈", L"开仓自动止损止盈（条件单、反手除外）", L"排队价",
	L"对价", L"超价", L"市价", L"最新价",L"增加",
	L"删除", L"合约号", L"最小变动价", L"止损价差(回撤)点数",L" 止盈价差(保本)点数",
	L"连续", L"笔成交满足条件，才触发", L"开仓自动止损的基准价", L"默认策略", L"止损单和保本单委托价格",
	L"止盈委托价格", L"说明：市价是以停板价发出委托，成交在最新价", L"自动止损合约号", L"最小变动价", L"止损价差点数(追踪回撤)",
	L"止盈价差点数(保本点数)", L"提示", L"不存在该合约",

};
static TCHAR szCHT[][50] = {
	L"第壹批成交價", L"委托發出時對價", L"限價止損 + 限價止盈", L"限價止損 + 保本", L"動態追蹤 + 保本",
	L"動態追蹤", L"限價止損", L"限價止盈", L"開倉自動止損止盈（條件單、反手除外）", L"排隊價",
	L"對價", L"超價", L"市價", L"最新價", L"增加",
	L"刪除", L"合約號", L"最小變動價", L"止損價差(回撤)點數", L" 止盈價差(保本)點數",
	L"連續", L"筆成交滿足條件，才觸發", L"開倉自動止損的基準價", L"默認策略", L"止損單和保本單委托價格",
	L"止盈委托價格", L"說明：市價是以停板價發出委托，成交在最新價", L"自動止損合約號", L"最小變動價", L"止損價差點數(追蹤回撤)",
	L"止盈價差點數(保本點數)", L"提示", L"不存在該合約",
};
static TCHAR szENU[][50] = {
	L"The first Filled Price", L"Counter Price sending orders", L"Stop Loss + Stop Profit", L"Stop Loss + Break Even", L"Float Point + Break Even",
	L"Float Point", L"Stop Loss", L"Stop Profit", L"Open Auto StopLP(Except Condition Order/BackHand)", L"Queued Price",
	L"Counter Price", L"Exceed Price", L"Market Price", L"Last Price", L"Add",
	L"Delete", L"ContractNo", L"MinChg Price", L"StopLoss Spread(Float)", L" StopProfit Spread(BreakEven)",
	L"連續", L"筆成交滿足條件，才觸發", L"開倉自動止損的基準價", L"默認策略", L"止損單和保本單委托價格",
	L"止盈委托價格", L"說明：市價是以停板價發出委托，成交在最新價", L"自動止損合約號", L"最小變動價", L"止損價差點數(追蹤回撤)",
	L"止盈價差點數(保本點數)", L"提示", L"不存在該合約",
};

static TCHAR* _GetStringById(int i)
{
	if (g_language == CHT)
	{
		if (i + 1 > (sizeof(szCHT) / sizeof(szCHT[0])))
			return 0;
		return szCHT[i];
	}
	else if (g_language == ENU)
	{
		if (i + 1 > (sizeof(szENU) / sizeof(szENU[0])))
			return 0;
		return szENU[i];
	}
	else
	{
		if (i + 1 > (sizeof(szCHS) / sizeof(szCHS[0])))
			return 0;
		return szCHS[i];
	}
}

extern bool g_MouseTrack;

TStopLossParamFrm::TStopLossParamFrm()
{
	m_bshowCodeList = true;
}
TStopLossParamFrm::~TStopLossParamFrm()
{
}
void TStopLossParamFrm::ShowFrm(HWND hParent, RECT rect, UINT show)
{
	if (!m_Hwnd)
	{
		//deal userno and sign
		CreateFrm(_T("StopLossParamFrm"), hParent, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE);
	}
	strcpy_s(m_Sign1, g_pGlobalModel->m_strSign.c_str());
	SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, show);
}
//bool TStopLossParamFrm::Create(HWND hParent)
//{
//	CreateFrm(_T("StopLossParamFrm"), hParent, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE);
//	g_pTradeData->GetAllUser(m_clUserContainer);
//	uint count = m_clUserContainer.size();
//	if (count)
//	{
//		const TUserInfo*pUser = (const TUserInfo*)m_clUserContainer.at(0);
//		strcpy_s(m_UserNo, pUser->UserNo);
//		strcpy_s(m_Sign1, pUser->Sign);
//	}
//	return true;
//}

void TStopLossParamFrm::MoveWindow(const int &x, const int &y, const int &cx, const int &cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, NULL, true);
}


void TStopLossParamFrm::UpdateConfig()
{
 	m_peditTriggerCount.SetWindowNumber(m_pstConfig->iTriggerCount);
	m_ckOpenAutoStopLoss.SetCheck(m_pstConfig->bOpenAutoStopLoss);

	m_cbOpenAutoStopLossBasicPrice.SetSelect(m_pstConfig->iOpenAutoStopLossBasicPrice);
	m_cbDefaultTactic.SetSelect(m_pstConfig->iDefaultTactic);
	m_cbStopLossOrderPrice.SetSelect(m_pstConfig->iStopLossOrderPrice);
	m_cbStopProfitOrderPrice.SetSelect(m_pstConfig->iStopProfitOrderPrice);
	ShowList();
}
//void TStopLossParamFrm::UpdateStruct()
//{
//	m_pstConfig->iTriggerCount = m_peditTriggerCount.GetWindowNumber();
//}
LRESULT TStopLossParamFrm::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClick(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM:
		OnMeasureListView(lParam);
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawListView(wParam, lParam);
		return PROCESSED;
	case SSWM_CLICK_CHECKBOX:
		m_pstConfig->bOpenAutoStopLoss = m_ckOpenAutoStopLoss.GetCheck();
		return PROCESSED;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		return PROCESSED;
	case SSWM_EDIT_PRESS_ENTER:
		if ((HWND)lParam == m_editContractNo.GetHwnd())
			m_PopupCodeList.OnReturn();
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtrlColor(wParam, lParam);
	default:
		return NOT_PROCESSED;
	}
}
void TStopLossParamFrm::OnCreate()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	rect.left = FIRST_COL_X_OFFSET + 30;
	rect.right = rect.left + 40;
	rect.top = FIRST_COL_Y_OFFSET + 25;
	rect.bottom = rect.top + 20;

	m_peditTriggerCount.Create(m_Hwnd, ES_NUMBER);
//	m_peditTriggerCount.SetWindowNumber(1);
	m_peditTriggerCount.SetRange(1, 100000);
	m_peditTriggerCount.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	m_cbOpenAutoStopLossBasicPrice.Create(m_Hwnd);
	m_cbOpenAutoStopLossBasicPrice.MoveWindow(FIRST_COL_X_OFFSET + 145, FIRST_COL_Y_OFFSET + 150, 145, 20*5);
	m_cbOpenAutoStopLossBasicPrice.AddString(_GetStringById(0));
	m_cbOpenAutoStopLossBasicPrice.AddString(_GetStringById(1));


	m_cbDefaultTactic.Create(m_Hwnd);
	m_cbDefaultTactic.MoveWindow(FIRST_COL_X_OFFSET + 54, FIRST_COL_Y_OFFSET + 175, 145, 20*5);
	m_cbDefaultTactic.AddString(_GetStringById(2));
	m_cbDefaultTactic.AddString(_GetStringById(3));
	m_cbDefaultTactic.AddString(_GetStringById(4));
	m_cbDefaultTactic.AddString(_GetStringById(5));
	m_cbDefaultTactic.AddString(_GetStringById(6));
	m_cbDefaultTactic.AddString(_GetStringById(7));

	m_ckOpenAutoStopLoss.Create(m_Hwnd, 0, _GetStringById(8));
	m_ckOpenAutoStopLoss.MoveWindow(FIRST_COL_X_OFFSET, FIRST_COL_Y_OFFSET + 125, 290, 20);
	m_ckOpenAutoStopLoss.SetFont(g_FontData.g_FontWord12);

	m_cbStopLossOrderPrice.Create(m_Hwnd);
	m_cbStopLossOrderPrice.MoveWindow(FIRST_COL_X_OFFSET + 160, FIRST_COL_Y_OFFSET + 50, 100, 20*5);
	m_cbStopLossOrderPrice.AddString(_GetStringById(9));
	m_cbStopLossOrderPrice.AddString(_GetStringById(10));
	m_cbStopLossOrderPrice.AddString(_GetStringById(11));
	m_cbStopLossOrderPrice.AddString(_GetStringById(12));
	m_cbStopLossOrderPrice.AddString(_GetStringById(13));

	m_cbStopProfitOrderPrice.Create(m_Hwnd);
	m_cbStopProfitOrderPrice.MoveWindow(FIRST_COL_X_OFFSET + 100, FIRST_COL_Y_OFFSET + 75, 100, 20*5);
	m_cbStopProfitOrderPrice.AddString(_GetStringById(9));
	m_cbStopProfitOrderPrice.AddString(_GetStringById(10));
	m_cbStopProfitOrderPrice.AddString(_GetStringById(11));
	m_cbStopProfitOrderPrice.AddString(_GetStringById(12));
	m_cbStopProfitOrderPrice.AddString(_GetStringById(13));


	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 120;
	rect.top = FIRST_COL_Y_OFFSET + 206; rect.bottom = rect.top + 20;

	m_editContractNo.Create(m_Hwnd, 0, ES_UPPERCASE, ID_EDITCONTRACTNO);
	m_editContractNo.SetFont(g_FontData.GetFontNum13());
	m_editContractNo.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = rect.right + 1;
	rect.right += 14;
	m_btnSearch.Create(m_Hwnd, " ", ID_BTNSEARCH);
	m_btnSearch.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_btnSearch.SetFont(g_FontData.g_FontNum13);

	if (g_language == CHT)
	{
		m_btnAdd.Create(m_Hwnd, "增加", ID_BTNADD);
		m_btnDelete.Create(m_Hwnd, "刪除", ID_BTNDELETE);

	}
	else if (g_language == ENU)
	{
		m_btnAdd.Create(m_Hwnd, "Add", ID_BTNADD);
		m_btnDelete.Create(m_Hwnd, "Delete", ID_BTNDELETE);
	}
	else
	{
		m_btnAdd.Create(m_Hwnd, "增加", ID_BTNADD);
		m_btnDelete.Create(m_Hwnd, "删除", ID_BTNDELETE);
	}
	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 100;
	rect.top = FIRST_COL_Y_OFFSET + 25 + 206; rect.bottom = rect.top + 20;
	m_btnAdd.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = rect.right + 10;
	rect.right += 120;
	m_btnDelete.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 100;
	rect.top = FIRST_COL_Y_OFFSET + 50 + 206; rect.bottom = rect.top + 20;
	m_editMinChangePrice.Create(m_Hwnd, CxEdit::floatStyle);
	m_editMinChangePrice.SetFont(g_FontData.g_FontNum15);
	m_editMinChangePrice.SetWindowNumber(1);
	m_editMinChangePrice.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 50;
	rect.top = FIRST_COL_Y_OFFSET + 75 + 206; rect.bottom = rect.top + 20;
	m_spinStopLoss.Create(m_Hwnd, ES_NUMBER);
//	m_spinStopLoss.sett(0);
	m_spinStopLoss.SetRange(0, 100);
	m_spinStopLoss.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 50;
	rect.top = FIRST_COL_Y_OFFSET + 100 + 206; rect.bottom = rect.top + 20;
	m_spinStopProfit.Create(m_Hwnd, CxEdit::intStyle);
	m_spinStopProfit.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
//	m_spinStopProfit.SetWindowTextW(0);
	m_spinStopProfit.SetRange(0, 100);

	//合约筛选下拉框
	m_PopupContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0);
	ShowWindow(m_PopupContractCode.GetHwnd(), SW_HIDE);
	//合约List控件
	DWORD dwStyle = WS_CLIPSIBLINGS | WS_BORDER | WS_POPUP;
	DWORD dwStyleeEx = WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE;
	m_PopupCodeList.CreatePopWin(this, L"popupcodeList", m_Hwnd, dwStyle, dwStyleeEx, 0, 0);
	m_PopupCodeList.UpdateData(g_pGlobalModel->m_strSign.c_str());
	ShowWindow(m_PopupContractCode.GetHwnd(), SW_HIDE);

	rect.left = SECOND_COL_X_OFFSET; rect.right = rect.left + 460;
	rect.top = SECOND_COL_Y_OFFSET; rect.bottom = rect.top + 300;
	m_listStopLoss.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_LISTDEFAULTQTY);
	m_listStopLoss.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_listStopLoss.AddColumn(_GetStringById(16), 120);
	m_listStopLoss.AddColumn(_GetStringById(17), 80);
	m_listStopLoss.AddColumn(_GetStringById(18), 130);
	m_listStopLoss.AddColumn(_GetStringById(19), 130);

	m_ckOpenAutoStopLoss.SetBkColor(g_ColorRefData.GetColorBackground());

	UpdateConfig();
}

void TStopLossParamFrm::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
	HPEN hpen = CreatePen(PS_SOLID, 1, RGB(180, 180, 180));
	SelectObject(memdc.GetHdc(), hpen);
	MoveToEx(memdc.GetHdc(), SECOND_COL_X_OFFSET - 5, SECOND_COL_Y_OFFSET, NULL);
	LineTo(memdc.GetHdc(), SECOND_COL_X_OFFSET - 5, SECOND_COL_Y_OFFSET + 175);
	LineTo(memdc.GetHdc(), FIRST_COL_X_OFFSET-5, SECOND_COL_Y_OFFSET + 175);
	DrawTextMessage(&memdc);
	DeleteObject(hpen);
}


void TStopLossParamFrm::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR p = (LPNMHDR)lParam;
	if (p->hwndFrom == m_listStopLoss.GetListHwnd() && p->code == NM_CLICK && m_listStopLoss.GetSelectItem() != -1)
	{
		int index = m_listStopLoss.GetSelectItem();
		if (m_listStopLoss.GetSelectItem() < m_listStopLoss.GetItemCount())
		{
			stSLContractPriceMargin st;
			if (!FindItem(index, st))
				return;
			SetContent(st);
		}
	}
}

void TStopLossParamFrm::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_editContractNo.GetHwnd() && (HWND)lParam == m_editContractNo.GetHwnd())
	{
		if (m_bshowCodeList &&  HIWORD(wParam) == EN_CHANGE)
		{
			string sCode = "";
			GetWindowTextAnsic(m_editContractNo.GetHwnd(), sCode);
			POINT pt = { 0, 0 };
			ClientToScreen(m_editContractNo.GetHwnd(), &pt);
			if (!IsWindowVisible(m_PopupCodeList.GetHwnd()))
			{
				pt.y += 20;
				SetWindowPos(m_PopupCodeList.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			}
			m_PopupCodeList.EditChange(sCode, m_Sign1);
		}
		else if (HIWORD(wParam) == EN_KILLFOCUS)
		{
			if (IsWindowVisible(m_PopupCodeList.GetHwnd()) && g_MouseTrack)
				ShowWindow(m_PopupCodeList.GetHwnd(), SW_HIDE);
		}
		return;
	}
	else if ((HWND)lParam == m_peditTriggerCount.GetEditHwnd())
	{
		if (HIWORD(wParam) == EN_CHANGE)
		{
			int i = m_peditTriggerCount.GetWindowNumber();
			if (i == 0)
			{
				i = 1;
				m_peditTriggerCount.SetWindowNumber(1);
			}
			m_pstConfig->iTriggerCount = i;
		}
	}
	HWND hwnd = (HWND)lParam;
	if (hwnd == m_cbOpenAutoStopLossBasicPrice.GetHwnd())
	{
		m_pstConfig->iOpenAutoStopLossBasicPrice = m_cbOpenAutoStopLossBasicPrice.GetSelect();
	}
	else if (hwnd == m_cbDefaultTactic.GetHwnd())
	{
		m_pstConfig->iDefaultTactic = m_cbDefaultTactic.GetSelect();
	}
	else if (hwnd == m_cbStopLossOrderPrice.GetHwnd())
	{
		m_pstConfig->iStopLossOrderPrice = m_cbStopLossOrderPrice.GetSelect();
	}
	else if (hwnd == m_cbStopProfitOrderPrice.GetHwnd())
	{
		m_pstConfig->iStopProfitOrderPrice = m_cbStopProfitOrderPrice.GetSelect();
	}
}

void TStopLossParamFrm::DrawTextMessage(TMemDC* pmemdc)
{
	//DrawText
	RECT rect;
	rect = { FIRST_COL_X_OFFSET, FIRST_COL_Y_OFFSET, FIRST_COL_X_OFFSET + 60, FIRST_COL_Y_OFFSET + 20 };
	SelectFont(pmemdc->GetHdc(), g_FontData.g_FontWord12);
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT);
	//DrawTextA(pmemdc->GetHdc(), "持续性", -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.left = FIRST_COL_X_OFFSET;	rect.right = rect.left + 30;
	rect.top = FIRST_COL_Y_OFFSET + 28; rect.bottom = rect.top + 20;
	DrawText(pmemdc->GetHdc(), _GetStringById(20), -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.left += 70; rect.right = rect.left + 150;
	DrawText(pmemdc->GetHdc(), _GetStringById(21), -1, &rect, DT_LEFT | DT_BOTTOM);
	rect.top += 125; rect.bottom += 125;
	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 140;
	DrawText(pmemdc->GetHdc(), _GetStringById(22), -1, &rect, DT_LEFT | DT_BOTTOM);
	rect.top += 25; rect.bottom += 25;
	rect.right = 55;
	DrawText(pmemdc->GetHdc(), _GetStringById(23), -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.top = FIRST_COL_Y_OFFSET + 53; rect.bottom = rect.top + 20;
	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 160;
	DrawText(pmemdc->GetHdc(), _GetStringById(24), -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.top = FIRST_COL_Y_OFFSET + 78; rect.bottom = rect.top + 20;
	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 100;
	DrawText(pmemdc->GetHdc(), _GetStringById(25), -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.top = FIRST_COL_Y_OFFSET + 103; rect.bottom = rect.top + 20;
	rect.left = FIRST_COL_X_OFFSET; rect.right = rect.left + 450;
	DrawText(pmemdc->GetHdc(), _GetStringById(26), -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.left = FIRST_COL_X_OFFSET + 150; rect.right = rect.left + 135;
	rect.top = FIRST_COL_Y_OFFSET + 3 + 206; rect.bottom = rect.top + 20;
	DrawText(pmemdc->GetHdc(), _GetStringById(27), -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.left = FIRST_COL_X_OFFSET + 110; rect.right = rect.left + 100;
	rect.top = FIRST_COL_Y_OFFSET + 53 + 206; rect.bottom = rect.top + 20;
	DrawText(pmemdc->GetHdc(), _GetStringById(28), -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.left = FIRST_COL_X_OFFSET + 50; rect.right = rect.left + 250;
	rect.top = FIRST_COL_Y_OFFSET + 78 + 206; rect.bottom = rect.top + 20;
	DrawText(pmemdc->GetHdc(), _GetStringById(29), -1, &rect, DT_LEFT | DT_BOTTOM);

	rect.left = FIRST_COL_X_OFFSET + 50; rect.right = rect.left + 250;
	rect.top = FIRST_COL_Y_OFFSET + 103 + 206; rect.bottom = rect.top + 20;
	DrawText(pmemdc->GetHdc(), _GetStringById(30), -1, &rect, DT_LEFT | DT_BOTTOM);
}


void TStopLossParamFrm::OnMeasureListView(LPARAM lParam)
{
	LPMEASUREITEMSTRUCT pmi = LPMEASUREITEMSTRUCT(lParam);
	if (pmi->CtlType == ODT_LISTVIEW && pmi->CtlID == ID_LISTDEFAULTQTY)
		m_listStopLoss.OnMeasureItem(pmi->itemHeight);
}
void TStopLossParamFrm::OnDrawListView(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = LPDRAWITEMSTRUCT(lParam);
	if (pDIS->CtlType == ODT_LISTVIEW && pDIS->CtlID == ID_LISTDEFAULTQTY)
		m_listStopLoss.OnDrawItem(lParam);
}


void TStopLossParamFrm::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BTNSEARCH:
		OnSearchCode();
		break;
	case ID_BTNADD:
		OnBtnAdd();
		break;
	case ID_BTNDELETE:
		OnBtnDelete();
		break;
	}
}

void TStopLossParamFrm::OnSearchCode()
{
	POINT pt = { FIRST_COL_X_OFFSET, FIRST_COL_Y_OFFSET + 20 + 200+6 };
	ClientToScreen(m_Hwnd, &pt);
	m_PopupContractCode.UpdateData(m_Sign1);
	SetWindowPos(m_PopupContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}
void TStopLossParamFrm::OnBtnAdd()
{
	InsertMap();
	ShowList();
}
void TStopLossParamFrm::OnBtnDelete()
{
	DeleteMap();
	ShowList();
}
extern TGlobalModel* g_pGlobalModel;
void TStopLossParamFrm::SetContractNo(string sCode)
{
	m_bshowCodeList = false;
	m_sCode = sCode;
	m_editContractNo.SetText(m_sCode.c_str());
	EnableWindow(m_editMinChangePrice.GetHwnd(), true);
	TContractKey stContract;
	memset(&stContract, 0, sizeof(stContract));
	memcpy(&stContract.Sign, g_pGlobalModel->m_strSign.c_str(), sizeof(stContract.Sign));
	if (!CConvert::TContractNoToTContractKey(sCode.c_str(), stContract))
		return;
	GetMinChangePrice(stContract);
	m_spinStopLoss.SetWindowNumber(0);
	m_spinStopProfit.SetWindowNumber(0);
	m_bshowCodeList = true;
}
void TStopLossParamFrm::InsertMap()
{
	stSLContractPriceMargin stConfig;
	memset(&stConfig.stContract, 0, sizeof(stConfig.stContract));

	stConfig.iStopLossPriceGap = m_spinStopLoss.GetWindowNumber();
	stConfig.iStopProfitPriceGap = m_spinStopProfit.GetWindowNumber();
	stConfig.dMinChangePrice = m_editMinChangePrice.GetDoubleData();

	if (!GetContract(stConfig.stContract))
		return;
	char cContractKey[150] = { 0 };
	GetContractKey(cContractKey, stConfig.stContract);
	std::map<std::string, stSLContractPriceMargin>::iterator iter = m_pstConfig->stdmapContract.find(cContractKey);
	if (iter != m_pstConfig->stdmapContract.end())
	{
		m_pstConfig->stdmapContract.erase(cContractKey);
		m_pstConfig->stdmapContract.insert(std::make_pair(cContractKey, stConfig));
	}
	else
		m_pstConfig->stdmapContract.insert(std::make_pair(cContractKey, stConfig));

}
void TStopLossParamFrm::DeleteMap()
{
	TContractKey stContractKey;
	memset(&stContractKey, 0, sizeof(stContractKey));
	if (!GetContract(stContractKey))
		return;
	char cContractKey[150] = { 0 };
	GetContractKey(cContractKey, stContractKey);

	std::map<std::string, stSLContractPriceMargin>::iterator iter = m_pstConfig->stdmapContract.find(cContractKey);
	if (iter != m_pstConfig->stdmapContract.end())
		m_pstConfig->stdmapContract.erase(cContractKey);
	else
	{
		_TMessageBox_Domodal(m_Hwnd, const_cast<char*>(LoadRC::unicode_to_ansi(_GetStringById(31)).c_str()), _GetStringById(32), TMB_OK);

		return;
	}
}
bool TStopLossParamFrm::GetContract(TContractKey& ContractKey)
{
	memcpy(ContractKey.Sign, m_Sign1, sizeof(m_Sign1));
	return CConvert::TContractNoToTContractKey(m_sCode.c_str(), ContractKey);
}
void TStopLossParamFrm::GetPriKey(char* cContractKey, stSLContractPriceMargin stQty)
{
	sprintf_s(cContractKey, 150, "%s|%c|%s|%s|%s|%c|%s|%s|%c,%d,%d,%f",
		stQty.stContract.ExchangeNo, stQty.stContract.CommodityType, stQty.stContract.CommodityNo,
		stQty.stContract.ContractNo, stQty.stContract.StrikePrice, stQty.stContract.OptionType,
		stQty.stContract.ContractNo2, stQty.stContract.StrikePrice2, stQty.stContract.OptionType2, stQty.iStopLossPriceGap, stQty.iStopProfitPriceGap, stQty.dMinChangePrice);
}
void TStopLossParamFrm::GetContractKey(char* cContractKey, TContractSingleKey stContract)
{
	sprintf_s(cContractKey, 150, "%s%c%s%s%s%c",
		stContract.ExchangeNo, stContract.CommodityType, stContract.CommodityNo,
		stContract.ContractNo, stContract.StrikePrice, stContract.OptionType);
}
void TStopLossParamFrm::ShowList()
{
	m_listStopLoss.DeleteAllItems();
	std::map<std::string, stSLContractPriceMargin>::iterator iter;
	std::string ContractCode;
	int iRow = 0;
	int iCol = 0;
	for (iter = m_pstConfig->stdmapContract.begin(); iter != m_pstConfig->stdmapContract.end(); iter++)
	{
		iCol = 0;
		if (!CConvert::TContractKeyToTContractNo(iter->second.stContract, ContractCode))
			return;
		m_listStopLoss.AddItem(ContractCode.c_str(), iRow, iCol++);
		m_listStopLoss.AddItem(iter->second.dMinChangePrice, iRow, iCol++);
		m_listStopLoss.AddItem(iter->second.iStopLossPriceGap, iRow, iCol++);
		m_listStopLoss.AddItem(iter->second.iStopProfitPriceGap, iRow, iCol++);
		iRow++;
	}
}

void TStopLossParamFrm::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT id = GetWindowLong(GetFocus(), GWL_ID);
	if (id == ID_EDITCONTRACTNO)
	{
		if (!IsWindowVisible(m_PopupCodeList.GetHwnd()))
			return;
		int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (nDelta > 0)
			m_PopupCodeList.DelNum();
		else
			m_PopupCodeList.AddNum();
	}
}

bool TStopLossParamFrm::FindItem(int index, stSLContractPriceMargin &st)
{
	if (index < 0)
		return false;
	std::map<std::string, stSLContractPriceMargin >::iterator iter;
	int i = 0;
	for (iter = m_pstConfig->stdmapContract.begin(); iter != m_pstConfig->stdmapContract.end(); iter++)
	{
		if (i == index)
		{
			memcpy(&st, &iter->second, sizeof(stSLContractPriceMargin));
			return true;
		}
		i++;
	}
	return false;
}

void TStopLossParamFrm::SetContent(stSLContractPriceMargin st)
{
	m_bshowCodeList = false;
	std::string ContractCode;
	if (!CConvert::TContractKeyToTContractNo(st.stContract, ContractCode))
		return;
	m_sCode = ContractCode;
	m_editContractNo.SetText(ContractCode.c_str());
	m_spinStopLoss.SetWindowNumber(st.iStopLossPriceGap);
	m_spinStopProfit.SetWindowNumber(st.iStopProfitPriceGap);
	m_editMinChangePrice.SetDoubleData(st.dMinChangePrice);
	m_bshowCodeList = true;
}

bool TStopLossParamFrm::GetMinChangePrice(TContractKey& key)
{
	const SCommodity* pstCommodityInfo = g_pTradeData->GetCommodity(&key);
	if (!pstCommodityInfo)
		return false;

	m_editMinChangePrice.SetDoubleData(pstCommodityInfo->PriceTick);
	
	EnableWindow(m_editMinChangePrice.GetHwnd(), false);
	return true;
}