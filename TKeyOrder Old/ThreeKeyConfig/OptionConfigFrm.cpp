#include"PreInclude.h"

//@String
static char szCHS[][50] = {
	"双击持仓和挂单列表的操作确认", "下单窗口位置记忆", "通用委托确认", "双击持仓列表平仓", "大单拆分(单笔手数超过交易所上限，自动拆分)",
	"该合约默认手数", "该合约全部可用持仓", "排队价", "对价", "超价",
	"市价", "最新价", "投机", "保值", "点持仓列表填单手数",
	"反手默认下单方式", "比例平仓下单方式", "账户清仓下单方式", "投机保值(内)",
};
static char szCHT[][50] = {
	"雙擊持倉和掛單列表的操作確認", "下單窗口位置記憶", "通用委托確認", "雙擊持倉列表平倉", "大單拆分(單筆手數超過交易所上限，自動拆分)",
	"該合約默認手數", "該合約全部可用持倉", "排隊價", "對價", "超價",
	"市價", "最新價", "投機", "保值", "點持倉列表填單手數",
	"反手默認下單方式", "比例平倉下單方式", "賬戶清倉下單方式", "投機保值(內)",
};
static char szENU[][50] = {
	"雙擊持倉和掛單列表的操作確認", "下單窗口位置記憶", "通用委托確認", "雙擊持倉列表平倉", "大單拆分(單筆手數超過交易所上限，自動拆分)",
	"該合約默認手數", "該合約全部可用持倉", "排隊價", "對價", "超價",
	"市價", "最新價", "投機", "保值", "點持倉列表填單手數",
	"反手默認下單方式", "比例平倉下單方式", "賬戶清倉下單方式", "投機保值(內)",
};

static char* _GetStringById(int i)
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


OptionConfigFrm::OptionConfigFrm()
{
}

OptionConfigFrm::~OptionConfigFrm()
{

}

void OptionConfigFrm::ShowFrm(HWND hParent, RECT rect, UINT show)
{
	if (!m_Hwnd)
		CreateFrm(_T("OptionConfigFrame"), hParent, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE);
	if (show&SWP_SHOWWINDOW)
		SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, show);
	else
		ShowWindow(m_Hwnd, SW_HIDE);
}

void OptionConfigFrm::MoveWindow(const int &x, const int &y, const int &cx, const int &cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, NULL, true);
}
void OptionConfigFrm::RegistSt(OptionConfig* pstConfig)
{
	if (pstConfig == NULL)return;
	m_stConfig = pstConfig;
}
void OptionConfigFrm::UpdateConfig()
{
	m_ckWndPosition.SetCheck(m_stConfig->bWndPosition);
	m_ckDoublePositionConfirm.SetCheck(m_stConfig->bDoublePositionConfirm);
	m_ckThreeKeyConfirm.SetCheck(m_stConfig->bOrderConfirm);
	m_ckDoubleClickPositionClose.SetCheck(m_stConfig->bDoubleClickPositionClose);
	m_ckSplitBigOrder.SetCheck(m_stConfig->bBigOrderSplit);

	m_cbPositionQty.SetSelect(m_stConfig->iPositionQty);
	m_cbBackFillOrderMod.SetSelect(m_stConfig->iBackFillOrderMod);
	m_cbProportionFillOrderMod.SetSelect(m_stConfig->iProportionFillOrderMod);
	if (m_stConfig->iCloseAllFillOrderMod == ptCounterPrice)
		m_cbCloseAllFillOrderMod.SetSelect(0);
	else if (m_stConfig->iCloseAllFillOrderMod == ptSuperPrice)
		m_cbCloseAllFillOrderMod.SetSelect(1);
	else if (m_stConfig->iCloseAllFillOrderMod == ptMarketPrice)
		m_cbCloseAllFillOrderMod.SetSelect(2);

	if (m_stConfig->Hedge == hSpeculate)
		m_cbHedge.SetSelect(0);
	else
		m_cbHedge.SetSelect(1);
}

