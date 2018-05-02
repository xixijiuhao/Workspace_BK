#include "BaseInclude.h"

#define timerID  101
#define maxPriceJump  3 

#define bigOrderQty 1000

TradeAction::TradeAction()
{
	m_pstChaseConfig = &g_pGlobalModel->m_stChaseConfig;
	CreateFrm(L"TradeAction", 0, 0);
	m_pConPosition = nullptr;
}
TradeAction::~TradeAction()
{
	if (g_pStarApi)
		g_pStarApi->UnsubAllQuote(this);
}

void TradeAction::InsertChaseOrder(const TOrder* pOrder)
{
	auto iter = m_mapChaseOrder.find(pOrder->OrderID);
	if (iter == m_mapChaseOrder.end())
	{
		TChaseOrder stTemp;
		memset(&stTemp, 0, sizeof(TChaseOrder));
		stTemp.iChaseCount = 0;
		stTemp.iTime = -1;
		stTemp.bCancel = true;
		stTemp.pOrder = pOrder;
		stTemp.dOrderPrice = pOrder->OrderPrice;
		m_mapChaseOrder.insert(make_pair(pOrder->OrderID, stTemp));
	}
	else
	{
		iter->second.iChaseCount = 0;
		iter->second.pOrder = pOrder;
		iter->second.dOrderPrice = pOrder->OrderPrice;
		iter->second.bCancel = true;
	}
	g_pTradeApi->CancelOrder(pOrder->OrderID);
}
LRESULT TradeAction::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_TIMER:
		OnTimer();
		return PROCESSED;
	case SSWM_QUOTE:
		OnBackOrderQuote(((const SContract*)wParam)->ContractNo);
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
	return NOT_PROCESSED;
}

void TradeAction::OnTimer()
{	
	for (auto iter = m_mapChaseOrder.begin(); iter != m_mapChaseOrder.end(); iter++)
	{
		if (iter->second.iTime == 0)
		{
			iter->second.bCancel = true;
			g_pTradeApi->CancelOrder(iter->first);
		}
		else
		{
			iter->second.iTime--;
		}
	}
}

void TradeAction::OnCreate()
{
	SetTimer(m_Hwnd, timerID, 1000, NULL);
}

void TradeAction::OnOrder(const TOrder * pOrder)
{
	switch (pOrder->OrderState)
	{
	case osCanceled:            //已撤单
	case osPartCanceled:		//已撤余单
		OnCancelChaseOrder(pOrder);  //处理追单
		m_mapShiftOrder.erase(pOrder->OrderID);
		OnCancelCoverOrder(pOrder); //处理清仓
		break;
	case osFilled:				//完全成交
		m_mapChaseOrder.erase(pOrder->OrderID);
		OnFillBackOrder(pOrder);
		OnFillShiftOrder(pOrder);
		break;
	case osFail:				//失败
	case osPartFailed:			//部分失败
		m_mapChaseOrder.erase(pOrder->OrderID);
		m_mapShiftOrder.erase(pOrder->OrderID);
		break;
	case osPartFilled:
		OnFillBackOrder(pOrder);
		break;
	default:
		break;
	}
}

