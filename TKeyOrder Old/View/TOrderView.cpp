#include "BaseInclude.h"

#define frmWidth   360
#define frmHeight  180
#define widgetTop  3

extern bool g_MouseTrack;


char szChs[][10] = { "价格▼", "撤单", "对价跟", "排队跟", "连续追", "买多", "卖空", "平仓" };
char szCht[][10] = { "價格▼", "撤單", "對價跟", "排隊跟", "連續追", "買多", "賣空", "平倉" };
char szEnu[][20] = { "Price▼", "Cancel", "Matching Follow", "Working Follow", "Continous Chase", "Buy", "Sell", "Close" };
TCHAR szChsW[][10] = { L"品种名", L"手数", L"涨板", L"跌板"};
TCHAR szChtW[][10] = { L"品種名", L"手數", L"漲板", L"跌板" };
TCHAR szEnuW[][20] = { L"Product", L"Qty", L"Limit Up", L"Limit Down" };


enum{
	Price_Index,
	Cancel_Index,
	CounterFollow_Index,
	QueueFollow_Index,
	ContiueChase_Index,
	Buy_Index,
	Sell_Index,
	Close_Index,
};
enum
{
	CommodityName_Index,
	Qty_Index,
	LimitUp_Index,
	LimitDown_Index,
};
char * _GetTextOV(int i)
{
	if (g_language == CHT)return szCht[i];
	else if (g_language == ENU)return szEnu[i];
	return szChs[i];
}
TCHAR * _GetTextOVW(int i)
{
	if (g_language == CHT)return szChtW[i];
	else if (g_language == ENU)return szEnuW[i];
	return szChsW[i];
}
TOrderView::TOrderView(IMediator* pMediator, HWND hParent) :IColleague(pMediator)
{
	m_hParent = hParent;
	m_bShow = false;

	m_pOrderData = NULL;
	m_pQuoteData = NULL;
	m_bShowCodeList = true;
	m_bChangeWithQuote = true;

	m_iPriceType = ptCounterPrice;
	char szText[20]{};
	TStopOrderPrice2Char(ptCounterPrice, szText, sizeof(szText));
	m_wstrPriceTypeText = LoadRC::ansi_to_unicode(szText);
}

TOrderView::~TOrderView()
{

}

