#ifndef _PRE_INCLUDE_H_
#define _PRE_INCLUDE_H_

#include <windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <CommCtrl.h>
#pragma comment( lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <time.h>
#include <Richedit.h>
#include <regex>

#include "PluginMgr_API.h"
#include "MainFrame_API.h"
#include "LoginFrm_API.h"
#include "TradeApi_API.h"
#include "TradeApi_Type.h"
#include "StarApi_API.h"
#include "RawTradeApi_API.h"

#include "Logging_API.h"


#include "share_struct.h"
#include "TradeQuoteString.h"
#include "TStringConvert.h"


#include "def_lang.h"
#include "load_rc.h"
#include "CharCodes.h"

#include "IxFrm.h"
#include "TMemDC.h"
#include "UIDesign.h"
#include "TModalWnd.h"


#include "TStaticButton.h"
#include "TListView.h"
#include "TStatic.h"



#include "CxSubClassWnd.h"
#include "CxEdit.h"
#include "CxCombox.h"
#include "CxSpin.h"
#include "CxSpinEdit.h"
#include "CxCheck.h"

#include "TMessageBox.h"

#include "TAccessAlarm.h"
#include "TRichEdit.h"

#include "trade_spi_ex.h"
#include "TDataContainer.h"


#include "TFollowConfig.h"

#include "TradeModel.h"

#include "TradeInfo.h"
#include "TFollowTrade.h"


#include "Export.h"




#endif
