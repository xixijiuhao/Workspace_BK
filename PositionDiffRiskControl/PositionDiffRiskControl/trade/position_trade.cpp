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
extern vPosDifCfg	vecTmp;				//��������

//���ô���
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

//���ӳɹ�
void __stdcall PositonTrade::OnConnect() {}
//���ӶϿ�
void __stdcall PositonTrade::OnDisconnect() {}
//��¼Ӧ��
void __stdcall PositonTrade::OnLogin(const SUserNoType user, const SErrCodeType err) {}
//��ʼ����ɣ��ڶ��ε�¼���ٳ�ʼ����
void __stdcall PositonTrade::OnInitComplete()
{
	//�û���֤
	if (!flagAuthent)
	{
		UserAuthentEx stAuthent;

		memset(&stAuthent, 0, sizeof(UserAuthentEx));
		bool bRet = g_pAuthent->GetAuthent("HXQH", "pd2017", stAuthent);

		if ( stAuthent.serverdate < stAuthent.auth.value[0].date)
		//if (stAuthent.serverdate != stAuthent.auth.value[0].date)
		{
			//ע��ͨ��ģ����
			g_common_api = (ICommonModule *)g_plugin_mgr->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);
			if (g_common_api)
			{
				g_common_api->Regist(dynamic_cast<ICommonInteractRiskCheck*>(this), InteractType_RiskCheck);
			}
			_ASSERT(g_common_api);

			//ע�����ô��ڲ��
			g_config_api = (IConfigFrame*)g_plugin_mgr->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);
			_ASSERT(g_config_api);
			if (g_config_api)
			{
				MENU_ID = g_config_api->reg_config_dlg(CConvert::LoadResString(IDS_Subtitle).c_str(), (dlg_create_func)CreateConfigWnd, cmtExpand, 0);
			}

			//ע������ܲ��
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
	//��ȡ�û���¼��Ϣ
	m_UserInfo = g_pTradeData->GetUser(TSOrder.UserNo, TSOrder.Sign);
	m_LoginUser = m_UserInfo->pLogin;

	//��ȡ�ֲ����ֵ
	bool hasCode = getPosDiffMax(&TSOrder, diffMax);

	////ͨ����������ȡ��ֵ
	////static const SConfigSwitchNoType				S_CONFIGNO_EQUITYTHRESHOLD = "PositionDif.EquityThreshold";//�ֲ��� Ȩ����ֵ ��0-3200�� ��λ��
	//SConfigSwitchInfo* temp[1];
	//g_StarApi->GetConfigSwitchInfo(S_CONFIGNO_EQUITYTHRESHOLD, temp, 1, false);
	//moneyThreshold = temp[0]->ConfigSwitchValue;
	//moneyThreshold = moneyThreshold * 10000;										//��λ��Ԫ

	////�ͻ���Ȩ��
	//const TMoney * pMoney = g_pTradeData->GetMoney(TSOrder.UserNo, nullptr, "CNY", TSOrder.Sign);
	//if(pMoney)
	//	equity = pMoney->PreEquity;	

	////�ж��������ú�Լ�����޴˺�Լ����true,�д˺�Լ�Ļ�����ִ��
	//string codeKey;
	//codeKey.append(TSOrder.ExchangeNo);
	//codeKey.append(1, ' ');
	//codeKey.append(TSOrder.CommodityNo);
	//codeKey.append(1, ' ');
	//codeKey.append(TSOrder.ContractNo);
	//bool hasCode = false;

	//for (vPosDifCfg::iterator it = vecTmp.begin(); it != vecTmp.end(); it++)
	//{
	//	if (strcmp(it->Code, codeKey.c_str()) != 0)			//�����
	//		continue;
	//	else
	//	{
	//		//��ȡ�ֲ����ֵ
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

	//��������Ʒ�������ù����ֵĺ�Լ��ֻ��ƽ�򣬲���ƽ��
	if (strcmp(TSOrder.ExchangeNo, exSHFE) == 0)
	{
		if (TSOrder.Offset == oCoverT)		//ƽ���
		{
			eError.nErrorLevel = EL_Error;
			StrCpyW(eError.error_text, CConvert::LoadResString(IDS_NoCoverT).c_str());
			return false;
		}
	}
	const TContractSingleKey contractKey = (TContractSingleKey)TSOrder;
	//��ʼ��
	buyPosition		= 0;
	sellPosition	= 0;
	buyOpenParQty	= 0;
	sellOpenParQty	= 0;
	buyCoverParQty	= 0;
	sellCoverParQty	= 0;
	//��ȡ�ֲ���
	//const TPosition * tPosition
	if(g_pTradeData->GetPositionTotal(TSOrder.UserNo, &contractKey, dBuy))
		buyPosition = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &contractKey, dBuy)->PositionQty;
	if(g_pTradeData->GetPositionTotal(TSOrder.UserNo, &contractKey, dSell))
		sellPosition = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &contractKey, dSell)->PositionQty;

	
	const TOrderQty * orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &contractKey);
	if (orderQty)
	{
		//��ȡ�򿪹ҵ�������
		buyOpenParQty = orderQty->BuyOpenQty;
		//��ȡ�����ҵ�������
		sellOpenParQty = orderQty->SellOpenQty;
		//��ȡ��ƽ�ҵ���������ƽ�� + ƽ��
		buyCoverParQty = orderQty->BuyCoverQty + orderQty->BuyCoverDayQty;
		//��ȡ��ƽ�ҵ���������ƽ�� + ƽ��
		sellCoverParQty = orderQty->SellCoverQty + orderQty->SellCoverDayQty;
	}

	if (TSOrder.Direct == dBuy && TSOrder.Offset == oOpen)
	{
		//��
		singleDiff = abs(buyOpenParQty + (long)TSOrder.OrderQty + sellCoverParQty);
		diff = abs((buyPosition + buyOpenParQty + (long)TSOrder.OrderQty - sellCoverParQty) - (sellPosition + sellOpenParQty - buyCoverParQty));
	}
	else if(TSOrder.Direct == dBuy && TSOrder.Offset == oCover)
	{
		//��ƽ
		singleDiff = abs(sellOpenParQty  + buyCoverParQty + (long)TSOrder.OrderQty);
		diff = abs((buyPosition + buyOpenParQty  - sellCoverParQty) - (sellPosition + sellOpenParQty - buyCoverParQty - (long)TSOrder.OrderQty));
	}
	else if (TSOrder.Direct == dSell && TSOrder.Offset == oOpen)
	{
		//����
		singleDiff = abs(sellOpenParQty + (long)TSOrder.OrderQty + buyCoverParQty);
		diff = abs((buyPosition + buyOpenParQty - sellCoverParQty) - (sellPosition + sellOpenParQty + (long)TSOrder.OrderQty - buyCoverParQty ));
	}
	else if (TSOrder.Direct == dSell && TSOrder.Offset == oCover)
	{
		//��ƽ
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

			wchar_t		beyondLimit[101];		//�ֲ���ֵwstr
			swprintf_s(beyondLimit, sizeof(beyondLimit) / 2, CConvert::LoadResString(IDS_RestingOrderBeyondLimit).c_str(), L"\r\n", 
				buyOpenParQty, L"\r\n", sellOpenParQty, L"\r\n", buyCoverParQty, L"\r\n", sellCoverParQty, L"\r\n",diffMax);
			StrCpyW(eError.error_text, beyondLimit);

			return false;
		}
		if (diff > diffMax)
		{
			eError.nErrorLevel = EL_Error;
			wchar_t		beyondLimit[101];		//�ֲ���ֵwstr
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
	//�ɳ�����ID
	std::set<int>  canCanelOrder;
	//���ɳ�����ID
	std::set<int>  cannotCanelOrder;
	//��ȡ�ҵ�
	Datas<const TOrder*> OrderData;
	g_pTradeData->GetParOrder(*(IDataContainer*)&OrderData, ftNone);
	//����(��������Ȩ)
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
			//��ͬһ�û�ͬһ��Լ
			if (strcmp(tempGroupOrder->conSingleKey().c_str(), groupStartOrder->conSingleKey().c_str()) == 0 &&
				strcmp(tempGroupOrder->UserNo, groupStartOrder->UserNo) == 0
				)
			{
				tempSetSameContractNo.insert(*it);
				vecOrderId.erase(*it);
			}
		}

		//��ӵ�map
		map_UserContract_OrderID.insert(std::map<pair<std::string, std::string>, std::set<int>>::value_type(make_pair(groupStartOrder->UserNo, groupStartOrder->conSingleKey()), tempSetSameContractNo));
	}

	bool canCancelAllOrder = TRUE;
	//ÿһ���ж�
	for (auto iter = map_UserContract_OrderID.begin(); iter != map_UserContract_OrderID.end(); iter++)
	{
		bool underPosDif = isUnderPosDif(iter->second);
		
		if (underPosDif)
		{
			//�ɳ�
			for (auto it : iter->second)
			{
				canCanelOrder.insert(it);
			}
		}
		else
		{
			for (auto iter_orderID : iter->second)
			{
				//���ֲ�,�Ƿ��ж�Ӧ��
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

//�ж��Ƿ���ڲֲ�,�����ú�Լ�������ֲ��true���򷵻�false, δ���ú�Լ����true
bool PositonTrade::isUnderPosDif(std::set<int> &OrderID)
{
	uint				diff;									//�ֲ�
	uint				diffMax = 0;							//�ֲ����ֵ
	long				buyPosition =0;							//��ֲ�
	long				sellPosition =0;						//���ֲ�
	long				buyOpenParQty =0;						//�򿪹ҵ���
	long				sellOpenParQty =0;						//�����ҵ���
	long				buyCoverParQty =0;						//��ƽ�ҵ���
	long				sellCoverParQty =0;						//��ƽ�ҵ���
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
	//��ȡ�ֲ���
	if (g_pTradeData->GetPositionTotal(numOrder->UserNo, &contractKey, dBuy))
		buyPosition = g_pTradeData->GetPositionTotal(numOrder->UserNo, &contractKey, dBuy)->PositionQty;
	if (g_pTradeData->GetPositionTotal(numOrder->UserNo, &contractKey, dSell))
		sellPosition = g_pTradeData->GetPositionTotal(numOrder->UserNo, &contractKey, dSell)->PositionQty;

	const TOrderQty * orderQty = g_pTradeData->GetParOrderQty(numOrder->UserNo, &contractKey);
	if (orderQty)
	{
		//��ȡ�򿪹ҵ�������
		buyOpenParQty = orderQty->BuyOpenQty;
		//��ȡ�����ҵ�������
		sellOpenParQty = orderQty->SellOpenQty;
		//��ȡ��ƽ�ҵ���������ƽ�� + ƽ��
		buyCoverParQty = orderQty->BuyCoverQty + orderQty->BuyCoverDayQty;
		//��ȡ��ƽ�ҵ���������ƽ�� + ƽ��
		sellCoverParQty = orderQty->SellCoverQty + orderQty->SellCoverDayQty;
	}

	//��ȡ�ֲ�
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

//��ȡ�ֲ����ֵ������true˵�������ã�����δ����
bool PositonTrade::getPosDiffMax(const TSendOrder* TSOrder, uint& diffMax)
{
	//ͨ����������ȡ��ֵ
	SConfigSwitchInfo* temp[1];
	g_pCspAccessApi->GetConfigSwitchInfo(S_CONFIGNO_EQUITYTHRESHOLD, temp, 1, false);
	moneyThreshold = temp[0]->ConfigSwitchValue;
	moneyThreshold = moneyThreshold * 10000;										//��λ��Ԫ

	//�ͻ���Ȩ��
	const TMoney * pMoney = g_pTradeData->GetMoney(TSOrder->UserNo, nullptr, "CNY", TSOrder->Sign);
	if (pMoney)
		equity = pMoney->PreEquity;

	//�ж��������ú�Լ�����޴˺�Լ����true,�д˺�Լ�Ļ�����ִ��
	string codeKey;
	codeKey.append(TSOrder->ExchangeNo);
	codeKey.append(1, ' ');
	codeKey.append(TSOrder->CommodityNo);
	codeKey.append(1, ' ');
	codeKey.append(TSOrder->ContractNo);
	bool hasCode = false;

	for (vPosDifCfg::iterator it = vecTmp.begin(); it != vecTmp.end(); it++)
	{
		if (strcmp(it->Code, codeKey.c_str()) != 0)			//�����
			continue;
		else
		{
			//��ȡ�ֲ����ֵ
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


//�Ƿ��ж�Ӧ��
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

	int halfY = -1;	//Ҫ������־
	int halfD = -1;	//��Ӧ����־

	for (uint i = 0; i < OrderData.size(); i++)
	{
		tempParOrder = (TOrder*)(OrderData.at(i));
		//����Ҫ������־
		if (strcmp(tempParOrder->OrderNo, tempOrder->OrderNo) == 0)
		{
			halfY = i;
		}
		//���Ҷ�Ӧ����־
		if (strcmp(tempParOrder->UserNo, tempOrder->UserNo) == 0 &&
			strcmp(tempParOrder->conSingleKey().c_str(), tempOrder->conSingleKey().c_str()) == 0 &&
			tempParOrder->Direct == (tempOrder->Direct == dBuy ? dSell : dBuy) &&
			tempParOrder->Offset == tempOrder->Offset &&
			tempParOrder->OrderQty == tempOrder->OrderQty)
		{
			//���ҵ���Ӧ��
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

		//������־���ҵ�break
		if (halfY >= 0 && halfD >= 0)
		{
			break;
		}

	}
	//����ж�Ӧ������ӹҵ���ɾ��Ҫ�����Ͷ�Ӧ��
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
	else	//��������ֲ���޶�Ӧ���ɳ�
	{
		cannotCanelOrder.insert(OrderID);
		return FALSE;
	}
}


//��ʾ���ɳ�������Ϣ
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