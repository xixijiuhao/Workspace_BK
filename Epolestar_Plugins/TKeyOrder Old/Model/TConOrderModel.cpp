#include "BaseInclude.h"


#define notTriggerLocal(pOrder)  (pOrder->OrderType == cotAutoOrder || pOrder->OrderType == cotPreOrder ||pOrder->OrderType==cotPriceServer)
#define triggerLocal(pOrder) (pOrder->OrderType==cotPrice || pOrder->OrderType==cotTime)

#define strKeyName L"Str"
#define intKeyName L"Int"
#define doubleKeyName L"Double"
TConOrderModel::TConOrderModel()
{
	m_iOrderId = 0;
}

TConOrderModel::~TConOrderModel()
{

}

bool TConOrderModel::InsertConOrder(TConditionOrder* pOrder)
{
	bool bRet = true;
	if (pOrder->OrderType == cotPrice)
	{
		m_mapContractId.insert(make_pair(pOrder->ContractId, pOrder));
		g_pStarApi->SubQuote(&pOrder->ContractId,1, g_Presenter);
	}
	else if (notTriggerLocal(pOrder))
	{
		bRet = SendOrder(pOrder);
		pOrder->Triggered = false;
		pOrder->RunState = rsRun;
	}
	if (bRet)
	{
		m_vecCondtionOrder.push_back(pOrder);
	}
	return true;
}
void TConOrderModel::ModifyPreAutoOrder(TConditionOrder* pOrder)
{
	if (!pOrder)return;
	g_pTradeApi->CancelOrder(pOrder->OrderID);
	if (notTriggerLocal(pOrder))
	{
		SendOrder(pOrder);
		pOrder->Triggered = false;
		pOrder->RunState = rsRun;
	}
}
//行情回调回来，PostMessage到消息循环中
//在消息循环中调用OnQuote
void TConOrderModel::OnQuote(const SContract* quote)
{
	auto mapRange = m_mapContractId.equal_range(quote->ContractNo);
	map<string, TConditionOrder*>::iterator iter;

	SQuoteSnapShot QuoteData;
	memset(&QuoteData, 0, sizeof(QuoteData));
	double dLastPrice = 0;
	if (g_pStarApi->GetSnapshot(quote->ContractNo, &QuoteData))
	{
		dLastPrice = QuoteData.Data[S_FID_LASTPRICE].Price;
	}
	if (dLastPrice == 0)
		return;

	bool bTriggered = false;
	for (iter = mapRange.first; iter != mapRange.second; iter++)
	{
		if (iter->second->OrderType==cotPrice &&iter->second->RunState==rsRun && CheckPrice(iter->second, dLastPrice)) //价格条件达到
		{
			SendOrder(iter->second);
			bTriggered = true;
		}
	}

	if (bTriggered)
	{
		DealData();
	}
}

void TConOrderModel::DealData()
{
	bool bIsNowUser = false;
	bool bErase = false;
	for (auto iter = m_vecCondtionOrder.begin(); iter != m_vecCondtionOrder.end();)
	{
		//TConditionOrder * pOrder = *(iter);
		//if (pOrder->Triggered)
		//{
		//	if (pOrder->OrderType == cotPrice)
		//	{
		//		auto iterMap = find(m_mapContractId.begin(), m_mapContractId.end(), pOrder);
		//		if (iterMap != m_mapContractId.end())
		//			m_mapContractId.erase(iterMap);
		//		iterMap = m_mapContractId.find(pOrder->ContractId);
		//		if (iterMap == m_mapContractId.end())
		//		{
		//			g_pQuoteApi->UnsubQuote(pOrder->ContractId, g_Presenter);
		//		}
		//	}
		//	if (!bIsNowUser)bIsNowUser = strcmp(pOrder->UserNo, g_pGlobalModel->m_strUserNo.c_str()) == 0;
		//	iter = m_vecCondtionOrder.erase(iter);
		//	m_vecTriggeredConOrder.push_back(pOrder);
		//}
		//else
		//{
		//	iter++;
		//}
		bool bRet = DealData(*iter,bErase);
		if (bErase)
			iter = m_vecCondtionOrder.erase(iter);
		else
			iter++;
		bErase = false;
		bIsNowUser = bIsNowUser ? true : bRet;

	}
	if (bIsNowUser)
		g_Presenter->UpdateConList();
	//Update List
}

