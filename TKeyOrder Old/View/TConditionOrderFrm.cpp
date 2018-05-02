#include "BaseInclude.h"

#define  closeWidth      9
#define  captionHeight   28

#define  widgetLeft      15
#define  widgetTop       (captionHeight + 7)
#define  widgetHeight    20

#define  ckWidth         120

#define  frameWidth     420
#define  frameHeight    290

#define  btnWidth       80
#define  btnHeight      21

#define  noticeSender   "ThreeKeyOrder"
#define  sendTimeOrder  "InsertConditionT"
#define  sendPriceOrder "InsertConditionP"


char szChs[][25] = { "合约","高级","简化","止损点数","止盈点数","回撤点数","止损价格","止盈价格","时间到达",
"订单", "元", "手", "确定", "取消", "止损", "买价", "卖价", "不设置", "限价止损+限价止盈", "动态追踪", "限价止损", "限价止盈", "指定价格止损止盈",
"提示"};
char szCht[][25] = { "合約", "高級", "簡化", "止損點數", "止盈點數", "回撤點數", "止損價格", "止盈價格", "時間到達",
"訂單", "元", "手", "確定", "取消", "止損", "買價", "賣價","不設置", "限價止損+限價止盈", "動態追蹤", "限價止損", "限價止盈", "指定價格止損止盈",
"提示"};
char szEnu[][25] = { "Contract","Advanced","Simple","Stop Loss Point","Stop Profit Point","Floating Point","Stop Loss Price","Stop Profit Price","Time to",
"Order","Yuan","Qty","OK","Cancel","Stop","BidPrice","AskPrice","None","LimitStop","Floating Stop","LimitStop Loss","LimitStop Profit","Specified Price Stop",
"Warning"};

enum
{
	Contract_Index,
	Advanced_Index,
	Simple_Index,
	StopLossPoint_Index,
	StopProfitPoint_Index,
	FloatingPoint_Index,
	StopLossPrice_Index,
	StopProfitPrice_Index,
	TimeTo_Index,
	OrderText_Index,
	Yuan_Index,
	OrderQty_Index,
	OK_Index,
	Cancel_Index,
	StopLoss_Index,
	BidPrice_Index,
	AskPrice_Index,
	NotSet_Index,
	LimitStop_Index,
	FloatingStop_Index,
	LimitStopLoss_Index,
	LimitStopProfit_Index,
	SpecPriceStop_Index,
	Warning_Index,
};

char*  _GetText(int i)
{
	if (g_language == CHT) return szCht[i];
	else if (g_language == ENU) return szEnu[i];
	return szChs[i];
}

TCHAR szInfoChsW[][55] = { L"触发价格不能为0 !!", L"触发价格设置不合理，已到触发条件 !!", L"委托价格不能为0 !!", L"委托数量不能为0 !!" ,
L"止损价格不能低于跌停价 !!", L"止盈价格不能高于涨停价 !!",L"止损价格不能高于涨停价",L"止盈价格不能低于跌停价" };
TCHAR szInfoChtW[][55] = { L"觸發價格不能為0 !!", L"觸發價格設置不合理，已到觸發條件 !!", L"委托價格不能為0 !!", L"委托數量不能為0 !!",
L"止損價格不能低於跌停價 !!", L"止盈價格不能高於漲停價 !!", L"止損價格不能高於漲停價", L"止盈價格不能低於跌停價" };
TCHAR szInfoEnuW[][55] = { L"Trigger Price can not be 0 !", L"Trigger price is inappropriate !",L"Order price can not be 0 !", L"Order Qrt can not be 0 !",
L"The stop loss price is less than limit down price !",L"The stop profit price is higher than limit up price !",
L"The stop loss price is higher than limit up price !",L"The profit loss price is less than limit down price !"};

enum{
	Error_TriggerPrice_0,
	Error_TriggerPriceUn,
	Error_OrderPrice_0,
	Error_OrderQty_0,
	Error_LossPriceLess,
	Error_ProfitPriceHigh,
	Error_LossPriceHigh,
	Error_ProfitPriceLess,

};

TCHAR * _GetInfoW(int i)
{
	if (g_language == CHT)return szInfoChtW[i];
	else if (g_language == ENU)return szInfoEnuW[i];
	return szInfoChsW[i];
}

