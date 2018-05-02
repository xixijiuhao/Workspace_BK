#include "PreInclude.h"

const int ZC_MAX_POSITIONDIF	= 40;
const int J_JM_MAX_POSTIONDIF	= 10;

extern ITradeData *			g_pTradeData;
extern IStarApi *			g_StarApi;
extern IAuthent *			g_pAuthent;
extern bool					flagAuthent;
extern TPluginMgrApi *		g_plugin_mgr;
extern ICommonModule *		g_common_api;
extern IConfigFrame *		g_config_api;
extern IMainFrame *			g_main_api;
extern ICspAccessApi *		g_pCspAccessApi;
extern unsigned int			MENU_ID;

CPositonDiffWin					g_PosDiffWin;
CPositionDiffBar				g_PosDiffBar;
extern vPosDifCfg	vecTmp;				//配置容器

//配置窗口
HWND __stdcall CreateConfigWnd(const wchar_t * sub_title, const HWND parent)
{
	if (!IsWindow(g_PosDiffWin.GetHwnd()))
	{
		g_PosDiffWin.Create(sub_title, parent, WS_CHILD|WS_VSCROLL, 0);
		return g_PosDiffWin.GetHwnd();
	}
	else
		return nullptr;
}

PositonTrade::PositonTrade()
	:buyPosition(0), 
	sellPosition(0),
	buyOpenParQty(0),
	sellOpenParQty(0),
	buyCoverParQty(0),
	sellCoverParQty(0)
{
	g_PosDiffBar.Create(L"CEFC", NULL, WS_POPUP /*| WS_VISIBLE*/, WS_EX_TOOLWINDOW);
}

PositonTrade::~PositonTrade()
{

}

//连接成功
void __stdcall PositonTrade::OnConnect() {}
//连接断开
void __stdcall PositonTrade::OnDisconnect() {}
//登录应答
void __stdcall PositonTrade::OnLogin(const SUserNoType user, const SErrCodeType err) {}
//初始化完成（第二次登录不再初始化）
void __stdcall PositonTrade::OnInitComplete()
{
	//用户认证
	if (!flagAuthent)
	{
		UserAuthentEx stAuthent;

		memset(&stAuthent, 0, sizeof(UserAuthentEx));
		bool bRet = g_pAuthent->GetAuthent("HXQH", "pd2017", stAuthent);

		if ( stAuthent.serverdate < stAuthent.auth.value[0].date)
		//if (stAuthent.serverdate != stAuthent.auth.value[0].date)
		{
			//注册通用模块插件
			g_common_api = (ICommonModule *)g_plugin_mgr->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);
			if (g_common_api)
			{
				g_common_api->Regist(dynamic_cast<ICommonInteractRiskCheck*>(this), InteractType_RiskCheck);
			}
			_ASSERT(g_common_api);

			//注册配置窗口插件
			g_config_api = (IConfigFrame*)g_plugin_mgr->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);
			_ASSERT(g_config_api);
			if (g_config_api)
			{
				MENU_ID = g_config_api->reg_config_dlg(CConvert::LoadResString(IDS_Subtitle).c_str(), (dlg_create_func)CreateConfigWnd, cmtExpand, 0);
			}

			//注册主框架插件
			g_main_api = (IMainFrame*)g_plugin_mgr->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
			_ASSERT(g_main_api);
			if (g_main_api)
			{
				//Sleep(5000);
				bool flag = g_main_api->RegStatusBarItemEx(1, g_PosDiffBar.GetHwnd(), 2);
			}

			flagAuthent = true;
		}
	}
}

