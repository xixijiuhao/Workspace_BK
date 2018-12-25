#include "BaseInclude.h"


char szChsTM[][10] = { "买开", "卖开", "买平", "卖平", "卖平今", "买平今", "手", "买", "卖", "委托", "状态" };
char szChtTM[][10] = { "買開", "賣開", "買平", "賣平", "賣平今", "買平今", "手", "買", "賣", "委托", "狀態" };
char szEnuTM[][20] = { "buy open ","sell open ","buy close ","sell close ","buy close ","sell close "," lots","buy ","sell ","Order","State" };

enum{
	BuyOpen_Index,
	SellOpen_Index,
	BuyClose_Index,
	SellClose_Index,
	SellCloseT_Index,
	BuyCloseT_Indedx,
	Lots_Index,
	Buy_Index,
	Sell_Index,
	Order_Indedx,
	State_Index,
};

char * _GetTextTM(int i)
{
	if (g_language == ENU)return szEnuTM[i];
	else if (g_language == CHT)return szChtTM[i];
	return szChsTM[i];
}
TradeInfoModel::TradeInfoModel()
{
	m_iParOrderID = -1;
	m_pNowSelectPosition = NULL;
	g_pStopLP->Attach(this);
	//SetPosition();
	//SetParOrder();
	//SetOrder();
	//SetMatch();
}
TradeInfoModel::~TradeInfoModel()
{
	g_pStopLP->Detach(this);
}

void TradeInfoModel::SetParOrder()
{
	TDataContainer stParOrder;
	g_pTradeData->GetParOrder(stParOrder);
	m_conParOrder.clear();
	bool bHasID = false;   //更新ParOrder时，要查看是否有OrderId
	for (UINT i = 0; i < stParOrder.size(); i++)
	{
		const TOrder* pOrder = (const TOrder*)stParOrder.at(i);
		if (!strcmp(pOrder->UserNo, g_pGlobalModel->m_strUserNo.c_str()) && !strcmp(pOrder->Sign, g_pGlobalModel->m_strSign.c_str()))
		{
			if ((pOrder->StrategyType == stPreOrder || pOrder->StrategyType == stAutoOrder)
				&& (pOrder->OrderState == osTriggering || pOrder->OrderState == osSended||pOrder->OrderState==osAccept)) continue;
			m_conParOrder.push_back(pOrder);
			if (pOrder->OrderID == m_iParOrderID)
				bHasID = true;
		}
	}

	if (!bHasID)
		m_iParOrderID = -1;

}
//确保回调回来的用户和当前用户一致
//true:更新全部
//false:更新部分
bool TradeInfoModel::SetPosition(const TPosition* pPosition)
{
	TDataContainer stPosition;
	g_pTradeData->GetAllPositionTotal(stPosition, g_pGlobalModel->m_strSign.c_str());

	uint iPreCount = m_conTotalPosition.size();
	m_conTotalPosition.clear();

	bool bHasPosition = false;
	int iCount = 0;
	int iIndex = -1;
	for (UINT i = 0; i < stPosition.size(); i++)
	{
		const TPosition* pTempPosition = (const TPosition*)stPosition.at(i);
		if (!strcmp(pTempPosition->UserNo, g_pGlobalModel->m_strUserNo.c_str()) /*&& !strcmp(pPosition->Sign, g_pGlobalModel->m_strSign.c_str())*/)
		{
			m_conTotalPosition.push_back(pTempPosition);

			if (pTempPosition == pPosition)
				iIndex = iCount;
			iCount++;

			if (pTempPosition == m_pNowSelectPosition)
				bHasPosition = true;
		}
	}
	if (!bHasPosition)
		m_pNowSelectPosition = nullptr;

	bool bIsUpdate = true;
	if (pPosition)
		bIsUpdate = IsAddOrUpdate(pPosition->PositionQty, iIndex);
	uint iNowCount = m_conTotalPosition.size();
	return iPreCount == iNowCount&&bIsUpdate || iPreCount != iNowCount;
}
bool TradeInfoModel::IsAddOrUpdate(uint iQty, int index)
{
	bool bRet = true;
	if (index >= 0 && index < m_vecPositionQty.size())
	{
		if (iQty == m_vecPositionQty[index]) //加仓减仓
			bRet =  false;
	}

	m_vecPositionQty.clear();
	for (uint i = 0; i < m_conTotalPosition.size(); i++)
		m_vecPositionQty.push_back(((const TPosition*)m_conTotalPosition.at(i))->PositionQty);

	return bRet;
}
void TradeInfoModel::SetOrder()
{
	TDataContainer stOrder;
	g_pTradeData->GetAllOrder(stOrder);
	m_conOrder.clear();

	bool bHasOrder = false;
	for (UINT i = 0; i < stOrder.size(); i++)
	{
		const TOrder* pOrder = (const TOrder*)stOrder.at(i);
		if (!strcmp(pOrder->UserNo, g_pGlobalModel->m_strUserNo.c_str()) && !strcmp(pOrder->Sign, g_pGlobalModel->m_strSign.c_str()))
		{
			m_conOrder.push_back(pOrder);
		}
	}
}

