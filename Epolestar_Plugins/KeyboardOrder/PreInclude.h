#ifndef _KEYBOARD_PREINCLUDE_
#define _KEYBOARD_PREINCLUDE_

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>
#include <windowsx.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <imm.h>
#include <regex> 
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "comctl32.lib")

#include "resource.h"
#include "commctrl.h"
#include "ctrl\IxFrm.h"
#include "load_rc.h"
#include "UIDesign.h"
#include "VKDefine.h"
#include "share\LockDef.h"
#include "Language.h"

#include "PluginMgr_API.h"
#include "StarApi_API.h"
#include "StarApi_TYPE.h"
#include "StrategyTrade_API.h"
#include "TradeApi_API.h"
#include "RawTradeApi_API.h"
#include "MainFrame_API.h"
#include "ConfigFrame_API.h"
#include "TradeApi_Type.h"
#include "ConfigFrame_API.h"
#include "CommonModule_API.h"
//#include "..\..\EsUpdate\include\BatchOrder_API.h"
#include "LanguageApi_API.h"

#include "trade_spi_ex.h"

#include "share\share_struct.h"
#include "share\AnsicUnicode.h"
#include "share\TradeQuoteString.h"
#include "share\IOperator.h"
#include "share\popwincode.h"
#include "share\popwincodeQ.h"

#include "ctrl\DrawHelper.h"
#include "ctrl\CxCombox.h"
#include "ctrl\CxSpin.h"
#include "ctrl\CxEdit.h"
#include "ctrl\CxCheck.h"

#include "ctrl\LockBtn.h"
#include "ctrl\DrawStatic.h"
#include "ctrl\CodeNameStatic.h"
#include "ctrl\OrderBtn.h"
#include "ctrl\SubClassEdit.h"
#include "ctrl\CxKeyBoardButton.h"
#include "ctrl\CxComboBox.h"
#include "ctrl\CxKeyBoardEdit.h"
#include "ctrl\CxKeyBoardSpin.h"
#include "ctrl\CxKeyBoardCheck.h"
#include "ctrl\SwitchBtn.h"
#include "ctrl\MultiColEdit.h"
#include "ctrl\MultiColComboBox.h"
#include "ctrl\CxDataCtrl.h"
#include "ctrl\TListView.h"

#include "PlugLoad.h"
#include "thread/Thread.h"
#include "KeyBoard_Struct.h"
#include "OFInputWndProc.h"
#include "KeyBoardSet.h"
#include "KeyBoardSort.h"
#include "KeyBoardConfig.h"
#include "ctrl\IxDlg.h"

#include "DataContainer_Trade.h"
#include "GeneralGlobal.h"
#include "trade_const_trans.h"

#include "OFInputDlg.h"


#endif