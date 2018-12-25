#include "PreInclude.h"

#define  closeWidth 9
#define VGAP  30
#define WidghtHeight  20
#define WidghtWidth  150

#define EMainFrmName L"class TMainFrame"
#define WIDGET_LEFT 12
#define WIDGET_HGAP 5
#define FRM_WIDTH  (WidghtWidth*2+WIDGET_LEFT*2 + WIDGET_HGAP)
#define FRM_HEIGHT  550

bool SHFEDeepQuoteFrm::bQuoteLoginSuccess = false;

int g_iCountPos=0;
SHFEDeepQuoteFrm::SHFEDeepQuoteFrm()
{
	MinWidth = FRM_WIDTH;
	MinHeight = 450;
	//m_QuoteData.SetQuoteUpdate(this);
	m_clSequence = new ColumnSequence();
	CMBLSpiHandler * handler = CMBLSpiHandler::GetMBLQuoteData();
	handler->SetQuoteUpdate(this);
	m_QuoteData = handler;
}

SHFEDeepQuoteFrm::~SHFEDeepQuoteFrm()
{
	if(m_QuoteData != nullptr)
	{
		delete m_QuoteData;
		m_QuoteData = nullptr;
	}

	if (m_clSequence != nullptr)
	{
		delete m_clSequence;
		m_clSequence = nullptr;
	}
}


void SHFEDeepQuoteFrm::TOnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
    TCHAR name[50] = { 0 };
    GetClassName(source, name, sizeof(name));
    if (lstrcmp(name, WC_EDIT) == 0)
        return;
    if (!g_DeepQuoteFrm)
    {
        g_DeepQuoteFrm = new SHFEDeepQuoteFrm;
        g_DeepQuoteFrm->ShowFrm(FindWindow(EMainFrmName, NULL),L"deep_quote",L"国富上期所深度行情");
    }
    if (IsWindowVisible(g_DeepQuoteFrm->GetHwnd()))
    {
        ShowWindow(g_DeepQuoteFrm->GetHwnd(), SW_HIDE);
    }
    else
    {
        ShowWindow(g_DeepQuoteFrm->GetHwnd(), SW_SHOW);
    }
}

void SHFEDeepQuoteFrm::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	if (!bQuoteLoginSuccess)
	{
		CLoginFrm * loginFrm = new CLoginFrm;
		loginFrm->Create(L"SHFEDeepQuoteLoginFrm", L"国富上期所深度行情登录");
		ShowWindow(loginFrm->GetHwnd(), SW_SHOW);
	}	
	else
	{
		//由于MBLApi当前Relese之后调用析构函数会崩溃，这里留坑先不判空，每次都new一个新的
		//if (!g_DeepQuoteFrm)
		//{
		g_DeepQuoteFrm = new SHFEDeepQuoteFrm;
		g_DeepQuoteFrm->ShowFrm(FindWindow(EMainFrmName, NULL), L"deep_quote", L"国富上期所深度行情");
		g_pMainFrame->AddLinkage(g_DeepQuoteFrm->GetHwnd(), 1, OnLinkageAction);
		//}
		ShowWindow(g_DeepQuoteFrm->GetHwnd(), SW_SHOW);
	}
}

void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{
	if (g_DeepQuoteFrm->GetHwnd() == To)
		SendMessage(To, WM_SHFEMESSAGE, 0, (LPARAM)(LPCSTR)Content);
}

LRESULT SHFEDeepQuoteFrm::OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        OnSize(wParam, lParam);
        return PROCESSED;
    case WM_DRAWITEM:
        OnDrawItem(wParam, lParam);
        return PROCESSED;
    case WM_MEASUREITEM:
        OnMeasureItem(wParam, lParam);
        return PROCESSED;
    case WM_CTLCOLORSTATIC:
        return OnCtlColor(wParam, lParam);
    case SSWM_COMBOX_SELECT:
        OnComboxSel(wParam, lParam);
        return PROCESSED;
	case WM_SHFEMESSAGE:
		OnContent(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_COLUMNQUENCE:
		OnColumnSequence(wParam);
		return PROCESSED;
	case WM_DESTROY:
		OnDestory();
		return PROCESSED;
    case WM_TIMER:
        OnTimer();
		return PROCESSED;
	case WM_UPDATEQUOTE:
		//RefreshData(m_clSequence);
		return PROCESSED;
    default:
        return NOT_PROCESSED;
    }
}


