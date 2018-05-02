#include "PreInclude.h"

extern IMainFrame*			g_pMainFrame;
extern TPluginMgrApi*		g_pmanager;
extern PointOrderConfig		g_stOrderConfig;
extern TClickPriceOrderWnd* g_pClickPriceOrder;
extern TClickPriceOrderOptionWnd* g_pTClickPriceOrderOptionWnd;
extern TClickPriceOrderTipsFrm g_clTips;

int g_Width[9];
//extern TPointConfig			g_clOrderConfig;
//vector<TPointOrderWnd*>		g_pVecPointWnd;

#define  closeWidth 9
#define  SetWidth 60
int CAPTION_H = 30;
int FRAME_H = 850;
int FRAME_V = 634;

#define LEFT 12
#define TOP 35
#define RIGHT 130
#define BOTTOM 20

#define WIDGETH 20
#define WIDGETV 120
#define X_VAP  10
#define Y_VAP  5

bool bHidePP = false;

#define  path "\\config\\PointOrderConfig.pri"


#define MainFrmName  L"class TMainFrame"
enum Title{ TitleName = 0, Config };
wchar_t CHSTitle[][50] = { L"点价下单", L"设置" };
wchar_t CHTitle[][50] = { L"點價下單", L"設置" };
wchar_t ENUTitle[][50] = { L"ClickOrder", L"Setting" };
wchar_t* _GetTitleString(int i)
{
	if (g_LanguageID == ENU)
		return ENUTitle[i];
	else if (g_LanguageID == CHT)
		return CHTitle[i];
	else
		return CHSTitle[i];
}

TCHAR CHSExplain[][20] = {L"买入",L" ←",L"卖出",L" →",L"偏移加一",L" ↑",L"偏移减一",L" ↓",L"开仓",L" Alt",L"平仓",L" Ctrl" };
TCHAR CHTExplain[][20] = {L"買入",L" ←",L"賣出",L" →",L"偏移加壹",L" ↑",L"偏移減壹",L" ↓",L"開倉",L" Alt",L"平倉",L" Ctrl" };
TCHAR ENUExplain[][20] = {L"Buy",L" ←",L"Sell",L" →",L"Offset+1",L" ↑",L"Offset-1",L" ↓",L"Open",L" Alt",L"Close",L" Ctrl" };

enum { PositionInfo = 0, ProfitInfo, FundInfo, PositionQty, ProfitQty, FundQty, HotKeyCheck, HotKeyDirectPosition, HotKeyDirectReserve, PPCheck, PPBuy, PPBuyD, PPSell, PPSellD, OrderQty, LastPriceCenter, CancelAll, CoverAll };
char CHSWidget[18][100] = { "净持仓量", "盈亏", "资金占用", "0", "0.0", "0.0%", "热键同步", "正向", "反向", "乒乓", "买", "档", "卖", "档", "手数", "最新价居中", "全撤(W)", "全平(Q)" };
char CHTWidget[18][100] = { "凈持倉量", "盈虧", "資金占用", "0", "0.0", "0.0%", "熱鍵同步", "正向", "反向", "乒乓", "買", "檔", "賣", "檔", "手數", "最新價居中", "全撤(W)", "全平(Q)" };
char ENUWidget[18][100] = { "Position", "Net Pn", "Funds Proportion", "0", "0.0", "0.0%", "HotkeySyn", "Positive", "Reverse", "PingPong", "Buy", "Ticks", " Sel", " Ticks", "Qty", " LastPrice Center", "CancelAll(W)", " CloseAll(Q)" };

TCHAR CHSFirstRow[][20] = {L"多头",L" 条件",L"委托",L" 买量",L"价格",L"卖量",L"委托",L"条件",L"空头" };
TCHAR CHTFirstRow[][20] = {L"多頭",L" 條件",L"委托",L"買量",L"價格",L"賣量",L"委托",L"條件",L"空頭" };
TCHAR ENUFirstRow[][20] = {L"Long",L"Condition",L"Order",L"Buy",L"Price",L"Sell",L"Order",L"Condition",L"Short" };

TCHAR SCHSFirstRow[][20] = { L"空头",L" 条件",L"委托",L"买量",L"价格",L"卖量",L"委托",L"条件",L"多头" };
TCHAR SCHTFirstRow[][20] = { L"空頭",L" 條件",L"委托",L" 買量",L"價格",L"賣量",L"委托",L"條件",L"多頭" };
TCHAR SENUFirstRow[][20] = { L"Short",L"Condition",L"Order",L"BuyQty",L"Price",L"SellQty",L"Order",L"Condition",L"Long" };

TCridHeader::TCridHeader()
{
	m_clBkGray = RGB(229, 235, 235);
	m_clBkWhite = RGB(255, 255, 255);
}
TCridHeader::~TCridHeader()
{

}

void TClickPriceOrderWnd::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	TOnHotKeyDown(false, false, false, VK_F7, false, NULL);
}

void TClickPriceOrderWnd::TOnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
	TCHAR name[50] = { 0 };
	GetClassName(source, name, sizeof(name));
	if (lstrcmp(name, WC_EDIT) == 0)
		return;

	if (!g_pClickPriceOrder)
	{
		g_pClickPriceOrder = new TClickPriceOrderWnd();
	}

	if (!CConvert::IsUserLogin())
	{
		HWND hwnd = FindWindow(L"class TMainFrame", NULL);
		_TMessageBox_Domodal(hwnd, "提示", L"请先登陆交易", TMB_OK);
		return;
	}

	g_pClickPriceOrder->ReadConfigFile();
	g_pClickPriceOrder->ShowFrm();

	g_pMainFrame->AddLinkage(g_pClickPriceOrder->GetHwnd(), 0, TClickPriceOrderWnd::TOnLinkageAction);
}

void TCridHeader::GetHeaderInfo(GridHeaderInfo& stInfo)
{
	memcpy(&stInfo, &m_stGridHeaderInfo, sizeof(GridHeaderInfo));
}
void TCridHeader::SetHeaderInfo(GridHeaderInfo stInfo)
{
	memcpy(&m_stGridHeaderInfo, &stInfo, sizeof(GridHeaderInfo));
}
void TCridHeader::DrawHeaderBase(TMemDC* pmemdc, const RECT& rect)
{
	int iold = SaveDC(pmemdc->GetHdc());
	SelectObject(pmemdc->GetHdc(), g_FontData.GetFontNum13());

	DrawFrame(pmemdc, rect);
	DrawRow(pmemdc, rect);
	RestoreDC(pmemdc->GetHdc(), iold);
}
void TCridHeader::DrawFrame(TMemDC* pmemdc, RECT rect)
{
	HBRUSH hbrFrame = CreateSolidBrush(m_clBkGray);
	SelectObject(pmemdc->GetHdc(), hbrFrame);
	FrameRect(pmemdc->GetHdc(), &rect, hbrFrame);
	DeleteObject(hbrFrame);
}
void TCridHeader::DrawRow(TMemDC* pmemdc,  RECT rect)
{
	RECT rcTemp = { rect.left, rect.top, rect.left + m_iHeaderWidth -1, rect.top + m_iHeaderHeight };
	for (int iRow = 0; iRow < m_iRow; iRow++)
	{
		rcTemp.top = rect.top + iRow * m_iHeaderHeight;
		rcTemp.bottom = rcTemp.top + m_iHeaderHeight;
		rcTemp.left = rect.left;
		for (int iCol = 0; iCol < m_iCol; iCol++)
		{
			rcTemp.right = rcTemp.left + g_Width[iCol];
			DrawRect(pmemdc, rcTemp, iRow, iCol);
			rcTemp.left += g_Width[iCol];
		}
	}
}
void TCridHeader::DrawRect(TMemDC* pmemdc, RECT rect, int iRow, int iCol)
{
	HBRUSH hbrRect = CreateSolidBrush(m_clBkGray);
	if (iRow == 0)
		FillRect(pmemdc->GetHdc(), &rect, hbrRect);

	HPEN hpen = CreatePen(PS_SOLID, 1, m_clBkGray);
	SelectObject(pmemdc->GetHdc(), hpen);

	if (iCol != 0)
		rect.left -= 1;
	FrameRect(pmemdc->GetHdc(), &rect, hbrRect);

	DrawHeaderText(pmemdc, rect, iRow, iCol);
	DeleteObject(hbrRect);
	DeleteObject(hpen);
}
void TCridHeader::DrawHeaderText(TMemDC* pmemdc, RECT rect, int iRow, int iCol)
{
	SetTextColor(pmemdc->GetHdc(), RGB(74, 74, 82));
	if (iRow == 0)
		DrawFirstRowText(pmemdc, rect, iCol);
	else
		DrawSecondRowText(pmemdc, rect, iCol);
}

