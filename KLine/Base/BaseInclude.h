#ifndef _BASE_INCLUDE_H_
#define _BASE_INCLUDE_H_

/* windows header and lib */
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <Commdlg.h>
#include <Imm.h>
#include <Richedit.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib,"Wininet.lib") 
#include <GdiPlus.h>  
#pragma comment(lib, "gdiplus.lib")

#include <thread>
/* c++ library */
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>
#include <regex>
#include <stack>
#include <windows.h>
#include <string>
#include <wininet.h>

#include "resource.h"
/* epolestar header*/
#include "PluginMgr_API.h"
#include "LanguageApi_API.h"
#include "TradeApi_API.h"
#include "TradeApi_Type.h"
#include "RawTradeApi_API.h"
#include "MainFrame_API.h"
#include "LoginFrm_API.h"
#include "StarApi_API.h"
#include "HisQuoteShell_API.h"
#include "ConfigFrame_API.h"

#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include "WinTools.h"
#include "httpTools.h"

/* load rc */
#include "def_lang.h"
#include "load_rc.h"
#include "CharCodes.h"

/* TradeSpi */
#include "trade_spi_ex.h"

#include "TDataContainer.h"

#include "TStringConvert.h"

/* model */


#include "IxFrm.h"
#include "TMemDC.h"
#include "TModalWnd.h"
#include "UIDesign.h"
#include "share_struct.h"
#include "TradeQuoteString.h"
#include "AnsicUnicode.h"
#include "TPopupWnd.h"

//#include "popwincode.h"
//#include "popwin.h"
//#include "popwincodelist.h"
#include "VirtualWnd.h"

#include "TMessageBox.h"
#include "CtlDefine.h"
#include "TFile.h"


#include "GlobalData.h"
#include "Export.h"
#include "GlobalDefine.h"
#include "Protocol.h"

#include "KLineUtil.h"


#include "KLineModel.h"
#include "KLineView.h"
#include "MainFrame.h"
#include "KLinePresenter.h"



#endif