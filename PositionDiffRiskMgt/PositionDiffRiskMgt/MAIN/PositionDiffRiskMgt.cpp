#include "PreInclude.h"

extern ITradeData *g_pTradeData;
extern ICommonModule *g_pCommonModule;
extern IMainFrame *g_mainframe;
extern IConfigFrame	*g_conFrame;
extern IAuthent *g_authent;
extern IStarApi	*g_StarApi;
extern ITradeApi *g_pTradeApi;
extern ICspAccessApi *g_pCspAccessApi;
extern PositionDiffRiskMgtWnd *g_pPositionDiffRiskMgtWnd;
extern UINT					ConfigDlgMenuID;

//下单时都会进行仓差风控检测，若下单之后的|多头-空头|大于仓差，则不允许下单
//客户撤单时也会进行仓差风控检测，若撤单之后的|多头-空头|大于仓差，则提供两个撤单选项 
//【不允许撤单】：即撤单之后的|多头-空头|如果大于仓差，则不允许撤单 
//【撤相反方向的最早一笔可撤挂单】：即撤单之后的|多头-空头|如果大于仓差，则连同反方向的最早一笔可撤挂单一同撤单，如果没有可撤挂单，同样不允许撤单
//【开平仓挂单量总和小于仓差】：如果勾选则|开多+平多|挂单及|开空+平空|挂单数量都不能超过仓差


PositionDiffRiskMgt::PositionDiffRiskMgt()
{
	m_CurContractKey = L"";
}

HWND __stdcall GetCfgWnd(const wchar_t *subtitle, const HWND parent)
{
	if (wcscmp(subtitle, LoadResWchar_t(IDS_PositionDiff_Risk_Management)) == 0)
	{
		if (g_pPositionDiffRiskMgtWnd)
		{
			if (!IsWindow(g_pPositionDiffRiskMgtWnd->GetHwnd()))
			{
				g_pPositionDiffRiskMgtWnd->CreateCfgWnd(subtitle);
			}
			return g_pPositionDiffRiskMgtWnd->GetHwnd();
		}
	}
	return NULL;
}

void PositionDiffRiskMgt::OnInitComplete()
{
	if (g_authent && g_pPositionDiffRiskMgtWnd)
	{
		g_pPositionDiffRiskMgtWnd->QryConfigFinish();
		UserAuthentEx tmpAuthent;
		if (g_authent->GetAuthent(AUTHENT_COMPANY, AUTHENT_KEY, tmpAuthent))
		{
			string curUserNO = tmpAuthent.auth.user;
			int index = 0;
			while ((index = curUserNO.find(' ', index)) != string::npos)
			{
				curUserNO.erase(index, 1);
			}

			auto curUserIter = g_pPositionDiffRiskMgtWnd->m_UserNoAndUserInfoMap.find(curUserNO);
			if (curUserIter != g_pPositionDiffRiskMgtWnd->m_UserNoAndUserInfoMap.end())
			{
				g_pPositionDiffRiskMgtWnd->m_MainGroupUserCfg = curUserNO;
				g_pPositionDiffRiskMgtWnd->m_bCurUserCanModify = curUserIter->second.OperateRight == '1';
				g_pPositionDiffRiskMgtWnd->m_sCurUserInGroupNO = g_pPositionDiffRiskMgtWnd->m_UserNoAndUserInfoMap.begin()->second.GroupNo;
				if (ConfigDlgMenuID == 0)
				{
					g_pCommonModule->Regist((ICommonInteractRiskCheck*)this, InteractType_RiskCheck);
					g_pTradeApi->RegistRiskModule((IRiskModuleApi*)this, 0);
					ConfigDlgMenuID = g_conFrame->reg_config_dlg(LoadResWchar_t(IDS_PositionDiff_Risk_Management), (dlg_create_func)GetCfgWnd, cmtAdvance, 15, "esunny.epolestar.configframe");
				}
			}
			else
			{
				return;
			}
		
		//	bool hasRight = (g_pPositionDiffRiskMgtWnd->m_UserNoAndUserInfoMap.size() > 0);
		//	if (hasRight)
		//	{

		//	}
		//	else if (ConfigDlgMenuID != 0)
		//	{
		//		g_pCommonModule->UnRegist((ICommonInteractRiskCheck*)this, InteractType_RiskCheck);
		//		g_pTradeApi->UnRegistRiskModule((IRiskModuleApi*)this);
		//		g_conFrame->unreg_config_dlg(ConfigDlgMenuID);
		//		ConfigDlgMenuID = 0;
		//	}
		}
	}
}