void TCridHeader::DrawFirstRowText(TMemDC* pmemdc, RECT rect, int iCol)
{
	UINT Format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	switch (g_LanguageID)
	{
	case NONE:
		break;
	case CHT:
		if(!g_stOrderConfig.iBuySellDirection)
			DrawText(pmemdc->GetHdc(), CHTFirstRow[iCol], -1, &rect, Format);
		else 
			DrawText(pmemdc->GetHdc(), SCHTFirstRow[iCol], -1, &rect, Format);
		break;
	case CHS:
		if (!g_stOrderConfig.iBuySellDirection)
			DrawText(pmemdc->GetHdc(), CHSFirstRow[iCol], -1, &rect, Format);
		else
			DrawText(pmemdc->GetHdc(), SCHSFirstRow[iCol], -1, &rect, Format);
		break;
	case ENU:
		if (!g_stOrderConfig.iBuySellDirection)
			DrawText(pmemdc->GetHdc(), ENUFirstRow[iCol], -1, &rect, Format);
		else
			DrawText(pmemdc->GetHdc(), SENUFirstRow[iCol], -1, &rect, Format);
		break;
	default:
		
		break;
	}	
}
void TCridHeader::DrawSecondRowText(TMemDC* pmemdc, RECT rect, int iCol)
{
	char szText[50] = { 0 };
	COLORREF colref = GetTextColor(pmemdc->GetHdc());
	UINT Format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	switch (iCol)
	{
	case 0:
		if (!g_stOrderConfig.iBuySellDirection){
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iBuyPositionQty);
			SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorTextGreen);
		}	
		else {
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iSellPositionQty);
			SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorTextRed);
		}
			
		
		DrawText(pmemdc->GetHdc(),LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, Format);
		SetTextColor(pmemdc->GetHdc(), colref);
		break;
	case 1:
		if (!g_stOrderConfig.iBuySellDirection)
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iBuyConditionQty);
		else
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iSellConditionQty);
		DrawText(pmemdc->GetHdc(),LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, Format);
		break;
	case 2:
		if (!g_stOrderConfig.iBuySellDirection)
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iBuyParOrderQty);
		else
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iSellParOrderQty);
		DrawText(pmemdc->GetHdc(),LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, Format);
		break;
	case 3:
		sprintf_s(szText, "%d", m_stGridHeaderInfo.iBuyQuoteQty);
		DrawText(pmemdc->GetHdc(),LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, Format);
		break;
	case 4:
		//sprintf_s(szText, "%d:%d", m_stGridHeaderInfo.iBuyQuoteQty, m_stGridHeaderInfo.iSellQuoteQty);
		DrawText(pmemdc->GetHdc(), L"", -1, &rect, Format);
		break;
	case 5:
		sprintf_s(szText, "%d", m_stGridHeaderInfo.iSellQuoteQty);
		DrawText(pmemdc->GetHdc(),LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, Format);
		break;
	case 6:
		if (!g_stOrderConfig.iBuySellDirection)
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iSellParOrderQty);
		else
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iBuyParOrderQty);
		DrawText(pmemdc->GetHdc(),LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, Format);
		break;
	case 7:
		if (!g_stOrderConfig.iBuySellDirection)
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iSellConditionQty);
		else
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iBuyConditionQty);
		DrawText(pmemdc->GetHdc(),LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, Format);
		break;
	case 8:
		if (!g_stOrderConfig.iBuySellDirection) {
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iSellPositionQty);
			SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorTextRed);
		}
		else {
			sprintf_s(szText, "%d", m_stGridHeaderInfo.iBuyPositionQty);
			SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorTextGreen);
		}
			
		
		DrawText(pmemdc->GetHdc(),LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, Format);
		SetTextColor(pmemdc->GetHdc(), colref);
		break;
	default:
		break;
	}
}
void TCridHeader::NumToChar(char*szText, int iSize, const int Num)
{
	sprintf_s(szText, iSize, "%d", Num);
}


