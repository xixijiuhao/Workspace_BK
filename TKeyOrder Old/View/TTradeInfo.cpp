#include "BaseInclude.h"

#define tabHeight   20
#define lvTop       tabHeight


#define tabPos_ParOrder    0
#define tabOrder		   1
#define tabMatch	       2
#define tabConditionOrder  3
#define tabStopLP          4
#define tabMoney		   5


char szChsTI[][16] = { "条", "暂停", "平33%", "平50%", "平100%", "反手", "止损", "风控", "持仓", "委托", "成交", "条件单", "止损单", "资金", "提示",
"更新时间", "币种", "逐笔平盈", "资金调整", "出金", "入金", "今权益", "今可用", "今可提", "保证金", "维持保证金", "权利金", "净盈亏", "盈亏率", "风险率",//money
"平仓", "最新价", "时间到达", "开盘自动触发", "手动发出", "指定价 ", "点",
"无", "限价止损", "限价止盈", "限价止损+止盈", "动态追踪", "指定价止损+止盈" };
char szChtTI[][16] = { "條", "暫停", "平33%", "平50%", "平100%", "反手", "止損", "風控", "持倉", "委托", "成交", "條件單", "止損單", "資金", "提示",
"更新時間", "幣種", "逐筆平盈", "資金調整", "出金", "入金", "今權益", "今可用", "今可提", "保證金", "維持保證金", "權利金", "凈盈虧", "盈虧率", "風險率",//money
"平倉", "最新價", "時間到達", "開盤自動觸發", "手動發出", "指定價 ", "點",
"無", "限價止損", "限價止盈", "限價止損+止盈", "動態追蹤", "指定價止損+止盈" };
char szEnuTI[][25] = { " items", "stop", "Close 33%", "Close 50%", "Close 100%", "Backhand", "Stop", "Risk", "Pos","All Orders","Fill","Cond.","Stop","Balance","Warning",
"Update Time","Currency","Current Realized PnL","Adjustment","Withdraw","Doposit","Account Value","Excess Cash","Cash Widthdrawable","Initial Margin",
"Maintain Margin", "Premium", "Net PnL", "PnL%", "Risk%", "Close", "Last Price", "Time To", "Trig. When Open", "Send by hand", "Specific Price ", " Point",
"None","Limit Stop Loss","Limit Stop Profit","Limit Stop","Floating","Specific Price Stop"};
enum{
	Items_Index,Stop_Index,Close33_Index,Close50_Index,Close100_Index,BackHand_Index,StopLoss_Index,Risk_Index,Position_Index,Order_Index,
	Match_Index,ConOrder_Index,StopLP_Index,Fund_Index,Warning_Index,UpdateTime_Index,CurrencyNo_Index,CoverProfitTBT_Index,MoneyAdjust_Index,
	CashOut_Index,CashIn_Index,Equity_Index,Available_Index,CanCashOut_Index,Deposit_Index,KeepDeposit_Index,Premium,NetPrfoit_Index,
	ProfitRate_Index,RiskRate_Index,Close_Index,LastPrice_Index,TimeTo_Index,TigeOpen_Index,Send_Index,SpecificPrice_Index,Point_Index,
	None_Index,LimitStopLoss_Index,LimitStopProfit_Index,LimitStop_Index,Floating_Index,SpecificPriceStop_Index,
};

char * _GetTextTI(int i)
{
	if (g_language == CHT)return szChtTI[i];
	else if (g_language == ENU)return szEnuTI[i];
	return szChsTI[i];
}

TCHAR szChsTIW[][10] = { L"对价平仓", L"反手", L"设置止损单", L"设置止盈单", L"设置保本单", L"平组合单", L"保存", L"刷新", L"超价平仓", L"市价平仓",
L"指定价平仓", L"平上海昨仓", L"锁仓", L"移仓", L"撤平仓单+账户清仓", L"超价反手", L"市价反手",/*position*/
L"撤单", L"改价", L"对价跟进", L"排队跟进", L"超价跟进", L"市价跟进", L"撤所有挂单",/*委托*/
L"立即发出", L"发出所有埋单", L"修改", L"删除", L"全部删除", L"暂停", L"启动", L"未触发列表", L"已触发列表",/*条件单*/
L"重新开始",//止损单
L"是否确认撤单",

L"时间", L"合约", L"状态", L"买卖", L"开平", L"委托价", L"委托量", L"可撤", L"已成交", L"投保",//orderstate
L"品种", L"手数", L"可用", L"逐笔浮盈", L"盈损", L"保证金", L"开仓均价", L"盯市浮盈", L"今手数", L"今可用",//position
L"已撤", L"资金账号", L"订单号",//order
L"成交价", L"成交量", L"交易所成交号", L"成交号",//match
L"类型", L"条件",  L"止损方式", L"止盈",//condition order
L"触发价", L"触发次数", L"触发方式", L"保本价差", L"回撤价差",//stoploss list
L"可成交", L"平仓盈亏", L"止损"
};
TCHAR szChtTIW[][10] = { L"對價平倉", L"反手", L"設置止損單", L"設置止盈單", L"設置保本單", L"平組合單", L"保存", L"刷新", L"超價平倉", L"市價平倉",
L"指定價平倉", L"平上海昨倉", L"鎖倉", L"移倉", L"撤平倉單+賬戶清倉", L"超價反手", L"市價反手",//position
L"撤單", L"改價", L"對價跟進", L"排隊跟進", L"超價跟進", L"市價跟進", L"撤所有掛單",/*委托*/
L"立即發出", L"發出所有埋單", L"修改", L"刪除", L"全部刪除", L"暫停", L"啟動", L"未觸發列表", L"已觸發列表",/*條件單*/
L"重新開始",//止損單
L"是否確認撤單",

L"時間", L"合約", L"狀態", L"買賣", L"開平", L"委托價", L"委托量", L"可撤", L"已成交", L"投保",//orderstate
L"品種", L"手數", L"可用", L"逐筆浮盈", L"盈損", L"保證金", L"開倉均價", L"盯市浮盈", L"今手數", L"今可用",//position
L"已撤", L"資金賬號", L"訂單號",//order
L"成交價", L"成交量", L"交易所成交號", L"成交號",//match
L"類型", L"條件",  L"止損方式", L"止盈",//condition order
L"觸發價", L"觸發次數", L"觸發方式", L"保本價差", L"回撤價差",//stoploss list
L"可成交", L"平倉盈虧", L"止損"
};
TCHAR szEnuTIW[][30] = { L"Match Close", L"Backhand", L"Stop Loss", L"Stop Profit", L"Brean Even", L"Close Combo", L"Save", L"Refresh", L"Over Close", L"Markt Close",
L"Sepcific Close", L"Close SHFE Pre", L"Lock", L"Shift", L"Cancel All + Close All", L"Over Backhand", L"Market Backhand",
L"Cancel", L"Change Price", L"Match Follow", L"Working Follow", L"Over Follow", L"Market Follow", L"Cancel All",
L"Send", L"Send All Pre Order", L"Modify", L"Delete", L"Delete All", L"Stop", L"Start", L"UnTrig", L"Trigged",
L"Restart", L"Cancel Order?",
L"Time", L"Contract", L"Status", L"Buy/Sell", L"Open/Close", L"Order Price", L"Order Qty", L"Can Cancel", L"Dealt Qty", L"Spec/Hedg",
L"Product", L"Qty", L"Available Qty", L"Current Floating PnL", L"Floating PnL", L"Initial Margin", L"Avg. Price", L"Dingshi PnL", L"Today's Qty",
L"Toady's Available Qty",
L"Cancelled", L"Account No.", L"OrderNo", 
L"Trans. Price", L"Dealt Qty", L"Exchange Trans. Mo.", L"Trans No.",
L"Type", L"Condition", L"Stop Method", L"Stop Profit",
L"Trig Price", L"Trig Count", L"Trig Cond.", L"Break Even Point", L"Floating Point",
L"Can Filling", L"Profit", L"Stop Loss"
};

enum{
	CounterClose_Index,BackHandW_Index,SetStopLossW_Index,SetStopProfitW_Index,SetBreakEvenW_Index,CloseCombo_Index,Save_Index,Refresh_Index,
	OverClose_Index,MarketClose_Index,SepcficCLose_Index,CloseSHFE_Index,Lock_Index,Shift_Index,CancelClose_Index,OverBackHand_Index,MarketBackHand_Index,
	Cancel_Index,ChangePrice_Index,CounterFollow_Index,QueueFollow_Index,OverFollow_Index,MarketFollow_Index,CancelAll_Index,
	SendW_Index,SendAllPreOrder_Index,Modify_Index,Delete_Index,DeleteAll_Index,StopW_Index,Start_Index,UnTrig_Index,Trigged_Index,
	Restart_Index,CancelOrder_Index,
	OrderTime_Index,Contract_Index,OrderStatus_Index,BuySell_Index,OpenClose_Index,OrderPrice_Index,OrderQty_Index,CanCancel_Index,MatchQtyAlready_Index,SpecHedg_Index,
	Product_Index,Qty_Index,AvaQty_Index,CurrentFloatingPnL_Index,FloatingPnL_Index,InitialMargin_Index,AvgPrice_Index,DingshiPnL_Index,ToadyQty_Index,
	ToadyAvaQty_Index,
	Cancelled_Index,AccountNo_Index,OrderNo_Index,
	MatchPrice_Index,MatchQty_Index,ExchangeMatchNo_INdex,MatchNo_Index,
	ConType_Index, Condition_Index, StopMethod_Index,StopProfitW_Index,
	TrigPrice_Index, TrigCount_Index, TrigCond_Index, BreakEvenPoint_Index,FloatingPoint_Index,
	CanMatch_Index,MatchProfit_Index,StopLossW_Index,
};
TCHAR * _GetTextTIW(int i)
{
	if (g_language == CHT)return szChtTIW[i];
	else if (g_language == ENU)return szEnuTIW[i];
	return szChsTIW[i];
}