LRESULT SHFEDeepQuoteFrm::OnNcDestory(WPARAM wParam, LPARAM lParam)
{
	//由于MBLApi当前Relese之后调用析构函数会崩溃，这里留坑先不delete，后续MBLApi修复析构函数后修改
	//delete this;
	//g_DeepQuoteFrm = nullptr;
	ShowWindow(g_DeepQuoteFrm->GetHwnd(), SW_HIDE);
	return PROCESSED;
}

void SHFEDeepQuoteFrm::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	m_listDeepQuotel.OnDrawItem(lParam);
}

void SHFEDeepQuoteFrm::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listDeepQuotel.OnMeasureItem(p->itemHeight);
}


void SHFEDeepQuoteFrm::OnTimer()
{
    //if (m_iTimerCount < 0)
    //{
    //    m_iRefreshFrequence = max(m_iRefreshFrequence, 3);
    //    m_iTimerCount = m_iRefreshFrequence;
    //    OnDealComContract();
    //}
    //else
    //{
    //    m_iTimerCount--;
    //}
	RefreshData(m_clSequence);
	m_QuoteData->ClearQuoteData();
}
void SHFEDeepQuoteFrm::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
    switch (lParam)
    {
    case ID_BTNSEARCH:
        OnSearchContract();
        break;
    default:
        break;
    }
}
void SHFEDeepQuoteFrm::OnSearchContract()
{
    if (!IsWindow(m_popupContractCode.GetHwnd()))
    {
        m_popupContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0,0,0,400);
    }
	char szSign[21] = {};

	//g_pTradeData->GetAllLogin(loginUsers);
	//uint loginCount = loginUsers.size();
	//if (loginUsers.size())
	//{
	//	const TLogin *pLogin = (const TLogin*)loginUsers.at(0);
	//	strcpy_s(m_szSign, pLogin->Sign);
	//}

	//strcpy_s(szSign, m_QuoteData->GetSign());
    m_popupContractCode.UpdateData(szSign);

    POINT pt = {m_ptPopup.x, m_ptPopup.y};
    ClientToScreen(m_Hwnd, &pt);
    SetWindowPos(m_popupContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

void SHFEDeepQuoteFrm::OnCreate()
{
	ReadColumnSequence();
	CreateWidget();
	//创建列表右键
	m_menuColumn = CreatePopupMenu();

    InitWidget();
	InitMenu();

	RECT rect = { 
		(GetSystemMetrics(SM_CXSCREEN) - FRM_WIDTH) / 2 + g_iCountPos * 50, 
		(GetSystemMetrics(SM_CYSCREEN) - FRM_HEIGHT) / 2 + 100 + g_iCountPos * 50,
		(GetSystemMetrics(SM_CXSCREEN) - FRM_WIDTH) / 2 + FRM_WIDTH + g_iCountPos * 50, 
		(GetSystemMetrics(SM_CYSCREEN) - FRM_HEIGHT) / 2 + 100 + FRM_HEIGHT + g_iCountPos * 50 
	};

	if (g_iCountPos < 5)
		g_iCountPos++;
	else
		g_iCountPos = 0;
	SetWindowPos(m_Hwnd, HWND_TOP, rect.left, rect.top, FRM_WIDTH, rect.bottom - rect.top, SWP_SHOWWINDOW);
	SetTimer(m_Hwnd, 0, 800, NULL);
	//m_QuoteData->GetUserInfo();
}

void SHFEDeepQuoteFrm::CreateWidget()
{
	m_comContract.Create(m_Hwnd, ID_COMCONTRACT);
	m_btnContract.CreateButton(m_Hwnd, m_btnContract, "...", ID_BTNSEARCH);
	m_listDeepQuotel.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_LISTDEEPQUOTEL);
	for (int i = 0; i < 2; i++)
	{
		m_stCount[i].Create(m_Hwnd, L"", i, SS_CENTER);
		m_threshold[i].Create(m_Hwnd, 0UL, i);
		m_ckthreshold[i].Create(m_Hwnd, "", 2, i);
	}
	m_RefreshFrequence.Create(m_Hwnd);
	ShowWindow(m_RefreshFrequence.GetHwnd(),SW_HIDE);
	m_stRefreshFrequenceTips.Create(m_Hwnd, L"");
	ShowWindow(m_stRefreshFrequenceTips.GetHwnd(), SW_HIDE);

	m_stQuote.Create(m_Hwnd, L"档位");
	m_pinQuote.Create(m_Hwnd, 0, ID_PINQUOTE);
	ShowWindow(m_stQuote.GetHwnd(), SW_HIDE);
	ShowWindow(m_pinQuote.GetHwnd(), SW_HIDE);
}