TConditionOrderFrm::TConditionOrderFrm(IMediator *pMediator):IColleague(pMediator)
{
	m_Direct = dBuy;
	m_Offset = oOpen;
	m_TriggerCondition = tcGreaterEqual;
	m_iCkSelect = id_ckPrice;

	m_bIsSimple = false;
	m_iStopLPMethod = stopNone;
	m_iPriceType = ptCounterPrice;
}
TConditionOrderFrm::~TConditionOrderFrm()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}
void TConditionOrderFrm::Update(bool bUpdateAll)
{
	char szText[100] = { 0 };
	char format[20] = { 0 };
	sprintf_s(format, "%%.%dlf", m_pCommodityInfo->iPrecision);
	sprintf_s(szText, format, m_pQuoteData->BuyPrice);
	m_labelBuyPrice.SetText(szText);
	sprintf_s(szText, format, m_pQuoteData->SellPrice);
	m_labelSellPeice.SetText(szText);
	sprintf_s(szText, format, m_pQuoteData->LastPrice);
	m_labelNewPrice.SetText(szText);
	if (!m_pOrderData->Contract ||!bUpdateAll)
	{
		return;
	}


	m_spinStopLP[0].SetWindowNumber(0);
	m_spinStopLP[1].SetWindowNumber(0);	

	m_dtTime.UpdateTime();

	TStopOrderPrice2Char(ptCounterPrice, szText, sizeof(szText));
	m_peditOrderPrice.SetText(szText);

	double price = 0.0;
	if (m_TriggerCondition == tcGreaterEqual)
		price = m_pQuoteData->LastPrice + m_pCommodityInfo->dMiniChangePrice;
	else
		price = m_pQuoteData->LastPrice - m_pCommodityInfo->dMiniChangePrice;
	sprintf_s(szText, format, price);
	//m_labelBuyPrice.SetText(szText);
	m_peditConditionPrice.SetText(szText);


//	sprintf_s(szText, "%s: %s%s",_GetText(Contract_Index), m_pOrderData->pContractKey->CommodityNo, m_pOrderData->pContractKey->ContractNo);
//	m_labelContract.SetText(szText);
	sprintf_s(szText, "%s:%s", _GetText(Contract_Index), m_pOrderData->szContractNo);
	m_labelContract.SetText(szText);

	m_cbOffset.Clear();
	TOffset offset = oOpen;
	TOffset2Char(offset, szText, sizeof(szText));
	m_cbOffset.AddString(LoadRC::ansi_to_unicode(szText).c_str());
	offset = oCover;
	TOffset2Char(offset, szText, sizeof(szText));
	m_cbOffset.AddString(LoadRC::ansi_to_unicode(szText).c_str());
	bool setSelect = false;
	if (strcmp(m_pOrderData->ContractKey.ExchangeNo, exSHFE) == 0)
	{
		offset = oCoverT;
		TOffset2Char(offset, szText, sizeof(szText));
		m_cbOffset.AddString(LoadRC::ansi_to_unicode(szText).c_str());

		setSelect = true;
	}

	if (m_Offset == oOpen)
		m_cbOffset.SetSelect(0);
	else if (m_Offset == oCover)
		m_cbOffset.SetSelect(1);
	else if (m_Offset == oCoverT)
	{
		if (setSelect)
			m_cbOffset.SetSelect(2);
		else
			m_cbOffset.SetSelect(1);
	}

}
void TConditionOrderFrm::ShowStopLoss()
{
	m_bIsSimple = !m_bIsSimple;

	if (m_bIsSimple)
	{
		m_iInfoTextTop = captionHeight + 183;
		m_btnSimple.SetButtonText(_GetText(Advanced_Index));
		ShowWindow(m_labStopLoss.GetHwnd(), SW_HIDE);
		ShowWindow(m_cbStopLossMethod.GetHwnd(), SW_HIDE);
		ShowWindow(m_labStopLP[0].GetHwnd(), SW_HIDE);
		ShowWindow(m_labStopLP[1].GetHwnd(), SW_HIDE);
		m_spinStopLP[0].Show(false);
		m_spinStopLP[1].Show(false);
	//	MoveWindow();
	//	SetWindowPos(m_Hwnd, NULL, 0, 0, frameWidth, frameHeight, /*SWP_NOMOVE|*/SWP_NOZORDER|SWP_SHOWWINDOW);

	}
	else
	{
		m_iInfoTextTop = captionHeight + 238;
		m_btnSimple.SetButtonText(_GetText(Simple_Index));
		ShowWindow(m_labStopLoss.GetHwnd(), SW_SHOW);
		ShowWindow(m_cbStopLossMethod.GetHwnd(), SW_SHOW);
		ShowStopLPWidget();
		//SetWindowPos(m_Hwnd, NULL, 0, 0, frameWidth, frameHeight +50, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	MoveWindow();
	SetBtnPos();
	
	UpdateWindow(m_Hwnd);
	InvalidateRect(m_Hwnd, NULL, false);
}

void TConditionOrderFrm::SetBtnPos()
{
	int top = m_iInfoTextTop + 42;
	int left = frameWidth - 20 - btnWidth;
	m_btnCancel.MoveWindow(left, top, btnWidth, btnHeight);
	left = left - btnWidth - 10;
	m_btnConfirm.MoveWindow(left, top, btnWidth, btnHeight);
	m_btnSimple.MoveWindow(widgetLeft, top, btnWidth, btnHeight);
}
void TConditionOrderFrm::GetPriceType()
{
	if (m_peditOrderPrice.IsText())
	{
		TCHAR wszText[100] = { 0 };
		m_peditOrderPrice.GetText(wszText, sizeof(wszText) / sizeof(wszText[0]));
		wstring wstrText = wszText;
		m_iPriceType = ::GetPriceType(wstrText);
	}
	else
	{
		m_iPriceType = ptSpecifiedPrice;
	}
}

LRESULT TConditionOrderFrm::OnDealMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message==WM_CTLCOLORSTATIC)
		return OnCtrlColor(wParam,lParam);
	else if (message == SSWM_CLICK_CHECKBOX)
	{
		HWND hwnd = (HWND)lParam;
		if (hwnd == m_ckPriceCondition.GetHwnd())
		{
			OnClickCheckBox(&m_ckPriceCondition, id_ckPrice);
		}
		else if (hwnd == m_ckTimeCondition.GetHwnd())
		{
			OnClickCheckBox(&m_ckTimeCondition, id_ckTime);
		}
		else if (hwnd == m_ckPreOrder.GetHwnd())
		{
			OnClickCheckBox(&m_ckPreOrder, id_ckPreOrder);
		}
		else if (hwnd == m_ckOpenCondition.GetHwnd())
		{
			OnClickCheckBox(&m_ckOpenCondition, id_ckOpen);
		}
		return PROCESSED;
	}
	return NOT_PROCESSED;
}

void TConditionOrderFrm::OnCreate()
{
	CreateWidget();
	InitWidget();
	SetWidgetPos();

	if (!m_bIsModify)
		ShowStopLoss();
	//else
	//	SetBtnPos();

	SetNeedDestroy(false);  //不需要点击关闭按钮destroy window

}
void TConditionOrderFrm::DrawMain(TMemDC &memdc)
{
	RECT rect;
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord12);
//	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	HPEN hPen = CreatePen(PS_SOLID, 2, g_ColorRefData.g_ColorGray);
	HPEN hOld = (HPEN)SelectObject(memdc.GetHdc(), hPen);
	MoveToEx(memdc.GetHdc(), widgetLeft, m_iInfoTextTop - 5, 0);
	LineTo(memdc.GetHdc(), widgetLeft + 400, m_iInfoTextTop - 5);
	DeleteObject(hPen);
	SelectObject(memdc.GetHdc(), hOld);
	rect = { widgetLeft, m_iInfoTextTop, widgetLeft + 400, m_iInfoTextTop + 30 };
	if (g_language == CHT)
		DrawText(memdc.GetHdc(), L"“時間條件單”和“價格條件單”為本地觸發，自動單有些後臺不支持。如需修改或者刪除改條件單，請在本地條件單列表中操作。", -1, &rect, DT_LEFT | DT_WORDBREAK);
	else if (g_language == ENU)
		DrawText(memdc.GetHdc(), L"Time and price condition order are triggered in local, some background not support auto order. You can \
								  modify or delete the order in the condition order list.", -1, &rect, DT_LEFT | DT_WORDBREAK);
	else
		DrawText(memdc.GetHdc(), L"“时间条件单”和“价格条件单”为本地触发，自动单有些后台不支持。如需修改或者删除改条件单，请在本地条件单列表中操作。", -1, &rect, DT_LEFT | DT_WORDBREAK);
}

