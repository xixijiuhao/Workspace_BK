#include "PreInclude.h"

const wchar_t PLUG_CAPITAL_RISKMGT_MONITOR_NAME[] = L"CapitalRiskMgt";

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
CapitalRiskMgtWnd   *g_pCapitalRiskMgtWnd = NULL;
CapitalRiskMgt      *g_pCapitalRiskMgt = NULL;

G_COLOREF		    g_ColorRefData;				//全局 色彩
G_UIFONT		    g_FontData;					//全局 字体

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
        g_pLanguage->SetModule(PLUG_CAPITAL_RISKMGT_MONITOR_NAME);
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

	g_mainframe->RegMenuItem(MAKE_MENUORDER(0, 10002, 3), LoadResString(IDS_Capital_Risk_Management).c_str(), CapitalRiskMgtWnd::OnMenuClick);
	//       g_pMainFrame->RegHotKey(false, false, false, VK_F8, false, SHFEDeepQuoteFrm::TOnHotKeyDown);
	if (!g_pCapitalRiskMgtWnd)
	{
		g_pCapitalRiskMgtWnd = new CapitalRiskMgtWnd;
	}
	if (!g_pCapitalRiskMgt)
	{
		g_pCapitalRiskMgt = new CapitalRiskMgt;
	}

	g_StarApi->RegEventSpi(g_pCapitalRiskMgt);
	g_pCspAccessApi->RegEventSpi((ICspAccessSpi*)g_pCapitalRiskMgt);
}

void __cdecl OnPluginFree()
{

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
