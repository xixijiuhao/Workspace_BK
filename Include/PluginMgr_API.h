#ifndef _PLUGIN_MGR_API_
#define _PLUGIN_MGR_API_

#pragma pack(push, 1)

//-------------------------------------------------------------------------------------------------
//插件管理器使用API
class TPluginMgrApi
{
public:
	//获取插件API，插件管理器通过pluginno找到插件，插件验证版本兼容性，决定是否返回api，调用方需要判断返回值是否为NULL
	virtual void* __cdecl CreatePluginApi(const wchar_t pno[], const wchar_t pver[]) = 0;
	//释放api
	virtual void __cdecl FreePluginApi(const wchar_t pno[], void* papi) = 0;
};

enum LanguageID
{
	NONE = 0x0000,  //未指定语言，默认检测系统语言
	CHT = 0x0404,	//Chinese (Taiwan)
	CHS = 0x0804,	//Chinese (PRC)
	ENU = 0x0409	//English (United States)
};

//插件被加载时的回调事件
typedef void(__cdecl *TOnPluginLoadFunc)(LanguageID l);
//插件全部加载完成后的回调事件
typedef void(__cdecl *TOnPluginInitFunc)(TPluginMgrApi* p);
//插件释放回调事件
typedef void(__cdecl *TOnPluginFreeFunc)();
//插件暴露API
typedef void* (__cdecl *TCreatePluginApiFunc)(const wchar_t v[]);
//插件暴露释放API
typedef void(__cdecl *TFreePluginApiFunc)(void* p);

#pragma pack(pop)

#endif