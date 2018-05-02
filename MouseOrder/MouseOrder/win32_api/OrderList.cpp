#include "PreInclude.h"

extern G_COLOREF	g_ColorRefData;
extern G_UIFONT		g_FontData;
//extern IQuoteApi	*g_QuoteApi;
extern IStarApi     *g_pStarApi;
extern ITradeApi	*g_pTradeApi;
extern ITradeData	*g_pTradeData;
extern TMouseConfig  g_MouseOrderStyle;
extern ICommonModule	*g_pCommonModule;


#define N_Arrow_Width	8
#define N_Last_Count	20
LRESULT OrderListEditWndProc(HWND hwnd, UINT message, WPARAM w, LPARAM l)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	COrderList *p_orderlist = (COrderList *)GetWindowLong(hwnd, GWL_USERDATA);
	if (!p_orderlist)
		return FALSE;

	if (WM_CHAR == message)
	{
		switch (id)
		{
		case LII_Tick_Edit:
			if ((w >= '0'&&w <= '9') || w == '\b' || w == '-')
			{
				char sText[20] = { 0 };
				GetWindowTextA(hwnd, sText, 20);
				string str = sText;
				if (string::npos != str.find('-') && w == '-')
					return 1;
				break;
			}
			else
				return FALSE;
		default:
			return FALSE;
		}
	}
	else if (WM_KEYDOWN==message)
	{	
		switch (w)
		{
		case VK_UP:
			p_orderlist->OnSpinDeltaUpDownNormal(1, id);
			break;
		case VK_DOWN:
			p_orderlist->OnSpinDeltaUpDownNormal(-1, id);
			break;
		}		
	}
	
	return CallWindowProc(p_orderlist->m_SetEdit, hwnd, message, w, l);
}

COrderList::COrderList() 
	: m_nScrollOffset(0)
	, m_bTracking(false)
	, m_nListHeight(N_HEIGHT_LIST_MIN)
	, m_BaseMidType(BMT_Last)
	, m_bInner(true)
	, m_nStopType(Tst_STP)
	, m_OrderQty(1)
	, m_bCreate(false)
{
	memset(m_DataAxis, 0, sizeof(m_DataAxis));
	m_DataAxis[DPT_VBase].nId = -1;
	memset(&m_ItemClickL, 0, sizeof(m_ItemClickL));
	memset(&m_ItemClickR, 0, sizeof(m_ItemClickR));
	memset(m_nCanCancelQty, 0, sizeof(m_nCanCancelQty));

	ResetHoverState();
}
COrderList::~COrderList()
{

}
void COrderList::SetColorStyle(bool bUseFor,bool bBlack)
{
	if (bUseFor)
	{
		m_Color[N_Buy][CL_HighLight] = RGB_Green_W;
		m_Color[N_Sell][CL_HighLight] = RGB_Red_W;
		m_Color[N_Buy][CL_LowLight] = RGB_Green_B;
		m_Color[N_Sell][CL_LowLight] = RGB_Red_B;
	}
	else
	{
		m_Color[N_Buy][CL_HighLight] = RGB_Red_W;
		m_Color[N_Sell][CL_HighLight] = RGB_Green_W;
		//m_Color[N_Buy][CL_LowLight] = RGB_Red_B;
		//m_Color[N_Sell][CL_LowLight] = RGB_Green_B;
        m_Color[N_Buy][CL_LowLight] = g_ColorRefData.GetColorWhite();
        m_Color[N_Sell][CL_LowLight] = g_ColorRefData.GetColorWhite();
	}
	//if (bBlack)//黑色系 白色系
	//{	
	//	if (bUseFor)
	//	{
	//		m_Color[N_Buy][CL_HighLight] = RGB_Green;
	//		m_Color[N_Sell][CL_HighLight] = RGB_Red;
	//	}
	//	else
	//	{
	//		m_Color[N_Buy][CL_HighLight] = RGB_Red;
	//		m_Color[N_Sell][CL_HighLight] = RGB_Green;
	//	}
	//	m_ExColor[CS_White] = g_ColorRefData.GetColorWhite();
	//	m_ExColor[CS_BKSpecial] = RGB_Black70;
	//	m_ExColor[CS_BKLine] = m_ExColor[CS_BKSpecial];
	//	m_ExColor[CS_PrcFG] = g_ColorRefData.GetColorWhite();
	//	m_ExColor[CS_BkMid] = g_ColorRefData.GetColorBlack();
	//	m_ExColor[CS_FGSpecial] = g_ColorRefData.GetColorWhite();
	//	m_ExColor[CS_LastPrice] = RGB_Yellow;
	//}
	//else
	{
        m_ExColor[CS_Black] = g_ColorRefData.GetColorBlack();
		m_ExColor[CS_White] = g_ColorRefData.GetColorWhite(); 
		m_ExColor[CS_BKSpecial] = g_ColorRefData.GetColorBackground();
		m_ExColor[CS_BKLine] =  RGB_WhiteSpecialBk;
		m_ExColor[CS_PrcFG] = g_ColorRefData.GetColorBlack();
		m_ExColor[CS_BkMid] = g_ColorRefData.GetColorWhite();
		m_ExColor[CS_FGSpecial] = RGB(0,80,152); 
		m_ExColor[CS_LastPrice] = RGB_Yellow_W;
	}

    DeleteBrushObject();
    CreateBrushObject();
   
}
void COrderList::SetChildRectContent()
{
	for (int m = 0; m < N_HEIGHT_LIST / N_HEIGHT_ITEM_LIST; m++)
	{
		m_ItemRect[DGC_LEFT][m].left = 1;
		m_ItemRect[DGC_LEFT][m].right = N_WIDTH_LIST / N_AVERAGE;
		m_ItemRect[DGC_MID][m].left = N_WIDTH_LIST / N_AVERAGE + 1;
		m_ItemRect[DGC_MID][m].right = m_ItemRect[DGC_MID][m].left + N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE * 2 - 1;
		m_ItemRect[DGC_RIGHT][m].left = N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE + 1;
		m_ItemRect[DGC_RIGHT][m].right = N_WIDTH_LIST - 1;

		m_ItemRect[DGC_LEFT][m].top = N_HEIGHT_ITEM_LIST*m + 1;
		m_ItemRect[DGC_LEFT][m].bottom = N_HEIGHT_ITEM_LIST*(m + 1);
		m_ItemRect[DGC_MID][m].top = m_ItemRect[DGC_LEFT][m].top;
		m_ItemRect[DGC_MID][m].bottom = m_ItemRect[DGC_LEFT][m].bottom;
		m_ItemRect[DGC_RIGHT][m].top = m_ItemRect[DGC_LEFT][m].top;
		m_ItemRect[DGC_RIGHT][m].bottom = m_ItemRect[DGC_LEFT][m].bottom;
	}
	//
	m_ItemRectSpecial[DSI_MarketBuy].rect.left = 1;
	m_ItemRectSpecial[DSI_MarketBuy].rect.right = N_WIDTH_LIST / N_AVERAGE;
	m_ItemRectSpecial[DSI_MarketBuy].rect.top = 1;
	m_ItemRectSpecial[DSI_MarketBuy].rect.bottom = N_HEIGHT_ITEM_LIST;
	wcscpy_s(m_ItemRectSpecial[DSI_MarketBuy].wtext, LoadResWchar_t(IDS_MarketBuy));

	m_ItemRectSpecial[DSI_MarketSell].rect = m_ItemRectSpecial[DSI_MarketBuy].rect;
	m_ItemRectSpecial[DSI_MarketSell].rect.left = N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE + 1;
	m_ItemRectSpecial[DSI_MarketSell].rect.right = N_WIDTH_LIST - 1;
	wcscpy_s(m_ItemRectSpecial[DSI_MarketSell].wtext, LoadResWchar_t(IDS_MarketSell));
	for (int i = DSI_Mid_1; i <= DSI_Mid_4; i++)
	{
		m_ItemRectSpecial[i].rect = m_ItemRectSpecial[DSI_MarketSell].rect;
        m_ItemRectSpecial[i].rect.left = N_WIDTH_LIST / N_AVERAGE + 1 + ((N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE * 2) / 4 + 1) * (i - DSI_Mid_1);//
        m_ItemRectSpecial[i].rect.right = m_ItemRectSpecial[i].rect.left + (N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE * 2) / 4 + ((i == DSI_Mid_4) ? -1 : 0);
	}
	wcscpy_s(m_PopWinMidStr[0][BMT_Last], LoadResWchar_t(IDS_LastMid));
	wcscpy_s(m_PopWinMidStr[1][BMT_Last], LoadResWchar_t(IDS_LastMidEx));
	wcscpy_s(m_PopWinMidStr[0][BMT_Bid1], LoadResWchar_t(IDS_Bid1Mid));
	wcscpy_s(m_PopWinMidStr[1][BMT_Bid1], LoadResWchar_t(IDS_Bid1MidEx));
	wcscpy_s(m_PopWinMidStr[0][BMT_Ask1], LoadResWchar_t(IDS_Ask1Mid));
	wcscpy_s(m_PopWinMidStr[1][BMT_Ask1], LoadResWchar_t(IDS_Ask1MidEx));
	wcscpy_s(m_PopWinMidStr[0][BMT_Hand], LoadResWchar_t(IDS_Hand));
	wcscpy_s(m_PopWinMidStr[1][BMT_Hand], LoadResWchar_t(IDS_Hand));
	wcscpy_s(m_PopWinStopStr[0][Tst_STP], L"STP");
	wcscpy_s(m_PopWinStopStr[1][Tst_STP], LoadResWchar_t(IDS_STP_Loss));
	wcscpy_s(m_PopWinStopStr[0][Tst_STL], L"STL");
	wcscpy_s(m_PopWinStopStr[1][Tst_STL], LoadResWchar_t(IDS_STL_Loss));
	wcscpy_s(m_PopWinStopStr[0][Tst_LMT], L"LMT");
	wcscpy_s(m_PopWinStopStr[1][Tst_LMT], LoadResWchar_t(IDS_LMT));

	UpdateMidHead();

	//子控件 创建
	m_EditExPoint.Create(LII_Tick_Edit_Win, LII_Tick_Edit, m_Hwnd, m_ItemRectSpecial[DSI_Mid_4].rect.left, m_ItemRectSpecial[DSI_Mid_4].rect.top + 5,
		m_ItemRectSpecial[DSI_Mid_4].rect.right - m_ItemRectSpecial[DSI_Mid_4].rect.left, 
        m_ItemRectSpecial[DSI_Mid_4].rect.bottom - m_ItemRectSpecial[DSI_Mid_4].rect.top - 10, 
        LoadResWchar_t(IDS_STRING1160), false);
	SendMessage(m_EditExPoint.GetEditWnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx17(), (LPARAM)TRUE);
	SendMessage(m_EditExPoint.GetEditWnd(), EM_SETLIMITTEXT, 3, 0);
	
    m_EditExPoint.UpdateFont();
	m_SetEdit = (WNDPROC)SetWindowLong(m_EditExPoint.GetEditWnd(), GWL_WNDPROC, (LONG)OrderListEditWndProc);
	SetWindowLong(m_EditExPoint.GetEditWnd(), GWL_USERDATA, LONG(this));//用户数据
}

void COrderList::CreateWin(CMousewin* opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, bool bUseFor)//创建弹出窗口
{
	m_opr = opr;

	//设置风格
    ResetColorStyle(bUseFor, g_MouseOrderStyle.nOrderListStyle);
    CreateBrushObject();

	CreateFrm(classname, parent, style, exstyle);
		
	//设置子区域 坐标 信息
	SetChildRectContent();
	//
	MoveWindow(m_Hwnd, nleft, ntop, N_WIDTH_LIST, m_nListHeight, TRUE);

	m_bCreate = true;
}
void COrderList::UpdateMidHead()
{
	if (m_bInner)
	{
		wcscpy_s(m_ItemRectSpecial[DSI_Mid_1].wtext, m_PopWinMidStr[0][BMT_Last]);
		wcscpy_s(m_ItemRectSpecial[DSI_Mid_2].wtext, m_PopWinMidStr[0][BMT_Bid1]);
		wcscpy_s(m_ItemRectSpecial[DSI_Mid_3].wtext, m_PopWinMidStr[0][BMT_Ask1]);
		wcscpy_s(m_ItemRectSpecial[DSI_Mid_4].wtext, m_PopWinMidStr[0][BMT_Hand]);
		if (IsWindowVisible(m_EditExPoint.GetHwnd()))
			ShowWindow(m_EditExPoint.GetHwnd(), SW_HIDE);
	}															   
	else														   
	{															   
		switch (m_BaseMidType)
		{
		case BMT_Bid1:
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_1].wtext, m_PopWinMidStr[0][BMT_Bid1]);
			break;
		case BMT_Ask1:
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_1].wtext, m_PopWinMidStr[0][BMT_Ask1]);
			break;
		case BMT_Hand:
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_1].wtext, m_PopWinMidStr[0][BMT_Hand]);
			break;
		case BMT_Last:
		default:
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_1].wtext, m_PopWinMidStr[0][BMT_Last]);
			break;
		}
		wcscpy_s(m_ItemRectSpecial[DSI_Mid_2].wtext, L"Home");
		if (m_nStopType == Tst_LMT)
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_3].wtext, m_PopWinStopStr[0][Tst_LMT]);
		else if (m_nStopType == Tst_STL)
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_3].wtext, m_PopWinStopStr[0][Tst_STL]);
		else 
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_3].wtext, m_PopWinStopStr[0][Tst_STP]);
	
		ShowWindow(m_EditExPoint.GetHwnd(), m_nStopType == Tst_STL?SW_SHOW:SW_HIDE);
	}
}
void COrderList::UpdateLastPriceData()
{
	/*for (list<DataPriceID>::iterator iter = m_listLastPriceId.begin(); iter != m_listLastPriceId.end(); iter++)
		iter->nId = DivResult((m_DataAxis[DPT_VBase].dPrice - iter->dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));

	list<DataPriceID>::reverse_iterator iter = m_listLastPriceId.rbegin();
	if (iter != m_listLastPriceId.rend())
	{
		if (m_DataAxis[DPT_Last].nId != iter->nId)
			m_listLastPriceId.push_back(m_DataAxis[DPT_Last]);
	}
	else
		m_listLastPriceId.push_back(m_DataAxis[DPT_Last]);

	while (m_listLastPriceId.size() > N_Last_Count)
		m_listLastPriceId.pop_front();*/

	if (!CConvert::DoubleSameValue(m_OldLast, m_DataAxis[DPT_Last].dPrice, FLOAT_SAME_ERRORDIFF))
	{
		m_LastUpdown = m_DataAxis[DPT_Last].dPrice > m_OldLast ? LastPrice_Up : LastPrice_Down;
		m_OldLast = m_DataAxis[DPT_Last].dPrice;
	}
	else
		m_LastUpdown = LastPrice_Keep;
}

