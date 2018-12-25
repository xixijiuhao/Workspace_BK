#include "PreInclude.h"
extern bool bHidePP;
#define  closeWidth 9
const int FRAME_H = 300;
const int FRAME_V = 454;
extern LanguageID g_LanguageID;
enum {ClickOrderConfig = 0, CoverAllPriceType, _CounterPrice, _MarketPrice, PingpongQueuetime, Second, TradingBoxPrompt,
	Yes, No, RedAndGreenDueTo, PreSettle, _LastPrice,BuyAndSellDirection,BuyLeft, SellLeft, Remark,  prompt , PingpongQueueTimeUnReasonable};
char StringChs[][250] = { "点价下单配置界面", "全平价格类型", "对手价", "市价", "乒乓排队时间", "秒(10-60)", "交易弹框提示",
"是", "否", "红绿颜色对比", "昨结算", "最新价","多空左右方向","左多右空","左空右多", "乒乓排队时间：如果委托排队时间超过设定时间，系统将自动撤单。", "提示", "乒乓排队时间设置不合理" };

char StringCht[][250] = { "點價下單配置界面", "全平價格類型", "對手價", "市價", "乒乓排隊時間", "秒(10-60)", "交易彈框提示",
"是", "否", "紅綠顏色對比", "昨結算", "最新價","多空左右方向","左多右空","左空右多", "乒乓排隊時間：如果委託排隊時間超過設定時間，系統將自動撤單。", "提示", "乒乓排隊時間設置不合理" };

char StringEng[][250] = { "ClickOrder Config", "CoverAll PriceType", "CounterPrice", "MarketPrice", "Pingpong Queue time", "Second(10-60)", "Trading Box Prompt",
"Yes", "No", "Red And Green Due to","PreSettlePrice", "LastPrice",  "Buy And Sell Direction","BuyLeft", "SellLeft","Pingpong Queuing Time: If the commissioned queue time exceeds the set time, the system will automatically cancel orders.", "Prompt", "Pingpong queuing time set unreasonable" };
char* _GetPointWndString(int i)
{
	if (g_LanguageID == CHT)
		return StringCht[i];
	else if (g_LanguageID == ENU)
		return StringEng[i];
	else
		return StringChs[i];
}

void TPointConfig::Create(HWND hParent)
{
	if (!IsWindow(m_Hwnd))
	{
		m_hParent = hParent;
		CreateFrm(L"PointOrderConfig", hParent, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS);
	}
	else
	{
		if (IsWindowVisible(m_Hwnd))
			ShowWindow(m_Hwnd, SW_HIDE);
		else
			ShowWindow(m_Hwnd, SW_SHOW);
	}
}
void TPointConfig::MoveWindow(int x, int y)
{
	SetWindowPos(m_Hwnd, 0, x, y, FRAME_V, FRAME_H, SWP_NOZORDER);
	InitRcClose();
}
void TPointConfig::RegistConfig(PointOrderConfig* stConfig)
{
	if (!stConfig)
		return;
	m_pstConfig = stConfig;
}
LRESULT TPointConfig::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	default:
		return NOT_PROCESSED;
	}
}

void TPointConfig::OnPaint()
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
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord13());
	SetTextColor(memdc.GetHdc(), RGB(238, 238, 238));
	DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(_GetPointWndString(ClickOrderConfig)).c_str(), -1, &rcCaption, DT_LEFT | DT_VCENTER | DT_SINGLELINE);


	SetTextColor(memdc.GetHdc(), RGB(0, 0, 0));
	RECT rcRemark = { 15, 250, 440, 290 };
	DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(_GetPointWndString(Remark)).c_str(), -1, &rcRemark, DT_LEFT | DT_WORDBREAK);
	DrawClose(&memdc);


	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}