void TradeInfoModel::SetContractNo(string strCode)
{
	CConvert::TContractNoToTContractKey(strCode.c_str(), m_stContract);
	SetLogInfo();
}

void TradeInfoModel::SetLogInfo()
{
	char loginfo[100] = { 0 };
	char szText[20] = { 0 };
	char szText2[20] = { 0 };
	char szPrice[20] = { 0 };
	int iLogCount = 0;
	memset(&m_stLogData, 0, sizeof(TLogData));
	for (int i = m_conOrder.size() - 1; i >= 0; i--)
	{
		TOrder* pOrder = (TOrder*)m_conOrder.at(i);
		if (!strcmp(pOrder->CommodityNo, m_stContract.CommodityNo) && !strcmp(pOrder->ContractNo, m_stContract.ContractNo))
		{
			char szText[50] = { 0 };
			char szText2[50] = { 0 };
			char szPrice[50] = { 0 };
			if (g_pTradeData)
			{
				const SCommodity* pCom = g_pTradeData->GetCommodity(pOrder);
				if (pCom == NULL)
					return;
				DoubleToChar(pCom->PricePrec, pOrder->OrderPrice, szPrice);
			}
			if (g_pGlobalModel->m_utUserType == utInner)
			{
				if (pOrder->Direct == dBuy && pOrder->Offset == oOpen)
					sprintf_s(szText, "%s%d%s", _GetTextTM(BuyOpen_Index), pOrder->OrderQty, _GetTextTM(Lots_Index));
				else if (pOrder->Direct == dSell && pOrder->Offset == oOpen)
					sprintf_s(szText, "%s%d%s", _GetTextTM(SellOpen_Index), pOrder->OrderQty, _GetTextTM(Lots_Index));
				else if (pOrder->Direct == dBuy && pOrder->Offset == oCover)
					sprintf_s(szText, "%s%d%s",_GetTextTM(BuyClose_Index),pOrder->OrderQty,_GetTextTM(Lots_Index));
				else if (pOrder->Direct == dSell && pOrder->Offset == oCover)
					sprintf_s(szText, "%s%d%s",  _GetTextTM(SellClose_Index), pOrder->OrderQty,_GetTextTM(Lots_Index));
				else if (pOrder->Direct == dSell && pOrder->Offset == oCoverT)
					sprintf_s(szText, "%s%d%s", _GetTextTM(SellCloseT_Index), pOrder->OrderQty, _GetTextTM(Lots_Index));
				else if (pOrder->Direct == dBuy && pOrder->Offset == oCoverT)
					sprintf_s(szText, "%s%d%s", _GetTextTM(BuyCloseT_Indedx), pOrder->OrderQty, _GetTextTM(Lots_Index));
			}
			else if (g_pGlobalModel->m_utUserType == utForeign)
			{
				if (pOrder->Direct==dBuy)
					sprintf_s(szText, "%s%d%s", _GetTextTM(Buy_Index), pOrder->OrderQty, _GetTextTM(Lots_Index));
				else if (pOrder->Direct==dSell)
					sprintf_s(szText, "%s%d%s", _GetTextTM(Sell_Index), pOrder->OrderQty, _GetTextTM(Lots_Index));
			}
			TOrderState2Char(pOrder->OrderState, pOrder->StrategyType, szText2, sizeof(szText2));


			char Time[20] = { 0 };
			vector<string> vstrTime = CConvert::SplitString(pOrder->UpdateDateTime, ' ');
			if (vstrTime.size() > 1)
			{
				sprintf_s(Time, "%s", vstrTime.at(1).c_str());
			}

			switch (pOrder->CommodityType)
			{
			case ctFutures:
				sprintf_s(m_stLogData.loginfo[iLogCount], "%s: %s%s %s %s (%s)", _GetTextTM(Order_Indedx), pOrder->CommodityNo, pOrder->ContractNo, szText, szPrice, Time);
				break;
			case ctOption:
				sprintf_s(m_stLogData.loginfo[iLogCount], "%s: %s%s%c%s %s %s (%s)", _GetTextTM(Order_Indedx), pOrder->CommodityNo, pOrder->ContractNo,
					pOrder->OptionType, pOrder->StrikePrice, szText, szPrice, Time);
				break;
			}

			if (pOrder->ErrorCode == 0)
			{
				sprintf_s(m_stLogData.LogState[iLogCount], "%s: %s", _GetTextTM(State_Index), szText2);
			}

			else
			{
				wstring  wErrorText = CConvert::UTF8ToUnicode(pOrder->ErrorText, g_language == CHT ? 950 : 936);			
				sprintf_s(m_stLogData.LogState[iLogCount], "%s: %s", szText2, UnicodeToANSI(wErrorText).c_str());
			}
			iLogCount++;
		}
		if (iLogCount == 2)
			break;
	}
}

