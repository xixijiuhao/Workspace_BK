#include "BaseInclude.h"

extern ITradeApi *    g_pTradeApi;
extern ITradeData*    g_pTradeData;
extern IMainFrame*    g_pMainFrame;
extern IQuoteApi*     g_pQuoteApi;

extern "C"
{
	__declspec(dllexport) void __cdecl OnPluginInit(TPluginMgrApi* p)
	{
		if (!p) return;
	
		g_pTradeApi = (ITradeApi *)p->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);

		if (g_pTradeApi)
			g_pTradeData = g_pTradeApi->GetTradeData();

		g_pMainFrame = (IMainFrame*)p->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);

		g_pQuoteApi = (IQuoteApi*)g_pManager->CreatePluginApi(PLUG_QUOTEAPI_NAME, PLUG_QUOTEAPI_VERSION);

	}
	__declspec(dllexport) void __cdecl OnPluginLoad(LanguageID l)
	{
		UIDesgin_INIT;
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
