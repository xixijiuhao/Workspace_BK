#include "BaseInclude.h"


extern TShiftPosition* g_pShiftPosition;
extern ShiftOrderConfig *g_pstConfig;
#define  AddConditionWnd 0
#define  ModifyConditionWnd 1
#define  AddNewCondition 2
TShiftCondition::TShiftCondition()
{
	m_cshiftContractNo[0] = '\0';
	m_iOrderId = -1;
	m_giOrderId = 1;
}
void TShiftCondition::Regist(std::map<string, ShiftListInfo>* pmapListInfo, std::map<int, ShiftCondition>* pmapshiftCondition, std::map<string, TUserInfo*>* pstUserInfo)
{
	if (pmapListInfo && pstUserInfo && pmapshiftCondition)
	{
		m_pmapShiftListInfo = pmapListInfo;
		m_pmapShiftCondition = pmapshiftCondition;
		m_pstUserInfo = pstUserInfo;
	}		
}
void TShiftCondition::Show()
{
	if (!IsWindow(GetHwnd()))
	{
		ShowFrm(g_pShiftPosition->GetHwnd(), L"TShiftCondition", "移仓条件单");
		POINT pt;
		GetCursorPos(&pt);
		MoveWindow(pt.x, pt.y, 370, 280);
	}
	else
	{
		if (!IsWindowVisible(GetHwnd()))
			ShowWindow(GetHwnd(), SW_SHOW);
		else
			ShowWindow(GetHwnd(), SW_HIDE);
	}
}
void TShiftCondition::ShowCondition(const char* UserNo,const char* UserNo2, const char* ShiftContractNo)
{
	if (strcmp(ShiftContractNo, "") == 0)
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"请选择合约！", TMB_OK);
		return;
	}
	strcpy_s(m_cshiftContractNo, sizeof(m_cshiftContractNo), ShiftContractNo);
	strcpy_s(m_UserNo, sizeof(m_UserNo), UserNo);
	strcpy_s(m_UserNo2, sizeof(m_UserNo2), UserNo2);
	Show();
	m_iWndType = AddConditionWnd;
	InitWidgetData(false);
}
void TShiftCondition::ModifyCondition(int OrderId)
{
	std::map<int, ShiftCondition>::iterator iter = m_pmapShiftCondition->find(OrderId);
	if (iter != m_pmapShiftCondition->end())
	{
		strcpy_s(m_cshiftContractNo, sizeof(m_cshiftContractNo), iter->second.ShiftContractNo);
		strcpy_s(m_UserNo, sizeof(m_UserNo), iter->second.UserNo);
		m_iOrderId = OrderId;
	}
	else
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"没有该条件单", TMB_OK);
		return;
	}
	Show();
	m_iWndType = ModifyConditionWnd;
	InitWidgetData(true);
}
void TShiftCondition::OnCreate()
{
	CreateWidget();
	SetWidgetPos();
	InitWidget();	
}
LRESULT TShiftCondition::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMHDR        lpnmh = (LPNMHDR)lParam;
	if (lpnmh->code == UDN_DELTAPOS && lpnmh->idFrom == ID_SPINTRIGGERPRICE)
	{
		char ShiftNo[51] = { 0 };
		m_editContractNo.GetText(ShiftNo, sizeof(ShiftNo));
		char cPrice[21] = { 0 };
		m_sEditTriggerPrice.GetText(cPrice, sizeof(cPrice));
		double Price = atof(cPrice);
		int iPrecision = 0;
		double MiniPrice = g_pShiftPosition->GetMiniPrice(ShiftNo, iPrecision, m_UserNo, m_UserNo2);
		if (((LPNMUPDOWN)lParam)->iDelta > 0)
			Price += MiniPrice;
		else
			Price -= MiniPrice;
		DoubleToChar(iPrecision, Price, cPrice);
		m_sEditTriggerPrice.SetWindowTextW(cPrice);
	}
	return NOT_PROCESSED;
}
LRESULT TShiftCondition::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		TCHAR UserNo[21];
		if (LOWORD(wParam) == ID_CBUSERNO)
		{
			m_cbUserNo.GetText(UserNo);
			strcpy_s(m_UserNo, sizeof(m_UserNo), LoadRC::unicode_to_ansi(UserNo).c_str());
		}
		else if (LOWORD(wParam) == ID_CBUSERNO2)
		{
			m_cbUserNo2.GetText(UserNo);
			strcpy_s(m_UserNo2, sizeof(m_UserNo2), LoadRC::unicode_to_ansi(UserNo).c_str());
		}

		else if (LOWORD(wParam) == ID_CBDIRECT)
		{
			if (m_cbDirect.GetSelect() < 2)
				m_labOpenQty.SetText("平仓数");
			else
				m_labOpenQty.SetText("开仓数");
		}
	}
	return NOT_PROCESSED;
}
LRESULT TShiftCondition::OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	default:
		break;
	}
	return NOT_PROCESSED;
}
void TShiftCondition::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BTNOK:
		OnBtnOk();
		break;
	case ID_BTNCANCEL:
		CloseWnd();
		break;
	default:
		break;
	}
}
bool TShiftCondition::CheckOrderPrice(double OrderPrice, ShiftListInfo shiftInfo)
{
	ShiftTriggerConditon iCondition = m_cbCondition.GetSelect();
	ShiftPriceCondition  iPrice = m_cbPrice.GetSelect();
	SDirect iDirect;
	if (m_cbDirect.GetSelect() == 0)
		iDirect = BuyCoverSellOpen;
	else if(m_cbDirect.GetSelect() == 1)
		iDirect = SellCoverBuyOpen;
	else if (m_cbDirect.GetSelect() == 2)
		iDirect = BuyOpenSellCover;
	else
		iDirect = SellOpenBuyCover;
	if (iCondition == tcEqualLarger)
	{
		if (iDirect == BuyCoverSellOpen || iDirect == BuyOpenSellCover)
		{
			if ((iPrice == pcLastPrice && shiftInfo.LastDif >= OrderPrice) || (iPrice == pcCounterPrice && shiftInfo.Long_CounterDif >= OrderPrice)
				|| (iPrice == pcQueuePrice && shiftInfo.Long_QueueDif >= OrderPrice))
			{
				_TMessageBox_Domodal(m_Hwnd, "提示", L"触发价已经触发，请重新设置", TMB_OK);
				return false;
			}
		}	
		else if(iDirect == SellCoverBuyOpen || iDirect == SellOpenBuyCover)
		{
			if ((iPrice == pcLastPrice && shiftInfo.LastDif >= OrderPrice) || (iPrice == pcCounterPrice && shiftInfo.Short_CounterDif >= OrderPrice)
				|| (iPrice == pcQueuePrice && shiftInfo.Short_QueueDif >= OrderPrice))
			{
				_TMessageBox_Domodal(m_Hwnd, "提示", L"触发价已经触发，请重新设置", TMB_OK);
				return false;
			}
		}
	}
	else
	{
		if (iDirect == BuyCoverSellOpen || iDirect == BuyOpenSellCover)
		{
			if ((iPrice == pcLastPrice && shiftInfo.LastDif <= OrderPrice) || (iPrice == pcCounterPrice && shiftInfo.Long_CounterDif <= OrderPrice)
				|| (iPrice == pcQueuePrice && shiftInfo.Long_QueueDif <= OrderPrice))
			{
				_TMessageBox_Domodal(m_Hwnd, "提示", L"触发价已经触发，请重新设置", TMB_OK);
				return false;
			}
		}
		else if (iDirect = SellCoverBuyOpen || iDirect == SellOpenBuyCover)
		{
			if ((iPrice == pcLastPrice && shiftInfo.LastDif <= OrderPrice) || (iPrice == pcCounterPrice && shiftInfo.Short_CounterDif <= OrderPrice)
				|| (iPrice == pcQueuePrice && shiftInfo.Short_QueueDif <= OrderPrice))
			{
				_TMessageBox_Domodal(m_Hwnd, "提示", L"触发价已经触发，请重新设置", TMB_OK);
				return false;
			}
		}
	}
	return true;
}
void TShiftCondition::OnBtnOk()
{
	ShiftCondition Condition;
	memset(&Condition, 0, sizeof(ShiftCondition));

	//获取相关信息，更新shiftListInfo；
	std::map<string, ShiftListInfo>::iterator iter = m_pmapShiftListInfo->find(m_cshiftContractNo);
	if (iter != m_pmapShiftListInfo->end())
	{
		//先判断价格是否合理
		char TriggerPrice[21] = { 0 };
		m_sEditTriggerPrice.GetText(TriggerPrice, sizeof(TriggerPrice));
		double OrderPrice = atof(TriggerPrice);
		if (CheckOrderPrice(OrderPrice, iter->second))
		{
			memcpy(Condition.ShiftContractNo, iter->second.ShiftContractNo, sizeof(Condition.ShiftContractNo));
			//2017-01-17
			//Condition.OrderQty1 = iter->second.OrderQty1;
			//Condition.OrderQty2 = iter->second.OrderQty2;
			if (m_cbDirect.GetSelect() <= 1)
			{
				if (m_cbDirect.GetSelect() == 0)
					Condition.iDirect = BuyCoverSellOpen;
				else
					Condition.iDirect = SellCoverBuyOpen;
				Condition.Firstclose = true;
			}
			else
			{
				if (m_cbDirect.GetSelect() == 2)
					Condition.iDirect = BuyOpenSellCover;
				else
					Condition.iDirect = SellOpenBuyCover;
				Condition.Firstclose = false;
			}
			//记录添加条件单时的下单逻辑
			if (g_pstConfig->m_bSingle)
			{
				Condition.TotalQty = m_sEditOpenQty.GetWindowNumber();
				Condition.OrderQty1 = iter->second.OrderQty1;
				Condition.OrderQty2 = iter->second.OrderQty2;
			}
			else
			{
				Condition.TotalQty = 0;
				Condition.OrderQty1 = m_sEditOpenQty.GetWindowNumber();
				//计算开仓数量
				Condition.OrderQty2 = Condition.OrderQty1 * iter->second.OrderQty2 / iter->second.OrderQty1;
			}
			//手数检查
			if (!g_pShiftPosition->QtyCheck(Condition.OrderQty1,Condition.OrderQty2,Condition.TotalQty,Condition.Firstclose))
				return;

			TCHAR UserNo[21] = { 0 };
			TCHAR UserNo2[21] = { 0 };
			m_cbUserNo.GetText(UserNo);
			m_cbUserNo2.GetText(UserNo2);
			memcpy(Condition.UserNo ,LoadRC::unicode_to_ansi(UserNo).c_str(), sizeof(Condition.UserNo));
			memcpy(Condition.UserNo2, LoadRC::unicode_to_ansi(UserNo2).c_str(), sizeof(Condition.UserNo2));
			std::map<string, TUserInfo*>::iterator Useriter = m_pstUserInfo->find(LoadRC::unicode_to_ansi(UserNo));
			std::map<string, TUserInfo*>::iterator Useriter2 = m_pstUserInfo->find(LoadRC::unicode_to_ansi(UserNo2));
			if (Useriter == m_pstUserInfo->end() || Useriter2 == m_pstUserInfo->end())
				return;
			memcpy(Condition.Sign, Useriter->second->Sign, sizeof(Condition.Sign));
			memcpy(Condition.Sign2, Useriter2->second->Sign, sizeof(Condition.Sign2));
			memcpy(Condition.Code1, iter->second.Code1, sizeof(Condition.Code1));
			memcpy(Condition.Code2, iter->second.Code2, sizeof(Condition.Code2));
			Condition.OrderPrice = atof(TriggerPrice);
			Condition.iPriceCondition = m_cbPrice.GetSelect();
			Condition.iTriggerCondition = m_cbCondition.GetSelect();

			//获取投保类型
			if (g_pstConfig->m_stChasePrice.Hedge == cxSpeculate)
				Condition.Hedge = hSpeculate;
			else
				Condition.Hedge = hHedge;
			
			Condition.OrderType1 = m_cbFirstPrice.GetSelect();
			Condition.OrderType2 = m_cbSecondPrice.GetSelect();
			Condition.OrderState = osRun;

			
			//插入数据；
			std::map<int, ShiftCondition>::iterator ConditionIter = m_pmapShiftCondition->find(m_iOrderId);
			if (ConditionIter == m_pmapShiftCondition->end())
			{
				Condition.OrderId = m_giOrderId++;
				m_pmapShiftCondition->insert(make_pair(Condition.OrderId, Condition));	
				iter->second.OrderId.push_back(Condition.OrderId);
			}
			else
			{
				m_pmapShiftCondition->erase(ConditionIter);
				Condition.OrderId = m_iOrderId;
				m_pmapShiftCondition->insert(make_pair(m_iOrderId, Condition));
			}
			m_iOrderId = -1;
			g_pShiftPosition->ShowAllCondition();
			CloseWnd();
		}	
	}
	else
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"移仓列表没有该合约", TMB_OK);
	}	
}