LRESULT SHFEDeepQuoteFrm::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
    SetBkMode((HDC)wParam, TRANSPARENT);
    if ((HWND)lParam == m_stCount[0].GetHwnd())
    {
        SetTextColor((HDC)wParam, g_ColorRefData.GetColorTextRed());
    }
    else  if ((HWND)lParam == m_stCount[1].GetHwnd())
    {
        SetTextColor((HDC)wParam, g_ColorRefData.GetColorTextGreen());
    }
    return (LRESULT)g_ColorRefData.g_brush_bk;

}

void SHFEDeepQuoteFrm::InitWidget()
{
    m_comContract.SetFont(g_FontData.GetFontWord13());
	m_listDeepQuotel.AddColumn(L"序号", 45);
	RefreshColumn(m_clSequence);

    for (int i = 0; i < 2; i++)
    {
        m_stCount[i].SetFont(g_FontData.g_FontWord12);
        m_threshold[i].SetWindowTextW(1);
        m_threshold[i].SetRange(1, 99999);
        m_threshold[i].SetFont(g_FontData.g_FontNum15);
        m_ckthreshold[i].SetBkColor(g_ColorRefData.g_ColorBackground);
        m_ckthreshold[i].SetFont(g_FontData.g_FontNum15);
        m_ckthreshold[i].SetValue(false);
        //m_stthresholdTips[i].SetFont(g_FontData.g_FontWord12);
    }
    m_stRefreshFrequenceTips.SetFont(g_FontData.g_FontWord12);
    m_stRefreshFrequenceTips.SetText("秒自动刷新（3-10）");
    m_ckthreshold[0].SetFont(g_FontData.g_FontWord12);
    m_ckthreshold[1].SetFont(g_FontData.g_FontWord12);

    m_ckthreshold[0].SetText("买高亮阈值");
    m_ckthreshold[1].SetText("卖高亮阈值");
    m_RefreshFrequence.SetRange(3, 10);
    m_RefreshFrequence.SetWindowTextW(3);
    m_RefreshFrequence.SetFont(g_FontData.g_FontNum15);

	m_pinQuote.SetWindowTextW(0);
	m_pinQuote.SetRange(1, 10000);
	m_pinQuote.SetFont(g_FontData.g_FontNum15);
}

void SHFEDeepQuoteFrm::InitMenu()
{
	AppendMenu(m_menuColumn, MF_STRING, ID_MENUCOLUMN, L"设置列头显示");
}