bool TConOrderModel::DealData(TConditionOrder* pOrder, bool& bErase)
{
	bool bSameUser = strcmp(pOrder->UserNo, g_pGlobalModel->m_strUserNo.c_str())==0;
	if (pOrder->Triggered)
	{
		if (pOrder->OrderType == cotPrice)
		{
			auto rangeMap = m_mapContractId.equal_range(pOrder->ContractId);
			map<string, TConditionOrder*>::iterator iterMap;
			for (iterMap = rangeMap.first; iterMap != rangeMap.second; iterMap++)
			{
				if (iterMap->second == pOrder)
				{
					m_mapContractId.erase(iterMap);
					break;
				}
			}
			//new
			auto findIter = m_mapContractId.find(pOrder->ContractId);
			if (findIter == m_mapContractId.end())
				g_pStarApi->UnsubQuote(&pOrder->ContractId,1, g_Presenter);
			//new
		}
		auto iter = find(m_vecCondtionOrder.begin(), m_vecCondtionOrder.end(), pOrder);
		if (iter != m_vecCondtionOrder.end())
		{
			bErase = true;
		}
		m_vecTriggeredConOrder.push_back(pOrder);
	}
	return strcmp(pOrder->UserNo, g_pGlobalModel->m_strUserNo.c_str()) == 0;
}

bool TConOrderModel::SendOrder(TConditionOrder* pOrder)
{
	bool bRet = true;
	TSendOrder stOrder; 
	CConvert::DefaultSendOrder(stOrder);

	pOrder->Triggered = true;
	pOrder->RunState = rsTriggered;
	stOrder.OrderQty = pOrder->iQty;
	stOrder.Direct = pOrder->Direct;
	stOrder.Offset = pOrder->Offset;

	//Edit for Verison 25 2017-11-09
	CConvert::SContractNoTypeToTradeContract("", pOrder->ContractId, stOrder);
	//memcpy(&stOrder, pOrder->pContract, sizeof(TContractKey));
	strcpy_s(stOrder.UserNo, g_pGlobalModel->m_strUserNo.c_str());
	strcpy_s(stOrder.Sign, g_pGlobalModel->m_strSign.c_str());

	if (pOrder->OrderType == cotPreOrder)
		stOrder.StrategyType = stPreOrder;
	else if (pOrder->OrderType == cotAutoOrder)
		stOrder.StrategyType = stAutoOrder;
	else if (pOrder->OrderType == cotPriceServer)
	{
		stOrder.StrategyType = stCondition;
		stOrder.TriggerCondition = pOrder->TriggerCondition;
		stOrder.TriggerPrice = pOrder->ConPrice;
	}
	if (pOrder->PriceType == ptSpecifiedPrice)
		stOrder.OrderPrice = pOrder->Price;
	else
		CConvert::GetPrice(&stOrder, stOrder.Direct, pOrder->PriceType, stOrder.OrderPrice);
	if (stOrder.Offset == oOpen)
	{
		pOrder->OrderID = g_pTradeApi->InsertOrder(stOrder); 
	}
	else
	{
		vector<uint>vecOrderId;
		bRet = g_pTradeAction->Cover(stOrder, true, &vecOrderId);
		if (vecOrderId.size())
			pOrder->OrderID = vecOrderId[0];
		if (vecOrderId.size() > 1)
			pOrder->OrderID2 = vecOrderId[1];  //平今，平昨可能有两个OrderID
	}
	return bRet;
}
void TConOrderModel::SendStopLPOrder(TConditionOrder*pOrder, const TOrder* pOrderBack)
{
	if (pOrder->StopType == TConditionOrderFrm::stopNone) return;

	TStopLPInsertItem InsertItem;
	InsertItem.MaxCount = g_pGlobalModel->m_stStopLossConfig.iTriggerCount;
	InsertItem.trigBase = tmbLatest;
	memcpy_s(InsertItem.userNo, sizeof(InsertItem.userNo), g_pGlobalModel->m_strUserNo.c_str(), sizeof(InsertItem.userNo));
	memcpy_s(InsertItem.sign, sizeof(InsertItem.sign), g_pGlobalModel->m_strSign.c_str(), sizeof(InsertItem.sign));

	string strCode = "";
	string contractId = pOrder->ContractId;
	CConvert::QContractNoTypeToStr(contractId, strCode);
	strcpy_s(InsertItem.code, strCode.c_str());

	if (pOrder->Direct == dBuy)
	{
		InsertItem.direct = dSell;
	}
	else if (pOrder->Direct == dSell)
	{
		InsertItem.direct = dBuy;
	}
	else
	{
		InsertItem.direct = pOrder->Direct;
	}
	InsertItem.orderPriceType = ptMarketPrice;//市价止损
	InsertItem.openPrice = pOrderBack->OrderPrice;//开仓均价
	InsertItem.Qty = pOrderBack->OrderQty;
	switch (pOrder->StopType)
	{
	case TConditionOrderFrm::stopLossProfit:
		if (pOrder->LossPoint)
		{
			InsertItem.orderType = otLoss;
			InsertItem.triggerPrice = pOrderBack->OrderPrice - pOrder->LossPoint*pOrder->dMinChangePrice;
			g_pStopLP->InsertLPOrder(&InsertItem);
		}
		if (pOrder->ProfitPoint)
		{
			InsertItem.orderType = otProfit;
			InsertItem.triggerPrice = pOrderBack->OrderPrice + pOrder->ProfitPoint*pOrder->dMinChangePrice;
			g_pStopLP->InsertLPOrder(&InsertItem);
		}
		break;
	case TConditionOrderFrm::stopFloatChase:
		if (pOrder->DrawBackPoint)
		{
			InsertItem.orderType = otFloat;
			InsertItem.floatPoint = pOrder->DrawBackPoint*pOrder->dMinChangePrice;
			g_pStopLP->InsertLPOrder(&InsertItem);
		}

		break;
	case TConditionOrderFrm::stopLoss:
		if (pOrder->LossPoint)
		{
			InsertItem.orderType = otLoss;
			InsertItem.triggerPrice = pOrderBack->OrderPrice - pOrder->LossPoint*pOrder->dMinChangePrice;
			g_pStopLP->InsertLPOrder(&InsertItem);
		}
		break;
	case TConditionOrderFrm::stopProfit:
		if (pOrder->ProfitPoint)
		{
			InsertItem.orderType = otProfit;
			InsertItem.triggerPrice = pOrderBack->OrderPrice + pOrder->ProfitPoint*pOrder->dMinChangePrice;
			g_pStopLP->InsertLPOrder(&InsertItem);
		}
		break;
	case TConditionOrderFrm::stopSpeciLossProfit:
		InsertItem.orderType = otLoss;
		if (pOrder->LossPrice)
		{
			InsertItem.triggerPrice = pOrder->LossPrice;
			g_pStopLP->InsertLPOrder(&InsertItem);
		}
		InsertItem.orderType = otProfit;
		if (pOrder->ProfitPrice)
		{
			InsertItem.triggerPrice = pOrder->ProfitPrice;
			g_pStopLP->InsertLPOrder(&InsertItem);
		}
		break;
	}
}