void TShiftCondition::DrawMain(TMemDC& pmemdc)
{

}
void TShiftCondition::CreateWidget()
{
	//UserNo
	m_labUserNo.Create(m_Hwnd, L"账号", 0, ID_LABUSERNO);
	m_cbUserNo.Create(m_Hwnd, ID_CBUSERNO);

	//UserNo2
	m_labUserNo2.Create(m_Hwnd, L"账号", 0, ID_LABUSERNO2);
	m_cbUserNo2.Create(m_Hwnd, ID_CBUSERNO2);

	//ContractNo;
	m_labContractNo.Create(m_Hwnd, L"合约号:", 0, ID_LABCONTRACTNO);
	m_editContractNo.Create(m_Hwnd, 0, 0, ID_EDITCONTRACTNO);

	//Condition
	m_labCondition.Create(m_Hwnd, L"条件:", 0, ID_LABCONDITON);
	m_cbPrice.Create(m_Hwnd, ID_CBPRICE);
	m_cbCondition.Create(m_Hwnd, ID_CBCONDITION);
	m_sEditTriggerPrice.Create(m_Hwnd, 0, ID_EDITTRIGGERPRICE, ID_SPINTRIGGERPRICE, false);

	//Direct;
	m_labDirect.Create(m_Hwnd, L"方向:", 0, ID_LABDIRECT);
	m_cbDirect.Create(m_Hwnd, ID_CBDIRECT);

	//开仓手数
	m_labOpenQty.Create(m_Hwnd, L"平仓数", 0, ID_LAB_OPENQTY);
	m_sEditOpenQty.Create(m_Hwnd, 0, ID_EDIT_OPENQTY, ID_SPIN_OPENQTY, true);

	//PriceType
	m_labFirstPrice.Create(m_Hwnd, L"第一腿:", 0, ID_LABFIRSTPRICE);
	m_cbFirstPrice.Create(m_Hwnd, ID_CBFIRSTPRICE);
	m_labSecondPrice.Create(m_Hwnd, L"第二腿:", 0, ID_LABSECONDPRICE);
	m_cbSecondPrice.Create(m_Hwnd, ID_CBSECONDPRICE);

	//Btn
	m_btnCancel.Create(m_Hwnd, "取消", ID_BTNCANCEL);
	m_btnOk.Create(m_Hwnd, "确定", ID_BTNOK);

	//Strategy
	m_cbStrategy.Create(m_Hwnd, ID_STRATEGY);
}
void TShiftCondition::InitWidget()
{
	//Edit Text 15font
	m_editContractNo.SetFont(g_FontData.GetFontWord15());
	m_cbCondition.SetFont(g_FontData.GetFontWord15());
	m_cbUserNo.SetFont(g_FontData.GetFontWord15());
	m_cbUserNo2.SetFont(g_FontData.GetFontWord15());
	//Edit Qty 13font
	m_sEditTriggerPrice.SetFont(g_FontData.GetFontWord13());
	m_sEditOpenQty.SetFont(g_FontData.GetFontWord13());

	m_cbPrice.AddString(L"最新价差");
	m_cbPrice.AddString(L"对价价差");
	m_cbPrice.AddString(L"排队价差");

	m_cbCondition.AddString(L">=");
	m_cbCondition.AddString(L"<=");

	m_cbDirect.AddString(L"买平-卖开");
	m_cbDirect.AddString(L"卖平-买开");
	m_cbDirect.AddString(L"买开-卖平");
	m_cbDirect.AddString(L"卖开-买平");

	m_cbFirstPrice.AddString(L"排队价");
	m_cbFirstPrice.AddString(L"对手价");
	m_cbFirstPrice.AddString(L"超价");
	m_cbFirstPrice.AddString(L"市价");
	m_cbFirstPrice.AddString(L"最新价");

	m_cbSecondPrice.AddString(L"排队价");
	m_cbSecondPrice.AddString(L"对手价");
	m_cbSecondPrice.AddString(L"超价");
	m_cbSecondPrice.AddString(L"市价");
	m_cbSecondPrice.AddString(L"最新价");

	m_cbStrategy.AddString(L"默认策略");
	m_cbStrategy.AddString(L"螺旋策略");

	m_cbUserNo.SetSelect(0);
	m_cbUserNo2.SetSelect(0);
	m_cbPrice.SetSelect(0);
	m_cbCondition.SetSelect(0);
	m_cbDirect.SetSelect(0);
	m_cbFirstPrice.SetSelect(1);
	m_cbSecondPrice.SetSelect(1);
	m_cbStrategy.SetSelect(0);

	EnableWindow(m_editContractNo.GetHwnd(), false);

}
void TShiftCondition::InitWidgetData(bool IsModify)
{
	//放交易账号
	std::map<string, TUserInfo*>::iterator Useriter = m_pstUserInfo->begin();
	m_cbUserNo.Clear();
	m_cbUserNo2.Clear();
	for (; Useriter != m_pstUserInfo->end(); Useriter++)
	{
		m_cbUserNo.AddString(LoadRC::ansi_to_unicode(Useriter->first.c_str()).c_str());
		m_cbUserNo2.AddString(LoadRC::ansi_to_unicode(Useriter->first.c_str()).c_str());
	}
		
	//账号
	if (strcmp(m_UserNo, "") == 0)
	{
		m_cbUserNo.SetSelect(0);
		m_cbUserNo2.SetSelect(0);
	}
	else
	{
		m_cbUserNo.SetSelect(LoadRC::ansi_to_unicode(m_UserNo).c_str());
		m_cbUserNo2.SetSelect(LoadRC::ansi_to_unicode(m_UserNo2).c_str());
	}
	//合约号
	EnableWindow(m_editContractNo.GetHwnd(), true);
	m_editContractNo.SetText(m_cshiftContractNo);
	EnableWindow(m_editContractNo.GetHwnd(), false);

	char Price[20] = { 0 };
	std::map<string, ShiftListInfo>::iterator iter = m_pmapShiftListInfo->find(m_cshiftContractNo);
	if (!IsModify)
	{
		if (iter != m_pmapShiftListInfo->end())
		{
			if (iter->second.LinkCondition == LcDiv)
				DoubleToChar(4, iter->second.LastDif, Price);
			else
			{
				int Presicion = 0;
				g_pShiftPosition->GetMiniPrice(m_cshiftContractNo, Presicion, m_UserNo, m_UserNo2);
				DoubleToChar(Presicion, iter->second.LastDif, Price);
			}
			m_sEditTriggerPrice.SetWindowTextW(Price);
			//2017-01-17
			m_sEditOpenQty.SetWindowNumber(iter->second.OrderQty1);
		}
	}
	else
	{
		std::map<int, ShiftCondition>::iterator iter = m_pmapShiftCondition->find(m_iOrderId);
		if (iter != m_pmapShiftCondition->end())
		{
			std::vector<string> vtContract = CConvert::SplitString(iter->second.ShiftContractNo, ' ');
			if (atoi(vtContract.at(4).c_str()) == LcDiv)
				DoubleToChar(4, iter->second.OrderPrice, Price);
			else
			{
				int Presicion = 0;
				g_pShiftPosition->GetMiniPrice(m_cshiftContractNo, Presicion, m_UserNo, m_UserNo2);
				DoubleToChar(Presicion, iter->second.OrderPrice, Price);
			}
			m_sEditTriggerPrice.SetWindowTextW(Price);
		}
		m_cbPrice.SetSelect(iter->second.iPriceCondition);
		m_cbCondition.SetSelect(iter->second.iTriggerCondition);
		m_cbDirect.SetSelect(iter->second.iDirect);
		m_cbFirstPrice.SetSelect(iter->second.OrderType1);
		m_cbSecondPrice.SetSelect(iter->second.OrderType2);
		//2017-01-17
		m_sEditOpenQty.SetWindowNumber(iter->second.OrderQty1);
	}
	return;
}
void TShiftCondition::SetWidgetPos()
{
	int ilabWidgetWidth = 50;
	int iWidgetWidth = 100;
	int iWidgetHeight = 20;
	int xVap = 10;
	int yVap = 10;
	POINT pt;

	//账号
	pt.x = xVap; pt.y = 30 + yVap;
	m_labUserNo.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
	pt.x += ilabWidgetWidth + xVap;
	m_cbUserNo.MoveWindow(pt.x, pt.y, 2 * iWidgetWidth, 5*iWidgetHeight);

	//账号2
	pt.x = xVap; pt.y += iWidgetHeight + yVap;
	m_labUserNo2.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
	pt.x += ilabWidgetWidth + xVap;
	m_cbUserNo2.MoveWindow(pt.x, pt.y, 2 * iWidgetWidth, 5 * iWidgetHeight);

	//合约号
	pt.x = xVap; pt.y += iWidgetHeight + yVap;
	m_labContractNo.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
	pt.x += ilabWidgetWidth + xVap;
	m_editContractNo.MoveWindow(pt.x, pt.y, 2 * iWidgetWidth, iWidgetHeight);

	//条件
	pt.x = xVap; pt.y += iWidgetHeight + yVap;
	m_labCondition.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
	pt.x += ilabWidgetWidth + xVap;
	m_cbPrice.MoveWindow(pt.x, pt.y, iWidgetWidth, 4 * iWidgetHeight);
	pt.x += iWidgetWidth + xVap;
	m_cbCondition.MoveWindow(pt.x, pt.y, ilabWidgetWidth, 3 * iWidgetHeight);
	pt.x += ilabWidgetWidth + xVap;
	m_sEditTriggerPrice.MoveWindow(pt.x, pt.y + 2, iWidgetWidth, iWidgetHeight);

	//订单
	pt.x = xVap; pt.y += iWidgetHeight + yVap;
	m_labDirect.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
	pt.x += xVap + ilabWidgetWidth;
	m_cbDirect.MoveWindow(pt.x, pt.y, iWidgetWidth, 5 * iWidgetHeight);

	//平仓手数
	pt.x += xVap + iWidgetWidth;
	m_labOpenQty.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
	pt.x += xVap + ilabWidgetWidth;
	m_sEditOpenQty.MoveWindow(pt.x, pt.y, iWidgetWidth, iWidgetHeight);

	//第一腿
	pt.x = xVap; pt.y += iWidgetHeight + yVap;
	m_labFirstPrice.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
	pt.x += ilabWidgetWidth + xVap; 
	m_cbFirstPrice.MoveWindow(pt.x, pt.y, iWidgetWidth, 6 * iWidgetHeight);

	//第二腿
	pt.x += iWidgetWidth + xVap; 
	m_labSecondPrice.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
	pt.x += ilabWidgetWidth + xVap;
	m_cbSecondPrice.MoveWindow(pt.x, pt.y, iWidgetWidth, 6 * iWidgetHeight);

	//平仓手数
// 	pt.x = xVap; pt.y += iWidgetHeight + yVap;
// 	m_labOpenQty.MoveWindow(pt.x, pt.y, ilabWidgetWidth, iWidgetHeight);
// 	pt.x += xVap + ilabWidgetWidth;
// 	m_sEditOpenQty.MoveWindow(pt.x, pt.y, iWidgetWidth, iWidgetHeight);

	//确定btn
	pt.y += iWidgetHeight + 2*yVap;
	m_btnOk.MoveWindow(pt.x, pt.y, 45, iWidgetHeight);
	pt.x += ilabWidgetWidth + xVap;
	m_btnCancel.MoveWindow(pt.x, pt.y, 45, iWidgetHeight);
}
//double TShiftCondition::GetMiniPrice(char* ShiftContracNo, int& iPresicion)
//{
//	vector<string> vtCode = CConvert::SplitString(ShiftContracNo, ' ');
//	if (vtCode.size() < 7)
//		return 0;
//	if (strcmp(vtCode.at(3).c_str(), "/") == 0)
//	{
//		iPresicion = 4;
//		return 0.0001;
//	}
//		
//	std::map<string, ShiftListInfo>::iterator iter = m_pmapShiftListInfo->find(ShiftContracNo);
//	if (iter == m_pmapShiftListInfo->end())
//	{
//		_TMessageBox_Domodal(m_Hwnd, "提示", L"移仓列表没有该合约！", TMB_OK);
//		return 1;
//	}
//
//
//	TContractKey stContract[2];
//	memset(&stContract, 0, sizeof(TContractKey));
//	std::map<std::string, TUserInfo*>::iterator UserIter = m_pstUserInfo->find(m_UserNo);
//	std::map<std::string, TUserInfo*>::iterator UserIter2 = m_pstUserInfo->find(m_UserNo2);
//	if (UserIter == m_pstUserInfo->end())
//	{
//		_TMessageBox_Domodal(m_Hwnd, "提示", L"没有该用户！", TMB_OK);
//		return 1;
//	}
//	memcpy(stContract[0].Sign, UserIter->second->Sign, sizeof(stContract[0].Sign));
//	CConvert::StrToTradeContract(iter->second.Code1, stContract[0]);
//	const TCommodity* pst = g_pTradeData->GetCommodity(&stContract[0]);
//	if (!pst)
//		return 0;
//	double MiniPrice1 = 0.0;
//	if (pst->LowerTick == 0)//最小变动价是1
//		MiniPrice1 = 1;
//	else if (pst->LowerTick == 1)//最小变动价是UpperTick
//		MiniPrice1 = pst->UpperTick;
//	int iPresicion1 = pst->Precision;
//
//	CConvert::StrToTradeContract(iter->second.Code2, stContract[0]);
//	pst = g_pTradeData->GetCommodity(&stContract[0]);
//	if (!pst)
//		return 0;
//	double MiniPrice2 = 0.0;
//	if (pst->LowerTick == 0)//最小变动价是1
//		MiniPrice2 = 1;
//	else if (pst->LowerTick == 1)//最小变动价是UpperTick
//		MiniPrice2 = pst->UpperTick;
//	int iPresicion2 = pst->Precision;
//	iPresicion = iPresicion1 > iPresicion2 ? iPresicion1 : iPresicion2;
//	return (MiniPrice1) < (MiniPrice2) ? (MiniPrice1) : (MiniPrice2);
//}
