#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

CxMousePos::CxMousePos()
{

}
CxMousePos::~CxMousePos()
{
	
}
void CxMousePos::Init(HWND hwnd, bool bUseCfg)
{
	SetWindowLong(GetHwnd(), GWL_STYLE, GetWindowLong(GetHwnd(),GWL_STYLE) | SS_OWNERDRAW);
	if (bUseCfg)
	{
		m_ColorBText = RGB_Green_W;
		m_ColorSText = RGB_Red_W;
	}
	else
	{	
		m_ColorBText = RGB_Red_W;
		m_ColorSText = RGB_Green_W;
	}
	CxSubClassWnd::Init(hwnd);
}
void CxMousePos::UpdatePosInfo(int nLong, int nShort, wchar_t* sPriceLong, wchar_t* sPriceShort)
{
	m_nLong = nLong;			//��ͷ�ֲ�
	m_nShort = nShort;			//��ͷ�ֲ�
	m_wLongPrice = sPriceLong;	//��ͷ�۸�
	m_wShortPrice = sPriceShort;//��ͷ�۸�
	InvalidateRect(m_hWnd, NULL, TRUE);
}
LRESULT CALLBACK CxMousePos::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			OnPaint();
			return 1;
		case WM_ERASEBKGND:
			return true;
		case WM_DESTROY:
			break;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //Ĭ�ϻص�����
}

void CxMousePos::OnPaint()
{
	PAINTSTRUCT paintstruct;
	HDC hdc = ::BeginPaint(GetHwnd(), &paintstruct);
	RECT rect;
	GetClientRect(GetHwnd(), &rect);
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmap);
	//�ı�
	HFONT hfont = (HFONT)SendMessage(GetHwnd(), WM_GETFONT, 0, 0);
	SelectObject(memdc, hfont);
	SetBkMode(memdc, TRANSPARENT);

	wchar_t	m_strText[MAX_PATH] = {0};	//��ʾ�ı�
	if (m_nLong&&m_nShort)
	{
		RECT rectL,rectR;
		rectL = rectR = rect;
		rectL.right = rectL.left + (rectL.right - rectL.left) / 2;
		rectR.left = rectL.right;
		swprintf_s(m_strText, L"%s %d",LoadResWchar_t(IDS_STRING1104), m_nLong);
		FillRect(memdc, &rectL, g_ColorRefData.GetHbrBackground());
		SetTextColor(memdc, m_ColorBText);
		DrawText(memdc, m_strText, wcslen(m_strText), &rectL, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		swprintf_s(m_strText, L"%s %d", LoadResWchar_t(IDS_STRING1105), m_nShort);
		FillRect(memdc, &rectR, g_ColorRefData.GetHbrBackground());
		SetTextColor(memdc, m_ColorSText);
		DrawText(memdc, m_strText, wcslen(m_strText), &rectR, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
	else if (m_nLong&&!m_nShort)
	{
		swprintf_s(m_strText, L"%s %d  %s", LoadResWchar_t(IDS_STRING1104), m_nLong, m_wLongPrice.c_str());
		FillRect(memdc, &rect, g_ColorRefData.GetHbrBackground());
		SetTextColor(memdc, m_ColorBText);
		DrawText(memdc, m_strText, wcslen(m_strText), &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
	else if (m_nShort&&!m_nLong)
	{
		swprintf_s(m_strText, L"%s %d  %s", LoadResWchar_t(IDS_STRING1105), m_nShort, m_wShortPrice.c_str());
		FillRect(memdc, &rect, g_ColorRefData.GetHbrBackground());
		SetTextColor(memdc, m_ColorSText);
		DrawText(memdc, m_strText, wcslen(m_strText), &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
	else
		FillRect(memdc, &rect,g_ColorRefData.GetHbrBackground());
		
	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
	
	DeleteDC(memdc);
	DeleteObject(bitmap);

	::EndPaint(GetHwnd(), &paintstruct);
}
