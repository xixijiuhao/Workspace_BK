#ifndef _PLUGIN_MGR_API_
#define _PLUGIN_MGR_API_

#pragma pack(push, 1)

//-------------------------------------------------------------------------------------------------
//���������ʹ��API
class TPluginMgrApi
{
public:
	//��ȡ���API�����������ͨ��pluginno�ҵ�����������֤�汾�����ԣ������Ƿ񷵻�api�����÷���Ҫ�жϷ���ֵ�Ƿ�ΪNULL
	virtual void* __cdecl CreatePluginApi(const wchar_t pno[], const wchar_t pver[]) = 0;
	//�ͷ�api
	virtual void __cdecl FreePluginApi(const wchar_t pno[], void* papi) = 0;
};

enum LanguageID
{
	NONE = 0x0000,  //δָ�����ԣ�Ĭ�ϼ��ϵͳ����
	CHT = 0x0404,	//Chinese (Taiwan)
	CHS = 0x0804,	//Chinese (PRC)
	ENU = 0x0409	//English (United States)
};

//���������ʱ�Ļص��¼�
typedef void(__cdecl *TOnPluginLoadFunc)(LanguageID l);
//���ȫ��������ɺ�Ļص��¼�
typedef void(__cdecl *TOnPluginInitFunc)(TPluginMgrApi* p);
//����ͷŻص��¼�
typedef void(__cdecl *TOnPluginFreeFunc)();
//�����¶API
typedef void* (__cdecl *TCreatePluginApiFunc)(const wchar_t v[]);
//�����¶�ͷ�API
typedef void(__cdecl *TFreePluginApiFunc)(void* p);

#pragma pack(pop)

#endif