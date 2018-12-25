#ifndef _T_EXPORT_H_
#define _T_EXPORT_H_

extern ITradeApi * g_pTradeApi;
extern ITradeData * g_pTradeData;
extern TPluginMgrApi*g_pPluginMgrApi;

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;
extern IStarApi*g_pStarApi;
extern LanguageID g_language;

extern IAuthent  * g_pAuthent;
extern ILoggingApi* g_pLogging;
extern IMainFrame * g_pMainFrm;

#define SSWM_STATIC_BUTTON_CLICKDOWN		WM_USER + 10
#define SSWM_TAB_SWITCH_FOCUS				WM_USER + 11
#define SSWM_COMBOX_SELECT					WM_USER + 12
#define SSWM_COMBOX_CHECK_CHANAGE           WM_USER + 13
#define SSWM_PINEDIT_UP                     WM_USER + 14
#define SSWM_PINEDIT_DOWN                   WM_USER + 15
#define SSWM_EDIT_PRESS_ENTER				WM_USER + 16
#define SSWM_CLICK_CHECKBOX					WM_USER + 17


#define RemarkCoverByUser					"Ö÷¶¯Æ½²Ö"		
#define  FollowConfigFilePath "\\config\\TraderCopier.pri"
#define  NightTradeTimeFilePath "\\config\\NightTradeTime.pri"

#endif