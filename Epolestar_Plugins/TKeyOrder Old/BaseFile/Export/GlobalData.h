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

#define SSWM_NOTICE                    WM_USER + 9
#define SSWM_MONEY                     WM_USER + 12
#define SSWM_POSITIIONTOTAL			   WM_USER + 13


//stopLP model
#define SSWM_STOPLP_ATTACH				WM_USER + 30
#define SSWM_STOPLP_ONQUOTE				WM_USER + 31
#define SSWM_STOPLP_INSERT_ORDER		WM_USER + 32
#define SSWM_STOPLP_DELETE_ORDER		WM_USER + 33
#define SSWM_STOPLP_MODIFY_ORDER		WM_USER + 34
#define SSWM_STOPLP_TRIG_ORDER			WM_USER + 37
#define SSWM_STOPLP_ONORDER				WM_USER + 35
#define SSWM_STOPLP_ONLIQUIDATE			WM_USER + 36



#define UIDesgin_INIT   g_FontData.Init();g_ColorRefData.Init()


#define SSWM_QUOTE					   WM_USER + 100
#define SSWM_ONRESETUSERNO			   WM_USER + 101
#define SSWM_ONORDER			       WM_USER + 102
#define SSWM_ONMATCH			       WM_USER + 103

#define MainFrmName L"class TMainFrame"
#define configFilePath  L"\\Config\\TKeyOrder.Config.pub"
#define optionConfigAppName L"OptionConfig"
#define conditionOrderPath L"\\config\\TKeyOrder.Cond.pri"
//#define 



extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;

extern ITradeApi * g_pTradeApi;
extern ITradeData * g_pTradeData;
extern TPresenter* g_Presenter;
extern IMainFrame* g_pMainFrm;
extern IStarApi*  g_pStarApi;
extern TPluginMgrApi* g_pPluginMgrApi;

extern TGlobalModel * g_pGlobalModel;
extern IStopLP * g_pStopLP;
extern TradeAction * g_pTradeAction;
extern TStatusBar	*g_pStatusBar;


extern TPresenter* g_Presenter;
extern TMediator * g_pMediator;
extern LanguageID	 g_language;

#define owBackHand				'b'

#define EN_OrderQty_0      -1
#define EN_OrderPrice_0	   -2
#define EN_NoAva_Position  -3
#define Msg_OKOrCancel	   -4
#define EN_No_Hedge	       -5
#define EN_Null_Position   -6
#define EN_Null_ParOrder   -7
#define EN_Not_SHFEPosition -8
#define EN_No_LastSHFEPosition  -9
#define Msg_CoverAll_Position  -10
#define EN_Not_CmbPosition  -11
#define EN_Not_FoundCmb     -12
#define EN_Invalid_ContractNo -13
#define EN_CoverQty_TooMore   -14    //平仓量大于持仓量
#define Msg_OK	   -15
#endif