void TradeInfoModel::SetMatch()
{
	TDataContainer stMatch;
	g_pTradeData->GetAllMatch(stMatch);
	m_conMatch.clear();

	bool bHasMatch = false;
	for (UINT i = 0; i < stMatch.size(); i++)
	{
		const TMatch* pMatch = (const TMatch*)stMatch.at(i);
		if (!strcmp(pMatch->UserNo, g_pGlobalModel->m_strUserNo.c_str()) && !strcmp(pMatch->Sign, g_pGlobalModel->m_strSign.c_str()))
		{
			m_conMatch.push_back(pMatch);
		}
	}
}

void TradeInfoModel::SetStopLPOrder(bool Notrig)
{
	int Trig = Notrig ? 1 : -1;
	m_conStopLPOrder.clear();
	g_pStopLP->GetALLStopLPOrder(m_conStopLPOrder, g_pGlobalModel->m_strUserNo.c_str(), "", dBoth, Trig);
	g_Presenter->Send(TPresenter::CMD_Presenter_OnStopLPOrderList, nullptr);
}

void TradeInfoModel::OnUserChange()
{
	//当用户变了之后，所有保存的和用户相关的变量都要重置
	SetPosition();
	SetParOrder();
	SetOrder();
	SetMatch();
	SetStopLPOrder(m_StopLPShowNotTrigger);
	//SetLogInfo();
	m_pNowSelectPosition = nullptr;
	m_iParOrderID = -1;
}
const TPosition* TradeInfoModel::GetPosition(int index)
{
	m_pNowSelectPosition = NULL;
	if (index < m_conTotalPosition.size())
	{
		m_pNowSelectPosition = (const TPosition*)m_conTotalPosition.at(index);
	}
	return m_pNowSelectPosition;
}