void TConditionOrderFrm::ResetCheckBox()
{
	m_ckPriceCondition.SetCheck(false);
 	m_ckOpenCondition.SetCheck(false);
	m_ckPreOrder.SetCheck(false);
	m_ckTimeCondition.SetCheck(false);
}
void TConditionOrderFrm::ShowWidget(UINT type)
{
	UINT priceShow = SW_HIDE;
	UINT timeShow = SW_HIDE;
	if (type == id_ckPrice)
	{
		priceShow = SW_SHOW;
	}
	else if (type == id_ckTime)
	{
		timeShow = SW_SHOW;
		m_dtTime.UpdateTime();
	}

	ShowWindow(m_labNewPrice.GetHwnd(), priceShow);
	ShowWindow(m_cbPriceCondition.GetHwnd(), priceShow);
	ShowWindow(m_peditConditionPrice.GetEditHwnd(), priceShow);
	ShowWindow(m_peditConditionPrice.GetSpinHwnd(), priceShow);
	ShowWindow(m_ckIsServerOrder.GetHwnd(), priceShow);

	ShowWindow(m_labelTime.GetHwnd(), timeShow);
	ShowWindow(m_dtTime.GetHwnd(), timeShow);
}
bool TConditionOrderFrm::OnClickCheckBox(CxCheck *pCheck, UINT type)
{
	if (pCheck->GetCheck() == false)
	{
		pCheck->SetCheck(true);
		//return false;//屏蔽连续点击，因为后续操作都是从false变为true之后的操作
	}
	ResetCheckBox();
	pCheck->SetCheck(true);
	m_iCkSelect = type;
	ShowWidget(type);
	return true;
}
LRESULT TConditionOrderFrm::OnNotify(WPARAM wParam, LPARAM lParam)
{
	uint id = LOWORD(wParam);
	LPNMUPDOWN p = (LPNMUPDOWN)lParam;
	char Price[20] = { 0 };
	CxSpinEdit* pEdit = nullptr;
	double dNow = 0;
	if (id == id_spinOrderPrice || id == id_spinPriceCondition)
	{

		m_iPriceType = ptSpecifiedPrice;
		if (id == id_spinOrderPrice)
			pEdit = &m_peditOrderPrice;
		else if (id == id_spinPriceCondition)
			pEdit = &m_peditConditionPrice;
		if (!pEdit) return PROCESSED;

		if (pEdit->IsText())
		{
			double dLastPrice = 0;
			CConvert::GetPrice(&m_pOrderData->ContractKey, m_Direct, ptLastPrice, dLastPrice);
			//dNow = m_pQuoteData->LastPrice;
			dNow = dLastPrice;
		}
		else
			dNow = pEdit->GetDoubleData();
		if (p->iDelta > 0)
			dNow += m_pCommodityInfo->dMiniChangePrice;
		else
			dNow -= m_pCommodityInfo->dMiniChangePrice;
		DoubleToChar(m_pCommodityInfo->iPrecision, dNow, Price);
		pEdit->SetText(Price);
	}
	else if (id == id_spinStopLP0 || id == id_spinStopLP1)
	{
		if (id == id_spinStopLP0)
			pEdit = &m_spinStopLP[0];
		else if (id == id_spinStopLP1)
			pEdit = &m_spinStopLP[1];
		if (!pEdit) return PROCESSED;
		if (m_iStopLPMethod != stopSpeciLossProfit)
		{
			int iNow = pEdit->GetWindowNumber();
			if (p->iDelta > 0)
				iNow += 1;
			else
				iNow -= 1;
			pEdit->SetWindowNumber(iNow);
		}
		else
		{
			dNow = pEdit->GetDoubleData();
			if (p->iDelta > 0)
				dNow += m_pCommodityInfo->dMiniChangePrice;
			else
				dNow -= m_pCommodityInfo->dMiniChangePrice;
			DoubleToChar(m_pCommodityInfo->iPrecision, dNow, Price);
			pEdit->SetText(Price);
		}
	}
	return PROCESSED;
}
LRESULT TConditionOrderFrm::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		double dOrderPrice = 0.0;
		if (LOWORD(wParam) == id_labelNew || LOWORD(wParam) == id_labelNewPrice)
		{
			dOrderPrice = m_pQuoteData->LastPrice;
		}
		else if (LOWORD(wParam) == id_labelBuy || LOWORD(wParam) == id_labelBuyPrice)
		{
			dOrderPrice = m_pQuoteData->BuyPrice;
		}
		else if (LOWORD(wParam) == id_labelSell || LOWORD(wParam) == id_labelSellPrice)
		{
			dOrderPrice = m_pQuoteData->SellPrice;
		}
		if (dOrderPrice > 0.0)
		{
			char szText[100] = { 0 };
			DoubleToChar(m_pCommodityInfo->iPrecision, dOrderPrice, szText);
			m_peditOrderPrice.SetText(szText);
		}
	}
	else if ((HWND)lParam == m_peditOrderPrice.GetEditHwnd())
	{
		if ( HIWORD(wParam) == EN_SETFOCUS)
		{
			char Price[20] = { 0 };
			double dPrice = 0;
			CConvert::GetPrice(&m_pOrderData->ContractKey, m_Direct, ptLastPrice, dPrice);
			//DoubleToChar(m_pCommodityInfo->iPrecision, m_pQuoteData->LastPrice, Price);
			DoubleToChar(m_pCommodityInfo->iPrecision, dPrice, Price);
			m_peditOrderPrice.SetText(Price);
			SetFocus(m_peditOrderPrice.GetEditHwnd());
			PostMessage(m_peditOrderPrice.GetEditHwnd(), EM_SETSEL, 0, -1);

			OnPriceType();
		}
	}
	else if ((HWND)lParam == m_peditOrderQty.GetEditHwnd())
	{
		if (HIWORD(wParam) == EN_CHANGE)
		{
			int iQty = m_peditOrderQty.GetWindowNumber();
			if (iQty == 0)
				m_peditOrderQty.SetWindowNumber(1);
			else if (iQty > 1000)
				m_peditOrderQty.SetWindowNumber(1000);
		}
	}
	else if ((HWND)lParam == m_spinStopLP[0].GetEditHwnd())
	{
		SetSpinStopLp(&m_spinStopLP[0]);
	}
	else if ((HWND)lParam == m_spinStopLP[1].GetEditHwnd())
	{
		SetSpinStopLp(&m_spinStopLP[1]);
	}
	else if ((HWND)lParam == m_cbPriceCondition.GetHwnd() && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbPriceCondition.GetSelect();
		m_TriggerCondition = tcGreaterEqual;
		if (index == 1)
			m_TriggerCondition = tcLessEqual;
	}
	else if ((HWND)lParam == m_cbDirect.GetHwnd() && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbDirect.GetSelect();
		m_Direct = dBuy;
		if (index == 1)
			m_Direct = dSell;
	}
	else if ((HWND)lParam == m_cbOffset.GetHwnd() && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbOffset.GetSelect();
		m_Offset = oOpen;
		if (index == 1)
			m_Offset = oCover;
		else if (index == 2)
			m_Offset = oCoverT;
	}
	else if ((HWND)lParam == m_cbStopLossMethod.GetHwnd() && HIWORD(wParam) == CBN_SELCHANGE)
	{
		m_iStopLPMethod = m_cbStopLossMethod.GetSelect();
		ShowStopLPWidget();
	}
	return PROCESSED;
}

