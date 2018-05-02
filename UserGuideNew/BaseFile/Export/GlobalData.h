#ifndef _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_

#define SSWM_STATIC_BUTTON_CLICKDOWN   WM_USER + 1
#define SSWM_TAB_SWITCH_FOCUS          WM_USER + 2
#define SSWM_EDIT_PRESS_ENTER          WM_USER + 3
#define SSWM_CLICK_CHECKBOX            WM_USER + 4
#define SSWM_NOTICE                    WM_USER + 5

#define SSWM_ONRESETUSERNO			   WM_USER + 101
#define SSWM_QUOTE						WM_USER + 102
#define SSWM_HOTKEYDOWN					WM_USER + 103


#define UIDesgin_INIT   g_FontData.Init();g_ColorRefData.Init()

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;

extern IMainFrame*  g_pMainFrame;

#define MainFrmName L"class TMainFrame"

#define allContractKey "ALL"
#define allContractKeyW L"ALL"

#define dataFilePath      L"\\config\\ConOrder.pri"
#define dataAppName       L"ConOrder"
#define hotKeyAppName     L"HotKey"

#endif