void TradeInfoModel::SetOrderStateClickIndex(int index)
{
	m_iParOrderID = -1;
	if (index < m_conParOrder.size())
	{
		m_iParOrderID = ((const TOrder*)m_conParOrder.at(index))->OrderID;
	}
}
void TradeInfoModel::OnClickBtn(WPARAM wParam, LPARAM lParam)
{
	if (wParam == btnFromOrderView)
	{
		if (m_iParOrderID == -1)return;
		const TOrder* pOrder = g_pTradeData->GetOrder(m_iParOrderID);
		if (!pOrder)return;
		switch (lParam)
		{
		case TOrderView::ID_CancelOrder:
			g_pTradeApi->CancelOrder(m_iParOrderID);
			break;
		case TOrderView::ID_Consideration://对价跟
			ModifyOrderPrice(ptCounterPrice, pOrder);
			break;
		case TOrderView::ID_LineUp://排队跟
			ModifyOrderPrice(ptQueuePrice, pOrder);
			break;
		case TOrderView::ID_Chase://追单
			g_pTradeAction->InsertChaseOrder(pOrder);
			break;
		}
	}
	else if (wParam == btnFromTradeInfo)
	{
		if (!m_pNowSelectPosition || !m_pNowSelectPosition->PositionQty) return;
		switch (lParam)
		{
		case TTradeInfo::ID_BtnCover33:
			Cover(m_pNowSelectPosition, m_pNowSelectPosition->PositionQty / 3);
			break;
		case TTradeInfo::ID_BtnCover50:
			Cover(m_pNowSelectPosition, m_pNowSelectPosition->PositionQty / 2);
			break;
		case TTradeInfo::ID_BtnCover100:
			Cover(m_pNowSelectPosition, m_pNowSelectPosition->PositionQty);
			break;
		case TTradeInfo::ID_BtnBackHand:
			g_pTradeAction->InsertBackOrder(m_pNowSelectPosition, g_pGlobalModel->m_stOptionConfig.iBackFillOrderMod);
			break;
		case TTradeInfo::ID_BtnStopLoss:
			break;
		case TTradeInfo::ID_BtnRisk:
			break;
		}
	}
}
void TradeInfoModel::Cover(const TPosition* pPosition, uint iQty)
{
	if (iQty == 0)
	{
		m_pPresenter->ShowMsgBox(TPresenter::Msg_OrderQty_Zero);
		return;
	}
	TSendOrder stOrder;
	memset(&stOrder, 0, sizeof(TSendOrder));
	CConvert::DefaultDealClose(pPosition, stOrder);
	stOrder.OrderQty = iQty;

	CConvert::GetPrice(&stOrder, stOrder.Direct, g_pGlobalModel->m_stOptionConfig.iProportionFillOrderMod, stOrder.OrderPrice); //价格获取失败怎么办
	int iRet = g_pTradeAction->InsertCloseOrder(stOrder);
	m_pPresenter->ShowMsgBox(iRet);
}
void TradeInfoModel::OnCommand(TMenuInfo *pMenuInfo)
{
	switch (pMenuInfo->MenuID)
	{
		//持仓右键菜单
	case TTradeInfo::IDM_RIVALCOLSE:      //对价平仓
	case TTradeInfo::IDM_OVERCLOSE:       //超价平仓
	case TTradeInfo::IDM_MARKETCLOSE:     //市价平仓
		Cover(pMenuInfo->MenuID);
		break;
	case TTradeInfo::IDM_BACKHAND:        //反手
		if (CheckPosition(pMenuInfo->pData))
			g_pTradeAction->InsertBackOrder((const TPosition*)pMenuInfo->pData, g_pGlobalModel->m_stOptionConfig.iBackFillOrderMod);
		break;
	case TTradeInfo::IDM_STOPLOSS:        //设置止损单
		if (CheckPosition(pMenuInfo->pData))
			g_Presenter->RClkMenuStopLPOrder((const TPosition*)pMenuInfo->pData, TStopLPView::Page_StopLoss);
		break;
	case TTradeInfo::IDM_STOPWIN:         //设置止盈单
		if (CheckPosition(pMenuInfo->pData))
			g_Presenter->RClkMenuStopLPOrder((const TPosition*)pMenuInfo->pData, TStopLPView::Page_StopProfit);
		break;
	case TTradeInfo::IDM_BREAKEVEN:       //设置保本单
		if (CheckPosition(pMenuInfo->pData))
			g_Presenter->RClkMenuStopLPOrder((const TPosition*)pMenuInfo->pData, TStopLPView::Page_BreakEven);
		break;
	case TTradeInfo::IDM_TIMINGCLOSE:     //定时平仓 变成了平组合单
		if (CheckPosition(pMenuInfo->pData))
			SetCmbContract((const TPosition*)pMenuInfo->pData);
		break;
	case TTradeInfo::IDM_CLOSESH:         //平上海昨仓
		if (CheckPosition(pMenuInfo->pData))
			CoverSHFELastPosition((const TPosition*)pMenuInfo->pData);
		break;
	case TTradeInfo::IDM_LOCKPOSITION:    //锁仓
		if (CheckPosition(pMenuInfo->pData))
			LockPosition((const TPosition*)pMenuInfo->pData);
		break;
	case TTradeInfo::IDM_OVERBACKHAND:    //超价反手
		if (CheckPosition(pMenuInfo->pData))
			g_pTradeAction->InsertBackOrder((const TPosition*)pMenuInfo->pData, ptSuperPrice);
		break;
	case TTradeInfo::IDM_MARKETBACKHAND:  //市价反手
		if (CheckPosition(pMenuInfo->pData))
			g_pTradeAction->InsertBackOrder((const TPosition*)pMenuInfo->pData, ptMarketPrice);
		break;
	case TTradeInfo::IDM_CANCELCLOSE:     //撤平仓单+清仓
		if (m_pPresenter->ShowMsgBox(Msg_CoverAll_Position))
			g_pTradeAction->CoverAll(&m_conParOrder, &m_conTotalPosition);
		break;
		//委托状态右键菜单
	case TTradeInfo::IDM_CANCEL:          //撤单
		if (CheckParOrder(pMenuInfo->pData))
			g_pTradeApi->CancelOrder(((const TOrder*)pMenuInfo->pData)->OrderID);
		break;
	case TTradeInfo::IDM_RIVALFOLLOW:     //对价跟进
		if (CheckParOrder(pMenuInfo->pData))
			ModifyOrderPrice(ptCounterPrice, (const TOrder*)pMenuInfo->pData);
		break;
	case TTradeInfo::IDM_LINEUPFOLLOW:    //排队跟进
		if (CheckParOrder(pMenuInfo->pData))
			ModifyOrderPrice(ptQueuePrice, (const TOrder*)pMenuInfo->pData);
		break;
	case TTradeInfo::IDM_OVERFOLLOW:      //超价跟进
		if (CheckParOrder(pMenuInfo->pData))
			ModifyOrderPrice(ptSuperPrice, (const TOrder*)pMenuInfo->pData);
		break;
	case TTradeInfo::IDM_MARKETFOLLOW:    //市价跟进
		if (CheckParOrder(pMenuInfo->pData))
			ModifyOrderPrice(ptMarketPrice, (const TOrder*)pMenuInfo->pData);
		break;
	case TTradeInfo::IDM_CANCELALL:       //撤所有单
	{
		for (uint i = 0; i < m_conParOrder.size(); i++)
			g_pTradeApi->CancelOrder(((const TOrder*)m_conParOrder.at(i))->OrderID);
	}
	break;
	//条件单右键菜单
		//=================================================止损单右键功能：
	case TTradeInfo::IDM_STOPCANCEL:
	{
		if (CheckStopLP(pMenuInfo->pData))
		{
			g_pStopLP->DeleteLPOrder(((const TStopLPitem*)pMenuInfo->pData)->keyID);
		}
		break;
	}
	case TTradeInfo::IDM_STOPCHANGE:
	{
		if (CheckStopLP(pMenuInfo->pData))
		{
			const TStopLPitem* pitem =(const TStopLPitem*)pMenuInfo->pData;
			TContractKey contractKey;
			memset(&contractKey, 0, sizeof(TContractKey));
			memcpy_s(&contractKey.Sign, 21, g_pGlobalModel->m_strSign.c_str(), 21);
			CConvert::TContractNoToTContractKey(pitem->code, contractKey);
			TDirect dir = (pitem->direct == dBuy) ? dSell : dBuy;
			const TPosition* pos = g_pTradeData->GetPositionTotal(g_pGlobalModel->m_strUserNo.c_str(), &contractKey, dir);
			if (pos == nullptr)
				return;
			g_Presenter->RClkMenuStopLPOrder(pos, pitem->keyID,true);
		}
		break;
	}
	case TTradeInfo::IDM_STOPPAUSE:
	{
		if (CheckStopLP(pMenuInfo->pData))
		{
			TStopLPitem pOrder;
			memcpy_s(&pOrder, sizeof(TStopLPitem), (const TStopLPitem*)pMenuInfo->pData, sizeof(TStopLPitem));
			pOrder.state = osSuspend;
			pOrder.TriggerCount = 0;
			pOrder.LastTirgger = false;//暂停，状态挂起，触发次数清空。
			g_pStopLP->ModifyLPOrder(&pOrder);
		}
		break;
	}
	case TTradeInfo::IDM_STOPSTART:
	{
		if (CheckStopLP(pMenuInfo->pData))
		{
			TStopLPitem pOrder;
			memcpy_s(&pOrder, sizeof(TStopLPitem), (const TStopLPitem*)pMenuInfo->pData, sizeof(TStopLPitem));
			if (osSuspend == pOrder.state)
			{
				pOrder.state = osGeneral;
			}
			g_pStopLP->ModifyLPOrder(&pOrder);
		}
		break;
	}
	case TTradeInfo::IDM_STOPDELETEALL://删除该客户该容器内的所有单
	{
		for (uint i = 0; i < m_conStopLPOrder.size(); i++)
		{
			g_pStopLP->DeleteLPOrder(((const TStopLPitem*)m_conStopLPOrder.at(i))->keyID);
		}
		break;
	}
	case TTradeInfo::IDM_TRIG:
		if (m_StopLPShowNotTrigger)
		{
			m_StopLPShowNotTrigger = false;
			SetStopLPOrder(m_StopLPShowNotTrigger);

		}
		break;
	case TTradeInfo::IDM_NOTTRIG:
		if (!m_StopLPShowNotTrigger)
		{
			m_StopLPShowNotTrigger = true;
			SetStopLPOrder(m_StopLPShowNotTrigger);
		}
		break;
		//委托和成交======保存  刷新

		//时间单
	case TTradeInfo::IDM_TCOND_DEL:			//删除
		break;
	case TTradeInfo::IDM_TCOND_SHOWTRIG:
		break;
	case TTradeInfo::IDM_TCOND_SHOWNOTTRIG:
		break;
	default:
		break;
	}
}