TTradeInfo::TTradeInfo(IMediator *pMediator, HWND hParent) :IColleague(pMediator)
{
	m_hParent = hParent;
	m_pSpeifiedCover = nullptr;
	m_pShiftPosition = nullptr;
	memset(&m_stMenuInfo,0,sizeof(TMenuInfo));
	m_iSelParOrderID = -1;

	m_bNeedUpdateOrder = false;
	m_bNeedUpdateMatch = false;
	m_bNeedUpdatePosition = true;
	m_bNeedUpdateParOrder = true;
	m_bNeddUpdateMoney = false;

	m_bShowTrig = false;
	m_bRunState = true;  //启动中

	m_pSelectedPosition = nullptr;
}
TTradeInfo::~TTradeInfo()
{

}

void TTradeInfo::SetShowRect(RECT rect)
{
	m_iFrmTop = rect.top;
	m_iFrmWidth = rect.right - m_iFrmLeft;
	m_iFrmHeight = rect.bottom - rect.top;
	if (rect.right < m_iFrmLeft)
		m_iFrmWidth = 0;
	if (m_bShow)
	{
		ShowFrm( -1,ID_frmTKeyOrder);
	}
}

void TTradeInfo::ShowFrm(int formerIndex, int nowIndex)
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
	SetWindowPos(m_Hwnd, 0, m_iFrmLeft, m_iFrmTop, m_iFrmWidth, m_iFrmHeight, show);
}

void TTradeInfo::UpdateParOrder(uint iParOrderId, bool bUpdate)
{
	if (bUpdate)
		m_bNeedUpdateParOrder = true;
	m_iSelParOrderID = iParOrderId;
	if (!m_pConParOrder || m_iTabSelect != tabPos_ParOrder)
	{
		m_bNeedUpdateParOrder = true;
		return;
	}
	if (!m_bNeedUpdateParOrder) return;
	m_listOrderState.DeleteAllItems();
	int i = 0;
	vector<const void*>*pVecData = m_pConParOrder->GetData();
	vector<const void*>::iterator iter = pVecData->begin();

	int iSelectIndex = -1;
	for (; iter != pVecData->end();)
	{
		TOrder * pOrder = (TOrder *)(*iter);
		char szText[260] = { 0 };
		int col = 0;

		m_listOrderState.AddItem(pOrder->InsertDateTime, i, col++);
		switch (pOrder->CommodityType)
		{
		case ctFutures:
			sprintf_s(szText, "%s%s", pOrder->CommodityNo, pOrder->ContractNo);
			break;
		case ctOption:
			sprintf_s(szText, "%s%s%c%s", pOrder->CommodityNo, pOrder->ContractNo, pOrder->OptionType, pOrder->StrikePrice);
			break;
		default:
			sprintf_s(szText, "%s%s", pOrder->CommodityNo, pOrder->ContractNo);
			break;
		}
		m_listOrderState.AddItem(szText, i, col++);
		TOrderState2Char(pOrder->OrderState, pOrder->StrategyType, szText);
		m_listOrderState.AddItem(szText, i, col++);;

		TDirect2Char(pOrder->Direct, szText);
		
		pOrder->Direct == dBuy ? m_listOrderState.AddItem(szText, i, col++, DT_LEFT, RGB(240, 0, 0))
			: m_listOrderState.AddItem(szText, i, col++, DT_RIGHT, RGB(0, 150, 0));
		TOffset2Char(pOrder->Offset, szText);
		m_listOrderState.AddItem(szText, i, col++);

		const SCommodity * pCommodity = pOrder->pCommodity;
		if (!pCommodity)
		{
			pCommodity = g_pTradeData->GetCommodity(pOrder);
		}
		if (pCommodity)
		{
			SPriceStrType strOrderPrice = "";
			g_pStarApi->FormatPrice(pOrder->OrderPrice, pCommodity, strOrderPrice, false, false);
			m_listOrderState.AddItem(strOrderPrice, i, col++);
		}
		else {
			col++;
		}


		m_listOrderState.AddItem((INT)pOrder->OrderQty, i, col++);
		m_listOrderState.AddItem((INT)pOrder->OrderQty - (INT)pOrder->MatchQty, i, col++);//可撤
		//TSTRING::find()
		m_listOrderState.AddItem((INT)pOrder->MatchQty, i, col++);
		THedge2Char(pOrder->Hedge, szText);
		m_listOrderState.AddItem(szText, i, col++);

		if (pOrder->OrderID == iParOrderId)
			iSelectIndex = i;
		i++;
		iter++;
	}
	m_listOrderState.SetItemCount(m_pConParOrder->size());
	m_listOrderState.SelectItem(iSelectIndex);
}
void TTradeInfo::UpdateAvaQty(const TOrder* pOrder)
{
	if (!pOrder) return;
	TDirect direct = dBuy;
	if (pOrder->Direct == dBuy)
		direct = dSell;
	const TPosition* pst = g_pTradeData->GetPositionTotal(pOrder->UserNo, pOrder, direct);
	if (!pst)return;
	int AvaQty = 0, TAvaQty = 0, iRow = 0;
	CConvert::GetAvailableQty(pst, AvaQty, TAvaQty);
	m_pConTotalPosition->FindItem(pst, iRow);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddItem(AvaQty, iRow, 4);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddItem(TAvaQty, iRow, 11);
	
	if (m_pConTotalPosition->size())
		m_listTradeInfo[ID_ListPosition - ID_ListPosition].SetItemCount(m_pConTotalPosition->size() + 1);
}
void TTradeInfo::UpdateOrder(TOrder* pOrder)
{
	UpdateAvaQty(pOrder);
	if (m_iTabSelect != tabOrder)
	{
		m_bNeedUpdateOrder = true;
		return;
	}
    if (pOrder == NULL)
    {
		m_bNeedUpdateOrder = true;
        ShowOrderAllItem();
		m_bNeedUpdateOrder = false;
        return;
    }

    int iRow = 0;
    m_pConOrder->FindItem(pOrder, iRow);
    if (iRow == m_listTradeInfo[ID_ListOrder-ID_ListPosition].GetItemCount())
    {
        ShowOrderOneItem(pOrder, iRow);
    }
    else
        UpdateOrderOneItem(pOrder, iRow);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].SetItemCount(m_pConOrder->size());
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].SelectItem(iRow);
}

void TTradeInfo::ShowOrderAllItem()
{
	if (!m_bNeedUpdateOrder) return;
    int count = m_pConOrder->size();
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].DeleteAllItems();
    for (int i = 0; i < count; i++)
    {
        TOrder * pOrder = (TOrder *)m_pConOrder->at(i);
        ShowOrderOneItem(pOrder, i);
    }
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].SetItemCount(m_pConOrder->size());
}
void TTradeInfo::ShowOrderOneItem(TOrder* pOrder, int i)
{
    char szText[260] = { 0 };
    int col = 0;
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(pOrder->InsertDateTime, i, col++);
	switch (pOrder->CommodityType)
	{
	case ctFutures:
		sprintf_s(szText, "%s%s", pOrder->CommodityNo, pOrder->ContractNo);
		break;
	case ctOption:
		sprintf_s(szText, "%s%s%c%s", pOrder->CommodityNo, pOrder->ContractNo, pOrder->OptionType, pOrder->StrikePrice);
		break;
	default:
		sprintf_s(szText, "%s%s", pOrder->CommodityNo, pOrder->ContractNo);
		break;
	}
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(szText, i, col++);
    col++;
    TDirect2Char(pOrder->Direct, szText);
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(szText, i, col++);
    TOffset2Char(pOrder->Offset, szText);
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(szText, i, col++);

	const SCommodity * pCommodity = pOrder->pCommodity;
	if (!pCommodity)
	{
		pCommodity = g_pTradeData->GetCommodity(pOrder);
	}
	if (pCommodity)
	{
		SPriceStrType strOrderPrice = "";
		g_pStarApi->FormatPrice(pOrder->OrderPrice, pCommodity, strOrderPrice, false, false);
		m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(strOrderPrice, i, col++);
	}
	else 
	{
		col++;
	}


    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem((int)pOrder->OrderQty, i, col++);

    UpdateOrderOneItem(pOrder, i);

    col = 10;
    THedge2Char(pOrder->Hedge, szText);
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(szText, i, col++);
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(pOrder->UserNo, i, col++);
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(pOrder->OrderNo, i, col++);
}

void TTradeInfo::UpdateOrderOneItem(TOrder* pOrder, int i)
{
    char szText[260] = { 0 };
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(pOrder->InsertDateTime, i, 0); //更新时间
    TOrderState2Char(pOrder->OrderState, pOrder->StrategyType, szText);
    m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(szText, i, 2);
    switch (pOrder->OrderState)
    {
    case osCanceled:
    case osPartCanceled:
        m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(0, i, 7);
        m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem((int)pOrder->MatchQty, i, 8);
        m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem((int)pOrder->OrderQty - (int)pOrder->MatchQty, i, 9);
        break;

    default:
        m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem((int)pOrder->OrderQty - (int)pOrder->MatchQty, i, 7);
        m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem((int)pOrder->OrderQty, i, 8);
        m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddItem(0, i, 9);
        break;
    }
}
void TTradeInfo::UpdateMatch(TMatch* pMatch)
{
	if (m_iTabSelect != tabMatch)
	{
		m_bNeedUpdateMatch = true;
		return;
	}
    if (pMatch == NULL)
	{
		m_bNeedUpdateMatch = true;
        ShowMatchAllItem();
		m_bNeedUpdateMatch = false;
        return;
    }
       
    int iRow = 0;
    m_pConMatch->FindItem(pMatch, iRow);
    if (iRow == m_listTradeInfo[ID_ListMatch-ID_ListPosition].GetItemCount())
    {
        ShowMatchOneItem(pMatch, iRow);
    }
    else
        UpdateMatchOneItem(pMatch, iRow);
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].SetItemCount(m_pConMatch->size());
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].SelectItem(iRow);

    int i = m_listTradeInfo[ID_ListMatch - ID_ListPosition].GetSelectItem();
}

