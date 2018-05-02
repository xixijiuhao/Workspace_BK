/**
* =====================================================================
*
*       [filename    ] StatusBarWnd.cpp
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-10-12
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/


#include "StatusBarWnd.h"
#include "UIDesign.h"
#include "Language.h"
#include "../share/AnsicUnicode.h"
#include "LanguageApi_API.h"
#include "ConfigFrame_API.h"

#define COLOR_BG RGB(60, 60, 60)
#define COLOR_GREEN RGB(0, 220, 0)

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;
extern ILanguageApi *g_pLanguage;
extern IConfigFrame	*g_conFrame;

StatusBarWnd::StatusBarWnd()
{
    m_brushBg = CreateSolidBrush(COLOR_BG);
    m_textColor = COLOR_GREEN;

    LOGFONT lg;
    ::GetObject(g_FontData.GetFontWordEx15(), sizeof(LOGFONT), &lg);
    if (g_pLanguage->LangId() == ENU)
    {
        lg.lfHeight = 18;
    }
    else
    {
        lg.lfHeight = 20;
    }
    lg.lfWeight = FW_BOLD;
    wcsncpy_s(lg.lfFaceName, L"Microsoft Yahei", _TRUNCATE);
    m_font = CreateFontIndirect(&lg);
}

StatusBarWnd::~StatusBarWnd()
{
    DeleteObject(m_brushBg);
}

void StatusBarWnd::Create()
{
    CreateFrm(L"StatusButton", 0, WS_VISIBLE | WS_POPUP, WS_EX_TOOLWINDOW);
    SetWindowPos(m_Hwnd, 0, 0, 0, 80, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

LRESULT StatusBarWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        OnPaint();
        break;
    case WM_LBUTTONDOWN:
        OnLButtonDown();
        break;
    case WM_ERASEBKGND:
        break;
    default:
        return NOT_PROCESSED;
    }
    return PROCESSED;
}


void StatusBarWnd::OnPaint()
{
    HDC hdc;
    PAINTSTRUCT ps;
    hdc = BeginPaint(m_Hwnd, &ps);
    RECT rect;
    GetClientRect(m_Hwnd, &rect);
    FillRect(hdc, &rect, m_brushBg);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, m_font);
    SetTextColor(hdc, m_textColor);
    DrawText(hdc, LoadResString(IDS_SW_Monitor).c_str(), wcslen(LoadResString(IDS_SW_Monitor).c_str()), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

    EndPaint(m_Hwnd, &ps);
}

void StatusBarWnd::OnSize()
{
    RECT rect;
    GetClientRect(m_Hwnd, &rect);
    MoveWindow(m_Hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

void StatusBarWnd::OnLButtonDown()
{
    g_conFrame->show(LoadResString(IDS_SW_Monitor).c_str(), cmtExpand);
}
