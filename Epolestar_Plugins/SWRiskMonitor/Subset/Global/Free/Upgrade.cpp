#include "stdafx.h"
#include "Upgrade.h"
#include "Logs.h"
#include "ErrorConsts.h"

//#pragma comment(lib, "Upgrade.lib")

class TIUpgrader
{
public:
	virtual void __cdecl SetProxy(char * ProxyIp, int ProxyPort) = 0;
	virtual void __cdecl Upgrader(int AutoUpgrade) = 0;
	virtual void __cdecl FreeSelf() = 0;
};
//extern "C" __declspec(dllimport) TIUpgrader* __stdcall CreateUpgrader(char * ConfigFileName); 

extern CEnvironment g_Environment;


CUpgrader * g_pUpgrader = NULL;               
TCriticalSection CUpgrader::m_CritSec;
/////////////////////////////////////////////////////////////////////////////////////////////////////
CUpgrader * CUpgrader::GetInstance(char * ConfigFileName)
{
	LockScope();

	if (g_pUpgrader==NULL)
		g_pUpgrader = new CUpgrader(ConfigFileName);
	return g_pUpgrader; 
}

CUpgrader::CUpgrader(char * ConfigFileName)
	: m_pIUpgrader(NULL)
	, m_hDll(0)
{
	char sUpgradeIni[MAX_PATH];
	if (ConfigFileName==NULL || strlen(ConfigFileName)==0)
		sprintf_s(sUpgradeIni, "%s%s", g_Environment.m_strSystemPath, "Servers.ini");	//默认配置
	else
		strcpy_s(sUpgradeIni, ConfigFileName);

	//静态调用
	//pIUpgrader = CreateUpgrader(sUpgradeIni);

	//动态调用	容错、方便调试
	m_hDll = LoadLibrary(g_Environment.m_strUpgradeDll.GetString());
	if (TLogs::GetInstance().IsReight(m_hDll!=0, Error_Lose_Dll, "dll load failed: \"%s\"", g_Environment.m_strUpgradeDll.GetString()))
	{
		typedef TIUpgrader* (__stdcall * PCreateUpgrader)(char * ConfigFileName);
		PCreateUpgrader pCreateUpgrader = (PCreateUpgrader)GetProcAddress(m_hDll, "CreateUpgrader");
		if (TLogs::GetInstance().IsReight(pCreateUpgrader!=NULL, Error_Lose_DllFun, "Not find function: \"CreateUpgrader\""))
			m_pIUpgrader = pCreateUpgrader(sUpgradeIni);
	}
}

CUpgrader::~CUpgrader()
{
	if (m_pIUpgrader!=NULL)
		m_pIUpgrader->FreeSelf();
	if (m_hDll!=0)
		FreeLibrary(m_hDll);	
}

void CUpgrader::SetProxy(char * ProxyIp, int ProxyPort)
{
	if (m_pIUpgrader!=NULL)
		m_pIUpgrader->SetProxy(ProxyIp, ProxyPort);
}

void CUpgrader::Upgrader(int AutoUpgrade)
{
	if (m_pIUpgrader!=NULL)
		m_pIUpgrader->Upgrader(AutoUpgrade);
}