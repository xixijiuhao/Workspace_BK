#include "PreInclude.h"
extern PointOrderConfig	g_stOrderConfig;
int InsertOrderAssitant::InsertOrder(TSendOrder & Data)
{
    if (Data.StrategyType != stNone)
    {
        g_pTradeApi->InsertOrder(Data);
        return 0;
    }
    int iQty = 0;
    if (Data.Offset == oOpen&&Data.StrategyType == stNone)
    {
        if (!g_pTradeData->GetTradableQty(Data, iQty) && iQty > 0 && iQty < Data.OrderQty)
            Data.OrderQty = iQty;
    }
    int iSplitQty = Data.OrderQty;
    if (iSplitQty >= 1000)
    {
        for (int i = 0; i < (iSplitQty / 1000); i++)
        {
            Data.OrderQty = 1000;
            g_pTradeApi->InsertOrder(Data);
        }
    }
    if (iSplitQty % 1000)
    {
        Data.OrderQty = iSplitQty % 1000;
        g_pTradeApi->InsertOrder(Data);
    }
    return 0;
}
bool InsertOrderAssitant::DealMesasagebox(TSendOrder & Data , bool isOrderConfirm)
{
	if (g_stOrderConfig.iMessageBox == 1)
	{
		const SCommodity* pstCommodity = g_pTradeData->GetCommodity(&Data);

		if (!pstCommodity)
		{
			return false;
		}

		char szText[50]{};
		char szPrice[50]{};
		DoubleToChar(pstCommodity->PricePrec, Data.OrderPrice, szPrice);

		char (*format)[50] = NULL;
		int i = 6;

		switch (g_LanguageID)
		{
		case NONE:
			break;
		case CHT:
			format = &CHTTips[i];
			break;
		case CHS:
			format = &CHSTips[i];
			break;			
		case ENU:
			format = &ENUTips[i];
			break;
		default:
			break;
		}
		if (format == NULL)
			return false;
		sprintf_s(szText, *format, Data.OrderQty, szPrice);

		i = 7;

		switch (g_LanguageID)
		{
		case NONE:
			break;
		case CHT:
			format = &CHTTips[i];
			break;
		case CHS:
			format = &CHSTips[i];
			break;
		case ENU:
			format = &ENUTips[i];
			break;
		default:
			break;
		}
		if (format == NULL)
			return false;
		if (!isOrderConfirm)
		{
			if (!_TMessageBox_Domodal(GetActiveWindow(), *format, (wchar_t*)ANSIToUnicode(szText).c_str(), TMB_OKCANCEL))
				return false;
			else
				return true;
		}
		else
		{
			char ContractInfo[250] = { 0 };
			sprintf_s(ContractInfo, "%s %s %s", Data.ExchangeNo, Data.CommodityNo, Data.ContractNo);
			char OrderPrice[250] = { 0 };
			sprintf_s(OrderPrice, "%0.2f", Data.OrderPrice);
			if (OrderConfirm_Domodal(GetActiveWindow(), ContractInfo, &(Data.OrderQty), OrderPrice, TMB_OKCANCEL))
				return true;
			else
				return false;
		}
	}
	else {
		return true;
	}
}