void TTradeInfo::ShowMatchOneItem(TMatch* pMatch, int i)
{
    char szText[100] = { 0 };
    int col = 0;
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem(pMatch->MatchDateTime, i, col++);

	switch (pMatch->CommodityType)
	{
	case ctFutures:
		sprintf_s(szText, "%s%s", pMatch->CommodityNo, pMatch->ContractNo);
		break;
	case ctOption:
		sprintf_s(szText, "%s%s%c%s", pMatch->CommodityNo, pMatch->ContractNo,pMatch->OptionType,pMatch->StrikePrice);
		break;
	default:
		sprintf_s(szText, "%s%s", pMatch->CommodityNo, pMatch->ContractNo);
		break;
	}
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem(szText, i, col++);
    TDirect2Char(pMatch->Direct, szText, sizeof(szText));
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem(szText, i, col++);
    TOffset2Char(pMatch->Offset, szText, sizeof(szText));
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem(szText, i, col++);
    UpdateMatchOneItem(pMatch, i);
    col = 7;
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem(pMatch->ExchangeMatchNo, i, col++);
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem(pMatch->MatchNo, i, col++);
}
void TTradeInfo::UpdateMatchOneItem(TMatch* pMatch, int i)
{
	const SCommodity * pCommodity = pMatch->pCommodity;
	if (!pCommodity)
	{
		pCommodity = g_pTradeData->GetCommodity(pMatch);
	}
	if (pCommodity)
	{
		SPriceStrType strPrice = "";
		g_pStarApi->FormatPrice(pMatch->MatchPrice, pCommodity, strPrice, false, false);
		m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem(strPrice, i, 4);
		g_pStarApi->FormatPrice(pMatch->CoverProfit, pCommodity, strPrice, false, false);
		m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem(strPrice, i, 5);
	}
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddItem((int)pMatch->MatchQty, i, 6);
}
void TTradeInfo::ShowMatchAllItem()
{
	if (!m_bNeedUpdateMatch) return;
    int count = m_pConMatch->size();
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].DeleteAllItems();
    for (int i = 0; i < count; i++)
    {
        TMatch * pMatch = (TMatch *)m_pConMatch->at(i);
        ShowMatchOneItem(pMatch, i);
    }
    m_listTradeInfo[ID_ListMatch - ID_ListPosition].SetItemCount(m_pConMatch->size());
}
void TTradeInfo::UpdatePosition(bool bUpdate)
{
	if (bUpdate)
		m_bNeedUpdatePosition = true;
	if (m_iTabSelect != tabPos_ParOrder) 
	{
		m_bNeedUpdatePosition = true;
		return;
	}
	if (!m_bNeedUpdatePosition) return;
	m_listTradeInfo[0].DeleteAllItems();

	uint count = m_pConTotalPosition->size();

	for (int i = 0; i < count; i++)
	{
		UpdateOnePosition(i);
	}
	if (count > 0)
	{
		char szText[20] = { 0 };
		sprintf_s(szText, "%d%s", count,_GetTextTI(Items_Index));
		m_listTradeInfo[0].AddItem(szText, count, 0);
		UpdateProfit(false);
	}

	SetSelectedPosition(m_pSelectedPosition);
}
void TTradeInfo::UpdateProfit(bool bUpdateRow)
{
	uint count = m_pConTotalPosition->size();
	double dProfitTBT = 0;
	double dMargin = 0;
	double dProfit = 0;

	for (uint i = 0; i < count; i++)
	{
		TPosition*pPosition = (TPosition*)m_pConTotalPosition->at(i);
		dProfitTBT += pPosition->FloatProfitTBT;
		dMargin += pPosition->Deposit;
		dProfit += pPosition->FloatProfit;
		if (bUpdateRow)
		{
			AddProfit(ID_ListPosition, i, 5, pPosition->FloatProfitTBT);
			//	AddProfit(ID_ListPosition, i, 7, pPosition->Deposit);
			m_listTradeInfo[0].AddItem(pPosition->Deposit, i, 7);
			AddProfit(ID_ListPosition, i, 9, pPosition->FloatProfit);
		}
	}
	if (count > 0)
	{
		AddProfit(ID_ListPosition, count, 5, dProfitTBT);
		m_listTradeInfo[0].AddItem(dMargin, count, 7);
		AddProfit(ID_ListPosition, count, 9, dProfit);
		m_listTradeInfo[0].SetItemCount(count + 1);
	}
}
void TTradeInfo::UpdateOnePosition(int i)
{
	TPosition*pPosition = (TPosition*)m_pConTotalPosition->at(i);
	int  col = 0;

	const SCommodity * pCommodity = pPosition->pCommodity;
	if (!pCommodity)
	{
		pCommodity = g_pTradeData->GetCommodity(pPosition);
	}

	if (pCommodity)
	{
		wstring name = CConvert::UTF8ToUnicode(pCommodity->CommodityName);
		m_listTradeInfo[0].AddItem(name.c_str(), i, col++);
	}
	else
		m_listTradeInfo[0].AddItem("", i, col++);

	char szText[260] = { 0 };
//	sprintf_s(szText, "%s%s%c", pPosition->CommodityNo, pPosition->ContractNo, pPosition->Direct);

	switch (pPosition->CommodityType)
	{
	case ctFutures:
		sprintf_s(szText, "%s%s", pPosition->CommodityNo, pPosition->ContractNo);
		break;
	case ctOption:
		sprintf_s(szText, "%s%s%c%s", pPosition->CommodityNo, pPosition->ContractNo, pPosition->OptionType, pPosition->StrikePrice);
		break;
	default:
		sprintf_s(szText, "%s%s", pPosition->CommodityNo, pPosition->ContractNo);
		break;
	}
		
	m_listTradeInfo[0].AddItem(szText, i, col++);;
	TDirect2Char(pPosition->Direct, szText);
	pPosition->Direct== dBuy ? m_listTradeInfo[0].AddItem(szText, i, col++, DT_LEFT, g_ColorRefData.g_ColorTextRed)
		: m_listTradeInfo[0].AddItem(szText, i, col++, DT_RIGHT, g_ColorRefData.g_ColorTextGreen);
	int AvaQty = 0;
	int TAvaQty = 0;
	CConvert::GetAvailableQty(pPosition, AvaQty, TAvaQty);
	m_listTradeInfo[0].AddItem((int)pPosition->PositionQty, i, col++);
	m_listTradeInfo[0].AddItem(AvaQty, i, col++);//可用？
	AddProfit(ID_ListPosition, i, col++, pPosition->FloatProfitTBT);

	m_listTradeInfo[0].AddItem("", i, col++);//盈损
	m_listTradeInfo[0].AddItem(pPosition->Deposit, i, col++);

	if (pCommodity)
	{
		SCommodityPrecType prec = pCommodity->PricePrec + 2;
		char szPriceFormat[20] = {};
		sprintf_s(szPriceFormat, "%%.%df", prec);
		m_listTradeInfo[0].AddItem(pPosition->PositionPrice, i, col++, szPriceFormat); // 开持仓均价？
	}
	else
		m_listTradeInfo[0].AddItem(pPosition->PositionPrice, i, col++, "%%.2f");

	AddProfit(ID_ListPosition, i, col++, pPosition->FloatProfit);
	m_listTradeInfo[0].AddItem((int)(pPosition->PositionQty - pPosition->PrePositionQty), i, col++);
	m_listTradeInfo[0].AddItem(TAvaQty, i, col++);
	THedge2Char(pPosition->Hedge, szText);
	m_listTradeInfo[0].AddItem(szText, i, col++);
}

void TTradeInfo::AddProfit(UINT lvID, int iRow, int iCol, double data)
{
	if (data > 0)
		m_listTradeInfo[lvID - ID_ListPosition].AddItem(data, iRow, iCol, DT_CENTER, g_ColorRefData.g_ColorTextRed);
	else if (data < 0)
		m_listTradeInfo[lvID - ID_ListPosition].AddItem(data, iRow, iCol, DT_CENTER, g_ColorRefData.g_ColorTextGreen);
	else
		m_listTradeInfo[lvID - ID_ListPosition].AddItem(data, iRow, iCol, DT_CENTER);
}

LRESULT TTradeInfo::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_SIZE:
		OnSize();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
    case WM_NOTIFY:
        OnNotify(wParam, lParam);
        return PROCESSED;
	case SSWM_TABTAG_SELECTED:
		OnTabSelChange();
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM:
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		m_Mediator->Send(CMD_ViewTrade_ClickBtn,(void*)lParam, this);
		return PROCESSED;
	case WM_COMMAND:
		if (HIWORD(wParam) == 0 && lParam == 0)
		{
			OnCommand(wParam, lParam);
		}
		return PROCESSED;
	}
	return NOT_PROCESSED;
}

