#include "PreInclude.h"
#include "MouseOrderConfigWnd.h"

const wchar_t PLUG_MOUSEORDER_NAME[] = L"MouseOrder";
const wchar_t PLUG_MOUSEORDER_VERSION[] = L"9.3.0.1";

//ȫ��
G_COLOREF			g_ColorRefData;				//ɫ��
G_UIFONT			g_FontData;					//����

TPluginMgrApi		* g_manager = NULL;			//����������ӿ� ��ָ��
IMainFrame			* g_mainframe = NULL;		//����ܽӿ� ��ָ��
IConfigFrame		*g_conFrame = NULL;			//ϵͳ���� ��ָ��
ILanguageApi		*g_pLanguage = NULL;		//������Դ

ICspAccessApi		*g_pAccessApi = NULL;		//����ӿ� ��ָ��
ITradeApi			*g_pTradeApi = NULL;		//���׽ӿ� ��ָ��
ITradeData			*g_pTradeData = NULL;		//�������ݽӿ� ��ָ�� 
IStarApi            *g_pStarApi = NULL;         //���� 
ICommonModule       *g_pCommonModule = NULL;	//ͨ�ýӿ�
IStrategyTradeAPI	*g_pStrategyApi = NULL;		//ֹ��ֹӯ ����

CMouseOrderPopWin	g_MousePopwin;				//���򵯳�����
vector<CMousewin*>	g_MouseVec;					//�����µ����� ��ָ�뼯��

MouseConfig         *g_mouseCfg = NULL;         //�������
TMouseConfig		g_MouseOrderStyle;			//�����ɫ����

CMyLock				g_lock;						//��Ϣ֪ͨ ��

extern void GetPropertyByStr(WinProperty &wProperty, char(&cProperty)[MAX_PATH]);
//////////////////////////////////////////////////////////////////////////
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
void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{
	if (g_manager&&g_mainframe&&g_pTradeApi&&g_pTradeData && g_pStarApi)//��� ���� ����
	{
		for (vector<CMousewin*>::iterator it = g_MouseVec.begin(); it != g_MouseVec.end(); it++)
		{
			if (To == (*it)->GetHwnd())
			{
				if (!strcmp(Sender, "PolestarQuote"))//���� K�ߺ�Լ�仯
				{
					(*it)->SetCodeFromQuote(Content, Action);
				}
				else if (!strcmp(Sender, "TradeDisplay"))//�б� �ĵ�
				{
					if (!strcmp(Action, W_CSTYLE_MODIFY) || !strcmp(Action, W_CSTYLE_MODIFY_BATCH))//�ĵ� �����ĵ�
					{
						string sKey1 = Content;
						int npos = sKey1.find_first_of('=');
						if (npos > 0)
							sKey1 = sKey1.substr(npos + 1, sKey1.length());
						else
							return;

						uint OrderId = atoi(sKey1.c_str());
						(*it)->ModifyOrder(OrderId, !strcmp(Action, W_CSTYLE_MODIFY_BATCH));	
					}
					else if (!strcmp(Action, W_BN_DBCLICK) || !strcmp(Action, W_BN_CLICK))//ί�� �ֲֺϼ� �ֲ���ϸ ��������
					{
						string sflag = Content;
						int npos = sflag.find(';');
						if (npos >= 0)
						{
							sflag = sflag.substr(0, npos);
							if (sflag == LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_PosTotal)))
								(*it)->SetCodeFromPosTotal(Content, Action);
							else if (sflag == LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_PosDetails)))
								(*it)->SetCodeFromPosSub(Content, Action);
							else if (sflag == LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_CommissionInfo)))
								(*it)->SetCodeFromOrder(Content, Action);
							else if (sflag == LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_SpreadTab)))
								(*it)->SetCodeFromSpread(Content, Action);
						}
					}
					else if (!strcmp(Action, W_HOLD_OPTION))	//�ֲ��б� ͬ����Ȩ����
					{
						string sflag = Content;
						int npos = sflag.find(';');
						if (npos >= 0)
						{
							sflag = sflag.substr(0, npos);
							if (sflag == LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_PosTotal)))
								(*it)->SetExecFromPosTotal(Content);
						}
					}
				}
				else if (!strcmp(Sender, "CommonModule"))//�ĵ���� ����ָ�
				{
					if (!strcmp(Action, W_MODIFY_RECOVER))		
						(*it)->SetModifyRecover();
				}
                else if (strcmp(Sender, "SYSMENU_SENDER") == 0)//ϵͳ�˵� ͬ���л���ǩ
                {
                    if (strcmp(Action, W_ON_MENUITEM) == 0)		
                    {
                        (*it)->SetHeadData(LoadRC::ansi_to_unicode(Content).c_str(), false);
                    }
                }
				break;
			}
		}
	}
}
void MouseWin(const unsigned int MenuIndex, const HWND PastLife)
{
	WinProperty wProperty;

	SConfigSwitchInfo *sInfo = { 0 };
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_INNER_FOREIGN, &sInfo, 1, false) != 0 &&
        strcmp(sInfo->ConfigSwitchNo, S_CONFIGNO_INNER_FOREIGN) == 0 && 
        sInfo->ConfigSwitchValue == S_CONFIGVALUE_INNER)
    {
        wProperty.cCoverMode = oCover;      /// ����
    }
    else
    {
        wProperty.cCoverMode = oNone;       /// ����
    }

	if (PastLife != 0)
	{
		char cProperty[MAX_PATH] = { 0 };
		g_mainframe->GetProperty(PastLife, cProperty, MAX_PATH);
		g_mainframe->SetProperty(PastLife, "");

		GetPropertyByStr(wProperty, cProperty);
	}
	
	if (g_manager&&g_mainframe&&g_pTradeApi&&g_pTradeData&&g_pCommonModule && g_pStarApi)//��� ���� ����
	{
		CMousewin *mouseApi = new CMousewin(wProperty);
		mouseApi->Create(L"MouseOrder", NULL, WS_CLIPCHILDREN, 0);
		
		g_mainframe->PutInnerEx(mouseApi->GetHwnd(), OnLinkageAction);
		SetWindowText(mouseApi->GetHwnd(), LoadResWchar_t(IDS_GENERALORDER));
		g_MouseVec.push_back(mouseApi);
	}
}

