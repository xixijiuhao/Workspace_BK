#pragma once

#ifdef POSITION_DIFF_EXPORTS
#define POSITION_DIFF_DLL extern "C" __declspec(dllexport)
#else
#define POSITION_DIFF_DLL extern "C" __declspec(dllimport)
#endif

//POSITION_DIFF_DLL void __cdecl OnPluginLoad(LanguageID lang_id);
//POSITION_DIFF_DLL void __cdecl OnPluginInit(TPluginMgrApi * plugin_mgr);
//POSITION_DIFF_DLL void * __cdecl CreatePluginApi(const wchar_t v[]) { return nullptr; }
//POSITION_DIFF_DLL void __cdecl FreePluginApi(void * p) {}
//POSITION_DIFF_DLL void __cdecl OnPluginFree();

//插件被加载时的回调事件
//typedef void(__cdecl *TOnPluginLoadFunc)(LanguageID l);
//插件全部加载完成后的回调事件
//typedef void(__cdecl *TOnPluginInitFunc)(TPluginMgrApi* p);
//插件释放回调事件
//typedef void(__cdecl *TOnPluginFreeFunc)();
typedef char							TPosDifContract[51];			//合约
struct TPostionDifInfo
{
	TPosDifContract		Code;					//合约代码

	int					nMax1Dif;					//小额仓差				
	int					nMax2Dif;					//大额仓差
};
typedef vector<TPostionDifInfo>			vPosDifCfg;