void TOrderView::UpdateView()
{
	SetWidgetPos();
}
void TOrderView::ShowFrm(int formerIndex, int nowIndex)
{
	if (!m_Hwnd)
		CreateFrm(L"TPlaceOrderFrame", m_hParent, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	UINT show = SWP_HIDEWINDOW;
	if (formerIndex == ID_frmTKeyOrder)
	{
		show = SWP_HIDEWINDOW;
		m_bShow = false;
	}
	else if (nowIndex == ID_frmTKeyOrder)
	{
		show = SWP_SHOWWINDOW;
		m_bShow = true;
	}
	SetWindowPos(m_Hwnd, 0, m_ptShow.x, m_ptShow.y, m_iFrmWidth, m_iFrmHeight, show);
}


void TOrderView::UpdateShowData()
{
	if (!m_pOrderData) return;
	m_btnLongBuyOpen.SetActionText(m_pOrderData->szBuyBtnText);
	m_btnShortSellOpen.SetActionText(m_pOrderData->szSellBtnText);
	m_btnClose.SetActionText(m_pOrderData->szCoverBtnText);

	m_bShowCodeList = false;
	m_editContractNo.SetText(m_pOrderData->szContractNo);
	m_bShowCodeList = true;

	m_spinPrice.SetText(m_wstrPriceTypeText.c_str());
	if (!m_pOrderData->SetCoverPrice)
		m_btnClose.SetPrice(m_pOrderData->szCoverPrice);

	if (m_pOrderData->OrderQty!=0)
		m_spinLots.SetWindowNumber(m_pOrderData->OrderQty);

	if (m_pOrderData->Contract)
	{
		const SCommodity* pCommodity = g_pTradeData->GetCommodity(&m_pOrderData->ContractKey);
		if (pCommodity)
		{

			m_labContractNo.SetText(CConvert::UTF8ToUnicode(pCommodity->CommodityName).c_str());
		}
	}

	m_bChangeWithQuote = true;

	SetLabSuperPriceText();
}
void TOrderView::UpdatePrice()
{
	if (!m_pCommodityInfo || !m_pQuoteData) return;
	char szText[100] = { 0 };
	DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->UpPrice, szText);
	m_btnUpPrice.SetButtonText(szText);
	DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->DownPrice, szText);
	m_btnDownPrice.SetButtonText(szText);

	if (!m_bChangeWithQuote) return;

	double dBuyPrice = 0;
	double dSellPrice = 0;
	GetPrice(dBuyPrice, dSellPrice);
	DoubleToChar(m_pCommodityInfo->iPrecision, dBuyPrice, szText);
	m_btnLongBuyOpen.SetPrice(szText);
	char szSellText[50] = { 0};
	DoubleToChar(m_pCommodityInfo->iPrecision, dSellPrice, szSellText);
	m_btnShortSellOpen.SetPrice(szSellText);

	if (m_pOrderData->SetCoverPrice)
	{
		if (m_pOrderData->TCoverDirect == dBuy)
			m_btnClose.SetPrice(szSellText);
		else if (m_pOrderData->TCoverDirect == dSell)
			m_btnClose.SetPrice(szText);
	}

}
void TOrderView::GetPrice(double &dBuyPrice, double &dSellPrice)
{
	if (!m_pQuoteData) return;
	dBuyPrice = 0.0;
	dSellPrice = 0.0;
	switch (m_iPriceType)
	{
	case ptQueuePrice:
		dBuyPrice = m_pQuoteData->BuyPrice;
		dSellPrice = m_pQuoteData->SellPrice;
		break;
	case ptCounterPrice:
		dSellPrice = m_pQuoteData->BuyPrice;
		dBuyPrice = m_pQuoteData->SellPrice;
		break;
	case ptSuperPrice:
		dBuyPrice = m_pQuoteData->SellPrice + m_pOrderData->iBuyOffset*m_pCommodityInfo->dMiniChangePrice;
		dSellPrice = m_pQuoteData->BuyPrice + m_pOrderData->iSellOffset*m_pCommodityInfo->dMiniChangePrice;
		break;
	case ptMarketPrice:
		dBuyPrice = m_pQuoteData->UpPrice;
		dSellPrice = m_pQuoteData->DownPrice;
		break;
	case ptLastPrice:
		dBuyPrice = m_pQuoteData->LastPrice;
		dSellPrice = m_pQuoteData->LastPrice;
		break;
	}
}
void TOrderView::SetShowRect(RECT rect)
{
	m_ptShow.x = rect.left;
	m_ptShow.y = rect.top;

	m_iFrmWidth = rect.right - rect.left;
	if (m_iFrmWidth > frmWidth)
		m_iFrmWidth = frmWidth;

	m_iFrmHeight = rect.bottom - rect.top;
	if (m_iFrmHeight > frmHeight)
		m_iFrmHeight = frmHeight;

	if (m_bShow)
	{
		ShowFrm(-1, 0);
	}
}

