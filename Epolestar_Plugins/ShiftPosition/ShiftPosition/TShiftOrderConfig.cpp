#include "BaseInclude.h"
#define  closeWidth 9
const int FRAME_H = 300;
const int FRAME_V = 454;
extern ShiftOrderConfig *g_pstConfig;
extern TShiftPosition	*g_pShiftPosition;


void TShiftConfig::Create(HWND hParent)
{
	if (!IsWindow(m_Hwnd))
	{
		m_hParent = hParent;
		CreateFrm(L"ShiftConfig", hParent, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS);
	} 
	else
	{
		if (IsWindowVisible(m_Hwnd))
			ShowWindow(m_Hwnd, SW_HIDE);
		else
			ShowWindow(m_Hwnd, SW_SHOW);
	}
}
void TShiftConfig::MoveWindow(int x, int y)
{
	SetWindowPos(m_Hwnd, 0, x, y, FRAME_V, FRAME_H, SWP_NOZORDER);
	InitRcClose();
}
void TShiftConfig::RegistConfig(ShiftOrderConfig* stConfig)
{
	if (!stConfig)
		return;
	m_pstConfig = stConfig;
}
LRESULT TShiftConfig::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_COMMAND:
		return OnCommand(wParam, lParam);
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtrlColor(wParam, lParam);
	case SSWM_CLICK_CHECKBOX:
		SingleOrder(wParam);
	default:
		return NOT_PROCESSED;
	}
}

void TShiftConfig::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	int oldDc = SaveDC(memdc.GetHdc());
	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	RECT rcClient;
	GetClientRect(m_Hwnd, &rcClient);
	HBRUSH hbrBorder = CreateSolidBrush(RGB(100, 100, 100));
	InflateRect(&rcClient, -1, -1);
	FrameRect(memdc.GetHdc(), &rcClient, hbrBorder);
	InflateRect(&rcClient, -1, -1);
	FrameRect(memdc.GetHdc(), &rcClient, hbrBorder);

	RECT rcCaption = { rcClient.left, rcClient.top, rcClient.right, CAPTION_H };

	HBRUSH hbrCaption = CreateSolidBrush(RGB(100, 100, 100));
	FillRect(memdc.GetHdc(), &rcCaption, hbrCaption);

	rcClient.top = CAPTION_H;
	FillRect(memdc.GetHdc(), &rcClient, g_ColorRefData.g_brush_white);

	rcCaption.left = 8;
	SelectObject(memdc.GetHdc(), g_FontData.GetFontNum13());
	SetTextColor(memdc.GetHdc(), RGB(238, 238, 238));
	DrawText(memdc.GetHdc(),L"移仓配置界面", -1, &rcCaption, DT_LEFT | DT_VCENTER | DT_SINGLELINE);


	SetTextColor(memdc.GetHdc(), RGB(0, 0, 0));
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord12());
	RECT rcRemark = { 15, FRAME_H - 20, FRAME_V - 15, FRAME_H };
	DrawText(memdc.GetHdc(), L"说明：追价参考 “通用下单” 追价参数介绍", -1, &rcRemark, DT_LEFT | DT_WORDBREAK);
	DrawClose(&memdc);


	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}