bool __cdecl PositonTrade::PositionRangeCheck(const TSendOrder &TSOrder, ErrorReflectInfo &eError)
{
	//获取用户登录信息
	m_UserInfo = g_pTradeData->GetUser(TSOrder.UserNo, TSOrder.Sign);
	m_LoginUser = m_UserInfo->pLogin;

	//获取仓差最大值
	bool hasCode = getPosDiffMax(&TSOrder, diffMax);

	////通过接入服务获取阈值
	////static const SConfigSwitchNoType				S_CONFIGNO_EQUITYTHRESHOLD = "PositionDif.EquityThreshold";//仓差监测 权益阈值 （0-3200） 单位万
	//SConfigSwitchInfo* temp[1];
	//g_StarApi->GetConfigSwitchInfo(S_CONFIGNO_EQUITYTHRESHOLD, temp, 1, false);
	//moneyThreshold = temp[0]->ConfigSwitchValue;
	//moneyThreshold = moneyThreshold * 10000;										//单位万元

	////客户昨权益
	//const TMoney * pMoney = g_pTradeData->GetMoney(TSOrder.UserNo, nullptr, "CNY", TSOrder.Sign);
	//if(pMoney)
	//	equity = pMoney->PreEquity;	

	////判断有无配置合约，若无此合约返回true,有此合约的话继续执行
	//string codeKey;
	//codeKey.append(TSOrder.ExchangeNo);
	//codeKey.append(1, ' ');
	//codeKey.append(TSOrder.CommodityNo);
	//codeKey.append(1, ' ');
	//codeKey.append(TSOrder.ContractNo);
	//bool hasCode = false;

	//for (vPosDifCfg::iterator it = vecTmp.begin(); it != vecTmp.end(); it++)
	//{
	//	if (strcmp(it->Code, codeKey.c_str()) != 0)			//不相等
	//		continue;
	//	else
	//	{
	//		//获取仓差最大值
	//		if (equity >= moneyThreshold)
	//			diffMax = it->nMax2Dif;
	//		if (equity < moneyThreshold && equity > 0)
	//			diffMax = it->nMax1Dif;
	//		hasCode = true;
	//		break;
	//	}
	//}
	//if (strcmp(TSOrder.CommodityNo, "ZC") == 0 || strcmp(TSOrder.CommodityNo, "J") == 0 || strcmp(TSOrder.CommodityNo, "JM") == 0)
	//{
	//	if (strcmp(TSOrder.CommodityNo ,"ZC")==0)
	//		diffMax = ZC_MAX_POSITIONDIF;
	//	if (strcmp(TSOrder.CommodityNo, "J") == 0 || strcmp(TSOrder.CommodityNo, "JM") == 0)
	//		diffMax = J_JM_MAX_POSTIONDIF;
	//	hasCode = true;
	//}
	if (hasCode == false)
		return TRUE;

	//对于上期品种且设置过限手的合约，只许平昨，不许平今。
	if (strcmp(TSOrder.ExchangeNo, exSHFE) == 0)
	{
		if (TSOrder.Offset == oCoverT)		//平今仓
		{
			eError.nErrorLevel = EL_Error;
			StrCpyW(eError.error_text, CConvert::LoadResString(IDS_NoCoverT).c_str());
			return false;
		}
	}
	const TContractSingleKey contractKey = (TContractSingleKey)TSOrder;
	//初始化
	buyPosition		= 0;
	sellPosition	= 0;
	buyOpenParQty	= 0;
	sellOpenParQty	= 0;
	buyCoverParQty	= 0;
	sellCoverParQty	= 0;
	//获取持仓量
	//const TPosition * tPosition
	if(g_pTradeData->GetPositionTotal(TSOrder.UserNo, &contractKey, dBuy))
		buyPosition = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &contractKey, dBuy)->PositionQty;
	if(g_pTradeData->GetPositionTotal(TSOrder.UserNo, &contractKey, dSell))
		sellPosition = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &contractKey, dSell)->PositionQty;

	
	const TOrderQty * orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &contractKey);
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

	if (TSOrder.Direct == dBuy && TSOrder.Offset == oOpen)
	{
		//买开
		singleDiff = abs(buyOpenParQty + (long)TSOrder.OrderQty + sellCoverParQty);
		diff = abs((buyPosition + buyOpenParQty + (long)TSOrder.OrderQty - sellCoverParQty) - (sellPosition + sellOpenParQty - buyCoverParQty));
	}
	else if(TSOrder.Direct == dBuy && TSOrder.Offset == oCover)
	{
		//买平
		singleDiff = abs(sellOpenParQty  + buyCoverParQty + (long)TSOrder.OrderQty);
		diff = abs((buyPosition + buyOpenParQty  - sellCoverParQty) - (sellPosition + sellOpenParQty - buyCoverParQty - (long)TSOrder.OrderQty));
	}
	else if (TSOrder.Direct == dSell && TSOrder.Offset == oOpen)
	{
		//卖开
		singleDiff = abs(sellOpenParQty + (long)TSOrder.OrderQty + buyCoverParQty);
		diff = abs((buyPosition + buyOpenParQty - sellCoverParQty) - (sellPosition + sellOpenParQty + (long)TSOrder.OrderQty - buyCoverParQty ));
	}
	else if (TSOrder.Direct == dSell && TSOrder.Offset == oCover)
	{
		//卖平
		singleDiff = abs(buyOpenParQty  + sellCoverParQty + (long)TSOrder.OrderQty);
		diff = abs((buyPosition + buyOpenParQty - sellCoverParQty - (long)TSOrder.OrderQty) - (sellPosition + sellOpenParQty - buyCoverParQty));
	}

	if (singleDiff <= diffMax && diff <= diffMax)
	{
		return true;
	}
	else 
	{
		if (singleDiff > diffMax) 
		{
			eError.nErrorLevel = EL_Error;

			wchar_t		beyondLimit[101];		//仓差阈值wstr
			swprintf_s(beyondLimit, sizeof(beyondLimit) / 2, CConvert::LoadResString(IDS_RestingOrderBeyondLimit).c_str(), L"\r\n", 
				buyOpenParQty, L"\r\n", sellOpenParQty, L"\r\n", buyCoverParQty, L"\r\n", sellCoverParQty, L"\r\n",diffMax);
			StrCpyW(eError.error_text, beyondLimit);

			return false;
		}
		if (diff > diffMax)
		{
			eError.nErrorLevel = EL_Error;
			wchar_t		beyondLimit[101];		//仓差阈值wstr
			swprintf_s(beyondLimit, 
				sizeof(beyondLimit) / 2, 
				CConvert::LoadResString(IDS_PDBeyondLimit).c_str(), L"\r\n", 
				L"\t", buyPosition, L"\r\n", 
				L"\t", sellPosition, L"\r\n",
				L"\t", buyOpenParQty, L"\r\n", 
				L"\t", sellOpenParQty, L"\r\n", 
				L"\t", buyCoverParQty, L"\r\n", 
				L"\t", sellCoverParQty, L"\r\n", 
				L"\t", diffMax);
			//StrCpyW(eError.error_text, CConvert::LoadResString(IDS_PDBeyondLimit).c_str());
			StrCpyW(eError.error_text, beyondLimit);
			return false;
		}
	}
	return false;
}

