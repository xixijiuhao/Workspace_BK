#include "PreInclude.h"

#define   textLeft    13
#define   text1Top    38
#define   text2Top    65
#define   textHeight  20


TOpenStopFrm::TOpenStopFrm()
{
	m_rcBuy = { 13, 130, 61, 153 };
	m_rcSell = { 61, 130, 109, 153 };
	m_rcBtnOrder = { 443, 130, 545, 153 };
	m_bIsBuy = true;
	m_bIsCilckOrderBtn = false;

	m_bIsPriceLink = true;  //要保存状态
	m_bIsDynamic = false;

}


char szChsOs[][15] = { "回撤: ", "价差: ", "提示",  "手", "持仓", "下单 (买 开仓)", "下单 (卖 开仓)",
"合约: ","点数: " };
char szChtOs[][15] = { "回撤: ", "價差: ", "提示", "手", "持倉", "下單 (買 開倉)", "下單 (賣 開倉)",
"合約: ","點數: " };
char szEnuOs[][20] = {"Retracement: ","Spread: ","Warning"," lots ","position","Buy Open","Sell Open",
"Contract: ", "Tick: " };
enum{
	Retracement_Index,
	Spread_Index,
	Warning_Index,
	Lots_Index,
	Position_Index,
	BuyOpen_Index,
	SellOpen_Index,
	Contract_Index,
	Tick_Index,
};
char * _GetTextOs(int i)
{
	if (g_language == CHT)return szChtOs[i];
	else if (g_language == ENU)return szEnuOs[i];
	return szChsOs[i];
}
TCHAR szChsOsW[][10] = { L"手数", L"价格", L"止损价", L"止盈价", L"跟踪回撤", L"数量不能为0 ！", L"价格不合适！", L"止损价过高", L"止盈价过低",
L"止盈价过高", L"止损价过低", L"涨板", L"跌板", L"买", L"卖", L"多头： ", L"空头： ",L"使用点数" };
TCHAR szChtOsW[][10] = { L"手數", L"價格", L"止損價", L"止盈價", L"跟蹤回撤", L"數量不能為0 ！", L"價格不合適！", L"止損價過高", L"止盈價過低",
L"止盈價過高", L"止損價過低", L"漲板", L"跌板", L"買", L"賣", L"多頭： ", L"空頭： ",L"使用點數"};
TCHAR szEnuOsW[][35] = { L"lots", L"price", L"stop loss price", L"stop profit price", L"Floating", L"Lost can not be 0 !", L"Price is inappropriate!",
L"Stop loss price is too high!", L"Stop profit price is too low!", L"Stop profit price is too high!", L"Stop loss price is too low!" 
, L"LimitUp", L"LimitDown", L"Buy", L"Sell", L"Long: ", L"Short: ",L"By Tick" };
enum{
	LotsW_Index,
	PriceW_Index,
	StopLossPrice_Index,
	StopProfitPrice_Index,
	Floating_Index,
	LostIs0_Index,
	PriceError_Index,
	StopLossHigh_Index,
	StopProfitLow_Index,
	StopProfitHigh_Index,
	StopLossLow_Index,
	LimitUp_Index,
	LimitDown_Index,
	Buy_Index,
	Sell_Index,
	Long_Index,
	Short_Index,
	PricePoint_Index,
};
TCHAR * _GetTextOsW(int i)
{
	if (g_language == CHT)return szChtOsW[i];
	else if (g_language == ENU)return szEnuOsW[i];
	return szChsOsW[i];
}
TOpenStopFrm::~TOpenStopFrm()
{

}
void TOpenStopFrm::OnCreate()
{
	m_bIsBuy = true;
	int top = 110;
	int height = 20;
	int width = 50;
	//TCHAR labelName[][10] = { L"手数", L"价格", L"止损价", L"止盈价" };
	for (int i = 0; i < 4; i++)
	{
		m_labels[i].Create(m_Hwnd, _GetTextOsW(LotsW_Index+i), SS_LEFT | SS_CENTERIMAGE, i + id_shand);
	}

	m_labels[0].MoveWindow(114, top, width, height);
	m_labels[1].MoveWindow(184, top, width, height);
	if (g_language == ENU)
	{
		width = 80;
	}
	m_labels[2].MoveWindow(266, top, width, height);
	m_labels[3].MoveWindow(356, top, width, height);

	top = top + height;
	height = 23;
	RECT rect = { 112, top, 112+63, top+height };
	m_pEdits[0].Create(m_Hwnd, CxEdit::intStyle, 0, 0, ES_RIGHT | ES_AUTOHSCROLL);
	m_pEdits[0].MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_pEdits[0].SetRange(1, 100000);
	rect.left = 182;
	rect.right = rect.left + 78;
	m_pEdits[1].Create(m_Hwnd, ES_RIGHT | ES_AUTOHSCROLL, 0, id_spinPrice, CxEdit::floatStyle, false);
	m_pEdits[1].MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_lcPriceChained.Create(m_Hwnd, IDI_LINK, IDI_DISLINK);//联动标识
	m_lcPriceChained.MoveWindow(234, rect.top - 17, 25, 17);
	m_lcPriceChained.SetValue(true);
	m_bIsPriceLink = true;

	rect.left = 264; rect.right = rect.left + 82;
	m_pEdits[2].Create(m_Hwnd, ES_RIGHT | ES_AUTOHSCROLL, 0, id_spinLoss, CxEdit::floatStyle, false);
	m_pEdits[2].MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	rect.left = 354; rect.right = rect.left + 84;
	m_pEdits[3].Create(m_Hwnd, ES_RIGHT | ES_AUTOHSCROLL, 0, id_spinProfit, CxEdit::floatStyle, false);
	m_pEdits[3].MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	for (int i = 0; i < 6; i++)
	{
		m_labelInfo[i].Create(m_Hwnd, L"", SS_LEFT | SS_CENTERIMAGE, id_scanopen + i);
	}
	top = top + height + 6;
	height = 20;
	width = 50;
	m_labelInfo[0].MoveWindow(125, top, width, height); //最大可开仓数
	width = 88;
	m_labelInfo[1].MoveWindow(175, top, width, height); //价差
	width = 80;
	m_labelInfo[2].MoveWindow(264, top, width, height);
	m_labelInfo[3].MoveWindow(354, top, width, height);
	top = 179;
	width = 48;
	if (g_language==ENU)
		width = 40;
	m_labelInfo[4].MoveWindow(120, top, width, height);
	m_labelInfo[5].MoveWindow(218, top, width, height);

	width = 80;
	int left = 266;
	m_ckDynamic.Create(m_Hwnd, id_ckDynamic, _GetTextOsW(Floating_Index));
	m_ckDynamic.MoveWindow(266, top, width, height);
	m_ckDynamic.SetBkColor(g_ColorRefData.g_ColorBackground);

	left += width + 10;
	m_ckPricePoint.Create(m_Hwnd, id_ckPricePoint, _GetTextOsW(PricePoint_Index));
	m_ckPricePoint.MoveWindow(left, top, width, height);
	m_ckPricePoint.SetBkColor(g_ColorRefData.g_ColorBackground);

	Init();
}
void TOpenStopFrm::Update()
{
	if (!IsWindowVisible(m_Hwnd))
		return;
	if (m_bIsPriceLink)
	{
		SetPrice();
	}
	if (!m_bHasSetUpDownPrice)
	{
		SetUpDownPrice();
	}
}
void TOpenStopFrm::UpdateCode()
{
	if (!IsWindowVisible(m_Hwnd))
		return;
	m_pEdits[2].SetText("0");
	m_pEdits[3].SetText("0");
	m_bIsPriceLink = true;
	m_lcPriceChained.SetValue(true);
	m_bHasSetUpDownPrice = false;

	OnClickCheckBox(id_ckPricePoint, 0);

	InvalidateRect(m_Hwnd, 0, false);
}
void TOpenStopFrm::Init()
{
	m_pEdits[0].SetText("1");
	m_pEdits[1].SetText("0");
	m_pEdits[2].SetText("0");
	m_pEdits[3].SetText("0");
	SetPrice();
	SetUpDownPrice();
}
void TOpenStopFrm::SetUpDownPrice()
{
	char szText[100] = { 0 };
	DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->UpPrice, szText);
	m_labelInfo[id_spriceup - id_scanopen].SetText(szText);
	DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->DownPrice, szText);
	m_labelInfo[id_spricedown - id_scanopen].SetText(szText);
	m_bHasSetUpDownPrice = true;
}
LRESULT TOpenStopFrm::OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case SSWM_CLICK_CHECKBOX:
		OnClickCheckBox(wParam,lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)g_ColorRefData.g_brush_bk;
	default:
		return NOT_PROCESSED;
	}
}