void TTradeInfo::OnCommand(WPARAM wParam, LPARAM lParam)
{
	m_stMenuInfo.MenuID = LOWORD(wParam);
	switch (LOWORD(wParam))
	{
	case IDM_SAVE:
	{
		TWriteInfo clWrite;
		clWrite.WriteTradeData(TWriteInfo::writePositionData, m_pConTotalPosition, m_Hwnd);
		break;
	}
	case IDM_ORDER_SAVE:
	{
		TWriteInfo clWrite;
		clWrite.WriteTradeData(TWriteInfo::writeOrderData, m_pConOrder, m_Hwnd);
		break;
	}
	case IDM_MATCH_SVAE:
	{
		TWriteInfo clWrite;
		clWrite.WriteTradeData(TWriteInfo::writeMatchData, m_pConMatch, m_Hwnd);
		break;;
	}
	case IDM_REFRESH:
	{
		UpdatePosition();
		break;
	}
	case IDM_ORDER_FRESH:
		ShowOrderAllItem();
		break;
	case IDM_MATCH_FRESH:
		ShowMatchAllItem();
		break;
	case IDM_SPECIFIEDCLOSE:
	{
		ChangePrice(TSpecialPrice::frm_Postion);
		break;
	}
	case IDM_REFRESHORDERSTATE://刷新委托状态
	{
		int index = m_listOrderState.GetSelectItem();
		uint iOrderId = -1;
		if (index != -1 && index < m_pConParOrder->size())
			iOrderId = ((const TOrder*)m_pConParOrder->at(index))->OrderID;
		UpdateParOrder(iOrderId);
	}
		break;
	case IDM_EDITPRICE:       //改价
		ChangePrice(TSpecialPrice::frm_ParOrder);
		break;
	case IDM_MOVEPOSITION:    //移仓
	{
		if (!m_pShiftPosition)
		{
			m_pShiftPosition = new TShiftPosition;
		}
		m_pShiftPosition->Create(m_Hwnd);
		m_pShiftPosition->SetOrderInfo((const TPosition*)m_stMenuInfo.pData);
	}
		break;
	case IDM_MONEYFRESH:
		m_Mediator->Send(CMD_ViewTrade_UpdateMoney, 0, this);
		break;
	case IDM_TRIG:
	{

		UINT uMenuState = GetMenuState(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_NOTTRIG, MF_BYCOMMAND);
		if (uMenuState&MF_CHECKED)
		{
			CheckMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_TRIG, MF_CHECKED);
			CheckMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_NOTTRIG, MF_UNCHECKED);
			m_stMenuInfo.MenuID = LOWORD(wParam);
			m_Mediator->Send(CMD_ViewTrade_OnCommand, (void*)&m_stMenuInfo, this);
		}
		break;
	}
	case IDM_NOTTRIG:
	{
		UINT uMenuState = GetMenuState(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_TRIG, MF_BYCOMMAND);
		if (uMenuState&MF_CHECKED)
		{
			CheckMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_NOTTRIG, MF_CHECKED);
			CheckMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_TRIG, MF_UNCHECKED);
			m_stMenuInfo.MenuID = LOWORD(wParam);
			m_Mediator->Send(CMD_ViewTrade_OnCommand, (void*)&m_stMenuInfo, this);
		}
		break;
	}
	case IDM_COND_UNTRIG:
		m_bShowTrig = false;
		EnableConMenu(m_stMenuInfo.Index);
		UpdateConOrder();
		break;
	case IDM_COND_TRIG:
		m_bShowTrig = true;
		EnableConMenu(m_stMenuInfo.Index);
		UpdateConOrder();
		break;
	case IDM_COND_STOP:
		if (m_stMenuInfo.pData)
		{
			((TConditionOrder*)m_stMenuInfo.pData)->RunState = rsStop;
			m_bRunState = false; //stop
			EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_STOP, MF_GRAYED);
			EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_START, MF_ENABLED);
			//应该写个更简洁的函数来刷新列表
			UpdateConOrder();
		}
		break;
	case IDM_COND_START:
		if (m_stMenuInfo.pData)
		{
			((TConditionOrder*)m_stMenuInfo.pData)->RunState = rsRun;
			m_bRunState = true;
			EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_STOP, MF_ENABLED);
			EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_START, MF_GRAYED);
			UpdateConOrder();
		}
		break;
	case TTradeInfo::IDM_COND_MODITFY:
		if (m_stMenuInfo.pData)
		{
			m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddItem(_GetTextTI(Stop_Index), m_stMenuInfo.Index, 2);
			EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_STOP, MF_GRAYED);
			EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_START, MF_ENABLED);
			m_clModifyConOrder.RegisterConOrder((TConditionOrder*)m_stMenuInfo.pData, m_Hwnd, m_Mediator);
		}
		break;
	case TTradeInfo::IDM_COND_DELALL:
	case TTradeInfo::IDM_COND_SENDORDER://立即发出
	case TTradeInfo::IDM_COND_DEL://删除、撤单
	case TTradeInfo::IDM_COND_SENDALLPREORDER:
	{
		int iShowTrig = m_bShowTrig;
		int i = iShowTrig & 0xFF;
		i <<= 24;
		m_stMenuInfo.Index = m_stMenuInfo.Index | ((iShowTrig & 0xFF) << 24);
		m_Mediator->Send(CMD_ViewTrade_ConMenuClick, &m_stMenuInfo, this);
		break;
	}
	default:
		m_Mediator->Send(CMD_ViewTrade_OnCommand, (void*)&m_stMenuInfo, this);
		break;
	}
}

void TTradeInfo::EnableConMenu(int index)
{
	UINT checkTrig = MF_UNCHECKED;
	UINT checkUnTrig = MF_CHECKED;
	UINT enable = MF_ENABLED;
	if (m_bShowTrig)
	{
		checkTrig = MF_CHECKED;
		checkUnTrig = MF_UNCHECKED;
		enable = MF_GRAYED;
	}
	CheckMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_UNTRIG, checkUnTrig);
	CheckMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_TRIG, checkTrig);

	EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_SENDORDER, enable);
	EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_SENDALLPREORDER, enable);
	EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_MODITFY, enable);

	UINT enableStop = MF_GRAYED;
	UINT enableStart = MF_GRAYED;
	if (!m_bShowTrig)
	{
		if (m_bRunState)
		{
			enableStop = MF_ENABLED;
		}
		else
		{
			enableStart = MF_ENABLED;
		}
	}
	EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_STOP, enableStop); 
	EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_START, enableStart);

	if (index == -1)
	{
		enable = MF_GRAYED;
		EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_SENDORDER, enable);
		EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_DEL, enable);
		EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_STOP, enable);
		EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_START, enable);
		EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_MODITFY, enable);
	}
	else
	{
		EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_DEL, MF_ENABLED);
	}
}

void TTradeInfo::ChangePrice(uint type)
{
	if (m_stMenuInfo.pData)
	{
		if (!m_pSpeifiedCover)
		{
			m_pSpeifiedCover = new TSpecialPrice;
		}
		m_pSpeifiedCover->Create(m_Hwnd, type);
		m_pSpeifiedCover->SetData(m_stMenuInfo.pData);
	}
}
void TTradeInfo::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	if (p->CtlID >= ID_ListPosition&&p->CtlID <= ID_ListMoney)
		m_listTradeInfo[p->CtlID - ID_ListPosition].OnDrawItem(lParam);
	else if (p->CtlID == ID_ListOrderState)
		m_listOrderState.OnDrawItem(lParam);
}
void TTradeInfo::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	if (p->CtlID >= ID_ListPosition&&p->CtlID <= ID_ListMoney)
		m_listTradeInfo[p->CtlID - ID_ListPosition].OnMeasureItem(p->itemHeight);
	else if (p->CtlID == ID_ListOrderState)
		m_listOrderState.OnMeasureItem(p->itemHeight);
}

void TTradeInfo::OnNotify(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case ID_ListPosition:
        DealPosition(lParam);
        break;
    case ID_ListOrder:
        DealOrderList(lParam);
        break;
    case ID_ListMatch:
        DealMatchList(lParam);
        break;
    case ID_ListConOrder:
		DealConditionOrder(lParam);
        break;
    case ID_ListStopLossOrder:
        DealStoplossOrder(lParam);
        break;
    case ID_ListMoney:
        DealMoneyList(lParam);
        break;
    case ID_ListOrderState:
        DealOrderState(lParam);
        break;
    default:
        break;
    }
}
void TTradeInfo::OnTabSelChange()
{
	int index = m_tabTradeInfo.GetSelIndex();
	if (m_iTabSelect == index)
		return;
	if (index == 0)  // 选择了第一个界面
	{
		RECT rect;
		GetClientRect(m_Hwnd, &rect);
		FirstTabPageShow(SW_SHOW);
		SetFirstPagePos(rect.right, rect.bottom);
		ShowWindow(m_listTradeInfo[m_iTabSelect].GetListHwnd(), SW_HIDE);
	}
	else
	{
		if (m_iTabSelect == 0)
			FirstTabPageShow(SW_HIDE);
		else
			ShowWindow(m_listTradeInfo[m_iTabSelect].GetListHwnd(), SW_HIDE);
		RECT rect;
		GetClientRect(m_Hwnd, &rect);
		SetWindowPos(m_listTradeInfo[index].GetListHwnd(), 0, 5, lvTop, rect.right - 5, rect.bottom - lvTop - 2, SWP_NOZORDER|SWP_SHOWWINDOW);
		//ShowWindow(m_listTradeInfo[index].GetListHwnd(), SW_SHOW);
	}
	m_iTabSelect = index;
	ShowTabFrm();
}
void TTradeInfo::ShowTabFrm()
{
	switch (m_iTabSelect)
	{
	case tabPos_ParOrder:
		UpdatePosition(false);
		UpdateParOrder(m_iSelParOrderID,false);
		m_bNeedUpdateParOrder = false;
		m_bNeedUpdatePosition = false;
		break;
	case tabOrder:
		ShowOrderAllItem();
		m_bNeedUpdateOrder = false;
		break;
	case tabMatch:
		ShowMatchAllItem();
		m_bNeedUpdateMatch = false;
		break;
	case tabConditionOrder:
		UpdateConOrder();
		break;
	case tabStopLP:
		m_Mediator->Send(CMD_ViewTrade_UpdateStopLP, 0, this);
		break;
	case tabMoney:
		m_Mediator->Send(CMD_ViewTrade_UpdateMoney, 0, this);
		m_bNeddUpdateMoney = false;
		break;
	}
}
void TTradeInfo::FirstTabPageShow(UINT show)
{
	ShowWindow(m_listTradeInfo[0].GetListHwnd(), show);
	ShowWindow(m_listOrderState.GetListHwnd(), show);
	for (int i = 0; i < 6; i++)
	{
		ShowWindow(m_btnStrategy[i].GetHwnd(), show);
	}
}
void TTradeInfo::OnSize()
{
	m_tabTradeInfo.MoveWindow(5, 0, m_iFrmWidth - 5, tabHeight);
	if (m_iTabSelect == 0)
	{
		SetFirstPagePos(m_iFrmWidth, m_iFrmHeight);
	}
	else
		SetWindowPos(m_listTradeInfo[m_iTabSelect].GetListHwnd(), 0, 5, lvTop, m_iFrmWidth - 5, m_iFrmHeight - lvTop - 2, SWP_NOZORDER);
}
void TTradeInfo::SetFirstPagePos(int cx, int cy)
{
	int height = (cy - lvTop - 2) / 2;
	int iWdith = cx - 5;
	SetWindowPos(m_listTradeInfo[0].GetListHwnd(), 0, 5, lvTop, iWdith, height, SWP_NOZORDER);
	SetWindowPos(m_listOrderState.GetListHwnd(), 0, 5, height + 57, iWdith, cy - height - 57, SWP_NOZORDER);
	int btnWidth = 64, btnHeight = 20, btnHgap = 72;
	int nBtn = (cx - 5) / 72;
	if (nBtn >= 0)
	{
		nBtn = nBtn > 6 ? 6 : nBtn;
		for (int i = 0; i < 6 - nBtn; i++)
			ShowWindow(m_btnStrategy[i].GetHwnd(), SW_HIDE);
		int left = cx - btnHgap;
		for (int i = 0; i < nBtn; i++)
		{
			SetWindowPos(m_btnStrategy[5 - i].GetHwnd(), 0, left, height + 32, btnWidth, btnHeight, SWP_SHOWWINDOW);
			left -= btnHgap;
		}
	}
}
void TTradeInfo::OnCreate()
{
	CreateWidget();
	InitWidget();
	SetWidgetPos();
    BindingData();
    InitListData();

	g_pTradeApi->SubscibePositionDeposit();
}
void TTradeInfo::BindingData()
{
	m_Mediator->Get(CMD_ViewTrade_BindPositionTotal, (void *&)m_pConTotalPosition, this);
	m_Mediator->Get(CMD_ViewTrade_BindParOrder, (void *&)m_pConParOrder, this);
    m_Mediator->Get(CMD_ViewTrade_BindOrder, (void *&)m_pConOrder, this);
	m_Mediator->Get(CMD_ViewTrade_BindMatch, (void *&)m_pConMatch, this);
	m_Mediator->Get(CMD_ViewTrade_BindMoney, (void *&)m_pConMoney, this);
	m_Mediator->Get(CMD_ViewTrade_BindStopLPOrder, (void *&)m_pConLPOrder, this);

	m_Mediator->Get(CMD_ViewTrade_BindConOrder, (void*&)m_pVecConOrder, this);
	m_Mediator->Get(CMD_ViewTrade_BindTrigConOrder, (void*&)m_pVecTrigConOrder, this);
}
void TTradeInfo::InitListData()
{
    UpdateParOrder(NULL);
    UpdatePosition();
    ShowOrderAllItem();
    ShowMatchAllItem();
	UpdateStopLP(NULL);
    //UpdateMoney();
    //UpdateConditionOrder();
}

