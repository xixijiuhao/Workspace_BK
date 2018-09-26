#include "PreInclude.h"

#define MenuGroupID	10002

extern CSujectKBOrder *	g_pSubject;					//���������

CPlugLoad				g_PlugLoad;					//����ȫ�ֽӿڹ���
KeyBoardConfig *		g_KeyBoardConfig = NULL;	//����������

HWND __stdcall SetWin(const wchar_t* subtitle, const HWND parent)//ϵͳ���� �����µ�
{
	wstring s = subtitle;
	if (s == LoadResString(IDS_STRING_RegMenu))
	{
		if (g_KeyBoardConfig)
		{
			if (!IsWindow(g_KeyBoardConfig->GetKeyBoardSetHWND()))//������Դ�Ƿ����� ���������´���
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

//���������ʱ�Ļص��¼�
void __cdecl OnPluginLoad(LanguageID l)
{
	
}
//���ȫ��������ɺ�Ļص��¼�
void __cdecl OnPluginInit(TPluginMgrApi* p)
{
	//��ʼ�����
	g_PlugLoad.Init(p);

	//ע������
	//RegConfigDlg();
	if (!g_KeyBoardConfig)
		g_KeyBoardConfig = new KeyBoardConfig();
	if (g_PlugLoad.GetConfigFrame())
		g_PlugLoad.GetConfigFrame()->reg_config_dlg(LoadResString(IDS_STRING_RegMenu).c_str(), (dlg_create_func)SetWin, cmtTrade, 30, "esunny.epolestar.configframe");

	//ע��˵�
	if (g_PlugLoad.GetMainFrm())
		g_PlugLoad.m_idmenu = g_PlugLoad.GetMainFrm()->RegMenuItem(MAKE_MENUORDER(0, MenuGroupID, 1), LoadResString(IDS_STRING_RegMenu).c_str(), OnMenuItemClick);
	//ע���ȼ�
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
//�����¶API
void*  __cdecl CreatePluginApi(const wchar_t v[])
{
	return NULL;
}
//�����¶�ͷ�API
void __cdecl FreePluginApi(void* p)
{
	return;
}