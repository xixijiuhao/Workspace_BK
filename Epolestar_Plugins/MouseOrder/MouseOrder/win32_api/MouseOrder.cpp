#include "PreInclude.h"
#include "MouseOrderConfigWnd.h"

const wchar_t PLUG_MOUSEORDER_NAME[] = L"MouseOrder";
const wchar_t PLUG_MOUSEORDER_VERSION[] = L"9.3.0.1";

//全局
G_COLOREF			g_ColorRefData;				//色彩
G_UIFONT			g_FontData;					//字体

TPluginMgrApi		* g_manager = NULL;			//插件管理器接口 类指针
IMainFrame			* g_mainframe = NULL;		//主框架接口 类指针
IConfigFrame		*g_conFrame = NULL;			//系统配置 类指针
ILanguageApi		*g_pLanguage = NULL;		//语言资源

ICspAccessApi		*g_pAccessApi = NULL;		//接入接口 类指针
ITradeApi			*g_pTradeApi = NULL;		//交易接口 类指针
ITradeData			*g_pTradeData = NULL;		//交易数据接口 类指针 
IStarApi            *g_pStarApi = NULL;         //行情 
ICommonModule       *g_pCommonModule = NULL;	//通用接口
IStrategyTradeAPI	*g_pStrategyApi = NULL;		//止损止盈 套利

CMouseOrderPopWin	g_MousePopwin;				//竖向弹出窗口
vector<CMousewin*>	g_MouseVec;					//竖向下单界面 类指针集合

MouseConfig         *g_mouseCfg = NULL;         //点价配置
TMouseConfig		g_MouseOrderStyle;			//点价配色方案

CMyLock				g_lock;						//消息通知 锁

extern void GetPropertyByStr(WinProperty &wProperty, char(&cProperty)[MAX_PATH]);
//////////////////////////////////////////////////////////////////////////
extern "C"
{
	//
	void __cdecl OnPluginLoad(LanguageID l);

	//插件全部加载完成后的回调事件
	void __cdecl OnPluginInit(TPluginMgrApi* p);

	//插件释放回调事件
	void __cdecl OnPluginFree();

	//插件暴露API
	void* __cdecl CreatePluginApi(const wchar_t v[]);

	//释放接口
	void __cdecl FreePluginApi(void* p);

}
void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{
	if (g_manager&&g_mainframe&&g_pTradeApi&&g_pTradeData && g_pStarApi)//多个 还是 单个
	{
		for (vector<CMousewin*>::iterator it = g_MouseVec.begin(); it != g_MouseVec.end(); it++)
		{
			if (To == (*it)->GetHwnd())
			{
				if (!strcmp(Sender, "PolestarQuote"))//行情 K线合约变化
				{
					(*it)->SetCodeFromQuote(Content, Action);
				}
				else if (!strcmp(Sender, "TradeDisplay"))//列表 改单
				{
					if (!strcmp(Action, W_CSTYLE_MODIFY) || !strcmp(Action, W_CSTYLE_MODIFY_BATCH))//改单 批量改单
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
					else if (!strcmp(Action, W_BN_DBCLICK) || !strcmp(Action, W_BN_CLICK))//委托 持仓合计 持仓明细 本地套利
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
					else if (!strcmp(Action, W_HOLD_OPTION))	//持仓列表 同步行权操作
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
				else if (!strcmp(Sender, "CommonModule"))//改单完成 界面恢复
				{
					if (!strcmp(Action, W_MODIFY_RECOVER))		
						(*it)->SetModifyRecover();
				}
                else if (strcmp(Sender, "SYSMENU_SENDER") == 0)//系统菜单 同步切换标签
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
        wProperty.cCoverMode = oCover;      /// 内盘
    }
    else
    {
        wProperty.cCoverMode = oNone;       /// 外盘
    }

	if (PastLife != 0)
	{
		char cProperty[MAX_PATH] = { 0 };
		g_mainframe->GetProperty(PastLife, cProperty, MAX_PATH);
		g_mainframe->SetProperty(PastLife, "");

		GetPropertyByStr(wProperty, cProperty);
	}
	
	if (g_manager&&g_mainframe&&g_pTradeApi&&g_pTradeData&&g_pCommonModule && g_pStarApi)//多个 还是 单个
	{
		CMousewin *mouseApi = new CMousewin(wProperty);
		mouseApi->Create(L"MouseOrder", NULL, WS_CLIPCHILDREN, 0);
		
		g_mainframe->PutInnerEx(mouseApi->GetHwnd(), OnLinkageAction);
		SetWindowText(mouseApi->GetHwnd(), LoadResWchar_t(IDS_GENERALORDER));
		g_MouseVec.push_back(mouseApi);
	}
}

//系统配置  点价样式配置窗口
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
//插件全部加载完成后的回调事件
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

	//颜色 字体
	g_ColorRefData.Init();
	g_FontData.Init(LoadRC::get_lang_id());

	//主框架接口
	g_mainframe = (IMainFrame*)g_manager->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
	if (g_mainframe)
		g_mainframe->RegMenuItem(MAKE_MENUORDER(0, 10001, 8), (LoadResWchar_t(IDS_GENERALORDER)), MouseWin);
	
	//交易接口
	g_pTradeApi = (ITradeApi*)g_manager->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
	if (g_pTradeApi)
		g_pTradeData = g_pTradeApi->GetTradeData();

    if (!g_mouseCfg)
    {
        g_mouseCfg = new MouseConfig;
    }

	//系统选项
	g_conFrame = (IConfigFrame*)g_manager->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);
    g_conFrame->reg_config_dlg(LoadResWchar_t(IDS_Ladder_Style), (dlg_create_func)GetCfgWnd, cmtGeneral, 20, "esunny.epolestar.configframe");

	//通用接口
	g_pCommonModule = (ICommonModule *)g_manager->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);
	
	//止损止盈
	g_pStrategyApi = (IStrategyTradeAPI *)g_manager->CreatePluginApi(PLUG_STRATEGYTRADE_NAME, PLUG_STRATEGYTRADE_VERSION);

    g_pStarApi = (IStarApi *)g_manager->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

	g_pAccessApi = (ICspAccessApi *)g_manager->CreatePluginApi(PLUG_ACCESSAPI_NAME, PLUG_ACCESSAPI_VERSION);
	//弹出创库
	g_MousePopwin.Init();

    //注册F12快捷键，显示竖向下单
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