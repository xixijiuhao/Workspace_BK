#include "PreInclude.h"
//typedef std::multimap <double, TOrder>::const_iterator OrderCIT;
//typedef pair<OrderCIT, OrderCIT> OrderRange;
//typedef std::multimap <double, TPosition>::const_iterator PositionCIT;
//typedef pair<PositionCIT, PositionCIT> PositionRange;
const double& ErrorRange = 0.00001;


int g_Width[9];


TPointOrderFrm::TPointOrderFrm()
	:m_bMouseTrack(false),
	m_bHover(false),
	m_bHasFocus(false),
	m_KBFocusOffset(0),
	m_iMouseWheelOffset(0),
	m_MouseMoveOffset(0),
	//GetKeyState(VK_CONTROL)<0(false),
	//GetKeyState(VK_MENU)<0(false),
	m_iVisiabalRow(0),
	m_dLastPrice(0),
	m_dPreSettlePrice(0),
	//iWight_Width(50),
	iWight_Height(20)
{
	pstCommodity = new SCommodity();
	memset(pstCommodity, 0, sizeof(SCommodity));
}

TPointOrderFrm::~TPointOrderFrm()
{
	delete pstCommodity;
}


bool TPointOrderFrm::Create(HWND hParent)
{

	CreateFrm(_T("=v="), hParent, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
	SetClassLong(m_Hwnd, GCL_STYLE, CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS | CS_DBLCLKS);
	if (m_Hwnd)
		m_clData.RegistHwnd(m_Hwnd);
	Init();
	g_clTips.Create();
	//判断内外盘


	return true;
}
void    TPointOrderFrm::Init()//本用于界面相关参数的初始化  现改到构造函数
{

}

void TPointOrderFrm::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, 0, true);
}
void TPointOrderFrm::SetContractNo(string sCode)
{
	m_clData.UpdataSign(&m_stOrderInfo);
	m_clData.SetContractNo(sCode, m_stOrderInfo.szUserNo, m_stConKey);
	m_sCode = sCode;
	strcpy_s(m_stConKey.Sign, m_stOrderInfo.szSign1);
	m_KBFocusOffset = 0;
	m_iMouseWheelOffset = 0;
	SQuoteSnapShot QuoteData;
	m_clData.GetQuoteData(QuoteData);
	m_dLastPrice = QuoteData.Data[S_FID_LASTPRICE].Price;
	m_dPreSettlePrice = QuoteData.Data[S_FID_PRESETTLEPRICE].Price;

	m_clPingPang.RegistConfig(m_stConfig);
	m_clPingPang.RegistHwnd(m_Hwnd);
}
void TPointOrderFrm::RegistOrderInfo(OrderInfo stOrderInfo)
{
	TDataContainer stUser;
	g_pTradeData->GetAllUser(stUser);
	if (stUser.size())
	{
		TUserInfo*pUser = (TUserInfo*)stUser.at(0);
		if (pUser->pLogin && pUser->pLogin->Logined == bYes)
		{
			if (!strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_FOREIGNTRADEAPI_NAME).c_str()) || !strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_DIPPERTRADEAPI_NAME).c_str()))
				m_bIsForeign = true;
		}
	}
	//根据内盘，外盘和语言设置来重置布局；
	if (g_LanguageID == ENU)
	{
		int Width[9];
		if (!m_bIsForeign)
		{
			int Width[9] = { 70, 60, 60, 70, 110, 70, 60, 60, 70 };
			memcpy(&g_Width[0], &Width[0], sizeof(Width));
		}
		else
		{
			int Width[9] = { 80, 0, 80, 90, 130, 90, 80, 0, 80 };
			memcpy(&g_Width[0], &Width[0], sizeof(Width));
		}
	}
	else
	{
		if (!m_bIsForeign)
		{
			int Width[9] = { 50, 40, 40, 50, 90, 50, 40, 40, 50 };
			memcpy(&g_Width[0], &Width[0], sizeof(Width));
		}
		else
		{
			int Width[9] = { 60, 0, 50, 60, 110, 60, 50, 0, 60 };
			memcpy(&g_Width[0], &Width[0], sizeof(Width));
		}
	}
	m_stOrderInfo = stOrderInfo;
}

void TPointOrderFrm::RegistConfig(PointOrderConfig* stConfig)
{
	if (!stConfig)
		return;
	m_stConfig = stConfig;
}

