#include "BaseInclude.h"
#define frmWidth   360
#define frmHeight  90

#define widgetTop   0
#define widgetLeft  5
#define widgetHeight 20
#define widgetHGap   5
#define bottomGap    30

#define btnWidth  60
#define btnEnuWidth  80


TCHAR szChsLog[][10] = { L"卖价", L"卖量", L"最新", L"买价", L"买量", L"现手", L"开仓止损", L"条件单" };
TCHAR szChtLog[][10] = { L"賣價", L"賣量", L"最新", L"買價", L"買量", L"現手", L"開倉止損", L"條件單" };
TCHAR szEnuLogp[][20] = {L"AskPrice",L"AskQty",L"LastPrice",L"BidPrice",L"BidQty",L"LastQty",L"OpenWithStop",L"Condition"};

enum{
	AskPrice_Index,
	AskQty_Index,
	LastPrice_Index,
	BidPrice_Index,
	BidQry_Index,
	LastQty_Index,
	OpenStop_Index,
	Condition_Index,
};
TCHAR * _GetTextLog(int i)
{
	if (g_language == CHT)return szChtLog[i];
	else if (g_language == ENU)return szEnuLogp[i];
	return szChsLog[i];
}
TLogView::TLogView(IMediator *pMediator, HWND hParent) :IColleague(pMediator)
{
	m_hParent = hParent;
	m_iFrmTop = 0;    //从OrderView中获得
    m_pQuoteData = NULL;
}

TLogView::~TLogView()
{

}