void TradeInfoModel::ModifyOrderPrice(UINT type, const TOrder* pOrder)
{
	if (pOrder == nullptr)
	{
		if (m_iParOrderID == -1)return;
		pOrder = g_pTradeData->GetOrder(m_iParOrderID);
		if (!pOrder)return;
	}
	TModifyOrder stModify;
	memcpy(&stModify, pOrder, sizeof(TModifyOrder));

	CConvert::GetPrice(&stModify, stModify.Direct, type, stModify.OrderPrice);
	if (g_pTradeApi)
		g_pTradeApi->ModifyOrder(stModify);
}

void TradeInfoModel::Cover(UINT priceType)
{
	if (!m_pNowSelectPosition)return;
	TSendOrder stOrder;
	CConvert::DefaultDealClose(m_pNowSelectPosition, stOrder);
	//获取价格
	switch (priceType)
	{
	case TTradeInfo::IDM_RIVALCOLSE:
		CConvert::GetPrice(&stOrder, stOrder.Direct, ptCounterPrice, stOrder.OrderPrice);
		break;
	case TTradeInfo::IDM_MARKETCLOSE:
		CConvert::GetPrice(&stOrder, stOrder.Direct, ptMarketPrice, stOrder.OrderPrice);
		break;
	case TTradeInfo::IDM_OVERCLOSE:
		CConvert::GetPrice(&stOrder, stOrder.Direct, ptSuperPrice, stOrder.OrderPrice);
		break;
	default:
		break;
	}
	int iRet = g_pTradeAction->InsertCloseOrder(stOrder);
	m_pPresenter->ShowMsgBox(iRet);
}


