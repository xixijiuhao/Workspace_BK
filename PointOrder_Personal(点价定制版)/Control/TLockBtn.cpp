#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;
CLockBtn::CLockBtn(UINT IDBitmap)
	:m_bMouseOnButton(false)
	, m_IDBitmap(IDBitmap)
	, m_bLock(false)
	, m_bFocus(false)
{
	HINSTANCE hInstance;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)&LoadRC::get_default_module, &hInstance);
	m_bitmapList = ImageList_LoadBitmap(hInstance, (LPCTSTR)(DWORD_PTR)m_IDBitmap, 16, 2, RGB(0, 0, 0));
}

CLockBtn::~CLockBtn()
{
	ImageList_Destroy(m_bitmapList);
}
void CLockBtn::CreateLockBtn(HWND hParent, int index)
{
	m_nindex = index;

	CreateFrm(_T("TLockBtn"), hParent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	SetClassLong(m_Hwnd, GCL_STYLE, CS_HREDRAW | CS_VREDRAW);
}
void CLockBtn::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, 0, true);
}
LRESULT CLockBtn::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_Hwnd, &ps);
		DrawItem(hdc);
		EndPaint(m_Hwnd, &ps);
	}
	break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_MOUSEMOVE:
	{
		TRACKMOUSEEVENT		csTME;
		csTME.cbSize = sizeof(csTME);
		csTME.dwFlags = TME_LEAVE;
		csTME.hwndTrack = m_Hwnd;
		::_TrackMouseEvent(&csTME);
		if (!m_bMouseOnButton)
		{
			m_bMouseOnButton = true;
			InvalidateRect(m_Hwnd, NULL, TRUE);
		}
	}
	break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		m_bFocus = false;
		InvalidateRect(m_Hwnd, 0, TRUE);
		break;
	case WM_MOUSEHOVER:
		m_bMouseOnButton = true;
		InvalidateRect(m_Hwnd, NULL, TRUE);
		break;
	case WM_MOUSELEAVE:
	{
		m_bMouseOnButton = false;
		InvalidateRect(m_Hwnd, NULL, TRUE);
	}
	break;
	//case WM_SETFOCUS:
	//	m_bFocus = true;
	//	InvalidateRect(m_Hwnd, 0, TRUE);
	//	break;
	//case WM_KILLFOCUS:
	//	m_bFocus = false;
	//	InvalidateRect(m_Hwnd, 0, TRUE);
	//	break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}
void CLockBtn::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	//SetFocus(m_Hwnd);
	m_bFocus = true;
	WPARAM param; param = MAKEWORD(0, BN_CLICKED);
	PostMessage(GetParent(), WM_COMMAND, param, (LPARAM)m_Hwnd);
	InvalidateRect(m_Hwnd, 0, true);
}
void CLockBtn::DrawItem(HDC hDC)
{
	RECT Rect;
	GetClientRect(m_Hwnd, &Rect);

	::SetBkColor(hDC, g_ColorRefData.g_ColorWhite);
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &Rect, NULL, 0, NULL);
	//贴背景
	HBITMAP bitmap = CreateCompatibleBitmap(hDC, 16, 16);
	HDC memdc = CreateCompatibleDC(hDC);
	SelectObject(memdc, bitmap);
	::SetBkColor(memdc, g_ColorRefData.g_ColorWhite);
	::ExtTextOut(memdc, 0, 0, ETO_OPAQUE, &Rect, NULL, 0, NULL);
	int imageID = 0;
	if (m_bLock)
		imageID = 1;
	ImageList_SetBkColor(m_bitmapList, g_ColorRefData.g_ColorWhite);
	ImageList_Draw(m_bitmapList, imageID, memdc, 0, 0, ILD_TRANSPARENT);
	TransparentBlt(hDC, (Rect.right - Rect.left - 16) / 2, (Rect.bottom - Rect.top - 16) / 2, 16, 16, memdc, 0, 0, 16, 16, RGB(0, 0, 0));
	//鼠标进入
	if (m_bFocus)
	{
		if (m_bMouseOnButton)
			Draw3dRect(hDC, &Rect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT));

	}
	else
	{
		if (m_bMouseOnButton)
			Draw3dRect(hDC, &Rect, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));
	}
	DeleteObject(bitmap);
	DeleteDC(memdc);


}
void CLockBtn::Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	Draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
		lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}
//类似于MFC中的Draw3dRect
void CLockBtn::Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void  CLockBtn::FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr)
{
	::SetBkColor(hdc, clr);
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + cx;
	rect.bottom = y + cy;
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}