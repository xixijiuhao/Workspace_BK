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

//�µ�ʱ������вֲ��ؼ�⣬���µ�֮���|��ͷ-��ͷ|���ڲֲ�������µ�
//�ͻ�����ʱҲ����вֲ��ؼ�⣬������֮���|��ͷ-��ͷ|���ڲֲ���ṩ��������ѡ�� 
//��������������������֮���|��ͷ-��ͷ|������ڲֲ�������� 
//�����෴���������һ�ʿɳ��ҵ�����������֮���|��ͷ-��ͷ|������ڲֲ����ͬ�����������һ�ʿɳ��ҵ�һͬ���������û�пɳ��ҵ���ͬ����������
//����ƽ�ֹҵ����ܺ�С�ڲֲ�������ѡ��|����+ƽ��|�ҵ���|����+ƽ��|�ҵ����������ܳ����ֲ�


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
		UserAuthentEx tmpAuthent;
		if (g_authent->GetAuthent(AUTHENT_COMPANY, AUTHENT_KEY, tmpAuthent))
		{
			auto curUserIter = g_pPositionDiffRiskMgtWnd->m_UserNoAndUserInfoMap.find(tmpAuthent.auth.user);
			if (curUserIter != g_pPositionDiffRiskMgtWnd->m_UserNoAndUserInfoMap.end())
			{
				g_pPositionDiffRiskMgtWnd->m_MainGroupUserCfg = tmpAuthent.auth.user;
				g_pPositionDiffRiskMgtWnd->m_bCurUserCanModify = curUserIter->second.OperateRight;
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

//�ֲ���
bool PositionDiffRiskMgt::PositionRangeCheck(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
	std::string tCode;
	if (!CConvert::TContractKeyToTContractNo(TSOrder, tCode))
	{
		return true;
	}

	// �ж��Ƿ������˲ֲ���
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

//�������
bool PositionDiffRiskMgt::CancelPositionRangeCheck(std::set<int>& vecOrderId, char * userNo)
{
	/*if (g_pPositionDiffRiskMgtWnd->m_cfgData.m_bIsCommonModel)
		return true;*/
	// �ж��Ƿ������˲ֲ���
	if (g_pPositionDiffRiskMgtWnd->m_cfgData.m_ContractPosiRangeMap.empty())
	{
		return true;
	}

	//�ɳ�����ID
	std::set<int>  canCanelOrder;
	//���ɳ�����ID
	std::set<int>  cannotCanelOrder;
	//����keyΪ<UserNO,Contract> valueΪOrderID <<UserNO,Contract>, OrderID>
	UserNOContract_OrderIDMAP  map_UserContract_OrderID;

	std::set<int> tempSetOrderID;
	//����ѭ������vecOrderId���ҳ�ͬһ�û�/ͬһ��Լ�����ж���
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
	//������к�̨��������Ч�ҵ�
	Datas<const TOrder*> OrderData;
	g_pTradeData->GetParOrder(*(IDataContainer*)&OrderData, ftNone);
	std::set<int> consideCanelOrder;
	bool canCancelAllOrder = false;
	wchar_t errorText[101] = L"";
	//ÿһ�˻���ÿ����Լ�Ķ�����һ�ж�
	for (auto iter = map_UserContract_OrderID.begin(); iter != map_UserContract_OrderID.end(); iter++)
	{
		//�жϺ�Լ�Ƿ񳬹��ֲ�
		bool bLessPositionRange = CheckPositionLessRange(iter->second, errorText);
		if (bLessPositionRange)
		{
			//�ɳ�
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
				//todo ���������Ĵ���
				swprintf_s(errorText, LoadResWchar_t(IDS_PositionOutofRangeForbidCancle), m_CurContractKey.c_str());
				MessageBox(NULL, errorText, LoadResWchar_t(IDS_Tips), MB_OK);
				return false;
			}
			for (auto iter_orderID : iter->second)
			{
				//�����Ѿ����ֲ�,�鿴�Ƿ��ж�Ӧ��
				canCancelAllOrder = CheckConsideOrder(iter_orderID, canCanelOrder, cannotCanelOrder, consideCanelOrder, OrderData);
			}
		}
	}
	return DealCanCancelOrder(vecOrderId, canCancelAllOrder, canCanelOrder, cannotCanelOrder, consideCanelOrder);
}

bool PositionDiffRiskMgt::DealCanCancelOrder(std::set<int>& vecOrderId,bool &canCancelAllOrder, 
	std::set<int> &canCanelOrder, std::set<int> &cannotCanelOrder, std::set<int> &consideCanelOrder)
{
	//û�г����ֲ���Գ�������ֱ�ӳ���
	if (canCancelAllOrder && consideCanelOrder.size() == 0)
	{
		vecOrderId.clear();
		for (auto it : canCanelOrder)
		{
			vecOrderId.insert(it);
		}
		return true;
	}
	//�����ֲ�����ҵ��˷���ɳ����ģ���ʾȷ��֮����
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
	//�洢ͬһ�û�ͬһ��Լ�����ж�����
	std::set<int> tempSetSameContractNo;

	//ɾ����Ч�Ķ���
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

	//ѭ������
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

		//��ͬһ�û�ͬһ��Լ�����ж����Ŵ���Set
		if (strcmp(groupCode.c_str(), tmpCode.c_str()) == 0 &&
			strcmp(groupStartOrder->UserNo, tempGroupOrder->UserNo) == 0
			)
		{
			tempSetSameContractNo.insert(*it);
			vecOrderId.erase(*it);
		}
	}

	//��ӵ�map
	map_UserContract_OrderID.insert(std::map<pair<std::string, std::string>, std::set<int>>::value_type(make_pair(groupStartOrder->UserNo, groupCode), tempSetSameContractNo));
}

bool PositionDiffRiskMgt::DealPositionRangeCheck(const TSendOrder & TSOrder, ErrorReflectInfo & eError)
{
	//�ݲ��������������Լ����
	if (TSOrder.CommodityType == ctSpreadCommodity || TSOrder.CommodityType == ctSpreadMonth)
	{
		/*TContractSingleKey key[2];
		memset(&key, 0, sizeof(TContractSingleKey) * 2);
		CombTContractKeyToTContractSingleKey(TSOrder, key[0], key[1]);

		return CheckPositionSingleKey(key[0], eError, TSOrder.Direct, TSOrder) &&
			CheckPositionSingleKey(key[1], eError, TSOrder.Direct == dBuy ? dSell : dBuy, TSOrder);*/

		return true;
	}
	//ֻ������Լ
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
		//�ӽ���API��ȡ�����û��Ĺҵ��ϼ�
		const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &TSOrder);
		int nBuyCount = 0;
		int	nSellCount = 0;
		CalBuyPositionAndSellPosition(nBuyCount, nSellCount, TSOrder);
		int singelDirectCount = 0;	
		int curPositionRange = 0;
		CalCurPosiRangeAndSglDirectPosi(nBuyCount, nSellCount, curPositionRange, singelDirectCount, TSOrder);

		//�жϿͻ���ǰ�µ����Ƿ񳬹��ֲ�
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
	//�ӽ���API��ȡ�����û�����ĳֲֺϼ� �ٵõ���ֲ���
	const TPosition *buyPositon = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &TSOrder, dBuy);
	//�ӽ���API��ȡ�����û�������ĳֲֺϼ� �ٵõ����ֲ���
	const TPosition *sellPositon = g_pTradeData->GetPositionTotal(TSOrder.UserNo, &TSOrder, dSell);
	//�ӽ���API��ȡ�����û��Ĺҵ��ϼ�
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
		//��ͷ = ��ͷ�ֲ� + �࿪�ҵ� - ��ƽ�ҵ�
		nBuyCount = nBuyCount + orderQty->BuyOpenQty - orderQty->SellCoverQty - orderQty->SellCoverDayQty;
		if (nBuyCount < 0)
		{
			nBuyCount = 0;
		}
		//��ͷ = ��ͷ�ֲ� + �տ��� - ��ƽ�ҵ�
		nSellCount = nSellCount + orderQty->SellOpenQty - orderQty->BuyCoverDayQty - orderQty->BuyCoverQty;
		if (nSellCount < 0)
		{
			nSellCount = 0;
		}
	}
}