void TShiftConfig::OnCreate()
{
	CreateWidget();
	SetWidgetPos();
	InitWidget();
}
void TShiftConfig::OnSize(WPARAM wParam, LPARAM lParam)
{

}
LRESULT TShiftConfig::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	int msg = HIWORD(wParam);

	if (id == id_edittime && msg == EN_CHANGE) {
		if (m_sEditTime.GetWindowNumber() < 1)
			m_sEditTime.SetWindowNumber(1);
		g_pstConfig->m_stChasePrice.iSecond = m_sEditTime.GetWindowNumber();
	}
	else if (id == id_cbprice && msg == CBN_SELCHANGE) {
		g_pstConfig->m_stChasePrice.iChasePriceCom = m_cbPrice.GetSelect();
	}
	else if (id == id_cbstop && msg == CBN_SELCHANGE) {
		g_pstConfig->m_stChasePrice.iChaseOver = m_cbStop.GetSelect();
	}
	else if (id == id_editconditiontime && msg == EN_CHANGE) {
		if (m_sEditConditionTime.GetWindowNumber() < 1)
			m_sEditConditionTime.SetWindowNumber(1);
		g_pstConfig->m_stChasePrice.iConditionSecond = m_sEditConditionTime.GetWindowNumber();
	}
	else if (id == id_cbinsuretype && msg == CBN_SELCHANGE) {
		g_pstConfig->m_stChasePrice.Hedge = m_cbInsureType.GetSelect();
	}
	return NOT_PROCESSED;
}
LRESULT TShiftConfig::OnCtrlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_white;
}
void TShiftConfig::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.bottom = rect.top + CAPTION_H;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}
void TShiftConfig::CreateWidget()
{
	m_labConfigName.Create(m_Hwnd, L"追价参数", 0, id_labconfigname);

	m_labTimeFore.Create(m_Hwnd, L"追价条件", 0, id_labtimefore);
	m_sEditTime.Create(m_Hwnd, 0, id_edittime, id_spintime);
	m_labTimeLater.Create(m_Hwnd, L"秒后触发", 0, id_labtimelater);

	m_labPrice.Create(m_Hwnd, L"追价价格", 0, id_labprice);
	m_cbPrice.Create(m_Hwnd, id_cbprice);

	m_labStop.Create(m_Hwnd, L"追价终止", 0, id_labstop);
	m_cbStop.Create(m_Hwnd, id_cbstop);

	m_ckSingleOrder.Create(m_Hwnd, id_cksingle, L"螺旋策略", 0);
	m_labStrategy.Create(m_Hwnd, L"默认策略:  每批份数根据优先腿成交量自动确定。");
	m_labStrategyA.Create(m_Hwnd, L"螺旋策略:  每批份数自定义");

	m_ckCondition.Create(m_Hwnd, id_ckcondition, L"条件单撤单", 0);
	m_sEditConditionTime.Create(m_Hwnd, 0, id_editconditiontime, id_spinconditiontime);
	m_labConditionTimeLater.Create(m_Hwnd, L"秒后，自动撤回条件单并重新添加入条件单列表");

	m_labInsureType.Create(m_Hwnd, L"投保类型", 0, id_labinsuretype);
	m_cbInsureType.Create(m_Hwnd, id_cbinsuretype);

}
void TShiftConfig::InitWidget()
{
	m_cbPrice.Clear();
	m_cbPrice.AddString(L"阶梯追价");
	m_cbPrice.AddString(L"市价");

	m_cbStop.Clear();
	m_cbStop.AddString(L"不终止");
	m_cbStop.AddString(L"1");
	m_cbStop.AddString(L"2");
	m_cbStop.AddString(L"3");
	m_cbStop.AddString(L"4");
	m_cbStop.AddString(L"5");
	m_cbStop.AddString(L"6");
	m_cbStop.AddString(L"7");
	m_cbStop.AddString(L"8");
	m_cbStop.AddString(L"9");
	m_cbStop.AddString(L"10");

	m_cbPrice.SetSelect(0);
	m_cbStop.SetSelect(0);

	m_sEditTime.SetWindowNumber(3);
	m_sEditTime.SetFont(g_FontData.GetFontNum13());
	m_sEditTime.SetRange(1, 100);

	m_sEditConditionTime.SetWindowNumber(3);
	m_sEditConditionTime.SetFont(g_FontData.GetFontNum13());
	m_sEditConditionTime.SetRange(1, 100);

	m_cbInsureType.Clear();
	m_cbInsureType.AddString(L"投机");
	m_cbInsureType.AddString(L"套保");
	m_cbInsureType.SetSelect(0);
}
void TShiftConfig::SetWidgetPos()
{
	
	int iHeight = 20;
	int ixVap = 5;
	int iyVap = 5;
	int iLeft = 15;
	int iTop = CAPTION_H + 15;
	int iRight = 0;


	int iWidth = 60;
	int icbWidth = 80;
	int ilabCommentsWidth = 250;
	POINT pt = { iLeft, iTop };
	//m_labConfigName.MoveWindow(pt.x, pt.y, iWidth, iHeight);
	
	//触发时间
	//pt.x = iLeft; 
	//pt.y += iHeight + iyVap;
	m_labTimeFore.MoveWindow(pt.x, pt.y, iWidth, iHeight);
	pt.x += iWidth + ixVap;
	m_sEditTime.MoveWindow(pt.x, pt.y, iWidth, iHeight);
	pt.x += iWidth + 3;
	m_labTimeLater.MoveWindow(pt.x, pt.y, iWidth, iHeight);
	iRight = pt.x + iWidth + ixVap * 6;

	//追价价格
	pt.x = iLeft;
	pt.y += iHeight + iyVap;
	m_labPrice.MoveWindow(pt.x, pt.y, iWidth, iHeight);
	pt.x += iWidth + ixVap;
	m_cbPrice.MoveWindow(pt.x, pt.y, icbWidth, 3*iHeight);

	//价格终止
	pt.x = iLeft;
	pt.y += iHeight + iyVap;
	m_labStop.MoveWindow(pt.x, pt.y, iWidth, iHeight);
	pt.x += iWidth + ixVap;
	m_cbStop.MoveWindow(pt.x, pt.y, icbWidth, 5*iHeight);

	//下单策略 
	pt.x = iLeft;
	pt.y += iHeight + iyVap * 3;
	m_ckSingleOrder.MoveWindow(pt.x, pt.y, icbWidth, iHeight);

	//策略注释
	pt.x = iLeft;
	pt.y += iHeight + iyVap;
	m_labStrategy.MoveWindow(pt.x, pt.y, ilabCommentsWidth, iHeight);

	pt.x = iLeft;
	pt.y += iHeight + iyVap;
	m_labStrategyA.MoveWindow(pt.x, pt.y, ilabCommentsWidth, iHeight);

	//条件单撤单设置
	pt.x = iLeft;
	pt.y += iHeight + iyVap * 2;
	m_ckCondition.MoveWindow(pt.x, pt.y, icbWidth, iHeight);
	pt.x += icbWidth + ixVap * 3;
	m_sEditConditionTime.MoveWindow(pt.x, pt.y, iWidth, iHeight);
	pt.x += iWidth;
	m_labConditionTimeLater.MoveWindow(pt.x, pt.y, ilabCommentsWidth, iHeight);

	//投保类型
	pt.x = iLeft;
	pt.y += iHeight + iyVap * 2;
	m_labInsureType.MoveWindow(pt.x, pt.y, iWidth, iHeight);
	pt.x += iWidth + ixVap + 1;
	m_cbInsureType.MoveWindow(pt.x, pt.y, icbWidth, iHeight * 3);
}
void TShiftConfig::InitRcClose()
{
	m_rcClose.right = FRAME_V - 8;
	m_rcClose.left = m_rcClose.right - closeWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + closeWidth;
}
void TShiftConfig::DrawClose(TMemDC *pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(238, 238, 238));
	HPEN hOldPen = (HPEN)SelectObject(pMemDC->GetHdc(), hPen);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.top, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right + 1, m_rcClose.bottom + 1);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.bottom, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right + 1, m_rcClose.top - 1);

	SelectObject(pMemDC->GetHdc(), hOldPen);
	DeleteObject(hPen);
	RestoreDC(pMemDC->GetHdc(), iold);
}

void TShiftConfig::SingleOrder(WPARAM wParam)
{
	int id = LOWORD(wParam);
	if (id == id_cksingle)
	{
		g_pstConfig->m_bSingle = m_ckSingleOrder.GetCheck();
		g_pShiftPosition->TotalQtyWidget(g_pstConfig->m_bSingle);
	}
	else if (id == id_ckcondition)
	{
		g_pstConfig->m_bCondition = m_ckCondition.GetCheck();
	}
}