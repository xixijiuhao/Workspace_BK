#include "popwindate.h"
#include "commctrl.h"
#include "AnsicUnicode.h"
#include "load_rc.h"
#define COLOR_FG RGB(0, 94, 167)	//前景色
#define COLOR_BG RGB(250, 250, 250)	//背景色
CPopWinDate::CPopWinDate() :m_opr(NULL),m_nPos(0), m_nRectSelect(0), m_nWidth(0), m_nHeight(0), m_nUintx(ND_UNIT_WIDTH), m_nUinty(ND_UNIT_HEIGHT), m_hMonthCtrl(NULL)
{

}
CPopWinDate::~CPopWinDate()
{

}

void CPopWinDate::CreatePopWin(IOperator *opr,LPCTSTR classname, HWND parent, DWORD style, vector<wstring>& vString, DWORD exstyle, int nleft, int ntop, UINT ncol, UINT nrow)
{
	m_nCol = ncol;
	m_nRow = nrow;
	m_nLeft = nleft;
	m_nTop = ntop;
	m_opr = opr;
	UpdateData(vString);
	CreateFrm(classname, parent, style | WS_POPUP | WS_OVERLAPPED | WS_BORDER, exstyle);
}

bool CPopWinDate::GetCurOption(wstring & str)
{
	if (m_vstring.size()>m_nRectSelect)
	{
		str = m_vstring[m_nRectSelect];
		return true;
	}
	return false;
}
string CPopWinDate::GetTime()
{
	return LoadRC::unicode_to_ansi(m_sTime);
}
bool CPopWinDate::UpdateData(vector<wstring>& vString)
{
	m_vstring.clear();
	m_vstring.insert(m_vstring.end(), vString.begin(), vString.end());
	return true;
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

			m_hFont = CreateFont(-(int)(fabs(double(90 * GetDeviceCaps(hdc, LOGPIXELSY) / 72) / 10.0 + 0.5)),
				0, 0, 0, 500, FALSE, FALSE, FALSE, 0, 0, 0, 0, 0, TEXT("MS Shell Dlg")); //建立字体  
			m_HdcMem = CreateCompatibleDC(hdc);
			m_HdcUnit = CreateCompatibleDC(hdc);
			
			ReleaseDC(m_Hwnd, hdc);
			
			//颜色 画刷
			m_ColorFG = COLOR_FG;
			m_ColorBK = COLOR_BG;
			m_hFGBrush = CreateSolidBrush(m_ColorFG);
			m_hBKBrush = CreateSolidBrush(m_ColorBK);

			//规格
			m_nUintx = ND_UNIT_WIDTH;
			m_nUinty = ND_UNIT_HEIGHT;
			m_reUnit.left = 0;
			m_reUnit.right = m_nUintx;
			m_reUnit.top = 0;
			m_reUnit.bottom = m_nUinty;

			m_nWidth = m_nCol*ND_UNIT_WIDTH+2;
			m_nHeight = ND_UNIT_HEIGHT;
			
			//月历
			RECT rc;
			m_hMonthCtrl = CreateWindowEx(0,
				MONTHCAL_CLASS,
				TEXT(""),
				 WS_CLIPCHILDREN|WS_CHILD | WS_VISIBLE | MCS_DAYSTATE,
				0, 0, 0, 0, // resize it later
				m_Hwnd,
				(HMENU)IDC_MONCTRL,
				GetModuleHandle(NULL),
				NULL);
			MonthCal_GetMinReqRect(m_hMonthCtrl, &rc);
			SetWindowPos(m_hMonthCtrl, NULL, ND_UNIT_WIDTH+1, m_nHeight+1, rc.right, rc.bottom, NULL);
			ShowWindow(m_hMonthCtrl, SW_HIDE);
			MonthCal_SetCurrentView(m_hMonthCtrl, MCMV_MONTH);

			//尺度变动
			int nwidth = ND_UNIT_WIDTH + rc.right - rc.left + 2;
			m_nWidth = m_nWidth > nwidth ? m_nWidth : nwidth;
			m_nHeight += rc.bottom+2;
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
			if (m_hFGBitmap)
				DeleteObject(m_hFGBitmap);

			m_hBKBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
			SelectObject(m_HdcMem, m_hBKBitmap);
			m_hFGBitmap = CreateCompatibleBitmap(hdc, m_nUintx, m_nUinty);
			SelectObject(m_HdcUnit, m_hFGBitmap);

			ReleaseDC(m_Hwnd, hdc);
			//////////////////////////////////////////////////////////////////////////
			break;
		}
		
	case WM_PAINT:
		{
			hdc = BeginPaint(m_Hwnd, &ps);

			FillRect(m_HdcMem, &m_reClient, m_hBKBrush);
			for (int j = 0; j < m_nHeight / m_nUinty; j++)//遍历刷新单元格
			{
				for (int i = 0; i < m_nWidth / m_nUintx; i++)
				{
					if (j*m_nCol + i >= m_vstring.size())
						break;
					reFresh.left = i*m_nUintx;
					reFresh.right = i*m_nUintx + m_nUintx;
					reFresh.top = j*m_nUinty;
					reFresh.bottom = j*m_nUinty + m_nUinty;
					if (m_nPos == i + j*(m_nWidth / m_nUintx))
					{
						FillRect(m_HdcUnit, &m_reUnit, m_hFGBrush);//背景蓝
						SetTextColor(m_HdcUnit, m_ColorBK);// 字体白
					}
					else
					{
						FillRect(m_HdcUnit, &m_reUnit, m_hBKBrush);//背景白
						SetTextColor(m_HdcUnit, m_ColorFG);// 字体蓝
					}
					SetBkMode(m_HdcUnit, TRANSPARENT);//设置字体背景 
					SelectObject(m_HdcUnit, m_hFont);
					if (int(m_vstring.size()) > i + j*(m_nWidth / m_nUintx))
					{
						DrawText(m_HdcUnit, m_vstring[i + j*(m_nWidth / m_nUintx)].c_str(), m_vstring[i + j*(m_nWidth / m_nUintx)].size(), & m_reUnit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						BitBlt(m_HdcMem, reFresh.left, reFresh.top, m_nUintx, m_nUinty, m_HdcUnit, 0, 0, SRCCOPY);
					}
				}
			}
			BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

			EndPaint(m_Hwnd, &ps);
			break;
		}
	case WM_MOUSEMOVE:
		{
			POINT pointM;
			pointM.x = LOWORD(lParam);
			pointM.y = HIWORD(lParam);
			if (pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx) < int(m_vstring.size()) && pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx) != m_nPos)
			{
				m_nPos = pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx);
				InvalidateRect(m_Hwnd, &m_reClient, FALSE);
			}
			break;
		}
		
	case WM_LBUTTONDOWN:
		{
			POINT pointB;
			pointB.x = LOWORD(lParam);
			pointB.y = HIWORD(lParam);
			m_nRectSelect = pointB.x / m_nUintx + pointB.y / m_nUinty*(m_nWidth / m_nUintx);
			if (m_nRectSelect < m_vstring.size())
			{
				if (m_nRectSelect + 1 == m_vstring.size())
					ShowWindow(m_hMonthCtrl, SW_SHOW);
				else
				{
					ShowWindow(m_hMonthCtrl, SW_HIDE);
					ShowWindow(m_Hwnd, SW_HIDE);
					if (m_opr)
					{
						wstring str;
						if (GetCurOption(str))
							m_opr->SetValidType(str);
					}
				}
			}
			break;
		}
	case WM_DESTROY:
		DeleteDC(m_HdcMem);
		DeleteDC(m_HdcUnit);
		DeleteObject(m_hBKBitmap);
		DeleteObject(m_hFGBitmap);
		DeleteObject(m_hFGBrush);
		DeleteObject(m_hBKBrush);
		DeleteObject(m_hFont);
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
				wchar_t sTime[20];
				wsprintf(sTime, TEXT("%4d-%02d-%02d"), lpSysTime.wYear, lpSysTime.wMonth, lpSysTime.wDay);
				m_sTime = sTime;
				if (m_opr)
					m_opr->SetValidType(m_sTime);
				ShowWindow(m_hMonthCtrl,SW_HIDE);	//隐藏月历
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