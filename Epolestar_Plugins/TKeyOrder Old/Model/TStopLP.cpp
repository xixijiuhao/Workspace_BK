#include "PreInclude.h"
#include "TStopLP.h"

extern IStarApi* g_pStarApi;
extern IStopLP * g_pStopLP;
extern TGlobalModel * g_pGlobalModel;


unsigned int TStopLP::staticKeyID = 0;

TStopLP::TStopLP()
{
	CreateFrm(L"StopLPforThreeKey", NULL, 0, 0);
}

TStopLP::~TStopLP()
{
	DelSub();
}

LRESULT TStopLP::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case SSWM_STOPLP_ONQUOTE:
		DealQuote(lParam);
		return PROCESSED;
	case SSWM_STOPLP_ATTACH:
		DealAttach(wParam,lParam);
		return PROCESSED;
	case SSWM_STOPLP_INSERT_ORDER:
		DealInsertOrder(wParam, lParam);
		return PROCESSED;
	case SSWM_STOPLP_DELETE_ORDER:
		DealDeleteOrder(wParam,lParam);
		return PROCESSED;
	case SSWM_STOPLP_MODIFY_ORDER:
		DealModifyOrder(wParam, lParam);
		return PROCESSED;
	case SSWM_STOPLP_TRIG_ORDER:
		DealTrigOrder(wParam, lParam);
		return PROCESSED;
	case SSWM_STOPLP_ONORDER:
		DealOnOrderCallBack(wParam);
		return PROCESSED;
	case SSWM_STOPLP_ONLIQUIDATE:
		DealOnLiquidateCallBack(wParam);
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}

void TStopLP::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
	if (quote)
	{
		memcpy_s(&m_strRawQuoteData, sizeof(SQuoteUpdate), quote, sizeof(SQuoteUpdate));
		memcpy_s(&m_quoteContract, sizeof(SContract), cont, sizeof(SContract));
		PostMessage(m_Hwnd, SSWM_STOPLP_ONQUOTE, 0, (LPARAM)&m_strRawQuoteData);
	}
}

void TStopLP::OnDataChange(DataType type, TStopLPitem*pdata)
{
	if (m_setpObs.size() == 0)return;
	std::set<IStopLPListener*>::iterator iter = m_setpObs.begin();
	while (iter != m_setpObs.end())
	{
		(*iter)->Update(type, pdata);
		iter++;
	}
}

//订阅止损单
void __cdecl TStopLP::Attach(IStopLPListener *pOb)
{
	PostMessage(m_Hwnd, SSWM_STOPLP_ATTACH, 0, (WPARAM)pOb);

}
//取消订阅
void __cdecl TStopLP::Detach(IStopLPListener *pOb)
{
	PostMessage(m_Hwnd, SSWM_STOPLP_ATTACH, 1, (WPARAM)pOb);
}

unsigned int __cdecl TStopLP::InsertLPOrder(const TStopLPInsertItem *item)
{
	if (item == nullptr)return 0;
	TStopLPitem i;
	memcpy_s(&i, sizeof(TStopLPInsertItem), item, sizeof(TStopLPInsertItem));
	i.keyID = staticKeyID++;
	i.state = osGeneral;
	i.TriggerCount = 0;
	i.LastTirgger = false;
	switch (i.orderType)
	{
	case otLoss:
	{
		TStopLossOrder* porder = new TStopLossOrder(&i);
		PostMessage(m_Hwnd, SSWM_STOPLP_INSERT_ORDER, (WPARAM)otLoss, (LPARAM)porder);
		break;
	}
	case otProfit:
	{
		TStopProfitOrder* porder = new TStopProfitOrder(&i);
		PostMessage(m_Hwnd, SSWM_STOPLP_INSERT_ORDER, (WPARAM)otProfit, (LPARAM)porder);
		break;
	}
	case otFloat:
	{
		TFloatStopLOrder* porder = new TFloatStopLOrder(&i);
		PostMessage(m_Hwnd, SSWM_STOPLP_INSERT_ORDER, (WPARAM)otFloat, (LPARAM)porder);
		break;
	}
	case otBreakEven:
	{
		TBreakEvenOrder* porder = new TBreakEvenOrder(&i);
		PostMessage(m_Hwnd, SSWM_STOPLP_INSERT_ORDER, (WPARAM)otBreakEven, (LPARAM)porder);
		break;
	}
	default:
		break;
	}
	return i.keyID;
}

void __cdecl TStopLP::DeleteLPOrder(const unsigned int KeyID)
{
	PostMessage(m_Hwnd, SSWM_STOPLP_DELETE_ORDER, 0, (LPARAM)KeyID);
}

//修改止损单
unsigned int __cdecl TStopLP::ModifyLPOrder(const TStopLPitem *item)
{
	switch (item->orderType)
	{
	case otLoss:
	{
		TStopLossOrder* porder = new TStopLossOrder(item);
		PostMessage(m_Hwnd, SSWM_STOPLP_MODIFY_ORDER, (WPARAM)item->keyID, (LPARAM)porder);
		break;
	}
	case otProfit:
	{
		TStopProfitOrder* porder = new TStopProfitOrder(item);
		PostMessage(m_Hwnd, SSWM_STOPLP_MODIFY_ORDER, (WPARAM)item->keyID, (LPARAM)porder);
		break;
	}
	case otFloat:
	{
		TFloatStopLOrder* porder = new TFloatStopLOrder(item);
		PostMessage(m_Hwnd, SSWM_STOPLP_MODIFY_ORDER, (WPARAM)item->keyID, (LPARAM)porder);
		break;
	}
	case otBreakEven:
	{
		TBreakEvenOrder* porder = new TBreakEvenOrder(item);
		PostMessage(m_Hwnd, SSWM_STOPLP_MODIFY_ORDER, (WPARAM)item->keyID, (LPARAM)porder);
		break;
	}
	default:
		break;
	}
	return item->keyID;
}

