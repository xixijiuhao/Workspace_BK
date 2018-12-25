/**
* =====================================================================
*
*       [filename    ] SWRiskConfigWnd.cpp
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-10-13
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#include "SWRiskConfigWnd.h"
#include "Language.h"
#include "UIDesign.h"
#include "../share/AnsicUnicode.h"
#include "../share/TradeQuoteString.h"
#include "Global.h"
#include "load_rc.h"

#include <fstream>

extern G_COLOREF g_ColorRefData;
extern G_UIFONT	g_FontData;
extern IStarApi	*g_StarApi;

int N_Group_Width = 500;

#define CONFIG_FILE					"config\\SWRiskMonitor.pri"

#define SECTION_CFG                 "config"
#define NODE_MAX_OPEN_QTY           "max_open_qty"
#define NODE_CANCEL_THRESHOLD       "cancel_threshold"
#define NODE_CONTRACT               "contract"
#define NODE_POSITION_RANGE         "position_range"

LRESULT CALLBACK ContractListViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int id = GetWindowLong(hwnd, GWL_ID);
    SWRiskConfigWnd *pWnd = (SWRiskConfigWnd*)GetWindowLong(hwnd, GWL_USERDATA);
    if (!pWnd)
        return false;
    switch (message)
    {
    case WM_DRAWITEM:
    {
        DRAWITEMSTRUCT *pDraw = (DRAWITEMSTRUCT*)(lParam);
        std::wstring wstr = TEXT("");
        if (id == ID_ListView_Contract)
        {
            switch (pDraw->itemID)
            {
            case 0:
                wstr = LoadResString(IDS_Contract);
                break;
            case 1:
                wstr = LoadResString(IDS_Position_Range);
                break;
            }
        }

        RECT rect = pDraw->rcItem;
        InflateRect(&rect, 1, 1);
        FillRect(pDraw->hDC, &rect, g_ColorRefData.GetHbrRGB150());
        FrameRect(pDraw->hDC, &rect, g_ColorRefData.GetHbrBackground());
        SetTextColor(pDraw->hDC, g_ColorRefData.GetColorWhite());
        SetBkMode(pDraw->hDC, TRANSPARENT);
        DrawText(pDraw->hDC, wstr.c_str(), wstr.size(), &pDraw->rcItem, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
        return true;
    }
    break;
    case WM_NOTIFY:
    {
        NMHDR *pNMHDR = (NMHDR *)lParam;
        LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
        switch (pNMListView->hdr.code)
        {
        case HDN_BEGINTRACKW:
        case HDN_BEGINTRACKA:
            return true;
        case HDN_DIVIDERDBLCLICK:
            return true;
        }
    }
    break;
    }
    if (id == ID_ListView_Contract)
        return CallWindowProc(pWnd->m_procListview, hwnd, message, wParam, lParam);
    else
        return false;
}

SWRiskConfigWnd::SWRiskConfigWnd()
    : m_bCheckedPwd(false)
{
    LoadConfig();

    m_password = "@ES2017";
}

void SWRiskConfigWnd::CreateCfgWnd(const wchar_t * title)
{
    CreateFrm(title, NULL, WS_OVERLAPPED, 0);
}

void SWRiskConfigWnd::SetContractNo(string sCode)
{
    SetWindowTextA(m_hControl[ID_Edit_Contract], sCode.c_str());
}

void SWRiskConfigWnd::SetPassword(const std::string strPwd)
{
    m_password = strPwd;
}

LRESULT SWRiskConfigWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        OnCreate();
        break;
    case WM_CTLCOLORSTATIC:			
        return (LONG)m_hStaticBKBrush;
    case WM_PAINT:
        OnPaint();
        break;
    case WM_COMMAND:
        OnCommand(wParam, lParam);
        break;
    case WM_DESTROY:
        SaveConfig();
        break;
    case WM_NOTIFY:
        OnNotify(wParam, lParam);
        break;
    default:
        return NOT_PROCESSED;
    }

    return PROCESSED;
}

void SWRiskConfigWnd::OnCreate()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    m_hStaticBKBrush = g_ColorRefData.GetHbrWhite();//背景色

    int nLeftStart = 12;
    int nStaticWidth = 80;
    int nEditWidth = 100;
    int nEditHeight = 22;
    int nButtonHeight = 22;
    int nButtonWidth = 50;
    int nTop = 10;
    int nInterval = 8;

    nLeftStart = 10;
    nTop = 3;
    //密码
    m_hControl[ID_Static_Password] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_Password).c_str(),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTERIMAGE | SS_NOTIFY | SS_LEFT,
        nLeftStart, nTop, 50, nEditHeight, m_Hwnd, (HMENU)ID_Static_Password, hInstance, NULL);
    nLeftStart += 50 + 2;
    m_hControl[ID_Edit_Password] = CreateWindowEx(0, L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_PASSWORD,
        nLeftStart, nTop, nEditWidth, nEditHeight, m_Hwnd, (HMENU)ID_Edit_Password, hInstance, NULL);
    nLeftStart += nEditWidth + 5;
    m_hControl[ID_Button_Check_Pwd] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Check_Password).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        nLeftStart, nTop, 80, nButtonHeight, m_Hwnd, (HMENU)ID_Button_Check_Pwd, hInstance, NULL);

    //最大开仓次数风控
    nLeftStart = 32;
    nTop = 45;
    m_hControl[ID_Static_Max_Open] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_Max_Open_Qty).c_str(),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTERIMAGE | SS_NOTIFY | SS_LEFT,
        nLeftStart, nTop + 19, nStaticWidth, nEditHeight, m_Hwnd, (HMENU)ID_Static_Max_Open, hInstance, NULL);
    nLeftStart += nStaticWidth + 2;
    m_hControl[ID_Edit_Max_Open] = CreateWindowEx(0, L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER,
        nLeftStart, nTop + 19, nEditWidth, nEditHeight, m_Hwnd, (HMENU)ID_Edit_Max_Open, hInstance, NULL);
    m_hControl[ID_Groupbox_Open] = CreateWindowEx(0, L"button", LoadResString(IDS_Open_Monitor).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        10, nTop, N_Group_Width, 50, m_Hwnd, (HMENU)ID_Groupbox_Open, hInstance, NULL);

    //回撤风控
    nLeftStart = 32;
    nTop = 77 + 45;
    m_hControl[ID_Static_Cancel] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_Threshold).c_str(),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTERIMAGE | SS_NOTIFY | SS_LEFT,
        nLeftStart, nTop, nStaticWidth, nEditHeight, m_Hwnd, (HMENU)ID_Static_Cancel, hInstance, NULL);
    nLeftStart += nStaticWidth + 2;
    m_hControl[ID_Edit_Cancel] = CreateWindowEx(0, L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER,
        nLeftStart, nTop, nEditWidth, nEditHeight, m_Hwnd, (HMENU)ID_Edit_Cancel, hInstance, NULL);
    nLeftStart += nEditWidth + 2;
    m_hControl[ID_Static_Cancel_Label] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_Cancel_Label).c_str(),
        WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
        nLeftStart, nTop, 170, nEditHeight, m_Hwnd, (HMENU)ID_Static_Cancel_Label, hInstance, NULL);
    m_hControl[ID_Groupbox_Cancel] = CreateWindowEx(0, L"button", LoadResString(IDS_Cancel_Monitor).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 
        10, 60 + 45, N_Group_Width, 50, m_Hwnd, (HMENU)ID_Groupbox_Cancel, hInstance, NULL);

    //仓差风控
    nLeftStart = 32;
    nTop = 140 + 45;

    m_hControl[ID_ListView_Contract] = CreateWindowEx(0, TEXT("SysListView32"),
        TEXT("List view"), WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT,
        nLeftStart, nTop, N_Group_Width - 48, 240,
        m_Hwnd, (HMENU)ID_ListView_Contract, hInstance, NULL);
    ListView_SetExtendedListViewStyleEx(m_hControl[ID_ListView_Contract], NULL, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT/**/);
    ListView_SetSelectedColumn(m_hControl[ID_ListView_Contract], 2);
    ListView_SetBkColor(m_hControl[ID_ListView_Contract], g_ColorRefData.GetColorWhite());
    ListView_SetTextBkColor(m_hControl[ID_ListView_Contract], g_ColorRefData.GetColorWhite());
    LV_COLUMN LvCol;

    memset(&LvCol, 0, sizeof(LvCol));							// Zero Members
    LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;				// Type of mask
    LvCol.fmt = LVCFMT_CENTER;
    LvCol.cx = N_Group_Width - 148;

    LvCol.pszText = (LPWSTR)LoadResString(IDS_Contract).c_str();
    ListView_InsertColumn(m_hControl[ID_ListView_Contract], 0, &LvCol);

    LvCol.cx = 100;
    LvCol.pszText = (LPWSTR)LoadResString(IDS_Position_Range).c_str();
    ListView_InsertColumn(m_hControl[ID_ListView_Contract], 1, &LvCol);

    nTop += 240 + 5;

    m_hControl[ID_Edit_Contract] = CreateWindowEx(0, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 
        nLeftStart, nTop, 150, nEditHeight, m_Hwnd, (HMENU)ID_Edit_Position_Minus, hInstance, NULL);
    nLeftStart += 150 + 4;
    m_hControl[ID_Button_Select_Contract] = CreateWindowEx(0, TEXT("button"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
        nLeftStart, nTop, 34, nEditHeight, m_Hwnd, (HMENU)ID_Button_Select_Contract, hInstance, NULL);
    SendMessage(m_hControl[ID_Edit_Contract], EM_SETLIMITTEXT, 51, 0);

    nLeftStart += nButtonWidth;
    m_hControl[ID_Static_Position_Minus] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_Position_Range).c_str(),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTERIMAGE | SS_NOTIFY | SS_RIGHT,
        nLeftStart, nTop, nStaticWidth, nEditHeight, m_Hwnd, (HMENU)ID_Static_Position_Minus, hInstance, NULL);
    nLeftStart += nStaticWidth + 4;
    m_hControl[ID_Edit_Position_Minus] = CreateWindowEx(0, L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER,
        nLeftStart, nTop, nStaticWidth, nEditHeight, m_Hwnd, (HMENU)ID_Edit_Position_Minus, hInstance, NULL);
    nLeftStart += nStaticWidth + 2;
    m_hControl[ID_Static_Position_Label] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_Position_Label).c_str(),
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_CENTERIMAGE | SS_NOTIFY | SS_LEFT,
        nLeftStart, nTop, 120, nEditHeight, m_Hwnd, (HMENU)ID_Static_Position_Label, hInstance, NULL);
    SendMessage(m_hControl[ID_Edit_Position_Minus], EM_SETLIMITTEXT, 3, 0);

    nLeftStart = 32;
    nTop += nEditHeight + 4;
    m_hControl[ID_Button_Add] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Add).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
        nLeftStart, nTop, 50, nButtonHeight, m_Hwnd, (HMENU)ID_Button_Add, hInstance, NULL);
    m_hControl[ID_Button_Delete] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Remove).c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
        nLeftStart + 69, nTop, 50, nButtonHeight, m_Hwnd, (HMENU)ID_Button_Delete, hInstance, NULL);

    m_hControl[ID_Groupbox_Position] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Position_Monitor).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 
        10, 120 + 45, N_Group_Width, 320, m_Hwnd, (HMENU)ID_Groupbox_Position, hInstance, NULL);

    m_EditContract.Init(m_hControl[ID_Edit_Contract]);
    m_EditCancel.Init(m_hControl[ID_Edit_Cancel]);
    m_EditOpen.Init(m_hControl[ID_Edit_Max_Open]);
    m_EditPosition.Init(m_hControl[ID_Edit_Position_Minus]);
    m_EditPassword.Init(m_hControl[ID_Edit_Password]);

    m_CheckPwdBtn.Init(m_hControl[ID_Button_Check_Pwd]);
    m_AddBtn.Init(m_hControl[ID_Button_Add]);
    m_DeleteBtn.Init(m_hControl[ID_Button_Delete]);
    m_SelectBtn.Init(m_hControl[ID_Button_Select_Contract]);
    m_SelectBtn.SetButtonText(TEXT("..."));

    InitListView();
    UpdateListView();

    //设置控件字体
    for (int i = ID_Begin + 1; i < ID_End; i++)
    {
        if (m_hControl[i] != 0)
        {
            SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
        }
    }

    SendMessage(m_EditPosition.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);
    SendMessage(m_EditOpen.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);
    SendMessage(m_EditCancel.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);
    SendMessage(m_EditContract.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);
    SendMessage(m_EditPassword.GetHwnd(), WM_SETFONT, (WPARAM)g_FontData.GetFontWord15(), (LPARAM)TRUE);

    if (m_cfgData.m_nMaxOpenQty >= 0)
    {
        SetWindowTextAnsic(m_hControl[ID_Edit_Max_Open], to_string(m_cfgData.m_nMaxOpenQty));
    }
    if (m_cfgData.m_nThreshold > 0)
    {
        SetWindowTextAnsic(m_hControl[ID_Edit_Cancel], to_string(m_cfgData.m_nThreshold));
    }

    EnableCtrl(false);
}

void SWRiskConfigWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case ID_Button_Select_Contract:
        if (HIWORD(wParam) == BN_CLICKED)
            OnBtnSelect();
        break;
    case ID_Button_Add:
        if (HIWORD(wParam) == BN_CLICKED)
            OnBtnAdd();
        break;
    case ID_Button_Delete:
        if (HIWORD(wParam) == BN_CLICKED)
            OnBtnDelete();
        break;
    case ID_Button_Check_Pwd:
        if (HIWORD(wParam) == BN_CLICKED)
            OnBtnCheckPwd();
        break;
    default:
        break;
    }
}

void SWRiskConfigWnd::OnPaint()
{
    HDC			hdc;
    PAINTSTRUCT ps;
    hdc = BeginPaint(m_Hwnd, &ps);
    RECT item;
    GetClientRect(m_Hwnd, &item);

    FillRect(hdc, &item, g_ColorRefData.GetHbrWhite());
    EndPaint(m_Hwnd, &ps);
}

void SWRiskConfigWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
    if ((int)wParam == ID_ListView_Contract)
    {
        NMHDR *pNMHDR = (NMHDR *)lParam;
        LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
        switch (pNMHDR->code)
        {
        case NM_DBLCLK:
        {
            LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
            wchar_t wtext[100] = { 0 };
            LVITEM lvitem;
            lvitem.mask = LVIF_TEXT;
            lvitem.iItem = lpnmitem->iItem;
            lvitem.iSubItem = 0;
            lvitem.cchTextMax = sizeof(wtext) / sizeof(wtext[0]);
            lvitem.pszText = wtext;
            ListView_GetItem(m_hControl[ID_ListView_Contract], &lvitem);
            SetWindowText(m_hControl[ID_Edit_Contract], wtext);

            lvitem.iSubItem = 1;
            ListView_GetItem(m_hControl[ID_ListView_Contract], &lvitem);
            SetWindowText(m_hControl[ID_Edit_Position_Minus], wtext);
        }
        break;
        }
    }
}

