#include "BaseInclude.h"
#include "TradeInfoStr.h"

TPositionView::TPositionView()
{

}
TPositionView::~TPositionView()
{

}
void TPositionView::Create(HWND hParent)
{
	CreateFrm(L"pos_view", hParent, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
}
void TPositionView::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_Hwnd, NULL, x, y, cx, cy, SWP_NOZORDER);
}

LRESULT TPositionView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	}
	return NOT_PROCESSED;
}

void TPositionView::OnCreate()
{
	m_posList.CreateList(m_Hwnd, LVS_REPORT /*| LVS_NOSORTHEADER*/ | LVS_OWNERDRAWFIXED | WS_BORDER | LVS_OWNERDATA, 20, 18, LVS_EX_FULLROWSELECT);
	m_hMenu = CreatePopupMenu();
	
	//Position
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::Product_Index), 50);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::Contract_Index), 80);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::BuySell_Index), 40);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::Qty_Index), 40);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::AvaQty_Index), 40);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::CurrentFloatingPnL_Index), 80);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::FloatingPnL_Index), 40);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::InitialMargin_Index), 80);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::AvgPrice_Index), 80);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::DingshiPnL_Index), 90);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::ToadyQty_Index), 60);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::ToadyAvaQty_Index), 60);
	m_posList.AddColumn(TradeInfoStr::_GetTextTIW(TradeInfoStr::SpecHedg_Index), 40);

	AppendMenu(m_hMenu, MF_STRING, IDM_RIVALCOLSE, TradeInfoStr::_GetTextTIW(TradeInfoStr::CounterClose_Index));
	AppendMenu(m_hMenu, MF_STRING, IDM_BACKHAND, TradeInfoStr::_GetTextTIW(TradeInfoStr::BackHandW_Index));
	AppendMenu(m_hMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(m_hMenu, MF_STRING, IDM_STOPLOSS, TradeInfoStr::_GetTextTIW(TradeInfoStr::SetStopLossW_Index));
	AppendMenu(m_hMenu, MF_STRING, IDM_STOPWIN, TradeInfoStr::_GetTextTIW(TradeInfoStr::SetStopProfitW_Index));
	AppendMenu(m_hMenu, MF_STRING, IDM_BREAKEVEN, TradeInfoStr::_GetTextTIW(TradeInfoStr::SetBreakEvenW_Index));
	AppendMenu(m_hMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(m_hMenu, MF_STRING, IDM_TIMINGCLOSE, TradeInfoStr::_GetTextTIW(TradeInfoStr::CloseCombo_Index));
	AppendMenu(m_hMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(m_hMenu, MF_STRING, IDM_SAVE, TradeInfoStr::_GetTextTIW(TradeInfoStr::Save_Index));
	AppendMenu(m_hMenu, MF_STRING, IDM_REFRESH, TradeInfoStr::_GetTextTIW(TradeInfoStr::Refresh_Index));

}

void TPositionView::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
}

void TPositionView::OnSize(WPARAM wParam, LPARAM lParam)
{
	m_posList.MoveWindow(0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}

void TPositionView::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	switch (p->hdr.code)
	{
	case NM_RCLICK:
	{
		//memset(&m_stMenuInfo, 0, sizeof(TMenuInfo));
		bool bEnable = false;
		if (p->iItem != -1 && p->iItem != m_posList.GetItemCount() - 1)
		{
			bEnable = true;
			//右键获取数据
			//m_stMenuInfo.Index = p->iItem;
			//if (m_stMenuInfo.Index < m_pConTotalPosition->size())
			//	m_stMenuInfo.pData = m_pConTotalPosition->at(m_stMenuInfo.Index);
		}
		else if (p->iItem == m_posList.GetItemCount() - 1)
		{
			m_posList.SelectItem(-1); //取消选中
			bEnable = false;
		}
		//EnableMenu(p->hdr.idFrom, bEnable);
		//ShowMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition]);
	}
	break;
	case NM_CLICK:
	{
		if (p->iItem != -1)
		{
			//m_Mediator->Send(CMD_ViewTrade_SetContract, (void*)p->iItem, this); //Set Contract
		}
	}
	break;
	case LVN_GETDISPINFO:
		m_posList.OnGetDispInfo(lParam);
		break;
	case NM_DBLCLK:
	{
		if (p->iItem != -1)
		{
			int iItemCount = m_posList.GetItemCount();
		//	if (p->iItem == iItemCount - 1 && iItemCount != 1)
		//		m_Mediator->Send(CMD_ViewTrade_DbLClick, 0, this);
		//	else if (p->iItem >= 0 && p->iItem < m_pConTotalPosition->size())
		//		m_Mediator->Send(CMD_ViewTrade_DbLClickOneLine, (void*)p->iItem, this);
		}
	}
	break;
	}
}

void TPositionView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDM_RIVALCOLSE:
		break; 
	case IDM_BACKHAND:
		break;
	case IDM_STOPLOSS:
			break; 
	case IDM_BREAKEVEN:
			break;
	case IDM_TIMINGCLOSE:
		break;
	case IDM_MARKETCLOSE:
		break;
	case IDM_CLOSESH:
		break;
	case IDM_MOVEPOSITION:
		break; 
	case IDM_MARKETBACKHAND:
		break;
	case IDM_REFRESH:
		break;
	case IDM_CANCELCLOSE:
		break;
	}
}