void __cdecl TStopLP::TriggerLPOrder(const unsigned int KeyID, bool Trigger)
{
	PostMessage(m_Hwnd, SSWM_STOPLP_TRIG_ORDER, (WPARAM)KeyID, (LPARAM)Trigger);
}

unsigned int __cdecl TStopLP::OpenLPOrder(const AutoStopInfo *item, const TSendOrder * stSendOrder)
{
	if (stSendOrder->Offset != oOpen)
		return -1;
	TSendOrder stOrder;
	memcpy_s(&stOrder, sizeof(TSendOrder), stSendOrder, sizeof(TSendOrder));
	CConvert::newGUID(stOrder.Remark);
	if (item->iOrderType == AutoStopType)
	{
		char ContractKey[150] = { 0 };
		GetContractKey(ContractKey, stSendOrder, sizeof(ContractKey));
		if (IsAutoStop(ContractKey))
			InsertRemarkToStopInfo(stOrder.Remark, item);
	}
	else if (item->iOrderType == StopOpenType)
	{
		if (IsStopOpen(item))
			InsertRemarkToStopInfo(stOrder.Remark, item);
	}
	if (g_pTradeApi)
		g_pTradeApi->InsertOrder(stOrder);
	return 0;
}

void __cdecl TStopLP::DeleteAllLPOrder()
{
	PostMessage(m_Hwnd, SSWM_STOPLP_DELETE_ORDER, 1, 0);

}


void __cdecl TStopLP::GetALLStopLPOrder(IDataContainer &Datas, const char * UserNo, const char* SCode, TDirect direct, int NoTrig)
{
	bool bUserDiff = (UserNo == "" || UserNo == nullptr) ? true : false;
	bool bCodeDiff = (SCode == "" || SCode == nullptr) ? true : false;
	bool bDirectDiff = (dBoth == direct);
	bool bTrigDiff = (0 == NoTrig);
	auto iter = m_Data.begin();
	while (iter != m_Data.end())
	{
		if ((bUserDiff || !strcmp(UserNo, (*iter)->GetOrder()->userNo)) && (bCodeDiff || !strcmp(SCode, (*iter)->GetOrder()->code))
			&& (bDirectDiff || (*iter)->GetOrder()->direct == direct) 
			&& (bTrigDiff || ((NoTrig < 0) && (*iter)->GetOrder()->state == osTrig) || ((NoTrig > 0) && (*iter)->GetOrder()->state != osTrig)))
		{
			Datas.push_back((*iter)->GetOrder());
		}
		iter++;
	}
}

void __cdecl TStopLP::GetSingleStopLPOrder(const TStopLPitem* pItem, const unsigned int KeyID)
{
	auto iter = m_Data.begin();
	while (iter != m_Data.end())
	{
		if ((*iter)->GetOrder()->keyID == KeyID)
		{
			pItem = (*iter)->GetOrder();
			return;
		}
		iter++;
	}
	pItem = nullptr;//若没找到则返回空；
}

int TStopLP::DealAttach(WPARAM wParam,LPARAM lParam)
{
	IStopLPListener* pOb = (IStopLPListener*)lParam;
	if (pOb == NULL)return 0;
	if (wParam == 0)
	{
		m_setpObs.insert(pOb);
		return m_setpObs.size();
	}
	else
	{
		m_setpObs.erase(pOb);
		return m_setpObs.size();
	}
}

void TStopLP::DealQuote(LPARAM lParam)
{
	if (m_quoteData.size() == 0)
	{
		return;
	}
	SContractNoType sztext;
	std::vector<unsigned int> keyIDs;
//	strcpy_s(sztext, quote->ContractId);
	strcpy_s(sztext, m_quoteContract.ContractNo);

	Range range = m_quoteData.equal_range(sztext);
	for (CIT iter = range.first; iter != range.second; iter++)
	{
		if (DealSingleOrder(sztext, iter->second))
		{
			keyIDs.push_back(iter->second);
		}
	}
	for (unsigned int i = 0; i < keyIDs.size(); i++)
	{
		DelSub(sztext, keyIDs[i]);
		//处理关联单子
		DealLinkedKeyIDs(keyIDs[i]);//这里有个问题，就是关联的两个单子万一同时触发。。那么还是会发出去两个单子
	}
}