//ϵͳ����  �����ʽ���ô���
HWND __stdcall GetCfgWnd(const wchar_t *subtitle, const HWND parent)
{
    if (wcscmp(subtitle, LoadResWchar_t(IDS_Ladder_Style)) == 0)
    {
        if (g_mouseCfg)
        {
            if (!IsWindow(g_mouseCfg->GetConfigWnd()))
            {
                g_mouseCfg->CreateCfgWnd(subtitle);
            }
            return g_mouseCfg->GetConfigWnd();
        }
    }
    return NULL;
}

void WriteConfig(const char *key, const char *value)
{
    char file[MAX_PATH];
    GetModuleFileNameA(nullptr, file, MAX_PATH);
    string sRoot = file;
    int npos = sRoot.find_last_of('\\');
    sRoot = sRoot.substr(0, npos);
    string path = sRoot + ("\\config\\MouseOrder.pri");
    
    WritePrivateProfileStringA("Config", key, value, path.c_str());
}

void GetConfig(const char *key, char *value)
{
    char file[MAX_PATH];
    GetModuleFileNameA(nullptr, file, MAX_PATH);
    string sRoot = file;
    int npos = sRoot.find_last_of('\\');
    sRoot = sRoot.substr(0, npos);
    string path = sRoot + ("\\config\\MouseOrder.pri");

    GetPrivateProfileStringA("Config", key, "", value, MAX_PATH, path.c_str());
}

int GetConfigInt(const char *key)
{
    char file[MAX_PATH];
    GetModuleFileNameA(nullptr, file, MAX_PATH);
    string sRoot = file;
    int npos = sRoot.find_last_of('\\');
    sRoot = sRoot.substr(0, npos);
    string path = sRoot + ("\\config\\MouseOrder.pri");

    return GetPrivateProfileIntA("Config", key, 0, path.c_str());
}

void __cdecl GOnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
    if (Exclusive)
        g_MousePopwin.OnHotKeyDown(Alt, Ctrl, Shift, VKey, Exclusive, source);
}


//////////////////////////////////////////////////////////////////////////
void __cdecl OnPluginLoad(LanguageID l)
{
	
}
//���ȫ��������ɺ�Ļص��¼�
void __cdecl OnPluginInit(TPluginMgrApi* p)
{
	if (p == NULL)
		return;
	if (g_manager==NULL)
		g_manager = p;
	
	g_pLanguage = (ILanguageApi*)g_manager->CreatePluginApi(PLUG_LANGUAGEAPI_NAME,PLUG_LANGUAGEAPI_VERSION);
	if (g_pLanguage)
	{
		g_pLanguage->SetModule(PLUG_MOUSEORDER_NAME);
		LoadRC::set_language(g_pLanguage->LangId());
	}

	g_MouseOrderStyle.nOrderListStyle = GetConfigInt("LadderStyle");
	g_MouseOrderStyle.nParqtyStyle = GetConfigInt("ParqtyStyle");

	//��ɫ ����
	g_ColorRefData.Init();
	g_FontData.Init(LoadRC::get_lang_id());

	//����ܽӿ�
	g_mainframe = (IMainFrame*)g_manager->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
	if (g_mainframe)
		g_mainframe->RegMenuItem(MAKE_MENUORDER(0, 10001, 8), (LoadResWchar_t(IDS_GENERALORDER)), MouseWin);
	
	//���׽ӿ�
	g_pTradeApi = (ITradeApi*)g_manager->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
	if (g_pTradeApi)
		g_pTradeData = g_pTradeApi->GetTradeData();

    if (!g_mouseCfg)
    {
        g_mouseCfg = new MouseConfig;
    }

	//ϵͳѡ��
	g_conFrame = (IConfigFrame*)g_manager->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);
    g_conFrame->reg_config_dlg(LoadResWchar_t(IDS_Ladder_Style), (dlg_create_func)GetCfgWnd, cmtGeneral, 20, "esunny.epolestar.configframe");

	//ͨ�ýӿ�
	g_pCommonModule = (ICommonModule *)g_manager->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);
	
	//ֹ��ֹӯ
	g_pStrategyApi = (IStrategyTradeAPI *)g_manager->CreatePluginApi(PLUG_STRATEGYTRADE_NAME, PLUG_STRATEGYTRADE_VERSION);

    g_pStarApi = (IStarApi *)g_manager->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

	g_pAccessApi = (ICspAccessApi *)g_manager->CreatePluginApi(PLUG_ACCESSAPI_NAME, PLUG_ACCESSAPI_VERSION);
	//��������
	g_MousePopwin.Init();

    //ע��F12��ݼ�����ʾ�����µ�
    g_mainframe->RegHotKey(false, false, false, VK_F12, true, GOnHotKeyDown);
}

void __cdecl OnPluginFree()
{
	g_MousePopwin.Deinit();

	for (vector<CMousewin*>::iterator iter = g_MouseVec.begin(); iter != g_MouseVec.end();)
	{
		if (*iter)
		{
			delete (*iter);
			iter = g_MouseVec.erase(iter);
		}
		else
			iter++;
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