TClickPriceOrderWnd::TClickPriceOrderWnd()
{
	if (g_LanguageID == ENU)
		FRAME_V = 634;
	else
		FRAME_V = 454;
	m_bUserLogin = false;
	m_rcClose.right = FRAME_V - 8;
	m_rcClose.left = m_rcClose.right - closeWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + closeWidth;

	m_rcSet.right = m_rcClose.left - 10;
	m_rcSet.left = m_rcSet.right - SetWidth;
	m_rcSet.top = 0;
	m_rcSet.bottom = m_rcSet.top + CAPTION_H;

	m_bCoverAll = false;

}
void TClickPriceOrderWnd::ShowFrm()
{

	HWND hwnd = FindWindow(L"class TMainFrame", NULL);
	CreateFrm(L"TPointOrderWnd", hwnd, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU | WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	if (m_rc.left)
	{
		SetWindowPos(m_Hwnd, HWND_TOP, m_rc.left, m_rc.top, m_rc.right - m_rc.left,m_rc.bottom - m_rc.top, SWP_SHOWWINDOW);
	}
	else
	{
		//int x = (GetSystemMetrics(SM_CXSCREEN) - FRAME_V) / 2 + 50);
		int x = GetSystemMetrics(SM_CXSCREEN) / 2 ;
		int y = 30;
		SetWindowPos(m_Hwnd, HWND_TOP, x, y, FRAME_V, GetSystemMetrics(SM_CYSCREEN) - 95, SWP_SHOWWINDOW);
	}
	OnDealInitProcess();
}
void TClickPriceOrderWnd::SetHeaderInfo(GridHeaderInfo stHeaderInfo)
{
	m_clHeader.SetHeaderInfo(stHeaderInfo);
	InvalidateRect(m_Hwnd, NULL, true);
}

void TClickPriceOrderWnd::WriteConfigFile()
{
	char filePath[200] = { 0 };
	CConvert::GetFilePath(filePath, sizeof(filePath), path);

	//获取界面的位置大小

	RECT rcClient;
	GetWindowRect(m_Hwnd, &rcClient);

	char Name[20] = "点价配置";
	char cKey[20] = { 0 };
	char cContent[150] = { 0 };
	sprintf_s(cKey, "%d", 0);
	//排队时间、平仓价格类型、是否弹框提示、颜色对比、买卖左右方向（默认是左买右卖）\界面left、界面right、界面top、界面bottom，
	sprintf_s(cContent, sizeof(cContent), "%d|%d|%d|%d|%d|%d|%d|%d|%d", 
		g_stOrderConfig.stPingPang.iQueueTime,
		g_stOrderConfig.iCoverAllPriceType,
		g_stOrderConfig.iMessageBox, 
		g_stOrderConfig.iComparePre , 
		g_stOrderConfig.iBuySellDirection,
		rcClient.left, 
		rcClient.right, 
		rcClient.top, 
		rcClient.bottom);
	WritePrivateProfileStringA(Name, cKey, cContent, filePath);
}

void TClickPriceOrderWnd::ReadConfigFile()
{
	char filePath[200] = { 0 };
	CConvert::GetFilePath(filePath, sizeof(filePath), path);

	char Name[10] = "点价配置";
	char cKey[2] = { 0 };
	char buffer[250] = { 0 };
	sprintf_s(cKey, "%d", 0);
	if (GetPrivateProfileStringA(Name, cKey, "Over", buffer, sizeof(buffer), filePath))
		InitStConfig(buffer);
}
void TClickPriceOrderWnd::InitStConfig(char* buf)
{
	if (strcmp(buf, "Over") == 0)
		return;
	int i = 0;
	std::string PriBuffer = buf;
	std::vector<std::string> vecPriBuffer;
	vecPriBuffer = CConvert::SplitString(PriBuffer, '|');
	if (vecPriBuffer.size() != 5 && vecPriBuffer.size() != 9)
		return;
	g_stOrderConfig.stPingPang.iQueueTime = atoi(vecPriBuffer[i++].c_str());
	g_stOrderConfig.iCoverAllPriceType = atoi(vecPriBuffer[i++].c_str());
	g_stOrderConfig.iMessageBox = atoi(vecPriBuffer[i++].c_str());
	g_stOrderConfig.iComparePre = atoi(vecPriBuffer[i++].c_str());
	g_stOrderConfig.iBuySellDirection = atoi(vecPriBuffer[i++].c_str());
	if (vecPriBuffer.size() == 9)
	{
		m_rc.left = atoi(vecPriBuffer[i++].c_str());
		m_rc.right = atoi(vecPriBuffer[i++].c_str());
		m_rc.top = atoi(vecPriBuffer[i++].c_str());
		m_rc.bottom = atoi(vecPriBuffer[i++].c_str());
		
	}
}
void __cdecl TClickPriceOrderWnd::TOnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{
	bool bValid = false;
	UINT type = 0;
	if (!strcmp(Sender, "PolestarQuote") && 
			(!strcmp(Action, "LButtonDown") || 
			!strcmp(Action, "VK_UP") ||
			!strcmp(Action, "VK_DOWN") ||
			!strcmp(Action, "SetContract")))
	{
		type = From_Quote;
		bValid = true;
	}
	else if (!strcmp(Sender, "PointOrder") && !strcmp(Action, "KEY_DOWN"))
	{
		if (IsWindow(g_pClickPriceOrder->GetHwnd()))
			g_pClickPriceOrder->DealHotKeySyn(atoi(Content),0);
	}
	else if (!strcmp(Sender, "TradeDisplay") && !strcmp(Action, "Bn_Click"))
	{
		bValid = true;
		type = From_TradeD;
	}
	else if (!strcmp(Sender, "TradeDisplay") && !strcmp(Action, "Bn_DbClick"))
	{
		bValid = true;
		type = From_TradeDDB;
	}
	else if (!strcmp(Sender, "TradeDisplay") && !strcmp(Action, "SHBn_Click"))
	{
		bValid = true;
		type = From_SHTradeD;
	}
	if (bValid)
	{
		if (IsWindow(g_pClickPriceOrder->GetHwnd()))
			g_pClickPriceOrder->DealNotice(Content, type);
	}
}
LRESULT TClickPriceOrderWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_SIZING:
		OnSizing(wParam, lParam);
		return PROCESSED;
	case WM_COMMAND:
		 return OnCommand(wParam, lParam);
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClick(wParam, lParam);
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_NCHITTEST:
		return OnNcHitTest(wParam, lParam);
	case WM_SETCURSOR:
		OnSetCursor(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_NCLBUTTONDOWN:
		OnNcLButtonDown(wParam, lParam);
		return NOT_PROCESSED;
	case WM_NOTICE:
		OnDealNotice((char*)wParam);
		return PROCESSED;
	case WM_RESETUSERNO:
		OnDealInitProcess();
		return PROCESSED;
	case SSWM_COMBOX_SELECT:
		OnComboxSel(wParam, lParam);
		return PROCESSED;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		//PostMessage(m_clOrderFrm.GetHwnd(), WM_ONFUNCKEYDOWN, wParam, lParam);
		if (g_stOrderConfig.stHotKey.bHotKey)
		{
			char szText[100] = { 0 };
			sprintf_s(szText, "%d", wParam);
			g_pMainFrame->Linkage(GetParent(), "PointOrder", "KEY_DOWN",szText);
		}
	}
	case WM_ONFUNCKEYDOWN:
		return OnKeyDown(message, wParam, lParam);
	case WM_SYSKEYUP:
	case WM_KEYUP:
		//PostMessage(m_clOrderFrm.GetHwnd(), WM_ONFUNCKEYUP, wParam, lParam);
	case WM_ONFUNCKEYUP:
		return PROCESSED;
	case WM_ONHEADERINFOCHANGED:
		OnDealHeaderChange(wParam, lParam);
		return PROCESSED;
	case WM_CHANGEBUYSELLDIRECTION:
		InvalidateRect(m_Hwnd, NULL, false);
		//PostMessage(m_clOrderFrm.GetHwnd(), WM_CHANGEBUYSELLDIRECTION, wParam, lParam);
		return PROCESSED;
	case WM_ONMONEY:
		OnMoney(wParam, lParam);
		return PROCESSED;
	case WM_NCDESTROY:
		OnDestroy(wParam, lParam);
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
void __cdecl TClickPriceOrderWnd::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
	if (CurrProgress == tipReady)
		PostMessage(m_Hwnd, WM_RESETUSERNO, 0, 0);
}
void __cdecl TClickPriceOrderWnd::OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin)
{
	PostMessage(m_Hwnd, WM_RESETUSERNO, 0, 0);
}
void TClickPriceOrderWnd::OnDealInitProcess()
{
	TDataContainer users;
	g_pTradeData->GetAllUser(users);
	uint count = users.size();
	m_bUserLogin = false;
	for (int i = 0; i < count; i++)
	{
		const TUserInfo*pUser = (const TUserInfo*)users.at(i);
		if (bYes==pUser->pLogin->Logined)
		{
			strcpy_s(m_stOrderInfo.szUserNo, pUser->UserNo);
			strcpy_s(m_stOrderInfo.szSign1, pUser->Sign);
			strcpy_s(m_stOrderInfo.szSign2, pUser->Sign);
			m_bUserLogin = true;
			break;
		}
	}
	//m_clOrderFrm.RegistOrderInfo(m_stOrderInfo);
}
LRESULT TClickPriceOrderWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_white;
}
void TClickPriceOrderWnd::OnDealHeaderChange(WPARAM wParam, LPARAM lparam)
{
	GridHeaderInfo* pst = (GridHeaderInfo*)lparam;
	if (!pst)
		return;
	SetHeaderInfo(*pst);
	char szPositionQty[20] = { 0 };
	sprintf_s(szPositionQty, "%d", pst->iBuyPositionQty - pst->iSellPositionQty);
	m_scPositionQty.SetText(szPositionQty);
}
void TClickPriceOrderWnd::OnMoney(WPARAM wParam, LPARAM lParam)
{
	//设置资金信息；
	TMoney* pMoney = (TMoney*)lParam;
	if (!pMoney)
		return;
	GridHeaderInfo stHeaderInfo;
	m_clHeader.GetHeaderInfo(stHeaderInfo);
	char szProfit[20] = { 0 };
	char szFund[20] = { 0 };
	sprintf_s(szProfit, "%.2f", pMoney->NetProfit);
	sprintf_s(szFund, "%.2f%%", (pMoney->Equity - pMoney->Available) * 100 / pMoney->Equity);
	
	m_scProfit.SetText(szProfit);
	m_scFund.SetText(szFund);
}
void TClickPriceOrderWnd::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	WriteConfigFile();
	delete this;
}

void TClickPriceOrderWnd::SetContractNo(string sCode)
{
	AddContractToComCtr(sCode);
	//m_clOrderFrm.SetContractNo(sCode);
}
void TClickPriceOrderWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	int cy = HIWORD(lParam);
	int cx = LOWORD(lParam);
	SetOrderFrmPos(cy);

	//设置说明矩形框；
	m_rcExplain.left = 3;
	m_rcExplain.right = FRAME_V - 3;
	m_rcExplain.bottom = cy - 3;
	m_rcExplain.top = m_rcExplain.bottom - 20;
}
void TClickPriceOrderWnd::OnSizing(WPARAM wParam, LPARAM lParam)
{
	LPRECT pRect = (LPRECT)lParam;
	if (pRect->bottom - pRect->top < 480)
	{
		if (wParam == WMSZ_BOTTOM)
			pRect->bottom = pRect->top + 480;
		else
			pRect->top = pRect->bottom - 480;
	}
}
void TClickPriceOrderWnd::OnPaint()
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
	DrawText(memdc.GetHdc(), _GetTitleString(0), -1, &rcCaption, DT_LEFT | DT_VCENTER  | DT_SINGLELINE);


	DrawClose(&memdc);
	DrawText(memdc.GetHdc(), _GetTitleString(1), -1, &m_rcSet, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);


	DrawContractInfo(&memdc);
	m_clHeader.DrawHeaderBase(&memdc, m_rcGirdHeader);

	if (g_LanguageID == ENU)
		DrawExplain(&memdc);
	else
		DrawExplainChinese(&memdc);

	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}