void TStopLP::DealInsertOrder(WPARAM wParam, LPARAM lParam)
{
	AbstractOrder* pOrder = (AbstractOrder*)lParam;
	m_Data.push_back(pOrder);
	OnDataChange(Add_Type, pOrder->GetOrder());

	SContractNoType sQuote = {};
	CConvert::StrToQContractNoType(pOrder->GetOrder()->code, sQuote);
	AddSub(sQuote, pOrder->GetOrder()->keyID);//监视map增加一条


	char PositionKey[150] = { 0 };
	TDirect direct = dNone;
	if (pOrder->GetOrder()->direct == dBuy)
		direct = dSell;
	else
		direct = dBuy;
	GetPositionKey(PositionKey, pOrder->GetOrder()->code, direct, sizeof(PositionKey));
	InsertPositionToKeyId(PositionKey, pOrder->GetOrder()->keyID);//持仓和KeyID关联增加一条

}

void TStopLP::DealDeleteOrder(WPARAM wParam,LPARAM lParam)
{
	if (wParam == 0)
	{
		auto iter = m_Data.begin();
		while (iter != m_Data.end())
		{
			if ((*iter)->GetKeyID() == (unsigned int)lParam)
			{

				SContractNoType sQuote = { 0 };
				CConvert::StrToQContractNoType((*iter)->GetOrder()->code, sQuote);
				DelSub(sQuote, (unsigned int)lParam);

				TStopLPitem* pitem = (*iter)->GetOrder();
				pitem->state = osDeleted;

				OnDataChange(Chg_Type, pitem);//要通知别人这条记录删掉了

				char PositionKey[150] = { 0 };
				TDirect direct = dNone;
				if (pitem->direct == dBuy)
					direct = dSell;
				else
					direct = dBuy;
				GetPositionKey(PositionKey, pitem->code, direct, sizeof(PositionKey));
				PositionKeyRange range = m_PositionToKeyId.equal_range(PositionKey);
				auto iterPos = range.first;
				for (; iterPos != range.second; iterPos++)
				{
					if (iterPos->second == pitem->keyID)
						break;
				}
				if (pitem->state == osDeleted)
					m_PositionToKeyId.erase(iterPos);//还要删除持仓关联KeyID中相关的那一条

				delete *iter;//是new出来的指针，也得把这个删掉
				m_Data.erase(iter);
				break;
			}
			iter++;
		}
	}
	else
	{
		OnDataChange(Del_Type, NULL);
		m_PositionToKeyId.clear();
		m_Data.clear();
		DelSub();//监视map全部删除
	}
}

//修改消息响应
//此处应该添加控制，能修改的部分进行替换，不能修改的部分保留原来的.（之前的是界面进行输入控制，不妥）
//注意：lParam传入的指针是new 出来的，不论怎样都要delete掉
//能修改的字段有：手数/触发价格/触发模式/委托价格类型/止损浮动价差/保本价差/超价点数/触发次数/状态
void TStopLP::DealModifyOrder(WPARAM wParam, LPARAM lParam)
{
	AbstractOrder* pOrder = (AbstractOrder*)lParam;
	TStopLPitem* pitem = pOrder->GetOrder();
	unsigned int KeyID = (unsigned int)wParam;
	auto iter = m_Data.begin();
	while (iter!=m_Data.end())
	{
		if ((*iter)->GetKeyID() == KeyID)
		{
			if ((*iter)->GetOrder()->orderType != pitem->orderType)//如果是修改成其他种类的单子，则需要整体覆盖
			{
				delete *iter;
				*iter = pOrder;
				OnDataChange(ChO_Type, pitem);//通知这条记录变动
				return;
			}
			(*iter)->GetOrder()->Qty = pitem->Qty;//uint
			(*iter)->GetOrder()->triggerPrice = pitem->triggerPrice;//double
			(*iter)->GetOrder()->trigBase = pitem->trigBase;//char
			(*iter)->GetOrder()->orderPriceType = pitem->orderPriceType;//int
			(*iter)->GetOrder()->floatPoint = pitem->floatPoint;//double
			(*iter)->GetOrder()->breakEvenPoint = pitem->breakEvenPoint;//double
			(*iter)->GetOrder()->point = pitem->point;//double
			(*iter)->GetOrder()->TriggerCount = pitem->TriggerCount;//uint
			(*iter)->GetOrder()->state = pitem->state;//char
			OnDataChange(Chg_Type, (*iter)->GetOrder());//通知这条记录变动
			break;
		}
		iter++;
	}
	if (iter == m_Data.end())
	{
		//如果数到最后一个，都没有修改的，则放弃修改。(原来是就变成添加，不妥)
	}
	delete pOrder;
}

void TStopLP::DealTrigOrder(WPARAM wParam, LPARAM lParam)
{
	unsigned int KeyID = (uint)wParam;
	bool Trig = (bool)lParam;
	auto iter = m_Data.begin();
	while (iter != m_Data.end())
	{
		if ((*iter)->GetKeyID() == KeyID)
		{
			if (Trig)
			{
				(*iter)->GetOrder()->TriggerCount++;
				(*iter)->GetOrder()->LastTirgger = true;
			}
			else
			{
				(*iter)->GetOrder()->TriggerCount = 0;
				(*iter)->GetOrder()->LastTirgger = false;
			}
			OnDataChange(Trg_Type, (*iter)->GetOrder());
			break;
		}
		iter++;
	}
}