void TPointConfig::OnCreate()
{
	CreateWidget();
	InitWidget();
	SetWidgetPos();
	if (bHidePP) {
		HideSomeConfig();
	}
}
void TPointConfig::OnSize(WPARAM wParam, LPARAM lParam)
{

}
LRESULT TPointConfig::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_COMBUYSELLDIRECTION:
		if (HIWORD(wParam) == CBN_SELCHANGE) {
			OnDealBuySellDirection();
		}
		break;
	case ID_COMPRICETYPE:
		OnDealPriceType();
		break;
	case ID_COMMESSAGEBOX:
		OnDealMessageBox();
		break;
	case ID_COMCOMPAREPRE:
		OnDealComparePrice();
		break;
	case ID_EDITSECOND:
		if (HIWORD(wParam) == EN_CHANGE)
		{
		}
		else if (HIWORD(wParam) == EN_KILLFOCUS)
		{
			char szText[20] = { 0 };
			GetWindowTextA(m_editTime.GetHwnd(), szText, sizeof(szText));
			int iQueueTime = atoi(szText);		
			m_pstConfig->stPingPang.iQueueTime = iQueueTime;
		}
		break;
	default:
		break;
	}
	return NOT_PROCESSED;
}
void TPointConfig::OnDealMessageBox()
{
	int index = SendMessage(m_cbMessageBox.GetHwnd(), CB_GETCURSEL, 0, 0);
	if (index != CB_ERR)
		m_pstConfig->iMessageBox = index;
}
void TPointConfig::OnDealComparePrice()
{
	int index = SendMessage(m_cbComparePrice.GetHwnd(), CB_GETCURSEL, 0, 0);
	if (index != CB_ERR)
		m_pstConfig->iComparePre = index;
}
void TPointConfig::OnDealBuySellDirection()
{
	int index = SendMessage(m_cbBuySellDirection.GetHwnd(), CB_GETCURSEL, 0, 0);
	if (index != CB_ERR) {
		m_pstConfig->iBuySellDirection = index;
		PostMessage(m_hParent, WM_CHANGEBUYSELLDIRECTION, 0, 0);
	}
}
void TPointConfig::OnDealPriceType()
{
	int index = SendMessage(m_cbCoverType.GetHwnd(), CB_GETCURSEL, 0, 0);
	if (index != CB_ERR)
		m_pstConfig->iCoverAllPriceType = index;
}
LRESULT TPointConfig::OnCtrlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_white;
}
void TPointConfig::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.bottom = rect.top + CAPTION_H;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		char szText[20] = { 0 };
		GetWindowTextA(m_editTime.GetHwnd(), szText, sizeof(szText));
		int iQueueTime = atoi(szText);
		if (iQueueTime > 60 || iQueueTime < 10)
		{
			_TMessageBox_Domodal(m_Hwnd, _GetPointWndString(prompt), (wchar_t*)LoadRC::ansi_to_unicode(_GetPointWndString(PingpongQueueTimeUnReasonable)).c_str(), TMB_OK);
			return;
		}
		ShowWindow(m_Hwnd, SW_HIDE);
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}
void TPointConfig::CreateWidget()
{
	m_scCoverType.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetPointWndString(CoverAllPriceType)).c_str());
	m_scQueueTime.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetPointWndString(PingpongQueuetime)).c_str());
	m_scSecond.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetPointWndString(Second)).c_str());
	//m_scExQueueTime.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetPointWndString(Remark)).c_str());
	m_scMessageBox.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetPointWndString(TradingBoxPrompt)).c_str());
	m_scComparePrice.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetPointWndString(RedAndGreenDueTo)).c_str());
	m_scBuySellDirection.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetPointWndString(BuyAndSellDirection)).c_str());




	HWND hedit = CreateWindowEx(0, WC_EDIT, TEXT(""), ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NUMBER | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS,
		0, 0, 0, 0, m_Hwnd, (HMENU)ID_EDITSECOND, GetModuleHandle(NULL), NULL);
	HWND hSpin = CreateWindowEx(0, UPDOWN_CLASS, TEXT(""), WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | WS_CLIPSIBLINGS,
		0, 0, 0, 0, m_Hwnd, (HMENU)ID_SPINSECOND, GetModuleHandle(NULL), NULL);
	SendMessage(hSpin, UDM_SETBUDDY, (WPARAM)hedit, 0);
	SendMessage(hSpin, UDM_SETRANGE32, 0, (LPARAM)MAKELPARAM(65535, 0));
	SendMessage(hSpin, UDM_SETBASE, 10, 0);
	SendMessage(hedit, EM_SETLIMITTEXT, 10, 0);
	SendMessage(hSpin, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)60, (short)10));
	m_spinTime.Init(hSpin);
	m_editTime.Init(hedit);

	HWND hcb = CreateWindowEx(0, WC_COMBOBOX, TEXT(""), WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST  | CBS_HASSTRINGS | WS_VSCROLL | WS_CLIPSIBLINGS | WS_TABSTOP,
		0, 0, 80, 80, m_Hwnd, (HMENU)ID_COMPRICETYPE, GetModuleHandle(NULL), NULL);
	m_cbCoverType.Init(hcb);

	hcb = CreateWindowEx(0, WC_COMBOBOX, TEXT(""), WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL | WS_CLIPSIBLINGS | WS_TABSTOP,
		0, 0, 80, 80, m_Hwnd, (HMENU)ID_COMMESSAGEBOX, GetModuleHandle(NULL), NULL);
	m_cbMessageBox.Init(hcb);


	hcb = CreateWindowEx(0, WC_COMBOBOX, TEXT(""), WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL | WS_CLIPSIBLINGS | WS_TABSTOP,
		0, 0, 80, 80, m_Hwnd, (HMENU)ID_COMCOMPAREPRE, GetModuleHandle(NULL), NULL);
	m_cbComparePrice.Init(hcb);

	hcb = CreateWindowEx(0, WC_COMBOBOX, TEXT(""), WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL | WS_CLIPSIBLINGS | WS_TABSTOP,
		0, 0, 80, 80, m_Hwnd, (HMENU)ID_COMBUYSELLDIRECTION, GetModuleHandle(NULL), NULL);
	m_cbBuySellDirection.Init(hcb);
}
void TPointConfig::InitWidget()
{
	m_scCoverType.SetFont(g_FontData.GetFontWord13());
	m_scQueueTime.SetFont(g_FontData.GetFontWord13());
	m_scSecond.SetFont(g_FontData.GetFontWord13());
	//m_scExQueueTime.SetFont(g_FontData.GetFontWord13());
	m_scMessageBox.SetFont(g_FontData.GetFontWord13());
	m_scComparePrice.SetFont(g_FontData.GetFontWord13());
	m_scBuySellDirection.SetFont(g_FontData.GetFontWord13());


	SendMessage(m_editTime.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), TRUE);
	SendMessage(m_cbCoverType.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), TRUE);
	SendMessage(m_cbMessageBox.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), TRUE);
	SendMessage(m_cbComparePrice.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), TRUE);
	SendMessage(m_cbBuySellDirection.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), TRUE);



	ComBoxStringMsgOperAnsic(m_cbCoverType.GetHwnd(), CB_ADDSTRING, 0, _GetPointWndString(_CounterPrice));
	ComBoxStringMsgOperAnsic(m_cbCoverType.GetHwnd(), CB_ADDSTRING, 0, _GetPointWndString(_MarketPrice));

	ComBoxStringMsgOperAnsic(m_cbMessageBox.GetHwnd(), CB_ADDSTRING, 0, _GetPointWndString(No));
	ComBoxStringMsgOperAnsic(m_cbMessageBox.GetHwnd(), CB_ADDSTRING, 0, _GetPointWndString(Yes));

	ComBoxStringMsgOperAnsic(m_cbComparePrice.GetHwnd(), CB_ADDSTRING, 0, _GetPointWndString(PreSettle));
	ComBoxStringMsgOperAnsic(m_cbComparePrice.GetHwnd(), CB_ADDSTRING, 0, _GetPointWndString(_LastPrice));

	ComBoxStringMsgOperAnsic(m_cbBuySellDirection.GetHwnd(), CB_ADDSTRING, 0, _GetPointWndString(BuyLeft));
	ComBoxStringMsgOperAnsic(m_cbBuySellDirection.GetHwnd(), CB_ADDSTRING, 0, _GetPointWndString(SellLeft));


	//根绝配置结构体设置默认值;
	char szText[50] = { 0 };
	sprintf_s(szText, "%d", m_pstConfig->stPingPang.iQueueTime);
	SetWindowTextA(m_editTime.GetHwnd(), szText);
	SendMessage(m_cbCoverType.GetHwnd(), CB_SETCURSEL, (WPARAM)m_pstConfig->iCoverAllPriceType, 0);
	SendMessage(m_cbMessageBox.GetHwnd(), CB_SETCURSEL, (WPARAM)m_pstConfig->iMessageBox, 0);
	SendMessage(m_cbComparePrice.GetHwnd(), CB_SETCURSEL, (WPARAM)m_pstConfig->iComparePre, 0);
	SendMessage(m_cbBuySellDirection.GetHwnd(), CB_SETCURSEL, (WPARAM)m_pstConfig->iBuySellDirection, 0);

	//SetWindowLong(m_scExQueueTime.GetHwnd(), GWL_STYLE, GetWindowLong(m_scExQueueTime.GetHwnd(), GWL_STYLE) & ~SS_CENTER);
	SetWindowLong(m_scCoverType.GetHwnd(), GWL_STYLE, GetWindowLong(m_scCoverType.GetHwnd(), GWL_STYLE) & ~SS_CENTER);
	SetWindowLong(m_scSecond.GetHwnd(), GWL_STYLE, GetWindowLong(m_scSecond.GetHwnd(), GWL_STYLE) & ~SS_CENTER);
	SetWindowLong(m_scQueueTime.GetHwnd(), GWL_STYLE, GetWindowLong(m_scQueueTime.GetHwnd(), GWL_STYLE) & ~SS_CENTER);
	SetWindowLong(m_scMessageBox.GetHwnd(), GWL_STYLE, GetWindowLong(m_scMessageBox.GetHwnd(), GWL_STYLE) & ~SS_CENTER);
	SetWindowLong(m_scComparePrice.GetHwnd(), GWL_STYLE, GetWindowLong(m_scComparePrice.GetHwnd(), GWL_STYLE) & ~SS_CENTER);
	SetWindowLong(m_scBuySellDirection.GetHwnd(), GWL_STYLE, GetWindowLong(m_scBuySellDirection.GetHwnd(), GWL_STYLE) & ~SS_CENTER);
}
void TPointConfig::HideSomeConfig()
{
	ShowWindow(m_scComparePrice.GetHwnd(), SW_HIDE);
	ShowWindow(m_scSecond.GetHwnd(), SW_HIDE);
	ShowWindow(m_scQueueTime.GetHwnd(), SW_HIDE);
	ShowWindow(m_scExQueueTime.GetHwnd(), SW_HIDE);
	ShowWindow(m_spinTime.GetHwnd(), SW_HIDE);
	ShowWindow(m_cbComparePrice.GetHwnd(), SW_HIDE);
	ShowWindow(m_editTime.GetHwnd(), SW_HIDE);
}
void TPointConfig::SetWidgetPos()
{
	const int ileft = 15;
	const int itop = 35;
	const int iHeight = 20;
	const int ixVap = 10;
	const int iyVap = 5;
	
	const int iEditWidth = 32;
	const int iSpinWidth = 17;

	int iCbWidth;
	int iScWidth;
	if (g_LanguageID == ENU )
	{
		iCbWidth = 100;
		iScWidth = 150;
	}
	else if (CHT == g_LanguageID)
	{
		iCbWidth = 80;
		iScWidth = 100;
	}
	else
	{
		iCbWidth = 80;
		iScWidth = 100;
	}

	POINT pst = { ileft, itop };
	m_scCoverType.MoveWindow(pst.x, pst.y, iScWidth, iHeight);

	pst.x += iScWidth + ixVap;
	SetWindowPos(m_cbCoverType.GetHwnd(), 0, pst.x, pst.y, iCbWidth, 5*iHeight, 0);

	if (!bHidePP) {
		pst.x = ileft;
		pst.y += iHeight + iyVap;
		m_scQueueTime.MoveWindow(pst.x, pst.y, iScWidth, iHeight);
		pst.x += iScWidth + ixVap;
		SetWindowPos(m_editTime.GetHwnd(), 0, pst.x, pst.y, iEditWidth, iHeight, 0);
		pst.x += iEditWidth + 1;
		SetWindowPos(m_spinTime.GetHwnd(), 0, pst.x, pst.y, iSpinWidth, iHeight, 0);
		pst.x += iSpinWidth + 4;
		m_scSecond.MoveWindow(pst.x, pst.y, iScWidth, iHeight);
	}


	pst.x = ileft;
	pst.y += iHeight + iyVap;
	SetWindowPos(m_scMessageBox.GetHwnd(), 0, pst.x, pst.y, iScWidth, iHeight, 0);
	pst.x += iScWidth + ixVap;
	SetWindowPos(m_cbMessageBox.GetHwnd(), 0, pst.x, pst.y, iCbWidth, iHeight, 0);

	if (!bHidePP) {
		pst.x = ileft;
		pst.y += iHeight + iyVap;
		SetWindowPos(m_scComparePrice.GetHwnd(), 0, pst.x, pst.y, iScWidth, iHeight, 0);
		pst.x += iScWidth + ixVap;
		SetWindowPos(m_cbComparePrice.GetHwnd(), 0, pst.x, pst.y, iCbWidth, iHeight, 0);
	}


	pst.x = ileft;
	pst.y += iHeight + iyVap;
	SetWindowPos(m_scBuySellDirection.GetHwnd(), 0, pst.x, pst.y, iScWidth, iHeight, 0);
	pst.x += iScWidth + ixVap;
	SetWindowPos(m_cbBuySellDirection.GetHwnd(), 0, pst.x, pst.y, iCbWidth, iHeight, 0);

	pst.x = ileft;
	pst.y = FRAME_H - 50;
	//m_scExQueueTime.MoveWindow(pst.x, pst.y, FRAME_V - 2*ileft, iHeight);
}
void TPointConfig::InitRcClose()
{
	m_rcClose.right = FRAME_V - 8;
	m_rcClose.left = m_rcClose.right - closeWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + closeWidth;
}
void TPointConfig::DrawClose(TMemDC *pMemDC)
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