void SWRiskConfigWnd::OnBtnSelect()
{
    RECT rectvalid;
    GetWindowRect(m_hControl[ID_Button_Select_Contract], &rectvalid);
    if (!IsWindow(m_popWinCode.GetHwnd()))
    {

        m_popWinCode.CreatePopWin(this, TEXT("m_pPopWinCode"), m_Hwnd, WS_CLIPCHILDREN, 0, rectvalid.left - 64 + 3, rectvalid.bottom);
        m_popWinCode.UpdateData();
        ::ShowWindow(m_popWinCode.GetHwnd(), SW_SHOWNORMAL);
    }
    else
    {
        m_popWinCode.UpdateData();
        SetWindowPos(m_popWinCode.GetHwnd(), NULL, rectvalid.left - 64 + 3, rectvalid.bottom, 0, 0, SWP_NOSIZE);
        ::ShowWindow(m_popWinCode.GetHwnd(), SW_SHOWNORMAL);
    }
}

void SWRiskConfigWnd::OnBtnAdd()
{
    char sText[51] = { 0 };
    GetWindowTextA(m_hControl[ID_Edit_Position_Minus], sText, 51);
    int nRange = atoi(sText);
    if (nRange <= 0)
    {
        MessageBox(m_Hwnd, LoadResString(IDS_Invalid_Position).c_str(), LoadResString(IDS_Tips).c_str(), NULL);
        return;
    }

    GetWindowTextA(m_hControl[ID_Edit_Contract], sText, 51);

    string sCode = sText;
    CConvert::TrimString(sCode);
    //判断是否是合法的交易合约
    TContractKey tempContract;
    memset(&tempContract, 0, sizeof(tempContract));
    if (!CConvert::TContractNoToTContractKey(sCode.c_str(), tempContract))
    {
        wchar_t msg[MAX_PATH];
        swprintf_s(msg, LoadResString(IDS_Invalid_Contract).c_str(), CConvert::UTF8ToUnicode(sCode).c_str());
        MessageBox(m_Hwnd, msg, LoadResString(IDS_Tips).c_str(), NULL);
        return;
    }
    if (g_StarApi)
    {
        SContractNoType typeId;
        typeId[0] = '\0';
        CConvert::TradeContractToSContractNoType(tempContract, typeId);

        if (tempContract.CommodityType == ctGoods)//合约到品种
        {
            if (!CConvert::IsValidQuoteCommodity(tempContract.ExchangeNo, tempContract.CommodityNo))
            {
                wchar_t msg[MAX_PATH];
                swprintf_s(msg, LoadResString(IDS_Invalid_Contract).c_str(), CConvert::UTF8ToUnicode(sCode).c_str());
                MessageBox(m_Hwnd, msg, LoadResString(IDS_Tips).c_str(), NULL);
                return;
            }
        }
        else//到合约
        {
            if (!CConvert::IsValidQuotContract(typeId))
            {
                wchar_t msg[MAX_PATH];
                swprintf_s(msg, LoadResString(IDS_Invalid_Contract).c_str(), CConvert::UTF8ToUnicode(sCode).c_str());
                MessageBox(m_Hwnd, msg, LoadResString(IDS_Tips).c_str(), NULL);
                return;
            }
        }

        m_cfgData.m_mapPositonRange[sCode] = nRange;
        
        UpdateListView();
    }
}

