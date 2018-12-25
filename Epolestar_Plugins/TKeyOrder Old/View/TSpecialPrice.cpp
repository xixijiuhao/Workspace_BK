#include "BaseInclude.h"

#define Y_GAP   18

#define widgetLeft  5
#define widgetHeight 20
#define widgetTop    50
#define labWidth  100
#define editWidth  70

#define btnWidth  65


char szChsSp[][20] = { "指定价平仓", "改价", "取消", "确定", "手", "价格", "买", "卖", "，开仓", "，平仓", "提示","，" };
char szChtSp[][20] = { "指定價平倉", "改價", "取消", "確定", "手", "價格", "買", "賣", "，開倉", "，平倉", "提示","，" };
char szEnuSp[][30] = { "Specific Price Close", "Change Price", "Cancel", "OK", " lots ", "price", " Buy", " Sell", ", Open", ", Close", "Warning",", " };

enum{
	SpecificPrice_Index,
	ChangePrice_Index,
	Cancel_Index,
	OK_Index,
	Lots_Index,
	Price_Index,
	Buy_Index,
	Sell_Index,
	Open_Index,
	Close_Index,
	Waring_Index,
	Dot_Index,
};

char * _GetTextSp(int i)
{
	if (g_language == CHT)return szChtSp[i];
	else if (g_language == ENU)return szEnuSp[i];
	return szChsSp[i];
}


TCHAR szChsSpW[][10] = { L"没有持仓！", L"挂单已撤！", L"挂单已成交！", L"是否将余单发送？" };
TCHAR szChtSpW[][10] = { L"沒有持倉！", L"掛單已撤！", L"掛單已成交！", L"是否將余單發送？" };
TCHAR szEnuSpW[][25] = { L"No position!", L"Order already canceled!", L"Order matched!", L"Send rest order?" };

enum{
	NoPosition_Index,
	Canceled_Index,
	Matched_Index,
	SendRest_Index,
};
TCHAR * _GetTextSpW(int i)
{
	if (g_language == CHT)return szChtSpW[i];
	else if (g_language == ENU)return szEnuSpW[i];
	return szChsSpW[i];
}
TSpecialPrice::TSpecialPrice()
{
	m_pstPosition = nullptr;
}
TSpecialPrice::~TSpecialPrice()
{

}
void TSpecialPrice::Create(HWND hParent, uint type)
{
	if (type==frm_Postion)
		ShowFrm(hParent, L"SpecialPrice", _GetTextSp(SpecificPrice_Index));
	else
		ShowFrm(hParent, L"SpecialPrice", _GetTextSp(ChangePrice_Index));
	m_iFrmType = type;
	POINT pst;
	GetCursorPos(&pst);
	MoveWindow(pst.x-140, pst.y-60, 280, 120);

	memset(&m_stSendOrder, 0, sizeof(TSendOrder));
	m_pstPosition = nullptr;
	m_pstOrder = nullptr;
	memset(&m_stCommodityInfo, 0, sizeof(SOFCommodityInf));
}

