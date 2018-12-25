#include "PreInclude.h"

#define widgetLeft  6
#define labOrderStateTop  3
#define labelHeight  23
#define labOrderStateWidth  200
#define labOrderStateBottom (labOrderStateTop +labelHeight)
#define lvOrderTop  (labOrderStateBottom + 10)
#define lvOrderWidth 380
#define frameHeight  180
#define lvOrderHeight (frameHeight-lvOrderTop-1)
#define lvOrderBottom (lvOrderTop + lvOrderHeight)
#define lvFollowOrderWidth  (380+60)
#define lvFollowOrderLeft  (widgetLeft + lvOrderWidth+ 9)
#define frameWidth (785 + 60 -4)


#define tabBtnWidth   60
#define tabBtnLeft    widgetLeft
#define tabBtnTop     lvOrderBottom
#define tabBtnHeight  18


#define floatProfitCol  5   //逐笔浮盈列索引

#define lvPositionWidth  480
#define lvPositionRight  (widgetLeft+lvPositionWidth)

#define btnCoverLeft  (lvPositionRight +8)
#define btnWidth      80
#define btnHGap       15
#define btnVgap       10
#define btnHeight     22

extern TFollowTrade * g_pFollowTrader;
TradeInfo::TradeInfo()
{
	m_iNowSelTab = 0;
}

TradeInfo::~TradeInfo()
{

}
void TradeInfo::Create(HWND hParent)
{
	CreateFrm(L"TradeInfo", hParent, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD|WS_VISIBLE);
	SetTimer(m_Hwnd, 100, 1000, NULL);
}
void TradeInfo::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_Hwnd, NULL, x, y, cx, cy, SWP_NOZORDER);
}
LRESULT TradeInfo::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM:
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(lParam);
		return PROCESSED;
	case WM_TIMER:
		OnTimer();
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		SetBkMode(HDC(wParam), TRANSPARENT);
		return (LRESULT)g_ColorRefData.g_brush_white;
	case WM_SIZE:
		OnSize(LOWORD(lParam), HIWORD(lParam));
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnClickBtn(wParam, lParam);
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
	return NOT_PROCESSED;
}


void TradeInfo::OnSize(int cx, int cy)
{
	m_iLvHeight = cy - tabBtnHeight-lvOrderTop;
	m_lvMainOrder.MoveWindow(widgetLeft, lvOrderTop, lvOrderWidth, m_iLvHeight);
	m_lvFollowOrder.MoveWindow(lvFollowOrderLeft, lvOrderTop, lvFollowOrderWidth, m_iLvHeight);
//	m_lvPosition.MoveWindow(widgetLeft, lvOrderTop, lvFollowOrderLeft - widgetLeft + lvFollowOrderWidth, m_iLvHeight);

	m_lvPosition.MoveWindow(widgetLeft, lvOrderTop, lvPositionWidth, m_iLvHeight);
	for (int i = 0; i < m_vecTabBtnRc.size(); i++)
	{
		m_vecTabBtnRc[i].top = cy-tabBtnHeight;
		m_vecTabBtnRc[i].bottom = cy;
	}
}

void TradeInfo::OnTimer()
{
	if (m_iNowSelTab == 0) return;
	UpdatePosProfit();
}
void TradeInfo::UpdatePosProfit()
{
	double totalProfit = 0;
	for (uint i = 0; i < m_totalPosition.size(); i++)
	{
		m_totalPosition[i].Profit = 0;
		for (uint j = 0; j < m_totalPosition[i].postions.size(); j++)
		{
			const TPosition* pPosition = m_totalPosition[i].postions[j];
			m_totalPosition[i].Profit += pPosition->FloatProfitTBT;
		}
		totalProfit += m_totalPosition[i].Profit;
		AddProfitItem(i, floatProfitCol, m_totalPosition[i].Profit);		
	}
	if (m_totalPosition.size() > 1)
	{
		AddProfitItem(m_totalPosition.size(), floatProfitCol, totalProfit);
		m_lvPosition.SetItemCount(m_totalPosition.size() + 1);
	}
}
void TradeInfo::AddProfitItem(int iRow, int iCol, double data)
{
	if (data > 0)
		m_lvPosition.AddItem(data, iRow, iCol, DT_CENTER, g_ColorRefData.g_ColorTextRed);
	else if (data < 0)
		m_lvPosition.AddItem(data, iRow, iCol, DT_CENTER, g_ColorRefData.g_ColorTextGreen);
	else
		m_lvPosition.AddItem(data, iRow, iCol, DT_CENTER);
}

