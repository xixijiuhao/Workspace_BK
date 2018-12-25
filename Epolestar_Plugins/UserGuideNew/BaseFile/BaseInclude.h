#ifndef _BASE_INCLUDE_H_
#define _BASE_INCLUDE_H_

/* windows header and lib */
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <GdiPlus.h>  
#pragma comment(lib, "gdiplus.lib")
/* c++ library */
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>

#include "resource.h"
/* epolestar header*/
#include "PluginMgr_API.h"
#include "MainFrame_API.h"
#include "IPointOrderAPI.h"
#include "TKeyOrderApi.h"

/* load rc */
#include "def_lang.h"
#include "load_rc.h"
#include "CharCodes.h"

/* TradeSpi */

#include "TDataContainer.h"

#include "IxFrm.h"
#include "TMemDC.h"
#include "TModalWnd.h"
#include "UIDesign.h"


#include "TMessageBox.h"

#include "CtlDefine.h"

#include "GlobalData.h"
#include "Export.h"

#include "TUserGudie.h"

#endif