void TTradeInfo::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
}
void TTradeInfo::InitMenu()
{
    for (int i = 0; i <= ID_ListMoney - ID_ListPosition; i++)
    {
        m_menuTradeInfo[i] = CreatePopupMenu();
    }
    m_menuOrderState = CreatePopupMenu();

    AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_RIVALCOLSE, _GetTextTIW(CounterClose_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_BACKHAND, _GetTextTIW(BackHandW_Index));
    AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_STOPLOSS, _GetTextTIW(SetStopLossW_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_STOPWIN, _GetTextTIW(SetStopProfitW_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_BREAKEVEN, _GetTextTIW(SetBreakEvenW_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_TIMINGCLOSE, _GetTextTIW(CloseCombo_Index));
    AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_SAVE, _GetTextTIW(Save_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_REFRESH, _GetTextTIW(Refresh_Index));

	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_MENUBARBREAK | MF_STRING, IDM_OVERCLOSE, _GetTextTIW(OverClose_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_MARKETCLOSE, _GetTextTIW(MarketClose_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_SPECIFIEDCLOSE, _GetTextTIW(SepcficCLose_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_CLOSESH, _GetTextTIW(CloseSHFE_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_LOCKPOSITION, _GetTextTIW(Lock_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_MOVEPOSITION, _GetTextTIW(Shift_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_CANCELCLOSE, _GetTextTIW(CancelClose_Index));
    AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_OVERBACKHAND, _GetTextTIW(OverBackHand_Index));
	AppendMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], MF_STRING, IDM_MARKETBACKHAND, _GetTextTIW(MarketBackHand_Index));


	AppendMenu(m_menuOrderState, MF_STRING, IDM_CANCEL, _GetTextTIW(Cancel_Index));
	AppendMenu(m_menuOrderState, MF_STRING, IDM_EDITPRICE, _GetTextTIW(ChangePrice_Index));
    AppendMenu(m_menuOrderState, MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuOrderState, MF_STRING, IDM_REFRESHORDERSTATE, _GetTextTIW(Refresh_Index));
	AppendMenu(m_menuOrderState, MF_STRING | MF_MENUBARBREAK, IDM_RIVALFOLLOW, _GetTextTIW(CounterFollow_Index));
	AppendMenu(m_menuOrderState, MF_STRING, IDM_LINEUPFOLLOW, _GetTextTIW(QueueFollow_Index));
	AppendMenu(m_menuOrderState, MF_STRING, IDM_OVERFOLLOW, _GetTextTIW(OverFollow_Index));
	AppendMenu(m_menuOrderState, MF_STRING, IDM_MARKETFOLLOW, _GetTextTIW(MarketFollow_Index));
    AppendMenu(m_menuOrderState, MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuOrderState, MF_STRING, IDM_CANCELALL, _GetTextTIW(CancelAll_Index));


	//条件单
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING, IDM_COND_SENDORDER, _GetTextTIW(SendW_Index));
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING, IDM_COND_SENDALLPREORDER, _GetTextTIW(SendAllPreOrder_Index));
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING, IDM_COND_MODITFY, _GetTextTIW(Modify_Index));
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING, IDM_COND_DEL, _GetTextTIW(Delete_Index));
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING, IDM_COND_DELALL, _GetTextTIW(DeleteAll_Index));
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING | MF_MENUBARBREAK, IDM_COND_STOP, _GetTextTIW(StopW_Index));
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING, IDM_COND_START, _GetTextTIW(Start_Index));
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING, IDM_COND_UNTRIG, _GetTextTIW(UnTrig_Index));
	AppendMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], MF_STRING, IDM_COND_TRIG, _GetTextTIW(Trigged_Index));
	CheckMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_UNTRIG, MF_UNCHECKED);
	CheckMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_TRIG, MF_CHECKED);
	EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_STOP, MF_ENABLED);
	EnableMenuItem(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition], IDM_COND_START, MF_GRAYED);

	AppendMenu(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], MF_STRING, IDM_STOPCANCEL, _GetTextTIW(Delete_Index));
	AppendMenu(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], MF_STRING, IDM_STOPCHANGE, _GetTextTIW(Modify_Index));
	AppendMenu(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], MF_STRING, IDM_STOPPAUSE, _GetTextTIW(StopW_Index));
	AppendMenu(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], MF_STRING, IDM_STOPSTART, _GetTextTIW(Restart_Index));
	AppendMenu(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], MF_STRING, IDM_STOPDELETEALL, _GetTextTIW(DeleteAll_Index));
	AppendMenu(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], MF_STRING | MF_UNCHECKED, IDM_TRIG, _GetTextTIW(Trigged_Index));
	AppendMenu(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], MF_STRING | MF_CHECKED, IDM_NOTTRIG, _GetTextTIW(UnTrig_Index));

	AppendMenu(m_menuTradeInfo[ID_ListOrder - ID_ListPosition], MF_STRING, IDM_ORDER_SAVE, _GetTextTIW(Save_Index));
	AppendMenu(m_menuTradeInfo[ID_ListOrder - ID_ListPosition], MF_STRING, IDM_ORDER_FRESH, _GetTextTIW(Refresh_Index));

	AppendMenu(m_menuTradeInfo[ID_ListMatch - ID_ListPosition], MF_STRING, IDM_MATCH_SVAE, _GetTextTIW(Save_Index));
	AppendMenu(m_menuTradeInfo[ID_ListMatch - ID_ListPosition], MF_STRING, IDM_MATCH_FRESH, _GetTextTIW(Refresh_Index));

	AppendMenu(m_menuTradeInfo[ID_ListMoney - ID_ListPosition], MF_STRING, IDM_MONEYFRESH, _GetTextTIW(Refresh_Index));

   
}

void TTradeInfo::CreateWidget()
{

	m_tabTradeInfo.Create(m_Hwnd);

	int iCount = sizeof(m_listTradeInfo) / sizeof(m_listTradeInfo[0]);
	for (int i = 0; i < iCount -1; i++)
	{
		m_listTradeInfo[i].CreateList(m_Hwnd, LVS_REPORT /*| LVS_NOSORTHEADER*/ | LVS_OWNERDRAWFIXED | WS_BORDER|LVS_OWNERDATA, 20, 18, LVS_EX_FULLROWSELECT, ID_ListPosition + i);
	}
	m_listTradeInfo[iCount - 1].CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER | LVS_NOCOLUMNHEADER, 20, 18, LVS_EX_FULLROWSELECT, ID_ListMoney);
	m_listOrderState.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER | LVS_OWNERDATA, 20, 18, LVS_EX_FULLROWSELECT, ID_ListOrderState);

	//char btnText[][10] = { "平33%", "平50%", "平100%", "反手", "止损", "风控" };
	iCount = sizeof(m_btnStrategy) / sizeof(m_btnStrategy[0]);
	for (int i = 0; i < iCount; i++)
	{
		m_btnStrategy[i].Create(m_Hwnd, _GetTextTI(Close33_Index+i), ID_BtnCover33 + i);
	}
}

