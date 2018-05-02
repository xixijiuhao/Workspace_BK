#ifndef _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_

#define SSWM_STATIC_BUTTON_CLICKDOWN   WM_USER + 1
#define SSWM_TAB_SWITCH_FOCUS          WM_USER + 2
#define SSWM_EDIT_PRESS_ENTER          WM_USER + 3
#define SSWM_CLICK_CHECKBOX            WM_USER + 4

#define WM_ONRESETUSERNO                 WM_USER + 5
#define WM_ONQUOTE                       WM_USER + 6
#define WM_ONORDER                       WM_USER + 7
#define WM_ONPOSITION                    WM_USER + 8
#define WM_ONMATCH                       WM_USER + 9
#define WM_ONPARORDER                    WM_USER + 10
#define WM_ONSHIFTLIST					 WM_USER + 11
#define WM_SELECTSHIFTLIST               WM_USER + 12
#define WM_LIST_LBUTTONDOWN              WM_USER + 13
#define SSWM_TABTAG_SELECTED             WM_USER + 14
 
#define SSWM_COMBOX_CHECK_CHANAGE		 WM_USER + 15
#define SSWM_COMBOX_SELECT		 WM_USER + 16

#define UIDesgin_INIT   g_FontData.Init();g_ColorRefData.Init()

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;

extern ITradeApi		* g_pTradeApi;
extern ITradeData		* g_pTradeData;
extern IStarApi			* g_pStarApi;
extern IMainFrame		* g_pMainFrame;

#endif