void SHFEDeepQuoteFrm::SetWidgetPos()
{
    RECT rect = {0};
    GetClientRect(m_Hwnd, &rect);
    rect.left += 12;

	int top = rect.top + CAPTION_HEIGHT + 8;
	m_comContract.MoveWindow(rect.left, top , WidghtWidth, WidghtHeight);
	m_ptPopup.x = rect.left;
	m_ptPopup.y = top + WidghtHeight;
	m_btnContract.MoveWindow(rect.left + WidghtWidth + 8, top, WidghtHeight, WidghtHeight);
	top += WidghtHeight + 8;

	int bottomTop = rect.bottom - VGAP;

	int left = rect.left;
    m_RefreshFrequence.MoveWindow(left, bottomTop, 50, WidghtHeight);
	left += 50;
    m_stRefreshFrequenceTips.MoveWindow(left, bottomTop, 120, WidghtHeight);

	left = rect.left+WidghtWidth*2+WIDGET_HGAP -60;
	m_pinQuote.MoveWindow(left, bottomTop, 60, WidghtHeight);
	left -= 25;
	m_stQuote.MoveWindow(left, bottomTop, 25, WidghtHeight);

	bottomTop -= VGAP;
	left = rect.left;
	m_ckthreshold[0].MoveWindow(left, bottomTop, 90, WidghtHeight);
	left += 90;
    m_threshold[0].MoveWindow(left, bottomTop, WidghtWidth-90, WidghtHeight);
	left = left + WidghtWidth - 90 + WIDGET_HGAP;
	m_ckthreshold[1].MoveWindow(left, bottomTop, 90, WidghtHeight);
	left += 90;
	m_threshold[1].MoveWindow(left , bottomTop, WidghtWidth - 90, WidghtHeight);

	bottomTop = bottomTop - VGAP + 8;
	left = rect.left;
    m_stCount[0].MoveWindow(left , bottomTop, WidghtWidth, WidghtHeight);
	left += WidghtWidth + WIDGET_HGAP;
    m_stCount[1].MoveWindow(left , bottomTop, WidghtWidth, WidghtHeight);

	bottomTop -= 5;
    m_listDeepQuotel.MoveWindow(rect.left, top, WidghtWidth * 2+WIDGET_HGAP, bottomTop -top);
}

LRESULT SHFEDeepQuoteFrm::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (HIWORD(wParam) == BN_CLICKED)
    {
        HWND hwnd = (HWND)lParam;
        if (hwnd == m_ckthreshold[0].GetHwnd() || hwnd == m_ckthreshold[1].GetHwnd())
        {
            RefreshData(m_clSequence);
        }
		else if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
		{
			switch (LOWORD(wParam))
			{
			case ID_MENUCOLUMN:
				if (!m_ColumuConfig)
				{
					m_ColumuConfig = new ColumnConfigView();
					m_ColumuConfig->ShowFrm(m_Hwnd, L"column", L"设置列头显示");
				}
				ShowWindow(m_ColumuConfig->GetHwnd(), SW_SHOW);
			default:
				return NOT_PROCESSED;
			}
		}
    }
    else if (((HWND)lParam == m_threshold[0].GetEditHwnd() &&m_ckthreshold[0].GetValue()) || 
		((HWND)lParam == m_threshold[1].GetEditHwnd() && m_ckthreshold[1].GetValue()))
    {
       if( HIWORD(wParam) == EN_CHANGE)
		   RefreshData(m_clSequence);
    }
    else if ((HWND)lParam == m_RefreshFrequence.GetEditHwnd())
    {
        if (HIWORD(wParam) == EN_KILLFOCUS)
        {
            int i = m_RefreshFrequence.GetWindowNumber();
            if (i)
                m_iRefreshFrequence = i;
        }
    }
	else if ((HWND)lParam == m_pinQuote.GetEditHwnd())
	{
		UpdateQty();
	}
	
	return PROCESSED;
}

void SHFEDeepQuoteFrm::UpdateQty()
{
	int index = m_pinQuote.GetWindowNumber();
	SetTotalQty(m_QuoteData->GetTotalMBLQty(dBuy, index), m_QuoteData->GetTotalMBLQty(dSell, index));
}

void SHFEDeepQuoteFrm::OnSize(WPARAM wParam, LPARAM lParam)
{
    SetWidgetPos();
}

void SHFEDeepQuoteFrm::SetContractNo(string sCode)
{
    AddContractToComCtr(sCode);
}



void SHFEDeepQuoteFrm::AddContractToComCtr(string sCode)
{
	if (sCode == "")
	{
		m_comContract.SetText(sCode.c_str());
		m_listDeepQuotel.DeleteAllItems();
		return;
	}
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
    OnDealComContract();

}

void SHFEDeepQuoteFrm::OnColumnSequence(WPARAM wParam)
{
	m_clSequence = (ColumnSequence*)wParam;
	for (int i = 4; i > 0; i--)
	{
		ListView_DeleteColumn(m_listDeepQuotel.GetListHwnd(), i);
	}
	RefreshColumn(m_clSequence);
	RefreshData(m_clSequence);
}

