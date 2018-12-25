#include "PreInclude.h"

ITradeApi*      g_pTradeApi;
ITradeData*     g_pTradeData;
IMainFrame*     g_pMainFrame;
IStarApi*      g_pStarApi;
TPluginMgrApi*  g_pmanager;
G_COLOREF g_ColorRefData;
G_UIFONT  g_FontData;
SHFEDeepQuoteFrm* g_DeepQuoteFrm;
ColumnConfigView*	m_ColumuConfig;
ILanguageApi*		g_pLanguage;
//MBLApi* g_pMBLApi;
//CMBLSpiHandler * g_pMBLSpiHandler;
extern "C"
{
	__declspec(dllexport) void __cdecl OnPluginInit(TPluginMgrApi* p)
	{
		if (!p)
			return;
		g_pmanager = p;
		g_pMainFrame = (IMainFrame*)p->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
		g_pTradeApi = (ITradeApi*)p->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
		g_pStarApi = (IStarApi*)p->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

		if (g_pTradeApi)
			g_pTradeData = g_pTradeApi->GetTradeData();

		g_pMainFrame->RegMenuItem(MAKE_MENUORDER(0, 10002, 3), L"Éî¶È¹Òµ¥", SHFEDeepQuoteFrm::OnMenuClick);
		//       g_pMainFrame->RegHotKey(false, false, false, VK_F8, false, SHFEDeepQuoteFrm::TOnHotKeyDown);

	}
	__declspec(dllexport) void __cdecl OnPluginLoad(LanguageID l)
	{
		g_ColorRefData.Init();
		g_FontData.Init(CHS);
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


bool MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(936, 0, lpcszStr, -1, NULL, 0);

	if (dwSize < dwMinSize)
	{
		return false;
	}
	MultiByteToWideChar(936, 0, lpcszStr, -1, lpwszStr, dwMinSize);
	return true;
}