bool __cdecl PositonTrade::CancelPositionRangeCheck(std::set<int> &vecOrderId, char *userNo)
{
	//可撤订单ID
	std::set<int>  canCanelOrder;
	//不可撤订单ID
	std::set<int>  cannotCanelOrder;
	//获取挂单
	Datas<const TOrder*> OrderData;
	g_pTradeData->GetParOrder(*(IDataContainer*)&OrderData, ftNone);
	//分组(不考虑期权)
	std::map<pair<std::string, std::string>, std::set<int>> map_UserContract_OrderID;

	while (!vecOrderId.empty())
	{
		std::set<int> tempSetOrderID;
		tempSetOrderID.clear();
		tempSetOrderID = vecOrderId;
		
		std::set<int> tempSetSameContractNo;
		tempSetSameContractNo.clear();

		const TOrder* groupStartOrder = g_pTradeData->GetOrder(*(tempSetOrderID.begin()));
		if (!groupStartOrder)
		{
			vecOrderId.erase(*(tempSetOrderID.begin()));
			continue;
		}

		for (set<int>::iterator it = tempSetOrderID.begin(); it != tempSetOrderID.end(); it++)
		{
			const TOrder* tempGroupOrder = g_pTradeData->GetOrder(*it);
			if (!tempGroupOrder)
			{
				continue;
			}
			//是同一用户同一合约
			if (strcmp(tempGroupOrder->conSingleKey().c_str(), groupStartOrder->conSingleKey().c_str()) == 0 &&
				strcmp(tempGroupOrder->UserNo, groupStartOrder->UserNo) == 0
				)
			{
				tempSetSameContractNo.insert(*it);
				vecOrderId.erase(*it);
			}
		}

		//添加到map
		map_UserContract_OrderID.insert(std::map<pair<std::string, std::string>, std::set<int>>::value_type(make_pair(groupStartOrder->UserNo, groupStartOrder->conSingleKey()), tempSetSameContractNo));
	}

	bool canCancelAllOrder = TRUE;
	//每一组判断
	for (auto iter = map_UserContract_OrderID.begin(); iter != map_UserContract_OrderID.end(); iter++)
	{
		bool underPosDif = isUnderPosDif(iter->second);
		
		if (underPosDif)
		{
			//可撤
			for (auto it : iter->second)
			{
				canCanelOrder.insert(it);
			}
		}
		else
		{
			for (auto iter_orderID : iter->second)
			{
				//超仓差,是否有对应单
				bool rightOrder = hasRightOrder(iter_orderID, canCanelOrder, cannotCanelOrder, OrderData);
				canCancelAllOrder &= rightOrder;
			}
		}


	}
	if (canCancelAllOrder)
	{
		vecOrderId.clear();
		for (auto it : canCanelOrder)
		{
			vecOrderId.insert(it);
		}
		return TRUE;
	}
	else
	{
		if (MessageBox(NULL, CConvert::LoadResString(IDS_CancelOrderWarning).c_str(), CConvert::LoadResString(IDS_Tips).c_str(), MB_YESNO) == IDYES)
		{
			vecOrderId.clear();
			for (auto it : cannotCanelOrder)
			{
				vecOrderId.insert(it);
			}
			for (auto it : canCanelOrder)
			{
				vecOrderId.insert(it);
			}
			return TRUE;
		}
		else 
		{
			return FALSE;
		}
	}



}