LRESULT TPointOrderFrm::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		break;
	case WM_SIZE:
		OnSize(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		break;
	case WM_RBUTTONDOWN:
		OnRButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		break;
	case WM_LBUTTONDBLCLK:
		OnLButtonDbClick(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave(wParam, lParam);
		break;
	case WM_SETFOCUS:
		OnSetFocus();
		return NOT_PROCESSED;
	case WM_KILLFOCUS:
		OnKillFocus();
		return NOT_PROCESSED;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		PostMessage(GetParent(), WM_ONFUNCKEYDOWN, wParam, lParam);
		if (m_stConfig->stHotKey.bHotKey)
		{
			char szText[100] = { 0 };
			sprintf_s(szText, "%d", wParam);
			g_pMainFrame->Linkage(GetParent(), "PointOrder", "KEY_DOWN", szText);
		}
	}

	case WM_ONFUNCKEYDOWN:
		OnKeyDown(wParam, lParam);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		PostMessage(GetParent(), WM_ONFUNCKEYUP, wParam, lParam);
	case WM_ONFUNCKEYUP:
		OnKeyUP(wParam, lParam);
		break;
	case WM_ENABLE:
		OnEnableWindow(wParam, lParam);
		break;
	case WM_ONQUOTE:
		OnQuote();
		return PROCESSED;
	case WM_ONORDER:
		OnOrder();
		m_clPingPang.OnDealOrder((const TOrder*)lParam);
		return PROCESSED;
	case WM_ONMATCH:
		m_clPingPang.OnDealMatch((const TMatch*)lParam);
		return PROCESSED;
	case WM_TIMER:
		m_clPingPang.OnTimer();
		return PROCESSED;
	case WM_POSITION:
		OnPosition();
		return PROCESSED;
	case WM_ONCONTRACTDATAINIT:
		OnContractDataInit();
		return PROCESSED;
	case WM_ONMONEY:
		PostMessage(GetParent(), message, wParam, lParam);
		return PROCESSED;
	case WM_CHANGEBUYSELLDIRECTION:
		InvalidateRect(m_Hwnd, 0, false);
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}


void TPointOrderFrm::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	m_MouseMoveOffset = lParam;
	if (!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMouseTrack = true;
	}
	m_bHasFocus = true;
	InvalidateRect(m_Hwnd, 0, false);
}


void TPointOrderFrm::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	m_iMouseWheelOffset -= GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
	InvalidateRect(m_Hwnd, NULL, false);
}

void TPointOrderFrm::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	RECT	 rect;
	GetClientRect(m_Hwnd, &rect);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	HBRUSH hbr;
	hbr = CreateSolidBrush(g_ColorRefData.GetColorWhite());
	FillRect(memdc.GetHdc(), &rect, hbr);
	DeleteObject(hbr);
	DrawMainData(memdc);
}

