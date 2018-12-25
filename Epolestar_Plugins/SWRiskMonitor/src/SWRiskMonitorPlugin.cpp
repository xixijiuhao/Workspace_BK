/**
* =====================================================================
*
*       [filename    ] SWRiskMonitorPlugin.cpp
*       [author      ] Hao Lin
*       [Description ]
*       [Timestamp   ] 2017-10-12
*
* ===========Copyright (C) 2002-2017 Esunny. All rights reserved.===========
*/

#include <windows.h>

#include "PluginMgr_API.h"
#include "MainFrame_API.h"
#include "CspAccess_API.h"
#include "StarApi_API.h"
#include "LanguageApi_API.h"
#include "StrategyTrade_API.h"
#include "TradeApi_API.h"
#include "ConfigFrame_API.h"
#include "CommonModule_API.h"
#include "load_rc.h"
#include "UIDesign.h"	
#include "StatusBarWnd.h"
#include "Language.h"
#include "../share/AnsicUnicode.h"
#include "SWRiskConfigWnd.h"
#include "SWRiskMonitor.h"
#include "LoginFrm_API.h"


const wchar_t PLUG_SWRISK_MONITOR_NAME[] = L"SWRiskMonitor";

TPluginMgrApi	    *g_manager = NULL;			//ȫ�� ����������ӿ� ��ָ��
IMainFrame		    *g_mainframe = NULL;		//ȫ�� ����ܽӿ� ��ָ��
IStarApi		    *g_StarApi = NULL;
ILanguageApi	    *g_pLanguage = NULL;		//ȫ�� ������Դ
IStrategyTradeAPI   *g_pStrategyApi = NULL;		//ȫ�� ֹ��ֹӯ ����
IConfigFrame	    *g_conFrame = NULL;			//ȫ�� ϵͳ���� ��ָ��
ITradeData	        *g_pTradeData = NULL;
ITradeApi           *g_pTradeApi = NULL;
ICommonModule       *g_pCommonModule = NULL;
ICspAccessApi		*g_pCspAccessApi = NULL;

G_COLOREF		    g_ColorRefData;				//ȫ�� ɫ��
G_UIFONT		    g_FontData;					//ȫ�� ����

StatusBarWnd        *g_statusBarWnd = NULL;
SWRiskConfigWnd     *g_cfgWnd = NULL;
SWRiskMonitor       *g_monitor = NULL;
IAuthent            *g_authent = NULL;


extern "C"
{
    //
    void __cdecl OnPluginLoad(LanguageID l);

    //���ȫ��������ɺ�Ļص��¼�
    void __cdecl OnPluginInit(TPluginMgrApi* p);

    //����ͷŻص��¼�
    void __cdecl OnPluginFree();

    //�����¶API
    void* __cdecl CreatePluginApi(const wchar_t v[]);

    //�ͷŽӿ�
    void __cdecl FreePluginApi(void* p);

}

void __cdecl OnPluginLoad(LanguageID l)
{


}

//���ȫ��������ɺ�Ļص��¼�
void __cdecl OnPluginInit(TPluginMgrApi* p)
{
    if (p == NULL)
        return;
    if (g_manager == NULL)
        g_manager = p;

    g_pLanguage = (ILanguageApi *)g_manager->CreatePluginApi(PLUG_LANGUAGEAPI_NAME, PLUG_LANGUAGEAPI_VERSION);
    if (g_pLanguage)
    {
        g_pLanguage->SetModule(PLUG_SWRISK_MONITOR_NAME);
        LoadRC::set_language(g_pLanguage->LangId());
    }

    g_ColorRefData.Init();
    g_FontData.Init(LoadRC::get_lang_id());

    //����ܽӿ�
    g_mainframe = (IMainFrame*)g_manager->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
    //ϵͳ���ÿ�ܽӿ�
    g_conFrame = (IConfigFrame*)g_manager->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);

    //���׽ӿ�
    g_pTradeApi = (ITradeApi*)g_manager->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
    if (g_pTradeApi)
        g_pTradeData = g_pTradeApi->GetTradeData();

	//�������
	g_pCspAccessApi = (ICspAccessApi*)g_manager->CreatePluginApi(PLUG_ACCESSAPI_NAME, PLUG_ACCESSAPI_VERSION);

	//����ӿ�
    g_StarApi = (IStarApi*)g_manager->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

    //����
    g_pStrategyApi = (IStrategyTradeAPI*)g_manager->CreatePluginApi(PLUG_STRATEGYTRADE_NAME, PLUG_STRATEGYTRADE_VERSION);

    //ͨ�ýӿ�
    g_pCommonModule = (ICommonModule *)g_manager->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);

    //��֤�ӿ�
    g_authent = (IAuthent *)g_manager->CreatePluginApi(PLUG_LOGINFRM_NAME, PLUG_LOGINFRM_VERSION);

    if (!g_mainframe || !g_StarApi)
        return;

    if (!g_statusBarWnd)
    {
        g_statusBarWnd = new StatusBarWnd;
        g_statusBarWnd->Create();
    }

    if (!g_cfgWnd)
    {
        g_cfgWnd = new SWRiskConfigWnd;
    }
    if (!g_monitor)
    {
        g_monitor = new SWRiskMonitor;
    }

    g_StarApi->RegEventSpi(g_monitor);
	g_pCspAccessApi->RegEventSpi((ICspAccessSpi*)g_monitor);
}

void __cdecl OnPluginFree()
{
    if (g_statusBarWnd)
    {
        delete g_statusBarWnd;
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

}
