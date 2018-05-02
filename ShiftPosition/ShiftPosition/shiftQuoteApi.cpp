#include "BaseInclude.h"

extern IStarApi* g_pStarApi;
extern TShiftPosition* g_pShiftPosition;
extern ShiftTradeApi*  g_pShiftTradeApi;
extern ShiftOrderConfig *g_pstConfig;

ShiftQuoteApi::ShiftQuoteApi()
{

}
ShiftQuoteApi::~ShiftQuoteApi()
{
	
}
bool ShiftQuoteApi::QuoteShiftContract(ShiftListInfo& ShiftContract, char* sign,char* sign2)
{

	if (!GetShiftContractNo(ShiftContract))
		return false;
	m_mapShiftContract->insert(make_pair(ShiftContract.ShiftContractNo, ShiftContract));
	m_multiQuoteContract.insert(make_pair(ShiftContract.Code1, ShiftContract));
	m_multiQuoteContract.insert(make_pair(ShiftContract.Code2, ShiftContract));
	//订阅行情;
	if (!SubQuote(ShiftContract.Code1, ShiftContract.Code2, sign, sign2)) {
		HWND hwnd = FindWindow(L"class TMainFrame", NULL);
		_TMessageBox_Domodal(hwnd, "提示", L"行情订阅失败");
		return false;
	}
	//抓取快照
	SContract stContract;
	memset(&stContract, 0, sizeof(stContract));
	char contractId[101] = {0};
	CConvert::StrToQContractNoType(ShiftContract.Code1,contractId);
	strcpy_s(stContract.ContractNo, sizeof(contractId), contractId);
	GetQuoteData(&stContract);

	g_pShiftPosition->UpdateShiftList();
	return true;
}
void ShiftQuoteApi::Regist(std::map<string, ShiftListInfo>* pShiftContract, std::map<int, ShiftCondition>* pShiftCondition)
{
	if (pShiftContract && pShiftCondition)
	{
		m_mapShiftContract = pShiftContract;
		m_pmapShiftCondition = pShiftCondition;
	}
}
void ShiftQuoteApi::GetQuoteData(SContract* pst)
{
	string QuoteIdNow = pst->ContractNo;
	string ContractCode;
	CConvert::QContractNoTypeToStr(QuoteIdNow, ContractCode);
	Range range = m_multiQuoteContract.equal_range(ContractCode);
	for (CIT iter = range.first; iter != range.second; iter++)
	{
		SQuoteSnapShot QuoteData[2];
		for (int i = 0; i < 2; i++)
			memset(&QuoteData[i], 0, sizeof(SQuoteSnapShot));

		//获取两腿数据数据;
		char QuoteId[2][101] = { 0 };
		if (strcmp(ContractCode.c_str(), iter->second.Code1) == 0)
		{
			strcpy_s(QuoteId[0], sizeof(pst->ContractNo), pst->ContractNo);
			CConvert::StrToQContractNoType(iter->second.Code2, QuoteId[1]);
		}
		else if (strcmp(ContractCode.c_str(), iter->second.Code2) == 0)
		{
			CConvert::StrToQContractNoType(iter->second.Code1, QuoteId[0]);
			strcpy_s(QuoteId[1], sizeof(pst->ContractNo), pst->ContractNo);
		}
		double Price[2][7] = { 0.0 };// 第一腿买价，卖家，最新价，均价，开盘，最高，最低；
		if (g_pStarApi && g_pStarApi->GetSnapshot(QuoteId[0], &QuoteData[0]) && g_pStarApi->GetSnapshot(QuoteId[1], &QuoteData[1]))
		{
			Price[0][buy] = QuoteData[0].Data[S_FID_BESTBIDPRICE].Price;
			Price[0][sell] = QuoteData[0].Data[S_FID_BESTASKPRICE].Price;
			Price[0][last] = QuoteData[0].Data[S_FID_LASTPRICE].Price;
			Price[0][average] = QuoteData[0].Data[S_FID_AVERAGEPRICE].Price;
			Price[0][opening] = QuoteData[0].Data[S_FID_OPENINGPRICE].Price;
			Price[0][high] = QuoteData[0].Data[S_FID_HIGHPRICE].Price;
			Price[0][low] = QuoteData[0].Data[S_FID_LOWPRICE].Price;

			Price[1][buy] = QuoteData[1].Data[S_FID_BESTBIDPRICE].Price;
			Price[1][sell] = QuoteData[1].Data[S_FID_BESTASKPRICE].Price;
			Price[1][last] = QuoteData[1].Data[S_FID_LASTPRICE].Price;
			Price[1][average] = QuoteData[1].Data[S_FID_AVERAGEPRICE].Price;
			Price[1][opening] = QuoteData[1].Data[S_FID_OPENINGPRICE].Price;
			Price[1][high] = QuoteData[1].Data[S_FID_HIGHPRICE].Price;
			Price[1][low] = QuoteData[1].Data[S_FID_LOWPRICE].Price;
			InitShiftDiffPrice(Price, iter->second);
			//更新列表数据
			std::map<string, ShiftListInfo>::iterator iterlist = m_mapShiftContract->find(iter->second.ShiftContractNo);
			if (iterlist != m_mapShiftContract->end())
			{
				UpdateShiftQuote(iterlist->second, iter->second);
				//处理条件单数据
				if (MonitorCondition(iterlist->second))
					g_pShiftPosition->ShowAllCondition(false);
				g_pShiftPosition->ShowOneShiftList(iterlist->second);
			}
		}
	}
}
void ShiftQuoteApi::DealOnQuote(WPARAM wParam)
{
	SContract* pst = (SContract*)wParam;
	if (!pst)
		return;
	GetQuoteData(pst);
}
bool ShiftQuoteApi::MonitorCondition(ShiftListInfo& shiftInfo)
{
	bool flag = false;
	double ComParePrice = 0.0;
	std::map<int, ShiftCondition>::iterator iter;
	//std::vector<int>::iterator Orderiter = shiftInfo.OrderId.begin();
	
	for (int i= 0; i < shiftInfo.OrderId.size(); i++)
	{
		iter = m_pmapShiftCondition->find(shiftInfo.OrderId.at(i));
		if (iter == m_pmapShiftCondition->end())
		{
			//shiftInfo.OrderId.erase(Orderiter++);
			continue;
		}
		if (iter->second.OrderState == osTrigger || iter->second.OrderState == osPause)
		{
			//Orderiter++;
			continue;
		}
			
		switch (iter->second.iPriceCondition)
		{
		case pcLastPrice:
			ComParePrice = shiftInfo.LastDif;
			break;
		case pcCounterPrice:
			if (iter->second.iDirect == BuyCoverSellOpen)
				ComParePrice = shiftInfo.Long_CounterDif;
			else
				ComParePrice = shiftInfo.Short_CounterDif;
			break;
		case pcQueuePrice:
			if (iter->second.iDirect == BuyCoverSellOpen)
				ComParePrice = shiftInfo.Long_QueueDif;
			else
				ComParePrice = shiftInfo.Short_QueueDif;
			break;
		}

		if ((iter->second.iTriggerCondition == tcEqualLarger && ComParePrice >= iter->second.OrderPrice) ||
			(iter->second.iTriggerCondition == tcEqualLess && ComParePrice <= iter->second.OrderPrice))
		{
				flag = true;
				iter->second.OrderState = osTrigger;
				memcpy(iter->second.Code1, shiftInfo.Code1, sizeof(iter->second.Code1));
				memcpy(iter->second.Code2, shiftInfo.Code2, sizeof(iter->second.Code2));
				SendConditionOrder(iter->second);	
		}
		//Orderiter++;
	}
	return flag;
}
bool ShiftQuoteApi::SendConditionOrder(ShiftCondition &Condition)
{
	ShiftSendOrder stOrder;
	memset(&stOrder, 0, sizeof(stOrder));
	stOrder.OrderId = -1;
	stOrder.TOrderId = -1;
	stOrder.ConditionOrderId = -1;
	memcpy(stOrder.UserNo, Condition.UserNo, sizeof(stOrder.UserNo));
	memcpy(stOrder.Sign, Condition.Sign, sizeof(stOrder.Sign));
	memcpy(stOrder.UserNo2, Condition.UserNo2, sizeof(stOrder.UserNo2));
	memcpy(stOrder.Sign2,Condition.Sign2,sizeof(stOrder.Sign2));
	memcpy(stOrder.ContractCode1, Condition.Code1, sizeof(stOrder.ContractCode1));
	memcpy(stOrder.ContractCode2, Condition.Code2, sizeof(stOrder.ContractCode2));
	stOrder.OrderQty1 = Condition.OrderQty1;
	stOrder.OrderQty2 = Condition.OrderQty2;
	stOrder.OrderPriceType1 = Condition.OrderType1;
	stOrder.OrderPriceType2 = Condition.OrderType2;
	stOrder.FirstColse = Condition.Firstclose;
	stOrder.TotalQty = Condition.TotalQty;
	stOrder.Hedge = Condition.Hedge;

	if (Condition.iDirect == BuyCoverSellOpen || Condition.iDirect == BuyOpenSellCover)
	{
		stOrder.Direct1 = dBuy;
		stOrder.Direct2 = dSell;
	}
	else
	{
		stOrder.Direct1 = dSell;
		stOrder.Direct2 = dBuy;
	}
	if (g_pstConfig->m_bCondition)							//如果没有勾选条件单撤单，触发后就当做普通单子
		stOrder.ConditionOrderId = Condition.OrderId;		//先赋值，后面处理ConditionOrderId不为-1表明是条件单触发的下单
	g_pShiftTradeApi->SendShiftOrder(stOrder);
	return true; 
}
void ShiftQuoteApi::InitShiftDiffPrice(double QuotePrice[2][7], ShiftListInfo& shiftInfo)
{
	int Condition[3] = { shiftInfo.LinkContract1, shiftInfo.LinkContract2, shiftInfo.LinkCondition };
	double Price[2][7] = { 0.0, 0.0, 0.0 }; //第一腿，第二腿
	int j = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			switch (Condition[i])
			{
			case LcMulti:
				Price[i][j] = QuotePrice[i][j] * shiftInfo.OrderQty1;
				break;
			case LcPlus:
				Price[i][j] = QuotePrice[i][j] + shiftInfo.OrderQty1;
				break;
			case LcSub:
				Price[i][j] = QuotePrice[i][j] - shiftInfo.OrderQty1;
				break;
			case LcDiv:
				Price[i][j] = QuotePrice[i][j] / shiftInfo.OrderQty1;
				break;
			default:
				break;
			}
		}	
	}
	switch ( shiftInfo.LinkCondition)
	{	
	case LcSub:
		shiftInfo.Long_CounterDif = Price[0][sell] - Price[1][buy];
		shiftInfo.Long_QueueDif = Price[0][buy] - Price[1][sell];
		shiftInfo.Short_CounterDif = Price[0][buy] - Price[1][sell];
		shiftInfo.Short_QueueDif = Price[0][sell] - Price[1][buy];
		shiftInfo.LastDif = Price[0][last] - Price[1][last];
		shiftInfo.AverageDif = Price[0][average] - Price[1][average];
		shiftInfo.OpenDif = Price[0][opening] - Price[1][opening];
		shiftInfo.Long_MarketDif = Price[0][high] - Price[1][high];
		shiftInfo.Short_MarketDif = Price[0][low] - Price[1][low];
		break;
	case LcDiv:
		shiftInfo.Long_CounterDif = Price[0][sell] / Price[1][buy];
		shiftInfo.Long_QueueDif = Price[0][buy] / Price[1][sell];
		shiftInfo.Short_CounterDif = Price[0][buy] / Price[1][sell];
		shiftInfo.Short_QueueDif = Price[0][sell] / Price[1][buy];
		shiftInfo.LastDif = Price[0][last] / Price[1][last];
		shiftInfo.AverageDif = Price[0][average] / Price[1][average];
		shiftInfo.OpenDif = Price[0][opening] / Price[1][opening];
		shiftInfo.Long_MarketDif = Price[0][high] / Price[1][high];
		shiftInfo.Short_MarketDif = Price[0][low] / Price[1][low];
		break;
	default:
		break;
	}
}