void TClickPriceOrderWnd::OnCreate()
{
	CreateWidget();
	InitWidget();
	SetWidgetPos();
	if (bHidePP) {
		HidePPCtr();
	}
	
}
LRESULT TClickPriceOrderWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_CKHOTKEY:
	case ID_CKPP:
	case ID_CKDEEPQUOTE:
	case ID_CKLASTPRICECENTER:
		OnDealCheck(wParam);
		break;
	case ID_EDITBUY:
	case ID_EDITSELL:
	case ID_EDITORDDERQTY:
		OnDealEditChange(wParam);
		break;
	}
	
	return NOT_PROCESSED;
}
void TClickPriceOrderWnd::OnDealCheck(WPARAM wParam)
{
	if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
	{
		switch (LOWORD(wParam))
		{
		case ID_CKHOTKEY:
			m_ckHotKey.SetCheck(!m_ckHotKey.GetCheck());
			g_stOrderConfig.stHotKey.bHotKey = m_ckHotKey.GetCheck();
			break;
		case ID_CKPP:
			m_ckPP.SetCheck(!m_ckPP.GetCheck());
			g_stOrderConfig.stPingPang.bPingPang = m_ckPP.GetCheck();
			EnablePP(m_ckPP.GetCheck());
			break;
		case ID_CKLASTPRICECENTER:
			m_ckLastPriceCenter.SetCheck(!m_ckLastPriceCenter.GetCheck());
			g_stOrderConfig.bLastPriceCenter = m_ckLastPriceCenter.GetCheck();
			break;
		case ID_CKDEEPQUOTE:
			m_ckDeepQuote.SetCheck(!m_ckDeepQuote.GetCheck());
			g_stOrderConfig.bDeepQuote = m_ckDeepQuote.GetCheck();
			break;
		default:
			break;
		}
	}	
}
void TClickPriceOrderWnd::OnDealEditChange(WPARAM wParam)
{	
	if (HIWORD(wParam) == EN_CHANGE)
	{
		char szText[20] = { 0 };
		if (LOWORD(wParam) == ID_EDITBUY)
		{
			GetWindowTextA(m_editBuy.GetHwnd(),szText, sizeof(szText));
			g_stOrderConfig.stPingPang.iBuyQty = atoi(szText);
		}
		else if (LOWORD(wParam) == ID_EDITSELL)
		{
			GetWindowTextA(m_editSell.GetHwnd(),szText, sizeof(szText));
			g_stOrderConfig.stPingPang.iSellQty = atoi(szText);
		}
		else if (LOWORD(wParam) == ID_EDITORDDERQTY)
		{
			GetWindowTextA(m_editOrderQty.GetHwnd(),szText, sizeof(szText));
			if (atoi(szText) == 0)
			{
				SetWindowTextA(m_editOrderQty.GetHwnd(), "1");
				g_stOrderConfig.iDefaultQty = 1;
			}
			else
				g_stOrderConfig.iDefaultQty = atoi(szText);
		}
	}	
}
void TClickPriceOrderWnd::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BTNSEARCH:
		OnSearchContract();
		break;
	case ID_BTNCLOSE:
		OnCoverAll();
		break;
	case ID_BTNCANCEL:
		OnCancelAll();
		break;
	default:
		break;
	}
}
void TClickPriceOrderWnd::OnCancelAll()
{
	m_clCanCel.RegistNotice(std::bind(&TClickPriceOrderWnd::OnCancelNotice, this, std::placeholders::_1));
	TContractSingleKey pst;

	CConvert::PosContractToStr(m_stOrderInfo.strCode, pst);
	memcpy(&pst.Sign, &m_stOrderInfo.szSign1, sizeof(pst.Sign));
	m_clCanCel.CanCelOrder(&pst);
}
void TClickPriceOrderWnd::OnCoverAll()
{
	m_bCoverAll = true;
	OnCancelAll();
}
void TClickPriceOrderWnd::InsertCloseOrder(TSendOrder& stOrder)
{
	int iCover = 0, iTCover = 0;
	CConvert::GetCloseNum(stOrder.UserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCover, iTCover);
	if (iTCover == 0 && iCover == 0)
	{
		int i = 0;
		switch (g_LanguageID)
		{
		case NONE:
			break;
		case CHT:
			g_clTips.SetText(ANSIToUnicode(CHTTips[i]).c_str());
			break;
		case CHS:
			g_clTips.SetText(ANSIToUnicode(CHSTips[i]).c_str());
			break;
		case ENU:
			g_clTips.SetText(ANSIToUnicode(ENUTips[i]).c_str());

			break;
		default:
			break;
		}
	}
	else if (iTCover == 0 && iCover != 0)
	{
		stOrder.Offset = oCover;
		stOrder.OrderQty = iCover;
		InsertOrder(stOrder);
	}
	else
	{
		stOrder.Offset = oCoverT;
		stOrder.OrderQty = iTCover;
		InsertOrder(stOrder);
		if (iCover != 0)
		{
			stOrder.Offset = oCover;
			stOrder.OrderQty = iCover;
			InsertOrder(stOrder);
		}
	}
}
void TClickPriceOrderWnd::InsertOrder(TSendOrder& stOrder)
{
	if (stOrder.OrderPrice == 0)
	{
		int i = 5;
		switch (g_LanguageID)
		{
		case NONE:
			break;
		case CHT:
			_TMessageBox_Domodal(m_Hwnd, "提示", LoadRC::ansi_to_unicode(CHTTips[i]).c_str(), TMB_OK);
			break;
		case CHS:
			_TMessageBox_Domodal(m_Hwnd, "提示", LoadRC::ansi_to_unicode(CHSTips[i]).c_str(), TMB_OK);
			break;
		case ENU:
			_TMessageBox_Domodal(m_Hwnd, "提示", LoadRC::ansi_to_unicode(ENUTips[i]).c_str(), TMB_OK);
			break;
		default:
			break;
		}
		return;
	}
	if (stOrder.OrderQty == 0)
	{
		int i = 4;
		switch (g_LanguageID)
		{
		case NONE:
			break;
		case CHT:
			_TMessageBox_Domodal(m_Hwnd, CHTTips[8],LoadRC::ansi_to_unicode(CHTTips[i]).c_str(), TMB_OK);
			break;
		case CHS:
			_TMessageBox_Domodal(m_Hwnd, CHSTips[8], LoadRC::ansi_to_unicode(CHSTips[i]).c_str(), TMB_OK);
			break;									
		case ENU:									
			_TMessageBox_Domodal(m_Hwnd, ENUTips[8], LoadRC::ansi_to_unicode(ENUTips[i]).c_str(), TMB_OK);
			break;
		default:
			break;
		}
		return;

	
	}
	if (g_pTradeApi)
	{

		InsertOrderAssitant::InsertOrder(stOrder);

	}
}
void TClickPriceOrderWnd::OnCancelNotice(bool bsuccess)
{
	if (m_bCoverAll)
	{
		TDataContainer clPosition;
		if (g_pTradeData)
			g_pTradeData->GetAllPositionTotal(clPosition);
		TContractSingleKey Contract;
		TSendOrder stOrder;
		CConvert::PosContractToStr(m_stOrderInfo.strCode, Contract);
		for (int i = 0; i < clPosition.size(); i++)
		{
			TPosition* pst = (TPosition*)clPosition.at(i);
			if (CConvert::Cmp2ContractSingleKey(*pst, Contract))
			{
				CConvert::DefaultDealClose(pst, stOrder);
				if (g_stOrderConfig.iCoverAllPriceType == 0)
					CConvert::GetPrice(&stOrder, stOrder.Direct, 1, stOrder.OrderPrice);
				else if (g_stOrderConfig.iCoverAllPriceType == 1)
					CConvert::GetPrice(&stOrder, stOrder.Direct, 3, stOrder.OrderPrice);
				InsertCloseOrder(stOrder);
			}
		}
	}
	m_bCoverAll = false;
}

