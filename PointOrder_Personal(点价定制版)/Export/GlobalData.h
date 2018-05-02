#ifndef _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_

#define SSWM_STATIC_BUTTON_CLICKDOWN   WM_USER + 1
#define SSWM_TAB_SWITCH_FOCUS          WM_USER + 2
#define SSWM_EDIT_PRESS_ENTER          WM_USER + 3
#define SSWM_CLICK_CHECKBOX            WM_USER + 4


#define UIDesgin_INIT   g_FontData.Init();g_ColorRefData.Init()

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;

extern ITradeApi * g_pTradeApi;
extern ITradeData * g_pTradeData;
extern IMainFrame*  g_pMainFrame;


#endif