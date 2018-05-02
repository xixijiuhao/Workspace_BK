#include "PreInclude.h"
//extern IQuoteApi* g_pQuoteApi;
//extern TGlobalModel* g_pGlobalModel;
//extern ITradeData* g_pTradeData;
TAddModStopLPData::TAddModStopLPData()
{
	g_pStopLP->Attach(this);
}

TAddModStopLPData::~TAddModStopLPData()
{
	UnSubQuote();
	g_pStopLP->Detach(this);
}


void TAddModStopLPData::RefreshContract(const TPosition* key)
{
	UnSubQuote();
	//选中当前用户
	m_Data.sign = g_pGlobalModel->m_strSign;
	m_Data.UserNo = g_pGlobalModel->m_strUserNo;
	SContractNoType  sQuote = {};
	CConvert::SingleTradeContractToQContractNoType(*key, sQuote);
	g_pStarApi->SubQuote(&sQuote, 1, this);//订阅当前合约行情
	m_Data.RawContractID = sQuote;

	const SCommodity* pCom = g_pTradeData->GetCommodity(key);
	m_Data.ProductDot = pCom ? pCom->TradeDot : 0;//每手乘数
	m_Data.minChangePrice = pCom ? pCom->PriceTick: 0;
	m_Data.iPrecision = pCom ? pCom->PricePrec: 0;

	m_Data.Qty = key->PositionQty;
	m_Data.direct = key->Direct;
	m_Data.InitDeposit = key->Deposit;
	RefreshComBoxContainer();
	OnQuote(nullptr, nullptr);
}

void TAddModStopLPData::UnSubQuote()
{
	g_pStarApi->UnsubAllQuote(this);
}

void TAddModStopLPData::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
	//更新最新价
	TContractKey TKey;
	CConvert::SContractNoTypeToTradeContract(m_Data.sign.c_str(), m_Data.RawContractID.c_str(), TKey);
	CConvert::GetPrice(&TKey, dBuy, ptLastPrice, m_Data.LastPrice);
}

void __cdecl TAddModStopLPData::Update(DataType type, const TStopLPitem* pdata)
{
	string strCode;
	CConvert::QContractNoTypeToStr(m_Data.RawContractID, strCode);
	if (strcmp(strCode.c_str(), pdata->code)||strcmp(m_Data.UserNo.c_str(),pdata->userNo))//合约以及用户判断，只对应变动当前用户的当前合约的
	{
		return;
	}
	//需要刷新Combox数据的情况
	//1.有增加新的单子
	//2.有删除或触发了的单子
	//3.启动（修改）或暂停，当前单子需要改变按钮和控件状态（如何区分？？）
	else
	{
		if (Add_Type == type)
		{
			m_ComBoxData.push_back(pdata);//Add的pdata是新添的止损单GetOrder得到的
			//通知界面刷新Combox
			g_Presenter->Send(TPresenter::CMD_Presenter_ComboxUpdate, pdata);
		}
		else if (Chg_Type == type)
		{
			int i;
			m_ComBoxData.FindItem(pdata, i);
			if (i == m_ComBoxData.size())//没找到
			{
				return;
			}
			if (osDeleted == pdata->state || osTrig == pdata->state)//已删除
			{

				g_Presenter->Send(TPresenter::CMD_Presenter_ComboxUpdate, pdata);
			}
			//else if (osTrig == pdata->state)
			//{
			//	g_Presenter->Send(TPresenter::CMD_Presenter_ComboxUpdate, pdata);
			//}
			//其他情况，如果是挂起或重新启动了，也应该通知界面。
			else
			{
				g_Presenter->Send(TPresenter::CMD_Presenter_ComboxUpdate, pdata);
			}
		}
		else if (Trg_Type == type)//触发次数变化，不刷新界面
		{

		}
		else if (ChO_Type == type)
		{
			RefreshComBoxContainer();
			g_Presenter->Send(TPresenter::CMD_Presenter_ComboxUpdate, nullptr);
		}

	}

}