void TClickPriceOrderWnd::OnComboxSel(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
	if (hwnd == m_comContract.GetHwnd())
		OnDealComContract();
	else if (hwnd == m_comHotKeyDirect.GetHwnd())
		OnDealHotKeyDirect();

	//m_clOrderFrm.SetContractNo(m_stOrderInfo.strCode);
}
LRESULT TClickPriceOrderWnd::OnKeyDown(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{	
	/*case VK_ADD:*/
	case 0x57://w
		if (g_stOrderConfig.iMessageBox == 1)
		{
			char(*szText)[50] = NULL;
			switch (g_LanguageID)
			{
			case NONE:
				break;
			case CHT:
				szText = &CHTTips[9];
				break;
			case CHS:
				szText = &CHSTips[9];
				break;
			case ENU:
				szText = &ENUTips[9];
				break;
			default:
				break;
			}
			char(*szText1)[50] = NULL;
			int i = 7;
			switch (g_LanguageID)
			{
			case NONE:
				break;
			case CHT:
				szText1 = &CHTTips[i];
				break;
			case CHS:
				szText1 = &CHSTips[i];
				break;
			case ENU:
				szText1 = &ENUTips[i];
				break;
			default:
				break;
			}
			if (!_TMessageBox_Domodal(GetActiveWindow(), *szText1, ANSIToUnicode(*szText).c_str(), TMB_OKCANCEL))
				return PROCESSED;
		}
		OnCancelAll();	
		return PROCESSED;
	/*case VK_SUBTRACT:*/
	case 0x51://q
		if (g_stOrderConfig.iMessageBox == 1)
		{
			char(*szText)[50] = NULL;
			int i = 10;
			switch (g_LanguageID)
			{
			case NONE:
				break;
			case CHT:
				szText = &CHTTips[i];
				break;
			case CHS:
				szText = &CHSTips[i];
				break;
			case ENU:
				szText = &ENUTips[i];
				break;
			default:
				break;
			}
			char(*szText1)[50] = NULL;
			i = 7;
			switch (g_LanguageID)
			{
			case NONE:
				break;
			case CHT:
				szText1 = &CHTTips[i];
				break;
			case CHS:
				szText1 = &CHSTips[i];
				break;
			case ENU:
				szText1 = &ENUTips[i];
				break;
			default:
				break;
			}
			if (!_TMessageBox_Domodal(GetActiveWindow(), *szText1, ANSIToUnicode(*szText).c_str(), TMB_OKCANCEL))
				return PROCESSED;
		}
		OnCoverAll();		
		return PROCESSED;
	default:
		return PROCESSED;
	}
}
void TClickPriceOrderWnd::OnDealComContract()
{
	m_stOrderInfo.strCode = m_comContract.GetText();
	InvalidateRect(m_Hwnd, NULL, true);
}
void TClickPriceOrderWnd::OnDealHotKeyDirect()
{
	if (strcmp(m_comHotKeyDirect.GetText(), "正向") == 0)
		g_stOrderConfig.stHotKey.iHotKeyDirct = 0;
	else if (strcmp(m_comHotKeyDirect.GetText(), "反向") == 0)
		g_stOrderConfig.stHotKey.iHotKeyDirct = 1;
		
}
void TClickPriceOrderWnd::OnSearchContract()
{
	if (!IsWindow(m_popupContractCode.GetHwnd()))
	{
		m_popupContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0);		
	}
	if (m_bUserLogin)
	{
		m_popupContractCode.UpdateData(m_stOrderInfo.szSign1);
		m_bUserLogin = false;
	}
	//m_clOrderFrm.RegistOrderInfo(m_stOrderInfo);
	POINT pt = m_ptSearchCode;
	ClientToScreen(m_Hwnd, &pt);
	SetWindowPos(m_popupContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

void TClickPriceOrderWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.bottom = rect.top + CAPTION_H;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		WriteConfigFile();
		g_pMainFrame->DelLinkage(m_Hwnd);
		DestroyWindow(m_Hwnd);
	}
	if (PtInRect(&m_rcSet, pt))
	{
		g_pTClickPriceOrderOptionWnd->RegistConfig(&g_stOrderConfig);
		g_pTClickPriceOrderOptionWnd->Create(m_Hwnd);
		g_pTClickPriceOrderOptionWnd->MoveWindow(500, 500);
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}
UINT TClickPriceOrderWnd::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);
	RECT rect;
	GetWindowRect(m_Hwnd, &rect);
	if (point.y <= rect.top + 5)
		return HTTOP;
	else if (point.y >= rect.bottom - 5)
		return HTBOTTOM;
	return NOT_PROCESSED;
}
void TClickPriceOrderWnd::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam != m_Hwnd)
		return;
	UINT nHitTest = LOWORD(lParam);
	switch (nHitTest)
	{
	case HTTOP:
	case HTBOTTOM:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		break;
	default:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		break;
	}
}
void TClickPriceOrderWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case HTTOP:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, lParam);
		break;
	case HTBOTTOM:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, lParam);
		break;
	default:
		break;
	}
}
char* _GetString(int i)
{
	switch (g_LanguageID)
	{
	case NONE:
		break;
	case CHT:
		return CHTWidget[i];
	case CHS:
		return CHSWidget[i];
	case ENU:
		return ENUWidget[i];
		break;
	default:
		break;
	}
}
void TClickPriceOrderWnd::CreateWidget()
{
	//========基本信息
	m_comContract.Create(m_Hwnd, ID_COMCONTRACT);
	m_btnContract.CreateButton(m_Hwnd, m_btnContract, "...", ID_BTNSEARCH);
	int i = 0;
	m_scPositionQtyInfo.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(PositionInfo)).c_str());
	m_scProfitInfo.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(ProfitInfo)).c_str());
	m_scFundInfo.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(FundInfo)).c_str());

	m_scPositionQty.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(PositionQty)).c_str());
	m_scProfit.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(ProfitQty)).c_str());
	m_scFund.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(FundQty)).c_str());
	//m_clOrderFrm.Create(m_Hwnd);
	//m_clOrderFrm.RegistConfig(&g_stOrderConfig);

	//=======热键和乒乓控件
	HWND hck = CreateWindowEx(0, WC_BUTTON, LoadRC::ansi_to_unicode(_GetString(HotKeyCheck)).c_str(), WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | WS_TABSTOP,
		0, 0, 0, 0, m_Hwnd, HMENU(ID_CKHOTKEY), GetModuleHandle(NULL), 0);
	m_ckHotKey.Init(hck);

	hck = CreateWindowEx(0, WC_BUTTON, LoadRC::ansi_to_unicode(_GetString(PPCheck)).c_str(), WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | WS_TABSTOP,
		0, 0, 0, 0, m_Hwnd, HMENU(ID_CKPP), GetModuleHandle(NULL), 0);
	m_ckPP.Init(hck);

	HWND hedit, hSpin;
	CreateSpinHwnd(hedit, hSpin, ID_EDITBUY, ID_SPINPPBUY);
	m_editBuy.Init(hedit);
	m_spinBuy.Init(hSpin);

	CreateSpinHwnd(hedit, hSpin, ID_EDITSELL, ID_SPINPPSELL);
	m_editSell.Init(hedit);
	m_spinSell.Init(hSpin);

	m_comHotKeyDirect.Create(m_Hwnd, ID_COMHOTKEYDIRECT);
	m_scPPBuy.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(PPBuy)).c_str());
	m_scPPBuyD.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(PPBuyD)).c_str());
	m_scPPSell.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(PPSell)).c_str());
	m_scPPSellD.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(PPSellD)).c_str());


	//手数，五档行情，最新价居中
	m_scOrderQty.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetString(OrderQty)).c_str());
	CreateSpinHwnd(hedit, hSpin, ID_EDITORDDERQTY, ID_SPINORDERQTY);
	m_editOrderQty.Init(hedit);
	m_spinOrderQty.Init(hSpin);


