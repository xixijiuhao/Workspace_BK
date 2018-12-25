#include "..\PreInclude.h"

extern HINSTANCE g_hinst;
extern wstring LoadResString(int strID);

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CSwitchBtn::CSwitchBtn() :m_nState(0), m_nSwitchLevel(0), m_sToolTips(TEXT("")),m_hTip(NULL)
{

}
CSwitchBtn::~CSwitchBtn()
{

}
void CSwitchBtn::InitButton(HWND hwnd, const wchar_t* w1,const wchar_t* w2,const wchar_t *wtip )
{
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | BS_OWNERDRAW);
	m_sTextlevel1 = w1;
	m_sTextlevel2 = w2;
	Init(hwnd);
	//提示信息
	RtlZeroMemory(&m_tip, sizeof(m_tip));
	m_tip.cbSize = sizeof(TOOLINFO) - sizeof(void*);
	m_tip.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT | TTF_IDISHWND;
	m_tip.hinst = GetModuleHandle(NULL);
	m_tip.lpszText = TEXT("");
	m_tip.uId = 0;

	m_sToolTips = wtip;
	m_hTip = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		0, 0,
		23, 23,
		m_hWnd, NULL, GetModuleHandle(NULL), NULL);

	SendMessage(m_hTip, TTM_SETDELAYTIME, TTDT_AUTOMATIC, 100);	//提示出现延时
	SendMessage(m_hTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 2000);	//提示消失延时
	SendMessage(m_hTip, TTM_SETMAXTIPWIDTH, 0, 200);			//提示最大宽度
}
LRESULT CALLBACK CSwitchBtn::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hWnd, &ps);
		DrawItem(hdc);
		EndPaint(m_hWnd, &ps);
	}
	break;
		case WM_MOUSEMOVE:
			{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 1;
			_TrackMouseEvent(&tme);
			}
			break;
		case WM_MOUSELEAVE:
			m_nState = 0;
			InvalidateRect(GetHwnd(), NULL, TRUE);
			break;
		case WM_MOUSEHOVER:
			m_nState = 1;
			{
				if (!m_sToolTips.empty())
				{
					m_tip.uId = (UINT)(m_tip.hwnd = m_hWnd);
					SendMessage(m_hTip, TTM_SETTITLE, TTI_NONE, (LPARAM)L"");//添加标题
					m_tip.lpszText = (LPWSTR)m_sToolTips.c_str();
					SendMessage(m_hTip, TTM_ADDTOOL, 0, (LPARAM)&m_tip);
				}	
			}
			InvalidateRect(GetHwnd(), NULL, TRUE);
			break;
		case WM_ERASEBKGND:
			return TRUE;
		case WM_DESTROY:
			DestroyWindow(m_hTip);
			break;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}

void CSwitchBtn::DrawItem(HDC hdc)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	HDC memdc;
	memdc = CreateCompatibleDC(hdc);
	HBITMAP bitmapOrgin = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmapOrgin);
	SelectObject(memdc,GetStockObject(NULL_BRUSH));

	DrawHelper::FillSolidRect(memdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, g_ColorRefData.GetColorBackground());

	if (m_nState==1)
	{
		HPEN pen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorLightBlue());
		SelectObject(memdc, pen);
		Rectangle(memdc, 0, 0, rect.right, rect.bottom);
		DeleteObject(pen);
	}
	HFONT hfont = (HFONT)SendMessage(m_hWnd,WM_GETFONT,0,0);
	SelectObject(memdc, hfont);

	SetBkMode(memdc,TRANSPARENT);
	SetTextColor(memdc, g_ColorRefData.GetColorBlue());

	if (m_nSwitchLevel==0)
		DrawText(memdc, m_sTextlevel1.c_str(), m_sTextlevel1.length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	else
		DrawText(memdc, m_sTextlevel2.c_str(), m_sTextlevel1.length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	DeleteObject(bitmapOrgin);
	
	StretchBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SRCCOPY);
	DeleteDC(memdc);
}