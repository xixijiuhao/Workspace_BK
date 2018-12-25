#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>
#include <windowsx.h>
#include <atlconv.h>
#include "commdlg.h"
#include <map>
#include <set>
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
//#include <cassert>
#include <imm.h>
#include <regex> 
#include <tchar.h>
#include <CommCtrl.h>
#include <mutex>
#include <ole2.h>
#include "gdiplus.h" 
//using namespace Gdiplus;

#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "GdiPlus.lib") 

#include "resource.h"
#include "Language.h"
#include "commctrl.h"

#include "load_rc.h"
#include "UIDesign.h"
#include "string_ex.h"

#include "PluginMgr_API.h"
#include "TradeApi_API.h"
#include "RawTradeApi_API.h"
#include "StarApi_API.h"
#include "StarApi_TYPE.h"
#include "CspAccess_API.h"
#include "MainFrame_API.h"
#include "ConfigFrame_API.h"
#include "TradeApi_Type.h"
#include "CommonModule_API.h"
#include "StrategyTrade_API.h"
#include "LanguageApi_API.h"
#include "SpotSystem_API.h"
//#include "trade_spi_ex.h"
#include "Str2Time.h"

#include "AnsicUnicode.h"
#include "trade_spi_ex.h"
#include "TMemDC.h"

#include "IxFrm.h"
#include "TPopupWnd.h"
#include "CxSubClassWnd.h"
//#include "TDefine.h"
#include "TStringConvert.h"
//#include "MouseConvert.h"
#include "GlobalMessage.h"
#include "TListView.h"
#include "popwindate.h"
#include "TStatic.h"
#include "CxEdit.h"
#include "CxSpin.h"
#include "CxSpinEdit.h"
#include "CxCombox.h"
#include "TStaticButton.h"
#include "CxComboxTime.h"
#include "TMessageBox.h"
#include "TLoginPopWnd.h"
#include "TAgreementPopWnd.h"
#include "popwincode.h"
#include "CxRichEdit.h"
#include "OrderButton.h"
#include "CxCheck.h"

#include"TradeQuoteString.h"

#include "TDefine.h"
#include "GridData.h"
#include "CellEdit.h"
#include "MiniEdit.h"
#include "FilterList.h"
#include "GridCellBase.h"
#include "GridCheckCell.h"
#include "GridEditCell.h"
#include "GridComboxCell.h"
#include "GridTreeCell.h"
#include "GridCursor.h"

#include "GridHeaderCell.h"

#include "GridCtrl.h"
//#include "TDataInfo.h"
//#include "TFormProcess.h"




#include "SystemFrame.h"
#include "SpecWnd.h"
#include "AddAgreementWnd.h"
#include "AddAgreementRelWnd.h"
#include "ModifyOrderWnd.h"
#include "AgreementWnd.h"
#include "SpecPriceWnd.h"
#include "SettlePriceWnd.h"
#include "CashInOutWnd.h"
#include "AddPositionWnd.h"
#include "PositionWnd.h"
#include "OrderWnd.h"
#include "AddFundWnd.h"
#include "FundWnd.h"
#include "TaxWnd.h"
#include "StatementWnd.h"
#include "AddManualCloseWnd.h"
#include "AddMatchWnd.h"
#include "MatchWnd.h"
#include "SendOrderWnd.h"
#include "PartAccountWnd.h"
#include "AllAccountWnd.h"
#include "InstantPositionWnd.h"
#include "ExposureWnd.h"

#include "AccountBillListWnd.h"
#include "IntegrationWnd.h"



#define WM_USER_MOUSEHOVER			WM_USER + 100
#define WM_USER_MOUSELEAVE			WM_USER + 101
#define WM_FUND_REFRESH				WM_USER + 102
#define WM_POSITION_REFRESH			WM_USER + 103
#define WM_MATCH_REFRESH			WM_USER + 104
#define WM_ORDER_REFRESH			WM_USER + 105
//#define WM_INSTANTPOSITION_REFRESH			WM_USER + 106
#define WM_SPECPRICE_REFRESH		WM_USER + 107
#define WM_AGREEEMENT_REFRESH		WM_USER + 108

