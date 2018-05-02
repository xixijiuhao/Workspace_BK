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


#include <thread>
/* c++ library */
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>
#include <regex>

#include "resource.h"
/* epolestar header*/
#include "PluginMgr_API.h"
#include "TradeApi_API.h"
#include "TradeApi_Type.h"
#include "RawTradeApi_API.h"
#include "MainFrame_API.h"
#include "LoginFrm_API.h"
#include "StarApi_API.h"
#include "ConfigFrame_API.h"

/* load rc */
#include "def_lang.h"
#include "load_rc.h"
#include "CharCodes.h"

/* TradeSpi */
#include "trade_spi_ex.h"

#include "TDataContainer.h"

#include "TStringConvert.h"

#include "IxFrm.h"
#include "TMemDC.h"
#include "TModalWnd.h"
#include "UIDesign.h"
#include "share_struct.h"
#include "TradeQuoteString.h"
#include "AnsicUnicode.h"
#include "TPopupWnd.h"

#include "VirtualWnd.h"

#include "TMessageBox.h"
#include "CtlDefine.h"

#include "popwincode.h"
#include "popwin.h"
#include "popwincodelist.h"

#include "IAppFramework.h"

#include "TStopLPEx.h"
#include "TStopLP.h"

#include "ConfigFrms.h"
#include "TConfigFrm.h"

#include "TOpenStopFrm.h"
#include "TConditionOrderFrm.h"
#include "TModifyConView.h"


//#include "TPositionView.h"

#include "TSpecialPrice.h"
#include "TShiftPosition.h"
#include "TOrderView.h"
#include "TTradeInfo.h"
#include "TLogView.h"

#include "TStopLPView.h"

#include "ViewThreeKey.h"

//#include "TChaseOrder.h"
#include "TradeAction.h"

#include "TWriteInfo.h"
#include "TUserData.h"
#include "TConOrderModel.h"
#include "TThreeKeyData.h"
#include "TradeInfoModel.h"
#include "TAddModStopLPData.h"

//#include "Test.h"

#include "Presenter.h"
#include "TStatusBar.h"

#include "Mediator.h"

#include "Model.h"

#include "TKeyOrderApi.h"


#include "GlobalData.h"
#include "Export.h"

#endif