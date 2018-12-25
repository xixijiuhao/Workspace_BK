/**
* =====================================================================
*
*       [filename    ] MouseOrderConfigWnd.cpp
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-07-14
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#include "PreInclude.h"

extern G_UIFONT g_FontData;			//字体
extern G_COLOREF g_ColorRefData;	//色彩
extern TMouseConfig g_MouseOrderStyle;        //点价样式

extern void WriteConfig(const char *key, const char *value);

MouseConfig::MouseConfig()
{
    m_cfgWnd = new MouseOrderConfigWnd;
}

MouseConfig::~MouseConfig()
{
    delete m_cfgWnd;
}

void MouseConfig::Subscribe(IOrderConfig * observer)
{
    if (m_cfgWnd)
    {
        m_cfgWnd->Subscribe(observer);
    }
}

void MouseConfig::UnSubscribe(IOrderConfig * observer)
{
    if (m_cfgWnd)
    {
        m_cfgWnd->UnSubscribe(observer);
    }
}

HWND MouseConfig::GetConfigWnd()
{
    if (m_cfgWnd)
    {
        return m_cfgWnd->GetHwnd();
    }
    return NULL;
}

void MouseConfig::CreateCfgWnd(const wchar_t * title)
{
    if (m_cfgWnd)
    {
        m_cfgWnd->Create(title, NULL, WS_OVERLAPPED, 0);
    }
}


MouseOrderConfigWnd::MouseOrderConfigWnd()
{
   
}

MouseOrderConfigWnd::~MouseOrderConfigWnd()
{
    
}

void MouseOrderConfigWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
    CreateFrm(classname, parent, style, exstyle);
}

void MouseOrderConfigWnd::Subscribe(IOrderConfig * observer)
{
    if (observer)
    {
        m_listObservers.push_back(observer);
    }
}

void MouseOrderConfigWnd::UnSubscribe(IOrderConfig * observer)
{
    for (auto it : m_listObservers)
    {
        if (it == observer)
        {
            m_listObservers.remove(it);
            break;
        }
    }
}

LRESULT MouseOrderConfigWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        OnCreateWindow();
        break;
    case WM_COMMAND:
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            OnColorStyleChanged(LOWORD(wParam));
        }
        break;
    case WM_DESTROY:
        SaveConfig();
        break;
    default:
        return NOT_PROCESSED;
        break;
    }
    return PROCESSED;
}

void MouseOrderConfigWnd::OnCreateWindow()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hStatic = CreateWindowEx(0, L"static", LoadResWchar_t(IDS_Ladder_Style), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
        10, 12, 80, 22, m_Hwnd, (HMENU)0, hInstance, NULL);
    SendMessage(hStatic, WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);
    HWND hCombobox = CreateWindowEx(0, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP | CBS_AUTOHSCROLL,
        100, 10, 100, 200, m_Hwnd, (HMENU)1, hInstance, NULL);

    SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)LoadResWchar_t(IDS_Ladder_Default));
    SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)LoadResWchar_t(IDS_Ladder_Light));
    SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)LoadResWchar_t(IDS_Ladder_Dark));
    SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)LoadResWchar_t(IDS_Ladder_Light_Yellow));
    SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)LoadResWchar_t(IDS_Ladder_Light_Blue));
    SendMessage(hCombobox, CB_ADDSTRING, 0, (LPARAM)LoadResWchar_t(IDS_Ladder_Light_Green));
    SendMessage(hCombobox, WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);
    SendMessage(hCombobox, CB_SETCURSEL, g_MouseOrderStyle.nOrderListStyle, 0);
	m_ComboxStyle.Init(hCombobox);

	HWND hStatic2 = CreateWindowEx(0, L"static", LoadResWchar_t(IDS_ParQtyText), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		230, 12, 80, 22, m_Hwnd, (HMENU)2, hInstance, NULL);
	SendMessage(hStatic2, WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);

	HWND hCombobox2 = CreateWindowEx(0, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP | CBS_AUTOHSCROLL,
		310, 10, 100, 200, m_Hwnd, (HMENU)3, hInstance, NULL);
	SendMessage(hCombobox2, CB_ADDSTRING, 0, (LPARAM)LoadResWchar_t(IDS_ParQtyRight));
	SendMessage(hCombobox2, CB_ADDSTRING, 0, (LPARAM)LoadResWchar_t(IDS_ParQtyBoth));
	SendMessage(hCombobox2, WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);
	SendMessage(hCombobox2, CB_SETCURSEL, g_MouseOrderStyle.nParqtyStyle, 0);
	m_ComboxParQty.Init(hCombobox2);
}

void MouseOrderConfigWnd::OnColorStyleChanged(int nID)
{
	switch (nID)
	{
	case EN_ListStyle_Combox:	g_MouseOrderStyle.nOrderListStyle = ComboBox_GetCurSel(m_ComboxStyle.GetHwnd());
		break;
	case EN_ParQtyStyle_Combox:	g_MouseOrderStyle.nParqtyStyle = ComboBox_GetCurSel(m_ComboxParQty.GetHwnd());
		break;
	default:
		break;
	}
   
    for (auto it : m_listObservers)
    {
        it->OnLadderStyleChanged();
    }
}

void MouseOrderConfigWnd::SaveConfig()
{
    char strIdx[8];
    _itoa_s(g_MouseOrderStyle.nOrderListStyle, strIdx, 10);
    WriteConfig("LadderStyle", strIdx);
	_itoa_s(g_MouseOrderStyle.nParqtyStyle, strIdx, 10);
	WriteConfig("ParqtyStyle", strIdx);
}

