#include "PreInclude.h"

#define MainFrmName L"class TMainFrame"
#define  frameWidth  1000
#define  frameHeight 60
//TPointOrderTipsFrm &TPointOrderTipsFrm::Instance()
//{
//    if (!m_pTips)
//        m_pTips = new TPointOrderTipsFrm;
//    return *m_pTips;
//}
TClickPriceOrderTipsFrm* TClickPriceOrderTipsFrm::__instance;

TClickPriceOrderTipsFrm& TClickPriceOrderTipsFrm::GetInstance()
{
    if (!__instance)
        __instance = new TClickPriceOrderTipsFrm();
    return *__instance;
}
TClickPriceOrderTipsFrm::TClickPriceOrderTipsFrm()
    :m_iSecondCount(-1),
    m_wstrText(L"")
{

}
void TClickPriceOrderTipsFrm::Create()
{
    HWND hwnd = FindWindow(MainFrmName, NULL);
    CreateFrm(L"Tips", hwnd, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT | WS_EX_NOACTIVATE);
    RECT rect = { 0 };
    int width = frameWidth;
    int height = frameHeight;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
    SetWindowPos(m_Hwnd, HWND_TOPMOST, rect.left + (rect.right - rect.left - width) / 2, rect.top + (rect.bottom - rect.top - height) / 2, width, height, SWP_SHOWWINDOW);
    SetLayeredWindowAttributes(m_Hwnd, g_ColorRefData.GetColorWhite(), 255, LWA_ALPHA | LWA_COLORKEY);
    SetTimer(m_Hwnd, 0, 1000, NULL);
}

LRESULT TClickPriceOrderTipsFrm::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        OnPaint();
        break;
    case WM_TIMER:
        OnTimer();
        break;
    }
    return NOT_PROCESSED;
}

void TClickPriceOrderTipsFrm::SetText(const int iNum)
{
    TCHAR szText[100] = { 0 };
    for (int i = 0; i < abs(iNum); i++)
    {
        szText[i] = '|';
    }
    wsprintf(szText + abs(iNum), L"%d", iNum);
    SetText(szText);
}


void TClickPriceOrderTipsFrm::SetText(const TCHAR *pszText)
{
    m_wstrText = pszText;
    InvalidateRect(m_Hwnd, NULL, true);
    m_iSecondCount = 1;
}

void TClickPriceOrderTipsFrm::SetText(const char *pszText)
{

	m_wstrText = ANSIToUnicode(pszText);
	InvalidateRect(m_Hwnd, NULL, true);
	m_iSecondCount = 1;
}

void TClickPriceOrderTipsFrm::OnPaint()
{
    TMemDC memdc(m_Hwnd);
    SetBkMode(memdc.GetHdc(), TRANSPARENT);
    RECT rect;
    GetClientRect(m_Hwnd, &rect);
    FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_white);
    if (m_iSecondCount < 0 )
        return;
    SetTextColor(memdc.GetHdc(), g_ColorRefData.GetColorTextGreen());
    SelectObject(memdc.GetHdc(), g_FontData.GetFontWord50());
    DrawText(memdc.GetHdc(), m_wstrText.c_str(), -1, &rect, DT_VCENTER | DT_CENTER);

}

void TClickPriceOrderTipsFrm::OnTimer()
{
    m_iSecondCount--;
    InvalidateRect(m_Hwnd, NULL, true);
}