//仓差检测
bool PositionDiffRiskMgt::PositionRangeCheck(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
	std::string tCode;
	if (!CConvert::TContractKeyToTContractNo(TSOrder, tCode))
	{
		return true;
	}

	// 判断是否设置了仓差风控
	auto mapIter = g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.find(tCode);
	if (mapIter == g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.end())
	{
		return true;
	}
	return DealPositionRangeCheck(TSOrder, eError);
}

int PositionDiffRiskMgt::CheckOrderOperation(const void *data, int nType)
{
	if (!g_pPositionDiffRiskMgtWnd->m_cfgData.m_bIsCommonModel) 
		return true;
	const TOrder *tOrder = (TOrder*)data;

	std::set<int> vecOrderId;
	int orderid = atoi(tOrder->OrderNo);
	vecOrderId.insert(orderid);
	int canCancel = (int)(!CancelPositionRangeCheck(vecOrderId, const_cast<char *>(tOrder->UserNo)));
	return canCancel;
}

//撤单检测
bool PositionDiffRiskMgt::CancelPositionRangeCheck(std::set<int>& vecOrderId, char * userNo)
{
	/*if (g_pPositionDiffRiskMgtWnd->m_cfgData.m_bIsCommonModel)
		return true;*/
	// 判断是否设置了仓差风控
	if (g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.empty())
	{
		return true;
	}

	//可撤订单ID
	std::set<int>  canCanelOrder;
	//不可撤订单ID
	std::set<int>  cannotCanelOrder;
	//分组key为<UserNO,Contract> value为OrderID <<UserNO,Contract>, OrderID>
	UserNOContract_OrderIDMAP  map_UserContract_OrderID;

	std::set<int> tempSetOrderID;
	//两层循环遍历vecOrderId，找出同一用户/同一合约的所有定单
	while (!vecOrderId.empty())
	{
		tempSetOrderID.clear();
		tempSetOrderID = vecOrderId;
		InitUserContract_OrderIDMap(vecOrderId, tempSetOrderID, map_UserContract_OrderID);
	}
	return DealCancelOrder(vecOrderId, map_UserContract_OrderID, canCanelOrder, cannotCanelOrder);
}

bool PositionDiffRiskMgt::DealCancelOrder(std::set<int>& vecOrderId, UserNOContract_OrderIDMAP &map_UserContract_OrderID, 
	std::set<int> &canCanelOrder, std::set<int> &cannotCanelOrder)
{
	//获得所有后台的所有有效挂单
	Datas<const TOrder*> OrderData;
	g_pTradeData->GetParOrder(*(IDataContainer*)&OrderData, ftNone);
	std::set<int> consideCanelOrder;
	bool canCancelAllOrder = false;
	wchar_t errorText[101] = L"";
	//每一账户的每个合约的定单逐一判断
	for (auto iter = map_UserContract_OrderID.begin(); iter != map_UserContract_OrderID.end(); iter++)
	{
		//判断合约是否超过仓差
		bool bLessPositionRange = CheckPositionLessRange(iter->second, errorText);
		if (bLessPositionRange)
		{
			//可撤
			for (auto it : iter->second)
			{
				canCanelOrder.insert(it);
			}
			canCancelAllOrder = true;
		}
		else
		{
			if (g_pPositionDiffRiskMgtWnd->m_cfgData.m_CancelSelType == Forbid_Cancel_Order)
			{
				//todo 不允许撤单的处理
				swprintf_s(errorText, LoadResWchar_t(IDS_PositionOutofRangeForbidCancle), m_CurContractKey.c_str());
				MessageBox(NULL, errorText, LoadResWchar_t(IDS_Tips), MB_OK);
				return false;
			}
			for (auto iter_orderID : iter->second)
			{
				//撤单已经超仓差,查看是否有对应单
				canCancelAllOrder = CheckConsideOrder(iter_orderID, canCanelOrder, cannotCanelOrder, consideCanelOrder, OrderData);
			}
		}
	}
	return DealCanCancelOrder(vecOrderId, canCancelAllOrder, canCanelOrder, cannotCanelOrder, consideCanelOrder);
}

