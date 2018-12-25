#include "BaseInclude.h"


void TModifyConView::RegisterConOrder(TConditionOrder* pOrder,HWND hParent, IMediator *pMediator)
{
	pOrder->RunState = rsStop;
	CConvert::SContractNoTypeToTradeContract("", pOrder->pContract->ContractNo, m_stOrderViewData.ContractKey);
	//m_stOrderViewData.pContractKey = pOrder->pContract;

	CConvert::GetCommodityInf(m_stOrderViewData.ContractKey, m_stCommodityInfo);

	CConvert::GetPrice(&m_stOrderViewData.ContractKey, dBuy, ptQueuePrice, m_stQuoteData.BuyPrice);
	CConvert::GetPrice(&m_stOrderViewData.ContractKey, dSell, ptQueuePrice, m_stQuoteData.SellPrice);
	CConvert::GetPrice(&m_stOrderViewData.ContractKey, dBuy, ptLastPrice, m_stQuoteData.LastPrice);
	CConvert::GetPrice(&m_stOrderViewData.ContractKey, dBuy, ptMarketPrice, m_stQuoteData.UpPrice);
	CConvert::GetPrice(&m_stOrderViewData.ContractKey, dSell, ptMarketPrice, m_stQuoteData.DownPrice);

	if (!m_pFrm)
	{
		m_pFrm = new TConditionOrderFrm(pMediator);
		m_pFrm->SetMode(true);
		m_pFrm->SetOrderData(&m_stOrderViewData);
		m_pFrm->SetQuoteData(&m_stQuoteData);
		m_pFrm->SetCommInfoData(&m_stCommodityInfo);

		char szText[50] = { 0 };
		if (g_language == CHT)strcpy_s(szText, "修改條件單");
		else if (g_language == ENU)strcpy_s(szText, "Modify Condition Order");
		else strcpy_s(szText, "修改条件单");
		//m_pFrm->ShowFrm(hParent, L"ModifyView", szText);
		m_pFrm->ShowFrm(hParent, L"ModifyView", szText);
		m_pFrm->MoveWindow();
	}
	
	m_pFrm->Update();
	m_pFrm->SetMode(true);
	m_pFrm->SetWidgetContent(pOrder, hParent);
	ShowWindow(m_pFrm->GetHwnd(), SW_SHOW);
	
//	EnableWindow(hParent, false);
}