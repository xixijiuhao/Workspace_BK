#include "PreInclude.h"

extern G_COLOREF	g_ColorRefData;
extern G_UIFONT		g_FontData;

#define N_WIDTH_BTN		N_NORMAL_WIDYH

#define N_HEIGHT_BTN	136
#define N_HEIGHT_ITEM	70

#define N_WIDTH_ITEM	95
#define N_WIDTH_ITEMF	150

#define N_INTERVAL		5

COrderButton::COrderButton() :
m_nInnerClickIndex(-1),
m_nForeignClickIndex(-1),
m_nFocusId(-1),
m_bShiftPosition(false)
{

}
COrderButton::~COrderButton()
{

}
void COrderButton::CreateWin(IOperator* opr, LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, COLORREF rbuy, COLORREF rsell)//创建弹出窗口
{
	m_opr = opr;
	m_ColorBuy = rbuy;
	m_ColorSell = rsell;
	CreateFrm(classname, parent, style, exstyle);
	MoveWindow(m_Hwnd, nleft, ntop, N_WIDTH_BTN, N_HEIGHT_BTN, TRUE);
}
LRESULT COrderButton::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:				OnCreate(wParam, lParam);
		break;
	case WM_COMMAND:			OnCommond(wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:		return OnCtlColorStatic(wParam, lParam);

	case WM_PAINT:				OnPaint(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:			OnLButtonUp(wParam, lParam);
		break;
	case WM_MOUSEMOVE:			OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:			OnMouseLeave(wParam, lParam);
		break;
	case WM_KEYDOWN:			OnKeyDown(wParam, lParam);
		break;
	case WM_KILLFOCUS:			OnKillFocus(wParam, lParam);
		break;
	case WM_ERASEBKGND:
		break;
	case WM_DESTROY:			OnDestory(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}
void COrderButton::OnCreate(WPARAM wParam, LPARAM lParam)
{
	m_BuyBrush = CreateSolidBrush(m_ColorBuy);
	m_SellBrush = CreateSolidBrush(m_ColorSell);
	m_BKBrush = g_ColorRefData.GetHbrBackground();
	HDC hdc = GetDC(m_Hwnd);
	m_HdcMem = CreateCompatibleDC(hdc);// bitmap
	m_hBitmap = CreateCompatibleBitmap(hdc, N_WIDTH_BTN, N_HEIGHT_BTN);
	SelectObject(m_HdcMem, m_hBitmap);
	ReleaseDC(m_Hwnd, hdc);

	UpdateLayout();
}
void COrderButton::OnCommond(WPARAM wParam, LPARAM lParam)
{

}
void COrderButton::OnDestory(WPARAM wParam, LPARAM lParam)
{
	DeleteObject(m_BuyBrush);
	DeleteObject(m_SellBrush);
	DeleteObject(m_HdcMem);
	DeleteObject(m_hBitmap);
}
void COrderButton::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(m_Hwnd, &ps);
	RECT rect;

	::GetClientRect(m_Hwnd, &rect);
	FillRect(m_HdcMem, &rect, g_ColorRefData.GetHbrBackground());

	if (IsInnerType())
	{
		if (m_PosInfo.nButton == obc_3)
			paint_obc_3();
		else
			paint_obc_4();
	}
	else
		paint_obc_2();

	BitBlt(hDC, 0, 0, N_WIDTH_BTN, N_HEIGHT_BTN, m_HdcMem, 0, 0, SRCCOPY);

	EndPaint(m_Hwnd, &ps);
}
void COrderButton::paint_obc_2()
{
	for (int i = OBD_BuyF; i <= OBD_SellF; i++)
	{
		HBRUSH hBkbrush = m_BuyBrush;
		switch (i)
		{
		case OBD_BuyF:
			hBkbrush = m_BuyBrush;
			break;
		case OBD_SellF:
			hBkbrush = m_SellBrush;
			break;
		default:
			break;
		}
		FillRect(m_HdcMem, &m_ForeignItem[i].rItem, hBkbrush);
		SetBkMode(m_HdcMem, TRANSPARENT);
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());

		//焦点 边框重绘
		if (m_ForeignItem[i].bInRect || m_nFocusId == i)
		{
			RECT rect(m_ForeignItem[i].rItem);
			InflateRect(&rect, -1, -1);
			FrameRect(m_HdcMem, &rect, g_ColorRefData.GetHbrWhite());
			InflateRect(&rect, -1, -1);
			FrameRect(m_HdcMem, &rect, g_ColorRefData.GetHbrWhite());
		}

		//左键状态 价格、文本 重绘
		SelectObject(m_HdcMem, g_FontData.GetFontNumEx17());
		DrawText(m_HdcMem, m_ForeignItem[i].wContent[IT_Price], wcslen(m_ForeignItem[i].wContent[IT_Price]), &(!m_bClickDown?m_ForeignItem[i].rTextUp[OB_Price]: m_ForeignItem[i].rTextDown[OB_Price]), DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
		SelectObject(m_HdcMem, g_FontData.GetFontWord22());
		DrawText(m_HdcMem, m_ForeignItem[i].wContent[IT_NoOffset], wcslen(m_ForeignItem[i].wContent[IT_NoOffset]), &(!m_bClickDown ? m_ForeignItem[i].rTextUp[OB_Direct]: m_ForeignItem[i].rTextDown[OB_Direct]), DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		
		//中线重绘
		RECT rectLine;
		rectLine = !m_bClickDown?m_ForeignItem[i].rTextUp[OB_Price]: m_ForeignItem[i].rTextDown[OB_Price];
		
		HPEN hPen_level2 = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorWhite());
		HPEN OldPen = (HPEN)SelectObject(m_HdcMem, hPen_level2);
		MoveToEx(m_HdcMem, rectLine.left + N_INTERVAL, rectLine.bottom + 1, NULL);
		LineTo(m_HdcMem, rectLine.right - N_INTERVAL, rectLine.bottom + 1);
		SelectObject(m_HdcMem, OldPen);
		DeleteObject(hPen_level2);
	}
	for (int i = COC_QtyB; i <= COC_CurrencyS; i++)
	{
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorGray());
		SelectObject(m_HdcMem, g_FontData.GetFontWord12());
		DrawText(m_HdcMem, m_CanOpenItemF[i].wContent, wcslen(m_CanOpenItemF[i].wContent), &m_CanOpenItemF[i].rItem, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	if (m_PosInfo.nLong>0)
	{
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());
		SelectObject(m_HdcMem, g_FontData.GetFontWord15());
		RECT rect = m_ForeignItem[OBD_BuyF].rItem;
		rect.left = m_ForeignItem[OBD_BuyF].rItem.right - 30;
		rect.right = m_ForeignItem[OBD_BuyF].rItem.right - 3;
		rect.top = m_ForeignItem[OBD_BuyF].rItem.bottom - 20;
		wchar_t text[20] = { 0 };
		swprintf_s(text, L"%d", m_PosInfo.nLong);
		DrawText(m_HdcMem, text, wcslen(text), &rect, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
	}
	if (m_PosInfo.nShort>0)
	{
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());
		SelectObject(m_HdcMem, g_FontData.GetFontWord15());
		RECT rect = m_ForeignItem[OBD_SellF].rItem;
		rect.right = m_ForeignItem[OBD_SellF].rItem.left + 30;
		rect.left = m_ForeignItem[OBD_SellF].rItem.left + 3;
		rect.top = m_ForeignItem[OBD_SellF].rItem.bottom - 20;
		wchar_t text[20] = { 0 };
		swprintf_s(text, L"%d", m_PosInfo.nShort);
		DrawText(m_HdcMem, text, wcslen(text), &rect, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	}
}
void COrderButton::paint_obc_3()
{
	for (int i = OBD_Buy; i <= OBD_Sell; i++)
	{
		int nIt = m_InnerItem[i].nIt;
		HBRUSH hBkbrush = m_BuyBrush;
		switch (i)
		{
		case OBD_Buy:
			hBkbrush = m_BuyBrush; 
            if (IsInnerOffer())
            {
                nIt = IT_Offer;
            }
            if (m_bShiftPosition)
            {
                nIt = IT_ShiftPosition;
            }
			break;
		case OBD_Cover:
			if (nIt == IT_HasPosition)
			{
				hBkbrush = m_SellBrush;
				wcscpy_s(m_InnerItem[i].wContent[IT_Price], m_InnerItem[OBD_Sell].wContent[IT_Price]);
                if (IsInnerOffer())
                {
                    nIt = IT_Offer;
                }
                if (m_bShiftPosition)
                {
                    nIt = IT_ShiftPosition2;
                }
			}
			else if (nIt == IT_HasMatchPosition)
			{
				hBkbrush = m_BuyBrush;
				wcscpy_s(m_InnerItem[i].wContent[IT_Price], m_InnerItem[OBD_Buy].wContent[IT_Price]);
				if (IsInnerOffer())
                    nIt = IT_Offer;
                if (m_bShiftPosition)
                {
                    nIt = IT_ShiftPosition;
                }
			}
			break;
		case OBD_Sell:
			hBkbrush = m_SellBrush;
            if (IsInnerOffer())
            {
                nIt = IT_Offer;
            }
            if (m_bShiftPosition)
            {
                nIt = IT_ShiftPosition;
            }
			break;
		default:
			break;
		}
		
		FillRect(m_HdcMem, &m_InnerItem[i].rItem, hBkbrush);
		SetBkMode(m_HdcMem, TRANSPARENT);
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());
		if (m_InnerItem[i].bInRect || (m_PosInfo.nButton == obc_3&&m_nFocusId == i))
		{
			RECT rect(m_InnerItem[i].rItem);
			InflateRect(&rect, -1, -1);
			FrameRect(m_HdcMem, &rect, g_ColorRefData.GetHbrWhite());
			InflateRect(&rect, -1, -1);
			FrameRect(m_HdcMem, &rect, g_ColorRefData.GetHbrWhite());
		}
		if (m_PosInfo.nButton == obc_4)
		{
			if ((m_nFocusId == 3 && i == 2) || (m_nFocusId == 0 && i == 0))
			{
				RECT rect(m_InnerItem[i].rItem);
				InflateRect(&rect, -1, -1);
				FrameRect(m_HdcMem, &rect, g_ColorRefData.GetHbrWhite());
			}
		}

		RECT rectLine;
		if (m_bClickDown&&i == m_nInnerClickIndex)
		{
			SelectObject(m_HdcMem, g_FontData.GetFontNumEx17());
			DrawText(m_HdcMem, m_InnerItem[i].wContent[IT_Price], wcslen(m_InnerItem[i].wContent[IT_Price]), &m_InnerItem[i].rTextDown[OB_Price], DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
			if(!IsInnerOffer())
				SelectObject(m_HdcMem, g_FontData.GetFontWord22());
			else
				SelectObject(m_HdcMem, g_FontData.GetFontWord19());
			DrawText(m_HdcMem, m_InnerItem[i].wContent[nIt], wcslen(m_InnerItem[i].wContent[nIt]), &m_InnerItem[i].rTextDown[OB_Direct], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			rectLine = m_InnerItem[i].rTextDown[OB_Price];

		}
		else
		{
			SelectObject(m_HdcMem, g_FontData.GetFontNumEx17());
			DrawText(m_HdcMem, m_InnerItem[i].wContent[IT_Price], wcslen(m_InnerItem[i].wContent[IT_Price]), &m_InnerItem[i].rTextUp[OB_Price], DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
			if (!IsInnerOffer())
				SelectObject(m_HdcMem, g_FontData.GetFontWord22());
			else
				SelectObject(m_HdcMem, g_FontData.GetFontWord19());
			DrawText(m_HdcMem, m_InnerItem[i].wContent[nIt], wcslen(m_InnerItem[i].wContent[nIt]), &m_InnerItem[i].rTextUp[OB_Direct], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			rectLine = m_InnerItem[i].rTextUp[OB_Price];
		}
		HPEN hPen_level2 = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorWhite());
		HPEN OldPen = (HPEN)SelectObject(m_HdcMem, hPen_level2);
		MoveToEx(m_HdcMem, rectLine.left + N_INTERVAL, rectLine.bottom + 1, NULL);
		LineTo(m_HdcMem, rectLine.right - N_INTERVAL, rectLine.bottom + 1);
		SelectObject(m_HdcMem, OldPen);
		DeleteObject(hPen_level2);		
	}
	for (int i = COC_OpenB; i <= COC_CoverS; i++)
	{
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorGray());
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());
		if (COC_CoverB == i|| COC_CoverS == i)
		{
			if(wcscmp(m_CanOpenItem[COC_CoverB].wContent, L""))
				DrawText(m_HdcMem, m_CanOpenItem[COC_CoverB].wContent, wcslen(m_CanOpenItem[COC_CoverB].wContent), &m_CanOpenItem[COC_CoverS].rItem, DT_TOP | DT_CENTER | DT_SINGLELINE);
			else
				DrawText(m_HdcMem, m_CanOpenItem[COC_CoverS].wContent, wcslen(m_CanOpenItem[COC_CoverS].wContent), &m_CanOpenItem[COC_CoverS].rItem, DT_TOP | DT_CENTER | DT_SINGLELINE);
		}
		else
			DrawText(m_HdcMem, m_CanOpenItem[i].wContent, wcslen(m_CanOpenItem[i].wContent), &m_CanOpenItem[i].rItem, DT_TOP | DT_CENTER | DT_SINGLELINE);
	}
}
void COrderButton::paint_obc_4()
{
	for (int i = OBP_Buy; i <= OBP_Sell; i++)
	{
		HBRUSH hBkbrush = m_BuyBrush;
		switch (i)
		{
		case OBP_Buy:
			hBkbrush = m_BuyBrush;
			break;
		case OBP_CoverS:
		case OBP_CoverL:
			if (m_InnerItemP[i].nIt == IT_NoPosition)
				hBkbrush = g_ColorRefData.GetHbrRGB200();
			else
				hBkbrush = OBP_CoverS == i ? m_BuyBrush : m_SellBrush;
			break;
		case OBP_Sell:
			hBkbrush = m_SellBrush;
			break;
		default:
			break;
		}
		if (OBP_CoverS == i || OBP_CoverL == i)
		{
			RECT rect1 = m_InnerItemP[i].rItem;
			FillRect(m_HdcMem, &rect1, hBkbrush);
			
			SetBkMode(m_HdcMem, TRANSPARENT);
			SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());
			InflateRect(&rect1, -1, -1);
			if (m_InnerItemP[i].bInRect || m_nFocusId == i)
				FrameRect(m_HdcMem, &rect1, g_ColorRefData.GetHbrWhite());
			if (!IsInnerOffer())
				SelectObject(m_HdcMem, g_FontData.GetFontWord22());
			else
				SelectObject(m_HdcMem, g_FontData.GetFontWord19());
			int nIt = OBP_CoverS == i ? IT_HasMatchPosition : IT_HasPosition;
			if (IsInnerOffer())
                nIt = IT_Offer;
            if (m_bShiftPosition)
            {
                nIt = OBP_CoverS == i ? IT_ShiftPosition : IT_ShiftPosition2;
            }

			if (m_bClickDown&&i == m_nInnerClickIndex)
				DrawText(m_HdcMem, m_InnerItemP[i].wContent[nIt], wcslen(m_InnerItemP[i].wContent[nIt]), &m_InnerItemP[i].rTextDown[OB_Price], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			else
				DrawText(m_HdcMem, m_InnerItemP[i].wContent[nIt], wcslen(m_InnerItemP[i].wContent[nIt]), &m_InnerItemP[i].rTextUp[OB_Price], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		else
		{
			FillRect(m_HdcMem, &m_InnerItemP[i].rItem, hBkbrush);
			SetBkMode(m_HdcMem, TRANSPARENT);
			SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());
			if (m_InnerItemP[i].bInRect || m_nFocusId == i)
			{
				RECT rect(m_InnerItemP[i].rItem);
				InflateRect(&rect, -1, -1);
				FrameRect(m_HdcMem, &rect, g_ColorRefData.GetHbrWhite());
				InflateRect(&rect, -1, -1);
				FrameRect(m_HdcMem, &rect, g_ColorRefData.GetHbrWhite());
			}

			int nIt = m_InnerItemP[i].nIt;
            if (IsInnerOffer())
            {
                nIt = IT_Offer;
            }
            if (m_bShiftPosition)
            {
                nIt = IT_ShiftPosition;
            }

			RECT rectLine;
			if (m_bClickDown&&i == m_nInnerClickIndex)
			{
				SelectObject(m_HdcMem, g_FontData.GetFontNumEx17());
				DrawText(m_HdcMem, m_InnerItemP[i].wContent[IT_Price], wcslen(m_InnerItemP[i].wContent[IT_Price]), &m_InnerItemP[i].rTextDown[OB_Price], DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
				if (!IsInnerOffer())
					SelectObject(m_HdcMem, g_FontData.GetFontWord22());
				else
					SelectObject(m_HdcMem, g_FontData.GetFontWord19());
				DrawText(m_HdcMem, m_InnerItemP[i].wContent[nIt], wcslen(m_InnerItemP[i].wContent[nIt]), &m_InnerItemP[i].rTextDown[OB_Direct], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
				rectLine = m_InnerItemP[i].rTextDown[OB_Price];
			}
			else
			{
				SelectObject(m_HdcMem, g_FontData.GetFontNumEx17());
				DrawText(m_HdcMem, m_InnerItemP[i].wContent[IT_Price], wcslen(m_InnerItemP[i].wContent[IT_Price]), &m_InnerItemP[i].rTextUp[OB_Price], DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
				if (!IsInnerOffer())
					SelectObject(m_HdcMem, g_FontData.GetFontWord22()); 
				else
					SelectObject(m_HdcMem, g_FontData.GetFontWord19());
				DrawText(m_HdcMem, m_InnerItemP[i].wContent[nIt], wcslen(m_InnerItemP[i].wContent[nIt]), &m_InnerItemP[i].rTextUp[OB_Direct], DT_VCENTER | DT_CENTER | DT_SINGLELINE);
				rectLine = m_InnerItemP[i].rTextUp[OB_Price];
			}
			HPEN hPen_level2 = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorWhite());
			HPEN OldPen = (HPEN)SelectObject(m_HdcMem, hPen_level2);
			MoveToEx(m_HdcMem, rectLine.left + N_INTERVAL, rectLine.bottom + 1, NULL);
			LineTo(m_HdcMem, rectLine.right - N_INTERVAL, rectLine.bottom + 1);
			SelectObject(m_HdcMem, OldPen);
			DeleteObject(hPen_level2);
		}
	}
	for (int i = COC_OpenB; i <= COC_CoverS; i++)
	{
		SetTextColor(m_HdcMem, g_ColorRefData.GetColorGray());
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());
		DrawText(m_HdcMem, m_CanOpenItem[i].wContent, wcslen(m_CanOpenItem[i].wContent), &m_CanOpenItem[i].rItem, DT_TOP | DT_CENTER | DT_SINGLELINE);
	}
}
LONG COrderButton::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LONG)m_BKBrush;
}
void COrderButton::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (::GetFocus() != m_Hwnd)
		SetFocus(m_Hwnd);
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	m_bClickDown = true;
	if (IsInnerType())
	{
		if (m_PosInfo.nButton==obc_3)
		{
			for (size_t i = OBD_Buy; i <= OBD_Sell; i++)
			{
				if (PtInRect(&m_InnerItem[i].rItem, pointB))
				{
					m_nInnerClickIndex = i;
					m_nFocusId = i;
					InvalidateRect(m_Hwnd, &m_InnerItem[i].rItem, TRUE);
				}
			}
		}
		else
		{
			for (size_t i = OBP_Buy; i <= OBP_Sell; i++)
			{
				if (PtInRect(&m_InnerItemP[i].rItem, pointB))
				{
					m_nInnerClickIndex = i;
					m_nFocusId = i;
					InvalidateRect(m_Hwnd, &m_InnerItemP[i].rItem, TRUE);
				}
			}
		}
	}
	else
	{
		for (size_t i = OBD_BuyF; i <= OBD_SellF; i++)
		{
			if (PtInRect(&m_ForeignItem[i].rItem, pointB))
			{
				m_nForeignClickIndex = i;
				m_nFocusId = i;
				InvalidateRect(m_Hwnd, &m_ForeignItem[i].rItem, TRUE);
			}
		}
	}
	UpdateWindow(m_Hwnd);
}
void COrderButton::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	if (IsInnerType())
	{
		if (m_PosInfo.nButton == obc_3)
		{
			for (size_t i = OBD_Buy; i <= OBD_Sell; i++)
			{
				if (PtInRect(&m_InnerItem[i].rItem, pointB) && m_nInnerClickIndex == i)
				{
					int npos = 0;
					if (i == OBD_Cover)
					{
						if (m_InnerItem[i].rItem.top + (m_InnerItem[i].rItem.bottom - m_InnerItem[i].rItem.top) / 2 - 1 > pointB.y)
							npos = -1;
						else if (m_InnerItem[i].rItem.top + (m_InnerItem[i].rItem.bottom - m_InnerItem[i].rItem.top) / 2 + 1 < pointB.y)
							npos = 1;
					}
					OnClickItemInner(i);
					InvalidateRect(m_Hwnd, &m_InnerItem[i].rItem, TRUE);
				}
			}
		}
		else
		{
			for (size_t i = OBP_Buy; i <= OBP_Sell; i++)
			{
				if (PtInRect(&m_InnerItemP[i].rItem, pointB) && m_nInnerClickIndex == i)
				{
					OnClickItemInnerP(i);
					InvalidateRect(m_Hwnd, &m_InnerItemP[i].rItem, TRUE);
				}
			}
		}
	}
	else
	{
		for (size_t i = OBD_BuyF; i <= OBD_SellF; i++)
		{
			if (PtInRect(&m_ForeignItem[i].rItem, pointB) && m_nForeignClickIndex == i)
			{
				OnClickItemForeign(i);
				InvalidateRect(m_Hwnd, &m_ForeignItem[i].rItem, TRUE);
			}
		}
	}
	m_nInnerClickIndex = -1;
	m_nForeignClickIndex = -1;
	m_bClickDown = false;
	UpdateWindow(m_Hwnd);
}
void COrderButton::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	if (IsInnerType())
	{
		if (m_PosInfo.nButton==obc_3)
		{
			for (size_t i = OBD_Buy; i <= OBD_Sell; i++)
			{
				bool bOldState = m_InnerItem[i].bInRect;
				if (PtInRect(&m_InnerItem[i].rItem, pointB))
					m_InnerItem[i].bInRect = true;
				else
					m_InnerItem[i].bInRect = false;

				if (bOldState != m_InnerItem[i].bInRect)
					InvalidateRect(m_Hwnd, &m_InnerItem[i].rItem, TRUE);
			}
		}
		else
		{
			for (size_t i = OBP_Buy; i <= OBP_Sell; i++)
			{
				bool bOldState = m_InnerItemP[i].bInRect;
				if (PtInRect(&m_InnerItemP[i].rItem, pointB))
					m_InnerItemP[i].bInRect = true;
				else
					m_InnerItemP[i].bInRect = false;

				if (bOldState != m_InnerItemP[i].bInRect)
					InvalidateRect(m_Hwnd, &m_InnerItemP[i].rItem, TRUE);
			}
		}
	}
	else
	{
		for (size_t i = OBD_BuyF; i <= OBD_SellF; i++)
		{
			bool bOldState = m_ForeignItem[i].bInRect;
			if (PtInRect(&m_ForeignItem[i].rItem, pointB))
				m_ForeignItem[i].bInRect = true;
			else
				m_ForeignItem[i].bInRect = false;
			if (bOldState != m_ForeignItem[i].bInRect)
				InvalidateRect(m_Hwnd, &m_ForeignItem[i].rItem, TRUE);
		}
	}

	//鼠标跟踪
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_Hwnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 50;
		m_bTracking = _TrackMouseEvent(&tme) == TRUE;
	}
}
void COrderButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (IsInnerType())
	{
		if (m_PosInfo.nButton == obc_3)
		{
			for (size_t i = OBD_Buy; i <= OBD_Sell; i++)
			{
				m_InnerItem[i].bInRect = false;
				InvalidateRect(m_Hwnd, &m_InnerItem[i].rItem, TRUE);
			}
		}
		else
		{
			for (size_t i = OBP_Buy; i <= OBP_Sell; i++)
			{
				m_InnerItemP[i].bInRect = false;
				InvalidateRect(m_Hwnd, &m_InnerItemP[i].rItem, TRUE);
			}
		}
	}
	else
	{
		for (size_t i = OBD_BuyF; i <= OBD_SellF; i++)
		{
			m_ForeignItem[i].bInRect = false;
			InvalidateRect(m_Hwnd, &m_ForeignItem[i].rItem, TRUE);
		}
	}

	m_bClickDown = false;
	m_bTracking = FALSE;
	UpdateWindow(m_Hwnd);
}
void COrderButton::OnSetFocus(int nID)
{
	SetFocus(m_Hwnd);
	m_nFocusId = nID;
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);
}
void COrderButton::OnSetFocus(char cDirect, char cOffset)
{
	switch (m_PosInfo.nButton)
	{
	case obc_2:
		if (cDirect == dBuy)
			OnSetFocus(OBD_BuyF);
		else
			OnSetFocus(OBD_SellF);
		break;
	case obc_3:
		if (cOffset!=oOpen&&(m_PosInfo.nLong>0 || m_PosInfo.nShort>0))
			OnSetFocus(OBD_Cover);
		else
			cDirect == dBuy ? OnSetFocus(OBD_Buy) : OnSetFocus(OBD_Sell);
		break;	
	case obc_4:
		if (cDirect==dBuy)
		{
			if (cOffset==oOpen)
				OnSetFocus(OBP_Buy);
			else
				OnSetFocus(OBP_CoverS);
		}
		else
		{
			if (cOffset == oOpen)
				OnSetFocus(OBP_Sell);
			else
				OnSetFocus(OBP_CoverL);
		}
		break;
	}
}
void COrderButton::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	m_nFocusId = -1;
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);
}
void COrderButton::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (m_nFocusId!=-1)
	{
		switch (wParam)
		{
		case VK_TAB:
			m_nFocusId++;
			if (m_nFocusId >= m_PosInfo.nButton)
				m_nFocusId = -1;				
			break;
		case VK_DOWN:
		case VK_RIGHT:
			m_nFocusId++;
			if (m_nFocusId >= m_PosInfo.nButton)
				m_nFocusId = m_PosInfo.nButton-1;
			break;
		case VK_LEFT:
		case VK_UP:
			m_nFocusId--;
			if (m_nFocusId < 0)
				m_nFocusId = 0;
			else if (m_nFocusId >= m_PosInfo.nButton)
				m_nFocusId = m_PosInfo.nButton-1;
			break;
		case VK_RETURN:
			{
				if (m_nFocusId < 0)
					break;
				if (IsInnerType())
				{
					if (m_PosInfo.nButton == obc_3&&m_nFocusId < obc_3)
					{
						OnClickItemInner(m_nFocusId);
						InvalidateRect(m_Hwnd, &m_InnerItem[m_nFocusId].rItem, TRUE);
					}
					else if (m_PosInfo.nButton == obc_4&&m_nFocusId < obc_4)
					{
						OnClickItemInnerP(m_nFocusId);
						InvalidateRect(m_Hwnd, &m_InnerItemP[m_nFocusId].rItem, TRUE);
					}
				}
				else
				{
					OnClickItemForeign(m_nFocusId);
					InvalidateRect(m_Hwnd, &m_ForeignItem[m_nFocusId].rItem, TRUE);	
				}
			}
			break;
		}	
		if(m_nFocusId == -1)
			m_opr->TableButtonToEdit();

		InvalidateRect(m_Hwnd, NULL, TRUE);
		UpdateWindow(m_Hwnd);
	}
	
}
void COrderButton::UpdateLayout()
{
	int nTop = 13;
	int nDem = 3;
	for (int i = OBD_Buy; i <= OBD_Sell; i++)
	{
		m_InnerItem[i].rItem.left = N_INTERVAL + i * (N_WIDTH_ITEM + N_INTERVAL + 3);
		m_InnerItem[i].rItem.right = m_InnerItem[i].rItem.left + N_WIDTH_ITEM;
		m_InnerItem[i].rItem.top = nTop;
		m_InnerItem[i].rItem.bottom = m_InnerItem[i].rItem.top +N_HEIGHT_ITEM;
		m_InnerItem[i].rTextUp[0] = m_InnerItem[i].rItem;
		m_InnerItem[i].rTextUp[0].bottom = nTop + N_HEIGHT_ITEM / nDem;
		m_InnerItem[i].rTextUp[1] = m_InnerItem[i].rItem;
		m_InnerItem[i].rTextUp[1].top = nTop+N_HEIGHT_ITEM / nDem;
		m_InnerItem[i].rTextUp[1].bottom = nTop + N_HEIGHT_ITEM - 1;
		m_InnerItem[i].rTextDown[0] = m_InnerItem[i].rItem;
		m_InnerItem[i].rTextDown[0].bottom = nTop + N_HEIGHT_ITEM / nDem + 1;
		m_InnerItem[i].rTextDown[1] = m_InnerItem[i].rItem;
		m_InnerItem[i].rTextDown[1].top = nTop + N_HEIGHT_ITEM / nDem + 1;
		m_InnerItem[i].rTextDown[1].bottom = nTop + N_HEIGHT_ITEM;

		switch (i)
		{
		case 0:
			m_CanOpenItem[COC_OpenB].rItem = m_InnerItem[i].rItem;
			m_CanOpenItem[COC_OpenB].rItem.top = m_CanOpenItem[COC_OpenB].rItem.bottom;
			m_CanOpenItem[COC_OpenB].rItem.bottom = m_CanOpenItem[COC_OpenB].rItem.top + 20;
			break;		
		case 2:
			m_CanOpenItem[COC_OpenS].rItem = m_InnerItem[i].rItem;
			m_CanOpenItem[COC_OpenS].rItem.top = m_CanOpenItem[COC_OpenS].rItem.bottom;
			m_CanOpenItem[COC_OpenS].rItem.bottom = m_CanOpenItem[COC_OpenS].rItem.top + 20;
			break;
		default:
			m_CanOpenItem[COC_CoverB].rItem = m_InnerItem[i].rItem;
			m_CanOpenItem[COC_CoverB].rItem.top = 0;
			m_CanOpenItem[COC_CoverB].rItem.bottom = 13;
			m_CanOpenItem[COC_CoverS].rItem = m_InnerItem[i].rItem;
			m_CanOpenItem[COC_CoverS].rItem.top = nTop + 70;
			m_CanOpenItem[COC_CoverS].rItem.bottom = m_CanOpenItem[COC_CoverS].rItem.top + 20;
			break;
		}	
	}
	
	nTop = 10;
	for (int i = OBD_BuyF; i <= OBD_SellF; i++)
	{
		m_ForeignItem[i].rItem.left = N_INTERVAL + i * (N_WIDTH_ITEMF + N_INTERVAL);
		m_ForeignItem[i].rItem.right = m_ForeignItem[i].rItem.left + N_WIDTH_ITEMF;
		m_ForeignItem[i].rItem.top = nTop;
		m_ForeignItem[i].rItem.bottom = m_ForeignItem[i].rItem.top + N_HEIGHT_ITEM;

		m_ForeignItem[i].rTextUp[0] = m_ForeignItem[i].rItem;
		m_ForeignItem[i].rTextUp[0].bottom = nTop + N_HEIGHT_ITEM / nDem;
		m_ForeignItem[i].rTextUp[1] = m_ForeignItem[i].rItem;
		m_ForeignItem[i].rTextUp[1].top = nTop + N_HEIGHT_ITEM / nDem;
		m_ForeignItem[i].rTextUp[1].bottom = nTop + N_HEIGHT_ITEM - 1;
		m_ForeignItem[i].rTextDown[0] = m_ForeignItem[i].rItem;
		m_ForeignItem[i].rTextDown[0].bottom = nTop + N_HEIGHT_ITEM / nDem + 1;
		m_ForeignItem[i].rTextDown[1] = m_ForeignItem[i].rItem;
		m_ForeignItem[i].rTextDown[1].top = nTop + N_HEIGHT_ITEM / nDem + 1;
		m_ForeignItem[i].rTextDown[1].bottom = nTop + N_HEIGHT_ITEM;

		m_CanOpenItemF[i * 3].rItem = m_ForeignItem[i].rItem;
		m_CanOpenItemF[i * 3].rItem.top = m_CanOpenItemF[i * 3].rItem.bottom;
		m_CanOpenItemF[i * 3].rItem.bottom = m_CanOpenItemF[i * 3].rItem.top + 14;
		m_CanOpenItemF[i * 3 + 1].rItem = m_CanOpenItemF[i * 3].rItem;
		m_CanOpenItemF[i * 3 + 1].rItem.top = m_CanOpenItemF[i * 3 + 1].rItem.bottom;
		m_CanOpenItemF[i * 3 + 1].rItem.bottom = m_CanOpenItemF[i * 3 + 1].rItem.top + 14;
		m_CanOpenItemF[i * 3 + 2].rItem = m_CanOpenItemF[i * 3 + 1].rItem;
		m_CanOpenItemF[i * 3 + 2].rItem.top = m_CanOpenItemF[i * 3 + 2].rItem.bottom;
		m_CanOpenItemF[i * 3 + 2].rItem.bottom = m_CanOpenItemF[i * 3 + 2].rItem.top + 14;
	}

	wcscpy_s(m_InnerItem[OBD_Buy].wContent[IT_NoOffset], LoadResWchar_t(IDS_BUY));
	wcscpy_s(m_InnerItem[OBD_Buy].wContent[IT_NoPosition], LoadResWchar_t(IDS_STRING1135));
	wcscpy_s(m_InnerItem[OBD_Buy].wContent[IT_HasPosition], LoadResWchar_t(IDS_AddLong));
	wcscpy_s(m_InnerItem[OBD_Buy].wContent[IT_HasMatchPosition], LoadResWchar_t(IDS_LockPosition));
	swprintf_s(m_InnerItem[OBD_Buy].wContent[IT_Offer],L"%s%s", LoadResWchar_t(IDS_STRING1135), LoadResWchar_t(IDS_STRING1133));
    wcscpy_s(m_InnerItem[OBD_Buy].wContent[IT_ShiftPosition], LoadResWchar_t(IDS_BuyOpenSellCover));

	wcscpy_s(m_InnerItem[OBD_Sell].wContent[IT_NoOffset], LoadResWchar_t(IDS_SELL));
	wcscpy_s(m_InnerItem[OBD_Sell].wContent[IT_NoPosition], LoadResWchar_t(IDS_STRING1133));
	wcscpy_s(m_InnerItem[OBD_Sell].wContent[IT_HasPosition], LoadResWchar_t(IDS_AddShort));
	wcscpy_s(m_InnerItem[OBD_Sell].wContent[IT_HasMatchPosition], LoadResWchar_t(IDS_LockPosition));
	swprintf_s(m_InnerItem[OBD_Sell].wContent[IT_Offer], L"%s%s", LoadResWchar_t(IDS_STRING1136), LoadResWchar_t(IDS_STRING1134));
    wcscpy_s(m_InnerItem[OBD_Sell].wContent[IT_ShiftPosition], LoadResWchar_t(IDS_SellOpenBuyCover));

	wcscpy_s(m_InnerItem[OBD_Cover].wContent[IT_NoOffset], LoadResWchar_t(IDS_CLOSE));
	wcscpy_s(m_InnerItem[OBD_Cover].wContent[IT_NoPosition], LoadResWchar_t(IDS_CLOSE));
	wcscpy_s(m_InnerItem[OBD_Cover].wContent[IT_HasPosition], LoadResWchar_t(IDS_CloseLong));
	wcscpy_s(m_InnerItem[OBD_Cover].wContent[IT_HasMatchPosition], LoadResWchar_t(IDS_CloseShort));
	swprintf_s(m_InnerItem[OBD_Cover].wContent[IT_Offer], L"%s%s", LoadResWchar_t(IDS_STRING1135), LoadResWchar_t(IDS_STRING1134));
	swprintf_s(m_InnerItem[OBD_Cover].wContent[IT_Offer2], L"%s%s", LoadResWchar_t(IDS_STRING1136), LoadResWchar_t(IDS_STRING1133));
    wcscpy_s(m_InnerItem[OBD_Cover].wContent[IT_ShiftPosition], LoadResWchar_t(IDS_BuyCoverSellOpen));
    wcscpy_s(m_InnerItem[OBD_Cover].wContent[IT_ShiftPosition2], LoadResWchar_t(IDS_SellCoverBuyOpen));

	wcscpy_s(m_ForeignItem[OBD_BuyF].wContent[IT_NoOffset], LoadResWchar_t(IDS_BUY));
	wcscpy_s(m_ForeignItem[OBD_SellF].wContent[IT_NoOffset], LoadResWchar_t(IDS_SELL));

	memcpy_s(&m_InnerItemP[OBP_Buy], sizeof(ItemInfo), &m_InnerItem[OBD_Buy], sizeof(ItemInfo));
	memcpy_s(&m_InnerItemP[OBP_CoverS], sizeof(ItemInfo), &m_InnerItem[OBD_Cover], sizeof(ItemInfo));
	m_InnerItemP[OBP_CoverS].rItem.bottom = m_InnerItemP[OBP_CoverS].rItem.top + 30;
	m_InnerItemP[OBP_CoverS].rItem.top = m_InnerItemP[OBP_CoverS].rItem.top;
	m_InnerItemP[OBP_CoverS].rTextUp[0] = m_InnerItemP[OBP_CoverS].rItem;
	m_InnerItemP[OBP_CoverS].rTextUp[0].bottom = m_InnerItemP[OBP_CoverS].rTextUp[0].bottom-1;
	m_InnerItemP[OBP_CoverS].rTextDown[0] = m_InnerItemP[OBP_CoverS].rItem;
	m_InnerItemP[OBP_CoverS].rTextDown[0].top = m_InnerItemP[OBP_CoverS].rTextDown[0].top + 1;
	swprintf_s(m_InnerItemP[OBP_CoverS].wContent[IT_Offer], L"%s%s", LoadResWchar_t(IDS_STRING1136), LoadResWchar_t(IDS_STRING1133));
    wcscpy_s(m_InnerItemP[OBP_CoverS].wContent[IT_ShiftPosition], LoadResWchar_t(IDS_BuyCoverSellOpen));

	memcpy_s(&m_InnerItemP[OBP_CoverL], sizeof(ItemInfo), &m_InnerItem[OBD_Cover], sizeof(ItemInfo));
	m_InnerItemP[OBP_CoverL].rItem.top = m_InnerItemP[OBP_CoverL].rItem.top + 30 + 10;
	m_InnerItemP[OBP_CoverL].rItem.bottom = m_InnerItemP[OBP_CoverL].rItem.top + 30;
	m_InnerItemP[OBP_CoverL].rTextUp[0] = m_InnerItemP[OBP_CoverL].rItem;
	m_InnerItemP[OBP_CoverL].rTextUp[0].bottom = m_InnerItemP[OBP_CoverL].rTextUp[0].bottom - 1;
	m_InnerItemP[OBP_CoverL].rTextDown[0] = m_InnerItemP[OBP_CoverL].rItem;
	m_InnerItemP[OBP_CoverL].rTextDown[0].top = m_InnerItemP[OBP_CoverL].rTextDown[0].top + 1;
	swprintf_s(m_InnerItemP[OBP_CoverL].wContent[IT_Offer], L"%s%s", LoadResWchar_t(IDS_STRING1135), LoadResWchar_t(IDS_STRING1134));
    wcscpy_s(m_InnerItemP[OBP_CoverL].wContent[IT_ShiftPosition], LoadResWchar_t(IDS_SellCoverBuyOpen));

	memcpy_s(&m_InnerItemP[OBP_Sell], sizeof(ItemInfo), &m_InnerItem[OBD_Sell], sizeof(ItemInfo));
	
}