void TOpenStopFrm::OnClickCheckBox(WPARAM wParam, LPARAM lParam)
{
	if (wParam == id_ckDynamic)
	{
		m_bIsDynamic = m_ckDynamic.GetCheck();
	}
	else if(wParam ==id_ckPricePoint)
	{
		if (m_ckPricePoint.GetCheck())
		{
			m_pEdits[2].SetText("1");
			m_pEdits[3].SetText("1");
		}
		else
		{
			double dPrice = m_pEdits[1].GetDoubleData();
			char szPrice1[100] = { 0 };
			char szPrice2[100] = { 0 };
			DoubleToChar(m_pCommodityInfo->iPrecision, dPrice - m_pCommodityInfo->dMiniChangePrice, szPrice1);
		
			DoubleToChar(m_pCommodityInfo->iPrecision, dPrice + m_pCommodityInfo->dMiniChangePrice, szPrice2);

			if (m_bIsBuy)
			{
				m_pEdits[2].SetText(szPrice1);
				m_pEdits[3].SetText(szPrice2);
			}
			else
			{
				m_pEdits[2].SetText(szPrice2);
				m_pEdits[3].SetText(szPrice1);
			}
		}
	}
	SetDiffPrice(id_spinLoss);
}


LRESULT TOpenStopFrm::OnNotify(WPARAM wParam, LPARAM lParam)
{
	uint id = LOWORD(wParam);
	if (id == id_spinPrice || id == id_spinLoss || id == id_spinProfit)
	{
		LPNMUPDOWN p = (LPNMUPDOWN)lParam;
		int index = id - id_spinLots;
		if (m_ckPricePoint.GetCheck() && id != id_spinPrice) //使用了点数但是不是止损止盈
		{
			int data = m_pEdits[index].GetWindowNumber();
			if (p->iDelta > 0)
			{
				data++;
			}
			else
			{
				data--;
				if (data < 0)
					data = 0;
			}
			m_pEdits[index].SetWindowNumber(data);
		}
		else
		{
			char Price[20] = { 0 };
			double dNow = m_pEdits[index].GetDoubleData();
			if (p->iDelta > 0)
				dNow += m_pCommodityInfo->dMiniChangePrice;
			else
				dNow -= m_pCommodityInfo->dMiniChangePrice;
			DoubleToChar(m_pCommodityInfo->iPrecision, dNow, Price);
			m_pEdits[index].SetText(Price);
		}

		if (index == 1) //价格
		{
			SetOrderPrice();
			m_bIsPriceLink = false;
			m_lcPriceChained.SetValue(false);
		}
		else
		{
			SetDiffPrice(id);
		}
	}
	return PROCESSED;
}

