#include "BaseInclude.h"

extern TPluginMgrApi*	g_pManager;
extern ITradeApi *		g_pTradeApi;
extern ITradeData*		g_pTradeData;
extern IMainFrame*		g_pMainFrame;
extern IStarApi*		g_pStarApi;
extern TShiftPosition*	g_pShiftPosition;
extern ShiftQuoteApi*	g_pShiftQuoteApi;

extern "C"
{
	__declspec(dllexport) void __cdecl OnPluginInit(TPluginMgrApi* p)
	{
		if (p == NULL)
			return;
		g_pManager = p;

		//主框架API
		g_pMainFrame = (IMainFrame*)p->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
		//g_pMainFrame->RegHotKey(false, false, false, VK_F12, false, TShiftPosition::TOnHotKeyDown);
		g_pMainFrame->RegMenuItem(MAKE_MENUORDER(0, 10002, 4), L"移仓", TShiftPosition::OnMenuClick);
		//交易和行情API
		g_pStarApi = (IStarApi*)g_pManager->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);
		g_pTradeApi = (ITradeApi*)g_pManager->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
		if (g_pTradeApi)
			g_pTradeData = g_pTradeApi->GetTradeData();

	}
	__declspec(dllexport) void __cdecl OnPluginLoad(LanguageID l)
	{
		UIDesgin_INIT;
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
		delete g_pShiftQuoteApi;
		delete g_pShiftPosition;
	}
}
