#include "PreInclude.h"


ITradeApi*      g_pTradeApi;
ITradeData*     g_pTradeData;
IMainFrame*     g_pMainFrame;
IStarApi*      g_pStarApi;
TPluginMgrApi*  g_pmanager;
G_COLOREF g_ColorRefData;
G_UIFONT  g_FontData;
TPointOrderTipsFrm g_clTips = TPointOrderTipsFrm::GetInstance();

//new
IAuthent  * g_pAuthent = nullptr;

TPointOrderAPI *	g_TPointOrderAPI;
PointOrderConfig	g_stOrderConfig;
TPointConfig		g_clOrderConfig;
LanguageID		g_LanguageID;
wchar_t CHSAppName[50] = { L"����µ�" };
wchar_t CHTAppName[50] = { L"�c�r��" };
wchar_t ENUAppName[50] = { L"ClickOrder" };
wchar_t* _GetAppNameString()
{
	if (g_LanguageID == ENU)
		return ENUAppName;
	else if (g_LanguageID == CHT)
		return CHTAppName;
	else
		return CHSAppName;
}
char CHSTips[][50] = { "�޿�ƽ�ֲ�", "�Զ���ƽ", "����", "ƽ��", "�µ���������Ϊ0", "�µ��۸���Ϊ0", "����:%d,�۸�:%s", "�µ�ȷ��", "��ʾ", "�Ƿ�ȫ��", "�Ƿ�ȫƽ" };
char CHTTips[][50] = { "�o��ƽ�ւ}", "�Ԅ��_ƽ", "�_�}", "ƽ�}", "�Δ������ܞ�0", "�΃r���ܞ�0", "�֔�:%d,�r��:%s", "�δ_�J", "��ʾ", "�Ƿ�ȫ��", "�Ƿ�ȫƽ" };
char ENUTips[][50] = { "No Avaiabe Positon", "Auto OpenCose", "Open", "Cose", "Order ots can't be 0", "Order Price can't be 0", "Number:%d, price:%s", "InsertOrder Confirmation", "tips", "CanceA?", "CoverA?" };

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
		g_pMainFrame->RegMenuItem(MAKE_MENUORDER(0, 10002, 2), _GetAppNameString() , TPointOrderWnd::OnMenuClick);
        g_pMainFrame->RegHotKey(false, false, false, VK_F8, false, TPointOrderWnd::TOnHotKeyDown);
		//��Ч����Ҫ F8 ��ݼ����������Ч��Ҫȡ��ע��
		//g_pMainFrame->RegHotKey(false, false, false, VK_F8, false, TPointOrderWnd::TOnHotKeyDown);

		g_pAuthent = (IAuthent*)p->CreatePluginApi(PLUG_LOGINFRM_NAME, PLUG_LOGINFRM_VERSION);
	}
	__declspec(dllexport) void __cdecl OnPluginLoad(LanguageID l)
	{
		g_ColorRefData.Init();
		g_FontData.Init(l);
		if (l == NONE)
			g_LanguageID = CHS;
		else
			g_LanguageID = l;
	}
	__declspec(dllexport) void* __cdecl CreatePluginApi(const wchar_t v[])
	{
		if (!g_TPointOrderAPI)
			g_TPointOrderAPI = new TPointOrderAPI;
		return g_TPointOrderAPI;
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