void TPointOrderFrm::OnSize(WPARAM wParam, LPARAM lParam)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	m_iVisiabalRow = y / iWight_Height;
}
int TPointOrderFrm::GetCol(int x)
{
	int iWidth = 0;
	for (int i = 0; i < 9; i++)
	{
		if (iWidth < x&&x < iWidth + g_Width[i])
			return i;
		iWidth += g_Width[i];
	}
}
void TPointOrderFrm::DrawMainData(TMemDC &memdc)
{
	if (!CConvert::GetCommodity(*pstCommodity, &m_stConKey)) {
		return;
	}

	double dLastPrice = 0;

	//GetQuoteData
	SQuoteSnapShot QuoteData = {};
	SQuoteSnapShotL2 buyQuoteData = {};
	SQuoteSnapShotL2 sellQuoteData = {};
	SImpliedDepthL2 impliedDepth = {};
	m_clData.GetQuoteData(QuoteData);
	m_clData.GetQuoteDataL2(buyQuoteData, sellQuoteData, impliedDepth);

	dLastPrice = QuoteData.Data[S_FID_LASTPRICE].Price;
	if (fabs(dLastPrice) < ErrorRange) //最新价为0
		dLastPrice = QuoteData.Data[S_FID_PRESETTLEPRICE].Price;//取昨结算

	RECT rect = { -1, -1, g_Width[0], iWight_Height };
	RECT rectCol = rect;
	char szText[100] = { 0 };
	SelectObject(memdc.GetHdc(), g_FontData.GetFontNum17());
	HBRUSH hbrOdd = CreateSolidBrush(RGB(237, 237, 237));
	HBRUSH hbrFrame = CreateSolidBrush(RGB(229, 255, 255));
	HBRUSH hbrMouseMove = CreateSolidBrush(RGB(219, 229, 250));
	HBRUSH hbrRed = CreateSolidBrush(g_ColorRefData.GetColorTextRed());
	HBRUSH hbrBlue = CreateSolidBrush(g_ColorRefData.GetColorBlue());
	HBRUSH hbrGray = CreateSolidBrush(g_ColorRefData.GetColorGray());

	double price;
	std::vector<int> drawIndex;
	for (int iCol = 0; iCol < 9; iCol++)
	{
		int oldCol = iCol;
		if (g_stOrderConfig.iBuySellDirection) {
			if (iCol != 5 && iCol != 3 && iCol != 4)
				iCol = 8 - oldCol;
		}
		rectCol.right = rectCol.left + g_Width[iCol];
		rect = rectCol;
		std::multimap <double, TPosition> PositionMap;
		std::multimap <double, TOrder> ParOrderMap;
		std::multimap <double, TOrder> ConditionMap;
		m_clData.GetConditionData(ConditionMap);
		m_clData.GetPositionData(PositionMap);
		m_clData.GetParOrderData(ParOrderMap);

		bool bIslastPriceFlag = false;
		//int iTmpMouseWheelOffset = m_iMouseWheelOffset;

		if (m_stConfig->bLastPriceCenter || fabs(m_dLastPrice - 0) < ErrorRange)
		{
			m_dLastPrice = dLastPrice;
			m_iMouseWheelOffset = 0;
		}
		int iLastPriceOffset;
		for (int iRow = 0; iRow < m_iVisiabalRow; iRow++)
		{
			int iOffset = iRow - m_iVisiabalRow / 2 + m_iMouseWheelOffset;
			int iQty = 0;

 			if (iCol % 2 == 0)
 			{
 				FillRect(memdc.GetHdc(), &rect, hbrOdd);
 			}
			//鼠标悬浮填充颜色
			int iMouseCol = GetCol(LOWORD(m_MouseMoveOffset));
 			if (oldCol == iMouseCol && iRow == HIWORD(m_MouseMoveOffset) / iWight_Height&&m_bMouseTrack && iCol != ID_COL_LONGPOSITION&&iCol != ID_COL_SHORTPOSITION&&iCol != ID_COL_ORDERPRICE)
 			{
 				FillRect(memdc.GetHdc(), &rect, hbrMouseMove);
 			}
 			if (oldCol == ID_COL_LONGCONDITIONORDER || oldCol == ID_COL_SHORTCONDITIONORDER)
 			{
 				if (m_bIsForeign)
 					FillRect(memdc.GetHdc(), &rect, hbrGray);
 			}

			price = m_dLastPrice - iOffset*pstCommodity->PriceTick;
			iLastPriceOffset = (m_dLastPrice - dLastPrice) / pstCommodity->PriceTick;
			
			switch (iCol)
			{
			case ID_COL_LONGPOSITION:
			{
				//多头数量
				COLORREF colref = GetTextColor(memdc.GetHdc());
				for (auto iter = PositionMap.lower_bound(price - ErrorRange); iter != PositionMap.upper_bound(price + ErrorRange); iter++)
				{
					if (iter->second.Direct == dBuy)
					{
						sprintf_s(szText, "%d", iter->second.PositionQty);
						SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorTextGreen());
						DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						break;
					}

				}
				SetTextColor(memdc.GetHdc(), colref);
				break;
			}
			case ID_COL_LONGPARORDER:
			{
				int OrderQty = 0;

				for (auto iter = ParOrderMap.lower_bound(price - ErrorRange); iter != ParOrderMap.upper_bound(price + ErrorRange); iter++)

				{
					if (iter->second.Direct == dBuy)
						OrderQty += iter->second.OrderQty;
				}

				if (OrderQty != 0)
				{
					sprintf_s(szText, "%d", OrderQty);

					DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}


				break;
			}

			case ID_COL_LONGCONDITIONORDER:
			{
				int OrderQty = 0;
				for (auto iter = ConditionMap.lower_bound(price - ErrorRange); iter != ConditionMap.upper_bound(price + ErrorRange); iter++)

				{
					if (iter->second.Direct == dBuy)
						OrderQty += iter->second.OrderQty;
				}
				if (OrderQty != 0)
				{
					sprintf_s(szText, "%d", OrderQty);
					DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				break;
			}
			case ID_COL_LONGORDERQTY:
			{
				//填充颜色 判断价格
				if (price <= dLastPrice)
				{
					HBRUSH LongPricebrush = CreateSolidBrush(RGB(234, 221, 109));
					FillRect(memdc.GetHdc(), &rect, LongPricebrush);
					DeleteObject(LongPricebrush);
				}

				//根据鼠标颜色填充价格
				if (oldCol == GetCol(LOWORD(m_MouseMoveOffset)) && iRow == HIWORD(m_MouseMoveOffset) / iWight_Height&&m_bMouseTrack && iCol != ID_COL_LONGPOSITION&&iCol != ID_COL_SHORTPOSITION&&iCol != ID_COL_ORDERPRICE)
				{
					FillRect(memdc.GetHdc(), &rect, hbrMouseMove);
				}
				int i = 0;
				for (i = 0; i < 10; i++)
				{
					if (fabs(price - buyQuoteData.Data[i].Price ) < ErrorRange)
					{
						iQty = buyQuoteData.Data[i].Qty;
						break;
					}

				}
				if (iQty > 0)
				{
					if (i+1 == impliedDepth.BidQtyDepth)
					{
						sprintf_s(szText, "*%d", iQty);
					}
					else
					{
						sprintf_s(szText, "%d", iQty);
					}

					DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				break;
			}
			case ID_COL_SHORTORDERQTY:
			{
				COLORREF colref = GetTextColor(memdc.GetHdc());
				//填充颜色 判断价格
				if (price >= dLastPrice)
				{
					HBRUSH LongPricebrush = CreateSolidBrush(RGB(117, 190, 100));
					FillRect(memdc.GetHdc(), &rect, LongPricebrush);
					DeleteObject(LongPricebrush);
				}
				if (oldCol == GetCol(LOWORD(m_MouseMoveOffset)) && iRow == HIWORD(m_MouseMoveOffset) / iWight_Height&&m_bMouseTrack && iCol != ID_COL_LONGPOSITION&&iCol != ID_COL_SHORTPOSITION&&iCol != ID_COL_ORDERPRICE)
				{
					FillRect(memdc.GetHdc(), &rect, hbrMouseMove);
				}

				int i = 0;
				for (i = 0; i < 10; i++)
				{
					if (fabs(price - sellQuoteData.Data[i].Price) < ErrorRange)
					{
						iQty = sellQuoteData.Data[i].Qty;
						break;
					}

				}
				if (iQty > 0)
				{
					if (i + 1 == impliedDepth.AskQtyDepth)
					{
						sprintf_s(szText, "*%d", iQty);
					}
					else
					{
						sprintf_s(szText, "%d", iQty);
					}
					
					SetTextColor(memdc.GetHdc(), g_ColorRefData.g_ColorBlack);
					DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				SetTextColor(memdc.GetHdc(), colref);
				break;
			}
			case ID_COL_SHORTPOSITION:
			{
				//空头数量
				//m_clData.GetPositionData(PositionMap);
				COLORREF colref = GetTextColor(memdc.GetHdc());
				for (auto iter = PositionMap.lower_bound(price - ErrorRange); iter != PositionMap.upper_bound(price + ErrorRange); iter++)

				{
					if (iter->second.Direct == dSell)
					{
						sprintf_s(szText, "%d", iter->second.PositionQty);
						SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorTextRed());
						DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						break;
					}
				}
				SetTextColor(memdc.GetHdc(), colref);
				break;
			}

			case ID_COL_SHORTPARORDER:
			{
				//m_clData.GetParOrderData(ParOrderMap);
				int OrderQty = 0;
				for (auto iter = ParOrderMap.lower_bound(price - ErrorRange); iter != ParOrderMap.upper_bound(price + ErrorRange); iter++)

				{
					if (iter->second.Direct == dSell)
						OrderQty += iter->second.OrderQty;
				}

				if (OrderQty != 0)
				{
					sprintf_s(szText, "%d", OrderQty);

					DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				break;
			}
			case ID_COL_SHORTCONDITIONORDER:
			{
				//m_clData.GetConditionData(ConditionMap);
				int OrderQty = 0;
				for (auto iter = ConditionMap.lower_bound(price - ErrorRange); iter != ConditionMap.upper_bound(price + ErrorRange); iter++)
				{
					if (iter->second.Direct == dSell)
						OrderQty += iter->second.OrderQty;
				}
				if (OrderQty != 0)
				{
					sprintf_s(szText, "%d", OrderQty);
					DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				break;
			}
			case ID_COL_ORDERPRICE:
			{
				COLORREF colref = GetTextColor(memdc.GetHdc());
				do
				{
					if (fabs(price - dLastPrice) < ErrorRange)
					{
						FillRect(memdc.GetHdc(), &rect, hbrMouseMove);
						break;
					}


					HBRUSH LongPricebrush = CreateSolidBrush(RGB(224, 218, 194));
					FillRect(memdc.GetHdc(), &rect, LongPricebrush);
					DeleteObject(LongPricebrush);

					//昨结算
					if (g_stOrderConfig.iComparePre==0)
					{
// 						if (price >= m_dPreSettlePrice)
// 							SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorTextRed());
// 						else if (price < m_dPreSettlePrice)
// 							SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorTextGreen());

						SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorBlack());
					}
					//最新价
					else if (g_stOrderConfig.iComparePre == 1)
					{
// 						if (price >= dLastPrice)
// 							SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorTextRed());
// 						else if (price < dLastPrice)
// 							SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorTextGreen());

						//绘制不通的颜色

						SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorBlack());


					}

					break;
				} while (false);


				if (fabs(price - dLastPrice)>ErrorRange)
				{
					if (m_KBFocusOffset == iOffset - iLastPriceOffset)
					{
						InflateRect(&rect, -1, -1);
						FrameRect(memdc.GetHdc(), &rect, hbrBlue);
						InflateRect(&rect, 1, 1);
					}
					//if (m_KBFocusOffset == -(iOffset - iLastPriceOffset))
					//{
					//    InflateRect(&rect, -1, -1);
					//    FillRect(memdc.GetHdc(), &rect, hbrFocus);
					//    InflateRect(&rect, 1, 1);
					//}
				}
				DoubleToChar(pstCommodity->PricePrec, price, szText);
				DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(szText).c_str(), strlen(szText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				SetTextColor(memdc.GetHdc(), colref);
				break;
			}
			default:
				break;
			}
			FrameRect(memdc.GetHdc(), &rect, hbrFrame);

			if (iRow == 0)
			{
				rect.top += 1;
				FrameRect(memdc.GetHdc(), &rect, hbrFrame);
				rect.top -= 1;
			}
			OffsetRect(&rect, 0, iWight_Height);
		}
		OffsetRect(&rectCol, g_Width[iCol], 0);
		iCol = oldCol;
	}
	DeleteObject(hbrFrame);
	DeleteObject(hbrOdd);
	DeleteObject(hbrMouseMove);
	DeleteObject(hbrRed);
	DeleteObject(hbrBlue);
	DeleteObject(hbrGray);

}
void TPointOrderFrm::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	SetFocus(m_Hwnd);
	int iCol = GetCol(LOWORD(lParam));
	if (g_stOrderConfig.iBuySellDirection) {
		iCol = 8 - iCol;
	}
	if (m_sCode.size() == 0)
		return;
	switch (iCol)
	{
		//case 	ID_COL_LONGPOSITION:
		//case	ID_COL_SHORTPOSITION:
		//	OnClickPosition();
	case	ID_COL_LONGCONDITIONORDER:
	case	ID_COL_SHORTCONDITIONORDER:
		if (!m_bIsForeign)
			InsertConditionOrder(wParam, lParam);
		break;
		//case	ID_COL_LONGPARORDER:
		//case	ID_COL_SHORTPARORDER:
		//	OnClickDirect();
	case	ID_COL_LONGORDERQTY:
	case	ID_COL_SHORTORDERQTY:
		OnClickOrder(wParam, lParam);
		break;

		//case	ID_COL_ORDERPRICE:
		//	OnClickPrice();
	default:
		break;
	}

	InvalidateRect(m_Hwnd, NULL, true);
}

void TPointOrderFrm::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	int iCol = GetCol(LOWORD(lParam));
	if (g_stOrderConfig.iBuySellDirection) {
		iCol = 8 - iCol;
	}
	switch (iCol)
	{
		//case 	ID_COL_LONGPOSITION:
		//case	ID_COL_SHORTPOSITION:
		//	OnRClickPosition();
	case	ID_COL_LONGCONDITIONORDER:
	case	ID_COL_SHORTCONDITIONORDER:
		CancelConditionOrder(wParam, lParam);
		break;
	case	ID_COL_LONGPARORDER:
	case	ID_COL_SHORTPARORDER:
		CancelParOrder(wParam, lParam);
		break;

		//case	ID_COL_LONGORDERQTY:
		//case	ID_COL_SHORTORDERQTY:
		//	OnRClickOrder();
		//case	ID_COL_ORDERPRICE:
		//	OnRClickPrice();
	default:
		break;
	}
}