bool TradeInfoModel::CheckPosition(const void* pPosition)
{
	if (!pPosition)
	{
		m_pPresenter->ShowMsgBox(EN_Null_Position);
		return false;
	}
	return true;
}
bool TradeInfoModel::CheckParOrder(const void* pOrder)
{
	if (!pOrder)
	{
		m_pPresenter->ShowMsgBox(EN_Null_ParOrder);
		return false;
	}
	return true;
}

bool TradeInfoModel::CheckStopLP(const void* pStopLP)
{
	if (!pStopLP)
	{
		m_pPresenter->ShowMsgBox(Msg_OK, "未选中止损单");
		return false;
	}
	return true;
}

void TradeInfoModel::LockPosition(const TPosition* pPosition)
{
	TSendOrder stOrder;
	CConvert::DefaultDealClose(pPosition, stOrder);
	CConvert::GetPrice(&stOrder, stOrder.Direct, ptCounterPrice, stOrder.OrderPrice);
	stOrder.Offset = oOpen;
	if (g_pTradeApi)
		g_pTradeApi->InsertOrder(stOrder);
}
void TradeInfoModel::CoverSHFELastPosition(const TPosition* pPosition)
{
	if (g_pTradeData)
	{
		if (strcmp(pPosition->ExchangeNo, exSHFE) == 0)
		{
			TSendOrder stOrder;
			CConvert::DefaultDealClose(pPosition, stOrder);
			uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
			g_pTradeData->GetPositionVol(pPosition->UserNo, pPosition, LongCount, ShortCount, DayLongCount, DayShortCount);
			CConvert::GetPrice(&stOrder, stOrder.Direct, ptCounterPrice, stOrder.OrderPrice);
			if ((stOrder.Direct == dBuy && (ShortCount > DayShortCount)) || (stOrder.Direct == dSell && (LongCount > DayLongCount)))
			{
				stOrder.Offset = oCover;
				if (stOrder.Direct == dBuy)
					stOrder.OrderQty = ShortCount - DayShortCount;
				else if (stOrder.Direct == dSell)
					stOrder.OrderQty = LongCount - DayLongCount;
			}
			else
			{
				m_pPresenter->ShowMsgBox(EN_No_LastSHFEPosition);
				return;
			}
			if (g_pTradeApi)
				g_pTradeApi->InsertOrder(stOrder);
		}
		else
			m_pPresenter->ShowMsgBox(EN_Not_SHFEPosition);
	}
}