/**********************
*CheckUser, 用户账号
*检测账号是否是跟单中登陆的账号，如果不是那么就直接返回
**********************/
bool TradeInfo::CheckUser(const char* pszUserNo)
{
	int iSize = m_pMainUser->size();
	for (int i = 0; i < iSize; i++)
	{
		if (strcmp(m_pMainUser->at(i).UserNo, pszUserNo) == 0)
			return true;
	}
	iSize = m_pFollowUser->size();
	for (int i = 0; i < iSize; i++)
	{
		if (strcmp(m_pFollowUser->at(i).UserNo, pszUserNo) == 0)
			return true;
	}
	return false;
}

void TradeInfo::GetPosition()
{
	TDataContainer allPos;
	g_pTradeData->GetAllPositionTotal(allPos);

	m_totalPosition.clear();
	for (uint i = 0; i < allPos.size(); i++)
	{
		const TPosition *pPosition = (const TPosition *)allPos.at(i);

		if(!CheckUser(pPosition->UserNo))continue;

		PositionInfo  stPos;
		stPos.Key[0] = pPosition->Direct;
		stPos.Key[1] = 0;
		strcat_s(stPos.Key, pPosition->CommodityNo); 
		strcat_s(stPos.Key, pPosition->ContractNo);

		auto iter = find(m_totalPosition.begin(), m_totalPosition.end(), stPos.Key);
		if (iter != m_totalPosition.end())
		{
			iter->Price = (iter->Price*iter->Qty + pPosition->PositionPrice*pPosition->PositionQty) / (iter->Qty + pPosition->PositionQty);
			iter->Margin += pPosition->Deposit;
			iter->Profit += pPosition->FloatProfitTBT;
			iter->Qty += pPosition->PositionQty;
			iter->postions.push_back(pPosition);

		}
		else
		{
			const SCommodity * pCommondity = g_pTradeData->GetCommodity(pPosition);
			if (pCommondity)
			{
				wstring wName = CConvert::UTF8ToUnicode(pCommondity->CommodityName);
				lstrcpy(stPos.CommodityName, wName.c_str());
			}
			else
				lstrcpy(stPos.CommodityName, L"");
			stPos.Direct = pPosition->Direct;
			stPos.Qty = pPosition->PositionQty;
			stPos.Price = pPosition->PositionPrice;
			stPos.Margin = pPosition->Deposit;
			stPos.Profit = pPosition->FloatProfitTBT;
			stPos.postions.push_back(pPosition);
			m_totalPosition.push_back(stPos);
		}
	}
}
void TradeInfo::UpdatePosition()
{
	int iCol = 0;

	m_lvPosition.DeleteAllItems(); 
	char szText[50] = { 0 };
	for (uint i = 0; i < m_totalPosition.size(); i++)
	{
		m_lvPosition.AddItem(m_totalPosition[i].CommodityName, i, iCol++, DT_CENTER);
		m_lvPosition.AddItem(&m_totalPosition[i].Key[1], i, iCol++,DT_CENTER); //因为第0位是direct
		TDirect2Char(m_totalPosition[i].Direct, szText, sizeof(szText));
		m_lvPosition.AddItem(szText, i, iCol++, DT_CENTER, SetDirectColor(m_totalPosition[i].Direct));
		m_lvPosition.AddItem((int)m_totalPosition[i].Qty, i, iCol++); 
		m_lvPosition.AddItem(m_totalPosition[i].Price, i, iCol++);
		//m_lvPosition.AddItem(m_totalPosition[i].Profit, i, iCol++);
		AddProfitItem(i, iCol++, m_totalPosition[i].Profit);
		m_lvPosition.AddItem(m_totalPosition[i].Margin, i, iCol++);
		iCol = 0;
	}
	if (m_totalPosition.size() > 1)
	{
		char szText[20] = { 0 };
		sprintf_s(szText, "%d条", m_totalPosition.size());
		m_lvPosition.AddItem(szText, m_totalPosition.size(), 0);
		m_lvPosition.SetItemCount(m_totalPosition.size() + 1);
	}
	else
		m_lvPosition.SetItemCount(m_totalPosition.size());
}
void TradeInfo::OnLButtonDown(LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	for (int i = 0; i < m_vecTabBtnRc.size(); i++)
	{
		if (PtInRect(&m_vecTabBtnRc[i], pt))
		{
			m_iNowSelTab = i;
			break;
		}
	}
	ShowHide();
	if (m_iNowSelTab == 1)
	{
		GetPosition();
		UpdatePosition();
	}
	RECT rc = m_vecTabBtnRc[0];
	rc.right = m_vecTabBtnRc[m_vecTabBtnRc.size() - 1].right;
	InvalidateRect(m_Hwnd, &rc, false);
}
void TradeInfo::OnCreate()
{
	m_labOrderState.Create(m_Hwnd, L"委托信息：");
	m_labOrderState.SetFont(g_FontData.g_FontWord19);
	m_labOrderState.MoveWindow(widgetLeft, labOrderStateTop, labOrderStateWidth, labelHeight);

	m_lvMainOrder.CreateList(m_Hwnd, LVS_REPORT /*| LVS_NOSORTHEADER*/ | LVS_OWNERDRAWFIXED | WS_BORDER | LVS_OWNERDATA, 20, 18, LVS_EX_FULLROWSELECT, id_lvMainOrder);
	m_lvMainOrder.MoveWindow(widgetLeft, lvOrderTop, lvOrderWidth, lvOrderHeight);
	m_lvFollowOrder.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER | LVS_OWNERDATA, 20, 18, LVS_EX_FULLROWSELECT, id_lvFollowOrder);
	m_lvFollowOrder.MoveWindow(lvFollowOrderLeft, lvOrderTop, lvFollowOrderWidth, lvOrderHeight);
	m_lvPosition.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER | LVS_OWNERDATA, 20, 18, LVS_EX_FULLROWSELECT, id_lvPosition);