void TTradeInfo::InitWidget()
{
	std::vector<std::string> tabstr;

	tabstr.push_back(_GetTextTI(Position_Index));
	tabstr.push_back(_GetTextTI(Order_Index));
	tabstr.push_back(_GetTextTI(Match_Index));
	tabstr.push_back(_GetTextTI(ConOrder_Index));
	tabstr.push_back(_GetTextTI(StopLP_Index));
	tabstr.push_back(_GetTextTI(Fund_Index));
	m_tabTradeInfo.SetData(tabstr);

	InitListView();
    InitMenu();
}

void TTradeInfo::SetWidgetPos()
{

}
void TTradeInfo::DealPosition(LPARAM lParam)
{
    LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	switch (p->hdr.code)
	{
	case NM_RCLICK:
	{
		memset(&m_stMenuInfo, 0, sizeof(TMenuInfo));
		bool bEnable = false;
		if (p->iItem != -1 && p->iItem != m_listTradeInfo[0].GetItemCount() - 1)
		{
			bEnable = true;
			m_stMenuInfo.Index = p->iItem;
			if (m_stMenuInfo.Index < m_pConTotalPosition->size())
				m_stMenuInfo.pData = m_pConTotalPosition->at(m_stMenuInfo.Index);
		}
		else if (p->iItem == m_listTradeInfo[0].GetItemCount() - 1)
		{
			m_listTradeInfo[0].SelectItem(-1);
			bEnable = false;
		}
		EnableMenu(p->hdr.idFrom, bEnable);
		ShowMenu(m_menuTradeInfo[ID_ListPosition - ID_ListPosition]);
	}
		break;
	case NM_CLICK:
	{
		if (p->iItem != -1)
		{
			m_Mediator->Send(CMD_ViewTrade_SetContract, (void*)p->iItem, this);
		}
	}
		break;
	case LVN_GETDISPINFO:
		m_listTradeInfo[0].OnGetDispInfo(lParam);
		break;
	case NM_DBLCLK:
	{
		if (p->iItem!=-1)
		{
			int iItemCount = m_listTradeInfo[0].GetItemCount();
			if (p->iItem == iItemCount - 1 && iItemCount!=1)
				m_Mediator->Send(CMD_ViewTrade_DbLClick, 0, this);
			else if (p->iItem>=0 && p->iItem < m_pConTotalPosition->size())
				m_Mediator->Send(CMD_ViewTrade_DbLClickOneLine, (void*)p->iItem, this);
		}
	}
		break;
	}
}
void TTradeInfo::DealMoneyList(LPARAM lParam)
{
    LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
    if (p->hdr.code == NM_RCLICK)
    {
        bool bEnable = true;
        if (p->iItem == -1)
            bEnable = false;
        EnableMenu(p->hdr.idFrom, bEnable);
        ShowMenu(m_menuTradeInfo[ID_ListMoney - ID_ListPosition]);
    }
}
void TTradeInfo::DealOrderList(LPARAM lParam)
{
    LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
    if (p->hdr.code == NM_RCLICK)
    {
		memset(&m_stMenuInfo, 0, sizeof(TMenuInfo));
        bool bEnable = false;
		if (p->iItem != -1)
		{
			bEnable = true;
			m_stMenuInfo.Index = p->iItem;
			if (m_stMenuInfo.Index < m_pConParOrder->size())
				m_stMenuInfo.pData = m_pConParOrder->at(m_stMenuInfo.Index);
		}
        EnableMenu(p->hdr.idFrom, bEnable);
        ShowMenu(m_menuTradeInfo[ID_ListOrder - ID_ListPosition]);

    }
    if (p->hdr.code == LVN_GETDISPINFO)
    {
        m_listTradeInfo[ID_ListOrder-ID_ListPosition].OnGetDispInfo(lParam);
        
    }
}
void TTradeInfo::DealMatchList(LPARAM lParam)
{
    LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
    if (p->hdr.code == NM_RCLICK)
    {
		memset(&m_stMenuInfo, 0, sizeof(TMenuInfo));
		bool bEnable = true;
        if (p->iItem == -1)
            bEnable = false;
        EnableMenu(p->hdr.idFrom, bEnable);
        ShowMenu(m_menuTradeInfo[ID_ListMatch - ID_ListPosition]);

    }
    if (p->hdr.code == LVN_GETDISPINFO)
    {
        m_listTradeInfo[ID_ListMatch - ID_ListPosition].OnGetDispInfo(lParam);

    }
}

void TTradeInfo::DealConditionOrder(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		memset(&m_stMenuInfo, 0, sizeof(TMenuInfo));
		bool bEnable = true;
		if (p->iItem == -1)
			bEnable = false;
		if (p->iItem != -1)
		{
			m_stMenuInfo.Index = p->iItem;
			if (m_bShowTrig&&m_stMenuInfo.Index < m_pVecTrigConOrder->size())
				m_stMenuInfo.pData = m_pVecTrigConOrder->at(m_stMenuInfo.Index);
			else if(!m_bShowTrig&&m_stMenuInfo.Index<m_pVecConOrder->size())
				m_stMenuInfo.pData = m_pVecConOrder->at(m_stMenuInfo.Index);
		}
		//EnableMenu(p->hdr.idFrom, bEnable);
		EnableConMenu(p->iItem);
		ShowMenu(m_menuTradeInfo[ID_ListConOrder - ID_ListPosition]);
	}
	else if (p->hdr.code == LVN_GETDISPINFO)
	{
		m_listTradeInfo[ID_ListConOrder - ID_ListPosition].OnGetDispInfo(lParam);
	}
	else if (p->hdr.code == NM_CLICK)
	{
		if (p->iItem == -1)
			m_listTradeInfo[ID_ListConOrder - ID_ListPosition].SelectItem(-1);
	}
}

void TTradeInfo::DealStoplossOrder(LPARAM lParam)
{
    LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
    if (p->hdr.code == NM_RCLICK)
    {
		memset(&m_stMenuInfo, 0, sizeof(TMenuInfo));
		if (p->iItem != -1)
		{
			m_stMenuInfo.Index = p->iItem;
			if (m_stMenuInfo.Index < m_pConLPOrder->size())
				m_stMenuInfo.pData = m_pConLPOrder->at(m_stMenuInfo.Index);
			EnableMenu(p->hdr.idFrom, true);
			if (((const TStopLPitem*)m_stMenuInfo.pData)->state == osSuspend)
			{
				EnableMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_STOPSTART, MF_ENABLED);
				EnableMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_STOPPAUSE, MF_GRAYED);
			}
			else
			{
				EnableMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_STOPPAUSE, MF_ENABLED);
				EnableMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_STOPSTART, MF_GRAYED);
			}
		}
		else
		{
			EnableMenu(p->hdr.idFrom, false);
		}
		UINT uMenuState = GetMenuState(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], IDM_TRIG, MF_BYCOMMAND);
		if (uMenuState&MF_CHECKED)//如果是已触发列表
		{
			for (UINT id = IDM_STOPCHANGE; id < IDM_STOPDELETEALL; id++)
			{
				EnableMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], id, MF_GRAYED);
			}
		}
		ShowMenu(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition]);
    }
	if (p->hdr.code == LVN_GETDISPINFO)
	{
		m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].OnGetDispInfo(lParam);

	}
}

