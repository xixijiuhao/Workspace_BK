#ifndef _PRE_INCLUDE_H_
#define _PRE_INCLUDE_H_

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>
#pragma comment( lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <GdiPlus.h>  
#pragma comment(lib, "gdiplus.lib")
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <functional> 


#include "PluginMgr_API.h"
#include "LanguageApi_API.h"
#include "TradeApi_API.h"
#include "TradeApi_Type.h"
#include "MainFrame_API.h"
#include "LoginFrm_API.h"
#include "RawTradeApi_API.h"
#include "StarApi_API.h"
#include "StarApi_TYPE.h"
#include "CommonModule_API.h"
#include "RawTradeApi_Struct.h"
#include "CommonModule_Struct.h"
#include "share_struct.h"
#include "ConfigFrame_API.h"
//#include "trade_spi_ex.h"
#include "def_lang.h"
#include "load_rc.h"
#include "AnsicUnicode.h"
//#include "IOperator.h"

#include "TDataContainer.h"

#include "IxFrm.h"
#include "TMemDC.h"
#include "UIDesign.h"
#include "TModalWnd.h"


#include "TradeQuoteString.h"

#endif






