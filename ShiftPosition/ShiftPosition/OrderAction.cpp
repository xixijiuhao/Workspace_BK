#include "BaseInclude.h"
extern ITradeApi*  g_pTradeApi;
extern ITradeData* g_pTradeData;
extern IStarApi*  g_pStarApi;
extern ShiftOrderConfig *g_pstConfig;
extern TShiftPosition* g_pShiftPosition;

void DoubleToChar(int iPrecision, double Price, char* Dest)
{
	char format[20] = { 0 };
	sprintf_s(format, "%%0.%df", iPrecision);
	sprintf_s(Dest, sizeof(format), format, Price);
}
ShiftTradeApi::ShiftTradeApi()
{
	//m_hParent = FindWindow(L"TShiftPosition", NULL);
	m_bCancelAll = false;
	m_bCoverAll = false;
	m_bModify = false;
	m_bShift = false;
}

void ShiftTradeApi::Register(std::map<int, ShiftCondition>* pShiftCondition)
{
	if (pShiftCondition)
		m_mapShiftCondition = pShiftCondition;
}

bool ShiftTradeApi::SendShiftOrder(ShiftSendOrder& stShift)
{
	if(!m_hParent)
		m_hParent = FindWindow(L"TShiftPosition", NULL);
	if (stShift.TotalQty > 0)						//判断是否逐笔下单
		SingleInsetShiftOrder(stShift);
	else
		InsertShiftOrder(stShift);
	return true;
}

bool ShiftTradeApi::InsertShiftOrder(ShiftSendOrder& stShift)
{
	m_bShift = true;
	TSendOrder stOrder;
	iCoverNum = 0;
	iTCoverNum = 0;
	wchar_t psText[51] = { 0 };
	int ss = sizeof(psText);
	//先判断第一腿是开还是平。
	if (stShift.FirstColse)
	{
		if (!DefaultSendOrder(stShift, stOrder, true))								//取出第一腿合约并初始化
			return false;
		if (CloseNum(stOrder.UserNo, stOrder, stOrder.OrderQty,stShift.FirstColse))		//计算可用持仓是否够（并区分今仓、昨仓）
		{

			//获取价格
			CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
			//发平仓单
			if (iTCoverNum == 0 && iCoverNum != 0)
			{
				stOrder.Offset = oCover;
				stOrder.OrderQty = iCoverNum;
				stShift.OrderId = g_pTradeApi->InsertOrder(stOrder);
				//表明本次下单没有今仓
				stShift.TOrderId = -1;
				StoreShiftOrderId(stShift.OrderId, stShift);
			}
			else
			{
				stOrder.Offset = oCoverT;
				stOrder.OrderQty = iTCoverNum;
				stShift.TOrderId = g_pTradeApi->InsertOrder(stOrder);
				if (iCoverNum != 0)
				{
					stOrder.OrderQty = iCoverNum;
					stOrder.Offset = oCover;
					stShift.OrderId = g_pTradeApi->InsertOrder(stOrder);
				}
				StoreShiftOrderId(stShift.TOrderId, stShift);
			}
		}
	}
	else
	{
		if (!DefaultSendOrder(stShift, stOrder, false))								//取出第二腿合约并初始化
			return false;
		if (CloseNum(stOrder.UserNo, stOrder, stOrder.OrderQty,stShift.FirstColse))		//判断第二退可用持仓是否够
		{
			if (!DefaultSendOrder(stShift, stOrder, true))								//取出第一腿合约并初始化
				return false;
			CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
			//下开仓单
			stOrder.Offset = oOpen;
			stShift.OrderId = g_pTradeApi->InsertOrder(stOrder);
			StoreShiftOrderId(stShift.OrderId, stShift);
			
		}
	}
	return true;
}

bool ShiftTradeApi::SingleInsetShiftOrder(ShiftSendOrder& stShift)
{
	m_bShift = true;
	TSendOrder stOrder;
	iCoverNum = 0;
	iTCoverNum = 0;
	//先判断第一腿是开还是平。
	if (stShift.FirstColse)
	{
		if (!DefaultSendOrder(stShift, stOrder, true))								//取出第一腿合约并初始化
			return false;
		if (CloseNum(stOrder.UserNo, stOrder, stShift.TotalQty ,stShift.FirstColse))		//计算可用持仓是否够（并区分今仓、昨仓）
		{
			//获取价格
			CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
			//发平仓单
			if (iTCoverNum == 0 && iCoverNum != 0)
			{
				stOrder.Offset = oCover;
				iCoverNum = stOrder.OrderQty;
				iTCoverNum = 0;
				stShift.OrderId = g_pTradeApi->InsertOrder(stOrder);
				stShift.CoverMatchQty += iCoverNum;
				stShift.TCoverMatchQty += iTCoverNum;
				StoreShiftOrderId(stShift.OrderId, stShift);
			}
			else
			{
				stOrder.Offset = oCoverT;
				if (iTCoverNum >= stShift.OrderQty1)
				{
					stOrder.OrderQty = stShift.OrderQty1;
					iTCoverNum = stOrder.OrderQty;
					iCoverNum = 0;
					stShift.TOrderId = g_pTradeApi->InsertOrder(stOrder);
				}
				else
				{
					stOrder.OrderQty = iTCoverNum;
					stShift.TOrderId = g_pTradeApi->InsertOrder(stOrder);
					stOrder.OrderQty = iCoverNum;
					stOrder.Offset = oCover;
					stShift.OrderId = g_pTradeApi->InsertOrder(stOrder);
				}
				stShift.CoverMatchQty += iCoverNum;
				stShift.TCoverMatchQty += iTCoverNum;
				StoreShiftOrderId(stShift.TOrderId, stShift);
			}
		}
	}
	else
	{
		if (!DefaultSendOrder(stShift, stOrder, false))								//取出第二腿合约并初始化
			return false;
		//计算平仓总数
		int CloseTotal = (stShift.TotalQty / stShift.OrderQty1)*stShift.OrderQty2;
		if (CloseNum(stOrder.UserNo, stOrder, CloseTotal, stShift.FirstColse))		//判断第二退可用持仓是否够
		{
			if (!DefaultSendOrder(stShift, stOrder, true))								//取出第一腿合约并初始化
				return false;
			CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
			//下开仓单
			stOrder.Offset = oOpen;
			stShift.OpenQty += stOrder.OrderQty;
			stShift.OrderId = InsertOrder(stOrder);
			StoreShiftOrderId(stShift.OrderId, stShift);
		}
	}
	return true;
}

