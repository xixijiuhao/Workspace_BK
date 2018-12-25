#include "PreInclude.h"

//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;


CPopWinPrice::CPopWinPrice() :
m_opr(NULL),
m_nPos(0), 
m_nWidth(0), 
m_nHeight(0), 
m_nUintx(N_PRICE_WIDTH), 
m_nUinty(N_PRICE_HEIGHT), 
m_iIndex(0),
m_sPrice("")
{

}

CPopWinPrice::~CPopWinPrice()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWinPrice::CreatePopWin(IOperator *opr,LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ncenter, UINT ncol, UINT nrow)
{
	m_nCol = ncol;
	m_nRow = nrow;
	m_nLeft = nleft;
	m_nCenter = ncenter;
	m_opr = opr;
	CreateFrm(classname, parent, style | WS_POPUP | WS_BORDER, exstyle);
}

bool CPopWinPrice::UpdateData(SOFCommodityInf CommkodityInfo, double dMidprice, double dMaxprice, double dMinprice)
{
	memcpy(&m_CommkodityInfo, &CommkodityInfo, sizeof(SOFCommodityInf));
	m_sPrice.empty();
	if (IsValidPrice(dMidprice)&&IsValidPrice(dMaxprice)&&IsValidPrice(dMinprice))
	{
		m_MidPrice = dMidprice;
		m_MaxPrice = dMaxprice;
		m_MinPrice = dMinprice;
		m_iIndex = (m_nRow) / 2;
		InvalidateRect(m_Hwnd, NULL, TRUE);
	}
	else
	{}
	return true;
}

void CPopWinPrice::GetWinSize(int &nwidth, int &nheight)
{
	nwidth = m_nWidth;
	nheight = m_nHeight;
}

