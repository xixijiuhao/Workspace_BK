#include "PreInclude.h"

ITradeApi * g_pTradeApi = NULL;
ITradeData* g_pTradeData = NULL;
IMainFrame* g_pMainFrm = nullptr;
IStarApi* g_pStarApi = nullptr;
TPluginMgrApi* g_pPluginMgrApi = nullptr;

TPresenter* g_Presenter = nullptr;
IStopLP * g_pStopLP = nullptr;

TGlobalModel *g_pGlobalModel = nullptr;

TradeAction * g_pTradeAction = nullptr;

TMediator * g_pMediator = nullptr;
G_UIFONT  g_FontData;
G_COLOREF g_ColorRefData;
TStatusBar			*g_pStatusBar = NULL;

LanguageID	 g_language;