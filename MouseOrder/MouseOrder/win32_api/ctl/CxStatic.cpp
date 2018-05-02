#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

#define NArrowWidth	 7
#define NArrowOffset 5
CxStatic::CxStatic()
{
	m_bHover = false;
	m_bTracking = false;
	m_bTriangle = false;
	memset(m_strText, 0, sizeof(m_strText));
}
CxStatic::~CxStatic()
{
	
}
void CxStatic::Init(HWND hwnd, const wchar_t *wtext, bool btriangle)
{
	SetWindowLong(GetHwnd(), GWL_STYLE, GetWindowLong(GetHwnd(),GWL_STYLE) | SS_OWNERDRAW);
	memcpy(m_strText, wtext, sizeof(m_strText));
	m_bTriangle = btriangle;
	CxSubClassWnd::Init(hwnd);
}
LRESULT CALLBACK CxStatic::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
			int id = GetWindowLong(m_hWnd, GWL_ID);
			SendMessage(::GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(id, BN_CLICKED), 0);
		}
		break;
		case WM_ERASEBKGND:
			return true;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}

void CxStatic::OnPaint()
{
	PAINTSTRUCT paintstruct;
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	
	if (m_bHover)
	{
		HBRUSH hbrush = CreateSolidBrush(RGB(209,232,255));
		FillRect(memdc, &rect, hbrush);
		DeleteObject(hbrush);
	}
	else
		FillRect(memdc, &rect, g_ColorRefData.GetHbrBackground());
	HFONT hfont = (HFONT)SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	SelectObject(memdc, hfont);
	SetBkMode(memdc,TRANSPARENT);

	if (IsWindowEnabled(m_hWnd))
		SetTextColor(memdc, g_ColorRefData.GetColorBlue());
	else
		SetTextColor(memdc, g_ColorRefData.GetColorBlack());

	if (m_bTriangle)//下拉箭头
	{
		SIZE sz;
		::GetTextExtentPoint32(memdc, m_strText, (int)wcslen(m_strText), &sz);
		int nArrowStart = (rect.right - rect.left) / 2 + sz.cx / 2 + NArrowOffset > rect.right - NArrowWidth - 2 ? rect.right - NArrowWidth - 2 : (rect.right - rect.left) / 2 + sz.cx / 2 + NArrowOffset;
		if (IsWindowEnabled(m_hWnd))
			DrawTriangle2(memdc, nArrowStart, (rect.bottom - rect.top) / 2 - 2, NArrowWidth);
	}	

	DrawText(memdc, m_strText, wcslen(m_strText), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	
	DeleteDC(memdc);
	DeleteObject(bitmap);

	::EndPaint(GetHwnd(), &paintstruct);
}
void CxStatic::DrawTriangle2(HDC &hdc, int nStartx, int nStarty, int nWidth)//宽度奇数
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