void TStopLP::DealOnOrderCallBack(WPARAM wParam)
{
	const TOrder* pst = (const TOrder*)wParam;
	if (!pst)
		return;
	//已撤余单或者完全成交 发送止损单;
	std::map<std::string, AutoStopInfo>::iterator iter = m_RemarkToStopInfo.find(pst->Remark);
	if (iter != m_RemarkToStopInfo.end())
	{
		if (pst->OrderState == osPartCanceled || pst->OrderState == osFilled || pst->OrderState == osPartFailed)
		{
			iter->second.dMatchPrice = pst->MatchPrice;
			InsertOnOrderCallBackOrder(iter->second, pst);
			DeleteRemarkToStopInfo(pst->Remark);
		}
		else if (pst->OrderState == osCanceled || pst->OrderState == osFail)
		{
			DeleteRemarkToStopInfo(pst->Remark);
		}
	}
}

void TStopLP::DealOnLiquidateCallBack(WPARAM wParam)
{
	TLiquidate* pst = (TLiquidate*)wParam;
	if (!pst)
		return;
	std::vector<unsigned int> vecKeyId;
	GetDelKeyId(pst, vecKeyId);
	for (int icount = 0; icount < vecKeyId.size(); icount++)
	{
		for (int cnt = 0; cnt < m_Data.size(); cnt++)
		{
			if (m_Data[cnt]->GetKeyID() == vecKeyId[icount] && m_Data[cnt]->GetOrder()->state != osTrig)
			{
				DeleteLPOrder(vecKeyId[icount]);
			}
		}
	}		
}

bool TStopLP::DealSingleOrder(char(&contractID)[101], unsigned int i)
{
	AbstractOrder* pSingleorder = nullptr;
	TStopLPitem item;
	for (auto dataiter = m_Data.begin(); dataiter != m_Data.end(); dataiter++)
	{
		if (nullptr == (*dataiter))
		{
			continue;
		}
		if ((*dataiter)->GetKeyID() == i)
		{
			pSingleorder = *dataiter;
			memcpy_s(&item, sizeof(TStopLPitem), (*dataiter)->GetOrder(), sizeof(TStopLPitem));
			break;
		}
	}
	if (item.state == osGeneral || item.state == osBreak)
	{
		//价格
		int PriceType = ptLastPrice;
		double Price = 0.0;
		TContractKey ContractKey;
		memset(&ContractKey, 0, sizeof(ContractKey));
		CConvert::SContractNoTypeToTradeContract(item.sign, contractID, ContractKey);
		CConvert::GetPrice(&ContractKey, item.direct, PriceType, Price);

		SingleOrderQtyandParOrder(item, ContractKey);
		
		if (m_iOrderQty >= m_iPositionQty)
			return false;
		if (m_iDayPositionQty < m_iDayOrderQty)
			return false;

		//触发价格、买卖方向以及止损止盈的判断
		if (nullptr != pSingleorder)
		{
			if (pSingleorder->TriggerJudge(Price))
			{
				SendOrder(item);
				return true;
			}
		}
	}
	return false;
}

