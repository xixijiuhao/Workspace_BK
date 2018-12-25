#include "PreInclude.h"

void TMediator::Send(unsigned int CommandID, const void* content, IColleague* colleague)
{
	if (colleague == m_colleaguePresenter)
	{
		DealPresenter(CommandID, content);
	}
	else if (colleague == m_TreeKeyView)
	{
		DealThreeKey(CommandID, content);
	}
    else if (colleague == m_LogView)
    {
        DealLog(CommandID, content);
    }
	else if (colleague == m_OrderView)
	{
		DealOrderView(CommandID, content);
	}
	else if (colleague == m_TradeView)
	{
		DealTradeView(CommandID, content);
	}
	else if (colleague == m_StopLPView)
	{
		DealStopLPView(CommandID, content);
	}
	else if (colleague == m_pConditionOrderView)
	{
		DealConView(CommandID, content);
	}
	else if (colleague == (IColleague*)100)
	{
		if (CommandID == TConditionOrderFrm::CMD_ConView_UpdateConOrder)
		{
			g_Presenter->ModifyPreAutoOrder((TConditionOrder*)content);
			m_TradeView->UpdateConOrder();
		}
	}
}

void TMediator::Get(unsigned int CommandID, void *& pVoid, IColleague *colleague)
{
    if (colleague == m_LogView)
    {
        DealLogGet(CommandID, pVoid);
    }
    else if (colleague == m_TradeView)
    {
        DealTradeGet(CommandID, pVoid);
    }

	else if (colleague == m_OrderView)
	{
		DealOrderGet(CommandID, pVoid);
	}
	else if (colleague == m_TreeKeyView)
	{
		DealTViewGet(CommandID, pVoid);
	}
	else if (colleague == m_StopLPView)
	{
		DealTStopLPGet(CommandID, pVoid);
	}
    
}
void TMediator::DealPresenter(unsigned int CommandID, const void* content)
{
	switch (CommandID)
	{
	case TPresenter::CMD_Presenter_ShowHide:
		m_TreeKeyView->ShowHide((int)content);
        break;
    case TPresenter::CMD_Presenter_Notice_OrderView:
    {
        m_OrderView->UpdateShowData();
        m_LogView->UpdateLog();
		if (m_pOpenStopView && IsWindowVisible(m_pOpenStopView->GetHwnd()))
			m_pOpenStopView->UpdateCode();
		if (m_pConditionOrderView&&IsWindowVisible(m_pConditionOrderView->GetHwnd()))
			m_pConditionOrderView->Update();
        break;
    }
    case TPresenter::CMD_Presenter_OnQuote:
    {
        m_LogView->UpdateData(); 
		m_OrderView->UpdatePrice();
		if (m_pOpenStopView && IsWindowVisible(m_pOpenStopView->GetHwnd()))
			m_pOpenStopView->Update();
		if (m_pConditionOrderView&&IsWindowVisible(m_pConditionOrderView->GetHwnd()))
			m_pConditionOrderView->Update(false);
        break;
    }
	case TPresenter::CMD_Presenter_UserLogin:
		m_TreeKeyView->OnUserLogin((int)content);
		m_OrderView->UpdateView();
		break;
	case TPresenter::CMD_Presenter_OnMoney:
		m_TreeKeyView->UpdateMoney((const TMoney*)content);
		m_TradeView->UpdateMoney((const TMoney*)content);
		break;
    case TPresenter::CMD_Presenter_OnParOrder:
    {
		m_TradeView->UpdateParOrder((uint)content);
        break; 
    }
    case TPresenter::CMD_Presenter_OnOrder:
    {
        m_TradeView->UpdateOrder((TOrder*)content);
		m_LogView->UpdateLog();
        break;
    }
    case TPresenter::CMD_Presenter_OnMatch:
    {
        m_TradeView->UpdateMatch((TMatch*)content);
        break;
    }
	case TPresenter::CMD_Presenter_OnPositionTotal:
		if ((bool)content)
		{
			m_TradeView->UpdatePosition();
			m_OrderView->UpdateShowData();
			m_OrderView->UpdatePrice();
		}
		break;
	case TPresenter::CMD_Presenter_SetStopLPView:
		m_StopLPView->SetData(*(int*)content,true);
		break;
	case TPresenter::CMD_Presenter_SetStopLPView_FindKeyID:
		m_StopLPView->SetData(*(int*)content,false);
		break;
	case TPresenter::CMD_Presenter_ComboxUpdate:
		m_StopLPView->ComboxUpdate((const TStopLPitem*)content);
		break;
	case TPresenter::CMD_Presenter_UpdateTViewMoney:
		m_TradeView->UpdateMoney((const TMoney*)content, false);
		break;
	case TPresenter::CMD_Presenter_TViewUpdate:
		m_TradeView->SetNeedUpdate();
		break;
	case TPresenter::CMD_Presenter_OnStopLPOrderList:
		m_TradeView->UpdateStopLP((const TStopLPitem*)content);
		break;
	case TPresenter::CMD_Presenter_UpdateConOrder:
		m_TradeView->UpdateConOrder();
		break;
	case TPresenter::CMD_Presenter_UpdateDeposit:
		m_TradeView->UpdateProfit();
		break;
	case TPresenter::CMD_Presenter_SetSelectedPosition:
		m_TradeView->SetSelectedPosition((const TPosition *)content);
		break;
	default:
		break;
	}
}