LRESULT TOrderView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case SSWM_ORDERBUTTON_UP:
		OnOrderBtn(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtrlColor(wParam, lParam);
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnClickBtn(wParam, lParam);
		return PROCESSED;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case SSWM_EDIT_PRESS_ENTER:
		if ((HWND)lParam == m_editContractNo.GetHwnd())
			m_popupCodeList.OnReturn();
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
void TOrderView::OnNotify(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == ID_SpinPrice)
	{
		LPNMUPDOWN pPriceUpDwon = (LPNMUPDOWN)lParam;
		char Price[20] = { 0 };
		m_bChangeWithQuote = false;
		if (m_spinPrice.IsText())
		{
			if (pPriceUpDwon->iDelta > 0)
			{
				DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->SellPrice, Price); 
			}
			else
			{
				DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->BuyPrice, Price);
			}
			m_spinPrice.SetText(Price);
		}
		else
		{
			double dNow = m_spinPrice.GetDoubleData();
			if (pPriceUpDwon->iDelta > 0)
				dNow += m_pCommodityInfo->dMiniChangePrice;
			else
				dNow -= m_pCommodityInfo->dMiniChangePrice;
			DoubleToChar(m_pCommodityInfo->iPrecision, dNow, Price);
			m_spinPrice.SetText(Price);
		}
		SetFocus(m_spinPrice.GetEditHwnd());
		PostMessage(m_spinPrice.GetEditHwnd(), EM_SETSEL, 0, -1);
		SetButtonSamePrice(Price);
	}
}
void TOrderView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == m_editContractNo.GetHwnd())
	{
		if (m_bShowCodeList && HIWORD(wParam) == EN_CHANGE)
		{
			if (!IsWindow(m_popupCodeList.GetHwnd()))
			{
				DWORD dwStyle = WS_CLIPSIBLINGS /*| WS_CHILD*/ | WS_BORDER | WS_POPUP;
				DWORD dwStyleeEx = WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE;
				m_popupCodeList.CreatePopWin(this, L"popupcodeList", m_Hwnd, dwStyle, dwStyleeEx, 0, 0, 150);
				m_popupCodeList.UpdateData(g_pGlobalModel->m_strSign.c_str());
			}
			POINT pt = m_ptSearchCode;
			ClientToScreen(m_Hwnd, &pt);
			char sCode[100] = { 0 };
			m_editContractNo.GetText(sCode, sizeof(sCode));
			if (!IsWindowVisible(m_popupCodeList.GetHwnd()))
			{
				SetWindowPos(m_popupCodeList.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			}
			m_popupCodeList.EditChange(sCode, g_pGlobalModel->m_strSign);
		}
		else if (HIWORD(wParam) == EN_KILLFOCUS)
		{
			if (IsWindowVisible(m_popupCodeList.GetHwnd()) && g_MouseTrack)
				ShowWindow(m_popupCodeList.GetHwnd(), SW_HIDE);
		}
	}
	else if ((HWND)lParam == m_spinPrice.GetEditHwnd())
	{
		if (m_spinPrice.IsText() && HIWORD(wParam) == EN_SETFOCUS)
		{
			char Price[20] = { 0 };
			DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->LastPrice, Price);
			m_spinPrice.SetText(Price);
			SetButtonSamePrice(Price);

			SetLabSuperPriceText(false);//清楚super price lab上的字

			SetFocus(m_spinPrice.GetEditHwnd());
			PostMessage(m_spinPrice.GetEditHwnd(), EM_SETSEL, 0, strlen(Price));
		}
		else if (!m_spinPrice.IsText() && HIWORD(wParam) == EN_CHANGE)
		{
			char Price[20] = { 0 };
			m_spinPrice.GetText(Price, sizeof(Price));

			SetButtonSamePrice(Price);

			SetLabSuperPriceText(false);//清楚super price lab上的字
		}
	}
	else if ((HWND)lParam == m_labelUp.GetHwnd() && HIWORD(wParam) == STN_CLICKED)
	{
		OnbtnUpDownPrice(m_btnUpPrice.GetHwnd());
	}
	else if ((HWND)lParam == m_labelDown.GetHwnd()&& HIWORD(wParam) == STN_CLICKED)
	{
		OnbtnUpDownPrice(m_btnDownPrice.GetHwnd());
	}
	else if ((HWND)lParam == m_lockContract.GetHwnd())
	{
		bool bLock = m_lockContract.GetValue();
		EnableWindow(m_editContractNo.GetHwnd(), !bLock);
		int lockState = bLock ? 1 : 0;
		m_Mediator->Send(CMD_OrderView_LockState, (const void*)lockState, this);
	}
	else if ((HWND)lParam == m_spinLots.GetEditHwnd() && HIWORD(wParam) == EN_CHANGE)
	{
		int ret = m_spinLots.GetWindowNumber();
		if (ret == 0)
			m_spinLots.SetText(L"1");
		if (g_pGlobalModel->m_utUserType == utForeign)
		{
			if (g_language == ENU)return;
			int diff = 0;
			if (m_pOrderData->iBuyPosQty)
			{
				if (m_pOrderData->iBuyPosQty < ret)
				{
					diff = ret - m_pOrderData->iBuyPosQty;
					sprintf_s(m_pOrderData->szSellBtnText, "平%d反%d", m_pOrderData->iBuyPosQty, diff);
				}
				else
				{
					strcpy_s(m_pOrderData->szSellBtnText, _GetTextOV(Close_Index));
				}
				m_btnShortSellOpen.SetActionText(m_pOrderData->szSellBtnText);
			}
			else if (m_pOrderData->iSellPosQty)
			{
				if (m_pOrderData->iSellPosQty < ret)
				{
					diff = ret - m_pOrderData->iSellPosQty;
					sprintf_s(m_pOrderData->szBuyBtnText, "平%d反%d", m_pOrderData->iSellPosQty, diff);
				}
				else
				{
					strcpy_s(m_pOrderData->szBuyBtnText, _GetTextOV(Close_Index));
				}
				m_btnLongBuyOpen.SetActionText(m_pOrderData->szBuyBtnText);
			}
		}
	}
}
void TOrderView::SetButtonSamePrice(const char * szPrice)
{
	m_bChangeWithQuote = false;
	m_btnLongBuyOpen.SetPrice(szPrice);
	m_btnShortSellOpen.SetPrice(szPrice);
    if (!m_pOrderData)
        return;
	if (m_pOrderData->SetCoverPrice)
		m_btnClose.SetPrice(szPrice);
}
void TOrderView::OnOrderBtn(WPARAM wParam, LPARAM lParam)
{
	TSendOrder stOrder;
	CConvert::DefaultSendOrder(stOrder);
	stOrder.OrderQty = m_spinLots.GetWindowNumber();
	switch (lParam)
	{
	case ID_LongBuyOpen:
		stOrder.Direct=dBuy;
		stOrder.Offset = oOpen;
		stOrder.OrderPrice = m_btnLongBuyOpen.GetPrice();
		break;
	case ID_ShorSellOpen:
		stOrder.Direct=dSell;
		stOrder.Offset = oOpen;
		stOrder.OrderPrice = m_btnShortSellOpen.GetPrice();
		break;
	case ID_Close:
		if (m_pOrderData->SetCoverPrice)
		{
			stOrder.OrderPrice = m_btnClose.GetPrice();
			stOrder.Offset = oCover;
		}
		break;
	}

	m_Mediator->Send(CMD_OrderView_InsertOrder, &stOrder, this);
}