void TradeAction::OnFillBackOrder(const TOrder *pOrder)
{
	if (pOrder->OrderWay != owBackHand) return;
	auto iter = m_mapBackOrder.find(pOrder->Remark);
	if (iter == m_mapBackOrder.end()) return;
	TSendOrder stOrder;
	memcpy(&stOrder, pOrder, sizeof(TSendOrder));

	stOrder.OrderQty = pOrder->MatchQty - iter->second.MatchQty;
	iter->second.MatchQty = pOrder->MatchQty; //保存上次成交量，以应对部分成交的情况

	CConvert::GetPrice(&stOrder, stOrder.Direct, iter->second.PriceType, stOrder.OrderPrice);

	stOrder.Offset = oOpen;

	if (stOrder.OrderQty != 0) 
		g_pTradeApi->InsertOrder(stOrder);
	
	if (pOrder->OrderState!=osPartFilled)
		iter->second.Count--;
	if (iter->second.Count == 0||pOrder->OrderState==osFilled)
	{
		m_mapBackOrder.erase(pOrder->Remark);
	}
}
void TradeAction::OnCancelChaseOrder(const TOrder* pOrder)
{
	auto iter = m_mapChaseOrder.find(pOrder->OrderID);
	if (iter != m_mapChaseOrder.end())
	{
		if (!iter->second.bCancel)   //不是追单模块撤的单，那就不再追
		{
			m_mapChaseOrder.erase(iter);
			return;
		}
		TSendOrder stOrder;
		memcpy(&stOrder, pOrder, sizeof(TSendOrder));

		stOrder.OrderType = otLimit;
		stOrder.StrategyType = stNone;
		stOrder.TriggerMode = tmNone;
		stOrder.TriggerCondition = tcNone;

		iter->second.iChaseCount++;
		iter->second.iTime = m_pstChaseConfig->iSecond;
		stOrder.OrderPrice = GetPrice(pOrder, &(iter->second));
		iter->second.bCancel = false;
		uint iOrderID = g_pTradeApi->InsertOrder(stOrder);
		stOrder.OrderQty = pOrder->PreWorkingQty;

		if (m_pstChaseConfig->iChaseOver&&GetPriceOffset(iter->second.pOrder,stOrder.OrderPrice) >= m_pstChaseConfig->iChaseOver)//追价次数达到
		{
			m_mapChaseOrder.erase(iter);
			return;
		}
		m_mapChaseOrder.insert(make_pair(iOrderID, iter->second));
		m_mapChaseOrder.erase(iter);
	}
}

double TradeAction::GetPrice(const TOrder* pOrder, TChaseOrder *pChase)
{
	double dPrice = pChase->dOrderPrice;
	//CConvert::GetPrice(const_cast<TOrder*>(pOrder), pOrder->Direct, ptCounterPrice, dPrice);

	double dMarketPrice = 0.0;
	CConvert::GetPrice(const_cast<TOrder*>(pOrder), pOrder->Direct, ptMarketPrice, dMarketPrice);

	if (m_pstChaseConfig->iChasePriceCom == ChaseLadderPrice)
	{
		double MinPrice = CConvert::GetMinChangePrice(pOrder);
		uint iChaseCount = pChase->iChaseCount < 3 ? pChase->iChaseCount : 3;

		if (pOrder->Direct == dBuy)
			dPrice += iChaseCount * MinPrice;
		else if (pOrder->Direct == dSell)
			dPrice -= iChaseCount * MinPrice;
		if ((dPrice > dMarketPrice && pOrder->Direct == dBuy) || (dPrice < dMarketPrice && pOrder->Direct == dSell))
			dPrice = dMarketPrice;
	}
	else if (m_pstChaseConfig->iChasePriceCom == ChaseMarketPrice)
	{
		dPrice = dMarketPrice;
	}
	return dPrice;
}

uint TradeAction::GetPriceOffset(const TOrder* pOrder, double dOrderPrice)
{
	double dMinChangePrice = CConvert::GetMinChangePrice(pOrder);
	if (dMinChangePrice!=0)
		return abs((dOrderPrice - pOrder->OrderPrice) / dMinChangePrice);
	return 0;
}


int TradeAction::CheckOrder(TSendOrder & stOrder)
{
	if (stOrder.OrderQty == 0)
	{
		return EN_OrderQty_0;
	}
	if (stOrder.OrderPrice == 0)
	{
		return EN_OrderPrice_0;
	}
	//if (stOrder.Hedge != hSpeculate && stOrder.Hedge != hHedge)
	//{
	//	return EN_No_Hedge;
	//}
	return 0;
}