LRESULT COrderList::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:				OnCreate(wParam, lParam);
		break;
	case WM_COMMAND:			OnCommond(wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:		return OnCtlColorStatic(wParam, lParam);

	case WM_PAINT:				OnPaint(wParam, lParam);
		break;
	case WM_SIZE:				OnSize(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:			OnLButtonUp(wParam, lParam);
		break;
	case WM_RBUTTONDOWN:		OnRButtonDown(wParam, lParam);
		break;
	case WM_RBUTTONUP:			OnRButtonUp(wParam, lParam);
		break;
	case WM_MOUSEMOVE:			OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSEHOVER:			OnMouseHover(wParam, lParam);
		break;
	case WM_MOUSELEAVE:			OnMouseLeave(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:			OnMouseWheel(wParam, lParam);
		break;
	case WM_KEYDOWN:			OnKeyDown(wParam, lParam);
		break;
	case WM_KILLFOCUS:			OnKillFocus(wParam, lParam);
		break;
	case WM_ERASEBKGND:
		break;
	case WM_DESTROY:			OnDestory(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}
bool COrderList::IsActive()
{
	return m_bCreate&&m_opr&&m_opr->IsLadderLayOut();
}
void COrderList::OnCreate(WPARAM wParam, LPARAM lParam)
{
	//画刷 
	for (int n = N_Buy; n < N_Sell + 1;n++)
		for (int m = CL_HighLight; m < CL_Size; m++)
			m_Brush[n][m] = CreateSolidBrush(m_Color[n][m]);
	for (int n = CS_White; n < CS_Size; n++)
		m_ExBrush[n] = CreateSolidBrush(m_ExColor[n]);
	
	//创建缓存DC 
	HDC hdc = GetDC(m_Hwnd);
	m_HdcMem = CreateCompatibleDC(hdc);// bitmap
	m_hBitmap = CreateCompatibleBitmap(hdc, N_WIDTH_LIST, m_nListHeight);
	SelectObject(m_HdcMem, m_hBitmap);
	ReleaseDC(m_Hwnd, hdc);
}
void COrderList::OnCommond(WPARAM wParam, LPARAM lParam)
{
	
}
void COrderList::OnDestory(WPARAM wParam, LPARAM lParam)
{
	for (int n = N_Buy; n < N_Sell + 1; n++)
		for (int m = CL_HighLight; m < CL_Size; m++)
			DeleteObject(m_Brush[n][m]);

	for (int n = CS_White; n < CS_Size; n++)
		DeleteObject(m_ExBrush[n]);

	DeleteObject(m_hBitmap);
	DeleteDC(m_HdcMem);
    DeleteBrushObject();
}
void COrderList::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	if (rect.bottom - rect.top < N_HEIGHT_LIST_MIN)
		m_nListHeight = N_HEIGHT_LIST_MIN;
	//else if (rect.bottom - rect.top > N_HEIGHT_LIST)
	//	m_nListHeight = N_HEIGHT_LIST;
	else
		m_nListHeight = (rect.bottom - rect.top) / N_HEIGHT_ITEM_LIST * N_HEIGHT_ITEM_LIST;

	m_ItemRectSpecial[DSI_CancelB] = m_ItemRectSpecial[DSI_MarketBuy];
	m_ItemRectSpecial[DSI_CancelB].rect.top = m_nListHeight - N_HEIGHT_ITEM_LIST + 1;
	m_ItemRectSpecial[DSI_CancelB].rect.bottom = m_nListHeight - 1;
	wcscpy_s(m_ItemRectSpecial[DSI_CancelB].wtext, LoadResWchar_t(IDS_CancelB));
	m_ItemRectSpecial[DSI_CancelS].rect = m_ItemRectSpecial[DSI_MarketSell].rect;
	m_ItemRectSpecial[DSI_CancelS].rect.top = m_nListHeight - N_HEIGHT_ITEM_LIST + 1;
	m_ItemRectSpecial[DSI_CancelS].rect.bottom = m_nListHeight - 1;
	wcscpy_s(m_ItemRectSpecial[DSI_CancelS].wtext, LoadResWchar_t(IDS_CancelS));
	m_ItemRectSpecial[DSI_CancelA].rect = m_ItemRectSpecial[DSI_CancelB].rect;
	m_ItemRectSpecial[DSI_CancelA].rect.left = N_WIDTH_LIST / N_AVERAGE + 1;
	m_ItemRectSpecial[DSI_CancelA].rect.right = N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE;
	wcscpy_s(m_ItemRectSpecial[DSI_CancelA].wtext, LoadResWchar_t(IDS_CancelA));

	m_nLine = m_nListHeight / N_HEIGHT_ITEM_LIST - 2;

	m_ListRect.left = 1;
	m_ListRect.right = N_WIDTH_LIST - 1;
	m_ListRect.top = N_HEIGHT_ITEM_LIST;
	m_ListRect.bottom = m_ListRect.top + N_HEIGHT_ITEM_LIST*m_nLine+1;
	m_HeadRect = m_FooterRect = m_ListRect;
	m_HeadRect.top = 1;
	m_HeadRect.bottom = N_HEIGHT_ITEM_LIST - 1;
	m_FooterRect.top = m_ListRect.bottom;
	m_FooterRect.bottom = m_ListRect.bottom + N_HEIGHT_ITEM_LIST;

	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	HDC hdc = GetDC(m_Hwnd);
	m_hBitmap = CreateCompatibleBitmap(hdc, N_WIDTH_LIST, m_nListHeight);
	SelectObject(m_HdcMem, m_hBitmap);
	ReleaseDC(m_Hwnd, hdc);

	UpdatePrice();
}
void COrderList::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(m_Hwnd, &ps);
	RECT rect;
	::GetClientRect(m_Hwnd, &rect);

	//背景
	RECT rectpart(rect);
	rectpart.left = rectpart.left + N_WIDTH_LIST / N_AVERAGE;
	rectpart.right = rectpart.left + N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE * 2;
    FillRect(m_HdcMem, &rectpart, m_BrushLine);

	RECT rectleft(rect);
	rectleft.right = rectleft.left + N_WIDTH_LIST / N_AVERAGE;
    FillRect(m_HdcMem, &rectleft, m_BrushBgBuy);

	RECT rectright(rect);
	rectright.left = rectright.left + N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE;
    FillRect(m_HdcMem, &rectright, m_BrushBgSell);

	//顶部 底部
	RECT rectpart2(rectpart);
	rectpart2.top = N_HEIGHT_ITEM_LIST;
	rectpart2.bottom = rectpart2.bottom - N_HEIGHT_ITEM_LIST;

    if (m_DataAxis[DPT_VBase].nId != 0)
    {
        FillRect(m_HdcMem, &rectpart2, m_BrushBackground);
    }
    else
    {
        FillRect(m_HdcMem, &rectpart2, m_BrushLine);
    }    

	//网格
	HPEN hPen_level2 = CreatePen(PS_SOLID, 1, m_ColorLine);
	HPEN OldPen = (HPEN)SelectObject(m_HdcMem, hPen_level2);
	
	FrameRect(m_HdcMem, &rect, m_BrushLine);
	
	MoveToEx(m_HdcMem, rect.left, rect.top+ N_HEIGHT_ITEM_LIST, NULL);
	LineTo(m_HdcMem, rect.right, rect.top + N_HEIGHT_ITEM_LIST);
	MoveToEx(m_HdcMem, rect.left, rect.bottom - N_HEIGHT_ITEM_LIST, NULL);
	LineTo(m_HdcMem, rect.right, rect.bottom - N_HEIGHT_ITEM_LIST);

	MoveToEx(m_HdcMem, rectpart.left, rectpart.top, NULL);
	LineTo(m_HdcMem, rectpart.left, rectpart.bottom);
	MoveToEx(m_HdcMem, rectpart.right, rectpart.top, NULL);
	LineTo(m_HdcMem, rectpart.right, rectpart.bottom);
	
	SelectObject(m_HdcMem, OldPen);
	DeleteObject(hPen_level2);
	SetBkMode(m_HdcMem, TRANSPARENT);

    DrawItem(m_HdcMem);

    SelectObject(m_HdcMem, g_FontData.GetFontNumEx22());

    //挂单
    DrawItemQty(m_HdcMem);
	
	//特殊
	DrawItemSpecial(m_HdcMem);

	BitBlt(hDC, 0, 0, N_WIDTH_LIST, m_nListHeight, m_HdcMem, 0, 0, SRCCOPY);

	EndPaint(m_Hwnd, &ps);
}

LONG COrderList::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LONG)g_ColorRefData.GetHbrBackground();
}
void COrderList::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (::GetFocus() != m_Hwnd)
		SetFocus(m_Hwnd);
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	
	memset(&m_ItemClickL, 0, sizeof(m_ItemClickL));
	for (int n = DSI_MarketBuy; n < DSI_Size;n++)
	{
		if (PtInRect(&m_ItemRectSpecial[n].rect,pointB))
		{
			m_ItemClickL.nSpecial = n;
			m_ItemClickL.nType = CT_Special;
			break;
		}		
	}
	for (int m = 1; m < m_nLine + 1;m++)
	{
		if (PtInRect(&m_ItemRect[DGC_LEFT][m], pointB))
		{
			m_ItemClickL.nCol = DGC_LEFT;
			m_ItemClickL.nRow = m;
			m_ItemClickL.nType = CT_BS;
			break;
		}
		else if (PtInRect(&m_ItemRect[DGC_RIGHT][m], pointB))
		{
			m_ItemClickL.nCol = DGC_RIGHT;
			m_ItemClickL.nRow = m;
			m_ItemClickL.nType = CT_BS;
			break; 
        else if (PtInRect(&m_ItemRect[DGC_MID][m], pointB))
        {
            m_SpecialItem[DS_Select].nCol = DGC_MID;
            m_SpecialItem[DS_Select].nRow = m;
        }
	}
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);
}
void COrderList::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	if (m_ItemClickL.nType != CT_None)
	{
		if (m_ItemClickL.nType==CT_Special)
		{
			if (PtInRect(&m_ItemRectSpecial[m_ItemClickL.nSpecial].rect, pointB))
			{
				OnClickSpecial(m_ItemClickL.nSpecial);
			}
		}
		else if (m_ItemClickL.nType == CT_BS)
		{
			if (PtInRect(&m_ItemRect[m_ItemClickL.nCol][m_ItemClickL.nRow], pointB))
			{
				OnClickBuySell(m_ItemClickL.nCol, m_ItemClickL.nRow);
			}
		}
	}
	memset(&m_ItemClickL, 0, sizeof(m_ItemClickL));
	InvalidateRect(m_Hwnd,NULL,TRUE);
	UpdateWindow(m_Hwnd);
}
void COrderList::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);

	memset(&m_ItemClickR, 0, sizeof(m_ItemClickR));
	for (int m = 1; m < m_nLine + 1; m++)
	{
		if (PtInRect(&m_ItemRect[DGC_LEFT][m], pointB))
		{
			m_ItemClickR.nCol = DGC_LEFT;
			m_ItemClickR.nRow = m;
			m_ItemClickR.nType = CT_BS;
			break;
		}
		else if (PtInRect(&m_ItemRect[DGC_RIGHT][m], pointB))
		{
			m_ItemClickR.nCol = DGC_RIGHT;
			m_ItemClickR.nRow = m;
			m_ItemClickR.nType = CT_BS;
			break;
		}
	}
}