void TSpecialPrice::SetData(const void* pData)
{
	if (m_iFrmType == frm_Postion)
		SetPositionInfo((const TPosition*)pData);
	else
		SetParOrderInfo((const TOrder*)pData);
}
void TSpecialPrice::OnCreate()
{
	m_labContractInfo.Create(m_Hwnd, L"", SS_RIGHT | SS_CENTERIMAGE);
	m_labContractInfo.MoveWindow(widgetLeft, widgetTop, labWidth+15, widgetHeight);

	int left = widgetLeft + labWidth + 5;
	m_spinPrice.Create(m_Hwnd, 0, 0, ID_SPINPRICE, CxEdit::floatStyle, false);
	m_spinPrice.SetFont(g_FontData.g_FontNum15);
	m_spinPrice.MoveWindow(left, widgetTop-1, editWidth, widgetHeight+2);

	left += editWidth + 5;
	m_labDirectInfo.Create(m_Hwnd, L"");
	m_labDirectInfo.MoveWindow(left, widgetTop, 60, widgetHeight);


	int top = widgetTop + widgetHeight + Y_GAP;
	m_btnCancle.Create(m_Hwnd, _GetTextSp(Cancel_Index), ID_BTNCANCLE);
	m_btnCancle.SetFont(g_FontData.g_FontWord12);
	
	m_btnCancle.MoveWindow(left, top, btnWidth, widgetHeight);

	left = left - btnWidth - 15;
	m_btnOK.Create(m_Hwnd, _GetTextSp(OK_Index), ID_BTNOK);
	m_btnOK.SetFont(g_FontData.g_FontWord12);
	m_btnOK.MoveWindow(left, top, btnWidth, widgetHeight);

	EnableWindow(::GetParent(m_Hwnd), FALSE);
}
void TSpecialPrice::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BTNOK:
		OnBtnOk();
		break;
	case ID_BTNCANCLE:
		OnBtnCancle();
		break;
	default:
		break;
	}
}
void TSpecialPrice::SetInfo()
{
	char ContractInfo[150] = { 0 };

	char contractNo[50] = { 0 };
	if (m_stSendOrder.CommodityType != ctOption)
		sprintf_s(contractNo, "%s%s", m_stSendOrder.CommodityNo, m_stSendOrder.ContractNo);
	else
		sprintf_s(contractNo, "%s%s%c%s", m_stSendOrder.CommodityNo, m_stSendOrder.ContractNo, m_stSendOrder.OptionType, m_stSendOrder.StrikePrice);
	sprintf_s(ContractInfo, "%s%s%d%s%s", contractNo,_GetTextSp(Dot_Index), m_stSendOrder.OrderQty, 
		_GetTextSp(Lots_Index), _GetTextSp(Price_Index));
	m_labContractInfo.SetText(ContractInfo);

	char szDirect[20] = { 0 };
	if (m_stSendOrder.Direct == dBuy)
		strcat_s(szDirect, _GetTextSp(Buy_Index));
	else 
		strcat_s(szDirect, _GetTextSp(Sell_Index));
	if (m_stSendOrder.Offset == oOpen)
		strcat_s(szDirect, _GetTextSp(Open_Index));
	else if (m_stSendOrder.Offset==oCoverT || m_stSendOrder.Offset==oCover)
		strcat_s(szDirect, _GetTextSp(Close_Index));
	m_labDirectInfo.SetText(szDirect);

	memset(&m_stCommodityInfo, 0, sizeof(SOFCommodityInf));
	CConvert::GetCommodityInf(m_stSendOrder, m_stCommodityInfo);

	if (m_stCommodityInfo.bIfFractionPrice)
	{
		m_stCommodityInfo.dMiniChangePrice = m_stCommodityInfo.dMiniChangePrice / m_stCommodityInfo.iPrecision;
		m_stCommodityInfo.iPrecision = 5;
	}

	SQuoteSnapShot stQuote;
	memset(&stQuote, 0, sizeof(SQuoteSnapShot));
	SContractNoType qNo;
	CConvert::TradeContractToQContractNoType(m_stSendOrder, qNo);
	if (m_iFrmType==frm_Postion&& g_pStarApi->GetSnapshot(qNo, &stQuote))
	{
		double dSellPrice = stQuote.Data[S_FID_BESTASKPRICE].Price;
		double dBuyPrice = stQuote.Data[S_FID_BESTBIDPRICE].Price;

		char szPrice[50] = { 0 };
		if (m_stSendOrder.Direct == dSell)
			DoubleToChar(m_stCommodityInfo.iPrecision, dBuyPrice, szPrice);
		else
			DoubleToChar(m_stCommodityInfo.iPrecision, dSellPrice, szPrice);
		m_spinPrice.SetText(szPrice);
	}
	else if (m_iFrmType == frm_ParOrder)
	{
		char szPrice[50] = { 0 };
		DoubleToChar(m_stCommodityInfo.iPrecision, m_stSendOrder.OrderPrice, szPrice);
		m_spinPrice.SetText(szPrice);
	}
}
void TSpecialPrice::SetPositionInfo(const TPosition* pPosition)
{
	//设置合约信息
	if (pPosition == NULL)
		return;
	EnableWindow(::GetParent(m_Hwnd), FALSE);
	
	m_pstPosition = pPosition;
	CConvert::DefaultDealClose(m_pstPosition, m_stSendOrder);
	SetInfo();
}
void TSpecialPrice::SetParOrderInfo(const TOrder* pOrder)
{
	if (pOrder == NULL)
		return;
	EnableWindow(::GetParent(m_Hwnd), FALSE);

	m_pstOrder = pOrder;

	memcpy(&m_stSendOrder, pOrder, sizeof(TSendOrder));

	SetInfo();
}