LRESULT CPopWinPrice::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		{
			//缓存 字体
			hdc = GetDC(m_Hwnd);
			
			m_HdcMem = CreateCompatibleDC(hdc);
			m_HdcUnit = CreateCompatibleDC(hdc);
			ReleaseDC(m_Hwnd, hdc);
			//规格
			m_nUintx = N_PRICE_WIDTH;
			m_nUinty = N_PRICE_HEIGHT;
			m_reUnit.left = 0;
			m_reUnit.right = m_nUintx;
			m_reUnit.top = 0;
			m_reUnit.bottom = m_nUinty;

			m_nWidth = m_nCol*N_PRICE_WIDTH + 2;
			m_nHeight = m_nRow*N_PRICE_HEIGHT+2;

			m_iIndex = (m_nRow) / 2;
			//MoveWindow(m_Hwnd, m_nLeft, m_nCenter-m_nHeight/2, m_nWidth, m_nHeight, TRUE);
			SetWindowPos(m_Hwnd, NULL, m_nLeft, m_nCenter - m_nHeight / 2, m_nWidth, m_nHeight, SWP_NOACTIVATE);
		}
		break;
	case WM_SIZE:
		{
			GetClientRect(m_Hwnd, &m_reClient);
			m_nWidth = m_reClient.right - m_reClient.left;
			m_nHeight = m_reClient.bottom - m_reClient.top;
			
			hdc = GetDC(m_Hwnd);

			if (m_hBKBitmap)
				DeleteObject(m_hBKBitmap);
			if (m_hFGBitmap)
				DeleteObject(m_hFGBitmap);

			m_hBKBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
			SelectObject(m_HdcMem, m_hBKBitmap);
			m_hFGBitmap = CreateCompatibleBitmap(hdc, m_nUintx, m_nUinty);
			SelectObject(m_HdcUnit, m_hFGBitmap);
			

			ReleaseDC(m_Hwnd, hdc);
		}
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(m_Hwnd, &ps);
  
		SelectObject(m_HdcUnit, g_FontData.GetFontNum13());

		FillRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrWhite());
		
		for (UINT i = 0; i < m_nRow; i++)
			DrawLineText(i);
		BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

		EndPaint(m_Hwnd, &ps);

	}
		break;
	case WM_MOUSEMOVE:
		POINT pointM;
		pointM.x = LOWORD(lParam);
		pointM.y = HIWORD(lParam);
		if (m_nPos != pointM.y / m_nUinty)
		{
			m_nPos = pointM.y / m_nUinty;
			InvalidateRect(m_Hwnd, NULL, TRUE);
		}
		break;
	case WM_LBUTTONDOWN:
		SetFocus(m_Hwnd);
		POINT pointB;
		pointB.x = LOWORD(lParam);
		pointB.y = HIWORD(lParam);
		m_nPos = pointB.y / m_nUinty;
		if (m_nPos == 0)
			m_sPrice = CConvert::GetStrPriceByPrice(m_MaxPrice, m_CommkodityInfo.bIfFractionPrice, m_CommkodityInfo.iPrecision);
		else if (m_nPos==m_nRow-1)
			m_sPrice = CConvert::GetStrPriceByPrice(m_MinPrice, m_CommkodityInfo.bIfFractionPrice, m_CommkodityInfo.iPrecision);
		else
			GetStrPriceByLineNo(m_nPos, m_sPrice);
		
		ShowWindow(m_Hwnd, SW_HIDE);
		if (m_opr)
			m_opr->SetPriceData(m_sPrice.c_str());	
		break;
	case WM_DESTROY:
		DeleteDC(m_HdcMem);
		DeleteDC(m_HdcUnit);
		DeleteObject(m_hBKBitmap);
		DeleteObject(m_hFGBitmap);
		
		
		break;
	case WM_ACTIVATE:
		if (WA_INACTIVE == wParam)
			ShowWindow(m_Hwnd, SW_HIDE);
		break;
	case WM_ERASEBKGND:
		break;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)//UP
			m_iIndex++;
		if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)//Down
			m_iIndex--;
		InvalidateRect(m_Hwnd,NULL,true);
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void CPopWinPrice::GetStrPriceByLineNo(int row, string &strPrice)
{
	char sPrice[64];
	if (m_CommkodityInfo.bIfFractionPrice)//分数   算出来的价格
	{
		SFractionPrice tempPrice;
		if (!GetFractionPriceByLineNo(row, tempPrice))
			return;
		
		if (tempPrice.iInteger <= 0 && tempPrice.iMolecules<0)
			sprintf_s(sPrice, "%d%d/%d", tempPrice.iInteger,/* nLen, */tempPrice.iMolecules, m_CommkodityInfo.iPrecision);
		else
			sprintf_s(sPrice, "%d+%d/%d", tempPrice.iInteger,/* nLen, */tempPrice.iMolecules, m_CommkodityInfo.iPrecision);
	}
	else//小数 算出来的价格
		sprintf_s(sPrice, "%0.*lf", m_CommkodityInfo.iPrecision, GetPriceByLineNo(row));
	strPrice = sPrice;
}

double CPopWinPrice::GetPriceByLineNo(int row)
{
	if (m_CommkodityInfo.bIfFractionPrice)//分数   算出来的价格
	{
		SFractionPrice tempPrice;
		if (!GetFractionPriceByLineNo(row, tempPrice))
			return 0;
		return  tempPrice.iInteger + (double)tempPrice.iMolecules / tempPrice.idenominator;
	}
	else
		return  m_MidPrice - (row - m_iIndex)*m_CommkodityInfo.dMiniChangePrice;
}

bool CPopWinPrice::GetFractionPriceByLineNo(int row, SFractionPrice &tempPrice)
{
	double dPrice = m_MidPrice;
	dPrice = dPrice + (m_iIndex - row)*((double)m_CommkodityInfo.dMiniChangePrice / m_CommkodityInfo.iPrecision);
	if (dPrice >= 0)
		CConvert::DecimalsToFractions(dPrice, m_CommkodityInfo.iPrecision, tempPrice);
	else
	{
		dPrice = -dPrice;
		CConvert::DecimalsToFractions(dPrice, m_CommkodityInfo.iPrecision, tempPrice);
		tempPrice.iInteger = -tempPrice.iInteger;
		tempPrice.iMolecules = -tempPrice.iMolecules;
	}
	return true;
}