bool PositionDiffRiskMgt::DealCanCancelOrder(std::set<int>& vecOrderId,bool &canCancelAllOrder, 
	std::set<int> &canCanelOrder, std::set<int> &cannotCanelOrder, std::set<int> &consideCanelOrder)
{
	//没有超过仓差，可以撤单的则直接撤单
	if (canCancelAllOrder && consideCanelOrder.size() == 0)
	{
		vecOrderId.clear();
		for (auto it : canCanelOrder)
		{
			vecOrderId.insert(it);
		}
		return true;
	}
	//超过仓差，但是找到了反向可撤单的，提示确认之后处理
	else if (canCancelAllOrder && consideCanelOrder.size() > 0)
	{
		wchar_t errorText[101];
		swprintf_s(errorText, sizeof(errorText) / 2, LoadResWchar_t(IDS_CancelOrderWithConsideOrder), m_CurContractKey.c_str());
		if (MessageBox(NULL, errorText, LoadResWchar_t(IDS_Tips), MB_YESNO) == IDYES)
		{
			vecOrderId.clear();
			for (auto it : canCanelOrder)
			{
				vecOrderId.insert(it);
			}
			for (auto it : consideCanelOrder)
			{
				vecOrderId.insert(it);
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		wchar_t errorText[101];
		swprintf_s(errorText,LoadResWchar_t(IDS_CancelOrderWarning), m_CurContractKey.c_str());
		MessageBox(NULL, errorText, LoadResWchar_t(IDS_Tips), MB_OK);
		return false;
	}
}

void PositionDiffRiskMgt::InitUserContract_OrderIDMap(std::set<int>& vecOrderId, std::set<int>& tempSetOrderID, 
	UserNOContract_OrderIDMAP &map_UserContract_OrderID)
{
	//存储同一用户同一合约的所有定单号
	std::set<int> tempSetSameContractNo;

	//删除无效的定单
	const TOrder* groupStartOrder = g_pTradeData->GetOrder(*(tempSetOrderID.begin()));
	if (!groupStartOrder)
	{
		vecOrderId.erase(*(tempSetOrderID.begin()));
		return;
	}
	std::string groupCode = GetContractKeyWithOrder(groupStartOrder);
	auto mapIter = g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.find(groupCode);
	if (groupCode == "" || mapIter == g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.end())
	{
		vecOrderId.erase(*(tempSetOrderID.begin()));
		return;
	}

	//循环遍历
	for (set<int>::iterator it = tempSetOrderID.begin(); it != tempSetOrderID.end(); it++)
	{
		const TOrder* tempGroupOrder = g_pTradeData->GetOrder(*it);
		if (!tempGroupOrder)
		{
			continue;
		}
		std::string tmpCode = GetContractKeyWithOrder(tempGroupOrder);
		if (tmpCode == "")
		{
			continue;
		}

		//是同一用户同一合约的所有定单号存入Set
		if (strcmp(groupCode.c_str(), tmpCode.c_str()) == 0 &&
			strcmp(groupStartOrder->UserNo, tempGroupOrder->UserNo) == 0
			)
		{
			tempSetSameContractNo.insert(*it);
			vecOrderId.erase(*it);
		}
	}

	//添加到map
	map_UserContract_OrderID.insert(std::map<pair<std::string, std::string>, std::set<int>>::value_type(make_pair(groupStartOrder->UserNo, groupCode), tempSetSameContractNo));
}

bool PositionDiffRiskMgt::DealPositionRangeCheck(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
	//暂不处理组合套利合约类型
	if (TSOrder.CommodityType == ctSpreadCommodity || TSOrder.CommodityType == ctSpreadMonth)
	{
		/*TContractSingleKey key[2];
		memset(&key, 0, sizeof(TContractSingleKey) * 2);
		CombTContractKeyToTContractSingleKey(TSOrder, key[0], key[1]);

		return CheckPositionSingleKey(key[0], eError, TSOrder.Direct, TSOrder) &&
			CheckPositionSingleKey(key[1], eError, TSOrder.Direct == dBuy ? dSell : dBuy, TSOrder);*/

		return true;
	}
	//只处理单合约
	else
	{
		return CheckPositionSingleKey(eError, TSOrder);
	}
}

bool PositionDiffRiskMgt::CheckPositionSingleKey(ErrorReflectInfo & eError,const TSendOrder &TSOrder)
{
	std::string tCode;
	if (!CConvert::PosContractToStr(tCode, TSOrder))
	{
		return true;
	}

	if (g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.find(tCode) != g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.end())
	{
		int nRange = g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap[tCode];
		if (nRange <= 0)
		{
			return true;
		}
		//从交易API获取到该用户的挂单合计
		const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &TSOrder);
		int nBuyCount = 0;
		int	nSellCount = 0;
		CalBuyPositionAndSellPosition(nBuyCount, nSellCount, TSOrder);
		int singelDirectCount = 0;	
		int curPositionRange = 0;
		CalCurPosiRangeAndSglDirectPosi(nBuyCount, nSellCount, curPositionRange, singelDirectCount, TSOrder);

		//判断客户当前下单后是否超过仓差
		if (curPositionRange > nRange)
		{
			eError.nErrorLevel = EL_Error;
			wcscpy_s(eError.error_text, LoadResWchar_t(IDS_Error_Position));
			return false;
		}

		if (singelDirectCount > nRange && g_pPositionDiffRiskMgtWnd->m_cfgData.m_bNeedLessPosiRag)
		{
			wchar_t beyondLimit[101];
			swprintf_s(beyondLimit,
				sizeof(beyondLimit) / 2,
				LoadResWchar_t(IDS_RestingOrderBeyondLimit), L"\r\n",
				orderQty->BuyOpenQty, L"\r\n",
				orderQty->SellOpenQty, L"\r\n",
				(orderQty->BuyCoverQty + orderQty->BuyCoverDayQty), L"\r\n",
				(orderQty->SellCoverQty + orderQty->SellCoverDayQty), L"\r\n",
				nRange);
			eError.nErrorLevel = EL_Error;
			wcscpy_s(eError.error_text, beyondLimit);
			return false;
		}
	}
	return true;
}

void PositionDiffRiskMgt::CalBuyPositionAndSellPosition(int &nBuyCount, int &nSellCount, const TSendOrder &TSOrder)
{
	//从交易API获取到该用户买方向的持仓合计 再得到买持仓量
	const TPosition *buyPositon = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &TSOrder, dBuy);
	//从交易API获取到该用户卖方向的持仓合计 再得到卖持仓量
	const TPosition *sellPositon = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &TSOrder, dSell);
	//从交易API获取到该用户的挂单合计
	const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &TSOrder);
	if (buyPositon)
	{
		nBuyCount = buyPositon->PositionQty;
	}
	if (sellPositon)
	{
		nSellCount = sellPositon->PositionQty;
	}
	if (orderQty)
	{
		//多头 = 多头持仓 + 多开挂单 - 多平挂单
		nBuyCount = nBuyCount + orderQty->BuyOpenQty - orderQty->SellCoverQty - orderQty->SellCoverDayQty;
		if (nBuyCount < 0)
		{
			nBuyCount = 0;
		}
		//空头 = 空头持仓 + 空开单 - 空平挂单
		nSellCount = nSellCount + orderQty->SellOpenQty - orderQty->BuyCoverDayQty - orderQty->BuyCoverQty;
		if (nSellCount < 0)
		{
			nSellCount = 0;
		}
	}
}