//
//可用持仓，上期所昨仓和今仓的判断，根据买卖方向不同做区分。
//
void TStopLP::SingleOrderQtyandParOrder(TStopLPitem& item,TContractKey& ContractKey)
{

	//持仓量
	uint	LongCount;
	uint	ShortCount;
	uint	DayLongCount;
	uint	DayShortCount;
	g_pTradeData->GetPositionVol(item.userNo, &ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	TOrderQty OrderQty;
	const TOrderQty* pOrderQty = g_pTradeData->GetParOrderQty(item.userNo, &ContractKey);
	if (nullptr == pOrderQty)
	{
		OrderQty.BuyQty = OrderQty.SellQty = OrderQty.BuyCoverQty = OrderQty.BuyCoverDayQty
			= OrderQty.SellCoverQty = OrderQty.SellCoverDayQty = 0;
	}
	else
	{
		OrderQty.BuyQty = pOrderQty->BuyQty;
		OrderQty.SellQty = pOrderQty->SellQty;
		OrderQty.SellCoverQty = pOrderQty->SellCoverQty;
		OrderQty.SellCoverDayQty = pOrderQty->SellCoverDayQty;
		OrderQty.BuyCoverQty = pOrderQty->BuyCoverQty;
		OrderQty.BuyCoverDayQty = pOrderQty->BuyCoverDayQty;
	}

	if (item.direct == dSell)
	{
		m_iOrderQty = OrderQty.SellCoverQty;
		m_iPositionQty = LongCount;
		if (strcmp(ContractKey.ExchangeNo, "SHFE") && strcmp(ContractKey.ExchangeNo, "ZCE") && strcmp(ContractKey.ExchangeNo, "DCE") && strcmp(ContractKey.ExchangeNo, "CFFEX"))//不是国内品种
		{
			m_iOrderQty = OrderQty.SellQty;
			m_iDayOrderQty = 0;
			m_iDayPositionQty = 0;
			return;
		}
		if (strcmp(ContractKey.ExchangeNo, "SHFE"))
		{
			m_iDayOrderQty = 0;
			m_iDayPositionQty = 0;
		}
		else
		{
			m_iDayOrderQty = OrderQty.SellCoverDayQty;
			m_iDayPositionQty = DayLongCount;
		}

	}
	else if (item.direct == dBuy)
	{
		m_iOrderQty = OrderQty.BuyCoverQty;
		m_iPositionQty = ShortCount;
		if (strcmp(ContractKey.ExchangeNo, "SHFE") && strcmp(ContractKey.ExchangeNo, "ZCE") && strcmp(ContractKey.ExchangeNo, "DCE") && strcmp(ContractKey.ExchangeNo, "CFFEX"))//不是国内品种
		{
			m_iOrderQty = OrderQty.BuyQty;
			m_iDayOrderQty = 0;
			m_iDayPositionQty = 0;
			return;
		}
		if (strcmp(ContractKey.ExchangeNo, "SHFE"))
		{
			m_iDayOrderQty = 0;
			m_iDayPositionQty = 0;
		}
		else
		{
			m_iDayOrderQty = OrderQty.BuyCoverDayQty;
			m_iDayPositionQty = DayShortCount;
		}
	}
}
//
//触发后的发单操作
//
void TStopLP::SendOrder(const TStopLPitem &item)
{
	TSendOrder storder;
	CConvert::DefaultSendOrder(storder);
	CConvert::TContractNoToTContractKey(item.code, storder);
	strcpy_s(storder.Sign, item.sign);
	strcpy_s(storder.UserNo, item.userNo);
	storder.Direct = item.direct;
	CConvert::GetPrice(&storder, storder.Direct, item.orderPriceType, storder.OrderPrice);

	if (m_iDayPositionQty == 0 && m_iDayOrderQty == 0)//不是上海单
	{
		if (m_iPositionQty - m_iOrderQty < item.Qty)//为防止持仓不足平的判断
		{
			storder.OrderQty = m_iPositionQty - m_iOrderQty;
		}
		else
		{
			storder.OrderQty = item.Qty;
		}
		storder.Offset = oCover;
		g_pTradeApi->InsertOrder(storder);
	}
	else//上海单
	{
		storder.Offset = oCoverT;
		if (m_iDayPositionQty - m_iDayOrderQty >= item.Qty)
		{
			storder.OrderQty = item.Qty;
			g_pTradeApi->InsertOrder(storder);
			return;
		}
		else
		{
			storder.OrderQty = m_iDayPositionQty - m_iDayOrderQty;
			g_pTradeApi->InsertOrder(storder);
			//昨仓可平
			int LastQty = m_iPositionQty - m_iDayPositionQty - (m_iOrderQty - m_iDayOrderQty);
			if (LastQty > 0)
			{
				storder.OrderQty = min(LastQty, item.Qty - (m_iDayPositionQty - m_iDayOrderQty));
				storder.Offset = oCover;
				g_pTradeApi->InsertOrder(storder);
			}
		}
	}

}


bool TStopLP::InsertLinkedKeyIDs(unsigned int ID1, unsigned int ID2)
{
	if (ID1 == ID2)
		return false;
	if ((m_stlLinkedKeyIDs.find(ID1) == m_stlLinkedKeyIDs.end()) && (m_stlLinkedKeyIDs.find(ID2) == m_stlLinkedKeyIDs.end()))
	{
		m_stlLinkedKeyIDs.insert(make_pair(ID1, ID2));
		m_stlLinkedKeyIDs.insert(make_pair(ID2, ID1));
		return true;
	}
	else
		return false;
}

void TStopLP::DealLinkedKeyIDs(unsigned int keyID)
{
	if (m_stlLinkedKeyIDs.size() == 0)return;
	std::map<unsigned int, unsigned int>::iterator iter = m_stlLinkedKeyIDs.find(keyID);
	if (iter != m_stlLinkedKeyIDs.end())
	{
		unsigned int keyID2 = iter->second;
		DeleteLPOrder(keyID2);//删除关联的另一条止损单
		m_stlLinkedKeyIDs.erase(keyID);
		m_stlLinkedKeyIDs.erase(keyID2);//删掉另一个KeyID
	}
}


void TStopLP::AddSub(char(&contractID)[101], unsigned int i)
{
//new	g_pQuoteApi->SubQuote(contractID, this);				//订阅行情
	g_pStarApi->SubQuote(&contractID,1, this);				//订阅行情

	m_quoteData.insert(make_pair(contractID, i));
}
void TStopLP::DelSub(char(&contractID)[101], unsigned int i)//删除某条监视
{
	Range range = m_quoteData.equal_range(contractID);
	for (CIT iter = range.first; iter != range.second; iter++)
	{
		if (iter->second == i)
		{
			if (++range.first == range.second)//(m_quoteData.count(iter->first)==1)
			{
				//g_pQuoteApi->UnsubQuote(contractID, this);
				//new
				g_pStarApi->UnsubQuote(&contractID,1, this);
			}
			m_quoteData.erase(iter);
			return;
		}
	}
}

void TStopLP::DelSub()//删除所有监视
{
	m_quoteData.clear();
	g_pStarApi->UnsubAllQuote(this);//取消所有订阅
}

void __cdecl TStopLP::OnLiquidate(const TLiquidate * pLiquidate)
{
	if (pLiquidate)
		PostMessage(m_Hwnd, SSWM_STOPLP_ONLIQUIDATE, (WPARAM)pLiquidate, 0);
}
void __cdecl TStopLP::OnOrder(const TOrder * pOrder)
{
	if (pOrder)
	{
		PostMessage(m_Hwnd, SSWM_STOPLP_ONORDER, (WPARAM)pOrder, 0);
	}
}


void TStopLP::GetContractKey(char* ContractKey, const TContractSingleKey* pst, int iSize)
{
	sprintf_s(ContractKey, iSize, "%s%c%s%s%s%c", pst->ExchangeNo, pst->CommodityType, pst->CommodityNo,
		pst->ContractNo, pst->StrikePrice, pst->OptionType);
}
void TStopLP::GetPositionKey(char* PositionKey, const char* scode, TDirect direct, int iSize)
{
	TContractKey pst;
	memset(&pst, 0, sizeof(TContractKey));
	CConvert::TContractNoToTContractKey(scode, pst);
	GetPositionKey(PositionKey, &pst, direct, iSize);
}
void TStopLP::GetPositionKey(char* PositionKey, TContractSingleKey* pContract, TDirect direct, int iSize)
{
	sprintf_s(PositionKey, iSize, "%s%c%s%s%s%c%c", pContract->ExchangeNo, pContract->CommodityType, pContract->CommodityNo, pContract->ContractNo,
		pContract->StrikePrice, pContract->OptionType, direct);
}

bool TStopLP::IsStopOpen(const AutoStopInfo* stInfo)
{
	if (stInfo->bFloat || stInfo->bLoss || stInfo->bProfit)
		return true;
	else
		return false;
}
bool TStopLP::IsAutoStop(char* ContractKey)
{
	std::map<std::string, stSLContractPriceMargin>::iterator iter = g_pGlobalModel->m_stStopLossConfig.stdmapContract.find(ContractKey);
	if (iter != g_pGlobalModel->m_stStopLossConfig.stdmapContract.end())
		return true;
	else
		return false;
}

void TStopLP::InsertRemarkToStopInfo(char* cRemark, const AutoStopInfo *stStopOrder)
{
	std::map<std::string, AutoStopInfo>::iterator iter = m_RemarkToStopInfo.find(cRemark);
	if (iter != m_RemarkToStopInfo.end())
	{
		m_RemarkToStopInfo.erase(iter);
		m_RemarkToStopInfo.insert(make_pair(cRemark, *stStopOrder));
	}
	else
	{
		m_RemarkToStopInfo.insert(make_pair(cRemark, *stStopOrder));
	}
}

bool TStopLP::DeleteRemarkToStopInfo(const char* cRemark)
{
	std::map<std::string, AutoStopInfo>::iterator iter = m_RemarkToStopInfo.find(cRemark);
	if (iter != m_RemarkToStopInfo.end())
	{
		m_RemarkToStopInfo.erase(iter);
		return false;
	}
	else
		return false;
}

void TStopLP::InsertOnOrderCallBackOrder(AutoStopInfo& stStop, const TOrder* pst)
{
	TStopLPInsertItem stStopOrder;
	DefaultStopOrder(pst, stStop, stStopOrder);
	if (stStop.iOrderType == AutoStopType)
	{
		SendAutoStop(stStop, pst, stStopOrder);
	}
	else
	{
		//根据stStop获取相关信息;
		SendOpenStop(stStop, pst, stStopOrder);
	}
}

void TStopLP::SendOpenStop(AutoStopInfo& stStop, const TOrder* pst, TStopLPInsertItem& stStopOrder)
{
	if (stStop.bLoss)
		AutoStopLoss(stStopOrder, stStop.dstopLoss);
	if (stStop.bProfit)
		AutoStopProfit(stStopOrder, stStop.dstopProfit);
	if (stStop.bFloat)
		AutoFloat(stStopOrder, stStop.dFloatPoint, pst->OrderPrice);
}

void TStopLP::SendAutoStop(AutoStopInfo& stStop, const TOrder* pst, TStopLPInsertItem& stStopOrder)
{
	double dLoss = 0.0;
	double dProfit = 0.0;
	double dBasePrice = 0.0;
	char ContractKey[150] = { 0 };
	GetContractKey(ContractKey, pst, sizeof(ContractKey));
	GetLossProfitPrice(ContractKey, dLoss, dProfit);
	(g_pGlobalModel->m_stStopLossConfig.iOpenAutoStopLossBasicPrice == 0) ? dBasePrice = stStop.dMatchPrice : dBasePrice = stStop.dCounterPrice;
	switch (g_pGlobalModel->m_stStopLossConfig.iDefaultTactic)
	{
	case 0:
		InsertLinkedKeyIDs(AutoStopLoss(stStopOrder, dLoss, dBasePrice), AutoStopProfit(stStopOrder, dProfit, dBasePrice));
		break;
	case 1:
		InsertLinkedKeyIDs(AutoStopLoss(stStopOrder, dLoss, dBasePrice), AutoBreakEven(stStopOrder, dProfit, dBasePrice));
		break;
	case 2:
		InsertLinkedKeyIDs(AutoFloat(stStopOrder, dLoss, dBasePrice), AutoBreakEven(stStopOrder, dProfit, dBasePrice));
		break;
	case 3:
		AutoFloat(stStopOrder, dLoss, dBasePrice);
		break;
	case 4:
		AutoStopLoss(stStopOrder, dLoss, dBasePrice);
		break;
	case 5:
		AutoStopProfit(stStopOrder, dProfit, dBasePrice);
		break;
	default:
		break;
	}
}

//============发单相关操作============//
unsigned int TStopLP::AutoStopLoss(TStopLPInsertItem &item, const double &stopLoss, const double &basePrice)
{
	item.orderType = otLoss;
	if (basePrice)
	{
		if (item.direct == dSell)
			item.triggerPrice = basePrice - stopLoss;
		else
			item.triggerPrice = basePrice + stopLoss;
	}
	else
		item.triggerPrice = stopLoss;

	item.orderPriceType = g_pGlobalModel->m_stStopLossConfig.iStopLossOrderPrice;
	return InsertLPOrder(&item);
}
unsigned int TStopLP::AutoStopProfit(TStopLPInsertItem &item, const double &stopProfit, const double &basePrice)
{
	item.orderType = otProfit;
	if (basePrice)
	{
		if (item.direct == dSell)
			item.triggerPrice = basePrice + stopProfit;
		else
			item.triggerPrice = basePrice - stopProfit;
	}
	else
		item.triggerPrice = stopProfit;
	item.orderPriceType = g_pGlobalModel->m_stStopLossConfig.iStopProfitOrderPrice;
	return InsertLPOrder(&item);
}
unsigned int TStopLP::AutoFloat(TStopLPInsertItem &item, const double &floatPiont, const double &basePrice)
{
	item.orderType = otFloat;
	if (item.direct == dSell)
		item.triggerPrice = basePrice - floatPiont;
	else
		item.triggerPrice = basePrice + floatPiont;
	item.floatPoint = floatPiont;
	item.orderPriceType = g_pGlobalModel->m_stStopLossConfig.iStopLossOrderPrice;
	return InsertLPOrder(&item);
}
unsigned int TStopLP::AutoBreakEven(TStopLPInsertItem &item, const double &BreakEvenPrice, const double &basePrice)
{
	item.orderType = otBreakEven;
	if (item.direct == dSell)
		item.triggerPrice = basePrice + BreakEvenPrice;
	else
		item.triggerPrice = basePrice - BreakEvenPrice;
	item.breakEvenPoint = BreakEvenPrice;
	item.orderPriceType = g_pGlobalModel->m_stStopLossConfig.iStopLossOrderPrice;
	return InsertLPOrder(&item);
}

void TStopLP::DefaultStopOrder(const TOrder* pst, const AutoStopInfo& stStop, TStopLPInsertItem& stStopOrder)
{
	memset(&stStopOrder, 0, sizeof(TStopLPInsertItem));
	strcpy_s(stStopOrder.sign, pst->Sign);
	strcpy_s(stStopOrder.userNo, pst->UserNo);
	if (pst->Direct == dBuy)
		stStopOrder.direct = dSell;
	else
		stStopOrder.direct = dBuy;

	sprintf_s(stStopOrder.code, "%s %s %s", pst->ExchangeNo, pst->CommodityNo, pst->ContractNo);
	(pst->OrderState == osPartCanceled) ? stStopOrder.Qty = pst->OrderQty - pst->MatchQty : stStopOrder.Qty = pst->OrderQty;

	stStopOrder.MaxCount = g_pGlobalModel->m_stStopLossConfig.iTriggerCount;
	stStopOrder.trigBase = tmbLatest;
}

void TStopLP::GetLossProfitPrice(char* ContractKey, double& dLoss, double& dProfit)
{
	std::map<std::string, stSLContractPriceMargin>::iterator iter = g_pGlobalModel->m_stStopLossConfig.stdmapContract.find(ContractKey);
	if (iter != g_pGlobalModel->m_stStopLossConfig.stdmapContract.end())
	{
		dLoss = iter->second.dMinChangePrice * iter->second.iStopLossPriceGap;
		dProfit = iter->second.dMinChangePrice * iter->second.iStopProfitPriceGap;
	}
}

void TStopLP::InsertPositionToKeyId(char* PositionKey, unsigned int KeyId)
{
	m_PositionToKeyId.insert(make_pair(PositionKey, KeyId));
}

void TStopLP::GetDelKeyId(TLiquidate* pst, vector<unsigned int>& vecKeyId)
{
	char PositionKey[150] = { 0 };
	TDirect direct = dNone;
	if (pst->CoverDirect == dBuy)
		direct = dSell;
	else
		direct = dBuy;
	GetPositionKey(PositionKey, pst, direct, sizeof(PositionKey));
	PositionKeyRange range = m_PositionToKeyId.equal_range(PositionKey);
	for (std::multimap<std::string, unsigned int>::const_iterator iter = range.first; iter != range.second;)
	{
		TDataContainer TotalPos;
		if (g_pTradeData)
			g_pTradeData->GetAllPositionTotal(TotalPos);
		if (TotalPos.size())
		{
			bool IsPosition = false;
			for (int i = 0; i < TotalPos.size(); i++)
			{
				TPosition* p = (TPosition*)TotalPos.at(i);
				GetPositionKey(PositionKey, p, p->Direct, sizeof(PositionKey));
				if (strcmp(iter->first.c_str(), PositionKey) == 0)
				{
					IsPosition = true;
					iter++;
					break;
				}
			}
			if (!IsPosition)
			{
				vecKeyId.push_back(iter->second);
				iter++;
			}
		}
		else
		{
			vecKeyId.push_back(iter->second);
			iter++;
		}
	}
}

bool TStopLossOrder::TriggerJudge(const double &Price)
{
	if ((m_order.direct == dSell&&Price <= m_order.triggerPrice) || (m_order.direct == dBuy&&Price >= m_order.triggerPrice))
	{
		if (m_order.TriggerCount + 1 < max(m_order.MaxCount, 1))//触发所需次数判断
		{
			g_pStopLP->TriggerLPOrder(m_order.keyID, true);
			return false;
		}
		else
		{
			//
			TStopLPitem itemtemp;
			memcpy_s(&itemtemp, sizeof(TStopLPitem), &m_order, sizeof(TStopLPitem));
			itemtemp.state = osTrig;
			itemtemp.TriggerCount++;
			g_pStopLP->ModifyLPOrder(&itemtemp);
			//trigger止损发单
			return true;
		}
	}
	else    //不连续，中断了
	{
		if (true == m_order.LastTirgger)
		{
			g_pStopLP->TriggerLPOrder(m_order.keyID, false);
		}
		return false;
	}
}

bool TStopProfitOrder::TriggerJudge(const double &Price)
{
	if ((m_order.direct == dSell&&Price >= m_order.triggerPrice) || (m_order.direct == dBuy&&Price <= m_order.triggerPrice))
	{
		if (m_order.TriggerCount + 1 < max(m_order.MaxCount, 1))//触发所需连续次数判断
		{
			g_pStopLP->TriggerLPOrder(m_order.keyID, true);
			return false;
		}
		else
		{
			//
			TStopLPitem itemtemp;
			memcpy_s(&itemtemp, sizeof(TStopLPitem), &m_order, sizeof(TStopLPitem));
			itemtemp.state = osTrig;
			itemtemp.TriggerCount++;
			g_pStopLP->ModifyLPOrder(&itemtemp);
			//trigger止盈发单
			return true;
		}

	}
	else    //不连续，中断了
	{
		if (true == m_order.LastTirgger)
		{
			g_pStopLP->TriggerLPOrder(m_order.keyID, false);
		}
		return false;
	}
}

bool TFloatStopLOrder::TriggerJudge(const double &Price)
{
	if ((m_order.direct == dSell&&Price <= m_order.triggerPrice) || (m_order.direct == dBuy&&Price >= m_order.triggerPrice))
	{
		if (m_order.TriggerCount + 1 < max(m_order.MaxCount, 1))//触发所需次数判断
		{
			g_pStopLP->TriggerLPOrder(m_order.keyID, true);
			return false;
		}
		else
		{
			//
			TStopLPitem itemtemp;
			memcpy_s(&itemtemp, sizeof(TStopLPitem), &m_order, sizeof(TStopLPitem));
			itemtemp.state = osTrig;
			itemtemp.TriggerCount++;
			g_pStopLP->ModifyLPOrder(&itemtemp);
			//trigger止损发单
			return true;
		}
	}
	else if (m_order.direct == dSell && Price > m_order.triggerPrice + m_order.floatPoint)//止损价跟踪
	{
		TStopLPitem itemtemp;
		memcpy_s(&itemtemp, sizeof(TStopLPitem), &m_order, sizeof(TStopLPitem));
		itemtemp.triggerPrice = Price - itemtemp.floatPoint;
		itemtemp.TriggerCount = 0;
		g_pStopLP->ModifyLPOrder(&itemtemp);
		return false;
	}
	else if (m_order.direct == dBuy && Price < m_order.triggerPrice - m_order.floatPoint)//止损价跟踪
	{
		TStopLPitem itemtemp;
		memcpy_s(&itemtemp, sizeof(TStopLPitem), &m_order, sizeof(TStopLPitem));
		itemtemp.triggerPrice = Price + itemtemp.floatPoint;
		itemtemp.TriggerCount = 0;
		g_pStopLP->ModifyLPOrder(&itemtemp);
		return false;
	}
	else
	{
		if (true == m_order.LastTirgger)
		{
			g_pStopLP->TriggerLPOrder(m_order.keyID, false);
		}
		return false;
	}
}

bool TBreakEvenOrder::TriggerJudge(const double &Price)
{
	if (m_order.state == osGeneral)
	{
		if ((m_order.direct == dSell&&Price >= m_order.triggerPrice) || (m_order.direct == dBuy&&Price <= m_order.triggerPrice))
		{
			TStopLPitem itemtemp;
			memcpy_s(&itemtemp, sizeof(TStopLPitem), &m_order, sizeof(TStopLPitem));
			itemtemp.state = osBreak;
			g_pStopLP->ModifyLPOrder(&itemtemp);
			return false;
		}
		else
			return false;
	}
	else if (m_order.state == osBreak)
	{
		if ((m_order.direct == dSell&&Price <= m_order.triggerPrice) || (m_order.direct == dBuy&&Price >= m_order.triggerPrice))
		{
			if (m_order.TriggerCount + 1 < max(m_order.MaxCount, 1))//触发所需次数判断
			{
				g_pStopLP->TriggerLPOrder(m_order.keyID, true);
				return false;
			}
			else
			{
				//
				TStopLPitem itemtemp;
				memcpy_s(&itemtemp, sizeof(TStopLPitem), &m_order, sizeof(TStopLPitem));
				itemtemp.state = osTrig;
				itemtemp.TriggerCount++;
				g_pStopLP->ModifyLPOrder(&itemtemp);
				//trigger止损发单
				return true;
			}
		}
		else    //不连续，中断了
		{
			if (true == m_order.LastTirgger)
			{
				g_pStopLP->TriggerLPOrder(m_order.keyID, false);
			}
			return false;
		}
	}
	return false;
}