void ShiftQuoteApi::UpdateShiftQuote(ShiftListInfo& shiftQuote, ShiftListInfo &shiftInfo)
{
	shiftQuote.Long_CounterDif = shiftInfo.Long_CounterDif;
	shiftQuote.Long_QueueDif = shiftInfo.Long_QueueDif;
	shiftQuote.Short_CounterDif = shiftInfo.Short_CounterDif;
	shiftQuote.Short_QueueDif = shiftInfo.Short_QueueDif;
	shiftQuote.LastDif = shiftInfo.LastDif;
	shiftQuote.AverageDif = shiftInfo.AverageDif;
	shiftQuote.OpenDif = shiftInfo.OpenDif;
	shiftQuote.Long_MarketDif = shiftInfo.Long_MarketDif;
	shiftQuote.Short_MarketDif = shiftInfo.Short_MarketDif;
}

bool ShiftQuoteApi::SubQuote(string sCode1, string sCode2, char* Sign, char* Sign2)
{
	TContractKey contract1;
	TContractKey contract2;
	memset(&contract1, 0, sizeof(contract1));
	memset(&contract2, 0, sizeof(contract2));
	memcpy(contract1.Sign, Sign , sizeof(contract1.Sign));
	memcpy(contract2.Sign, Sign2, sizeof(contract2.Sign));
	if (!(CConvert::TContractNoToTContractKey(sCode1.c_str(), contract1) && CConvert::TContractNoToTContractKey(sCode2.c_str(), contract2)))
		return false;
	SContractNoType sQuote1 = { 0 };
	SContractNoType sQuote2 = { 0 };
	SContractNoType sQuote[2] = { 0 };
	CConvert::TradeContractToQContractNoType(contract1, sQuote[0]);
	CConvert::TradeContractToQContractNoType(contract2, sQuote[1]);
	int iQcount = 0;
	if (g_pStarApi)
	{
		int len = sizeof(sQuote) / sizeof(SContractNoType);
		iQcount = g_pStarApi->SubQuote(sQuote, len, g_pShiftPosition);
// 		SContract pst;
// 		memset(&pst, 0, sizeof(SContract));
// 		memcpy(pst.ContractNo, sQuote[0], sizeof(pst.ContractNo));
// 		GetQuoteData(&pst);
		return true;
	}
	else {
		return false;
	}	
}
bool ShiftQuoteApi::GetShiftContractNo(ShiftListInfo& ShiftContract)
{
	char ContractNo1[25] = { 0 };
	char ContractNo2[25] = { 0 };
	char sCondition[3] = { 0 };//第一腿，第二腿，两腿之间的条件；
	vector<string> vtContract1 = CConvert::SplitString(ShiftContract.Code1, ' ');
	vector<string> vtContract2 = CConvert::SplitString(ShiftContract.Code2, ' ');
	if (vtContract1.size() < 2 || vtContract2.size() < 2)
		return false;

	int iCondition[3] = { ShiftContract.LinkContract1, ShiftContract.LinkContract2, ShiftContract.LinkCondition };
	for (int i = 0; i < sizeof(iCondition) / sizeof(int); i++)
	{
		switch (iCondition[i])
		{
		case LcMulti:
			sCondition[i] = '*';
			break;
		case LcPlus:
			sCondition[i] = '+';
			break;
		case LcSub:
			sCondition[i] = '-';
			break;
		case LcDiv:
			sCondition[i] = '/';
			break;
		default:
			return false;
		}
	}
	GetSingleContractNo(vtContract1, sCondition[0], ShiftContract.OrderQty1, ContractNo1);
	GetSingleContractNo(vtContract2, sCondition[1], ShiftContract.OrderQty2, ContractNo2);
	sprintf_s(ShiftContract.ShiftContractNo, "%s %c %s", ContractNo1, sCondition[2], ContractNo2);
	return true;
}
void ShiftQuoteApi::GetSingleContractNo(vector<string> vecstr, char Condition, int Qty, char (&ContractNo)[25])
{
	char tempContractNo[25] = { 0 };
	if (vecstr.size() == 2)
	{
		sprintf_s(ContractNo, "(%s %c %d)", vecstr.at(1).c_str(), Condition, Qty);
		
	}
	else if (vecstr.size() == 3)
	{
		sprintf_s(ContractNo, "(%s%s %c %d)", vecstr.at(1).c_str(), vecstr.at(2).c_str(), Condition, Qty);
		
		/*int num = sizeof(ContractNo);
		int i = strlen(tempContractNo);
		strncpy_s(ContractNo, sizeof(ContractNo), tempContractNo, i);*/
	}
}