bool CPopWinPrice::IsValidPrice(double dPrice)
{
	if (!m_CommkodityInfo.bCanNegative &&dPrice < 0)
		return false;
	return true;
}

void CPopWinPrice::DrawLineText(int nLine)
{
	HBRUSH hFGBrush = g_ColorRefData.GetHbrLightBlue();//背景画刷
	HBRUSH hBKBrush = g_ColorRefData.GetHbrBlack();//前景画刷 单元
	//价格
	string strPrice;
	if (0 == nLine&&IsValidPrice(m_MaxPrice))
	{
		strPrice = CConvert::GetStrPriceByPrice(m_MaxPrice, m_CommkodityInfo.bIfFractionPrice, m_CommkodityInfo.iPrecision);
		FillRect(m_HdcUnit, &m_reUnit, hBKBrush);//
		if (m_nPos == nLine)
			FillRect(m_HdcUnit, &m_reUnit, hFGBrush);//
		SetBkMode(m_HdcUnit, TRANSPARENT);//设置字体背景 
		SetTextColor(m_HdcUnit, g_ColorRefData.GetColorBuyRed());// 字体白
		DrawTextUnicode(m_HdcUnit, strPrice.c_str(), &m_reUnit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		BitBlt(m_HdcMem, m_reClient.left, nLine*N_PRICE_HEIGHT, m_nUintx, m_nUinty, m_HdcUnit, 0, 0, SRCCOPY);
	}	
	else if ((m_nRow - 1 == nLine) && IsValidPrice(m_MinPrice))
	{
		strPrice = CConvert::GetStrPriceByPrice(m_MinPrice, m_CommkodityInfo.bIfFractionPrice, m_CommkodityInfo.iPrecision);
		FillRect(m_HdcUnit, &m_reUnit, hBKBrush);//背景
		if (m_nPos == nLine)
			FillRect(m_HdcUnit, &m_reUnit, hFGBrush);//背景
		SetBkMode(m_HdcUnit, TRANSPARENT);//设置字体背景 
		SetTextColor(m_HdcUnit, g_ColorRefData.GetColorTextGreen());// 字体白
		DrawTextUnicode(m_HdcUnit, strPrice.c_str(), &m_reUnit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		BitBlt(m_HdcMem, m_reClient.left, nLine*N_PRICE_HEIGHT, m_nUintx, m_nUinty, m_HdcUnit, 0, 0, SRCCOPY);
	}	
	else
	{
		GetStrPriceByLineNo(nLine, strPrice);
		string sPriceLast = CConvert::GetStrPriceByPrice(m_MidPrice, m_CommkodityInfo.bIfFractionPrice, m_CommkodityInfo.iPrecision);
		FillRect(m_HdcUnit, &m_reUnit, hBKBrush);//背景
		SetBkMode(m_HdcUnit, TRANSPARENT);//设置字体背景 
		SetTextColor(m_HdcUnit, g_ColorRefData.GetColorLightBlue());// 字体
		if (m_nPos == nLine)
		{
			FillRect(m_HdcUnit, &m_reUnit, hFGBrush);//背景
			SetTextColor(m_HdcUnit, g_ColorRefData.GetColorWhite());// 字体白
		}
		if (sPriceLast == strPrice)
			SetTextColor(m_HdcUnit, RGB(250,250,0));// 字体红
		DrawTextUnicode(m_HdcUnit, strPrice.c_str(), &m_reUnit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		BitBlt(m_HdcMem, m_reClient.left, nLine*N_PRICE_HEIGHT, m_nUintx, m_nUinty, m_HdcUnit, 0, 0, SRCCOPY);
	}
	return;
}