int TradeAction::InsertCloseOrder(TSendOrder & stOrder, vector<uint>*pOrderID)
{
	int iRet = CheckOrder(stOrder);
	if (iRet != 0)
		return iRet;

	InsertCloseOrderNoCheck(stOrder, pOrderID);

	return 0;
}
void TradeAction::InsertOrder(TSendOrder & stOrder, vector<uint>*pOrderID)
{
	int iOrderID = g_pTradeApi->InsertOrder(stOrder);
	if (pOrderID)
		pOrderID->push_back(iOrderID);
}
void TradeAction::BigOrderForClose(TSendOrder& stOrder, vector<uint>*pOrderID)
{
	int iOrderQty = stOrder.OrderQty;
	while (iOrderQty > bigOrderQty)
	{
		stOrder.OrderQty = bigOrderQty;
		int iOrderID = g_pTradeApi->InsertOrder(stOrder);
		if (pOrderID)
		{
			pOrderID->push_back(iOrderID);
		}
		iOrderQty -= bigOrderQty;
	}
	stOrder.OrderQty = iOrderQty;
	int iOrderID = g_pTradeApi->InsertOrder(stOrder);
	if (pOrderID)
	{
		pOrderID->push_back(iOrderID);
	}
}

void TradeAction::InsertCloseOrderNoCheck(TSendOrder & stOrder, vector<uint>*pOrderID)
{
	int iCover = 0, iTCover = 0;
	CConvert::GetCloseNum(stOrder.UserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCover, iTCover);

	//如果传进来的vector指针为空，则创建一个vector存放OrderID
	vector<uint> vecTempOrderID;
	if (pOrderID == nullptr)
		pOrderID = &vecTempOrderID;
	//处理完毕
	if (iCover == 0 && iTCover == 0)
	{
		g_Presenter->ShowMsgBox(EN_NoAva_Position);
		return;
	}
	if (stOrder.OrderQty > (iCover + iTCover))
	{
		char szText[100] = { 0 };
		if (g_language==CHT)
			sprintf_s(szText, "%s%s平倉量大於實際持倉量，是否按照實際持倉量%d手平倉?", stOrder.CommodityNo, stOrder.ContractNo, iCover+iTCover);
		else if (g_language==ENU)
			sprintf_s(szText, "Close %s%s qty is more than position qty, close use position qty?", stOrder.CommodityNo, stOrder.ContractNo);
		else
			sprintf_s(szText, "%s%s平仓量大于实际持仓量，是否按照实际持仓量%d手平仓？", stOrder.CommodityNo, stOrder.ContractNo, iCover + iTCover);
		if (!g_Presenter->ShowMsgBox(EN_CoverQty_TooMore,szText))
			return ;
	}
	if (iTCover == 0 && iCover != 0)
	{
		stOrder.Offset = oCover;
		stOrder.OrderQty = iCover;
		if (g_pGlobalModel->m_stOptionConfig.bBigOrderSplit)
			BigOrderForClose(stOrder, pOrderID);
		else
			InsertOrder(stOrder, pOrderID);
	}
	else
	{
		stOrder.Offset = oCoverT;
		stOrder.OrderQty = iTCover;
		if (g_pGlobalModel->m_stOptionConfig.bBigOrderSplit)
			BigOrderForClose(stOrder, pOrderID);
		else
			InsertOrder(stOrder, pOrderID);
		if (iCover != 0)
		{
			stOrder.OrderQty = iCover;
			stOrder.Offset = oCover;
			if (g_pGlobalModel->m_stOptionConfig.bBigOrderSplit)
				BigOrderForClose(stOrder, pOrderID);
			else
				InsertOrder(stOrder, pOrderID);
		}
	}

	//所有的平仓都经过这里，在这里保存外盘的平仓单量，其实外盘没有平仓这一说
	//g_pGlobalModel->ForeignCoverOrder(stOrder, pOrderID);
}

void TradeAction::InsertBackOrder(const TPosition* pPosition, int priceType)
{
	TSendOrder stOrder;
	CConvert::DefaultDealClose(pPosition, stOrder);
	CConvert::GetPrice(const_cast<TPosition*>(pPosition), stOrder.Direct, priceType, stOrder.OrderPrice);

	vector<uint> vecOrderID;
	stOrder.OrderWay = owBackHand;
	CConvert::newGUID(stOrder.Remark);
	TBackOrder stBack = { 0 };
	stBack.MatchQty = 0;
	stBack.PriceType = priceType;

	InsertCloseOrder(stOrder, &vecOrderID);
	stBack.Count = vecOrderID.size(); //记录发了几笔平仓单
	m_mapBackOrder.insert(make_pair(stOrder.Remark, stBack));
}