void OptionConfigFrm::OnCreate()
{	
	m_ckDoublePositionConfirm.Create(m_Hwnd,4, ANSIToUnicode(_GetStringById(0)).c_str());
	m_ckDoublePositionConfirm.MoveWindow(0, 0 + m_iThreeKeyYoffset, 220, 20);
	m_ckDoublePositionConfirm.SetFont(g_FontData.g_FontWord12);

	m_ckWndPosition.Create(m_Hwnd, 1, ANSIToUnicode(_GetStringById(1)).c_str());
	m_ckWndPosition.MoveWindow(220, 0 + m_iThreeKeyYoffset, 140, 20);
	m_ckWndPosition.SetFont(g_FontData.g_FontWord12);

	m_ckThreeKeyConfirm.Create(m_Hwnd, 2, ANSIToUnicode(_GetStringById(2)).c_str());
	m_ckThreeKeyConfirm.MoveWindow(0, 20 + m_iThreeKeyYoffset, 140, 20);
	m_ckThreeKeyConfirm.SetFont(g_FontData.g_FontWord12);

	m_ckDoubleClickPositionClose.Create(m_Hwnd, 3, ANSIToUnicode(_GetStringById(3)).c_str());
	m_ckDoubleClickPositionClose.MoveWindow(220, 20 + m_iThreeKeyYoffset, 140, 20);
	m_ckDoubleClickPositionClose.SetFont(g_FontData.g_FontWord12);

	m_ckSplitBigOrder.Create(m_Hwnd, 5, ANSIToUnicode(_GetStringById(4)).c_str());
	 m_ckSplitBigOrder.MoveWindow(0, 40 + m_iThreeKeyYoffset, 300, 20);
	 m_ckSplitBigOrder.SetFont(g_FontData.g_FontWord12);

	m_cbPositionQty.Create(m_Hwnd);
	m_cbPositionQty.MoveWindow(130, CHECKBOXYOFFSET + m_iThreeKeyYoffset, 145, 20*5);
	m_cbPositionQty.AddString(ANSIToUnicode(_GetStringById(5)).c_str());
	m_cbPositionQty.AddString(ANSIToUnicode(_GetStringById(6)).c_str());
//	m_cbPositionQty.SetSelect(0); //会触发OnCommand消息

	m_cbBackFillOrderMod.Create(m_Hwnd,1);
	m_cbBackFillOrderMod.MoveWindow(130, CHECKBOXYOFFSET + m_iThreeKeyYoffset+25, 100, 20*5);
	m_cbBackFillOrderMod.AddString(ANSIToUnicode(_GetStringById(7)).c_str());
	m_cbBackFillOrderMod.AddString(ANSIToUnicode(_GetStringById(8)).c_str());
	m_cbBackFillOrderMod.AddString(ANSIToUnicode(_GetStringById(9)).c_str());
	m_cbBackFillOrderMod.AddString(ANSIToUnicode(_GetStringById(10)).c_str());
	m_cbBackFillOrderMod.AddString(ANSIToUnicode(_GetStringById(11)).c_str());
//	m_cbBackFillOrderMod.SetSelect(0);

	m_cbProportionFillOrderMod.Create(m_Hwnd,2);
	m_cbProportionFillOrderMod.MoveWindow(130, CHECKBOXYOFFSET + m_iThreeKeyYoffset+25*2, 100, 20*5);
	m_cbProportionFillOrderMod.AddString(ANSIToUnicode(_GetStringById(7)).c_str());
	m_cbProportionFillOrderMod.AddString(ANSIToUnicode(_GetStringById(8)).c_str());
	m_cbProportionFillOrderMod.AddString(ANSIToUnicode(_GetStringById(9)).c_str());
	m_cbProportionFillOrderMod.AddString(ANSIToUnicode(_GetStringById(10)).c_str());
	m_cbProportionFillOrderMod.AddString(ANSIToUnicode(_GetStringById(11)).c_str());
//	m_cbProportionFillOrderMod.SetSelect(0);

	m_cbCloseAllFillOrderMod.Create(m_Hwnd,3);
	m_cbCloseAllFillOrderMod.MoveWindow(130, CHECKBOXYOFFSET + m_iThreeKeyYoffset+25*3, 60, 20*5);
	m_cbCloseAllFillOrderMod.AddString(ANSIToUnicode(_GetStringById(8)).c_str());
	m_cbCloseAllFillOrderMod.AddString(ANSIToUnicode(_GetStringById(9)).c_str());
	m_cbCloseAllFillOrderMod.AddString(ANSIToUnicode(_GetStringById(10)).c_str());
//	m_cbCloseAllFillOrderMod.SetSelect(0);

	m_cbHedge.Create(m_Hwnd, 4);
	m_cbHedge.MoveWindow(130, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 * 4, 60, 20 * 5);
	m_cbHedge.AddString(ANSIToUnicode(_GetStringById(12)).c_str());
	m_cbHedge.AddString(ANSIToUnicode(_GetStringById(13)).c_str());
//	m_cbHedge.SetSelect(0);

	m_ckWndPosition.SetBkColor(g_ColorRefData.GetColorBackground());
	m_ckDoublePositionConfirm.SetBkColor(g_ColorRefData.GetColorBackground());
	m_ckThreeKeyConfirm.SetBkColor(g_ColorRefData.GetColorBackground());
	m_ckDoubleClickPositionClose.SetBkColor(g_ColorRefData.GetColorBackground());
	m_ckSplitBigOrder.SetBkColor(g_ColorRefData.GetColorBackground());//大单拆分

	UpdateConfig();
}