void SWRiskConfigWnd::OnBtnDelete()
{
    UINT count = ListView_GetSelectedCount(m_hControl[ID_ListView_Contract]);
    if (count <= 0)
        return;

    wchar_t cc[101] = { 0 };
    while (ListView_GetNextItem(m_hControl[ID_ListView_Contract], -1, LVNI_ALL | LVNI_SELECTED) != -1)
    {
        int nItem = ListView_GetNextItem(m_hControl[ID_ListView_Contract], -1, LVNI_ALL | LVNI_SELECTED);
        ListView_GetItemText(m_hControl[ID_ListView_Contract], nItem, 0, cc, sizeof(cc) / sizeof(cc[0]));
        m_cfgData.m_mapPositonRange.erase(LoadRC::unicode_to_ansi(cc).c_str());
        ListView_DeleteItem(m_hControl[ID_ListView_Contract], nItem);
    }
}

void SWRiskConfigWnd::OnBtnCheckPwd()
{
    char sText[10] = { 0 };
    GetWindowTextA(m_hControl[ID_Edit_Password], sText, 10);

    if (strcmp(sText, m_password.c_str()) == 0)
    {
        EnableCtrl(true);
        m_bCheckedPwd = true;
    }
    else
    {
        MessageBox(m_Hwnd, LoadResString(IDS_Error_Password).c_str(), LoadResString(IDS_Tips).c_str(), NULL);
    }
}