void TTradeInfo::DealOrderState(LPARAM lParam)
{
    LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
    {
		memset(&m_stMenuInfo, 0, sizeof(TMenuInfo));
        bool bEnable = false;
		if (p->iItem != -1)
		{
			bEnable = true;
			m_stMenuInfo.Index = p->iItem;
			if (m_stMenuInfo.Index < m_pConParOrder->size())
				m_stMenuInfo.pData = m_pConParOrder->at(m_stMenuInfo.Index);
		}
        EnableMenu(p->hdr.idFrom, bEnable);
        ShowMenu(m_menuOrderState);
    }
	else if (p->hdr.code == LVN_GETDISPINFO)
	{
		m_listOrderState.OnGetDispInfo(lParam);
	}
	else if (p->hdr.code == NM_CLICK)
	{
		if (p->iItem != -1)
		{
			m_Mediator->Send(CMD_ViewTrade_ClickOrderState, (void*)p->iItem, this);
		}
	}
	else if (p->hdr.code == NM_DBLCLK&&p->iItem!=-1)
	{
		if (g_pGlobalModel->m_stOptionConfig.bDoublePositionConfirm)
		{
			if (_TMessageBox_Domodal(m_Hwnd, _GetTextTI(Warning_Index), _GetTextTIW(CancelOrder_Index), TMB_OKCANCEL))
				g_pTradeApi->CancelOrder(((const TOrder*)m_pConParOrder->at(p->iItem))->OrderID);
		}
		else
			g_pTradeApi->CancelOrder(((const TOrder*)m_pConParOrder->at(p->iItem))->OrderID);
	}

}
void TTradeInfo::InitListView()
{
	//OrderState
	m_listOrderState.AddColumn(_GetTextTIW(OrderTime_Index), 160);
	m_listOrderState.AddColumn(_GetTextTIW(Contract_Index), 80);
	m_listOrderState.AddColumn(_GetTextTIW(OrderStatus_Index), 80);
	m_listOrderState.AddColumn(_GetTextTIW(BuySell_Index), 40);
	m_listOrderState.AddColumn(_GetTextTIW(OpenClose_Index), 40);
	m_listOrderState.AddColumn(_GetTextTIW(OrderPrice_Index), 60);
	m_listOrderState.AddColumn(_GetTextTIW(OrderQty_Index), 60);
	m_listOrderState.AddColumn(_GetTextTIW(CanCancel_Index), 40);
	m_listOrderState.AddColumn(_GetTextTIW(MatchQtyAlready_Index), 60);
	m_listOrderState.AddColumn(_GetTextTIW(SpecHedg_Index), 40);
	//Position
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(Product_Index), 50);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(Contract_Index), 80);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(BuySell_Index), 40);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(Qty_Index), 40);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(AvaQty_Index), 40);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(CurrentFloatingPnL_Index), 80);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(FloatingPnL_Index), 40);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(InitialMargin_Index), 80);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(AvgPrice_Index), 80);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(DingshiPnL_Index), 90);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(ToadyQty_Index), 60);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(ToadyAvaQty_Index), 60);
	m_listTradeInfo[ID_ListPosition - ID_ListPosition].AddColumn(_GetTextTIW(SpecHedg_Index), 40);
	//Order
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderTime_Index), 160);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(Contract_Index), 60);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderStatus_Index), 60);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(BuySell_Index), 40);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(OpenClose_Index), 40);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderPrice_Index), 60);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderQty_Index), 60);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(CanCancel_Index), 60);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(CanMatch_Index), 60);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(Cancelled_Index), 60);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(SpecHedg_Index), 40);
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(AccountNo_Index), 150);//todo
	m_listTradeInfo[ID_ListOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderNo_Index), 150);//todo
	//Match
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(OrderTime_Index), 160);
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(Contract_Index), 80);
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(BuySell_Index), 40);
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(OpenClose_Index), 40);
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(MatchPrice_Index), 80);
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(MatchProfit_Index), 80); //平仓盈亏
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(MatchQty_Index), 60);
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(ExchangeMatchNo_INdex), 160);//todo
	m_listTradeInfo[ID_ListMatch - ID_ListPosition].AddColumn(_GetTextTIW(MatchNo_Index), 160);//todo

	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddColumn(L"",  80);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddColumn(L"",  120);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddColumn(L"",  80);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddColumn(L"",  120);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddColumn(L"",  80);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddColumn(L"",  120);


	//条件单
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderTime_Index), 140);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(Contract_Index), 60);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderStatus_Index), 60);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(ConType_Index), 70);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(Condition_Index), 120);
//	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(L"时间条件", 100);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderPrice_Index), 60);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderQty_Index), 60);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(BuySell_Index), 60);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(OpenClose_Index), 60);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(StopMethod_Index), 90);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(StopLossW_Index), 70);
	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].AddColumn(_GetTextTIW(StopProfitW_Index), 70);


	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(Contract_Index), 80);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(ConType_Index), 80);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(BuySell_Index), 60);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(OpenClose_Index), 40);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(TrigPrice_Index), 80);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(TrigCount_Index), 70);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(TrigCond_Index), 70);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderStatus_Index), 75);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(Qty_Index), 60);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(OrderPrice_Index), 80);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(BreakEvenPoint_Index), 80);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddColumn(_GetTextTIW(FloatingPoint_Index), 80);

	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode( _GetTextTI(UpdateTime_Index)).c_str(), 0, 0);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(CurrencyNo_Index)).c_str(), 0, 2);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(CoverProfitTBT_Index)).c_str(), 0, 4);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(MoneyAdjust_Index)).c_str(), 1, 0);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(CashOut_Index)).c_str(), 1, 2);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(CashIn_Index)).c_str(), 1, 4);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(Equity_Index)).c_str(), 2, 0);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(Available_Index)).c_str(), 2, 2);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(CanCashOut_Index)).c_str(), 2, 4);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(Deposit_Index)).c_str(), 3, 0);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(KeepDeposit_Index)).c_str(), 3, 2);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(Premium)).c_str(), 3, 4);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(NetPrfoit_Index)).c_str(), 4, 0);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(ProfitRate_Index)).c_str(), 4, 2);
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(LoadRC::ansi_to_unicode(_GetTextTI(RiskRate_Index)).c_str(), 4, 4);


}
void TTradeInfo::EnableMenu(UINT which, bool bEnale)
{
    UINT enable = MF_ENABLED;
    if (bEnale == false)
        enable = MF_GRAYED;
    switch (which)
    {
    case ID_ListPosition:
        for (UINT i = IDM_RIVALCOLSE; i <= IDM_MARKETBACKHAND; i++)
            EnableMenuItem(m_menuTradeInfo[ID_ListPosition - ID_ListPosition], i, enable);
		if (m_stMenuInfo.pData)
		{
			const TPosition* pPosition = (const TPosition*)m_stMenuInfo.pData;
			if (pPosition->IsCmb == bYes)
				EnableMenuItem(m_menuTradeInfo[0], IDM_TIMINGCLOSE, MF_ENABLED);
			else
				EnableMenuItem(m_menuTradeInfo[0], IDM_TIMINGCLOSE, MF_GRAYED);
			if (g_pGlobalModel->m_utUserType == utInner)
			{
				EnableMenuItem(m_menuTradeInfo[0], IDM_LOCKPOSITION, MF_ENABLED);
				EnableMenuItem(m_menuTradeInfo[0], IDM_CLOSESH, MF_ENABLED);
			}
			else
			{
				EnableMenuItem(m_menuTradeInfo[0], IDM_LOCKPOSITION, MF_GRAYED);
				EnableMenuItem(m_menuTradeInfo[0], IDM_CLOSESH, MF_GRAYED);
			}
		}
        break;
    case ID_ListOrderState:
        for (UINT i = IDM_CANCEL; i <= IDM_MARKETFOLLOW; i++)
            EnableMenuItem(m_menuOrderState, i, enable);
        break;
    case ID_ListStopLossOrder:
        for (UINT i = IDM_STOPCANCEL; i <= IDM_STOPSTART; i++)
            EnableMenuItem(m_menuTradeInfo[ID_ListStopLossOrder - ID_ListPosition], i, enable);
		break;
    case ID_ListOrder:
        EnableMenuItem(m_menuTradeInfo[ID_ListOrder - ID_ListPosition], IDM_ORDER_SAVE, enable);
        EnableMenuItem(m_menuTradeInfo[ID_ListOrder - ID_ListPosition], IDM_ORDER_FRESH, enable);
        break;
    case ID_ListMatch:
        EnableMenuItem(m_menuTradeInfo[ID_ListMatch - ID_ListPosition], IDM_MATCH_SVAE, enable);
        EnableMenuItem(m_menuTradeInfo[ID_ListMatch - ID_ListPosition], IDM_MATCH_FRESH, enable);
        break;
    }
}
void TTradeInfo::ShowMenu(HMENU menu)
{
    POINT pt;
    GetCursorPos(&pt);
    TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}


void TTradeInfo::UpdateMoney(const TMoney* pMoney, bool bUpdate)
{
	if (m_iTabSelect == tabPos_ParOrder)
	{
		UpdateProfit();
	}
	//
	if (bUpdate)
		m_bNeddUpdateMoney = true;
	if (m_iTabSelect != tabMoney)
	{
		m_bNeddUpdateMoney = true;
		return;
	}
	if (!m_bNeddUpdateMoney) return;
	int iRow = 0; int iCol = 1;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->UpdateTime, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->CurrencyNo, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->CoverProfitTBT, iRow, iCol);

	iRow++; iCol = 1;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->Adjust, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->CashIn, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->CashOut, iRow, iCol);

	iRow++; iCol = 1;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->Equity, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->Available, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->CanCashOut, iRow, iCol);

	iRow++; iCol = 1;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->Deposit, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->KeepDeposit, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->Premium, iRow, iCol);

	iRow++; iCol = 1;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->NetProfit, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->ProfitRate, iRow, iCol);
	iCol += 2;
	m_listTradeInfo[ID_ListMoney - ID_ListPosition].AddItem(pMoney->RiskRate, iRow, iCol);
}
void TTradeInfo::SetNeedUpdate()
{
	m_bNeedUpdateOrder = true;
	m_bNeedUpdateMatch = true;
	m_bNeedUpdatePosition = true;
	m_bNeedUpdateParOrder = true;
	m_bNeddUpdateMoney = true;
}
void TTradeInfo::UpdateStopLP(const TStopLPitem* pitem)
{
	if (pitem == nullptr)
	{
		m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].DeleteAllItems();
		uint count = m_pConLPOrder->size();
		for (int i = 0; i < count; i++)
		{
			UpdateStopLPOneItem(i, -1);
		}
		m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].SetItemCount(count);
	}
	else
	{
		uint count = m_pConLPOrder->size();
		for (int i = 0; i < count; i++)
		{
			TStopLPitem* pAtitem = (TStopLPitem*)m_pConLPOrder->at(i);
			if (pAtitem->keyID == pitem->keyID)
			{
				ModStopLPOneItem(i,pitem);
			}
		}
		m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].SetItemCount(count);
	}

}

void TTradeInfo::UpdateStopLPOneItem(int atIndex, int SelKeyID)
{
	TStopLPitem*pitem = (TStopLPitem*)m_pConLPOrder->at(atIndex);
	int col = 0;
	std::vector<string> code;
	code = CConvert::SplitString(pitem->code, ' ');
	TContractKey contractKey;
	memset(&contractKey, 0, sizeof(TContractKey));
	strcpy_s(contractKey.Sign, g_pGlobalModel->m_strSign.c_str());

	char Temp[50] = { 0 };
	if (CConvert::TContractNoToTContractKey(pitem->code, contractKey))
	{
		switch (contractKey.CommodityType)
		{
		case ctFutures:
			sprintf_s(Temp, "%s%s", contractKey.CommodityNo, contractKey.ContractNo);
			break;
		case ctOption:
			sprintf_s(Temp, "%s%s%c%s", contractKey.CommodityNo, contractKey.ContractNo,contractKey.OptionType,contractKey.StrikePrice);
			break;
		default:
			sprintf_s(Temp, "%s%s", contractKey.CommodityNo, contractKey.ContractNo);
			break;
		}
	}
	else
	{
		sprintf_s(Temp, "%s%s", code.at(1).c_str(), code.at(2).c_str());
	}

	
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, atIndex, col++);
	TStopOrderType2Char(pitem->orderType, Temp, sizeof(Temp));
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, atIndex, col++);
	TDirect2Char(pitem->direct, Temp, sizeof(Temp));
	pitem->direct == dBuy ? m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, atIndex, col++, DT_LEFT, g_ColorRefData.g_ColorTextRed)
		: m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, atIndex, col++, DT_RIGHT, g_ColorRefData.g_ColorTextGreen);
	
	_GetTextTI(Close_Index);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(_GetTextTI(Close_Index) , atIndex, col++);
	
	//价格精度显示
	const SCommodity * pCommodity = g_pTradeData->GetCommodity(&contractKey);
	if (pCommodity)
	{
		char szFormat[20] = {};
		sprintf_s(szFormat, "%%.%df", pCommodity->PricePrec);
		sprintf_s(Temp, szFormat, pitem->triggerPrice);
	}
	else
	{
		sprintf_s(Temp, "%.4f", pitem->triggerPrice);
	}
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, atIndex, col++);
	//处理完毕

	sprintf_s(Temp, "%d/%d", pitem->TriggerCount, max(pitem->MaxCount, 1));
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, atIndex, col++);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(_GetTextTI(LastPrice_Index) , atIndex, col++);
	TStopState2Char(pitem->state, Temp, sizeof(Temp));
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, atIndex, col++);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem((int)pitem->Qty, atIndex, col++);
	TStopOrderPrice2Char(pitem->orderPriceType, Temp, sizeof(Temp));
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, atIndex, col++);

	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(pitem->breakEvenPoint, atIndex, col++);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(pitem->floatPoint, atIndex, col++);
	if (pitem->keyID == SelKeyID)
	{
	//	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].SelectItem(atIndex);
	}
}

