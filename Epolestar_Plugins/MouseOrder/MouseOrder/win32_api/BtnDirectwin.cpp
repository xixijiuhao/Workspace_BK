#include "PreInclude.h"	//自定义按钮类

//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

//规格
#define NBTN_UNIT_WIDTH		100
#define NBTN_UNIT_HEIGHT	33


CBtnWin::CBtnWin() :
m_opr(NULL),
m_nWidth(0),
m_nHeight(0),
m_tOffset(oNone),
m_tDirect(dNone),
m_tSellOffset(oNone),
m_nState(BTN_S_NORMAL),
m_bMouseTrack(TRUE),
m_nLong(0),
m_nShort(0),
m_bShowPos(false),
m_bFocus(false),
m_bInFlag(false),
m_bOffer(false),
m_cPreFreeBaseShow(TEXT("")),
m_cCanQtyShow(TEXT("")),
m_cPreFreeShow(TEXT(""))
{

}
CBtnWin::~CBtnWin()
{

}

void CBtnWin::CreatePopWin(IOperator *opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, int nwidth, int nheight, char cDirect, char cOffset, char cSellOffset, COLORREF rbuy, COLORREF rsell)
{
	m_nLeft = nleft;
	m_nTop = ntop;
	m_nWidth = nwidth;
	m_nHeight = nheight;
	m_tOffset = cOffset;
	m_tDirect = cDirect;
	m_tSellOffset = cSellOffset;
	m_opr = opr;
	m_BuyColor = rbuy;
	m_SellColor = rsell;
	CreateFrm(classname, parent, style  /*|WS_OVERLAPPED | WS_BORDER*/, exstyle);
}