LRESULT TOpenStopFrm::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == EN_CHANGE)
	{
		for (int i = 1; i < 4; i++)
		{
			if ((HWND)lParam == m_pEdits[i].GetEditHwnd())
			{
				if (i == 1)  //价格
					SetOrderPrice();
				else
					SetDiffPrice(i + id_spinLots);
				return PROCESSED;
			}
		}
	}
	else if (HIWORD(wParam) == EN_SETFOCUS && (HWND)lParam == m_pEdits[1].GetEditHwnd())
	{
		m_bIsPriceLink = false;
		m_lcPriceChained.SetValue(false);
		PostMessage(m_pEdits[1].GetEditHwnd(), EM_SETSEL, 0, -1);
	}
	else if ((HWND)lParam == m_lcPriceChained.GetHwnd() && HIWORD(wParam) == BN_CLICKED)
	{		
		m_bIsPriceLink = m_lcPriceChained.GetValue();
		if (m_bIsPriceLink == true)
		{
			SetFocus(m_Hwnd);
		}
		else
		{
			SetFocus(m_pEdits[1].GetEditHwnd());
		}
	}
	return NOT_PROCESSED;
}

//主要用来设置止损价、止盈价下面的价差
void TOpenStopFrm::SetOrderPrice()
{
	double dPrice = m_pEdits[1].GetDoubleData();
	double dOrderPrice = m_pQuoteData->BuyPrice;      // 对手价
	if (m_bIsBuy)
		dOrderPrice = m_pQuoteData->SellPrice;

	SetLabelText(dPrice - dOrderPrice, id_spricegap); //设置价格价差

	//设置止损止盈的价差，如果止损止盈价是0，则不用设置
	//如果计算出来的价差是0，也不用设置
	SetDiffPrice(id_spinLoss);
	SetDiffPrice(id_spinProfit);
}