void TPointOrderFrm::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{

}
void TPointOrderFrm::OnLButtonDbClick(WPARAM wParam, LPARAM lParam)
{
	int iCol = GetCol(LOWORD(lParam));
	switch (iCol)
	{
	case	ID_COL_LONGPARORDER:
	case	ID_COL_SHORTPARORDER:
		CancelParOrder(wParam, lParam);
		break;
	}
}

void TPointOrderFrm::OnVKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (m_stConfig->stHotKey.iHotKeyDirct == 1)
	{
		if (wParam == VK_UP)
			wParam = VK_DOWN;
		else if (wParam == VK_DOWN)
			wParam = VK_UP;
	}
	if (wParam == VK_UP)
		m_KBFocusOffset -= 1;
	else if (wParam == VK_DOWN)
		m_KBFocusOffset += 1;

	if (m_KBFocusOffset > m_iMouseWheelOffset + m_iVisiabalRow / 2 && !m_stConfig->bLastPriceCenter)
		m_iMouseWheelOffset++;
	else if (m_KBFocusOffset < m_iMouseWheelOffset - m_iVisiabalRow / 2 && !m_stConfig->bLastPriceCenter)
		m_iMouseWheelOffset--;
	if ((GetFocus() == m_Hwnd || GetFocus() == GetParent()) && abs(m_iMouseWheelOffset)<50)
		g_clTips.SetText(-m_KBFocusOffset);
	InvalidateRect(m_Hwnd, NULL, false);
}
void TPointOrderFrm::OnHKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (!CConvert::GetCommodity(*pstCommodity, &m_stConKey)) {
		return;
	}
	double dLastPrice;
	SQuoteSnapShot QuoteData;
	m_clData.GetQuoteData(QuoteData);
	dLastPrice = QuoteData.Data[S_FID_LASTPRICE].Price;

	TSendOrder stOrder;
	CConvert::DefaultSendOrder(stOrder);
	if (!FillOrder(stOrder))
		return;
	if (m_stConfig->stHotKey.iHotKeyDirct == 1)
	{
		if (wParam == VK_LEFT)
			wParam = VK_RIGHT;
		else if (wParam == VK_RIGHT)
			wParam = VK_LEFT;
	}
	if (wParam == VK_LEFT)
	{
		stOrder.Direct = dBuy;
	}
	else
	{
		stOrder.Direct = dSell;
	}

	stOrder.OrderPrice = dLastPrice - (double)m_KBFocusOffset*pstCommodity->PriceTick;
	
	stOrder.OrderQty = m_stConfig->iDefaultQty;
	if (!InsertOrderAssitant::DealMesasagebox(stOrder,true))
		return;
	if (m_stConfig->stPingPang.bPingPang)
	{
		stOrder.Offset = oOpen;
		m_clPingPang.InsertPingPangOrder(stOrder);
	}
	else
	{
		if (GetKeyState(VK_MENU)<0)
		{
			stOrder.Offset = oOpen;

			InsertOrderAssitant::InsertOrder(stOrder);
		}
		else if (GetKeyState(VK_CONTROL)<0)
		{
			int iCover = 0, iTCover = 0;
			stOrder.Offset = oCover;
			CConvert::GetCloseNum(m_stOrderInfo.szUserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCover, iTCover);
			int iCoverVol = iCover + iTCover;
			if (stOrder.OrderQty <= iCoverVol&&iCoverVol != 0)
			{
				stOrder.OrderQty = iCoverVol;
				m_clData.InsertCloseOrder(stOrder, iCover, iTCover);
			}
			else if (stOrder.OrderQty > iCoverVol)
			{
				m_clData.InsertCancelCover(stOrder);
			}
			else if (stOrder.OrderQty == 0 && iCoverVol == 0)
			{
				int i = 0;
				switch (g_LanguageID)
				{
				case NONE:
					break;
				case CHT:
					g_clTips.SetText(CHTTips[i]);
					break;
				case CHS:
					g_clTips.SetText(CHSTips[i]);
					break;
				case ENU:
					g_clTips.SetText(ENUTips[i]);

					break;
				default:
					break;
				}
			}
		}
		else
		{
			m_clData.DealAutoOpenCloseOrder(stOrder, m_stOrderInfo.szUserNo);
		}
	}
	return;
}