void TLogView::ShowFrm(int formerIndex, int nowIndex)
{
	if (!m_Hwnd)
		CreateFrm(L"TLogView", m_hParent, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
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

	SetWindowPos(m_Hwnd, 0, m_iFrmLeft, m_iFrmTop, m_iFrmWidth, m_iActualHeight, show);
}
void TLogView::UpdateLog()
{
	SendMessage(m_editLog.GetHwnd(), EM_SETREADONLY, false, 0);
    m_editLog.Clear();
    m_editLog.AddString(m_pLogData->loginfo[1], RGB(0, 0, 0));
    m_editLog.AddString(m_pLogData->LogState[1], RGB(0, 0, 0));
    m_editLog.AddString(m_pLogData->loginfo[0], RGB(0, 0, 0));
    m_editLog.AddString(m_pLogData->LogState[0], RGB(0, 0, 0));
	SendMessage(m_editLog.GetHwnd(), EM_SETREADONLY, true, 0);
}

void TLogView::UpdateData()
{
    char sztext[250] = { 0 };
    DoubleToChar(m_pQuoteData->iPrecision, m_pQuoteData->SellPrice, sztext);

    m_btnlogSell.SetButtonText(sztext);
    sprintf_s(sztext, "%d", m_pQuoteData->SellQty);

    m_btnlogSellQty.SetButtonText(sztext);
    DoubleToChar(m_pQuoteData->iPrecision, m_pQuoteData->LastPrice, sztext);


    m_btnlogNewPrice.SetButtonText(sztext);
    DoubleToChar(m_pQuoteData->iPrecision, m_pQuoteData->BuyPrice, sztext);


    m_btnlogBuy.SetButtonText(sztext);
    sprintf_s(sztext, "%d", m_pQuoteData->BuyQty);

    m_btnlogBuyQty.SetButtonText(sztext);
    sprintf_s(sztext, "%d", m_pQuoteData->LastQty);

    m_btnlogNewQty.SetButtonText(sztext);
}

void TLogView::SetShowRect(RECT rect)
{
	m_iFrmWidth = rect.right - rect.left;
	if (m_iFrmWidth > frmWidth)
		m_iFrmWidth = frmWidth;

	m_iFrmLeft = rect.left;

	m_iFrmHeight = rect.bottom - m_iFrmTop;
	m_iActualHeight = m_iFrmHeight;
	if (m_iFrmHeight < frmHeight)
		m_iFrmHeight = frmHeight;

	if (m_bShow)
	{
		ShowFrm(-1, ID_frmTKeyOrder);
	}
}

void TLogView::OnSize()
{
	m_editLog.MoveWindow(1, m_iLogTop, m_iFrmWidth-2, m_iFrmHeight - bottomGap - m_iLogTop);
	int top = m_iFrmHeight - bottomGap + 9;
	int width = btnWidth;
	if (g_language == ENU)
	{
		width = btnEnuWidth;
	}
	m_btnOpenLoss.MoveWindow(widgetLeft, top, width, widgetHeight);
	m_btnConOrder.MoveWindow(widgetLeft + width + 15, top, width, widgetHeight);
}

LRESULT TLogView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case  WM_SIZE:
		OnSize();
		return PROCESSED;
    case SSWM_STATIC_BUTTON_CLICKDOWN:
        OnClickBtn(wParam, lParam);
        return PROCESSED;
    case WM_CTLCOLORSTATIC:
        return OnCtrlColor(wParam, lParam);
	default:
		return NOT_PROCESSED;
	}
}
LRESULT TLogView::OnCtrlColor(WPARAM wParam, LPARAM lParam)
{

    if ((HWND)lParam == m_lablogBuy.GetHwnd() || (HWND)lParam == m_lablogBuyQty.GetHwnd())
    {
        SetBkMode((HDC)wParam, TRANSPARENT);
        SetTextColor((HDC)wParam, RGB(240, 0, 0));
        
        return (LRESULT)g_ColorRefData.g_brush_bk;
    }
    else if ((HWND)lParam == m_lablogSell.GetHwnd() || (HWND)lParam == m_lablogSellQty.GetHwnd())
    {
        SetBkMode((HDC)wParam, TRANSPARENT);
        SetTextColor((HDC)wParam, RGB(0, 200, 0));
        return (LRESULT)g_ColorRefData.g_brush_bk;
    }

    return NOT_PROCESSED;
}
void TLogView::OnCreate()
{
	CreateWidget();
	InitWidget();
	SetWidgetPos();
    m_Mediator->Get(CMD_TLog_GetQuoteData, (void*&)m_pQuoteData, this);
    m_Mediator->Get(CMD_TLog_GetLog, (void*&)m_pLogData, this);

}
void TLogView::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
	rect.top = m_iBtnTop;
	rect.bottom = m_iFrmHeight - bottomGap+1;
	if (rect.right < frmWidth)
		rect.right = frmWidth;
	FrameRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_gray);
}
void TLogView::OnClickBtn(WPARAM wParam, LPARAM lParam)
{
    //ID_BtnSell, ID_BtnSellQty, ID_BtnNewPrice, ID_BtnBuy, ID_BtnBuyQty,
    char szText[20] = { 0 };
    switch (lParam)
    {
    case ID_BtnSell:
        GetWindowTextA(m_btnlogSell.GetHwnd(), szText, sizeof(szText));
        break;
    case ID_BtnSellQty:
        GetWindowTextA(m_btnlogSellQty.GetHwnd(), szText, sizeof(szText));
        break;
    case ID_BtnNewPrice:
        GetWindowTextA(m_btnlogNewPrice.GetHwnd(), szText, sizeof(szText));
        break;
    case ID_BtnBuy:
        GetWindowTextA(m_btnlogBuy.GetHwnd(), szText, sizeof(szText));
        break;
    case ID_BtnBuyQty:
        GetWindowTextA(m_btnlogBuyQty.GetHwnd(), szText, sizeof(szText));
        break;
    case ID_BtnNewQty:
        GetWindowTextA(m_btnlogNewQty.GetHwnd(), szText, sizeof(szText));
        break;
	case ID_BtnOpenLoss:
		m_Mediator->Send(CMD_TLog_OpenLoss, (const void*)lParam, this);
		return;
	case ID_BtnConOrder:
		m_Mediator->Send(CMD_TLog_ConOrder, (const void*)lParam, this);
		return;
    default:
        break;
    }
    if (lParam == ID_BtnSellQty || lParam == ID_BtnBuyQty || lParam == ID_BtnNewQty)
        m_Mediator->Send(CMD_TLog_SetQty, szText, this);
    else if (lParam == ID_BtnSell || lParam == ID_BtnNewPrice || lParam == ID_BtnBuy)
        m_Mediator->Send(CMD_TLog_Setprice, szText, this);

}
void TLogView::CreateWidget()
{
	m_labContractInfo.Create(m_Hwnd, L"");

	m_lablogSell.Create(m_Hwnd, _GetTextLog(AskPrice_Index));
	m_btnlogSell.Create(m_Hwnd, "", ID_BtnSell);

	m_lablogSellQty.Create(m_Hwnd, _GetTextLog(AskQty_Index));
	m_btnlogSellQty.Create(m_Hwnd, "", ID_BtnSellQty);

	m_lablogNewPrice.Create(m_Hwnd, _GetTextLog(LastPrice_Index));
	m_btnlogNewPrice.Create(m_Hwnd, "", ID_BtnNewPrice);

	m_lablogBuy.Create(m_Hwnd, _GetTextLog(BidPrice_Index));
	m_btnlogBuy.Create(m_Hwnd, "", ID_BtnBuy);

	m_lablogBuyQty.Create(m_Hwnd, _GetTextLog(BidQry_Index));
	m_btnlogBuyQty.Create(m_Hwnd, "", ID_BtnBuyQty);

	m_lablogNewQty.Create(m_Hwnd, _GetTextLog(LastQty_Index));
	m_btnlogNewQty.Create(m_Hwnd, "", ID_BtnNewQty);

	m_editLog.Create(m_Hwnd);
	

	if (g_language == CHT)
	{
		m_btnOpenLoss.Create(m_Hwnd, "開倉止損", ID_BtnOpenLoss);
		m_btnConOrder.Create(m_Hwnd, "條件單", ID_BtnConOrder);
	}
	else if (g_language == ENU)
	{
		m_btnOpenLoss.Create(m_Hwnd, "OpenWithStop", ID_BtnOpenLoss);
		m_btnConOrder.Create(m_Hwnd, "Condition", ID_BtnConOrder);
	}
	else
	{
		m_btnOpenLoss.Create(m_Hwnd, "止损开仓", ID_BtnOpenLoss);
		m_btnConOrder.Create(m_Hwnd, "条件单", ID_BtnConOrder);
	}

}

