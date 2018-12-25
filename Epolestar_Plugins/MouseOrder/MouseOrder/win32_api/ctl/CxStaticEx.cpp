#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

#define NArrowWidth	7
#define NArrowOffset 5
void CxStaticEx::DrawTriangle2(HDC &hdc, int nStartx, int nStarty, int nWidth)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;
	HPEN   hPen = CreatePen(PS_SOLID, 1, IsWindowEnabled(m_hWnd) ? g_ColorRefData.GetColorLightBlue() : g_ColorRefData.GetColorBlack());
	SelectObject(hdc, hPen);

	for (int i = 0; i < nWidth / 2 + 1; i++)
	{
		MoveToEx(hdc, nStartx + i, nStarty + i, NULL);
		LineTo(hdc, nStartx + nWidth - i, nStarty + i);
	}

	DeleteObject(hPen);
}
CxStaticEx::CxStaticEx()
{
	m_oper = 0;
	m_nType = INNER;
	m_bHover = false;
	m_bTracking = false;
	memset(m_strText, 0, sizeof(m_strText));
	memset(m_stype, 0, sizeof(m_stype));
	memset(m_nSize, 0, sizeof(m_nSize));
	memset(m_nRow, 1, sizeof(m_nRow));
	memset(m_nCol, 1, sizeof(m_nCol));
}
CxStaticEx::~CxStaticEx()
{
	
}
void CxStaticEx::Init(IOperator *p, HWND hwnd, int nRef, TMouPriceTypeStrW stype[FOREIGN + 1][10], int nSize[FOREIGN + 1], int nRow[FOREIGN + 1], int nCol[FOREIGN + 1], PopType ntype, bool bNeedHover)
{
	m_oper = p;
	m_nType = ntype;
	m_bNeedHover = bNeedHover;
	SetWindowLong(GetHwnd(), GWL_STYLE, GetWindowLong(GetHwnd(),GWL_STYLE) | SS_OWNERDRAW);
	memcpy_s(m_nSize, sizeof(m_nSize), nSize, sizeof(m_nSize));
	memcpy_s(m_nRow, sizeof(m_nRow), nRow, sizeof(m_nRow));
	memcpy_s(m_nCol, sizeof(m_nCol), nCol, sizeof(m_nCol));
	memcpy_s(m_stype, sizeof(m_stype), stype, sizeof(m_stype));
	m_nRef = nRef;
	CxSubClassWnd::Init(hwnd);
}
void CxStaticEx::UpdatePopType(PopType ntype)
{
	m_nType = ntype;
	ShowWindow(m_hlist[INNER].GetHwnd(), SW_HIDE);
	ShowWindow(m_hlist[FOREIGN].GetHwnd(), SW_HIDE);
	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}
LRESULT CALLBACK CxStaticEx::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			OnPaint();
			return 1;
		case WM_MOUSEMOVE:
			if (!m_bTracking)
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = 50;
				m_bTracking = _TrackMouseEvent(&tme) == TRUE;
			}
			break;
		case WM_MOUSELEAVE:
			m_bTracking = false;
			m_bHover = FALSE;
			InvalidateRect(m_hWnd, NULL, TRUE);
			break;
		case WM_MOUSEHOVER:
			m_bHover = TRUE;
			InvalidateRect(m_hWnd, NULL, TRUE);		
			break;
		case WM_LBUTTONDOWN:
		{
			if (m_nSize[m_nType]==1)
				break;
			RECT rect;
			GetWindowRect(m_hWnd, &rect);
			if (!IsWindow(m_hlist[m_nType].GetHwnd()))
				m_hlist[m_nType].CreatePopWin(m_oper, GetParent(GetHwnd()), rect.right - rect.left, rect.left, rect.bottom, m_nRef, m_stype[m_nType], m_nSize[m_nType], m_nRow[m_nType], m_nCol[m_nType]);
			m_hlist[m_nType].UpdateUnit(rect.right - rect.left);
			if (m_nCol[m_nType] != 1)
			{
				RECT rect2;
				GetWindowRect(m_hlist[m_nType].GetHwnd(), &rect2);
				SetWindowPos(m_hlist[m_nType].GetHwnd(), NULL, (rect.right + rect.left)/2 - (rect2.right - rect2.left)/2, rect.bottom, 0, 0, SWP_NOSIZE);
			}
			else
				SetWindowPos(m_hlist[m_nType].GetHwnd(), NULL, rect.left, rect.bottom, 0, 0, SWP_NOSIZE);
			ShowWindow(m_hlist[m_nType].GetHwnd(), SW_SHOWNORMAL);	
		}
			break;
		case WM_ERASEBKGND:
			return true;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CxStaticEx::CxSetWindowText(const wchar_t* wstr)
{
	wcsncpy_s(m_strText, wstr, sizeof(m_strText) / sizeof(m_strText[0]));
	if (::IsWindow(m_hWnd))
		InvalidateRect(GetHwnd(),NULL,TRUE);
}
void CxStaticEx::CxGetWindowText(wchar_t* wstr,int nMaxLen)
{
	wcsncpy_s(wstr, nMaxLen, m_strText, sizeof(m_strText) / sizeof(m_strText[0]));
}
void CxStaticEx::OnPaint()
{
	HBRUSH hNormalBkBrush = g_ColorRefData.GetHbrBackground();
	HBRUSH hNormalEdgeBrush = g_ColorRefData.GetHbrBlue();
	
	PAINTSTRUCT paintstruct;
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	if (m_bHover && m_bNeedHover)
	{
		HBRUSH hbrush = CreateSolidBrush(RGB(209, 232, 255));
		FillRect(memdc, &rect, hbrush);
		DeleteObject(hbrush);
	}
	else
		FillRect(memdc, &rect, hNormalBkBrush);

	//显示内容
	HFONT hfont = (HFONT)SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	SelectObject(memdc, hfont);
	SetBkMode(memdc,TRANSPARENT);

	if (IsWindowEnabled(m_hWnd) && m_nSize[m_nType] > 1)
		SetTextColor(memdc, g_ColorRefData.GetColorBlue());
	else
		SetTextColor(memdc, g_ColorRefData.GetColorBlack());

	//绘制下拉箭头
	SIZE sz;
	::GetTextExtentPoint32(memdc, m_strText, (int)wcslen(m_strText), &sz);
	int nArrowStart = (rect.right - rect.left) / 2 + sz.cx / 2 + NArrowOffset > rect.right - NArrowWidth - 2 ? rect.right - NArrowWidth - 2 : (rect.right - rect.left) / 2 + sz.cx / 2 + NArrowOffset;
	if (IsWindowEnabled(m_hWnd) && m_nSize[m_nType] > 1)
		DrawTriangle2(memdc, nArrowStart, (rect.bottom - rect.top) / 2 - 2, NArrowWidth);

	DrawText(memdc, m_strText, wcslen(m_strText), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	
	DeleteDC(memdc);
	DeleteObject(bitmap);

	::EndPaint(GetHwnd(), &paintstruct);
}
void CxStaticEx::HidePopWin()
{
	if (IsWindowVisible(m_hlist[m_nType].GetHwnd()))
		ShowWindow(m_hlist[m_nType].GetHwnd(), SW_HIDE);
}