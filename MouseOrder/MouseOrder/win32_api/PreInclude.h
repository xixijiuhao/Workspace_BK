#ifndef _MOUSE_PREINCLUDE_
#define _MOUSE_PREINCLUDE_

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>
#include <windowsx.h>
#include <map>
#include <set>
#include <list>
#include <string>
#include <vector>
#include <imm.h>
#include <regex> 
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")

#include "resource.h"
#include "Language.h"
#include "commctrl.h"
#include "IxFrm.h"
#include "load_rc.h"
#include "UIDesign.h"

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
#include "trade_spi_ex.h"
#include "Str2Time.h"

#include "mouse_type.h"
#include "MouseOrderString.h"
#include "share\share_struct.h"
#include "share\AnsicUnicode.h"
#include "share\TradeQuoteString.h"
#include "share\IOperator.h"
#include "share\popwincode.h"
//#include "share\popwincodequote.h"
#include "share\popwintip.h"
#include "share\popwindate.h"
#include "share\popwinlist.h"	// �����б�
#include "share\popwinsearch.h"	// �����б�
#include "share\MouseHead.h"
#include "popwinqty.h"			//ί������
#include "popwinprice.h"		//ί�м۸�
#include "popwincodelist.h"		//��Լƥ���б�
#include "ctl\CxSubClassWnd.h"	//�Զ��尴ť��
#include "ctl\CxDrawButton.h"	//�Զ��尴ť��
#include "ctl\CxSubButton.h"	//�Զ��尴ť��

#include "ctl\CxCombox.h"
#include "ctl\CxSpin.h"
#include "ctl\CxEdit.h"
#include "ctl\CxEditEx.h"
#include "ctl\CxCheck.h"
#include "ctl\CxStatic.h"
#include "ctl\CxStaticEx.h"
#include "ctl\CxComboxEx.h"
#include "ctl\CxComboxTime.h"
#include "BaseFrm.h"
#include "BaseFrame.h"
#include "MousePriceEdit.h"
#include "MouseComboEdit.h"
#include "MouseComboBox.h"
#include "MouseStatic.h"
#include "OrderButton.h"
#include "OrderList.h"
#include "MousePosStatic.h"

#include "share\MouseOrderPop.h"
#include "MouseOrderConfigWnd.h"

#include "LockDef.h"
#include "LockBtn.h"
#include "OptionStrategy.h"

#include "mousewin.h"

#endif // !_MOUSE_PREINCLUDE_
