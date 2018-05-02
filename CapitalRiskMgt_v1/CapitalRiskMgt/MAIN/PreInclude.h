#ifndef _PRE_INCLUDE_H_
#define _PRE_INCLUDE_H_
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <string>
#include <map>
#include <set>
#include <cmath>
#include <fstream>
#include <vector>
#include <CommCtrl.h>
#pragma comment( lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "PluginMgr_API.h"
#include "MainFrame_API.h"
#include "CspAccess_API.h"
#include "StarApi_API.h"
#include "LanguageApi_API.h"
#include "StrategyTrade_API.h"
#include "TradeApi_API.h"
#include "ConfigFrame_API.h"
#include "CommonModule_API.h"
#include "LoginFrm_API.h"

#include "string_ex.h"
#include "_base.h"
#include "load_rc.h"
#include "AnsicUnicode.h"
#include "TradeQuoteString.h"

#include "resource.h"
#include "stdafx.h"
#include "Global.h"

#include "IOperator.h"
#include "UIDesign.h"
#include "IxFrm.h"
#include "TMemDC.h"
//#include "CxSubClassWnd.h"
//#include "CxEdit.h"
//#include "CxButton.h"
//#include "CxStatic.h"
//#include "CxCombox.h"

#include "TPopupWnd.h"
#include "popwincode.h"
#include "TCheckbox.h"
#include "TComboxList.h"
#include "TCombox.h"
#include "TEdit.h"
#include "TStatic.h"
#include "TStaticButton.h"
#include "TModalWnd.h"
#include "TMessageBox.h"

#include "Langeage.h"
#include "CapitalRiskMgtWnd.h"
#include "CapitalRiskMgt.h"

#endif