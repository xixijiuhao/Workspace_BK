#ifndef _BASE_INCLUDE_H_
#define _BASE_INCLUDE_H_

/* windows header and lib */
#include <windows.h>
#include <windowsx.h>
#include <Richedit.h>
#include <tchar.h>
#include <CommCtrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment( lib, "comctl32.lib") 


/* c++ library */
#include <string>

#include <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>
#include <iterator>
#include <regex>

/* epolestar header*/
#include "PluginMgr_API.h"
#include "TradeApi_Struct.h"
#include "TradeApi_API.h"
#include "TradeApi_Type.h"
#include "MainFrame_API.h"
#include "LoginFrm_API.h"
#include "share_struct.h"
#include "StarApi_API.h"
#include "StarApi_TYPE.h"


#include "TStringConvert.h"
#include "ConfigFrame_API.h"
#include "RawTradeApi_API.h"
#include "RawTradeApi_Struct.h"



/* load rc */
#include "def_lang.h"
#include "load_rc.h"
#include "CharCodes.h"

/* TradeSpi */
#include "trade_spi_ex.h"
#include "TDataContainer.h"
#include "AnsicUnicode.h"

#include "TradeQuoteString.h"

#include "IxFrm.h"
#include "TMemDC.h"
#include "TModalWnd.h"
#include "UIDesign.h"

#include "TMessageBox.h"

#include "CtlDefine.h"
#include "popwincodelist.h"
#include "popwincode.h"

#include "GlobalData.h"
#include "Export.h"


#include "VirtualWnd.h"
#include "ShiftPosition_Stuct.h"
#include "ShiftQuoteApi.h"
#include "OrderAction.h"
#include "TShiftCondition.h"
#include "TShiftOrderConfig.h"
#include "TAddContractView.h"
#include "TShiftPositionView.h"



#endif