void PositionDiffRiskMgt::CalCurPosiRangeAndSglDirectPosi(int &nBuyCount, int &nSellCount, int &curRange, int &singelDirectCount, const TSendOrder &TSOrder)
{
	//从交易API获取到该用户的挂单合计
	const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &TSOrder);
	if (!orderQty)
	{
		return;
	}
	//检查当前客户的下单情况
	if (TSOrder.Direct == dBuy)
	{
		//买开 即补充多头
		if (TSOrder.Offset == oOpen)
		{
			nBuyCount += TSOrder.OrderQty;
			singelDirectCount = abs(long(orderQty->BuyOpenQty + orderQty->SellCoverQty + orderQty->SellCoverDayQty + TSOrder.OrderQty));
		}
		//买平 即补充空头
		else
		{
			nSellCount -= TSOrder.OrderQty;
			singelDirectCount = abs(long(orderQty->SellOpenQty + orderQty->BuyCoverQty + orderQty->BuyCoverDayQty + TSOrder.OrderQty));
		}
	}

	if (TSOrder.Direct == dSell)
	{
		//卖开 即补充空头  =》加空
		if (TSOrder.Offset == oOpen)
		{
			nSellCount += TSOrder.OrderQty;
			singelDirectCount = abs(long(orderQty->SellOpenQty+ orderQty->BuyCoverQty + orderQty->BuyCoverDayQty + TSOrder.OrderQty));
		}
		//卖平 即补充多头
		else
		{
			nBuyCount -= TSOrder.OrderQty;
			singelDirectCount = abs(long(orderQty->BuyOpenQty + orderQty->SellCoverQty + orderQty->SellCoverDayQty + TSOrder.OrderQty));
		}
	}
	curRange = abs(nBuyCount - nSellCount);
}