// 	hck = CreateWindowEx(0, WC_BUTTON, _GetString(i++), WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | WS_TABSTOP,
// 		0, 0, 0, 0, m_Hwnd, HMENU(ID_CKDEEPQUOTE), GetModuleHandle(NULL), 0);
// 	m_ckDeepQuote.Init(hck);

	hck = CreateWindowEx(0, WC_BUTTON, LoadRC::ansi_to_unicode(_GetString(LastPriceCenter)).c_str(), WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | WS_TABSTOP,
		0, 0, 0, 0, m_Hwnd, HMENU(ID_CKLASTPRICECENTER), GetModuleHandle(NULL), 0);
	m_ckLastPriceCenter.Init(hck);

	m_btnCancleOrder.CreateButton(m_Hwnd, m_btnCancleOrder, _GetString(CancelAll), ID_BTNCANCEL);
	m_btnCloseOrder.CreateButton(m_Hwnd, m_btnCloseOrder, _GetString(CoverAll), ID_BTNCLOSE);
	m_hSet = LoadIcon(GetModuleHandle(L"PointOrder.dll"), MAKEINTRESOURCE(IDI_ICON2));
}
void TClickPriceOrderWnd::EnablePP(bool Enable)
{
	EnableWindow(m_spinBuy.GetHwnd(), Enable);
	EnableWindow(m_spinSell.GetHwnd(), Enable);
	EnableWindow(m_editBuy.GetHwnd(), Enable);
	EnableWindow(m_editSell.GetHwnd(), Enable);
}
void TClickPriceOrderWnd::CreateSpinHwnd(HWND& hedit, HWND& hSpin, int idEdit, int idSpin)
{
	hedit = CreateWindowEx(0, WC_EDIT, TEXT(""), ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NUMBER | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS,
		0, 0, 0, 0, m_Hwnd, (HMENU)idEdit, GetModuleHandle(NULL), NULL);
	hSpin = CreateWindowEx(0, UPDOWN_CLASS, TEXT(""), WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | WS_CLIPSIBLINGS,
		0, 0, 0, 0, m_Hwnd, (HMENU)idSpin, GetModuleHandle(NULL), NULL);
	SendMessage(hSpin, UDM_SETBUDDY, (WPARAM)hedit, 0);
	SendMessage(hSpin, UDM_SETRANGE32, 0, (LPARAM)MAKELPARAM(65535, 0));
	SendMessage(hSpin, UDM_SETBASE, 10, 0);
	SendMessage(hedit, EM_SETLIMITTEXT, 10, 0);
	SendMessage(hSpin, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)1000, (short)1));
}
void TClickPriceOrderWnd::InitWidget()
{
	//========基本信息
	m_comContract.SetFont(g_FontData.GetFontWord13());
	m_scPositionQtyInfo.SetFont(g_FontData.GetFontNum17());
	m_scProfitInfo.SetFont(g_FontData.GetFontNum17());
	m_scFundInfo.SetFont(g_FontData.GetFontNum17());
	m_scPositionQty.SetFont(g_FontData.GetFontNum17());
	m_scProfit.SetFont(g_FontData.GetFontNum17());
	m_scFund.SetFont(g_FontData.GetFontNum17());

	//=======热键和乒乓控件
	m_ckHotKey.SetFont(g_FontData.g_FontWord13);
	m_comHotKeyDirect.SetFont(g_FontData.g_FontWord13);
	m_scPPBuy.SetFont(g_FontData.g_FontWord13);
	m_ckPP.SetFont(g_FontData.g_FontWord13);
	m_scPPBuyD.SetFont(g_FontData.g_FontWord13);
	m_scPPSell.SetFont(g_FontData.g_FontWord13);
	m_scPPSellD.SetFont(g_FontData.g_FontWord13);
	m_scOrderQty.SetFont(g_FontData.g_FontWord13);
	m_editOrderQty.SetFont(g_FontData.g_FontWord13);
	m_ckDeepQuote.SetFont(g_FontData.g_FontWord13);
	m_ckLastPriceCenter.SetFont(g_FontData.g_FontWord13);

	m_editBuy.SetFont(g_FontData.GetFontWord13());
	m_editSell.SetFont(g_FontData.GetFontWord13());

	m_comHotKeyDirect.AddString(_GetString(HotKeyDirectPosition));
	m_comHotKeyDirect.AddString(_GetString(HotKeyDirectReserve));
	m_comHotKeyDirect.SetSelect(0);

	m_btnCancleOrder.SetFont(g_FontData.g_FontWord13);
	m_btnCloseOrder.SetFont(g_FontData.g_FontWord13);

	SetWindowTextA(m_editBuy.GetHwnd(), "1");
	SetWindowTextA(m_editSell.GetHwnd(), "1");
	EnablePP(false);
	SetWindowTextA(m_editOrderQty.GetHwnd(), "1");
}
void TClickPriceOrderWnd::SetWidgetPos()
{
	SetBaseInfoPos();
	SetHotKeyAndPPPos();
	SetComCtrPos();
	SetGridHeaderPos();
}
void TClickPriceOrderWnd::HidePPCtr()
{
	ShowWindow(m_ckHotKey.GetHwnd(), SW_HIDE);
	ShowWindow(m_comHotKeyDirect.GetHwnd(), SW_HIDE);
	ShowWindow(m_ckPP.GetHwnd(), SW_HIDE);
	ShowWindow(m_scPPBuy.GetHwnd(), SW_HIDE);
	ShowWindow(m_editBuy.GetHwnd(), SW_HIDE);
	ShowWindow(m_spinBuy.GetHwnd(), SW_HIDE);
	ShowWindow(m_scPPBuyD.GetHwnd(), SW_HIDE);
	ShowWindow(m_scPPSell.GetHwnd(), SW_HIDE);
	ShowWindow(m_editSell.GetHwnd(), SW_HIDE);
	ShowWindow(m_spinSell.GetHwnd(), SW_HIDE);
	ShowWindow(m_scPPSellD.GetHwnd(), SW_HIDE);
}
void TClickPriceOrderWnd::SetGridHeaderPos()
{
	m_rcGirdHeader.left = 2;
	m_rcGirdHeader.right = FRAME_V - 2;
	if (bHidePP) {
		m_rcGirdHeader.top = m_rcContractInfo.bottom + 3 * Y_VAP + WIDGETH;
		m_rcGirdHeader.bottom = m_rcContractInfo.top + 40;
	}
	else {
		m_rcGirdHeader.top = 180;
		m_rcGirdHeader.bottom = 220;
	}

	
}
void TClickPriceOrderWnd::SetOrderFrmPos(int cy)
{
	m_rcOrderFrm.left = 2;
	m_rcOrderFrm.right = FRAME_V - 2;
	if (bHidePP) {
		m_rcOrderFrm.top = 220 - 2 * Y_VAP - WIDGETH;
	}
	else {
		m_rcOrderFrm.top = 220;
	}
	
	m_rcOrderFrm.bottom = cy - 25;
	//m_clOrderFrm.MoveWindow(m_rcOrderFrm.left, m_rcOrderFrm.top, m_rcOrderFrm.right - m_rcOrderFrm.left,
		//m_rcOrderFrm.bottom - m_rcOrderFrm.top);
}
void TClickPriceOrderWnd::SetComCtrPos()
{
	int scWigetV = 30;
	int editWigetV = 42;
	int spinWigetV = 17;
	int ckWigetV = 110;
	int iVap = 10;
	int BtnV = 50;
	if (g_LanguageID == ENU)
	{
		ckWigetV += 30;
		BtnV += 40;
	}
	POINT point;

	if (bHidePP) {
		point = { m_rcContractInfo.left, m_rcContractInfo.bottom + Y_VAP };
	}
	else {
		point = { m_rcContractInfo.left, m_rcContractInfo.bottom + 3 * Y_VAP + WIDGETH };
	}
	
	m_scOrderQty.MoveWindow(point.x, point.y, scWigetV, WIDGETH);
	point.x += scWigetV + iVap;
	SetWindowPos(m_editOrderQty.GetHwnd(), 0, point.x, point.y, editWigetV, WIDGETH, 0);
	SetWindowPos(m_spinOrderQty.GetHwnd(), 0, point.x + editWigetV + 1, point.y, spinWigetV, WIDGETH, 0);

	point.x += 60 + iVap;
	//SetWindowPos(m_ckDeepQuote.GetHwnd(), 0, point.x, point.y, ckWigetV, WIDGETH, 0);
	SetWindowPos(m_ckLastPriceCenter.GetHwnd(), 0, point.x, point.y, ckWigetV, WIDGETH, 0);
	point.x += ckWigetV + iVap;
	//SetWindowPos(m_ckLastPriceCenter.GetHwnd(), 0, point.x, point.y, ckWigetV - 20, WIDGETH, 0);

	
	point.x += ckWigetV - 20 ;
	m_btnCloseOrder.MoveWindow(point.x, point.y, BtnV, WIDGETH);
	point.x += BtnV + Y_VAP;
	m_btnCancleOrder.MoveWindow(point.x, point.y, BtnV, WIDGETH);
}
void TClickPriceOrderWnd::SetHotKeyAndPPPos()
{
	int iWigetH = WIDGETH;
	int iCheckV = 75;
	int iCheckVPP = 50;
	int iComBoxV = 60;
	int iStaticV = 20;
	int iSpinV = 50;
	int iVap = 2;
	if (g_LanguageID == ENU)
	{
		iCheckV += 30;
		iCheckVPP += 30;
		iComBoxV += 20;
		iStaticV += 20;
	}

	POINT point = { m_rcContractInfo.left, m_rcContractInfo.bottom + Y_VAP };
	SetWindowPos(m_ckHotKey.GetHwnd(),0, point.x, point.y, iCheckV, iWigetH,0);

	point.x += iCheckV + iVap;
	m_comHotKeyDirect.MoveWindow(point.x, point.y, iComBoxV, 18);

	point.x += iComBoxV + 10*Y_VAP;
	SetWindowPos(m_ckPP.GetHwnd(), 0, point.x, point.y, iCheckVPP, iWigetH, 0);
	point.x += iCheckVPP + iVap;
	m_scPPBuy.MoveWindow(point.x, point.y, iStaticV, iWigetH);
	point.x += iStaticV;
	SetWindowPos(m_editBuy.GetHwnd(), 0, point.x, point.y, 32, iWigetH, 0);
	SetWindowPos(m_spinBuy.GetHwnd(), 0, point.x + 33, point.y, 17, iWigetH, 0);
	point.x += iSpinV;
	m_scPPBuyD.MoveWindow(point.x, point.y, iStaticV, iWigetH);


	point.x += iStaticV + Y_VAP;
	m_scPPSell.MoveWindow(point.x, point.y, iStaticV, iWigetH);
	point.x += iStaticV;
	SetWindowPos(m_editSell.GetHwnd(), 0, point.x, point.y, 32, iWigetH, 0);
	SetWindowPos(m_spinSell.GetHwnd(), 0, point.x + 33, point.y, 17, iWigetH, 0);
	point.x += iSpinV;
	m_scPPSellD.MoveWindow(point.x, point.y, iStaticV, iWigetH);
	
}
void TClickPriceOrderWnd::SetBaseInfoPos()
{
	int ContractInfoV = 180;
	POINT pt = { LEFT, TOP };
	m_comContract.MoveWindow(pt.x, pt.y, ContractInfoV, 3 * WIDGETH);
	m_ptSearchCode.x = pt.x;
	m_ptSearchCode.y = pt.y + WIDGETH;

	pt.x += ContractInfoV + 2;
	m_btnContract.MoveWindow(pt.x, pt.y, 25, WIDGETH);

	InitRcContractInfo(pt);

	int reWidth = (FRAME_V - m_rcContractInfo.right) / 3 - 1;
	int reHeigh = (m_rcContractInfo.bottom - m_rcContractInfo.top) / 2;
	pt.x = m_rcContractInfo.right;
	pt.y = m_rcContractInfo.top;
	m_scPositionQtyInfo.MoveWindow(pt.x, pt.y, reWidth, reHeigh);
	pt.x += reWidth;
	m_scProfitInfo.MoveWindow(pt.x, pt.y, reWidth, reHeigh);
	pt.x += reWidth;
	m_scFundInfo.MoveWindow(pt.x, pt.y, reWidth, reHeigh);


	pt.x = m_rcContractInfo.right;
	pt.y = m_rcContractInfo.top + reHeigh;
	m_scPositionQty.MoveWindow(pt.x, pt.y, reWidth, reHeigh);
	pt.x += reWidth;
	m_scProfit.MoveWindow(pt.x, pt.y, reWidth, reHeigh);
	pt.x += reWidth;
	m_scFund.MoveWindow(pt.x, pt.y, reWidth, reHeigh);

}

