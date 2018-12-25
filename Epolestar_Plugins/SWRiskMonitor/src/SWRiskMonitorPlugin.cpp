/**
* =====================================================================
*
*       [filename    ] SWRiskMonitorPlugin.cpp
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-10-12
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#include <windows.h>

#include "PluginMgr_API.h"
#include "MainFrame_API.h"
#include "CspAccess_API.h"
#include "StarApi_API.h"
#include "LanguageApi_API.h"
#include "StrategyTrade_API.h"
#include "TradeApi_API.h"
#include "ConfigFrame_API.h"
#include "CommonModule_API.h"
#include "load_rc.h"
#include "UIDesign.h"	
#include "StatusBarWnd.h"
#include "Language.h"
#include "../share/AnsicUnicode.h"
#include "SWRiskConfigWnd.h"
#include "SWRiskMonitor.h"
#include "LoginFrm_API.h"


const wchar_t PLUG_SWRISK_MONITOR_NAME[] = L"SWRiskMonitor";

TPluginMgrApi	    *g_manager = NULL;			//全局 插件管理器接口 类指针
IMainFrame		    *g_mainframe = NULL;		//全局 主框架接口 类指针
IStarApi		    *g_StarApi = NULL;
ILanguageApi	    *g_pLanguage = NULL;		//全局 语言资源
IStrategyTradeAPI   *g_pStrategyApi = NULL;		//全局 止损止盈 套利
IConfigFrame	    *g_conFrame = NULL;			//全局 系统配置 类指针
ITradeData	        *g_pTradeData = NULL;
ITradeApi           *g_pTradeApi = NULL;
ICommonModule       *g_pCommonModule = NULL;
ICspAccessApi		*g_pCspAccessApi = NULL;

G_COLOREF		    g_ColorRefData;				//全局 色彩
G_UIFONT		    g_FontData;					//全局 字体

StatusBarWnd        *g_statusBarWnd = NULL;
SWRiskConfigWnd     *g_cfgWnd = NULL;
SWRiskMonitor       *g_monitor = NULL;
IAuthent            *g_authent = NULL;


extern "C"
{
    //
    void __cdecl OnPluginLoad(LanguageID l);

    //插件全部加载完成后的回调事件
    void __cdecl OnPluginInit(TPluginMgrApi* p);

    //插件释放回调事件
    void __cdecl OnPluginFree();

    //插件暴露API
    void* __cdecl CreatePluginApi(const wchar_t v[]);

    //释放接口
    void __cdecl FreePluginApi(void* p);

}

void __cdecl OnPluginLoad(LanguageID l)
{


}

//插件全部加载完成后的回调事件
void __cdecl OnPluginInit(TPluginMgrApi* p)
{
    if (p == NULL)
        return;
    if (g_manager == NULL)
        g_manager = p;

    g_pLanguage = (ILanguageApi *)g_manager->CreatePluginApi(PLUG_LANGUAGEAPI_NAME, PLUG_LANGUAGEAPI_VERSION);
    if (g_pLanguage)
    {
        g_pLanguage->SetModule(PLUG_SWRISK_MONITOR_NAME);
        LoadRC::set_language(g_pLanguage->LangId());
    }

    g_ColorRefData.Init();
    g_FontData.Init(LoadRC::get_lang_id());

    //主框架接口
    g_mainframe = (IMainFrame*)g_manager->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
    //系统配置框架接口
    g_conFrame = (IConfigFrame*)g_manager->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);

    //交易接口
    g_pTradeApi = (ITradeApi*)g_manager->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
    if (g_pTradeApi)
        g_pTradeData = g_pTradeApi->GetTradeData();

	//接入服务
	g_pCspAccessApi = (ICspAccessApi*)g_manager->CreatePluginApi(PLUG_ACCESSAPI_NAME, PLUG_ACCESSAPI_VERSION);

	//行情接口
    g_StarApi = (IStarApi*)g_manager->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

    //策略
    g_pStrategyApi = (IStrategyTradeAPI*)g_manager->CreatePluginApi(PLUG_STRATEGYTRADE_NAME, PLUG_STRATEGYTRADE_VERSION);

    //通用接口
    g_pCommonModule = (ICommonModule *)g_manager->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);

    //认证接口
    g_authent = (IAuthent *)g_manager->CreatePluginApi(PLUG_LOGINFRM_NAME, PLUG_LOGINFRM_VERSION);

    if (!g_mainframe || !g_StarApi)
        return;

    if (!g_statusBarWnd)
    {
        g_statusBarWnd = new StatusBarWnd;
        g_statusBarWnd->Create();
    }

    if (!g_cfgWnd)
    {
        g_cfgWnd = new SWRiskConfigWnd;
    }
    if (!g_monitor)
    {
        g_monitor = new SWRiskMonitor;
    }

    g_StarApi->RegEventSpi(g_monitor);
	g_pCspAccessApi->RegEventSpi((ICspAccessSpi*)g_monitor);
}

void __cdecl OnPluginFree()
{
    if (g_statusBarWnd)
    {
        delete g_statusBarWnd;
    }
}

//插件暴露API
void*  __cdecl CreatePluginApi(const wchar_t v[])
{
    return NULL;
}

//插件暴露释放API
void __cdecl FreePluginApi(void* p)
{

}