void COrderButton::UpdateData(const PositionInfo& info)
{
	int nsize = m_PosInfo.nButton;
	memcpy_s(&m_PosInfo, sizeof(PositionInfo), &info,sizeof(PositionInfo));
	if (IsInnerType())
	{
		if ((m_PosInfo.nLong <= 0 && m_PosInfo.nShort <= 0) || ((m_PosInfo.nLong > 0 && m_PosInfo.nShort > 0)))
		{
			for (int i = OBP_Buy; i <= OBP_Sell; i++)
			{
				switch (i)
				{
				case OBP_Buy:
					if (!IsInnerSwap())
					{
						if (m_PosInfo.nShort > 0 && m_PosInfo.nShort >= (m_PosInfo.nLong + m_PosInfo.nEditQty))
							m_InnerItemP[i].nIt = IT_HasMatchPosition;//锁仓
						else if (m_PosInfo.nShort > 0 || m_PosInfo.nLong > 0)
							m_InnerItemP[i].nIt = IT_HasPosition;//加多
						else
							m_InnerItemP[i].nIt = IT_NoPosition;//买开
					}
					else
					{
						if (m_PosInfo.nShort == 0)
							m_InnerItem[i].nIt = IT_HasPosition;//加多
						else
							m_InnerItem[i].nIt = IT_NoPosition;//买开
					}
					break;
				case OBP_CoverS:
				case OBP_CoverL:
				{					
					if (m_PosInfo.nLong <= 0 && m_PosInfo.nShort <= 0)
						m_InnerItemP[i].nIt = IT_NoPosition;
					else 
						m_InnerItemP[i].nIt = OBP_CoverS == i ?IT_HasMatchPosition  : IT_HasPosition;//平空 平多
				}
				break;
				case OBP_Sell:
					if (!IsInnerSwap())
					{
						if (m_PosInfo.nLong > 0 && m_PosInfo.nLong >= (m_PosInfo.nShort + m_PosInfo.nEditQty))
							m_InnerItemP[i].nIt = IT_HasMatchPosition;//锁仓
						else if (m_PosInfo.nLong > 0 || m_PosInfo.nShort > 0)
							m_InnerItemP[i].nIt = IT_HasPosition;//加空
						else
							m_InnerItemP[i].nIt = IT_NoPosition;//卖开
					}
					else
					{
						if (m_PosInfo.nLong == 0)
							m_InnerItem[i].nIt = IT_HasPosition;//加多
						else
							m_InnerItem[i].nIt = IT_NoPosition;//买开
					}

					break;
				default:
					break;
				}
			}
			m_PosInfo.nButton = obc_4;
		}
		else
		{
			for (int i = OBD_Buy; i <= OBD_Sell; i++)
			{
				switch (i)
				{
				case OBD_Buy:
					if (!IsInnerSwap())
					{
						if (m_PosInfo.nShort > 0 && m_PosInfo.nShort >= (m_PosInfo.nLong + m_PosInfo.nEditQty))
							m_InnerItem[i].nIt = IT_HasMatchPosition;//锁仓
						else if (m_PosInfo.nShort > 0 || m_PosInfo.nLong > 0)
							m_InnerItem[i].nIt = IT_HasPosition;//加多
						else
							m_InnerItem[i].nIt = IT_NoPosition;//买开
					}
					else
					{
						if (m_PosInfo.nShort == 0)
							m_InnerItem[i].nIt = IT_HasPosition;//加多
						else
							m_InnerItem[i].nIt = IT_NoPosition;//买开
					}
					break;
				case OBD_Cover:
				{				
					if (m_PosInfo.nLong > 0)
						m_InnerItem[i].nIt = IT_HasPosition;//平多
					else
						m_InnerItem[i].nIt = IT_HasMatchPosition;//平空
				}
				break;
				case OBD_Sell:
					if (!IsInnerSwap())
					{
						if (m_PosInfo.nLong > 0 && m_PosInfo.nLong >= (m_PosInfo.nShort + m_PosInfo.nEditQty))
							m_InnerItem[i].nIt = IT_HasMatchPosition;//锁仓
						else if (m_PosInfo.nLong > 0 || m_PosInfo.nShort > 0)
							m_InnerItem[i].nIt = IT_HasPosition;//加空
						else
							m_InnerItem[i].nIt = IT_NoPosition;//卖开
					}
					else
					{
						if (m_PosInfo.nLong == 0)
							m_InnerItem[i].nIt = IT_HasPosition;//加多
						else
							m_InnerItem[i].nIt = IT_NoPosition;//买开
					}
					break;
				default:
					break;
				}			
			}
			m_PosInfo.nButton = obc_3;
		}
	}
	else
		m_PosInfo.nButton = obc_2;
	
	if (nsize != m_PosInfo.nButton)
	{
		m_nInnerClickIndex = -1;
		m_nForeignClickIndex = -1;
		m_bClickDown = false;
		m_nFocusId = -1;
	}

	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);
}
void COrderButton::UpdateEditQty(int nQty)
{
	m_PosInfo.nEditQty = nQty;
	UpdateData(m_PosInfo);
}
void COrderButton::UpdateForeignQty(wchar_t* wcon, TDirect direct)
{
	if (direct == dBuy)
	{
		wcscpy_s(m_CanOpenItemF[COC_QtyB].wContent, wcon);
		InvalidateRect(m_Hwnd, &m_CanOpenItemF[COC_QtyB].rItem, TRUE);
	}
	else
	{
		wcscpy_s(m_CanOpenItemF[COC_QtyS].wContent, wcon);
		InvalidateRect(m_Hwnd, &m_CanOpenItemF[COC_QtyS].rItem, TRUE);
	}
	UpdateWindow(m_Hwnd);
}
void COrderButton::UpdateForeignPre(wchar_t* wMoney, wchar_t* wCurrency, TDirect direct)
{
	if (direct == dBuy)
	{
		wcscpy_s(m_CanOpenItemF[COC_MoneyB].wContent, wMoney);
		wcscpy_s(m_CanOpenItemF[COC_CurrencyB].wContent, wCurrency);
		InvalidateRect(m_Hwnd, &m_CanOpenItemF[COC_MoneyB].rItem, TRUE);
		InvalidateRect(m_Hwnd, &m_CanOpenItemF[COC_CurrencyB].rItem, TRUE);
	}
	else
	{
		wcscpy_s(m_CanOpenItemF[COC_MoneyS].wContent, wMoney);
		wcscpy_s(m_CanOpenItemF[COC_CurrencyS].wContent, wCurrency);
		InvalidateRect(m_Hwnd, &m_CanOpenItemF[COC_MoneyS].rItem, TRUE);
		InvalidateRect(m_Hwnd, &m_CanOpenItemF[COC_CurrencyS].rItem, TRUE);
	}
	UpdateWindow(m_Hwnd);
}
void COrderButton::UpdateCanOpen(int nBuy, int nSell)
{
	if (IsInnerType())
	{
		wchar_t wtext[N_LONGTEXT_LEN] = { 0 };
		if (nBuy > 0)
			swprintf_s(wtext, L"<= %d", nBuy);
		wcscpy_s(m_CanOpenItem[COC_OpenB].wContent, wtext);
		if (nSell > 0)
			swprintf_s(wtext, L"<= %d", nSell);
		else
			swprintf_s(wtext, L"");
		if (IsInnerOffer())
		{
			int npos = min(m_PosInfo.nLong, m_PosInfo.nShort);
			if (npos>0)
				swprintf_s(wtext, L"<= %d", npos);
			else
				swprintf_s(wtext, L"");
		}
		wcscpy_s(m_CanOpenItem[COC_OpenS].wContent, wtext);
		if (m_PosInfo.nLong > 0)
			swprintf_s(wtext, L"<= %d", m_PosInfo.nLong);
		else
			swprintf_s(wtext, L"");
		wcscpy_s(m_CanOpenItem[COC_CoverS].wContent, wtext);
		if (m_PosInfo.nShort > 0)
			swprintf_s(wtext, L"<= %d", m_PosInfo.nShort);
		else
			swprintf_s(wtext, L"");
		wcscpy_s(m_CanOpenItem[COC_CoverB].wContent, wtext);

		InvalidateRect(m_Hwnd, &m_CanOpenItem[COC_OpenB].rItem, TRUE);
		InvalidateRect(m_Hwnd, &m_CanOpenItem[COC_OpenS].rItem, TRUE);
		InvalidateRect(m_Hwnd, &m_CanOpenItem[COC_CoverB].rItem, TRUE);
		InvalidateRect(m_Hwnd, &m_CanOpenItem[COC_CoverS].rItem, TRUE);
	}
	
	UpdateWindow(m_Hwnd);
}
void COrderButton::UpdatePrice(wchar_t *wBuy, wchar_t *wSell)
{
	//两键
	wcscpy_s(m_ForeignItem[OBD_BuyF].wContent[IT_Price], wBuy);
	wcscpy_s(m_ForeignItem[OBD_SellF].wContent[IT_Price], wSell);
	//三键
	wcscpy_s(m_InnerItem[OBD_Buy].wContent[IT_Price], wBuy);
	wcscpy_s(m_InnerItem[OBD_Sell].wContent[IT_Price], wSell);
	//四键
	wcscpy_s(m_InnerItemP[OBP_Buy].wContent[IT_Price], wBuy);
	wcscpy_s(m_InnerItemP[OBP_Sell].wContent[IT_Price], wSell);
		
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);
}
void COrderButton::OnClickItemInner(int item)
{
	MsgOrderInfo orderInfo;	//
	switch (item)
	{
	case OBD_Buy:
		orderInfo.Direct = dBuy;
		orderInfo.Offset = oOpen;
		if (m_PosInfo.type == obInnerOffer)
			orderInfo.SellOffset = oOpen;
		break;
	case OBD_Sell:
		orderInfo.Direct = dSell;
		if (m_PosInfo.type == obInnerOffer)
			orderInfo.Offset = oCover;
		else
			orderInfo.Offset = oOpen;
		break;
	case OBD_Cover:
		orderInfo.Direct = m_PosInfo.nLong>0?dSell:dBuy;
		orderInfo.Offset = oCover;
		if (m_PosInfo.type == obInnerOffer)
			orderInfo.SellOffset = oCover;
		break;
	}
	
	ButtonSendOrder(orderInfo);
}
void COrderButton::OnClickItemInnerP(int item)
{
	MsgOrderInfo orderInfo;	//
	switch (item)
	{
	case OBP_Buy:
		orderInfo.Direct = dBuy;
		orderInfo.Offset = oOpen;
		if (m_PosInfo.type == obInnerOffer)
			orderInfo.SellOffset = oOpen;
		break;
	case OBP_Sell:
		orderInfo.Direct = dSell;
		if (m_PosInfo.type == obInnerOffer)
		{
			orderInfo.Offset = oCover;
			orderInfo.SellOffset = oCover;
		}
		else
			orderInfo.Offset = oOpen;
		break;
	case OBP_CoverS:
		if (m_PosInfo.nShort <= 0)
		{
			if (MessageBox(m_Hwnd, LoadResWchar_t(IDS_NoPositionTip), LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDCANCEL)
				return;
		}
		orderInfo.Direct = dBuy;
		orderInfo.Offset = oCover;
		if (m_PosInfo.type == obInnerOffer)
		{
			orderInfo.Offset = oCover;
			orderInfo.SellOffset = oOpen;
		}
		break;
	case OBP_CoverL:
		if (m_PosInfo.nLong <= 0)
		{
			if (MessageBox(m_Hwnd, LoadResWchar_t(IDS_NoPositionTip), LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDCANCEL)
				return;
		}
		orderInfo.Direct = dSell;
		orderInfo.Offset = oCover;
		if (m_PosInfo.type == obInnerOffer)
		{
			orderInfo.Offset = oOpen;
			orderInfo.SellOffset = oCover;
		}
		break;
	}

	ButtonSendOrder(orderInfo);
}
void COrderButton::OnClickItemForeign(int item)
{
	MsgOrderInfo orderInfo;	//
	switch (item)
	{
	case OBD_BuyF:
		orderInfo.Direct = dBuy;
		orderInfo.Offset = oNone;
		break;
	case OBD_SellF:
		orderInfo.Direct = dSell;
		orderInfo.Offset = oNone;
		break;
	default:
		return;
	}

	ButtonSendOrder(orderInfo);
}
void COrderButton::ButtonSendOrder(MsgOrderInfo &Msg)
{
	if (m_opr)
	{
		if (!m_opr->SendOrder(&Msg))
		{
			m_bTracking = false;
			InvalidateRect(m_Hwnd,NULL,TRUE);
			UpdateWindow(m_Hwnd);
		}
	}
}
void COrderButton::SetBShitPosition(bool bShiftPosition)
{
    m_bShiftPosition = bShiftPosition;
}
bool COrderButton::IsInnerType()
{
	return m_PosInfo.type == obInner || m_PosInfo.type == obInnerSwap || m_PosInfo.type == obInnerOffer;
};