void TTradeInfo::ModStopLPOneItem(int i, const TStopLPitem* pitem)//只修改其中几行
{
	char Temp[50] = { 0 };
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(pitem->triggerPrice, i, 4);
	sprintf_s(Temp, "%d/%d", pitem->TriggerCount, max(pitem->MaxCount, 1));
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, i, 5);
	TStopState2Char(pitem->state, Temp, sizeof(Temp));
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, i, 7);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem((int)pitem->Qty, i, 8);
	TStopOrderType2Char(pitem->orderType, Temp, sizeof(Temp));
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(Temp, i, 1);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(pitem->breakEvenPoint, i, 10);
	m_listTradeInfo[ID_ListStopLossOrder - ID_ListPosition].AddItem(pitem->floatPoint, i, 11);
}

bool TTradeInfo::UpdateOneConOrder(TConditionOrder * pOrder, bool bSetItemCount, int iRow)
{
	if (pOrder->Triggered != m_bShowTrig) return false;

	int index = ID_ListConOrder -ID_ListPosition;

	if (iRow==-1)
		iRow = m_listTradeInfo[index].GetItemCount();
	char szText[260] = { 0 };
	char szText2[260] = { 0 };
	int col = 0;

	TContractKey tradeContractKey = {};
	CConvert::SContractNoTypeToTradeContract("", pOrder->pContract->ContractNo, tradeContractKey);
	switch (tradeContractKey.CommodityType)
	{
	case ctFutures:
		sprintf_s(szText, "%s%s", tradeContractKey.CommodityNo, tradeContractKey.ContractNo);
		break;
	case ctOption:
		sprintf_s(szText, "%s%s%c%s", tradeContractKey.CommodityNo, tradeContractKey.ContractNo, tradeContractKey.OptionType, tradeContractKey.StrikePrice);
		break;
	default:
		sprintf_s(szText, "%s%s", tradeContractKey.CommodityNo, tradeContractKey.ContractNo);
		break;
	}
	m_listTradeInfo[index].AddItem(pOrder->InsertTime, iRow, col++);//时间
	m_listTradeInfo[index].AddItem(szText, iRow, col++);//合约
	TConOrderState2Char(pOrder->RunState, szText,sizeof(szText));
	m_listTradeInfo[index].AddItem(szText, iRow, col++);//运行状态
	TConOrderType2Char(pOrder->OrderType, szText, sizeof(szText));
	m_listTradeInfo[index].AddItem(szText, iRow, col++);//类型

	char szFormat[10] = { 0 };
	sprintf_s(szFormat, "%%.%df", pOrder->Precision);
	if (pOrder->OrderType == cotPrice || pOrder->OrderType==cotPriceServer)
	{
		switch (pOrder->TriggerCondition)
		{
		case tcGreaterEqual:
			strcpy_s(szText, ">= ");
			break;
		case tcLessEqual:
			strcpy_s(szText, "<= ");
			break;
		default:
			strcpy_s(szText, "");
			break;
		}
		sprintf_s(szText2, szFormat, pOrder->ConPrice);
		strcat_s(szText, szText2);
	}
	else if (pOrder->OrderType == cotTime)
		sprintf_s(szText, "%s %s", _GetTextTI(TimeTo_Index) ,pOrder->TimeOut);
	else if (pOrder->OrderType == cotAutoOrder)
		strcpy_s(szText, _GetTextTI(TigeOpen_Index));
	else if (pOrder->OrderType == cotPreOrder)
		strcpy_s(szText, _GetTextTI(Send_Index));
	else
		strcpy_s(szText, "");
	m_listTradeInfo[index].AddItem(szText, iRow, col++);//条件

	TStopOrderPrice2Char(pOrder->PriceType, szText, sizeof(szText));
	if (pOrder->PriceType == ptSpecifiedPrice)
	{
		strcpy_s(szText, _GetTextTI(SpecificPrice_Index));
		sprintf_s(szText2, szFormat, pOrder->Price);
		strcat_s(szText, szText2);
	}
	m_listTradeInfo[index].AddItem(szText, iRow, col++);//委托价格

	m_listTradeInfo[index].AddItem((int)pOrder->iQty, iRow, col++);//手数
	TDirect2Char(pOrder->Direct, szText);
	m_listTradeInfo[index].AddItem(szText, iRow, col++);//买卖方向
	TOffset2Char(pOrder->Offset, szText);
	m_listTradeInfo[index].AddItem(szText, iRow, col++);//开平方向

	GetStopMethodText(pOrder->StopType, szText, sizeof(szText));
	m_listTradeInfo[index].AddItem(szText, iRow, col++);  //止损方式

	GetStopLossProfitText(pOrder, szText, szText2, ARRAYSIZE(szText2));
	m_listTradeInfo[index].AddItem(szText, iRow, col++);  //止损
	m_listTradeInfo[index].AddItem(szText2, iRow, col++);  //止盈


	if (bSetItemCount)
		m_listTradeInfo[index].SetItemCount(iRow + 1);
	return true;
}

void TTradeInfo::GetStopLossProfitText(TConditionOrder*pOrder, char* pLossText, char* pProfitText, int length)
{
	strcpy_s(pLossText, length, "");
	strcpy_s(pProfitText, length, "");
	switch (pOrder->StopType)
	{
	case TConditionOrderFrm::stopLoss:
		sprintf_s(pLossText, length, "%d%s", pOrder->LossPoint,_GetTextTI(Point_Index));
		break;
	case TConditionOrderFrm::stopProfit:
		sprintf_s(pProfitText, length, "%d%s", pOrder->ProfitPoint, _GetTextTI(Point_Index));
		break;
	case TConditionOrderFrm::stopLossProfit:
		sprintf_s(pLossText, length, "%d%s", pOrder->LossPoint, _GetTextTI(Point_Index));
		sprintf_s(pProfitText, length, "%d%s", pOrder->ProfitPoint, _GetTextTI(Point_Index));
		break;
	case TConditionOrderFrm::stopFloatChase:
		sprintf_s(pLossText, length, "%d%s", pOrder->DrawBackPoint, _GetTextTI(Point_Index));
		break;
	case TConditionOrderFrm::stopSpeciLossProfit:
	{
		char format[10] = { 0 };
		sprintf_s(format, "%%.%df", pOrder->Precision);
		sprintf_s(pLossText, length, format, pOrder->LossPrice);
		sprintf_s(pProfitText, length, format, pOrder->ProfitPrice);
		break;
	}
	}
}

void TTradeInfo::GetStopMethodText(int iMethod, char*pText, int length)
{
	switch (iMethod)
	{
	case TConditionOrderFrm::stopNone:
		strcpy_s(pText, length,  _GetTextTI(None_Index));
		break;
	case TConditionOrderFrm::stopLoss:
		strcpy_s(pText, length, _GetTextTI(LimitStopLoss_Index));
		break;
	case TConditionOrderFrm::stopProfit:
		strcpy_s(pText, length, _GetTextTI(LimitStopProfit_Index));
		break;
	case TConditionOrderFrm::stopLossProfit:
		strcpy_s(pText, length, _GetTextTI(LimitStop_Index));
		break;
	case TConditionOrderFrm::stopFloatChase:
		strcpy_s(pText, length, _GetTextTI(Floating_Index));
		break;
	case TConditionOrderFrm::stopSpeciLossProfit:
		strcpy_s(pText, length, _GetTextTI(SpecificPriceStop_Index));
		break;
	}
}
void TTradeInfo::UpdateConOrder()
{
	vector<TConditionOrder*>* pVecData = m_pVecConOrder;
	if (m_bShowTrig)
		pVecData = m_pVecTrigConOrder;

	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].DeleteAllItems();

	int iCount = 0;
	for (uint i = 0; i < pVecData->size(); i++)
	{
		if (g_pGlobalModel->m_strUserNo == (pVecData->at(i))->UserNo)//只刷当前用户
		{
			if(UpdateOneConOrder(pVecData->at(i), false, iCount))
				iCount++;
		}
	}

	m_listTradeInfo[ID_ListConOrder - ID_ListPosition].SetItemCount(iCount);
}


void TTradeInfo::SetSelectedPosition(const TPosition * pPosition)
{
	m_pSelectedPosition = pPosition;
	for (int i=0; i<m_pConTotalPosition->size(); i++)
	{
		const TPosition * pTemp = (const TPosition *)m_pConTotalPosition->at(i);
		if (pTemp == pPosition)
		{
			m_listTradeInfo[ID_ListPosition - ID_ListPosition].SelectItem(i);
			break;
		}
	}
}