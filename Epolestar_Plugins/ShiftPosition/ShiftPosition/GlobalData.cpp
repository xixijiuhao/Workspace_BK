#include "BaseInclude.h"

TPluginMgrApi* g_pManager = NULL;
ITradeApi *    g_pTradeApi = NULL;
ITradeData*    g_pTradeData = NULL;
IMainFrame*    g_pMainFrame = NULL;
IStarApi*      g_pStarApi = NULL;

TShiftPosition* g_pShiftPosition = NULL;
ShiftQuoteApi* g_pShiftQuoteApi = NULL;
ShiftTradeApi* g_pShiftTradeApi = NULL;

int g_language = CHS;

G_UIFONT  g_FontData;
G_COLOREF g_ColorRefData;