void TradeAction::InsertShiftOrder(TSendOrder &stOrder, const char * pszContractNo)
{
	vector<uint>vecOrderID;
	InsertCloseOrder(stOrder, &vecOrderID);

	for (uint i = 0; i < vecOrderID.size(); i++)
	{
		m_mapShiftOrder.insert(make_pair(vecOrderID[i], pszContractNo));
	}
}
void TradeAction::OnFillShiftOrder(const TOrder*pOrder)
{
	auto iter = m_mapShiftOrder.find(pOrder->OrderID);
	if (iter != m_mapShiftOrder.end())
	{
		TSendOrder stOrder;
		memcpy(&stOrder, pOrder, sizeof(TSendOrder));
		stOrder.Offset = oOpen;
		if (stOrder.Direct == dBuy)
			stOrder.Direct = dSell;
		else
			stOrder.Direct = dBuy;
		strcpy_s(stOrder.ContractNo, iter->second.c_str());
		
		stOrder.OrderPrice = 0;
		CConvert::GetPrice(&stOrder, stOrder.Direct, ptCounterPrice, stOrder.OrderPrice);
		if (stOrder.OrderPrice != 0)
		{
			if (g_pTradeApi)
				g_pTradeApi->InsertOrder(stOrder);
		}
		else
		{
			SubQuoteAndInsert(stOrder); //订阅了行情，要退订的，在哪退呢
			
			//订阅行情进行处理
		}
		m_mapShiftOrder.erase(iter);
	}
}

void TradeAction::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
	//g_pQuoteApi->UnsubQuote(quote->ContractId,this);
	PostMessage(m_Hwnd, SSWM_QUOTE, WPARAM(cont), LPARAM(quote));
}

void TradeAction::SubQuoteAndInsert(TSendOrder &stOrder)
{
	SContractNoType QuoteData;
	memset(&QuoteData, 0, sizeof(QuoteData));
	CConvert::SingleTradeContractToQContractNoType(stOrder, QuoteData);
	m_mapBackOrderQuote.insert(make_pair(QuoteData, stOrder));
	g_pStarApi->SubQuote(&QuoteData,1, this);//不一定可行
}

void TradeAction::OnBackOrderQuote(const SContractNoType quoteID)
{
	//if (!quoteID) return;

	auto mapRange = m_mapBackOrderQuote.equal_range(quoteID);

	map<string, TSendOrder>::iterator iter;
	for (iter = mapRange.first; iter != mapRange.second; iter++)
	{
		SQuoteSnapShot QuoteData;
		if (g_pStarApi && g_pStarApi->GetSnapshot(quoteID, &QuoteData))
		{
			if (iter->second.Direct==dBuy)
				iter->second.OrderPrice = QuoteData.Data[S_FID_BESTASKPRICE].Price;
			else
				iter->second.OrderPrice = QuoteData.Data[S_FID_BESTBIDPRICE].Price;
			if (iter->second.OrderPrice && g_pTradeApi)
				g_pTradeApi->InsertOrder(iter->second);
		}
	}
	m_mapBackOrderQuote.erase(quoteID);
	if (g_pStarApi)
		g_pStarApi->UnsubQuote((SContractNoType*)&quoteID,1, this);
}

bool TradeAction::CoverAll(IDataContainer*pConParOrder, IDataContainer*pConPosition)
{
	uint iCount = pConParOrder->size();

	for (uint i = 0; i < iCount; i++)
	{
		const TOrder* pOrder = (const TOrder*)pConParOrder->at(i);
		if (pOrder->Offset == oCover || pOrder->Offset == oCoverT)
			m_setCoverAllParOrder.insert(pOrder);
	}
	if (m_setCoverAllParOrder.size() == 0)
	{
		CoverAllPosition(pConPosition);
		return false;
	}

	for (auto iter = m_setCoverAllParOrder.begin(); iter != m_setCoverAllParOrder.end(); iter++)
	{
		g_pTradeApi->CancelOrder((*iter)->OrderID);
	}

	m_pConPosition = pConPosition;

	return true;
}