//判断是否低于仓差,已设置合约低于最大仓差返回true否则返回false, 未设置合约返回true
bool PositonTrade::isUnderPosDif(std::set<int> &OrderID)
{
	uint				diff;									//仓差
	uint				diffMax = 0;							//仓差最大值
	long				buyPosition =0;							//买持仓
	long				sellPosition =0;						//卖持仓
	long				buyOpenParQty =0;						//买开挂单量
	long				sellOpenParQty =0;						//卖开挂单量
	long				buyCoverParQty =0;						//买平挂单量
	long				sellCoverParQty =0;						//卖平挂单量
	long				cancelBuy = 0;
	long				cancelSell = 0;

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
	const TContractSingleKey contractKey = (TContractSingleKey)*numOrder;
	//获取持仓量
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
	bool hasCode = getPosDiffMax((TSendOrder*)numOrder, *&diffMax);
	if (hasCode)
	{
		diff = abs((buyPosition + buyOpenParQty + buyCoverParQty - cancelBuy) - (sellPosition + sellOpenParQty + sellCoverParQty - cancelSell));
		if (diff > diffMax)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return TRUE;
	}

}

//获取仓差最大值，返回true说明已设置，否则未设置
bool PositonTrade::getPosDiffMax(const TSendOrder* TSOrder, uint& diffMax)
{
	//通过接入服务获取阈值
	SConfigSwitchInfo* temp[1];
	g_pCspAccessApi->GetConfigSwitchInfo(S_CONFIGNO_EQUITYTHRESHOLD, temp, 1, false);
	moneyThreshold = temp[0]->ConfigSwitchValue;
	moneyThreshold = moneyThreshold * 10000;										//单位万元

	//客户昨权益
	const TMoney * pMoney = g_pTradeData->GetMoney(TSOrder->UserNo, nullptr, "CNY", TSOrder->Sign);
	if (pMoney)
		equity = pMoney->PreEquity;

	//判断有无配置合约，若无此合约返回true,有此合约的话继续执行
	string codeKey;
	codeKey.append(TSOrder->ExchangeNo);
	codeKey.append(1, ' ');
	codeKey.append(TSOrder->CommodityNo);
	codeKey.append(1, ' ');
	codeKey.append(TSOrder->ContractNo);
	bool hasCode = false;

	for (vPosDifCfg::iterator it = vecTmp.begin(); it != vecTmp.end(); it++)
	{
		if (strcmp(it->Code, codeKey.c_str()) != 0)			//不相等
			continue;
		else
		{
			//获取仓差最大值
			if (equity >= moneyThreshold)
				diffMax = it->nMax2Dif;
			if (equity < moneyThreshold && equity > 0)
				diffMax = it->nMax1Dif;
			hasCode = true;
			break;
		}
	}
	if (strcmp(TSOrder->CommodityNo, "ZC") == 0 || strcmp(TSOrder->CommodityNo, "J") == 0 || strcmp(TSOrder->CommodityNo, "JM") == 0)
	{
		if (strcmp(TSOrder->CommodityNo, "ZC") == 0)
			diffMax = ZC_MAX_POSITIONDIF;
		if (strcmp(TSOrder->CommodityNo, "J") == 0 || strcmp(TSOrder->CommodityNo, "JM") == 0)
			diffMax = J_JM_MAX_POSTIONDIF;
		hasCode = true;
	}
	return hasCode;
}


