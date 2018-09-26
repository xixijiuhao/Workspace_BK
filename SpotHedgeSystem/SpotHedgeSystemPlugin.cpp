#include "PreInclude.h"

const wchar_t PLUG_SPOTHEDGESYSTEM_NAME[] = L"SpotHedgeSystem";
const wchar_t PLUG_SPOTHEDGESYSTEM_VERSION[] = L"9.3.30.0";

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
ISpotSystemApi		*g_pSpotSystem = NULL;		//�ױ�ƽ̨
int					g_language;
string				g_specNoPrice("");

vector<SystemFrame*>	g_FrameVec;					//�ױ����� ��ָ�뼯��
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

}

void CreateFrame(const unsigned int MenuIndex, const HWND PastLife)
{
	if (g_manager && g_mainframe && g_pTradeApi && g_pTradeData&&g_pCommonModule && g_pStarApi)//��� ���� ����
	{
		if (!g_FrameVec.empty())
		{
			HWND hwnd = FindWindow(L"class TMainFrame", NULL);
			_TMessageBox_Domodal(hwnd, "��ʾ", L"�Ѵ��ױ�����ҳ�棡", TMB_OK);
			return;
		}

		SystemFrame *frameApi = new SystemFrame();
		frameApi->Create(L"SpotHedgeSystem", NULL, WS_CLIPCHILDREN, 0);
		g_mainframe->PutInnerEx(frameApi->GetHwnd(), OnLinkageAction);
		SetWindowText(frameApi->GetHwnd(), L"�ױ�ϵͳ");
		g_FrameVec.push_back(frameApi);
		frameApi->SetWindow();
		frameApi->switch_index(0, 0);
		//��ȡmanage�ѵ�½�˺�
		int count = g_pSpotSystem->GetLoginCount();
		if ( count > 0)
		{
			SmsLoginRsp** out;
			out = new SmsLoginRsp*[count];
			g_pSpotSystem->GetLogin(out);
			if (out[0])
				frameApi->OnInitComplete(*out[0]);
		}

		//������ѵ�¼���˺ţ������³�ʼ������


	}
}

//////////////////////////////////////////////////////////////////////////
void __cdecl OnPluginLoad(LanguageID l)
{
	g_language = l;
}
//���ȫ��������ɺ�Ļص��¼�
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

	//��ɫ ����
	g_ColorRefData.Init();
	g_FontData.Init(LoadRC::get_lang_id());

	//����ܽӿ�
	g_mainframe = (IMainFrame*)g_manager->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);
	if (g_mainframe)
		g_mainframe->RegMenuItem(MAKE_MENUORDER(0, 10001, 20), L"�ױ�ϵͳ", CreateFrame);

	//���׽ӿ�
	g_pTradeApi = (ITradeApi*)g_manager->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
	if (g_pTradeApi)
		g_pTradeData = g_pTradeApi->GetTradeData();

	//ϵͳѡ��
	g_conFrame = (IConfigFrame*)g_manager->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);

	//ͨ�ýӿ�
	g_pCommonModule = (ICommonModule *)g_manager->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);

	g_pStarApi = (IStarApi *)g_manager->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);

	g_pAccessApi = (ICspAccessApi *)g_manager->CreatePluginApi(PLUG_ACCESSAPI_NAME, PLUG_ACCESSAPI_VERSION);

	//�ױ�����ƽ̨
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