void TConditionOrderFrm::SetSpinStopLp(CxSpinEdit* pEdit)
{
	int iPoint = pEdit->GetWindowNumber();
	if (iPoint < 0)
		pEdit->SetWindowNumber(0);
	if (m_iStopLPMethod != stopSpeciLossProfit)
	{
		if (iPoint > 100)
			pEdit->SetWindowNumber(100);
	}
}

void TConditionOrderFrm::HideSecond()
{
	m_spinStopLP[0].SetWindowNumber(0);
	ShowWindow(m_labStopLP[0].GetHwnd(), SW_SHOW);
	ShowWindow(m_labStopLP[1].GetHwnd(), SW_HIDE);
	m_spinStopLP[0].Show(true);
	m_spinStopLP[1].Show(false);
}
void TConditionOrderFrm::ShowFirstSecond()
{
	m_spinStopLP[0].SetWindowNumber(0);
	m_spinStopLP[1].SetWindowNumber(0);
	ShowWindow(m_labStopLP[0].GetHwnd(), SW_SHOW);
	ShowWindow(m_labStopLP[1].GetHwnd(), SW_SHOW);
	m_spinStopLP[0].Show(true);
	m_spinStopLP[1].Show(true);
}
void TConditionOrderFrm::ShowStopLPWidget()
{
	switch (m_iStopLPMethod)
	{
	case stopNone:
		ShowWindow(m_labStopLP[0].GetHwnd(), SW_HIDE);
		ShowWindow(m_labStopLP[1].GetHwnd(), SW_HIDE);
		m_spinStopLP[0].Show(false);
		m_spinStopLP[1].Show(false);
		break;
	case stopLossProfit:
		m_labStopLP[0].SetText(_GetText(StopLossPoint_Index));
		m_labStopLP[1].SetText(_GetText(StopProfitPoint_Index));
		ShowFirstSecond();
		break;
	case stopFloatChase:
		m_labStopLP[0].SetText(_GetText(FloatingPoint_Index));
		HideSecond();
		break;
	case stopLoss:
		m_labStopLP[0].SetText(_GetText(StopLossPoint_Index));
		HideSecond();
		break;
	case stopProfit:
		m_labStopLP[0].SetText(_GetText(StopProfitPoint_Index));
		HideSecond();
		break;
	case stopSpeciLossProfit:
		m_labStopLP[0].SetText(_GetText(StopLossPrice_Index));
		m_labStopLP[1].SetText(_GetText(StopProfitPrice_Index));
		ShowFirstSecond();
		break;
	}
}
void TConditionOrderFrm::OnPriceType()
{
	if (m_popupPriceType.GetHwnd() == NULL)
	{
		vector<wstring> vPriceType;
		char szText[100]{};
		TStopOrderPrice2Char(ptCounterPrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		TStopOrderPrice2Char(ptMarketPrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		TStopOrderPrice2Char(ptLastPrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		TStopOrderPrice2Char(ptSuperPrice, szText, sizeof(szText));
		vPriceType.push_back(LoadRC::ansi_to_unicode(szText));
		m_popupPriceType.CreatePopWin(this, L"PopupPriceType", m_Hwnd, 0, vPriceType, 0, 0, 0, 2, 2);

	}
	if (!IsWindowVisible(m_popupPriceType.GetHwnd()))
	{
		POINT pt = m_ptPopupPriceTypeFrm;
		ClientToScreen(m_Hwnd, &pt);
		SetWindowPos(m_popupPriceType.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	}
}
void TConditionOrderFrm::SetPriceType(wstring str)
{
	m_peditOrderPrice.SetText(str.c_str());
	m_iPriceType = ::GetPriceType(str);
}
void TConditionOrderFrm::OnBtnClick(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case id_btnCancel:
		ShowWindow(m_Hwnd, SW_HIDE);
		if (m_bIsModify)
			EnableWindow(m_hParent, true);
		OnClose();
		break;
	case id_btnConfirm:
		//SendConditionOrder();
		InsertOrder();
		break;
	case id_btnSimple:
		ShowStopLoss();
		if (m_bIsModify)
			SetSpinStopLPContent(m_pModifyOrder);
		break;
	default:
		break;
	}
}
void TConditionOrderFrm::InsertOrder()
{
	if (m_bIsModify)
	{
		GetDataFromWidget(&m_stModifyOrder);
		if (m_pModifyOrder)
		{
			m_stModifyOrder.RunState = rsRun;
			if (m_pModifyOrder->Triggered == false)
			{
				if (CheckConditionOrder(&m_stModifyOrder))
				{
					memcpy(m_pModifyOrder, &m_stModifyOrder, sizeof(TConditionOrder));
					
					m_Mediator->Send(CMD_ConView_UpdateConOrder, m_pModifyOrder, (IColleague*)100);
					EnableWindow(m_hParent, true);
					ShowWindow(m_Hwnd, SW_HIDE);
				}
			}
			else
			{
				EnableWindow(m_hParent, true);
				ShowWindow(m_Hwnd, SW_HIDE);
			}
		}
		return;
	}
	TConditionOrder *pOrder = new TConditionOrder;

	memset(pOrder, 0, sizeof(TConditionOrder));
	if (!m_pOrderData->Contract) return;

	SContract *contract[1] = {};
	SContractNoType sContractNo = {};
	CConvert::TradeContractToQContractNoType(m_pOrderData->ContractKey, sContractNo);
	g_pStarApi->GetContractData("", sContractNo, contract, 1, false);

	if (!contract[0]) return;
	pOrder->pContract = contract[0];

	pOrder->Precision = 2;  //default

	if (contract[0]->Commodity)
		pOrder->Precision = contract[0]->Commodity->PricePrec;

	SYSTEMTIME stTime = { 0 };
	GetLocalTime(&stTime);
	sprintf_s(pOrder->InsertTime, "%04d-%02d-%02d %02d:%02d:%02d", stTime.wYear, stTime.wMonth, stTime.wDay, 
		stTime.wHour, stTime.wMinute, stTime.wSecond);

	
	pOrder->RunState = rsRun;
	pOrder->Triggered = false;
	strcpy_s(pOrder->UserNo, g_pGlobalModel->m_strUserNo.c_str());
	strcpy_s(pOrder->Sign, g_pGlobalModel->m_strSign.c_str());

	GetDataFromWidget(pOrder);
	
	if (CheckConditionOrder(pOrder))
	{
		m_Mediator->Send(CMD_ConView_InsertOrder, pOrder, this);
		ShowWindow(m_Hwnd, SW_HIDE);
	}
}
void TConditionOrderFrm::GetDataFromWidget(TConditionOrder* pOrder)
{
	pOrder->bHasSendStopLP = false;
	pOrder->dMinChangePrice = m_pCommodityInfo->dMiniChangePrice;
	pOrder->Price = m_peditOrderPrice.GetDoubleData();
	pOrder->Direct = m_Direct;
	pOrder->Offset = m_Offset;
	pOrder->iQty = m_peditOrderQty.GetWindowNumber();
	GetPriceType();
	pOrder->PriceType = m_iPriceType;
	pOrder->SendToServer = m_ckIsServerOrder.GetCheck();

	strcpy_s(pOrder->ContractId, pOrder->pContract->ContractNo);

	if (m_iCkSelect == id_ckPrice)
	{
		pOrder->OrderType = cotPrice;
		pOrder->ConPrice = m_peditConditionPrice.GetDoubleData();
		pOrder->TriggerCondition = m_TriggerCondition;

		if (m_ckIsServerOrder.GetCheck())
			pOrder->OrderType = cotPriceServer;
	}
	else if (m_iCkSelect == id_ckTime)
	{
		pOrder->OrderType = cotTime;
		m_dtTime.GetTime(pOrder->TimeOut);
		pOrder->iTimeOut = m_dtTime.GetTime();
	}
	else if (m_iCkSelect == id_ckOpen)
	{
		pOrder->OrderType = cotAutoOrder;
	}
	else if (m_iCkSelect == id_ckPreOrder)
	{
		pOrder->OrderType = cotPreOrder;
	}

	GetStopLPInfo(pOrder);
}
bool TConditionOrderFrm::CheckConditionOrder(TConditionOrder* pOrder)
{
	if (m_iCkSelect == id_ckPrice)
	{
		double dLastePrice = 0;
		CConvert::GetPrice(&m_pOrderData->ContractKey, m_Direct, ptLastPrice, dLastePrice);
		if (pOrder->ConPrice == 0)
		{
			_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_TriggerPrice_0), TMB_OK);
			return false;
		}
		else if (pOrder->TriggerCondition == tcGreaterEqual && pOrder->ConPrice <= dLastePrice/*m_pQuoteData->LastPrice*/)
		{
			_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_TriggerPriceUn), TMB_OK);
			return false;
		}
		else if (pOrder->TriggerCondition == tcLessEqual && pOrder->ConPrice >= dLastePrice /*m_pQuoteData->LastPrice*/)
		{
			_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_TriggerPriceUn), TMB_OK);
			return false;
		}
	}
	if (pOrder->PriceType == ptSpecifiedPrice &&pOrder->Price == 0)
	{
		_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_OrderPrice_0), TMB_OK);
		return false;
	}
	else if (pOrder->iQty == 0)
	{
		_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_OrderQty_0), TMB_OK);
		return false;
	}
	if (pOrder->StopType == stopSpeciLossProfit && m_pQuoteData->DownPrice!=0 && m_pQuoteData->UpPrice!=0)
	{
		if (pOrder->LossPrice!=0)
		{
			if (pOrder->LossPrice < m_pQuoteData->DownPrice)
			{
				_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_LossPriceLess), TMB_OK);
				return false;
			}
			if (pOrder->LossPrice > m_pQuoteData->UpPrice)
			{
				_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_LossPriceHigh), TMB_OK);
				return false;
			}
		}
		if (pOrder->ProfitPrice!=0 )
		{
			if(pOrder->ProfitPrice > m_pQuoteData->UpPrice)
			{
				_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_ProfitPriceHigh), TMB_OK);
				return false;
			}
			if (pOrder->ProfitPrice < m_pQuoteData->DownPrice)
			{
				_TMessageBox_Domodal(m_Hwnd, _GetText(Warning_Index), _GetInfoW(Error_ProfitPriceLess), TMB_OK);
				return false;
			}
		}
	}
	return true;
}