bool ShiftTradeApi::CloseNum(char* UserNo, TSendOrder stOrder, int qty ,bool firstclose)
{
	//检查平仓单可用持仓数量;
	CConvert::GetCloseNum(UserNo, &stOrder, stOrder.Direct, qty, iCoverNum, iTCoverNum);
	//逐比下单 第一次下单，开平。平的Total=total/qty1*qty2
	if (qty > iCoverNum + iTCoverNum)
	{
		//m_hParent = FindWindow(L"TShiftPosition", NULL);
		if (firstclose)
		{
			if (_TMessageBox_Domodal(m_hParent, "提示", L"第一腿可用持仓不足！", TMB_OK))
				return false;
		}
		else
		{
			if (_TMessageBox_Domodal(m_hParent, "提示", L"第二腿可用持仓不足！", TMB_OK))
				return false;
		}
	}
	return true;
}


void ShiftTradeApi::ModifyOrder(int OrderId, TSendOrder& stOrder)
{
	m_bModify = true;
	std::map<int, TSendOrder>::iterator iter = m_mapModifyOrder.find(OrderId);
	if (iter == m_mapModifyOrder.end())
		m_mapModifyOrder.insert(make_pair(OrderId, stOrder));

	if (g_pTradeApi)
		g_pTradeApi->CancelOrder(OrderId);
}
void ShiftTradeApi::CloseOrder(TPosition* pst, double OrderPrice)
{
	TSendOrder stOrder;
	CConvert::DefaultDealClose(pst, stOrder);
	stOrder.OrderPrice = OrderPrice;
	//平今平昨自适应（平调可用持仓）;
	int iCoverNum;
	int iTCoverNum;
	CConvert::GetCloseNum(pst->UserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCoverNum, iTCoverNum);
	if (iTCoverNum == 0 && iCoverNum == 0)
		_TMessageBox_Domodal(m_hParent, "提示", L"没有可用持仓！", TMB_OK);
	else if (iTCoverNum == 0 && iCoverNum != 0)
	{
		stOrder.Offset = oCover;
		stOrder.OrderQty = iCoverNum;
		InsertOrder(stOrder);
	}
	else
	{
		stOrder.Offset = oCoverT;
		stOrder.OrderQty = iTCoverNum;
		InsertOrder(stOrder);
		if (iCoverNum != 0)
		{
			stOrder.OrderQty = iCoverNum;
			stOrder.Offset = oCover;
			InsertOrder(stOrder);
		}
	}
}
void ShiftTradeApi::CanCelAll(TContainer ParOrder)
{
	m_bCancelAll = true;
	if (!g_pTradeData || !g_pTradeApi)
		return;
	for (int i = 0; i < ParOrder.size(); i++)
	{
		TOrder* pst = (TOrder*)ParOrder.at(i);
		if (!pst)
			continue;
		m_vcCancelOrderId.push_back(pst->OrderID);
		g_pTradeApi->CancelOrder(pst->OrderID);
	}
}
void ShiftTradeApi::CloseAll(TContainer stPosition, TContainer stOrder)
{
	m_bCoverAll = true;
	if (stOrder.size() == 0)
	{
		//直接清仓，先撤单在清仓
		double price = 0.0;
		for (int i = 0; i < stPosition.size(); i++)
		{
			TPosition* pPosition = (TPosition*)stPosition.at(i);
			if (!pPosition)
				continue;
			if (pPosition->Direct == dBuy)
				CConvert::GetPrice(pPosition, dSell, MarketPrice, price);
			else
				CConvert::GetPrice(pPosition, dBuy, MarketPrice, price);
			CloseOrder(pPosition, price);
		}
	}
	else
	{
		//先撤单在清仓
		for (int i = 0; i < stPosition.size(); i++)
		{
			TPosition* pPosition = (TPosition*)stPosition.at(i);
			if (!pPosition)
				continue;
			m_vcPosition.push_back(pPosition);
		}
		for (int i = 0; i < stOrder.size(); i++)
		{
			TOrder* pOrder = (TOrder*)stOrder.at(i);
			if (!pOrder)
				continue;
			m_vcCoverOrderId.push_back(pOrder->OrderID);
			g_pTradeApi->CancelOrder(pOrder->OrderID);
		}
	}
}
void ShiftTradeApi::OnDealOrder(const TOrder* pst)
{
	OnDealCanCelAll(pst);
	OnDealCloseAll(pst);
	OnDealModify(pst);
	OnDealShift(pst);
	OnDealWarning(pst);
	switch (pst->OrderState)
	{
	//case osQueued:
		//OnDealChaseQueue(pst);
	case osCanceled:
	case osPartFilled:
		OnDealChasePrice(pst);
		OnDealCondition(pst);
		break;
	case osFilled:
	case osFail:
	case osPartFailed:
		m_mapChaseOrder.erase(pst->OrderID);
		break;
	default:
		break;
	}	
}
void ShiftTradeApi::OnDealTimer()
{
	std::map<int, TChaseOrder>::iterator iter = m_mapChaseOrder.begin();
	for (; iter != m_mapChaseOrder.end(); iter++) {
		if (iter->second.iTime == 0) {
			iter->second.bCancel = true;
			g_pTradeApi->CancelOrder(iter->first);//根据orderid撤单
		}
		else {
			iter->second.iTime--;
		}
	}

	std::map<int, TConditionOrder>::iterator conditionIter = m_mapConditionOrder.begin();
	for (; conditionIter != m_mapConditionOrder.end(); conditionIter++)
	{
		if (conditionIter->second.iTime == 0)
		{
			conditionIter->second.bCancel = true;
			g_pTradeApi->CancelOrder(conditionIter->second.iOrderId);
		}
		else
		{
			conditionIter->second.iTime--;
		}
	}
}

