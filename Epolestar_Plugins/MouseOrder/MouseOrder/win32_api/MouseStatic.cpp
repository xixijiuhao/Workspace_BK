#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CMouseStatic::CMouseStatic()
{
	m_bHover = false;
	m_bTracking = false;
}
CMouseStatic::~CMouseStatic()
{
	
}
void CMouseStatic::Init(HWND hwnd)
{
	SetWindowLong(GetHwnd(), GWL_STYLE, GetWindowLong(GetHwnd(),GWL_STYLE) | SS_OWNERDRAW);
	CxSubClassWnd::Init(hwnd);
	//提示信息
	RtlZeroMemory(&m_tip, sizeof(m_tip));
	m_tip.cbSize = sizeof(TOOLINFO) - sizeof(void*);
	m_tip.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT | TTF_IDISHWND;
	m_tip.hinst = GetModuleHandle(NULL);
	m_tip.lpszText = L"";
	m_tip.uId = 0;

	m_hTip = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		0, 0,
		23, 23,
		m_hWnd, NULL, GetModuleHandle(NULL), NULL);

	SendMessage(m_hTip, TTM_SETDELAYTIME, TTDT_AUTOMATIC, 100);	//提示出现延时
	SendMessage(m_hTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 10000);	//提示消失延时
	SendMessage(m_hTip, TTM_SETMAXTIPWIDTH, 0, 200);			//提示最大宽度
	SendMessage(m_hTip, TTM_SETTIPBKCOLOR, (WPARAM)(COLORREF)g_ColorRefData.GetColorWhite(), 0);
}
LRESULT CALLBACK CMouseStatic::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			OnPaint();
			return true;
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
			{
				//if (!m_sToolTips.empty())
				{
					m_tip.uId = (UINT)(m_tip.hwnd = m_hWnd);
					SendMessage(m_hTip, TTM_SETTITLE, TTI_NONE, (LPARAM)"");//添加标题
					m_tip.lpszText = (LPWSTR)m_sToolTips.c_str();
					SendMessage(m_hTip, TTM_ADDTOOL, 0, (LPARAM)&m_tip);
				}
			}
			break;
		case WM_ERASEBKGND:
			return true;
		case WM_DESTROY:
			DestroyWindow(m_hTip);
			break;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CMouseStatic::UpdateText(string wstrText)
{
	m_strText = LoadRC::ansi_to_unicode(wstrText.c_str());
	if (::IsWindow(m_hWnd))
		InvalidateRect(GetHwnd(),NULL,TRUE);
}

void CMouseStatic::OnPaint()
{
	PAINTSTRUCT paintstruct;
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	
	FillRect(memdc, &rect, g_ColorRefData.GetHbrBackground());

	SelectObject(memdc, g_FontData.GetFontWord13());
	SetBkMode(memdc,TRANSPARENT);
	
	if (m_bHover)
		SetTextColor(memdc, g_ColorRefData.GetColorLightBlue());
	else
		SetTextColor(memdc, g_ColorRefData.GetColorBlue());

	DrawText(memdc, m_strText.c_str(), m_strText.length(), &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	
	DeleteDC(memdc);
	DeleteObject(bitmap);
	::EndPaint(GetHwnd(), &paintstruct);
}