void TClickPriceOrderWnd::InitRcContractInfo(POINT pt)
{
	//合约显示界面;
	m_rcContractInfo.left = LEFT;
	m_rcContractInfo.top = pt.y + WIDGETH + Y_VAP;
	m_rcContractInfo.right = m_rcContractInfo.left + 128;
	m_rcContractInfo.bottom = m_rcContractInfo.top + 55;
}
void TClickPriceOrderWnd::DrawClose(TMemDC *pMemDC)
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
void TClickPriceOrderWnd::DrawContractInfo(TMemDC *pMemDC)
{
	int ioldDc = SaveDC(pMemDC->GetHdc());
	DrawLine(pMemDC);
	SetTextColor(pMemDC->GetHdc(), RGB(192, 142, 65));
	char cInfo[50] = { 0 };
	GetContractInfo(cInfo, sizeof(cInfo));
	if (bHidePP) {
		SelectObject(pMemDC->GetHdc(), g_FontData.g_fontWord28_1);
	}
	else {
		SelectObject(pMemDC->GetHdc(), g_FontData.g_fontWord28_2);
	}
	DrawText(pMemDC->GetHdc(), LoadRC::ansi_to_unicode(cInfo).c_str(), -1, &m_rcContractInfo, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	RestoreDC(pMemDC->GetHdc(), ioldDc);
}

void TClickPriceOrderWnd::DrawLine(TMemDC *pMemDC)
{
	int ioldDc = SaveDC(pMemDC->GetHdc());
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(229, 235, 235));
	HPEN oldPen = (HPEN)SelectObject(pMemDC->GetHdc(), pen);
	MoveToEx(pMemDC->GetHdc(), 2, m_rcContractInfo.top, 0);
	LineTo(pMemDC->GetHdc(), FRAME_V - 2, m_rcContractInfo.top);


	MoveToEx(pMemDC->GetHdc(), 2, m_rcContractInfo.bottom, 0);
	LineTo(pMemDC->GetHdc(), FRAME_V - 2, m_rcContractInfo.bottom);

	MoveToEx(pMemDC->GetHdc(), 2, m_rcContractInfo.bottom + 2 * Y_VAP + WIDGETH, 0);
	LineTo(pMemDC->GetHdc(), FRAME_V - 2, m_rcContractInfo.bottom + 2 * Y_VAP + WIDGETH);
	
	if (!bHidePP) {
		MoveToEx(pMemDC->GetHdc(), 2, m_rcContractInfo.bottom + 2 * (2 * Y_VAP + WIDGETH), 0);
		LineTo(pMemDC->GetHdc(), FRAME_V - 2, m_rcContractInfo.bottom + 2 * (2 * Y_VAP + WIDGETH));
	}
	
	MoveToEx(pMemDC->GetHdc(), m_rcContractInfo.right, m_rcContractInfo.top, 0);
	LineTo(pMemDC->GetHdc(), m_rcContractInfo.right, m_rcContractInfo.bottom);

	int reWidth = (FRAME_V - m_rcContractInfo.right) / 3 - 1;
	int reHeigh = (m_rcContractInfo.bottom - m_rcContractInfo.top) / 2 - 1;

	MoveToEx(pMemDC->GetHdc(), m_rcContractInfo.right + reWidth, m_rcContractInfo.top, 0);
	LineTo(pMemDC->GetHdc(), m_rcContractInfo.right + reWidth, m_rcContractInfo.bottom);


	MoveToEx(pMemDC->GetHdc(), m_rcContractInfo.right + 2 * reWidth, m_rcContractInfo.top, 0);
	LineTo(pMemDC->GetHdc(), m_rcContractInfo.right + 2 * reWidth, m_rcContractInfo.bottom);



	SelectObject(pMemDC->GetHdc(), oldPen);

	DeleteObject(pen);
	RestoreDC(pMemDC->GetHdc(), ioldDc);
}
void TClickPriceOrderWnd::DrawExplainChinese(TMemDC* pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());

	int iWidth = 0;
	RECT rcTemp = m_rcExplain;
	rcTemp.left = 2;
	rcTemp.right = 2;
	COLORREF crBlack = RGB(74, 74, 82);
	COLORREF crBlue = RGB(52, 158, 246);
	SetTextColor(pMemDC->GetHdc(), crBlack);
	UINT Format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	TCHAR(*cExplain)[20] = NULL;
	switch (g_LanguageID)
	{
	case CHT:
		cExplain = &CHTExplain[0];
		break;
	case CHS:
		cExplain = &CHSExplain[0];
		break;
	case ENU:
		cExplain = &ENUExplain[0];
		break;
	default:
		break;
	}
	if (cExplain == NULL)
		return;
	int szLength[12]{40, 20, 40, 20, 70, 20, 70, 20, 40, 30, 40, 30};

	for (int i = 0; i < 16; i++)
	{

		if (i % 2 == 0)
		{
			SetTextColor(pMemDC->GetHdc(), crBlack);
		}
		else
		{
			SetTextColor(pMemDC->GetHdc(), crBlue);
		}

		if (i)
		{
			rcTemp.left = rcTemp.right;
			rcTemp.right = rcTemp.left + szLength[i];
		}
		else
		{
			rcTemp.right = rcTemp.left + szLength[i];
		}

		DrawText(pMemDC->GetHdc(), cExplain[i], -1, &rcTemp, Format);
	}
	RestoreDC(pMemDC->GetHdc(), iold);
}
void TClickPriceOrderWnd::DrawExplain(TMemDC *pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());
	int iWidth = 40;
	RECT rcTemp = m_rcExplain;
	rcTemp.left = 2;
	COLORREF crBlack = RGB(74, 74, 82);
	COLORREF crBlue = RGB(52, 158, 246);
	UINT Format = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	TCHAR (*cExplain)[20]=NULL;
	SelectObject(pMemDC->GetHdc(), g_FontData.GetFontNum13());
	switch (g_LanguageID)
	{
	case CHT:
		cExplain = &CHTExplain[0];
		break;
	case CHS:
		cExplain = &CHSExplain[0];
		break;
	case ENU:
		cExplain = &ENUExplain[0];
		break;
	default:
		break;
	}
	if (cExplain == NULL)
		return;
	int szLength[12]{40, 20, 40, 20, 60, 20, 60, 20, 40, 30, 40, 30};
	for (int i = 0; i < 12; i++)
	{
		if (i % 2 == 0)
			SetTextColor(pMemDC->GetHdc(), crBlack);
		else
			SetTextColor(pMemDC->GetHdc(), crBlue);



		if (i)
		{
		
			/*if ( i  >= 4 && i <= 10 && i % 2 == 0)
				rcTemp.right = rcTemp.left + 2 * iWidth;
			else
			{
				if (i <= 11)
					rcTemp.right = rcTemp.left + iWidth - 5;
				else
					rcTemp.right = rcTemp.left + iWidth;
			}			*/
			rcTemp.left = rcTemp.right;
			rcTemp.right = rcTemp.left + szLength[i];
			DrawText(pMemDC->GetHdc(), cExplain[i], -1, &rcTemp, Format);			
		}
		else
		{
			rcTemp.right = rcTemp.left + szLength[i];
			DrawText(pMemDC->GetHdc(), cExplain[i], -1, &rcTemp, Format);
		}
	}
	RestoreDC(pMemDC->GetHdc(), iold);
}
void TClickPriceOrderWnd::GetContractInfo(char* Info, int iSize)
{
	vector<string> str = CConvert::SplitString(m_stOrderInfo.strCode, ' ');
	if (str.size() == 3) {
		sprintf_s(Info, iSize, "%s%s", str[1].c_str(), str[2].c_str());
	}
	else if (str.size() == 2) {
		sprintf_s(Info, iSize, "%s", str[1].c_str());
	}
	else {
		return;
	}
	
}
void TClickPriceOrderWnd::DealNotice(const char * strContent, UINT iNoticeType)
{
	if (g_stOrderConfig.stHotKey.bHotKey)
		return;
	m_strNoticeContent = strContent;
	m_iNoticeType = iNoticeType;
	PostMessage(m_Hwnd, WM_NOTICE, (WPARAM)strContent, 0);
}
void TClickPriceOrderWnd::DealHotKeySyn(WPARAM wParam, LPARAM lParam)
{
	if (g_stOrderConfig.stHotKey.bHotKey)
	{
		PostMessage(m_Hwnd, WM_ONFUNCKEYDOWN, wParam, lParam);
		//PostMessage(m_clOrderFrm.GetHwnd(), WM_ONFUNCKEYDOWN, wParam, lParam);

	}
}

