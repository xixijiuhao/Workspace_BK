#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;

CCodeNameStatic::CCodeNameStatic() :m_AlignText(DT_LEFT)
{
}

CCodeNameStatic::~CCodeNameStatic()
{
}
void CCodeNameStatic::InitStatic(HWND hwnd)
{
	SetWindowLong(GetHwnd(), GWL_STYLE, GetWindowLong(GetHwnd(), GWL_STYLE) | SS_OWNERDRAW);
	Init(hwnd);
}
LRESULT  CCodeNameStatic::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			OnPaint();
			return TRUE;
		case WM_DESTROY:
			DeleteObject(m_fontText);
			break;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CCodeNameStatic::OnPaint()
{
	PAINTSTRUCT paintstruct;
	//memset(&paintstruct, 0, sizeof(PAINTSTRUCT));
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(),&rect);
	HDC memdc = CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	DrawHelper::FillSolidRect(memdc, &rect, g_ColorRefData.GetColorBackground());
	SelectObject(memdc,m_fontText);
	DrawText(memdc,m_strText.c_str(),m_strText.length(), &rect, DT_SINGLELINE | m_AlignText | DT_VCENTER);
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	DeleteDC(memdc);
	DeleteObject(bitmap);
	::EndPaint(GetHwnd(), &paintstruct);
}
void CCodeNameStatic::SetStaticText(const wchar_t * szText)
{
	m_strText = szText;
	if (this->m_hWnd)	
		InvalidateRect(GetHwnd(), NULL, TRUE);
}
void CCodeNameStatic::SetTextFont(LOGFONT &font, UINT AlignText)
{
	m_AlignText = AlignText;
	DeleteObject(m_fontText);
	m_fontText=CreateFontIndirect(&font);
	if (this->m_hWnd)	
		InvalidateRect(GetHwnd(), NULL, TRUE);
}