void TConditionOrderFrm::CreateWidget()
{
	m_labelContract.Create(m_Hwnd,LoadRC::ansi_to_unicode(_GetText(Contract_Index)).c_str());                                     

	char szText[50] = { 0 };
	TConOrderType2Char(cotPrice, szText, sizeof(szText));
	m_ckPriceCondition.Create(m_Hwnd, id_ckPrice, LoadRC::ansi_to_unicode(szText).c_str());

	TStopOrderPrice2Char(ptLastPrice, szText, sizeof(szText));
	m_labNewPrice.Create(m_Hwnd, LoadRC::ansi_to_unicode(szText).c_str());
	m_cbPriceCondition.Create(m_Hwnd,id_cbPrice);                              
	m_peditConditionPrice.Create(m_Hwnd, 0, 0, id_spinPriceCondition, CxEdit::floatStyle, false);

	TConOrderType2Char(cotTime, szText, sizeof(szText));
	m_ckTimeCondition.Create(m_Hwnd, id_ckTime, LoadRC::ansi_to_unicode(szText).c_str());
	m_labelTime.Create(m_Hwnd,LoadRC::ansi_to_unicode(_GetText(TimeTo_Index)).c_str());         
	m_dtTime.Create(m_Hwnd, DTS_TIMEFORMAT);

	TConOrderType2Char(cotAutoOrder, szText, sizeof(szText));
	m_ckOpenCondition.Create(m_Hwnd, id_ckOpen, LoadRC::ansi_to_unicode(szText).c_str());
	TConOrderType2Char(cotPreOrder, szText, sizeof(szText));
	m_ckPreOrder.Create(m_Hwnd, id_ckPreOrder, LoadRC::ansi_to_unicode(szText).c_str());

	m_labelOrder.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetText(OrderText_Index)).c_str());;
	m_cbDirect.Create(m_Hwnd);          
	m_cbOffset.Create(m_Hwnd);      
	m_peditOrderPrice.Create(m_Hwnd, 0, 0, id_spinOrderPrice, CxEdit::floatStyle, false);
	m_labelYuan.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetText(Yuan_Index)).c_str());
	m_peditOrderQty.Create(m_Hwnd);
	m_labelQty.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetText(OrderQty_Index)).c_str());

	m_btnConfirm.Create(m_Hwnd, _GetText(OK_Index), id_btnConfirm);
	m_btnCancel.Create(m_Hwnd, _GetText(Cancel_Index), id_btnCancel);

	TStopOrderPrice2Char(ptLastPrice, szText, sizeof(szText));
	strcat_s(szText, ":");
	m_labelNew.Create(m_Hwnd, LoadRC::ansi_to_unicode(szText).c_str(), SS_LEFT | SS_CENTERIMAGE, id_labelNew);
	m_labelNewPrice.Create(m_Hwnd, L"", SS_LEFT | SS_CENTERIMAGE, id_labelNewPrice);

	strcpy_s(szText, _GetText(BidPrice_Index));
	strcat_s(szText, ":");
	m_labelBuy.Create(m_Hwnd, LoadRC::ansi_to_unicode(szText).c_str(), SS_LEFT | SS_CENTERIMAGE, id_labelBuy);
	m_labelBuyPrice.Create(m_Hwnd, L"", SS_LEFT | SS_CENTERIMAGE, id_labelBuyPrice);

	strcpy_s(szText, _GetText(AskPrice_Index));
	strcat_s(szText, ":");
	m_labelSell.Create(m_Hwnd, LoadRC::ansi_to_unicode(szText).c_str(), SS_LEFT | SS_CENTERIMAGE, id_labelSell);
	m_labelSellPeice.Create(m_Hwnd, L"", SS_LEFT | SS_CENTERIMAGE, id_labelSellPrice);


	m_labStopLoss.Create(m_Hwnd, LoadRC::ansi_to_unicode(_GetText(StopLoss_Index)).c_str());
	m_cbStopLossMethod.Create(m_Hwnd);
	m_labStopLP[0].Create(m_Hwnd,L"");
	m_labStopLP[1].Create(m_Hwnd, L"");
	m_spinStopLP[0].Create(m_Hwnd, 0, 0, id_spinStopLP0, CxEdit::floatStyle, false);
	m_spinStopLP[1].Create(m_Hwnd, 0, 0, id_spinStopLP1, CxEdit::floatStyle, false);
	m_btnSimple.Create(m_Hwnd, _GetText(Advanced_Index), id_btnSimple);

	m_ckIsServerOrder.Create(m_Hwnd, id_ckServerOrder, L"下到服务器");

}
void TConditionOrderFrm::InitWidget()
{
	m_labelContract.SetFont(g_FontData.g_FontWord12);
	m_ckPriceCondition.SetCheck(true);
	m_cbPriceCondition.AddString(L">=");
	m_cbPriceCondition.AddString(L"<=");
	m_cbPriceCondition.SetSelect(0);
	m_cbPriceCondition.SetFont(g_FontData.g_FontWord12);
	
	m_peditConditionPrice.SetWindowNumber(1);
	m_peditConditionPrice.SetRange(1, 100000);

	ShowWindow(m_labelTime.GetHwnd(), SW_HIDE);
	ShowWindow(m_dtTime.GetHwnd(),SW_HIDE);

	char szText[50] = { 0 };
	TDirect direct = dBuy;
	TDirect2Char(direct, szText, sizeof(szText));
	m_cbDirect.AddString(LoadRC::ansi_to_unicode(szText).c_str());
	direct = dSell;
	TDirect2Char(direct, szText, sizeof(szText));
	m_cbDirect.AddString(LoadRC::ansi_to_unicode(szText).c_str());
	m_cbDirect.SetSelect(0);

	TOffset offset = oOpen;
	TOffset2Char(offset, szText, sizeof(szText));
	m_cbOffset.AddString(LoadRC::ansi_to_unicode(szText).c_str());
	offset = oCover;
	TOffset2Char(offset, szText, sizeof(szText));
	m_cbOffset.AddString(LoadRC::ansi_to_unicode(szText).c_str());
	//m_cbOffset.AddString(L"平今");

	m_cbOffset.SetSelect(0);

	m_peditOrderPrice.SetRange(1, 100000);
	m_peditOrderQty.SetWindowNumber(1);
	m_peditOrderQty.SetRange(1, 1000);

	m_peditOrderPrice.SetFont(g_FontData.g_FontWord12);

	m_ckPriceCondition.SetBkColor(g_ColorRefData.g_ColorBackground);
	m_ckTimeCondition.SetBkColor(g_ColorRefData.g_ColorBackground);
	m_ckOpenCondition.SetBkColor(g_ColorRefData.g_ColorBackground);
	m_ckPreOrder.SetBkColor(g_ColorRefData.g_ColorBackground);
	m_ckIsServerOrder.SetBkColor(g_ColorRefData.g_ColorBackground);

	TStopOrderPrice2Char(ptCounterPrice, szText, sizeof(szText));
	m_peditOrderPrice.SetText(szText);

	for (int i = 0; i < 6; i++)
	{
		m_cbStopLossMethod.AddString(LoadRC::ansi_to_unicode(_GetText(NotSet_Index+i)).c_str());
	}
	m_cbStopLossMethod.SetSelect(0);
	m_spinStopLP[0].SetRange(0, 100);
	m_spinStopLP[1].SetRange(0, 100);

	m_iInfoTextTop = captionHeight + 183;//simple
}
void TConditionOrderFrm::SetWidgetPos()
{
	int top = widgetTop;
	int left = widgetLeft;
	int width = 100;
	m_labelContract.MoveWindow(left,top,width,widgetHeight);

	top += widgetHeight + 4;
	width = ckWidth -20;
	m_ckPriceCondition.MoveWindow(left, top, width, widgetHeight);
	left += width + 5;
	width = 50;
	m_labNewPrice.MoveWindow(left,top,width,widgetHeight);
	left += width + 5;
	width = 50;
	m_cbPriceCondition.MoveWindow(left,top-1,width,widgetHeight*5);
	left += width + 5;
	width = 80;
	m_peditConditionPrice.MoveWindow(left, top, width, widgetHeight);
	//new 下到服务器
	left += width + 10;
	width = 80;
	m_ckIsServerOrder.MoveWindow(left, top, width, widgetHeight);
	//

	top += widgetHeight + 5;
	left = widgetLeft;
	width = ckWidth -20;
	m_ckTimeCondition.MoveWindow(left, top, width, widgetHeight);
	left += width + 5;
	width = 50;
	m_labelTime.MoveWindow(left, top, width, widgetHeight);
	left += width + 5;
	width = 100;
	m_dtTime.MoveWindow(left, top-1, width, widgetHeight + 2);

	top += widgetHeight + 5;
	left = widgetLeft;
	width = ckWidth;
	m_ckOpenCondition.MoveWindow(left, top, width, widgetHeight);
	top += widgetHeight + 5;
	width = 70;
	m_ckPreOrder.MoveWindow(left, top, width, widgetHeight);

	//new
	//top += 13;
	int priceTop = top + 18;
	left += width + 5;

	int firstLabWidth = 40;
	int secondLabWidth = 30;
	int thirdLabWidth = 30;
	int labPriceWidth = 60;
	if (g_language == ENU)
	{
		firstLabWidth = 55;
		secondLabWidth = 42;
		thirdLabWidth = 48;
		labPriceWidth = 50;
	}
	width = firstLabWidth;
	m_labelNew.MoveWindow(left, priceTop, width, widgetHeight);
	left += width;
	width = labPriceWidth;
	m_labelNewPrice.MoveWindow(left, priceTop, width, widgetHeight);
	left += width + 5;
	width = secondLabWidth;
	m_labelBuy.MoveWindow(left, priceTop, width, widgetHeight);
	left += width + 3;
	width = labPriceWidth;
	m_labelBuyPrice.MoveWindow(left, priceTop, width, widgetHeight);
	left += width + 5;
	width = thirdLabWidth;
	m_labelSell.MoveWindow(left, priceTop, width, widgetHeight);
	left += width + 3;
	width = labPriceWidth;
	m_labelSellPeice.MoveWindow(left, priceTop, width, widgetHeight);
	//new

	top += widgetHeight + 21;
	left = widgetLeft;
	width = 30;
	m_labelOrder.MoveWindow(left, top, width, widgetHeight);
	left += width + 20;
	width = 50;
	m_cbDirect.MoveWindow(left, top-1, width, widgetHeight*5);
	left += width + 5;
	width = 80;
	m_cbOffset.MoveWindow(left, top-1, width, widgetHeight*5);
	left += width + 5;
	width = 80;
	m_peditOrderPrice.MoveWindow(left, top, width, widgetHeight);
	m_ptPopupPriceTypeFrm.x = left;
	m_ptPopupPriceTypeFrm.y = top + widgetHeight;
	left += width + 3;
	width = 15;
	m_labelYuan.MoveWindow(left, top, width, widgetHeight);
	left += width + 3;
	width = 70;
	m_peditOrderQty.MoveWindow(left, top, width, widgetHeight);
	left += width + 3;
	width = 20;
	m_labelQty.MoveWindow(left, top, width, widgetHeight);

	//
	top += widgetHeight+ 10;
	left = widgetLeft;
	int labWidth = 50;
	int editWidth = 80;
	if (g_language == ENU)
	{
		labWidth = 80;
		editWidth = 60;
	}
	width = 50;
	m_labStopLoss.MoveWindow(left, top, width, widgetHeight);
	left += width;
	m_cbStopLossMethod.MoveWindow(left,top,120,widgetHeight*5);
	top += widgetHeight + 9;
	left = widgetLeft + 50;
	width = labWidth;
	m_labStopLP[0].MoveWindow(left, top, width, widgetHeight);
	left += width + 5;
	width = editWidth;
	m_spinStopLP[0].MoveWindow(left, top, width, widgetHeight);
	left += width + 15;
	width = labWidth;
	m_labStopLP[1].MoveWindow(left, top, width, widgetHeight);
	left += width + 5;
	width = editWidth;
	m_spinStopLP[1].MoveWindow(left, top, width, widgetHeight);

	//btn
	/*left = frameWidth - 20 - btnWidth;
	top = m_iInfoTextTop+70;
	m_btnCancel.MoveWindow(left, top, btnWidth, btnHeight);
	left = left - btnWidth - 10;
	m_btnConfirm.MoveWindow(left, top, btnWidth, btnHeight);
	m_btnSimple.MoveWindow(widgetLeft, top, btnWidth, btnHeight);*/

//	m_popupPriceType;
//	m_btnPriceType;
}
LRESULT TConditionOrderFrm::OnCtrlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	if ((HWND)lParam == m_labelBuy.GetHwnd())
	{
		SetTextColor((HDC)wParam, RGB(240, 0, 0));
	}
	else if ((HWND)lParam == m_labelSell.GetHwnd())
	{
		SetTextColor((HDC)wParam, RGB(0, 200, 0));
	}
	else if ((HWND)lParam == m_labelNew.GetHwnd())
	{
		SetTextColor((HDC)wParam, RGB(20, 15, 225));
	}
	return (LRESULT)g_ColorRefData.g_brush_bk;
}
void TConditionOrderFrm::MoveWindow()
{
	RECT rect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	int iFrmHeight = frameHeight;
	if (!m_bIsSimple)
		iFrmHeight += 50;
	POINT pt;
	GetCursorPos(&pt);
	int left = pt.x;
	int top = pt.y-iFrmHeight/2;
	__super::MoveWindow(left, top, frameWidth, iFrmHeight);

}