bool TConOrderModel::CheckPrice(TConditionOrder* pOrder, double dPrice)
{
	if (pOrder->TriggerCondition == tcGreaterEqual)
	{
		return dPrice >= pOrder->ConPrice;
	}
	else if (pOrder->TriggerCondition == tcLessEqual)
	{
		return dPrice <= pOrder->ConPrice;
	}
	return false;
}

void TConOrderModel::OnTimer()
{
	time_t tNow = time(nullptr);
	bool bTrrigred = false;
	for (uint i = 0; i < m_vecCondtionOrder.size(); i++)
	{
		if (m_vecCondtionOrder[i]->RunState==rsRun && m_vecCondtionOrder[i]->OrderType == cotTime)
		{
			if (tNow == m_vecCondtionOrder[i]->iTimeOut)
			{
				SendOrder(m_vecCondtionOrder[i]);
				bTrrigred = true;
			}
		}
	}

	if (bTrrigred)
		DealData();
}

void TConOrderModel::OnConMenuClick(TMenuInfo* pMenuInfo)
{
	if (!pMenuInfo->pData) return;
	bool bShowTrig = (pMenuInfo->Index >> 24) & 0xFF;
	TConditionOrder *pOrder = (TConditionOrder*)pMenuInfo->pData;
	int index = pMenuInfo->Index & 0xFF000000;
	switch (pMenuInfo->MenuID)
	{
	case TTradeInfo::IDM_COND_SENDORDER://立即发出
		if (pOrder->OrderType == cotPrice || pOrder->OrderType == cotTime)
		{
			SendOrder(pOrder);
		}
		else if (pOrder->OrderType == cotPreOrder)
		{
			g_pTradeApi->ResumeOrder(pOrder->OrderID);
			pOrder->Triggered = true;
			pOrder->RunState = rsTriggered;
		}
		DealData();
		break;
	case TTradeInfo::IDM_COND_DEL://删除、撤单
		DeleteOne(pOrder, bShowTrig);
		break;
	case TTradeInfo::IDM_COND_DELALL:
		DeleteAll(bShowTrig);
		break;
	case TTradeInfo::IDM_COND_MODITFY:
		break;
	case TTradeInfo::IDM_COND_SENDALLPREORDER://发出所有埋单
		SendAllPreOrder();
		break;
	}
}