void TMediator::DealThreeKey(unsigned int CommandID, const void* content)
{
	if (!m_ConfigView) return;
	switch (CommandID)
	{
	case TViewThreeKey::CMD_TView_SwitchFrm:
	{
		WPARAM data = (WPARAM)content;
		int formerIndex = HIWORD(data);
		int nowIndex = LOWORD(data);
		m_ConfigView->ShowFrm(formerIndex, nowIndex);
		m_OrderView->ShowFrm(formerIndex, nowIndex);
		m_LogView->ShowFrm(formerIndex, nowIndex);
		m_TradeView->ShowFrm(formerIndex, nowIndex);
		if (nowIndex == ID_frmTKeyOrder)//从其他界面切换过来，配置数据做了修改，要更新
		{
			m_colleaguePresenter->UpdateConfig();
		}
		break;
	}
	case TViewThreeKey::CMD_TView_SizeChange:
		m_ConfigView->SetShowRect(*(RECT*)content);
		m_OrderView->SetShowRect(*(RECT*)content);
		m_LogView->SetShowRect(*(RECT*)content);
		m_TradeView->SetShowRect(*(RECT*)content);
		break;
	case TViewThreeKey::CMD_TView_UserChange:
		m_colleaguePresenter->UserChange((int)content);
		m_OrderView->UpdateView();
		break;
	}
}
void TMediator::DealTViewGet(unsigned int CommandID, void*&content)
{
	switch (CommandID)
	{
	case TViewThreeKey::CMD_TView_GetComboxCont:
		content = m_colleaguePresenter->GetUserComboxData();
		break;
	case TViewThreeKey::CMD_TView_GetCurrencyNo:
		content = m_colleaguePresenter->GetCurrencyNo();
		break;
	}
}