void TPointOrderFrm::OnKeyUP(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_CONTROL:
		//GetKeyState(VK_CONTROL)<0 = false;
		if (GetFocus() == m_Hwnd || GetFocus() == GetParent())
		{
			int i = 1;
			switch (g_LanguageID)
			{
			case NONE:
				break;
			case CHT:
				g_clTips.SetText(CHTTips[i]);
				break;
			case CHS:
				g_clTips.SetText(CHSTips[i]);
				break;
			case ENU:
				g_clTips.SetText(ENUTips[i]);

				break;
			default:
				break;
			}
		}
		break;
	case VK_MENU:
		//GetKeyState(VK_MENU)<0 = false;
		if (GetFocus() == m_Hwnd || GetFocus() == GetParent())
		{
			int i = 1;
			switch (g_LanguageID)
			{
			case NONE:
				break;
			case CHT:
				g_clTips.SetText(CHTTips[i]);
				break;
			case CHS:
				g_clTips.SetText(CHSTips[i]);
				break;
			case ENU:
				g_clTips.SetText(ENUTips[i]);

				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}

}

void TPointOrderFrm::OnKeyDown(WPARAM wParam, LPARAM lParam)
{

	switch (wParam)
	{
	case VK_UP:
	case VK_DOWN:
		OnVKeyDown(wParam, lParam);
		break;

	case VK_LEFT:
	case VK_RIGHT:
		OnHKeyDown(wParam, lParam);
		break;
	case VK_CONTROL:
	{
		//GetKeyState(VK_CONTROL)<0 = true;
		if (GetFocus() == m_Hwnd || GetFocus() == GetParent())
		{
			switch (g_LanguageID)
			{
			case NONE:
				break;
			case CHT:
				g_clTips.SetText(CHTTips[3]);

				break;
			case CHS:
				g_clTips.SetText(CHSTips[3]);

				break;
			case ENU:
				g_clTips.SetText(ENUTips[3]);

				break;
			default:
				break;
			}
		}
		break;
	}

	case VK_MENU:
	{
		//GetKeyState(VK_MENU)<0 = true;
		if (GetFocus() == m_Hwnd || GetFocus() == GetParent())
		{
			switch (g_LanguageID)
			{
			case NONE:
				break;
			case CHT:
				g_clTips.SetText(CHTTips[2]);
				break;
			case CHS:
				g_clTips.SetText(CHSTips[2]);
				break;
			case ENU:
				g_clTips.SetText(ENUTips[2]);

				break;
			default:
				break;
			}
		}
		break;
	}

	default:
		break;
	}
}
void TPointOrderFrm::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseTrack = false;
	InvalidateRect(m_Hwnd, 0, false);
}
void TPointOrderFrm::OnEnableWindow(WPARAM wParam, LPARAM lParam)
{}