void SHFEDeepQuoteFrm::RefreshData(ColumnSequence* clSequence)
{
	vector<TDeepQuote> vecBuy;
	vector<TDeepQuote> vecSell;

	m_QuoteData->GetQuoteData(vecBuy, vecSell);

	vector<TDeepQuote> *pVecBuy = &vecBuy;
	vector<TDeepQuote> *pVecSell = &vecSell;

	int buySize = pVecBuy->size();
	int sellSize = pVecSell->size();
	int size = buySize < sellSize ? buySize : sellSize;

	const char * priceFormat = m_QuoteData->GetPriceFormat();
	char szPrice[2][100] = {};
	char szBuyPrice[100] = {};
	char szSellPrice[100] = {};

	int totalBuy = 0;
	int totalSell = 0;
	int quoteIndex = m_pinQuote.GetWindowNumber();

	//ShowWindow(m_listDeepQuotel.GetListHwnd(), SW_HIDE);
	m_listDeepQuotel.DeleteAllItems();

	for (int i = 0; i < size; i++)
	{
		//TDeepQuote * pBuyQuote = &(pVecBuy->at(buySize - 1 - i));
		TDeepQuote * pBuyQuote = &(pVecBuy->at(i));
		sprintf_s(szBuyPrice, priceFormat, pBuyQuote->Price);
		//TDeepQuote * pSellQuote = &(pVecSell->at(sellSize - 1 - i));
		TDeepQuote * pSellQuote = &(pVecSell->at(i));
		sprintf_s(szSellPrice, priceFormat, pSellQuote->Price);

		m_listDeepQuotel.AddItem(i + 1, i, 0);
		for (int j = 1; j < 5; j++)
		{
			if (j == 1)
				SetItem(clSequence->FirstColumn,i, j, pBuyQuote, pSellQuote, szBuyPrice, szSellPrice);
			else if (j == 2)
				SetItem(clSequence->SecondColumn,i, j, pBuyQuote, pSellQuote, szBuyPrice, szSellPrice);
			else if (j == 3)
				SetItem(clSequence->ThirdColumn,i, j, pBuyQuote, pSellQuote, szBuyPrice, szSellPrice);
			else
				SetItem(clSequence->FourthColumn,i, j, pBuyQuote, pSellQuote, szBuyPrice, szSellPrice);
		}

	}
	UpdateQty();
	//ShowWindow(m_listDeepQuotel.GetListHwnd(), SW_SHOW);
	m_listDeepQuotel.AdjustScroll();
}

void SHFEDeepQuoteFrm::SetItem(int num,int line, int column, TDeepQuote* pBuyQuote, TDeepQuote* pSellQuote,char* szBuyPrice,char* szSellPrice)
{
	bool bBuyHlight = m_ckthreshold[0].GetValue();
	bool bSellHlight = m_ckthreshold[1].GetValue();
	int buyThreshold = m_threshold[0].GetWindowNumber();
	int sellThreshold = m_threshold[1].GetWindowNumber();

	if (num == 0)//该列为买价
	{
		if (bBuyHlight && pBuyQuote->Qty >= buyThreshold)
			m_listDeepQuotel.AddItem(szBuyPrice, line, column, 0, g_ColorRefData.g_ColorTextRed);
		else
			m_listDeepQuotel.AddItem(szBuyPrice, line, column);
	}
	else if (num == 1)//该列为买量
	{
		if (bBuyHlight && pBuyQuote->Qty >= buyThreshold)
			m_listDeepQuotel.AddItem((int)pBuyQuote->Qty, line, column, 0, g_ColorRefData.g_ColorTextRed);
		else
			m_listDeepQuotel.AddItem((int)pBuyQuote->Qty, line, column);
	}
	else if (num == 2)//该列为卖价
	{
		if (bSellHlight && pSellQuote->Qty >= sellThreshold)
			m_listDeepQuotel.AddItem(szSellPrice, line, column, 0, g_ColorRefData.g_ColorTextRed);
		else
			m_listDeepQuotel.AddItem(szSellPrice, line, column );
	}
	else if (num == 3)//该列为卖量
	{
		if (bSellHlight && pSellQuote->Qty >= sellThreshold)
			m_listDeepQuotel.AddItem((int)pSellQuote->Qty, line, column, 0, g_ColorRefData.g_ColorTextRed);
		else
			m_listDeepQuotel.AddItem((int)pSellQuote->Qty, line, column);
	}
}