void TConOrderModel::SendAllPreOrder()
{
	for (uint i = 0; i < m_vecCondtionOrder.size(); i++)
	{
		if (m_vecCondtionOrder[i]->OrderType == cotPreOrder)
		{
			g_pTradeApi->ResumeOrder(m_vecCondtionOrder[i]->OrderID);
			if (m_vecCondtionOrder[i]->OrderID2)
				g_pTradeApi->ResumeOrder(m_vecCondtionOrder[i]->OrderID2);
		}
	}
}

void TConOrderModel::DeleteOne(TConditionOrder*pOrder, bool bShowTrig)
{
	CancelOrder(pOrder);

	if (!bShowTrig)
	{
		auto iter = find(m_vecCondtionOrder.begin(), m_vecCondtionOrder.end(), pOrder);
		if (iter != m_vecCondtionOrder.end())
			m_vecCondtionOrder.erase(iter);
		auto rangeMap = m_mapContractId.equal_range(pOrder->ContractId);
		map<string, TConditionOrder*>::iterator iterMap;
		for (iterMap = rangeMap.first; iterMap != rangeMap.second; iterMap++)
		{
			if (iterMap->second == pOrder)
			{
				m_mapContractId.erase(iterMap); 
				break;
			}
		}
		delete pOrder;
	}
	else
	{
		auto iter = find(m_vecTriggeredConOrder.begin(), m_vecTriggeredConOrder.end(), pOrder);
		if (iter != m_vecTriggeredConOrder.end())
			m_vecTriggeredConOrder.erase(iter);
		delete pOrder;
	}
	g_Presenter->UpdateConList();  //删除的肯定是当前用户
}

void TConOrderModel::DeleteAll(bool bShowTrig) //删除当前用户的数据
{
	if (!bShowTrig)
	{
		for (auto iter =m_vecCondtionOrder.begin(); iter != m_vecCondtionOrder.end();)
		{
			if (g_pGlobalModel->m_strUserNo == (*iter)->UserNo)
			{
				CancelOrder(*iter);
				auto rangeMap = m_mapContractId.equal_range((*iter)->ContractId);
				std::map<string, TConditionOrder*>::iterator iterMap;
				for (iterMap = rangeMap.first; iterMap != rangeMap.second; iterMap++)
				{
					if (iterMap->second == *iter)
					{
						m_mapContractId.erase(iterMap);
						break;
					}
				}
				delete *iter;
				iter = m_vecCondtionOrder.erase(iter);
			}
			else
				iter++;
		}
	}
	else
	{
		for (auto iter = m_vecTriggeredConOrder.begin(); iter != m_vecTriggeredConOrder.end();)
		{
			if (g_pGlobalModel->m_strUserNo == (*iter)->UserNo)
			{
				delete *iter;
				iter = m_vecTriggeredConOrder.erase(iter);
			}
			else
				iter++;
		}
	}
	g_Presenter->UpdateConList();
}

void TConOrderModel::CancelOrder(TConditionOrder *pOrder)
{
	if (notTriggerLocal(pOrder))
	{
		g_pTradeApi->CancelOrder(pOrder->OrderID);
		if (pOrder->OrderID2 > 0)
			g_pTradeApi->CancelOrder(pOrder->OrderID2);
	}

}

