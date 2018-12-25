#include "PreInclude.h"

const wchar_t PLUG_KEYBOARDORDER_NAME[] = L"KeyboardOrder";
const wchar_t PLUG_KEYBOARDORDER_VERSION[] = L"9.3.0.1";
//ȫ�ֶ�����
ITradeApi * g_pTradeApi;
ITradeData * g_pTradeData;
ILanguageApi * g_pLanguage;
IStarApi * g_StarApi;
G_COLOREF	g_ColorRefData;
G_UIFONT	g_FontData;					//ȫ������

CPlugLoad::CPlugLoad() :m_pPluginMgr(NULL)
, m_pConfigFrame(NULL)
, m_pMainFrm(NULL)
, m_pStarApi(NULL)
, m_pCommonModule(NULL)
{

}
CPlugLoad::~CPlugLoad()
{
	
}
void CPlugLoad::Init(TPluginMgrApi* p)
{
	if (m_pPluginMgr) 
		return;
	m_pPluginMgr = p;

	
	g_pLanguage = (ILanguageApi *)p->CreatePluginApi(PLUG_LANGUAGEAPI_NAME, PLUG_LANGUAGEAPI_VERSION);
	if (g_pLanguage)
	{
		g_pLanguage->SetModule(PLUG_KEYBOARDORDER_NAME);
		LoadRC::set_language(g_pLanguage->LangId());
	}

	//��ɫ����
	g_ColorRefData.Init();
	g_FontData.Init(LoadRC::get_lang_id());

	//ϵͳ����
	if (!m_pConfigFrame)
		m_pConfigFrame = (IConfigFrame*)p->CreatePluginApi(PLUG_CONFIGFRAME_NAME, PLUG_CONFIGFRAME_VERSION);

	//���
	if (!m_pMainFrm)
		m_pMainFrm = (IMainFrame*)p->CreatePluginApi(PLUG_MAINFRAME_NAME, PLUG_MAINFRAME_VERSION);

	//����API
	if (!m_pStarApi)
	{
		m_pStarApi = (IStarApi *)p->CreatePluginApi(PLUG_STARAPI_NAME, PLUG_STARAPI_VERSION);
		g_StarApi = m_pStarApi;
	}
		
	//����api
	ITradeApi *pInterface = (ITradeApi*)p->CreatePluginApi(PLUG_TRADEAPI_NAME, PLUG_TRADEAPI_VERSION);
	if (pInterface)
	{
		g_pTradeApi = pInterface;
		g_pTradeData = pInterface->GetTradeData();
	}

	//ͨ��
	if (!m_pCommonModule)
		m_pCommonModule = (ICommonModule*)p->CreatePluginApi(PLUG_COMMONMOULE_NAME, PLUG_COMMONMOULE_VERSION);
	//���Ե�
	if (!m_pStrategyTrade)
		m_pStrategyTrade = (IStrategyTradeAPI*)p->CreatePluginApi(PLUG_STRATEGYTRADE_NAME, PLUG_STRATEGYTRADE_VERSION);
	
}