void TPointOrderFrm::OnSetFocus()
{}
void TPointOrderFrm::OnKillFocus()
{
	//GetKeyState(VK_MENU)<0 = false;
	//GetKeyState(VK_CONTROL)<0 = false;
}
void  TPointOrderFrm::InsertConditionOrder(WPARAM wParam, LPARAM lParam)
{
	int iRow = HIWORD(lParam) / iWight_Height;
	int iOffset = iRow - m_iVisiabalRow / 2 + m_iMouseWheelOffset;
	int iCol = GetCol(LOWORD(lParam));
	if (g_stOrderConfig.iBuySellDirection) {
		iCol = 8 - iCol;
	}

	TSendOrder stOrder;
	CConvert::DefaultSendOrder(stOrder);
	if (!FillOrder(stOrder))
		return;
	GetPrice(iOffset, stOrder.OrderPrice);
	if (iCol == ID_COL_LONGCONDITIONORDER)
	{
		stOrder.TriggerCondition = tcGreaterEqual;
		stOrder.Direct = dBuy;
	}
	else if (iCol == ID_COL_SHORTCONDITIONORDER)
	{
		stOrder.TriggerCondition = tcLessEqual;
		stOrder.Direct = dSell;
	}
	stOrder.Offset = oOpen;
	stOrder.OrderQty = m_stConfig->iDefaultQty;
	stOrder.StrategyType = stCondition;
	stOrder.TriggerMode = tmLatest;
	stOrder.TriggerPrice = stOrder.OrderPrice;
	g_pTradeApi->InsertOrder(stOrder);
	//sprintf_s(buf, "code=%s;price=%.4f;qty=%d;direct=%c;offset=%c;TriggerPrice=%.4f;TriggerCondition=%c", m_stOrderInfo.strCode.c_str(),
	//	stOrder.OrderPrice, stOrder.OrderQty, stOrder.Direct, stOrder.Offset, stOrder.TriggerPrice, stOrder.TriggerCondition);
}

