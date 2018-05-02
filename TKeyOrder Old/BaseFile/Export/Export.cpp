#include "PreInclude.h"

extern TPresenter* g_Presenter;
extern IStopLP * g_pStopLP;


extern "C"
{
	__declspec(dllexport) void __cdecl OnPluginInit(TPluginMgrApi* p)
	{
		if (!p) return;

		g_pPluginMgrApi = p;
		g_pMainFrm = (IMainFrame*)p->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
		//g_pMainFrm->RegHotKey(false,  false, false, VK_SPACE, false, TPresenter::OnHotKeyDown);

		g_pTradeApi = (ITradeApi *)p->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
		if (g_pTradeApi)
			g_pTradeData = g_pTradeApi->GetTradeData();

		g_pStarApi = (IStarApi*)p->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

		g_pStatusBar = new TStatusBar;
		g_pStatusBar->ShowFrm();
		g_pMainFrm->RegStatusBarItem(13, g_pStatusBar->GetHwnd());

	}
	__declspec(dllexport) void __cdecl OnPluginLoad(LanguageID l)
	{
		UIDesgin_INIT;
		LoadLibrary(TEXT("Msftedit.dll"));
		g_language = l;
	}
	__declspec(dllexport) void* __cdecl CreatePluginApi(const wchar_t v[])
	{
		static TKeyOrder sTKeyOrderApi;
		return &sTKeyOrderApi;
		//return 0;
	}
	__declspec(dllexport) void __cdecl OnPluginFree()
	{
	}

	__declspec(dllexport) void __cdecl FreePluginApi(void* p)
	{

	}
	}