void ShiftTradeApi::OnDealWarning(const TOrder* pst)
{
	//只有下一单是平仓单时，才会有警告单添加
	std::map<int, WarningShift>::iterator iter = m_mapWaringOrder.find(pst->OrderID);
	int Qty = 0;
	int TQty = 0;
	if (iter != m_mapWaringOrder.end())
	{
		
		if (iter->second.Warning)
			iter->second.Warning = false;
		else
			return;
		if (iter->second.TOrderId == pst->OrderID && iter->second.OrderId != -1)
		{
			const TOrder* Ypst = NULL;
			Ypst = g_pTradeData->GetOrder(iter->second.OrderId);
			if (Ypst->OrderState != osFilled || Ypst->OrderState != osCanceled || Ypst->OrderState != osPartCanceled || Ypst->OrderState != osFail)
			{
				//如果为今仓单，且存在昨仓单，并且昨仓单尚未结束。重置warning
				iter->second.Warning = true;
				return;
			}
			Qty = pst->OrderQty - pst->MatchQty;
		}
		if (pst->OrderState == osFilled || pst->OrderState == osCanceled || pst->OrderState == osPartCanceled || pst->OrderState == osFail)
		{
			TQty = pst->OrderQty - pst->MatchQty;
		}
		iter->second.MatchQty = iter->second.MatchQty - Qty - TQty;
		StopWarning(iter->second.FirstColse, iter->second.ContractCode1, iter->second.ContractCode2,iter->second.NeedOpenQty, iter->second.NeedMatchQty, iter->second.OpenQty, iter->second.MatchQty);
		m_mapWaringOrder.erase(iter);
	}
	else
	{
		for (iter = m_mapWaringOrder.begin(); iter != m_mapWaringOrder.end(); iter++)
		{
			if (iter->second.OrderId == pst->OrderID)
			{
				if (iter->second.Warning)
					iter->second.Warning = false;
				else
					return;
				if (iter->second.TOrderId != -1)
				{
					const TOrder* Tpst = NULL;
					Tpst = g_pTradeData->GetOrder(iter->second.TOrderId);
					if (Tpst->OrderState != osFilled || Tpst->OrderState != osCanceled || Tpst->OrderState != osPartCanceled || Tpst->OrderState != osFail)
					{
						//若今仓单尚未完成，重置warning
						iter->second.Warning = true;
						return;
					}
						
					
					TQty = pst->OrderQty - pst->MatchQty;
				}
				if (pst->OrderState == osFilled || pst->OrderState == osCanceled || pst->OrderState == osPartCanceled || pst->OrderState == osFail)
				{
					Qty = pst->OrderQty - pst->MatchQty;
				}
				iter->second.MatchQty = iter->second.MatchQty - Qty - TQty;
				StopWarning(iter->second.FirstColse, iter->second.ContractCode1, iter->second.ContractCode2, iter->second.NeedOpenQty, iter->second.NeedMatchQty, iter->second.OpenQty, iter->second.MatchQty);
				m_mapWaringOrder.erase(iter);
			}
		}
	}
}

void ShiftTradeApi::OnDealShift(const TOrder* pst)
{
	std::map<int, ShiftSendOrder>::iterator iter = m_mapShiftOrder.find(pst->OrderID);
	
	if (iter != m_mapShiftOrder.end())
	{
		//逐笔下单
		if (iter->second.TotalQty > 0)
		{
			TSendOrder stOrder;
			int		OrderId = -1;
			bool	FirstOrSecond;
			//确定下一笔委托是第一腿还是第二腿合约，并获得价格
			FirstOrSecond = FirstorSecond(pst, iter->second.FirstColse);
			GetOrderPrice(stOrder, iter->second, FirstOrSecond);
			if(iter->second.FirstColse)	//平——开
			{
				if (pst->OrderState == osFilled)
				{
					if (FirstOrSecond == false)
						SingleOpen(iter->second, pst, stOrder, FirstOrSecond);	//上一笔为平仓单
					else if (FirstOrSecond == true)
						SingleClose(iter->second, pst, stOrder, FirstOrSecond);	//上一笔为开仓单
				}
			}
			else//开——平
			{
				if (pst->OrderState == osFilled)
				{
					if (FirstOrSecond == true)
						SingleOpen(iter->second, pst, stOrder, FirstOrSecond);//上一笔为平仓单
					else if (FirstOrSecond == false)
						SingleClose(iter->second, pst, stOrder, FirstOrSecond);//上一笔为开仓单
				}
				
			}
			//撤单、撤余单另做处理
		}
		//比例下单(可提取，单独拿出来)
		else
		{
			if (pst->OrderState == osFilled || pst->OrderState == osPartFilled)
			{
				TSendOrder stOrder;
				if (iter->second.FirstColse)
				{
					//第一笔为平仓单,下开仓单
					DefaultSendOrder(iter->second, stOrder, false);
					if (GetSecondShiftQty(pst->OrderID, iter->second, pst->MatchQty, stOrder.OrderQty))		//计算上期所平今平昨，先开仓则不用考虑
					{
						CConvert::GetPrice(&stOrder, stOrder.Direct, iter->second.OrderPriceType2, stOrder.OrderPrice, iter->second.OrderPriceSuperPot2);
						stOrder.Offset = oOpen;
						//下单要判断是否追价；
						if (iter->second.bChasePrice)
							InsertChaseOrder(stOrder, iter->second);			//在记录追价部分下单
						else
							InsertOrder(stOrder);
						if (iter->second.OpenQty == iter->second.OrderQty2)
							m_mapShiftOrder.erase(iter);
					}
				}
				else
				{
					BOOL PositionDef = false;
					int OrderId = -1;
					int TOrderId = -1;
					//第一笔为开仓单，下平仓单
					if (pst->OrderState == osFilled || pst->OrderState == osPartFilled)
					{
						DefaultSendOrder(iter->second, stOrder, false);
						//记录已经的开仓数量
						iter->second.OpenQty = pst->MatchQty;
						//计算需平仓的数量
						stOrder.OrderQty = (pst->MatchQty * iter->second.OrderQty2) / iter->second.OrderQty1 - iter->second.CoverMatchQty - iter->second.TCoverMatchQty;
						CConvert::GetCloseNum(stOrder.UserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCoverNum, iTCoverNum);

						if (stOrder.OrderQty > iCoverNum + iTCoverNum)
						{
							stOrder.OrderQty = iTCoverNum + iCoverNum;
							if (!iter->second.Warning)
							{
								iter->second.Warning = true;
								//  不管有没有开完。平仓手数不足一定会导致平仓不满从而无法删除移仓单
								//  在不管有没有开完的情况下。Warning也能起到相同的作用；
								PositionDef = true;
								if (stOrder.OrderQty == 0)
								{
									WarningShift waShift;
									DefaultWarningShift(waShift, iter->second);
									m_mapShiftOrder.erase(iter);
									StopWarning(waShift.FirstColse, waShift.ContractCode1, waShift.ContractCode2, waShift.NeedOpenQty, waShift.NeedMatchQty, waShift.OpenQty, waShift.MatchQty);
									return;
								}
							}
						}

						if (stOrder.OrderQty != 0)
						{
							CConvert::GetPrice(&stOrder, stOrder.Direct, iter->second.OrderPriceType2, stOrder.OrderPrice, iter->second.OrderPriceSuperPot2);
							iter->second.CoverMatchQty += iCoverNum;
							iter->second.TCoverMatchQty += iTCoverNum;
							if (iTCoverNum == 0 && iCoverNum != 0)
							{
								stOrder.OrderQty = iCoverNum;
								stOrder.Offset = oCover;
								if (iter->second.bChasePrice)
									OrderId = InsertChaseOrder(stOrder, iter->second);
								else
									OrderId = InsertOrder(stOrder);
								TOrderId = -1;
							}
							else
							{
								stOrder.Offset = oCoverT;
								stOrder.OrderQty = iTCoverNum;
								if (iter->second.bChasePrice)
									TOrderId = InsertChaseOrder(stOrder, iter->second);
								else
									TOrderId = InsertOrder(stOrder);
								OrderId = -1;
								if (iCoverNum != 0)
								{
									stOrder.OrderQty = iCoverNum;
									stOrder.Offset = oCover;
									if (iter->second.bChasePrice)
										OrderId = InsertChaseOrder(stOrder, iter->second);
									else
										OrderId = InsertOrder(stOrder);
								}
							}
							if (iter->second.Warning)
							{
								WarningShift waShift;
								DefaultWarningShift(waShift, iter->second);
								waShift.OrderId = OrderId;
								waShift.TOrderId = TOrderId;
								if (waShift.TOrderId != -1)
									m_mapWaringOrder.insert(make_pair(waShift.TOrderId, waShift));
								else
									m_mapWaringOrder.insert(make_pair(waShift.OrderId, waShift));
							}
							//平仓正好或者开仓结束，最后一笔平仓不足
							if (((iter->second.CoverMatchQty + iter->second.TCoverMatchQty) == iter->second.OrderQty2) || PositionDef)
								m_mapShiftOrder.erase(iter);
						}
					}
				}
			}
		}
		//撤单、撤余单处理   条件单的撤单这边不处理
		if ((pst->OrderState == osCanceled || pst->OrderState == osPartCanceled )  && iter->second.ConditionOrderId == -1)
		{
			//如果是勾选了开仓追价的移仓单，且返回的撤单消息来自平开的开仓单或开平的平仓单则不处理
			//在追价的地方会对此类开仓单进行处理
			if (!(iter->second.bChasePrice && ((iter->second.FirstColse && pst->Offset == oOpen) || (!iter->second.FirstColse && (pst->Offset == oCover || pst->Offset == oCoverT)))))
				m_mapShiftOrder.erase(iter);
		}
		//指令失败撤单
		if (pst->OrderState == osFail)
		{
			OnDeleteConditionOrder(pst);
			m_mapShiftOrder.erase(iter);
		}
	}
	else
	{
		if (pst->OrderState == osFail || pst->OrderState == osCanceled || pst->OrderState == osPartCanceled)
		{
			for (iter = m_mapShiftOrder.begin(); iter != m_mapShiftOrder.end(); iter++)
			{
				if (iter->second.OrderId == pst->OrderID)
				{
					if (iter->second.TotalQty == 0)	//比例下单，撤平仓单
					{
						if (iter->second.FirstColse)
							iter->second.OrderQty1 -= pst->OrderQty - pst->MatchQty;
						else
							iter->second.OrderQty2 -= pst->OrderQty - pst->MatchQty;
						iter->second.OrderId = -1;
					}
					else
					{
						m_mapShiftOrder.erase(iter);//撤单，因为必须完全成交才能进行下一笔的开仓
					}
				}
			}
		}
		else if (pst->OrderState == osFilled && pst->ExchangeNo=="SHFE")
		{
			for (iter = m_mapShiftOrder.begin(); iter != m_mapShiftOrder.end(); iter++)
			{
				//如果找到肯定是上期所的平仓单 并且为今仓不足，用昨仓补得昨仓单
				if (iter->second.OrderId == pst->OrderID && iter->second.TotalQty > 0)	//逐笔下单的平仓单
				{
					TSendOrder stOrder;
					int		OrderId = -1;
					bool	FirstOrSecond;
					//确定下一笔委托是第一腿还是第二腿合约，并获得价格
					FirstOrSecond = FirstorSecond(pst, iter->second.FirstColse);
					GetOrderPrice(stOrder, iter->second, FirstOrSecond);
					if (pst->OrderState == osFilled)
					{
						//平——开，且第二笔
						if (iter->second.FirstColse && !FirstOrSecond)
							SingleOpen(iter->second, pst, stOrder, FirstOrSecond);
						//开——平，且第一笔
						else if (!iter->second.FirstColse && FirstOrSecond)
							SingleClose(iter->second, pst, stOrder, FirstOrSecond);
					}
				}
			}
		}
	}
}