void TLogView::InitWidget()
{
	TStaticButton *btnGroup[] = { &m_btnlogSell, &m_btnlogSellQty, &m_btnlogNewPrice, &m_btnlogBuy, &m_btnlogBuyQty, &m_btnlogNewQty };
	int iCount = sizeof(btnGroup) / sizeof(btnGroup[0]);
	for (int i = 0; i < iCount; i++)
	{
		btnGroup[i]->SetbtnCursorStyle(IDC_HAND);
		btnGroup[i]->SetFrameColor(g_ColorRefData.g_ColorBackground);
		btnGroup[i]->SetDTstyle(DT_LEFT);
		btnGroup[i]->SetBkColor(g_ColorRefData.g_ColorBackground);
	}
    m_editLog.SetFont(g_FontData.GetFontNum13());
}

void TLogView::SetWidgetPos()
{
	int left = widgetLeft;
	int top = widgetTop;
	int width = 200;

	m_labContractInfo.MoveWindow(0, top, width, widgetHeight);

	top =top +  widgetHeight+3;
	m_iBtnTop = top;

	TStatic *pLab[][2] = { &m_lablogSell, &m_lablogSellQty, &m_lablogBuy, &m_lablogBuyQty, &m_lablogNewPrice, &m_lablogNewQty };
	TStaticButton *pBtn[][2] = { &m_btnlogSell, &m_btnlogSellQty, &m_btnlogBuy, &m_btnlogBuyQty, &m_btnlogNewPrice, &m_btnlogNewQty };

	top += 5;
	for (int i = 0; i < 3; i++)
	{
		width = 25;
		if (g_language == ENU)
			width = 45;
		pLab[i][0]->MoveWindow(left, top, width, widgetHeight);
		pLab[i][1]->MoveWindow(left, top + widgetHeight, width, widgetHeight);
		left += width + widgetHGap;
		width = 60;
		if (g_language == ENU)
			width = 50;
		pBtn[i][0]->MoveWindow(left, top, width, widgetHeight);
		pBtn[i][1]->MoveWindow(left, top + widgetHeight, width, widgetHeight);
		left += width + 5;
	}
	top = top + widgetHeight * 2 + 5;
	m_iLogTop = top;
	m_editLog.MoveWindow(1, top, m_iFrmWidth-2, m_iFrmHeight - bottomGap- top);
	
	left = widgetLeft;
	top = m_iFrmHeight - bottomGap + 9;
	width = btnWidth;
	if (g_language == ENU)
	{
		width = btnEnuWidth;
	}

	m_btnOpenLoss.MoveWindow(left,top,width,widgetHeight);
	m_btnConOrder.MoveWindow(left + width + 15, top, width, widgetHeight);
}