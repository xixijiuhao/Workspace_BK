#include "BaseInclude.h"

IPointOrderAPI* g_pPointOrder = nullptr;
TKeyOrderApi*   g_pTKeyOrder = nullptr;

LanguageID    g_language = CHS;
extern "C"
{
	__declspec(dllexport) void __cdecl OnPluginInit(TPluginMgrApi* p)
	{
		if (!p) return;
		g_pMainFrame = (IMainFrame*)p->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
		if(g_language==CHT)
			g_pMainFrame->RegMenuItem(MAKE_MENUORDER(0, 10002, 6), L"用戶向導", TUserGuide::OnMenuClk);
		else if(g_language==ENU)
			g_pMainFrame->RegMenuItem(MAKE_MENUORDER(0, 10002, 6), L"User Guide", TUserGuide::OnMenuClk);
		else
			g_pMainFrame->RegMenuItem(MAKE_MENUORDER(0, 10002, 6), L"用户向导", TUserGuide::OnMenuClk);
		//g_pMainFrame->RegHotKey(false, false, false, VK_F8, false, TUserGuide::OnHotKeyDown); 

		g_pPointOrder = (IPointOrderAPI*)p->CreatePluginApi(PLUG_POINTORDER_NAME, PLUG_POINTORDER_VERSION);
		g_pTKeyOrder = (TKeyOrderApi*)p->CreatePluginApi(PLUG_TKEYORDER_NAME, PLUG_TKEYORDER_VERSION);
		
	}
	__declspec(dllexport) void __cdecl OnPluginLoad(LanguageID l)
	{
		g_language = l;
		if (l == NONE)
			g_language = CHS;

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