void ShiftTradeApi::SingleOpen(ShiftSendOrder stShift,const TOrder* pst, TSendOrder stOrder, bool FirstOrSecond)
{
	std::map<int, ShiftSendOrder>::iterator iter = m_mapShiftOrder.find(pst->OrderID);
	int OrderId = -1;
	//若为上期所平仓单时，且为今仓不足，分两笔发出去时
	//确保两笔平仓单均完全成交时，再下开仓单
	if (pst->OrderID == iter->second.TOrderId && iter->second.OrderId != -1)
	{
		const TOrder* Ypst = NULL;
		Ypst = g_pTradeData->GetOrder(iter->second.OrderId);
		if (Ypst->OrderState != osFilled)
			return;
	}
	else if (pst->OrderID == iter->second.OrderId && iter->second.TOrderId != -1)
	{
		const TOrder* Tpst = NULL;
		Tpst = g_pTradeData->GetOrder(iter->second.TOrderId);
		if (Tpst->OrderState != osFilled)
			return;
	}
	stOrder.Offset = oOpen;
	iter->second.OpenQty += stOrder.OrderQty;
	//下单要判断是否追价，并且只有第二腿需要追
	if (iter->second.bChasePrice && iter->second.FirstColse)
	{
		//追价处理
		InsertChaseOrder(stOrder,iter->second);			//在记录追价部分下单
		//原先的移仓单删除，追价时重新加入
		m_mapShiftOrder.erase(iter);
	}
	else
	{
		OrderId = InsertOrder(stOrder);
		if(iter->second.FirstColse)
			iter->second.TotalQty -= iter->second.OrderQty1;
		IsTriggerByCondition(iter->second.ConditionOrderId, OrderId);
		if (iter->second.TotalQty <= 0)
			m_mapShiftOrder.erase(iter);
		else
		{
			iter->second.OrderId = OrderId;
			m_mapShiftOrder.insert(make_pair(OrderId, iter->second));
			m_mapShiftOrder.erase(iter);
		}
		
	}
}