//	m_lvPosition.MoveWindow(widgetLeft, lvOrderTop, lvFollowOrderLeft-widgetLeft+lvFollowOrderWidth, lvOrderHeight);
	m_lvPosition.MoveWindow(widgetLeft, lvOrderTop, lvPositionWidth, lvOrderHeight);

	char szBtnText[][10] = {"平33%","平50%","平100%","清仓"};
	for (int i = 0; i < ARRAYSIZE(m_btnCover); i++)
	{
		m_btnCover[i].CreateButton(m_Hwnd, szBtnText[i], id_btnCover33 + i);
	}
	SetBtnPos();


	m_iLvHeight = lvOrderHeight;
	ShowHide();

	m_lvMainOrder.AddColumn(L"合约", 60);
	m_lvMainOrder.AddColumn(L"状态", 70);
	m_lvMainOrder.AddColumn(L"买卖", 60);
	m_lvMainOrder.AddColumn(L"开平", 50);
	m_lvMainOrder.AddColumn(L"委托价", 70);
	m_lvMainOrder.AddColumn(L"委托量", 50);

	m_lvFollowOrder.AddColumn(L"账号", 60);
	m_lvFollowOrder.AddColumn(L"合约", 60);
	m_lvFollowOrder.AddColumn(L"状态", 70);
	m_lvFollowOrder.AddColumn(L"买卖", 60);
	m_lvFollowOrder.AddColumn(L"开平", 50);
	m_lvFollowOrder.AddColumn(L"委托价", 70);
	m_lvFollowOrder.AddColumn(L"委托量", 50);

	m_lvPosition.AddColumn(L"品种", 60);
	m_lvPosition.AddColumn(L"合约", 60);
	m_lvPosition.AddColumn(L"买卖", 60);
	m_lvPosition.AddColumn(L"持仓量", 60);
	m_lvPosition.AddColumn(L"计算价格", 80);
	m_lvPosition.AddColumn(L"逐笔浮盈", 80);
	m_lvPosition.AddColumn(L"保证金", 80);


	RECT rc = { tabBtnLeft, tabBtnTop, tabBtnLeft + tabBtnWidth, tabBtnTop + tabBtnHeight };
	m_vecTabBtnRc.push_back(rc);
	rc.left = m_vecTabBtnRc[0].right;
	rc.right = rc.left + tabBtnWidth;
	m_vecTabBtnRc.push_back(rc);
}

