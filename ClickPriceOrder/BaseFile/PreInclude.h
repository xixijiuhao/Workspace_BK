#ifndef _PRE_INCLUDE_H_
#define _PRE_INCLUDE_H_

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>
#pragma comment( lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <functional> 
#include <regex>

#include "PluginMgr_API.h"
#include "TradeApi_API.h"
#include "TradeApi_Type.h"
#include "MainFrame_API.h"
#include "RawTradeApi_API.h"
#include "LoginFrm_API.h"
#include "StarApi_API.h"

#include "share_struct.h"
#include "ConfigFrame_API.h"
#include "trade_spi_ex.h"
#include "def_lang.h"
#include "load_rc.h"
#include "resource.h"
#include "AnsicUnicode.h"

#include "TDataContainer.h"

#include "IxFrm.h"
#include "TMemDC.h"
#include "UIDesign.h"
#include "TModalWnd.h"

#include "TStaticButton.h"
#include "TListView.h"
#include "TStatic.h"
#include "TEdit.h"
#include "TPopupWnd.h"
#include "TComboxList.h"
#include "TCombox.h"
#include "TLockBtn.h"
#include "TPinEdit.h"
#include "TCheckbox.h"
#include "TDateTime.h"
#include "TMessageBox.h"
#include "popwincode.h"
#include "popwincodelist.h"
#include "popwin.h"
#include "TAccessAlarm.h"
#include "OrderConfirm.h"
#include "FollowOrder.h"
#include "TOrderButton.h"
#include "IPointOrderApi.h"
#include "TPointOrderApi.h"
#include "Export.h"
#include "InsertOrderAssitant.h"
#include "TradeQuoteString.h"
#include "TCanCelAllOrder.h"
#include "TPingPangOrder.h"

#include "ClickPriceOrderTipsFrm.h"
#include "ClickPriceOrderOptionWnd.h"
#include "ClickPriceOrderWnd.h"


#endif