bool TradeInfoModel::GetCmbContract(const TPosition* pPosition, TContractKey& stContractKey, TDirect& direct)
{
	if (pPosition->IsCmb != bYes)
	{
		m_pPresenter->ShowMsgBox(EN_Not_CmbPosition);
		return false;
	}
	for (uint i = 0; i < m_conTotalPosition.size(); i++)
	{
		memset(&stContractKey, 0, sizeof(TContractKey));
		memcpy(&stContractKey, pPosition, sizeof(TContractSingleKey));
		stContractKey.OptionType2 = ctNone;

		const TPosition* pTemp = (const TPosition*)m_conTotalPosition.at(i);
		if (pTemp == pPosition || strcmp(pPosition->ExchangeNo, pTemp->ExchangeNo) || pPosition->Direct == pTemp->Direct) continue;
		bool bSameCommodity = strcmp(pPosition->CommodityNo, pTemp->CommodityNo) == 0;
		bool bSameContract = strcmp(pPosition->ContractNo, pTemp->ContractNo) == 0;
		if (bSameCommodity&&!bSameContract) //跨期，品种一致，合约号不一致
		{
			strcpy_s(stContractKey.ContractNo2, pTemp->ContractNo);
			stContractKey.CommodityType = ctSpreadMonth;
		}
		else if (!bSameCommodity&&bSameContract)//品种不一致，合约一致
		{
			sprintf_s(stContractKey.CommodityNo, "%s&%s", pPosition->ContractNo, pTemp->ContractNo);
			stContractKey.CommodityType = ctSpreadCommodity;
			strcpy_s(stContractKey.ContractNo2, stContractKey.ContractNo);
		}
		if (CConvert::IsValidTradeCommodity(stContractKey) && CConvert::IsCombContract(stContractKey.CommodityType))
		{
			direct = pPosition->Direct;
			return true;
		}
		else if (stContractKey.CommodityType == ctSpreadMonth)    //跨期
		{
			strcpy_s(stContractKey.ContractNo, pTemp->ContractNo);
			strcpy_s(stContractKey.ContractNo2, pPosition->ContractNo);
			if (CConvert::IsValidTradeCommodity(stContractKey))
			{
				direct = pTemp->Direct;
				return true;
			}
		}
		else if (stContractKey.CommodityType == ctSpreadCommodity) //跨品种
		{
			sprintf_s(stContractKey.CommodityNo, "%s&%s", pTemp->ContractNo, pPosition->ContractNo);
			if (CConvert::IsValidTradeCommodity(stContractKey))
			{
				direct = pTemp->Direct;
				return true;
			}
		}
	}
	return false;
}

