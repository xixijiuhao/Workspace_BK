#include "PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CxCheck::CxCheck()
{
	m_bInRect= false;
	m_bChecked = false;
}
CxCheck::~CxCheck()
{
	
}
void  CxCheck::Init(HWND hwnd, COLORREF color)
{
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | BS_OWNERDRAW);
	m_colorBK = color;

	CxSubClassWnd::Init(hwnd);

	Resize();
}
LRESULT CALLBACK CxCheck::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		return 0;
	case WM_ERASEBKGND:
		return true;
	case WM_ENABLE:
		InvalidateRect(m_hWnd, 0, TRUE);
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONDBLCLK:
		OnLButtonDbClick(wParam, lParam);
		break;
	case WM_LBUTTONUP:		
		OnLButtonUp(wParam, lParam);
		break;
	}
	
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程

}
void CxCheck::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_bInRect = true;
}
void CxCheck::OnLButtonDbClick(WPARAM wParam, LPARAM lParam)
{
	m_bInRect = false;
	
	POINT pointB;
	pointB.x = GET_X_LPARAM(lParam);
	pointB.y = GET_Y_LPARAM(lParam);
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	if (PtInRect(&rect, pointB))
		SetCheck(GetCheck() == BST_CHECKED ? false : true);	
}
void CxCheck::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if (m_bInRect)
	{
		POINT pointB;
		pointB.x = GET_X_LPARAM(lParam);
		pointB.y = GET_Y_LPARAM(lParam);
		RECT rect;
		GetClientRect(m_hWnd, &rect);
		if (PtInRect(&rect, pointB))
			SetCheck(GetCheck() == BST_CHECKED ? false : true);
	}
	m_bInRect = false;
}
void CxCheck::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdctmp = BeginPaint(m_hWnd, &ps);
	
	//缓存DC 
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	HDC memdc = CreateCompatibleDC(hdctmp);
	HBITMAP bitmap = CreateCompatibleBitmap(hdctmp, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc, bitmap);
	SetBkMode(memdc, TRANSPARENT);
	
	//背景色填充
	HBRUSH hBkBrush = CreateSolidBrush(m_colorBK);
	FillRect(memdc, &rect, hBkBrush);

	//勾选框绘制
	RECT rcsel;
	rcsel.left = 0; 
	rcsel.top = (rect.bottom - rect.top - m_selchecksize) / 2;
	rcsel.right = rcsel.left + m_selchecksize; 
	rcsel.bottom = rcsel.top + m_selchecksize;
	HPEN pen = CreatePen(PS_USERSTYLE, 1, g_ColorRefData.GetColorRGB150());
	SelectObject(memdc, pen);
	Rectangle(memdc, rcsel.left, rcsel.top, rcsel.right, rcsel.bottom);

	if (!IsWindowEnabled(m_hWnd))
	{	
		InflateRect(&rcsel, -1, -1);	//改变矩形大小
		FillRect(memdc, &rcsel, g_ColorRefData.GetHbrRGB200());
	}
	//绘制checkbox 勾选
	else if (m_bChecked)
	{
		HPEN hpen = CreatePen(PS_SOLID, 2, g_ColorRefData.GetColorRGB150());
		SelectObject(memdc, hpen);
		InflateRect(&rcsel, -2, -2);

		POINT pp[3];
		pp[0].x = rcsel.left + 1;
		pp[0].y = rcsel.top + (rcsel.bottom - rcsel.top) / 2;
		pp[1].x = pp[0].x + 2;
		pp[1].y = rcsel.bottom - 2;
		pp[2].x = rcsel.right - 1;
		pp[2].y = rcsel.top + 2;

		MoveToEx(memdc, pp[0].x, pp[0].y, NULL);
		LineTo(memdc, pp[1].x, pp[1].y);
		MoveToEx(memdc, pp[1].x, pp[1].y, NULL);
		LineTo(memdc, pp[2].x, pp[2].y);

		InflateRect(&rcsel, 2, 2);
		DeleteObject(hpen);
	}

	//字体 颜色 区域
	wchar_t strText[MAX_PATH] = { 0 };
	CxSubClassWnd::CxGetWindowText(strText, MAX_PATH);
	//HFONT hfont = (HFONT)SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	//字体
	//g_FontData.Init();
	HFONT hfont = g_FontData.GetFontWord13();
	SelectObject(memdc, hfont);	
	SetTextColor(memdc, m_color);

	rcsel.left = rcsel.right + 4; 
	rcsel.right = rect.right;
	rcsel.top = rect.top;
	rcsel.bottom = rect.bottom;

	DrawText(memdc, strText, wcslen(strText),&rcsel,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

	BitBlt(hdctmp, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);

	DeleteObject(hBkBrush);
	DeleteObject(pen);
	DeleteObject(bitmap);
	DeleteDC(memdc);
	EndPaint(GetHwnd(), &ps);
}

void CxCheck::SetCheck(int check)
{ 
	m_bChecked = (check == BST_CHECKED) ? true : false; 
	InvalidateRect(GetHwnd(), NULL, TRUE); 
	int id = GetWindowLong(m_hWnd, GWL_ID);
	HWND h = ::GetParent(m_hWnd);
	SendMessage(h, WM_COMMAND, MAKEWPARAM(id, BN_CLICKED), 0);
}
void CxCheck::Resize()
{
	//改变窗口大小
	HDC hDC = GetDC(m_hWnd);

	HFONT hfont = (HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0);
	SelectObject(hDC, hfont);
	
	//获取文本占用宽度
	wchar_t strText[MAX_PATH] = { 0 };
	CxSubClassWnd::CxGetWindowText(strText, MAX_PATH);
	SIZE sz;
	::GetTextExtentPoint32(hDC, strText, (int)wcslen(strText), &sz);

	DeleteDC(hDC);

	RECT rectCheck;
	GetWindowRect(m_hWnd, &rectCheck);
	rectCheck.right = rectCheck.left + sz.cx + 20 + 1;
	SetWindowPos(m_hWnd, NULL, 0, 0, rectCheck.right - rectCheck.left, rectCheck.bottom - rectCheck.top, SWP_NOMOVE);
}