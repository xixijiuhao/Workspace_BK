#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

#define NArrowWidth	9
#define NArrowOffset 5

void CxComboxEx::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;

	HPEN   hGrayPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
	SelectObject(hdc, hGrayPen);

	int nMidx = nStartx + nWidth / 2;
	int nMidy = nStarty + nWidth / 2;
	MoveToEx(hdc, nStartx, nStarty, NULL);
	LineTo(hdc, nMidx, nMidy);

	int nEndx = nStartx + nWidth - 1;
	int nEndy = nStarty;
	LineTo(hdc, nEndx + 1, nEndy - 1);

	DeleteObject(hGrayPen);
}
CxComboxEx::CxComboxEx()
{
	m_bHover = false;
	m_bTracking = false;
	m_bHasArrow = true;
	memset(m_strText, 0, sizeof(m_strText));
	memset(m_stype, 0, sizeof(m_stype));
}
CxComboxEx::~CxComboxEx()
{
	
}
bool CxComboxEx::SetSelect(const wchar_t* sStrategy, int nRef)
{
	CxSetWindowText(sStrategy);
	return true;
}

void CxComboxEx::Init(HWND hwnd, TMouPriceTypeStrW stype[10], int nSize,bool bArrow)
{
	SetWindowLong(GetHwnd(), GWL_STYLE, GetWindowLong(GetHwnd(),GWL_STYLE) | SS_OWNERDRAW);
	m_nSize = nSize;
	m_bHasArrow = bArrow;
	memcpy_s(m_stype, sizeof(m_stype), stype, sizeof(m_stype));
	CxSubClassWnd::Init(hwnd);
}
LRESULT CALLBACK CxComboxEx::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
			RECT rect;
			GetWindowRect(m_hWnd, &rect);
			if (!IsWindow(m_hlist.GetHwnd()))
				m_hlist.CreatePopWin(this, GetParent(GetHwnd()), rect.right - rect.left, rect.left, rect.bottom, 0, m_stype, m_nSize, m_nSize, 1,true);
			m_hlist.UpdateUnit(rect.right - rect.left);
			SetWindowPos(m_hlist.GetHwnd(), NULL, rect.left, rect.bottom, 0, 0, SWP_NOSIZE);
			ShowWindow(m_hlist.GetHwnd(), SW_SHOWNORMAL);	
		}
			break;
		case WM_ERASEBKGND:
			return true;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CxComboxEx::CxSetWindowText(const wchar_t* wstr)
{
	wcsncpy_s(m_strText, wstr, sizeof(m_strText) / sizeof(m_strText[0]));
	if (::IsWindow(m_hWnd))
		InvalidateRect(GetHwnd(),NULL,TRUE);
}
void CxComboxEx::CxGetWindowText(wchar_t* wstr,int nMaxLen)
{
	wcsncpy_s(wstr, nMaxLen, m_strText, sizeof(m_strText) / sizeof(m_strText[0]));
}
void CxComboxEx::OnPaint()
{
	HBRUSH hNormalBkBrush = g_ColorRefData.GetHbrWhite();
	
	PAINTSTRUCT paintstruct;
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	
	FillRect(memdc, &rect, hNormalBkBrush);
	
	FrameRect(memdc, &rect, m_bHover? g_ColorRefData.GetHbrLightBlue() : g_ColorRefData.GetHbrRGB150());

	//显示内容
	HFONT hfont = (HFONT)SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	SelectObject(memdc, hfont);
	SetBkMode(memdc,TRANSPARENT);

	if (IsWindowEnabled(m_hWnd) && m_nSize > 1)
		SetTextColor(memdc, g_ColorRefData.GetColorBlack());
	else
		SetTextColor(memdc, g_ColorRefData.GetColorRGB150());

	if (IsWindowEnabled(m_hWnd) && m_nSize > 1 && m_bHasArrow)
		DrawTriangle(memdc, rect.right -13, (rect.bottom - rect.top) / 2 - 2, NArrowWidth);

	RECT rect1(rect);
	rect1.left = rect1.left + 4;
	DrawText(memdc, m_strText, wcslen(m_strText), &rect1, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	
	DeleteDC(memdc);
	DeleteObject(bitmap);

	::EndPaint(GetHwnd(), &paintstruct);
}
void CxComboxEx::HidePopWin()
{
	if (IsWindowVisible(m_hlist.GetHwnd()))
		ShowWindow(m_hlist.GetHwnd(), SW_HIDE);
}