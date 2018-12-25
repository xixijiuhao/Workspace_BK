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
	if (stShift.TotalQty > 0)						//�ж��Ƿ�����µ�
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
	//���жϵ�һ���ǿ�����ƽ��
	if (stShift.FirstColse)
	{
		if (!DefaultSendOrder(stShift, stOrder, true))								//ȡ����һ�Ⱥ�Լ����ʼ��
			return false;
		if (CloseNum(stOrder.UserNo, stOrder, stOrder.OrderQty,stShift.FirstColse))		//������óֲ��Ƿ񹻣������ֽ�֡���֣�
		{

			//��ȡ�۸�
			CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
			//��ƽ�ֵ�
			if (iTCoverNum == 0 && iCoverNum != 0)
			{
				stOrder.Offset = oCover;
				stOrder.OrderQty = iCoverNum;
				stShift.OrderId = g_pTradeApi->InsertOrder(stOrder);
				//���������µ�û�н��
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
		if (!DefaultSendOrder(stShift, stOrder, false))								//ȡ���ڶ��Ⱥ�Լ����ʼ��
			return false;
		if (CloseNum(stOrder.UserNo, stOrder, stOrder.OrderQty,stShift.FirstColse))		//�жϵڶ��˿��óֲ��Ƿ�
		{
			if (!DefaultSendOrder(stShift, stOrder, true))								//ȡ����һ�Ⱥ�Լ����ʼ��
				return false;
			CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
			//�¿��ֵ�
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
	//���жϵ�һ���ǿ�����ƽ��
	if (stShift.FirstColse)
	{
		if (!DefaultSendOrder(stShift, stOrder, true))								//ȡ����һ�Ⱥ�Լ����ʼ��
			return false;
		if (CloseNum(stOrder.UserNo, stOrder, stShift.TotalQty ,stShift.FirstColse))		//������óֲ��Ƿ񹻣������ֽ�֡���֣�
		{
			//��ȡ�۸�
			CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
			//��ƽ�ֵ�
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
		if (!DefaultSendOrder(stShift, stOrder, false))								//ȡ���ڶ��Ⱥ�Լ����ʼ��
			return false;
		//����ƽ������
		int CloseTotal = (stShift.TotalQty / stShift.OrderQty1)*stShift.OrderQty2;
		if (CloseNum(stOrder.UserNo, stOrder, CloseTotal, stShift.FirstColse))		//�жϵڶ��˿��óֲ��Ƿ�
		{
			if (!DefaultSendOrder(stShift, stOrder, true))								//ȡ����һ�Ⱥ�Լ����ʼ��
				return false;
			CConvert::GetPrice(&stOrder, stOrder.Direct, stShift.OrderPriceType1, stOrder.OrderPrice, stShift.OrderPriceSuperPot1);
			//�¿��ֵ�
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
	//���ƽ�ֵ����óֲ�����;
	CConvert::GetCloseNum(UserNo, &stOrder, stOrder.Direct, qty, iCoverNum, iTCoverNum);
	//����µ� ��һ���µ�����ƽ��ƽ��Total=total/qty1*qty2
	if (qty > iCoverNum + iTCoverNum)
	{
		//m_hParent = FindWindow(L"TShiftPosition", NULL);
		if (firstclose)
		{
			if (_TMessageBox_Domodal(m_hParent, "��ʾ", L"��һ�ȿ��óֲֲ��㣡", TMB_OK))
				return false;
		}
		else
		{
			if (_TMessageBox_Domodal(m_hParent, "��ʾ", L"�ڶ��ȿ��óֲֲ��㣡", TMB_OK))
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
	//ƽ��ƽ������Ӧ��ƽ�����óֲ֣�;
	int iCoverNum;
	int iTCoverNum;
	CConvert::GetCloseNum(pst->UserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCoverNum, iTCoverNum);
	if (iTCoverNum == 0 && iCoverNum == 0)
		_TMessageBox_Domodal(m_hParent, "��ʾ", L"û�п��óֲ֣�", TMB_OK);
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
		//ֱ����֣��ȳ��������
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
		//�ȳ��������
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
			g_pTradeApi->CancelOrder(iter->first);//����orderid����
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
	//ֻ����һ����ƽ�ֵ�ʱ���Ż��о��浥���
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
				//���Ϊ��ֵ����Ҵ�����ֵ���������ֵ���δ����������warning
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
						//����ֵ���δ��ɣ�����warning
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
		//����µ�
		if (iter->second.TotalQty > 0)
		{
			TSendOrder stOrder;
			int		OrderId = -1;
			bool	FirstOrSecond;
			//ȷ����һ��ί���ǵ�һ�Ȼ��ǵڶ��Ⱥ�Լ������ü۸�
			FirstOrSecond = FirstorSecond(pst, iter->second.FirstColse);
			GetOrderPrice(stOrder, iter->second, FirstOrSecond);
			if(iter->second.FirstColse)	//ƽ������
			{
				if (pst->OrderState == osFilled)
				{
					if (FirstOrSecond == false)
						SingleOpen(iter->second, pst, stOrder, FirstOrSecond);	//��һ��Ϊƽ�ֵ�
					else if (FirstOrSecond == true)
						SingleClose(iter->second, pst, stOrder, FirstOrSecond);	//��һ��Ϊ���ֵ�
				}
			}
			else//������ƽ
			{
				if (pst->OrderState == osFilled)
				{
					if (FirstOrSecond == true)
						SingleOpen(iter->second, pst, stOrder, FirstOrSecond);//��һ��Ϊƽ�ֵ�
					else if (FirstOrSecond == false)
						SingleClose(iter->second, pst, stOrder, FirstOrSecond);//��һ��Ϊ���ֵ�
				}
				
			}
			//���������൥��������
		}
		//�����µ�(����ȡ�������ó���)
		else
		{
			if (pst->OrderState == osFilled || pst->OrderState == osPartFilled)
			{
				TSendOrder stOrder;
				if (iter->second.FirstColse)
				{
					//��һ��Ϊƽ�ֵ�,�¿��ֵ�
					DefaultSendOrder(iter->second, stOrder, false);
					if (GetSecondShiftQty(pst->OrderID, iter->second, pst->MatchQty, stOrder.OrderQty))		//����������ƽ��ƽ���ȿ������ÿ���
					{
						CConvert::GetPrice(&stOrder, stOrder.Direct, iter->second.OrderPriceType2, stOrder.OrderPrice, iter->second.OrderPriceSuperPot2);
						stOrder.Offset = oOpen;
						//�µ�Ҫ�ж��Ƿ�׷�ۣ�
						if (iter->second.bChasePrice)
							InsertChaseOrder(stOrder, iter->second);			//�ڼ�¼׷�۲����µ�
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
					//��һ��Ϊ���ֵ�����ƽ�ֵ�
					if (pst->OrderState == osFilled || pst->OrderState == osPartFilled)
					{
						DefaultSendOrder(iter->second, stOrder, false);
						//��¼�Ѿ��Ŀ�������
						iter->second.OpenQty = pst->MatchQty;
						//������ƽ�ֵ�����
						stOrder.OrderQty = (pst->MatchQty * iter->second.OrderQty2) / iter->second.OrderQty1 - iter->second.CoverMatchQty - iter->second.TCoverMatchQty;
						CConvert::GetCloseNum(stOrder.UserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCoverNum, iTCoverNum);

						if (stOrder.OrderQty > iCoverNum + iTCoverNum)
						{
							stOrder.OrderQty = iTCoverNum + iCoverNum;
							if (!iter->second.Warning)
							{
								iter->second.Warning = true;
								//  ������û�п��ꡣƽ����������һ���ᵼ��ƽ�ֲ����Ӷ��޷�ɾ���Ʋֵ�
								//  �ڲ�����û�п��������¡�WarningҲ������ͬ�����ã�
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
							//ƽ�����û��߿��ֽ��������һ��ƽ�ֲ���
							if (((iter->second.CoverMatchQty + iter->second.TCoverMatchQty) == iter->second.OrderQty2) || PositionDef)
								m_mapShiftOrder.erase(iter);
						}
					}
				}
			}
		}
		//���������൥����   �������ĳ�����߲�����
		if ((pst->OrderState == osCanceled || pst->OrderState == osPartCanceled )  && iter->second.ConditionOrderId == -1)
		{
			//����ǹ�ѡ�˿���׷�۵��Ʋֵ����ҷ��صĳ�����Ϣ����ƽ���Ŀ��ֵ���ƽ��ƽ�ֵ��򲻴���
			//��׷�۵ĵط���Դ��࿪�ֵ����д���
			if (!(iter->second.bChasePrice && ((iter->second.FirstColse && pst->Offset == oOpen) || (!iter->second.FirstColse && (pst->Offset == oCover || pst->Offset == oCoverT)))))
				m_mapShiftOrder.erase(iter);
		}
		//ָ��ʧ�ܳ���
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
					if (iter->second.TotalQty == 0)	//�����µ�����ƽ�ֵ�
					{
						if (iter->second.FirstColse)
							iter->second.OrderQty1 -= pst->OrderQty - pst->MatchQty;
						else
							iter->second.OrderQty2 -= pst->OrderQty - pst->MatchQty;
						iter->second.OrderId = -1;
					}
					else
					{
						m_mapShiftOrder.erase(iter);//��������Ϊ������ȫ�ɽ����ܽ�����һ�ʵĿ���
					}
				}
			}
		}
		else if (pst->OrderState == osFilled && pst->ExchangeNo=="SHFE")
		{
			for (iter = m_mapShiftOrder.begin(); iter != m_mapShiftOrder.end(); iter++)
			{
				//����ҵ��϶�����������ƽ�ֵ� ����Ϊ��ֲ��㣬����ֲ�����ֵ�
				if (iter->second.OrderId == pst->OrderID && iter->second.TotalQty > 0)	//����µ���ƽ�ֵ�
				{
					TSendOrder stOrder;
					int		OrderId = -1;
					bool	FirstOrSecond;
					//ȷ����һ��ί���ǵ�һ�Ȼ��ǵڶ��Ⱥ�Լ������ü۸�
					FirstOrSecond = FirstorSecond(pst, iter->second.FirstColse);
					GetOrderPrice(stOrder, iter->second, FirstOrSecond);
					if (pst->OrderState == osFilled)
					{
						//ƽ���������ҵڶ���
						if (iter->second.FirstColse && !FirstOrSecond)
							SingleOpen(iter->second, pst, stOrder, FirstOrSecond);
						//������ƽ���ҵ�һ��
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
	//��Ϊ������ƽ�ֵ�ʱ����Ϊ��ֲ��㣬�����ʷ���ȥʱ
	//ȷ������ƽ�ֵ�����ȫ�ɽ�ʱ�����¿��ֵ�
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
	//�µ�Ҫ�ж��Ƿ�׷�ۣ�����ֻ�еڶ�����Ҫ׷
	if (iter->second.bChasePrice && iter->second.FirstColse)
	{
		//׷�۴���
		InsertChaseOrder(stOrder,iter->second);			//�ڼ�¼׷�۲����µ�
		//ԭ�ȵ��Ʋֵ�ɾ����׷��ʱ���¼���
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
			m_mapShiftOrder.erase(iter);		//��һ�ȳֲֲ��㣬�����µ�
			StopWarning(waShift.FirstColse, waShift.ContractCode1, waShift.ContractCode2, waShift.NeedOpenQty, waShift.NeedMatchQty, waShift.OpenQty, waShift.MatchQty);
			return;
		}
		else
		{
			stOrder.OrderQty = iCoverNum + iTCoverNum;		//�ڶ��ȳֲֲ��㣬���ټ����Ʋ�,��ʣ��ֲ�ƽ��
			if (stOrder.OrderQty == 0)		//��ʣ��ֲ�Ϊ0��ֹͣƽ�֡�
			{
				m_mapShiftOrder.erase(iter);
				StopWarning(waShift.FirstColse, waShift.ContractCode1, waShift.ContractCode2, waShift.NeedOpenQty, waShift.NeedMatchQty, waShift.OpenQty, waShift.MatchQty);
				return;
			}
		}
	}
	if (iTCoverNum == 0 && iCoverNum != 0)
	{
		iCoverNum =stOrder.OrderQty;		//��ʼ����ʱ�������Ĭ���µ�����
		iTCoverNum = 0;
		stOrder.Offset = oCover;
		iter->second.TOrderId = -1;		//���������µ�û�н��
		iter->second.OrderId = 0;		//���ڱ�������
		OrderId = iter->second.OrderId;
	}
	else
	{
		stOrder.Offset = oCoverT;
		iter->second.TOrderId = 0;		//���ڱ���н�ֵ�
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
			iter->second.OrderId = 0;		//���ڱ�������
		}
	}
	iter->second.TCoverMatchQty += iTCoverNum;
	iter->second.CoverMatchQty += iCoverNum;

	if (iter->second.bChasePrice && !iter->second.FirstColse)		//׷��
	{
		InsertChaseOrder(stOrder, iter->second);
		if (iter->second.OrderId == 0 && iter->second.TOrderId == 0)
		{
			stOrder.OrderQty = iCoverNum;
			stOrder.Offset = oCover;
			InsertChaseOrder(stOrder, iter->second);
		}
		m_mapShiftOrder.erase(iter);		//ԭ�ȵ��Ʋֵ�ɾ����׷��ʱ���¼���
	}
	else
	{
		if (iter->second.TOrderId == -1)		//�����������ĵ���
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
		if (iter->second.Warning)		//�ֲֲ���
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
		if ((iter->second.TotalQty <= 0) || iter->second.Warning)		//��һ�ȳֲֲ��㲻���ߵ��������ĳֲֲ��㾯��϶��ǵڶ��ȵĲ��㾯��
			m_mapShiftOrder.erase(iter);
		else
		{
			m_mapShiftOrder.insert(make_pair(OrderId, iter->second));		//����
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
	DefaultSendOrder(stShift, stOrder, firOrsec);//��ʼ����ʱ���Ѿ����µ����������˸�ֵ
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
				_TMessageBox_Domodal(m_hParent, "��ʾ", L"ȫ�������ɹ�", TMB_OK);
		}
		else if (pst->OrderState == osFail)
		{
			m_vcCancelOrderId.erase(iter);
			char Info[150];
			sprintf_s(Info, "�����ţ�%d ����ʧ��!", pst->OrderID);
			_TMessageBox_Domodal(m_hParent, "��ʾ", LoadRC::ansi_to_unicode(Info).c_str(), TMB_OK);
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
			sprintf_s(Info, "�����ţ�%d ����ʧ��!������...", pst->OrderID);
			_TMessageBox_Domodal(m_hParent, "��ʾ", LoadRC::ansi_to_unicode(Info).c_str(), TMB_OK);
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
			_TMessageBox_Domodal(m_hParent, "��ʾ", L"��ֵ�ȫ������!", TMB_OK);
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
		if (!iter->second.bCancel)   //����׷��ģ�鳷�ĵ����ǾͲ���׷
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

		if (g_pstConfig->m_stChasePrice.iChaseOver&&GetPriceOffset(&stOrder, iter->second.dFirstOrderPrice, stOrder.OrderPrice) >= g_pstConfig->m_stChasePrice.iChaseOver)//׷�۴����ﵽ
		{
			m_mapChaseOrder.erase(iter);
			return;
		}
		if (itershift != m_mapShiftOrder.end())
		{
			if (itershift->second.OrderId == -1)	//��������һ��ƽ��ĵ���
				itershift->second.TOrderId = iOrderID;
			else
				itershift->second.OrderId = iOrderID;//���������µ������֣�ƽ���ID
			m_mapShiftOrder.insert(make_pair(iOrderID, itershift->second));
			m_mapShiftOrder.erase(itershift);
		}
		std::map<int, ShiftSendOrder>::iterator iterOfSHFE = m_mapShiftOrder.begin();
		for (; iterOfSHFE != m_mapShiftOrder.end(); iterOfSHFE++)
		{
			if (pOrder->OrderID == iterOfSHFE->second.OrderId)
				iterOfSHFE->second.OrderId = iOrderID;		//����������ƽ��(����ƽ��������)��ID
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
	int OrderId = -1;//��¼�·����Ķ�����;
	//��ͨ�ĸĵ�
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
			_TMessageBox_Domodal(m_hParent, "��ʾ", L"�ĵ�ʧ��!", TMB_OK);
		}
		if (m_mapModifyOrder.size())
			m_bModify = false;
	}

	if (OrderId == -1)
		return;
	std::map<int, ShiftSendOrder>::iterator itershift = m_mapShiftOrder.find(pst->OrderID);
	if (itershift != m_mapShiftOrder.end())
	{
		
		//����������Ʋֵĵ��ӣ�Ҫ���¶�����;
		if (itershift->second.OrderId == pst->OrderID) //û��ƽ��ĵ���
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
	//����׷�ӵ���Ҫ����׷�ӱ���
	stChaseOrder.iOrderId = OrderId;
	m_mapChaseOrder.insert(std::make_pair(OrderId, stChaseOrder));
	//����µ��Ŀ�ƽ/ƽ����ƽ��/���ֵ�(��������ƽ/ƽ�������һ��ƽ��/���ֵ�)�������Ʋ��б���
	if (stShift.TotalQty > 0)
	{
		//���������һ�Σ� ��֤�µ�����
		//��һ���µ���ʱ���µ�������һ�Σ�����׷�����ټ�
		stShift.TotalQty -= stShift.OrderQty1;
		//ƽ�����Ŀ��ֵ�ֱ�Ӽ����Ʋֵ��б��У����һ�ʿ��ֵ�������
		if (stShift.FirstColse)
		{
			stShift.OrderId = OrderId;
			if (stShift.TotalQty > 0)
				m_mapShiftOrder.insert(std::make_pair(OrderId, stShift));
		}
		else//��ƽ����ƽ�ֵ���Ҫ����
		{
			if (stShift.TOrderId == -1)		//����ֻ����ֵ��������������ĵ��ӣ�
			{
				stShift.OrderId = OrderId;
				if (stShift.TotalQty > 0 && !stShift.Warning)		//�������һ��ƽ�ֵ�����û�гֲ־���
					m_mapShiftOrder.insert(std::make_pair(OrderId, stShift));
			}
			else
			{
				if (offset == oCover)	//����ĿǰҪ�µ������
				{
					stShift.OrderId = OrderId;
				}
				else                     //�����µ��ǽ��
				{
					stShift.TOrderId = OrderId;
					if (stShift.TotalQty > 0 && !stShift.Warning)		//�������һ��ƽ�ֵ�����û�гֲ־���
						m_mapShiftOrder.insert(std::make_pair(OrderId, stShift));
				}
			}

			if (stShift.Warning)			//�ֲֲ��㾯��
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
		_TMessageBox_Domodal(m_hParent, "��ʾ", L"�µ��۸���Ϊ0", TMB_OK);
		return -1;
	}
	if (stOrder.OrderQty == 0)
	{
		_TMessageBox_Domodal(m_hParent, "��ʾ", L"�µ���������Ϊ0", TMB_OK);
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
	CConvert::DefaultSendOrder(stOrder);				//��ʼ��
	stOrder.Hedge = stShift.Hedge;						//Ͷ�����ͳ�ʼ��

	TContractKey contract;
	memset(&contract, 0, sizeof(contract));
	if (bFirst)
	{
		flag = CConvert::TContractNoToTContractKey(stShift.ContractCode1, contract);
		stOrder.OrderQty = stShift.OrderQty1;
		stOrder.Direct = stShift.Direct1;						//��������
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
	//��OrderIDΪ�򵥹ؼ��֣���iMatchΪ�򵥳ɽ�������ʱȡ����pstΪ�񵥡�
	if (stShift.OrderId == OrderId && stShift.TOrderId != -1)
	{
		pst = g_pTradeData->GetOrder(stShift.TOrderId);
	}
	else if (stShift.TOrderId == OrderId && stShift.OrderId != -1)
	{
		pst = g_pTradeData->GetOrder(stShift.OrderId);
	}
	//������+�񵥳ɽ���֮��
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
		sprintf_s(psText, "�Ʋ���ֹ:\n %s\nӦƽ %d �� ʵƽ %d ��\n %s\nӦ�� %d �� ʵ�� %d ��", ContractCode1, Qty1, OpentQty, ContractCode2, Qty2, CloseQty);
	else 
		sprintf_s(psText, "�Ʋ���ֹ:\n %s\nӦ�� %d �� ʵ�� %d ��\n %s\nӦƽ %d �� ʵƽ %d ��", ContractCode1, Qty1, OpentQty, ContractCode2, Qty2, CloseQty);
	
	int psTextSize = MultiByteToWideChar(CP_ACP, 0, psText, -1, NULL, 0);
	memset(text, 0, sizeof(text));
	MultiByteToWideChar(CP_ACP, 0, psText, -1, text, psTextSize);
	
	_TMessageBox_Domodal(m_hParent, "��ʾ", text, TMB_OK);
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
	stConditionOrder.iTime = g_pstConfig->m_stChasePrice.iConditionSecond;				//ʱ��Ӧ���Ǵ������ж�ȡ
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
		//�ж�������������ϵͳ����
		if (conorderiter != m_mapConditionOrder.end() && coniter != m_mapShiftCondition->end())
		{
			if (conorderiter->second.bCancel)		//ϵͳ����
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
			m_mapConditionOrder.erase(conorderiter);	//������ϵͳ���������ֶ���������Ҫ����������Ƶ�
			
		}
	}
}

void ShiftTradeApi::IsTriggerByCondition(int ConditionId, int newOrderId)
{
	if (ConditionId != -1)		//��ʾ���������������µ�
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