void COrderList::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	if (m_ItemClickR.nType != CT_None && m_bRightCancel)
	{
		if (m_ItemClickR.nType == CT_BS)
		{
			if (PtInRect(&m_ItemRect[m_ItemClickR.nCol][m_ItemClickR.nRow], pointB))
				OnRCancelOrder(m_ItemClickR.nCol, m_ItemClickR.nRow);
		}
	}
	memset(&m_ItemClickR, 0, sizeof(m_ItemClickR));
}
void COrderList::UpdateRect(ItemRowCol &item)
{
	if (item.nCol != DGC_MID)
	{
		InvalidateRect(m_Hwnd, &m_ItemRect[item.nCol][item.nRow], TRUE);
		if (item.nRow != 0 && item.nRow != m_nLine + 1)
			InvalidateRect(m_Hwnd, &m_ItemRect[DGC_MID][item.nRow], TRUE);
	}
	else
	{
		if (item.nRow == 0 && item.nSubItem!=-1)
		{
			if (!m_bInner && m_nStopType != Tst_STL && (item.nSubItem + DSI_Mid_1 == DSI_Mid_3 || item.nSubItem + DSI_Mid_1 == DSI_Mid_4))
			{
				RECT rect = m_ItemRectSpecial[DSI_Mid_3].rect;
				rect.right = m_ItemRectSpecial[DSI_Mid_4].rect.right;
				InvalidateRect(m_Hwnd, &rect, TRUE);
			}
			else
				InvalidateRect(m_Hwnd, &m_ItemRectSpecial[item.nSubItem + DSI_Mid_1].rect, TRUE);
		}	
		else
			InvalidateRect(m_Hwnd, &m_ItemRect[DGC_MID][item.nRow], TRUE);
	}
}
void COrderList::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	//记录前次激活位置
	m_SpecialItem[DS_LastHover] = m_SpecialItem[DS_Hover];
	m_SpecialItem[DS_Hover].nSubItem = -1;
	//更新本次坐标
	if (pointB.x < N_WIDTH_LIST / N_AVERAGE)
		m_SpecialItem[DS_Hover].nCol = DGC_LEFT;
	else if (pointB.x > N_WIDTH_LIST - N_WIDTH_LIST / N_AVERAGE)
		m_SpecialItem[DS_Hover].nCol = DGC_RIGHT;
	else
		m_SpecialItem[DS_Hover].nCol = DGC_MID;
	
	m_SpecialItem[DS_Hover].nRow = pointB.y / N_HEIGHT_ITEM_LIST;
	if (pointB.y % N_HEIGHT_ITEM_LIST==0)
		m_SpecialItem[DS_Hover].nRow = -1;
	//居中类型 特殊处理
	if (m_SpecialItem[DS_Hover].nRow == 0 && m_SpecialItem[DS_Hover].nCol == DGC_MID)
	{
		for (int n = DSI_Mid_1; n <= DSI_Mid_4; n++)
		{
			if (PtInRect(&m_ItemRectSpecial[n].rect, pointB))
			{
				m_SpecialItem[DS_Hover].nCol = DGC_MID;
				m_SpecialItem[DS_Hover].nSubItem = n - DSI_Mid_1;
				break;
			}
		}
	}
		
	//区域刷新
	if ((m_SpecialItem[DS_LastHover].nCol != m_SpecialItem[DS_Hover].nCol || m_SpecialItem[DS_LastHover].nRow != m_SpecialItem[DS_Hover].nRow) || m_SpecialItem[DS_LastHover].nSubItem!=m_SpecialItem[DS_Hover].nSubItem)
	{
		UpdateRect(m_SpecialItem[DS_Hover]);
		UpdateRect(m_SpecialItem[DS_LastHover]);
	}
	
	//鼠标跟踪
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_Hwnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 0;
		m_bTracking = _TrackMouseEvent(&tme) == TRUE;
	}
}
void COrderList::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	if (GetFocus()!=m_EditExPoint.GetEditWnd())
		SetFocus(m_Hwnd);
}
void COrderList::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = false;
	ResetHoverState(true);
	UpdateWindow(m_Hwnd);
}
void COrderList::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	if (GetFocus()==m_Hwnd)
	{
		if (!m_bTracking)
			return;
		int noffset = GET_WHEEL_DELTA_WPARAM(wParam);
		if (noffset > 0)
			m_nScrollOffset--;
		else
			m_nScrollOffset++;
		InvalidateRect(m_Hwnd, &m_ListRect, TRUE);
	}
	else if (GetFocus() == m_EditExPoint.GetEditWnd())
		OnSpinDeltaUpDownNormal(GET_WHEEL_DELTA_WPARAM(wParam), LII_Tick_Edit);
}
void COrderList::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
    m_SpecialItem[DS_Select].nRow = 0;
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);
}
void COrderList::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (!m_bTracking)
		return;
	switch (wParam)
	{
	case VK_UP:
		//if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Up].dPrice, FLOAT_SAME_ERRORDIFF))
		//{
		//	int nMin = m_DataAxis[DPT_Up].nId - m_DataAxis[DPT_VStart].nId;
		//	if (nMin < m_nScrollOffset - 1)
		//		m_nScrollOffset--;
		//	else
		//		m_nScrollOffset = nMin;
		//}
		//else
			m_nScrollOffset--;
		break;
	case VK_DOWN:
		//if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Down].dPrice, FLOAT_SAME_ERRORDIFF))
		//{
		//	int nMax = m_DataAxis[DPT_Down].nId - m_DataAxis[DPT_VEnd].nId;
		//	if (nMax > m_nScrollOffset + 1)
		//		m_nScrollOffset++;
		//	else
		//		m_nScrollOffset = nMax;
		//}
		//else
			m_nScrollOffset++;
		break;
    case VK_LEFT:
    {
        if (m_SpecialItem[DS_Select].nRow > 0)
        {
            OnClickBuySell(DGC_LEFT, m_SpecialItem[DS_Select].nRow);
        }
        break;
    }
    case VK_RIGHT:
    {
        if (m_SpecialItem[DS_Select].nRow > 0)
        {
            OnClickBuySell(DGC_RIGHT, m_SpecialItem[DS_Select].nRow);
        }
        break;
    }
	case VK_PRIOR:
		if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Up].dPrice, FLOAT_SAME_ERRORDIFF))
		{
			int nMin = m_DataAxis[DPT_Up].nId - m_DataAxis[DPT_VStart].nId;
			if (nMin < m_nScrollOffset - m_nLine)
				m_nScrollOffset = m_nScrollOffset - m_nLine;
			else
				m_nScrollOffset = nMin;
		}
		else
			m_nScrollOffset = m_nScrollOffset - m_nLine;
		break;
	case VK_NEXT:
		if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Down].dPrice, FLOAT_SAME_ERRORDIFF))
		{
			int nMax = m_DataAxis[DPT_Down].nId - m_DataAxis[DPT_VEnd].nId;
			if (nMax > m_nScrollOffset + m_nLine)
				m_nScrollOffset = m_nScrollOffset + m_nLine;
			else
				m_nScrollOffset = nMax;
		}
		else
			m_nScrollOffset = m_nScrollOffset + m_nLine;
		break;
	case VK_HOME:
		if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Up].dPrice, FLOAT_SAME_ERRORDIFF))
			m_nScrollOffset = m_DataAxis[DPT_Up].nId - m_DataAxis[DPT_VStart].nId;
		break;
	case VK_END:
		if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Down].dPrice, FLOAT_SAME_ERRORDIFF))
			m_nScrollOffset =  m_DataAxis[DPT_Down].nId - m_DataAxis[DPT_VEnd].nId;
		break;
	default:
		return;
	}
	InvalidateRect(m_Hwnd, &m_ListRect, TRUE);
}
void COrderList::DrawItemLeft(HDC hdc, int n, int m)						
{
	TMouFormatPriceStrW wtext = { 0 };
	if (!m_bInner&&m_nStopType != Tst_LMT&&m_DataAxis[DPT_Ask1].nQty > 0)
	{
		SetTextColor(hdc, m_ColorTextLmt);
		SelectObject(hdc, g_FontData.GetFontNum9());
		if (m_DataAxis[DPT_VStart].nId + m_nScrollOffset + m - 1 == m_DataAxis[DPT_Ask1].nId)
			swprintf_s(wtext, L"LMT");
		else if (m_DataAxis[DPT_VStart].nId + m_nScrollOffset + m - 1 == m_DataAxis[DPT_Ask1].nId - 1)
		{
			swprintf_s(wtext, m_nStopType == Tst_STP ? L"STP" : L"STL");
			DrawDiv(hdc, m_ItemRect[n][m].left, m_ItemRect[n][m].bottom, m_ItemRect[n][m].right - m_ItemRect[n][m].left);
		}
		DrawTextW(hdc, wtext, wcslen(wtext), &m_ItemRect[n][m], DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	}
	if (m_SpecialItem[DS_Hover].nCol == n && m_SpecialItem[DS_Hover].nRow == m && m*n >= 0)
	{
		SetTextColor(hdc, m_ColorTextHover);
		FillRect(hdc, &m_ItemRect[n][m], m_BrushBuy);
		RECT rect(m_ItemRect[n][m]);		
		//InflateRect(&rect, -1, -1);
		FrameRect(hdc, &rect, m_BrushHoverFrame);
		InflateRect(&rect, -1, -1);
		FrameRect(hdc, &rect, m_BrushWhite);
		if (m_ItemClickL.nType == CT_BS && n == m_ItemClickL.nCol && m == m_ItemClickL.nRow)
			SelectObject(hdc, g_FontData.GetFontWord15());
		else
			SelectObject(hdc, g_FontData.GetFontWord13());
		swprintf_s(wtext,L" %s %d", LoadResWchar_t(IDS_BuyList), m_OrderQty);
		DrawTextW(hdc, wtext, wcslen(wtext), &m_ItemRect[n][m], DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	}
}
void COrderList::DrawItemMid(HDC hdc, int n, int m)						
{
	TMouFormatPriceStrW wtext = { 0 };
	RECT rect(m_ItemRect[n][m]);
	int nCurId(m_DataAxis[DPT_VStart].nId + m_nScrollOffset + m - 1);

	//挂单价
    if (m_DataAxis[DPT_Ask1].nQty > 0 && nCurId <= m_DataAxis[DPT_Ask1].nId)//卖挂单
    {
        SetTextColor(hdc, m_ColorSell);
        FillRect(hdc, &rect, m_BrushBgAsk);
    }
    else if (m_DataAxis[DPT_Bid1].nQty > 0 && nCurId >= m_DataAxis[DPT_Bid1].nId)//买挂单
    {
        SetTextColor(hdc, m_ColorBuy);//买卖间 无挂单区间
        FillRect(hdc, &rect, m_BrushBgBid);
    }
    else
    {
        SetTextColor(hdc, m_ColorTextNormal);
        FillRect(hdc, &rect, m_BrushBackground);
    }
		

    if (m_DataAxis[DPT_Ask1].nQty > 0 && nCurId == m_DataAxis[DPT_Ask1].nId)
    {
        //HBRUSH green = CreateSolidBrush(RGB(224, 242, 223));
        FillRect(hdc, &rect, m_BrushBgAsk1);
    }
    else if (m_DataAxis[DPT_Bid1].nQty > 0 && nCurId == m_DataAxis[DPT_Bid1].nId)
    {
        //HBRUSH red = CreateSolidBrush(RGB(246, 226, 226));
        FillRect(hdc, &rect, m_BrushBgBid1);
    }
    //else
    //    FillRect(hdc, &rect, m_BrushBackground);

    if (m_SpecialItem[DS_Select].nRow == m && GetFocus() == m_Hwnd)
    {
        FillRect(hdc, &rect, m_BrushSelect);
    }

	//最新价
	if (m_DataAxis[DPT_Last].nId == nCurId)
		SetTextColor(hdc, m_ColorLastPrice);

	if (m_CommodityInfo.bIfFractionPrice)
		SelectObject(hdc, g_FontData.GetFontNumEx13());//分数
	else if ((m_DataAxis[DPT_Ask1].nQty > 0 && m_DataAxis[DPT_Ask1].nId == nCurId&& m_BaseMidType == BMT_Ask1) || (m_DataAxis[DPT_Bid1].nQty > 0 && m_DataAxis[DPT_Bid1].nId == nCurId&& m_BaseMidType == BMT_Bid1)
		|| (!CConvert::DoubleSameValue(m_DataAxis[DPT_Last].dPrice, 0, FLOAT_SAME_ERRORDIFF) && m_DataAxis[DPT_Last].nId == nCurId && m_BaseMidType == BMT_Last))
		SelectObject(hdc, g_FontData.GetFontNumEx22());//居中
	else
		SelectObject(hdc, g_FontData.GetFontNumEx17());//普通

	if (GetPriceStrByLineId(wtext, m))
	{
		//价格
		DrawTextW(hdc, wtext, wcslen(wtext), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		//涨跌箭头
		if (m_DataAxis[DPT_Last].nId == nCurId)
			DrawArrow(m_HdcMem, m_ItemRect[DGC_MID][m].left + 30, m_ItemRect[DGC_MID][m].top + 3, 11, 19);
	}

	//最高价 最低价 nQty用作更新标识
	if ((m_DataAxis[DPT_High].nQty > 0 && m_DataAxis[DPT_High].nId == nCurId))
		DrawHighLow(hdc, m_ItemRect[n - 1][m]);
	if((m_DataAxis[DPT_Low].nQty > 0 && m_DataAxis[DPT_Low].nId == nCurId))
		DrawHighLow(hdc, m_ItemRect[n - 1][m],false);
	
	//多空 持仓
	if ((m_DataAxis[DPT_VLong].nId == nCurId && m_DataAxis[DPT_VLong].nQty > 0) || (m_DataAxis[DPT_VShort].nId == nCurId && m_DataAxis[DPT_VShort].nQty > 0))
	{
		COLORREF ref = nCurId == m_DataAxis[DPT_VLong].nId ? m_ColorBuy : m_ColorSell;
		if (m_DataAxis[DPT_VLong].nId == m_DataAxis[DPT_VShort].nId && (m_DataAxis[DPT_VLong].nQty > m_DataAxis[DPT_VShort].nQty&&m_DataAxis[DPT_VShort].nQty > 0))
			ref = m_ColorHoverFrame;
		HPEN hPen_level3 = CreatePen(PS_SOLID, 1, ref);
		HPEN OldPen3 = (HPEN)SelectObject(hdc, hPen_level3);
		MoveToEx(hdc, rect.left, rect.bottom, NULL);
		LineTo(hdc, rect.right, rect.bottom);
		MoveToEx(hdc, rect.left, rect.bottom - 1, NULL);
		LineTo(hdc, rect.right, rect.bottom - 1);
		SelectObject(hdc, OldPen3);
		DeleteObject(hPen_level3);

		SetTextColor(hdc, m_ColorTextNormal);
		SelectObject(hdc, g_FontData.GetFontWord12());
		swprintf_s(wtext, LoadResWchar_t(IDS_Pos));
		DrawTextW(hdc, wtext, wcslen(wtext), &m_ItemRect[n + 1][m], DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
	}

	//边框
    if (m_SpecialItem[DS_Hover].nRow == m&&m_SpecialItem[DS_Hover].nCol != DGC_MID)
    {
        //FrameRect(hdc, &m_ItemRect[DGC_MID][m], m_BrushLastPrice);
        FrameRect(hdc, &m_ItemRect[DGC_MID][m], m_SpecialItem[DS_Hover].nCol == DGC_LEFT ? m_BrushBuy : m_BrushSell);
    }	
	else if (m_SpecialItem[DS_Hover].nCol == DGC_MID && m_SpecialItem[DS_Hover].nRow == m&&m_SpecialItem[DS_Hover].nRow != 0 && m_SpecialItem[DS_Hover].nRow != m_nLine + 2)
		FrameRect(hdc, &m_ItemRect[DGC_MID][m], m_BrushLastPrice);
}
void COrderList::DrawItemRight(HDC hdc, int n, int m)	
{
	TMouFormatPriceStrW wtext = { 0 };
	if (!m_bInner&&m_nStopType != Tst_LMT&&m_DataAxis[DPT_Bid1].nQty > 0)
	{
		SetTextColor(hdc, m_ColorTextLmt);
		SelectObject(hdc, g_FontData.GetFontNum9());
		if (m_DataAxis[DPT_VStart].nId + m_nScrollOffset + m - 1 == m_DataAxis[DPT_Bid1].nId)
		{
			swprintf_s(wtext, L"LMT");
			DrawDiv(hdc, m_ItemRect[n][m].left, m_ItemRect[n][m].bottom, m_ItemRect[n][m].right - m_ItemRect[n][m].left);
		}
		else if (m_DataAxis[DPT_VStart].nId + m_nScrollOffset + m - 1 == m_DataAxis[DPT_Bid1].nId + 1)
			swprintf_s(wtext, m_nStopType == Tst_STP ? L"STP" : L"STL");
		DrawTextW(hdc, wtext, wcslen(wtext), &m_ItemRect[n][m], DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
	}

	if (m_SpecialItem[DS_Hover].nCol == n && m_SpecialItem[DS_Hover].nRow == m && m*n >= 0)
	{
		SetTextColor(hdc, m_ColorTextHover);
		FillRect(hdc, &m_ItemRect[n][m], m_BrushSell);
		RECT rect(m_ItemRect[n][m]);
		//InflateRect(&rect, -1, -1);
		FrameRect(hdc, &rect, m_BrushHoverFrame);
		InflateRect(&rect, -1, -1);
		FrameRect(hdc, &rect, m_BrushWhite);


        //auto iter = m_LocalWorkData.find(m_DataAxis[DPT_VStart].nId + m - 1 + m_nScrollOffset);
        //if (iter != m_LocalWorkData.end())
        //{
        //    SetTextColor(hdc, m_ExColor[CS_White]);
        //    SelectObject(hdc, g_FontData.GetFontNumEx22());

        //    if (iter->second.nQtyNormal[N_Buy]>0 || iter->second.nQtyCondition[N_Buy]>0)
        //    {
        //        RECT rect(m_ItemRect[DGC_LEFT][iter->first - (m_DataAxis[DPT_VStart].nId + m_nScrollOffset) + 1]);
        //        rect.right = rect.right - 3;
        //        TMouFormatPriceStrW wtext = { 0 };
        //        if (iter->second.nQtyNormal[N_Buy] && iter->second.nQtyCondition[N_Buy])
        //            swprintf_s(wtext, L"%d/%d*", iter->second.nQtyNormal[N_Buy], iter->second.nQtyCondition[N_Buy]);
        //        else if (iter->second.nQtyNormal[N_Buy])
        //            swprintf_s(wtext, L"%d", iter->second.nQtyNormal[N_Buy]);
        //        else
        //            swprintf_s(wtext, L"%d*", iter->second.nQtyCondition[N_Buy]);
        //        DrawTextW(hdc, wtext, wcslen(wtext), &rect, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
        //    }
        //    if (iter->second.nQtyNormal[N_Sell]>0 || iter->second.nQtyCondition[N_Sell]>0)
        //    {
        //        RECT rect(m_ItemRect[DGC_RIGHT][iter->first - (m_DataAxis[DPT_VStart].nId + m_nScrollOffset) + 1]);
        //        rect.left = rect.left + 3;
        //        TMouFormatPriceStrW wtext = { 0 };
        //        if (iter->second.nQtyNormal[N_Sell] && iter->second.nQtyCondition[N_Sell])
        //            swprintf_s(wtext, L"%d/%d*", iter->second.nQtyNormal[N_Sell], iter->second.nQtyCondition[N_Sell]);
        //        else if (iter->second.nQtyNormal[N_Sell])
        //            swprintf_s(wtext, L"%d", iter->second.nQtyNormal[N_Sell]);
        //        else
        //            swprintf_s(wtext, L"%d*", iter->second.nQtyCondition[N_Sell]);
        //        DrawTextW(hdc, wtext, wcslen(wtext), &rect, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
        //    }

        //    SetTextColor(hdc, m_ExColor[CS_Black]);
        //    SelectObject(hdc, g_FontData.GetFontWord13());
        //}

        if (m_ItemClickL.nType == CT_BS && n == m_ItemClickL.nCol && m == m_ItemClickL.nRow)
            SelectObject(hdc, g_FontData.GetFontWord15());
        else
            SelectObject(hdc, g_FontData.GetFontWord13());
        swprintf_s(wtext, L"%s %d ", LoadResWchar_t(IDS_SellList), m_OrderQty);
		DrawTextW(hdc, wtext, wcslen(wtext), &m_ItemRect[n][m], DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
	}
}
void COrderList::DrawItem(HDC hdc)
{
	if (m_DataAxis[DPT_VBase].nId != 0)
		return;
	//价格
	SelectObject(m_HdcMem, g_FontData.GetFontNum15());
	for (int m = 1; m < m_nListHeight / N_HEIGHT_ITEM_LIST - 1; m++)
		DrawItemMid(hdc, DGC_MID, m);
	SelectObject(m_HdcMem, g_FontData.GetFontWord13());
	for (int m = 1; m < m_nListHeight / N_HEIGHT_ITEM_LIST - 1; m++)
		DrawItemLeft(hdc, DGC_LEFT, m);
	for (int m = 1; m < m_nListHeight / N_HEIGHT_ITEM_LIST - 1; m++)
		DrawItemRight(hdc, DGC_RIGHT, m);	
}
void COrderList::DrawItemQty(HDC hdc)
{
	for (map<int, DataQuoteQty>::iterator iter = m_WorkData.begin(); iter != m_WorkData.end();iter++)
	{
		if (iter->first >= m_DataAxis[DPT_VStart].nId + m_nScrollOffset&&iter->first <= m_DataAxis[DPT_VEnd].nId + m_nScrollOffset)
		{
			UINT format = DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
			RECT rect(m_ItemRect[DGC_MID][iter->first - (m_DataAxis[DPT_VStart].nId + m_nScrollOffset) + 1]);
			TMouFormatPriceStrW wtext = { 0 };
			swprintf_s(wtext, L"%d", iter->second.nQty);	

			if (iter->first <= m_DataAxis[DPT_Ask1].nId)
				SetTextColor(hdc, m_ColorSell);
			else if (iter->first >= m_DataAxis[DPT_Bid1].nId)
			{
				SetTextColor(hdc, m_ColorBuy);
				if (g_MouseOrderStyle.nParqtyStyle==1)
					format = DT_VCENTER | DT_LEFT | DT_SINGLELINE;
			}

			DrawTextW(hdc, wtext, wcslen(wtext), &rect, format);
		}
	}
	for (map<int, DataLocalWorkQty>::iterator iter = m_LocalWorkData.begin(); iter != m_LocalWorkData.end();iter++)
	{
		if (iter->first >= m_DataAxis[DPT_VStart].nId + m_nScrollOffset&&iter->first <= m_DataAxis[DPT_VEnd].nId + m_nScrollOffset)
		{
			SetTextColor(hdc, m_ColorTextNormal);
			if (iter->second.nQtyNormal[N_Buy]>0 || iter->second.nQtyCondition[N_Buy]>0)
			{
				RECT rect(m_ItemRect[DGC_LEFT][iter->first - (m_DataAxis[DPT_VStart].nId + m_nScrollOffset) + 1]);
				rect.right = rect.right - 3;
				TMouFormatPriceStrW wtext = { 0 };
				if (iter->second.nQtyNormal[N_Buy] && iter->second.nQtyCondition[N_Buy])
					swprintf_s(wtext, L"%d/%d*", iter->second.nQtyNormal[N_Buy], iter->second.nQtyCondition[N_Buy]);
				else if (iter->second.nQtyNormal[N_Buy])
					swprintf_s(wtext, L"%d", iter->second.nQtyNormal[N_Buy]);
				else
					swprintf_s(wtext, L"%d*", iter->second.nQtyCondition[N_Buy]);
				DrawTextW(hdc, wtext, wcslen(wtext), &rect, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
			}
			if (iter->second.nQtyNormal[N_Sell]>0 || iter->second.nQtyCondition[N_Sell]>0)
			{
				RECT rect(m_ItemRect[DGC_RIGHT][iter->first - (m_DataAxis[DPT_VStart].nId + m_nScrollOffset) + 1]);
				rect.left = rect.left + 3;
				TMouFormatPriceStrW wtext = { 0 };
				if (iter->second.nQtyNormal[N_Sell] && iter->second.nQtyCondition[N_Sell])
					swprintf_s(wtext, L"%d/%d*", iter->second.nQtyNormal[N_Sell], iter->second.nQtyCondition[N_Sell]);
				else if (iter->second.nQtyNormal[N_Sell])
					swprintf_s(wtext, L"%d", iter->second.nQtyNormal[N_Sell]);
				else
					swprintf_s(wtext, L"%d*", iter->second.nQtyCondition[N_Sell]);
				DrawTextW(hdc, wtext, wcslen(wtext), &rect, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
                //DrawTextW(hdc, wtext, wcslen(wtext), &rect, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
			}
		}
	}
}
void COrderList::DrawItemSpecial(HDC hdc)
{
	for (int n = DSI_MarketBuy; n < DSI_Size; n++)
	{
		SetTextColor(hdc, m_ColorHoverFrame);
		switch (n)
		{
		case DSI_MarketBuy:
			if (m_SpecialItem[DS_Hover].nCol == DGC_LEFT && m_SpecialItem[DS_Hover].nRow == 0)
			{
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBuy);
				RECT rect(m_ItemRectSpecial[n].rect);
				InflateRect(&rect, -1, -1);
				FrameRect(hdc, &rect, g_ColorRefData.GetHbrWhite());
				SetTextColor(hdc, g_ColorRefData.GetColorWhite());
			}
            else
            {
                FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgSpecial);
            }
			break;
		case DSI_CancelB:
			if (m_SpecialItem[DS_Hover].nCol == DGC_LEFT && m_SpecialItem[DS_Hover].nRow == m_nListHeight / N_HEIGHT_ITEM_LIST - 1)
			{
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBuy);
				RECT rect(m_ItemRectSpecial[n].rect);
				InflateRect(&rect, -1, -1);
				FrameRect(hdc, &rect, g_ColorRefData.GetHbrWhite());
				SetTextColor(hdc, g_ColorRefData.GetColorWhite());
			}
			else
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgSpecial);
			break;
		case DSI_MarketSell:
			if (m_SpecialItem[DS_Hover].nCol == DGC_RIGHT && m_SpecialItem[DS_Hover].nRow == 0)
			{
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushSell);
				RECT rect(m_ItemRectSpecial[n].rect);
				InflateRect(&rect, -1, -1);
				FrameRect(hdc, &rect, g_ColorRefData.GetHbrWhite());
				SetTextColor(hdc, g_ColorRefData.GetColorWhite());
			}
			else
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgSpecial);
			break;
		case DSI_CancelS:
			if (m_SpecialItem[DS_Hover].nCol == DGC_RIGHT && m_SpecialItem[DS_Hover].nRow == m_nListHeight / N_HEIGHT_ITEM_LIST - 1)
			{
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushSell);
				RECT rect(m_ItemRectSpecial[n].rect);
				InflateRect(&rect, -1, -1);
				FrameRect(hdc, &rect, g_ColorRefData.GetHbrWhite());
				SetTextColor(hdc, g_ColorRefData.GetColorWhite());
			}
			else
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgSpecial);
			break;
		case DSI_Mid_1:
		case DSI_Mid_2:
		case DSI_Mid_3:
		case DSI_Mid_4:
			if (m_bInner)
			{
				if ((m_SpecialItem[DS_Hover].nCol == DGC_MID && m_SpecialItem[DS_Hover].nRow == 0 && m_SpecialItem[DS_Hover].nSubItem == n - DSI_Mid_1) || m_BaseMidType == n - DSI_Mid_1)
					FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgMid);
				else
					FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgSpecial);
			}
			else
			{
				if (DSI_Mid_1 == n || (DSI_Mid_3 == n && m_nStopType == Tst_STL))
				{
					if ((m_SpecialItem[DS_Hover].nCol == DGC_MID && m_SpecialItem[DS_Hover].nRow == 0 && m_SpecialItem[DS_Hover].nSubItem == n - DSI_Mid_1))
						FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgMid);
					else
						FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgSpecial);
					DrawTriangle(hdc, m_ItemRectSpecial[n].rect.right - N_Arrow_Width - 1, m_ItemRectSpecial[n].rect.top + 14, N_Arrow_Width - 1);
				}
				else if (DSI_Mid_2 == n)
				{
					if ((m_SpecialItem[DS_Hover].nCol == DGC_MID && m_SpecialItem[DS_Hover].nRow == 0 && m_SpecialItem[DS_Hover].nSubItem == n - DSI_Mid_1))
						FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgMid);
					else
						FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgSpecial);
				}
				else if (DSI_Mid_3 == n && m_nStopType != Tst_STL)
				{
					RECT  rect = m_ItemRectSpecial[DSI_Mid_3].rect;
					rect.right = m_ItemRectSpecial[DSI_Mid_4].rect.right;
					if ((m_SpecialItem[DS_Hover].nCol == DGC_MID && m_SpecialItem[DS_Hover].nRow == 0 && m_SpecialItem[DS_Hover].nSubItem == DSI_Mid_3 - DSI_Mid_1) 
						|| (m_SpecialItem[DS_Hover].nCol == DGC_MID && m_SpecialItem[DS_Hover].nRow == 0 && m_SpecialItem[DS_Hover].nSubItem == DSI_Mid_4 - DSI_Mid_1))
						FillRect(hdc, &rect, m_BrushBgMid);
					else
						FillRect(hdc, &rect, m_BrushBgSpecial);
				}
                else if (DSI_Mid_4 == n && m_nStopType == Tst_STL)
                {
                    FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushWhite);
                }
			}
			break;
		case DSI_CancelA:
			if (m_SpecialItem[DS_Hover].nCol == DGC_MID && m_SpecialItem[DS_Hover].nRow == m_nListHeight / N_HEIGHT_ITEM_LIST - 1)
			{
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushLastPrice);
				RECT rect(m_ItemRectSpecial[n].rect);
				InflateRect(&rect, -1, -1);
				FrameRect(hdc, &rect, g_ColorRefData.GetHbrWhite());
				SetTextColor(hdc, g_ColorRefData.GetColorWhite());
			}
			else
				FillRect(hdc, &m_ItemRectSpecial[n].rect, m_BrushBgSpecial);
			break;
		default:
			break;
		}
		if (m_ItemClickL.nSpecial==n&&m_ItemClickL.nType==CT_Special)
			SelectObject(hdc, g_FontData.GetFontWord15());
		else
			SelectObject(hdc, g_FontData.GetFontWord13());
		//撤单量显示
		if (n == DSI_CancelB&&m_nCanCancelQty[N_Buy])
		{
			TMouQtyStrW wShow = { 0 };
			swprintf_s(wShow, L"%s(%d)", m_ItemRectSpecial[n].wtext,m_nCanCancelQty[N_Buy]);
			DrawTextW(hdc, wShow, wcslen(wShow), &m_ItemRectSpecial[n].rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		else if (n == DSI_CancelS&&m_nCanCancelQty[N_Sell])
		{
			TMouQtyStrW wShow = { 0 };
			swprintf_s(wShow, L"%s(%d)", m_ItemRectSpecial[n].wtext, m_nCanCancelQty[N_Sell]);
			DrawTextW(hdc, wShow, wcslen(wShow), &m_ItemRectSpecial[n].rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		else if (n == DSI_CancelA&&(m_nCanCancelQty[N_Sell] + m_nCanCancelQty[N_Buy])!=0)
		{
			TMouQtyStrW wShow = { 0 };
			swprintf_s(wShow, L"%s(%d)", m_ItemRectSpecial[n].wtext, m_nCanCancelQty[N_Sell] + m_nCanCancelQty[N_Buy]);
			DrawTextW(hdc, wShow, wcslen(wShow), &m_ItemRectSpecial[n].rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		else if (!m_bInner && (n == DSI_Mid_1 || n == DSI_Mid_3 || n == DSI_Mid_4))
		{
			if (DSI_Mid_1 == n)
			{
				RECT  rect = m_ItemRectSpecial[n].rect;
				rect.right = rect.right - N_Arrow_Width - 1;
				DrawTextW(hdc, m_ItemRectSpecial[n].wtext, wcslen(m_ItemRectSpecial[n].wtext), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			}
			else
			{
				if (m_nStopType != Tst_STL)
				{
					RECT  rect = m_ItemRectSpecial[DSI_Mid_3].rect;
					rect.right = m_ItemRectSpecial[DSI_Mid_4].rect.right;
					DrawTextW(hdc, m_ItemRectSpecial[DSI_Mid_3].wtext, wcslen(m_ItemRectSpecial[DSI_Mid_3].wtext), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					DrawTriangle(hdc, m_ItemRectSpecial[DSI_Mid_4].rect.left + (m_ItemRectSpecial[DSI_Mid_4].rect.right - m_ItemRectSpecial[DSI_Mid_4].rect.left)/2 - N_Arrow_Width - 1, m_ItemRectSpecial[n].rect.top + 14, N_Arrow_Width - 1);
				}
				else
				{
					RECT  rect = m_ItemRectSpecial[DSI_Mid_3].rect;
					DrawTextW(hdc, m_ItemRectSpecial[DSI_Mid_3].wtext, wcslen(m_ItemRectSpecial[DSI_Mid_3].wtext), &m_ItemRectSpecial[n].rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					DrawTriangle(hdc, m_ItemRectSpecial[DSI_Mid_4].rect.left + (m_ItemRectSpecial[DSI_Mid_4].rect.right - m_ItemRectSpecial[DSI_Mid_4].rect.left) / 2 - N_Arrow_Width - 1, m_ItemRectSpecial[n].rect.top + 14, N_Arrow_Width - 1);
				}
			}
		}
		else
			DrawTextW(hdc, m_ItemRectSpecial[n].wtext, wcslen(m_ItemRectSpecial[n].wtext), &m_ItemRectSpecial[n].rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
}

void COrderList::DrawHighLow(HDC& hdc,RECT& rect, bool bHigh)
{
	COLORREF ref = bHigh ? m_ColorSell : m_ColorBuy;
	int ny = bHigh ? rect.top - 1 : rect.bottom;
	HPEN hPen_level3 = CreatePen(PS_SOLID, 1, ref);
	HPEN OldPen3 = (HPEN)SelectObject(hdc, hPen_level3);
	MoveToEx(hdc, 1, ny, NULL);
	LineTo(hdc, N_WIDTH_LIST - 1, ny);
	SelectObject(hdc, OldPen3);
	DeleteObject(hPen_level3);

	SetTextColor(hdc, m_ColorHighLow);
	SelectObject(hdc, g_FontData.GetFontWord12());
	TMouFormatPriceStrW wtext = { 0 };
	if (bHigh)
	{
		swprintf_s(wtext, LoadResWchar_t(IDS_High));
		DrawTextW(hdc, wtext, wcslen(wtext), &rect, DT_TOP | DT_RIGHT | DT_SINGLELINE);
	}
	else
	{
		swprintf_s(wtext, LoadResWchar_t(IDS_Low));
		DrawTextW(hdc, wtext, wcslen(wtext), &rect, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	}
}
void COrderList::ResetHoverState(bool bUpdateWin)
{
	ItemRowCol oldhover(m_SpecialItem[DS_Hover]);
	ItemRowCol oldhoverlast(m_SpecialItem[DS_LastHover]);
	m_SpecialItem[DS_Hover].nCol = -1;
	m_SpecialItem[DS_Hover].nRow = -1;
	m_SpecialItem[DS_LastHover].nCol = -1;
	m_SpecialItem[DS_LastHover].nRow = -1;
	if (bUpdateWin)
	{
		InvalidateRect(m_Hwnd, &m_ItemRect[oldhover.nCol][oldhover.nRow], TRUE);
		InvalidateRect(m_Hwnd, &m_ItemRect[DGC_MID][oldhover.nRow], TRUE);
		InvalidateRect(m_Hwnd, &m_ItemRect[oldhoverlast.nCol][oldhoverlast.nRow], TRUE);
		InvalidateRect(m_Hwnd, &m_ItemRect[DGC_MID][oldhoverlast.nRow], TRUE);
	}
}
void COrderList::UpdateCode(const SContractNoType sCode, SOFCommodityInf& sinfo)
{
	if (!IsActive())
		return;
	if (strcmp(sCode, m_sQuoteCode) != 0)
	{
		m_listLastPriceId.clear();
		m_LastUpdown = LastPrice_Keep;
	}
	memcpy(m_sQuoteCode, sCode, sizeof(SContractNoType));
	memcpy(&m_CommodityInfo, &sinfo, sizeof(SOFCommodityInf));
	memset(m_DataAxis, 0, sizeof(m_DataAxis));
	m_DataAxis[DPT_VBase].nId = -1;
	m_nScrollOffset = 0;
	m_WorkData.clear();
	m_LocalWorkData.clear();
	UpdatePrice();
	UpdateUser(m_UserNo.c_str(), m_Sign.c_str(),m_CommodityInfo.cCoverMode);
}

void COrderList::UpdatePrice(bool bBaseChange)
{
	if (!IsActive())
		return;
	bool bUpdateBase = false;
	//获取方向 价格 数量
	SContract* pQuote = NULL;
	if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteCode, &pQuote, 1, false))
	{
		m_DataAxis[DPT_Last].dPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);	
		m_DataAxis[DPT_Bid1].dPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTBIDPRICE); 
		m_DataAxis[DPT_Ask1].dPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTASKPRICE);
		m_DataAxis[DPT_Last].nQty = int(CConvert::GetQtyBySnapShot(pQuote->SnapShot, S_FID_LASTQTY));
		m_DataAxis[DPT_Bid1].nQty = int(CConvert::GetQtyByContract(pQuote, S_FID_BESTBIDQTY));
		m_DataAxis[DPT_Ask1].nQty = int(CConvert::GetQtyByContract(pQuote, S_FID_BESTASKQTY));
		if (m_BaseMidType != BMT_Hand && (!(m_bTracking&&m_DataAxis[DPT_VBase].nId == 0)|| bBaseChange) )
		{
			switch (m_BaseMidType)
			{
			case BMT_Last:
				m_DataAxis[DPT_VBase].dPrice = m_DataAxis[DPT_Last].dPrice;			
				break;
			case BMT_Bid1:
				m_DataAxis[DPT_VBase].dPrice = m_DataAxis[DPT_Bid1].dPrice;
				break;
			case BMT_Ask1:
				m_DataAxis[DPT_VBase].dPrice = m_DataAxis[DPT_Ask1].dPrice;
				break;
			default:
				break;
			}
			if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_VBase].dPrice, FLOAT_SAME_ERRORDIFF))
			{
				m_DataAxis[DPT_VBase].nId = 0;
				m_nScrollOffset = 0;
				bUpdateBase = true;
			}
		}
		else
		{
			if (m_DataAxis[DPT_VBase].nId != 0)
			{
				if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Last].dPrice, FLOAT_SAME_ERRORDIFF))
					m_DataAxis[DPT_VBase].dPrice = m_DataAxis[DPT_Last].dPrice;
				else if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Bid1].dPrice, FLOAT_SAME_ERRORDIFF))
					m_DataAxis[DPT_VBase].dPrice = m_DataAxis[DPT_Bid1].dPrice;
				else if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Ask1].dPrice, FLOAT_SAME_ERRORDIFF))
					m_DataAxis[DPT_VBase].dPrice = m_DataAxis[DPT_Ask1].dPrice;

				if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_VBase].dPrice, FLOAT_SAME_ERRORDIFF))
				{
					m_DataAxis[DPT_VBase].nId = 0;
					bUpdateBase = true;
				}
			}
		}
		if (m_DataAxis[DPT_VBase].nId == 0)//基准价有了 最高最低 涨停跌停
		{
			//更新涨停跌停
			if (bUpdateBase)
			{
				m_DataAxis[DPT_Up].dPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITUPPRICE);
				m_DataAxis[DPT_Down].dPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITDOWNPRICE);
				if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Up].dPrice, FLOAT_SAME_ERRORDIFF))
					m_DataAxis[DPT_Up].nId = DivResult((m_DataAxis[DPT_VBase].dPrice - m_DataAxis[DPT_Up].dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
				if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Down].dPrice, FLOAT_SAME_ERRORDIFF))
					m_DataAxis[DPT_Down].nId = DivResult((m_DataAxis[DPT_VBase].dPrice - m_DataAxis[DPT_Down].dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
			}
		
			//更新最高最低
			m_DataAxis[DPT_High].dPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_HIGHPRICE); 
			m_DataAxis[DPT_Low].dPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LOWPRICE);
			if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_High].dPrice, FLOAT_SAME_ERRORDIFF))
			{
				m_DataAxis[DPT_High].nId = DivResult((m_DataAxis[DPT_VBase].dPrice - m_DataAxis[DPT_High].dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
				m_DataAxis[DPT_High].nQty = 1;
			}
			if (!CConvert::DoubleSameValue(0, m_DataAxis[DPT_Low].dPrice, FLOAT_SAME_ERRORDIFF))
			{
				m_DataAxis[DPT_Low].nId = DivResult((m_DataAxis[DPT_VBase].dPrice - m_DataAxis[DPT_Low].dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
				m_DataAxis[DPT_Low].nQty = 1;
			}

			//更新 最新 买卖一档
			for (int n = DPT_Last; n <= DPT_Ask1; n++)
			{
				if (m_DataAxis[n].nId != 0 || !CConvert::DoubleSameValue(0, m_DataAxis[n].dPrice, FLOAT_SAME_ERRORDIFF))
					m_DataAxis[n].nId = DivResult((m_DataAxis[DPT_VBase].dPrice - m_DataAxis[n].dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
			}

			//更新 开始行 结束行
			m_DataAxis[DPT_VStart].nId = m_DataAxis[DPT_VBase].nId - m_nLine / 2;
			m_DataAxis[DPT_VStart].dPrice = m_DataAxis[DPT_VBase].dPrice + CConvert::GetMinChangePrice(m_CommodityInfo)*(m_nLine / 2);
			m_DataAxis[DPT_VEnd].nId = m_DataAxis[DPT_VStart].nId + m_nLine - 1;
			m_DataAxis[DPT_VEnd].dPrice = m_DataAxis[DPT_VStart].dPrice + CConvert::GetMinChangePrice(m_CommodityInfo)*(m_nLine - 1);

			//行情 挂单清空 更新
			m_WorkData.clear();

            SQuoteSnapShotL2 quoteDataBuy;
            SQuoteSnapShotL2 quoteDataSell;
            SImpliedDepthL2 quoteDepth = { 0 };
			memset(&quoteDataBuy, 0, sizeof(quoteDataBuy));
            memset(&quoteDataSell, 0, sizeof(quoteDataSell));
			
            //TO-DO::深度行情
            if (g_pStarApi->GetImpliedSnapshot(pQuote, &quoteDataBuy, &quoteDataSell, quoteDepth) ||
                g_pStarApi->GetSnapshotL2(pQuote->ContractNo, &quoteDataBuy, &quoteDataSell))
            {
                for (int m = 0; m < 2; m++)
                {
                    for (int n = 0; n < MAX_L2_DEPTH; n++)
                    {
                        SQtyType nQty = quoteDataBuy.Data[n].Qty;
                        if (nQty > 0)
                        {
                            DataQuoteQty data;
                            data.nQty = nQty;
                            data.dPrice = quoteDataBuy.Data[n].Price;
                            int nId = DivResult((m_DataAxis[DPT_VBase].dPrice - data.dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
                            m_WorkData.insert(pair<int, DataQuoteQty>(nId, data));
                        }

                        nQty = quoteDataSell.Data[n].Qty;
                        if (nQty > 0)
                        {
                            DataQuoteQty data;
                            data.nQty = nQty;
                            data.dPrice = quoteDataSell.Data[n].Price;
                            int nId = DivResult((m_DataAxis[DPT_VBase].dPrice - data.dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
                            m_WorkData.insert(pair<int, DataQuoteQty>(nId, data));
                        }
                    }

                    //for (int n = FidMean_BidQty_1 + m * 40; n <= FidMean_BidQty_20 + m * 40; n++)
                    //{
                    //    int nQty = int(QuoteData.Data[n].FidAttr ? QuoteData.Data[n].Qty : 0);
                    //    if (nQty > 0)
                    //    {
                    //        DataQuoteQty data;
                    //        data.nQty = nQty;
                    //        data.dPrice = QuoteData.Data[n - 20].FidAttr ? QuoteData.Data[n - 20].Price : 0;
                    //        int nId = DivResult((m_DataAxis[DPT_VBase].dPrice - data.dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
                    //        m_WorkData.insert(pair<int, DataQuoteQty>(nId, data));
                    //    }
                    //}
                }
            }

			//最新价队列 更新
			if (!CConvert::DoubleSameValue(m_DataAxis[DPT_Last].dPrice, 0, FLOAT_SAME_ERRORDIFF))			
				UpdateLastPriceData();			

			//更新挂单
			UpdateParOrderQty(false);
			//更新持仓
			UpdatePosition(false);

			InvalidateRect(m_Hwnd, &m_FooterRect, TRUE);
		}	
	}
	InvalidateRect(m_Hwnd, &m_ListRect, TRUE);
}
bool COrderList::GetPriceStrByLineId(TMouFormatPriceStrW &wtext, int nID)
{
	if (m_DataAxis[DPT_VBase].nId == 0)
	{	
		double dprice = m_DataAxis[DPT_VStart].dPrice - (nID - 1 + m_nScrollOffset)*CConvert::GetMinChangePrice(m_CommodityInfo);
			swprintf_s(wtext, L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dprice, m_CommodityInfo)).c_str());				
		return true;
	}
	else
		return false;
}
int  COrderList::DivResult(const double Value, const double SubValue, const double ErrorDiff)
{
	//正数？
	bool bplus = false;
	if (Value*SubValue > 0)	bplus = true;

	double n = 0;
	double m = modf(abs(Value / SubValue), &n);
	int i = 0;//返回0说明错误
	if (m < ErrorDiff)
		i = (int)n;
	else if (1 - m < ErrorDiff)
		i = (int)n + 1;
	
	if (!bplus) i = -i;
	return i;
}
void COrderList::OnClickSpecial(int nItem)
{
	ListOrderInfo listOrder;
	listOrder.Offset = (m_opr&&m_opr->IsCheckOpenContinues())?oOpen:oNone;
	listOrder.OrderPrice = 0;
	listOrder.TriggerPrice = 0;
	switch (nItem)
	{		
	case DSI_MarketBuy:
		listOrder.Direct = dBuy;
		listOrder.OrderType = otMarket;
		ListSendOrder(listOrder);
		return;
	case DSI_MarketSell:
		listOrder.Direct = dSell;
		listOrder.OrderType = otMarket;
		ListSendOrder(listOrder);
		return;
	case DSI_CancelB:
		OnCancelOrder(0,dBuy);
		return;
	case DSI_CancelS:
		OnCancelOrder(0, dSell);
		return;
	case DSI_CancelA:
		OnCancelOrder(0, dBoth);
		return;
	case DSI_Mid_1:
		if (m_bInner)
		{
			if (m_BaseMidType != BMT_Last)
			{
				m_BaseMidType = BMT_Last;
				ClearData();
			}
		}
		else
		{
			if (!IsWindow(m_PopWinList[PWT_Mid].GetHwnd()))
			{
				RECT rect;
				GetWindowRect(m_Hwnd, &rect);
				m_PopWinList[PWT_Mid].CreatePopWin(this, m_Hwnd, (m_ItemRectSpecial[DSI_Mid_1].rect.right - m_ItemRectSpecial[DSI_Mid_1].rect.left) * 2+1,
					rect.left + m_ItemRectSpecial[DSI_Mid_1].rect.left, rect.top + m_ItemRectSpecial[DSI_Mid_1].rect.bottom, PWT_Mid, m_PopWinMidStr[1], BMT_Size, BMT_Size,1);
			}
			else
			{
				RECT rect;
				GetWindowRect(m_Hwnd, &rect);
				SetWindowPos(m_PopWinList[PWT_Mid].GetHwnd(), NULL, rect.left + m_ItemRectSpecial[DSI_Mid_1].rect.left, rect.top + m_ItemRectSpecial[DSI_Mid_1].rect.bottom, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
			}
			ShowWindow(m_PopWinList[PWT_Mid].GetHwnd(), SW_SHOW);
			return;
		}
		break;
	case DSI_Mid_2:
		if (m_bInner)
		{
			if (m_BaseMidType != BMT_Bid1)
			{
				m_BaseMidType = BMT_Bid1;
				ClearData();
			}
		}
		else
			m_nScrollOffset = 0;
		break;
	case DSI_Mid_3:
		if (m_bInner)
		{
			if (m_BaseMidType != BMT_Ask1)
			{
				m_BaseMidType = BMT_Ask1;
				ClearData();
			}
		}
		else
		{
			if (!IsWindow(m_PopWinList[PWT_Stop].GetHwnd()))
			{
				RECT rect;
				GetWindowRect(m_Hwnd, &rect);
				m_PopWinList[PWT_Stop].CreatePopWin(this, m_Hwnd, (m_ItemRectSpecial[DSI_Mid_3].rect.right - m_ItemRectSpecial[DSI_Mid_3].rect.left) * 2+1,
					rect.left + m_ItemRectSpecial[DSI_Mid_3].rect.left, rect.top + m_ItemRectSpecial[DSI_Mid_3].rect.bottom, PWT_Stop, m_PopWinStopStr[1], Tst_Size, Tst_Size,1);
			}
			else
			{
				RECT rect;
				GetWindowRect(m_Hwnd, &rect);
				SetWindowPos(m_PopWinList[PWT_Stop].GetHwnd(), NULL, rect.left + m_ItemRectSpecial[DSI_Mid_3].rect.left, rect.top + m_ItemRectSpecial[DSI_Mid_3].rect.bottom, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
			}
			ShowWindow(m_PopWinList[PWT_Stop].GetHwnd(), SW_SHOW);
			return;
		}
		break;
	case DSI_Mid_4:
		if (m_bInner)
			m_BaseMidType = BMT_Hand;
		else if (m_nStopType != Tst_STL)
		{
			if (!IsWindow(m_PopWinList[PWT_Stop].GetHwnd()))
			{
				RECT rect;
				GetWindowRect(m_Hwnd, &rect);
				m_PopWinList[PWT_Stop].CreatePopWin(this, m_Hwnd, (m_ItemRectSpecial[DSI_Mid_3].rect.right - m_ItemRectSpecial[DSI_Mid_3].rect.left) * 2 + 1,
					rect.left + m_ItemRectSpecial[DSI_Mid_3].rect.left, rect.top + m_ItemRectSpecial[DSI_Mid_3].rect.bottom, PWT_Stop, m_PopWinStopStr[1], Tst_Size, Tst_Size,1);
			}
			else
			{
				RECT rect;
				GetWindowRect(m_Hwnd, &rect);
				SetWindowPos(m_PopWinList[PWT_Stop].GetHwnd(), NULL, rect.left + m_ItemRectSpecial[DSI_Mid_3].rect.left, rect.top + m_ItemRectSpecial[DSI_Mid_3].rect.bottom, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
			}
			ShowWindow(m_PopWinList[PWT_Stop].GetHwnd(), SW_SHOW);
			return;
		}
		break;
	default:
		break;
	}
	UpdatePrice(true);
}
bool COrderList::SetSelect(const wchar_t* sStrategy, int nRef)
{
	switch (nRef)
	{
	case PWT_Mid:
		if (!m_bInner)
		{
			wstring wTemp = sStrategy;
			int npos = wTemp.find(L'(');
			if (0 < npos)
				wTemp = wTemp.substr(0, npos);
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_1].wtext, wTemp.c_str());
			for (int n = BMT_Last; n < BMT_Size; n++)
			{
				if (wTemp == m_PopWinMidStr[0][n])
				{
					m_BaseMidType = n;
					break;
				}
			}
			InvalidateRect(m_Hwnd, &m_ItemRectSpecial[DSI_Mid_1].rect, TRUE);
			UpdateWindow(m_Hwnd);
			UpdatePrice(true);
		}
		break;

	case PWT_Stop:
		if (!m_bInner)
		{
			wstring wTemp = sStrategy;
			int npos = wTemp.find(L'-');
			if (0 < npos)
				wTemp = wTemp.substr(0, npos);
			wcscpy_s(m_ItemRectSpecial[DSI_Mid_3].wtext, wTemp.c_str());
			for (int n = Tst_STP; n < Tst_Size; n++)
			{
				if (wTemp == m_PopWinStopStr[0][n])
				{
					m_nStopType = n;
					break;
				}
			}
			RECT rect = m_ItemRectSpecial[DSI_Mid_3].rect;
			rect.right = m_ItemRectSpecial[DSI_Mid_4].rect.right;
			ShowWindow(m_EditExPoint.GetHwnd(), m_nStopType == Tst_STL ? SW_SHOW : SW_HIDE);
			InvalidateRect(m_Hwnd, &rect, TRUE);
			UpdateWindow(m_Hwnd);
		}
		break;

	default:
		break;
	}
	return true;
}
void COrderList::SetRightCancel(bool b)
{
    m_bRightCancel = b;
}
void COrderList::OnClickBuySell(int nCol, int nRow)
{
	if (m_DataAxis[DPT_VBase].nId!=0)
	{
		MessageBox(m_Hwnd, LoadResWchar_t(IDS_InValidQuote), LoadResWchar_t(IDS_TIP), NULL);
		return;
	}
	ListOrderInfo listOrder;
	listOrder.Offset = (m_opr&&m_opr->IsCheckOpenContinues()) ? oOpen : oNone;
	listOrder.OrderPrice = 0;
	listOrder.TriggerPrice = 0;
	switch (nCol)
	{
	case DGC_LEFT:
		listOrder.Direct = dBuy;	
		if (!m_bInner&&m_nStopType != Tst_LMT && (nRow - 1) + m_nScrollOffset<m_DataAxis[DPT_Ask1].nId - m_DataAxis[DPT_VStart].nId)
		{
			if (m_nStopType==Tst_STP)
			{
				listOrder.OrderType = otMarketStop;
				listOrder.TriggerPrice = m_DataAxis[DPT_VStart].dPrice - ((nRow - 1) + m_nScrollOffset)*CConvert::GetMinChangePrice(m_CommodityInfo);
			}
			else if (m_nStopType==Tst_STL)
			{
				listOrder.OrderType = otLimitStop;
				listOrder.TriggerPrice = m_DataAxis[DPT_VStart].dPrice - ((nRow - 1) + m_nScrollOffset)*CConvert::GetMinChangePrice(m_CommodityInfo);
				listOrder.OrderPrice = listOrder.TriggerPrice + GetSTLTick()*CConvert::GetMinChangePrice(m_CommodityInfo);
			}
		}
		else
		{
			listOrder.OrderType = otLimit;
			listOrder.OrderPrice = m_DataAxis[DPT_VStart].dPrice - ((nRow - 1) + m_nScrollOffset)*CConvert::GetMinChangePrice(m_CommodityInfo);
		}
		ListSendOrder(listOrder);
		break;
	case DGC_RIGHT:
		listOrder.Direct = dSell;
		listOrder.OrderType = otLimit;
		if (!m_bInner&&m_nStopType != Tst_LMT && (nRow - 1) + m_nScrollOffset>m_DataAxis[DPT_Bid1].nId - m_DataAxis[DPT_VStart].nId)
		{
			if (m_nStopType == Tst_STP)
			{
				listOrder.OrderType = otMarketStop;
				listOrder.TriggerPrice = m_DataAxis[DPT_VStart].dPrice - ((nRow - 1) + m_nScrollOffset)*CConvert::GetMinChangePrice(m_CommodityInfo);
			}
			else if (m_nStopType == Tst_STL)
			{
				listOrder.OrderType = otLimitStop;
				listOrder.TriggerPrice = m_DataAxis[DPT_VStart].dPrice - ((nRow - 1) + m_nScrollOffset)*CConvert::GetMinChangePrice(m_CommodityInfo);
				listOrder.OrderPrice = listOrder.TriggerPrice - GetSTLTick()*CConvert::GetMinChangePrice(m_CommodityInfo);
			}
		}
		else
		{
			listOrder.OrderType = otLimit;
			listOrder.OrderPrice = m_DataAxis[DPT_VStart].dPrice - ((nRow - 1) + m_nScrollOffset)*CConvert::GetMinChangePrice(m_CommodityInfo);
		}
		ListSendOrder(listOrder);
		break;
	}
}
void COrderList::OnRCancelOrder(int nCol, int nRow)
{
	vector<uint> vCancelIds;
	TDirect direct = nCol == DGC_LEFT?dBuy:dSell;
	int nID = m_DataAxis[DPT_VStart].nId + nRow - 1 + m_nScrollOffset;
	for (map<int, const TOrder*>::iterator it = m_ParOrderInfo.begin(); it != m_ParOrderInfo.end(); it++)
	{
		double dPrice = (it->second->OrderType == otLimitStop || it->second->OrderType == otMarketStop) ? it->second->TriggerPrice : it->second->OrderPrice;
		if (nID == DivResult((m_DataAxis[DPT_VBase].dPrice - dPrice), CConvert::GetMinChangePrice(m_CommodityInfo)))
		{
			if ((it->second->Direct == dBuy&&nCol == DGC_LEFT) || (it->second->Direct == dSell&&nCol == DGC_RIGHT))
				vCancelIds.push_back(it->second->OrderID);
		}		
	}	
	for (vector<uint>::iterator itr= vCancelIds.begin();itr!=vCancelIds.end();itr++)
	{
		if (!OnCancelOrder(*itr, direct, true))
			return;
	}
	
}
void COrderList::UpdateUser(const char* UserNo, const char* Sign, char cCoverMode)
{
	if (!IsActive())
		return;

	bool bOldType = m_bInner;
	if (cCoverMode == oCover || cCoverMode == oCoverT)
		m_bInner = true;
	else
		m_bInner = false;

	if (bOldType != m_bInner)
		UpdateMidHead();

	m_UserNo = UserNo;
	m_Sign = Sign;

	m_ParOrderInfo.clear();
	TDataContainer tempDataCon;
	g_pTradeData->GetParOrder(tempDataCon, ftValid, Sign);//所有定单
	for (uint i = 0; i < tempDataCon.size(); i++)
	{
		const TOrder *pRef = (TOrder*)(tempDataCon.at(i));
		SContractNoType code = { 0 };
		CConvert::TradeContractToQContractNoType(*pRef, code);
		//判断是否是此用户的委托
		if (0 != strcmp(code, m_sQuoteCode) || (0 != strcmp(UserNo, pRef->UserNo)) || (0 != strcmp(Sign, pRef->Sign)))//不是 就不需保存了
			continue;
		UpdateTradeOrder(pRef);
	}
	UpdateParOrderQty();
	UpdatePosition();
	InvalidateRect(m_Hwnd, NULL, TRUE);
}
void COrderList::UpdateTradeOrder(const TOrder* data)
{
	if (!IsActive())
		return;

	map<int, const TOrder*>::iterator it = m_ParOrderInfo.find(data->OrderID);
	if (it == m_ParOrderInfo.end())//没找到
	{
		if (osQueued == data->OrderState || osPartFilled == data->OrderState || (osTriggering == data->OrderState && (data->OrderType == otLimitStop || data->OrderType == otMarketStop)))
			m_ParOrderInfo.insert(make_pair(data->OrderID, data));
	}
	else//找到
	{
		if (osFilled == data->OrderState || osPartCanceled == data->OrderState || osCanceled == data->OrderState || osFail == data->OrderState)//已经成交 已经撤单 指令失败 界面不需要显示了
			m_ParOrderInfo.erase(it);
	}
}
void COrderList::UpdatePosition(bool bUpdateUI)													//持仓线更新
{
	if (!IsActive())
		return;

	m_DataAxis[DPT_VShort].nId = 0;
	m_DataAxis[DPT_VLong].nId = 0;
	m_DataAxis[DPT_VShort].nQty = 0;
	m_DataAxis[DPT_VLong].nQty = 0;
	if (m_UserNo=="")
		return;
	
	TContractKey key;
	CConvert::SContractNoTypeToTradeContract(m_Sign.c_str(), m_sQuoteCode, key);
	double dMin = CConvert::GetMinChangePrice(m_CommodityInfo);
	if (CConvert::DoubleSameValue(0, dMin, FLOAT_SAME_ERRORDIFF))
		return;
	const TPosition * pL = g_pTradeData->GetPositionTotal(m_UserNo.c_str(), &key, dBuy);
	if (pL)
	{
		int nOffset = 0;
		if (!CConvert::DoubleSameValue(int(pL->PositionPrice / dMin)*dMin, pL->PositionPrice, m_CommodityInfo.bIfFractionPrice ? FLOAT_SAME_ERRORDIFF : pow(10, -(m_CommodityInfo.iPrecision + 1))))
			nOffset = 1;
		int nId = DivResult((m_DataAxis[DPT_VBase].dPrice - int(pL->PositionPrice / dMin + nOffset)*dMin), dMin);
		m_DataAxis[DPT_VLong].nId = nId;
		m_DataAxis[DPT_VLong].dPrice = pL->PositionPrice;
		m_DataAxis[DPT_VLong].nQty = pL->PositionQty;
	}
	const TPosition * pS = g_pTradeData->GetPositionTotal(m_UserNo.c_str(), &key, dSell);
	if (pS)
	{
		int nOffset = 0;
		if (!CConvert::DoubleSameValue(int(pS->PositionPrice / dMin)*dMin, pS->PositionPrice, m_CommodityInfo.bIfFractionPrice ? FLOAT_SAME_ERRORDIFF:pow(10,-(m_CommodityInfo.iPrecision+1))))
			nOffset = 1;
		int nId = DivResult((m_DataAxis[DPT_VBase].dPrice - int(pS->PositionPrice / dMin + nOffset)*dMin), dMin);
		m_DataAxis[DPT_VShort].nId = nId;
		m_DataAxis[DPT_VShort].dPrice = pS->PositionPrice;
		m_DataAxis[DPT_VShort].nQty = pS->PositionQty;
	}
	if (bUpdateUI)
		InvalidateRect(m_Hwnd, &m_ListRect, TRUE);
}
void COrderList::UpdateParOrderQty(bool bUpdateUI)
{
	if (!IsActive())
		return;

	memset(m_nCanCancelQty, 0, sizeof(m_nCanCancelQty));
	m_LocalWorkData.clear();
	for (map<int, const TOrder*>::iterator it = m_ParOrderInfo.begin(); it != m_ParOrderInfo.end(); it++)
	{	
		double dPrice = (it->second->OrderType == otLimitStop || it->second->OrderType == otMarketStop) ? it->second->TriggerPrice : it->second->OrderPrice;
		int nId = DivResult((m_DataAxis[DPT_VBase].dPrice - dPrice), CConvert::GetMinChangePrice(m_CommodityInfo));
		map<int, DataLocalWorkQty>::iterator ite = m_LocalWorkData.find(nId);
		if (ite != m_LocalWorkData.end())
		{
			if (it->second->OrderType == otLimitStop || it->second->OrderType == otMarketStop)
			{
				if (it->second->Direct == dBuy)
					ite->second.nQtyCondition[N_Buy] += it->second->OrderQty - it->second->MatchQty;
				else if (it->second->Direct == dSell)
					ite->second.nQtyCondition[N_Sell] += it->second->OrderQty - it->second->MatchQty;
			}
			else
			{
				if (it->second->Direct == dBuy)
					ite->second.nQtyNormal[N_Buy] += it->second->OrderQty - it->second->MatchQty;
				else if (it->second->Direct == dSell)
					ite->second.nQtyNormal[N_Sell] += it->second->OrderQty - it->second->MatchQty;
			}
		}
		else
		{
			DataLocalWorkQty workQty = {0};
			if (it->second->OrderType == otLimitStop || it->second->OrderType == otMarketStop)
			{
				if (it->second->Direct == dBuy)
					workQty.nQtyCondition[N_Buy] += it->second->OrderQty - it->second->MatchQty;
				else if (it->second->Direct == dSell)
					workQty.nQtyCondition[N_Sell] += it->second->OrderQty - it->second->MatchQty;
			}
			else
			{
				if (it->second->Direct == dBuy)
					workQty.nQtyNormal[N_Buy] += it->second->OrderQty - it->second->MatchQty;
				else if (it->second->Direct == dSell)
					workQty.nQtyNormal[N_Sell] += it->second->OrderQty - it->second->MatchQty;
			}
			m_LocalWorkData.insert(pair<int, DataLocalWorkQty>(nId, workQty));
		}	
		if (it->second->Direct == dBuy)
			m_nCanCancelQty[N_Buy] += it->second->OrderQty - it->second->MatchQty;
		else
			m_nCanCancelQty[N_Sell] += it->second->OrderQty - it->second->MatchQty;
	}
	if (bUpdateUI)
	{
		InvalidateRect(m_Hwnd, &m_ListRect, TRUE);
		InvalidateRect(m_Hwnd, &m_FooterRect, TRUE);	
	}
}

bool COrderList::OnCancelOrder(int nOrderId, TDirect direct, bool bShowMsg)
{
	const TUserInfo *pInfo = g_pTradeData->GetUser(m_UserNo.c_str(), m_Sign.c_str());
	if (pInfo&&pInfo->pLogin)
	{
		if (pInfo->pLogin->Logined == bNo)//登录不成功
		{
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1106), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
		else
		{
			if (nOrderId != 0)
			{			
				const TOrder* p = g_pTradeData->GetOrder(nOrderId);
				if (p)
				{
                    set<int> setOrder;
                    setOrder.insert(p->OrderID);
                    return g_pCommonModule->CancelOrder(setOrder);
                    //return g_pTradeApi->CancelOrder(p->OrderID);
					//if (bShowMsg)
					//{
					//	wchar_t cShow[100] = { 0 };
					//	if (p->OrderType == otLimitStop || p->OrderType == otMarketStop)
					//		swprintf_s(cShow, LoadResWchar_t(IDS_CancelTip1), LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(p->TriggerPrice, m_CommodityInfo)).c_str(), p->OrderID);
					//	else
					//		swprintf_s(cShow, LoadResWchar_t(IDS_CancelTip2), LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(p->OrderPrice, m_CommodityInfo)).c_str(), p->OrderID);
					//	if (MessageBox(m_Hwnd, cShow, LoadResWchar_t(IDS_TIP), MB_OKCANCEL)==IDOK)
					//		return g_pTradeApi->CancelOrder(p->OrderID);
					//	else
					//		return -1;
					//}
				}		
			}
			else if (direct == dBoth)
			{
                set<int> setOrder;
                for (map<int, const TOrder*>::iterator iter = m_ParOrderInfo.begin(); iter != m_ParOrderInfo.end(); iter++)
                {
                    setOrder.insert(iter->second->OrderID);
                }
                return g_pCommonModule->CancelOrder(setOrder);
				//g_pTradeApi->CancelOrder(iter->second->OrderID);
			}
			else 
			{
                set<int> setOrder;
				for (map<int, const TOrder*>::iterator iter = m_ParOrderInfo.begin(); iter != m_ParOrderInfo.end(); iter++)
				{
                    if (direct == iter->second->Direct)
                    {
                        //g_pTradeApi->CancelOrder(iter->second->OrderID);
                        setOrder.insert(iter->second->OrderID);
                    }
				}
                return g_pCommonModule->CancelOrder(setOrder);
			}		
		}
	}
	return true;
}
void COrderList::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;
	HPEN  hGrayPen = CreatePen(PS_SOLID, 1, m_ColorHoverFrame);
	HPEN hOld = (HPEN)SelectObject(hdc, hGrayPen);
	
	int nMidx = nStartx + nWidth / 2;
	int nMidy = nStarty + nWidth / 2;
	MoveToEx(hdc, nStartx, nStarty, NULL);
	LineTo(hdc, nMidx, nMidy);

	int nEndx = nStartx + nWidth - 1;
	int nEndy = nStarty;
	LineTo(hdc, nEndx + 1, nEndy - 1);
	
	SelectObject(hdc, hOld);
	DeleteObject(hGrayPen);
}

void COrderList::DrawArrow(HDC &hdc, int nStartx, int nStarty, int nWidth, int nHeight)	//绘制箭头
{
	if (nWidth % 2 == 0 || m_LastUpdown == 0|| m_CommodityInfo.bIfFractionPrice)
		return;
	COLORREF ref = m_LastUpdown > 0 ? m_ColorBuy : m_ColorSell;
	HPEN hGrayPen = CreatePen(PS_SOLID, 1, ref);
	HPEN hOld = (HPEN)SelectObject(hdc, hGrayPen);
	if (m_LastUpdown>0)
	{
		MoveToEx(hdc, nStartx + 1, nStarty + 5, NULL);
		LineTo(hdc, nStartx + 3, nStarty);
		LineTo(hdc, nStartx + 5 + 1, nStarty + 5+1);
	}
	else
	{
		MoveToEx(hdc, nStartx + 1, nStarty + nHeight - 5, NULL);
		LineTo(hdc, nStartx + 3, nStarty + nHeight);
		LineTo(hdc, nStartx + 5 + 1, nStarty + nHeight - 5 - 1);
	}

	MoveToEx(hdc, nStartx + 3, nStarty, NULL);
	LineTo(hdc, nStartx + 3, nStarty + nHeight);
	
	SelectObject(hdc, hOld);
	DeleteObject(hGrayPen);
}
void COrderList::DrawDiv(HDC &hdc, int nStartx, int nStarty, int nWidth)
{
	HPEN hGrayPen = CreatePen(PS_SOLID, 1, m_ColorLine);
	HPEN hOld = (HPEN)SelectObject(hdc, hGrayPen);

	MoveToEx(hdc, nStartx, nStarty, NULL);
	LineTo(hdc, nStartx + nWidth, nStarty);
	
	SelectObject(hdc, hOld);
	DeleteObject(hGrayPen);
}
void COrderList::OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax, int nMin)
{
	switch (nID)
	{
	case LII_Tick_Edit:
		{	
			if (nDelta > 0)
			{
				TMouQtyStrW sInt = { 0 };
				GetWindowText(m_EditExPoint.GetEditWnd(), sInt, sizeof(TMouQtyStrW));
				int a = _wtoi(sInt);
				a++;
				a = a > nMax ? nMax : a;
				SetWindowInt(m_EditExPoint.GetEditWnd(), a);
			}
			else
			{
				TMouQtyStrW sInt = { 0 };
				GetWindowText(m_EditExPoint.GetEditWnd(), sInt, sizeof(TMouQtyStrW));
				int a = _wtoi(sInt);
				a--;
				a = a < nMin ? nMin : a;
				SetWindowInt(m_EditExPoint.GetEditWnd(), a);
			}
		}
		break;
	default:
		break;
	}
}
int COrderList::GetSTLTick()
{
	if (IsWindowVisible(m_EditExPoint.GetEditWnd()))
	{
		TMouQtyStrW wQty = { 0 };
		GetWindowText(m_EditExPoint.GetEditWnd(), wQty, sizeof(wQty));
		return _wtoi(wQty);
	}
	else
	{
		return 0;
	}
}
void COrderList::ClearData()
{
	memset(m_DataAxis, 0, sizeof(m_DataAxis));
	m_DataAxis[DPT_VBase].nId = -1;
	m_nScrollOffset = 0;
	m_WorkData.clear();
	m_LocalWorkData.clear();
}
void COrderList::CreateBrushObject()
{
    m_BrushBackground   = CreateSolidBrush(m_ColorBackground);
    m_BrushTextHover    = CreateSolidBrush(m_ColorTextHover);
    m_BrushTextQty      = CreateSolidBrush(m_ColorOrderQty);
    m_BrushTextBuy      = CreateSolidBrush(m_ColorTextBuy);
    m_BrushTextNormal   = CreateSolidBrush(m_ColorTextNormal);
    m_BrushTextSell     = CreateSolidBrush(m_ColorTextSell);
    m_BrushBuy          = CreateSolidBrush(m_ColorBuy);
    m_BrushSell         = CreateSolidBrush(m_ColorSell);
    m_BrushBgBuy        = CreateSolidBrush(m_ColorBgBuy);
    m_BrushBgSell       = CreateSolidBrush(m_ColorBgSell);
    m_BrushHighLow      = CreateSolidBrush(m_ColorHighLow);
    m_BrushLine         = CreateSolidBrush(m_ColorLine);
    m_BrushHoverFrame   = CreateSolidBrush(m_ColorHoverFrame);
    m_BrushLastPrice    = CreateSolidBrush(m_ColorLastPrice);
    m_BrushBgMid        = CreateSolidBrush(m_ColorBgMid);
    m_BrushBlack        = CreateSolidBrush(m_ColorBlack);
    m_BrushWhite        = CreateSolidBrush(m_ColorWhite);
    m_BrushBgAsk1       = CreateSolidBrush(m_ColorBgAsk1);
    m_BrushBgBid1       = CreateSolidBrush(m_ColorBgBid1);
    m_BrushBgAsk        = CreateSolidBrush(m_ColorBgAsk);
    m_BrushBgBid        = CreateSolidBrush(m_ColorBgBid);
    m_BrushBgSpecial    = CreateSolidBrush(m_ColorBgSpecial);
    m_BrushSelect       = CreateSolidBrush(Select_Color);
}
void COrderList::DeleteBrushObject()
{
    DeleteObject(m_BrushBackground);
    DeleteObject(m_BrushTextHover);
    DeleteObject(m_BrushTextQty);
    DeleteObject(m_BrushTextBuy);
    DeleteObject(m_BrushTextSell);
    DeleteObject(m_BrushBuy);
    DeleteObject(m_BrushSell);
    DeleteObject(m_BrushHighLow);
    DeleteObject(m_BrushLine);
    DeleteObject(m_BrushHoverFrame);
    DeleteObject(m_BrushLastPrice);
    DeleteObject(m_BrushBgMid);
    DeleteObject(m_BrushTextNormal);
    DeleteObject(m_BrushBlack);
    DeleteObject(m_BrushWhite);
    DeleteObject(m_BrushBgAsk1);
    DeleteObject(m_BrushBgBid1);
    DeleteObject(m_BrushBgAsk);
    DeleteObject(m_BrushBgBid);
    DeleteObject(m_BrushBgSell);
    DeleteObject(m_BrushBgBuy);
    DeleteObject(m_BrushBgSpecial);

    DeleteObject(m_BrushSelect);
}

void COrderList::ListSendOrder(ListOrderInfo& order)
{
	if (m_opr)
	{
		if (!m_opr->LadderSendOrder(order))
		{
			m_bTracking = false;
			ResetHoverState(true);
			UpdateWindow(m_Hwnd);
		}
	}		
}

void COrderList::UpdateColorStyle(bool bUseFor)
{
    ResetColorStyle(bUseFor, g_MouseOrderStyle.nOrderListStyle);

    DeleteBrushObject();
    CreateBrushObject();
    InvalidateRect(m_Hwnd, NULL, TRUE);
}

void COrderList::ResetColorStyle(bool bUseFor, int style)
{
    switch (style)
    {
    case Style_Defalut:
    {//default
        if (bUseFor)
        {
            m_ColorBuy = Default_Green;
            m_ColorSell = Default_Red;
            m_ColorBgAsk1 = Default_AskBid_Red;
            m_ColorBgBid1 = Default_AskBid_Green;
        }
        else
        {
            m_ColorBuy = Default_Red;
            m_ColorSell = Default_Green;
            m_ColorBgAsk1 = Default_AskBid_Green;
            m_ColorBgBid1 = Default_AskBid_Red;
        }

        m_ColorBgAsk = g_ColorRefData.GetColorBackground();
        m_ColorBgBid = g_ColorRefData.GetColorBackground();
        m_ColorBgSell = g_ColorRefData.GetColorBackground();
        m_ColorBgBuy = g_ColorRefData.GetColorBackground();
        m_ColorBackground = g_ColorRefData.GetColorBackground();
        m_ColorTextHover = g_ColorRefData.GetColorWhite();
        m_ColorOrderQty = g_ColorRefData.GetColorBlack();
        m_ColorTextBuy = m_ColorBuy;
        m_ColorTextSell = m_ColorSell;
        m_ColorHighLow = g_ColorRefData.GetColorBlue();
        m_ColorLine = RGB_WhiteSpecialBk;
        m_ColorHoverFrame = RGB(0, 80, 152);
        m_ColorLastPrice = RGB_Yellow_W;
        m_ColorBgMid = RGB(230, 230, 230);
        m_ColorTextNormal = g_ColorRefData.GetColorBlack();
        m_ColorTextLmt = g_ColorRefData.GetColorBlack();
        m_ColorBgSpecial = g_ColorRefData.GetColorBackground();
        break;
    }
    case Style_Light_RG:
    {//
        if (bUseFor)
        {
            m_ColorBuy = Light_RG_Green;
            m_ColorSell = Light_RG_Red;
            m_ColorBgAsk1 = Default_AskBid_Red;
            m_ColorBgBid1 = Default_AskBid_Green;
            m_ColorBgSell = Light_RG_Red;
            m_ColorBgBuy = Light_RG_Green;
        }
        else
        {
            m_ColorBuy = Light_RG_Red;
            m_ColorSell = Light_RG_Green;
            m_ColorBgAsk1 = Default_AskBid_Green;
            m_ColorBgBid1 = Default_AskBid_Red;
            m_ColorBgSell = Light_RG_Green;
            m_ColorBgBuy = Light_RG_Red;
        }

        m_ColorBgAsk = g_ColorRefData.GetColorBackground();
        m_ColorBgBid = g_ColorRefData.GetColorBackground();
        m_ColorBackground = g_ColorRefData.GetColorBackground();
        m_ColorTextHover = g_ColorRefData.GetColorWhite();
        m_ColorOrderQty = g_ColorRefData.GetColorWhite();
        m_ColorTextBuy = m_ColorBuy;
        m_ColorTextSell = m_ColorSell;
        m_ColorHighLow = g_ColorRefData.GetColorWhite();
        m_ColorLine = RGB_WhiteSpecialBk;
        m_ColorHoverFrame = RGB(0, 80, 200);
        m_ColorLastPrice = RGB_Yellow_W;
        m_ColorBgMid = RGB(230, 230, 230);
        m_ColorTextNormal = g_ColorRefData.GetColorBlack();
        m_ColorTextLmt = g_ColorRefData.GetColorWhite();
        m_ColorBgSpecial = g_ColorRefData.GetColorBackground();

        break;
    }
    case Style_Dark:
    {//Dark
        if (bUseFor)
        {
            m_ColorBuy = Dark_Green;//RGB(0, 255, 0);
            m_ColorSell = Dark_Red;//RGB(255, 0, 0);
            m_ColorBgAsk1 = Dark_AskBid_Red;
            m_ColorBgBid1 = Dark_AskBid_Green;
            m_ColorBgSell = Dark_Bg_Red;
            m_ColorBgBuy = Dark_Bg_Green;
            m_ColorBgAsk = Dark_Bg_Red;
            m_ColorBgBid = Dark_Bg_Green;
        }
        else
        {
            m_ColorBuy = Dark_Red;
            m_ColorSell = Dark_Green;
            m_ColorBgAsk1 = RGB(60, 110, 60);
            m_ColorBgBid1 = RGB(110, 60, 60);
            m_ColorBgSell = g_ColorRefData.GetColorBlack(); //Dark_Black;
            m_ColorBgBuy = g_ColorRefData.GetColorBlack(); //Dark_Black;
            m_ColorBgBid = Dark_Bg_Red;
            m_ColorBgAsk = Dark_Bg_Green;
        }

        m_ColorBackground = g_ColorRefData.GetColorBlack();//Dark_Black;
        m_ColorTextHover = g_ColorRefData.GetColorWhite();
        m_ColorOrderQty = g_ColorRefData.GetColorWhite();
        m_ColorTextBuy = m_ColorBuy;
        m_ColorTextSell = m_ColorSell;
        m_ColorHighLow = g_ColorRefData.GetColorBlue();
        m_ColorLine = g_ColorRefData.GetColorRGB150();
        m_ColorHoverFrame = RGB(0, 80, 200);
        m_ColorLastPrice = RGB_Yellow_W;
        m_ColorBgMid = RGB(30, 30, 30);
        m_ColorTextNormal = g_ColorRefData.GetColorWhite();
        m_ColorTextLmt = g_ColorRefData.GetColorWhite();
        m_ColorBgSpecial = g_ColorRefData.GetColorBlack();

        break;
    }
    case Style_Light_Yellow:
    {
        if (bUseFor)
        {
            m_ColorBuy = Default_Green;
            m_ColorSell = Default_Red;
            m_ColorBgAsk1 = Default_AskBid_Red;
            m_ColorBgBid1 = Default_AskBid_Green;
        }
        else
        {
            m_ColorBuy = Default_Red;
            m_ColorSell = Default_Green;
            m_ColorBgAsk1 = Default_AskBid_Green;
            m_ColorBgBid1 = Default_AskBid_Red;
        }

        m_ColorBgSell = Light_Yellow; 
        m_ColorBgBuy = Light_Yellow;
        m_ColorBgAsk = Light_Yellow;
        m_ColorBgBid = Light_Yellow;
        m_ColorBgSpecial = Light_Yellow;
        m_ColorBgMid = RGB(231, 219, 200);
        m_ColorBackground = Light_Yellow;

        m_ColorTextHover = g_ColorRefData.GetColorWhite();
        m_ColorOrderQty = g_ColorRefData.GetColorBlack();
        m_ColorTextBuy = m_ColorBuy;
        m_ColorTextSell = m_ColorSell;
        m_ColorHighLow = g_ColorRefData.GetColorBlue();
        m_ColorLine = g_ColorRefData.GetColorWhite();
        m_ColorHoverFrame = RGB(0, 80, 152);
        m_ColorLastPrice = RGB_Yellow_W;
        m_ColorTextNormal = g_ColorRefData.GetColorBlack();
        m_ColorTextLmt = g_ColorRefData.GetColorBlack();
        

        break;
    }
    case Style_Light_Blue:
    {
        if (bUseFor)
        {
            m_ColorBuy = Default_Green;
            m_ColorSell = Default_Red;
            m_ColorBgAsk1 = Default_AskBid_Red;
            m_ColorBgBid1 = Default_AskBid_Green;
        }
        else
        {
            m_ColorBuy = Default_Red;
            m_ColorSell = Default_Green;
            m_ColorBgAsk1 = Default_AskBid_Green;
            m_ColorBgBid1 = Default_AskBid_Red;
        }

        m_ColorBgSell = Light_Blue;
        m_ColorBgBuy = Light_Blue;
        m_ColorBgAsk = Light_Blue;
        m_ColorBgBid = Light_Blue;
        m_ColorBgSpecial = Light_Blue;
        m_ColorBgMid = RGB(175, 195, 215);
        m_ColorBackground = Light_Blue;

        m_ColorTextHover = g_ColorRefData.GetColorWhite();
        m_ColorOrderQty = g_ColorRefData.GetColorBlack();
        m_ColorTextBuy = m_ColorBuy;
        m_ColorTextSell = m_ColorSell;
        m_ColorHighLow = g_ColorRefData.GetColorBlue();
        m_ColorLine = g_ColorRefData.GetColorWhite();
        m_ColorHoverFrame = RGB(0, 80, 152);
        m_ColorLastPrice = RGB_Yellow_W;
        m_ColorTextNormal = g_ColorRefData.GetColorBlack();
        m_ColorTextLmt = g_ColorRefData.GetColorBlack();

        break;
    }
    case Style_Light_Green:
    {
        if (bUseFor)
        {
            m_ColorBuy = Default_Green;
            m_ColorSell = Default_Red;
            m_ColorBgAsk1 = Default_AskBid_Red;
            m_ColorBgBid1 = Default_AskBid_Green;
        }
        else
        {
            m_ColorBuy = Default_Red;
            m_ColorSell = Default_Green;
            m_ColorBgAsk1 = RGB(180, 255, 180); 
            m_ColorBgBid1 = RGB(255, 180, 180);
        }

        m_ColorBgSell = Light_Green;
        m_ColorBgBuy = Light_Green;
        m_ColorBgAsk = Light_Green;
        m_ColorBgBid = Light_Green;
        m_ColorBgSpecial = Light_Green;
        m_ColorBgMid = RGB(199, 225, 204);
        m_ColorBackground = Light_Green;

        m_ColorTextHover = g_ColorRefData.GetColorWhite();
        m_ColorOrderQty = g_ColorRefData.GetColorBlack();
        m_ColorTextBuy = m_ColorBuy;
        m_ColorTextSell = m_ColorSell;
        m_ColorHighLow = g_ColorRefData.GetColorBlue();
        m_ColorLine = g_ColorRefData.GetColorWhite();
        m_ColorHoverFrame = RGB(0, 80, 152);
        m_ColorLastPrice = RGB_Yellow_W;
        m_ColorTextNormal = g_ColorRefData.GetColorBlack();
        m_ColorTextLmt = g_ColorRefData.GetColorBlack();

        break;
    }
    default:
        break;
    }

    m_ColorBlack = g_ColorRefData.GetColorBlack();
    m_ColorWhite = g_ColorRefData.GetColorWhite();
}