//判断是否低于仓差,已设置合约低于最大仓差返回true否则返回false, 未设置合约返回true
bool PositionDiffRiskMgt::CheckPositionLessRange(std::set<int> &OrderID, wchar_t *errorText)
{
	uint				nCurRange;							//当前仓差
	uint				nRange = 0;							//仓差最大值
	long				buyPosition = 0;					//买持仓
	long				sellPosition = 0;					//卖持仓
	long				buyOpenParQty = 0;					//买开挂单量
	long				sellOpenParQty = 0;					//卖开挂单量
	long				buyCoverParQty = 0;					//买平挂单量
	long				sellCoverParQty = 0;				//卖平挂单量
	long				cancelBuy = 0;						//当前要撤的所有买挂单量
	long				cancelSell = 0;						//当前要撤的所有卖挂单量

	for (auto it : OrderID)
	{
		const TOrder* tempOrder = g_pTradeData->GetOrder(it);
		if (!tempOrder)
		{
			continue;
		}
		if (tempOrder->Direct == dBuy)
		{
			cancelBuy += tempOrder->OrderQty;
		}
		if (tempOrder->Direct == dSell)
		{
			cancelSell += tempOrder->OrderQty;
		}
	}

	const TOrder * numOrder = g_pTradeData->GetOrder(*(OrderID.begin()));
	std::string tCode = GetContractKeyWithOrder(numOrder);
	if (tCode == "")
	{
		return true;
	}
	m_CurContractKey = LoadRC::ansi_to_unicode(tCode);
	//获取持仓量
	TContractSingleKey contractKey = (TContractSingleKey)*numOrder;
	if (g_pTradeData->GetPositionTotal(numOrder->UserNo, &contractKey, dBuy))
		buyPosition = g_pTradeData->GetPositionTotal(numOrder->UserNo, &contractKey, dBuy)->PositionQty;

	if (g_pTradeData->GetPositionTotal(numOrder->UserNo, &contractKey, dSell))
		sellPosition = g_pTradeData->GetPositionTotal(numOrder->UserNo, &contractKey, dSell)->PositionQty;

	const TOrderQty * orderQty = g_pTradeData->GetParOrderQty(numOrder->UserNo, &contractKey);
	if (orderQty)
	{
		//获取买开挂单的数量
		buyOpenParQty = orderQty->BuyOpenQty;
		//获取卖开挂单的数量
		sellOpenParQty = orderQty->SellOpenQty;
		//获取买平挂单的数量（平昨 + 平今）
		buyCoverParQty = orderQty->BuyCoverQty + orderQty->BuyCoverDayQty;
		//获取卖平挂单的数量（平昨 + 平今）
		sellCoverParQty = orderQty->SellCoverQty + orderQty->SellCoverDayQty;
	}

	//获取仓差
	if (g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.find(tCode) == g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.end())
	{
		return true;
	}
	else
	{
		nRange = g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap[tCode];
	}

	nCurRange = abs((buyPosition + buyOpenParQty + buyCoverParQty - cancelBuy) - (sellPosition + sellOpenParQty + sellCoverParQty - cancelSell));
	
	if ((nCurRange > nRange))
	{
		/*swprintf_s(errorText,
			sizeof(errorText) / 2 ,
			LoadResWchar_t(IDS_PositionOutofRange),
			LoadRC::ansi_to_unicode(tCode.c_str()),L"\r\n",
			L"\t", buyPosition, L"\r\n",
			L"\t", sellPosition, L"\r\n",
			L"\t", buyOpenParQty, L"\r\n",
			L"\t", sellOpenParQty, L"\r\n",
			L"\t", buyCoverParQty, L"\r\n",
			L"\t", sellCoverParQty, L"\r\n",
			L"\t", nRange);*/
		return false;
	}
	else
	{
		return true;
	}
}