void SHFEDeepQuoteFrm::RefreshColumn(ColumnSequence* clSequence)
{
	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			SetColumn(clSequence->FirstColumn);
		}
		else if (i == 1)
		{
			SetColumn(clSequence->SecondColumn);
		}
		else if (i == 2)
		{
			SetColumn(clSequence->ThirdColumn);
		}
		else
		{
			SetColumn(clSequence->FourthColumn);
		}
	}
}

void SHFEDeepQuoteFrm::SetColumn(int num)
{
	if (num == 0)
		m_listDeepQuotel.AddColumn(L"买价", 75);
	else if (num == 1)
		m_listDeepQuotel.AddColumn(L"买量", 45);
	else if (num == 2)
		m_listDeepQuotel.AddColumn(L"卖价", 75);
	else if (num == 3)
		m_listDeepQuotel.AddColumn(L"卖量", 45);
}

void SHFEDeepQuoteFrm::SetTotalQty(int buyQty, int sellQty)
{
	char szText[50] = { 0 };
	sprintf_s(szText, "总买量  %d", buyQty);
	m_stCount[0].SetText(szText);
	sprintf_s(szText, "总卖量  %d", sellQty);
	m_stCount[1].SetText(szText);
}

void SHFEDeepQuoteFrm::OnComboxSel(WPARAM wParam, LPARAM lParam)
{
    HWND hwnd = (HWND)lParam;
    if (hwnd == m_comContract.GetHwnd())
        OnDealComContract();
}

void SHFEDeepQuoteFrm::OnDealComContract()
{
    m_stOrderInfo.strCode = m_comContract.GetText();
    TContractKey key;
    memset(&key, 0, sizeof(key));
    CConvert::TContractNoToTContractKey(m_stOrderInfo.strCode.c_str(), key);
    memcpy(key.Sign, m_stOrderInfo.szSign1, sizeof(m_stOrderInfo.szSign1));
   
	//屏蔽老的深度行情推送SPI
	//m_QuoteData.QrySHFEDeepQuote(key);

	//采用国富的接口进行深度行情订阅
	m_QuoteData->SubSHFEDeepQuote(key);
}

void SHFEDeepQuoteFrm::OnContent(WPARAM wParam, LPARAM lParam)
{
	string Mcontent = (LPCSTR)lParam;
	string strCon;
	vector<NoticeContent> vector1;
	vector<string> vector2;
	vector1.clear();
	CConvert::SplitNotice(Mcontent, ';', '=', vector1);
	strCon = vector1.at(0).strContent;
	vector2 = GetContract(strCon, vector2);
	if (vector2.size() < 4 || vector2.at(0) != "SHFE")
	{
		return;
	}
	if (vector2.at(3) == "MAIN" || vector2.at(3) == "NEARBY")   //指数就是index,主连就是main，近月就是nearby
	{
		SContract* tempContract = g_pStarApi->GetContractUnderlay(vector1.at(0).strContent.c_str());
		string content = tempContract->ContractNo;
		//解析合约代码
		vector2.clear();
		vector2 = GetContract(content, vector2);
		ShowContract(vector2);
	}
	else if (vector2.at(3) == "INDEX" || vector2.at(3) == "")
		AddContractToComCtr("");
	else
		ShowContract(vector2);
}

vector<string> SHFEDeepQuoteFrm::GetContract(string content, vector<string> vecstr)
{
	string temp;
	string::size_type isplit = content.find('|');
	while (string::npos != isplit)
	{
		temp = content.substr(0, isplit);
		content = content.substr(++isplit, string::npos);
		isplit = content.find('|');
		vecstr.push_back(temp);
	}
	vecstr.push_back(content);
	return vecstr;
}