void SWRiskConfigWnd::InitListView()
{
    SetWindowLong(ListView_GetHeader(m_hControl[ID_ListView_Contract]), GWL_STYLE, HDS_FLAT);

    for (int i = 0; i < 2; i++)
    {
        HDITEM hditem;
        hditem.mask = HDI_FORMAT;
        Header_GetItem(ListView_GetHeader(m_hControl[ID_ListView_Contract]), i, &hditem);
        hditem.fmt = hditem.fmt | HDF_OWNERDRAW | HDF_FIXEDWIDTH;
        Header_SetItem(ListView_GetHeader(m_hControl[ID_ListView_Contract]), i, &hditem);
    }

    m_procListview = (WNDPROC)SetWindowLong(m_hControl[ID_ListView_Contract], GWL_WNDPROC, (LONG)ContractListViewProc);
    SetWindowLong(m_hControl[ID_ListView_Contract], GWL_USERDATA, LONG(this));
}

void SWRiskConfigWnd::SaveConfig()
{
    if (!m_bCheckedPwd)
    {
        return;
    }
    std::string cfg_file = FileFun::GetAbsolutePathForExe(CONFIG_FILE);
    //清空内容
    std::ofstream ofs;
    ofs.open(cfg_file, std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    char sText[10] = { 0 };
    GetWindowTextA(m_hControl[ID_Edit_Max_Open], sText, 10);
    if (strcmp(sText, "") != 0)
    {
        m_cfgData.m_nMaxOpenQty = atoi(sText);
    }
    else
    {
        m_cfgData.m_nMaxOpenQty = -1;
    }
    GetWindowTextA(m_hControl[ID_Edit_Cancel], sText, 10);
    if (strcmp(sText, "") != 0)
    {
        m_cfgData.m_nThreshold = atoi(sText);
    }
    else
    {
        m_cfgData.m_nThreshold = -1;
    }
    
    std::string value = m_cfgData.m_nMaxOpenQty >= 0 ? to_string(m_cfgData.m_nMaxOpenQty) : "";
    WritePrivateProfileStringA(SECTION_CFG, NODE_MAX_OPEN_QTY, value.c_str(), cfg_file.c_str());
    value = m_cfgData.m_nThreshold > 0 ? to_string(m_cfgData.m_nThreshold) : "";
    WritePrivateProfileStringA(SECTION_CFG, NODE_CANCEL_THRESHOLD, value.c_str(), cfg_file.c_str());
    
    int i = 0;
    std::string strKey = "";
    for (auto it : m_cfgData.m_mapPositonRange)
    {
        strKey = NODE_CONTRACT + to_string(i);
        WritePrivateProfileStringA(SECTION_CFG, strKey.c_str(), it.first.c_str(), cfg_file.c_str());
        strKey = NODE_POSITION_RANGE + to_string(i);
        WritePrivateProfileStringA(SECTION_CFG, strKey.c_str(), to_string(it.second).c_str(), cfg_file.c_str());
        i++;
    }

}

void SWRiskConfigWnd::LoadConfig()
{
    std::string cfg_file = FileFun::GetAbsolutePathForExe(CONFIG_FILE);
    m_cfgData.m_nMaxOpenQty = GetPrivateProfileIntA(SECTION_CFG, NODE_MAX_OPEN_QTY, -1, cfg_file.c_str());
    m_cfgData.m_nThreshold = GetPrivateProfileIntA(SECTION_CFG, NODE_CANCEL_THRESHOLD, -1, cfg_file.c_str());

    std::string strKey = "";
    char contract[101];
    char value[20];
    for (int i = 0; ; i++)
    {
        strKey = NODE_CONTRACT + to_string(i);
        GetPrivateProfileStringA(SECTION_CFG, strKey.c_str(), "", contract, 101, cfg_file.c_str());
        if (strcmp(contract, "") == 0)
        {
            break;
        }
        strKey = NODE_POSITION_RANGE + to_string(i);
        GetPrivateProfileStringA(SECTION_CFG, strKey.c_str(), "", value, 20, cfg_file.c_str());
        if (strcmp(value, "") == 0)
        {
            break;
        }
        m_cfgData.m_mapPositonRange[std::string(contract)] = atoi(value);
    }
}

void SWRiskConfigWnd::UpdateListView()
{
    wchar_t cc[101] = { 0 };
    ListView_DeleteAllItems(m_hControl[ID_ListView_Contract]);
    int i = 0;
    for (auto it : m_cfgData.m_mapPositonRange)
    {
        wchar_t text[70] = { 0 };
        wcscpy_s(text, LoadRC::ansi_to_unicode(it.first).c_str());
        LV_ITEM ltm;
        ltm.mask = LVIF_TEXT | LVIF_IMAGE;
        ltm.pszText = text;
        ltm.iItem = i;
        ltm.iSubItem = 0;
        ListView_InsertItem(m_hControl[ID_ListView_Contract], &ltm);
        swprintf_s(cc, TEXT("%d"), it.second);
        ListView_SetItemText(m_hControl[ID_ListView_Contract], i, 1, cc);
        i++;
    }
}

void SWRiskConfigWnd::EnableCtrl(bool bEnable)
{
    EnableWindow(m_EditCancel.GetHwnd(), bEnable);
    EnableWindow(m_EditContract.GetHwnd(), bEnable);
    EnableWindow(m_EditOpen.GetHwnd(), bEnable);
    EnableWindow(m_EditPosition.GetHwnd(), bEnable);

    EnableWindow(m_SelectBtn.GetHwnd(), bEnable);
    EnableWindow(m_DeleteBtn.GetHwnd(), bEnable);
    EnableWindow(m_AddBtn.GetHwnd(), bEnable);
    EnableWindow(m_EditCancel.GetHwnd(), bEnable);
}