void TClickPriceOrderWnd::OnDealNotice(char* strContent)
{
	string ContractId = m_strNoticeContent;
	if(GetContractID(ContractId))
		AddContractToComCtr(ContractId);
}
bool TClickPriceOrderWnd::GetContractID(string &strContractId)
{
	vector<string>vecContent;
	if (m_iNoticeType == From_Quote)
	{
		string::size_type pos = m_strNoticeContent.find('=');
		if (pos != string::npos)
		{
			strContractId = m_strNoticeContent.substr(++pos, string::npos);
			string::size_type bpos = strContractId.find(';');
			if (bpos != string::npos)
			{
				strContractId = strContractId.substr(0, bpos);
				SContractNoType q;
				strcpy_s(q, strContractId.c_str());
				SContract* qRet = g_pStarApi->GetContractUnderlay(q);
				if (qRet)
					strContractId = qRet->ContractNo;
				vecContent = CConvert::SplitString(strContractId, '|');
				if (vecContent.size() == 4)
				{
	/*				if (vecContent[3] == "NEARBY" || vecContent[3] == "MAIN" || vecContent[3] == "INDEX")
						return false;*/
					strContractId = vecContent[0] + ' ' + vecContent[2] + ' ' + vecContent[3];
				}
				else if (vecContent.size() == 3) {
					//现货处理
					strContractId = vecContent[0] + ' ' + vecContent[2] + ' ';
				}

			}
		}
	}
	else if (m_iNoticeType == From_TradeD)
	{
		string::size_type pos = m_strNoticeContent.find("contractkey=");
		string::size_type rpos = m_strNoticeContent.find("||");
		if (pos != string::npos)
		{
			pos = pos + strlen("contractkey=");
			strContractId = m_strNoticeContent.substr(pos, rpos - pos);
			vecContent = CConvert::SplitString(strContractId, '|');
			if (vecContent.size() == 4)
				strContractId = vecContent[0] + ' ' + vecContent[1] + ' ' + vecContent[3];
		}
	}
	else if (m_iNoticeType == From_SHTradeD || m_iNoticeType == From_TradeDDB)
	{
		string::size_type pos = m_strNoticeContent.find("orderid=");
		if (pos == string::npos)
			return false;
		pos += strlen("orderid=");
		int iOrderID = atoi(m_strNoticeContent.substr(pos, string::npos - pos - 1).c_str());
		const TOrder* pOrder = g_pTradeData->GetOrder(iOrderID);
		if (pOrder == NULL)
			return false;
		char tmp[51];
		sprintf_s(tmp, "%s %s %s", pOrder->ExchangeNo, pOrder->CommodityNo, pOrder->ContractNo);
		strContractId = tmp;
		
	}
	return true;
}

void TClickPriceOrderWnd::SetContractCode(string Code)
{

}
void TClickPriceOrderWnd::AddContractToComCtr(string sCode)
{
	int index = m_comContract.GetSameTextIndex(sCode);
	if (index == -1)
	{
		m_comContract.AddString(sCode.c_str());
		m_comContract.ResizeData();
		m_comContract.SetSelect(m_comContract.GetSize() - 1);
	}
	else
	{
		m_comContract.SetSelect(index);
	}
	m_stOrderInfo.strCode = m_comContract.GetText();
	//m_clOrderFrm.SetContractNo(m_stOrderInfo.strCode);
	InvalidateRect(m_Hwnd, NULL, true);
}

//用于授权验证

bool TClickPriceOrderWnd::IsAuthent()
{
	return true;
	/*if (!g_pAuthent)
	{
		_TMessageBox_Domodal(FindWindow(MainFrmName, NULL), "提示", L"认证出错！", TMB_OK);
		return false;
	}

	UserAuthentEx stAuthent;
	memset(&stAuthent, 0, sizeof(UserAuthentEx));

	bool bRet = g_pAuthent->GetAuthent("SZGJ", "zc2016", stAuthent); //这两个都需要改
	//bool bRet = g_pAuthent->GetAuthent("100010", "zoyvr3", stAuthent); //这两个都需要改

	if (!bRet)
	{
		TAccessAlarm alarmFrm(FindWindow(MainFrmName, NULL), "认证未通过", TAccessAlarm::noAuthent);
		alarmFrm.Domodal();
		return false;
	}
	if (stAuthent.serverdate < stAuthent.auth.value[0].date) //在授权日期内
	{
		TAccessAlarm alarmFrm(FindWindow(MainFrmName, NULL), "使用时间提醒", TAccessAlarm::daysLeftWarn);
		if (alarmFrm.SetDate(stAuthent.serverdate, stAuthent.auth.value[0].date))
			alarmFrm.Domodal();
		return true;
	}
	else
	{
		TAccessAlarm alarmFrm(FindWindow(MainFrmName, NULL), "使用时间过期", TAccessAlarm::userTimeOut);
		alarmFrm.Domodal();
	}

	return false;*/
}