void TMediator::DealLog(unsigned int CommandID, const void* content)
{
    if (!m_LogView) return;
    switch (CommandID)
    {
        case TLogView::CMD_TLog_Setprice:
			m_OrderView->SetOrderPrice((const char*)content);
            break;
        case TLogView::CMD_TLog_SetQty:
			m_OrderView->SetOrderQty((const char*)content);
            break;
		case TLogView::CMD_TLog_OpenLoss:
		{
			if (!m_pOpenStopView)
			{
				m_pOpenStopView = new TOpenStopFrm;
				m_pOpenStopView->SetCommInfoData((const SOFCommodityInf*)m_colleaguePresenter->GetCommInfo());
				m_pOpenStopView->SetOrderData((const TOrderViewData*)m_colleaguePresenter->GetOrderData());
				m_pOpenStopView->SetQuoteData((const TQuoteData*)m_colleaguePresenter->GetQuoteData());

			}
			char szText[20] = { 0 };
			if (g_language == CHT)strcpy_s(szText, "止損開倉");
			else if (g_language == ENU)strcpy_s(szText, "Open Width Stop");
			else strcpy_s(szText, "止损开仓");
			m_pOpenStopView->ShowFrm(m_LogView->GetHwnd(), L"OpenStopFrm", szText);
			POINT pt;
			GetCursorPos(&pt);
			m_pOpenStopView->MoveWindow(pt.x, pt.y - 120, 557, 237);
			m_pOpenStopView->Update();
			m_pOpenStopView->UpdateCode();
		}
			break;
		case TLogView::CMD_TLog_ConOrder:
		{
			if (!m_pConditionOrderView)
			{
				m_pConditionOrderView = new TConditionOrderFrm(this);
				m_pConditionOrderView->SetCommInfoData((const SOFCommodityInf*)m_colleaguePresenter->GetCommInfo());
				m_pConditionOrderView->SetOrderData((const TOrderViewData*)m_colleaguePresenter->GetOrderData());
				m_pConditionOrderView->SetQuoteData((const TQuoteData*)m_colleaguePresenter->GetQuoteData());
			//	m_pConditionOrderView->SetLinkageHwnd(m_colleaguePresenter->GetHwnd());
				char szText[20] = { 0 };
				if (g_language == CHT)strcpy_s(szText, "條件單");
				else if (g_language == ENU)strcpy_s(szText, "Condition Order");
				else strcpy_s(szText, "条件单");
				m_pConditionOrderView->ShowFrm(m_LogView->GetHwnd(), L"ConOrder", szText);
			}
			m_pConditionOrderView->MoveWindow();
			m_pConditionOrderView->Update();
		}
			break;
    }
}
void TMediator::DealLogGet(unsigned int CommandID, void*& content)
{
    if (!m_LogView) return;
    switch (CommandID)
    {
    case TLogView::CMD_TLog_GetQuoteData:
		content = m_colleaguePresenter->GetQuoteData();
		break;
    case TLogView::CMD_TLog_GetLog:
        content = m_colleaguePresenter->GetLog();

    }
}

void TMediator::DealTradeView(unsigned int CommandID, const void* content)
{
	switch (CommandID)
	{
	case TTradeInfo::CMD_ViewTrade_SetContract:
		m_colleaguePresenter->SetContract((int)content);
		break;
	case TTradeInfo::CMD_ViewTrade_ClickOrderState:
		m_colleaguePresenter->SetOrderStateClcikIndex((int)content);
		break;
	case TTradeInfo::CMD_ViewTrade_ClickBtn:
		m_colleaguePresenter->OnClickBtn(TradeInfoModel::btnFromTradeInfo, (LPARAM)content);
		break;
	case TTradeInfo::CMD_ViewTrade_OnCommand:
		m_colleaguePresenter->OnCommand((TMenuInfo*)content);
		break;
	case TTradeInfo::CMD_ViewTrade_DbLClick:
		m_colleaguePresenter->CoverAll();
		break;
	case TTradeInfo::CMD_ViewTrade_UpdateMoney:
		m_colleaguePresenter->UpdateMoney();
		break;
	case TTradeInfo::CMD_ViewTrade_DbLClickOneLine:
		m_colleaguePresenter->CoverOne((int)content);
		break;
	case TTradeInfo::CMD_ViewTrade_ConMenuClick:
		m_colleaguePresenter->OnConMenuClick((TMenuInfo*)content);
		break;
	case TTradeInfo::CMD_ViewTrade_UpdateStopLP:
		m_colleaguePresenter->UpdateStopLP();
		break;

	}
}
void TMediator::DealTradeGet(unsigned int CommandID, void*& content)
{
    switch (CommandID)
    {
    case TTradeInfo::CMD_ViewTrade_BindPositionTotal:
		content = m_colleaguePresenter->GetPositionTotal();
        break;
    case TTradeInfo::CMD_ViewTrade_BindParOrder:
		content = m_colleaguePresenter->GetParOrder();
        break;
    case TTradeInfo::CMD_ViewTrade_BindOrder:
        content = m_colleaguePresenter->GetOrder();
        break;
    case TTradeInfo::CMD_ViewTrade_BindMatch:
        content = m_colleaguePresenter->GetMatch();
        break;
    case TTradeInfo::CMD_ViewTrade_BindMoney:
        break;
	case TTradeInfo::CMD_ViewTrade_BindStopLPOrder:
		content = m_colleaguePresenter->GetStopLPOrder();
		break;
	case TTradeInfo::CMD_ViewTrade_BindConOrder:
		content = m_colleaguePresenter->GetConOrder();
		break;
	case TTradeInfo::CMD_ViewTrade_BindTrigConOrder:
		content = m_colleaguePresenter->GetTrigConOrder();
		break;
    default:
        break;
    }

}