void TradeInfo::SetBtnPos()
{
	int left = btnCoverLeft;
	int top = lvOrderTop+20;

	//m_btnCover[0].MoveWindow(left, top, btnWidth, btnHeight);
	//left += btnWidth + btnHGap;
	//m_btnCover[1].MoveWindow(left, top, btnWidth, btnHeight);
	//left = btnCoverLeft;
	//top += btnHeight + btnVgap;
	//m_btnCover[2].MoveWindow(left, top, btnWidth, btnHeight);
	//left += btnWidth + btnHGap;
	//m_btnCover[3].MoveWindow(left, top, btnWidth, btnHeight);

	for (int i = 0; i < ARRAYSIZE(m_btnCover); i++)
	{
		m_btnCover[i].MoveWindow(btnCoverLeft, top, btnWidth, btnHeight);
		top += btnHeight + btnVgap;
	}
}

void TradeInfo::ShowHide()
{
	UINT showOrder = SW_SHOW;
	UINT showPos = SW_HIDE;
	m_labOrderState.SetText(L"委托信息：");
	if (m_iNowSelTab == 1)
	{
		showOrder = SW_HIDE;
		showPos = SW_SHOW;
		m_labOrderState.SetText(L"持仓汇总：");
	}
	ShowWindow(m_lvFollowOrder.GetListHwnd(), showOrder);
	ShowWindow(m_lvMainOrder.GetListHwnd(), showOrder);
	ShowWindow(m_lvPosition.GetListHwnd(), showPos);
}
void TradeInfo::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	switch (wParam)
	{
	case id_lvMainOrder:
		OnLVNotify(&m_lvMainOrder, p);
		break;
	case id_lvFollowOrder:
		OnLVNotify(&m_lvFollowOrder, p);
		break;
	case id_lvPosition:
		OnLVNotify(&m_lvPosition, p);
		break;
	default:
		break;
	}

}
void TradeInfo::OnLVNotify(TListView *pList, LPNMITEMACTIVATE p)
{
	if (p->hdr.code == LVN_GETDISPINFO)
		pList->OnGetDispInfo((LPARAM)p);
	else if (p->hdr.code == NM_CLICK)
	{
		if (p->iItem == -1)
			pList->SelectItem(-1);
	}
	else if (pList == &m_lvPosition)
	{
		if (p->hdr.code == NM_DBLCLK && p->iItem!=-1)
		{
			Cover(p->iItem);
		}
	}
}