void TAddModStopLPData::RefreshComBoxContainer()
{
	m_ComBoxData.clear();
	string strCode;
	TDirect direct = dBoth;
	if (m_Data.direct == dBuy)
	{
		direct = dSell;
	}
	else if (m_Data.direct == dSell)
	{
		direct = dBuy;
	}
	CConvert::QContractNoTypeToStr(m_Data.RawContractID, strCode);
	g_pStopLP->GetALLStopLPOrder(m_ComBoxData, m_Data.UserNo.c_str(), strCode.c_str(), direct, 1);
}

void TAddModStopLPData::UpdatePositionPrice()
{
	TContractKey key;
	CConvert::SContractNoTypeToTradeContract(m_Data.sign.c_str(), m_Data.RawContractID, key);
	const TPosition* pos  = g_pTradeData->GetPositionTotal(m_Data.UserNo.c_str(), &key, m_Data.direct);
	if (pos == nullptr)return;
	m_Data.OpenPrice = pos->PositionPrice;
	m_Data.InitDeposit = pos->Deposit;

}


void TAddModStopLPData::DealModifyOrder(const TStopLPitem* TStop)
{
	if (TStop->state == osSuspend)//如果是暂停(挂起)，就不用检验单子了
	{
		g_pStopLP->ModifyLPOrder(TStop);
		return;
	}
	if (CheckOrder(TStop))
	{
		g_pStopLP->ModifyLPOrder(TStop);
	}
}
void TAddModStopLPData::DealInsertOrder(const TStopLPInsertItem* TStop)
{
	if (CheckOrder(TStop))
	{
		g_pStopLP->InsertLPOrder(TStop);
	}
}


bool TAddModStopLPData::CheckOrder(const TStopLPInsertItem* TStop)
{
	//检验单子
	double sign = (m_Data.direct == dBuy) ? 1 : -1;
	if (0 >= TStop->Qty)
	{
		m_pPresenter->ShowMsgBox(EN_OrderQty_0);
		return false;
	}
	//**有无可用持仓的判断
	TContractKey ckey;
	CConvert::SContractNoTypeToTradeContract(m_Data.sign.c_str(), m_Data.RawContractID, ckey);
	const TPosition* pos = g_pTradeData->GetPositionTotal(m_Data.UserNo.c_str(), &ckey, m_Data.direct);
	if (pos == nullptr)
	{
		m_pPresenter->ShowMsgBox(EN_NoAva_Position);
		return false;
	}
	//**
	switch (TStop->orderType)
	{
	case otLoss:
		if (sign * (m_Data.LastPrice - TStop->triggerPrice) <= 0)
		{
			if (g_language == ENU)
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "StopLoss price is not appropriate!");
			}
			else if (g_language == CHT)
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "止損價格不合時宜");
			}
			else
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "止损价格不合时宜");
			}
			return false;
		}
		break;
	case otFloat:
		if (TStop->floatPoint <= 0)
		{
			if (g_language == ENU)
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "Float spread shouldn't be zero.");
			}
			else if (g_language == CHT)
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "浮動價差不能為零");
			}
			else
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "浮动价差不能为零");
			}
			return false; 
		}
		break;
	case otProfit:
		if (sign * (m_Data.LastPrice - TStop->triggerPrice) >= 0)
		{
			if (g_language == ENU)
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "StopProfit price is not appropriate!");
			}
			else if (g_language == CHT)
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "止盈價格不合時宜");
			}
			else
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "止盈价格不合时宜");
			}
			return false;
		}
		break;
	case otBreakEven:
		if (TStop->breakEvenPoint <= 0)
		{
			if (g_language == ENU)
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "BreakEven spread shouldn't be zero.");
			}
			else if (g_language == CHT)
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "保本價差不能為零");
			}
			else
			{
				m_pPresenter->ShowMsgBox(Msg_OK, "保本价差不能为零");
			}
			return false;
		}
		break;
	}

	return true;
}