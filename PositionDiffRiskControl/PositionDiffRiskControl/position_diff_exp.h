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

//���������ʱ�Ļص��¼�
//typedef void(__cdecl *TOnPluginLoadFunc)(LanguageID l);
//���ȫ��������ɺ�Ļص��¼�
//typedef void(__cdecl *TOnPluginInitFunc)(TPluginMgrApi* p);
//����ͷŻص��¼�
//typedef void(__cdecl *TOnPluginFreeFunc)();
typedef char							TPosDifContract[51];			//��Լ
struct TPostionDifInfo
{
	TPosDifContract		Code;					//��Լ����

	int					nMax1Dif;					//С��ֲ�				
	int					nMax2Dif;					//���ֲ�
};
typedef vector<TPostionDifInfo>			vPosDifCfg;