void PositionDiffRiskMgt::CalCurPosiRangeAndSglDirectPosi(int &nBuyCount, int &nSellCount, int &curRange, int &singelDirectCount, const TSendOrder &TSOrder)
{
	//�ӽ���API��ȡ�����û��Ĺҵ��ϼ�
	const TOrderQty *orderQty = g_pTradeData->GetParOrderQty(TSOrder.UserNo, &TSOrder);
	if (!orderQty)
	{
		return;
	}
	//��鵱ǰ�ͻ����µ����
	if (TSOrder.Direct == dBuy)
	{
		//�� �������ͷ
		if (TSOrder.Offset == oOpen)
		{
			nBuyCount += TSOrder.OrderQty;
			singelDirectCount = abs(long(orderQty->BuyOpenQty + orderQty->SellCoverQty + orderQty->SellCoverDayQty + TSOrder.OrderQty));
		}
		//��ƽ �������ͷ
		else
		{
			nSellCount -= TSOrder.OrderQty;
			singelDirectCount = abs(long(orderQty->SellOpenQty + orderQty->BuyCoverQty + orderQty->BuyCoverDayQty + TSOrder.OrderQty));
		}
	}

	if (TSOrder.Direct == dSell)
	{
		//���� �������ͷ  =���ӿ�
		if (TSOrder.Offset == oOpen)
		{
			nSellCount += TSOrder.OrderQty;
			singelDirectCount = abs(long(orderQty->SellOpenQty+ orderQty->BuyCoverQty + orderQty->BuyCoverDayQty + TSOrder.OrderQty));
		}
		//��ƽ �������ͷ
		else
		{
			nBuyCount -= TSOrder.OrderQty;
			singelDirectCount = abs(long(orderQty->BuyOpenQty + orderQty->SellCoverQty + orderQty->SellCoverDayQty + TSOrder.OrderQty));
		}
	}
	curRange = abs(nBuyCount - nSellCount);
}

