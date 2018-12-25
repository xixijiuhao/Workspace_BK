#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT	g_FontData;


CPopWinDate::CPopWinDate() :m_opr(NULL),m_nPos(0), m_nRectSelect(0), m_nWidth(0), m_nHeight(0),m_hMonthCtrl(NULL)
{

}
CPopWinDate::~CPopWinDate()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWinDate::CreatePopWin(IOperator *opr, HWND parent, TMouValidStrW vType[10], int nSize, int nleft, int ntop)
{
	m_nLeft = nleft;
	m_nTop = ntop;
	m_opr = opr;
	m_vstring.clear();
	for (int i = 0; i < nSize; i++)
		m_vstring.push_back(vType[i]);
	CreateFrm(L"PopWinDate", parent, WS_POPUP | WS_OVERLAPPED, 0);
}

LRESULT CPopWinDate::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	RECT		reFresh;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		{
			//缓存 字体
			hdc = GetDC(m_Hwnd);		
			m_HdcMem = CreateCompatibleDC(hdc);
			ReleaseDC(m_Hwnd, hdc);

			m_nWidth = ND_UNIT_WIDTH;
			m_nHeight = ND_UNIT_HEIGHT*int(m_vstring.size());
			
			//月历
			m_hMonthCtrl = CreateWindowEx(0,
				MONTHCAL_CLASS,
				L"",
				 WS_CLIPCHILDREN|WS_CHILD | WS_VISIBLE | MCS_DAYSTATE,
				0, 0, 0, 0, // resize it later
				m_Hwnd,
				(HMENU)IDC_MONCTRL,
				GetModuleHandle(NULL),
				NULL);
			MonthCal_GetMinReqRect(m_hMonthCtrl, &m_rcMonth);
			SetWindowPos(m_hMonthCtrl, NULL, ND_UNIT_WIDTH + 1, 1, m_rcMonth.right, m_rcMonth.bottom, NULL);
			ShowWindow(m_hMonthCtrl, SW_HIDE);
			MonthCal_SetCurrentView(m_hMonthCtrl, MCMV_MONTH);

			MoveWindow(m_Hwnd, m_nLeft, m_nTop, m_nWidth, m_nHeight, TRUE);
		}
		break;
	case WM_SIZE:
		{
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
		}
		
	case WM_PAINT:
		{
			hdc = BeginPaint(m_Hwnd, &ps);

			FillRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrWhite());
			FrameRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrRGB200());
			for (int j = 0; j < m_nHeight / ND_UNIT_HEIGHT; j++)//遍历刷新单元格
			{
				if (j >= (int)m_vstring.size())
					break;
				reFresh.left = ND_BORDER;
				reFresh.right = ND_UNIT_WIDTH - ND_BORDER;
				reFresh.top = j*ND_UNIT_HEIGHT + ND_BORDER;
				reFresh.bottom = (j + 1)*ND_UNIT_HEIGHT - ND_BORDER;
				if (m_nPos == j)
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
				SelectObject(m_HdcMem, g_FontData.GetFontWord13());
				if (int(m_vstring.size()) > j)
					DrawText(m_HdcMem, m_vstring[j].c_str(), m_vstring[j].size(), &reFresh, DT_CENTER | DT_VCENTER | DT_SINGLELINE);				
			}
			BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

			EndPaint(m_Hwnd, &ps);	
		}
		break;
	case WM_MOUSEMOVE:
		{
			POINT pointM;
			pointM.x = LOWORD(lParam);
			pointM.y = HIWORD(lParam);
			if (pointM.x < ND_UNIT_WIDTH && pointM.y / ND_UNIT_HEIGHT != m_nPos)
			{
				m_nPos = pointM.y / ND_UNIT_HEIGHT;
				InvalidateRect(m_Hwnd, &m_reClient, FALSE);
			}
			break;
		}	
	case WM_LBUTTONDOWN:
		{
			POINT pointB;
			pointB.x = LOWORD(lParam);
			pointB.y = HIWORD(lParam);
			m_nRectSelect = pointB.y / ND_UNIT_HEIGHT;
			if (pointB.x<ND_UNIT_WIDTH&&m_nRectSelect < m_vstring.size())
			{
				if (m_vstring[m_nRectSelect] == LoadResWchar_t(IDS_EFFECTIVE_DEADLINE))
				{
					ShowWindow(m_hMonthCtrl, SW_SHOW);
					if (m_nWidth == ND_UNIT_WIDTH)
					{
						m_nWidth = m_nWidth + m_rcMonth.right + ND_BORDER;
						if (m_nHeight < m_rcMonth.bottom + ND_BORDER)
							m_nHeight = m_rcMonth.bottom + ND_BORDER;
						RECT rect2;
						GetWindowRect(m_Hwnd, &rect2);
						
						rect2.left = rect2.right - m_nWidth;
						SetWindowPos(m_Hwnd, NULL, rect2.left, rect2.top, m_nWidth, m_nHeight, NULL);
					}				
				}
				else
				{
					ShowWindow(m_hMonthCtrl, SW_HIDE);
					if (m_nWidth != ND_UNIT_WIDTH)
					{
						m_nWidth = ND_UNIT_WIDTH;
						m_nHeight = ND_UNIT_HEIGHT*int(m_vstring.size());
						SetWindowPos(m_Hwnd, NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE);
					}
					ShowWindow(m_Hwnd, SW_HIDE);
					if (m_opr)
					{
						m_opr->SetValidType(m_vstring[m_nRectSelect].c_str());
						m_nPos = -1;
						return true;
					}
				}
			}
			break;
		}
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
	case WM_NOTIFY:
		switch ((wParam))
		{
		case IDC_MONCTRL:
			NMHDR *p = (NMHDR*)lParam;
			if (p->code==MCN_SELECT)
			{
				SYSTEMTIME lpSysTime;
				MonthCal_GetCurSel(m_hMonthCtrl, &lpSysTime);
				wchar_t sTime[20] = {0};
				wsprintf(sTime, TEXT("%4d-%02d-%02d"), lpSysTime.wYear, lpSysTime.wMonth, lpSysTime.wDay);
				if (m_opr)
					m_opr->SetValidType(sTime);
				ShowWindow(m_hMonthCtrl,SW_HIDE);	//隐藏月历
				if (m_nWidth != ND_UNIT_WIDTH)
				{
					m_nWidth = ND_UNIT_WIDTH;
					m_nHeight = ND_UNIT_HEIGHT*int(m_vstring.size());
					SetWindowPos(m_Hwnd, NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE);
				}
				ShowWindow(m_Hwnd, SW_HIDE);		//隐藏有效类型窗口
			}
			break;
		}
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}