void TradeInfo::OnClickBtn(WPARAM wParam, LPARAM lParam)
{
	int index = m_lvPosition.GetSelectItem();
	switch (lParam)
	{
	case id_btnCover33:
		Cover(index, 0.33333);
		break;
	case id_btnCover50:
		Cover(index, 0.5);
		break;
	case id_btnCover100:
		Cover(index); //默认是1.0
		break;
	case id_btnCoverAll:
		CoverAll();
		break;
	}
}

void TradeInfo::Cover(int index, double coverPercent)
{
 	if (index >= 0 && index < m_totalPosition.size())
	{
		TSendOrder stOrder;
		char szText[100] = { 0 };
		for (uint i = 0; i < m_totalPosition[index].postions.size(); i++)
		{
			const TPosition* pPosition = m_totalPosition[index].postions[i];
			uint iQty = ceil(pPosition->PositionQty*coverPercent); //向上取整
			if(iQty<1)continue;
			CConvert::DefaultDealClose(pPosition, stOrder);
			stOrder.OrderQty = iQty;

			CConvert::GetPrice(&stOrder, stOrder.Direct, ptMarketPrice, stOrder.OrderPrice);
			strcpy_s(stOrder.Remark, RemarkCoverByUser);

			stOrder.Offset = oCover;
		
			CConvert::CoverPos(pPosition, stOrder, iQty);

			if (stOrder.Direct == dSell)
				sprintf_s(szText, "账号%s，主动对%s卖出平仓，%d手", pPosition->UserNo, &m_totalPosition[index].Key[1], stOrder.OrderQty);
			else if (stOrder.Direct==dBuy)
				sprintf_s(szText, "账号%s，主动对%s买入平仓，%d手", pPosition->UserNo, &m_totalPosition[index].Key[1], stOrder.OrderQty);

			g_pFollowTrader->AddLog(TFollowTrade::coverByUser, szText, NULL);
		}
	}
}

void TradeInfo::CoverAll()
{
	int iSize = m_totalPosition.size();
	for (int i = 0; i < iSize; i++)
	{
		Cover(i);
	}
}

void TradeInfo::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord12);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_white);
	InflateRect(&rect, -2, 0);
	//FrameRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_gray);

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(30, 30, 30));
	SelectObject(memdc.GetHdc(), hPen);
	MoveToEx(memdc.GetHdc(), 8, labOrderStateBottom + 2, 0);
	LineTo(memdc.GetHdc(), frameWidth - 8, labOrderStateBottom + 2);
	DeleteObject(hPen);

	DrawTabBtn(memdc.GetHdc());
}

