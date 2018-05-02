#ifndef _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_

#define SSWM_STATIC_BUTTON_CLICKDOWN   WM_USER + 1
#define SSWM_TAB_SWITCH_FOCUS          WM_USER + 2
#define SSWM_EDIT_PRESS_ENTER          WM_USER + 3
#define SSWM_CLICK_CHECKBOX			   WM_USER + 4
#define SSWM_ORDERBUTTON_UP            WM_USER + 5
#define SSWM_COMBOX_CHECK_CHANAGE      WM_USER + 6
#define SSWM_COMBOX_SELECT             WM_USER + 7
#define SSWM_TABTAG_SELECTED           WM_USER + 8
#define SSWM_HTTP_SPI					WM_USER + 9




#define UIDesgin_INIT   g_FontData.Init();g_ColorRefData.Init()


#define WM_RESETUSERNO                      WM_USER + 18
#define WM_NOTICE                           WM_USER + 19
#define WM_ONQUOTE                          WM_USER + 20
#define WM_POSITION                         WM_USER + 21
#define WM_SUPERPRICE                       WM_USER + 22
#define WM_ONCALLBACK                       WM_USER + 23
#define WM_BETALIMIT						WM_USER + 24

#define SDWM_ONCONTRACT_SELECT				WM_USER + 25  //self define window message
#define SDWM_CLICK_LISTBOX_ITEM				WM_USER + 26
#define MainFrmName L"class TMainFrame"
#define mulContractFilePath "\\Config\\MulContract.pri"

//#define 
#define atNone             'N'
#define atInnerOne        'o'
#define atForeignOne	  'O'
#define atOnlyInnerLess3   'i'
#define atOnlyForeignLess3 'f'
#define atInnerForeignLess3 's'
#define atOnlyInner  'I'
#define atOnlyForeign 'F'
#define atInnerForeign 'S'






extern G_COLOREF		g_ColorRefData;
extern G_UIFONT			g_FontData;
extern ITradeApi *		g_pTradeApi;
extern ITradeData *		g_pTradeData;
extern IMainFrame*		g_pMainFrm;
extern IStarApi*		g_pQuoteApi;
extern TPluginMgrApi*	g_pPluginMgrApi;
extern ILanguageApi*	g_pLanguageApi;
extern LanguageID		g_language;

enum StrID
{
	HighFrequencyStrID = 0,
	IntradayStrID,
	ShortTermStrID,
	MidTermStrID,
	TitleStrID,
	CommodityStrID,
	ForeignExchangeStrID,
	NoDataTipStrID,
	GettingDataStrID
};



#endif