#include "..\PreInclude.h"

extern wstring LoadResString(int strID);

extern G_UIFONT g_FontData;

CDrawStatic::CDrawStatic()
{
	m_font = CreateFont(-16, 0, 0, 0, 400, FALSE, FALSE, 0, GB2312_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, g_FontData.GetFontWordName().c_str());

	m_pen=CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	m_brush =NULL;
}
CDrawStatic::~CDrawStatic()
{
	DeleteObject(m_pen);
	DeleteObject(m_brush);
	DeleteObject(m_font);
}
void CDrawStatic::InitStatic(HWND hwnd)
{
	//SetWindowLong(GetHwnd(), GWL_STYLE, GetWindowLong(GetHwnd(),GWL_STYLE) | SS_OWNERDRAW);
	Init(hwnd);
}
LRESULT CALLBACK CDrawStatic::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			OnPaint();
			return 1;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CDrawStatic::UpdateText(unsigned int  PosQty, wstring wstrText, LOGFONT &lgFont, COLORREF rgbBk, COLORREF rgbText)
{
	m_iPosQty = PosQty;
	m_strText = wstrText;
	m_rgbBk = rgbBk;
	m_rgbText = rgbText;
	DeleteObject(m_brush);
	m_brush = CreateSolidBrush(m_rgbBk);
	DeleteObject(m_font);
	m_font = CreateFontIndirect(&lgFont);
	if (::IsWindow(m_hWnd))
		InvalidateRect(GetHwnd(),NULL,TRUE);
}
void CDrawStatic::UpdateText(unsigned int  PosQty, wstring strText, COLORREF rgbText)
{
	m_iPosQty = PosQty;
	m_strText = strText;
	m_rgbText = rgbText;
	if (::IsWindow(m_hWnd))	
		InvalidateRect(GetHwnd(), NULL, TRUE);
}
void CDrawStatic::UpdateText(unsigned int  PosQty, wstring strText, COLORREF rgbText, COLORREF rgbBk)
{
	m_iPosQty = PosQty;
	m_strText = strText;
	m_rgbText = rgbText;
	m_rgbBk = rgbBk;
	DeleteObject(m_brush);
	m_brush=CreateSolidBrush(m_rgbBk);
	if (::IsWindow(m_hWnd))	
		InvalidateRect(GetHwnd(), NULL, TRUE);
}
void CDrawStatic::SetAutoSize()
{
	if (::IsWindow(m_hWnd))
	{
		wstring str = LoadResString(IDS_STR_POSTEST);
		HDC hDC = GetDC(GetHwnd());
		SelectObject(hDC, m_font);
		SIZE sz;
		::GetTextExtentPoint32(hDC, str.c_str(), str.length(), &sz);

		RECT rectEdit;
		GetWindowRect(GetHwnd(), &rectEdit);
		POINT pt;
		pt.x = rectEdit.left;
		pt.y = rectEdit.top;
		ScreenToClient(GetHwnd(), &pt);
		rectEdit.right = rectEdit.right - rectEdit.left + pt.x;
		rectEdit.bottom = rectEdit.bottom - rectEdit.top + pt.y;
		rectEdit.left = pt.x;
		rectEdit.top = pt.y;

		rectEdit.right = rectEdit.left + sz.cx + 2;
		rectEdit.bottom = rectEdit.top + sz.cy + 2;
		SetWindowPos(GetHwnd(), NULL, 0, 0, rectEdit.right - rectEdit.left, rectEdit.bottom - rectEdit.top, SWP_NOMOVE);
		ReleaseDC(GetHwnd(), hDC);
	}
}
void CDrawStatic::OnPaint()
{
	PAINTSTRUCT paintstruct;
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	
	SelectObject(memdc, m_font);
	SelectObject(memdc, m_pen);
	FillRect(memdc, &rect, m_brush);
	wchar_t str[21] = { 0 };
	swprintf_s(str,_T("%s %d"),m_strText.c_str(), m_iPosQty);
	
	SetBkMode(memdc,TRANSPARENT);
	SetTextColor(memdc,m_rgbText);
	wstring strtext(str);
	DrawText(memdc, strtext.c_str(), strtext.length(), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	
	DeleteDC(memdc);
	DeleteObject(bitmap);
	::EndPaint(GetHwnd(), &paintstruct);
}