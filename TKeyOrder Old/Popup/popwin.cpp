#include "PreInclude.h"

//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CPopWin::CPopWin() 
:m_opr(NULL)
,m_nPos(0)
, m_nWidth(0)
, m_nHeight(0)
, m_nUintx(N_UNIT_WIDTH)
, m_nUinty(N_UNIT_HEIGHT)
{

}

CPopWin::~CPopWin()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWin::CreatePopWin(IOperator* opr, LPCTSTR classname, HWND parent, DWORD style, vector<wstring>& vString, DWORD exstyle, int nleft, int ntop, UINT ncol, UINT nrow)
{
	m_nCol = ncol;
	m_nRow = nrow;
	m_nLeft = nleft;
	m_nTop = ntop;
	m_opr = opr;
	UpdateData(vString);
	CreateFrm(classname, parent, style | WS_POPUP | WS_BORDER, exstyle);
}

bool CPopWin::GetCurOption(wstring & str)
{
	if (m_vstring.size()>m_nPos)
	{
		str = m_vstring[m_nPos];
		return true;
	}
	return false;
}

bool CPopWin::UpdateData(vector<wstring>& vString)
{
	m_vstring.clear();
	m_vstring.insert(m_vstring.end(), vString.begin(), vString.end());
	return true;
}

LRESULT CPopWin::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	RECT		reFresh;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);
		 
			//////////////////////////////////////////////////////////////////////////
			m_HdcMem = CreateCompatibleDC(hdc);
			m_HdcUnit = CreateCompatibleDC(hdc);
			//////////////////////////////////////////////////////////////////////////
			ReleaseDC(m_Hwnd, hdc);

			m_nUintx = N_UNIT_WIDTH;
			m_nUinty = N_UNIT_HEIGHT;
			m_reUnit.left = 0;
			m_reUnit.right = m_nUintx;
			m_reUnit.top = 0;
			m_reUnit.bottom = m_nUinty;

			m_nWidth = m_nCol*N_UNIT_WIDTH + 2;
			m_nHeight = m_nRow*N_UNIT_HEIGHT+2;

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
		if (m_hFGBitmap)
			DeleteObject(m_hFGBitmap);

		m_hBKBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
		SelectObject(m_HdcMem, m_hBKBitmap);
		m_hFGBitmap = CreateCompatibleBitmap(hdc, m_nUintx, m_nUinty);
		SelectObject(m_HdcUnit, m_hFGBitmap);

		ReleaseDC(m_Hwnd, hdc);
		//////////////////////////////////////////////////////////////////////////
		break;
	case WM_PAINT:
	{
		HBRUSH	hBKBrush = CreateSolidBrush(g_ColorRefData.GetColorWhite());	//背景画刷
		HBRUSH  hFGBrush = CreateSolidBrush(g_ColorRefData.GetColorBlue());	//前景画刷 单元
		
		hdc = BeginPaint(m_Hwnd, &ps);

		SelectObject(m_HdcUnit, g_FontData.GetFontWord13());

		FillRect(m_HdcMem, &m_reClient, hBKBrush);
		for (int j = 0; j < m_nHeight / m_nUinty; j++)//遍历刷新单元格
			for (int i = 0; i < m_nWidth / m_nUintx; i++)
			{
				reFresh.left = i*m_nUintx;
				reFresh.right = i*m_nUintx + m_nUintx;
				reFresh.top = j*m_nUinty;
				reFresh.bottom = j*m_nUinty + m_nUinty;
				if (m_nPos == i + j*(m_nWidth / m_nUintx))
				{
					FillRect(m_HdcUnit, &m_reUnit, hFGBrush);//背景蓝
					SetTextColor(m_HdcUnit, g_ColorRefData.GetColorWhite());// 字体白
				}
				else
				{
					FillRect(m_HdcUnit, &m_reUnit, hBKBrush);//背景白
					SetTextColor(m_HdcUnit, g_ColorRefData.GetColorBlue());// 字体蓝
				}
				SetBkMode(m_HdcUnit, TRANSPARENT);//设置字体背景 

				if (int(m_vstring.size()) > i + j*(m_nWidth / m_nUintx))
				{
					DrawText(m_HdcUnit, m_vstring[i + j*(m_nWidth / m_nUintx)].c_str(), m_vstring[i + j*(m_nWidth / m_nUintx)].size(), &m_reUnit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					BitBlt(m_HdcMem, reFresh.left, reFresh.top, m_nUintx, m_nUinty, m_HdcUnit, 0, 0, SRCCOPY);
				}
			}
		BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

	

		EndPaint(m_Hwnd, &ps);

		DeleteObject(hFGBrush);
		DeleteObject(hBKBrush);
		
	}	
		break;
	case WM_MOUSEMOVE:
		POINT pointM;
		pointM.x = LOWORD(lParam);
		pointM.y = HIWORD(lParam);
		if (pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx)<int(m_vstring.size())&&pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx) != m_nPos)
		{
			m_nPos = pointM.x / m_nUintx + pointM.y / m_nUinty*(m_nWidth / m_nUintx);
			InvalidateRect(m_Hwnd, &m_reClient, TRUE);
		}
		break;
	case WM_LBUTTONDOWN:
		POINT pointB;
		pointB.x = LOWORD(lParam);
		pointB.y = HIWORD(lParam);
		m_nPos = pointB.x / m_nUintx + pointB.y / m_nUinty*(m_nWidth / m_nUintx);
		if (m_nPos < m_vstring.size())
		{
			ShowWindow(m_Hwnd, SW_HIDE);
			if (m_opr)
			{
				wstring str;
				if (GetCurOption(str))
					m_opr->SetPriceType(str);
			}
		}
		break;
	case WM_DESTROY:
		DeleteObject(m_hBKBitmap);
		DeleteObject(m_hFGBitmap);
		DeleteDC(m_HdcMem);
		DeleteDC(m_HdcUnit);
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