void TMediator::DealOrderView(unsigned int CommandID, const void* content)
{
	switch (CommandID)
	{
	case TOrderView::CMD_OrderView_InsertOrder:
		m_colleaguePresenter->InsertOrder((TSendOrder*)content);
		break;
	case TOrderView::CMD_OredrView_SetContractNo:
		m_colleaguePresenter->DealContractCode(*(string*)content);
		break;
	case TOrderView::CMD_OrderView_LockState:
		m_colleaguePresenter->SetLockState((bool)(content));
		break;
	case TOrderView::CMD_OrderView_ClickBtn:
		m_colleaguePresenter->OnClickBtn(TradeInfoModel::btnFromOrderView, (LPARAM)content);
		break;
	}
}

void TMediator::DealOrderGet(unsigned int CommandID, void*& content)
{
	switch (CommandID)
	{
	case TOrderView::CMD_OrderView_GetOrderData:
		content = m_colleaguePresenter->GetOrderData();
		break;
	case TOrderView::CMD_OrderView_GetQuoteData:
		content = m_colleaguePresenter->GetQuoteData();
		break;
	case TOrderView::CMD_OrderView_GetCommInfo:
		content = m_colleaguePresenter->GetCommInfo();
		break;
	}
}

void TMediator::DealStopLPView(unsigned int CommandID, const void* content)
{
	switch (CommandID)
	{
	case TStopLPView::CMD_StopLPView_InsertOrder:
		m_colleaguePresenter->InsertStopLPOrder((TStopLPInsertItem*)content);
		break;
	case TStopLPView::CMD_StopLPView_UpdatePositionPrice:
		m_colleaguePresenter->UpdatePositionPrice();
		break;
	case TStopLPView::CMD_StopLPView_DeleteOrder:
		m_colleaguePresenter->DeleteStopLPOrder((const unsigned int*)content);
		break;
	case TStopLPView::CMD_StopLPView_ModifyOrder:
		m_colleaguePresenter->ModifyStopLPOrder((TStopLPitem*)content);
		break;
	default:
		break;
	}
}

void TMediator::DealTStopLPGet(unsigned int CommandID, void*&content)
{
	switch (CommandID)
	{
	case TStopLPView::CMD_StopLPView_GetData:
		content = m_colleaguePresenter->GetStopLPData();
		break;
	case TStopLPView::CMD_StopLPView_GetComboxData:
		content = m_colleaguePresenter->GetStopLPComboxData();
	default:
		break;
	}
}

void TMediator::DealConView(unsigned int CommandID, const void* content)
{
	if (CommandID == TConditionOrderFrm::CMD_ConView_InsertOrder)
	{
		if(m_colleaguePresenter->InsertConOrder((TConditionOrder*)content))
			m_TradeView->UpdateOneConOrder((TConditionOrder*)content);
	}
	
}