void TOpenStopFrm::SetDiffPrice(UINT edit_id)
{
	if (!m_ckPricePoint.GetCheck()) //使用价格
	{
		double dPrice = m_pEdits[1].GetDoubleData();

		CxSpinEdit *pSpin = m_pEdits + edit_id - id_spinLots;
		double dData = pSpin->GetDoubleData();

		if (dData == 0)
			SetLabelText(0, edit_id - id_spinLots + id_scanopen); //找出要修改的label id
		else
			SetLabelText(dData - dPrice, edit_id - id_spinLots + id_scanopen);
	}
	else //使用点数
	{
		SetTickText(edit_id - id_spinLots + id_scanopen);
	}
}


void TOpenStopFrm::SetTickText(UINT label_id)
{
	int data = m_pEdits[label_id - id_scanopen].GetWindowNumber();
	
	char szText[50] = { 0 };
	if (data != 0)
	{
		if (label_id == id_sloss &&m_bIsDynamic)
		{
			sprintf_s(szText, "%s%d", _GetTextOs(Retracement_Index), data);
		}
		else
		{
			sprintf_s(szText, "%s%d", _GetTextOs(Tick_Index), data);
		}
		m_labelInfo[label_id - id_scanopen].SetText(szText);
	}
	else
	{
		m_labelInfo[label_id - id_scanopen].SetText("");
	}
}