//是否有对应单
bool PositonTrade::hasRightOrder(int OrderID, std::set<int>& canCanelOrder, std::set<int>& cannotCanelOrder, Datas<const TOrder*>& OrderData)
{

	const TOrder* tempOrder = g_pTradeData->GetOrder(OrderID);
	if (!tempOrder)
	{
		cannotCanelOrder.insert(OrderID);
		return FALSE;
	}

	TOrder * tempParOrder;
	BOOL hasSameOrder = FALSE;

	int halfY = -1;	//要撤单标志
	int halfD = -1;	//对应单标志

	for (uint i = 0; i < OrderData.size(); i++)
	{
		tempParOrder = (TOrder*)(OrderData.at(i));
		//查找要撤单标志
		if (strcmp(tempParOrder->OrderNo, tempOrder->OrderNo) == 0)
		{
			halfY = i;
		}
		//查找对应单标志
		if (strcmp(tempParOrder->UserNo, tempOrder->UserNo) == 0 &&
			strcmp(tempParOrder->conSingleKey().c_str(), tempOrder->conSingleKey().c_str()) == 0 &&
			tempParOrder->Direct == (tempOrder->Direct == dBuy ? dSell : dBuy) &&
			tempParOrder->Offset == tempOrder->Offset &&
			tempParOrder->OrderQty == tempOrder->OrderQty)
		{
			//已找到对应单
			if (hasSameOrder)
			{
				//do nothing
			}
			else
			{
				canCanelOrder.insert(OrderID);
				canCanelOrder.insert(tempParOrder->OrderID);
				hasSameOrder = TRUE;

				halfD = i;
			}

		}

		//两个标志都找到break
		if (halfY >= 0 && halfD >= 0)
		{
			break;
		}

	}
	//如果有对应单，则从挂单中删除要撤单和对应单
	if (hasSameOrder)
	{
		if (halfY > halfD)
		{
			OrderData.erase(halfY);
			OrderData.erase(halfD);
		}
		else
		{
			OrderData.erase(halfD);
			OrderData.erase(halfY);
		}
		return TRUE;
	}
	else	//如果超过仓差并且无对应单可撤
	{
		cannotCanelOrder.insert(OrderID);
		return FALSE;
	}
}


//提示不可撤订单信息
void PositonTrade::showCannotCancelOrder(std::set<int>& cannotCanelOrder)
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