void TradeInfo::DrawTabBtn(HDC hdc)
{
	TCHAR szText[][20] = { L"委托", L"持仓汇总" };
	for (int i = 0; i < m_vecTabBtnRc.size(); i++)
	{
		if (i == m_iNowSelTab)
		{
			FillRect(hdc, &m_vecTabBtnRc[i], g_ColorRefData.g_brush_white);
			SetTextColor(hdc, g_ColorRefData.g_ColorBlack);
		}
		else
		{
			FillRect(hdc, &m_vecTabBtnRc[i], g_ColorRefData.g_brush_gray);
			SetTextColor(hdc, g_ColorRefData.g_ColorWhite);
		}
		DrawText(hdc, szText[i], -1, &m_vecTabBtnRc[i], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	RECT rc = m_vecTabBtnRc[m_vecTabBtnRc.size() - 1]; 
	rc.left = rc.right;
	rc.right = frameWidth -6;
	FillRect(hdc, &rc, g_ColorRefData.g_brush_gray);
}

void TradeInfo::PushBackOrder(const TOrder*pOrder)
{
	TUser stUser = { 0 };
	strcpy_s(stUser.UserNo, pOrder->UserNo);
	list<const void*>::iterator iter;
	UINT i = 0;
	list<const void*> *pOrderVector = NULL;
	TListView * pListView = NULL;
	if (find(m_pMainUser->begin(), m_pMainUser->end(), stUser) != m_pMainUser->end())
	{
		pOrderVector = &m_mainOrder;
		pListView = &m_lvMainOrder;
	}
	else if (find(m_pFollowUser->begin(), m_pFollowUser->end(), stUser) != m_pFollowUser->end())
	{
		pOrderVector = &m_followOrder;
		pListView = &m_lvFollowOrder;
	}
	else //如果不在主账号也不在跟单账号中，则直接返回
	{
		return;
	}
	for (iter = pOrderVector->begin(); iter != pOrderVector->end(); iter++)
	{
		if (*iter == pOrder)
		{
			char szText[50] = { 0 };
			TOrderState2Char(pOrder->OrderState, pOrder->StrategyType, szText, sizeof(szText));
			if (pListView == &m_lvMainOrder)
				pListView->AddItem(szText, i, 1, DT_CENTER, SetOrderStateColor(pOrder->OrderState));
			else
				pListView->AddItem(szText, i, 2, DT_CENTER, SetOrderStateColor(pOrder->OrderState));
			pListView->SetItemCount(pOrderVector->size());
			pListView->SelectItem(i);
			return;
		}
		i++;
	}
	pOrderVector->push_back(pOrder); //之前是 push_front
	UpdateOrderState(pOrder, pListView, pOrderVector->size() - 1);
	pListView->SetItemCount(pOrderVector->size());
}

void TradeInfo::UpdateOrderState(const TOrder* pOrder, TListView *listView, int iRow)
{
	char szText[50] = { 0 };
	int iCol = 0;
	//iRow 最新的在最下面
	if (listView == &m_lvFollowOrder)
		listView->AddItem(pOrder->UserNo, iRow, iCol++);
	sprintf_s(szText, "%s%s", pOrder->CommodityNo, pOrder->ContractNo);
	listView->AddItem(szText, iRow, iCol++, DT_CENTER);
	TOrderState2Char(pOrder->OrderState, pOrder->StrategyType, szText, sizeof(szText));
	listView->AddItem(szText, iRow, iCol++, DT_CENTER, SetOrderStateColor(pOrder->OrderState));
	TDirect2Char(pOrder->Direct, szText, sizeof(szText));
	listView->AddItem(szText, iRow, iCol++, DT_CENTER, SetDirectColor(pOrder->Direct));
	TOffset2Char(pOrder->Offset, szText, sizeof(szText));
	listView->AddItem(szText, iRow, iCol++);
	sprintf_s(szText, "%.1f", pOrder->OrderPrice);
	listView->AddItem(szText, iRow, iCol++);
	sprintf_s(szText, "%d", pOrder->OrderQty);
	listView->AddItem(szText, iRow, iCol++, DT_CENTER);
}

void TradeInfo::SetUser(vector<TUser>* pMain, vector<TFollowUser>* pFollow)
{
	m_pMainUser = pMain;
	m_pFollowUser = pFollow;
}

void TradeInfo::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	if (p&&p->CtlID == id_lvMainOrder)
		m_lvMainOrder.OnDrawItem(lParam);
	else if (p&&p->CtlID == id_lvFollowOrder)
		m_lvFollowOrder.OnDrawItem(lParam);
	else if (p&&p->CtlID == id_lvPosition)
		m_lvPosition.OnDrawItem(lParam);
}
void TradeInfo::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	if (p&&p->CtlID == id_lvMainOrder)
		m_lvMainOrder.OnMeasureItem(p->itemHeight);
	else if (p&&p->CtlID == id_lvFollowOrder)
		m_lvFollowOrder.OnMeasureItem(p->itemHeight);
	else if (p&&p->CtlID == id_lvPosition)
		m_lvPosition.OnMeasureItem(p->itemHeight);
}

void TradeInfo::OnPosition()
{
	m_bAfterPosition = true;

	if (m_iNowSelTab == 0) return;
	GetPosition();
	UpdatePosition();
}