void TConOrderModel::OnOrder(const TOrder* pOrder)
{
	CheckNeedStopLP(pOrder);
	bool bTriggered = OnParOrder(pOrder);
	for (uint i = 0; i < m_vecCondtionOrder.size(); i++)
	{
		if ((notTriggerLocal(m_vecCondtionOrder[i])) &&
			(m_vecCondtionOrder[i]->OrderID == pOrder->OrderID || m_vecCondtionOrder[i]->OrderID2 == pOrder->OrderID))
		{
			switch (pOrder->OrderState)
			{
			case osFilled:
			case osPartFilled:
			case osQueued:
				bTriggered = true;
				m_vecCondtionOrder[i]->Triggered = true;
				m_vecCondtionOrder[i]->RunState = rsTriggered;
				break;
			case osCanceled:
			case osFail:
				DeleteOne(m_vecCondtionOrder[i], false);
				break;
			}
			break;
		}
	}
	if (bTriggered)
		DealData();
}
void TConOrderModel::CheckNeedStopLP(const TOrder* pOrder)
{
	if (CConvert::GetUserType(pOrder->UserNo)==utInner&& pOrder->Offset!=oOpen)return;

	if (pOrder->OrderState == osFilled || pOrder->OrderState == osPartFilled)
	{
		TConditionOrder* pConOrder = nullptr;
		for (int i = 0; i < m_vecCondtionOrder.size(); i++)
		{
			if (!m_vecCondtionOrder[i]->bHasSendStopLP && pOrder->OrderID == m_vecCondtionOrder[i]->OrderID)
			{
				pConOrder = m_vecCondtionOrder[i];
				break;
			}
		}
		if (!pConOrder)
		{
			for (int i = 0; i < m_vecTriggeredConOrder.size(); i++)
			{
				if (!m_vecTriggeredConOrder[i]->bHasSendStopLP && pOrder->OrderID == m_vecTriggeredConOrder[i]->OrderID)
				{
					pConOrder = m_vecTriggeredConOrder[i];
					break;
				}
			}
		}

		if (pConOrder)
		{
			SendStopLPOrder(pConOrder, pOrder);
			pConOrder->bHasSendStopLP = true;
		}
	}
}
void TConOrderModel::OnUserChange()
{
	//m_vecCondtionOrder.clear();
	//m_vecTriggeredConOrder.clear();

	//for (uint i = 0; i < m_vecAllConditionOrder.size(); i++)
	//{
	//	if (strcmp(m_vecAllConditionOrder[i]->UserNo, g_pGlobalModel->m_strUserNo.c_str()) == 0)
	//		m_vecCondtionOrder.push_back(m_vecAllConditionOrder[i]);
	//}

	//for (uint i = 0; i < m_vecAllTriggeredConOrder.size(); i++)
	//{
	//	if (strcmp(m_vecAllTriggeredConOrder[i]->UserNo, g_pGlobalModel->m_strUserNo.c_str()) == 0)
	//		m_vecCondtionOrder.push_back(m_vecAllTriggeredConOrder[i]);
	//}
	static bool bFirstStart = true;
	if (bFirstStart)
	{
		ReadOrder();
		bFirstStart = false;
	}
	g_Presenter->UpdateConList();
}

bool TConOrderModel::CanUnsubQuote(const char* pszContractId)
{
	return m_mapContractId.find(pszContractId) == m_mapContractId.end();
}

void TConOrderModel::SaveOrder()
{
	TCHAR szFilePath[200] = { 0 };
	GetFilePath(szFilePath, ARRAYSIZE(szFilePath), conditionOrderPath);
	TCHAR szContent[200] = { 0 };
	TCHAR szAppName[10] = { 0 };
	char szStr[200] = { 0 };
	int iIndex = 0;
	for (int i = 0; i < m_vecCondtionOrder.size(); i++)
	{
		TConditionOrder * pOrder = m_vecCondtionOrder.at(i);
		if (notTriggerLocal(pOrder)) continue;
		wsprintf(szAppName, L"%d", iIndex);

		sprintf_s(szStr, "%s,%s,%s,%s,%s", pOrder->InsertTime, pOrder->UserNo, pOrder->TimeOut, pOrder->ContractId,pOrder->Sign);
		WritePrivateProfileString(szAppName, strKeyName, ANSIToUnicode(szStr).c_str(), szFilePath);
		swprintf_s(szContent, L"%d,%lld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", (int)pOrder->OrderType, pOrder->iTimeOut, (int)pOrder->Direct, (int)pOrder->Offset, pOrder->iQty,
			pOrder->PriceType, (int)pOrder->TriggerCondition, pOrder->Precision, (int)pOrder->StopType,
			pOrder->DrawBackPoint, pOrder->LossPoint, pOrder->ProfitPoint,(int)pOrder->bHasSendStopLP,(int)pOrder->SendToServer);
		WritePrivateProfileString(szAppName, intKeyName, szContent, szFilePath);
		swprintf_s(szContent, L"%.6f,%.6f,%.6f,%.6f,%.6f", pOrder->ConPrice, pOrder->Price, pOrder->dMinChangePrice, pOrder->LossPrice, pOrder->ProfitPrice);
		WritePrivateProfileString(szAppName, doubleKeyName, szContent, szFilePath);
		iIndex++;
	}
}

