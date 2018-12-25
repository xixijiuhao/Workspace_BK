#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CPopWinSearch::CPopWinSearch() 
: m_nWidth(0)
, m_nHeight(0)
, m_bHover(false)
, m_bTracking(false)
, m_pOper(NULL)
, m_bShowStrategy(true)
, m_bFirst(true)
, m_bShowLimitStopInner(true)
{

}

CPopWinSearch::~CPopWinSearch()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWinSearch::CreatePopWin(CMousewin *p, HWND parent, TMouPriceTypeStrW sMem[CSP_END][N_MAX_CHILD], int nSize[CSP_END],bool bShowLimitStopInner)
{
	m_pOper = p;
	m_nUnit = 100;
	m_nWidth = 240;
	m_nHeight = 200;
	memcpy(m_string, sMem, sizeof(m_string));
	memcpy(m_nSize, nSize,sizeof(m_nSize));
	m_bShowLimitStopInner = bShowLimitStopInner;
	CreateFrm(L"Winlist", parent, WS_POPUP | WS_CLIPCHILDREN, 0);
}
void CPopWinSearch::UpdateLayout(int nType, bool bShowStrategy)
{
	if (IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_HIDE);

	int bOldLoginType = m_LoginType;
	bool bOldShowStrategy = m_bShowStrategy;
	
	m_LoginType = nType;// nType;
	m_bShowStrategy = bShowStrategy;
	int nCol = 0;
	int nRowMax = 0;	
	int nSize = m_LoginType == TYPE_Both ? 4 : 3;
	for (int n=0;n<nSize;n++)
	{
		int i = CPS_Normal;
		switch (nCol)
		{
		case CPS_Normal:
			break;
		case CPS_Advanced_Inner:
			if (m_LoginType == TYPE_For)
				i = CPS_Advanced_For;
			else
				i = CPS_Advanced_Inner;		
			break;
		case CPS_Advanced_For:
			if (m_LoginType==TYPE_Both)
				i = CPS_Advanced_For;
			else if (!m_bShowStrategy)
				continue;
			else
				i = CPS_Strategy;
			break;
		case CPS_Strategy:
			if (!m_bShowStrategy)
				continue;
			else
				i = CPS_Strategy;
			break;
		}
		m_nHead[nCol] = i;
		for (int j = 0; j < m_nSize[i]; j++)
		{
			m_ItemRect[nCol][j].left = nCol*m_nUnit + 1;
			m_ItemRect[nCol][j].right = (nCol + 1)*m_nUnit;
			m_ItemRect[nCol][j].top = j * 20 + 1;
			m_ItemRect[nCol][j].bottom = (j + 1) * 20;
		}
		nRowMax = max(nRowMax, m_nSize[i]);
		nCol++;
	}
	m_nCol = nCol;
	m_nRow = nRowMax;
	m_nWidth = m_nCol*m_nUnit+1;
	m_nHeight = 20 * nRowMax+5;

	UpdateSelectItems(bOldLoginType, bOldShowStrategy);
	
	int nLeft = 0,nTop = 0;
	m_pOper->GetPopTopLeftAxis(m_nWidth, nLeft, nTop);
	SetWindowPos(m_Hwnd, NULL, nLeft, nTop, m_nWidth, m_nHeight, SWP_NOZORDER);
		
	if (m_bFirst)
		m_bFirst = false;
}
void CPopWinSearch::UpdateData(int nSize, TMouPriceTypeStrW str[N_MAX_CHILD])
{
	m_SelectedItem.clear();
	for (int i = 0;i<nSize;i++)
		m_SelectedItem.insert(str[i]);
}
LRESULT CPopWinSearch::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		SelectObject(m_HdcMem, g_FontData.GetFontWord13());

		ReleaseDC(m_Hwnd, hdc);
		//////////////////////////////////////////////////////////////////////////
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(m_Hwnd, &ps);
		
		FillRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrWhite());

		FrameRect(m_HdcMem, &m_reClient, g_ColorRefData.GetHbrRGB150());

		SelectObject(m_HdcMem, g_FontData.GetFontWord13());
		SetBkMode((HDC)m_HdcMem, TRANSPARENT);
		for (int i=0;i<m_nCol;i++)
		{
			int nindex = m_nHead[i];
			for (int j=0;j<m_nSize[nindex];j++)
			{			
				if (j == 0)
				{
					RECT rect(m_ItemRect[i][j]);
					rect.left = rect.left + 25;
					SetTextColor(m_HdcMem, g_ColorRefData.GetColorLightBlue());
					DrawTextW(m_HdcMem, m_string[nindex][j], wcslen(m_string[nindex][j]), &rect, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
				}
				else
				{
					set<wstring>::iterator it = m_SelectedItem.find(m_string[nindex][j]);
					bool bHover = m_CurItem.nx == i && m_CurItem.ny == j && m_CurItem.bInCheck;
					if (it!= m_SelectedItem.end())
						DrawFix(m_ItemRect[i][j],true, bHover);
					else
						DrawFix(m_ItemRect[i][j],false, bHover);
					RECT rect(m_ItemRect[i][j]);
					rect.left = rect.left + 20;
					if (m_CurItem.nx == i && m_CurItem.ny == j && !m_CurItem.bInCheck)
					{
						FillRect(m_HdcMem, &rect, g_ColorRefData.GetHbrLightBlue());//背景
						SetTextColor(m_HdcMem, g_ColorRefData.GetColorWhite());// 字体
					}
					else
					{
						SetTextColor(m_HdcMem, g_ColorRefData.GetColorGray());
						FillRect(m_HdcMem, &rect, g_ColorRefData.GetHbrWhite());//背景白
					}
					rect.left = rect.left + 5;
					DrawTextW(m_HdcMem, m_string[nindex][j], wcslen(m_string[nindex][j]), &rect/*m_ItemRect[i][j]*/, DT_VCENTER | DT_LEFT | DT_SINGLELINE);					
				}
			}
		}

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
		{
			POINT pointC;
			pointC.x = LOWORD(lParam);
			pointC.y = HIWORD(lParam);
			bool bFind = false;
			RECTITEM itemOld = m_CurItem;
			for (int i = 0; i < m_nCol; i++)
			{
				int nindex = m_nHead[i];
				for (int j = 0; j < m_nSize[nindex]; j++)
				{
					if (PtInRect(&m_ItemRect[i][j], pointC))
					{
						m_CurItem.nx = i;
						m_CurItem.ny = j;
						if (IsInCheck(m_ItemRect[i][j], pointC))
							m_CurItem.bInCheck = true;
						else
							m_CurItem.bInCheck = false;
						bFind = true;
					}
				}
			}
			if (!bFind)
			{
				m_CurItem.nx = -1;
				m_CurItem.ny = -1;
			}
			if (itemOld.nx != m_CurItem.nx || m_CurItem.ny != itemOld.ny|| m_CurItem.bInCheck != itemOld.bInCheck)
			{
				if (itemOld.nx >= 0 && itemOld.ny >= 0)
					InvalidateRect(m_Hwnd, &m_ItemRect[itemOld.nx][itemOld.ny], TRUE);
				if (m_CurItem.nx >= 0 && m_CurItem.ny >= 0)
					InvalidateRect(m_Hwnd, &m_ItemRect[m_CurItem.nx][m_CurItem.ny], TRUE);
			}
		}
		break;
	case WM_MOUSELEAVE:
		m_bTracking = false;
		m_bHover = FALSE;
		m_CurItem.nx = -1;
		m_CurItem.ny = -1;
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
		for (int i = 0; i < m_nCol; i++)
		{
			int nindex = m_nHead[i];
			for (int j = 1; j < m_nSize[nindex]; j++)
			{
				if(PtInRect(&m_ItemRect[i][j], pointB))
				{
					if (IsInCheck(m_ItemRect[i][j], pointB))
					{
						UpdateSelectItem(m_string[nindex][j]);
					}
					else
					{
						m_pOper->SetHeadData(m_string[nindex][j], false);
						ShowWindow(m_Hwnd, SW_HIDE);
					}			
				}
			}
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
void CPopWinSearch::DrawFix(RECT rect, bool bCheck, bool bHover)
{
	RECT rcsel;
	rcsel.left = rect.left + 3;
	rcsel.top = rect.top + 3;
	rcsel.right = rcsel.left + m_nCheckWidth;
	rcsel.bottom = rcsel.top + m_nCheckWidth;

	if ((bHover)&&(bCheck || m_SelectedItem.size() < N_Search_Size))
		FillRect(m_HdcMem, &rect, g_ColorRefData.GetHbrBackground());
		
	if (!bCheck)
	{
		HPEN hpen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
		SelectObject(m_HdcMem, hpen);
		
		if (m_SelectedItem.size() < N_Search_Size)
		{
			int nx = rcsel.left + 2;
			int ny = rcsel.top + 7;
			MoveToEx(m_HdcMem, nx, ny, NULL);
			LineTo(m_HdcMem, nx + 5, ny);

			MoveToEx(m_HdcMem, nx + 4, ny - 4, NULL);
			LineTo(m_HdcMem, nx + 4, ny + 4);

			RECT re;
			re.left = nx + 5;
			re.right = re.left + 6;
			re.top = ny - 3;
			re.bottom = ny + 3;
			FillRect(m_HdcMem, &re, g_ColorRefData.GetHbrRGB150());
			InflateRect(&re, -1, -1);
			re.bottom = re.bottom - 1;
			FillRect(m_HdcMem, &re, g_ColorRefData.GetHbrWhite());
		}

		DeleteObject(hpen);
	}
	else
	{
		HPEN hpen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorLightBlue());
		SelectObject(m_HdcMem, hpen);
		int nx = rcsel.left + 7;
		int ny = rcsel.bottom - 2;
		MoveToEx(m_HdcMem, nx, ny, NULL);
		LineTo(m_HdcMem, nx, ny - 5);

		MoveToEx(m_HdcMem, nx - 4 , ny - 4, NULL);
		LineTo(m_HdcMem, nx + 4, ny - 4);

		RECT re;
		re.bottom = ny - 4;
		re.top = re.bottom - 6;
		re.left = nx - 3;
		re.right = nx + 3;
		FillRect(m_HdcMem, &re, g_ColorRefData.GetHbrLightBlue());
		InflateRect(&re, -1, -1);
		re.right = re.right - 1;
		FillRect(m_HdcMem, &re, g_ColorRefData.GetHbrWhite());
		DeleteObject(hpen);
	}

	
}
void CPopWinSearch::UpdateSelectItem(const wchar_t* str)
{
	set<wstring>::iterator it=m_SelectedItem.find(str);
	if (it != m_SelectedItem.end())
		m_SelectedItem.erase(it);	
	else if (m_SelectedItem.size() < N_Search_Size)
		m_SelectedItem.insert(str);
	else
		return;

	UpdateSelectItemToUI();
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);
}
bool CPopWinSearch::IsInCheck(const RECT& rect, const POINT& point)
{
	RECT re(rect);
	re.right = rect.left + 15;
	return PtInRect(&re, point)?true:false;
}
void CPopWinSearch::UpdateSelectItemToUI(bool bClearSpecial)
{
	TMouPriceTypeStrW str[10] = { 0 };
	int i = 0;
	for (set<wstring>::iterator it = m_SelectedItem.begin();it!= m_SelectedItem.end()&& i<N_Search_Size;it++)
		memcpy(str[i++], (*it).c_str(), sizeof(TMouPriceTypeStrW));
	
	if (m_pOper)
		m_pOper->UpdateSelectInfo(str,i, bClearSpecial);
}
void CPopWinSearch::UpdateSelectItems(int nOldLoginType,bool bOldShowStrategy)
{
	//////////////////////////////////////////////////////////////////////////

	bool bUpdate = false;
	if (m_bFirst || (m_LoginType == TYPE_For&& nOldLoginType == TYPE_Inner) || (m_LoginType == TYPE_Inner&& nOldLoginType == TYPE_For))
	{
		int nID = m_LoginType == TYPE_For ? CPS_Advanced_Inner : CPS_Advanced_For;
		for (int i = 0; i < m_nSize[nID]; i++)
		{
			set<wstring>::iterator it = m_SelectedItem.find(m_string[nID][i]);
			if (it != m_SelectedItem.end())
			{
				if (m_bShowLimitStopInner && (*it == LoadResWchar_t(IDS_LIMIT_STOP_PRICE) || *it == LoadResWchar_t(IDS_STOP_PRICE)))//内盘 止损显示特殊处理
					continue;
				bUpdate = true;
				m_SelectedItem.erase(it);
			}
		}
	}
	if ((m_bFirst || m_bShowStrategy != bOldShowStrategy) && m_bShowStrategy == false)
	{
		int nID = CPS_Strategy;
		for (int i = 0; i < m_nSize[nID]; i++)
		{
			set<wstring>::iterator it = m_SelectedItem.find(m_string[nID][i]);
			if (it != m_SelectedItem.end())			
				m_SelectedItem.erase(it);
		}
		bUpdate = true;
	}
	if (m_bFirst)
	{
		for (set<wstring>::iterator it = m_SelectedItem.begin(); it != m_SelectedItem.end();)
		{
			bool bFind = false;
			for (int n = 0; n < 4; n++)
			{
				for (int j = 0; j < m_nSize[n]; j++)
				{
					if (*it == m_string[n][j])
						bFind = true;
					if (bFind)
						break;
				}
				if (bFind)
					break;
			}
			if (!bFind)
			{
				bUpdate = true;
				it = m_SelectedItem.erase(it);
			}			
			else
				it++;
		}
	}
	if (bUpdate)
		UpdateSelectItemToUI(m_bShowStrategy != bOldShowStrategy && m_bShowStrategy == false);
	//////////////////////////////////////////////////////////////////////////
}