void ShiftTradeApi::SingleClose(ShiftSendOrder stShift, const TOrder* pst, TSendOrder stOrder, bool FirstOrSecond)
{
	int OrderId = -1;
	int tempOrderQty;
	
	std::map<int, ShiftSendOrder>::iterator iter = m_mapShiftOrder.find(pst->OrderID);
	if (iter->second.FirstColse)
		CConvert::GetCloseNum(iter->second.UserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCoverNum, iTCoverNum);
	else
		CConvert::GetCloseNum(iter->second.UserNo2, &stOrder, stOrder.Direct, stOrder.OrderQty, iCoverNum, iTCoverNum);

	if (stOrder.OrderQty > iCoverNum + iTCoverNum)
	{
		iter->second.Warning = true;
		WarningShift waShift;
		DefaultWarningShift(waShift, iter->second);
		if (FirstOrSecond)
		{
			m_mapShiftOrder.erase(iter);		//第一腿持仓不足，不再下单
			StopWarning(waShift.FirstColse, waShift.ContractCode1, waShift.ContractCode2, waShift.NeedOpenQty, waShift.NeedMatchQty, waShift.OpenQty, waShift.MatchQty);
			return;
		}
		else
		{
			stOrder.OrderQty = iCoverNum + iTCoverNum;		//第二腿持仓不足，不再继续移仓,将剩余持仓平掉
			if (stOrder.OrderQty == 0)		//若剩余持仓为0，停止平仓。
			{
				m_mapShiftOrder.erase(iter);
				StopWarning(waShift.FirstColse, waShift.ContractCode1, waShift.ContractCode2, waShift.NeedOpenQty, waShift.NeedMatchQty, waShift.OpenQty, waShift.MatchQty);
				return;
			}
		}
	}
	if (iTCoverNum == 0 && iCoverNum != 0)
	{
		iCoverNum =stOrder.OrderQty;		//初始化的时候添加了默认下单数量
		iTCoverNum = 0;
		stOrder.Offset = oCover;
		iter->second.TOrderId = -1;		//表明本次下单没有今仓
		iter->second.OrderId = 0;		//用于标记有昨仓
		OrderId = iter->second.OrderId;
	}
	else
	{
		stOrder.Offset = oCoverT;
		iter->second.TOrderId = 0;		//用于标记有今仓单
		if (iTCoverNum >= stOrder.OrderQty)
		{
			iTCoverNum = stOrder.OrderQty;
			iCoverNum = 0;
			iter->second.OrderId = -1;
		}
		else
		{
			stOrder.OrderQty = iTCoverNum;
			if (iter->second.FirstColse)
				iCoverNum = iter->second.OrderQty1 - iTCoverNum;
			else
				iCoverNum = iter->second.OrderQty2 - iTCoverNum;
			iter->second.OrderId = 0;		//用于标记有昨仓
		}
	}
	iter->second.TCoverMatchQty += iTCoverNum;
	iter->second.CoverMatchQty += iCoverNum;

	if (iter->second.bChasePrice && !iter->second.FirstColse)		//追价
	{
		InsertChaseOrder(stOrder, iter->second);
		if (iter->second.OrderId == 0 && iter->second.TOrderId == 0)
		{
			stOrder.OrderQty = iCoverNum;
			stOrder.Offset = oCover;
			InsertChaseOrder(stOrder, iter->second);
		}
		m_mapShiftOrder.erase(iter);		//原先的移仓单删除，追价时重新加入
	}
	else
	{
		if (iter->second.TOrderId == -1)		//不是上期所的单子
		{
			iter->second.OrderId = g_pTradeApi->InsertOrder(stOrder);
			OrderId = iter->second.OrderId;
		}
		else
		{
			iter->second.TOrderId = g_pTradeApi->InsertOrder(stOrder);
			if(iter->second.OrderId != -1)
			{
				stOrder.OrderQty = iCoverNum;
				stOrder.Offset = oCover;
				iter->second.OrderId = g_pTradeApi->InsertOrder(stOrder);
			}
			OrderId = iter->second.TOrderId;
		}
		IsTriggerByCondition(iter->second.ConditionOrderId, OrderId);
		if (iter->second.Warning)		//持仓不足
		{
			WarningShift waShift;
			DefaultWarningShift(waShift, iter->second);
			if (iter->second.TOrderId != -1)
				m_mapWaringOrder.insert(make_pair(waShift.TOrderId, waShift));
			else
				m_mapWaringOrder.insert(make_pair(waShift.OrderId, waShift));
		}
		if (!iter->second.FirstColse)
			iter->second.TotalQty -= iter->second.OrderQty1;
		if ((iter->second.TotalQty <= 0) || iter->second.Warning)		//第一腿持仓不足不会走到这里，这里的持仓不足警告肯定是第二腿的不足警告
			m_mapShiftOrder.erase(iter);
		else
		{
			m_mapShiftOrder.insert(make_pair(OrderId, iter->second));		//更新
			m_mapShiftOrder.erase(iter);
		}
	}
}

bool ShiftTradeApi::FirstorSecond(const TOrder* pst, bool firstclose)
{
	if (firstclose)
	{
		if (pst->Offset == 'O')
			return true;
		else if (pst->Offset == 'C' || pst->Offset == 'T')
			return false;
	}
	else
	{
		if (pst->Offset == 'O')
			return false;
		else if (pst->Offset == 'C' || pst->Offset == 'T')
			return true;
	}
}

void ShiftTradeApi::GetOrderPrice(TSendOrder& stOrder, ShiftSendOrder stShift, bool firOrsec)
{
	DefaultSendOrder(stShift, stOrder, firOrsec);//初始化的时候已经对下单数量进行了赋值
	if(firOrsec)
		CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
	else
		CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType2, stOrder.OrderPrice, stShift.OrderPriceSuperPot2);
}