LRESULT OptionConfigFrm::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_SHOWWINDOW:
		OnShowWindow(wParam, lParam);
		return PROCESSED;
	case SSWM_CLICK_CHECKBOX:
		OnClickCheckBox(wParam, lParam);
		break;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		break;
	case WM_CTLCOLORBTN:
		return (LRESULT)g_ColorRefData.g_brush_white;
	case WM_CTLCOLORSTATIC:
		return OnCtrlColor(wParam, lParam);
	default:
		break;
	}
	return NOT_PROCESSED;
}

void OptionConfigFrm::OnShowWindow(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0)
	{
		if (wParam)
			UpdateConfig();
		else               //call ShowWindow to hide the frm
			GetData();
	}
}
void OptionConfigFrm::OnClickCheckBox(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = HWND(lParam);
	if (hwnd == m_ckWndPosition.GetHwnd())
		m_stConfig->bWndPosition = m_ckWndPosition.GetCheck();
	else if (hwnd == m_ckDoublePositionConfirm.GetHwnd())
		m_stConfig->bDoublePositionConfirm = m_ckDoublePositionConfirm.GetCheck();
	else if (hwnd == m_ckThreeKeyConfirm.GetHwnd())
		m_stConfig->bOrderConfirm = m_ckThreeKeyConfirm.GetCheck();
	else if (hwnd == m_ckDoubleClickPositionClose.GetHwnd())
		m_stConfig->bDoubleClickPositionClose = m_ckDoubleClickPositionClose.GetCheck();
	else if (hwnd == m_ckSplitBigOrder.GetHwnd())
		m_stConfig->bBigOrderSplit = m_ckSplitBigOrder.GetCheck();
}
void OptionConfigFrm::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
	if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (m_cbPositionQty.GetHwnd() == hwnd)
		{
			m_stConfig->iPositionQty = m_cbPositionQty.GetSelect();
		}
		else if (m_cbBackFillOrderMod.GetHwnd() == hwnd)
		{
			m_stConfig->iBackFillOrderMod = m_cbBackFillOrderMod.GetSelect();
		}
		else if (m_cbProportionFillOrderMod.GetHwnd() == hwnd)
		{
			m_stConfig->iProportionFillOrderMod = m_cbProportionFillOrderMod.GetSelect();
		}
		else if (m_cbCloseAllFillOrderMod.GetHwnd() == hwnd)
		{
			int index = m_cbCloseAllFillOrderMod.GetSelect();
			if (index == 0)
				m_stConfig->iCloseAllFillOrderMod = ptCounterPrice;
			else if (index == 1)
				m_stConfig->iCloseAllFillOrderMod = ptSuperPrice;
			else if (index == 2)
				m_stConfig->iCloseAllFillOrderMod = ptMarketPrice;
		}
		else if (m_cbHedge.GetHwnd() == hwnd)
		{
			int index = m_cbHedge.GetSelect();
			if (index == 0)
				m_stConfig->Hedge = hSpeculate;
			else
				m_stConfig->Hedge = hHedge;
		}
	}

}

