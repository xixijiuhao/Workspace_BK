#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern HINSTANCE g_hinst;

using namespace std;
CLockBtn::CLockBtn(UINT IDBitmap)
	:m_bMouseOnButton(false)
	, m_IDBitmap(IDBitmap)
	, m_bLock(false) 
	, m_bDown(false)
	, m_bMouseTrack(TRUE)
	, m_sToolTips(L"")
	, m_hTip(NULL)
{
	m_bitmapList = ImageList_LoadBitmap(g_hinst, (LPCTSTR)(DWORD_PTR)m_IDBitmap, 16, 2, RGB(0, 0, 0));
}
CLockBtn::~CLockBtn()
{
	ImageList_Destroy(m_bitmapList);
}

BOOL CLockBtn ::Init(HWND hwnd)
{
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
	SendMessage(m_hTip, TTM_SETTIPBKCOLOR, (WPARAM)(COLORREF)g_ColorRefData.GetColorLightBlue(), 0);
	SendMessage(m_hTip, TTM_SETTIPTEXTCOLOR, (WPARAM)(COLORREF)g_ColorRefData.GetColorWhite(), 0);
	
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | BS_OWNERDRAW);
	return CxSubClassWnd::Init(hwnd);
}
LRESULT CLockBtn::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc= BeginPaint(m_hWnd, &ps);
			DrawItem(hdc);
			EndPaint(m_hWnd, &ps);
		}
		break;
		case WM_DRAWITEM://此消息是发送给parent的 MSDN上有介绍
			break;
		case WM_ERASEBKGND:
			return TRUE;
			break;
		case WM_MOUSEMOVE:
		{
			if (m_bMouseTrack)
			{
				TRACKMOUSEEVENT		csTME;
				csTME.cbSize = sizeof(csTME);
				csTME.dwFlags = TME_LEAVE | TME_HOVER;
				csTME.hwndTrack = m_hWnd;
				csTME.dwHoverTime = 10;  //鼠标在按钮上停留超过10ms，才认为状态为HOVER
				::_TrackMouseEvent(&csTME);
				m_bMouseTrack = FALSE;
			}
			if (!m_bMouseOnButton)
			{
				m_bMouseOnButton = true;
				InvalidateRect(m_hWnd,NULL,TRUE);
			}
		}
			break;
		case WM_MOUSEHOVER:
		{
			if (!m_sToolTips.empty())
			{
				m_tip.uId = (UINT)(m_tip.hwnd = m_hWnd);
				SendMessage(m_hTip, TTM_SETTITLE, TTI_NONE, (LPARAM)L"");//添加标题
				m_tip.lpszText = (LPWSTR)m_sToolTips.c_str();
				SendMessage(m_hTip, TTM_ADDTOOL, 0, (LPARAM)&m_tip);
			}	
		}
			break;
		case WM_MOUSELEAVE:
		{
			m_bMouseTrack = TRUE;
			m_bMouseOnButton = false;
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
			break;
		case WM_LBUTTONDOWN:
			m_bDown = true;
			InvalidateRect(m_hWnd, NULL, TRUE);
			break;
		case WM_LBUTTONUP:
			m_bDown = false;
			InvalidateRect(m_hWnd, NULL, TRUE);
			break;
		case WM_DESTROY:
			DestroyWindow(m_hTip);
			break;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CLockBtn::DrawItem(HDC hDC)
{
	RECT Rect ;
	GetClientRect(m_hWnd, &Rect);
	
	::SetBkColor(hDC, g_ColorRefData.GetColorBackground());
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &Rect, NULL, 0, NULL);
	//贴背景
	HBITMAP bitmap = CreateCompatibleBitmap(hDC, 16, 16);
	HDC memdc = CreateCompatibleDC(hDC);
	SelectObject(memdc, bitmap);
	::SetBkColor(memdc, g_ColorRefData.GetColorBackground());
	::ExtTextOut(memdc, 0, 0, ETO_OPAQUE, &Rect, NULL, 0, NULL);
	int imageID = 0;
	if (m_bLock)
		imageID = 1;
	ImageList_SetBkColor(m_bitmapList, g_ColorRefData.GetColorBackground());
	ImageList_Draw(m_bitmapList, imageID, memdc, 0, 0, ILD_TRANSPARENT);
	TransparentBlt(hDC, (Rect.right - Rect.left - 16) / 2, (Rect.bottom - Rect.top - 16) / 2, 16, 16, memdc, 0, 0, 16, 16, RGB(0, 0, 0));
	//鼠标进入
	if (m_bDown)
		DrawHelper::Draw3dRect(hDC, &Rect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT));
	else
	{
		if (m_bMouseOnButton)
			DrawHelper::Draw3dRect(hDC, &Rect, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));
	}
	DeleteObject(bitmap);
	DeleteDC(memdc);
}