void  TPointOrderFrm::OnClickOrder(WPARAM wParam, LPARAM lParam)
{
	int iRow = HIWORD(lParam) / iWight_Height;
	int iCol = GetCol(LOWORD(lParam));
	if (g_stOrderConfig.iBuySellDirection) {
		iCol = 8 - iCol;
	}

	int iOffset = iRow - m_iVisiabalRow / 2 + m_iMouseWheelOffset;

	TSendOrder stOrder;
	CConvert::DefaultSendOrder(stOrder);
	if (!FillOrder(stOrder))
		return;
	if (iCol == ID_COL_LONGORDERQTY)
		stOrder.Direct = dBuy;
	else if (iCol == ID_COL_SHORTORDERQTY)
		stOrder.Direct = dSell;
	GetPrice(iOffset, stOrder.OrderPrice);

	int iQty = 0;
	if (!g_pTradeData->GetTradableQty(stOrder, iQty))
		stOrder.OrderQty = min(m_stConfig->iDefaultQty, iQty);
	else
		stOrder.OrderQty = m_stConfig->iDefaultQty;

	if (!InsertOrderAssitant::DealMesasagebox(stOrder, true))
		return;
	if (!m_stConfig->stPingPang.bPingPang)
	{
		if (GetKeyState(VK_MENU)<0)
		{
			stOrder.Offset = oOpen;
			InsertOrderAssitant::InsertOrder(stOrder);
		}		
		else if (GetKeyState(VK_CONTROL)<0)
		{
			int iCover = 0, iTCover = 0;
			stOrder.Offset = oCover;
			CConvert::GetCloseNum(m_stOrderInfo.szUserNo, &stOrder, stOrder.Direct, stOrder.OrderQty, iCover, iTCover);
			int iCoverVol = iCover + iTCover;
			if (stOrder.OrderQty <= iCoverVol&&iCoverVol != 0)
			{
				stOrder.OrderQty = iCoverVol;
				m_clData.InsertCloseOrder(stOrder, iCover, iTCover);
			}
			else if (stOrder.OrderQty > iCoverVol)
			{

				m_clData.InsertCancelCover(stOrder);
			}
			else if (stOrder.OrderQty == 0 && iCoverVol == 0)
			{
				switch (g_LanguageID)
				{
				case NONE:
					break;
				case CHT:
					g_clTips.SetText(CHTTips[0]);

					break;
				case CHS:
					g_clTips.SetText(CHSTips[0]);

					break;
				case ENU:
					g_clTips.SetText(ENUTips[0]);

					break;
				default:
					break;
				}
			}
		}
		else
		{
			m_clData.DealAutoOpenCloseOrder(stOrder, m_stOrderInfo.szUserNo);
		}
	}
	else
	{
		stOrder.Offset = oOpen;
		m_clPingPang.InsertPingPangOrder(stOrder);
	}
}

void TPointOrderFrm::CancelConditionOrder(WPARAM wParam, LPARAM lParam)
{
	std::multimap <double, TOrder> ConditionMap;
	int iRow = HIWORD(lParam) / iWight_Height;
	double price;
	int iOffset = iRow - m_iVisiabalRow / 2 + m_iMouseWheelOffset;

	m_clData.GetConditionData(ConditionMap);
	GetPrice(iOffset, price);

	int iCol = GetCol(LOWORD(lParam));
	if (g_stOrderConfig.iBuySellDirection) {
		iCol = 8 - iCol;
	}

	for (auto iter = ConditionMap.lower_bound(price - ErrorRange); iter != ConditionMap.upper_bound(price + ErrorRange); iter++)
	{
		if (iCol == ID_COL_LONGCONDITIONORDER&&iter->second.Direct == dBuy)
			g_pTradeApi->CancelOrder(iter->second.OrderID);
		else if (iCol == ID_COL_SHORTCONDITIONORDER&&iter->second.Direct == dSell)
			g_pTradeApi->CancelOrder(iter->second.OrderID);
	}
}

void TPointOrderFrm::CancelParOrder(WPARAM wParam, LPARAM lParam)
{
	std::multimap <double, TOrder> ParOrderMap;
	int iRow = HIWORD(lParam) / iWight_Height;
	double price;
	int iOffset = iRow - m_iVisiabalRow / 2 + m_iMouseWheelOffset;

	GetPrice(iOffset, price);
	m_clData.GetParOrderData(ParOrderMap);

	int iCol = GetCol(LOWORD(lParam));
	if (g_stOrderConfig.iBuySellDirection) {
		iCol = 8 - iCol;
	}
	for (auto iter = ParOrderMap.lower_bound(price - ErrorRange); iter != ParOrderMap.upper_bound(price + ErrorRange); iter++)
	{
		if (iCol == ID_COL_LONGPARORDER&&iter->second.Direct == dBuy)
			g_pTradeApi->CancelOrder(iter->second.OrderID);
		else if (iCol == ID_COL_SHORTPARORDER&&iter->second.Direct == dSell)
			g_pTradeApi->CancelOrder(iter->second.OrderID);
	}
}
void TPointOrderFrm::OnContractDataInit()
{
	SetQuoteHeaderInfo();
	SetPositionHeaderInfo();
	SetOrderHeaderIndo();
	PostMessage(GetParent(), WM_ONHEADERINFOCHANGED, 0, (LPARAM)&m_stGridHeaderInfo);
	InvalidateRect(m_Hwnd, NULL, true);
}