void OptionConfigFrm::GetData()
{
	m_stConfig->bWndPosition = m_ckWndPosition.GetCheck();
	m_stConfig->bDoublePositionConfirm = m_ckDoublePositionConfirm.GetCheck();
	m_stConfig->bOrderConfirm = m_ckThreeKeyConfirm.GetCheck();
	m_stConfig->bDoubleClickPositionClose = m_ckDoubleClickPositionClose.GetCheck();
	m_stConfig->bBigOrderSplit = m_ckSplitBigOrder.GetCheck();

	m_stConfig->iPositionQty = m_cbPositionQty.GetSelect();
	m_stConfig->iBackFillOrderMod = m_cbBackFillOrderMod.GetSelect();
	m_stConfig->iProportionFillOrderMod = m_cbProportionFillOrderMod.GetSelect();

	int index = m_cbCloseAllFillOrderMod.GetSelect();
	if (index == 0)
		m_stConfig->iCloseAllFillOrderMod = ptCounterPrice;
	else if (index == 1)
		m_stConfig->iCloseAllFillOrderMod = ptSuperPrice;
	else if (index == 2)
		m_stConfig->iCloseAllFillOrderMod = ptMarketPrice;

	index = m_cbHedge.GetSelect();
	if (index == 0)
		m_stConfig->Hedge = hSpeculate;
	else
		m_stConfig->Hedge = hHedge;
}
void OptionConfigFrm::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);

	DrawTextMessage(&memdc);
}

void OptionConfigFrm::DrawTextMessage(TMemDC* pmemdc)
{
	RECT rect;
	rect = { 0, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 2, 120, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 20 + 2 };
	SelectFont(pmemdc->GetHdc(), g_FontData.g_FontWord12);
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT);
	DrawText(pmemdc->GetHdc(), LoadRC::ansi_to_unicode(_GetStringById(14)).c_str(), -1, &rect, DT_LEFT | DT_VCENTER);
	rect = { 0, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 + 2, 120, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 + 20 + 2 };
	DrawText(pmemdc->GetHdc(), LoadRC::ansi_to_unicode(_GetStringById(15)).c_str(), -1, &rect, DT_LEFT | DT_VCENTER);
	rect = { 0, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 * 2 + 2, 120, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 * 2 + 20 + 2 };
	DrawText(pmemdc->GetHdc(), LoadRC::ansi_to_unicode(_GetStringById(16)).c_str(), -1, &rect, DT_LEFT | DT_VCENTER);
	rect = { 0, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 * 3 + 2, 120, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 * 3 + 20 + 2 };
	DrawText(pmemdc->GetHdc(), LoadRC::ansi_to_unicode(_GetStringById(17)).c_str(), -1, &rect, DT_LEFT | DT_VCENTER);
	rect = { 0, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 * 4 + 2, 120, CHECKBOXYOFFSET + m_iThreeKeyYoffset + 25 * 4 + 20 + 2 };
	DrawText(pmemdc->GetHdc(), LoadRC::ansi_to_unicode(_GetStringById(18)).c_str(), -1, &rect, DT_LEFT | DT_VCENTER);
}