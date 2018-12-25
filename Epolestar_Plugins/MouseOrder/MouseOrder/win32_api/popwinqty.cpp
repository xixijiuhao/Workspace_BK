#include "PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;


CPopWinQty::CPopWinQty() :m_opr(NULL),m_nPos(0), m_nWidth(0), m_nHeight(0)
{

}
CPopWinQty::~CPopWinQty()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWinQty::CreatePopWin(IOperator *opr,LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle, int nleft, int ntop, UINT ncol, UINT nrow)
{
	m_nCol = ncol;
	m_nRow = nrow;
	m_nLeft = nleft;
	m_nTop = ntop;
	m_opr = opr;
	m_vstring.push_back("1");
	m_vstring.push_back("2");
	m_vstring.push_back("3");
	m_vstring.push_back("5");
	m_vstring.push_back("10");
	m_vstring.push_back("20");
	m_vstring.push_back("50");
	m_vstring.push_back("100");
	m_vstring.push_back("set");
	CreateFrm(classname, parent, style | WS_CHILD | WS_OVERLAPPED, exstyle);
}

LRESULT CPopWinQty::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	RECT		reFresh;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);	
			m_HdcMem = CreateCompatibleDC(hdc);
			ReleaseDC(m_Hwnd, hdc);

			m_nWidth = m_nCol*NQ_UNIT_WIDTH;
			m_nHeight = m_nRow*NQ_UNIT_HEIGHT;

			SetWindowPos(m_Hwnd, NULL, m_nLeft, m_nTop, m_nWidth, m_nHeight, SWP_NOACTIVATE);
		}
		break;
	case WM_SIZE:
		GetClientRect(m_Hwnd, &m_reClient);
		m_nWidth = m_reClient.right - m_reClient.left;
		m_nHeight = m_reClient.bottom - m_reClient.top;

		hdc = GetDC(m_Hwnd);

		if (m_hBKBitmap)
			DeleteObject(m_hBKBitmap);
		m_hBKBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
		SelectObject(m_HdcMem, m_hBKBitmap);
	
		ReleaseDC(m_Hwnd, hdc);
		break;
	case WM_PAINT:
	{	
		hdc = BeginPaint(m_Hwnd, &ps);
	
		FillRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrWhite());
		FrameRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrRGB200());
		for (int j = 0; j < m_nHeight / NQ_UNIT_HEIGHT; j++)//遍历刷新单元格
			for (int i = 0; i < m_nWidth / NQ_UNIT_WIDTH; i++)
			{
				reFresh.left = i*NQ_UNIT_WIDTH + NQ_BORDER;
				reFresh.right = (i + 1)*NQ_UNIT_WIDTH - NQ_BORDER;
				reFresh.top = j*NQ_UNIT_HEIGHT + NQ_BORDER;
				reFresh.bottom = (j + 1)*NQ_UNIT_HEIGHT - NQ_BORDER;
				if (m_nPos == i + j*(m_nWidth / NQ_UNIT_WIDTH))
				{
					FillRect(m_HdcMem, &reFresh, g_ColorRefData.GetHbrBlue());//背景蓝
					SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());// 字体白
				}
				else
				{
					FillRect(m_HdcMem, &reFresh, g_ColorRefData.GetHbrWhite());//背景白
					SetTextColor(m_HdcMem, g_ColorRefData.GetColorBlue());// 字体蓝
				}
				SetBkMode(m_HdcMem, TRANSPARENT);//设置字体背景 
				SelectObject(m_HdcMem, g_FontData.GetFontNum15());
				if (int(m_vstring.size()) > i + j*(m_nWidth / NQ_UNIT_WIDTH))
					DrawTextUnicode(m_HdcMem, m_vstring[i + j*(m_nWidth / NQ_UNIT_WIDTH)].c_str(), &reFresh, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);
		
		EndPaint(m_Hwnd, &ps);
	}	
		break;
	case WM_MOUSEMOVE:
		POINT pointM;
		pointM.x = LOWORD(lParam);
		pointM.y = HIWORD(lParam);
		if (pointM.x / NQ_UNIT_WIDTH + pointM.y / NQ_UNIT_HEIGHT*(m_nWidth / NQ_UNIT_WIDTH)<int(m_vstring.size()) && pointM.x / NQ_UNIT_WIDTH + pointM.y / NQ_UNIT_HEIGHT*(m_nWidth / NQ_UNIT_WIDTH) != m_nPos)
		{
			m_nPos = pointM.x / NQ_UNIT_WIDTH + pointM.y / NQ_UNIT_HEIGHT*(m_nWidth / NQ_UNIT_WIDTH);
			InvalidateRect(m_Hwnd, &m_reClient, TRUE);
		}
		break;
	case WM_LBUTTONDOWN:
		POINT pointB;
		pointB.x = LOWORD(lParam);
		pointB.y = HIWORD(lParam);
		m_nPos = pointB.x / NQ_UNIT_WIDTH + pointB.y / NQ_UNIT_HEIGHT*(m_nWidth / NQ_UNIT_WIDTH);
		if (m_nPos < m_vstring.size())
		{
			ShowWindow(m_Hwnd, SW_HIDE);
			if (m_opr)
				m_opr->SetMaxMinQty(m_vstring[m_nPos].c_str());
		}
		break;
	case WM_DESTROY:
		DeleteDC(m_HdcMem);
		DeleteObject(m_hBKBitmap);
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