void TradeAction::CoverAllPosition(IDataContainer*pConPosition)
{
	if (!pConPosition) return;
	TSendOrder stOrder;
	for (uint i = 0; i < pConPosition->size(); i++)
	{
		const TPosition* pPosition = (const TPosition*)pConPosition->at(i);
		CConvert::DefaultDealClose(pPosition, stOrder);
		CConvert::GetPrice(&stOrder, stOrder.Direct, g_pGlobalModel->m_stOptionConfig.iCloseAllFillOrderMod, stOrder.OrderPrice);
		if (stOrder.OrderPrice)
			InsertCloseOrder(stOrder);
		else
			SubQuoteAndInsert(stOrder); //订阅行情，在GetPrice发单
	}
}

void TradeAction::OnCancelCoverOrder(const TOrder* pOrder)
{
	auto iter = m_setCoverAllParOrder.find(pOrder);
	if (iter != m_setCoverAllParOrder.end())
	{
		m_setCoverAllParOrder.erase(pOrder);
		if (!m_setCoverAllParOrder.size())
		{
			CoverAllPosition(m_pConPosition);
			m_pConPosition = nullptr;
		}
	}
}

bool TradeAction::Cover(TSendOrder & stOrder, bool bShowMsg, vector<uint>*pVecOrderID)
{
	int iCover = 0;
	int iCoverT = 0;
	CConvert::GetCloseNum(g_pGlobalModel->m_strUserNo.c_str(), &stOrder, stOrder.Direct, stOrder.OrderQty, iCover, iCoverT);

	if (iCover == 0 && iCoverT == 0)
	{
		if (bShowMsg)
			g_Presenter->ShowMsgBox(EN_NoAva_Position);
			//_TMessageBox_Domodal(g_Presenter->GetTKeyHwnd(), "提示", L"无可用持仓!", TMB_OK);
		return false;
	}
	if (stOrder.OrderQty == 0)
	{
		if (bShowMsg)
			g_Presenter->ShowMsgBox(EN_OrderQty_0);
			//_TMessageBox_Domodal(g_Presenter->GetTKeyHwnd(), "提示", L"下单数量不能为0！", TMB_OK);
		return false;
	}

	if (stOrder.OrderPrice == 0)
	{
		if (bShowMsg)
			g_Presenter->ShowMsgBox(EN_OrderPrice_0);
			//_TMessageBox_Domodal(g_Presenter->GetTKeyHwnd(), "提示", L"价格不能为0！", TMB_OK);
		return false;
	}
	if (CConvert::IsDistinguishCoverType(&stOrder)) //区分平仓、平昨
	{
		if (stOrder.Offset == oCoverT) //平今
		{
			if (iCoverT)
				InsertCoverOrder(stOrder, pVecOrderID);
		}
		else
		{
			if (iCoverT)
			{
				stOrder.Offset = oCoverT;
				stOrder.OrderQty = iCoverT;
				InsertCoverOrder(stOrder, pVecOrderID);
			}
			if (iCover)
			{
				stOrder.OrderQty = iCover;
				stOrder.Offset = oCover;
				InsertCoverOrder(stOrder, pVecOrderID);
			}
		}
	}
	else  //不区分平今平昨
	{
		if (iCover)
		{
			stOrder.OrderQty = stOrder.OrderQty > iCover ? iCover : stOrder.OrderQty;
			stOrder.Offset = oCover;
			InsertCoverOrder(stOrder, pVecOrderID);
		}
	}
	return true;
}

void TradeAction::InsertCoverOrder(TSendOrder & stOrder, vector<uint>*pVecOrderID)
{
	if (g_pGlobalModel->m_stOptionConfig.bBigOrderSplit)
		BigOrderForClose(stOrder, pVecOrderID);
	else
		InsertOrder(stOrder, pVecOrderID);
}