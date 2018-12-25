#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern HINSTANCE g_hinst;

CLockBtn::CLockBtn(UINT IDBitmap)
	:m_bMouseOnButton(false)
	, m_IDBitmap(IDBitmap)
	, m_bLock(false) 
	, m_bDown(false)
	, m_bMouseTrack(TRUE)
	, m_nInflate(0)
{
	m_bitmapList = ImageList_LoadBitmap(g_hinst, (LPCTSTR)(DWORD_PTR)m_IDBitmap, 16, 2, RGB(0, 0, 0));
}
CLockBtn::~CLockBtn()
{
	ImageList_Destroy(m_bitmapList);
}

BOOL CLockBtn ::Init(HWND hwnd, int nInflate)
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

	SendMessage(m_hTip, TTM_SETDELAYTIME, TTDT_AUTOMATIC, 100);	//��ʾ������ʱ
	SendMessage(m_hTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 10000);	//��ʾ��ʧ��ʱ
	SendMessage(m_hTip, TTM_SETMAXTIPWIDTH, 0, 200);			//��ʾ�����
	SendMessage(m_hTip, TTM_SETTIPBKCOLOR, (WPARAM)(COLORREF)g_ColorRefData.GetColorLightBlue(), 0);
	SendMessage(m_hTip, TTM_SETTIPTEXTCOLOR, (WPARAM)(COLORREF)g_ColorRefData.GetColorWhite(), 0);
	
	m_nInflate = nInflate;
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
		case WM_DRAWITEM://����Ϣ�Ƿ��͸�parent�� MSDN���н���
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
				csTME.dwHoverTime = 10;  //����ڰ�ť��ͣ������10ms������Ϊ״̬ΪHOVER
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
				SendMessage(m_hTip, TTM_SETTITLE, TTI_NONE, (LPARAM)"");//��ӱ���
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
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //Ĭ�ϻص�����
}
void CLockBtn::DrawItem(HDC hDC)
{
	RECT Rect ;
	GetClientRect(m_hWnd, &Rect);
	
	::SetBkColor(hDC, g_ColorRefData.GetColorBackground());
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &Rect, NULL, 0, NULL);
	//������
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
	
	if(m_nInflate)
		InflateRect(&Rect, -m_nInflate, -m_nInflate);
	//������
	if (m_bDown)
		Draw3dRect(hDC, &Rect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT));
	else
	{
		if (m_bMouseOnButton)
			Draw3dRect(hDC, &Rect, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));
	}
	DeleteObject(bitmap);
	DeleteDC(memdc);
}
//������MFC�е�FillSolidRect
void CLockBtn::FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr)
{
	::SetBkColor(hdc, clr);
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + cx;
	rect.bottom = y + cy;
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}
//������MFC�е�Draw3dRect
void CLockBtn::Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	Draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
		lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}
//������MFC�е�Draw3dRect
void CLockBtn::Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}