void TConditionOrderFrm::SetWidgetContent(TConditionOrder* pOrder, HWND hParent)
{
	m_pModifyOrder = pOrder;
	m_hParent = hParent;
	memcpy(&m_stModifyOrder, pOrder, sizeof(TConditionOrder));

	EnableWindow(m_ckPriceCondition.GetHwnd(), false);
	EnableWindow(m_ckPreOrder.GetHwnd(), false);
	EnableWindow(m_ckOpenCondition.GetHwnd(), false);
	EnableWindow(m_ckTimeCondition.GetHwnd(), false); 

	char format[10];
	sprintf_s(format, "%%.%df", pOrder->Precision);
	char szText[50];
	switch (pOrder->OrderType)
	{
	case cotPrice:
	case cotPriceServer:
		EnableWindow(m_ckPriceCondition.GetHwnd(), true);
		OnClickCheckBox(&m_ckPriceCondition, id_ckPrice);
		sprintf_s(szText, format, pOrder->ConPrice);
		m_peditConditionPrice.SetText(szText);
		m_cbPriceCondition.SetSelect(pOrder->TriggerCondition == tcGreaterEqual ? 0 : 1);
		m_ckIsServerOrder.SetCheck(pOrder->SendToServer);
		break;
	case cotTime:
		EnableWindow(m_ckTimeCondition.GetHwnd(), true);
		OnClickCheckBox(&m_ckTimeCondition, id_ckTime);
		m_dtTime.SetTime(pOrder->TimeOut);
		break;
	case cotAutoOrder:
		EnableWindow(m_ckOpenCondition.GetHwnd(), true);
		OnClickCheckBox(&m_ckOpenCondition, id_ckOpen);
		break;
	case cotPreOrder:
		EnableWindow(m_ckPreOrder.GetHwnd(), true);
		OnClickCheckBox(&m_ckPreOrder, id_ckPreOrder);
		break;
	}

	m_cbDirect.SetSelect(pOrder->Direct == dBuy ? 0 : 1); 
	m_Direct = pOrder->Direct;
	m_Offset = pOrder->Offset;
	m_TriggerCondition = pOrder->TriggerCondition;

	switch (pOrder->PriceType)
	{
	case ptMarket:
		TStopOrderPrice2Char(ptMarketPrice, szText, sizeof(szText));
		m_peditOrderPrice.SetText(LoadRC::ansi_to_unicode(szText).c_str());
		break;
	case ptSuperPrice:
		TStopOrderPrice2Char(ptSuperPrice, szText, sizeof(szText));
		m_peditOrderPrice.SetText(LoadRC::ansi_to_unicode(szText).c_str());
		break;
	case ptLastPrice:
		TStopOrderPrice2Char(ptLastPrice, szText, sizeof(szText));
		m_peditOrderPrice.SetText(LoadRC::ansi_to_unicode(szText).c_str());
		break;
	case ptCounterPrice:
		TStopOrderPrice2Char(ptCounterPrice, szText, sizeof(szText));
		m_peditOrderPrice.SetText(LoadRC::ansi_to_unicode(szText).c_str());
		break;
	case ptSpecifiedPrice:
	{	
		sprintf_s(szText, format, pOrder->Price);
		m_peditOrderPrice.SetText(szText);
	}
		break;
	}
	ShowWindow(m_labelNew.GetHwnd(),SW_HIDE);
	ShowWindow(m_labelNewPrice.GetHwnd(), SW_HIDE);
	ShowWindow(m_labelBuy.GetHwnd(), SW_HIDE);
	ShowWindow(m_labelBuyPrice.GetHwnd(), SW_HIDE);
	ShowWindow(m_labelSell.GetHwnd(), SW_HIDE);
	ShowWindow(m_labelSellPeice.GetHwnd(), SW_HIDE);

	m_peditOrderQty.SetWindowNumber(pOrder->iQty);

	//new
	if (pOrder->StopType != stopNone)
		m_bIsSimple = true;
	else
		m_bIsSimple = false;
	m_iStopLPMethod = pOrder->StopType;
	m_cbStopLossMethod.SetSelect(pOrder->StopType);
	ShowStopLoss();
	SetSpinStopLPContent(pOrder);
}
void TConditionOrderFrm::SetSpinStopLPContent(TConditionOrder*pOrder)
{
	switch (pOrder->StopType)
	{
	case stopLoss:
		m_spinStopLP[0].SetWindowNumber(pOrder->LossPoint);
		break;
	case stopProfit:
		m_spinStopLP[0].SetWindowNumber(pOrder->ProfitPoint);
		break;
	case stopFloatChase:
		m_spinStopLP[0].SetWindowNumber(pOrder->DrawBackPoint);
		break;
	case stopLossProfit:
		m_spinStopLP[0].SetWindowNumber(pOrder->LossPoint);
		m_spinStopLP[1].SetWindowNumber(pOrder->ProfitPoint);
		break;
	case stopSpeciLossProfit:
	{
		char format[20] = { 0 };
		sprintf_s(format, "%%.%dlf", pOrder->Precision);
		char szText[50] = { 0 };
		sprintf_s(szText, format, pOrder->LossPrice);
		m_spinStopLP[0].SetText(szText);
		sprintf_s(szText, format, pOrder->ProfitPrice);
		m_spinStopLP[1].SetText(szText);
	}
	break;
	}
}
void TConditionOrderFrm::GetStopLPInfo(TConditionOrder*pOrder)
{
	pOrder->StopType = m_iStopLPMethod;

	pOrder->LossPoint = 0;
	pOrder->ProfitPoint = 0;
	pOrder->DrawBackPoint = 0;
	pOrder->LossPrice = 0;
	pOrder->ProfitPrice = 0;
	switch (pOrder->StopType)
	{
	case stopNone:
		break;
	case stopLossProfit:
		pOrder->LossPoint = m_spinStopLP[0].GetWindowNumber();
		pOrder->ProfitPoint = m_spinStopLP[1].GetWindowNumber();
		break;
	case stopFloatChase:
		pOrder->DrawBackPoint = m_spinStopLP[0].GetWindowNumber();
		break;
	case stopLoss:
		pOrder->LossPoint = m_spinStopLP[0].GetWindowNumber();
		break;
	case stopProfit:
		pOrder->ProfitPoint = m_spinStopLP[0].GetWindowNumber();
		break;
	case stopSpeciLossProfit:
		pOrder->LossPrice = m_spinStopLP[0].GetDoubleData();
		pOrder->ProfitPrice = m_spinStopLP[1].GetDoubleData();
		break;
	}
}

void TConditionOrderFrm::OnClose()
{
	if (m_bIsModify && m_pModifyOrder&&m_pModifyOrder->Triggered == false)
	{
		m_pModifyOrder->RunState = rsRun;
		m_Mediator->Send(CMD_ConView_UpdateConOrder, 0, (IColleague*)100);
	}
}