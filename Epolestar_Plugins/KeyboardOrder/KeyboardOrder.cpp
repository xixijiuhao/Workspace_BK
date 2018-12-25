#include "PreInclude.h"

#define MenuGroupID	10002

extern CSujectKBOrder *	g_pSubject;					//横向管理类

CPlugLoad				g_PlugLoad;					//横向全局接口管理
KeyBoardConfig *		g_KeyBoardConfig = NULL;	//横向配置类

HWND __stdcall SetWin(const wchar_t* subtitle, const HWND parent)//系统配置 竖向下单
{
	wstring s = subtitle;
	if (s == LoadResString(IDS_STRING_RegMenu))
	{
		if (g_KeyBoardConfig)
		{
			if (!IsWindow(g_KeyBoardConfig->GetKeyBoardSetHWND()))//窗口资源是否被销毁 销毁则重新创建
			{
				g_KeyBoardConfig->CreateKeyBoardSetWnd(subtitle);
			}
			return g_KeyBoardConfig->GetKeyBoardSetHWND();
		}
		else
			return NULL;
	}
	else
		return NULL;
}

//插件被加载时的回调事件
void __cdecl OnPluginLoad(LanguageID l)
{
	
}
//插件全部加载完成后的回调事件
void __cdecl OnPluginInit(TPluginMgrApi* p)
{
	//初始化插件
	g_PlugLoad.Init(p);

	//注册配置
	//RegConfigDlg();
	if (!g_KeyBoardConfig)
		g_KeyBoardConfig = new KeyBoardConfig();
	if (g_PlugLoad.GetConfigFrame())
		g_PlugLoad.GetConfigFrame()->reg_config_dlg(LoadResString(IDS_STRING_RegMenu).c_str(), (dlg_create_func)SetWin, cmtTrade, 30, "esunny.epolestar.configframe");

	//注册菜单
	if (g_PlugLoad.GetMainFrm())
		g_PlugLoad.m_idmenu = g_PlugLoad.GetMainFrm()->RegMenuItem(MAKE_MENUORDER(0, MenuGroupID, 1), LoadResString(IDS_STRING_RegMenu).c_str(), OnMenuItemClick);
	//注册热键
	if (g_PlugLoad.GetMainFrm())
		RegFrmHotKey();
}
void __cdecl OnPluginFree()
{
	if (g_pSubject)
	{
		delete g_pSubject;
		g_pSubject = NULL;
	}
}
//插件暴露API
void*  __cdecl CreatePluginApi(const wchar_t v[])
{
	return NULL;
}
//插件暴露释放API
void __cdecl FreePluginApi(void* p)
{
	return;
}