void TOpenStopFrm::SetLabelText(double Price, UINT label_id)
{
	if (Price == 0)
	{
		m_labelInfo[label_id - id_scanopen].SetText("");
		return;
	}

	char szText[50] = { 0 };
	char str[100];

	if (id_sloss == label_id && m_bIsDynamic)
	{
		DoubleToChar(m_pCommodityInfo->iPrecision, -Price, szText);
		sprintf_s(str, "%s%s",_GetTextOs(Retracement_Index), szText);
	}
	else
	{
		DoubleToChar(m_pCommodityInfo->iPrecision, Price, szText);
		sprintf_s(str, "%s%s", _GetTextOs(Spread_Index), szText);
	}

	m_labelInfo[label_id - id_scanopen].SetText(str);
}
void TOpenStopFrm::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if (m_bIsCilckOrderBtn)
	{
		m_bIsCilckOrderBtn = false;
		InvalidateRect(m_Hwnd, &m_rcBtnOrder, true);
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (PtInRect(&m_rcBtnOrder, pt))//发单子出去
		{
			SendOrder();

		//CloseWnd();
		}
	}
}
void TOpenStopFrm::SendOrder()
{
	TSendOrder stOrder;
	CConvert::DefaultSendOrder(stOrder);
	int orderQty = m_pEdits[0].GetWindowNumber();

	if (orderQty <= 0)
	{
		_TMessageBox_Domodal(m_Hwnd, _GetTextOs(Warning_Index), _GetTextOsW(LostIs0_Index),TMB_OKCANCEL);
		return;
	}
	stOrder.OrderQty = orderQty;
	double price = m_pEdits[1].GetDoubleData();
	if (m_pQuoteData->DownPrice != 0 && price <= m_pQuoteData->DownPrice
		|| m_pQuoteData->UpPrice != 0 && price >= m_pQuoteData->UpPrice)
	{
		_TMessageBox_Domodal(m_Hwnd, _GetTextOs(Warning_Index), _GetTextOsW(PriceError_Index), TMB_OKCANCEL);
		return;
	}
	stOrder.OrderPrice = price;
	stOrder.Offset = oOpen;
	stOrder.Direct = dSell;
	if (m_bIsBuy)
		stOrder.Direct = dBuy;
	strcpy_s(stOrder.UserNo, g_pGlobalModel->m_strUserNo.c_str());
	strcpy_s(stOrder.Sign, g_pGlobalModel->m_strSign.c_str());
	SetStopLPOrder(stOrder);
}
void TOpenStopFrm::SetStopLPOrder(TSendOrder &stOrder)
{
	//填充 StopInfo结构体;
	AutoStopInfo stStopInfo;
	//止损开仓
	stStopInfo.iOrderType = StopOpenType;


	//获取止损价 止盈价
	double dstopLoss = 0.0;
	double dstopProfit = 0.0;
	GetLossAndProfitPrice(dstopLoss, dstopProfit);
	if (!m_pOrderData->Contract) return;

	memcpy(&stOrder, &m_pOrderData->ContractKey, sizeof(TContractKey));

	if (CheckPrice(dstopLoss, dstopProfit, stOrder.OrderPrice, stOrder.Direct))
	{
		SetStopOrderType(dstopLoss, dstopProfit, stStopInfo);
		stStopInfo.iOrderQty = stOrder.OrderQty;
		stStopInfo.dstopLoss = dstopLoss;
		stStopInfo.dstopProfit = dstopProfit;
		stStopInfo.dFloatPoint = abs(stOrder.OrderPrice - dstopLoss);
		g_pStopLP->OpenLPOrder(&stStopInfo, &stOrder);

	}	
}
void TOpenStopFrm::GetLossAndProfitPrice(double& dLoss, double& dProfit)
{
	if (!m_ckPricePoint.GetCheck())
	{
		dLoss = m_pEdits[2].GetDoubleData();
		dProfit = m_pEdits[3].GetDoubleData();
	}
	else
	{
		int loss = m_pEdits[2].GetWindowNumber();
		int profit = m_pEdits[3].GetWindowNumber();
		double dPrice = m_pEdits[1].GetDoubleData();
		if (loss != 0)
		{
			if(m_bIsBuy)
				dLoss = dPrice - loss*m_pCommodityInfo->dMiniChangePrice;
			else
				dLoss=dPrice+loss*m_pCommodityInfo->dMiniChangePrice;
		}
		else
		{
			dLoss = 0;
		}

		if (profit != 0)
		{
			if(m_bIsBuy) //区分买卖
				dProfit = dPrice + profit*m_pCommodityInfo->dMiniChangePrice;
			else
				dProfit = dPrice - loss*m_pCommodityInfo->dMiniChangePrice;
		}
		else
		{
			dProfit = 0;
		}
	}
}
void TOpenStopFrm::SetStopOrderType(const double& dstopLoss, const double& dstopProfit, AutoStopInfo& stStopInfo)
{
	if (dstopProfit == 0 && dstopLoss == 0)
	{
		stStopInfo.bLoss = false;
		stStopInfo.bFloat = false;
		stStopInfo.bProfit = false;
	}
	else if (m_bIsDynamic)
	{
		stStopInfo.bFloat = true;
		stStopInfo.bLoss = false;
		if (dstopProfit == 0)
			stStopInfo.bProfit = false;
		else
			stStopInfo.bProfit = true;
	}
	else
	{
		stStopInfo.bFloat = false;
		if (dstopProfit == 0)
			stStopInfo.bProfit = false;
		else
			stStopInfo.bProfit = true;
		if (dstopLoss == 0)
			stStopInfo.bLoss = false;
		else
			stStopInfo.bLoss = true;
	}
}
bool TOpenStopFrm::CheckPrice(const double& dstopLoss, const double& dstopProfit,const double& OrderPrice, TDirect direct)
{
	if (dBuy == direct) //买
	{
		if (dstopLoss >= OrderPrice) //止损价应该小于委托价
		{
			_TMessageBox_Domodal(m_Hwnd,_GetTextOs(Warning_Index), _GetTextOsW(StopLossHigh_Index));
			return false;
		}
		if (dstopProfit != 0 && dstopProfit <= OrderPrice)//止盈价应该大于委托价
		{
			_TMessageBox_Domodal(m_Hwnd, _GetTextOs(Warning_Index), _GetTextOsW(StopProfitLow_Index));
			return false;
		}
		return true;
	}
	else  //卖
	{
		if (dstopProfit >= OrderPrice) //止盈价不应该高于委托价
		{
			_TMessageBox_Domodal(m_Hwnd, _GetTextOs(Warning_Index), _GetTextOsW(StopProfitHigh_Index));
			return false;
		}
		if (dstopLoss != 0 && dstopLoss <= OrderPrice)//止损价不应该小于委托价
		{
			_TMessageBox_Domodal(m_Hwnd, _GetTextOs(Warning_Index), _GetTextOsW(StopLossLow_Index));
			return false;
		}
		return true;
	}
}