void TOrderView::OnCreate()
{
    m_Mediator->Get(CMD_OrderView_GetOrderData, (void*&)m_pOrderData, this);
    m_Mediator->Get(CMD_OrderView_GetQuoteData, (void*&)m_pQuoteData, this);
    m_Mediator->Get(CMD_OrderView_GetCommInfo, (void*&)m_pCommodityInfo, this);
	CreateWidget();
	InitWidget();
	SetWidgetPos();


}



void TOrderView::CreateWidget()
{
	m_labContractNo.Create(m_Hwnd, _GetTextOVW(CommodityName_Index));
	m_lockContract.Create(m_Hwnd,IDI_LOCK,IDI_UNLOCK);
	m_editContractNo.Create(m_Hwnd, 0, ES_UPPERCASE | ES_AUTOHSCROLL, ID_editContractNo);
	m_btnSearch.Create(m_Hwnd, "···", ID_BtnSearch);

	m_labLots.Create(m_Hwnd, _GetTextOVW(Qty_Index));
	m_spinLots.Create(m_Hwnd,ES_RIGHT,0,ID_SpinLots,CxEdit::intStyle);

	m_btnPriceType.Create(m_Hwnd,_GetTextOV(Price_Index),ID_BtnPriceType);
	m_spinPrice.Create(m_Hwnd, 0, ID_editPriceType, ID_SpinPrice,CxEdit::floatStyle,false);

	m_labelUp.Create(m_Hwnd, _GetTextOVW(LimitUp_Index));
	m_labelDown.Create(m_Hwnd, _GetTextOVW(LimitDown_Index));

	m_btnUpPrice.Create(m_Hwnd,"",ID_BtnUpPrice);
	m_btnDownPrice.Create(m_Hwnd, "", ID_BtnDownPrice);


	m_labSuperPrice.Create(m_Hwnd, L"");

	m_btnLongBuyOpen.Create(m_Hwnd,ID_LongBuyOpen); 
	m_btnShortSellOpen.Create(m_Hwnd,ID_ShorSellOpen);
	m_btnClose.Create(m_Hwnd, ID_Close); 


	m_btnCancel.Create(m_Hwnd, _GetTextOV(Cancel_Index), ID_CancelOrder);
	m_btnConsideration.Create(m_Hwnd, _GetTextOV(CounterFollow_Index), ID_Consideration);
	m_btnLineup.Create(m_Hwnd, _GetTextOV(QueueFollow_Index), ID_LineUp);
	m_btnChase.Create(m_Hwnd, _GetTextOV(ContiueChase_Index), ID_Chase);
}