void TPointOrderFrm::OnQuote()
{
	SetQuoteHeaderInfo();
	PostMessage(GetParent(), WM_ONHEADERINFOCHANGED, 0, (LPARAM)&m_stGridHeaderInfo);
	InvalidateRect(m_Hwnd, NULL, true);
}
void TPointOrderFrm::OnPosition()
{
	SetPositionHeaderInfo();

	PostMessage(GetParent(), WM_ONHEADERINFOCHANGED, 0, (LPARAM)&m_stGridHeaderInfo);
	InvalidateRect(m_Hwnd, NULL, true);
}

void TPointOrderFrm::OnOrder()
{
	SetOrderHeaderIndo();
	PostMessage(GetParent(), WM_ONHEADERINFOCHANGED, 0, (LPARAM)&m_stGridHeaderInfo);
	InvalidateRect(m_Hwnd, NULL, true);
}

bool TPointOrderFrm::FillOrder(TSendOrder& stSendOrder)
{
	strcpy_s(stSendOrder.UserNo, m_stOrderInfo.szUserNo);
	strcpy_s(stSendOrder.Sign, m_stOrderInfo.szSign1);
	if (!CConvert::TContractNoToTContractKey(m_sCode.c_str(), stSendOrder))
		return false;
	if (strlen(stSendOrder.UserNo) == 0 || strlen(stSendOrder.Sign) == 0)
		return false;
	else
		return true;
}

void TPointOrderFrm::SetQuoteHeaderInfo()
{
	m_stGridHeaderInfo.iBuyQuoteQty = 0;
	m_stGridHeaderInfo.iSellQuoteQty = 0;

	SQuoteSnapShotL2 buyQuoteData = {};
	SQuoteSnapShotL2 sellQuoteData = {};
	SImpliedDepthL2 impliedDepth = {};
	m_clData.GetQuoteDataL2(buyQuoteData, sellQuoteData, impliedDepth);

	for (int iOffset = 0; iOffset < 10; iOffset++)
	{
		m_stGridHeaderInfo.iBuyQuoteQty += buyQuoteData.Data[iOffset].Qty ;
		m_stGridHeaderInfo.iSellQuoteQty += sellQuoteData.Data[iOffset].Qty;
	}
}
void TPointOrderFrm::SetPositionHeaderInfo()
{
	m_stGridHeaderInfo.iBuyPositionQty = 0;
	m_stGridHeaderInfo.iSellPositionQty = 0;
	std::multimap <double, TPosition> PositionMap;
	m_clData.GetPositionData(PositionMap);
	for (auto iter : PositionMap)
	{
		if (iter.second.Direct == dBuy)
			m_stGridHeaderInfo.iBuyPositionQty += iter.second.PositionQty;
		else if (iter.second.Direct == dSell)
			m_stGridHeaderInfo.iSellPositionQty += iter.second.PositionQty;
	}
}
void TPointOrderFrm::SetOrderHeaderIndo()
{
	m_stGridHeaderInfo.iBuyParOrderQty = 0;
	m_stGridHeaderInfo.iSellParOrderQty = 0;
	std::multimap <double, TOrder> ParOrderMap;
	m_clData.GetParOrderData(ParOrderMap);
	for (auto iter : ParOrderMap)
	{
		if (iter.second.Direct == dBuy)
			m_stGridHeaderInfo.iBuyParOrderQty += iter.second.OrderQty;
		else if (iter.second.Direct == dSell)
			m_stGridHeaderInfo.iSellParOrderQty += iter.second.OrderQty;
	}
	m_stGridHeaderInfo.iBuyConditionQty = 0;
	m_stGridHeaderInfo.iSellConditionQty = 0;
	std::multimap <double, TOrder> ConditionMap;
	m_clData.GetConditionData(ConditionMap);
	for (auto iter : ConditionMap)
	{
		if (iter.second.Direct == dBuy)
			m_stGridHeaderInfo.iBuyConditionQty += iter.second.OrderQty;
		else if (iter.second.Direct == dSell)
			m_stGridHeaderInfo.iSellConditionQty += iter.second.OrderQty;
	}
}

void TPointOrderFrm::GetPrice(int iOffset, double &price)
{
	if (!CConvert::GetCommodity(*pstCommodity, &m_stConKey)) {
		return;
	}
	SQuoteSnapShot QuoteData;
	m_clData.GetQuoteData(QuoteData);

	price = m_dLastPrice - iOffset*pstCommodity->PriceTick;
}