//是否有对应单
bool PositionDiffRiskMgt::CheckConsideOrder(int OrderID, std::set<int>& canCanelOrder, std::set<int>& cannotCanelOrder,
	std::set<int>& consideCanelOrder, Datas<const TOrder*>& OrderData)
{
	//tempOrder 当前订单
	const TOrder* tempOrder = g_pTradeData->GetOrder(OrderID);
	if (!tempOrder)
	{
		cannotCanelOrder.insert(OrderID);
		return false;
	}

	TOrder * tempParOrder = new TOrder;
	int curCancelOrderNum = -1;	//要撤单标志
	int conOrderNum = -1;	//对应单标志

	//遍历所有挂单与当前定单匹配
	FindCurCancelOrderAndConOrder(OrderData, tempParOrder, tempOrder, curCancelOrderNum, conOrderNum, canCanelOrder,consideCanelOrder);

	//如果有对应单，则从挂单中删除要撤单和对应单
	if (conOrderNum >= 0 && curCancelOrderNum >=0)
	{
		//vector 先删除后面的，再删除前面的
		if (conOrderNum > curCancelOrderNum)
		{
			OrderData.erase(conOrderNum);
			OrderData.erase(curCancelOrderNum);
		}
		else
		{
			OrderData.erase(curCancelOrderNum);
			OrderData.erase(conOrderNum);
		}
		return true;
	}
	else	//如果超过仓差并且无对应单可撤
	{
		cannotCanelOrder.insert(OrderID);
		return false;
	}
}

void PositionDiffRiskMgt::FindCurCancelOrderAndConOrder(Datas<const TOrder*>& OrderData, TOrder * tempParOrder, const TOrder* tempOrder,
	int &curCancelOrderNum,int &conOrderNum, std::set<int>& canCanelOrder, std::set<int>& consideCanelOrder)
{
	std::string tempContractKeyCode = GetContractKeyWithOrder(tempOrder);
	if (tempContractKeyCode == "")
	{
		return;
	}
	for (uint i = 0; i < OrderData.size(); i++)
	{
		tempParOrder = (TOrder*)(OrderData.at(i));

		std::string tCode = GetContractKeyWithOrder(tempParOrder);
		if (tCode == "")
		{
			continue;
		}
		//查找当前要撤单在所有定单中的位置
		if (strcmp(tempParOrder->OrderNo, tempOrder->OrderNo) == 0)
		{
			curCancelOrderNum = i;
		}
		//查找对应单在所有定单中的位置 账户/合约/开平/手数都一致，方向相反即为对应单
		if (strcmp(tempParOrder->UserNo, tempOrder->UserNo) == 0 &&
			strcmp(tCode.c_str(), tempContractKeyCode.c_str()) == 0 &&
			tempParOrder->Direct == (tempOrder->Direct == dBuy ? dSell : dBuy) &&
			tempParOrder->OrderQty == tempOrder->OrderQty) //tempParOrder->Offset == tempOrder->Offset &&
		{
			//找到对应单的位置后复制
			if (conOrderNum < 0)
			{
				canCanelOrder.insert(tempOrder->OrderID);
				consideCanelOrder.insert(tempParOrder->OrderID);
				conOrderNum = i;
			}
		}
		//两个位置都找到break
		if (curCancelOrderNum >= 0 && conOrderNum >= 0)
		{
			break;
		}
	}
}

//提示不可撤订单信息
void PositionDiffRiskMgt::showCannotCancelOrder(std::set<int>& cannotCanelOrder)
{
	for (auto it : cannotCanelOrder)
	{
		const TOrder* tempOrder = g_pTradeData->GetOrder(it);
		if (!tempOrder)
		{
			continue;
		}

	}
}

std::string PositionDiffRiskMgt::GetContractKeyWithOrder(const TOrder* tOrder)
{
	const TContractSingleKey contractKey = (TContractSingleKey)*tOrder;
	std::string tCode = "";
	if (!CConvert::PosContractToStr(tCode, contractKey))
	{
		return tCode;
	}
	return tCode;
}