LRESULT CBtnWin::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
	{
		hdc = GetDC(m_Hwnd);
		if (m_tOffset==oNone)
		{
			SetRect(&m_reCanQty, 1, 25, m_nWidth - 1, 40);
			SetRect(&m_rePreFree, 1, 42, m_nWidth - 1, 57);
			SetRect(&m_rePreFreeBase, 1, 59, m_nWidth - 1, 74);
		}
			
		m_HdcMem = CreateCompatibleDC(hdc);// bitmap
		m_hdctemp = CreateCompatibleDC(hdc);
		
		ReleaseDC(m_Hwnd, hdc);

		SetWindowPos(m_Hwnd, NULL, m_nLeft, m_nTop, m_nWidth, m_nHeight, SW_HIDE);
	}
	break;
	case WM_SIZE:
	{
		HBRUSH hRED = CreateSolidBrush(m_BuyColor);
		HBRUSH hBLUE = CreateSolidBrush(m_SellColor);
		

		hdc = GetDC(m_Hwnd);

		GetClientRect(m_Hwnd, &m_reClient);
		m_nWidth = m_reClient.right - m_reClient.left;
		m_nHeight = m_reClient.bottom - m_reClient.top;//获取窗口大小客户区

		if (m_hBitmap)
			DeleteObject(m_hBitmap);
		if (m_hbmptemp)
			DeleteObject(m_hbmptemp);

		m_hBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
		SelectObject(m_HdcMem, m_hBitmap);
		if (m_tDirect == dBuy)
			FillRect(m_HdcMem, &m_reClient, hRED);
		else
			FillRect(m_HdcMem, &m_reClient, hBLUE);
	
		//边框 
		HBRUSH hRECT;
		
		RECT rect(m_reClient);
		COLORREF cRef;
		if (m_tDirect == dBuy)
		{
			int nInc = 40;
			int r = GetRValue(m_BuyColor) + nInc > 255 ? 255 : GetRValue(m_BuyColor) + nInc;
			int g = GetGValue(m_BuyColor) + nInc > 255 ? 255 : GetGValue(m_BuyColor) + nInc;
			int b = GetBValue(m_BuyColor) + nInc > 255 ? 255 : GetBValue(m_BuyColor) + nInc;
			cRef = RGB(r, g, b);//g_ColorRefData.GetColorLightRed();
		}	
		else
		{
			int nInc = 40;
			int r = GetRValue(m_SellColor) + nInc > 255 ? 255 : GetRValue(m_SellColor) + nInc;
			int g = GetGValue(m_SellColor) + nInc > 255 ? 255 : GetGValue(m_SellColor) + nInc;
			int b = GetBValue(m_SellColor) + nInc > 255 ? 255 : GetBValue(m_SellColor) + nInc;
			cRef = RGB(r, g, b);//g_ColorRefData.GetColorLightRed();
		}	
		hRECT = CreateSolidBrush(cRef);
		::FrameRect(m_HdcMem, &rect, hRECT);

		DeleteObject(hRECT);

		m_hbmptemp = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
		SelectObject(m_hdctemp, m_hbmptemp);

		m_rePos.left = m_nWidth * 2 / 3;
		m_rePos.top = 4;
		m_rePos.right = m_nWidth-4;
		m_rePos.bottom = m_nHeight / 4;
		m_rePrice = m_rePos;
		m_rePrice.left = 4;
		m_rePrice.right = m_nWidth * 2 / 5;
		ReleaseDC(m_Hwnd, hdc);

		DeleteObject(hRED);
		DeleteObject(hBLUE);
		
	}
		break;
	case WM_PAINT:
	{
		HBRUSH m_hFocusBrush;
		HBRUSH m_hShadowBrush;
		
		m_hFocusBrush = CreateSolidBrush(g_ColorRefData.GetColorWhite());// brush	//边框画刷
		m_hShadowBrush = CreateSolidBrush(g_ColorRefData.GetColorGray());		//背景画刷
			
		hdc = BeginPaint(m_Hwnd, &ps);

		RECT itemRect = m_reClient;
		BitBlt(m_hdctemp, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);
		switch (m_nState)//线条
		{
		case BTN_S_NORMAL:
			break;
		case BTN_S_FOCUS:
		case BTN_S_UP:
			InflateRect(&itemRect, -1, -1);
			::FrameRect(m_hdctemp, &itemRect, m_hFocusBrush);
			break;
		case BTN_S_DOWN:
			::FrameRect(m_hdctemp, &itemRect, m_hShadowBrush);
			break;
		case BTN_S_OVER:
			break;
		case BTN_S_LEAVE:
			break;
		}
		DrawItemText(m_hdctemp, m_reClient);//文本
		BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_hdctemp, 0, 0, SRCCOPY);

		EndPaint(m_Hwnd, &ps);

		DeleteObject(m_hFocusBrush);
		DeleteObject(m_hShadowBrush);
		break;
	}
	case WM_MOUSEMOVE:
		if (m_bMouseTrack)    //若允许追踪，则。
		{
			TRACKMOUSEEVENT csTME;
			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE | TME_HOVER;
			csTME.hwndTrack = m_Hwnd;//指定要追踪的窗口
			csTME.dwHoverTime = 10;  //鼠标在按钮上停留超过10ms，才认为状态为HOVER
			::TrackMouseEvent(&csTME); //开启Windows的WM_MOUSELEAVE，WM_MOUSEHOVER事件支持
			m_bMouseTrack = FALSE;   //若已经追踪，则停止追踪
		}
		break;
	case WM_MOUSEHOVER:
		m_nState = BTN_S_FOCUS;
		InvalidateRect(m_Hwnd, &m_reClient, FALSE);
		break;
	case WM_MOUSELEAVE:
		m_bMouseTrack = TRUE;
		{
			HWND h = GetFocus();
			if (h!=m_Hwnd)
			{ 
				m_nState = BTN_S_NORMAL;
				InvalidateRect(m_Hwnd, &m_reClient, FALSE);
			}		
		}
		m_bInFlag = false;
		break;
	case WM_LBUTTONDOWN:
		SetFocus(m_Hwnd);
		m_nState = BTN_S_DOWN;
		InvalidateRect(m_Hwnd, &m_reClient, FALSE);
		if (!m_bInFlag)
			m_bInFlag = true;
		break;
	case WM_LBUTTONDBLCLK://双击转单击
		SendMessage(m_Hwnd, WM_LBUTTONDOWN, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		{
			MsgOrderInfo orderInfo;	//
			m_nState = BTN_S_UP;
			InvalidateRect(m_Hwnd, &m_reClient, FALSE);
			orderInfo.Direct = m_tDirect;
			orderInfo.Offset = m_tOffset;
			//orderInfo.SellOffset = m_tSellOffset;
			if (m_opr&&m_bInFlag)
			{
				m_opr->SendOrder(&orderInfo);
				m_bMouseTrack = TRUE;
			}
		}
		break;
	case WM_SETFOCUS:
		m_nState = BTN_S_FOCUS;
		InvalidateRect(m_Hwnd, &m_reClient, FALSE);
		SetCheck(true);
		break;
	case WM_KILLFOCUS:
		m_nState = BTN_S_NORMAL;
		InvalidateRect(m_Hwnd, &m_reClient, FALSE);
		SetCheck(false);
		break;
	case WM_DESTROY:
		DeleteDC(m_HdcMem);
		DeleteDC(m_hdctemp);
		DeleteObject(m_hBitmap);
		DeleteObject(m_hbmptemp);
	
		m_nLong = 0;
		m_nShort = 0;
		break;
	case WM_ORDER:
		{
			MsgOrderInfo orderInfo;	//
			orderInfo.Direct = m_tDirect;
			orderInfo.Offset = m_tOffset;
			//orderInfo.SellOffset = m_tSellOffset;
			if (m_opr)
				m_opr->SendOrder(&orderInfo);
		}	
		break;
	case WM_ERASEBKGND:
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}
void CBtnWin::DrawItemText(HDC &hdc, RECT &rect)
{
	
	RECT rText = rect;
	rText.top = 2;
	wstring strText;
	SetBkMode(hdc, TRANSPARENT);
	if (m_tOffset == oOpen)
	{
		if (m_bOffer)
			strText = LoadRC::w_load_str(IDS_STRING1133);
		else
			strText = LoadRC::w_load_str(IDS_OPEN);
	}
	else if (m_tOffset == oCover)
	{
		if (m_bOffer)
			strText = LoadRC::w_load_str(IDS_STRING1134);
		else
			strText = LoadRC::w_load_str(IDS_CLOSE);
	}
	else if (m_tOffset == oCoverT)
	{
		strText = LoadRC::w_load_str(IDS_CLOSE_DAY);
	}
	else
	{
		rText.left = m_nWidth / 3;
		rText.top = 0;
		rText.right = m_nWidth * 2 / 3;
		rText.bottom = m_nHeight / 3;
		if (m_tDirect==dBuy)
			strText = LoadRC::w_load_str(IDS_BUY);
		else
			strText = LoadRC::w_load_str(IDS_SELL);
	}
	
	if (m_tOffset==oNone)
	{
		SelectObject(hdc, g_FontData.GetFontNum13());
		//持仓
		if (m_bShowPos)
		{
			if (m_tDirect == dBuy)
				wsprintf(m_cPosShow, LoadRC::w_load_str(IDS_STRING1116).c_str(), m_nLong);
			else
				wsprintf(m_cPosShow, LoadRC::w_load_str(IDS_STRING1117).c_str(), m_nShort);
			
			DrawText(hdc, m_cPosShow, wcslen(m_cPosShow), &m_rePos, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
		}
		if (m_dPrice.size() > 10)
		{
			SelectObject(hdc, g_FontData.GetFontWord9());
			//价格
			DrawText(hdc, m_dPrice.c_str(), m_dPrice.size(), &m_rePrice, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
		}
		else
			DrawText(hdc, m_dPrice.c_str(), m_dPrice.size(), &m_rePrice, DT_VCENTER | DT_LEFT | DT_SINGLELINE);	
		
		SelectObject(hdc, g_FontData.GetFontWord13());
		SetTextColor(hdc, g_ColorRefData.GetColorBtnFace());
		//可开 预冻结 基币
		DrawText(hdc, m_cCanQtyShow.c_str(), m_cCanQtyShow.size(), &m_reCanQty, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		DrawText(hdc, m_cPreFreeShow.c_str(), m_cPreFreeShow.size(), &m_rePreFree, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		DrawText(hdc, m_cPreFreeBaseShow.c_str(), m_cPreFreeBaseShow.size(), &m_rePreFreeBase, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		

		SelectObject(hdc, g_FontData.GetFontWord22());
		SetTextColor(hdc, g_ColorRefData.GetColorBlack());
		DrawText(hdc, strText.c_str(), strText.size(), &rText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	}
	else
	{	
		SelectObject(hdc, g_FontData.GetFontNum15());
		SetTextColor(hdc, g_ColorRefData.GetColorWhite());
		DrawText(hdc, strText.c_str(), strText.size(), &rText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);	
	}	
}
void CBtnWin::UpdateWindow(RECT &rect)
{
	if (IsWindow(m_Hwnd))
		InvalidateRect(m_Hwnd, &m_reClient, false);
}
void CBtnWin::SetLongData(int nlong)
{ 
	m_nLong = nlong; 
	UpdateWindow(m_rePos);
}		
void CBtnWin::SetShortData(int nshort)
{ 
	m_nShort = nshort; 
	UpdateWindow(m_rePos);
}

void CBtnWin::SetPriceData(wchar_t *dprice)
{ 
	m_dPrice = dprice;
	UpdateWindow(m_rePrice);
}		
void CBtnWin::SetOffset(TCoverMode cm)
{
	m_tOffset = cm;
	UpdateWindow(m_reClient);
}
void CBtnWin::SetCheck(bool bGetF)
{
	m_bFocus = bGetF;
}
bool CBtnWin::GetCheck()
{
	return m_bFocus;
}
void  CBtnWin::SetTextQty(wchar_t *cQty)
{
	m_cCanQtyShow = cQty;
	UpdateWindow(m_reClient);
}
void  CBtnWin::SetTextPre(wchar_t *cPre, wchar_t *cPreBase)
{
	m_cPreFreeShow = cPre;
	m_cPreFreeBaseShow = cPreBase;
	UpdateWindow(m_reClient);
}
void CBtnWin::SetPosShowFlag(bool bShow)
{
	m_bShowPos = bShow;
	UpdateWindow(m_reClient);
}	

CBtnDirectWin::CBtnDirectWin() :
m_opr(NULL),
m_nPos(0), 
m_nWidth(0), 
m_nHeight(0), 
m_tDirect(oNone),
m_nLong(0),
m_nShort(0),
m_bShowPos(false)
{

}
CBtnDirectWin::~CBtnDirectWin()
{

}

void CBtnDirectWin::CreatePopWin(IOperator* opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, int nwidth, int nheight, char cDirect, COLORREF rbuy, COLORREF rsell)
{
	m_nLeft = nleft;
	m_nTop = ntop;
	m_nWidth = nwidth;
	m_nHeight = nheight;
	m_tDirect = cDirect;
	m_opr = opr;
	m_BuyColor = rbuy;
	m_SellColor = rsell;
	CreateFrm(classname, parent, style | WS_OVERLAPPED /*| WS_BORDER*/, exstyle);
}

LRESULT CBtnDirectWin::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;
	
	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);
		
			//////////////////////////////////////////////////////////////////////////
			m_HdcMem = CreateCompatibleDC(hdc);
			
			//////////////////////////////////////////////////////////////////////////
			ReleaseDC(m_Hwnd, hdc);
			if (!m_opr)
				break;
			if (m_tDirect==dBuy)
			{
				m_Btnwinright.CreatePopWin(m_opr, TEXT("BClose"), m_Hwnd, WS_CHILD | WS_TABSTOP, 0, 107, 44, NBTN_UNIT_WIDTH, NBTN_UNIT_HEIGHT, dBuy, oCover, oCover, m_BuyColor, m_SellColor);
				ShowWindow(m_Btnwinright.GetHwnd(), SW_NORMAL);
				m_Btnwinleft.CreatePopWin(m_opr, TEXT("Buy"), m_Hwnd, WS_CHILD | WS_TABSTOP, 0, 3, 44, NBTN_UNIT_WIDTH, NBTN_UNIT_HEIGHT, dBuy, oOpen, oOpen, m_BuyColor, m_SellColor);
				ShowWindow(m_Btnwinleft.GetHwnd(), SW_NORMAL);
				m_sDirect = LoadRC::w_load_str(IDS_BUY);
				m_sDirectOffer = LoadRC::w_load_str(IDS_STRING1135);
			}
			else
			{
				m_Btnwinright.CreatePopWin(m_opr, TEXT("SClose"), m_Hwnd, WS_CHILD | WS_TABSTOP, 0, 107, 44, NBTN_UNIT_WIDTH, NBTN_UNIT_HEIGHT, dSell, oCover, oCover, m_BuyColor, m_SellColor);
				ShowWindow(m_Btnwinright.GetHwnd(), SW_NORMAL);
				m_Btnwinleft.CreatePopWin(m_opr, TEXT("Sell"), m_Hwnd, WS_CHILD | WS_TABSTOP, 0, 3, 44, NBTN_UNIT_WIDTH, NBTN_UNIT_HEIGHT, dSell, oOpen, oOpen, m_BuyColor, m_SellColor);
				ShowWindow(m_Btnwinleft.GetHwnd(), SW_NORMAL);
				m_sDirect = LoadRC::w_load_str(IDS_SELL);
				m_sDirectOffer = LoadRC::w_load_str(IDS_STRING1136);
			}
			m_reLeftBtn.left = 3;
			m_reLeftBtn.right = 3 + NBTN_UNIT_WIDTH;
			m_reLeftBtn.top = 44;
			m_reLeftBtn.bottom = 44 + NBTN_UNIT_HEIGHT;
			m_reRightBtn.left = 107;
			m_reRightBtn.right = 107 + NBTN_UNIT_WIDTH;
			m_reRightBtn.top = 44;
			m_reRightBtn.bottom = 44 + NBTN_UNIT_HEIGHT;
			SetWindowPos(m_Hwnd,NULL,m_nLeft,m_nTop,m_nWidth,m_nHeight,SW_HIDE);
		}
		break;
	case WM_SIZE:
		{
			hdc = GetDC(m_Hwnd);
			GetClientRect(m_Hwnd, &m_reClient);
			m_nWidth = m_reClient.right - m_reClient.left;
			m_nHeight = m_reClient.bottom - m_reClient.top;
			m_reText.left = m_nWidth / 3;
			m_reText.top = 0;
			m_reText.right = m_nWidth * 2 / 3;
			m_reText.bottom = m_nHeight / 3;
			m_rePos.left = m_nWidth * 2 / 3;
			m_rePos.top = 4;
			m_rePos.right = m_nWidth-4;
			m_rePos.bottom = m_nHeight / 4;
			m_rePrice = m_rePos;
			m_rePrice.left = 4;
			m_rePrice.right = m_nWidth *2/5;
			m_reCanOpenCover.left = 0;
			m_reCanOpenCover.right = m_nWidth;
			m_reCanOpenCover.top = m_nHeight / 3;
			m_reCanOpenCover.bottom = m_nHeight / 3 + 15;
			if (m_hBitmap)
				DeleteObject(m_hBitmap);

			m_hBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_reCanOpenCover.bottom);
			SelectObject(m_HdcMem, m_hBitmap);

			ReleaseDC(m_Hwnd, hdc);
		}
		break;
	case WM_PAINT:
	{
		HRGN hRgnLeft = CreateRectRgnIndirect(&m_reLeftBtn);
		HRGN hRgnRight = CreateRectRgnIndirect(&m_reRightBtn);
		HRGN hRgnClient = CreateRectRgnIndirect(&m_reClient);
		CombineRgn(hRgnClient, hRgnClient, hRgnLeft, RGN_XOR);
		CombineRgn(hRgnClient, hRgnClient, hRgnRight, RGN_XOR);

		RECT rect;
		GetUpdateRect(m_Hwnd, &rect, FALSE);
		
		hdc = BeginPaint(m_Hwnd, &ps);
		if (m_tDirect == dBuy)
		{
			HBRUSH hRED = CreateSolidBrush(m_BuyColor);//红色画刷
			if (rect.bottom != m_reText.bottom)
				FillRgn(hdc, hRgnClient, hRED);

			FillRect(m_HdcMem, &m_reClient, hRED);
			DeleteObject(hRED);
			
		}
		else
		{
			HBRUSH hBLUE = CreateSolidBrush(m_SellColor);//蓝色画刷
			if (rect.bottom != m_reText.bottom)
				FillRgn(hdc, hRgnClient, hBLUE);
			
			FillRect(m_HdcMem, &m_reClient, hBLUE);
			DeleteObject(hBLUE);
		}
			
		SetBkMode(m_HdcMem, TRANSPARENT);

		SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());
		DrawText(m_HdcMem, m_sCanOpenCover, wcslen(m_sCanOpenCover), &m_reCanOpenCover, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

		SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlack());
		SelectObject(m_HdcMem, g_FontData.GetFontWord22());	
		DrawText(m_HdcMem, m_sDirect.c_str(), m_sDirect.size(), &m_reText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

		if (m_bShowPos)
		{
			if (m_tDirect == dBuy)
				wsprintf(m_cPosShow, LoadRC::w_load_str(IDS_STRING1116).c_str(), m_nLong);
			else
				wsprintf(m_cPosShow, LoadRC::w_load_str(IDS_STRING1117).c_str(), m_nShort);
			SelectObject(m_HdcMem, g_FontData.GetFontNum13());
			DrawText(m_HdcMem, m_cPosShow, wcslen(m_cPosShow), &m_rePos, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
		}
		//价格
		if (m_dPrice.size() > 10)
		{
			SelectObject(m_HdcMem, g_FontData.GetFontNum9());
			DrawText(m_HdcMem, m_dPrice.c_str(), m_dPrice.size(), &m_rePrice, DT_VCENTER | DT_LEFT | DT_SINGLELINE);	
		}
		else
		{
			SelectObject(m_HdcMem, g_FontData.GetFontNum13());
			DrawText(m_HdcMem, m_dPrice.c_str(), m_dPrice.size(), &m_rePrice, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
		}
			
		
		BitBlt(hdc, 0, 0, m_nWidth, m_reCanOpenCover.bottom, m_HdcMem, 0, 0, SRCCOPY);
		EndPaint(m_Hwnd, &ps);

		DeleteObject(hRgnLeft);
		DeleteObject(hRgnRight);
		DeleteObject(hRgnClient);
	}	
		break;
	case WM_DESTROY:
		DeleteDC(m_HdcMem);
		DeleteObject(m_hBitmap);
		m_nLong = 0;
		m_nShort = 0;
		break;
	case WM_ERASEBKGND:
		break;
	case WM_SETFOCUS://父窗口获取焦点时 焦点落在子窗口上
		{
			if (!m_Btnwinleft.GetCheck())
				SetFocus(m_Btnwinleft.GetHwnd());
			else
				SetFocus(m_Btnwinright.GetHwnd());
		}
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}
void CBtnDirectWin::UpdateWindow(RECT &rect)
{
	if (IsWindow(m_Hwnd))
		InvalidateRect(m_Hwnd, &rect, true);
}
void CBtnDirectWin::SetLongData(int nlong)
{
	m_nLong = nlong;
	UpdateWindow(m_rePos);
}
void CBtnDirectWin::SetShortData(int nshort)
{
	m_nShort = nshort;
	UpdateWindow(m_rePos);
}

void CBtnDirectWin::SetPriceData(wchar_t* dprice)
{
	m_dPrice = dprice;
	UpdateWindow(m_rePrice);
}
void CBtnDirectWin::SetOffset(TCoverMode cm)
{
	m_Btnwinright.SetOffset(cm);
}
void CBtnDirectWin::SetCanOpenCover(wchar_t* wCon)
{
	wcscpy_s(m_sCanOpenCover, sizeof(m_sCanOpenCover) / sizeof(wchar_t), wCon);
	UpdateWindow(m_reCanOpenCover);
}
HWND CBtnDirectWin::GetHwndOpen()
{
	return m_Btnwinleft.GetHwnd();
}
HWND CBtnDirectWin::GetHwndClose()
{
	return m_Btnwinright.GetHwnd();
}
void CBtnDirectWin::SetPosShowFlag(bool bShow)
{ 
	m_bShowPos = bShow; 
	UpdateWindow(m_reClient);
}	//设置显示持仓