
#include "PreInclude.h"

TPluginMgrApi *					g_plugin_mgr = nullptr;
ICommonModule *					g_common_api = nullptr;
IConfigFrame *					g_config_api = nullptr;
IMainFrame *					g_main_api = nullptr;
ITradeApi*						g_pTradeApi = nullptr;
ITradeData *					g_pTradeData = nullptr;
IStarApi *						g_StarApi = nullptr;
//ISQuoteEventSpi*				g_QuoteSpi;
ILanguageApi *					g_pLanguage;
IAuthent *						g_pAuthent;
ICspAccessApi *					g_pCspAccessApi = nullptr;

//CPositonDiffWin					g_PosDiffWin;
//CPositionDiffBar				g_PosDiffBar;

G_COLOREF						g_ColorRefData;
G_UIFONT						g_FontData;
IDataContainer					* g_OrderData;

vPosDifCfg						vecTmp;

bool							flagAuthent = false;						//是否认证过
unsigned int					MENU_ID;

PositonTrade* 					g_position_trade;


////配置窗口
//HWND __stdcall CreateConfigWnd(const wchar_t * sub_title, const HWND parent)
//{
//	if (!IsWindow(g_PosDiffWin.GetHwnd()))
//	{
//		g_PosDiffWin.Create(sub_title, parent, WS_CHILD, 0);
//		return g_PosDiffWin.GetHwnd();
//	}
//	else
//		return nullptr;
//}
////系统选项
//void __cdecl PosDiffMenuItem(const unsigned int MenuIndex, const HWND PastLife)
//{
//	//g_config_api->show(L"仓差风控", cmtExpand);
//	//if (PastLife != 0)
//	//	return;
//	//if (!IsWindow(g_PosDiffWin.GetHwnd()))
//	//{
//	//	g_PosDiffWin.Create(MenuIndex, PastLife, WS_CHILD, 0);
//	//}
//
//}

//POSITION_DIFF_DLL void __cdecl OnPluginLoad(LanguageID lang_id);
//POSITION_DIFF_DLL void __cdecl OnPluginInit(TPluginMgrApi * plugin_mgr);
POSITION_DIFF_DLL void * __cdecl CreatePluginApi(const wchar_t v[]) { return nullptr; };
POSITION_DIFF_DLL void __cdecl FreePluginApi(void * p) {};
//POSITION_DIFF_DLL void __cdecl OnPluginFree();

POSITION_DIFF_DLL void __cdecl OnPluginLoad(LanguageID lang_id)
{
	LoadRC::set_language(lang_id);
}

POSITION_DIFF_DLL void __cdecl OnPluginInit(TPluginMgrApi * plugin_mgr)
{
	_ASSERT(plugin_mgr);
	g_plugin_mgr = plugin_mgr;

	g_ColorRefData.Init();
	g_FontData.Init(LoadRC::get_lang_id());

	//TradeApiData
	g_pTradeApi = (ITradeApi*)plugin_mgr->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
	if (g_pTradeApi)
		g_pTradeData = g_pTradeApi->GetTradeData();
	_ASSERT(g_pTradeData);

	//language
	g_pLanguage = (ILanguageApi*)plugin_mgr->CreatePluginApi(PLUG_LANGUAGEAPI_NAME, PLUG_LANGUAGEAPI_VERSION);
	_ASSERT(g_pLanguage);
	if (g_pLanguage)
		g_pLanguage->SetModule(L"PositiondifRiskctrl");

	//注册认证插件
	g_pAuthent = (IAuthent*)g_plugin_mgr->CreatePluginApi(PLUG_LOGINFRM_NAME, PLUG_MAINFRAME_VERSION);
	_ASSERT(g_pAuthent);

	g_position_trade = new PositonTrade;
	//注册接入服务
	g_pCspAccessApi = (ICspAccessApi*)plugin_mgr->CreatePluginApi(PLUG_ACCESSAPI_NAME, PLUG_ACCESSAPI_VERSION);
	_ASSERT(g_pCspAccessApi);
	g_pCspAccessApi->RegEventSpi((ICspAccessSpi*)g_position_trade);
	//IStarApi
	g_StarApi = (IStarApi*)plugin_mgr->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);
	_ASSERT(g_StarApi);
	g_StarApi->RegEventSpi((ISQuoteEventSpi*)g_position_trade);

	//注册主框架插件
	//g_main_api = (IMainFrame*)g_plugin_mgr->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
	
	////注册通用模块插件
	//g_common_api = (ICommonModule *)g_plugin_mgr->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);
	//if (g_common_api)
	//{
	//	g_common_api->Regist(dynamic_cast<ICommonInteractRiskCheck*>(this), InteractType_RiskCheck);
	//}
	//_ASSERT(g_common_api);

	////注册配置窗口插件
	//g_config_api = (IConfigFrame*)g_plugin_mgr->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);
	//if (g_config_api)
	//{
	//	MENU_ID = g_config_api->reg_config_dlg(CConvert::LoadResString(IDS_Subtitle).c_str(), (dlg_create_func)CreateConfigWnd, cmtExpand, 0);
	//}
	//_ASSERT(g_config_api);

	////注册主框架插件
	//g_main_api = (IMainFrame*)g_plugin_mgr->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
	//if (g_main_api)
	//{
	//	g_PosDiffBar.Create(L"CEFC", NULL, WS_POPUP | WS_VISIBLE, 0);
	//	g_main_api->RegStatusBarItemEx(1, g_PosDiffBar.GetHwnd(), 2);

	//}
	//_ASSERT(g_main_api);
}




POSITION_DIFF_DLL void __cdecl OnPluginFree()
{
	if (g_StarApi && g_position_trade)
		g_StarApi->UnregEventSpi((ISQuoteEventSpi*)g_position_trade);
	if (g_common_api && g_position_trade)
		g_common_api->UnRegist(dynamic_cast<ICommonInteractRiskCheck*>(g_position_trade), InteractType_RiskCheck);
	if(g_config_api)
		g_config_api->unreg_config_dlg(MENU_ID);
	if (g_position_trade)
		delete(g_position_trade);
}