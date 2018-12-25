#include "..\PreInclude.h"
//É«²Ê
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;
#define N_EDIT_HEIGHT 24
CxEditEx::CxEditEx()
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
}
CxEditEx::~CxEditEx()
{
	
}
void CxEditEx::Create(int nID,int nSubID, HWND parent, int nx, int ny, int nwidth, int nheight, const wchar_t *defshow, bool bFrame)
{
	m_nWidth = nwidth;
	m_nHeight = nheight;
	m_nLeft = nx;
	m_nTop = ny;
	m_nEditId = nSubID;
    m_bFrame = bFrame;
	memcpy(m_wDefShow, defshow, sizeof(m_wDefShow));
	CreateFrm(L"EditEx", parent, WS_CHILD | WS_CLIPSIBLINGS, 0);
}
void CxEditEx::OnPaint(bool bHover)
{
	HDC			hDC;
	PAINTSTRUCT ps;
	hDC = BeginPaint(m_Hwnd, &ps);
	
	RECT rect, rectClient;
	::GetWindowRect(m_Hwnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;

	if (!IsWindowVisible(m_hEdit))
	{
		RECT rect(rectClient);
		rect.left = rectClient.left + 5;
		rect.top = rectClient.top + 6;
		rect.bottom = rectClient.bottom - 5;
		RECT rect2(rectClient);
		InflateRect(&rect2, -1, -1);
		if (!IsWindowEnabled(m_Hwnd))
			FillRect(hDC, &rect2, g_ColorRefData.GetHbrBackground());
		else
			FillRect(hDC, &rect2, g_ColorRefData.GetHbrWhite());
		SetTextColor(hDC, g_ColorRefData.GetColorRGB200());
		SetBkMode(hDC, TRANSPARENT);
		SelectObject(hDC, g_FontData.GetFontWord13());
		DrawTextW(hDC, m_wDefShow, wcslen(m_wDefShow), &rect, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	}
	
    if (m_bFrame)
    {
        if (bHover || GetFocus() == m_hEdit)
            ::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrLightBlue());
        else
            ::FrameRect(hDC, &rectClient, g_ColorRefData.GetHbrRGB150());
    }

	EndPaint(m_Hwnd, &ps);
}
LRESULT CxEditEx::WndProc(UINT message, WPARAM w, LPARAM l)
{
	switch (message)
	{
	case WM_CREATE:
		{
			HINSTANCE hInstance = GetModuleHandle(NULL);
			m_hEdit = CreateWindowEx(0, L"edit", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | ES_AUTOHSCROLL, 2, 4, m_nWidth - 4, m_nHeight - 6, m_Hwnd, (HMENU)m_nEditId, hInstance, NULL);
			ShowWindow(m_hEdit, SW_HIDE);

			SetWindowLong(m_Hwnd, GWL_ID, m_nWinId);
			SetWindowPos(m_Hwnd, NULL, m_nLeft, m_nTop, m_nWidth, m_nHeight, SWP_NOZORDER);
		}
		break;
	case WM_COMMAND:
	if(m_nEditId==LOWORD(w))	
		if (HIWORD(w) == EN_CHANGE)
		{
			wchar_t wText[100] = { 0 };
			GetWindowTextW(m_hEdit, wText, 100);
			if (wcscmp(wText, L"") && !IsWindowVisible(m_hEdit))
				ShowWindow(m_hEdit, SW_SHOW);
			else if (wcscmp(wText, L"") == 0 && IsWindowVisible(m_hEdit)&&GetFocus()!=m_hEdit)
				ShowWindow(m_hEdit, SW_HIDE);
			SendMessage(::GetParent(m_Hwnd), message, w, l);
		}
		else if (HIWORD(w) == EN_KILLFOCUS)
		{
			wchar_t wText[100] = { 0 };
			GetWindowTextW(m_hEdit, wText, 100);
			if (!wcscmp(wText, L""))
				ShowWindow(m_hEdit, SW_HIDE);
			OnDealKillFocud();
			SendMessage(::GetParent(m_Hwnd), message,w,l);
		}
		else if (HIWORD(w) == EN_SETFOCUS)
			OnDealSetFocus();
		break;
	case WM_PAINT:
		OnPaint(m_bHover);
		break;
	case WM_LBUTTONDOWN:
		if(!IsWindowVisible(m_hEdit))
			ShowWindow(m_hEdit, SW_SHOW);
		if (GetFocus() != m_hEdit)
			SetFocus(m_hEdit);
		InvalidateRect(m_Hwnd, NULL, TRUE);
		break;
	case WM_MOUSEMOVE:
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_Hwnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 0;
			m_bTracking = _TrackMouseEvent(&tme) == TRUE;
		}
		break;
	case WM_MOUSELEAVE:
		OnDealLeave();
		break;
	case WM_MOUSEHOVER:
		OnDealHover();
		break;
	case WM_SETFOCUS:
		OnDealSetFocus();
		break;
	case WM_KILLFOCUS:
		OnDealKillFocud();
		break;
	case WM_ENABLE:
		EnableWindow(m_hEdit, w);
		InvalidateRect(m_Hwnd, NULL, TRUE);
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}
void CxEditEx::OnDealHover()
{
	m_bHover = TRUE;
	InvalidateRect(m_Hwnd, NULL, TRUE);	
}
void CxEditEx::OnDealLeave()
{
	POINT pt;
	::GetCursorPos(&pt);
	RECT rect;
	GetWindowRect(m_Hwnd, &rect);
	if (!PtInRect(&rect, pt))
	{
		m_bHover = FALSE;
		InvalidateRect(m_Hwnd, NULL, TRUE);
	}
	m_bTracking = FALSE;
}
void CxEditEx::OnDealSetFocus()
{
	if (!IsWindowVisible(m_hEdit))
		ShowWindow(m_hEdit, SW_SHOW);
	m_bHover = TRUE;
	InvalidateRect(m_Hwnd, NULL, TRUE);
}
void CxEditEx::OnDealKillFocud()
{
	m_bHover = FALSE;
	InvalidateRect(m_Hwnd, NULL, TRUE);
}
void CxEditEx::UpdateFont()
{
	HDC hDC = GetDC(m_hEdit);
	HFONT hfont = (HFONT)SendMessage(m_hEdit, WM_GETFONT, 0, 0);
	SelectObject(hDC, hfont);
	SIZE sz;
	::GetTextExtentPoint32(hDC, L"T", (int)wcslen(L"T"), &sz);
	DeleteDC(hDC);
	int nver = (N_EDIT_HEIGHT- sz.cy) / 2;
	MoveWindow(m_hEdit, 2, nver, m_nWidth - 4, sz.cy, TRUE);
}