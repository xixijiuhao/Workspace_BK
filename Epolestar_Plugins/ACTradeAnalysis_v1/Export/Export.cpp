#include "BaseInclude.h"

extern "C"
{
	__declspec(dllexport) void __cdecl OnPluginInit(TPluginMgrApi* p)
	{
		if (!p) return;

		g_pPluginMgrApi = p;
		g_pMainFrm = (IMainFrame*)p->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);

		g_pTradeApi = (ITradeApi *)p->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
		if (g_pTradeApi)
			g_pTradeData = g_pTradeApi->GetTradeData();

		g_pQuoteApi = (IStarApi*)p->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

		g_pLanguageApi = (ILanguageApi*)p->CreatePluginApi(PLUG_LANGUAGEAPI_NAME, PLUG_LANGUAGEAPI_VERSION);
		//是否需要判空？？？
		bool flag = g_pLanguageApi->SetModule(L"ACTradeAnalysis");

		/*注册MenuItem
		@Param In :const unsigned int menuorder  建议排序方式
		@Param In :const wchar_t* menutext 在Menu中显示的标签
		@Param In :TOnMenuItemClick onclick
		@Param Out :返回值为实际使用的menuindex，返回0表示注册失败
		*/
		g_pMainFrm->RegMenuItem(MAKE_MENUORDER(0, 10002, 14), L"ACTradeAnalysis", ACAnalysisPresenter::OnMenuClick);
	}
	__declspec(dllexport) void __cdecl OnPluginLoad(LanguageID l)
	{
		UIDesgin_INIT;
		g_language = l;
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