void TConOrderModel::ReadOrder()
{
	TCHAR szFilePath[200] = { 0 };
	GetFilePath(szFilePath, ARRAYSIZE(szFilePath), conditionOrderPath);
	TCHAR szContent[200] = { 0 };
	TCHAR szAppName[10] = { 0 };
	char szStr[200] = { 0 };
	int iIndex = 0;
	wsprintf(szAppName, L"%d", iIndex);
	vector<wstring>vecContent;

	while (GetPrivateProfileString(szAppName, strKeyName, L"", szContent, ARRAYSIZE(szContent), szFilePath))
	{
		bool bIntRet = false;
		bool strRet = false;
		TConditionOrder tempOrder;
		memset(&tempOrder, 0, sizeof(TConditionOrder));
		vecContent.clear();
		SplitString(szContent, L',', vecContent);
		if (vecContent.size() == 5)
		{
			strcpy_s(tempOrder.InsertTime, UnicodeToANSI(vecContent[0]).c_str());
			strcpy_s(tempOrder.UserNo, UnicodeToANSI(vecContent[1]).c_str());
			strcpy_s(tempOrder.TimeOut, UnicodeToANSI(vecContent[2]).c_str());
			strcpy_s(tempOrder.ContractId, UnicodeToANSI(vecContent[3]).c_str());
			strcpy_s(tempOrder.Sign, UnicodeToANSI(vecContent[4]).c_str());
			strRet = true;
		}

		if (strRet && GetPrivateProfileString(szAppName, intKeyName, L"", szContent, ARRAYSIZE(szContent), szFilePath))
		{
			vecContent.clear();
			SplitString(szContent, L',', vecContent);
			if (vecContent.size() == 14)
			{
				tempOrder.OrderType = _wtoi(vecContent[0].c_str());
				tempOrder.iTimeOut = _wtoi(vecContent[1].c_str());
				
				if (tempOrder.OrderType==cotTime && tempOrder.iTimeOut < time(NULL)) //如果是时间条件单，且时间已过期
				{
					iIndex++;
					wsprintf(szAppName, L"%d", iIndex);
					continue;
				}
				tempOrder.Direct = _wtoi(vecContent[2].c_str());
				tempOrder.Offset = _wtoi(vecContent[3].c_str());
				tempOrder.iQty = _wtoi(vecContent[4].c_str());
				tempOrder.PriceType = _wtoi(vecContent[5].c_str());
				tempOrder.TriggerCondition = _wtoi(vecContent[6].c_str());
				tempOrder.Precision = _wtoi(vecContent[7].c_str());
				tempOrder.StopType = _wtoi(vecContent[8].c_str());
				tempOrder.DrawBackPoint = _wtoi(vecContent[9].c_str());
				tempOrder.LossPoint = _wtoi(vecContent[10].c_str());
				tempOrder.ProfitPoint = _wtoi(vecContent[11].c_str());
				tempOrder.bHasSendStopLP = _wtoi(vecContent[12].c_str());
				tempOrder.SendToServer = _wtoi(vecContent[13].c_str());

				bIntRet = true;
			}
		}
		if (bIntRet &&GetPrivateProfileString(szAppName, doubleKeyName, L"", szContent, ARRAYSIZE(szContent), szFilePath))
		{
			vecContent.clear();
			SplitString(szContent, L',', vecContent);
			if (vecContent.size() == 5)
			{
				tempOrder.ConPrice = _wtof(vecContent[0].c_str());
				tempOrder.Price = _wtof(vecContent[1].c_str());
				tempOrder.dMinChangePrice = _wtof(vecContent[2].c_str());
				tempOrder.LossPrice = _wtof(vecContent[3].c_str());
				tempOrder.ProfitPrice = _wtof(vecContent[4].c_str());

				TConditionOrder *pOrder = new TConditionOrder;
				memcpy(pOrder, &tempOrder, sizeof(TConditionOrder));
				pOrder->RunState = rsRun;
				pOrder->Triggered = false;

				SContract * contract[1] = {};
				g_pStarApi->GetContractData("", pOrder->ContractId, contract, 1, false);
				pOrder->pContract = contract[0];
				if (pOrder->pContract)
					m_vecCondtionOrder.push_back(pOrder);
			}
		}
		iIndex++;
		wsprintf(szAppName, L"%d", iIndex);
	}

	for (int i = 0; i < iIndex; i++)
	{
		wsprintf(szAppName, L"%d", i);
		WritePrivateProfileString(szAppName, NULL, NULL, szFilePath);
	}
	//TDataContainer stParOrder;
	//g_pTradeData->GetParOrder(stParOrder);
	//for (int i = 0; i < stParOrder.size(); i++)
	//{
	//	const TOrder* pParOrder = (const TOrder*)stParOrder.at(i);
	//	GetOrderFromParOrder(pParOrder);
	//}

	TDataContainer stOrders;
	g_pTradeData->GetAllOrder(stOrders);
	for (int i = 0; i < stOrders.size(); i++)
	{
		const TOrder* pTempOrder = (const TOrder*)stOrders.at(i);
		GetOrderFromOrder(pTempOrder);
	}

}