void ShiftTradeApi::OnDealCanCelAll(const TOrder* pst)
{
	std::vector<int>::iterator iter = find(m_vcCancelOrderId.begin(), m_vcCancelOrderId.end(), pst->OrderID);
	if (iter != m_vcCancelOrderId.end())
	{
		if (pst->OrderState == osCanceled || pst->OrderState == osPartCanceled)
		{
			m_vcCancelOrderId.erase(iter);
			if (m_vcCancelOrderId.size() == 0)
				_TMessageBox_Domodal(m_hParent, "提示", L"全部撤单成功", TMB_OK);
		}
		else if (pst->OrderState == osFail)
		{
			m_vcCancelOrderId.erase(iter);
			char Info[150];
			sprintf_s(Info, "订单号：%d 撤单失败!", pst->OrderID);
			_TMessageBox_Domodal(m_hParent, "提示", LoadRC::ansi_to_unicode(Info).c_str(), TMB_OK);
		}

		if (m_vcCancelOrderId.size() == 0)
			m_bCancelAll = false;
	}
}
void ShiftTradeApi::OnDealCloseAll(const TOrder* pst)
{
	std::vector<int>::iterator iter = find(m_vcCoverOrderId.begin(), m_vcCoverOrderId.end(), pst->OrderID);
	if (iter != m_vcCoverOrderId.end())
	{
		if (pst->OrderState == osCanceled || pst->OrderState == osPartCanceled)
		{
			m_vcCoverOrderId.erase(iter);
		}
		else if (pst->OrderState == osFail)
		{
			m_vcCoverOrderId.erase(iter);
			char Info[150];
			sprintf_s(Info, "订单号：%d 撤单失败!请重试...", pst->OrderID);
			_TMessageBox_Domodal(m_hParent, "提示", LoadRC::ansi_to_unicode(Info).c_str(), TMB_OK);
		}

		if (m_vcCoverOrderId.size() == 0)
		{
			for (int i = 0; i < m_vcPosition.size(); i++)
			{
				TPosition* pst = (TPosition*)m_vcPosition.at(i);
				if (!pst)
					continue;
				double OrderPrice = 0.0;
				TDirect direct;
				if (pst->Direct == dBuy)
					direct = dSell;
				else
					direct = dBuy;
				CConvert::GetPrice(pst, direct, MarketPrice, OrderPrice);
				CloseOrder(pst, OrderPrice);
			}
			m_bCoverAll = false;
			_TMessageBox_Domodal(m_hParent, "提示", L"清仓单全部发出!", TMB_OK);
		}
	}
}
void ShiftTradeApi::OnDealChaseQueue(const TOrder* pst)
{
	auto iter = m_mapChaseOrder.find(pst->OrderID);
	if (iter != m_mapChaseOrder.end()) {
		iter->second.iChaseCount++;
		iter->second.iTime = g_pstConfig->m_stChasePrice.iSecond;
		g_pTradeApi->CancelOrder(iter->first);
	}
}
void ShiftTradeApi::OnDealChasePrice(const TOrder* pOrder)
{
	auto iter = m_mapChaseOrder.find(pOrder->OrderID);
	auto itershift = m_mapShiftOrder.find(pOrder->OrderID);
	if (iter != m_mapChaseOrder.end())
	{
		if (!iter->second.bCancel)   //不是追单模块撤的单，那就不再追
		{
			if (itershift != m_mapShiftOrder.end())
				m_mapShiftOrder.erase(itershift);
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
		iter->second.iTime = g_pstConfig->m_stChasePrice.iSecond;
		stOrder.OrderPrice = GetPrice(pOrder, &(iter->second));
		iter->second.bCancel = false;
		uint iOrderID = g_pTradeApi->InsertOrder(stOrder);
		stOrder.OrderQty = pOrder->PreWorkingQty;

		if (g_pstConfig->m_stChasePrice.iChaseOver&&GetPriceOffset(&stOrder, iter->second.dFirstOrderPrice, stOrder.OrderPrice) >= g_pstConfig->m_stChasePrice.iChaseOver)//追价次数达到
		{
			m_mapChaseOrder.erase(iter);
			return;
		}
		if (itershift != m_mapShiftOrder.end())
		{
			if (itershift->second.OrderId == -1)	//表明这是一个平今的单子
				itershift->second.TOrderId = iOrderID;
			else
				itershift->second.OrderId = iOrderID;//撤单重新下单，开仓，平昨的ID
			m_mapShiftOrder.insert(make_pair(iOrderID, itershift->second));
			m_mapShiftOrder.erase(itershift);
		}
		std::map<int, ShiftSendOrder>::iterator iterOfSHFE = m_mapShiftOrder.begin();
		for (; iterOfSHFE != m_mapShiftOrder.end(); iterOfSHFE++)
		{
			if (pOrder->OrderID == iterOfSHFE->second.OrderId)
				iterOfSHFE->second.OrderId = iOrderID;		//更新上期所平昨单(存在平今的情况下)的ID
		}
		m_mapChaseOrder.insert(make_pair(iOrderID, iter->second));
		m_mapChaseOrder.erase(iter);
	}
}
uint ShiftTradeApi::GetPriceOffset(const TSendOrder* pOrder, double dFirstOrderPrice, double OrderPrice)
{
	double dMinChangePrice = GetMinChangePrice(pOrder);
	if (dMinChangePrice != 0)
		return abs((OrderPrice - dFirstOrderPrice) / dMinChangePrice);
	return 0;
}
double ShiftTradeApi::GetPrice(const TOrder* pOrder, TChaseOrder *pChase)
{
	double dPrice = pChase->dFirstOrderPrice;
	//CConvert::GetPrice(const_cast<TOrder*>(pOrder), pOrder->Direct, CounterPrice, dPrice);

	double dMarketPrice = 0.0;
	CConvert::GetPrice(const_cast<TOrder*>(pOrder), pOrder->Direct, MarketPrice, dMarketPrice);

	if (g_pstConfig->m_stChasePrice.iChasePriceCom == ChaseLadderPrice)
	{
		double MinPrice = GetMinChangePrice(pOrder);
		uint iChaseCount = pChase->iChaseCount < 3 ? pChase->iChaseCount : 3;

		if (pOrder->Direct == dBuy)
			dPrice += iChaseCount * MinPrice;
		else if (pOrder->Direct == dSell)
			dPrice -= iChaseCount * MinPrice;
		if ((dPrice > dMarketPrice && pOrder->Direct == dBuy) || (dPrice < dMarketPrice && pOrder->Direct == dSell))
			dPrice = dMarketPrice;
	}
	else if (g_pstConfig->m_stChasePrice.iChasePriceCom == ChaseMarketPrice)
	{
		dPrice = dMarketPrice;
	}
	return dPrice;
}
void ShiftTradeApi::OnDealModify(const TOrder* pst)
{
	int OrderId = -1;//记录新发单的订单号;
	//普通的改单
	std::map<int, TSendOrder>::iterator iter = m_mapModifyOrder.find(pst->OrderID);
	if (iter != m_mapModifyOrder.end())
	{
		if (pst->OrderState == osCanceled)
		{
			if (g_pTradeApi)
				OrderId = g_pTradeApi->InsertOrder(iter->second);
			m_mapModifyOrder.erase(iter);
		}
		else if (pst->OrderState == osFail)
		{
			_TMessageBox_Domodal(m_hParent, "提示", L"改单失败!", TMB_OK);
		}
		if (m_mapModifyOrder.size())
			m_bModify = false;
	}

	if (OrderId == -1)
		return;
	std::map<int, ShiftSendOrder>::iterator itershift = m_mapShiftOrder.find(pst->OrderID);
	if (itershift != m_mapShiftOrder.end())
	{
		
		//如果单子是移仓的单子，要更新订单号;
		if (itershift->second.OrderId == pst->OrderID) //没有平今的单子
		{
			itershift->second.OrderId = OrderId;
			m_mapShiftOrder.insert(make_pair(OrderId, itershift->second));
			m_mapShiftOrder.erase(itershift);
		}
		else if (itershift->second.TOrderId == pst->OrderID)
		{
			itershift->second.TOrderId = OrderId;
			m_mapShiftOrder.insert(make_pair(OrderId, itershift->second));
			m_mapShiftOrder.erase(itershift);
		}
	}
	else
	{
		for (itershift = m_mapShiftOrder.begin(); itershift != m_mapShiftOrder.end(); itershift++)
		{
			if (itershift->second.OrderId == pst->OrderID)
			{
			
				itershift->second.OrderId = OrderId;
				break;
			}
		}
	}
}
int ShiftTradeApi::InsertChaseOrder(TSendOrder& stOrder,ShiftSendOrder stShift, int iChaseOrderId)
{
	int iOrderId = -1;
	TChaseOrder stChaseOrder;
	memset(&stChaseOrder, 0, sizeof(TChaseOrder));
	stChaseOrder.iChaseCount = 0;
	stChaseOrder.bCancel = false;
	stChaseOrder.dFirstOrderPrice = stOrder.OrderPrice;
	stChaseOrder.iTime = g_pstConfig->m_stChasePrice.iSecond;

	if (iChaseOrderId < 0) 
	{
		iOrderId = InsertOrder(stOrder);
		if (iOrderId > 0) {
			RecordChaseOrder(stShift, stChaseOrder, iOrderId, stOrder.Offset);
			IsTriggerByCondition(stShift.ConditionOrderId, iOrderId);
		}
	}
	else {
		stChaseOrder.iOrderId = iChaseOrderId;
		m_mapChaseOrder.insert(std::make_pair(iChaseOrderId, stChaseOrder));
		g_pTradeApi->CancelOrder(stChaseOrder.iOrderId);
	}
	return iOrderId;
}

void ShiftTradeApi::RecordChaseOrder(ShiftSendOrder stShift, TChaseOrder stChaseOrder, int OrderId, TOffset offset)
{
	//所有追加单均要加入追加表中
	stChaseOrder.iOrderId = OrderId;
	m_mapChaseOrder.insert(std::make_pair(OrderId, stChaseOrder));
	//逐笔下单的开平/平开的平仓/开仓单(不包括开平/平开的最后一笔平仓/开仓单)，加入移仓列表中
	if (stShift.TotalQty > 0)
	{
		//总手数需减一次， 保证下单次数
		//第一次下单的时候下单次数减一次，后面追单不再减
		stShift.TotalQty -= stShift.OrderQty1;
		//平开单的开仓单直接加入移仓单列表中，最后一笔开仓单不加入
		if (stShift.FirstColse)
		{
			stShift.OrderId = OrderId;
			if (stShift.TotalQty > 0)
				m_mapShiftOrder.insert(std::make_pair(OrderId, stShift));
		}
		else//开平单的平仓单需要处理
		{
			if (stShift.TOrderId == -1)		//表明只有昨仓单（不是上期所的单子）
			{
				stShift.OrderId = OrderId;
				if (stShift.TotalQty > 0 && !stShift.Warning)		//不是最后一笔平仓单，且没有持仓警告
					m_mapShiftOrder.insert(std::make_pair(OrderId, stShift));
			}
			else
			{
				if (offset == oCover)	//表明目前要下的是昨仓
				{
					stShift.OrderId = OrderId;
				}
				else                     //现在下的是今仓
				{
					stShift.TOrderId = OrderId;
					if (stShift.TotalQty > 0 && !stShift.Warning)		//不是最后一笔平仓单，且没有持仓警告
						m_mapShiftOrder.insert(std::make_pair(OrderId, stShift));
				}
			}

			if (stShift.Warning)			//持仓不足警告
			{
				WarningShift waShift;
				DefaultWarningShift(waShift, stShift);
				if (stShift.TOrderId != -1)
					m_mapWaringOrder.insert(make_pair(waShift.TOrderId, waShift));
				else
					m_mapWaringOrder.insert(make_pair(waShift.OrderId, waShift));
			}
		}

	}
}



int ShiftTradeApi::InsertOrder(TSendOrder& stOrder)
{
	if (stOrder.OrderPrice == 0)
	{
		_TMessageBox_Domodal(m_hParent, "提示", L"下单价格不能为0", TMB_OK);
		return -1;
	}
	if (stOrder.OrderQty == 0)
	{
		_TMessageBox_Domodal(m_hParent, "提示", L"下单数量不能为0", TMB_OK);
		return -1;
	}
	int iOrderQty = stOrder.OrderQty;
	while (iOrderQty > 1000)
	{
		stOrder.OrderQty = 1000;
		g_pTradeApi->InsertOrder(stOrder);
		iOrderQty -= 1000;
	}
	stOrder.OrderQty = iOrderQty;
	return g_pTradeApi->InsertOrder(stOrder);
}
double ShiftTradeApi::GetMinChangePrice(const TCommodityKey *pKey)
{
	const SCommodity* pCommodity = g_pTradeData->GetCommodity(pKey);
	if (!pCommodity) return 0;
	return pCommodity->PriceDeno == 1 ? pCommodity->PriceNume : pCommodity->PriceNume / pCommodity->PriceDeno;
}
void ShiftTradeApi::StoreShiftOrderId(int OrderId, ShiftSendOrder stShift)
{
	std::map<int, ShiftSendOrder>::iterator iter;
	iter = m_mapShiftOrder.find(OrderId);
	if (iter == m_mapShiftOrder.end())
	{
		m_mapShiftOrder.insert(make_pair(OrderId, stShift));
		if (stShift.ConditionOrderId != -1)
 			addConditionOrder(OrderId, stShift.ConditionOrderId);
	}
	else
	{
		m_mapShiftOrder.erase(iter);									
		m_mapShiftOrder.insert(make_pair(OrderId, stShift));
		if (stShift.ConditionOrderId != -1)
			addConditionOrder(OrderId, stShift.ConditionOrderId);
	}
}
bool ShiftTradeApi::DefaultSendOrder(ShiftSendOrder stShift, TSendOrder& stOrder, bool bFirst)
{
	bool flag = false;
	CConvert::DefaultSendOrder(stOrder);				//初始化
	stOrder.Hedge = stShift.Hedge;						//投保类型初始化

	TContractKey contract;
	memset(&contract, 0, sizeof(contract));
	if (bFirst)
	{
		flag = CConvert::TContractNoToTContractKey(stShift.ContractCode1, contract);
		stOrder.OrderQty = stShift.OrderQty1;
		stOrder.Direct = stShift.Direct1;						//买卖方向
		memcpy(&stOrder, &contract, sizeof(TContractKey));
		memcpy(&stOrder.UserNo, stShift.UserNo, sizeof(stOrder.UserNo));
		memcpy(&stOrder.Sign, stShift.Sign, sizeof(stOrder.Sign));
	}
	else
	{
		flag = CConvert::TContractNoToTContractKey(stShift.ContractCode2, contract);
		stOrder.OrderQty = stShift.OrderQty2;
		stOrder.Direct = stShift.Direct2;
		memcpy(&stOrder, &contract, sizeof(TContractKey));
		memcpy(&stOrder.UserNo, stShift.UserNo2, sizeof(stOrder.UserNo));
		memcpy(&stOrder.Sign, stShift.Sign2, sizeof(stOrder.Sign));
	}
	
	
	return flag;
}
bool ShiftTradeApi::GetSecondShiftQty(int OrderId, ShiftSendOrder& stShift, int iMatch, uint& OrderQty)
{
	const TOrder* pst = NULL;
	int iOtherMatchQty = 0;
	//若OrderID为昨单关键字，则iMatch为昨单成交量，此时取出的pst为今单。
	if (stShift.OrderId == OrderId && stShift.TOrderId != -1)
	{
		pst = g_pTradeData->GetOrder(stShift.TOrderId);
	}
	else if (stShift.TOrderId == OrderId && stShift.OrderId != -1)
	{
		pst = g_pTradeData->GetOrder(stShift.OrderId);
	}
	//计算昨单+今单成交量之和
	if (pst)
		iMatch += pst->MatchQty;
	OrderQty = (iMatch * stShift.OrderQty2) / stShift.OrderQty1;
	if (OrderQty != 0 && OrderQty > stShift.OpenQty)
	{
		OrderQty -= stShift.OpenQty;
		stShift.OpenQty += OrderQty;
		return true;
	}	
	return false;
}

void ShiftTradeApi::StopWarning(bool firstclose, char *ContractCode1, char *ContractCode2, int Qty1, int Qty2, int OpentQty, int CloseQty)
{
	wchar_t text[100];
	char psText[100];
	if (firstclose)
		sprintf_s(psText, "移仓终止:\n %s\n应平 %d 手 实平 %d 手\n %s\n应开 %d 手 实开 %d 手", ContractCode1, Qty1, OpentQty, ContractCode2, Qty2, CloseQty);
	else 
		sprintf_s(psText, "移仓终止:\n %s\n应开 %d 手 实开 %d 手\n %s\n应平 %d 手 实平 %d 手", ContractCode1, Qty1, OpentQty, ContractCode2, Qty2, CloseQty);
	
	int psTextSize = MultiByteToWideChar(CP_ACP, 0, psText, -1, NULL, 0);
	memset(text, 0, sizeof(text));
	MultiByteToWideChar(CP_ACP, 0, psText, -1, text, psTextSize);
	
	_TMessageBox_Domodal(m_hParent, "提示", text, TMB_OK);
}

void ShiftTradeApi::DefaultWarningShift(WarningShift &waShift, ShiftSendOrder stShift)
{
	waShift.FirstColse = stShift.FirstColse;
	memcpy(waShift.ContractCode1, stShift.ContractCode1, sizeof(waShift.ContractCode1));
	memcpy(waShift.ContractCode2, stShift.ContractCode2, sizeof(waShift.ContractCode2));
	if (stShift.RecordTotal > 0)
	{
		if (waShift.FirstColse)
		{
			waShift.NeedOpenQty = stShift.RecordTotal / stShift.OrderQty1*stShift.OrderQty2;
			waShift.NeedMatchQty = stShift.RecordTotal;
		}
		else
		{
			waShift.NeedOpenQty = stShift.RecordTotal;
			waShift.NeedMatchQty = stShift.RecordTotal / stShift.OrderQty1*stShift.OrderQty2;
		}
	}
	else
	{
		waShift.NeedOpenQty = stShift.OrderQty1;
		waShift.NeedMatchQty = stShift.OrderQty2;
	}
	waShift.MatchQty = stShift.CoverMatchQty + stShift.TCoverMatchQty;
	waShift.OpenQty = stShift.OpenQty;
	waShift.OrderId = stShift.OrderId;
	waShift.TOrderId = stShift.TOrderId;
}

void ShiftTradeApi::addConditionOrder(int orderId, int ConditionOrderId)
{
	TConditionOrder stConditionOrder;
	memset(&stConditionOrder, 0, sizeof(TConditionOrder));
	stConditionOrder.iOrderId = orderId;
	stConditionOrder.ConditionId = ConditionOrderId;
	stConditionOrder.iTime = g_pstConfig->m_stChasePrice.iConditionSecond;				//时间应该是从设置中读取
	stConditionOrder.iMatchQty = 0;
	stConditionOrder.bCancel = false;

	m_mapConditionOrder.insert(make_pair(ConditionOrderId, stConditionOrder));
}

void ShiftTradeApi::OnDealCondition(const TOrder* pst)
{
	auto iterShift = m_mapShiftOrder.find(pst->OrderID);
	if (iterShift != m_mapShiftOrder.end() && iterShift->second.ConditionOrderId != -1)
	{
		auto coniter = m_mapShiftCondition->find(iterShift->second.ConditionOrderId);
		auto conorderiter = m_mapConditionOrder.find(iterShift->second.ConditionOrderId);
		//判断主动撤单还是系统撤单
		if (conorderiter != m_mapConditionOrder.end() && coniter != m_mapShiftCondition->end())
		{
			if (conorderiter->second.bCancel)		//系统撤单
			{
				if (iterShift->second.TotalQty > 0)
				{
					coniter->second.TotalQty = iterShift->second.TotalQty;
					coniter->second.OrderState = osRun;
				}
				else
				{
					if (iterShift->second.FirstColse)
					{
						coniter->second.OrderQty1 = iterShift->second.OrderQty1 - iterShift->second.CoverMatchQty - iterShift->second.TCoverMatchQty;
						coniter->second.OrderQty2 = iterShift->second.OrderQty2 - iterShift->second.OpenQty;
					}
					else
					{
						coniter->second.OrderQty2 = iterShift->second.OrderQty2 - iterShift->second.CoverMatchQty - iterShift->second.TCoverMatchQty;
						coniter->second.OrderQty1 = iterShift->second.OrderQty1 - iterShift->second.OpenQty;
					}
					coniter->second.OrderState = osRun;
				}
				g_pShiftPosition->ShowAllCondition();
			}
			m_mapShiftOrder.erase(iterShift);
			m_mapConditionOrder.erase(conorderiter);	//无论是系统撤单还是手动撤单，都要清除条件控制单
			
		}
	}
}

void ShiftTradeApi::IsTriggerByCondition(int ConditionId, int newOrderId)
{
	if (ConditionId != -1)		//表示是条件单触发的下单
	{
		std::map<int, TConditionOrder>::iterator tConditionOrder = m_mapConditionOrder.find(ConditionId);
		tConditionOrder->second.iOrderId = newOrderId;
	}
}

void ShiftTradeApi::OnDeleteConditionOrder(const TOrder* pst)
{
	auto iterShift = m_mapShiftOrder.find(pst->OrderID);
	if (iterShift != m_mapShiftOrder.end())
	{
		auto coniter = m_mapShiftCondition->find(iterShift->second.ConditionOrderId);
		if (coniter != m_mapShiftCondition->end())
			m_mapShiftCondition->erase(coniter);
	}
}