//�ж��Ƿ���ڲֲ�,�����ú�Լ�������ֲ��true���򷵻�false, δ���ú�Լ����true
bool PositionDiffRiskMgt::CheckPositionLessRange(std::set<int> &OrderID, wchar_t *errorText)
{
	uint				nCurRange;							//��ǰ�ֲ�
	uint				nRange = 0;							//�ֲ����ֵ
	long				buyPosition = 0;					//��ֲ�
	long				sellPosition = 0;					//���ֲ�
	long				buyOpenParQty = 0;					//�򿪹ҵ���
	long				sellOpenParQty = 0;					//�����ҵ���
	long				buyCoverParQty = 0;					//��ƽ�ҵ���
	long				sellCoverParQty = 0;				//��ƽ�ҵ���
	long				cancelBuy = 0;						//��ǰҪ����������ҵ���
	long				cancelSell = 0;						//��ǰҪ�����������ҵ���

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
	//��ȡ�ֲ���
	TContractSingleKey contractKey = (TContractSingleKey)*numOrder;
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

//�Ƿ��ж�Ӧ��
bool PositionDiffRiskMgt::CheckConsideOrder(int OrderID, std::set<int>& canCanelOrder, std::set<int>& cannotCanelOrder,
	std::set<int>& consideCanelOrder, Datas<const TOrder*>& OrderData)
{
	//tempOrder ��ǰ����
	const TOrder* tempOrder = g_pTradeData->GetOrder(OrderID);
	if (!tempOrder)
	{
		cannotCanelOrder.insert(OrderID);
		return false;
	}

	TOrder * tempParOrder = new TOrder;
	int curCancelOrderNum = -1;	//Ҫ������־
	int conOrderNum = -1;	//��Ӧ����־

	//�������йҵ��뵱ǰ����ƥ��
	FindCurCancelOrderAndConOrder(OrderData, tempParOrder, tempOrder, curCancelOrderNum, conOrderNum, canCanelOrder,consideCanelOrder);

	//����ж�Ӧ������ӹҵ���ɾ��Ҫ�����Ͷ�Ӧ��
	if (conOrderNum >= 0 && curCancelOrderNum >=0)
	{
		//vector ��ɾ������ģ���ɾ��ǰ���
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
	else	//��������ֲ���޶�Ӧ���ɳ�
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
		//���ҵ�ǰҪ���������ж����е�λ��
		if (strcmp(tempParOrder->OrderNo, tempOrder->OrderNo) == 0)
		{
			curCancelOrderNum = i;
		}
		//���Ҷ�Ӧ�������ж����е�λ�� �˻�/��Լ/��ƽ/������һ�£������෴��Ϊ��Ӧ��
		if (strcmp(tempParOrder->UserNo, tempOrder->UserNo) == 0 &&
			strcmp(tCode.c_str(), tempContractKeyCode.c_str()) == 0 &&
			tempParOrder->Direct == (tempOrder->Direct == dBuy ? dSell : dBuy) &&
			tempParOrder->OrderQty == tempOrder->OrderQty) //tempParOrder->Offset == tempOrder->Offset &&
		{
			//�ҵ���Ӧ����λ�ú���
			if (conOrderNum < 0)
			{
				canCanelOrder.insert(tempOrder->OrderID);
				consideCanelOrder.insert(tempParOrder->OrderID);
				conOrderNum = i;
			}
		}
		//����λ�ö��ҵ�break
		if (curCancelOrderNum >= 0 && conOrderNum >= 0)
		{
			break;
		}
	}
}

//��ʾ���ɳ�������Ϣ
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