TConditionOrder * TConOrderModel::DealTradeOrder(const TOrder * pTradeOrder)
{
	if (pTradeOrder->StrategyType == stPreOrder || pTradeOrder->StrategyType == stAutoOrder || pTradeOrder->StrategyType == stCondition)
	{
		SContract *contract[1] = {};
		SContractNoType contractNo = {};
		CConvert::TradeContractToQContractNoType(*pTradeOrder, contractNo);
		g_pStarApi->GetContractData("", contractNo, contract, 1, false);

		if (!contract[0])
		{
			return nullptr;
		}

		TConditionOrder *pOrder = new TConditionOrder;
		pOrder->pContract = contract[0];
		strcpy_s(pOrder->InsertTime, pTradeOrder->InsertDateTime);
		strcpy_s(pOrder->UserNo, pTradeOrder->UserNo);
		strcpy_s(pOrder->Sign, pTradeOrder->Sign);

		strcpy_s(pOrder->TimeOut, "");
		pOrder->iTimeOut = 0;
		pOrder->ConPrice = 0;
		pOrder->Precision = 0;
		pOrder->Price = pTradeOrder->OrderPrice;
		const SCommodity* pCommodity = g_pTradeData->GetCommodity(pTradeOrder);
		if (pCommodity)
		{
			pOrder->Precision = pCommodity->PricePrec;
		}
		if (pTradeOrder->StrategyType == stCondition)
		{
			pOrder->ConPrice = pTradeOrder->TriggerPrice;
			pOrder->TriggerCondition = pTradeOrder->TriggerCondition;
			pOrder->OrderType = cotPriceServer;
			pOrder->SendToServer = true;
		}
		else if (pTradeOrder->StrategyType == stPreOrder)
		{
			pOrder->OrderType = cotPreOrder;
		}
		else if (pTradeOrder->StrategyType == stAutoOrder)
		{
			pOrder->OrderType = cotAutoOrder;
		}
		pOrder->Direct = pTradeOrder->Direct;
		pOrder->Offset = pTradeOrder->Offset;
		pOrder->iQty = pTradeOrder->OrderQty;
		pOrder->PriceType = ptSpecifiedPrice;

		pOrder->StopType = TConditionOrderFrm::stopNone;
		pOrder->DrawBackPoint = 0;
		pOrder->LossPoint = 0;
		pOrder->ProfitPoint = 0;
		pOrder->bHasSendStopLP = false;

		pOrder->OrderID = pTradeOrder->OrderID;
		return pOrder;
	}
	return nullptr;
}

//从委托中找出已经条件单，保存到对应容器中
void TConOrderModel::GetOrderFromOrder(const TOrder * pParOrder)
{
	if (pParOrder->StrategyType == stPreOrder || pParOrder->StrategyType == stAutoOrder || pParOrder->StrategyType == stCondition)
	{
		SContract *contract[1] = {};
		SContractNoType contractNo = {};
		CConvert::TradeContractToQContractNoType(*pParOrder, contractNo);
		g_pStarApi->GetContractData("", contractNo, contract, 1, false);

		if (!contract[0])
		{
			return;
		}

		TConditionOrder *pOrder = new TConditionOrder;
		pOrder->pContract = contract[0];
		strcpy_s(pOrder->InsertTime, pParOrder->InsertDateTime);
		strcpy_s(pOrder->UserNo, pParOrder->UserNo);
		strcpy_s(pOrder->Sign, pParOrder->Sign);

		strcpy_s(pOrder->TimeOut, "");
		pOrder->iTimeOut = 0;
		pOrder->ConPrice = 0;
		pOrder->Precision = 0;
		pOrder->Price = pParOrder->OrderPrice;
		const SCommodity* pCommodity = g_pTradeData->GetCommodity(pParOrder);
		if (pCommodity)
		{
			pOrder->Precision = pCommodity->PricePrec;
		}
		if (pParOrder->StrategyType == stCondition)
		{
			pOrder->ConPrice = pParOrder->TriggerPrice;
			pOrder->TriggerCondition = pParOrder->TriggerCondition;
			pOrder->OrderType = cotPriceServer;
			pOrder->SendToServer = true;
		}
		else if (pParOrder->StrategyType == stPreOrder)
		{
			pOrder->OrderType = cotPreOrder;
		}
		else if (pParOrder->StrategyType == stAutoOrder)
		{
			pOrder->OrderType = cotAutoOrder;
		}
		pOrder->Direct = pParOrder->Direct;
		pOrder->Offset = pParOrder->Offset;
		pOrder->iQty = pParOrder->OrderQty;
		pOrder->PriceType = ptSpecifiedPrice;

		pOrder->StopType = TConditionOrderFrm::stopNone;
		pOrder->DrawBackPoint = 0;
		pOrder->LossPoint = 0;
		pOrder->ProfitPoint = 0;
		pOrder->bHasSendStopLP = false;

		pOrder->OrderID = pParOrder->OrderID;

		//如果是条件单，且满足以下这几种状态，说明已经触发
		if (pParOrder->OrderState == osPartTriggered || pParOrder->OrderState == osFillTriggered || pParOrder->OrderState == osQueued 
			|| pParOrder->OrderState == osFilled)
		{
			pOrder->RunState = rsTriggered;
			pOrder->Triggered = true;
			
			char szLog[200] = {};
			sprintf_s(szLog, "trigger contract:%s, orderNo:%s, orderID:%d\r\n", contract[0]->ContractNo, pParOrder->OrderNo, pParOrder->OrderID);
			OutputDebugStringA(szLog);

			m_vecTriggeredConOrder.push_back(pOrder);
		}
		else if (pParOrder->OrderState == osTriggering)
		{
			pOrder->RunState = rsRun;
			pOrder->Triggered = false;

			char szLog[200] = {};
			sprintf_s(szLog, "not trigger contract:%s, orderNo:%s, orderID:%d\r\n", contract[0]->ContractNo, pParOrder->OrderNo, pParOrder->OrderID);
			OutputDebugStringA(szLog);

			m_vecCondtionOrder.push_back(pOrder);
		}
	}
}