void TOrderView::InitWidget()
{
	m_btnLongBuyOpen.SetColor(RGB(215, 227, 241), RGB(230, 0, 0), RGB(14, 166, 217), RGB(230, 0, 0));
	m_btnLongBuyOpen.SetActionText(_GetTextOV(Buy_Index));
	m_btnShortSellOpen.SetColor(RGB(215, 227, 241), RGB(0, 160, 0), RGB(14, 166, 217), RGB(0, 160, 0));
	m_btnShortSellOpen.SetActionText(_GetTextOV(Sell_Index));
	m_btnClose.SetColor(RGB(215, 227, 241), RGB(0, 0, 180), RGB(14, 166, 217), RGB(0, 0, 180));
	m_btnClose.SetActionText(_GetTextOV(Close_Index));

	m_btnSearch.SetFont(g_FontData.g_FontNum15);
	m_spinLots.SetFont(g_FontData.g_FontNum15);
	m_spinLots.SetWindowNumber(1);
	m_spinLots.SetRange(1, 1000000);
	m_spinPrice.SetFont(g_FontData.g_FontNum15);

	m_labContractNo.SetFont(g_FontData.g_FontWord12);

	m_spinPrice.SetText(m_wstrPriceTypeText.c_str());
	m_btnClose.SetActionText(_GetTextOV(Close_Index));
	m_editContractNo.SetFont(g_FontData.g_FontNum15);

	SetBtnFormat(&m_btnUpPrice);
	SetBtnFormat(&m_btnDownPrice);
	SetBtnFormat(&m_btnPriceType);
}
void TOrderView::SetBtnFormat(TStaticButton *pBtn)
{
	pBtn->SetbtnCursorStyle(IDC_HAND);
	pBtn->SetFont(g_FontData.g_FontWord12);
	pBtn->SetDTstyle(DT_LEFT);
	pBtn->SetFrameColor(g_ColorRefData.g_ColorWhite);
	pBtn->SetBkColor(g_ColorRefData.g_ColorBackground);
}
void TOrderView::SetWidgetPos()
{

	int left = 7;
	int top = widgetTop + 20;
	int width = 75;
	int height = 22;

	m_labContractNo.MoveWindow(left, widgetTop, 55, 16);
	m_editContractNo.MoveWindow(left, top, width, height);
	m_ptSearchCode.x = left;
	m_ptSearchCode.y = top + height;
	left +=width + 4;
	width = 21;
	m_lockContract.MoveWindow(left, widgetTop, 16, 16);
	m_btnSearch.MoveWindow(left, top, width, height);

	left +=width + 4;
	width = 60;
	m_spinLots.MoveWindow(left, top, width, height);
	m_labLots.MoveWindow(left, widgetTop, 30, 17);

	left +=width + 8;
	width = 78;
	m_spinPrice.MoveWindow(left, top, width, height);
	m_btnPriceType.MoveWindow(left, widgetTop, 40, 18);
	m_labSuperPrice.MoveWindow(left, top +height, width, 16);

	m_ptPopupPriceType.x = left;
	m_ptPopupPriceType.y = top + height;

	int updownPriceGap = 15;
	int btnlabGap = 10;
	int updownLabWidth = 26;
	if (g_language == ENU)
	{
		updownPriceGap = 10;
		btnlabGap = 2;
		updownLabWidth = 50;
	}
	left += width + updownPriceGap;
	width = 35;
	top = widgetTop + 8;
	height = 17;
	m_btnUpPrice.MoveWindow(left, top, width, height);
	m_labelUp.MoveWindow(left + width + btnlabGap, top, updownLabWidth, 16);
	top += 22;
	m_btnDownPrice.MoveWindow(left, top, width, height);
	m_labelDown.MoveWindow(left + width + btnlabGap, top, updownLabWidth, 16);

	m_iOrderBtnLeft = left;
	m_iOrderBtnTop = top;
	SetOrderBtnPos(left, top);

	int btnLeft = 65;
	width = 60;
	int gap = 10;
	int btnCancelWidth = 60;
	if (g_language == ENU)
	{
		btnLeft = 30;
		width = 80;
		gap = 5;
	}
	top += 15;
	left = btnLeft;
	height = 19;
	m_btnCancel.MoveWindow(left, top, btnCancelWidth, height);
	left += btnCancelWidth + gap;
	m_btnConsideration.MoveWindow(left, top, width, height);
	left += width + gap;
	m_btnLineup.MoveWindow(left, top, width, height);
	left += width + gap;
	m_btnChase.MoveWindow(left, top, width, height);
}
void TOrderView::SetOrderBtnPos(int left, int &top)
{
	top += 30;
	int height = 60;
	int width = 93;
	left = 26;
	int gap = 15;
	if (g_pGlobalModel->m_utUserType == utForeign)
	{
		width = 120;
		ShowWindow(m_btnClose.GetHwnd(), SW_HIDE);
	}
	m_btnLongBuyOpen.MoveWindow(left, top, width, height);
	left += width + gap;
	m_btnShortSellOpen.MoveWindow(left, top, width, height);
	if (g_pGlobalModel->m_utUserType != utForeign)
	{
		left += width + gap;
		m_btnClose.MoveWindow(left, top, width, height);
		ShowWindow(m_btnClose.GetHwnd(), SW_SHOW);
	}
	top += height;
}
void TOrderView::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
	if (rect.right < frmWidth)
		rect.right = frmWidth;
	FrameRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_gray);
}

