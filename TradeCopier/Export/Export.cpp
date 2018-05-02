#include "PreInclude.h"


TPluginMgrApi* g_pPluginMgrApi;
ITradeApi * g_pTradeApi = NULL;
ITradeData * g_pTradeData = NULL;
IStarApi*g_pStarApi = nullptr;

G_COLOREF g_ColorRefData;
G_UIFONT  g_FontData;
LanguageID g_language = CHS;

IAuthent  * g_pAuthent = nullptr;

ILoggingApi * g_pLogging= nullptr;

IMainFrame * g_pMainFrm = nullptr;

extern "C"
{
	__declspec(dllexport)
	void __cdecl OnPluginInit(TPluginMgrApi* p)
	{
		if (!p)
			return;
		g_pTradeApi = (ITradeApi *)p->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
		if (g_pTradeApi)
			g_pTradeData = g_pTradeApi->GetTradeData();
		g_pMainFrm = (IMainFrame*)p->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);

		g_pPluginMgrApi = p;

		g_pMainFrm->RegHotKey(false, false, false, VK_F9, false, TFollowTrade::OnHotKeyDown);

		g_pStarApi = (IStarApi*)p->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

		g_pMainFrm->RegMenuItem(MAKE_MENUORDER(0, 40000, 0), L"¸úµ¥", TFollowTrade::OnMenuClick);

		g_pAuthent = (IAuthent*)p->CreatePluginApi(PLUG_LOGINFRM_NAME, PLUG_LOGINFRM_VERSION);

		g_pLogging = (ILoggingApi*)p->CreatePluginApi(PLUG_LOGGINGAPI_NAME, PLUG_LOGGINGAPI_VERSION);
	}
	__declspec(dllexport) void __cdecl OnPluginLoad(LanguageID l)
	{
		g_ColorRefData.Init();
		g_FontData.Init();
		LoadLibrary(TEXT("Msftedit.dll"));
	}
	__declspec(dllexport) void* __cdecl CreatePluginApi(const wchar_t v[])
	{
		return 0;
	}
	__declspec(dllexport) void __cdecl OnPluginFree()
	{

	}

	__declspec(dllexport) void __cdecl FreePluginApi(void* p)
	{

	}
}