void TConOrderModel::GetOrderFromParOrder(const TOrder* pParOrder)
{
	GetOrderFromOrder(pParOrder);
	//TConditionOrder * pOrder = DealTradeOrder(pParOrder);
	//if (pOrder)
	//{
	//	pOrder->RunState = rsRun;
	//	pOrder->Triggered = false;
	//	m_vecCondtionOrder.push_back(pOrder);
	//}
	//if (pParOrder->StrategyType == stPreOrder || pParOrder->StrategyType == stAutoOrder || pParOrder->StrategyType == stCondition)
	//{
	//	SContract *contract[1] = {};
	//	SContractNoType contractNo = {};
	//	CConvert::TradeContractToQContractNoType(*pParOrder, contractNo);
	//	g_pStarApi->GetContractData("", contractNo, contract, 1, false);

	//	if (!contract[0])
	//	{
	//		return;
	//	}

	//	TConditionOrder *pOrder = new TConditionOrder;
	//	pOrder->pContract = contract[0];
	//	strcpy_s(pOrder->InsertTime, pParOrder->InsertDateTime);
	//	strcpy_s(pOrder->UserNo, pParOrder->UserNo);
	//	strcpy_s(pOrder->Sign, pParOrder->Sign);
	//	pOrder->RunState = rsRun;
	//	pOrder->Triggered = false;

	//	strcpy_s(pOrder->TimeOut, "");
	//	pOrder->iTimeOut = 0;
	//	pOrder->ConPrice = 0;
	//	pOrder->Precision = 0;
	//	pOrder->Price = pParOrder->OrderPrice;
	//	const SCommodity* pCommodity = g_pTradeData->GetCommodity(pParOrder);
	//	if (pCommodity)
	//	{
	//		pOrder->Precision = pCommodity->PricePrec;
	//	}
	//	if (pParOrder->StrategyType == stCondition)
	//	{
	//		pOrder->ConPrice = pParOrder->TriggerPrice;
	//		pOrder->TriggerCondition = pParOrder->TriggerCondition;
	//		pOrder->OrderType = cotPriceServer;
	//		pOrder->SendToServer = true;
	//	}
	//	else if (pParOrder->StrategyType == stPreOrder)
	//	{
	//		pOrder->OrderType = cotPreOrder;
	//	}
	//	else if (pParOrder->StrategyType == stAutoOrder)
	//	{
	//		pOrder->OrderType = cotAutoOrder;
	//	}
	//	pOrder->Direct = pParOrder->Direct;
	//	pOrder->Offset = pParOrder->Offset;
	//	pOrder->iQty = pParOrder->OrderQty;
	//	pOrder->PriceType = ptSpecifiedPrice;

	//	pOrder->StopType = TConditionOrderFrm::stopNone;
	//	pOrder->DrawBackPoint = 0;
	//	pOrder->LossPoint = 0;
	//	pOrder->ProfitPoint = 0;
	//	pOrder->bHasSendStopLP = false;

	//	pOrder->OrderID = pParOrder->OrderID;
	//	m_vecCondtionOrder.push_back(pOrder);
	//}
}

bool TConOrderModel::OnParOrder(const TOrder* pParOrder)
{
	if (pParOrder&&pParOrder->OrderState == osTriggering &&
		(pParOrder->StrategyType == stPreOrder || pParOrder->StrategyType == stAutoOrder || pParOrder->StrategyType == stCondition))
	{
		for (int i = 0; i < m_vecCondtionOrder.size(); i++)
		{
			TConditionOrder* pOrder = m_vecCondtionOrder[i];
			if (pOrder->OrderID == pParOrder->OrderID)
				return false;
		}
		GetOrderFromParOrder(pParOrder);
		return true;
	}
	return false;
}