LRESULT TOrderView::OnCtrlColor(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == m_labelUp.GetHwnd())
	{
		SetTextColor((HDC)wParam, RGB(240, 0, 0));
	}
	else if ((HWND)lParam == m_labelDown.GetHwnd())
	{
		SetTextColor((HDC)wParam, RGB(0, 200, 0));
	}
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_bk;
}

void TOrderView::OnClickBtn(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnPriceType:
		OnPriceType();
		break;
	case ID_BtnSearch:
		OnSearchCode();
		break;
	case ID_BtnUpPrice:
		OnbtnUpDownPrice(m_btnUpPrice.GetHwnd());
		break;
	case ID_BtnDownPrice:
		OnbtnUpDownPrice(m_btnDownPrice.GetHwnd());
		break;
	case ID_CancelOrder:
	case ID_Consideration:
	case ID_LineUp:
	case ID_Chase:
		m_Mediator->Send(CMD_OrderView_ClickBtn, (void*)lParam, this);
		break;
	default:
		break;
	}
}
void TOrderView::OnbtnUpDownPrice(HWND hwnd)
{
	m_bChangeWithQuote = false;
	char Temp[20] = { 0 };
	GetWindowTextA(hwnd, Temp, sizeof(Temp));
	SetButtonSamePrice(Temp);
	m_spinPrice.SetText(Temp);
	SetFocus(m_spinPrice.GetEditHwnd());
	PostMessage(m_spinPrice.GetEditHwnd(), EM_SETSEL, 0, -1);
}