void TSpecialPrice::OnBtnOk()
{
	if (m_iFrmType == frm_Postion)
	{
		if (!m_pstPosition) return;
		if (m_pstPosition->PositionQty == 0)
		{
			_TMessageBox_Domodal(m_Hwnd, _GetTextSp(Waring_Index), _GetTextSpW(NoPosition_Index), TMB_OK);
			return;
		}
		m_stSendOrder.OrderPrice = m_spinPrice.GetDoubleData();
		g_pTradeAction->InsertCloseOrder(m_stSendOrder);
	}
	else if (m_iFrmType == frm_ParOrder)
	{
		if (!m_pstOrder) return;
		switch (m_pstOrder->OrderState)
		{
		case osCanceled:
		case osPartCanceled:
			_TMessageBox_Domodal(m_Hwnd, _GetTextSp(Waring_Index), _GetTextSpW(Canceled_Index), TMB_OK);
			return;
		case osFilled:
			_TMessageBox_Domodal(m_Hwnd, _GetTextSp(Waring_Index), _GetTextSpW(Matched_Index), TMB_OK);
			return;
		case osPartFilled:
			if (_TMessageBox_Domodal(m_Hwnd, _GetTextSp(Waring_Index), _GetTextSpW(SendRest_Index), TMB_OKCANCEL))
			{
				m_stSendOrder.OrderQty = m_stSendOrder.OrderQty - m_pstOrder->MatchQty;
			}
			else
			{
				return;
			}
			break;
		}
		m_stSendOrder.OrderPrice = m_spinPrice.GetDoubleData();
		TModifyOrder stModify;
		memcpy(&stModify, &m_stSendOrder, sizeof(TSendOrder));
		stModify.OrderID = m_pstOrder->OrderID;
		stModify.ServerFlag = m_pstOrder->ServerFlag;
		if (g_pTradeApi)
		{
			int iOrderId = g_pTradeApi->ModifyOrder(stModify);
			iOrderId = 0;
		}
		
	}
	OnBtnCancle();
}
void TSpecialPrice::OnBtnCancle()
{
	m_pstPosition = nullptr;
	CloseWnd();
}

LRESULT TSpecialPrice::OnNotify(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == ID_SPINPRICE)
	{
		LPNMUPDOWN pPriceUpDwon = (LPNMUPDOWN)lParam;
		double dNow = m_spinPrice.GetDoubleData();
		if (pPriceUpDwon->iDelta > 0)
			dNow += m_stCommodityInfo.dMiniChangePrice;
		else
			dNow -= m_stCommodityInfo.dMiniChangePrice;
		char Price[20] = { 0 };
		DoubleToChar(m_stCommodityInfo.iPrecision, dNow, Price);
		m_spinPrice.SetText(Price);
		SetFocus(m_spinPrice.GetEditHwnd());
		PostMessage(m_spinPrice.GetEditHwnd(), EM_SETSEL, 0, -1);
	}
	return PROCESSED;
}