void TOpenStopFrm::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if (!OnClickBuyOrSell(pt))
	{
		OnClickOrderBtn(pt);
	}
}
bool TOpenStopFrm::OnClickBuyOrSell(POINT pt)
{
	if ((PtInRect(&m_rcBuy, pt) && !m_bIsBuy) || (PtInRect(&m_rcSell, pt) && m_bIsBuy))
	{
		m_bIsBuy = !m_bIsBuy;
		SetPrice();
		RECT rc = m_rcBuy;
		rc.right = m_rcBtnOrder.right;
		InvalidateRect(m_Hwnd, &rc, true);
		return true;
	}
	return false;
}
void TOpenStopFrm::SetPrice()
{
	char szText[100];
	if (m_bIsBuy)
		DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->SellPrice, szText);
	else
		DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->BuyPrice, szText);
	m_pEdits[1].SetText(szText);
}
void TOpenStopFrm::OnClickOrderBtn(POINT pt)
{
	if (PtInRect(&m_rcBtnOrder, pt))
	{
		m_bIsCilckOrderBtn = true;
		SetFocus(m_Hwnd);
		InvalidateRect(m_Hwnd, &m_rcBtnOrder, true);
	}
}
void TOpenStopFrm::DrawMain(TMemDC &pMemDC)
{
	int left = textLeft;
	int top = text1Top;
	RECT rc = { left, top, 200, textHeight + top };
	string strText = _GetTextOs(Contract_Index);
	if (m_pOrderData)
		strText += m_pOrderData->szContractNo;
	DrawText(pMemDC.GetHdc(),ANSIToUnicode(strText).c_str(), -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	HPEN hPen = CreatePen(PS_SOLID, 2, g_ColorRefData.g_ColorGray);
	SelectObject(pMemDC.GetHdc(), hPen);
	top = 92;
	MoveToEx(pMemDC.GetHdc(), left, top, 0);
	LineTo(pMemDC.GetHdc(), Wnd_Width-10, top);


	DrawPosition(&pMemDC);
	DrawBuySellBtn(&pMemDC);
	DrawOrderBtn(&pMemDC);
	
	SetTextColor(pMemDC.GetHdc(),g_ColorRefData.g_ColorBuyRed);
	rc.left = 87; rc.top = 179;
	int widht = 28;
	if (g_language == ENU)
	{
		rc.left = 80;
		widht = 35;
	}
	rc.bottom = rc.top + 20; rc.right = rc.left + widht;
	DrawText(pMemDC.GetHdc(), _GetTextOsW(LimitUp_Index), -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	rc.left = 178; 
	widht = 28;
	if (g_language == ENU)
	{
		rc.left = 168;
		widht = 50;
	}
	rc.right = rc.left + widht;
	SetTextColor(pMemDC.GetHdc(), g_ColorRefData.g_ColorTextGreen);
	DrawText(pMemDC.GetHdc(), _GetTextOsW(LimitDown_Index), -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(hPen);

}
void TOpenStopFrm::DrawPosition(TMemDC *pMemDC)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.left = textLeft; rect.top = text2Top;
	rect.bottom = rect.top + textHeight;
	char szText[100] = { 0 };
	int textWidth = 40;
	int length = rect.right;
	if (m_pOrderData->iBuyPosQty)
	{
		SetTextColor(pMemDC->GetHdc(), RGB(240, 10, 10));
		sprintf_s(szText, " %d%s%s", m_pOrderData->iBuyPosQty, _GetTextOs(Lots_Index), _GetTextOs(Position_Index));
		rect.right = rect.left + textWidth;
		DrawText(pMemDC->GetHdc(), _GetTextOsW(Long_Index), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		SetTextColor(pMemDC->GetHdc(), RGB(10, 10, 10));
		rect.left = rect.right; rect.right = length / 2;
		DrawText(pMemDC->GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	if (m_pOrderData->iSellPosQty)
	{
		rect.left = rect.right;
		rect.right *= 2;
		SetTextColor(pMemDC->GetHdc(), RGB(10, 240, 10));
		rect.left = length / 2;
		rect.right = rect.left + textWidth;
		DrawText(pMemDC->GetHdc(), _GetTextOsW(Short_Index), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		rect.left = rect.right;
		rect.right = length;
		sprintf_s(szText, " %d%s%s", m_pOrderData->iSellPosQty, _GetTextOs(Lots_Index), _GetTextOs(Position_Index));
		SetTextColor(pMemDC->GetHdc(), RGB(10, 10, 10));
		DrawText(pMemDC->GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
}
void TOpenStopFrm::DrawBuySellBtn(TMemDC *pMemDC)
{
	COLORREF btn_buy_color = RGB(68, 138, 197); //foucs color
	COLORREF btn_sell_color = RGB(215, 227, 241);//unfocus color
	if (!m_bIsBuy)
	{
		btn_buy_color = RGB(215, 227, 241);
		btn_sell_color = RGB(68, 138, 197);
	}
	HBRUSH hbrBuy = CreateSolidBrush(btn_buy_color);
	HBRUSH hbrSell = CreateSolidBrush(btn_sell_color);
	HBRUSH hbrBorder = CreateSolidBrush(RGB(43, 115, 174));
	FillRect(pMemDC->GetHdc(), &m_rcBuy, hbrBuy);
	FillRect(pMemDC->GetHdc(), &m_rcSell, hbrSell);
	RECT rc = m_rcBuy;
	rc.right = m_rcSell.right;
	FrameRect(pMemDC->GetHdc(), &rc, hbrBorder);
	SelectObject(pMemDC->GetHdc(), g_FontData.g_FontWord12);
	COLORREF buy_text_color = RGB(255, 255, 255);
	COLORREF sell_text_color = RGB(18, 18, 18);
	if (!m_bIsBuy)
	{
		buy_text_color = RGB(18, 18, 18);
		sell_text_color = RGB(255, 255, 255);
	}
	SetTextColor(pMemDC->GetHdc(), buy_text_color);
	DrawText(pMemDC->GetHdc(), _GetTextOsW(Buy_Index), -1, &m_rcBuy, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
	SetTextColor(pMemDC->GetHdc(), sell_text_color);
	DrawText(pMemDC->GetHdc(), _GetTextOsW(Sell_Index), -1, &m_rcSell, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
	DeleteObject(hbrBuy);
	DeleteObject(hbrSell);
	DeleteObject(hbrBorder);
}
void TOpenStopFrm::DrawOrderBtn(TMemDC *pMemDC)
{
	COLORREF color_bk = RGB(199, 69, 71);
	COLORREF color_border = RGB(144, 28, 28);

	string str = _GetTextOs(BuyOpen_Index);
	if (!m_bIsBuy)
	{
		color_bk = RGB(64, 158, 96);
		color_border = RGB(32,117,60);
		str.assign(_GetTextOs(SellOpen_Index));
	}
	HBRUSH hbrBk = CreateSolidBrush(color_bk);
	HBRUSH hbrBorder = CreateSolidBrush(color_border);
	FillRect(pMemDC->GetHdc(), &m_rcBtnOrder, hbrBk);
	FrameRect(pMemDC->GetHdc(), &m_rcBtnOrder, hbrBorder);
	SetTextColor(pMemDC->GetHdc(), g_ColorRefData.g_ColorWhite);
	RECT rc = m_rcBtnOrder;
	if (m_bIsCilckOrderBtn)
	{
		InflateRect(&rc, -3, -2);
		HBRUSH hbr = CreateSolidBrush(RGB(56, 186, 184));
		FrameRect(pMemDC->GetHdc(), &rc, hbr);
		DeleteObject(hbr);
		InflateRect(&rc, 3, 2);
		rc.top += 2;
	}
	DrawText(pMemDC->GetHdc(), ANSIToUnicode(str).c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(hbrBk);
	DeleteObject(hbrBorder);
}