void TOrderView::OnPriceType()
{
	if (m_popupPriceType.GetHwnd() == NULL)
	{
		vector<wstring> vPriceType;
		char szText[50] = { 0 };
		TStopOrderPrice2Char(ptQueuePrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		TStopOrderPrice2Char(ptCounterPrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		TStopOrderPrice2Char(ptMarketPrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		TStopOrderPrice2Char(ptLastPrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		TStopOrderPrice2Char(ptSuperPrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		m_popupPriceType.CreatePopWin(this, L"PopupPriceType", m_Hwnd, 0, vPriceType, 0, 0, 0, 3, 2);

	}
	POINT pt = m_ptPopupPriceType;
	ClientToScreen(m_Hwnd, &pt);
	SetWindowPos(m_popupPriceType.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
}
void TOrderView::OnSearchCode()
{
	if (m_lockContract.GetValue()) return;
	if (!IsWindow(m_popupContractCode.GetHwnd()))
	{
		m_popupContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0);
	}
	m_popupContractCode.UpdateData(g_pGlobalModel->m_strSign.c_str());
	POINT pt = m_ptSearchCode;
	ClientToScreen(m_Hwnd, &pt);
	SetWindowPos(m_popupContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}
void TOrderView::SetPriceType(wstring str)
{
	m_spinPrice.SetText(str.c_str());
	m_iPriceType = GetPriceType(str);

	m_wstrPriceTypeText = str;
	m_bChangeWithQuote = true;
	UpdatePrice();

	SetLabSuperPriceText();
}

void TOrderView::SetContractNo(string sCode)
{
	m_Mediator->Send(CMD_OredrView_SetContractNo, &sCode, this);
}

void TOrderView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT id = GetWindowLong(GetFocus(), GWL_ID);
	if (id == ID_editContractNo)
	{
		if (!IsWindowVisible(m_popupCodeList.GetHwnd()))
			return;
		int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (nDelta > 0)
			m_popupCodeList.DelNum();
		else
			m_popupCodeList.AddNum();
	}
}

void TOrderView::SetLabSuperPriceText(bool bSetText)
{
	if (bSetText &&m_iPriceType==ptSuperPrice)
	{
		char szText[30] = { 0 };
		if (m_pOrderData->iBuyOffset >= 0 && m_pOrderData->iSellOffset >= 0)
			sprintf_s(szText, "+%d  +%d", m_pOrderData->iBuyOffset, m_pOrderData->iSellOffset);
		else if (m_pOrderData->iBuyOffset >= 0)
			sprintf_s(szText, "+%d  %d", m_pOrderData->iBuyOffset, m_pOrderData->iSellOffset);
		else if (m_pOrderData->iSellOffset >= 0)
			sprintf_s(szText, "%d  +%d", m_pOrderData->iBuyOffset, m_pOrderData->iSellOffset);
		else
			sprintf_s(szText, "%d  %d", m_pOrderData->iBuyOffset, m_pOrderData->iSellOffset);

		m_labSuperPrice.SetText(szText);
	}
	else
	{
		m_labSuperPrice.SetText("");   //清楚内容
	}
}