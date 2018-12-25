#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

#define N_WIN_HEIGHT	28

CPopWinList::CPopWinList() 
: m_nPos(-1)
, m_nWidth(0)
, m_nHeight(0)
, m_bHover(false)
, m_bTracking(false)
, m_pOper(NULL)
{

}

CPopWinList::~CPopWinList()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWinList::CreatePopWin(IOperator *p, HWND parent,int nwidth, int nleft, int ntop, int nRef, TMouPriceTypeStrW sMem[10], int nSize, int nRow, int nCol, bool bCombox)
{
	if (!(nwidth > 0 && nSize > 0 && nRow > 0 && nCol > 0))
		return;
	m_pOper = p;
	m_bCombox = bCombox;
	m_nLeft = nleft;
	m_nTop = ntop;
	m_nRef = nRef;
	m_nUnit = nwidth;
	m_nRow = nRow;
	m_nCol = nCol;
	m_nWidth = nwidth*nCol;
	m_nHeight = nRow * N_WIN_HEIGHT;
	UpdateData(sMem,nSize);
	CreateFrm(L"Winlist", parent, WS_POPUP | WS_CLIPCHILDREN, 0);
}
void CPopWinList::UpdateUnit(int nWidth)
{
	m_nUnit = nWidth;
	m_nWidth = m_nUnit*m_nCol;
	SetWindowPos(m_Hwnd, NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE|SWP_NOZORDER);
}
void CPopWinList::UpdateData(TMouPriceTypeStrW sMem[10], int nSize)
{
	m_vstring.clear();
	for (int i = 0; i < nSize;i++)
		m_vstring.push_back(sMem[i]);
}
void CPopWinList::DrawList()
{
	SelectObject(m_HdcMem, g_FontData.GetFontWord13());

	RECT		reFresh;
	for (int j = 0; j < m_nRow; j++)//遍历刷新单元格
		for (int i = 0; i < m_nCol; i++)
		{
			int nCurId = j*m_nCol + i;
			if (nCurId > int(m_vstring.size()))
				break;
			reFresh.left = i*m_nUnit + 2;
			reFresh.right = (i + 1)*m_nUnit - 2;
			reFresh.top = j*N_WIN_HEIGHT + 2;
			reFresh.bottom = (j + 1)*N_WIN_HEIGHT - 2;
			if (m_nPos != nCurId)
			{
				FillRect(m_HdcMem, &reFresh, g_ColorRefData.GetHbrWhite());//背景白
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorLightBlue());// 字体白
			}
			else
			{
				FillRect(m_HdcMem, &reFresh, g_ColorRefData.GetHbrBackground());//背景
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlue());// 字体
			}
			SetBkMode(m_HdcMem, TRANSPARENT);//设置字体背景 

			if (int(m_vstring.size()) > nCurId)
				DrawText(m_HdcMem, m_vstring[nCurId].c_str(), m_vstring[nCurId].size(), &reFresh, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
}
void CPopWinList::DrawCombox()
{
	SelectObject(m_HdcMem, g_FontData.GetFontWordEx15());

	RECT		reFresh;
	for (int j = 0; j < m_nRow; j++)//遍历刷新单元格
		for (int i = 0; i < m_nCol; i++)
		{
			int nCurId = j*m_nCol + i;
			if (nCurId > int(m_vstring.size()))
				break;
			reFresh.left = i*m_nUnit + 2;
			reFresh.right = (i + 1)*m_nUnit - 2;
			reFresh.top = j*N_WIN_HEIGHT + 2;
			reFresh.bottom = (j + 1)*N_WIN_HEIGHT - 2;
			if (m_nPos != nCurId)
			{
				FillRect(m_HdcMem, &reFresh, g_ColorRefData.GetHbrWhite());//背景白
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlack());// 字体白
			}
			else
			{
				FillRect(m_HdcMem, &reFresh, g_ColorRefData.GetHbrBlue());//背景
				SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());// 字体
			}
			SetBkMode(m_HdcMem, TRANSPARENT);//设置字体背景 

			if (int(m_vstring.size()) > nCurId)
				DrawText(m_HdcMem, m_vstring[nCurId].c_str(), m_vstring[nCurId].size(), &reFresh, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
}
LRESULT CPopWinList::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;
	int			npos = 0;
	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);
			m_HdcMem = CreateCompatibleDC(hdc);		
			ReleaseDC(m_Hwnd, hdc);
			MoveWindow(m_Hwnd, m_nLeft, m_nTop, m_nWidth, m_nHeight, TRUE);
		}
		break;
	case WM_SIZE:
		GetClientRect(m_Hwnd, &m_reClient);
		m_nWidth = m_reClient.right - m_reClient.left;
		m_nHeight = m_reClient.bottom - m_reClient.top;
		//////////////////////////////////////////////////////////////////////////
		hdc = GetDC(m_Hwnd);

		if (m_hBKBitmap)
			DeleteObject(m_hBKBitmap);
		
		m_hBKBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
		SelectObject(m_HdcMem, m_hBKBitmap);
		
		ReleaseDC(m_Hwnd, hdc);
		//////////////////////////////////////////////////////////////////////////
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(m_Hwnd, &ps);

		FillRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrWhite());
		FrameRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrRGB200());
		
		if (m_bCombox)
			DrawCombox();
		else
			DrawList();
		
		BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

		EndPaint(m_Hwnd, &ps);
	}	
		break;
	case WM_MOUSEMOVE:
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_Hwnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 50;
			m_bTracking = _TrackMouseEvent(&tme) == TRUE;
		}
		POINT pointM;
		pointM.x = LOWORD(lParam);
		pointM.y = HIWORD(lParam);
		npos = pointM.y / N_WIN_HEIGHT*m_nCol + pointM.x / m_nUnit;
		if (npos<int(m_vstring.size()) && npos != m_nPos)
		{
			m_nPos = npos;
			InvalidateRect(m_Hwnd, &m_reClient, TRUE);
		}
		break;
	case WM_MOUSELEAVE:
		m_bTracking = false;
		m_bHover = FALSE;
		m_nPos = -1;
		InvalidateRect(m_Hwnd, NULL, TRUE);
		ShowWindow(m_Hwnd, SW_HIDE);	
		break;
	case WM_MOUSEHOVER:
		m_bHover = TRUE;
		InvalidateRect(m_Hwnd, NULL, TRUE);
		break;
	case WM_LBUTTONDOWN:
		POINT pointB;
		pointB.x = LOWORD(lParam);
		pointB.y = HIWORD(lParam);
		m_nPos = pointB.y / N_WIN_HEIGHT*m_nCol + pointB.x / m_nUnit;
		if (m_nPos>=0 && m_nPos < m_vstring.size())
		{
			ShowWindow(m_Hwnd, SW_HIDE);
			m_pOper->SetSelect(m_vstring[m_nPos].c_str(), m_nRef);
		}
		break;
	case WM_DESTROY:
		DeleteObject(m_hBKBitmap);
		DeleteDC(m_HdcMem);
		break;
	case WM_ACTIVATE:
		if (WA_INACTIVE == wParam)
			ShowWindow(m_Hwnd, SW_HIDE);
		break;
	case WM_ERASEBKGND:
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}