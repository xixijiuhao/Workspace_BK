#pragma once


#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

#include <windows.h>
#include <windowsx.h>


#include "PluginMgr_API.h"
#include "ConfigFrame_API.h"
#include "MainFrame_API.h"
#include "TradeApi_API.h"
#include "StarApi_API.h"
#include "CspAccess_API.h"
#include "RawTradeApi_API.h"
#include "CommonModule_API.h"
#include "LanguageApi_API.h"
#include "IxFrm.h"
#include "LoginFrm_API.h"
#include "resource.h"
#include "load_rc.h"
#include <cmath>
#include <map>
#include <vector>
#include <set>
#include <wchar.h>
#include <Global.h>
#include "datas.h"

using namespace std;
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")

#include "UIDesign.h"
#include "CxSubClassWnd.h"
#include "CxEdit.h"
#include "CxStatic.h"
#include "CxButton.h"
#include "CxCheck.h"
#include "CxSpin.h"
#include "popwincode.h"
#include "resource.h"
//#include "msgpopwin.h"

#include "TradeQuoteString.h"
#include "position_diff_exp.h"
#include "position_config.h"
#include "position_trade.h"
#include "position_bar.h"




