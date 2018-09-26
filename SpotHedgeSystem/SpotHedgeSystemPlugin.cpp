#include "PreInclude.h"

const wchar_t PLUG_SPOTHEDGESYSTEM_NAME[] = L"SpotHedgeSystem";
const wchar_t PLUG_SPOTHEDGESYSTEM_VERSION[] = L"9.3.30.0";

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
ISpotSystemApi		*g_pSpotSystem = NULL;		//套保平台
int					g_language;
string				g_specNoPrice("");

vector<SystemFrame*>	g_FrameVec;					//套保界面 类指针集合
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

}

void CreateFrame(const unsigned int MenuIndex, const HWND PastLife)
{
	if (g_manager && g_mainframe && g_pTradeApi && g_pTradeData&&g_pCommonModule && g_pStarApi)//多个 还是 单个
	{
		if (!g_FrameVec.empty())
		{
			HWND hwnd = FindWindow(L"class TMainFrame", NULL);
			_TMessageBox_Domodal(hwnd, "提示", L"已打开套保管理页面！", TMB_OK);
			return;
		}

		SystemFrame *frameApi = new SystemFrame();
		frameApi->Create(L"SpotHedgeSystem", NULL, WS_CLIPCHILDREN, 0);
		g_mainframe->PutInnerEx(frameApi->GetHwnd(), OnLinkageAction);
		SetWindowText(frameApi->GetHwnd(), L"套保系统");
		g_FrameVec.push_back(frameApi);
		frameApi->SetWindow();
		frameApi->switch_index(0, 0);
		//获取manage已登陆账号
		int count = g_pSpotSystem->GetLoginCount();
		if ( count > 0)
		{
			SmsLoginRsp** out;
			out = new SmsLoginRsp*[count];
			g_pSpotSystem->GetLogin(out);
			if (out[0])
				frameApi->OnInitComplete(*out[0]);
		}

		//如果有已登录的账号，就重新初始化界面


	}
}

//////////////////////////////////////////////////////////////////////////
void __cdecl OnPluginLoad(LanguageID l)
{
	g_language = l;
}
//插件全部加载完成后的回调事件
void __cdecl OnPluginInit(TPluginMgrApi* p)
{
	if (p == NULL)
		return;
	if (g_manager == NULL)
		g_manager = p;

	g_pLanguage = (ILanguageApi*)g_manager->CreatePluginApi(PLUG_LANGUAGEAPI_NAME, PLUG_LANGUAGEAPI_VERSION);
	if (g_pLanguage)
	{
		g_pLanguage->SetModule(PLUG_SPOTHEDGESYSTEM_NAME);
		LoadRC::set_language(g_pLanguage->LangId());
	}

	//颜色 字体
	g_ColorRefData.Init();
	g_FontData.Init(LoadRC::get_lang_id());

	//主框架接口
	g_mainframe = (IMainFrame*)g_manager->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
	if (g_mainframe)
		g_mainframe->RegMenuItem(MAKE_MENUORDER(0, 10001, 20), L"套保系统", CreateFrame);

	//交易接口
	g_pTradeApi = (ITradeApi*)g_manager->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
	if (g_pTradeApi)
		g_pTradeData = g_pTradeApi->GetTradeData();

	//系统选项
	g_conFrame = (IConfigFrame*)g_manager->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);

	//通用接口
	g_pCommonModule = (ICommonModule *)g_manager->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);

	g_pStarApi = (IStarApi *)g_manager->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

	g_pAccessApi = (ICspAccessApi *)g_manager->CreatePluginApi(PLUG_ACCESSAPI_NAME, PLUG_ACCESSAPI_VERSION);

	//套保管理平台
	g_pSpotSystem = (ISpotSystemApi* )g_manager->CreatePluginApi(PLUG_SPOTSYSTEM_NAME, PLUG_SPOTSYSTEM_VERSION);

}

void __cdecl OnPluginFree()
{
	for (vector<SystemFrame*>::iterator iter = g_FrameVec.begin(); iter != g_FrameVec.end();)
	{
		if (*iter)
		{
			delete (*iter);
			iter = g_FrameVec.erase(iter);
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