void TradeInfoModel::SetCmbContract(const TPosition* pPosition)
{
	TContractKey stKey;
	TDirect direct = dNone;
	if (GetCmbContract(pPosition, stKey, direct))
	{
		string sCode = "";
		CConvert::TContractKeyToTContractNo(stKey, sCode);
		g_Presenter->DealContractCode(sCode, direct);
	}
	else
	{
		m_pPresenter->ShowMsgBox(EN_Not_FoundCmb);
	}
}

void __cdecl TradeInfoModel::Update(DataType type, const TStopLPitem*pdata)
{
	if (strcmp(g_pGlobalModel->m_strUserNo.c_str(), pdata->userNo))//用户判断，只对应变动当前用户
	{
		return;
	}
	if (m_StopLPShowNotTrigger)
	{
		switch (type)
		{
		case Add_Type:
		{
			m_conStopLPOrder.push_back(pdata);
			g_Presenter->Send(TPresenter::CMD_Presenter_OnStopLPOrderList, nullptr);
			break;
		}
		case Del_Type:
		{
			m_conStopLPOrder.clear();
			g_Presenter->Send(TPresenter::CMD_Presenter_OnStopLPOrderList, nullptr);
			break;
		}
		case Chg_Type:
		{
			int i;
			m_conStopLPOrder.FindItem(pdata, i);
			if (osDeleted == pdata->state || osTrig == pdata->state)
			{
				if (i == m_conStopLPOrder.size())//没找到
				{
					return;
				}
				m_conStopLPOrder.Delete(i);
				g_Presenter->Send(TPresenter::CMD_Presenter_OnStopLPOrderList, nullptr);
			}
			else
			{
				g_Presenter->Send(TPresenter::CMD_Presenter_OnStopLPOrderList, pdata);
			}
			break;
		}
		case Trg_Type:
		{
			g_Presenter->Send(TPresenter::CMD_Presenter_OnStopLPOrderList, pdata);
			break;
		}
		case ChO_Type:
		{
			SetStopLPOrder();
			break;
		}
		default:
			break;
		}
	}
	else
	{
		if (Chg_Type == type)
		{

			if (osTrig == pdata->state)
			{
				m_conStopLPOrder.push_back(pdata);

			}
			else if (osDeleted == pdata->state)
			{
				int i;
				m_conStopLPOrder.FindItem(pdata, i);
				if (i < m_conStopLPOrder.size())
				{
					m_conStopLPOrder.Delete(i);
				}
			}
			g_Presenter->Send(TPresenter::CMD_Presenter_OnStopLPOrderList, nullptr);
		}
	}

}