void SHFEDeepQuoteFrm::ShowContract(vector<string> vecstr)
{
	string tempStr;
	tempStr.append(vecstr.at(0));
	tempStr.append(" ");
	tempStr.append(vecstr.at(2));
	tempStr.append(" ");
	tempStr.append(vecstr.at(3));
	AddContractToComCtr(tempStr);
}

LRESULT SHFEDeepQuoteFrm::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_LISTDEEPQUOTEL:
		DealColumnHeader(lParam);
		return PROCESSED;
	default:
		break;
	}
	
	return NOT_PROCESSED;
}

void SHFEDeepQuoteFrm::DealColumnHeader(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenuItem(m_menuColumn, ID_MENUCOLUMN, MF_ENABLED);
		ShowMenu(m_menuColumn);
	}
}

void SHFEDeepQuoteFrm::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void SHFEDeepQuoteFrm::Update()
{
	PostMessage(m_Hwnd, WM_UPDATEQUOTE, 0, 0);
	//RefreshData(m_clSequence);
}

void SHFEDeepQuoteFrm::OnDestory()
{
	WriteColumnSequence();
}

void SHFEDeepQuoteFrm::ReadColumnSequence()
{
	char Name[10] = "列头顺序";
	int iKey = 0;
	char buffer[6] = { 0 };
	char cKey[4] = { 0 };

	TCHAR fileName[150] = L"\\Config\\SHFEColumnSequence.pri";
	TCHAR filePath[150] = { 0 };
	CConvert::GetFilePath(filePath, sizeof(filePath) / 2, fileName);

	while (strcmp(buffer, "Over") != 0)
	{
		sprintf_s(cKey, "%d", iKey);
		if (!GetPrivateProfileStringA(Name, cKey, "Over", buffer, sizeof(buffer), LoadRC::unicode_to_ansi(filePath).c_str()))
			return;
		//初始化Column
		InitColumn(buffer, iKey);
		iKey++;
	}
}

void SHFEDeepQuoteFrm::InitColumn(char* buffer,int Column)
{
	if (strcmp(buffer, "Over") == 0)
		return;
	std::string PriBuffer = buffer;
	if (Column == 0)
		m_clSequence->FirstColumn = atoi(PriBuffer.c_str());
	else if (Column == 1)
		m_clSequence->SecondColumn = atoi(PriBuffer.c_str());
	else if (Column == 2)
		m_clSequence->ThirdColumn = atoi(PriBuffer.c_str());
	else if (Column == 3)
		m_clSequence->FourthColumn = atoi(PriBuffer.c_str());
}

void SHFEDeepQuoteFrm::WriteColumnSequence()
{
	char Name[10] = "列头顺序";
	int iKey = 0;
	char cKey[4] = { 0 };
	char ColumnName[4] = { 0 };

	TCHAR fileName[150] = L"\\Config\\SHFEColumnSequence.pri";
	TCHAR filePath[150] = { 0 };
	CConvert::GetFilePath(filePath, sizeof(filePath) / 2, fileName);
	WritePrivateProfileStringA(Name, NULL, NULL, LoadRC::unicode_to_ansi(filePath).c_str());
	 
	for (int i = 0; i < 4; i++)
	{
		sprintf_s(cKey, "%d", iKey);
		GetColumnSequence(ColumnName, i);
		WritePrivateProfileStringA(Name, cKey, ColumnName, LoadRC::unicode_to_ansi(filePath).c_str());
		iKey++;
	}
}

void SHFEDeepQuoteFrm::GetColumnSequence(char* Columnname, int i)
{
	if (i == 0)
		sprintf_s(Columnname, 4, "%d", m_clSequence->FirstColumn);
	else if (i == 1)
		sprintf_s(Columnname, 4, "%d", m_clSequence->SecondColumn);
	else if (i == 2)
		sprintf_s(Columnname, 4, "%d", m_clSequence->ThirdColumn);
	else if (i == 3)
		sprintf_s(Columnname, 4, "%d", m_clSequence->FourthColumn);
}

ColumnSequence SHFEDeepQuoteFrm::GetColumnSequence()
{
	return *m_clSequence;
}
