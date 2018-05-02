#include "PreInclude.h"

using namespace std;
extern G_COLOREF			g_ColorRefData;
extern G_UIFONT				g_FontData;

extern ICspAccessApi		*g_pAccessApi;
extern IStarApi             *g_pStarApi;
extern ITradeApi			*g_pTradeApi;
extern ITradeData			*g_pTradeData;
extern ICommonModule		*g_pCommonModule;
extern IStrategyTradeAPI	*g_pStrategyApi;

extern IMainFrame			*g_mainframe;
extern IConfigFrame			*g_conFrame;

extern CMouseOrderPopWin	g_MousePopwin;
extern vector<CMousewin*>	g_MouseVec;

extern MouseConfig          *g_mouseCfg;
extern bool					g_MouseTrack;

//行距
#define CtrlInterval		5	//行距 
#define BottomTipHeight		50	//底部提示区域高度 

//
long Trunc(double Value)
{
	if (Value > 0)
		return (long)floor(Value);
	else
		return (long)ceil(Value);
};
double SimpleRoundTo(const double Value, const int Precision = 4)
{
	double LFactor = pow(10.0, -Precision);
	if (Value < 0)
		return Trunc((Value / LFactor) - 0.5) * LFactor;
	else
		return Trunc((Value / LFactor) + 0.5) * LFactor;
}
int HoursDiff(time_t t1, time_t t2)
{
	double Result = difftime(t1, t2);
	return (int)(Result);
}
//控件子类化 窗口过程
LRESULT CALLBACK TabsProc(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);

	CMousewin *p_mouse = (CMousewin *)GetWindowLong(hwnd, GWL_USERDATA);
	if (!p_mouse)
		return FALSE;
	switch (message)
	{
		case WM_CHAR://字符过滤
		{
			switch (id)
			{
			case CI_OrderPrice_Edit:
			case CI_SellPrice_Edit:
			case CI_TriggerPrice_Edit:
			case CI_StopLoss_Edit:
			case CI_StopProfit_Edit:
				if (wParam == '\b')
					break;
				if ((wParam >= '0'&&wParam <= '9') || (p_mouse && ((wParam == '-'&&p_mouse->m_CommodityInfo.bCanNegative) || (wParam == '.'&&p_mouse->m_CommodityInfo.iPrecision > 0 && !p_mouse->m_CommodityInfo.bIfFractionPrice))))
				{
					int nStart = -1, nEnd = -1;
					bool bIsValidNum = false;
					string stemp = GetWindowTextAnsic(hwnd);
					DWORD dValue = SendMessage(hwnd, EM_GETSEL, 0, 0);
					nStart = LOWORD(dValue);
					nEnd = HIWORD(dValue);
					if (nStart != -1 && nStart < nEnd)
					{
						TMouPriceStrA str = { 0 };
						string sleft = stemp.substr(0, nStart);
						string sright = stemp.substr(nEnd, stemp.size() - nEnd);
						sprintf_s(str, "%s%c%s", sleft.c_str(), wParam, sright.c_str());
						stemp = str;
					}
					else if (nStart == nEnd)
						stemp.insert(nEnd, 1, wParam);

					if (p_mouse->m_CommodityInfo.bIfFractionPrice)//说明是分数
						bIsValidNum = true;
					else
					{
						if (p_mouse->m_CommodityInfo.bCanNegative)//说明是价差
							bIsValidNum = CConvert::IsValidNumber(stemp.c_str(), false, true);
						else//说明是小数
						{
							if (p_mouse->m_CommodityInfo.iPrecision == 0)//整数
								bIsValidNum = CConvert::IsValidNumber(stemp.c_str(), true, false);
							else//可以有小数点
								bIsValidNum = CConvert::IsValidNumber(stemp.c_str(), false, false);
						}
					}

					if (bIsValidNum)
						break;
					else
						return FALSE;
				}
				else
					return FALSE;
			case CI_ContractNo_Edit:
				if ((wParam >= 'a'&&wParam <= 'z'))
					wParam = wParam - 32;
				break;
			case CI_EnquriyNo_Edit:
				if ((wParam == '\b')||(wParam >= '0'&&wParam <= '9') || (wParam >= 'A'&&wParam <= 'Z') || (wParam >= 'a'&&wParam <= 'z'))
					break;
				else
					return FALSE;
			case CI_MaxQty_Edit:
			case CI_MinQty_Edit:
			case CI_OrderQty_Edit:
			case CI_OrderPrice_Edit_Mole:
			case CI_SellPrice_Edit_Mole:
			case CI_TriggerPrice_Edit_Mole:
			case CI_StopLoss_Edit_Mole:
			case CI_StopProfit_Edit_Mole:
				if ((wParam >= '0'&&wParam <= '9') || wParam == '\b')
					break;
				else
					return FALSE;
			case CI_OverPoint_Edit:
				if ((wParam >= '0'&&wParam <= '9') || wParam == '\b' || wParam == '-')
				{
					TMouIntStrA sText = { 0 };
					GetWindowTextA(hwnd, sText, sizeof(TMouIntStrA));
					string str = sText;
					if (string::npos != str.find('-') && wParam == '-')
						return 1;
					break;
				}
				else
					return FALSE;
			default:
				break;
			}	
			break;
		}	
		case WM_KEYDOWN://tab键 Enter键 上下键处理
		{
			switch (wParam)
			{
			case VK_TAB:
				if (!p_mouse->DealTabKey(hwnd, id))	//对tab键进行处理
					return FALSE;
				break;
			case VK_RETURN:
				if (!p_mouse->DealReturnKey(hwnd, id))  //对回车键进行处理
					return FALSE;
				break;
			case VK_UP:
				{
					switch (id)
					{
					case CI_ContractNo_Edit:
						SendMessage(GetParent(GetParent(hwnd)), WM_CODE_WHEEL_CHANGED, 1, id);
						return TRUE;
					case CI_OrderPrice_Edit:
					case CI_SellPrice_Edit:
					case CI_TriggerPrice_Edit:
						SendMessage(GetParent(hwnd), WM_PRICE_UPDOWN, 1, id);
						return TRUE;
					case CI_OrderQty_Edit:
					case CI_MaxQty_Edit:
					case CI_MinQty_Edit:
						SendMessage(GetParent(hwnd), WM_QTY_UPDOWN, 1, id);
						return TRUE;
					case CI_OverPoint_Edit:
						SendMessage(GetParent(GetParent(hwnd)), WM_QTY_UPDOWN, 1, id);
						return TRUE;
					default:
						break;
					}
				}	
				break;
			case VK_DOWN:
				{
					switch (id)
					{
					case CI_ContractNo_Edit:
						SendMessage(GetParent(GetParent(hwnd)), WM_CODE_WHEEL_CHANGED, -1, id);
						return TRUE;
					case CI_OrderPrice_Edit:
					case CI_SellPrice_Edit:
					case CI_TriggerPrice_Edit:
						SendMessage(GetParent(hwnd), WM_PRICE_UPDOWN, -1, id);
						return TRUE;
					case CI_OrderQty_Edit:
					case CI_MaxQty_Edit:
					case CI_MinQty_Edit:
						SendMessage(GetParent(hwnd), WM_QTY_UPDOWN, -1, id);
						return TRUE;
					case CI_OverPoint_Edit:
						SendMessage(GetParent(GetParent(hwnd)), WM_QTY_UPDOWN, 1, id);
						return TRUE;
					default:
						break;
					}
				}
				break;
			case VK_LEFT:
				break;
			case VK_RIGHT:
				break;					
			default:
				break;
			}
			break;
		}
		case WM_LBUTTONDBLCLK:
			switch (id)
			{
			case CI_ContractNo_Edit:
				p_mouse->OnPopCodeSelectWnd();
				return TRUE;
			case CI_OrderPrice_Edit:
			case CI_SellPrice_Edit:
			case CI_TriggerPrice_Edit:
				p_mouse->OnPopPriceSelectWnd(id);
				break;
			default:
				break;
			}		
		default:
			break;
	}
	map<int, WNDPROC>::iterator it = p_mouse->m_gTabs.find(id);
	if (it == p_mouse->m_gTabs.end())
		return FALSE;
	else
		return CallWindowProc(it->second, hwnd, message, wParam, lParam);//多个对象
}

//////////////////////////////////////////////////////////////////////////竖向下单类
CMousewin::CMousewin(WinProperty wProperty)
	: m_nQtySelectID(CI_OrderQty_Edit),
	m_nPriceSelectID(CI_OrderPrice_Edit),
	m_bAutoCover(false),
	m_bModifyOrderOper(false),
	m_bBatchModifyOrderOper(false),
	m_sCode(""),
    m_sSpreadCode(""),
	m_cUserType(uiUser),
	m_sQuoteContractId(""),
	m_sOptionTarget(""),
	m_sQuoteKline(""),
	m_sQuoteOption(""),
	m_bShowFuzzyQuery(true),
	m_LockUserNoBtn(IDB_BITMAP_LOCK),
	m_LockCodeBtn(IDB_BITMAP_LOCK),
	m_SyncUserNoBtn(IDB_BITMAP_SYNC),
	m_SearchBtn(IDB_BITMAP_SEARCH),
	m_EditOrderPrice(m_FillOrderAssist.uiPricePos),
	m_EditSellPrice(m_FillOrderAssist.uiPricePos),
	m_EditTrigPrice(m_FillOrderAssist.uiPricePos),
	m_EditLoss(m_FillOrderAssist.uiPricePos),
	m_EditProfit(m_FillOrderAssist.uiPricePos),
	m_bCreateComplete(false),
	m_bCanShowPop(true),
	m_bStopPreHangOrder(false),
	m_bTradeLogined(false),
	m_bCanStrategyTrade(true),
	m_bCanBatchOrder(true),
	m_bBatchInfoModify(true),
	m_bShowStopSubmit(false),
	m_OrderReturn({0}),
	m_fastOrderInfo({0}),
	m_bTracking(false)
{
	//初始化 未登录交易 开平确定 内外盘布局
	memcpy(&m_wProperty, &wProperty, sizeof(WinProperty));
	m_CommodityInfo.cCoverMode = m_wProperty.cCoverMode;
	m_LoginType = (m_CommodityInfo.cCoverMode == oCover || m_CommodityInfo.cCoverMode == oCoverT) ? TYPE_Inner : TYPE_For;
	memset(&m_OrderModifyParam, 0, sizeof(m_OrderModifyParam));
	memset(&m_FillOrderInfo, 0, sizeof(m_FillOrderInfo));
	memset(m_SearchString, 0, sizeof(m_SearchString));
	memset(m_nSearchSize, 0, sizeof(m_nSearchSize));
	m_OrderModifyParam.OrderID = -1;
	m_sValidtype = LoadResWchar_t(IDS_EFFECTIVE_THEDAY);
	
    if (g_pCommonModule)
    {
        g_pCommonModule->Regist((ICommonInteract*)this);        //通用配置注册
        g_pCommonModule->Regist((ICommonInteractFast*)this, InteractType_FastOrder);    //快速下单注册
    }
    if (g_mouseCfg)
    {
        g_mouseCfg->Subscribe(this);
    }
	
	//配置 按钮颜色 备注 冰山影子 有效类型
	m_strPath = GetModulePathEx();

    SConfigSwitchInfo *sInfo = { 0 };
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_COLORUPDOWN, &sInfo, 1, false) != 0)
    {
        m_MouseCfg.bUseCfgColor = sInfo->ConfigSwitchValue == 1;
    }
	
	if (m_MouseCfg.bUseCfgColor)
	{
		m_MouseCfg.BuyRgb = g_ColorRefData.GetColorSellBlue();
		m_MouseCfg.SellRgb = g_ColorRefData.GetColorBuyRed();
	}
	else
	{
		m_MouseCfg.BuyRgb = g_ColorRefData.GetColorBuyRed();
		m_MouseCfg.SellRgb = g_ColorRefData.GetColorSellBlue();
	}

    /// 备注是否显示
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_REMARK, &sInfo, 1, false) > 0)
    {
        m_MouseCfg.bShowNoteInfo = sInfo->ConfigSwitchValue == 0;
    }
    else
    {
        m_MouseCfg.bShowNoteInfo = false;
    }
    /// 点价是否显示
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_LADDERORDER, &sInfo, 1, false) > 0)
    {
        m_MouseCfg.bShowLadder = sInfo->ConfigSwitchValue == 0;
    }
    else
    {
        m_MouseCfg.bShowLadder = true;
    }
    /// 批量
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_BATCHORDER, &sInfo, 1, false) > 0)
    {
        m_MouseCfg.bShowBatchOrder = sInfo->ConfigSwitchValue == 0;
    }
    else
    {
        m_MouseCfg.bShowBatchOrder = true;
    }
    /// 影子单是否显示
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_GHOST, &sInfo, 1, false) > 0)
    {
        m_MouseCfg.cFlag[Type_Shadow] = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
    }
    else
    {
        m_MouseCfg.cFlag[Type_Shadow] = 1;
    }
    /// 冰山单
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_ICEBERG, &sInfo, 1, false) > 0)
    {
        m_MouseCfg.cFlag[Type_IceBerg] = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
    }
    else
    {
        m_MouseCfg.cFlag[Type_IceBerg] = 1;
    }
    /// 行权 询价 期权
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_EXE_ENQUIRY, &sInfo, 1, false) > 0)
    {
        m_MouseCfg.cFlag[Type_Option] = sInfo->ConfigSwitchValue == 0 ? 1 : 0;
    }
    else
    {
        m_MouseCfg.cFlag[Type_Option] = 1;
    }
    /// 应价
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_OFFER, &sInfo, 1, false) > 0)
    {
        m_MouseCfg.cFlag[Type_Reply] = sInfo->ConfigSwitchValue == 0 ? 1 : 0 ;
    }
    else
    {
        m_MouseCfg.cFlag[Type_Reply] = 1;
    }

    //有效类型
    if (g_pAccessApi->GetConfigSwitchInfo(S_CONFIGNO_VALIDTYPE, &sInfo, 1, false) > 0)
    {
        m_ValidTypeCfgValue = sInfo->ConfigSwitchValue;
    }
    else
    {
        m_ValidTypeCfgValue = 0x11111;
    }

	if (!m_bTradeLogined)
		m_bTradeLogined = CConvert::IsTraderLogined();

	if (g_pStrategyApi)
		m_bCanStrategyTrade = g_pStrategyApi->RegistStrategyInfo(this) && (g_pStrategyApi->GetStartegyWorkState()&SWS_SpreadTrade || g_pStrategyApi->GetStartegyWorkState()&SWS_StopTrade);
	else
		m_bCanStrategyTrade = false;
	m_bCanBatchOrder = m_MouseCfg.bShowBatchOrder&&!m_bTradeLogined;
}

CMousewin::~CMousewin()
{
    if (g_pCommonModule)
    {
        g_pCommonModule->UnRegist((ICommonInteract*)this);
        g_pCommonModule->UnRegist((ICommonInteractFast *)this, InteractType_FastOrder);
    }
	if (g_pStrategyApi)
		g_pStrategyApi->UnRegistStrategyInfo(this);
    if (g_pStarApi)
    {
        g_pStarApi->UnsubAllQuote(this);
    }
	if (g_mouseCfg)
	{
		g_mouseCfg->UnSubscribe(this);
	}
}

//创建窗口
void CMousewin::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname,parent, style,exstyle);
}

LRESULT CMousewin::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:					OnCreateWindow();
									OnStopInfoUpdate(m_bShowStopSubmit);
		break;
	case WM_PAINT:					OnPaint(wParam, lParam);
		break;
	case WM_SIZE:					OnSize(wParam, lParam);
		break;
	case WM_COMMAND:				OnCommond(wParam, lParam);
		break;
	case WM_NOTIFY:					OnNotify(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:				OnMouseWheel(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:			OnLButtonDown(wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
			int i = GetWindowLong((HWND)lParam, GWL_ID);
			if(i== CI_OrderBuyPrice_Static)
				SetTextColor((HDC)wParam, !m_MouseCfg.bUseCfgColor ? g_ColorRefData.GetColorBuyRed(): g_ColorRefData.GetColorTextGreen());
			else if(i == CI_OrderSellPrice_Static)
				SetTextColor((HDC)wParam, m_MouseCfg.bUseCfgColor ? g_ColorRefData.GetColorBuyRed() : g_ColorRefData.GetColorTextGreen());
			return (LONG)m_hStaticBKBrush;
		}
	case WM_CTLCOLOREDIT:
	{
		int i = GetWindowLong((HWND)lParam, GWL_ID);
		switch (i)
		{
		case CI_UserNo_Edit2:
		case CI_UserNo_Edit3:
		case CI_UserNo_Edit4:
		{
			wchar_t cText[51] = { 0 };
			GetWindowText(m_hControl[i], cText, 51);
			if (wcscmp(cText, LoadResWchar_t(IDS_AccNo2 + i- CI_UserNo_Edit2))==0)
				SetTextColor((HDC)wParam, g_ColorRefData.GetColorRGB200());
			else
				SetTextColor((HDC)wParam, g_ColorRefData.GetColorBlack());
		}
		default:
			break;
		}
	
		return (LONG)g_ColorRefData.GetHbrWhite();
	}
	//////////////////////////////////////////////////////////////////////////
	case WM_ONQUOTE:				OnDealQuote();
		break;
	case WM_ONQUOTETARGET:			OnDealQuoteOptionTarget();
		break;
	case WM_ONPOSITION:				OnDealPosition();
		break;
	case WM_ONRESETUSERNO:			ReSetUserNos();
									UpdateSearchLayOut();//更新下列头
		break;
	case WM_BATCHORDERCHANGE:		OnDealBatchOrderChange();
		break;
	case WM_DESTROY:				OnDestroy();
		break;
	case WM_CODE_WHEEL_CHANGED:		OnWheelCodeList(lParam,wParam);	//编辑框上下键 消息响应		子类化
		break;
	case WM_ONCODELISTRETURN:		OnCodeReturn(wParam);			//合约列表窗口响应 回车按钮	子类化
		break;
	case WM_PRICE_UPDOWN:			OnSpinDeltaUpDownEx(wParam, lParam, lParam + 3);
		break;
	case WM_QTY_UPDOWN:	
		if (CI_OverPoint_Edit == lParam)
			OnSpinDeltaUpDownNormal(wParam, lParam, MPoint_Max, MPoint_Min);
		else
			OnSpinDeltaUpDownNormal(wParam, lParam);
		break;
	case WM_DEALQUOTECOMMON:		OnDealQuoteCommon();
		break;
	case WM_DEALPOSCOMMON:			OnDealPositionCommon();
		break;
	case WM_DEALORDERCOMMON:		OnDealOrderCommon();
		break;
	case WM_DEALSPREADCOMMON:		OnDealSpreadCommon();
		break;
	case WM_DEALMODIFYRECOVER:		UpdateModifyRecover();
		break;
	case WM_DEALTRADABLEQTY:		OnDealTradableQty(wParam);
		break;
	case WM_DEALPREFREEZE:			OnDealPreFreeze(wParam);
		break;
	case WM_DEALKLINECONTRACT:		OnDealKLineContract();
		break;
	case WM_DEALOPTIONSTATEGY:		OnDealOptionStartegy();
		break;
	case WM_ONEXEOPTION:			OnDealOptionExe();
		break;
	case WM_NCDESTROY:				OnNcDestory();//加锁
		break;
	case WM_DRAWITEM:				OnDrawItem(wParam,lParam);
		break;
	case WM_MEASUREITEM:			OnMeasureItem(wParam, lParam);
		break;
	case WM_MOUSEMOVE:				OnMouseMove();
		break;
	case WM_MOUSELEAVE:				OnMouseLeave();
		break;
	case WM_MOUSEHOVER:				OnMouseHover();
		break;
	case WM_KILLFOCUS:				m_bTracking = FALSE;
		break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}

//控件Tab键处理
BOOL CMousewin::DealTabKey(HWND hwnd, int nID)
{
	map<int, WNDPROC>::iterator it = m_gTabs.find(nID);
	if (it == m_gTabs.end())
		return FALSE;
	else
	{
		for (uint i = 0; i < m_gTabs.size(); i++)
		{
			it++;
			bool bfind = false;
			if (it == m_gTabs.end())
			{
				if (TablesEditToButton())
					return FALSE;
				it = m_gTabs.begin();
			}
			switch (it->first)
			{
			case CI_UserNo_Edit:
				if (IsWindowVisible(m_hControl[CI_UserNo_Combox]) && IsWindowEnabled(m_hControl[CI_UserNo_Combox]))
				{
					SetFocus(m_hControl[CI_UserNo_Combox]);
					bfind = true;
				}
				else if (IsWindowVisible(m_hControl[CI_ContractNo_Edit_Win]))
				{
					SetFocus(m_hControl[CI_ContractNo_Edit]);
					bfind = true;
				}
				break;
			case CI_OverPoint_Edit:
				if (IsWindowVisible(m_hControl[CI_OverPoint_Edit_Win]) && IsWindowEnabled(m_hControl[CI_OverPoint_Edit_Win]))
				{
					SetFocus(m_hControl[CI_OverPoint_Edit]);
					bfind = true;
				}
				break;
			case CI_ContractNo_Edit:
				if (IsWindowVisible(m_hControl[CI_ContractNo_Edit_Win]) && IsWindowEnabled(m_hControl[CI_ContractNo_Edit_Win]))
				{
					SetFocus(m_hControl[CI_ContractNo_Edit]);
					bfind = true;
				}
				break;
			default:
				if (IsWindowVisible(m_hControl[it->first]) && IsWindowEnabled(m_hControl[it->first]))
				{
					SetFocus(m_hControl[it->first]);
					bfind = true;
				}
				break;
			}
			if (bfind)
				break;
		}
		return TRUE;
	}
}
//控件Enter键处理
BOOL CMousewin::DealReturnKey(HWND hwnd, int nID)
{
	switch (nID)
	{
	case CI_Modify_Button:
	case CI_Cancel_Button:
		SendMessage(m_Hwnd, WM_COMMAND, nID, NULL);
		SetFocus(m_hControl[CI_UserNo_Combox]);
		return TRUE;
	case CI_ContractNo_Edit:
		SendMessage(m_Hwnd, WM_ONCODELISTRETURN, nID, 0);

	default:
		return DealTabKey(hwnd, nID);
	}
}
////////////////////////////////////////////////////////////////////////// 系统通知消息处理函数
//定单修改处理
void CMousewin::ModifyOrder(const uint OrderID, bool bBatch)
{
	//锁定状态不响应修改操作
	if (m_LockUserNoBtn.GetLockState())
		return;

	const TOrder* pOrder = g_pTradeData->GetOrder(OrderID);
	if (!pOrder)
		return;

	m_bModifyOrderOper = true;
	m_bBatchModifyOrderOper = bBatch;
	//状态重置
	ResetFastCheck();
	ResetPriceLevel();

	m_BtnModify.SetText(LoadResWchar_t(IDS_STRING1102));
	m_BtnCancel.SetText(LoadResWchar_t(IDS_STRING1103));

	//高级
	if (pOrder->OrderType == otGhost || pOrder->OrderType == otIceberg || pOrder->OrderType == otLimitStop || pOrder->OrderType==otMarketStop
		|| pOrder->StrategyType != stNone)
	{
		switch (pOrder->StrategyType)
		{
		case stPreOrder:
			m_nCurClickItem.nCurChildType = CHA_PreOrder;
			break;
		case stAutoOrder:
			m_nCurClickItem.nCurChildType = CHA_AutoOrder;
			break;
		case stCondition:
			m_nCurClickItem.nCurChildType = CHA_Condition;
			break;
		default:
			{
				switch (pOrder->OrderType)
				{
				case otGhost:
					m_nCurClickItem.nCurChildType = CHA_Shadow;
					break;
				case otIceberg:
					m_nCurClickItem.nCurChildType = CHA_Iceberg;
					break;
				case otLimitStop:
					m_nCurClickItem.nCurChildType = CHA_LimtStop;
					break;
				case otMarketStop:
					m_nCurClickItem.nCurChildType = CHA_Stop;
					break;
				default:
					return;
				}
			}
		}	
		
		m_nCurClickItem.nCurHeadType = MHI_Advanced;
		UpdateCurHeadStr();
		LayOut();	
	}
	else
	{
		if (m_nCurClickItem.nCurHeadType != MHI_Normal|| m_nCurClickItem.nCurChildType != CHN_Vert)
		{
			//竖向恢复
			m_nCurClickItem.nCurChildType = CHN_Vert;

			//列头恢复
			m_nCurClickItem.nCurHeadType = MHI_Normal;
			UpdateCurHeadStr();
			LayOut();	
		}
	}
	
	FillBaseDataToDlg(pOrder);

	SetWindowCtrlEnableOrDisable(false);
}
void CMousewin::SetCodeFromQuote(const char * sKey, const char* cType)	//双击 单击 行情填单
{
	if (!strcmp(cType, "OptionOrder"))
	{
		SetCodeFromOption(sKey);
		return;
	}

	string strContent = sKey;
	vector<NoticeContent> vecNoticeContent;
	if (!CConvert::SplitNotice(strContent, ';','=', vecNoticeContent))
		return;
	if (vecNoticeContent.size() < ID_SRC+1)
		return;
	
	if (vecNoticeContent[ID_SRC].strContent == W_SRC_GRID)//盘口 报价
	{
		if (!strcmp(cType, W_LButtonDown)|| !strcmp(cType, W_LButtonDbClk))
		{
			memcpy_s(m_FillOrderInfo.sUserNo,sizeof(m_FillOrderInfo.sUserNo), m_sUserNo[0].c_str(), sizeof(m_FillOrderInfo.sUserNo));
            if (g_pCommonModule&&g_pCommonModule->SetCodeFromQuote(vecNoticeContent[ID_CODE].strContent.c_str(),
                vecNoticeContent[ID_FIELD].strContent.c_str(), 0, cType, m_sSign[0].c_str(), m_FillOrderInfo))
            {
                PostMessage(m_Hwnd, WM_DEALQUOTECOMMON, 0, 0);
            }	
		}
		else if (!strcmp(cType, W_VK_UP) || !strcmp(cType, W_VK_DOWN))
			SetCodeFromQuoteKline(vecNoticeContent[ID_CODE].strContent.c_str());
	}
	else if (vecNoticeContent[ID_SRC].strContent == W_SRC_PANEL)
	{
		if (!strcmp(cType, W_LButtonDown) || !strcmp(cType, W_LButtonDbClk))
		{
			memcpy_s(m_FillOrderInfo.sUserNo, sizeof(m_FillOrderInfo.sUserNo), m_sUserNo[0].c_str(), sizeof(m_FillOrderInfo.sUserNo));
            if (g_pCommonModule&&g_pCommonModule->SetCodeFromQuote(vecNoticeContent[ID_CODE].strContent.c_str(),
                vecNoticeContent[ID_FIELD].strContent.c_str(), atoi(vecNoticeContent[ID_LEVEL].strContent.c_str()),
                cType, m_sSign[0].c_str(), m_FillOrderInfo))
            {
                PostMessage(m_Hwnd, WM_DEALQUOTECOMMON, 0, 0);
            }
		}
	}
	else if (vecNoticeContent[ID_SRC].strContent == W_SRC_KLINE)
		SetCodeFromQuoteKline(vecNoticeContent[ID_CODE].strContent.c_str());
}
void CMousewin::SetCodeFromPosTotal(const char * sKey, const char* cType)						//点击持仓合计
{
	if (g_pCommonModule&&g_pCommonModule->SetCodeFromPosTotal(sKey, cType, m_FillOrderInfo))
		PostMessage(m_Hwnd, WM_DEALPOSCOMMON, 0, 0);
}
void CMousewin::SetCodeFromPosSub(const char * sKey, const char* cType)						//点击持仓明细
{
	if (g_pCommonModule&&g_pCommonModule->SetCodeFromPosSub(sKey, cType, m_FillOrderInfo))
		PostMessage(m_Hwnd, WM_DEALPOSCOMMON, 0, 0);
}
void CMousewin::SetCodeFromOrder(const char *sOrderID, const char* cType)					//双击 单击 委托填单	
{
	if (strcmp(cType, W_BN_DBCLICK) == 0)
	{
		m_sOrderInfo = sOrderID;
		PostMessage(m_Hwnd, WM_DEALORDERCOMMON, 0, 0);
	}
}
void CMousewin::SetCodeFromSpread(const char *sOrderID, const char* cType)				//双击 单击 套利填单	
{
	if (strcmp(cType, W_BN_DBCLICK) == 0)
	{
		m_sSpreadInfo = sOrderID;
		PostMessage(m_Hwnd, WM_DEALSPREADCOMMON, 0, 0);
	}	
}
void CMousewin::SetCodeFromOption(const char *sContent)//考虑同步异步
{
	m_sQuoteOption = sContent;
	PostMessage(m_Hwnd, WM_DEALOPTIONSTATEGY,0,0);
}
void CMousewin::SetExecFromPosTotal(const char * sKey)
{
	m_sExeOption = sKey;
	PostMessage(m_Hwnd, WM_ONEXEOPTION, 0, 0);
}
void CMousewin::SetCodeFromQuoteKline(const char *sContent)
{
	SContract *refOut = !IsSpreadLayOut() ? g_pStarApi->GetContractUnderlay(sContent): NULL;
	m_sQuoteKline = refOut ? refOut->ContractNo : sContent;
	PostMessage(GetHwnd(), WM_DEALKLINECONTRACT, 0, 0);
}
void CMousewin::SetModifyRecover()
{
	PostMessage(m_Hwnd, WM_DEALMODIFYRECOVER, 0, 0);
}
////////////////////////////////////////////////////////////////////////////交易回调
void CMousewin::OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin)
{
	if (IsWindowVisible(m_hControl[CI_UserNo_Combox]))
		SetFocus(m_hControl[CI_UserNo_Combox]);
	
    PostMessage(GetHwnd(), WM_ONRESETUSERNO, 0, 0);
	//更新 交易员信息	
}
//////////////////////////////////////////////////////////////////////////
void CMousewin::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
	if (ErrorCode == 0 && CurrProgress == tipReady)
	{
		bool bOldState = m_bTradeLogined;
		m_bTradeLogined = CConvert::IsTraderLogined();	
		if (bOldState != m_bTradeLogined)
			UpdateCheckState();//快速勾选更新

		if (m_bCanStrategyTrade)//不可逆
			m_bCanStrategyTrade = !m_bTradeLogined;

		if (m_bCanBatchOrder)
			m_bCanBatchOrder = !m_bTradeLogined;

		if (m_LoginType != TYPE_Both)
			m_LoginType = m_LoginType|CConvert::ReadVersionType();

		PostMessage(GetHwnd(), WM_ONRESETUSERNO, 0, 0);
	}		
}

void CMousewin::OnOrder(const TOrder * pOrder)
{
	if (!pOrder || !GetCreateFlag())
		return;
	
	m_MaxOrdID = pOrder->OrderID;

	//点价挂单更新
	
	if (pOrder->UserNo == m_sUserNo[0])
	{
		string sCode = ("");
		CConvert::TContractKeyToTContractNo(*pOrder, sCode);
		if (sCode == m_sCode)
		{
			if (IsLadderLayOut())
			{
				m_OrderList.UpdateTradeOrder(pOrder);
				m_OrderList.UpdateParOrderQty();
			}
			else if (IsOptionExAbnLayOut()&&pOrder->OrderType==otExecute)
				UpdatePositionQty();//委托变动：可行权数更新
		}	
		
	}
}
void CMousewin::OnMatch(const TMatch * pMatch)
{
	if (!pMatch || !GetCreateFlag())
		return;
	string sCode;
	CConvert::TContractKeyToTContractNo(*(TContractKey*)pMatch, sCode);

	if (sCode == m_sCode || (m_CommodityInfo.bCanNegative && IsOneOfComboCode(sCode, m_sCode)/*strstr(m_sCode.c_str(), sCode.c_str())*/))
		PostMessage(GetHwnd(), WM_ONPOSITION, 0, 0);
    else if (m_sCode.find('/') != std::string::npos)
    {
        TContractKey	TradeContract;	//交易合约
        memset(&TradeContract, 0, sizeof(TradeContract));
        CConvert::TContractNoToTContractKey(m_sCode.c_str(), TradeContract);

        if (TradeContract.CommodityType == ctSpreadCommodity || TradeContract.CommodityType == ctSpreadMonth)
        {
            TContractSingleKey key[2];
            memset(&key[0], 0, sizeof(TContractSingleKey));
            memset(&key[1], 0, sizeof(TContractSingleKey));
            CConvert::CombTContractKeyToTContractSingleKey(TradeContract, key[0], key[1]);

            string sCode1, sCode2;
            CConvert::PosContractToStr(sCode1, key[0]);
            CConvert::PosContractToStr(sCode2, key[1]);

            if (sCode == sCode1 || sCode == sCode2)
            {
                PostMessage(GetHwnd(), WM_ONPOSITION, 0, 0);
            }
        }
    }
    else if (IsSpreadLayOut())
    {
        char sQuoteCode[101] = { 0 };
        CConvert::TradeContractToQContractNoType(*(TContractKey*)pMatch, sQuoteCode);
        if (strstr(m_sSpreadCode.c_str(), sQuoteCode))
        {
            PostMessage(GetHwnd(), WM_ONPOSITION, 0, 0);
        }
    }
		
}
void CMousewin::OnPreFrozen(const TPreFreeze * pPreFreeze)
{
	if (pPreFreeze == NULL || pPreFreeze->pCurrency == NULL)
		return;
	CMyLock::Guard lock(m_lock);
	if (pPreFreeze->Direct==dBuy)
	{
		memcpy_s(&m_BuyPreFreezetemp, sizeof(TPreFreeze), pPreFreeze, sizeof(TPreFreeze));
		PostMessage(m_Hwnd, WM_DEALPREFREEZE, dBuy, 0);
	}
	else if (pPreFreeze->Direct == dSell)
	{
		memcpy_s(&m_SellPreFreezetemp, sizeof(TPreFreeze), pPreFreeze, sizeof(TPreFreeze));
		PostMessage(m_Hwnd, WM_DEALPREFREEZE, dSell, 0);
	}
}

void CMousewin::OnTradableQty(const TTradableQty * pTradableQty)
{
	if (!pTradableQty)
		return;
	CMyLock::Guard lock(m_lock);
	if (pTradableQty->Direct==dBuy)
	{
		memcpy_s(&m_BuyTradableQtytemp, sizeof(TTradableQty), pTradableQty, sizeof(TTradableQty));
		//账号
		if (strcmp(m_sUserNo[0].c_str(), m_BuyTradableQtytemp.UserNo))
			return;
		PostMessage(m_Hwnd, WM_DEALTRADABLEQTY, 0, 0);
	}
	else if (pTradableQty->Direct == dSell)
	{
		memcpy_s(&m_SellTradableQtytemp, sizeof(TTradableQty), pTradableQty, sizeof(TTradableQty));
		//账号
		if (strcmp(m_sUserNo[0].c_str(), m_SellTradableQtytemp.UserNo))
			return;
		PostMessage(m_Hwnd, WM_DEALTRADABLEQTY, 1, 0);
	}
}
////////////////////////////////////////////////////////////////////////////行情回调
void CMousewin::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
	if (m_sQuoteContractId == cont->ContractNo&&IsWindow(GetHwnd()))
		PostMessage(GetHwnd(), WM_ONQUOTE, 0, 0);
	if (m_sOptionTarget == cont->ContractNo&&IsWindow(GetHwnd()))
		PostMessage(GetHwnd(), WM_ONQUOTETARGET, 0, 0);
}
//套利行情回调
void CMousewin::OnSpreadQuote(const SSpreadContract* scont)
{
	if (m_sQuoteContractId == scont->ContractNo && IsWindow(GetHwnd()))
		PostMessage(GetHwnd(), WM_ONQUOTE, 0, 0);
}
//设置有效类型
void CMousewin::SetValidType(const wchar_t*  sVData)
{
	if (wcscmp(sVData, L""))
	{
		m_sValidtype = sVData;
		m_BtnValidType.SetText(m_sValidtype.c_str());
	}
}
//设置最小 最大 委托量
void CMousewin::SetMaxMinQty(const char* sQty)
{
	if (strcmp(sQty, "set")==0)
	{
		if (g_conFrame)
			g_conFrame->show(LoadResWchar_t(IDS_FastOrderCfg),cmtTrade);
	}
	else if (strcmp(sQty,""))
	{
		SetWindowTextAnsic(m_hControl[m_nQtySelectID], sQty);
		if (GetFocus() != m_hControl[m_nQtySelectID])
		{
			m_bCanShowPop = false;
			SetFocus(m_hControl[m_nQtySelectID]);
			m_bCanShowPop = true;
		}
	}
}
//界面信息填写订单 发单
void CMousewin::FillAndSendOrder(TSendOrder& sendorder, bool bGetPrice, bool bGetCode)
{
	if (FillOrder(sendorder,bGetPrice, bGetCode))//填单 单腿	
	{
		//自动开平
		bool bToday = IsWindowVisible(m_CheckCloseDay.GetHwnd()) && m_CheckCloseDay.GetCheck() == BST_CHECKED ? true : false;//平今勾选
		if ((m_bAutoCover || (!bGetPrice&&m_CheckOpenContinue.GetCheck()!=BST_CHECKED))
			&&m_CommodityInfo.cCoverMode != cmUnfinish&&m_CommodityInfo.cCoverMode != cmNone
			&&m_nCurClickItem.nCurHeadType != MHI_Strategys
			&&!IsOptionLayOut() 
			&&!IsOptionStrategyLayOut()
			&&!IsSpreadApplyLayOut() 
			&&!IsHedgeApplyLayOut()
			&&!IsBatchLayOut())//自动开平 内盘 非客户端止损
		{
			if (CConvert::OrderOffsetIsOpen(sendorder.Direct, sendorder.UserNo, &sendorder, sendorder.CommodityType))
				sendorder.Offset = oOpen;
			else
				sendorder.Offset = bToday ? oCoverT : oCover;
		}

		//下单反馈
		ErrorReflectInfo eError;
		memset(&eError, 0, sizeof(ErrorReflectInfo));

		CM_PARAM param = { 0 };
		if (m_nCurClickItem.nCurHeadType == MHI_BatchOrder)
		{
			param.cmParam |= CM_BATCHQTY;
			param.ctParam |= ((IsWindowVisible(m_CheckAssistOrder.GetHwnd()) && m_CheckAssistOrder.GetCheck()) ? CT_RECOVE : CT_NONE);
		}
        if (m_nCurClickItem.nCurHeadType == MHI_Normal && m_nCurClickItem.nCurChildType == CHN_Vert)
        {
            param.ctParam |= m_CheckCoverOpen.GetCheck() == BST_CHECKED ? CT_COVEROPEN : CT_NONE;
        }
		TVecInt vOrderIds;
		if (!g_pCommonModule->InsertOrder(sendorder, eError, vOrderIds,&param))//通用下单
		{
			if (eError.nErrorLevel == 1)
				MessageBox(m_Hwnd, eError.error_text, LoadResWchar_t(IDS_Reject), NULL);
		}
		else
		{
			//客户端止损
			if (m_nCurClickItem.nCurHeadType == MHI_Strategys && m_nCurClickItem.nCurChildType == CHS_PCStop)
				SetStopPreHangOrder(sendorder, vOrderIds);
			//港交所T+1
			if (m_OrderReturn.bCancelAddOne&&IsWindowVisible(m_hControl[CI_AddOne_Check]))
				m_CheckAddOne.SetCheck(BST_UNCHECKED);
			//下单后取消强平
			if (m_OrderReturn.bCancelForceCover && IsWindowVisible(m_hControl[CI_ForceClose_Check]))
				m_CheckForceClose.SetCheck(BST_UNCHECKED);

			//下单后默认当日有效
			if (m_OrderReturn.bValidTypeGFD && IsWindowVisible(m_BtnValidType.GetHwnd()))
			{
				m_sValidtype = CMouseString::ValidTypeToWStr(vtGFD);
				m_BtnValidType.SetText(m_sValidtype.c_str());
			}

			//下单后默认限价
			if (m_OrderReturn.bOrderTypeLimit && IsWindowVisible(m_StaticPriceSwitch.GetHwnd()))
			{
				memcpy_s(m_sPricetype, sizeof(m_sPricetype), LoadResWchar_t(IDS_LIMIT_PRICE), sizeof(m_sPricetype));
				m_StaticPriceSwitch.CxSetWindowText(m_sPricetype);
				LayOut();
			}

			if (IsWindowVisible(m_hControl[CI_NoteInfo_Edit]))
				SetWindowText(m_hControl[CI_NoteInfo_Edit], L"");
		}
	}
	//下单后操作
	if (IsWindowVisible(m_OrderList.GetHwnd())||IsWindowVisible(m_OptionView.GetHwnd()))
		return;
	else if (!IsWindowEnabled(m_hControl[CI_UserNo_Combox]))//焦点
		SetFocus(m_hControl[CI_ContractNo_Edit]);
	else
		SetFocus(m_hControl[CI_UserNo_Combox]);
}
bool CMousewin::SendOrder(MsgOrderInfo *msg)
{
	SetShowTips(L"");
	if (TradeLoginLinkage())
		return false;
	if (IsSpreadLayOut())//套利单
	{
		TSpreadSendOrder sorder;
		memset(&sorder,0,sizeof(TSpreadSendOrder));
		sorder.TrigPriceMode = tpm_Match;
		if (msg)
		{
			sorder.Direct[0] = msg->Direct;
			sorder.Offset[0] = msg->Offset;
			sorder.Offset[1] = msg->Offset;//内外盘分
		}
		else
			return false;
		if (sorder.Direct[0] == dBuy)
			sorder.TrigCondition = tcLessEqual;
		else
			sorder.TrigCondition = tcGreaterEqual;

		if (FillSpreadOrder(sorder))
		{
			int nret = 0;
            if (g_pStrategyApi)
            {
                if (nret = g_pStrategyApi->AddSpreadOrder(sorder))
                {
                    TMouErrorTipStrW str = { 0 };
                    swprintf_s(str, LoadResWchar_t(IDS_SpreadSucceed), nret);
                    SetShowTips(str);
                }
                else
                    SetShowTips(LoadResWchar_t(IDS_SpreadFailed));
            }
		}
	}
	else
	{
		TSendOrder sendorder;
		CConvert::DefaultSendOrder(sendorder);
		if (msg)
		{
			sendorder.Direct = msg->Direct;
			sendorder.Offset = msg->Offset;
			sendorder.SellOffset = msg->SellOffset;
		}

		FillAndSendOrder(sendorder);
	}
	return true;
}
bool CMousewin::CheckSameCode(const char* sCode, bool bSpread)				//检测设置合约是否相同				
{
    if (bSpread && m_sSpreadCode == sCode)
    {
        //刷新 持仓显示
        UpdatePositionQty();//合约相同：刷新持仓信息
                            
        //套利 更新平仓方式
        char CoverMode = m_CommodityInfo.cCoverMode;
        CodeUpdateCommodityInfo();
        if (CoverMode != m_CommodityInfo.cCoverMode)
            LayOut();

        return true;
    }
    else if (m_sCode == sCode && !m_bModifyOrderOper)
    {
        UpdateCodeEdit();
        //刷新 持仓显示
        UpdatePositionQty();//合约相同：刷新持仓信息

        return true;
    }
    else
    {
        return false;
    }
	//if (m_sCode == sCode && !m_bModifyOrderOper)//合约相同 不是改单
	//{
	//	//更新合约显示
	//	UpdateCodeEdit();
	//	//刷新 持仓显示
	//	UpdatePositionQty();//合约相同：刷新持仓信息
	//	//套利 更新平仓方式
	//	if (bSpread)
	//	{
	//		char CoverMode = m_CommodityInfo.cCoverMode;
	//		CodeUpdateCommodityInfo();
	//		if (CoverMode != m_CommodityInfo.cCoverMode)
	//			LayOut();
	//	}

	//	return true;
	//}
	//else
	//	return false;
}

void CMousewin::SetContractNo(const char* sCode, CodeSetType csType)
{
	if (IsSpreadLayOut())
	{
		////合约选择 套利合约转设置界面
		//if (IsSpreadCodeAdd(sCode))
		//	return;		
		
        SetWindowText(m_hControl[CI_OrderRatio_Static], L"");

		//交易合约字符串转 单腿行情合约
        SSpreadContract sQuote;
        if (g_pStarApi && g_pStarApi->GetSpreadInfo(sCode, &sQuote, 1, false) > 0)
		{
            //更新数量比例
            wstring strRatio = to_wstring(sQuote.SpreadQty[0]);
            for (int i = 1; i < sQuote.ValidLegs; i++)
            {
                strRatio += L":" + to_wstring(sQuote.SpreadQty[i]);
            }
            wchar_t strText[51] = { 0 };
            swprintf_s(strText, LoadResWchar_t(IDS_SpreadRatio), strRatio.c_str());
            SetWindowText(m_hControl[CI_OrderRatio_Static], strText);

			//相同合约检测
			if (CheckSameCode(sCode, true))
				return;

			m_sSpreadCode = sCode;
            if (csType&cstChange)//更新显示重绘 失去焦点时 不需要再刷新
            {
                UpdateSpreadCodeEdit(sQuote);
            }

			//更新品种信息
			CodeUpdateCommodityInfo();
			//行情订阅 格式规范化
			UpdateQuoteContract(sQuote.ContractNo, csType);

			//m_OrderList.UpdateCode("", m_CommodityInfo);

			PostMessage(GetHwnd(), WM_ONQUOTE, 0, 0);

			//重新布局
			LayOut();
		}
	}
	else
	{
		if (strcmp(sCode, ""))//非空合约
		{
			//相同合约检测
			if (CheckSameCode(sCode))
				return;

			m_sCode = sCode;
			if (csType&cstChange)//更新显示重绘 失去焦点时 不需要再刷新
				UpdateCodeEdit();

			//更新品种信息
			CodeUpdateCommodityInfo();

			//行情订阅 格式规范化
			if (g_pStarApi)
			{
				SContractNoType sQuote= { 0 };
				CConvert::StrToQContractNoType(sCode, sQuote);

				UpdateQuoteContract(sQuote,csType);
				
				UpdateOptionTarget();
				
				m_OrderList.UpdateCode(m_sQuoteContractId.c_str(), m_CommodityInfo);
			}
			//重新布局
			LayOut();

			//刷新 可开可平 预冻结保证金
			FormatDisplayQtyAndFreeze(csType&cstQtyFreeze ? true : false, csType&cstQtyFreeze ? true : false);

			if (IsWindowVisible(m_hControl[CI_OrderPrice_Edit]))
			{
				if (!wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1089)))//相等
				{
					if (!(csType&cstQuote))
						SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, 0);
				}
				else
					PostMessage(m_Hwnd, WM_ONQUOTE, 0, 0);
			}
			PostMessage(m_Hwnd, WM_ONQUOTETARGET, 0, 0);
		}
		else//清空合约
		{
			m_sCode = sCode;
			UpdateCodeEdit();
			if (g_pStarApi)//行情订阅 格式规范化
			{
				if (!m_sQuoteContractId.empty())
				{
					SContractNoType qCode = { 0 };
					memcpy(qCode, m_sQuoteContractId.c_str(), sizeof(SContractNoType));
                    g_pStarApi->UnsubQuote(&qCode, 1, this);
					m_sQuoteContractId = "";
				}
				if (!m_sOptionTarget.empty())
				{
					SContractNoType qCode = { 0 };
					memcpy(qCode, m_sOptionTarget.c_str(), sizeof(SContractNoType));
                    g_pStarApi->UnsubQuote(&qCode, 1, this);
					m_sOptionTarget = "";
				}
				m_OrderList.UpdateCode(m_sQuoteContractId.c_str(), m_CommodityInfo);
			}

			FormatDisplayQtyAndFreeze(csType&cstQtyFreeze ? true : false, csType&cstQtyFreeze ? true : false);
		}
	}
	
	//清空行情数据显示
	if (IsWindowVisible(m_hControl[CI_OrderSellPrice_Static]))
		SetWindowText(m_hControl[CI_OrderSellPrice_Static], L"");
	if (IsWindowVisible(m_hControl[CI_OrderBuyPrice_Static]))
		SetWindowText(m_hControl[CI_OrderBuyPrice_Static], L"");
	
}
void CMousewin::SetPriceData(const char* sdata)
{
	if (strcmp(sdata,""))
	{
		SetFocus(m_hControl[m_nPriceSelectID]);
		if (m_CommodityInfo.bIfFractionPrice)
		{
			int nInt, nMole;
			sscanf_s(sdata, "%d %d/", &nInt, &nMole);
			if ((nInt<0 || nMole<0) && !m_CommodityInfo.bCanNegative)
			{
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_NONEGATIVE), LoadResWchar_t(IDS_TIP), NULL);
				return;
			}
			SetWindowInt(m_hControl[m_nPriceSelectID], nInt);
			if (nMole < 0)
			{
				SetWindowTextAnsic(m_hControl[m_nPriceSelectID + 2], "-");
				SetWindowInt(m_hControl[m_nPriceSelectID + 3], -nMole);
			}
			else
			{
				SetWindowTextAnsic(m_hControl[m_nPriceSelectID + 2], "+");
				SetWindowInt(m_hControl[m_nPriceSelectID + 3], nMole);
			}
		}
		else
			SetWindowTextAnsic(m_hControl[m_nPriceSelectID], sdata);
	}
}

bool CMousewin::SetHeadData(const wchar_t* sPType, bool bHead)
{
	if (wcscmp(sPType,L""))
	{
		if (!bHead)
			m_cHeadWin.SetShowData(sPType);

		int nLast = m_nCurClickItem.nCurHeadType;
		GetShowItemByStr(m_nCurClickItem,sPType);
		
        if (IsSwapLayOut())
        {
            m_cOrderBtn.SetBShitPosition(true);
        }
        else if (IsSpreadLayOut())
        {
            m_cOrderBtn.SetBShitPosition(m_CheckMovePosition.GetCheck() == BST_CHECKED);
        }
        else
        {
            m_cOrderBtn.SetBShitPosition(false);
        }
		if (m_nCurClickItem.nCurHeadType == MHI_BatchOrder && m_bBatchInfoModify)
			ReSetUserNos();
		else if ((nLast == MHI_BatchOrder || m_nCurClickItem.nCurHeadType == MHI_BatchOrder) && m_nCurClickItem.nCurHeadType != nLast)
			ReSetUserNos();
		else
			LayOut();	
	}
	
	InvalidateRect(m_hControl[CI_Head_Button], NULL, TRUE);
	return true;
}
bool CMousewin::SetSelect(const wchar_t* sSelect, int nRef)
{
	if (wcscmp(sSelect, L""))
	{
		switch (nRef)
		{
		case mptPrice:
			m_StaticPriceSwitch.CxSetWindowText(sSelect);
			m_StaticPriceSwitch.CxGetWindowText(m_sPricetype);
			UpdatePriceStyleControl();
			LayOut();
			break;
		case mptSpreadTrig:
			m_StaticSpCtSwitch.CxSetWindowText(sSelect);
			m_StaticSpCtSwitch.CxGetWindowText(m_sPriceSpreadtype);
			LayOut();
			break;
		default:
			break;
		}
	}
	return true;
}
bool CMousewin::TableButtonToEdit()
{		
	if (IsWindowVisible(m_hControl[CI_UserNo_Combox]) && IsWindowEnabled(m_hControl[CI_UserNo_Combox]))
		SetFocus(m_hControl[CI_UserNo_Combox]);
	else if (IsWindowVisible(m_hControl[CI_ContractNo_Edit_Win]) && IsWindowEnabled(m_hControl[CI_ContractNo_Edit_Win]))
		SetFocus(m_hControl[CI_ContractNo_Edit]);
	
	return true;
}
void CMousewin::OnStopInfoUpdate(bool bShwoUpdate)
{
	m_bShowStopSubmit = bShwoUpdate;
	if (m_bShowStopSubmit&&IsWindow(m_Hwnd))
	{
		if (IsWindow(m_cStopTip.GetHwnd()))
		{
			RECT rect;
			GetClientRect(m_Hwnd, &rect);
			MoveWindow(m_cStopTip.GetHwnd(), rect.left, rect.bottom - BottomTipHeight > 0 ? rect.bottom - BottomTipHeight : 0, rect.right - rect.left, rect.bottom > BottomTipHeight ? BottomTipHeight : rect.bottom, TRUE);
			ShowWindow(m_cStopTip.GetHwnd(), SW_SHOW);
		}	
	}
	else
	{
		if (IsWindow(m_cStopTip.GetHwnd()))
			ShowWindow(m_cStopTip.GetHwnd(), SW_HIDE);
	}
}
//////////////////////////////////////////////////////////////////////////布局
//设置区域是否可见
void CMousewin::SetAreaVisible(int nID, bool bshow, bool bshowsub)
{
	for (vector<sAreaField>::iterator iter = m_AreaInfo.begin(); iter != m_AreaInfo.end(); iter++)
	{
		if (nID == iter->nID)
		{
			iter->bShow = bshow;
			iter->bSubShow = bshowsub;
			break;
		}
	}
}
//初始化区域信息
void CMousewin::InitOrderTypeData()
{
	m_AreaInfo.clear();
	for (int i = Area_Head; i < Area_End; i++)
	{
		sAreaField sAF;
		sAF.nID = i;
		m_AreaInfo.push_back(sAF);
	}
}
void CMousewin::UpdateAreaInfoNormal()
{
	SetAreaVisible(Area_ValidType, true);

	if (m_nCurClickItem.nCurChildType == CHN_Ladder)
	{
		SetAreaVisible(Area_FastOrder, false);
		SetAreaVisible(Area_AutoDirect, false);
		SetAreaVisible(Area_ValidType, false);
		SetAreaVisible(Area_OrderPrice, false);
		SetAreaVisible(Area_Picture, false);
		SetAreaVisible(Area_List, true);
		if(m_CommodityInfo.cCoverMode==cmCover|| m_CommodityInfo.cCoverMode==cmCoverToday)
			SetAreaVisible(Area_OpenContinue, true);
		m_OrderList.UpdateCode(m_sQuoteContractId.c_str(), m_CommodityInfo);
		m_OrderList.UpdateUser(m_sUserNo[0].c_str(), m_sSign[0].c_str(), m_CommodityInfo.cCoverMode);
	}
	else if (m_nCurClickItem.nCurChildType == CHN_Option)
	{
		SetAreaVisible(Area_FastOrder, false);
		SetAreaVisible(Area_Commodity, false);
		SetAreaVisible(Area_OrderQty, false);
		SetAreaVisible(Area_AutoDirect, false);
		SetAreaVisible(Area_AddOne, false);
		SetAreaVisible(Area_ValidType, false);
		SetAreaVisible(Area_OrderPrice, false);
		SetAreaVisible(Area_Picture, false);
		SetAreaVisible(Area_Option, true);
		SetAreaVisible(Area_CloseDay, false);
	}
    else if (m_nCurClickItem.nCurChildType == CHN_Vert)
    {//内盘 显示反手 平开
        if (m_CommodityInfo.cCoverMode == cmCover || m_CommodityInfo.cCoverMode == cmCoverToday)
            SetAreaVisible(Area_CoverOpen, true);
    }
}
void CMousewin::UpdateAreaInfoAdvanced()
{
	//限价
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE))&& m_nCurClickItem.nCurChildType != CHA_Condition)//不等于
	{
		memcpy_s(m_sPricetype, sizeof(m_sPricetype), LoadResWchar_t(IDS_LIMIT_PRICE), sizeof(m_sPricetype));
		m_StaticPriceSwitch.CxSetWindowText(m_sPricetype);
	}
	
	switch (m_nCurClickItem.nCurChildType)
	{
	case CHA_Condition:
		SetAreaVisible(Area_TrigPrice, true, true);
		break;
	case CHA_PreOrder:
	case CHA_AutoOrder:
		SetAreaVisible(Area_FastOrder, false);
		break;
	case CHA_Offer:
		SetAreaVisible(Area_SellPrice, true);
		SetAreaVisible(Area_FastOrder, false);
		SetAreaVisible(Area_AutoDirect, false);
		SetAreaVisible(Area_Picture, true);
		SetAreaVisible(Area_ValidType, false);
		SetAreaVisible(Area_EnquiryNo, true);
		break;
	case CHA_Enquiry:
		SetAreaVisible(Area_OrderQty, false);
		SetAreaVisible(Area_OrderPrice, false);
		SetAreaVisible(Area_FastOrder, false);
		SetAreaVisible(Area_AutoDirect, false);
		SetAreaVisible(Area_Picture, false);
		SetAreaVisible(Area_ValidType, false);
        SetAreaVisible(Area_AddOne, false);
        if (m_CommodityInfo.cCoverMode == cmCover || m_CommodityInfo.cCoverMode == cmCoverToday)
        {
            SetAreaVisible(Area_Ok, true);
            SetAreaVisible(Area_Modify, false);
            SetAreaVisible(Area_Cancel, false);
        }
        else
        {
            SetAreaVisible(Area_Ok, false);
            SetAreaVisible(Area_Modify, true);
            SetAreaVisible(Area_Cancel, true);
            SetAreaVisible(Area_OrderQty, true);
            m_BtnModify.SetText(LoadResWchar_t(IDS_Buy_Enquiry));
            m_BtnCancel.SetText(LoadResWchar_t(IDS_Sell_Enquiry));
        }
		break;
	case CHA_ExecAban:
		SetAreaVisible(Area_OrderPrice, false);
		SetAreaVisible(Area_Modify, true);
		SetAreaVisible(Area_Cancel, true);
		SetAreaVisible(Area_FastOrder, false);
		SetAreaVisible(Area_AutoDirect, false);
		SetAreaVisible(Area_Picture, false);
		SetAreaVisible(Area_ValidType, false);
        SetAreaVisible(Area_CloseDay, false);
        if (m_CommodityInfo.cCoverMode == cmCoverToday)//平今
        {
            SetAreaVisible(Area_ExeAutoClose, true);
            SetAreaVisible(Area_CloseDay, true);
        }
		m_BtnModify.SetText(LoadResWchar_t(IDS_EXECUTE));
		if (IsCodeOfDCE())
			m_BtnCancel.SetText(LoadResWchar_t(IDS_CancelAutoExe));
		else
			m_BtnCancel.SetText(LoadResWchar_t(IDS_ABANDON));
		break;
	case CHA_Shadow:
		SetAreaVisible(Area_AutoDirect, false);
		SetAreaVisible(Area_FastOrder, false);
		m_bAutoCover = true;
		break;
	case CHA_LimtStop:
		SetAreaVisible(Area_TrigPrice, true, false);
		SetAreaVisible(Area_FastOrder, false);
		//m_bAutoCover = true;
		break;
	case CHA_Stop:
		SetAreaVisible(Area_OrderPrice, false);
		SetAreaVisible(Area_TrigPrice, true, false);
		SetAreaVisible(Area_FastOrder, false);
		SetWindowTextW(m_hControl[CI_OrderPrice_Edit], L"");
		//m_bAutoCover = true;
		break;
	case CHA_Iceberg:
		SetAreaVisible(Area_MaxQty, true);
		SetAreaVisible(Area_MinQty, true);
		SetAreaVisible(Area_FastOrder, false);
		SetAreaVisible(Area_AutoDirect, false);
		m_bAutoCover = true;
		break;
	case CHA_SpreadApply:
	case CHA_HedgeApply:
		SetAreaVisible(Area_OrderPrice, false);
		SetAreaVisible(Area_FastOrder, false);
		SetAreaVisible(Area_AutoDirect, false);
		SetAreaVisible(Area_Force, false);
		SetAreaVisible(Area_CloseDay, false);
		SetAreaVisible(Area_AddOne, false);
		SetAreaVisible(Area_Picture, false);
		SetAreaVisible(Area_Modify, true);
		SetAreaVisible(Area_Cancel, true);
		m_BtnModify.SetText(LoadResWchar_t(IDS_BuyApply));
		m_BtnCancel.SetText(LoadResWchar_t(IDS_SellApply));
		break;
	case CHA_Swap:
		SetAreaVisible(Area_FastOrder, false);
		break;
    case CHA_AutoClose:
    {
        SetAreaVisible(Area_OrderPrice, false);
        SetAreaVisible(Area_Modify, true);
        SetAreaVisible(Area_Cancel, true);
        SetAreaVisible(Area_FastOrder, false);
        SetAreaVisible(Area_AutoDirect, false);
        SetAreaVisible(Area_Picture, false);
        SetAreaVisible(Area_ValidType, false);
        SetAreaVisible(Area_CloseDay, false);
        SetAreaVisible(Area_CoverOpen, false);
        SetAreaVisible(Area_Condition, false);
        SetAreaVisible(Area_ExeAutoClose, false);
        SetAreaVisible(Area_AddOne, false);
        SetAreaVisible(Area_Force, false);
        SetAreaVisible(Area_OpenContinue, false);
        SetAreaVisible(Area_CmbFlag, false);
        SetAreaVisible(Area_AssistOrder, false);
        SetAreaVisible(Area_TimerCheck, false);
        SetAreaVisible(Area_CmbFlag, false);
        m_BtnModify.SetText(LoadResWchar_t(IDS_Option_Auto_Close));
        m_BtnCancel.SetText(LoadResWchar_t(IDS_Future_Auto_Close));
        break;
    }
	default:
		break;
	}	
}
void CMousewin::UpdateAreaInfoStartegy()
{
	//禁用
	SetAreaVisible(Area_ValidType, false);
	SetAreaVisible(Area_FastOrder, false);

	//限价
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)))//不等于
	{
		memcpy_s(m_sPricetype, sizeof(m_sPricetype), LoadResWchar_t(IDS_LIMIT_PRICE), sizeof(m_sPricetype));
		m_StaticPriceSwitch.CxSetWindowText(m_sPricetype);
	}
	// 当日有效
	m_sValidtype = LoadResWchar_t(IDS_EFFECTIVE_THEDAY);
	m_BtnValidType.SetText(m_sValidtype.c_str());

	if (m_nCurClickItem.nCurChildType == CHS_PCStop)//客户端止损
	{
		SetAreaVisible(Area_AutoDirect, false);
		SetAreaVisible(Area_CloseDay, false);
		SetAreaVisible(Area_LocalStop, true);
	}
	else if (IsSpreadLayOut())//套利合约字体切换  
	{
		bool bTrig = IsSpreadTriggered();

        SetAreaVisible(Area_Commodity, false);
        SetAreaVisible(Area_Spread_Code, true);
		
		//触发价
		SetAreaVisible(Area_OrderPrice, true, m_CheckAdvanced.GetCheck() == BST_CHECKED && !bTrig);
		//触发时间
		SetAreaVisible(Area_TimePick, m_CheckTimePicker.GetCheck() == BST_CHECKED && !bTrig);

		SetAreaVisible(Area_AssistOrder, true);
		SetAreaVisible(Area_Advanced, !bTrig);
		SetAreaVisible(Area_TimerCheck, !bTrig);

        if (m_CommodityInfo.cCoverMode == cmNone || m_CommodityInfo.cCoverMode == cmUnfinish)//外盘
        {
            SetAreaVisible(Area_MovePosition, false);
        }
        else
        {
            SetAreaVisible(Area_MovePosition, true);
        }

		if (bTrig)//市*价
			SetWindowText(m_hControl[CI_OrderPrice_Edit], L"");
		
		SetWindowTextW(m_hControl[CI_OrderQty_Static], LoadResWchar_t(IDS_SpreadPairs));
		SendMessage(m_hControl[CI_ContractNo_Edit], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx13(), (LPARAM)TRUE);
		m_cOrderBtn.UpdateForeignQty(L"", dBuy);
		m_cOrderBtn.UpdateForeignQty(L"", dSell);
		m_cOrderBtn.UpdateForeignPre(L"", L"", dBuy);
		m_cOrderBtn.UpdateForeignPre(L"", L"", dSell);
	}
}
//更新区域信息
void CMousewin::UpdateAreaInfo()
{
	//重置
	for (vector<sAreaField>::iterator iter = m_AreaInfo.begin(); iter != m_AreaInfo.end(); iter++)
	{
		iter->bShow = false;
		iter->bSubShow = false;
	}
	//必有项
	SetAreaVisible(Area_Head, true);
	SetAreaVisible(Area_Commodity, true);
	//默认
	SetAreaVisible(Area_OrderQty, true);		//询价
	SetAreaVisible(Area_Picture, true);	//改单 询价 应价 行权 期权 点价 --隐藏
	SetAreaVisible(Area_OrderPrice, true);		//套利高级

	//资金账号
	if (m_UserInfo.size()>1)
	{
		SetAreaVisible(Area_UserNo, true);
		if (m_nCurClickItem.nCurHeadType == MHI_Strategys && m_nCurClickItem.nCurChildType != CHS_PCStop)
			SetAreaVisible(Area_UserNo, true, true);
	}

	//备注信息
	if (m_MouseCfg.bShowNoteInfo&&m_cUserType == uiTrader)
		SetAreaVisible(Area_Note, true);//改单 隐藏
	//自动开平 平今 当日有效
	if (m_CommodityInfo.cCoverMode == cmNone || m_CommodityInfo.cCoverMode == cmUnfinish)//外盘
	{
		SetAreaVisible(Area_ValidType, true);
		SetAreaVisible(Area_AutoDirect, false);
		//强平
		if (m_cUserType == uiTrader)
			SetAreaVisible(Area_Force, true);
		if (m_CommodityInfo.CmbDirectFlag==dfSecond)
			SetAreaVisible(Area_CmbFlag, true);
	}
	else//内盘 或者未选合约
	{
		SetAreaVisible(Area_AutoDirect, true);
		if(!IsBatchLayOut())
			m_bAutoCover = m_CheckAutoDirect.GetCheck() == BST_CHECKED ? true : false;
		//if (m_CommodityInfo.cCoverMode == cmCoverToday)//平今
		//	SetAreaVisible(Area_CloseDay, true);							
	}
	//港交所合约
	if (m_CommodityInfo.bAddOne && !IsSpreadLayOut())
		SetAreaVisible(Area_AddOne, true);
	
	//快速
	if (!m_bTradeLogined)
	{
		SetAreaVisible(Area_FastOrder, true);
		SetAreaVisible(Area_OrderQtyRate, m_CheckFastOrder.GetCheck() == BST_CHECKED);
	}

	//改单
	if (m_bModifyOrderOper)
	{
		SetAreaVisible(Area_Picture, false);	
		SetAreaVisible(Area_Note, false);	
		SetAreaVisible(Area_Modify, true);
		SetAreaVisible(Area_Cancel, true);
	}
	switch (m_nCurClickItem.nCurHeadType)
	{
	case MHI_Strategys:	UpdateAreaInfoStartegy();
		break;
	case MHI_Advanced:	UpdateAreaInfoAdvanced();
		break;
	case MHI_Normal:	UpdateAreaInfoNormal();
		break;
	case MHI_BatchOrder: 
    {
        SetAreaVisible(Area_AssistOrder, g_pStrategyApi&&g_pStrategyApi->GetStartegyWorkState()&SWS_Assist);
        SetAreaVisible(Area_AutoDirect, false);
        if (m_CommodityInfo.cCoverMode == cmNone || m_CommodityInfo.cCoverMode == cmUnfinish)
        {//外盘
            SetAreaVisible(Area_Condition, false);
        }
        else
        {
            SetAreaVisible(Area_Condition, true);
            if (m_CheckCondition.GetCheck() == BST_CHECKED)
            {
                SetAreaVisible(Area_TrigPrice, true, true);
            }
            else
            {
                SetAreaVisible(Area_TrigPrice, false, false);
            }
        }
        
        break;
    }
	default:
		break;
	}

    if (IsLadderLayOut())
    {
        SetAreaVisible(Area_RightCancel, true);
    }
	
	//非套利界面
	if(!IsSpreadLayOut())
	{	
		SetWindowTextW(m_hControl[CI_OrderQty_Static], LoadResWchar_t(IDS_STRING1081));
		SendMessage(m_hControl[CI_ContractNo_Edit], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx17(), (LPARAM)TRUE);
	}
		
	//合约字体更新
	m_EditExContract.UpdateFont();
}
//刷新布局
void CMousewin::LayOut()
{
	UpdateAreaInfo();
	int nCurHeight = 0;//当前纵坐标位置
	int nCurWidth = 4;//当前横坐标位置
	bool bDiv = false;
	for (vector<sAreaField>::iterator iter = m_AreaInfo.begin(); iter != m_AreaInfo.end(); iter++)
	{
		switch (iter->nID)
		{
		case Area_Head:							MoveArea_Head(nCurHeight, iter->bShow);
			break;
		case Area_UserNo:						MoveArea_UserNo(nCurHeight, iter->bShow, iter->bSubShow);
			break;
		case Area_Commodity:					MoveArea_Commodity(nCurHeight, iter->bShow);	
			break;
        case Area_Spread_Code:                  MoveArea_SpreadCode(nCurHeight, iter->bShow);
            break;
		case Area_OrderQty:						MoveArea_OrderQty(nCurHeight, iter->bShow);
			break;
		case Area_ValidType:					MoveArea_ValidType(iter->bShow);
			break;
		case Area_OrderQtyRate:					MoveArea_OrderQtyRate(nCurHeight, iter->bShow);
			break;
		case Area_OrderPrice:					MoveArea_OrderPrice(nCurHeight, iter->bShow, iter->bSubShow);
			break;
		case Area_TimePick:						MoveArea_TimePicker(nCurHeight, iter->bShow);
			break;
		case Area_SellPrice:					MoveArea_SellPrice(nCurHeight, iter->bShow);	
			break;
		case Area_EnquiryNo:					MoveArea_Enquriy(nCurHeight, iter->bShow);
			break;
		case Area_LocalStop:					MoveArea_LocalStop(nCurHeight, iter->bShow);
			break;
		case Area_MaxQty:						MoveArea_MaxQty(nCurHeight, iter->bShow);
			break;
		case Area_MinQty:						MoveArea_MinQty(nCurHeight, iter->bShow);
			break;
		case Area_TrigPrice:					MoveArea_TrigPrice(nCurHeight, iter->bShow, iter->bSubShow);
			break;
		case Area_Note:							MoveArea_Note(nCurHeight, iter->bShow);//备注 交易员使用
			break;
		case Area_FastOrder:					MoveArea_Check(CI_FastOrder_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//快速 非交易员
			break; 
		case Area_AutoDirect:					MoveArea_Check(CI_AutoDirect_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//内盘
			break;
		case Area_Force:						MoveArea_Check(CI_ForceClose_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//外盘 交易员
			break;
		case Area_AddOne:						MoveArea_Check(CI_AddOne_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//外盘 T+1
			break;
		case Area_CloseDay:						MoveArea_Check(CI_CloseDay_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//上期 平今
			break;
		case Area_OpenContinue:					MoveArea_Check(CI_OpenContinue_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//连续开仓
			break;
		case Area_AssistOrder:					MoveArea_Check(CI_AssistOrder_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//套利 追单 非交易员
			break;
		case Area_Advanced:						MoveArea_Check(CI_Advanced_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//套利 高级 非交易员
			break;	
		case Area_TimerCheck:					MoveArea_Check(CI_TimeValid_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//套利 时间条件 非交易员
			break;
        case Area_RightCancel:                  MoveArea_Check(CI_Right_Cancel_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);
            break;
        case Area_CoverOpen:                    MoveArea_Check(CI_CoverOpen_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);
            break;
        case Area_Condition:                    MoveArea_Check(CI_Condition_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);
            break;
        case Area_ExeAutoClose:                 MoveArea_Check(CI_ExeAutoClose_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);//行权 行权后自对冲
            break;
        case Area_MovePosition:                 MoveArea_Check(CI_MovePosition_Check, nCurHeight, nCurWidth, iter->bShow, bDiv);
            break;
		case Area_CmbFlag:						MoveArea_CmbFlag(nCurHeight,iter->bShow, bDiv);
			break;
		case Area_Picture:						MoveArea_Picture(nCurHeight, iter->bShow, bDiv);
			break;
		case Area_List:							MoveArea_List(nCurHeight, iter->bShow, bDiv);
			break;
		case Area_Option:						MoveArea_Option(nCurHeight, iter->bShow, bDiv);
			break;
		case Area_Ok:							MoveArea_Ok(nCurHeight, iter->bShow);
			break;
		case Area_Modify:						MoveArea_Modify(nCurHeight, nCurWidth, iter->bShow);
			break;
		case Area_Cancel:						MoveArea_Cancel(nCurHeight, iter->bShow);
			break;                  
		}
	}

	//非改单 快速状态恢复
	if (!m_bModifyOrderOper)
		SetFastControlState();

	//重置价格状态
	SetPriceControlState();

	UpdateWindow(m_Hwnd);

	//配置项保存
	UpdatePropertyCfg();
}
bool CMousewin::ResizeCtrlWindowWidth(HWND hwnd, int nWidth)						//横向移动基准控件窗口
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	ScreenToClient(m_Hwnd, LPPOINT(&rect));
	ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);
	if (nWidth != (rect.right - rect.left))
	{
		MoveWindow(hwnd, rect.left, rect.top, nWidth, rect.bottom - rect.top, TRUE /*FALSE*/);
		return true;
	}
	else
		return false;
}
//基准窗口布局
RECT CMousewin::MoveCtrlWindow(HWND hwnd, int &nHeightStart)
{
	int nnHeightCenter = 0;
	return MoveCtrlWindow(hwnd, nHeightStart, nnHeightCenter);
}
//基准窗口布局
RECT CMousewin::MoveCtrlWindow(HWND hwnd, int &nHeightStart, int &nHeightCenter)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	ScreenToClient(m_Hwnd, LPPOINT(&rect));
	ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);

	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	rect.top = nHeightStart;
	rect.bottom = rect.top + nHeight;
	nHeightCenter = rect.top + nHeight / 2;
	nHeightStart += nHeight;
	MoveWindow(hwnd, rect.left, rect.top, nWidth, nHeight, TRUE /*FALSE*/);
	return rect;
}
//行 控件布局
RECT CMousewin::MoveCtrlRowWindow(HWND hwnd, int nHeightCenter)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	ScreenToClient(m_Hwnd, LPPOINT(&rect));
	ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	rect.top = nHeightCenter - nHeight / 2;
	rect.bottom = rect.top + nHeight;
	MoveWindow(hwnd, rect.left, rect.top, nWidth, nHeight,TRUE /*FALSE*/);
	return rect;
}
//列 控件布局
RECT CMousewin::MoveCtrlColWindow(HWND hwnd, int &nWidthStart)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	ScreenToClient(m_Hwnd, LPPOINT(&rect));
	ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	rect.left = nWidthStart;
	rect.right = rect.left + nHeight;
	nWidthStart += nWidth;
	MoveWindow(hwnd, rect.left, rect.top, nWidth, nHeight, TRUE );
	return rect;
}
//列 横向移动布局
RECT CMousewin::MoveCtrlColWindowEx(HWND hwnd, const int nWidthMove)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	ScreenToClient(m_Hwnd, LPPOINT(&rect));
	ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;	
	rect.left = rect.left + nWidthMove;
	MoveWindow(hwnd, rect.left, rect.top, nWidth, nHeight, TRUE);
	return rect;
}
//控件无效性控制
void CMousewin::SetWindowCtrlEnableOrDisable(bool bEnable)
{
	EnableWindow(m_cHeadWin.GetHwnd(), bEnable);
	EnableWindow(m_hControl[CI_UserNo_Combox], bEnable);
	
	EnableWindow(m_cCodeWin.GetHwnd(), bEnable);
	
	EnableWindow(m_hControl[CI_ContractNo_Edit_Win], bEnable);
	
	EnableWindow(m_hControl[CI_Head_Search], bEnable);
	EnableWindow(m_hControl[CI_UserNo_Lock], bEnable);
	EnableWindow(m_hControl[CI_ContractNo_Lock], bEnable);

	EnableWndStaticOwnerDraw(CI_OrderPrice_Static, bEnable);
	EnableWndStaticOwnerDraw(CI_ValidStyle_Button, bEnable);

	EnableWindow(m_hControl[CI_MaxQty_Edit], bEnable);
	EnableWindow(m_hControl[CI_MaxQty_Spin], bEnable);

	EnableWindow(m_hControl[CI_MinQty_Edit], bEnable);
	EnableWindow(m_hControl[CI_MinQty_Spin], bEnable);

	EnableWindow(m_hControl[CI_AutoDirect_Check], bEnable);
	EnableWindow(m_hControl[CI_ForceClose_Check], bEnable);
	EnableWindow(m_hControl[CI_AddOne_Check], bEnable);
	EnableWindow(m_hControl[CI_FastOrder_Check], bEnable);
    EnableWindow(m_hControl[CI_Right_Cancel_Check], bEnable);
    EnableWindow(m_hControl[CI_CoverOpen_Check], bEnable);
    EnableWindow(m_hControl[CI_Condition_Check], bEnable);

	if (m_bBatchModifyOrderOper&&!bEnable)//批量有用
	{
		m_BtnModify.SetText(LoadResWchar_t(IDS_BatchModify));
		EnableWindow(m_hControl[CI_OrderQty_Edit], FALSE);
		EnableWindow(m_hControl[CI_OrderQty_Spin], FALSE);
	}
	else
	{
		m_BtnModify.SetText(LoadResWchar_t(IDS_STRING1102));
		EnableWindow(m_hControl[CI_OrderQty_Edit], TRUE);
		EnableWindow(m_hControl[CI_OrderQty_Spin], TRUE);
	}
}
//////////////////////////////////////////////////////////////////////////
//窗口创建响应函数
void CMousewin::OnCreateWindow()
{
	InitCommonControls();
	CreateControlWnd();		//创建子窗口	
	InitOrderTypeData();	//初始化区域信息

	ReSetUserNos();			//更新资金账号
	
	InitTotalInfo();		//加载所有项 配置未配置
	UpdatePropertyCurHead();	//列头属性更新
	UpdatePropertyToHeadItem();//
	UpdateSearchLayOut();

	LayOut();				//布局
	UpdatePropertyControl();//控件属性更新

	SetCreateFlag(true);
	return;
}
//创建窗口子控件
void CMousewin::CreateControlWnd()
{
	//控件宽度
	const int nStaticWidth = 60;		//文本宽度
	const int nComboxWidth = 107;		//下拉框宽度
	const int nEditWidthContract = 215;	//合约编辑框宽度
	const int nEditWidth = 88;			//编辑框宽度
	const int nStaticCalWidth = 9;		//计算标识文本宽度
	const int nSmallEditWidth = 36;		//分子编辑框宽度
	const int nSpinWidth = 18;			//Spin宽度
	const int nLongButtonWidth = 108;	//长按钮宽度 定单类型 有效类型
	//控件高度
	const int nStaticHeight = 20;		//文本高度
	const int nComboxHeight = 200;		//下拉框宽度
	const int nEditButtonHeight = 24;	//普通编辑框 按钮高度
	const int nCalHeight = 11;			//运算符高度
	const int nSmallStaticHeight = 13;	//买卖价格 挂单量 高度
	//控件
	const int nStaticLeft = 1;			//文本左侧起点

	const int nSEditLeft = nStaticLeft + nStaticWidth ;					//文本后 编辑框左起点
	const int nSEButtonSet = nSEditLeft + nEditWidthContract + 1;		//文本-合约编辑框后 按钮左起点
	const int nSESpinLeft = nSEditLeft + nEditWidth + 1;				//文本-编辑框后 微调控件左起点
	const int nSESBStaticLeft = nSESpinLeft + nSpinWidth + 2;			//文本-编辑框-微调控件-按钮后 运算标识文本左起点
	const int nSESBSEditLeft = nSESBStaticLeft + nStaticCalWidth + 3;	//文本-编辑框-微调控件-按钮-运算标识文本后 分子编辑框左起点
	const int nSESBSEStatic = nSESBSEditLeft + nSmallEditWidth + 5 + CtrlInterval;	//文本-编辑框-微调控件-按钮-运算标识文本-分子编辑框后 分数标识文本左起点
	const int nSESBSESStatic = nSESBSEStatic + nStaticCalWidth + CtrlInterval;		//文本-编辑框-微调控件-按钮-运算标识文本-分子编辑框-分数标识文本后 分母文本左起点

	const int nRightEnd = 300;			//右侧终点
	m_hInstance = GetModuleHandle(NULL);
	
	//列头
	m_cHeadWin.CreateHeadWin(this, L"MouseHead",0, m_Hwnd, WS_CLIPSIBLINGS);
	CreateButtonControl(CI_Head_Search, L"", BS_PUSHBUTTON, N_MOUSEHEAD_WIDTH+0, 4, nStaticHeight + 2, nStaticHeight + 2);
	
	//资金账号
	CreateStaticControl(CI_UserNoBatch_Static, LoadResWchar_t(IDS_GroupNo), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 6, nStaticWidth - 2, nEditButtonHeight);
	CreateStaticControl(CI_UserNo_Static, LoadResWchar_t(IDS_STRING1077), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 6, nStaticWidth - 2, nEditButtonHeight);
	CreateStaticControl(CI_UserNo_Static2, LoadResWchar_t(IDS_STRING1077), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 6, nStaticWidth - 2, nEditButtonHeight);
	CreateComboxControl(CI_UserNo_Combox, L"", CBS_OWNERDRAWVARIABLE | CBS_AUTOHSCROLL | CBS_DROPDOWN, nSEditLeft, 4, nComboxWidth, nComboxHeight);
	CreateButtonControl(CI_UserNo_Lock, L"", BS_PUSHBUTTON,nSEditLeft + nComboxWidth + 1, 4, nStaticHeight + 2, nStaticHeight + 2);
	CreateStaticControl(CI_UserName_Static, L"", SS_NOTIFY | SS_LEFT, nSEditLeft + nComboxWidth + nStaticHeight + 6, 4, nComboxWidth, nStaticHeight);
	CreateComboxControl(CI_UserNo_Combox2, L"", CBS_OWNERDRAWVARIABLE| CBS_AUTOHSCROLL | CBS_DROPDOWN, nSEditLeft + nComboxWidth + 1, 4, nComboxWidth, nComboxHeight);
	CreateButtonControl(CI_UserNo_Sync, L"", BS_PUSHBUTTON, nSEButtonSet, 4, nStaticHeight + 2, nStaticHeight + 2);

    //账号34
    CreateComboxControl(CI_UserNo_Combox3, L"", CBS_OWNERDRAWVARIABLE | CBS_AUTOHSCROLL | CBS_DROPDOWN, nSEditLeft, 4, nComboxWidth, nComboxHeight);
	CreateComboxControl(CI_UserNo_Combox4, L"", CBS_OWNERDRAWVARIABLE | CBS_AUTOHSCROLL | CBS_DROPDOWN, nSEditLeft + nComboxWidth + 1, 4, nComboxWidth, nComboxHeight);
	//合约
	CreateStaticControl(CI_ContractNo_Static, LoadResWchar_t(IDS_STRING1078), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 30, nStaticWidth - 2, nEditButtonHeight);
	m_EditExContract.Create(CI_ContractNo_Edit_Win, CI_ContractNo_Edit, m_Hwnd, nSEditLeft, 30, nEditWidthContract, nEditButtonHeight, LoadResWchar_t(IDS_SelectCode));
	CreateButtonControl(CI_ContractNo_Lock, L"", BS_PUSHBUTTON, nSEButtonSet, 4, nStaticHeight + 2, nStaticHeight + 2);

    //套利合约
	CreateStaticControl(CI_Spread_Static, LoadResWchar_t(IDS_STRING1078), SS_CENTER, nStaticLeft+1, 30, nStaticWidth - 2, nEditButtonHeight);
	CreateEditControl(CI_Spread_Edit, L"", ES_LEFT | ES_READONLY |ES_AUTOHSCROLL, nSEditLeft, 30, nEditWidthContract, nEditButtonHeight+2);
    
	//数量
	CreateStaticControl(CI_OrderQty_Static, LoadResWchar_t(IDS_STRING1081), SS_CENTER, nStaticLeft, 57, nStaticWidth, nStaticHeight);
	CreateEditControl(CI_OrderQty_Edit, L"1", ES_AUTOHSCROLL | ES_NUMBER, nSEditLeft, 57, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_OrderQty_Spin, L"", 0, nSESpinLeft, 57, nSpinWidth, nEditButtonHeight);

    //数量比例 套利
    CreateStaticControl(CI_OrderRatio_Static, L"", SS_LEFT, nSEditLeft + nComboxWidth + 10, 57, 130, nStaticHeight);
	
	//有效类型 外盘
	m_BtnValidType.CreateWin(L"ValidType", m_Hwnd, WS_CHILD | WS_CLIPSIBLINGS, nSEditLeft + nComboxWidth + 10, 57, nComboxWidth-9, nEditButtonHeight, LoadResWchar_t(IDS_EFFECTIVE_THEDAY), true);
	//数量倍率
	CreateStaticControl(CI_OrderQtyRate_Static, LoadResWchar_t(IDS_OrderQtyRate), SS_CENTER, nStaticLeft, 57, nStaticWidth, nStaticHeight);
	CreateEditControl(CI_OrderQtyRate_Edit, L"1", ES_AUTOHSCROLL | ES_NUMBER, nSEditLeft, 57, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_OrderQtyRate_Spin, L"", 0, nSESpinLeft, 57, nSpinWidth, nEditButtonHeight);
	
	//最大 最小数量
	CreateStaticControl(CI_MaxQty_Static, LoadResWchar_t(IDS_STRING1085), SS_CENTER, nStaticLeft, 84, nStaticWidth, nStaticHeight);
	CreateEditControl(CI_MaxQty_Edit, L"1", ES_AUTOHSCROLL | ES_NUMBER, nSEditLeft, 84, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_MaxQty_Spin, L"", 0, nSESpinLeft, 84, nSpinWidth, nEditButtonHeight);
	CreateStaticControl(CI_MinQty_Static, LoadResWchar_t(IDS_STRING1087), SS_CENTER, nStaticLeft, 111, nStaticWidth, nStaticHeight);
	CreateEditControl(CI_MinQty_Edit, L"1", ES_AUTOHSCROLL | ES_NUMBER, nSEditLeft, 111, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_MinQty_Spin, L"", 0, nSESpinLeft, 111, nSpinWidth, nEditButtonHeight);
	
	//委托价格 	
	CreateStaticControl(CI_OrderPrice_Static, LoadResWchar_t(IDS_LIMIT_PRICE), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 138, nStaticWidth - 2, nEditButtonHeight);
	CreateEditControl(CI_OrderPrice_Edit, L"", ES_AUTOHSCROLL, nSEditLeft, 138, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_OrderPrice_Spin, L"", 0, nSESpinLeft, 138, nSpinWidth, nEditButtonHeight);
	CreateStaticControl(CI_OrderPrice_Static_Cal, L"+", SS_NOTIFY | SS_CENTER, nSESBStaticLeft, 138, nStaticCalWidth, nCalHeight);
	CreateEditControl(CI_OrderPrice_Edit_Mole, L"", ES_AUTOHSCROLL, nSESBSEditLeft, 138, nSmallEditWidth + 5, nEditButtonHeight);
	CreateStaticControl(CI_OrderPrice_Static_Div, L"/", SS_CENTER, nSESBSEStatic, 138, nStaticCalWidth, nEditButtonHeight);
	CreateStaticControl(CI_OrderPrice_Edit_Dem, L"", SS_CENTER, nSESBSESStatic, 138, nSmallEditWidth, nEditButtonHeight);
	
	//超价点数
	CreateStaticControl(CI_OverPoint_Static, L"+", SS_CENTER, nSEditLeft, 138, nEditButtonHeight, nEditButtonHeight);
	CreateSpinControl(CI_OverPoint_Spin, L"", 0, nSESpinLeft, 138, nSpinWidth, nEditButtonHeight);
	m_EditExOverPoint.Create(CI_OverPoint_Edit_Win,CI_OverPoint_Edit, m_Hwnd, nSEditLeft + nEditButtonHeight, 138, nEditWidth - nEditButtonHeight, nEditButtonHeight, LoadResWchar_t(IDS_STRING1160));
	
	//卖出价	
	CreateStaticControl(CI_BuyPrice_Static, LoadResWchar_t(IDS_STRING1091), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 138, nStaticWidth - 2, nEditButtonHeight);
	CreateStaticControl(CI_SellPrice_Static, LoadResWchar_t(IDS_STRING1095), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 138, nStaticWidth - 2, nEditButtonHeight);
	CreateEditControl(CI_SellPrice_Edit, L"", ES_AUTOHSCROLL, nSEditLeft, 138, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_SellPrice_Spin, L"", 0, nSESpinLeft, 138, nSpinWidth, nEditButtonHeight);
	CreateStaticControl(CI_SellPrice_Static_Cal, L"+", SS_NOTIFY | SS_CENTER, nSESBStaticLeft, 138, nStaticCalWidth, nCalHeight);
	CreateEditControl(CI_SellPrice_Edit_Mole, L"", ES_AUTOHSCROLL, nSESBSEditLeft, 138, nSmallEditWidth + 5, nEditButtonHeight);
	CreateStaticControl(CI_SellPrice_Static_Div, L"/", SS_CENTER, nSESBSEStatic, 138, nStaticCalWidth, nEditButtonHeight);
	CreateStaticControl(CI_SellPrice_Edit_Dem, L"", SS_CENTER, nSESBSESStatic, 138, nSmallEditWidth, nEditButtonHeight);

	//询价号
	CreateStaticControl(CI_EnquriyNo_Static, LoadResWchar_t(IDS_AskPriceNo), SS_CENTER, nStaticLeft, 30, nStaticWidth, nStaticHeight);
	CreateEditControl(CI_EnquriyNo_Edit, L"", ES_AUTOHSCROLL, nSEditLeft, 30, nComboxWidth, nEditButtonHeight);

	//买卖行情
	CreateStaticControl(CI_OrderSellPrice_Static, L"", SS_LEFT | SS_NOTIFY, nSEditLeft + nComboxWidth + 10, 134, nEditWidth + 75, 16);
	CreateStaticControl(CI_OrderBuyPrice_Static, L"", SS_LEFT | SS_NOTIFY, nSEditLeft + nComboxWidth + 10, 148, nEditWidth + 75, 16);

	//持仓多头空头
	CreateStaticControl(CI_OrderLongShort_Static, L"", SS_LEFT, nSEditLeft + nComboxWidth + 10, 134, nLongButtonWidth+25, nEditButtonHeight);
	
	//触发价格
	CreateStaticControl(CI_TriggerPrice_Static, LoadResWchar_t(IDS_STRING1093), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 165, nStaticWidth - 2, nEditButtonHeight);
	CreateEditControl(CI_TriggerPrice_Edit, L"", ES_AUTOHSCROLL, nSEditLeft, 165, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_TriggerPrice_Spin, L"", 0, nSESpinLeft, 165, nSpinWidth, nEditButtonHeight);
	CreateStaticControl(CI_TriggerPrice_Static_Cal, L"+", SS_NOTIFY | SS_CENTER, nSESBStaticLeft, 165, nStaticCalWidth, nCalHeight);
	CreateEditControl(CI_TriggerPrice_Edit_Mole, L"", ES_AUTOHSCROLL, nSESBSEditLeft, 165, nSmallEditWidth + 5, nEditButtonHeight);
	CreateStaticControl(CI_TriggerPrice_Static_Div, L"/", SS_CENTER, nSESBSEStatic, 165, nStaticCalWidth, nEditButtonHeight);
	CreateStaticControl(CI_TriggerPrice_Edit_Dem, L"", SS_CENTER, nSESBSESStatic, 165, nSmallEditWidth, nEditButtonHeight);
	
	//快速 自动开平 强平 T+1 平今
	CreateButtonControl(CI_AutoDirect_Check, LoadResWchar_t(IDS_STRING1097), BS_AUTOCHECKBOX, nStaticLeft, 219, 100, nStaticHeight);
	CreateButtonControl(CI_ForceClose_Check, LoadResWchar_t(IDS_STRING1098), BS_AUTOCHECKBOX, nSEditLeft, 219, 100, nStaticHeight);
	CreateButtonControl(CI_AddOne_Check, LoadResWchar_t(IDS_STRING1099), BS_AUTOCHECKBOX, 142, 219, 100, nStaticHeight);
	CreateButtonControl(CI_CloseDay_Check, LoadResWchar_t(IDS_STRING1100), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
	CreateButtonControl(CI_FastOrder_Check, LoadResWchar_t(IDS_FastOrder), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
	CreateButtonControl(CI_AssistOrder_Check, LoadResWchar_t(IDS_AssistOrder), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
	CreateButtonControl(CI_Advanced_Check, LoadResWchar_t(IDS_Advanced), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
	CreateButtonControl(CI_TimeValid_Check, LoadResWchar_t(IDS_TimeCondition), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
	CreateButtonControl(CI_OpenContinue_Check, LoadResWchar_t(IDS_OpenContinue), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
	CreateStaticControl(CI_CmbFlag_Static, LoadResWchar_t(IDS_CmbFlag), SS_CENTER, N_NORMAL_WIDYH-120, 138, 120, nEditButtonHeight);
    CreateButtonControl(CI_Right_Cancel_Check, LoadResWchar_t(IDS_Right_Cancel), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
    CreateButtonControl(CI_CoverOpen_Check, LoadResWchar_t(IDS_Cover_Open), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
    CreateButtonControl(CI_Condition_Check, LoadResWchar_t(IDS_Condition), BS_AUTOCHECKBOX, 245, 219, 100, nStaticHeight);
    CreateButtonControl(CI_MovePosition_Check, LoadResWchar_t(IDS_MovePosition), BS_AUTOCHECKBOX, 245, 219, 160, nStaticHeight);
    CreateButtonControl(CI_ExeAutoClose_Check, LoadResWchar_t(IDS_EXE_AUTO_CLOSE), BS_AUTOCHECKBOX, 245, 219, 160, nStaticHeight);
	
	//条件
	CreateStaticControl(CI_TrigMode_Combox, L"", SS_NOTIFY | SS_CENTER, nSEditLeft, 165, nEditWidth-10, nEditButtonHeight);
	CreateStaticControl(CI_TrigMode_Combox2, L"", SS_NOTIFY | SS_CENTER, nSEditLeft + nEditWidth - 10 + 1, 165, nSpinWidth+10, nEditButtonHeight);

	CreateStaticControl(CI_SpreadSwitch_Static, LoadResWchar_t(IDS_STRING1093), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 165, nStaticWidth - 2, nEditButtonHeight);
	CreateStaticControl(CI_SpreadTrigMode_Combox, L"", SS_NOTIFY | SS_CENTER, nSEditLeft, 165, nEditWidth - 10, nEditButtonHeight);
	CreateStaticControl(CI_SpreadTrigMode_Combox2, L"", SS_NOTIFY |SS_CENTER, nSEditLeft + nEditWidth - 10 + 1, 165, nSpinWidth + 10, nEditButtonHeight);

	//套利触发时间
	CreateStaticControl(CI_SpreadBeignTime_Static, LoadResWchar_t(IDS_Begin_Time), SS_CENTER, nStaticLeft, 30, nStaticWidth, nStaticHeight);
	m_hControl[CI_SpreadBeginTime_Picker] = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT, 
        nSEditLeft+1, 30, 217, nEditButtonHeight+2, m_Hwnd, (HMENU)CI_SpreadBeginTime_Picker, m_hInstance, NULL);
	//DateTime_SetFormat(m_hControl[CI_SpreadTrigTime_Picker], L"yyyy-MM-dd HH:mm");
    CreateStaticControl(CI_SpreadEndTime_Static, LoadResWchar_t(IDS_End_Time), SS_CENTER, nStaticLeft, 30, nStaticWidth, nStaticHeight);
    m_hControl[CI_SpreadEndTime_Picker] = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
        nSEditLeft + 1, 30, 217, nEditButtonHeight + 2, m_Hwnd, (HMENU)CI_SpreadEndTime_Picker, m_hInstance, NULL);
	
	//本地止损价格
	CreateStaticControl(CI_StopLoss_Static, LoadResWchar_t(IDS_StopLoss), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 165, nStaticWidth - 2, nEditButtonHeight);
	CreateEditControl(CI_StopLoss_Edit, L"", ES_AUTOHSCROLL, nSEditLeft, 165, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_StopLoss_Spin, L"", 0, nSESpinLeft, 165, nSpinWidth, nEditButtonHeight);
	CreateStaticControl(CI_StopLoss_Static_Cal, L"+", SS_NOTIFY | SS_CENTER, nSESBStaticLeft, 165, nStaticCalWidth, nCalHeight);
	CreateEditControl(CI_StopLoss_Edit_Mole, L"", ES_AUTOHSCROLL, nSESBSEditLeft, 165, nSmallEditWidth + 5, nEditButtonHeight);
	CreateStaticControl(CI_StopLoss_Static_Div, L"/", SS_CENTER, nSESBSEStatic, 165, nStaticCalWidth, nEditButtonHeight);
	CreateStaticControl(CI_StopLoss_Edit_Dem, L"", SS_CENTER, nSESBSESStatic, 165, nSmallEditWidth, nEditButtonHeight);
	CreateStaticControl(CI_StopProfit_Static, LoadResWchar_t(IDS_StopProfit), SS_NOTIFY | SS_CENTER, nStaticLeft + 1, 165, nStaticWidth - 2, nEditButtonHeight);
	CreateEditControl(CI_StopProfit_Edit, L"", ES_AUTOHSCROLL, nSEditLeft, 165, nEditWidth, nEditButtonHeight);
	CreateSpinControl(CI_StopProfit_Spin, L"", 0, nSESpinLeft, 165, nSpinWidth, nEditButtonHeight);
	CreateStaticControl(CI_StopProfit_Static_Cal, L"+", SS_NOTIFY | SS_CENTER, nSESBStaticLeft, 165, nStaticCalWidth, nCalHeight);
	CreateEditControl(CI_StopProfit_Edit_Mole, L"", ES_AUTOHSCROLL, nSESBSEditLeft, 165, nSmallEditWidth + 5, nEditButtonHeight);
	CreateStaticControl(CI_StopProfit_Static_Div, L"/", SS_CENTER, nSESBSEStatic, 165, nStaticCalWidth, nEditButtonHeight);
	CreateStaticControl(CI_StopProfit_Edit_Dem, L"", SS_CENTER, nSESBSESStatic, 165, nSmallEditWidth, nEditButtonHeight);

	//确定按钮
	m_BtnOk.CreateWin(L"m_BtnOk", m_Hwnd, WS_CHILD | WS_CLIPSIBLINGS, nSEditLeft + nComboxWidth + 10, 246, nComboxWidth-9, nEditButtonHeight, LoadResWchar_t(IDS_ASK_PRICE));
	//修改按钮
	m_BtnModify.CreateWin(L"m_BtnModify", m_Hwnd, WS_CHILD | WS_CLIPSIBLINGS, 5, 246, 145, 65, LoadResWchar_t(IDS_STRING1102));
	//取消按钮
	m_BtnCancel.CreateWin(L"m_BtnCancel", m_Hwnd, WS_CHILD | WS_CLIPSIBLINGS, 155, 246, 145, 65, LoadResWchar_t(IDS_STRING1103));
	
	//下单按钮
	m_cOrderBtn.CreateWin(this, L"sell", m_Hwnd, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS, 0, 0, 275, m_MouseCfg.BuyRgb, m_MouseCfg.SellRgb);
	
	//备注信息
	CreateStaticControl(CI_NoteInfo_Static, LoadResWchar_t(IDS_ExtraInfo), SS_CENTER, nStaticLeft, 30, nStaticWidth, nStaticHeight);
	CreateEditControl(CI_NoteInfo_Edit, L"", ES_AUTOHSCROLL, nSEditLeft, 30, nComboxWidth, nEditButtonHeight);	
	
	//期权虚实值 到期日
	m_hControl[CI_ShowTip_Option] = CreateWindowEx(0, L"static", L"", WS_CHILD | WS_CLIPSIBLINGS | SS_LEFT | SS_CENTERIMAGE, nSESpinLeft, 30, nRightEnd - nSESpinLeft, nEditButtonHeight, m_Hwnd, (HMENU)CI_ShowTip_Option, m_hInstance, NULL);
	
	//可行权数
	m_hControl[CI_CanExcute_Option] = CreateWindowEx(0, L"static", L"", WS_CHILD | WS_CLIPSIBLINGS | SS_LEFT | SS_CENTERIMAGE, nSEditLeft, 246, nComboxWidth - 9, nEditButtonHeight, m_Hwnd, (HMENU)CI_CanExcute_Option, m_hInstance, NULL);
	
	//提示信息
	m_hControl[CI_ShowTip_Static] = CreateWindowEx(0, L"static", L"", WS_CHILD | WS_CLIPSIBLINGS, nStaticLeft, 30, nRightEnd - nStaticLeft, nStaticHeight*3, m_Hwnd, (HMENU)CI_ShowTip_Static, m_hInstance, NULL);
	//止损提示
	m_cStopTip.CreatePopWin(L"tip", m_Hwnd, WS_CHILD | WS_CLIPSIBLINGS, 0);
	//点价下单
	m_OrderList.CreateWin(this, L"list", m_Hwnd, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS, 0, 3, N_NORMAL_WIDYH, m_MouseCfg.bUseCfgColor);
	m_OrderList.UpdateQty(1);
	//期权
	m_OptionView.CreateWin(this, m_Hwnd, 3, 3, m_MouseCfg.bUseCfgColor);
	
	//ID绑定
	SetIdWithWnd();
	//限制编辑框内容长度
	SetSpinSize();
	//spin控件重置大小 适应系统大字体变形
	SetEditLen();
	//字体设置
	SetControlFont();
	//下拉内容更新
	SetPopListContent();
	//重绘更新
	SetRedrawWnd();
	//子类化 窗口控件 对输入 按键进行响应
	SubClassWindow();

	m_cPopSearch.CreatePopWin(this, m_Hwnd, m_SearchString, m_nSearchSize, true);

	SetWindowPos(m_cStopTip.GetHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
void CMousewin::CreateStaticControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight)
{
	m_hControl[nID] = CreateWindowEx(0, L"static", text, WS_CHILD | WS_CLIPSIBLINGS | SS_CENTERIMAGE | type, left, top, nwidth, nheight, m_Hwnd, (HMENU)nID, m_hInstance, NULL);
}
void CMousewin::CreateEditControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight)
{
	m_hControl[nID] = CreateWindowEx(0, L"edit", text, WS_CHILD | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS | type, left, top, nwidth, nheight, m_Hwnd, (HMENU)nID, m_hInstance, NULL);
}
void CMousewin::CreateComboxControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight)
{
	m_hControl[nID] = CreateWindowEx(0, L"COMBOBOX", text, WS_CHILD | WS_VSCROLL | WS_CLIPSIBLINGS | WS_TABSTOP | CBS_HASSTRINGS | CBS_NOINTEGRALHEIGHT | type, left, top, nwidth, nheight, m_Hwnd, (HMENU)nID, m_hInstance, NULL);
}
void CMousewin::CreateButtonControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight)
{
	m_hControl[nID] = CreateWindowEx(0, L"button", text, WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | type, left, top, nwidth, nheight, m_Hwnd, (HMENU)nID, m_hInstance, NULL);
}
void CMousewin::CreateSpinControl(int nID, const wchar_t* text, DWORD type, int left, int top, int nwidth, int nheight)
{
	m_hControl[nID] = CreateWindowEx(0, L"msctls_updown32", text, WS_CHILD | WS_CLIPSIBLINGS | type, left, top, nwidth, nheight, m_Hwnd, (HMENU)nID, m_hInstance, NULL);
}
void CMousewin::SetIdWithWnd()
{
	//ID绑定
	SetWindowLong(m_cHeadWin.GetHwnd(), GWL_ID, (LONG)CI_Head_Button);
	m_hControl[CI_Head_Button] = m_cHeadWin.GetHwnd();

	SetWindowLong(m_BtnValidType.GetHwnd(), GWL_ID, (LONG)CI_ValidStyle_Button);
	m_hControl[CI_ValidStyle_Button] = m_BtnValidType.GetHwnd();

	SetWindowLong(m_BtnOk.GetHwnd(), GWL_ID, (LONG)CI_Ok_Button);
	m_hControl[CI_Ok_Button] = m_BtnOk.GetHwnd();

	SetWindowLong(m_BtnModify.GetHwnd(), GWL_ID, (LONG)CI_Modify_Button);
	m_hControl[CI_Modify_Button] = m_BtnModify.GetHwnd();

	SetWindowLong(m_BtnCancel.GetHwnd(), GWL_ID, (LONG)CI_Cancel_Button);
	m_hControl[CI_Cancel_Button] = m_BtnCancel.GetHwnd();

	m_hControl[CI_ContractNo_Edit_Win] = m_EditExContract.GetHwnd();
	m_hControl[CI_ContractNo_Edit] = m_EditExContract.GetEditWnd();
	m_hControl[CI_OverPoint_Edit_Win] = m_EditExOverPoint.GetHwnd();
	m_hControl[CI_OverPoint_Edit] = m_EditExOverPoint.GetEditWnd();
	m_hControl[CI_OrderBtn] = m_cOrderBtn.GetHwnd();

	SetWindowPos(m_cOrderBtn.GetHwnd(), m_hControl[CI_ShowTip_Static], 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
void CMousewin::SetSpinSize()
{
	//spin控件重置大小 适应系统大字体变形
	int nSpinWidth = 18, nEditButtonHeight = 24;
	SetWindowPos(m_hControl[CI_OrderQty_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[CI_OrderQtyRate_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[CI_MaxQty_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[CI_MinQty_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[CI_OrderPrice_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[CI_OverPoint_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[CI_StopLoss_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[CI_StopProfit_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(m_hControl[CI_TriggerPrice_Spin], NULL, 0, 0, nSpinWidth, nEditButtonHeight, SWP_NOMOVE | SWP_NOZORDER);
}
void CMousewin::SetEditLen()
{
	//限制编辑框内容长度
	SendMessage(m_hControl[CI_UserNo_Combox], CB_LIMITTEXT, 20, 0);
	SendMessage(m_hControl[CI_UserNo_Combox2], CB_LIMITTEXT, 20, 0);
    SendMessage(m_hControl[CI_UserNo_Combox3], CB_LIMITTEXT, 20, 0);
    SendMessage(m_hControl[CI_UserNo_Combox4], CB_LIMITTEXT, 20, 0);
	SendMessage(m_hControl[CI_ContractNo_Edit], EM_SETLIMITTEXT, 60, 0);
	SendMessage(m_hControl[CI_OrderQty_Edit], EM_SETLIMITTEXT, MQty_Limit, 0);
	SendMessage(m_hControl[CI_OrderQtyRate_Edit], EM_SETLIMITTEXT, MQty_Limit, 0);
	SendMessage(m_hControl[CI_MaxQty_Edit], EM_SETLIMITTEXT, MQty_Limit, 0);
	SendMessage(m_hControl[CI_MinQty_Edit], EM_SETLIMITTEXT, MQty_Limit, 0);
	SendMessage(m_hControl[CI_OrderPrice_Edit], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_SellPrice_Edit], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_OrderPrice_Edit_Mole], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_SellPrice_Edit_Mole], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_OverPoint_Edit], EM_SETLIMITTEXT, MPoint_Limit, 0);
	SendMessage(m_hControl[CI_TriggerPrice_Edit], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_TriggerPrice_Edit_Mole], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_StopLoss_Edit], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_StopProfit_Edit], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_StopLoss_Edit_Mole], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_StopProfit_Edit_Mole], EM_SETLIMITTEXT, MPrice_Limit, 0);
	SendMessage(m_hControl[CI_NoteInfo_Edit], EM_SETLIMITTEXT, 50, 0);
	SendMessage(m_hControl[CI_EnquriyNo_Edit], EM_SETLIMITTEXT, 20, 0);
}
void CMousewin::SetControlFont()
{
	for (int i = CI_UserNoBatch_Static; i < CI_Mouse_End; i++)
	{
		if (m_hControl[i] != 0)
		{
			switch (i)
			{
			case CI_ContractNo_Edit:
            case CI_Spread_Edit:
			case CI_OrderQty_Edit:
			case CI_OrderQtyRate_Edit:
			case CI_OrderPrice_Edit:
			case CI_OrderPrice_Edit_Mole:
			case CI_SellPrice_Edit:
			case CI_SellPrice_Edit_Mole:
			case CI_EnquriyNo_Edit:
			case CI_MaxQty_Edit:
			case CI_MinQty_Edit:
			case CI_TriggerPrice_Edit:
			case CI_TriggerPrice_Edit_Mole:
			case CI_NoteInfo_Edit:
			case CI_StopLoss_Edit:
			case CI_StopProfit_Edit:
			case CI_StopLoss_Edit_Mole:
			case CI_StopProfit_Edit_Mole:
			case CI_OverPoint_Edit:
			case CI_SpreadBeginTime_Picker:
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx17(), (LPARAM)TRUE);
				break;
            case CI_SpreadEndTime_Picker:
                SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx17(), (LPARAM)TRUE);
                break;
			case CI_OverPoint_Static:
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontWord22(), (LPARAM)TRUE);
				break;
			case CI_UserNo_Combox:
			case CI_UserNo_Combox2:
            case CI_UserNo_Combox3:
            case CI_UserNo_Combox4:
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx15(), (LPARAM)TRUE);
				break;	
			case CI_TrigMode_Combox:
			case CI_SpreadTrigMode_Combox:
			case CI_TrigMode_Combox2:
			case CI_SpreadTrigMode_Combox2:
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontWordEx15(), (LPARAM)TRUE);
				break;
			case CI_OrderLongShort_Static:
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx15(), (LPARAM)TRUE);
				break;
			case CI_OrderBuyPrice_Static:
			case CI_OrderSellPrice_Static:
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontNumEx15(), (LPARAM)TRUE);
				break;
			case CI_OverPoint_Edit_Win:
			case CI_ContractNo_Edit_Win:
				break;
			default:
				SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
				break;
			}
		}
	}
	m_BtnOk.SetFont(g_FontData.GetFontWord13());
	m_BtnModify.SetFont(g_FontData.GetFontWord22());
	m_BtnCancel.SetFont(g_FontData.GetFontWord22());
	m_BtnValidType.SetFont(g_FontData.GetFontWord13());
	m_EditExOverPoint.UpdateFont();
	m_EditExContract.UpdateFont();
}
void CMousewin::SetRedrawWnd()
{
	m_StaticCode.Init(m_hControl[CI_ContractNo_Static], LoadResWchar_t(IDS_STRING1078), true);
	m_StaticUserNo.Init(m_hControl[CI_UserNo_Static], LoadResWchar_t(IDS_STRING1077));
    //m_StaticUserNo2.Init(m_hControl[CI_UserNo_Static2], LoadResWchar_t(IDS_STRING1077));
	//控件重绘
	m_SpinQty.Init(m_hControl[CI_OrderQty_Spin]);
	m_SpinQtyRate.Init(m_hControl[CI_OrderQtyRate_Spin]);
	m_SpinMaxQty.Init(m_hControl[CI_MaxQty_Spin]);
	m_SpinMinQty.Init(m_hControl[CI_MinQty_Spin]);
	m_SpinPrice.Init(m_hControl[CI_OrderPrice_Spin]);
	m_SpinSellPrice.Init(m_hControl[CI_SellPrice_Spin]);
	m_LossPrice.Init(m_hControl[CI_StopLoss_Spin]);
	m_ProfitPrice.Init(m_hControl[CI_StopProfit_Spin]);
	m_SpinTrigPrice.Init(m_hControl[CI_TriggerPrice_Spin]);
	m_SpinOverPoint.Init(m_hControl[CI_OverPoint_Spin]);

	m_EditNote.Init(m_hControl[CI_NoteInfo_Edit]);
	m_EditEnquiry.Init(m_hControl[CI_EnquriyNo_Edit]);
	m_EditOrderQty.Init(m_hControl[CI_OrderQty_Edit]);
	m_EditOrderQtyRate.Init(m_hControl[CI_OrderQtyRate_Edit]);
	m_EditMaxQty.Init(m_hControl[CI_MaxQty_Edit]);
	m_EditMinQty.Init(m_hControl[CI_MinQty_Edit]);
	m_EditOrderPrice.Init(m_hControl[CI_OrderPrice_Edit]);
	m_EditOrderPriceMole.Init(m_hControl[CI_OrderPrice_Edit_Mole]);
	m_EditSellPrice.Init(m_hControl[CI_SellPrice_Edit]);
	m_EditSellPriceMole.Init(m_hControl[CI_SellPrice_Edit_Mole]);
	m_EditTrigPrice.Init(m_hControl[CI_TriggerPrice_Edit]);
	m_EditTrigPriceMole.Init(m_hControl[CI_TriggerPrice_Edit_Mole]);
	m_EditLoss.Init(m_hControl[CI_StopLoss_Edit]);
	m_EditLossMole.Init(m_hControl[CI_StopLoss_Edit_Mole]);
	m_EditProfit.Init(m_hControl[CI_StopProfit_Edit]);
	m_EditProfitMole.Init(m_hControl[CI_StopProfit_Edit_Mole]);

	//锁定提示信息
	m_SearchBtn.Init(m_hControl[CI_Head_Search]);
	m_LockUserNoBtn.Init(m_hControl[CI_UserNo_Lock]);
	TMouLockTipStrW wtip = { 0 };
	swprintf_s(wtip, LoadResWchar_t(IDS_LockTip), L"\n");
	m_LockUserNoBtn.SetToolTips(wtip);
	m_SyncUserNoBtn.Init(m_hControl[CI_UserNo_Sync]);
	m_SyncUserNoBtn.SetToolTips(LoadResWchar_t(IDS_SyncSpread));
	m_LockCodeBtn.Init(m_hControl[CI_ContractNo_Lock]);


	//下拉内容设置
	m_ComboxUserNo.SetReverFind(m_FillOrderAssist.bBackMatchUserNo);

	//勾选框
	m_CheckAutoDirect.Init(m_hControl[CI_AutoDirect_Check], g_ColorRefData.GetColorBackground());
	m_CheckForceClose.Init(m_hControl[CI_ForceClose_Check], g_ColorRefData.GetColorBackground());
	m_CheckAddOne.Init(m_hControl[CI_AddOne_Check], g_ColorRefData.GetColorBackground());
	m_CheckCloseDay.Init(m_hControl[CI_CloseDay_Check], g_ColorRefData.GetColorBackground());
	m_CheckFastOrder.Init(m_hControl[CI_FastOrder_Check], g_ColorRefData.GetColorBackground());
	m_CheckAssistOrder.Init(m_hControl[CI_AssistOrder_Check], g_ColorRefData.GetColorBackground());
	m_CheckAdvanced.Init(m_hControl[CI_Advanced_Check], g_ColorRefData.GetColorBackground());
	m_CheckTimePicker.Init(m_hControl[CI_TimeValid_Check], g_ColorRefData.GetColorBackground());
	m_CheckOpenContinue.Init(m_hControl[CI_OpenContinue_Check], g_ColorRefData.GetColorBackground());
    m_CheckRightCancel.Init(m_hControl[CI_Right_Cancel_Check], g_ColorRefData.GetColorBackground());
    m_CheckCoverOpen.Init(m_hControl[CI_CoverOpen_Check], g_ColorRefData.GetColorBackground());
    m_CheckCondition.Init(m_hControl[CI_Condition_Check], g_ColorRefData.GetColorBackground());
    m_CheckAutoClose.Init(m_hControl[CI_ExeAutoClose_Check], g_ColorRefData.GetColorBackground());
    m_CheckMovePosition.Init(m_hControl[CI_MovePosition_Check], g_ColorRefData.GetColorBackground());

	m_ComboxUserNo.Init(m_hControl[CI_UserNo_Combox]);
	m_StaticUserName.Init(m_hControl[CI_UserName_Static]);
	m_ComboxUserNo2.Init(m_hControl[CI_UserNo_Combox2]);
    m_ComboxUserNo3.Init(m_hControl[CI_UserNo_Combox3]);
    m_ComboxUserNo4.Init(m_hControl[CI_UserNo_Combox4]);

	m_CombBeginTime.Init(m_hControl[CI_SpreadBeginTime_Picker]);
    m_CombEndTime.Init(m_hControl[CI_SpreadEndTime_Picker]);

	m_StaticPosInfo.Init(m_hControl[CI_OrderLongShort_Static], m_MouseCfg.bUseCfgColor);

	SetClassLong(m_Hwnd, GCL_HBRBACKGROUND, NULL);
	m_hStaticBKBrush = g_ColorRefData.GetHbrBackground();
}
void CMousewin::SetPopListContent()
{
	//定单类型
	TMouPriceTypeStrW m_vstring[2][10] = { 0 };
	int nsize[FOREIGN + 1] = { 0 };
	int nRow[FOREIGN + 1] = { 0 };
	int nCol[FOREIGN + 1] = { 0 };
	memcpy_s(m_vstring[INNER][0], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_LIMIT_PRICE), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[INNER][1], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_MARKET_PRICE), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[INNER][2], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1154), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[INNER][3], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1155), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_vstring[INNER][4], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1122), sizeof(TMouPriceTypeStrW));
	nsize[INNER] = 5;
	nRow[INNER] = 5;
	nCol[INNER] = 1;
	m_StaticPriceSwitch.Init(this, m_hControl[CI_OrderPrice_Static], mptPrice, m_vstring, nsize, nRow, nCol,INNER, true);
	m_StaticPriceSwitch.CxSetWindowText(LoadResWchar_t(IDS_LIMIT_PRICE));
	m_StaticPriceSwitch.CxGetWindowText(m_sPricetype);
	//高级 内
	int i = 0;
	memcpy(m_SearchString[CPS_Advanced_Inner][i++], LoadResWchar_t(IDS_AdvancedInner), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_PreOrder), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_Condition), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_AutoOrder), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_SWAP), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_LIMIT_STOP_PRICE), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STOP_PRICE), sizeof(TMouPriceTypeStrW));

	if (m_MouseCfg.cFlag[Type_Option])
		memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_ASK_PRICE), sizeof(TMouPriceTypeStrW));	
	if (m_MouseCfg.cFlag[Type_Reply])
		memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_REPLY_PRICE), sizeof(TMouPriceTypeStrW));
	if (m_MouseCfg.cFlag[Type_Option])
		memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_EXECUTE_ABANDON), sizeof(TMouPriceTypeStrW));
	
	memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_ARBAPP), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_HEDGEAPP), sizeof(TMouPriceTypeStrW));
    if (m_MouseCfg.cFlag[Type_Option])
        memcpy_s(m_SearchString[CPS_Advanced_Inner][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_Auto_Close), sizeof(TMouPriceTypeStrW));

	m_nSearchSize[CPS_Advanced_Inner] = i;
	//高级 外盘
	i = 0;
	memcpy(m_SearchString[CPS_Advanced_For][i++], LoadResWchar_t(IDS_AdvancedFor), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_For][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_LIMIT_STOP_PRICE), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_For][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STOP_PRICE), sizeof(TMouPriceTypeStrW));
	if (m_MouseCfg.cFlag[Type_IceBerg])
		memcpy_s(m_SearchString[CPS_Advanced_For][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_ICEBERG), sizeof(TMouPriceTypeStrW));
	if (m_MouseCfg.cFlag[Type_Shadow])
		memcpy_s(m_SearchString[CPS_Advanced_For][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_SHADOW), sizeof(TMouPriceTypeStrW));
    if (m_MouseCfg.cFlag[Type_Option])
        memcpy_s(m_SearchString[CPS_Advanced_For][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_ASK_PRICE), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Advanced_For][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_PreOrder), sizeof(TMouPriceTypeStrW));

	m_nSearchSize[CPS_Advanced_For] = i;

	//策略
	i = 0;
	memcpy(m_SearchString[CPS_Strategy][i++], LoadResWchar_t(IDS_Strategys), sizeof(TMouPriceTypeStrW));
	//止损 套利
	if (g_pStrategyApi)
	{
		if(g_pStrategyApi->GetStartegyWorkState()&SWS_StopTrade)
			memcpy_s(m_SearchString[CPS_Strategy][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_LocalStop), sizeof(TMouPriceTypeStrW));
		if (g_pStrategyApi->GetStartegyWorkState()&SWS_SpreadTrade)
		{
			memcpy_s(m_SearchString[CPS_Strategy][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1178), sizeof(TMouPriceTypeStrW));
			memcpy_s(m_SearchString[CPS_Strategy][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1179), sizeof(TMouPriceTypeStrW));

			memcpy_s(m_vstring[INNER][0], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_STRING1093), sizeof(TMouPriceTypeStrW));
			memcpy_s(m_vstring[INNER][1], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_NoPrice), sizeof(TMouPriceTypeStrW));
			
			nsize[INNER] = 2;
			nRow[INNER] = 2;
			nCol[INNER] = 1;
			m_StaticSpCtSwitch.Init(this, m_hControl[CI_SpreadSwitch_Static], mptSpreadTrig, m_vstring, nsize, nRow, nCol, INNER, true);
			m_StaticSpCtSwitch.CxSetWindowText(LoadResWchar_t(IDS_STRING1093));
			m_StaticSpCtSwitch.CxGetWindowText(m_sPriceSpreadtype);
		}	
	}
	//批量
	if(m_MouseCfg.bShowBatchOrder)
		memcpy(m_SearchString[CPS_Strategy][i++], LoadResWchar_t(IDS_BatchOrder), sizeof(TMouPriceTypeStrW));
	m_nSearchSize[CPS_Strategy] = i;

	//普通
	i = 0;
	memcpy(m_SearchString[CPS_Normal][i++], LoadResWchar_t(IDS_Normal), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Normal][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_VertOrder), sizeof(TMouPriceTypeStrW));
	if (m_MouseCfg.bShowLadder)
		memcpy_s(m_SearchString[CPS_Normal][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_LadderOrder), sizeof(TMouPriceTypeStrW));
	memcpy_s(m_SearchString[CPS_Normal][i++], sizeof(TMouPriceTypeStrW), LoadResWchar_t(IDS_OptionHead), sizeof(TMouPriceTypeStrW));
	m_nSearchSize[CPS_Normal] = i;
	
	//有效类型
	TMouValidStrW m_vvstring[10] = { 0 };
    i = 0;
    //if (m_ValidTypeCfgValue & S_CONFIGVALUE_GFD)
    //{
    //    memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_EFFECTIVE_THEDAY), sizeof(TMouValidStrW));
    //}
    //当日有效不做判断，必须显示
    memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_EFFECTIVE_THEDAY), sizeof(TMouValidStrW));
    if (m_ValidTypeCfgValue & S_CONFIGVALUE_GTC)
    {
        memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_EFFECTIVE_LONGTERM), sizeof(TMouValidStrW));
    }
    if (m_ValidTypeCfgValue & S_CONFIGVALUE_GTD)
    {
        memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_EFFECTIVE_DEADLINE), sizeof(TMouValidStrW));
    }
    if (m_ValidTypeCfgValue & S_CONFIGVALUE_FOK)
    {
        memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_IMMEDIATE_ALL), sizeof(TMouValidStrW));
    }
    if (m_ValidTypeCfgValue & S_CONFIGVALUE_IOC)
    {
        memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_IMMEDIATE_PART), sizeof(TMouValidStrW));
    }

	m_cValidTypeWin.CreatePopWin(this, m_Hwnd, m_vvstring, i, 0, 0);
	::ShowWindow(m_cValidTypeWin.GetHwnd(), SW_HIDE);

	//触发方式 触发条件 
	i = 0;
	memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_STRING1122), sizeof(TMouValidStrW));
	memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_BuyPrice), sizeof(TMouValidStrW));
	memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_SellPrice), sizeof(TMouValidStrW));
	m_ComboxMode.Init(m_hControl[CI_TrigMode_Combox], m_vvstring,i);
	m_ComboxMode.CxSetWindowText(LoadResWchar_t(IDS_STRING1122));
	i = 0;
	memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_MatchSpread), sizeof(TMouValidStrW));
	memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_WorkSpread), sizeof(TMouValidStrW));
    memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), LoadResWchar_t(IDS_STRING1122), sizeof(TMouValidStrW));
	m_ComboxSpreadMode.Init(m_hControl[CI_SpreadTrigMode_Combox], m_vvstring, i);
	m_ComboxSpreadMode.CxSetWindowText(LoadResWchar_t(IDS_MatchSpread));
	i = 0;
	memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), L">=", sizeof(TMouValidStrW));
	memcpy_s(m_vvstring[i++], sizeof(TMouValidStrW), L"<=", sizeof(TMouValidStrW));
	m_ComboxCompare.Init(m_hControl[CI_TrigMode_Combox2], m_vvstring, i, false);
	m_ComboxCompare.CxSetWindowText(L"<=");
	m_ComboxSpreadCompare.Init(m_hControl[CI_SpreadTrigMode_Combox2], m_vvstring, i, false);
	m_ComboxSpreadCompare.CxSetWindowText(L"<=");	

	//时间条件
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_CombBeginTime.GetHwnd(), GDT_VALID, &time);
    DateTime_SetFormat(m_hControl[CI_SpreadBeginTime_Picker], L"yyyy-MM-dd HH:mm");
    DateTime_SetSystemtime(m_CombEndTime.GetHwnd(), GDT_VALID, &time);
    DateTime_SetFormat(m_hControl[CI_SpreadEndTime_Picker], L"yyyy-MM-dd HH:mm");
}
void CMousewin::SubClass(int nID)
{
	WNDPROC w;
	w = (WNDPROC)SetWindowLong(m_hControl[nID], GWL_WNDPROC, (LONG)TabsProc);
	m_gTabs.insert(pair<int, WNDPROC>(nID, w));
	SetWindowLong(m_hControl[nID], GWL_USERDATA, LONG(this));//用户数据
}
//子类化 控件 1、实现Tab，enter切换 2、数据过滤
void  CMousewin::SubClassWindow()
{
	//////////////////////////////////////////////////////////////////////////通用控件 子类化
	//用户名-合约1-委托数量-委托价格-最大量-最小量-触发价格-修改-取消
	WNDPROC w;
	//用户名
	COMBOBOXINFO info = { sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(m_hControl[CI_UserNo_Combox], &info);
	SetWindowLong(info.hwndItem, GWL_ID, (LONG)CI_UserNo_Edit);
	m_hControl[CI_UserNo_Edit] = info.hwndItem;
	w = (WNDPROC)SetWindowLong(info.hwndItem, GWL_WNDPROC, (LONG)TabsProc);
	m_gTabs.insert(pair<int, WNDPROC>(CI_UserNo_Edit, w));
	SetWindowLong(info.hwndItem, GWL_USERDATA, LONG(this));//用户数据

	GetComboBoxInfo(m_hControl[CI_UserNo_Combox2], &info);
	SetWindowLong(info.hwndItem, GWL_ID, (LONG)CI_UserNo_Edit2);
	m_hControl[CI_UserNo_Edit2] = info.hwndItem;
	GetComboBoxInfo(m_hControl[CI_UserNo_Combox3], &info);
	SetWindowLong(info.hwndItem, GWL_ID, (LONG)CI_UserNo_Edit3);
	m_hControl[CI_UserNo_Edit3] = info.hwndItem;
	GetComboBoxInfo(m_hControl[CI_UserNo_Combox4], &info);
	SetWindowLong(info.hwndItem, GWL_ID, (LONG)CI_UserNo_Edit4);
	m_hControl[CI_UserNo_Edit4] = info.hwndItem;
	//////////////////////////////////////////////////////////////////////////
	//合约1
	SubClass(CI_ContractNo_Edit);
	//委托数量
	SubClass(CI_OrderQty_Edit);
	//委托价格
	SubClass(CI_OrderPrice_Edit);
	SubClass(CI_OrderPrice_Edit_Mole);
	//卖出价格
	SubClass(CI_SellPrice_Edit);
	SubClass(CI_SellPrice_Edit_Mole);
	//询价号
	SubClass(CI_EnquriyNo_Edit);
	//超价点数
	SubClass(CI_OverPoint_Edit);
	//最大量
	SubClass(CI_MaxQty_Edit);
	//最小量
	SubClass(CI_MinQty_Edit);
	//触发价格
	SubClass(CI_TriggerPrice_Edit);
	SubClass(CI_TriggerPrice_Edit_Mole);
	//修改
	SubClass(CI_Modify_Button);
	//取消
	SubClass(CI_Cancel_Button);
	//止损
	SubClass(CI_StopLoss_Edit);
	SubClass(CI_StopLoss_Edit_Mole);
	SubClass(CI_StopProfit_Edit);
	SubClass(CI_StopProfit_Edit_Mole);
}
void CMousewin::OnButtonUserLock()				//账号锁
{
	m_LockUserNoBtn.SetLockState(!m_LockUserNoBtn.GetLockState());
	EnableWindow(m_hControl[CI_UserNo_Combox], m_LockUserNoBtn.GetLockState() ? FALSE : TRUE);
}
void CMousewin::OnButtonUserSync()
{
	m_SyncUserNoBtn.SetLockState(!m_SyncUserNoBtn.GetLockState());
	EnableWindow(m_hControl[CI_UserNo_Combox2], m_SyncUserNoBtn.GetLockState() ? FALSE : TRUE);
    EnableWindow(m_hControl[CI_UserNo_Combox3], m_SyncUserNoBtn.GetLockState() ? FALSE : TRUE);
    EnableWindow(m_hControl[CI_UserNo_Combox4], m_SyncUserNoBtn.GetLockState() ? FALSE : TRUE);
	if (m_SyncUserNoBtn.GetLockState())
	{
		string str = GetWindowTextAnsic(m_hControl[CI_UserNo_Combox]);
		map<string, sUserInfor>::iterator it1 = m_UserInfo.find(str);
		if (it1 != m_UserInfo.end())
		{
			SetWindowTextW(m_hControl[CI_UserNo_Combox2], LoadRC::ansi_to_unicode(str).c_str());
			OnEditChangeUserNo(CI_UserNo_Combox2);	
            SetWindowTextW(m_hControl[CI_UserNo_Combox3], LoadRC::ansi_to_unicode(str).c_str());
            OnEditChangeUserNo(CI_UserNo_Combox3);
            SetWindowTextW(m_hControl[CI_UserNo_Combox4], LoadRC::ansi_to_unicode(str).c_str());
            OnEditChangeUserNo(CI_UserNo_Combox4);
		}
	}
}
//有效类型 按钮响应
void CMousewin::OnButtonValidStyle()
{
	RECT rectvalid;
	GetWindowRect(m_hControl[CI_ValidStyle_Button], &rectvalid);
	RECT rect2;
	GetWindowRect(m_cValidTypeWin.GetHwnd(), &rect2);
	rectvalid.left = rectvalid.right - (rect2.right - rect2.left);

	SetWindowPos(m_cValidTypeWin.GetHwnd(), NULL, rectvalid.left, rectvalid.bottom, 0, 0, SWP_NOSIZE);
	::ShowWindow(m_cValidTypeWin.GetHwnd(), SW_SHOWNORMAL);
}

//委托 最大 最小 数量 按钮响应
void CMousewin::OnPopQtySelectWnd(int nID)
{
    return;/// 去掉数量弹窗

	if (!m_bCanShowPop)
		return;

	//更新选择ID
	m_nQtySelectID = nID;

	RECT rectqty;
	GetWindowRect(m_hControl[nID], &rectqty);
	ScreenToClient(m_Hwnd, (LPPOINT)&rectqty);
	ScreenToClient(m_Hwnd, (LPPOINT)&rectqty + 1);

	//if (!IsWindow(m_cOrderQtyWin.GetHwnd()))
	//	m_cOrderQtyWin.CreatePopWin(this, L"m_cOrderQtyWin", m_Hwnd, 0, 0, rectqty.left, rectqty.bottom);

	//SetWindowPos(m_cOrderQtyWin.GetHwnd(), HWND_TOP, rectqty.left, rectqty.bottom, 0, 0,SWP_NOSIZE | SWP_NOACTIVATE);
	//::ShowWindow(m_cOrderQtyWin.GetHwnd(), SW_SHOW);
}

//委托 触发价格按钮响应
void CMousewin::OnPopPriceSelectWnd(int nID)
{
	//获取涨停跌停价以及最新价//
	UpdatePriceRange();

	//更新选择ID
	m_nPriceSelectID = nID;

	RECT rectqty;
	GetWindowRect(m_hControl[nID], &rectqty);
	if (!IsWindow(m_cOrderPriceWin.GetHwnd()))
	{
		m_cOrderPriceWin.CreatePopWin(this, L"m_cOrderPriceWin", m_Hwnd, 0, 0, rectqty.left - N_PRICE_WIDTH, (rectqty.bottom + rectqty.top) / 2);
		m_cOrderPriceWin.UpdateData(m_CommodityInfo, m_sPriceLast, m_sPriceMax, m_sPriceMin);
		::ShowWindow(m_cOrderPriceWin.GetHwnd(), SW_SHOW);
	}
	else
	{
		m_cOrderPriceWin.UpdateData(m_CommodityInfo, m_sPriceLast, m_sPriceMax, m_sPriceMin);
		int nWidth, nheight;
		m_cOrderPriceWin.GetWinSize(nWidth, nheight);
		SetWindowPos(m_cOrderPriceWin.GetHwnd(), NULL, rectqty.left - N_PRICE_WIDTH, (rectqty.bottom + rectqty.top) / 2 - nheight / 2, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		::ShowWindow(m_cOrderPriceWin.GetHwnd(), SW_SHOW);
	}
}

//分数 连接运算符
void CMousewin::OnButtonCalFlag(int nID)
{
	if (m_CommodityInfo.bCanNegative&&m_CommodityInfo.bIfFractionPrice)
	{
		string sFlag = GetWindowTextAnsic(m_hControl[nID]);
		if (sFlag == "+")
			SetWindowText(m_hControl[nID], L"-");
		else
			SetWindowText(m_hControl[nID], L"+");

		if (nID == CI_OrderPrice_Static_Cal)
			OnSelChangeEditPrice(CI_OrderPrice_Edit);
	}
}

void CMousewin::UpdatePriceStyleControl()
{
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE))==0)//指定价格
	{
		SetWindowText(m_hControl[CI_OrderPrice_Edit], L"");
		if (m_CommodityInfo.bIfFractionPrice)
		{
			SetWindowText(m_hControl[CI_OrderPrice_Edit_Mole], L"");
			::ShowWindow(m_hControl[CI_OrderPrice_Static_Cal], SW_SHOW);
			::ShowWindow(m_hControl[CI_OrderPrice_Edit_Mole], SW_SHOW);
			::ShowWindow(m_hControl[CI_OrderPrice_Static_Div], SW_SHOW);
			::ShowWindow(m_hControl[CI_OrderPrice_Edit_Dem], SW_SHOW);
		}
		EnableWindow(m_hControl[CI_OrderPrice_Edit], TRUE);	
		EnableWindow(m_hControl[CI_OrderPrice_Spin], TRUE);
	}	
	else
	{
		SetWindowText(m_hControl[CI_OrderPrice_Edit], L"");
		if (m_CommodityInfo.bIfFractionPrice)
		{
			::ShowWindow(m_hControl[CI_OrderPrice_Static_Cal], SW_HIDE);
			::ShowWindow(m_hControl[CI_OrderPrice_Edit_Mole], SW_HIDE);
			::ShowWindow(m_hControl[CI_OrderPrice_Static_Div], SW_HIDE);
			::ShowWindow(m_hControl[CI_OrderPrice_Edit_Dem], SW_HIDE);
		}
		EnableWindow(m_hControl[CI_OrderPrice_Edit], FALSE);
		EnableWindow(m_hControl[CI_OrderPrice_Spin], FALSE);
		PostMessage(m_Hwnd, WM_ONQUOTE, 0, 0);
	}
}

void CMousewin::OnButtonContractLock()				//账号锁
{
	m_LockCodeBtn.SetLockState(!m_LockCodeBtn.GetLockState());
	EnableWindow(m_hControl[CI_ContractNo_Edit_Win], m_LockCodeBtn.GetLockState() ? FALSE : TRUE);
	EnableWindow(m_hControl[CI_ContractNo_Static], m_LockCodeBtn.GetLockState() ? FALSE : TRUE);
	InvalidateRect(m_hControl[CI_ContractNo_Static], NULL, TRUE);
	UpdateWindow(m_hControl[CI_ContractNo_Static]);
}
void CMousewin::OnButtonSearch()
{
	m_cPopSearch.UpdateLayout(m_LoginType, m_bCanStrategyTrade || m_bCanBatchOrder);
	
	::ShowWindow(m_cPopSearch.GetHwnd(), SW_SHOW);	
}
void CMousewin::OnButtonAutoDirect()
{
	m_bAutoCover = m_CheckAutoDirect.GetCheck() == BST_CHECKED;
	LayOut();
}
void CMousewin::OnButtonSpreadAdvanced()
{
	LayOut();
}
void  CMousewin::OnButtonSpreadTimer()
{
	LayOut();
}
void CMousewin::OnButtonFastOrder()
{
	if (m_CheckFastOrder.GetCheck() == BST_CHECKED)
	{
		m_CheckFastOrder.SetColor(g_ColorRefData.GetColorBuyRed());
		g_pCommonModule->StartFastOrder(this);
		ResetPriceLevel();
		EnableWndStaticOwnerDraw(CI_OrderPrice_Static, false);

		if (m_nCurClickItem.nCurHeadType == MHI_Advanced)
			m_nCurClickItem.nCurChildType = CHA_Condition;
		EnableWindow(m_cHeadWin.GetHwnd(),false);
	}
	else
	{
		LoadKeyboardLayout(L"0x0409", KLF_ACTIVATE | KLF_SETFORPROCESS);
		m_CheckFastOrder.SetColor(g_ColorRefData.GetColorBlack());
		g_pCommonModule->StopFastOrder(this);
		EnableWndStaticOwnerDraw(CI_OrderPrice_Static, false);
		EnableWindow(m_cHeadWin.GetHwnd(), true);
		SetShowTips(L"");
	}
	LayOut();
}
void CMousewin::OnButtonQuotePrice(int nID)
{
	string sPrice = GetWindowTextAnsic(m_hControl[nID]);
	int n = sPrice.find(":",0);
	int nend = sPrice.find(" ", 0);
	if (n != -1 && nend != -1 && n < (int)sPrice.size() && nend < (int)sPrice.size() && (n + 1)<nend)
	{
		if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE))!=0||IsSpreadTriggered())
			return;
		sPrice = sPrice.substr(n+1, nend-(n +1));
		m_nPriceSelectID = CI_OrderPrice_Edit;
		SetPriceData(sPrice.c_str());
	}
	else
		return;
}
void CMousewin::OnCheckRightCancel()
{
    bool b = m_CheckRightCancel.GetCheck() == BST_CHECKED;
    m_OrderList.SetRightCancel(b);
    LayOut();
}
void CMousewin::OnSpinDeltaUpDownEx(int nDelta, int nID, int nMoleID)
{
	double dMintick = 1;
	if (m_CommodityInfo.bIfFractionPrice)//分数
	{
		string sInt = GetWindowTextAnsic(m_hControl[nID]);
		int nInt = atoi(sInt.c_str());
		sInt = GetWindowTextAnsic(m_hControl[nMoleID]);
		int nMole = atoi(sInt.c_str());
		if (nDelta > 0)
		{
			if (m_CommodityInfo.bCanNegative)
			{
				if (nInt == 0 && nMole == 0)
					SetWindowTextAnsic(m_hControl[nMoleID - 1], "+");
				sInt = GetWindowTextAnsic(m_hControl[nMoleID - 1]);
				if (sInt=="-")//分子减小 值增大
				{
					if (nMole > 0)
						nMole -= int(m_CommodityInfo.dMiniChangePrice);
					else
					{
						nInt++;
						nMole = m_CommodityInfo.iPrecision - int(m_CommodityInfo.dMiniChangePrice);
					}
				}//"+" 分子增大 值增大
				else
				{
					nMole += int(m_CommodityInfo.dMiniChangePrice);
					if (nMole >= m_CommodityInfo.iPrecision)
					{
						nMole = 0;
						nInt++;
					}
				}
			}
			else
			{
				nMole += int(m_CommodityInfo.dMiniChangePrice);
				if (nMole >= m_CommodityInfo.iPrecision)
				{
					nMole = 0;
					nInt++;
				}
			}
		}
		else
		{
			if (m_CommodityInfo.bCanNegative)
			{
				if (nInt == 0 && nMole == 0)
					SetWindowTextAnsic(m_hControl[nMoleID - 1], "-");
				sInt = GetWindowTextAnsic(m_hControl[nMoleID - 1]);
				if (sInt=="-")//分子增大 值减小
				{
					nMole += int(m_CommodityInfo.dMiniChangePrice);
					if (nMole >= m_CommodityInfo.iPrecision)
					{
						nInt--;
						nMole = 0;
					}
				}
				else//分子减小 值减小
				{
					if (nMole > 0)
						nMole -= int(m_CommodityInfo.dMiniChangePrice);
					else
					{
						nMole = m_CommodityInfo.iPrecision - int(m_CommodityInfo.dMiniChangePrice);
						nInt--;
					}
				}	
			}
			else
			{
				
				if (nMole>0)
					nMole -= int(m_CommodityInfo.dMiniChangePrice);
				else
				{
					if (nInt <= 0)
					{
						nInt = 0;
						nMole = 0;
					}
					else
					{
						nInt--;
						nMole = m_CommodityInfo.iPrecision - int(m_CommodityInfo.dMiniChangePrice);
					}	
				}
			}
		}
		SetWindowInt(m_hControl[nMoleID], nMole);
		SetWindowInt(m_hControl[nID], nInt);
		
	}	
	else//非分数
	{ 
		dMintick = m_CommodityInfo.dMiniChangePrice;
		if (nDelta > 0)
		{
			string sInt = "";
			sInt = GetWindowTextAnsic(m_hControl[nID]);
			double a = atof(sInt.c_str());
			a += dMintick;
			SetWindowDouble(m_hControl[nID], m_CommodityInfo.iPrecision, a);
		}
		else
		{
			string sInt = "";
			sInt = GetWindowTextAnsic(m_hControl[nID]);
			double a = atof(sInt.c_str());
			a -= dMintick;
			if (!m_CommodityInfo.bCanNegative&&a < 0)//非负数
				a = 0;
			SetWindowDouble(m_hControl[nID], m_CommodityInfo.iPrecision, a);
		}
	}	
}
void CMousewin::OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax, int nMin)
{
	if (nDelta > 0)
	{
		string sInt = GetWindowTextAnsic(m_hControl[nID]);
		int a = atoi(sInt.c_str());
		if (a<nMax - 1)
			a++;
		
		SetOrderQty(nID, a);
	}
	else
	{
		string sInt = GetWindowTextAnsic(m_hControl[nID]);
		int a = atoi(sInt.c_str());
		if (a > nMin)
			a--;
		
		a = a < nMin ? nMin : a;
		SetOrderQty(nID, a);
	}
}
void CMousewin::OnSpinDeltaUpDownRate(int nDelta, int nID)
{
	string sInt = GetWindowTextAnsic(m_hControl[nID]);
	double a = atof(sInt.c_str());
	
	if (nDelta > 0)
	{
		if (a < MQty_Max - 1)
			a += m_fastOrderInfo.dChangePoint;
	}
	else
	{
		if (a > MQty_Min)
			a -= m_fastOrderInfo.dChangePoint;
		a = a < MQty_Min ? MQty_Min : a;
	}
	TMouQtyRateStrW strValue = { 0 };
	swprintf_s(strValue, L"%0.*lf", 1, a);
	SetWindowTextW(m_hControl[nID], strValue);
}
void CMousewin::OnWheelCodeList(int nID, int nDelta)
{
	if (nID == CI_ContractNo_Edit)
	{
		if (!IsWindowVisible(m_CodeList.GetHwnd()))
			return;

		if (nDelta > 0)
			m_CodeList.DelNum();
		else
			m_CodeList.AddNum();
	}
}

void CMousewin::OnCodeReturn(int nID)
{
	if (nID == CI_ContractNo_Edit)
		m_CodeList.OnReturn();
}

void CMousewin::OnClickUserStatic()
{
	TMouUserNoStrA sUser = { 0 };
	GetWindowTextA(m_hControl[CI_UserNo_Combox], sUser, sizeof(sUser));
	if (m_nCurClickItem.nCurHeadType != MHI_BatchOrder && g_mainframe)
	{
		map<string, sUserInfor>::iterator it1 = m_UserInfo.find(sUser);
		if (it1 != m_UserInfo.end())
		{
			TMouLinkMsgStrA content = { 0 };
			sprintf_s(content, "user=%s", sUser);
			g_mainframe->Linkage(m_Hwnd, "MouseOrder", W_USER_CHANGED, content);
		}
	}
}
//资金账号 失去焦点处理
void CMousewin::OnKillFocusUserNo(int nID)
{
	if (m_nCurClickItem.nCurHeadType != MHI_BatchOrder)
	{	
		string sUser = GetWindowTextAnsic(m_hControl[nID]);
		if (nID == CI_UserNo_Combox)
		{
			//用户未变动
			bool bReLayOut = false;
			if (m_sUserNo[0] != sUser)
			{
				char ctype = m_cUserType;
				//更新用户信息
				UpdateUserInfo(sUser);
				//更新品种
				CodeUpdateCommodityInfo();
				//更新持仓
				UpdatePositionQty(); //资金账号:失去焦点处理
				bReLayOut = ctype == m_cUserType;
			}

			//刷新布局 登录类型变换
			if (bReLayOut)		
				LayOut();

			//刷新  
			FormatDisplayQtyAndFreeze(true, true);
		}
        else
        {
            bool bUpdate = false;
            if (nID == CI_UserNo_Combox2 && m_sUserNo[1] != sUser)
            {
                bUpdate = true;
            }
            else if (nID == CI_UserNo_Combox3 && m_sUserNo[2] != sUser)
            {
                bUpdate = true;
            }
            else if (nID == CI_UserNo_Combox4 && m_sUserNo[3] != sUser)
            {
                bUpdate = true;
            }

            if (bUpdate)
            {
                //更新用户信息
                UpdateUserInfo(sUser, 1);
                //更新品种
                CodeUpdateCommodityInfo();
                //更新持仓
                UpdatePositionQty();//资金账号:失去焦点处理
            }
        }
	}
	else
		m_StaticUserName.SetToolTips(L"");
}
//资金账号信息列表选择项改变处理 //此时 编辑框内容还未更新 不能直接从编辑框获取用户信息 
void CMousewin::OnSelChangeUserNo(int nID)//问题 主用户切换和失去焦点 触发 OnUserUpdateContract（发现相同用户 没有继续处理） UpdatePositionQty
{
	//通过索引 获取用户名
	TMouUserNoStrA sUser = { 0 };
	int nidex = SendMessage(m_hControl[nID], CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	SendMessageA(m_hControl[nID], CB_GETLBTEXT, nidex, LPARAM(sUser));
	if (m_nCurClickItem.nCurHeadType != MHI_BatchOrder)
	{
		if (nID==CI_UserNo_Combox)
		{
			//用户未变动
			bool bReLayOut = false;

			if (m_sUserNo[0] != sUser)
			{
				char ctype = m_cUserType;
				if (UpdateUserInfo(sUser))
				{
					if (m_SyncUserNoBtn.GetLockState())
					{
						SetWindowTextW(m_hControl[CI_UserNo_Combox2], LoadRC::ansi_to_unicode(sUser).c_str());
						OnEditChangeUserNo(CI_UserNo_Combox2);
                        SetWindowTextW(m_hControl[CI_UserNo_Combox3], LoadRC::ansi_to_unicode(sUser).c_str());
                        OnEditChangeUserNo(CI_UserNo_Combox3);
                        SetWindowTextW(m_hControl[CI_UserNo_Combox4], LoadRC::ansi_to_unicode(sUser).c_str());
                        OnEditChangeUserNo(CI_UserNo_Combox4);
					}
				}
				UpdatePositionQty();//资金账号:切换账号
				CodeUpdateCommodityInfo();
				bReLayOut = ctype == m_cUserType && !IsSpreadLayOut()? false : true;
				m_CodeList.UpdateData(m_CommodityInfo.Sign);//更新下拉匹配列表 交易合约	
			}

			//刷新布局
			if (bReLayOut)
				LayOut();

			if (g_mainframe)
			{
				map<string, sUserInfor>::iterator it1 = m_UserInfo.find(sUser);
				if (it1 != m_UserInfo.end())
				{
					TMouLinkMsgStrA content = { 0 };
					sprintf_s(content, "user=%s", sUser);
					g_mainframe->Linkage(m_Hwnd, "MouseOrder", W_USER_CHANGED, content);
				}
			}
		}
		else
		{
			bool bReLayOut = false;
            if (nID == CI_UserNo_Combox2)
            {
                if (m_sUserNo[1] != sUser)
                {
                    bReLayOut = true;
                }
                UpdateUserInfo(sUser, 1);
            }
            else if (nID == CI_UserNo_Combox3)
            {
                if (m_sUserNo[2] != sUser)
                {
                    bReLayOut = true;
                }
                UpdateUserInfo(sUser, 2);
            }
            else if (nID == CI_UserNo_Combox4)
            {
                if (m_sUserNo[3] != sUser)
                {
                    bReLayOut = true;
                }
                UpdateUserInfo(sUser, 3);
            }

			UpdatePositionQty();//资金账号:切换账号
			if (bReLayOut)
			{
				CodeUpdateCommodityInfo();
				LayOut();
			}
		}
	}
	else//批量
	{
		map<string, BatchGroupInfo>::iterator iter = m_batchGroupVec.find(sUser);
		if (iter != m_batchGroupVec.end())
		{
			SetWindowTextW(m_hControl[CI_UserName_Static], LoadRC::ansi_to_unicode(iter->second.BatchGroupName).c_str());
			m_StaticUserName.UpdateText(iter->second.BatchGroupName);
			for (map<string, sUserInfor>::iterator ite = m_UserInfo.begin(); ite != m_UserInfo.end(); ite++)
			{
				if (strcmp(ite->second.sUserNo,iter->second.BaseUser)==0)
				{
					m_sSign[0] = ite->second.sSign;
					memcpy(m_CommodityInfo.Sign, m_sSign[0].c_str(), sizeof(m_CommodityInfo.Sign));
					m_sUserNo[0] = ite->second.sUserNo;
					break;
				}
			}
			ComboBox_SetText(m_hControl[CI_UserNo_Combox], LoadRC::ansi_to_unicode(iter->second.BatchGroupNo).c_str());
			char cMode = m_CommodityInfo.cCoverMode;
			map<string, char>::iterator it2 = m_UserCoverModeMap.find(m_sUserNo[0]);
			if (it2 != m_UserCoverModeMap.end())
			{
				m_CommodityInfo.cCoverMode = it2->second;
                CodeUpdateCommodityInfo();
				if (cMode != m_CommodityInfo.cCoverMode)
					LayOut();
			}
		}
		m_StaticUserName.SetToolTips(L"");
	}
}
bool CMousewin::UpdateUserInfo(string sUser, int index)
{
	map<string, sUserInfor>::iterator it1 = m_UserInfo.find(sUser);
    if (index == 0)
    {
        if (it1 != m_UserInfo.end())
        {
            m_sUserNo[0] = it1->first;
            m_sUserName = it1->second.sUserName;
            m_sSign[0] = it1->second.sSign;
            if (m_UserInfo.size() == 1)
            {
                m_sSign[1] = m_sSign[0];
                m_sSign[2] = m_sSign[0];
                m_sSign[3] = m_sSign[0];
            }
            memcpy(m_CommodityInfo.Sign, m_sSign[0].c_str(), sizeof(m_CommodityInfo.Sign));
            m_cUserType = it1->second.cUserType;

            char cMode = m_CommodityInfo.cCoverMode;
            map<string, char>::iterator it2 = m_UserCoverModeMap.find(m_sUserNo[0]);
            if (it2 != m_UserCoverModeMap.end())
            {
                if (!(cMode == cmCoverToday && it2->second == cmCover))
                    m_CommodityInfo.cCoverMode = it2->second;

                CodeUpdateCommodityInfo();

                if (cMode != m_CommodityInfo.cCoverMode)
                    LayOut();
            }

            m_StaticUserName.UpdateText(m_sUserName);
            //////////////////////////////////////////////////////////////////////////客户信息
            wstring sInfo = L"";
            if (!m_sUserNo[0].empty())//仅外盘有提示信息
            {
                const TUserInfo *tuser = g_pTradeData->GetUser(m_sUserNo[0].c_str(), m_sSign[0].c_str());
                if (tuser)
                    sInfo = FormatUserSummary(tuser);
            }
            m_StaticUserName.SetToolTips(sInfo.c_str());

            m_OrderList.UpdateUser(m_sUserNo[0].c_str(), m_sSign[0].c_str(), m_CommodityInfo.cCoverMode);
            return true;
        }
        else
        {
            /// 如果账号不存在，不显示信息
            m_StaticUserName.UpdateText("");
            m_StaticUserName.SetToolTips(L"");

            m_OrderList.UpdateUser("", "", m_CommodityInfo.cCoverMode);
            return false;
        }
    }
    else
    {
        if (it1 != m_UserInfo.end())
        {
            m_sUserNo[index] = it1->first;
            m_sSign[index] = it1->second.sSign;
            return true;
        }
        else
            return false;
    }
		
}
//bool CMousewin::UpdateUserInfo2(string sUser)						
//{
//	map<string, sUserInfor>::iterator it1 = m_UserInfo.find(sUser);
//	if (it1 != m_UserInfo.end())
//	{
//        for (int i = 1; i < 4; i++)
//        {
//            m_sUserNo[i] = it1->first;
//            m_sSign[i] = it1->second.sSign;
//        }
//		return true;
//	}
//	else
//		return false;
//}
void CMousewin::OnEditChangeUserNo(int nID, bool bSync)
{
	string str = GetWindowTextAnsic(m_hControl[nID]);
	if (nID==CI_UserNo_Combox)
	{
		if (UpdateUserInfo(str))
		{
			//联动账号
			if (!bSync)
			{
				TMouLinkMsgStrA content = { 0 };
				sprintf_s(content, "user=%s", str.c_str());
				g_mainframe->Linkage(m_Hwnd, "MouseOrder", W_USER_CHANGED, content);
			}		
			if (m_SyncUserNoBtn.GetLockState())
			{
				SetWindowTextW(m_hControl[CI_UserNo_Combox2], LoadRC::ansi_to_unicode(str).c_str());
				OnEditChangeUserNo(CI_UserNo_Combox2);
                SetWindowTextW(m_hControl[CI_UserNo_Combox3], LoadRC::ansi_to_unicode(str).c_str());
                OnEditChangeUserNo(CI_UserNo_Combox3);
                SetWindowTextW(m_hControl[CI_UserNo_Combox4], LoadRC::ansi_to_unicode(str).c_str());
                OnEditChangeUserNo(CI_UserNo_Combox4);
			}
		}
	}
    else
    {
        bool bReLayOut = false;
        bool bUpdate = false;
        if (nID == CI_UserNo_Combox2)
        {
            if (m_sUserNo[1] != str)
            {
                bReLayOut = true;
            }
            bUpdate = UpdateUserInfo(str, 1) && IsWindowVisible(m_hControl[CI_UserNo_Combox2]);
        }
        else if (nID == CI_UserNo_Combox3)
        {
            if (m_sUserNo[2] != str)
            {
                bReLayOut = true;
            }
            bUpdate = UpdateUserInfo(str, 2) && IsWindowVisible(m_hControl[CI_UserNo_Combox3]);
        }
        else if (nID == CI_UserNo_Combox4)
        {
            if (m_sUserNo[3] != str)
            {
                bReLayOut = true;
            }
            bUpdate = UpdateUserInfo(str, 3) && IsWindowVisible(m_hControl[CI_UserNo_Combox4]);
        }

        if (bUpdate)
        {
            UpdatePositionQty();//资金账号:编辑修改
            if (bReLayOut)
            {
                CodeUpdateCommodityInfo();
                LayOut();
            }
        }
    }
}
void CMousewin::OnEditChangeNoteInfo()
{
	string str = GetWindowTextAnsic(m_hControl[CI_NoteInfo_Edit]);
	if (str.length()>NNoteLen - 1)//超出限制
	{
		char cCut[NNoteLen] = { 0 };
		memcpy_s(cCut, NNoteLen - 1, str.c_str(), NNoteLen-1);
		wstring wstr = LoadRC::ansi_to_unicode(cCut);
		if (!wstr.empty())
		{
			wstring::iterator iter = wstr.begin() + wstr.length() - 1;
			if (*iter==L'?')//分割出不可见字符 
				cCut[NNoteLen-2] = '\0';
		}
		SetWindowTextA(m_hControl[CI_NoteInfo_Edit], cCut);
	}
}

//合约代码失去焦点处理
void CMousewin::OnKillFocusCode(int nID)
{
	if (g_MouseTrack)	//合约编辑框失去焦点 隐藏弹出匹配的列表
		ShowWindow(m_CodeList.GetHwnd(), SW_HIDE);
	if (nID== CI_ContractNo_Edit)
	{
		string sCode = GetWindowTextAnsic(m_hControl[CI_ContractNo_Edit]);
		SetContractNo(sCode.c_str(), cstSelect | cstQtyFreeze | cstQuote);
	}	
}
void CMousewin::OnPopCodeSelectWnd(int nID)
{
	int nInc = 1;
	
	RECT rectvalid;
	GetWindowRect(m_hControl[CI_ContractNo_Edit_Win], &rectvalid);
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	int nScreenWidth = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int nScreenStart = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
	int nHeight = 10 * N_CODE_HEIGHT;
	if (!IsSpreadLayOut())
	{
		if (rectvalid.bottom + nHeight >= rect.bottom)
			rectvalid.bottom = rectvalid.top - nHeight;
		if (rectvalid.left + 432 - 64 + nInc >= nScreenStart + nScreenWidth)
			rectvalid.left = rectvalid.right - 432;
		else
			rectvalid.left = rectvalid.left - 64 + nInc;
		if (!IsWindow(m_cCodeWin.GetHwnd()))
		{
			m_cCodeWin.CreatePopWin(this, L"m_cCodeWin", m_Hwnd, WS_CLIPCHILDREN, 0, rectvalid.left, rectvalid.bottom);
			m_cCodeWin.UpdateData(m_CommodityInfo.Sign);

			::ShowWindow(m_cCodeWin.GetHwnd(), SW_SHOWNORMAL);
		}
		else
		{
			m_cCodeWin.UpdateData(m_CommodityInfo.Sign);

			SetWindowPos(m_cCodeWin.GetHwnd(), NULL, rectvalid.left, rectvalid.bottom, 0, 0, SWP_NOSIZE);
			::ShowWindow(m_cCodeWin.GetHwnd(), SW_SHOWNORMAL);
		}
	}
	//else
	//{
	//	if (rectvalid.bottom + nHeight >= rect.bottom)
	//		rectvalid.bottom = rectvalid.top - nHeight;
	//	if (!IsWindow(m_cCodeSpreadWin.GetHwnd()))
	//	{
	//		m_cCodeSpreadWin.CreatePopWin(this, L"m_cSpreadCodeWin", m_Hwnd, WS_CLIPCHILDREN, 0, rectvalid.left - 64 + nInc, rectvalid.bottom);
	//		m_cCodeSpreadWin.UpdateData();

	//		::ShowWindow(m_cCodeSpreadWin.GetHwnd(), SW_SHOWNORMAL);
	//	}
	//	else
	//	{
	//		m_cCodeSpreadWin.UpdateData();

	//		SetWindowPos(m_cCodeSpreadWin.GetHwnd(), NULL, rectvalid.left - 64 + nInc, rectvalid.bottom, 0, 0, SWP_NOSIZE);
	//		::ShowWindow(m_cCodeSpreadWin.GetHwnd(), SW_SHOWNORMAL);
	//	}
	//}
	
}
//合约代码变化处理
void CMousewin::OnSelChangeCode(int nID)
{	
	if (m_bShowFuzzyQuery&&!m_bModifyOrderOper)
	{
		RECT rectvalid;
		GetWindowRect(m_hControl[CI_ContractNo_Edit_Win], &rectvalid);
		ScreenToClient(m_Hwnd, (LPPOINT)&rectvalid);
		ScreenToClient(m_Hwnd, (LPPOINT)&rectvalid + 1);
		
		if (!IsWindow(m_CodeList.GetHwnd()))
		{
			DWORD dwStyle = WS_CLIPSIBLINGS | WS_CHILD | WS_BORDER;
			DWORD dwStyleeEx = WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE;
			m_CodeList.CreatePopWin(this, L"m_cCodeListWin", m_Hwnd, dwStyle, dwStyleeEx, rectvalid.left, rectvalid.bottom, rectvalid.right - rectvalid.left);
		}

		SetWindowPos(m_CodeList.GetHwnd(), HWND_TOP, rectvalid.left, rectvalid.bottom, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

		if (!IsSpreadLayOut())
		{
			string sCode = GetWindowTextAnsic(m_hControl[CI_ContractNo_Edit]);
			m_CodeList.EditChange(sCode, m_sSign[0]);
		}
	}	
}

//价格改变处理
void CMousewin::OnSelChangeEditPrice(int nID)
{
	if (nID==CI_OrderPrice_Edit||nID==CI_OrderPrice_Edit_Mole)
	{
		if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE))!=0)
			PostMessage(m_Hwnd,WM_ONQUOTE,0,0);
		else
		{
			double dprice = 0;
			TMouFormatPriceStrW wText = { 0 };
			if (!GetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, dprice, dBuy, false)&&!IsSpreadLayOut())
			{
				m_cOrderBtn.UpdatePrice(L"", L"");
				return;
			}
			string cPrice = "";
			cPrice = CConvert::FormatPriceToStr(dprice, m_CommodityInfo);
			swprintf_s(wText, L"%s", LoadRC::ansi_to_unicode(cPrice).c_str());
			
			m_cOrderBtn.UpdatePrice(wText, wText);
			UpdateBtnCanOpen();
		}
	}
}
void CMousewin::OnSelChangeEditPoint()					//超价点数变动处理
{
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)) != 0)
		PostMessage(m_Hwnd, WM_ONQUOTE, 0, 0);
	UpdateBtnCanOpen();
}
//价格编辑框得到焦点处理
void CMousewin::OnSetFocusEditPrice(int nID)
{
	switch (nID)
	{
		case CI_StopLoss_Edit:
		case CI_StopProfit_Edit:
			SetShowTips(LoadResWchar_t(IDS_StopEditTip));
		case CI_OrderPrice_Edit:
		case CI_SellPrice_Edit:
		case CI_TriggerPrice_Edit:
		{
			TMouPriceStrA strText= { 0 };
			GetWindowTextA(m_hControl[nID], strText, sizeof(TMouPriceStrA));
			double dPrice = 0;
			if (m_CommodityInfo.bIfFractionPrice)
			{
				TMouPriceStrA sMole = { 0 };
				TMouCalStrA sCal = { 0 };
				GetWindowTextA(m_hControl[nID + 3], sMole, sizeof(TMouPriceStrA));
				GetWindowTextA(m_hControl[nID + 2], sCal, sizeof(TMouCalStrA));
				if (!strcmp(sCal, "+"))
					dPrice = atof(strText) + atof(sMole) / m_CommodityInfo.iPrecision;
				else
					dPrice = atof(strText) - atof(sMole) / m_CommodityInfo.iPrecision;			
			}
			else
				dPrice = atof(strText);

			if (CConvert::DoubleSameValue(0, dPrice, FLOAT_SAME_ERRORDIFF))
			{
				UpdatePriceRange();
				SetOrderPrice(nID, nID + 3, m_sPriceLast);
			}
			if (m_FillOrderAssist.uiPricePos < 0)
				return;
			if ((UINT)strlen(strText)>= m_FillOrderAssist.uiPricePos)
				Edit_SetSel(m_hControl[nID], (int)(strlen(strText) - m_FillOrderAssist.uiPricePos), (int)strlen(strText));
			else
				Edit_SetSel(m_hControl[nID], 0, -1);
		}
		break;
		case CI_UserNo_Combox:
		{
			TMouPriceStrA strText = { 0 };
			GetWindowTextA(m_hControl[CI_UserNo_Edit], strText, sizeof(TMouPriceStrA));
			if (m_FillOrderAssist.uiPricePos <= 0)
				return;
			if ((UINT)strlen(strText) >= m_FillOrderAssist.uiPricePos)
				Edit_SetSel(m_hControl[CI_UserNo_Edit], (int)(strlen(strText) - m_FillOrderAssist.uiPricePos), (int)strlen(strText));
			else
				Edit_SetSel(m_hControl[CI_UserNo_Edit], 0, -1);
		}
		break;
		default:
			break;
	}
}
void CMousewin::OnKillFocusOrderQty()
{
	FormatDisplayQtyAndFreeze(true,true);
}
void CMousewin::OnSetFocusOrderQty()							//委托数量得到焦点
{
	FormatDisplayQtyAndFreeze(true, true);
	{
		TMouPriceStrA strText = { 0 };
		GetWindowTextA(m_hControl[CI_OrderQty_Edit], strText, sizeof(TMouPriceStrA));
		Edit_SetSel(m_hControl[CI_OrderQty_Edit], 0, (int)strlen(strText));
	}
}
void CMousewin::OnUpdateOrderQty()							//委托数量更新
{
	TMouQtyStrW wtype = { 0 };
	GetWindowText(m_hControl[CI_OrderQty_Edit], wtype, sizeof(TMouQtyStrW));
	m_cOrderBtn.UpdateEditQty(_wtoi(wtype));
	m_OrderList.UpdateQty(_wtoi(wtype));
}
//////////////////////////////////////////////////////////////////////////数据 更新界面
void CMousewin::FillBaseDataToDlg(const TSendOrder * pOrder)
{
	//资金账号
	if (m_UserInfo.size() > 1)
		ComBoxStringMsgOperAnsic(m_hControl[CI_UserNo_Combox], CB_SELECTSTRING, -1, pOrder->UserNo);
	UpdateUserInfo(pOrder->UserNo);

	m_sValidtype = CMouseString::ValidTypeToWStr(pOrder->ValidType);//有效类型

	if (pOrder->ValidType == vtGTD)
		m_BtnValidType.SetText(LoadRC::ansi_to_unicode(pOrder->ValidTime).c_str());
	else
		m_BtnValidType.SetText(m_sValidtype.c_str());

	CConvert::TContractKeyToTContractNo(*pOrder, m_sCode);

	SetContractNo(m_sCode.c_str(),cstChange);

	//刷新的数据
	switch (pOrder->OrderType)
	{
	case otLimit:
		SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);
		SetOrderQty(CI_OrderQty_Edit, pOrder->OrderQty);
		break;
	case otMarketStop:
		SetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, pOrder->TriggerPrice);
		SetOrderQty(CI_OrderQty_Edit, pOrder->OrderQty);
		break;
	case otLimitStop:
		SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);
		SetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, pOrder->TriggerPrice);
		SetOrderQty(CI_OrderQty_Edit, pOrder->OrderQty);
		break;
	case otIceberg:
		SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);
		SetOrderQty(CI_OrderQty_Edit, pOrder->OrderQty);
		{
			SetOrderQty(CI_MaxQty_Edit, pOrder->MaxOrderQty);
			SetOrderQty(CI_MinQty_Edit, pOrder->MinOrderQty);
		}
		break;
	case otGhost:
		break;
	default:
		break;
	}

	if (pOrder->AddOneIsValid == tsNight&&IsWindowVisible(m_hControl[CI_AddOne_Check]))//是否T+1有效
		m_CheckAddOne.SetCheck(BST_CHECKED);// SendMessage(m_hControl[CI_AddOne_Check], BM_SETCHECK, 1, 0);

	if (pOrder->IsRiskOrder == bYes&&IsWindowVisible(m_hControl[CI_ForceClose_Check]))
		m_CheckForceClose.SetCheck(BST_CHECKED); //SendMessage(m_hControl[CI_ForceClose_Check], BM_SETCHECK, 1, 0);
	memcpy_s(&m_OrderModifyParam, sizeof(m_OrderModifyParam), pOrder, sizeof(m_OrderModifyParam));
}

//////////////////////////////////////////////////////////////////////////窗口处理过程响应函数
void CMousewin::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case CI_TriggerPrice_Spin:
	case CI_OrderPrice_Spin:
	case CI_SellPrice_Spin:
	case CI_StopLoss_Spin:
	case CI_StopProfit_Spin:
		switch (((LPNMHDR)lParam)->code)
		{
		case UDN_DELTAPOS:
			OnSpinDeltaUpDownEx(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1, LOWORD(wParam) + 2);
		}
		break;
	case CI_MaxQty_Spin:
	case CI_MinQty_Spin:
	case CI_OrderQty_Spin:
		switch (((LPNMHDR)lParam)->code)
		{
		case UDN_DELTAPOS:
			OnSpinDeltaUpDownNormal(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1);
		}
		break;
	case CI_OrderQtyRate_Spin:
		switch (((LPNMHDR)lParam)->code)
		{
		case UDN_DELTAPOS:
			OnSpinDeltaUpDownRate(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1);
		}
		break;
	case CI_OverPoint_Spin:
		switch (((LPNMHDR)lParam)->code)
		{
		case UDN_DELTAPOS:
			OnSpinDeltaUpDownNormal(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1, MPoint_Max, MPoint_Min);
		}
		break;
	}
}

void CMousewin::OnCommond(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case CI_UserNo_Combox:
		if (HIWORD(wParam) == CBN_KILLFOCUS)
			OnKillFocusUserNo(CI_UserNo_Combox);
		else if (HIWORD(wParam) == CBN_SELCHANGE)
			OnSelChangeUserNo(CI_UserNo_Combox);
		else if (HIWORD(wParam) == CBN_EDITCHANGE)
			OnEditChangeUserNo(CI_UserNo_Combox);
		else if (HIWORD(wParam) == CBN_EDITUPDATE)
			m_ComboxUserNo.OnCbnEditupdate();
		break;
	case CI_UserNo_Combox2:
		if (HIWORD(wParam) == CBN_KILLFOCUS)
		{
			OnKillFocusUserNo(CI_UserNo_Combox2);
			UpdateShowText(CI_UserNo_Edit2, false);
		}
		else if (HIWORD(wParam) == CBN_SELCHANGE)
			OnSelChangeUserNo(CI_UserNo_Combox2);
		else if (HIWORD(wParam) == CBN_EDITCHANGE)
			OnEditChangeUserNo(CI_UserNo_Combox2);
		else if (HIWORD(wParam) == CBN_EDITUPDATE)
			m_ComboxUserNo2.OnCbnEditupdate();
		else if (HIWORD(wParam) == CBN_SETFOCUS)
			UpdateShowText(CI_UserNo_Edit2, true);
		break;
    case CI_UserNo_Combox3:
		if (HIWORD(wParam) == CBN_KILLFOCUS)
		{
			OnKillFocusUserNo(CI_UserNo_Combox3);
			UpdateShowText(CI_UserNo_Edit3, false);
		}
        else if (HIWORD(wParam) == CBN_SELCHANGE)
            OnSelChangeUserNo(CI_UserNo_Combox3);
        else if (HIWORD(wParam) == CBN_EDITCHANGE)
            OnEditChangeUserNo(CI_UserNo_Combox3);
        else if (HIWORD(wParam) == CBN_EDITUPDATE)
            m_ComboxUserNo3.OnCbnEditupdate();
		else if (HIWORD(wParam) == CBN_SETFOCUS)
			UpdateShowText(CI_UserNo_Edit3,true);
        break;
    case CI_UserNo_Combox4:
		if (HIWORD(wParam) == CBN_KILLFOCUS)
		{
			OnKillFocusUserNo(CI_UserNo_Combox4);
			UpdateShowText(CI_UserNo_Edit4, false);
		}
        else if (HIWORD(wParam) == CBN_SELCHANGE)
            OnSelChangeUserNo(CI_UserNo_Combox4);
        else if (HIWORD(wParam) == CBN_EDITCHANGE)
            OnEditChangeUserNo(CI_UserNo_Combox4);
        else if (HIWORD(wParam) == CBN_EDITUPDATE)
            m_ComboxUserNo4.OnCbnEditupdate();
		else if (HIWORD(wParam) == CBN_SETFOCUS)
			UpdateShowText(CI_UserNo_Edit4, true);
        break;
	case CI_ValidStyle_Button:
		if (HIWORD(wParam) == BN_CLICKED)
			OnButtonValidStyle();		
		break;
	case CI_Ok_Button:
		if (HIWORD(wParam) == BN_CLICKED)
			OnOkSendOrder();
		break;
	case CI_Modify_Button:
		if (HIWORD(wParam) == BN_CLICKED)
			OnModifySendOrder();
		break;
	case CI_Cancel_Button:
		if (HIWORD(wParam) == BN_CLICKED)
			OnCancel();
		break;
	case CI_OrderPrice_Static_Cal:
		if (HIWORD(wParam) == BN_CLICKED)
			OnButtonCalFlag(CI_OrderPrice_Static_Cal);
		break;
	case CI_TriggerPrice_Static_Cal:
		if (HIWORD(wParam) == BN_CLICKED)
			OnButtonCalFlag(CI_TriggerPrice_Static_Cal);
		break;
	case CI_AutoDirect_Check:
        if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
        {
            if (m_CheckAutoDirect.GetCheck() == BST_CHECKED)
            {//自动开平与移仓操作冲突 只能勾选一个
                m_CheckMovePosition.SetCheck(BST_UNCHECKED);
            }
            OnButtonAutoDirect();
        }
		break;
	case CI_FastOrder_Check:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonFastOrder();
		break;
	case CI_Advanced_Check:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonSpreadAdvanced();
		break;
	case CI_TimeValid_Check:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonSpreadTimer();
		break;
	case CI_AddOne_Check:
	case CI_AssistOrder_Check:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			UpdatePropertyCfg();
		break;
    case CI_Right_Cancel_Check:
        if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
        {
            OnCheckRightCancel();
        }
        break;
    case CI_CoverOpen_Check:
        if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
        {
            UpdatePropertyCfg();
        }
        break;
    case CI_Condition_Check:
    {
        if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
        {
            LayOut();
        }
        break;
    }
    case CI_MovePosition_Check:
    {
        if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
        {
            if (m_CheckMovePosition.GetCheck() == BST_CHECKED)
            {
                m_CheckAutoDirect.SetCheck(BST_UNCHECKED);
            }
            m_cOrderBtn.SetBShitPosition(m_CheckMovePosition.GetCheck() == BST_CHECKED);
            LayOut();
        }
        break;
    }
	case CI_ContractNo_Edit:
		if (HIWORD(wParam) == EN_CHANGE)
			OnSelChangeCode(LOWORD(wParam));
		else if (HIWORD(wParam) == EN_KILLFOCUS)
			OnKillFocusCode(LOWORD(wParam));
		break;
	case CI_OrderPrice_Edit:
		if (HIWORD(wParam) == EN_CHANGE)
			OnSelChangeEditPrice(CI_OrderPrice_Edit);
		else if (HIWORD(wParam) == EN_SETFOCUS)
			OnSetFocusEditPrice(CI_OrderPrice_Edit);
		break;
	case CI_OrderPrice_Edit_Mole:
		if (HIWORD(wParam) == EN_CHANGE)
			OnSelChangeEditPrice(CI_OrderPrice_Edit_Mole);
		break;
	case CI_OverPoint_Edit:
		if (HIWORD(wParam) == EN_CHANGE)
			OnSelChangeEditPoint();
		break;
	case CI_SellPrice_Edit:
		if (HIWORD(wParam) == EN_SETFOCUS)
			OnSetFocusEditPrice(CI_SellPrice_Edit);
		break;
	case CI_TriggerPrice_Edit:
		if (HIWORD(wParam) == EN_SETFOCUS)
			OnSetFocusEditPrice(CI_TriggerPrice_Edit);
		break;
	case CI_OrderSellPrice_Static:
		if (HIWORD(wParam) == BN_CLICKED)
			OnButtonQuotePrice(CI_OrderSellPrice_Static);
		break;
	case CI_OrderBuyPrice_Static:
		if (HIWORD(wParam) == BN_CLICKED)
			OnButtonQuotePrice(CI_OrderBuyPrice_Static);
		break;
	case CI_OrderQty_Edit:
		if (HIWORD(wParam) == EN_KILLFOCUS)
			OnKillFocusOrderQty();
		else if (HIWORD(wParam) == EN_SETFOCUS)
			OnSetFocusOrderQty();			
		else if (HIWORD(wParam) == EN_UPDATE)
			OnUpdateOrderQty();	
		break;
	case CI_MaxQty_Edit:
	case CI_MinQty_Edit:
		//if (HIWORD(wParam) == EN_KILLFOCUS || HIWORD(wParam) == EN_UPDATE)
		//{
		//	if (IsWindowVisible(m_cOrderQtyWin.GetHwnd()))
		//		::ShowWindow(m_cOrderQtyWin.GetHwnd(), SW_HIDE);
		//}
		//else if (HIWORD(wParam) == EN_SETFOCUS)
		//	OnPopQtySelectWnd(LOWORD(wParam));
		break;
	case CI_UserNo_Lock:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonUserLock();
		break;
	case CI_ContractNo_Lock:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonContractLock();
		break;
	case CI_Head_Search:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonSearch();
		break;
	case CI_UserNo_Sync:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnButtonUserSync();
		break;
	case CI_NoteInfo_Edit:
		if (HIWORD(wParam) == EN_CHANGE)
			OnEditChangeNoteInfo();
		break;	
	case CI_StopLoss_Edit:
	case CI_StopProfit_Edit:
		if (HIWORD(wParam) == EN_SETFOCUS)
			OnSetFocusEditPrice(LOWORD(wParam));
		else if (HIWORD(wParam) == EN_KILLFOCUS)
			SetShowTips(L"");
		break;
	case CI_ContractNo_Static:
		if (HIWORD(wParam) == BN_CLICKED)
			OnPopCodeSelectWnd();
		break;
	case CI_UserNo_Static:
		if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			OnClickUserStatic();
		break;
	default:
		break;
	}
}
void CMousewin::OnSize(WPARAM wParam, LPARAM lParam)			//拖拽处理
{
	if (IsWindow(m_cStopTip.GetHwnd()) && IsWindowVisible(m_cStopTip.GetHwnd()))
	{
		RECT rect;
		GetClientRect(m_Hwnd, &rect);
		MoveWindow(m_cStopTip.GetHwnd(), rect.left, rect.bottom - BottomTipHeight > 0 ? rect.bottom - BottomTipHeight : 0, rect.right - rect.left, rect.bottom > BottomTipHeight ? BottomTipHeight : rect.bottom, TRUE);
	}
	LayOut();
}
void CMousewin::OnPaint(WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(m_Hwnd, &ps);
	RECT itemRect,item;
	GetClientRect(m_Hwnd, &itemRect);
	GetClientRect(m_hControl[CI_Head_Search], &item);
	FillRect(hdc, &itemRect, m_hStaticBKBrush);
	if (itemRect.right > item.left)
	{
		HPEN hpen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
		SelectObject(hdc, hpen);
		MoveToEx(hdc, item.left, N_MOUSEHEAD_HEIGHT - 1, NULL);
		LineTo(hdc, itemRect.right, N_MOUSEHEAD_HEIGHT - 1);
		DeleteObject(hpen);
	}

	EndPaint(m_Hwnd, &ps);
}
void CMousewin::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int nID = GetWindowLong(GetFocus(), GWL_ID);
	switch (nID)
	{
	case CI_OrderPrice_Edit:
	case CI_SellPrice_Edit:
	case CI_TriggerPrice_Edit:
		OnSpinDeltaUpDownEx(GET_WHEEL_DELTA_WPARAM(wParam), nID, nID + 3);
		break;
	case CI_OrderPrice_Edit_Mole:
	case CI_SellPrice_Edit_Mole:
	case CI_TriggerPrice_Edit_Mole:
		OnSpinDeltaUpDownEx(GET_WHEEL_DELTA_WPARAM(wParam), nID - 3, nID);
		break;
	case CI_OrderQty_Edit:
	case CI_MaxQty_Edit:
	case CI_MinQty_Edit:
		OnSpinDeltaUpDownNormal(GET_WHEEL_DELTA_WPARAM(wParam), nID);
		break;
	case CI_ContractNo_Edit:
		OnWheelCodeList(nID,GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case CI_StopLoss_Edit:
	case CI_StopProfit_Edit:
		OnSpinDeltaUpDownEx(GET_WHEEL_DELTA_WPARAM(wParam), nID, nID + 3);
		break;
	case CI_StopLoss_Edit_Mole:
	case CI_StopProfit_Edit_Mole:
		OnSpinDeltaUpDownEx(GET_WHEEL_DELTA_WPARAM(wParam), nID - 3, nID);
		break;
	case CI_OverPoint_Edit:
		OnSpinDeltaUpDownNormal(GET_WHEEL_DELTA_WPARAM(wParam), nID, MPoint_Max, MPoint_Min);
		break;
	}
	return;
}

void CMousewin::OnNcDestory()
{
	vector<CMousewin*>::iterator iter = find(g_MouseVec.begin(), g_MouseVec.end(), this);
	if (iter != g_MouseVec.end())
	{
		g_MouseVec.erase(iter);
		delete this;
	}
}
	
void CMousewin::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	int id = (int)wParam;
	DRAWITEMSTRUCT *pDraw = (DRAWITEMSTRUCT*)(lParam);
	switch (id)
	{
	case CI_UserNo_Combox:
		m_ComboxUserNo.DrawItem(pDraw);
		break;
	case CI_UserNo_Combox2:
		m_ComboxUserNo2.DrawItem(pDraw);
		break;
    case CI_UserNo_Combox3:
        m_ComboxUserNo3.DrawItem(pDraw);
        break;
    case CI_UserNo_Combox4:
        m_ComboxUserNo4.DrawItem(pDraw);
        break;
	}
}
	
void CMousewin::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	int id = (int)wParam;
	MEASUREITEMSTRUCT*pMeasure = (MEASUREITEMSTRUCT*)(lParam);
	switch (id)
	{
	case CI_UserNo_Combox:
		m_ComboxUserNo.MeasureItem(pMeasure);
		break;
	case CI_UserNo_Combox2:
		m_ComboxUserNo2.MeasureItem(pMeasure);
		break;
    case CI_UserNo_Combox3:
        m_ComboxUserNo3.MeasureItem(pMeasure);
        break;
    case CI_UserNo_Combox4:
        m_ComboxUserNo4.MeasureItem(pMeasure);
        break;
	}
}

void CMousewin::OnMouseMove()
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_Hwnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme) == TRUE;
	}
}
	
void CMousewin::OnMouseLeave()
{
	m_bTracking = FALSE;
}

void CMousewin::OnMouseHover()
{
	//if (IsWindowVisible(m_cOrderQtyWin.GetHwnd()))
	//	::ShowWindow(m_cOrderQtyWin.GetHwnd(), SW_HIDE);
	if (IsWindowVisible(m_cCodeWin.GetHwnd()))
		::ShowWindow(m_cCodeWin.GetHwnd(), SW_HIDE);
	if (IsWindowVisible(m_cValidTypeWin.GetHwnd()))
		::ShowWindow(m_cValidTypeWin.GetHwnd(), SW_HIDE);
	
	m_StaticPriceSwitch.HidePopWin();
} 

void CMousewin::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	SetFocus(m_Hwnd);
}

//////////////////////////////////////////////////////////////////////////自定义消息处理函数
//行情数据处理
void CMousewin::OnDealQuote()
{
	if (IsSpreadLayOut())
	{
        SSpreadContract pQuote;
        if (g_pStarApi && g_pStarApi->GetSpreadInfo(m_sQuoteContractId.c_str(), &pQuote, 1, false) > 0)
        {
            if (!pQuote.SnapShot)
            {
                return;
            }
            SQtyType nQtyAsk = 0, nQtyBid = 0;
            double dPriceAsk = 0, dPriceBid = 0;
            nQtyAsk = CConvert::GetQtyBySnapShot(pQuote.SnapShot, S_FID_BESTASKQTY);
            dPriceAsk = CConvert::GetPriceBySnapShot(pQuote.SnapShot, S_FID_BESTASKPRICE);
            nQtyBid = CConvert::GetQtyBySnapShot(pQuote.SnapShot, S_FID_BESTBIDQTY);
            dPriceBid = CConvert::GetPriceBySnapShot(pQuote.SnapShot, S_FID_BESTBIDPRICE);

            TMouBuySellStrW wText = { 0 };
            if (nQtyAsk)
                swprintf_s(wText, L"%s%s  %lld", LoadResWchar_t(IDS_SELLTEXT), LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dPriceAsk, m_CommodityInfo)).c_str(), nQtyAsk);
            else
                swprintf_s(wText, L"%s", LoadResWchar_t(IDS_SELLTEXT));
            SetWindowText(m_hControl[CI_OrderSellPrice_Static], wText);
            if (nQtyBid)
                swprintf_s(wText, L"%s%s  %lld", LoadResWchar_t(IDS_BUYTEXT), LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dPriceBid, m_CommodityInfo)).c_str(), nQtyBid);
            else
                swprintf_s(wText, L"%s", LoadResWchar_t(IDS_BUYTEXT));
            SetWindowText(m_hControl[CI_OrderBuyPrice_Static], wText);
        }
	}
	else
	{
		SContract *pQuote = NULL;
		if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteContractId.c_str(), &pQuote, 1, false))
		{
			double dSellPrice = 0, dBuyPrice = 0, dLastPrice = 0;
			SQtyType nSellQty = 0, nBuyQty = 0;
			dLastPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
			dBuyPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTBIDPRICE);
			dSellPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTASKPRICE);
			nBuyQty = CConvert::GetQtyByContract(pQuote, S_FID_BESTBIDQTY);
			nSellQty = CConvert::GetQtyByContract(pQuote, S_FID_BESTASKQTY);

			TMouBuySellStrW wText = { 0 };
			if (nSellQty)
				swprintf_s(wText, L"%s%s  %lld", LoadResWchar_t(IDS_SELLTEXT), LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dSellPrice, m_CommodityInfo)).c_str(), nSellQty);
			else
				swprintf_s(wText, L"%s", LoadResWchar_t(IDS_SELLTEXT));
			SetWindowText(m_hControl[CI_OrderSellPrice_Static], wText);

			if (nBuyQty)
				swprintf_s(wText, L"%s%s  %lld", LoadResWchar_t(IDS_BUYTEXT), LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dBuyPrice, m_CommodityInfo)).c_str(), nBuyQty);
			else
				swprintf_s(wText, L"%s", LoadResWchar_t(IDS_BUYTEXT)); 
			SetWindowText(m_hControl[CI_OrderBuyPrice_Static], wText);

			if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)) != 0)//买卖按钮显示跟盘价
			{
				double doverprice = 0;

				if (IsWindowVisible(m_hControl[CI_OverPoint_Edit]))
				{
					TMouQtyStrW wOver = { 0 };
					GetWindowTextW(m_hControl[CI_OverPoint_Edit], wOver, sizeof(TMouQtyStrW) / sizeof(wchar_t));
					int nOver = _wtoi(wOver);
					if (m_CommodityInfo.bIfFractionPrice)
						doverprice = nOver*(m_CommodityInfo.dMiniChangePrice / m_CommodityInfo.iPrecision);
					else
						doverprice = nOver*m_CommodityInfo.dMiniChangePrice;
				}

				if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1154)) == 0)
				{
					if (!CConvert::DoubleSameValue(doverprice, 0, FLOAT_SAME_ERRORDIFF))
					{
						if (!CConvert::DoubleSameValue(dSellPrice, 0, FLOAT_SAME_ERRORDIFF))
							dSellPrice += doverprice;
						if (!CConvert::DoubleSameValue(dBuyPrice, 0, FLOAT_SAME_ERRORDIFF))
							dBuyPrice -= doverprice;
					}
					TMouFormatPriceStrW wPrice[2] = { 0 };
					swprintf_s(wPrice[0], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dSellPrice, m_CommodityInfo)).c_str());
					swprintf_s(wPrice[1], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dBuyPrice, m_CommodityInfo)).c_str());
					m_cOrderBtn.UpdatePrice(wPrice[0], wPrice[1]);
				}
				else if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1155)) == 0)
				{
					if (!CConvert::DoubleSameValue(doverprice, 0, FLOAT_SAME_ERRORDIFF))
					{
						if (!CConvert::DoubleSameValue(dBuyPrice, 0, FLOAT_SAME_ERRORDIFF))
							dBuyPrice += doverprice;
						if (!CConvert::DoubleSameValue(dSellPrice, 0, FLOAT_SAME_ERRORDIFF))
							dSellPrice -= doverprice;
					}
					TMouFormatPriceStrW wPrice[2] = { 0 };
					swprintf_s(wPrice[0], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dBuyPrice, m_CommodityInfo)).c_str());
					swprintf_s(wPrice[1], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dSellPrice, m_CommodityInfo)).c_str());
					m_cOrderBtn.UpdatePrice(wPrice[0], wPrice[1]);
				}
				else if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1122)) == 0)
				{
					if (!CConvert::DoubleSameValue(dLastPrice, 0, FLOAT_SAME_ERRORDIFF) && !CConvert::DoubleSameValue(doverprice, 0, FLOAT_SAME_ERRORDIFF))
					{
						TMouFormatPriceStrW wPrice[2] = { 0 };
						swprintf_s(wPrice[0], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dLastPrice + doverprice, m_CommodityInfo)).c_str());
						swprintf_s(wPrice[1], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dLastPrice - doverprice, m_CommodityInfo)).c_str());
						m_cOrderBtn.UpdatePrice(wPrice[0], wPrice[1]);
					}
					else
					{
						swprintf_s(wText, L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dLastPrice, m_CommodityInfo)).c_str());
						m_cOrderBtn.UpdatePrice(wText, wText);
					}
				}
				else
				{
					swprintf_s(wText, L"");
					m_cOrderBtn.UpdatePrice(wText, wText);
				}
			}
		}
		else
		{
			TMouFormatPriceStrW wText = { 0 };
			swprintf_s(wText, L"%s", LoadResWchar_t(IDS_SELLTEXT));
			SetWindowText(m_hControl[CI_OrderSellPrice_Static], wText);
			swprintf_s(wText, L"%s", LoadResWchar_t(IDS_BUYTEXT));
			SetWindowText(m_hControl[CI_OrderBuyPrice_Static], wText);
			if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)) != 0)
				m_cOrderBtn.UpdatePrice(L"", L"");
		}
		m_OrderList.UpdatePrice();
	}	
}
void CMousewin::OnDealQuoteOptionTarget()//处理期权标的行情更新
{
	if (!m_sOptionTarget.empty() && m_OptionInfo.cOptionType != otNone)
	{
		SContract *pQuote = NULL;
		if (g_pStarApi && g_pStarApi->GetContractData("", m_sOptionTarget.c_str(), &pQuote, 1, false))
		{
			SPriceType dLastPrice = 0;
			dLastPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
			if (pQuote->SnapShot && pQuote->SnapShot->Data[S_FID_LASTPRICE].FidAttr &&!CConvert::DoubleSameValue(dLastPrice,0,FLOAT_SAME_ERRORDIFF))
			{		
				SPriceStrType str = { 0 };
                g_pStarApi->FormatPrice(dLastPrice, pQuote->Commodity, str, false, false);
				wstring sShow = LoadResWchar_t(IDS_TargetPrice);
				sShow += LoadRC::ansi_to_unicode(str);
				sShow += L" ";
				
				int n = 0;
				if (CConvert::DoubleSameValue(m_OptionInfo.dStrikePrice, dLastPrice, FLOAT_SAME_ERRORDIFF))
					n = 0;
				else if (m_OptionInfo.dStrikePrice > dLastPrice)
					n = m_OptionInfo.cOptionType == otCall ? -1 : 1;
				else
					n = m_OptionInfo.cOptionType == otCall ? 1 : -1;
				switch (n)//虚实值
				{
				case 0:
					sShow += LoadResWchar_t(IDS_Equal);
					break;
				case 1:
					sShow += LoadResWchar_t(IDS_Real);
					break;
				case -1:
					sShow += LoadResWchar_t(IDS_Virtual);
					break;
				default:
					return;
				}
				
				SetWindowText(m_hControl[CI_ShowTip_Option], sShow.c_str());
			}
			else
				SetWindowText(m_hControl[CI_ShowTip_Option], L"");
		}
	}
	else
		SetWindowText(m_hControl[CI_ShowTip_Option], L"");
}
//持仓数据处理
void CMousewin::OnDealPosition()
{
	UpdatePositionQty();//持仓变化：刷新
	if (!IsSpreadLayOut())
		FormatDisplayQtyAndFreeze(true, false);//刷新
}
void CMousewin::OnDealPreFreeze(int nDirect)//处理预冻结
{
	CMyLock::Guard lock(m_lock);
	if (IsSpreadLayOut())
		return;
	if (nDirect == dBuy)
	{
		TCurrency tcurrency;
		memcpy_s(&tcurrency, sizeof(TCurrency), m_BuyPreFreezetemp.pCurrency, sizeof(TCurrency));
		const TCurrency* tCur = g_pTradeData->GetCurrency(tcurrency.CurrencyNo, tcurrency.Sign);
		if (tCur == NULL)
			return;
		if (strcmp(m_sUserNo[0].c_str(), m_BuyPreFreezetemp.UserNo))
			return;
		string sCode = "";
		CConvert::TContractKeyToTContractNo((TContractKey&)m_BuyPreFreezetemp, sCode);
		if (sCode != m_sCode)
			return;
		
		uint nQty = 0;
		if (!GetOrderQty(CI_OrderQty_Edit, nQty, false))
			return;
		else if (nQty != m_BuyPreFreezetemp.OrderQty)
			return;

		memcpy_s(&m_LastBuyFreeze, sizeof(TPreFreeze), &m_BuyPreFreezetemp, sizeof(TPreFreeze));
		ShowOrderFrozen(&m_BuyPreFreezetemp);
	}
	else if (nDirect == dSell)
	{
		TCurrency tcurrency;
		memcpy_s(&tcurrency, sizeof(TCurrency), m_SellPreFreezetemp.pCurrency, sizeof(TCurrency));
		const TCurrency* tCur = g_pTradeData->GetCurrency(tcurrency.CurrencyNo, tcurrency.Sign);
		if (tCur == NULL)
			return;
		if (strcmp(m_sUserNo[0].c_str(), m_SellPreFreezetemp.UserNo))
			return;
		string sCode = "";
		CConvert::TContractKeyToTContractNo((TContractKey&)m_SellPreFreezetemp, sCode);
		if (sCode != m_sCode)
			return;
		uint nQty = 0;
		if (!GetOrderQty(CI_OrderQty_Edit, nQty, false))
			return;
		else if (nQty != m_SellPreFreezetemp.OrderQty)
			return;
		memcpy_s(&m_LastSellFreeze, sizeof(TPreFreeze), &m_SellPreFreezetemp, sizeof(TPreFreeze));
		ShowOrderFrozen(&m_SellPreFreezetemp);
	}	
}
void CMousewin::OnDealTradableQty(int nDirect)							//处理可开可冲
{
	CMyLock::Guard lock(m_lock);
	//保存最后一次数据
	if (IsSpreadLayOut())
		return;
	if (nDirect == 0)
	{
		if (m_BuyTradableQtytemp.OpenQty < 0 || m_BuyTradableQtytemp.OpenQty>100000000 || m_BuyTradableQtytemp.CoverQty < 0 || m_BuyTradableQtytemp.CoverQty>10000000)
			return;
		memcpy_s(&m_LastBuyQty, sizeof(TTradableQty), &m_BuyTradableQtytemp, sizeof(TTradableQty));
		//显示数量
		ShowCanOrderVol(&m_BuyTradableQtytemp);
	}	
	else if (nDirect == 1)
	{
		if (m_SellTradableQtytemp.OpenQty < 0 || m_SellTradableQtytemp.OpenQty>100000000 || m_SellTradableQtytemp.CoverQty < 0 || m_SellTradableQtytemp.CoverQty>10000000)
			return;
		memcpy_s(&m_LastSellQty, sizeof(TTradableQty), &m_SellTradableQtytemp, sizeof(TTradableQty));
		//显示数量
		ShowCanOrderVol(&m_SellTradableQtytemp);
	}
}
//发送
void CMousewin::OnOkSendOrder()
{
	TSendOrder sendorder;
	CConvert::DefaultSendOrder(sendorder);
	FillAndSendOrder(sendorder);
}
//发送改单
void CMousewin::OnModifySendOrder()
{
	if (m_nCurClickItem.nCurHeadType == MHI_Advanced&&m_nCurClickItem.nCurChildType == CHA_ExecAban)//行权弃权
	{
		m_bExecute = true;
		SendOrder(NULL);
		return;
	}
    else if (m_nCurClickItem.nCurHeadType == MHI_Advanced&&m_nCurClickItem.nCurChildType == CHA_AutoClose)//行权弃权
    {
        m_bOptionAutoClose = true;
        SendOrder(NULL);
        return;
    }
    else if (m_nCurClickItem.nCurHeadType == MHI_Advanced&&m_nCurClickItem.nCurChildType == CHA_Enquiry)//询价
    {
        MsgOrderInfo info;
        info.Direct = dBuy;
        SendOrder(&info);
        return;
    }
	else if (IsSpreadApplyLayOut()|| IsHedgeApplyLayOut())//套利申请 套保申请
	{
		MsgOrderInfo info;
		info.Direct = dBuy;
		SendOrder(&info);
		return;
	}
	else//定单修改
	{
		if (-1 == m_OrderModifyParam.OrderID)	return;
		if (!FillOrder(m_OrderModifyParam))
			return;

		if (m_OrderModifyParam.OrderQty < m_OrderModifyParam.MatchQty)
		{
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_OrderQtyInvalid), LoadResWchar_t(IDS_TIP), MB_OK);
			return;
		}
		bool bDeal = false;
		if (!m_bBatchModifyOrderOper)
			bDeal = g_pCommonModule->ModifyOrder(m_OrderModifyParam);
		else
			bDeal = g_pCommonModule->BatchModify(&m_OrderModifyParam);

		if (bDeal)
			OnDealModifyRecover();
	}
}
void CMousewin::OnDestroy()
{
    for (int i = 0; i < 4; i++)
    {
        m_sUserNo[i].clear();				//资金账号
    }
	
	m_sValidtype.clear();			//有效类型字符串
	m_sCode.clear();				//合约编码
    m_sSpreadCode.clear();
	m_UserInfo.clear();				//资金账户信息清空
	m_sPriceLast = 0;				//最新价
	m_sPriceMax = 0;				//涨停价
	m_sPriceMin = 0;				//跌停价
	m_sPriceBid = 0;				//买价
	m_sPriceAsk = 0;				//卖价

	//弹出窗口
	DestroyWindow(m_cValidTypeWin.GetHwnd());
	DestroyWindow(m_cCodeWin.GetHwnd());
	DestroyWindow(m_cOrderPriceWin.GetHwnd());
	
	//子类化窗口
	DestroyWindow(m_hControl[CI_Modify_Button]);
	DestroyWindow(m_hControl[CI_Cancel_Button]);

	m_gTabs.clear();//控件子类化 窗口处理过程

}
void CMousewin::OnCancel()
{
	if (m_nCurClickItem.nCurHeadType == MHI_Advanced&&m_nCurClickItem.nCurChildType == CHA_ExecAban)//行权弃权
	{
		m_bExecute = false;
		SendOrder(NULL);
		return;
	}
    else if (m_nCurClickItem.nCurHeadType == MHI_Advanced&&m_nCurClickItem.nCurChildType == CHA_AutoClose)//自动对冲
    {
        m_bOptionAutoClose = false;
        SendOrder(NULL);
        return;
    }
    else if (m_nCurClickItem.nCurHeadType == MHI_Advanced&&m_nCurClickItem.nCurChildType == CHA_Enquiry)//询价
    {
        MsgOrderInfo info;
        info.Direct = dSell;
        SendOrder(&info);
        return;
    }
	else if (IsSpreadApplyLayOut() || IsHedgeApplyLayOut())//套利申请 套保申请
	{
		MsgOrderInfo info;
		info.Direct = dSell;
		SendOrder(&info);
		return;
	}
	else
		OnDealModifyRecover();
}
void CMousewin::OnOK()
{
	SendOrder(NULL);
}
//////////////////////////////////////////////////////////////////////////界面获取数据
bool  CMousewin::GetOrderUser(TMouUserNoStrA(&UserNo), bool bShowMsg)
{
	string sText = GetWindowTextAnsic(m_hControl[CI_UserNo_Combox]);
	if (m_nCurClickItem.nCurHeadType != MHI_BatchOrder)
	{
		if (m_UserInfo.size()==0)
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_NoUserLogin), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
		else if (m_UserInfo.size() == 1)
		{
			map<string, sUserInfor>::iterator it1 = m_UserInfo.begin();
			m_sUserNo[0] = it1->first;
			m_sSign[0] = it1->second.sSign;
			m_cUserType = it1->second.cUserType;
			memcpy(m_CommodityInfo.Sign, m_sSign[0].c_str(), sizeof(m_CommodityInfo.Sign));
		}
		else 
		{
			map<string, sUserInfor>::iterator it1 = m_UserInfo.find(sText);
			if (it1 != m_UserInfo.end())
			{
				m_sUserNo[0] = it1->first;
				m_sSign[0] = it1->second.sSign;
				m_cUserType = it1->second.cUserType;
				memcpy(m_CommodityInfo.Sign, m_sSign[0].c_str(), sizeof(m_CommodityInfo.Sign));
			}
			else
			{
				if (bShowMsg)
					MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1107), LoadResWchar_t(IDS_TIP), NULL);
				return false;
			}	
		}
		const TUserInfo *pInfo = g_pTradeData->GetUser(m_sUserNo[0].c_str(), m_sSign[0].c_str());
		//判断该账户是否登陆
		if (pInfo && pInfo->pLogin)
		{
			if (pInfo->pLogin->Logined == bYes)//登录成功
			{
				strcpy_s(UserNo, m_sUserNo[0].c_str());
				return true;
			}
			else
			{
				if (bShowMsg)
					MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1107), LoadResWchar_t(IDS_TIP), NULL);
				return false;
			}
		}
		else
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1107), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
	}
	else
	{
		//交易员该功能屏蔽不显示
		if (m_bTradeLogined)
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_TraderForbid), LoadResWchar_t(IDS_TIP), MB_OK);
			return false;
		}
		if (m_UserInfo.size()<=1)
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_InvalidBatchGroup), LoadResWchar_t(IDS_TIP), MB_OK);
			return false;
		}
		map<string, BatchGroupInfo>::iterator iter = m_batchGroupVec.find(sText);
		if (iter == m_batchGroupVec.end())
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_InvalidBatchGroup), LoadResWchar_t(IDS_TIP), MB_OK);
			return false;
		}
		strncpy_s(UserNo, sText.c_str(), _TRUNCATE);
		return true;
	}
}
bool CMousewin::GetOrderUser2(TMouUserNoStrA(&UserNo), int index, bool bShowMsg)
{
    string sText = "";
    if (index == 1)
    {
        sText = GetWindowTextAnsic(m_hControl[CI_UserNo_Combox2]);
    }
    else if (index == 2)
    {
        sText = GetWindowTextAnsic(m_hControl[CI_UserNo_Combox3]);
    }
    else if (index == 3)
    {
        sText = GetWindowTextAnsic(m_hControl[CI_UserNo_Combox4]);
    }
	
	if (m_UserInfo.size() == 0)
	{
		if (bShowMsg)
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1107), LoadResWchar_t(IDS_TIP), NULL);
		return false;
	}
	else if (m_UserInfo.size() == 1)
	{
		map<string, sUserInfor>::iterator it1 = m_UserInfo.begin();
		m_sUserNo[index] = it1->first;
		m_sSign[index] = it1->second.sSign;
	}
	else
	{
		map<string, sUserInfor>::iterator it1 = m_UserInfo.find(sText);
		if (it1 != m_UserInfo.end())
		{
			m_sUserNo[index] = it1->first;
			m_sSign[index] = it1->second.sSign;
		}
		else
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1107), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
	}
	const TUserInfo *pInfo = g_pTradeData->GetUser(m_sUserNo[index].c_str(), m_sSign[index].c_str());
	//判断该账户是否登陆过
	if (pInfo && pInfo->pLogin)
	{
		if (pInfo->pLogin->Logined == bYes)//登录成功
		{
			strcpy_s(UserNo, m_sUserNo[index].c_str());
			return true;
		}
		else
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1107), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
	}
	else
	{
		if (bShowMsg)
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1107), LoadResWchar_t(IDS_TIP), NULL);
		return false;
	}
}
bool CMousewin::GetOrderCode(TContractKey& key, string &sCode,int nID, bool bShowMsg)
{
	if (IsWindowVisible(m_hControl[nID]))
	{
		string sText = GetWindowTextAnsic(m_hControl[nID]);
		if (!CConvert::TContractNoToTContractKey(sText.c_str(), key) || !CConvert::IsValidTradeCommodity(key))
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1109), LoadResWchar_t(IDS_TIP), NULL);
			sCode = "";
			return false;
		}
		else
		{
			sCode = sText;
			return true;
		}	
	}
	else
	{
		if (bShowMsg)
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1109), LoadResWchar_t(IDS_TIP), NULL);
		sCode = "";
		return false;
	}		
}
//获取套利合约
bool CMousewin::GetSpreadOrderCode(SSpreadContract &sQuote, TSpreadSendOrder& sOrder,bool bShowMsg)
{
    TContractKey key[4];
    memset(&key, 0, sizeof(TContractKey) * 4);
    memcpy(key[0].Sign, sOrder.Sign[0], sizeof(key[0].Sign));
    memcpy(key[1].Sign, sOrder.Sign[1], sizeof(key[1].Sign));

    CConvert::SContractNoTypeToTradeContract(sOrder.Sign[0], sQuote.Contract[0]->ContractNo, key[0]);
	if (!CConvert::IsValidTradeCommodity(key[0]))
	{
		if (bShowMsg)
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_InValidCodeF), LoadResWchar_t(IDS_TIP), NULL);
		m_sSpreadCode = "";
		return false;
	}
    CConvert::SContractNoTypeToTradeContract(sOrder.Sign[1], sQuote.Contract[1]->ContractNo, key[1]);
    if (!CConvert::IsValidTradeCommodity(key[1]))
    {
        if (bShowMsg)
            MessageBox(m_Hwnd, LoadResWchar_t(IDS_InValidCodeS), LoadResWchar_t(IDS_TIP), NULL);
        m_sSpreadCode = "";
        return false;
    }
    sOrder.RateMode[0] = sQuote.RateMode[0];
    if (sQuote.ValidLegs > 2)
    {
        memcpy(key[2].Sign, sOrder.Sign[2], sizeof(key[2].Sign));
        CConvert::SContractNoTypeToTradeContract(sOrder.Sign[2], sQuote.Contract[2]->ContractNo, key[2]);
        if (!CConvert::IsValidTradeCommodity(key[2]))
        {
            if (bShowMsg)
                MessageBox(m_Hwnd, LoadResWchar_t(IDS_InValidCodeThird), LoadResWchar_t(IDS_TIP), NULL);
            m_sSpreadCode = "";
            return false;
        }
        sOrder.RateMode[1] = sQuote.RateMode[1];
        memcpy(&sOrder.Contract[2], &key[2], sizeof(TContractSingleKey));
    }
    if (sQuote.ValidLegs > 3)
    {
        memcpy(key[3].Sign, sOrder.Sign[3], sizeof(key[3].Sign));
        CConvert::SContractNoTypeToTradeContract(sOrder.Sign[3], sQuote.Contract[3]->ContractNo, key[3]);
        if (!CConvert::IsValidTradeCommodity(key[3]))
        {
            if (bShowMsg)
                MessageBox(m_Hwnd, LoadResWchar_t(IDS_InValidCodeFourth), LoadResWchar_t(IDS_TIP), NULL);
            m_sSpreadCode = "";
            return false;
        }
        sOrder.RateMode[2] = sQuote.RateMode[2];
        memcpy(&sOrder.Contract[3], &key[3], sizeof(TContractSingleKey));
    }

    strcpy_s(sOrder.ContractNo, sQuote.ContractNo);
    memcpy(&sOrder.Contract[0], &key[0], sizeof(TContractSingleKey));
    memcpy(&sOrder.Contract[1], &key[1], sizeof(TContractSingleKey));

    m_sSpreadCode = sQuote.ContractNo;
	
	return true;
}
bool  CMousewin::GetOrderPrice(int nID, int nMoleID, double &dPrice, TDirect td, bool bShowMsg, MPriceType type)
{
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE))!=0 && nID == CI_OrderPrice_Edit)
	{
		SContract *pQuote = NULL;
		if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteContractId.c_str(), &pQuote, 1, false))
		{
			double dSellPrice = 0, dBuyPrice = 0, dLastPrice = 0;
			dLastPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
			dBuyPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTBIDPRICE);
			dSellPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTASKPRICE);

			if (td == dBuy)
				dPrice = dSellPrice;
			else
				dPrice = dBuyPrice;

			if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1154))==0)
			{	
				if (td == dBuy)
					dPrice = dSellPrice;
				else
					dPrice = dBuyPrice;
			}
			else if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1155))==0)
			{
				if (td == dBuy)
					dPrice = dBuyPrice;
				else
					dPrice = dSellPrice;
			}
			else if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1122))==0)
				dPrice = dLastPrice;
		}
		if (CConvert::DoubleSameValue(dPrice,0,FLOAT_SAME_ERRORDIFF) && !m_CommodityInfo.bCanNegative)
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1074), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
		else
		{
			double doverprice = 0;
			TMouQtyStrW wOver = { 0 };
			GetWindowTextW(m_hControl[CI_OverPoint_Edit], wOver, sizeof(TMouQtyStrW)/sizeof(wchar_t));
			int nOver = _wtoi(wOver);
			if (m_CommodityInfo.bIfFractionPrice)
				doverprice = nOver*(m_CommodityInfo.dMiniChangePrice / m_CommodityInfo.iPrecision);
			else
				doverprice = nOver*m_CommodityInfo.dMiniChangePrice;
			dPrice = td == dBuy ? (dPrice + doverprice) : (dPrice - doverprice);
			return CheckOrderPrice(dPrice, bShowMsg, type);
		}		
	}
	else
		return GetDoublePrice(nID, nMoleID, dPrice, bShowMsg, type);		
}
//获取买卖委托价格
bool CMousewin::GetDoubleOrderPrice(double& dPriceBuy, double& dPriceSell)														
{
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)) != 0)
	{
		SContract *pQuote = NULL;
		if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteContractId.c_str(), &pQuote, 1, false))
		{
			double dSellPrice = 0, dBuyPrice = 0, dLastPrice = 0, dUp = 0, dDown = 0;;
			dLastPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
			dBuyPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTBIDPRICE);
			dSellPrice = CConvert::GetPriceByContract(pQuote, S_FID_BESTASKPRICE);
			dUp = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITUPPRICE);
			dDown = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITDOWNPRICE);

			if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1154)) == 0)
			{
				dPriceBuy = dSellPrice;
				dPriceSell = dBuyPrice;
			}
			else if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1155)) == 0)
			{
				dPriceBuy = dBuyPrice;
				dPriceSell = dSellPrice;
			}
			else if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_STRING1122)) == 0)
			{
				dPriceBuy = dLastPrice;
				dPriceSell = dLastPrice;
			}
			//超价点数
			TMouQtyStrW wOver = { 0 };
			GetWindowTextW(m_hControl[CI_OverPoint_Edit], wOver, sizeof(TMouQtyStrW) / sizeof(wchar_t));
			int nOver = _wtoi(wOver);
			double doverprice = 0;
			if (m_CommodityInfo.bIfFractionPrice)
				doverprice = nOver*(m_CommodityInfo.dMiniChangePrice / m_CommodityInfo.iPrecision);
			else
				doverprice = nOver*m_CommodityInfo.dMiniChangePrice;
			bool bret1 = false, bret2 = false;
			//涨跌停
			if (!CConvert::DoubleSameValue(dPriceBuy, 0, FLOAT_SAME_ERRORDIFF))
			{
				dPriceBuy += doverprice;
				if ((!CConvert::DoubleSameValue(dUp, 0, FLOAT_SAME_ERRORDIFF) && !CConvert::DoubleSameValue(dDown, 0, FLOAT_SAME_ERRORDIFF)))
				{
					dPriceBuy = min(dPriceBuy, dUp);
					dPriceBuy = max(dPriceBuy, dDown);
				}
				bret1 = true;
			}
			if (!CConvert::DoubleSameValue(dPriceSell, 0, FLOAT_SAME_ERRORDIFF))
			{
				dPriceSell -= doverprice;
				if ((!CConvert::DoubleSameValue(dUp, 0, FLOAT_SAME_ERRORDIFF) && !CConvert::DoubleSameValue(dDown, 0, FLOAT_SAME_ERRORDIFF)))
				{
					dPriceSell = min(dPriceSell, dUp);
					dPriceSell = max(dPriceSell, dDown);
				}
				bret2 = true;
			}
			return bret1&&bret2;
		}
		else
			return false;
	}
	else
	{
		GetDoublePrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, dPriceBuy, false, mptOrder);
		dPriceSell = dPriceBuy;
		SContract *pQuote = NULL;
		bool ret = false;
		if (!CConvert::DoubleSameValue(dPriceBuy, 0, FLOAT_SAME_ERRORDIFF))
		{
			double dUp = 0, dDown = 0;
			if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteContractId.c_str(), &pQuote, 1, false))
			{
				dUp = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITUPPRICE);
				dDown = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITDOWNPRICE);
				if ((!CConvert::DoubleSameValue(dUp, 0, FLOAT_SAME_ERRORDIFF) && !CConvert::DoubleSameValue(dDown, 0, FLOAT_SAME_ERRORDIFF)))
				{
					dPriceBuy = min(dPriceBuy, dUp);
					dPriceBuy = max(dPriceBuy, dDown);
					dPriceSell = dPriceBuy;
				}	
			}
			ret = true;
		}
		return ret;
	}
}
//获取套利触发价格
bool CMousewin::GetSpreadTrigPrice(double &dPrice, bool bShowMsg)			
{
	string sInt = GetWindowTextAnsic(m_hControl[CI_OrderPrice_Edit]);

	//未填价格
	if (sInt == "")
	{
		dPrice = 0;
		if (bShowMsg)
			PopPriceCheckMsg(mptTrig, metEmpty);
		return false;
	}

	dPrice = atof(sInt.c_str());

	//非零检测
	if (CConvert::DoubleSameValue(dPrice, 0, FLOAT_SAME_ERRORDIFF) && !m_CommodityInfo.bCanNegative)
	{
		dPrice = 0;
		if (bShowMsg)
			PopPriceCheckMsg(mptTrig, metZero);
		return false;
	}

	if (!CConvert::IsDiv(dPrice, m_CommodityInfo.dMiniChangePrice, pow(10,(-(10- m_CommodityInfo.iPrecision)))))
	{
		if (bShowMsg)
			PopPriceCheckMsg(mptTrig, metMinTick);
		return false;
	}
	else
		return true;
}
bool CMousewin::GetDoublePrice(int nID, int nMoleID, double &dPrice, bool bShowMsg, MPriceType type)
{
	string sInt = GetWindowTextAnsic(m_hControl[nID]);
	
	//未填价格
	if (sInt=="")
	{
		dPrice = 0;
		if (bShowMsg)
			PopPriceCheckMsg(type,metEmpty);
		return false;
	}
	
	if (m_CommodityInfo.bIfFractionPrice)
	{
		string sMole = GetWindowTextAnsic(m_hControl[nMoleID]);
		string sCal = GetWindowTextAnsic(m_hControl[nMoleID - 1]);
		if (sCal == "+")
			dPrice = atof(sInt.c_str()) + atof(sMole.c_str()) / m_CommodityInfo.iPrecision;
		else
			dPrice = atof(sInt.c_str()) - atof(sMole.c_str()) / m_CommodityInfo.iPrecision;

		if (!CConvert::IsDiv(atof(sMole.c_str()), m_CommodityInfo.dMiniChangePrice, FLOAT_SAME_ERRORDIFF))
		{
			if (bShowMsg)
				PopPriceCheckMsg(type, metMinTick);
			return false;
		}
	}
	else
		dPrice = atof(sInt.c_str());

	//非零检测
	if (CConvert::DoubleSameValue(dPrice, 0, FLOAT_SAME_ERRORDIFF)&& !m_CommodityInfo.bCanNegative)
	{
		dPrice = 0;
		if (bShowMsg)
			PopPriceCheckMsg(type,metZero);
		return false;
	}

	return CheckOrderPrice(dPrice, bShowMsg, type);
}
bool  CMousewin::SetOrderPrice(int nID, int nMoleID, double dPrice)
{
	if (m_CommodityInfo.bIfFractionPrice)
	{
		//把小数价格转为分数 denominator分母 		
		if (!m_CommodityInfo.bCanNegative)
		{
			int   iInteger = 0;
			double fDecimal = 0;							//分数的整数部分和小数部分
			iInteger = (int)dPrice;
			fDecimal = dPrice - (double)iInteger;
			int iMole = int(fDecimal*m_CommodityInfo.iPrecision+0.5);	
			SetWindowInt(m_hControl[nMoleID], iMole);
			SetWindowInt(m_hControl[nID], iInteger);
		}
		else
		{
			int   iInteger = 0;
			double fDecimal = 0;							//分数的整数部分和小数部分
			iInteger = (int)dPrice;
			if (dPrice<0)
			{
				fDecimal = dPrice - (double)iInteger;
				int iMole = int(fDecimal*m_CommodityInfo.iPrecision - 0.5);
				SetWindowInt(m_hControl[nMoleID], abs(iMole));
				TMouCalStrA sCal = { 0 };
				GetWindowTextA(m_hControl[nMoleID - 1], sCal, sizeof(TMouCalStrA));
				if (!strcmp(sCal,"+"))
					SetWindowTextA(m_hControl[nMoleID - 1], "-");
				SetWindowInt(m_hControl[nID], iInteger);
			}
			else
			{
				fDecimal = dPrice - (double)iInteger;
				int iMole = int(fDecimal*m_CommodityInfo.iPrecision + 0.5);
				SetWindowInt(m_hControl[nMoleID], iMole);
				TMouCalStrA sCal = { 0 };
				GetWindowTextA(m_hControl[nMoleID - 1], sCal, sizeof(TMouCalStrA));
				if (!strcmp(sCal, "-"))
					SetWindowTextA(m_hControl[nMoleID - 1], "+");
				SetWindowInt(m_hControl[nID], iInteger);
			}
		}
	}
	else
		SetWindowDouble(m_hControl[nID], m_CommodityInfo.iPrecision, dPrice);

	return true;
}
void CMousewin::SetOrderQty(int nID, int nQty)//数量显示到界面
{
	SetWindowInt(m_hControl[nID], nQty);
}

bool CMousewin::CheckOrderPrice(double dPrice, bool bShowMsg, MPriceType type)
{
	if (m_CommodityInfo.bIfFractionPrice)//分数的情况下  分子 分母都是整数，所以只要限定不能输入小数就行了,这里只需要检查你输入分子的整数是否已经大于分母了，大于了应该进位
	{
		if (0 == static_cast<int>(m_CommodityInfo.dMiniChangePrice) / m_CommodityInfo.iPrecision)   //分子没有大于分母
			return true;
		else
		{
			if (bShowMsg)
				PopPriceCheckMsg(type,metMinTick);
			return false;
		}	
	}
	else
	{
		if (!CConvert::IsDiv(dPrice, m_CommodityInfo.dMiniChangePrice, FLOAT_SAME_ERRORDIFF))
		{
			if (bShowMsg)
				PopPriceCheckMsg(type, metMinTick);
			return false;
		}
		else
			return true;
	}
}
bool CMousewin::CheckIsOption(char ctype, bool bShowMsg)
{
	if (!CConvert::IsOptionContract(ctype))
	{
		if (bShowMsg)
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1072), LoadResWchar_t(IDS_TIP), NULL);
		return false;	//询价 应价 行权 弃权 都是期权合约
	}
	else
		return true;
}
bool CMousewin::GetOrderType(TSendOrder &sOrder)
{
	//非高级
	if (m_nCurClickItem.nCurHeadType != MHI_Advanced)
	{
		//价格类型
		m_StaticPriceSwitch.CxGetWindowText(m_sPricetype);
		sOrder.OrderType = CMouseString::WStrToOrderType(m_sPricetype);

		//策 略
		if (m_nCurClickItem.nCurHeadType == MHI_Strategys)
		{
			//客户端止损
			if (m_nCurClickItem.nCurChildType == CHS_PCStop)
			{
				//标记止损
				m_bStopPreHangOrder = true;

				//内盘 开仓止损
				if (m_CommodityInfo.cCoverMode != cmUnfinish&&m_CommodityInfo.cCoverMode != cmNone)
					sOrder.Offset = oOpen;
				else
					sOrder.Offset = oNone;
			}

			//定单类型不可选 统一：限价
			sOrder.OrderType = otLimit;	
		}
		else
			m_bStopPreHangOrder = false;


        if (m_nCurClickItem.nCurHeadType == MHI_BatchOrder)
        {
            if (IsWindowVisible(m_hControl[CI_Condition_Check]) && m_CheckCondition.GetCheck() == BST_CHECKED)
            {
                sOrder.StrategyType = stCondition;
                if (!GetConditionInfo(sOrder.TriggerMode, sOrder.TriggerCondition, sOrder.TriggerPrice))
                    return false;
            }
        }
	}
	else
	{
		m_bStopPreHangOrder = false;
		m_StaticPriceSwitch.CxGetWindowText(m_sPricetype);
		sOrder.OrderType = CMouseString::WStrToOrderType(m_sPricetype);
		TMouPriceTypeStrW wStr = {0};
		switch (m_nCurClickItem.nCurChildType)
		{
		case CHA_Shadow:
		case CHA_LimtStop:
		case CHA_Stop:
		case CHA_Iceberg:
		case CHA_Swap:
		case CHA_Enquiry:
		case CHA_Offer:
			if(GetShowStrByItem(m_nCurClickItem, wStr))
				sOrder.OrderType = CMouseString::WStrToOrderType(wStr);
			else
			{
				MessageBox(m_Hwnd,L"Invalid OrderType",LoadResWchar_t(IDS_TIP),NULL);
				return false;
			}
			break;	
		case CHA_ExecAban:
			sOrder.OrderType = m_bExecute? otExecute: otAbandon;
			break;
		case CHA_SpreadApply:
			sOrder.OrderType = otSpreadApply;
			break;
		case CHA_HedgeApply:
			sOrder.OrderType = otHedgApply;
			break;
		case CHA_Condition:
			sOrder.StrategyType = stCondition;
			if (!GetConditionInfo(sOrder.TriggerMode, sOrder.TriggerCondition, sOrder.TriggerPrice))
				return false;
			break;
		case CHA_PreOrder:
			sOrder.StrategyType = stPreOrder;
			break;
		case CHA_AutoOrder:
			sOrder.StrategyType = stAutoOrder;
			break;		
        case CHA_AutoClose:
        {
            sOrder.OrderType = m_bOptionAutoClose ? otOptionAutoClose : otFutureAutoClose;
            break;
        }
		}
	}
	
	switch (sOrder.OrderType)
	{
	case otSwap:
	case otLimit:
	case otGhost:
		if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))										return false;	//委托数量
		if (!GetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, sOrder.OrderPrice, sOrder.Direct))			return false;	//委托价格
		GetOrderValidStyle(sOrder);																					//有效类型
		break;
	case otMarket:
		if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))										return false;	//委托数量
		GetOrderValidStyle(sOrder);																					//有效类型
		break;
	case otLimitStop:
		if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))										return false;	//委托数量
		if (!GetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, sOrder.OrderPrice, sOrder.Direct))			return false;	//委托价格
		if (!GetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, sOrder.TriggerPrice, sOrder.Direct,true,mptTrig))	return false;	//触发价格
		GetOrderValidStyle(sOrder);																					//有效类型
		break;
	case otMarketStop:
		if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))										return false;	//委托数量
		if (!GetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, sOrder.TriggerPrice, sOrder.Direct, true, mptTrig))	return false;	//触发价格
		GetOrderValidStyle(sOrder);																					//有效类型
		break;
	case otIceberg:
		if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))										return false;	//委托数量
		if (!GetOrderQty(CI_MaxQty_Edit, sOrder.MaxOrderQty))										return false;	//最大数量
		if (!GetOrderQty(CI_MinQty_Edit, sOrder.MinOrderQty))										return false;	//最小数量
		if (!GetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, sOrder.OrderPrice, sOrder.Direct))			return false;	//委托价格
		GetOrderValidStyle(sOrder);																					//有效类型
		break;
	case otEnquiry:
		GetOrderValidStyle(sOrder);
		if (!CheckIsOption(sOrder.CommodityType, true))												return false;	//判断是否期权合约
        if (m_CommodityInfo.cCoverMode != cmCover && m_CommodityInfo.cCoverMode != cmCoverToday)
        {
            if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))										
                return false;	//委托数量
        }
		break;
	case otOffer:
		if (!CheckIsOption(sOrder.CommodityType, true))												return false;	//判断是否期权合约
		if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))										return false;	//委托数量
		if (!GetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, sOrder.OrderPrice, sOrder.Direct))			return false;	//委托价格
		if (!GetOrderPrice(CI_SellPrice_Edit, CI_SellPrice_Edit_Mole, sOrder.SellOrderPrice, sOrder.Direct))		return false;	//委托价格
		sOrder.SellOrderQty = sOrder.OrderQty;
		if (IsWindowVisible(m_CheckCloseDay.GetHwnd())&& m_CheckCloseDay.GetCheck()==BST_CHECKED)
		{
			sOrder.Offset = oCover == sOrder.Offset ? oCoverT : sOrder.Offset;
			sOrder.SellOffset = oCover == sOrder.SellOffset ? oCoverT : sOrder.SellOffset;
		}
		GetWindowTextA(m_hControl[CI_EnquriyNo_Edit], sOrder.EnquiryNo, sizeof(sOrder.EnquiryNo));
		GetOrderValidStyle(sOrder);
		break;
	case otExecute:
	case otAbandon:
		GetOrderValidStyle(sOrder);
        if (strcmp(sOrder.ExchangeNo, exSHFE) == 0)
        {
            sOrder.Offset = oCover;
        }
        if (m_bExecute && IsWindowVisible(m_hControl[CI_ExeAutoClose_Check]) &&
            m_CheckAutoClose.GetCheck() == BST_CHECKED)
        {
            sOrder.FutureAutoCloseFlag = bYes;
        }
        if (m_bExecute && IsWindowVisible(m_hControl[CI_CloseDay_Check]) &&
            m_CheckCloseDay.GetCheck() == BST_CHECKED)
        {
            sOrder.Offset = oCoverT;
        }
		if (!CheckIsOption(sOrder.CommodityType, true))												return false;	//判断是否期权合约
		if(!(IsCodeOfDCE() && sOrder.OrderType==otAbandon))
			if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))									return false;	//委托数量		
		break;
	case otSpreadApply:
	case otHedgApply:
		GetOrderValidStyle(sOrder);
		if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))										return false;	//委托数量	
		break;
    case otOptionAutoClose:
    case otFutureAutoClose:
        GetOrderValidStyle(sOrder);
        if (!CheckIsOption(sOrder.CommodityType, true))												return false;	//判断是否期权合约
        if (!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))									    return false;	//委托数量		
        break;
	default:
		break;
	}
	//止损 限价止损
	if (sOrder.OrderType == otLimitStop || sOrder.OrderType == otMarketStop)
	{
		SContract *pQuote = NULL;
		if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteContractId.c_str(), &pQuote, 1, false))
		{
			double dLastPrice = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
			if (sOrder.Direct == dSell && (dLastPrice <= sOrder.TriggerPrice ||
				(sOrder.OrderType == otLimitStop && sOrder.TriggerPrice <= sOrder.OrderPrice)) ||
				sOrder.Direct == dBuy && (dLastPrice >= sOrder.TriggerPrice ||
				(sOrder.OrderType == otLimitStop && sOrder.TriggerPrice >= sOrder.OrderPrice)))
			{
				if (MessageBox(m_Hwnd, LoadResWchar_t(IDS_StopTrig), LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDCANCEL)
					return false;
			}		
		}
	}

	//获取止损止盈价格
	if (m_bStopPreHangOrder)
	{
		if (!GetStopData(sOrder.Direct,sOrder.OrderPrice))
			return false;
	}
	return true;
}
//检测 定单类型支持 特殊支持
bool CMousewin::CheckOrderTypeSupport(TSendOrder &sOrder)																		
{
	//根据定单类型检测
	if (m_CommodityInfo.cCoverMode == cmCover|| m_CommodityInfo.cCoverMode == cmCoverToday)
	{
		switch (sOrder.OrderType)
		{
		case otSwap:
			//互换 仅大商所支持 
			if (strcmp(sOrder.ExchangeNo, exDCE)&& strcmp(sOrder.ExchangeNo, exZCE))
			{
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_SwapDCE), LoadResWchar_t(IDS_TIP), NULL);
				return false;
			}
			//互换 仅支持跨期、跨品种
			if (ctSpreadMonth != sOrder.CommodityType && ctSpreadCommodity != sOrder.CommodityType)
			{
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1071), LoadResWchar_t(IDS_TIP), NULL);
				return false;
			}
			break;
		case otSpreadApply:
			//套利申请 仅支持郑商所套利合约
			if (strcmp(sOrder.ExchangeNo, exZCE) || (sOrder.CommodityType!=ctSpreadMonth&&sOrder.CommodityType != ctSTD&&sOrder.CommodityType != ctSTG))
			{
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_CrossCom), LoadResWchar_t(IDS_TIP), NULL);
				return false;
			}
        case otOptionAutoClose:
        case otFutureAutoClose:
        {
            if (strcmp(sOrder.ExchangeNo, exSHFE) != 0)
            {
                MessageBox(m_Hwnd, LoadResWchar_t(IDS_SHFE_Support_Only), LoadResWchar_t(IDS_TIP), NULL);
                return false;
            }
            break;
        }
		default:
			break;
		}
	}
	else
	{
		switch (sOrder.OrderType)
		{
		case otIceberg:
			if (sOrder.OrderQty < sOrder.MaxOrderQty)
			{
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_MaxOverOrder), LoadResWchar_t(IDS_TIP), NULL);
				return false;	//数量关系
			}
			else if (sOrder.MaxOrderQty < sOrder.MinOrderQty)
			{
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_MinOverMax), LoadResWchar_t(IDS_TIP), NULL);
				return false;	//数量关系
			}
			break;
		default:
			break;
		}	
	}

	//开仓止损品种类型检测
	if (m_bStopPreHangOrder && !(sOrder.CommodityType == ctFutures || sOrder.CommodityType == ctGoods || sOrder.CommodityType == ctDefer|| sOrder.CommodityType == ctOption))
	{
		MessageBox(m_Hwnd, LoadResWchar_t(IDS_StopOnlyFuture), LoadResWchar_t(IDS_TIP), MB_OK);
		return false;
	}

	return true;
}
bool CMousewin::GetOrderQty(int nID, uint &nQty, bool bShowMsg)
{
	string sText = GetWindowTextAnsic(m_hControl[nID]);
	nQty = atoi(sText.c_str());
	if (nQty <= 0)
	{
		if (bShowMsg)
		{
			switch (nID)
			{
			case CI_OrderQty_Edit:
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1110), LoadResWchar_t(IDS_TIP), NULL);
				break;
			case CI_MaxQty_Edit:
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1111), LoadResWchar_t(IDS_TIP), NULL);
				break;
			case CI_MinQty_Edit:
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1112), LoadResWchar_t(IDS_TIP), NULL);
				break;
			default:
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1113), LoadResWchar_t(IDS_TIP), NULL);
				break;
			}
		}
		return false;
	}	
	//快速数量倍率
	if (m_CheckFastOrder.GetCheck() && nID==CI_OrderQty_Edit)
	{
		TMouQtyRateStrW sMutil = { 0 };
		GetWindowText(m_hControl[CI_OrderQtyRate_Edit], sMutil, sizeof(TMouQtyRateStrW)/sizeof(wchar_t));
		int nMutil = _wtoi(sMutil);
		if (nMutil>0)
			nQty = nQty*nMutil;
		else
		{
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_invalidMutilQty), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}		
	}
	return true;
}

bool CMousewin::GetOrderValidStyle(TSendOrder &sOrder)
{
	if (m_bModifyOrderOper)
		return true;
	if (IsWindowVisible(m_BtnValidType.GetHwnd()))
	{
		wstring sText = m_BtnValidType.GetText();
		sOrder.ValidType = CMouseString::WStrToValidType(sText.c_str());
		if (sOrder.ValidType == vtGTD)
		{
			string sDate = LoadRC::unicode_to_ansi(sText);
			strncpy_s(sOrder.ValidTime, sDate.c_str(), sizeof(sOrder.ValidTime));
		}
	}
	else
		sOrder.ValidType = vtGFD;
	return true;
}
bool CMousewin::GetSpreadPriceInfo(TDLPriceMode &cMode, char &cCondition, double &dPrice, bool bShowMsg)			//获取套利价格信息
{
	if (IsSpreadTriggered())
	{
		cCondition = ctNone;
		return true;
	}
	else if (IsWindowVisible(m_CheckAdvanced.GetHwnd()) && m_CheckAdvanced.GetCheck() == BST_CHECKED)
	{
		TMouModeStrW wCon = { 0 };
		//触发方式
		int n = sizeof(wCon) / sizeof(wchar_t);
		m_ComboxSpreadMode.CxGetWindowText(wCon, sizeof(wCon) / sizeof(wchar_t));
        if (!wcscmp(wCon, LoadResWchar_t(IDS_MatchSpread)))
            cMode = tpm_Match;
        else if (!wcscmp(wCon, LoadResWchar_t(IDS_WorkSpread)))
            cMode = tpm_Work;
        else if (wcscmp(wCon, LoadResWchar_t(IDS_STRING1122)) == 0)
            cMode = tpm_Last;
		else
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_InvalidTrigMode), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
		//触发条件
		m_ComboxSpreadCompare.CxGetWindowText(wCon, sizeof(wCon) / sizeof(wchar_t));
		if (!wcscmp(wCon, L">="))
			cCondition = tcGreaterEqual;
		else if (!wcscmp(wCon, L"<="))
			cCondition = tcLessEqual;
		else
		{
			if (bShowMsg)
				MessageBox(m_Hwnd, LoadResWchar_t(IDS_InvalidTrigCon), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
	}
	
	//套利 触发价
	if (!GetSpreadTrigPrice(dPrice, true))
		return false;

	return true;
}
bool CMousewin::GetConditionInfo(char &cMode, char &cCondition, double &dPrice, bool bShowMsg)
{
	TMouModeStrW wCon = { 0 };
	//触发方式
	int n = sizeof(wCon) / sizeof(wchar_t);
	m_ComboxMode.CxGetWindowText(wCon, sizeof(wCon)/sizeof(wchar_t));
	if (!wcscmp(wCon, LoadResWchar_t(IDS_STRING1122)))
		cMode = tmLatest;
	else if (!wcscmp(wCon, LoadResWchar_t(IDS_BuyPrice)))
		cMode = tmBid;
	else if (!wcscmp(wCon, LoadResWchar_t(IDS_SellPrice)))
		cMode = tmAsk;
	else
	{
		if (bShowMsg)
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_InvalidTrigMode), LoadResWchar_t(IDS_TIP), NULL);
		return false;
	}
	//触发条件
	m_ComboxCompare.CxGetWindowText(wCon, sizeof(wCon) / sizeof(wchar_t));
	if (!wcscmp(wCon, L">="))
		cCondition = tcGreaterEqual;
	else if (!wcscmp(wCon, L"<="))
		cCondition = tcLessEqual;
	else
	{
		if (bShowMsg)
			MessageBox(m_Hwnd, LoadResWchar_t(IDS_InvalidTrigCon), LoadResWchar_t(IDS_TIP), NULL);
		return false;
	}
	//触发价格
	if (!GetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, dPrice, dBuy, true, mptTrig))
		return false;

	return true;
}
bool  CMousewin::FillOrder(TSendOrder& sOrder, bool bGetPrice, bool bGetCode)
{
	//用户名
	if (!GetOrderUser(sOrder.UserNo))
		return false;

	//服务器标识
	strcpy_s(sOrder.Sign, m_CommodityInfo.Sign);

	//期权 点价 竖向
	if(IsOptionStrategyLayOut())
		sOrder.OrderWay = owVert;
	else if (IsLadderLayOut())
	{
		if (!GetOrderCode(sOrder, m_sCode, CI_ContractNo_Edit))
			return false;
		CorrectOrderPrice(sOrder);
		sOrder.OrderWay = owClickPrice;
	}
	else
	{
		if(!GetOrderCode(sOrder, m_sCode, CI_ContractNo_Edit))
			return false;
		if(!GetOrderType(sOrder))
			return false;		
		CorrectOrderPrice(sOrder);
		sOrder.OrderWay = owVert;
	}

	//检测交易所支持
	if (!CheckOrderTypeSupport(sOrder))
		return false;

	//上期所 平今
	if (IsWindowVisible(m_CheckCloseDay.GetHwnd()) && m_CheckCloseDay.GetCheck() == BST_CHECKED&&sOrder.Offset==oCover)
		sOrder.Offset = oCoverT;//修改平今

	//风险报单 强平 交易员 非 询价 行权 弃权
	if (IsWindowVisible( m_hControl[CI_ForceClose_Check])&& m_CheckForceClose.GetCheck())
		sOrder.IsRiskOrder = bYes;

	//T+1有效
	if (IsWindowVisible(m_hControl[CI_AddOne_Check])&& m_CheckAddOne.GetCheck())
		sOrder.AddOneIsValid = tsNight;

	//备注信息
	if (IsWindowVisible(m_hControl[CI_NoteInfo_Edit]) && m_MouseCfg.bShowNoteInfo)
	{
		string sRemark = GetWindowTextAnsic(m_hControl[CI_NoteInfo_Edit]);
		memcpy_s(sOrder.Remark, sizeof(sOrder.Remark) - 1, sRemark.c_str(), sizeof(sOrder.Remark) - 1);
	}

	return true;
}
bool CMousewin::FillSpreadOrder(TSpreadSendOrder& sOrder)
{
    if (m_sSpreadCode.empty())
    {
        MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1109), LoadResWchar_t(IDS_TIP), NULL);
        return false;
    }
    SSpreadContract sQuote;
    if (g_pStarApi && g_pStarApi->GetSpreadInfo(m_sSpreadCode.c_str(), &sQuote, 1, false) == 0)
    {
        MessageBox(m_Hwnd, LoadResWchar_t(IDS_STRING1109), LoadResWchar_t(IDS_TIP), NULL);
        m_sSpreadCode = "";
        return false;
    }

	if (!GetOrderUser(sOrder.UserNo[0]))
		return false;
    if (!GetOrderUser2(sOrder.UserNo[1], 1))
		return false;
    if (sQuote.ValidLegs > 2 && !GetOrderUser2(sOrder.UserNo[2], 2))
        return false;
    if (sQuote.ValidLegs > 3 && !GetOrderUser2(sOrder.UserNo[3], 3))
        return false;

	//服务器标识
	strcpy_s(sOrder.Sign[0], m_CommodityInfo.Sign);
    if (m_UserInfo.size() > 1)
    {
        strcpy_s(sOrder.Sign[1], m_sSign[1].c_str());
    }
    else
    {
        strcpy_s(sOrder.Sign[1], sOrder.Sign[0]);
    }

	//套利类型
	if (m_nCurClickItem.nCurChildType == CHS_SyncSpread)
		sOrder.SpreadType = stSpreadSyn;
	else 
		sOrder.SpreadType = stSpreadSequ;
	
	if (!GetSpreadPriceInfo(sOrder.TrigPriceMode, sOrder.TrigCondition, sOrder.OrderPrice))
		return false;

	if (!GetSpreadOrderCode(sQuote, sOrder))
		return false;
	
	//检测套利价格
	if (!IsSpreadTriggered()&& m_OrderDealInfo.bShowTipSpreadTrig && CheckSpreadOrderPriceTrigState(sOrder))
		return false;
	//获取数量
	if(!GetOrderQty(CI_OrderQty_Edit, sOrder.OrderQty))
		return false;
	
	//上期所 平今
	if (IsWindowVisible(m_CheckCloseDay.GetHwnd()) && m_CheckCloseDay.GetCheck() == BST_CHECKED&&sOrder.Offset[0] == oCover)
	{
		sOrder.Offset[0] = oCoverT;//修改平今
		sOrder.Offset[1] = sOrder.Offset[0];
	}
	//时间
	if (!GetActiveTime(sOrder))
		return false;

	//自动开平
	bool bToday = IsWindowVisible(m_CheckCloseDay.GetHwnd()) && m_CheckCloseDay.GetCheck() == BST_CHECKED ? true : false;//平今勾选
	if (m_bAutoCover && m_CommodityInfo.cCoverMode != cmUnfinish&&m_CommodityInfo.cCoverMode != cmNone)//自动开平 内盘
	{
		int nPos = 0;

        TContractKey key[4];
        memset(&key, 0, sizeof(key));
        uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
        for (int i = 0; i < sQuote.ValidLegs; i++)
        {
            CConvert::SContractNoTypeToTradeContract(m_sSign[i].c_str(), sQuote.Contract[i]->ContractNo, key[i]);
            g_pTradeData->GetPositionVol(m_sUserNo[i].c_str(), &key[i], LongCount, ShortCount, DayLongCount, DayShortCount);//单笔持仓
            if (i == 0)
                nPos = sOrder.Direct[0] == dSell ? LongCount / (uint)sQuote.SpreadQty[0] : ShortCount / (uint)sQuote.SpreadQty[0];       
            else
            {
                if (sOrder.Direct[0] == dSell)
                    nPos = min(nPos, int(sQuote.RateMode[i - 1] != S_SPREADMODE_SUM ? (ShortCount / (uint)sQuote.SpreadQty[i]) : (LongCount / (uint)sQuote.SpreadQty[i])));
                else
                    nPos = min(nPos, int(sQuote.RateMode[i - 1] != S_SPREADMODE_SUM ? (LongCount / (uint)sQuote.SpreadQty[i]) : (ShortCount / (uint)sQuote.SpreadQty[i])));
            }
        }

		if (nPos == 0)
		{
			sOrder.Offset[0] = oOpen;
			sOrder.Offset[1] = sOrder.Offset[0];
		}
		else
		{
			sOrder.Offset[0] = bToday ? oCoverT : oCover;
			sOrder.Offset[1] = sOrder.Offset[0];
		}
	}
    auto it = m_UserCoverModeMap.find(sOrder.UserNo[0]);
    if (it != m_UserCoverModeMap.end())
    {
        if (it->second == cmNone)
            sOrder.Offset[0] = oNone;
    }
    it = m_UserCoverModeMap.find(sOrder.UserNo[1]);
    if (it != m_UserCoverModeMap.end())
    {
        if (it->second == cmNone)
            sOrder.Offset[1] = oNone;
    }

    sOrder.IsRecove[0] = bNo;
    sOrder.IsRecove[1] = bNo;
    bool bRecove = IsWindowVisible(m_CheckAssistOrder.GetHwnd()) && m_CheckAssistOrder.GetCheck() == BST_CHECKED;
    if (bRecove)
    {
        sOrder.IsRecove[0] = sQuote.PriorityLeg == S_PRIORITYLEG_SECOND ? bYes : bNo;//优先腿是下单时优先下单，追价追第二下单的
        sOrder.IsRecove[1] = sQuote.PriorityLeg == S_PRIORITYLEG_FIRST ? bYes : bNo;
    }

    //后两个合约处理
    if (sQuote.ValidLegs > 2)
    {
        if (m_UserInfo.size() > 1)
        {
            strcpy_s(sOrder.Sign[2], m_sSign[2].c_str());
        }
        else
        {
            strcpy_s(sOrder.Sign[2], sOrder.Sign[0]);
        }

        sOrder.Offset[2] = sOrder.Offset[0];
        it = m_UserCoverModeMap.find(sOrder.UserNo[2]);
        if (it != m_UserCoverModeMap.end())
        {
            if (it->second == cmNone)
                sOrder.Offset[2] = oNone;
        }

        sOrder.IsRecove[2] = bRecove ? bYes : bNo;
        if (bRecove)
        {//优先腿考虑

        }
    }
    if (sQuote.ValidLegs > 3)
    {
        if (m_UserInfo.size() > 1)
        {
            strcpy_s(sOrder.Sign[3], m_sSign[3].c_str());
        }
        else
        {
            strcpy_s(sOrder.Sign[3], sOrder.Sign[0]);
        }
        sOrder.Offset[3] = sOrder.Offset[0];
        it = m_UserCoverModeMap.find(sOrder.UserNo[3]);
        if (it != m_UserCoverModeMap.end())
        {
            if (it->second == cmNone)
                sOrder.Offset[3] = oNone;
        }

        sOrder.IsRecove[3] = bRecove ? bYes : bNo;
        if (bRecove)
        {

        }
    }

    if (IsWindowVisible(m_CheckMovePosition.GetHwnd()) && m_CheckMovePosition.GetCheck() == BST_CHECKED && sOrder.Offset[0] != oNone)
    {//移仓处理
        if (!UpdateOffsetOfMovePosition(sOrder))
        {
            return false;
        }
    }

	return true;
}
void CMousewin::OnDealBatchOrderChange()
{
	if (m_nCurClickItem.nCurHeadType == MHI_BatchOrder)
		ReSetUserNos();
	else
		m_bBatchInfoModify = true;
}
bool CMousewin::ReSetUserNos()
{
	if (g_pTradeData)
	{
		if (IsWindow(m_hControl[CI_UserName_Static]))
		{
			m_StaticUserName.UpdateText("");
			m_StaticUserName.SetToolTips(L"");
		}
		//清理数据
		m_ComboxUserNo.ResetContent();
		m_ComboxUserNo2.ResetContent();
        m_ComboxUserNo3.ResetContent();
        m_ComboxUserNo4.ResetContent();
		m_UserInfo.clear();
		m_UserCoverModeMap.clear();
        for (int i = 0; i < 4; i++)
        {
            m_sUserNo[i].clear();
            m_sSign[i].clear();
        }
		m_sUserName = "";
		//////////////////////////////////////////////////////////////////////////
		TDataContainer tUsers;
		g_pTradeData->GetAllUser(tUsers, "");
		uint count = tUsers.size();
		if (m_nCurClickItem.nCurHeadType != MHI_BatchOrder)
		{
			for (uint i = 0; i < count; i++)
			{
				const TUserInfo *pUser = (const TUserInfo *)tUsers.at(i);
				if (!(pUser->pLogin && (bYes == pUser->pLogin->Logined)))	continue;//判断是否登录

				map<string, sUserInfor>::iterator it1 = m_UserInfo.find(pUser->UserNo);
				if (it1 == m_UserInfo.end())
				{
					//更新用户数据
					sUserInfor sUser;
					memcpy(sUser.sUserNo, pUser->UserNo, sizeof(sUser.sUserNo));
					memcpy(sUser.sUserName, !strcmp(pUser->UserName, "") ? pUser->UserNo : pUser->UserName, sizeof(sUser.sUserName));
					memcpy(sUser.sSign, pUser->Sign, sizeof(sUser.sSign));
					sUser.cUserType = pUser->pLogin->UserType;
					m_UserInfo.insert(pair<string, sUserInfor>(pUser->UserNo, sUser));
					if (!strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_FOREIGNTRADEAPI_NAME).c_str()) || !strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_DIPPERTRADEAPI_NAME).c_str())
						/*|| !strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_KINGSTARTRADEAPI_NAME).c_str())*/)//外盘 默认不显示开平
						m_UserCoverModeMap.insert(pair<string, char>(pUser->UserNo, cmNone));
					else
						m_UserCoverModeMap.insert(pair<string, char>(pUser->UserNo, cmCover));	
				}
			}
			if (m_UserInfo.size() > 1)//多个资金账户
			{
				//更新 空账号 支持联动全选
				sUserInfor sUser;
				memset(&sUser, 0, sizeof(sUser));
				map<string, sUserInfor>::iterator it = m_UserInfo.begin();
				if (it != m_UserInfo.end())
					memcpy(sUser.sSign, it->second.sSign, sizeof(sUser.sSign));
				m_UserInfo.insert(pair<string, sUserInfor>("", sUser));
			} 
			//更新界面数据
			for (map<string, sUserInfor>::iterator it1 = m_UserInfo.begin(); it1 != m_UserInfo.end();it1++)
			{
				sUserInfo sInfo; 
				memcpy(sInfo.UserNo, it1->second.sUserNo, sizeof(sInfo.UserNo));
				memcpy(sInfo.UserName, it1->second.sUserName, sizeof(sInfo.UserName));
				m_ComboxUserNo.AddRow(sInfo);
                if (!m_bTradeLogined)
                {
                    m_ComboxUserNo2.AddRow(sInfo);
                    m_ComboxUserNo3.AddRow(sInfo);
                    m_ComboxUserNo4.AddRow(sInfo);
                }
			}
			
			if (m_UserInfo.size() == 1 )//单个资金账户 
			{		
				//数据
				map<string, sUserInfor>::iterator it = m_UserInfo.begin();
                for (int i = 0; i < 4; i++)
                {
                    m_sUserNo[i] = it->first;
                    m_sSign[i] = it->second.sSign;
                }

				m_sUserName = it->second.sUserName;		
				m_cUserType = it->second.cUserType;
				memcpy(m_CommodityInfo.Sign, m_sSign[0].c_str(), sizeof(m_CommodityInfo.Sign));
				
				map<string, char>::iterator it2 = m_UserCoverModeMap.find(m_sUserNo[0]);
				if (it2 != m_UserCoverModeMap.end())
					m_CommodityInfo.cCoverMode = it2->second;

				//界面
				sUserInfo sInfo;
				memcpy(sInfo.UserNo, m_sUserNo[0].c_str(), sizeof(sInfo.UserNo));
				memcpy(sInfo.UserName, m_sUserName.c_str(), sizeof(sInfo.UserName));
				m_ComboxUserNo.ResetContent();
				m_ComboxUserNo.AddRow(sInfo);
				SetWindowTextAnsic(m_hControl[CI_UserNo_Combox], m_sUserNo[0].c_str());
				FormatDisplayQtyAndFreeze(true, true);//刷新
				EnableWindow(m_hControl[CI_UserNo_Combox], FALSE);

				//点价
				m_OrderList.UpdateUser(m_sUserNo[0].c_str(), m_sSign[0].c_str(),m_CommodityInfo.cCoverMode);

				if (g_mainframe)
				{
					char content[50] = { 0 };
					sprintf_s(content, "user=%s", m_sUserNo[0].c_str());
					g_mainframe->Linkage(m_Hwnd, _MouseOrder, W_USER_CHANGED, content);			
				}
			}
			else if (!m_UserInfo.empty())
			{
				map<string, sUserInfor>::iterator it = ++m_UserInfo.begin();
				if (it != m_UserInfo.end())//第一个用户信息
				{
					m_sSign[0] = it->second.sSign;
					m_cUserType = it->second.cUserType;
					memcpy(m_CommodityInfo.Sign, m_sSign[0].c_str(), sizeof(m_CommodityInfo.Sign));
					
					map<string, char>::iterator it2 = m_UserCoverModeMap.begin();
                    if (it2 != m_UserCoverModeMap.end())//第一个用户信息
                    {
                        m_CommodityInfo.cCoverMode = it2->second;
                    }
				}
				EnableWindow(m_hControl[CI_UserNo_Combox], TRUE);
				SetWindowText(m_hControl[CI_UserNo_Combox2], LoadResWchar_t(IDS_AccNo2));
				SetWindowText(m_hControl[CI_UserNo_Combox3], LoadResWchar_t(IDS_AccNo3));
				SetWindowText(m_hControl[CI_UserNo_Combox4], LoadResWchar_t(IDS_AccNo4));
				if (g_mainframe)			
					g_mainframe->Linkage(m_Hwnd, _MouseOrder, W_USER_CHANGED, "user=");		

			}			
			m_CodeList.UpdateData(m_CommodityInfo.Sign);//更新下拉匹配列表 交易合约	
			
			if (GetCreateFlag())
			{
				CodeUpdateCommodityInfo();
				LayOut();
			}
			return true;
		}
		else
		{
			for (uint i = 0; i < count; i++)
			{
				const TUserInfo *pUser = (const TUserInfo *)tUsers.at(i);
				if (!(pUser->pLogin && (bYes == pUser->pLogin->Logined)))	continue;//判断是否登录

				map<string, sUserInfor>::iterator it1 = m_UserInfo.find(pUser->UserNo);
				if (it1 == m_UserInfo.end())
				{
					sUserInfor sUser;
					memcpy(sUser.sUserNo, pUser->UserNo, sizeof(sUser.sUserNo));
					memcpy(sUser.sUserName, !strcmp(pUser->UserName, "") ? pUser->UserNo : pUser->UserName, sizeof(sUser.sUserName));
					memcpy(sUser.sSign, pUser->Sign, sizeof(sUser.sSign));
					sUser.cUserType = pUser->pLogin->UserType;
					//////////////////////////////////////////////////////////////////////////
					m_UserInfo.insert(pair<string, sUserInfor>(pUser->UserNo, sUser));
                    if (!strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_FOREIGNTRADEAPI_NAME).c_str()) ||
                        !strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_DIPPERTRADEAPI_NAME).c_str()))//外盘 默认不显示开平
                    {
                        m_UserCoverModeMap.insert(pair<string, char>(pUser->UserNo, cmNone));
                    }
					else
						m_UserCoverModeMap.insert(pair<string, char>(pUser->UserNo, cmCover));
				}
			}
			if (m_batchGroupVec.empty())
			{
				m_bBatchInfoModify = false;
				if (GetCreateFlag())
					LayOut();
				return false;
			}
			for (map<string, BatchGroupInfo>::iterator iter = m_batchGroupVec.begin(); iter != m_batchGroupVec.end(); iter++)
			{
				sUserInfo user;
				memcpy(user.UserNo, iter->second.BatchGroupNo, sizeof(iter->second.BatchGroupNo));
				memcpy(user.UserName, iter->second.BatchGroupName, sizeof(iter->second.BatchGroupName));
				m_ComboxUserNo.AddRow(user);
			}
			if (m_batchGroupVec.size() == 1 && ::IsWindow(m_ComboxUserNo.GetHwnd()))
			{
				map<string, BatchGroupInfo>::iterator it = m_batchGroupVec.begin();
				if (IsWindow(m_hControl[CI_UserName_Static]))
					m_StaticUserName.UpdateText(it->second.BatchGroupName); 
				for (map<string, sUserInfor>::iterator ite = m_UserInfo.begin(); ite != m_UserInfo.end(); ite++)
				{
					if (strcmp(ite->second.sUserNo,it->second.BaseUser)==0)
					{
						m_sUserNo[0] = ite->second.sUserNo;
						m_sSign[0] = ite->second.sSign;
						memcpy(m_CommodityInfo.Sign, m_sSign[0].c_str(), sizeof(m_CommodityInfo.Sign));
						map<string, char>::iterator it2 = m_UserCoverModeMap.find(m_sUserNo[0]);
                        if (it2 != m_UserCoverModeMap.end())//第一个用户信息
                        {
                            m_CommodityInfo.cCoverMode = it2->second;
                            CodeUpdateCommodityInfo();
                        }					
						break;
					}
				}
				ComboBox_SetText(m_hControl[CI_UserNo_Combox], LoadRC::ansi_to_unicode(it->second.BatchGroupNo).c_str());
			}
			m_CodeList.UpdateData(m_CommodityInfo.Sign);//更新下拉匹配列表 交易合约	

			if (GetCreateFlag())
				LayOut();
			m_bBatchInfoModify = false;
			return true;
		}
	}
	else
		return false;
}
void CMousewin::UpdatePositionQtySpread()							//更新持仓显示
{
	if (m_sUserNo[0].empty() || m_sUserNo[1].empty())
	{
		ClearPosition();
		return;
	}

    SSpreadContract sQuote;
	if (g_pStarApi && g_pStarApi->GetSpreadInfo(m_sSpreadCode.c_str(), &sQuote, 1, false))
	{
		TContractKey key[4];
		memset(&key, 0, sizeof(key));

        uint nLong = 0, nShort = 0;
        uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
        if (IsWindowVisible(m_hControl[CI_MovePosition_Check]) && m_CheckMovePosition.GetCheck() == BST_CHECKED)
        {
			for (int i = 0; i < sQuote.ValidLegs; i++)
			{
				if (i == 0)
				{
					CConvert::SContractNoTypeToTradeContract(m_sSign[i].c_str(), sQuote.Contract[i]->ContractNo, key[i]);
					g_pTradeData->GetPositionVol(m_sUserNo[i].c_str(), &key[i], LongCount, ShortCount, DayLongCount, DayShortCount);//单笔持仓
					nLong = LongCount / (uint)sQuote.SpreadQty[i];
					nShort = ShortCount / (uint)sQuote.SpreadQty[i];
				}
				else if (sQuote.RateMode[i - 1] == S_SPREADMODE_SUM)
				{
					CConvert::SContractNoTypeToTradeContract(m_sSign[i].c_str(), sQuote.Contract[i]->ContractNo, key[i]);
					g_pTradeData->GetPositionVol(m_sUserNo[i].c_str(), &key[i], LongCount, ShortCount, DayLongCount, DayShortCount);//单笔持仓
					nLong = min(LongCount / (uint)sQuote.SpreadQty[i], nLong);
					nShort = min(ShortCount / (uint)sQuote.SpreadQty[i], nShort);
				}
			}       
        }
        else
        {
            for (int i = 0; i < sQuote.ValidLegs; i++)
            {
                CConvert::SContractNoTypeToTradeContract(m_sSign[i].c_str(), sQuote.Contract[i]->ContractNo, key[i]);
                g_pTradeData->GetPositionVol(m_sUserNo[i].c_str(), &key[i], LongCount, ShortCount, DayLongCount, DayShortCount);//单笔持仓
                if (i == 0)
                {
                    nLong = LongCount / (uint)sQuote.SpreadQty[0];
                    nShort = ShortCount / (uint)sQuote.SpreadQty[0];
                }
                else
                {
                    nLong = min(nLong, (sQuote.RateMode[i - 1] != S_SPREADMODE_SUM ? (ShortCount / (uint)sQuote.SpreadQty[i]) : (LongCount / (uint)sQuote.SpreadQty[i])));
                    nShort = min(nShort, (sQuote.RateMode[i - 1] != S_SPREADMODE_SUM ? (LongCount / (uint)sQuote.SpreadQty[i]) : (ShortCount / (uint)sQuote.SpreadQty[i])));
                }
            }
        }

        TMouQtyStrW cQty = { 0 };
        GetWindowText(m_hControl[CI_OrderQty_Edit], cQty, sizeof(TMouQtyStrW));
        m_posinfo.nEditQty = _wtoi(cQty);
        m_posinfo.type = GetModeType();
        m_posinfo.nLong = nLong;
        m_posinfo.nShort = nShort;
        m_cOrderBtn.UpdateData(m_posinfo);
        m_cOrderBtn.UpdateCanOpen(0, 0);
	}
	else
	{
		ClearPosition();
		return;
	}
}
void CMousewin::UpdatePositionQtySwap()									//更新持仓显示
{
	string strCode = m_sCode;
	TContractKey key;
	memset(&key, 0, sizeof(key));
	memcpy(key.Sign, m_CommodityInfo.Sign, sizeof(key.Sign));
	if (CConvert::TContractNoToTContractKey(strCode.c_str(), key) && 
        (key.CommodityType == ctSpreadMonth || key.CommodityType == ctSpreadCommodity) && 
        (strcmp(key.ExchangeNo, exDCE) == 0 || strcmp(key.ExchangeNo, exZCE) == 0))
	{
		TContractSingleKey SigCont1, SigCont2;
        memset(&SigCont1, 0, sizeof(TContractSingleKey));
        memset(&SigCont2, 0, sizeof(TContractSingleKey));
		CConvert::CombTContractKeyToTContractSingleKey(key, SigCont1, SigCont2);
		uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
		uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
		g_pTradeData->GetPositionVol(m_sUserNo[0].c_str(), &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
		g_pTradeData->GetPositionVol(m_sUserNo[0].c_str(), &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);
		
		m_posinfo.type = GetModeType();
		m_posinfo.nLong = LongCount1;
		m_posinfo.nShort = ShortCount1;
		m_cOrderBtn.UpdateData(m_posinfo);

		//m_cOrderBtn.UpdateCanOpen(ShortCount2,LongCount2);
        m_cOrderBtn.UpdateCanOpen(LongCount2, ShortCount2);
	}
	else
		ClearPosition();
}
void CMousewin::UpdatePositionQtyNormal()								//更新持仓显示
{
	//清空
	if (m_sUserNo[0].empty())
	{
		ClearPosition();
		return;
	}

	//持仓计算	
	TContractKey key;
	memset(&key, 0, sizeof(TContractKey));
	memcpy(key.Sign, m_CommodityInfo.Sign, sizeof(key.Sign));
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	if (CConvert::TContractNoToTContractKey(m_sCode.c_str(), key) && CConvert::IsValidTradeCommodity(key))
		GetPositionVol(m_sUserNo[0].c_str(), &key, LongCount, ShortCount, DayLongCount, DayShortCount);//单笔持仓
	else
	{
		ClearPosition();	
		return;
	}

	//可行权数更新
	UpdateCanExecuteQty(key.CommodityType,LongCount);

	//行权 期权 应价 点价 //持仓价格
	UpdateStaticPosInfo(key,LongCount,ShortCount);
	
	//点价界面更新
	m_OrderList.UpdatePosition();

	//三键 按钮可平
	UpdateBtnOpenCover(key,LongCount,ShortCount);
}
void CMousewin::UpdatePositionQty()
{
	SetShowTips(L"");
	if (IsSpreadLayOut())
		UpdatePositionQtySpread();
	else if (IsSwapLayOut())
		UpdatePositionQtySwap();
	else
		UpdatePositionQtyNormal();
}
void CMousewin::UpdateBtnCanOpen(const TContractKey* key)
{
	if (IsSpreadLayOut() || IsSwapLayOut())
		return;

	//清空
	if (m_sUserNo[0].empty())
	{
		ClearPosition();
		return;
	}
	//内盘有效
	if (m_CommodityInfo.cCoverMode == cmNone || m_CommodityInfo.cCoverMode == cmUnfinish)
		return;

	//可开
	TSendOrder tempSendOrder;
	CConvert::DefaultSendOrder(tempSendOrder);

	memcpy(tempSendOrder.UserNo, m_sUserNo[0].c_str(), sizeof(tempSendOrder.UserNo));//用户名 服务器标识
	memcpy(tempSendOrder.Sign, m_CommodityInfo.Sign, sizeof(tempSendOrder.Sign));
	
	if (key)
		memcpy(&tempSendOrder, key, sizeof(TContractKey));	
	else
	{
		//持仓计算	
		TContractKey key2;
		memset(&key2, 0, sizeof(TContractKey));
		memcpy(key2.Sign, m_CommodityInfo.Sign, sizeof(key2.Sign));
		if (CConvert::TContractNoToTContractKey(m_sCode.c_str(), key2) && CConvert::IsValidTradeCommodity(key2))
			memcpy(&tempSendOrder, &key2, sizeof(TContractKey));
		else
			return;
	}
	
	m_StaticPriceSwitch.CxGetWindowText(m_sPricetype);
	tempSendOrder.OrderType = CMouseString::WStrToOrderType(m_sPricetype);

	double dbuy = 0, dsell = 0;
	if (tempSendOrder.OrderType==otLimit&&!GetDoubleOrderPrice(dbuy, dsell))
	{
		m_cOrderBtn.UpdateCanOpen(0, 0);
		return;
	}

	tempSendOrder.Direct = dBuy;
	tempSendOrder.Offset = oOpen;
	tempSendOrder.OrderPrice = dbuy;

	//可开
	int ibuyret = CConvert::GetCanOpenQty(m_CommodityInfo.cCoverMode, tempSendOrder, m_OrderDealInfo.bOpenAutoCancel);

	tempSendOrder.Direct = dSell;
	tempSendOrder.OrderPrice = dsell;
	//可开
	int isellret = CConvert::GetCanOpenQty(m_CommodityInfo.cCoverMode, tempSendOrder, m_OrderDealInfo.bOpenAutoCancel);

	m_cOrderBtn.UpdateCanOpen(ibuyret, isellret);
	
}
void CMousewin::UpdateBtnOpenCover(const TContractKey& key, uint nLong, uint nShort)
{
	//可平 是否减去挂单
	m_posinfo.type = GetModeType();
	m_posinfo.nLong = nLong;
	m_posinfo.nShort = nShort;

	TMouQtyStrW cQty = { 0 };
	GetWindowText(m_hControl[CI_OrderQty_Edit], cQty, sizeof(TMouQtyStrW));
	m_posinfo.nEditQty = _wtoi(cQty);
	m_cOrderBtn.UpdateData(m_posinfo);

	UpdateBtnCanOpen(&key);
}
void CMousewin::UpdateCanExecuteQty(char cCommodityType, uint nPos)									//更新可行权数
{
	if (cCommodityType == ctOption && nPos > 0 && IsOptionExAbnLayOut() && !m_bTradeLogined)
	{
		uint n = GetParOrderExcute();

		if (nPos > n)
		{
			TMouFormatPriceStrW wstr = { 0 };
			swprintf_s(wstr, LoadResWchar_t(IDS_Available), nPos - n);
			SetWindowTextW(m_hControl[CI_CanExcute_Option], wstr);
		}
		else
			SetWindowTextW(m_hControl[CI_CanExcute_Option], LoadResWchar_t(IDS_AvailableZero));
		RedrawWindow(m_hControl[CI_CanExcute_Option], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	}
	else
		SetWindowTextW(m_hControl[CI_CanExcute_Option], LoadResWchar_t(IDS_AvailableZero));
}
void CMousewin::UpdateStaticPosInfo(const TContractKey& key, uint nLong, uint nShort)
{
	if (!IsOptionLayOut() && !IsLadderLayOut())
		return;

	TMouQtyStrW wPrice[2] = { 0 };
	double dPriceB = 0, dPriceS = 0;
	if (nLong)
	{
		const TPosition* p = g_pTradeData->GetPositionTotal(m_sUserNo[0].c_str(), &key, dBuy);
		dPriceB = p ? p->PositionPrice : 0;
		if (!CConvert::DoubleSameValue(0, dPriceB, FLOAT_SAME_ERRORDIFF))
			swprintf_s(wPrice[0], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dPriceB, m_CommodityInfo)).c_str());
	}
	if (nShort)
	{
		const TPosition* p = g_pTradeData->GetPositionTotal(m_sUserNo[0].c_str(), &key, dSell);
		dPriceS = p ? p->PositionPrice : 0;
		if (!CConvert::DoubleSameValue(0, dPriceS, FLOAT_SAME_ERRORDIFF))
			swprintf_s(wPrice[1], L"%s", LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dPriceS, m_CommodityInfo)).c_str());
	}
	m_StaticPosInfo.UpdatePosInfo(nLong, nShort, wPrice[0], wPrice[1]);
}
void CMousewin::ClearPosition()
{
	m_posinfo.type = GetModeType();
	m_posinfo.nLong = 0;
	m_posinfo.nShort = 0;
	m_posinfo.nEditQty = 0;
	m_cOrderBtn.UpdateData(m_posinfo);
	m_cOrderBtn.UpdateCanOpen(0, 0);
	m_StaticPosInfo.UpdatePosInfo(0, 0, L"", L"");
	SetWindowTextW(m_hControl[CI_CanExcute_Option], LoadResWchar_t(IDS_AvailableZero));
	m_OrderList.UpdatePosition();
}

bool CMousewin::GetTradeCommodityInf(TContractKey &key, SOFCommodityInf &CommodityInfo)
{
	const SCommodity * pCommodityInfo = g_pTradeData->GetCommodity(&key);//由参数传递给的
	if (!pCommodityInfo)
		return false;
	if (0 == pCommodityInfo->PriceDeno)
	{
		CommodityInfo.bCanNegative = false;
		CommodityInfo.dMiniChangePrice = 1;
		CommodityInfo.iPrecision = 0;
		CommodityInfo.CmbDirectFlag = dfNone;
		return false;
	}
	TCommodityType CommdityType = CConvert::QuoteTypeToTradeType(pCommodityInfo->CommodityType);
    
	if (ctGoods == CommdityType || ctFutures == CommdityType || ctOption == CommdityType)
		CommodityInfo.bCanNegative = false;
	else if (ctSpreadMonth == CommdityType || ctSpreadCommodity == CommdityType ||
		ctBUL == CommdityType || ctBER == CommdityType || ctBLT == CommdityType || ctBRT == CommdityType || ctSTD == CommdityType || ctSTG == CommdityType)	
		CommodityInfo.bCanNegative = true;
	
	//组合买卖标记
	CommodityInfo.CmbDirectFlag = pCommodityInfo->SpreadDirect;

	//开平信息
	CommodityInfo.cCoverMode = pCommodityInfo->CoverMode;

	switch (pCommodityInfo->PriceDeno)
	{
		case 1:																//对应最小变动价是小数
		{
			CommodityInfo.bIfFractionPrice = false;
			CommodityInfo.dMiniChangePrice = pCommodityInfo->PriceNume;		//最小变动价
			CommodityInfo.iPrecision = pCommodityInfo->PricePrec;
			break;
		}
		default:															//对应最小变动价是分数 形如10/320 分数没有精度 精度没意义 等于多少不知道
		{
			CommodityInfo.bIfFractionPrice = true;
			CommodityInfo.dMiniChangePrice = pCommodityInfo->PriceNume;		//分子变动单位 当整数用
			CommodityInfo.iPrecision = pCommodityInfo->PriceDeno;   		//分母
			break;
		}
	}
	//港交所T+1
	CommodityInfo.bAddOne = CConvert::IsHK(m_sCode.c_str()) ? true : false;

	return true;
}
void CMousewin::UpdatePriceRange()
{
	if (IsSpreadLayOut())
	{
        SSpreadContract sQuote;
        if (g_pStarApi && g_pStarApi->GetSpreadInfo(m_FillOrderInfo.sCode, &sQuote, 1, false) > 0 && sQuote.SnapShot)
        {
            m_sPriceMax = CConvert::GetPriceBySnapShot(sQuote.SnapShot, S_FID_LIMITUPPRICE);
            m_sPriceMin = CConvert::GetPriceBySnapShot(sQuote.SnapShot, S_FID_LIMITDOWNPRICE);
            m_sPriceLast = CConvert::GetPriceBySnapShot(sQuote.SnapShot, S_FID_LASTPRICE);
            return;
        }

		//价格未更新 则重置
		m_sPriceMax = 0;
		m_sPriceMin = 0;
		m_sPriceLast = 0;
	}
	else
	{
		SContract *pQuote = NULL;
		if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteContractId.c_str(), &pQuote, 1, false))
		{
			m_sPriceMax = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITUPPRICE);
			m_sPriceMin = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITDOWNPRICE);
			m_sPriceLast = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
		}
		else
		{
			m_sPriceMax = 0;
			m_sPriceMin = 0;
			m_sPriceLast = 0;
		}
	}
}
void CMousewin::UpdateShowText(int nEditID, bool setfocus)
{
	wchar_t cText[51] = { 0 };
	GetWindowText(m_hControl[nEditID], cText, 51);
	if (setfocus)
	{
		switch (nEditID)
		{
		case CI_UserNo_Edit2:
		case CI_UserNo_Edit3:
		case CI_UserNo_Edit4:
		{
			if (wcscmp(cText, LoadResWchar_t(IDS_AccNo2 + nEditID - CI_UserNo_Edit2)) == 0)
				SetWindowText(m_hControl[nEditID], L"");
		}
		default:
			break;
		}
	}
	else
	{
		switch (nEditID)
		{
		case CI_UserNo_Edit2:
		case CI_UserNo_Edit3:
		case CI_UserNo_Edit4:
		{
			if (wcscmp(cText, L"") == 0)
				SetWindowText(m_hControl[nEditID], LoadResWchar_t(IDS_AccNo2 + nEditID - CI_UserNo_Edit2));
		}
		default:
			break;
		}
	}
	
}
void CMousewin::UpdateConditionData(char cDirect)
{
	wstring wMode = LoadResWchar_t(IDS_STRING1122);
	switch (m_FillOrderAssist.uiTrigMode)
	{
	case fot_Last:
		wMode = LoadResWchar_t(IDS_STRING1122);
		break;
	case fot_Buy:
		wMode = LoadResWchar_t(IDS_BuyPrice);
		break;
	case fot_Sell:
		wMode = LoadResWchar_t(IDS_SellPrice);
		break;
	}
	SendMessage(m_hControl[CI_TrigMode_Combox], CB_SELECTSTRING, -1, (LPARAM)(LPWSTR)wMode.c_str());
	double dPrice = 0;
	GetDefaultTrigPrice(m_sQuoteContractId.c_str(), cDirect, dPrice);
	SetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, dPrice);
	if (cDirect == dBuy)
		SendMessage(m_hControl[CI_TrigMode_Combox2], CB_SELECTSTRING, -1, m_FillOrderAssist.uiTrigCondition == foc_Loss ? (LPARAM)(LPWSTR)L">=" : (LPARAM)(LPWSTR)L"<=");
	else
		SendMessage(m_hControl[CI_TrigMode_Combox2], CB_SELECTSTRING, -1, m_FillOrderAssist.uiTrigCondition == foc_Loss ? (LPARAM)(LPWSTR)L"<=" : (LPARAM)(LPWSTR)L">=");
}
bool CMousewin::GetDefaultTrigPrice(const SContractNoType ContractId,TDirect tdirect, double& dPrice)
{
	//最小变动价
	double dMinPrice = CConvert::GetMinChangePrice(m_CommodityInfo);
	SContract *pQuote = NULL;
	if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteContractId.c_str(), &pQuote, 1, false))
	{
		double dlast = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LASTPRICE);
		double dbuy = CConvert::GetPriceByContract(pQuote, S_FID_BESTBIDPRICE);
		double dsell = CConvert::GetPriceByContract(pQuote, S_FID_BESTASKPRICE);

		switch (m_FillOrderAssist.uiTrigPriceMode)
		{
		case fopm_Last:
			switch (tdirect)
			{
			case dBuy:
				if (!CConvert::DoubleSameValue(0, dlast, FLOAT_SAME_ERRORDIFF))
					dPrice = dlast + dMinPrice*m_FillOrderAssist.uiTrigOverPoint;
				break;
			case dSell:
				if (!CConvert::DoubleSameValue(0, dlast, FLOAT_SAME_ERRORDIFF))
					dPrice = dlast - dMinPrice*m_FillOrderAssist.uiTrigOverPoint;
				break;
			}
			break;
		case fopm_Follow:		
			switch (tdirect)
			{
			case dBuy:
				if (!CConvert::DoubleSameValue(0, dsell, FLOAT_SAME_ERRORDIFF))
					dPrice = dsell + dMinPrice*m_FillOrderAssist.uiTrigOverPoint;
				break;
			case dSell:
				if (!CConvert::DoubleSameValue(0, dbuy, FLOAT_SAME_ERRORDIFF))
					dPrice = dbuy - dMinPrice*m_FillOrderAssist.uiTrigOverPoint;
				break;
			}		
			break;
		case fopm_Work:
			switch (tdirect)
			{
			case dBuy:
				if (!CConvert::DoubleSameValue(0, dbuy, FLOAT_SAME_ERRORDIFF))
					dPrice = dbuy + dMinPrice*m_FillOrderAssist.uiTrigOverPoint;
				break;
			case dSell:
				if (!CConvert::DoubleSameValue(0, dsell, FLOAT_SAME_ERRORDIFF))
					dPrice = dsell - dMinPrice*m_FillOrderAssist.uiTrigOverPoint;
				break;
			}
			break;
		case fopm_Nodefault:
			dPrice = 0;
			break;	
		}
	}
	if (!CConvert::DoubleSameValue(0, dPrice, FLOAT_SAME_ERRORDIFF))
		return false;
	else
		return true;
}
void CMousewin::UpdateFocus(char cDirect, char cOffset)
{
	if (IsWindowVisible(m_cOrderBtn.GetHwnd()))
		m_cOrderBtn.OnSetFocus(cDirect, cOffset);
	
	if (IsWindowVisible(m_hControl[CI_CloseDay_Check]))
	{
		if (cOffset == oCoverT&&m_CheckCloseDay.GetCheck() == BST_UNCHECKED)
			m_CheckCloseDay.SetCheck(BST_CHECKED);
		else if (cOffset == oCover&&m_CheckCloseDay.GetCheck() == BST_CHECKED)
			m_CheckCloseDay.SetCheck(BST_UNCHECKED);
	}
}
void CMousewin::GetPositionVol(const char * UserNo, const TContractKey * ContractKey
	, uint &LongCount, uint &ShortCount, uint &DayLongCount, uint &DayShortCount)
{
	if (!g_pTradeData)
		return;
	if (UserNo == NULL || ContractKey == NULL)
	{
		g_pTradeData->GetPositionVol(NULL, NULL, LongCount, ShortCount, DayLongCount, DayShortCount);
		return;
	}
		
	if (!CConvert::IsCombContract(ContractKey->CommodityType))
		g_pTradeData->GetPositionVol(UserNo, ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	else
	{
		TContractSingleKey SigCont1, SigCont2;
        memset(&SigCont1, 0, sizeof(TContractSingleKey));
        memset(&SigCont2, 0, sizeof(TContractSingleKey));
		CConvert::CombTContractKeyToTContractSingleKey(*ContractKey, SigCont1, SigCont2);
		uint LongCount1 = 0, ShortCount1 = 0, DayLongCount1 = 0, DayShortCount1 = 0;
		uint LongCount2 = 0, ShortCount2 = 0, DayLongCount2 = 0, DayShortCount2 = 0;
		g_pTradeData->GetPositionVol(UserNo, &SigCont1, LongCount1, ShortCount1, DayLongCount1, DayShortCount1);
		g_pTradeData->GetPositionVol(UserNo, &SigCont2, LongCount2, ShortCount2, DayLongCount2, DayShortCount2);
		//多头
		LongCount = LongCount1 > ShortCount2 ? ShortCount2 : LongCount1;
		DayLongCount = DayLongCount1 > DayShortCount2 ? DayShortCount2 : DayLongCount1;

		//空头
		ShortCount = ShortCount1 > LongCount2 ? LongCount2 : ShortCount1;
		DayShortCount = DayShortCount1 > DayLongCount2 ? DayLongCount2 : DayShortCount1;
	}
}
uint CMousewin::GetParOrderExcute()
{
	uint nParQty = 0;
	TDataContainer tempDataCon;
	g_pTradeData->GetParOrder(tempDataCon);
	for (uint i = 0; i < tempDataCon.size(); i++)
	{
		const TOrder* pOrder = ((const TOrder*)(tempDataCon.at(i)));
		if (!pOrder)
			continue;
		if (pOrder->OrderType == otExecute && m_sUserNo[0] == pOrder->UserNo)
		{
			string sCode = "";
			if (CConvert::TContractKeyToTContractNo(*pOrder, sCode) && sCode==m_sCode)
				nParQty += pOrder->OrderQty;	
		}
		else if (strcmp(pOrder->ExchangeNo, exDCE) != 0 && pOrder->OrderType == otAbandon && m_sUserNo[0] == pOrder->UserNo)
		{
			string sCode = "";
			if (CConvert::TContractKeyToTContractNo(*pOrder, sCode) && sCode == m_sCode)
				nParQty += pOrder->OrderQty;
		}
	}
	return nParQty;
}
void CMousewin::ShowCanOrderVol(const TTradableQty * pTradableQty)
{
	//过滤是不是需要显示
	if (!pTradableQty)	
		return;
	//用户名检测
	if (strcmp(pTradableQty->UserNo, m_sUserNo[0].c_str()))	
		return;
	TContractKey tempContractKey;
	memset(&tempContractKey, 0, sizeof(tempContractKey));
	strcpy_s(tempContractKey.Sign, m_CommodityInfo.Sign);
	if (!GetOrderCode(tempContractKey,m_sCode,CI_ContractNo_Edit, FALSE))		
		return;
	if (memcmp(&tempContractKey, pTradableQty, sizeof(TContractKey)))	
		return;
	TMouCanQtyStrW str = { 0 };
	swprintf_s(str, LoadResWchar_t(IDS_STRING1131), pTradableQty->OpenQty + pTradableQty->CoverQty, pTradableQty->CoverQty, pTradableQty->OpenQty);
	if (cmNone == m_CommodityInfo.cCoverMode || cmUnfinish == m_CommodityInfo.cCoverMode)
	{
		if (dBuy == pTradableQty->Direct)
			m_cOrderBtn.UpdateForeignQty(str, dBuy);
		else if (dSell == pTradableQty->Direct)
			m_cOrderBtn.UpdateForeignQty(str, dSell);
	}
}
void CMousewin::ShowOrderFrozen(const TPreFreeze * pPreFreeze)
{
	//过滤是不是需要显示
	if (!pPreFreeze || !pPreFreeze->pCurrency)	
		return;

	if (strcmp(pPreFreeze->UserNo, m_sUserNo[0].c_str()))			
		return;

	TContractKey tempContractKey;
	memset(&tempContractKey, 0, sizeof(tempContractKey));
	strcpy_s(tempContractKey.Sign, m_CommodityInfo.Sign);

	if (!GetOrderCode(tempContractKey, m_sCode, CI_ContractNo_Edit, FALSE))		
		return;

	if (memcmp(&tempContractKey, pPreFreeze, sizeof(TContractKey)))	
		return;

	double dUserDeposit = 0, dBaseDeposit = 0;//客户币种 基币 
	dUserDeposit = pPreFreeze->Deposit;
	dBaseDeposit = pPreFreeze->pCurrency->ExchangeRate * pPreFreeze->Deposit;
	TMouCanQtyStrW str = { 0 };
	TMouCanQtyStrW str2 = { 0 };
	swprintf_s(str, LoadResWchar_t(IDS_STRING1132), dBaseDeposit);
	swprintf_s(str2, L"%s %0.2lf", LoadRC::ansi_to_unicode(pPreFreeze->pCurrency->CurrencyNo).c_str(), dUserDeposit);
	if ((cmNone == m_CommodityInfo.cCoverMode || cmUnfinish == m_CommodityInfo.cCoverMode))
	{
		if (dBuy == pPreFreeze->Direct)		m_cOrderBtn.UpdateForeignPre(str, str2, dBuy);
		else if (dSell == pPreFreeze->Direct)	m_cOrderBtn.UpdateForeignPre(str, str2, dSell);
	}
}
bool CMousewin::GetAvailableMoney(double &dAvailabe)//得到可用资金
{
	const TMoney *pMoney = g_pTradeData->GetMoney(m_sUserNo[0].c_str(), nullptr, Currency_Base, m_sSign[0].c_str());
	if (!pMoney)	
		return false;
	dAvailabe = pMoney->Available;
	return true;
}
bool CMousewin::IsVaild(const TSendOrder *pOrder, double dPrice)
{
	string strKey1, strKey2;
	CConvert::FormatTradeContract(*pOrder, strKey1);
	if (pOrder->Direct == dBuy)
	{
		if (pOrder->OrderQty == 0)//可开可平
		{
			CConvert::FormatTradeContract(m_LastBuyQty, strKey2);
			return (m_LastBuyQty.MaxOrdID == m_MaxOrdID) && CConvert::DoubleSameValue(dPrice, m_LastBuyQty.Available, FLOAT_SAME_ERRORDIFF) &&
				(strcmp(m_LastBuyQty.UserNo, pOrder->UserNo) == 0) && (strKey1 == strKey2) && 
                (pOrder->Direct == m_LastBuyQty.Direct) && 
                (pOrder->Offset == m_LastBuyQty.Offset) && 
                (pOrder->Hedge == m_LastBuyQty.Hedge);
		}
		else//保证金占用
		{
			CConvert::FormatTradeContract(m_LastBuyFreeze, strKey2);
			return (m_LastBuyFreeze.MaxOrdID == m_MaxOrdID) && CConvert::DoubleSameValue(dPrice, m_LastBuyFreeze.Available, FLOAT_SAME_ERRORDIFF) &&
				(strcmp(m_LastBuyFreeze.UserNo, pOrder->UserNo) == 0) && (strKey1 == strKey2) && 
                (pOrder->Direct == m_LastBuyFreeze.Direct) && (pOrder->Offset == m_LastBuyFreeze.Offset)
				&& (pOrder->Hedge == m_LastBuyFreeze.Hedge) && (pOrder->OrderQty == m_LastBuyFreeze.OrderQty)
				&& CConvert::DoubleSameValue(pOrder->OrderPrice, m_LastBuyFreeze.OrderPrice, FLOAT_SAME_ERRORDIFF);
		}
	}
	else
	{
		if (pOrder->OrderQty == 0)//可开可平
		{
			CConvert::FormatTradeContract(m_LastSellQty, strKey2);
			return (m_LastSellQty.MaxOrdID == m_MaxOrdID) && CConvert::DoubleSameValue(dPrice, m_LastSellQty.Available, FLOAT_SAME_ERRORDIFF) &&
				(strcmp(m_LastSellQty.UserNo, pOrder->UserNo) == 0) && (strKey1 == strKey2) && 
                (pOrder->Direct == m_LastSellQty.Direct) && (pOrder->Offset == m_LastSellQty.Offset)
				&& (pOrder->Hedge == m_LastSellQty.Hedge);
		}
		else//保证金占用
		{
			CConvert::FormatTradeContract(m_LastSellFreeze, strKey2);
			return (m_LastSellFreeze.MaxOrdID == m_MaxOrdID) && CConvert::DoubleSameValue(dPrice, m_LastSellFreeze.Available, FLOAT_SAME_ERRORDIFF) &&
				(strcmp(m_LastSellFreeze.UserNo, pOrder->UserNo) == 0) && (strKey1 == strKey2) && 
                (pOrder->Direct == m_LastSellFreeze.Direct) && (pOrder->Offset == m_LastSellFreeze.Offset)
				&& (pOrder->Hedge == m_LastSellFreeze.Hedge) && (pOrder->OrderQty == m_LastSellFreeze.OrderQty)
				&& CConvert::DoubleSameValue(pOrder->OrderPrice, m_LastSellFreeze.OrderPrice, FLOAT_SAME_ERRORDIFF);
		}
	}
	return true;
}
void CMousewin::FormatDisplayQtyAndFreeze(bool bQty, bool bFreeze)
{
	if (IsSpreadLayOut())
		return;

	bool bValidData = true;
	TSendOrder tempSendOrder;
	CConvert::DefaultSendOrder(tempSendOrder);
	//更新sign
	strcpy_s(tempSendOrder.Sign, sizeof(tempSendOrder.Sign), m_sSign[0].c_str());
	//用户账号
	if (!GetOrderUser(tempSendOrder.UserNo, FALSE))		
		bValidData = false;
	//获取商品信息
	string sCode;
	if (!GetOrderCode(tempSendOrder, sCode , CI_ContractNo_Edit, FALSE))			
		bValidData = false;

	if (!bValidData)
	{
		m_cOrderBtn.UpdateForeignQty(L"", dBuy);
		m_cOrderBtn.UpdateForeignQty(L"", dSell);
		m_cOrderBtn.UpdateForeignPre(L"", L"", dBuy);
		m_cOrderBtn.UpdateForeignPre(L"", L"", dSell);
		return;
	}
	//有的后台需要委托价格 有的不需要  全填让API过滤
	double dOrderPrice = 0;
	GetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, dOrderPrice, dBuy, false);
	
	double dAvalibale = 0;
	GetAvailableMoney(dAvalibale);

	if (bQty)
	{
		//买入 可开 可平
		tempSendOrder.Direct = dBuy;
		tempSendOrder.OrderPrice = dOrderPrice;
		if (!IsVaild(&tempSendOrder, dAvalibale))//请求的数据已经有了 直接显示
			g_pTradeApi->QryTradableQty(tempSendOrder);
		else
			ShowCanOrderVol(&m_LastBuyQty);
		//卖出 可开 可平
		tempSendOrder.Direct = dSell;
		tempSendOrder.OrderPrice = dOrderPrice;
		if (!IsVaild(&tempSendOrder, dAvalibale))//请求的数据已经有了 直接显示
			g_pTradeApi->QryTradableQty(tempSendOrder);
		else
			ShowCanOrderVol(&m_LastSellQty);
	}
		
	if (bFreeze)
	{
		//获取委托数量
		uint nQty = 0;
		if (!GetOrderQty(CI_OrderQty_Edit, nQty, false))
		{
			m_cOrderBtn.UpdateForeignPre(L"", L"", dBuy);
			m_cOrderBtn.UpdateForeignPre(L"", L"", dSell);
			return;
		}
		else
			tempSendOrder.OrderQty = nQty;
		//买入 可开 可平
		tempSendOrder.Direct = dBuy;
		tempSendOrder.OrderPrice = dOrderPrice;
		if (!IsVaild(&tempSendOrder, dAvalibale))//请求的数据已经有了 直接显示
			g_pTradeApi->QryPreFreeze(tempSendOrder);
		else
			ShowOrderFrozen(&m_LastBuyFreeze);
		//卖出 可开 可平
		tempSendOrder.Direct = dSell;
		tempSendOrder.OrderPrice = dOrderPrice;
		if (!IsVaild(&tempSendOrder, dAvalibale))//请求的数据已经有了 直接显示
			g_pTradeApi->QryPreFreeze(tempSendOrder);
		else
			ShowOrderFrozen(&m_LastSellFreeze);
	}
	else
	{
		m_cOrderBtn.UpdateForeignPre(L"", L"", dBuy);
		m_cOrderBtn.UpdateForeignPre(L"", L"", dSell);
		return;
	}
}
void CMousewin::OnDealQuoteCommon()
{
	UpdateModifyRecover();//改单界面恢复

	if (m_LockCodeBtn.GetLockState())//锁定合约
		return;

    if (strcmp(m_FillOrderInfo.sCode, "") == 0)
    {
        return;
    }
	//套利合约判断
    SSpreadContract sQuote;
    if (g_pStarApi && g_pStarApi->GetSpreadInfo(m_FillOrderInfo.sCode, &sQuote, 1, false) > 0)
	{
		if (!IsSpreadLayOut())
		{	
			if (m_bCanStrategyTrade)
			{
				if (IsBatchLayOut())
					return;
                ResetFastCheck();//切到套利取消快速

				m_nCurClickItem.nCurHeadType = MHI_Strategys;
				m_nCurClickItem.nCurChildType = CHS_SequSpread;
				UpdateCurHeadStr();
				LayOut();
			}
			else
				return;
		}
	}
	else if (IsSpreadLayOut() || IsOptionStrategyLayOut())
	{
		m_nCurClickItem.nCurHeadType = MHI_Normal;
		m_nCurClickItem.nCurChildType = CHN_Vert;
		UpdateCurHeadStr();
		LayOut();
	}
		
	SetContractNo(m_FillOrderInfo.sCode);//合约

	if (IsWindowVisible(m_StaticPriceSwitch.GetHwnd()))
	{
		if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)) != 0)
			PostMessage(m_Hwnd, WM_ONQUOTE, 0, 0);
		else
			SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, m_FillOrderInfo.dPrice);
	}
	else if (IsSpreadLayOut())
		SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, m_FillOrderInfo.dPrice);

	if (m_FillOrderInfo.nQtyType == 0)
		SetOrderQty(CI_OrderQty_Edit, m_FillOrderInfo.nQty);

	if (IsConditionLayout())
		UpdateConditionData(m_FillOrderInfo.cDirect);
	
	if (m_FillOrderInfo.nClickType == 1&& IsWindowVisible(m_hControl[CI_OrderPrice_Edit]))
		SetFocus(m_hControl[CI_OrderPrice_Edit]);
	
	TContractKey key;
	memset(&key, 0, sizeof(key));
	memcpy(key.Sign, m_CommodityInfo.Sign, sizeof(m_CommodityInfo.Sign));
	if (!CConvert::TContractNoToTContractKey(m_FillOrderInfo.sCode, key)
		|| !CConvert::IsValidTradeCommodity(key))
	{
		if (m_FillOrderInfo.nQtyType == 2)
			SetOrderQty(CI_OrderQty_Edit, m_FillOrderInfo.nOpenQty);
		return;
	}

	TMouUserNoStrA sUser = { 0 };
	if (!GetOrderUser(sUser, FALSE))
		return;
		
	if (m_FillOrderInfo.nQtyType == 2)
	{
		char tOffset = CConvert::GetOffset(sUser, &key, m_FillOrderInfo.cDirect);
		if (tOffset == oCover || tOffset == oCoverT)
			SetOrderQty(CI_OrderQty_Edit, m_FillOrderInfo.nCoverQty);
		else
			SetOrderQty(CI_OrderQty_Edit, m_FillOrderInfo.nOpenQty);
	}
}
void CMousewin::OnDealOrderCommon()									//处理委托 委托 数据
{
	//改单界面恢复
	UpdateModifyRecover();
	//锁定合约	
	string sOrderStr = m_sOrderInfo;
	int npos = sOrderStr.find(';');
	if (npos > 0)
	{
		sOrderStr = sOrderStr.substr(npos + 1, sOrderStr.length());
		npos = sOrderStr.find('=');
		if (npos > 0)
		{
			sOrderStr = sOrderStr.substr(npos + 1, sOrderStr.length());
			//定单信息
			const TOrder *pOrder = g_pTradeData->GetOrder((uint)atoi(sOrderStr.c_str()));
			if (pOrder)
			{
				//账号锁定
				if (m_LockUserNoBtn.GetLockState() && pOrder->UserNo != m_sUserNo[0])
					return;

				string sTradeCode = "";//交易合约	
				//获取交易合约字符串
				const SCommodity * pCommodityInfo = g_pTradeData->GetCommodity(pOrder);
                //TO-DO::确认能否通过"CoverMode == cmUnfinish"确定3M合约
				if (pCommodityInfo&&pCommodityInfo->CoverMode == cmUnfinish)
				{
					char strText[100] = "";
					sprintf_s(strText, "%s %s %s", pOrder->ExchangeNo, pOrder->CommodityNo, _3M);
					sTradeCode = strText;
				}
				else
					CConvert::TContractKeyToTContractNo((TContractKey&)*pOrder, sTradeCode);

				//合约锁定
				if (m_LockCodeBtn.GetLockState()&& sTradeCode!=m_sCode)
					return;
				
				//更新账号
				if (pOrder->UserNo != m_sUserNo[0])
				{
					string str = GetWindowTextAnsic(m_hControl[CI_UserNo_Combox]);
					if (str != pOrder->UserNo&&m_nCurClickItem.nCurHeadType != MHI_BatchOrder)
						UpdateUserInfo(pOrder->UserNo);

					ComBoxStringMsgOperAnsic(m_hControl[CI_UserNo_Combox], CB_SELECTSTRING, -1, pOrder->UserNo);
					OnEditChangeUserNo(CI_UserNo_Combox, true);
				}	

				//有效类型
				m_sValidtype = CMouseString::ValidTypeToWStr(pOrder->ValidType);//有效类型
				if (pOrder->ValidType== vtGTD)
					m_BtnValidType.SetText(LoadRC::ansi_to_unicode(pOrder->ValidTime).c_str());
				else
					m_BtnValidType.SetText(m_sValidtype.c_str());

				int nLastHeadType = m_nCurClickItem.nCurHeadType; 

				//定单类型切换界面
				UpdateShowItemByOrderType(pOrder->OrderType, pOrder->StrategyType);
				if (MHI_BatchOrder == nLastHeadType)
					ReSetUserNos();

				//合约
				if (sTradeCode != m_sCode)
					SetContractNo(sTradeCode.c_str(), cstChange | cstQuote);

				if(pOrder->StrategyType == stCondition)
				{	
					if (pOrder->OrderType == otLimit)
						SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);
					
					SetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, pOrder->TriggerPrice);
					switch (pOrder->TriggerMode)
					{
					case tmLatest:
						SendMessage(m_hControl[CI_TrigMode_Combox], CB_SELECTSTRING, -1, (LPARAM)LoadResWchar_t(IDS_STRING1122));
						break;
					case fot_Buy:
						SendMessage(m_hControl[CI_TrigMode_Combox], CB_SELECTSTRING, -1, (LPARAM)LoadResWchar_t(IDS_BuyPrice));
						break;
					case fot_Sell:
						SendMessage(m_hControl[CI_TrigMode_Combox], CB_SELECTSTRING, -1, (LPARAM)LoadResWchar_t(IDS_SellPrice));
						break;
					}		
					SendMessage(m_hControl[CI_TrigMode_Combox2], CB_SELECTSTRING, -1, pOrder->TriggerCondition == tcLessEqual ? (LPARAM)(LPWSTR)L"<=" : (LPARAM)(LPWSTR)L">=");
				}
				else
				{
					switch (pOrder->OrderType)
					{
					case otIceberg:
						SetOrderQty(CI_MinQty_Edit, pOrder->MinOrderQty);
						SetOrderQty(CI_MaxQty_Edit, pOrder->MaxOrderQty);
						SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);
						break;
					case otMarket:
					case otLimit:
						if (pOrder->OrderType == otLimit)
							SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);					
						break;
					case otLimitStop:				
						SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);
						SetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, pOrder->TriggerPrice);
						break;
					case otMarketStop:
						SetOrderPrice(CI_TriggerPrice_Edit, CI_TriggerPrice_Edit_Mole, pOrder->TriggerPrice);
						break;
					case otGhost:						
						SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);	
						break;
					case otOffer:
						SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, pOrder->OrderPrice);
						SetOrderPrice(CI_SellPrice_Edit, CI_SellPrice_Edit_Mole, pOrder->SellOrderPrice);
						SetWindowTextAnsic(m_hControl[CI_EnquriyNo_Edit],pOrder->EnquiryNo);
						break;
					case otEnquiry:
					case otExecute:
					case otAbandon:
					case otSwap:
					case otHedgApply:
						break;
					case otSpreadApply:
						break;			
					
					default:
						break;
					}
				}	
				

				SetOrderQty(CI_OrderQty_Edit, pOrder->OrderQty);
				if (pOrder->OrderType != otOffer)
					UpdateFocus(pOrder->Direct, pOrder->Offset);
				return;
			}
			else
				return;
		}
		else
			return;
	}
	else
		return;
}
void CMousewin::OnDealSpreadCommon()
{
	string sOrderStr = m_sSpreadInfo;
	int npos = sOrderStr.find(';');
	if (npos > 0)
	{
		sOrderStr = sOrderStr.substr(npos + 1, sOrderStr.length());
		npos = sOrderStr.find('=');
		if (npos > 0)
		{
			sOrderStr = sOrderStr.substr(npos + 1, sOrderStr.length());
			const TSpreadOrder* sp = g_pStrategyApi->GetSpreadOrderInfo(atoi(sOrderStr.c_str()));
			if (sp)
			{
                ResetFastCheck();//切到套利取消快速

				m_nCurClickItem.nCurHeadType = MHI_Strategys;
				if (sp->sendorder.SpreadType== stSpreadSyn)			
					m_nCurClickItem.nCurChildType = CHS_SyncSpread;//同步
				else												
					m_nCurClickItem.nCurChildType = CHS_SequSpread;//先后

				//更新列头
				UpdateCurHeadStr();

				//更新布局
				LayOut();

				//账号信息
				UpdateUserInfo(sp->sendorder.UserNo[0]);
				ComBoxStringMsgOperAnsic(m_hControl[CI_UserNo_Combox], CB_SELECTSTRING, -1, sp->sendorder.UserNo[0]);
				OnEditChangeUserNo(CI_UserNo_Combox, true);
				if (!IsUserNoSync())
				{
					UpdateUserInfo(sp->sendorder.UserNo[1], 1);
					ComBoxStringMsgOperAnsic(m_hControl[CI_UserNo_Combox2], CB_SELECTSTRING, -1, sp->sendorder.UserNo[1]);
                    UpdateUserInfo(sp->sendorder.UserNo[2], 2);
                    ComBoxStringMsgOperAnsic(m_hControl[CI_UserNo_Combox3], CB_SELECTSTRING, -1, sp->sendorder.UserNo[2]);
                    UpdateUserInfo(sp->sendorder.UserNo[3], 3);
                    ComBoxStringMsgOperAnsic(m_hControl[CI_UserNo_Combox4], CB_SELECTSTRING, -1, sp->sendorder.UserNo[3]);
				}
				//数量
				SetOrderQty(CI_OrderQty_Edit, sp->sendorder.OrderQty);
				//触发价
				SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, sp->sendorder.OrderPrice);
				//合约
				SetContractNo(sp->sendorder.ContractNo);
				//条件
				if (m_CheckAdvanced.GetCheck()==BST_CHECKED)
				{
                    if (sp->sendorder.TrigPriceMode == tpm_Match)
                    {
                        m_ComboxSpreadMode.CxSetWindowText(LoadResWchar_t(IDS_MatchSpread));
                    }
                    else if (sp->sendorder.TrigPriceMode == tpm_Work)
                    {
                        m_ComboxSpreadMode.CxSetWindowText(LoadResWchar_t(IDS_WorkSpread));
                    }
                    else if (sp->sendorder.TrigPriceMode == tpm_Last)
                    {
                        m_ComboxSpreadMode.CxSetWindowText(LoadResWchar_t(IDS_STRING1122));
                    }

					m_ComboxSpreadCompare.CxSetWindowText(sp->sendorder.TrigCondition == tcGreaterEqual ? L">=" : L"<=");
				}
				//时间//暂不处理	
				
				UpdateFocus(sp->sendorder.Direct[0], sp->sendorder.Offset[0]);			
			}
		}
	}	
}
void CMousewin::OnDealPositionCommon()
{
	UpdateModifyRecover();//改单界面恢复
	
	//有效类型
	m_sValidtype = CMouseString::ValidTypeToWStr(vtGFD);//有效类型
	m_BtnValidType.SetText(m_sValidtype.c_str());

	if (IsSpreadLayOut()||IsOptionStrategyLayOut())
	{
		m_nCurClickItem.nCurHeadType = MHI_Normal;
		m_nCurClickItem.nCurChildType = CHN_Vert;
		UpdateCurHeadStr();

		LayOut();
	}
	//资金账号
	if (m_UserInfo.size() > 1)
	{	
		if (m_LockUserNoBtn.GetLockState())//
		{
			string str = GetWindowTextAnsic(m_hControl[CI_UserNo_Combox]);
			if (str == m_FillOrderInfo.sUserNo)
			{
				//委托数量
				if (m_FillOrderInfo.nQtyType == 0)
					SetOrderQty(CI_OrderQty_Edit, m_FillOrderInfo.nQty);
			}
		}
		else
		{
			//更新
			string str = GetWindowTextAnsic(m_hControl[CI_UserNo_Combox]);
			if (str != m_FillOrderInfo.sUserNo&&m_nCurClickItem.nCurHeadType != MHI_BatchOrder)
				UpdateUserInfo(m_FillOrderInfo.sUserNo);

			ComBoxStringMsgOperAnsic(m_hControl[CI_UserNo_Combox], CB_SELECTSTRING, -1, m_FillOrderInfo.sUserNo);
			OnEditChangeUserNo(CI_UserNo_Combox, true);
			//委托数量
			if (m_FillOrderInfo.nQtyType == 0)
				SetOrderQty(CI_OrderQty_Edit, m_FillOrderInfo.nQty);
		}					
	}
	else
	{
		//委托数量
		if (m_FillOrderInfo.nQtyType == 0)
			SetOrderQty(CI_OrderQty_Edit, m_FillOrderInfo.nQty);
	}
	//锁定合约	
	if (m_LockCodeBtn.GetLockState())
		return;

	//合约 布局
	SetContractNo(m_FillOrderInfo.sCode, cstChange | cstQuote);

	//委托价格
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE))!=0)
		PostMessage(m_Hwnd, WM_ONQUOTE, 0, 0);
	else if(IsWindowVisible(m_hControl[CI_OrderPrice_Edit]))
		SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, m_FillOrderInfo.dPrice);

	if (IsConditionLayout())
		UpdateConditionData(m_FillOrderInfo.cDirect);

	//双击 按钮获取焦点 平今状态更新
	if (m_FillOrderInfo.nClickType)
		UpdateFocus(m_FillOrderInfo.cDirect, m_FillOrderInfo.cOffset);
}
void CMousewin::OnDealModifyRecover()
{
	if (m_bModifyOrderOper)
	{
		UpdateModifyRecover();//改单界面恢复

		if (g_mainframe)
			g_mainframe->Linkage(m_Hwnd, _CommonModule, W_MODIFY_RECOVER, "");
	}
}
void CMousewin::OnDealOptionStartegy()
{
	ResetFastCheck();
	SetHeadData(LoadResWchar_t(IDS_OptionHead), false);
	
	string sQuote = m_sQuoteOption;//"TYPE&0;ZCE|O|SR|703C7000&B;ZCE|O|SR|703C6500&B;ZCE|O|SR|703C6800&S;ZCE|O|SR|703C6900&B;ZCE|O|SR|703C7100&B";
	vector<NoticeContent> vecNoticeContent;
	CConvert::SplitNotice(sQuote, ';', '&', vecNoticeContent);
		
	int nSize = 0;
	TOptionInfo infor[N_5_row];
	vector<NoticeContent>::iterator iter = vecNoticeContent.begin();
	int cType = atoi(iter->strContent.c_str());
	iter++;
	for (; iter != vecNoticeContent.end()&& nSize < N_5_row; iter++)
	{
		strcpy_s(infor[nSize].Contract, iter->strKey.c_str());
		infor[nSize++].Direct = iter->strContent.at(0);
	}
	m_OptionView.UpdateOptionInfo(nSize, infor, cType);
}
void CMousewin::OnDealOptionExe()
{
	string strContent = m_sExeOption;
	vector<NoticeContent> vecNoticeContent;
	TContractSingleKey sOrderKey;
	if (!CConvert::SplitNoticePos(strContent, ';', vecNoticeContent, 6))
		return;

	vector<string> vecstr = CConvert::SplitString(vecNoticeContent[0].strContent, '|');
	if (vecstr.size() != 6)
		return;
	strcpy_s(sOrderKey.ExchangeNo, vecstr[0].c_str());
	strcpy_s(sOrderKey.CommodityNo, vecstr[1].c_str());
	sOrderKey.CommodityType = vecstr[2][0];
	strcpy_s(sOrderKey.ContractNo, vecstr[3].c_str());
	strcpy_s(sOrderKey.StrikePrice, vecstr[4].c_str());
	sOrderKey.OptionType = vecstr[5][0];
	strcpy_s(sOrderKey.Sign, vecNoticeContent[4].strContent.c_str());
	const TPosition *pPosition = g_pTradeData->GetPositionTotal(vecNoticeContent[1].strContent.c_str(), &sOrderKey, vecNoticeContent[2].strContent[0]);
	if (pPosition)
	{
		SetHeadData(LoadResWchar_t(IDS_EXECUTE_ABANDON), false);
		string sTradeCode = "";//交易合约
		CConvert::TContractKeyToTContractNo((TContractKey&)*pPosition, sTradeCode);
		SetContractNo(sTradeCode.c_str());
		SetWindowInt(m_hControl[CI_OrderQty_Edit], pPosition->PositionQty);
		if (vecNoticeContent[3].strContent=="1")
			SetFocus(m_hControl[CI_Modify_Button]);
		else
			SetFocus(m_hControl[CI_Cancel_Button]);
	}
}
void CMousewin::OnDealKLineContract()		//K线切换处理
{
	UpdateModifyRecover();//改单界面恢复
	
	if (m_LockCodeBtn.GetLockState())//锁定合约
		return;

	string sCode = "";

	if (CConvert::IsValidSpreadContract(m_sQuoteKline.c_str()))
		sCode = m_sQuoteKline;//套利合约
	else
		CConvert::QContractNoTypeToStr(m_sQuoteKline, sCode);//普通合约

	//套利合约判断
	SSpreadContract sQuote;
	if (g_pStarApi && g_pStarApi->GetSpreadInfo(sCode.c_str(), &sQuote, 1, false) > 0)
	{
		if (!IsSpreadLayOut())
		{
			if (m_bCanStrategyTrade)
			{
				if (IsBatchLayOut())
					return;
				ResetFastCheck();//切到套利取消快速

				m_nCurClickItem.nCurHeadType = MHI_Strategys;
				m_nCurClickItem.nCurChildType = CHS_SequSpread;
				UpdateCurHeadStr();
				LayOut();
			}
			else
				return;
		}
	}
	else if (IsSpreadLayOut() || IsOptionStrategyLayOut())
	{
		m_nCurClickItem.nCurHeadType = MHI_Normal;
		m_nCurClickItem.nCurChildType = CHN_Vert;
		UpdateCurHeadStr();
		LayOut();
	}

	SetContractNo(sCode.c_str());	
}
void CMousewin::OnCommonCfgChanged(CM_SUBID PartID, char* pdata)
{
	switch (PartID)
	{
	case AssistFillID:
		OnDealAssistFill((C_FillOrderAssistInfo*)pdata);
		break;
	case DealOrderID:
		OnDealOrderDeal((C_OrderDealInfo*)pdata);
		break;
	case ExpandInfoID:
		OnDealOrderReturn((C_OrderReturnInfo*)pdata);
		break;
	case FastInfoID:
		OnDealFastOrderInfo((C_FastOrderInfo*)pdata);
		break;
	case BatchInfoID:
		OnDealBatchGroupInfo((C_BatchGroupInfo*)pdata);
		break;
	default:
		break;
	}
}
void CMousewin::OnDealAssistFill(const C_FillOrderAssistInfo* pdata)
{
	if (sizeof(C_FillOrderAssistInfo) > pdata->uSize)
	{
		memset(&m_FillOrderAssist, 0, sizeof(C_FillOrderAssistInfo));
		memcpy_s(&m_FillOrderAssist, pdata->uSize, pdata, pdata->uSize);
	}
	else
		memcpy_s(&m_FillOrderAssist, sizeof(C_FillOrderAssistInfo), pdata, sizeof(C_FillOrderAssistInfo));

	m_ComboxUserNo.SetReverFind(m_FillOrderAssist.bBackMatchUserNo);
}
void CMousewin::OnDealOrderDeal(const C_OrderDealInfo* pdata)
{
	if (sizeof(C_OrderDealInfo) > pdata->uSize)
	{
		memset(&m_OrderDealInfo, 0, sizeof(C_OrderDealInfo));
		memcpy_s(&m_OrderDealInfo, pdata->uSize, pdata, pdata->uSize);
	}
	else
		memcpy_s(&m_OrderDealInfo, sizeof(C_OrderDealInfo), pdata, sizeof(C_OrderDealInfo));
	
}
void CMousewin::OnDealOrderReturn(const C_OrderReturnInfo* pdata)
{
	if (sizeof(C_OrderReturnInfo) > pdata->uSize)
	{
		memset(&m_OrderReturn, 0, sizeof(C_OrderReturnInfo));
		memcpy_s(&m_OrderReturn, pdata->uSize, pdata, pdata->uSize);
	}
	else
		memcpy_s(&m_OrderReturn, sizeof(C_OrderReturnInfo), pdata, sizeof(C_OrderReturnInfo));
}
void CMousewin::OnDealFastOrderInfo(const C_FastOrderInfo* pdata)
{
	if (sizeof(C_FastOrderInfo) > pdata->uSize)
	{
		memset(&m_fastOrderInfo, 0, sizeof(C_FastOrderInfo));
		memcpy_s(&m_fastOrderInfo, pdata->uSize, pdata, pdata->uSize);
	}
	else
		m_fastOrderInfo.dChangePoint = pdata->dChangePoint;
}
void CMousewin::OnDealBatchGroupInfo(const C_BatchGroupInfo* pdata)
{
	m_batchGroupVec.clear();
	if (pdata->uMember == 0)
	{
		PostMessage(m_Hwnd, WM_BATCHORDERCHANGE, NULL, NULL);
		return;
	}

	unsigned int nStructSize = ((const BatchGroupInfo*)pdata->pBatchGroupInfo)->uSize;
	if (sizeof(BatchGroupInfo) > nStructSize)
	{
		SetShowTips(L"UnMatched Version");
		return;
	}

	for (unsigned int n = 0; n < pdata->uMember; n++)
	{
		char * pData = (char *)pdata->pBatchGroupInfo;
		pData += nStructSize*n;
		const BatchGroupInfo *p = (const BatchGroupInfo*)pData;
		if (p == NULL)
			return;
		m_batchGroupVec.insert(pair<string, BatchGroupInfo>(p->BatchGroupNo, *p));
	}
	PostMessage(m_Hwnd, WM_BATCHORDERCHANGE, NULL, NULL);
}
wstring CMousewin::GetModulePathEx()
{
	wchar_t wstr[_MAX_PATH];
	GetModuleFileName(NULL, wstr, sizeof(wstr));
	wstring wstr1 = wstr;
	int npos = wstr1.find_last_of(L'\\');
	wstr1 = wstr1.substr(0, npos);
	wstr1 += L"\\config\\";
	return wstr1;
}
void CMousewin::OnFastStateInfo(int nstate)
{
	if (m_CheckFastOrder.GetCheck() == BST_CHECKED)
	{
		ResetFastCheck();
		if (nstate == 1)
		{
			EnableWndStaticOwnerDraw(CI_OrderPrice_Static, FALSE);
			m_CheckFastOrder.SetColor(g_ColorRefData.GetColorBuyRed());
		}			
		else
		{
			EnableWndStaticOwnerDraw(CI_OrderPrice_Static, TRUE);
			m_CheckFastOrder.SetColor(g_ColorRefData.GetColorBlack());
			SetShowTips(L"");
		}			
		LayOut();
	}
	else
	{
		//只更新 控件状态
		if (nstate == 1)
		{
			if (m_CheckFastOrder.GetColor() == g_ColorRefData.GetColorBuyRed())
				return;
			EnableWndStaticOwnerDraw(CI_OrderPrice_Static, FALSE);
			m_CheckFastOrder.SetColor(g_ColorRefData.GetColorBuyRed());
		}
		else
		{
			if (m_CheckFastOrder.GetColor() == g_ColorRefData.GetColorBlack())
				return;
			EnableWndStaticOwnerDraw(CI_OrderPrice_Static, TRUE);
			m_CheckFastOrder.SetColor(g_ColorRefData.GetColorBlack());
			SetShowTips(L"");
		}
		LayOut();
	}
}
bool CMousewin::GetFastInfo(sFastInfo& sInfo)
{ 
	if (m_CheckFastOrder.GetCheck() && IsWindowVisible(m_CheckFastOrder.GetHwnd()))//勾选快速 快速在顶层窗口可见/弹出窗口可见
	{
		TradeLoginLinkage();
		//判断焦点是否是在编辑框中
		HWND hwnd = GetFocus();
		if (hwnd)
		{
			wchar_t w[50] = { 0 };
			GetClassName(hwnd, w, 50);
			if (!wcscmp(L"Edit", w)||!wcscmp(L"SysDateTimePick32",w))
				return false;
		}
		else
			return false;

		sInfo.bFastOrder = true;
		//合约
		string sCode = GetWindowTextAnsic(m_hControl[CI_ContractNo_Edit]);
		TContractKey key;
		memset(&key, 0, sizeof(key));
		memcpy(key.Sign, m_sSign[0].c_str(), sizeof(key.Sign));
		if (CConvert::TContractNoToTContractKey(sCode.c_str(), key) && CConvert::IsValidTradeCommodity(key))//有效交易合约
		{
			//有效资金账号
			if (!GetOrderUser(sInfo.sFastUserNo,false))
				memset(sInfo.sFastUserNo, 0, sizeof(sInfo.sFastUserNo));
			//有效类型
			wstring sText = m_BtnValidType.GetText();
			sInfo.ValidType = CMouseString::WStrToValidType(sText.c_str());
			if (sInfo.ValidType == vtGTD)
			{
				string sDate = LoadRC::unicode_to_ansi(sText);
				strncpy_s(sInfo.ValidTime, sDate.c_str(), sizeof(sInfo.ValidTime));
			}

			//数量倍率
			TMouQtyRateStrW sMulti = { 0 };
			GetWindowTextW(m_hControl[CI_OrderQtyRate_Edit], sMulti, sizeof(TMouQtyRateStrW)/sizeof(wchar_t));
			double nMutil = _wtof(sMulti);
			sInfo.dQtyRatio = (nMutil >= 0 ? nMutil : 0);
			//服务器标识 合约代码
			strncpy_s(sInfo.sFastSign, m_sSign[0].c_str(), sizeof(sInfo.sFastSign));
			strncpy_s(sInfo.sFastCode, sCode.c_str(), sizeof(sInfo.sFastCode));

			//反手
            if (IsWindowVisible(m_hControl[CI_CoverOpen_Check]) && m_CheckCoverOpen.GetCheck() == BST_CHECKED)
            {
                sInfo.bBackHand = true;
            }
            else
            {
                sInfo.bBackHand = false;
            }
			//T+1
			sInfo.bAddOne = IsWindowVisible(m_CheckAddOne.GetHwnd()) && m_CheckAddOne.GetCheck() == BST_CHECKED;
			//批次
			sInfo.bBatchTimes = false;

			//条件 
			if (IsConditionLayout())
			{
				if (!GetConditionInfo(sInfo.cTrigMode, sInfo.cTrigCondition, sInfo.dTrigPrice, false))
				{
					//提示信息
					SetShowTips(LoadResWchar_t(IDS_ConditionError));
					return false;
				}
				else
					sInfo.bcondition = true;
			}
			else
				sInfo.bcondition = false;
			
			//批量
			if (m_nCurClickItem.nCurHeadType == MHI_BatchOrder)
			{
				sInfo.bBatchQuantities = true;
				if (IsWindowVisible(m_CheckAssistOrder.GetHwnd()) && m_CheckAssistOrder.GetCheck() == BST_CHECKED)
					sInfo.bRecove = true;
				else
					sInfo.bRecove = false;

                if (IsWindowVisible(m_hControl[CI_Condition_Check]) && m_CheckCondition.GetCheck() == BST_CHECKED)
                {
                    if (!GetConditionInfo(sInfo.cTrigMode, sInfo.cTrigCondition, sInfo.dTrigPrice, false))
                    {
                        //提示信息
                        SetShowTips(LoadResWchar_t(IDS_ConditionError));
                        return false;
                    }
                    sInfo.bcondition = true;
                }
			}
			else
				sInfo.bBatchQuantities = false;

			return true;
		}
		else
		{
			//提示信息
			SetShowTips(LoadResWchar_t(IDS_STRING1109));
			return false;
		}
	}
	return false;
}
void CMousewin::SetFastControlState()
{
	bool bcheck = m_bTradeLogined || m_CheckFastOrder.GetColor() != g_ColorRefData.GetColorBuyRed();
	
	EnableWindow(m_LockUserNoBtn.GetHwnd(), bcheck);
	EnableWindow(m_LockCodeBtn.GetHwnd(), bcheck);
	EnableWindow(m_SyncUserNoBtn.GetHwnd(), bcheck);
	EnableWindow(m_SearchBtn.GetHwnd(), bcheck);
	//资金账号
	if (m_LockUserNoBtn.GetLockState() && IsWindowVisible(m_LockUserNoBtn.GetHwnd()))
		EnableWindow(m_hControl[CI_UserNo_Combox], false);
	else
	{
		EnableWindow(m_hControl[CI_UserNo_Combox], bcheck);
		if (!bcheck)
			Edit_SetSel(m_ComboxUserNo.m_Edit.GetHwnd(), 0, 0);
	}
    if (m_SyncUserNoBtn.GetLockState() && IsWindowVisible(m_SyncUserNoBtn.GetHwnd()))
    {
        EnableWindow(m_hControl[CI_UserNo_Combox2], false);
        EnableWindow(m_hControl[CI_UserNo_Combox3], false);
        EnableWindow(m_hControl[CI_UserNo_Combox4], false);
    }
	else
	{
		EnableWindow(m_hControl[CI_UserNo_Combox2], bcheck);
        EnableWindow(m_hControl[CI_UserNo_Combox3], bcheck);
        EnableWindow(m_hControl[CI_UserNo_Combox4], bcheck);
        if (!bcheck)
        {
            Edit_SetSel(m_ComboxUserNo2.m_Edit.GetHwnd(), 0, 0);
            Edit_SetSel(m_ComboxUserNo3.m_Edit.GetHwnd(), 0, 0);
            Edit_SetSel(m_ComboxUserNo4.m_Edit.GetHwnd(), 0, 0);
        }
	}
	//合约代码
	if (m_LockCodeBtn.GetLockState())
	{
		EnableWindow(m_hControl[CI_ContractNo_Edit_Win], false);
		EnableWindow(m_hControl[CI_ContractNo_Static], false);
	}		
	else
	{
		EnableWindow(m_hControl[CI_ContractNo_Edit_Win], bcheck);
		EnableWindow(m_hControl[CI_ContractNo_Static], true);
	}	

	EnableWindow(m_hControl[CI_OrderQty_Edit], bcheck);
	EnableWindow(m_hControl[CI_OrderQty_Spin], bcheck);
	EnableWindow(m_hControl[CI_OrderQtyRate_Edit], bcheck);
	EnableWndStaticOwnerDraw(CI_OrderPrice_Static, bcheck && !(m_nCurClickItem.nCurHeadType == MHI_Strategys || (m_nCurClickItem.nCurHeadType == MHI_Advanced&&m_nCurClickItem.nCurChildType!=CHA_Condition)));
	EnableWndStaticOwnerDraw(CI_SpreadSwitch_Static, bcheck);
	EnableWindow(m_hControl[CI_OrderPrice_Edit], bcheck&&!IsSpreadTriggered());
	EnableWindow(m_hControl[CI_OverPoint_Edit_Win], bcheck);
	EnableWindow(m_hControl[CI_OrderPrice_Edit_Mole], bcheck);
	EnableWndStaticOwnerDraw(CI_SpreadTrigMode_Combox, bcheck);
	EnableWndStaticOwnerDraw(CI_SpreadTrigMode_Combox2, bcheck);
	EnableWindow(m_hControl[CI_SellPrice_Edit], bcheck);
	EnableWindow(m_hControl[CI_SellPrice_Edit_Mole], bcheck);
	EnableWindow(m_hControl[CI_SellPrice_Spin], bcheck);
	EnableWndStaticOwnerDraw(CI_ValidStyle_Button, bcheck);
	EnableWindow(m_hControl[CI_TriggerPrice_Edit], bcheck);
	EnableWindow(m_hControl[CI_TriggerPrice_Edit_Mole], bcheck);
	EnableWndStaticOwnerDraw(CI_TrigMode_Combox, bcheck);
	EnableWndStaticOwnerDraw(CI_TrigMode_Combox2, bcheck);
	EnableWindow(m_hControl[CI_MaxQty_Edit], bcheck);
	EnableWindow(m_hControl[CI_MinQty_Edit], bcheck);
	EnableWindow(m_hControl[CI_StopLoss_Edit], bcheck);
	EnableWindow(m_hControl[CI_StopLoss_Edit_Mole], bcheck);
	EnableWindow(m_hControl[CI_StopProfit_Edit], bcheck);
	EnableWindow(m_hControl[CI_StopProfit_Edit_Mole], bcheck);
	EnableWindow(m_ComboxMode.GetHwnd(), bcheck);
	EnableWindow(m_ComboxCompare.GetHwnd(), bcheck);	
	EnableWindow(m_hControl[CI_SpreadBeginTime_Picker], bcheck);
	EnableWindow(m_hControl[CI_SpreadBeignTime_Static], bcheck);
    EnableWindow(m_hControl[CI_SpreadEndTime_Picker], bcheck);
    EnableWindow(m_hControl[CI_SpreadEndTime_Static], bcheck);

}
void CMousewin::SetPriceControlState()
{
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)) == 0)//委托价格 控件状态重置
	{
		bool benable = (m_bTradeLogined || m_CheckFastOrder.GetColor() != g_ColorRefData.GetColorBuyRed())&&!IsSpreadTriggered();//有快速勾选
		EnableWindow(m_hControl[CI_OrderPrice_Edit], benable);
		EnableWindow(m_hControl[CI_OrderPrice_Edit_Mole], benable);
		EnableWindow(m_hControl[CI_OrderPrice_Spin], benable);
		OnSelChangeEditPrice(CI_OrderPrice_Edit);
	}
	else
	{
		EnableWindow(m_hControl[CI_OrderPrice_Edit], FALSE);
		EnableWindow(m_hControl[CI_OrderPrice_Spin], FALSE);
		EnableWindow(m_hControl[CI_OrderPrice_Edit_Mole], FALSE);
	}
}
//快速下单错误信息反馈
void CMousewin::OnFastError(const ErrorReflectInfo* eInfo)
{
	if (eInfo)
		SetShowTips(eInfo->error_text);
	else
		SetShowTips(L"");
}
//快速下单回填订单信息
void CMousewin::OnFastOrderInfo(const TSendOrder* TSOrder, double dRatio)
{
	if (TSOrder)
	{
		SetOrderPrice(CI_OrderPrice_Edit, CI_OrderPrice_Edit_Mole, TSOrder->OrderPrice);
		SetWindowInt(m_hControl[CI_OrderQty_Edit], TSOrder->OrderQty);
	}
	//数量倍率
	if (!CConvert::DoubleSameValue(dRatio, 0, FLOAT_SAME_ERRORDIFF))
		SetWindowDouble(m_hControl[CI_OrderQtyRate_Edit], 1, dRatio);
	SetShowTips(L"");
}

wstring CMousewin::FormatUserSummary(const TUserInfo* tInfo)
{
	if (!tInfo)
		return L"";
	wstring sInfo = L"";
	TMouUserSummuryStrW cText = { 0 };
	swprintf_s(cText, LoadResWchar_t(IDS_UserNoInfo), LoadRC::ansi_to_unicode(tInfo->UserNo).c_str());
	sInfo = cText;
	if (strcmp(tInfo->FullName,""))
	{
		swprintf_s(cText, LoadResWchar_t(IDS_UserNameInfo), LoadRC::ansi_to_unicode(tInfo->FullName).c_str());
		sInfo = sInfo + L"\n" + cText;
	}
	if (strcmp(tInfo->Phone, ""))
	{
		swprintf_s(cText, LoadResWchar_t(IDS_PhoneInfo), LoadRC::ansi_to_unicode(tInfo->Phone).c_str());
		sInfo = sInfo + L"\n" + cText;
	}
	if (strcmp(tInfo->IdentigyNo, ""))
	{
		swprintf_s(cText, LoadResWchar_t(IDS_IdentifyInfo), LoadRC::ansi_to_unicode(tInfo->IdentigyNo).c_str());
		sInfo = sInfo + L"\n" + cText;
	}
	if (strcmp(tInfo->Address, ""))
	{
		swprintf_s(cText, LoadResWchar_t(IDS_AddressInfo), LoadRC::ansi_to_unicode(tInfo->Address).c_str());
		sInfo = sInfo + L"\n" + cText;
	}
	if (strcmp(tInfo->Remark, ""))
	{
		swprintf_s(cText, LoadResWchar_t(IDS_RemarkInfo), LoadRC::ansi_to_unicode(tInfo->Remark).c_str());
		sInfo = sInfo + L"\n" + cText;
	}
	return sInfo;
}
void CMousewin::ResetFastCheck()
{
	if (m_CheckFastOrder.GetCheck() == BST_CHECKED)
		m_CheckFastOrder.SetCheck(BST_UNCHECKED);
}
void CMousewin::ResetPriceLevel()
{
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE))!=0)
	{
		m_StaticPriceSwitch.CxSetWindowText(LoadResWchar_t(IDS_LIMIT_PRICE));
		m_StaticPriceSwitch.CxGetWindowText(m_sPricetype);
	}
}

void CMousewin::SetStopPreHangOrder(const TSendOrder &TOrderInfo, TVecInt& vOrderID)
{
	if (g_pStrategyApi&&m_bStopPreHangOrder&&m_StopType)
	{
		if (TOrderInfo.Offset == oOpen || TOrderInfo.Offset == oNone)
		{
			TOrderStopInfo torderinfo;
			memset(&torderinfo, 0, sizeof(TOrderStopInfo));
			CConvert::TradeContractToQContractNoType(TOrderInfo, torderinfo.item.Code);
			strcpy_s(torderinfo.item.Sign, TOrderInfo.Sign);
			strcpy_s(torderinfo.item.UserNo, TOrderInfo.UserNo);
			torderinfo.item.Direct = TOrderInfo.Direct;

			if (CConvert::IsValidQuotContract(torderinfo.item.Code))
			{
				torderinfo.item.stype = m_StopType;
				torderinfo.item.dBasePrice = TOrderInfo.OrderPrice;
				torderinfo.item.dStopLossPrice = m_dLossPrice;
				torderinfo.item.dStopProfitPrice = m_dProfitPrice;
				torderinfo.item.dStopKeepPrice = 0;
				torderinfo.item.dFloatPrice = 0;

				for (uint i = 0; i < vOrderID.size(); i++)
				{
					torderinfo.OrderID[torderinfo.uCount++] = vOrderID.at(i);
					if (torderinfo.uCount >= 100)
					{
						g_pStrategyApi->AddOrderStopRecord(torderinfo);
						memset(torderinfo.OrderID, 0, sizeof(int) * 100);
						torderinfo.uCount = 0;
					}
				}
				if (torderinfo.uCount != 0)
					g_pStrategyApi->AddOrderStopRecord(torderinfo);
			}
			else
				SetShowTips(LoadResWchar_t(IDS_InvalidQuoteCode));
		}
		else
			SetShowTips(LoadResWchar_t(IDS_CoverConflictStop));
	}
}
bool CMousewin::DoubleSameOrExceed(double dFirst, double dSecond, bool bFirst)
{
	if (bFirst)
		return dFirst>dSecond || CConvert::DoubleSameValue(dFirst, dSecond, FLOAT_SAME_ERRORDIFF);
	else
		return dFirst<dSecond || CConvert::DoubleSameValue(dFirst, dSecond, FLOAT_SAME_ERRORDIFF);
}
bool CMousewin::DoubleExceed(double dFirst, double dSecond, bool bFirst)
{
	if (bFirst)
		return dFirst > dSecond && !CConvert::DoubleSameValue(dFirst, dSecond, FLOAT_SAME_ERRORDIFF);
	else
		return dFirst < dSecond && !CConvert::DoubleSameValue(dFirst, dSecond, FLOAT_SAME_ERRORDIFF);
}
bool CMousewin::GetStopData(TDirect direct, double dprice)
{
	m_StopType = 0;
	GetDoublePrice(CI_StopLoss_Edit, CI_StopLoss_Edit_Mole, m_dLossPrice);
	if (!CConvert::DoubleSameValue(0, m_dLossPrice, FLOAT_SAME_ERRORDIFF))
	{
		if (!CheckOrderPrice(m_dLossPrice, true, mptLoss))
			return false;
		if (DoubleSameOrExceed(m_dLossPrice, dprice,direct==dBuy))
		{
			if (direct == dBuy)
				MessageBoxW(m_Hwnd, LoadResWchar_t(IDS_BuyLossTip), LoadResWchar_t(IDS_TIP), NULL);
			else
				MessageBoxW(m_Hwnd, LoadResWchar_t(IDS_SellLossTip), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
		m_StopType = m_StopType | stLoss;
	}
	GetDoublePrice(CI_StopProfit_Edit, CI_StopProfit_Edit_Mole, m_dProfitPrice);
	if (!CConvert::DoubleSameValue(0, m_dProfitPrice, FLOAT_SAME_ERRORDIFF))
	{
		if (!CheckOrderPrice(m_dProfitPrice, true, mptProfit))
			return false;
		if (DoubleSameOrExceed(m_dProfitPrice, dprice, direct == dSell))
		{
			if (direct == dBuy)
				MessageBoxW(m_Hwnd, LoadResWchar_t(IDS_BuyProfitTip), LoadResWchar_t(IDS_TIP), NULL);
			else
				MessageBoxW(m_Hwnd, LoadResWchar_t(IDS_SellProfitTip), LoadResWchar_t(IDS_TIP), NULL);
			return false;
		}
		m_StopType = m_StopType | stProfit;
	}
	if (m_StopType==0)
		MessageBoxW(m_Hwnd, LoadResWchar_t(IDS_InvalidStopPrice), LoadResWchar_t(IDS_TIP), NULL);

	return m_StopType?true:false;
}
void CMousewin::ResetStopContent()
{
	SetWindowTextW(m_hControl[CI_StopLoss_Edit], L"");
	SetWindowTextW(m_hControl[CI_StopProfit_Edit], L"");
	SetWindowTextW(m_hControl[CI_StopLoss_Edit_Mole], L"");
	SetWindowTextW(m_hControl[CI_StopProfit_Edit_Mole], L"");
}
void CMousewin::CodeUpdateCommodityInfo()
{
	//套利界面
	if (IsSpreadLayOut())
	{
        SSpreadContract sQuote;
        if (g_pStarApi && !m_sSpreadCode.empty() && g_pStarApi->GetSpreadInfo(m_sSpreadCode.c_str(), &sQuote, 1, false) > 0)
		{
            m_CommodityInfo.cCoverMode = sQuote.CoverMode;
            m_CommodityInfo.dMiniChangePrice = sQuote.PriceTick;
            m_CommodityInfo.iPrecision = sQuote.PricePrec;
            m_CommodityInfo.bIfFractionPrice = false;
            m_CommodityInfo.bCanNegative = true;
		}	
        else
        {
            m_CommodityInfo.iPrecision = 4;
            m_CommodityInfo.dMiniChangePrice = 0.0001;
            m_CommodityInfo.bIfFractionPrice = false;
            m_CommodityInfo.bCanNegative = true;
        }
	}
	//普通界面
	else
	{
		TContractKey key;
		memset(&key, 0, sizeof(key));
		memcpy(key.Sign, m_CommodityInfo.Sign, sizeof(m_CommodityInfo.Sign));
		if (CConvert::TContractNoToTContractKey(m_sCode.c_str(), key) && CConvert::IsValidTradeCommodity(key))//有效交易合约
		{
			//更新交易品种信息
			if (!GetTradeCommodityInf(key, m_CommodityInfo))
				return;
		}
		else
		{
			//更新行情品种信息
			if (!CConvert::GetQuoteCommodityInf(m_sCode.c_str(), m_CommodityInfo))
				return;
		}
	}
}
void CMousewin::UpdateQuoteContract(const SContractNoType sQuote, CodeSetType csType)
{
	//通知行情切换
	if ((csType&cstQuote))
	{
		string str = sQuote;
		str = "contractid=" + str;
		g_mainframe->Linkage(m_Hwnd, "MouseOrder", W_SETCONTRACT, str.c_str());
	}

	//取消订阅
	if (!m_sQuoteContractId.empty() && m_sQuoteContractId != sQuote)
	{
		SContractNoType qCode = { 0 };
		memcpy(qCode, m_sQuoteContractId.c_str(), sizeof(SContractNoType));
		g_pStarApi->UnsubQuote(&qCode, 1, this);
		m_sQuoteContractId = "";
		if (IsWindowVisible(m_hControl[CI_StopLoss_Edit]))
			ResetStopContent();
	}

	//订阅
	SContractNoType qQuote = { 0 };
	memcpy(qQuote, sQuote, sizeof(SContractNoType));
	g_pStarApi->SubQuote(&qQuote, 1, this);
	m_sQuoteContractId = sQuote;
}
bool CMousewin::UpdateOptionTarget()
{
	SContractNoType sTarget = { 0 };
	
	TContractKey key;
	memset(&key, 0, sizeof(key));
	if (CConvert::TContractNoToTContractKey(m_sCode.c_str(), key))
	{
		if (key.CommodityType == ctOption)
		{
			SContractNoType sQuote = { 0 };
			if (CConvert::TradeContractToOptionTarget(key, sQuote))
			{
				SContract* qContract[1];
				if (g_pStarApi->GetOptionSeriesTarget(sQuote, "", qContract, 1, false))
					memcpy(sTarget, qContract[0]->ContractNo,sizeof(SContractNoType));
			}
		}
	}
	
	if (!m_sOptionTarget.empty()&&sTarget != m_sOptionTarget)
	{
		SContractNoType qTarget = { 0 };
		memcpy(qTarget, m_sOptionTarget.c_str(), sizeof(SContractNoType));
        g_pStarApi->UnsubQuote(&qTarget, 1, this);
		m_sOptionTarget = "";
	}
	//订阅
	m_sOptionTarget = sTarget;
	if (!m_sOptionTarget.empty())
	{
        g_pStarApi->SubQuote(&sTarget, 1, this);
		m_OptionInfo.cOptionType = key.OptionType;
		m_OptionInfo.dStrikePrice = atof(key.StrikePrice);
		PostMessage(m_Hwnd,WM_ONQUOTETARGET,0,0);
	}
	else
	{
		memset(&m_OptionInfo, 0, sizeof(m_OptionInfo));
		m_OptionInfo.cOptionType = oNone;
	}
	return false;
}
void CMousewin::UpdateCodeEdit(int nID)
{
	SetFuzzyQuery(false);
	SetWindowTextAnsic(m_hControl[CI_ContractNo_Edit], m_sCode.c_str());
    //清理套利合约代码
    SetWindowText(m_hControl[CI_Spread_Edit], L"");
    m_sSpreadCode = "";
	SetFuzzyQuery(true);
}
void CMousewin::UpdateSpreadCodeEdit(SSpreadContract &sQuote)
{
    SetFuzzyQuery(false);  
	SContractNoType sCode = {0};
	g_pStarApi->GetContractCode(sQuote.ContractNo, sCode);
    SetWindowTextAnsic(m_hControl[CI_Spread_Edit], sCode);
    //清理非套利合约代码
    SetWindowTextAnsic(m_hControl[CI_ContractNo_Edit], "");
    m_sCode = "";
    SetFuzzyQuery(true);
}
void CMousewin::UpdateExchangeSupport(bool bCheck)
{
	m_wProperty.bExchangeSup = bCheck;
	UpdatePropertyCfg();
}
void CMousewin::GetProperty(char(&cProperty)[MAX_PATH])
{
	m_wProperty.nMouseHead = m_nCurClickItem.nCurHeadType;
	m_wProperty.nChildHead = m_nCurClickItem.nCurChildType;
    sprintf_s(cProperty, "FastCheck=%d;AutoCover=%d;CoverMode=%c;AddOne=%d;ChaseOrder=%d;Advanced=%d;ExchangeSupport=%d;MouseHead=%d;ChildHead=%d;HeadItems=%d|%d|%d|%d|%d|%d|%d;RightCancel=%d;CoverOpen=%d;Condition=%d;",
        m_CheckFastOrder.GetCheck() == BST_CHECKED ? 1 : 0,
		m_CheckAutoDirect.GetCheck() == BST_CHECKED ? 1 : 0,
		(m_CommodityInfo.cCoverMode == oCover || m_CommodityInfo.cCoverMode == oCoverT) ? oCover : oNone,
		m_CheckAddOne.GetCheck() == BST_CHECKED ? 1 : 0, 
        m_CheckAssistOrder.GetCheck() == BST_CHECKED ? 1 : 0, 
        m_CheckAdvanced.GetCheck() == BST_CHECKED ? 1 : 0, 
        m_wProperty.bExchangeSup ? 1 : 0, 
        m_wProperty.nMouseHead, 
        m_wProperty.nChildHead,
		m_HeadCurInfo.nSize, 
        m_HeadCurInfo.tHeadInfo[0].nCurHeadType, 
        m_HeadCurInfo.tHeadInfo[0].nCurChildType, 
        m_HeadCurInfo.tHeadInfo[1].nCurHeadType, 
        m_HeadCurInfo.tHeadInfo[1].nCurChildType, 
        m_HeadCurInfo.tHeadInfo[2].nCurHeadType, 
        m_HeadCurInfo.tHeadInfo[2].nCurChildType,
        m_CheckRightCancel.GetCheck() == BST_CHECKED ? 1 : 0,
        m_CheckCoverOpen.GetCheck() == BST_CHECKED ? 1 : 0,
        m_CheckCondition.GetCheck() == BST_CHECKED ? 1 : 0);
}
void CMousewin::UpdatePropertyCfg()
{
	char cProperty[MAX_PATH] = {0};
	GetProperty(cProperty);
	if (g_mainframe&&!g_MousePopwin.IsGlobalMousePop(m_Hwnd))
		g_mainframe->SetProperty(m_Hwnd, cProperty);
}
void CMousewin::UpdatePropertyCurHead()
{
	//策略单库剔除 重置列头选中项
	if (m_wProperty.nMouseHead == MHI_Strategys )
	{
		if (!g_pStrategyApi
			|| ((m_wProperty.nChildHead == CHS_SyncSpread|| m_wProperty.nChildHead == CHS_SequSpread)&&!(g_pStrategyApi->GetStartegyWorkState()&SWS_SpreadTrade))
			|| (m_wProperty.nChildHead == CHS_PCStop) && !(g_pStrategyApi->GetStartegyWorkState()&SWS_StopTrade))
		{
			m_wProperty.nMouseHead = MHI_Normal;
			m_wProperty.nChildHead = CHN_Vert;
		}	
	}
	else if (m_wProperty.nMouseHead == MHI_BatchOrder && !m_MouseCfg.bShowBatchOrder)
	{
		m_wProperty.nMouseHead = MHI_Normal;
		m_wProperty.nChildHead = CHN_Vert;
	}

	switch (m_wProperty.nMouseHead)
	{
	case MHI_Advanced:
		m_nCurClickItem.nCurHeadType = MHI_Advanced;
		if((m_MouseCfg.cFlag[Type_IceBerg] == 0 && m_nCurClickItem.nCurChildType == CHA_Iceberg)||
			(m_MouseCfg.cFlag[Type_Shadow] == 0 && m_nCurClickItem.nCurChildType == CHA_Shadow))
			m_nCurClickItem.nCurChildType = CHA_LimtStop;
		else
			m_nCurClickItem.nCurChildType = m_wProperty.nChildHead;
		break;
	case MHI_Strategys:
		m_nCurClickItem.nCurHeadType = MHI_Strategys;
		m_nCurClickItem.nCurChildType = m_wProperty.nChildHead;
		break;
	default:
		m_nCurClickItem.nCurHeadType = MHI_Normal;
		if (m_wProperty.nChildHead == CHN_Ladder && m_MouseCfg.bShowLadder)
			m_nCurClickItem.nCurChildType = m_wProperty.nChildHead;
		else if (m_wProperty.nChildHead == CHN_Option)
			m_nCurClickItem.nCurChildType = m_wProperty.nChildHead;
		else
			m_nCurClickItem.nCurChildType = CHN_Vert;
		break;
	}
	UpdateCurHeadStr();
}
void CMousewin::UpdateCurHeadStr()			//更新选中项
{
	TMouPriceTypeStrW str = { 0 };
	if (GetShowStrByItem(m_nCurClickItem, str))
		m_cHeadWin.SetShowData(str);

    if (IsSwapLayOut())
    {
        m_cOrderBtn.SetBShitPosition(true);
    }
    else if (IsSpreadLayOut())
    {
        m_cOrderBtn.SetBShitPosition(m_CheckMovePosition.GetCheck() == BST_CHECKED);
    }
    else
    {
        m_cOrderBtn.SetBShitPosition(false);
    }
}
void CMousewin::UpdatePropertyControl()
{
	if (m_wProperty.bCheckFast)
		m_CheckFastOrder.SetCheck(BST_CHECKED);
	if (m_wProperty.bCheckAutoCOver)
		m_CheckAutoDirect.SetCheck(BST_CHECKED);	
	if (m_wProperty.bAddOne)
		m_CheckAddOne.SetCheck(BST_CHECKED);
	if (m_wProperty.bChaseOrder&&g_pStrategyApi)
		m_CheckAssistOrder.SetCheck(BST_CHECKED);
	if (m_wProperty.bAdvanced&&g_pStrategyApi)
		m_CheckAdvanced.SetCheck(BST_CHECKED);
	if (m_wProperty.bExchangeSup)
		m_OptionView.SetExchangeSupport(m_wProperty.bExchangeSup);
    if (m_wProperty.bRightCancel)
    {
        m_CheckRightCancel.SetCheck(BST_CHECKED);
    }
    if (m_wProperty.bCoverOpen)
    {
        m_CheckCoverOpen.SetCheck(BST_CHECKED);
    }
    if (m_wProperty.bCondition)
    {
        m_CheckCondition.SetCheck(BST_CHECKED);
    }
}
void  CMousewin::UpdateSearchLayOut()						//更新列头
{
	m_cPopSearch.UpdateLayout(m_LoginType, m_bCanStrategyTrade || m_bCanBatchOrder);
}
void  CMousewin::UpdatePropertyToHeadItem()
{
	vector<string> vInfo;
	vInfo = CConvert::SplitString(m_wProperty.HeadItemsInfo, '|');
	if (vInfo.size()==7)
	{
		int i = 0;
		m_HeadCurInfo.nSize = atoi(vInfo[i++].c_str());//列头数
		for (int j = 0;j<m_HeadCurInfo.nSize;j++)
		{
			m_HeadCurInfo.tHeadInfo[j].nCurHeadType = atoi(vInfo[i++].c_str());
			m_HeadCurInfo.tHeadInfo[j].nCurChildType = atoi(vInfo[i++].c_str());
		}
	}
	//更新头
	TMouPriceTypeStrW wstr[10] = { 0 };
	int i = 0;
	for (; i < m_HeadCurInfo.nSize; i++)
		GetShowStrByItem(m_HeadCurInfo.tHeadInfo[i], wstr[i]);

	if (i==0)
	{
		wcscpy_s(wstr[i], LoadResWchar_t(IDS_VertOrder));
		GetShowItemByStr(m_HeadCurInfo.tHeadInfo[i++], LoadResWchar_t(IDS_VertOrder));
		if (m_MouseCfg.bShowLadder)
		{
			wcscpy_s(wstr[i], LoadResWchar_t(IDS_LadderOrder));
			GetShowItemByStr(m_HeadCurInfo.tHeadInfo[i++], LoadResWchar_t(IDS_LadderOrder));
		}
		else
		{
			wcscpy_s(wstr[i], LoadResWchar_t(IDS_OptionHead));
			GetShowItemByStr(m_HeadCurInfo.tHeadInfo[i++], LoadResWchar_t(IDS_OptionHead));
		}
	}
	m_cHeadWin.UpdateData(i, wstr);
	OnSizeHeadWidth();
	m_cPopSearch.UpdateData(i, wstr);
}
void  CMousewin::UpdateSelectInfo(TMouPriceTypeStrW str[N_Search_Size], int nSize,bool bClearSpecial)													//更新列头
{
	//固定列排序
	map<string, wstring> strMap;
	for (int i=0;i<nSize;i++)
	{
		TMouPriceStrA strA;
		TClickHeadInfo hInfo;
		GetShowItemByStr(hInfo, str[i]);
		sprintf_s(strA, "%d%02d", hInfo.nCurHeadType, hInfo.nCurChildType);
		strMap.insert(pair<string,wstring>(strA, str[i]));	
	}
	
	TMouPriceTypeStrW strtemp[N_Search_Size] = { 0 };
	int n = 0;
	for (map<string, wstring>::iterator iter = strMap.begin(); iter != strMap.end()&&n<N_Search_Size; iter++)
	{
		TClickHeadInfo hInfo;
		GetShowItemByStr(hInfo, iter->second.c_str());
		m_HeadCurInfo.tHeadInfo[n] = hInfo;
		memcpy(strtemp[n++], iter->second.c_str(), sizeof(TMouPriceTypeStrW));
	}
	m_HeadCurInfo.nSize = n;
	m_cHeadWin.UpdateData(n, strtemp, bClearSpecial);

	UpdatePropertyCfg();
}
void CMousewin::OnSizeHeadWidth()
{
	if (IsWindow(m_hControl[CI_Head_Button]))
	{
		RECT rect;
		GetWindowRect(m_hControl[CI_Head_Button], &rect);
		int nStart = rect.right - rect.left;
		MoveCtrlColWindow(m_hControl[CI_Head_Search], nStart);
		if (nStart < N_MOUSEHEAD_WIDTH)
			m_SearchBtn.SetLockState(false);
		else
			m_SearchBtn.SetLockState(true);
	}
}
void CMousewin::GetPopTopLeftAxis(const int nWidth, int &left,int &nTop)
{
	RECT rect;
	GetWindowRect(m_Hwnd, &rect);
	RECT rect2;
	GetWindowRect(m_hControl[CI_Head_Search], &rect2);
	left = rect2.right - nWidth < rect.left ? rect.left : rect2.right - nWidth;
	nTop = rect2.bottom;
}
void CMousewin::UpdateCheckState()							//更新快速勾选
{
	if (m_CheckFastOrder.GetCheck() == BST_CHECKED)
		m_CheckFastOrder.SetCheck(BST_UNCHECKED);

	if (m_CheckForceClose.GetCheck() == BST_CHECKED)
		m_CheckForceClose.SetCheck(BST_UNCHECKED);
}
OrderButtonType  CMousewin::GetModeType()
{
	if (IsInnerCommodity() && IsOfferLayOut())
		return obInnerOffer;
	else if (IsInnerCommodity() && (!m_bAutoCover||IsBatchLayOut()) && !IsStopLayOut())
	{
		if (IsSwapLayOut())
			return obInnerSwap;
		else
			return obInner;
	}
	else
		return  obForeign;
}
bool CMousewin::TablesEditToButton()
{
	if (IsWindowVisible(m_cOrderBtn.GetHwnd()))
	{
		m_cOrderBtn.OnSetFocus();
		return true;
	}
	else
		return false;
}
void CMousewin::UpdateModifyRecover()
{
	if (m_bModifyOrderOper)//改单界面恢复
	{
		m_bModifyOrderOper = false;
		SetWindowCtrlEnableOrDisable(true);
		LayOut();
	}
}
bool CMousewin::IsCodeOfDCE()
{
	TContractKey key;
	memset(&key, 0, sizeof(TContractKey));
	if (CConvert::TContractNoToTContractKey(m_sCode.c_str(), key))
	{
		if (strcmp(key.ExchangeNo, exDCE) == 0)
			return true;
	}
	return false;
}
bool CMousewin::IsSpreadLayOut()
{
	return (m_nCurClickItem.nCurHeadType == MHI_Strategys && (m_nCurClickItem.nCurChildType == CHS_SyncSpread || m_nCurClickItem.nCurChildType == CHS_SequSpread));
}
bool CMousewin::IsStopLayOut()
{
	return m_nCurClickItem.nCurHeadType == MHI_Strategys && m_nCurClickItem.nCurChildType == CHS_PCStop;
}
bool CMousewin::IsLadderLayOut()
{
	return m_nCurClickItem.nCurHeadType == MHI_Normal && m_nCurClickItem.nCurChildType == CHN_Ladder;
}
bool CMousewin::IsOptionLayOut()
{
	return m_nCurClickItem.nCurHeadType == MHI_Advanced && (m_nCurClickItem.nCurChildType >= CHA_Enquiry&&m_nCurClickItem.nCurChildType <= CHA_ExecAban);
}
bool CMousewin::IsOptionExAbnLayOut()												//期权 行权弃权
{
	return m_nCurClickItem.nCurHeadType == MHI_Advanced && m_nCurClickItem.nCurChildType == CHA_ExecAban;
}
bool CMousewin::IsOptionStrategyLayOut()
{
	return m_nCurClickItem.nCurHeadType == MHI_Normal && m_nCurClickItem.nCurChildType==CHN_Option;
}
bool CMousewin::IsAutoCloseLayOut()
{
    return m_nCurClickItem.nCurHeadType == MHI_Advanced && m_nCurClickItem.nCurChildType == CHA_AutoClose;
}
bool CMousewin::IsBatchLayOut()
{
	return m_nCurClickItem.nCurHeadType == MHI_BatchOrder;
}
bool  CMousewin::IsConditionLayout()
{
	return m_nCurClickItem.nCurHeadType == MHI_Advanced && m_nCurClickItem.nCurChildType == CHA_Condition;
}
bool CMousewin::IsSpreadApplyLayOut()
{
	return m_nCurClickItem.nCurHeadType == MHI_Advanced && m_nCurClickItem.nCurChildType == CHA_SpreadApply;
}
bool CMousewin::IsHedgeApplyLayOut()												//套保申请布局
{
	return m_nCurClickItem.nCurHeadType == MHI_Advanced && m_nCurClickItem.nCurChildType == CHA_HedgeApply;
}
bool CMousewin::IsSwapLayOut()
{
	return m_nCurClickItem.nCurHeadType == MHI_Advanced && m_nCurClickItem.nCurChildType == CHA_Swap;
}
bool CMousewin::IsOfferLayOut()
{
	return m_nCurClickItem.nCurHeadType == MHI_Advanced && m_nCurClickItem.nCurChildType == CHA_Offer;
}
bool CMousewin::IsInnerCommodity()
{
	return m_CommodityInfo.cCoverMode == oCoverT || m_CommodityInfo.cCoverMode == oCover;
}
bool CMousewin::IsUserNoSync()												
{
	return m_SyncUserNoBtn.GetLockState() && IsWindowVisible(m_SyncUserNoBtn.GetHwnd());
}
bool  CMousewin::IsUserNoLock()													
{
	return m_LockUserNoBtn.GetLockState() && IsWindowVisible(m_LockUserNoBtn.GetHwnd());
}
bool CMousewin::IsSpreadCodeAdd(const char* str)
{
	if (str == LoadRC::unicode_to_ansi(LoadResWchar_t(IDS_SprContract)))
	{
		g_conFrame->show(LoadResWchar_t(IDS_SpreadSet), cmtAdvance);
		return true;
	}
	else
		return false;
}
bool CMousewin::IsSpreadTriggered()
{
	return m_nCurClickItem.nCurHeadType == MHI_Strategys && m_nCurClickItem.nCurChildType == CHS_SyncSpread && wcscmp(m_sPriceSpreadtype, LoadResWchar_t(IDS_NoPrice)) == 0;
}
bool CMousewin::IsCheckOpenContinues()
{
	return m_CheckOpenContinue.GetCheck()==BST_CHECKED && IsWindowVisible(m_CheckOpenContinue.GetHwnd());
}
void CMousewin::PopPriceCheckMsg(MPriceType ptype,MErrorType etype)
{
	int n_Price_ID = IDS_STRING1074;
	int n_Error_ID = IDS_STRING1070;
	switch (ptype)
	{
	case mptTrig:
		n_Price_ID = IDS_InvalidTrigPrice;
		break;
	case mptLoss:
		n_Price_ID = IDS_InvalidLossPrice;
		break;
	case mptProfit:
		n_Price_ID = IDS_InvalidProfitPrice;
		break;
	}
	switch (etype)
	{
	case metEmpty:
		n_Error_ID = IDS_STRING1076;
		break;
	case metZero:
		n_Error_ID = IDS_ZeroInvalid;
		break;
	case metNoNegtive:
		n_Error_ID = IDS_NONEGATIVE;
		break;		
	}
	TMouErrorTipStrW wtext = { 0 };
	swprintf_s(wtext, L"%s:%s", LoadResWchar_t(n_Price_ID), LoadResWchar_t(n_Error_ID));
	MessageBox(m_Hwnd, wtext, LoadResWchar_t(IDS_TIP), NULL);
}
void CMousewin::GetErrorText(TMouPriceExceed &ErrorText, int nID, const SOFCommodityInf &sInfo, double dp1, double dp2)
{
	swprintf_s(ErrorText, LoadResWchar_t(nID), LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dp1, sInfo)).c_str(),
		LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dp2, sInfo)).c_str());
}
void CMousewin::GetSpeExchangeText(TMouPriceExceed &ErrorText, int nID, const SOFCommodityInf &sInfo, const wchar_t *wExchange, double dp)
{
	swprintf_s(ErrorText, LoadResWchar_t(nID), wExchange, LoadRC::ansi_to_unicode(CConvert::FormatPriceToStr(dp, sInfo)).c_str());
}
void CMousewin::EnableWndStaticOwnerDraw(int nID,BOOL bEnable)
{
	if (IsWindowEnabled(m_hControl[nID]) == bEnable)
		return;
	EnableWindow(m_hControl[nID], bEnable);
	if (IsWindowVisible(m_hControl[nID]))
	{
		InvalidateRect(m_hControl[nID], NULL, TRUE);
		UpdateWindow(m_hControl[nID]);
	}	
}
void CMousewin::SetShowTips(const wchar_t* wtips)
{
	if (wcscmp(wtips, L"")==0)
		ShowWindow(m_hControl[CI_ShowTip_Static], SW_HIDE);
	else
	{
		TMouErrorTipStrW Error = { 0 };
		swprintf_s(Error, L"%s: %s", LoadResWchar_t(IDS_TipShow), wtips);
		SetWindowText(m_hControl[CI_ShowTip_Static], Error);
		ShowWindow(m_hControl[CI_ShowTip_Static], SW_SHOW);
	}		
}
bool CMousewin::OptionSendOrder(TSendOrder &OrderInfo)
{
	FillAndSendOrder(OrderInfo, false, false);
	return true;
}
bool CMousewin::LadderSendOrder(ListOrderInfo &OrderInfo)
{
	if (TradeLoginLinkage())
		return false;
	TSendOrder sendorder;
	CConvert::DefaultSendOrder(sendorder);
	sendorder.Direct = OrderInfo.Direct;
	sendorder.Offset = OrderInfo.Offset;
	sendorder.OrderType = OrderInfo.OrderType;
	sendorder.OrderPrice = OrderInfo.OrderPrice;
	sendorder.TriggerPrice = OrderInfo.TriggerPrice;
	sendorder.ValidType = vtGFD;
	if (!GetOrderQty(CI_OrderQty_Edit, sendorder.OrderQty))
		return false ;	//委托数量
	FillAndSendOrder(sendorder,false);
	return true;
}
bool CMousewin::CxGetWindowText(MH_TYPE m_type, int& nSize, wchar_t* wstr, int nMaxLen)
{
	return true;
}

void CMousewin::OnLadderStyleChanged()
{
    m_OrderList.UpdateColorStyle(m_MouseCfg.bUseCfgColor);
}

bool CMousewin::TradeLoginLinkage()
{
	if (m_UserInfo.size() == 0)
	{
		if (g_mainframe)
			g_mainframe->Linkage(m_Hwnd, _MouseOrder, W_USER_LOGIN, "");
		return true;
	}
	else
		return false;
}
bool CMousewin::CorrectOrderPrice(TSendOrder &sOrder)//校正价格
{
	SContract *pQuote = NULL;
	if (g_pStarApi && g_pStarApi->GetContractData("", m_sQuoteContractId.c_str(), &pQuote, 1, false))
	{
		double dUp = 0, dDown = 0;
		dUp = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITUPPRICE);
		dDown = CConvert::GetPriceBySnapShot(pQuote->SnapShot, S_FID_LIMITDOWNPRICE);
			
		if (sOrder.OrderType != otMarket&&sOrder.OrderType != otMarketStop)
		{
			//价格 涨跌停价 存在
			if (!CConvert::DoubleSameValue(0, sOrder.OrderPrice, FLOAT_SAME_ERRORDIFF)&& !CConvert::DoubleSameValue(0, dUp, FLOAT_SAME_ERRORDIFF)&& !CConvert::DoubleSameValue(0, dDown, FLOAT_SAME_ERRORDIFF))
			{
				if (DoubleExceed(sOrder.OrderPrice, dUp))
				{
					TMouPriceExceed ErrorText = { 0 };
					GetErrorText(ErrorText, IDS_ExUp, m_CommodityInfo, sOrder.OrderPrice, dUp);
					if (MessageBox(m_Hwnd, ErrorText, LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDOK)
						sOrder.OrderPrice = dUp;
				}
				else if (DoubleExceed(sOrder.OrderPrice, dDown, false))
				{
					TMouPriceExceed ErrorText = { 0 };
					GetErrorText(ErrorText, IDS_ExDown, m_CommodityInfo, sOrder.OrderPrice, dDown);
					if (MessageBox(m_Hwnd, ErrorText, LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDOK)
						sOrder.OrderPrice = dDown;
				}
			}
		}
		else if (sOrder.OrderType == otMarket)
		{
			//上期所市价 //港交所市价 （以涨跌停挂单）
			if (m_CommodityInfo.cCoverMode == oCoverT || m_CommodityInfo.bAddOne)
			{
				if (sOrder.Direct==dBuy&& !CConvert::DoubleSameValue(0, dUp, FLOAT_SAME_ERRORDIFF))
				{
                    //上期所市价单自动转涨跌停价发限价单 如果没有涨跌停，发出市价单 交易所返回错误
                    sOrder.OrderPrice = dUp;
                    sOrder.OrderType = otLimit;
				}
				else if(sOrder.Direct == dSell && !CConvert::DoubleSameValue(0, dDown, FLOAT_SAME_ERRORDIFF))
				{
                    sOrder.OrderPrice = dDown;
                    sOrder.OrderType = otLimit;
				}		
			}
		}
	}
	
	return true;
}
bool CMousewin::CheckSpreadOrderPriceTrigState(TSpreadSendOrder &Sspread)//套利价格 触发检测
{
	bool bExceed = false;
	SSpreadContract sQuote;
	if (g_pStarApi && g_pStarApi->GetSpreadInfo(Sspread.ContractNo, &sQuote, 1, false) && sQuote.SnapShot)
	{
		double dPriceAsk = 0, dPriceBid = 0;
		dPriceAsk = CConvert::GetPriceBySnapShot(sQuote.SnapShot, S_FID_BESTASKPRICE);
		dPriceBid = CConvert::GetPriceBySnapShot(sQuote.SnapShot, S_FID_BESTBIDPRICE);
        double dLast = CConvert::GetPriceBySnapShot(sQuote.SnapShot, S_FID_LASTPRICE);
	
		double dCompareAsk = SimpleRoundTo(dPriceAsk, sQuote.PricePrec);
		double dCompareBid = SimpleRoundTo(dPriceBid, sQuote.PricePrec);

		double dPrice = 0;
		if (sQuote.SnapShot)
		{
			if (Sspread.TrigCondition==tcGreaterEqual)
			{
				if (((Sspread.Direct[0] == dBuy && Sspread.TrigPriceMode == tpm_Match) || (Sspread.Direct[0] == dSell&&Sspread.TrigPriceMode == tpm_Work))
					&& sQuote.SnapShot->Data[S_FID_BESTASKPRICE].FidAttr)
				{
					if(DoubleSameOrExceed(dCompareAsk, Sspread.OrderPrice))
					{
						bExceed = true;
						dPrice = dCompareAsk;
					}
				}
				if (((Sspread.Direct[0] == dSell && Sspread.TrigPriceMode == tpm_Match) || (Sspread.Direct[0] == dBuy&&Sspread.TrigPriceMode == tpm_Work))
					&& sQuote.SnapShot->Data[S_FID_BESTBIDPRICE].FidAttr)
				{
					if (DoubleSameOrExceed(dCompareBid, Sspread.OrderPrice))
					{
						bExceed = true;
						dPrice = dCompareBid;
					}
				}
                if (Sspread.TrigPriceMode == tpm_Last && sQuote.SnapShot->Data[S_FID_BESTASKPRICE].FidAttr)
                {
                    if (DoubleSameOrExceed(dLast, Sspread.OrderPrice))
                    {
                        bExceed = true;
                        dPrice = dLast;
                    }
                }
			}
			else
			{ 
				if (((Sspread.Direct[0] == dBuy && Sspread.TrigPriceMode == tpm_Match) || (Sspread.Direct[0] == dSell&&Sspread.TrigPriceMode == tpm_Work))
					&& sQuote.SnapShot->Data[S_FID_BESTASKPRICE].FidAttr)
				{
					if(DoubleSameOrExceed(dCompareAsk, Sspread.OrderPrice,false))
					{
						bExceed = true;
						dPrice = dCompareAsk;
					}
				}
				if (((Sspread.Direct[0] == dSell && Sspread.TrigPriceMode == tpm_Match) || (Sspread.Direct[0] == dBuy&&Sspread.TrigPriceMode == tpm_Work))
					&& sQuote.SnapShot->Data[S_FID_BESTBIDPRICE].FidAttr)
				{
					if (DoubleSameOrExceed(dCompareBid, Sspread.OrderPrice,false))
					{
						bExceed = true;
						dPrice = dCompareBid;
					}
				}
                if (Sspread.TrigPriceMode == tpm_Last && sQuote.SnapShot->Data[S_FID_BESTASKPRICE].FidAttr)
                {
                    if (DoubleSameOrExceed(dLast, Sspread.OrderPrice, false))
                    {
                        bExceed = true;
                        dPrice = dLast;
                    }
                }
			}
			if (bExceed)
			{
				TMouPriceExceed ErrorText = { 0 };
				if (Sspread.TrigPriceMode==tpm_Match)
					GetErrorText(ErrorText, IDS_SpreadMatchTrig, m_CommodityInfo, dPrice, Sspread.OrderPrice);
				else if (Sspread.TrigPriceMode == tpm_Work)
					GetErrorText(ErrorText, IDS_SpreadWorkTrig, m_CommodityInfo, dPrice, Sspread.OrderPrice);
                else if (Sspread.TrigPriceMode == tpm_Last)
                    GetErrorText(ErrorText, IDS_SpreadLastTrig, m_CommodityInfo, dPrice, Sspread.OrderPrice);
                
				if (MessageBox(m_Hwnd, ErrorText, LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDOK)
					bExceed = false;
			}
		}	
	}
	
	return bExceed;
}

void CMousewin::MoveArea_Head(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		MoveCtrlWindow(m_hControl[CI_Head_Button], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_Head_Search], nCenterHeight);
		nCurHeight += 5;
	}
	::ShowWindow(m_hControl[CI_Head_Button], bshow);
	::ShowWindow(m_hControl[CI_Head_Search], bshow);
}

void CMousewin::MoveArea_UserNo(int &nCurHeight, bool bshow, bool bshowsub)
{
	bool bShowUserNo3 = false;
	bool bShowUserNo4 = false;
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_UserNo_Combox], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_UserNoBatch_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_UserNo_Static], nCenterHeight);	
		MoveCtrlRowWindow(m_hControl[CI_UserNo_Lock], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_UserName_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_UserNo_Combox2], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_UserNo_Sync], nCenterHeight);
        if (IsSpreadLayOut())
        {
            SSpreadContract sQuote;
            if (!m_sQuoteContractId.empty() && g_pStarApi && g_pStarApi->GetSpreadInfo(m_sQuoteContractId.c_str(), &sQuote, 1, false) > 0)
            {
                if (sQuote.ValidLegs > 2)
                {
                    nCurHeight += CtrlInterval;
                    MoveCtrlWindow(m_hControl[CI_UserNo_Combox3], nCurHeight, nCenterHeight);
					MoveCtrlRowWindow(m_hControl[CI_UserNo_Static2], nCenterHeight);
					bShowUserNo3 = true;
                }
				if (sQuote.ValidLegs > 3)
				{
					MoveCtrlRowWindow(m_hControl[CI_UserNo_Combox4], nCenterHeight);
					bShowUserNo4 = true;
				}
            }
        }
	}
	::ShowWindow(m_hControl[CI_UserNo_Combox], bshow);
	::ShowWindow(m_hControl[CI_UserNoBatch_Static], bshow&&IsBatchLayOut());
	::ShowWindow(m_hControl[CI_UserNo_Static], bshow&&!IsBatchLayOut());
	::ShowWindow(m_hControl[CI_UserNo_Lock], bshow && !bshowsub);
	::ShowWindow(m_hControl[CI_UserName_Static], bshow && !bshowsub);
	::ShowWindow(m_hControl[CI_UserNo_Combox2], bshow && bshowsub);
	::ShowWindow(m_hControl[CI_UserNo_Sync], bshow && bshowsub);

	::ShowWindow(m_hControl[CI_UserNo_Static2], bShowUserNo3);
	::ShowWindow(m_hControl[CI_UserNo_Combox3], bShowUserNo3);
    ::ShowWindow(m_hControl[CI_UserNo_Combox4], bShowUserNo4);
}
void CMousewin::MoveArea_Commodity(int &nCurHeight, bool bshow, bool bshowsub)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_ContractNo_Edit_Win], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_ContractNo_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_ContractNo_Lock], nCenterHeight);
	}
	::ShowWindow(m_hControl[CI_ContractNo_Edit_Win], bshow);
	::ShowWindow(m_hControl[CI_ContractNo_Static], bshow);
    ::ShowWindow(m_hControl[CI_ContractNo_Lock], bshow);
}
void CMousewin::MoveArea_SpreadCode(int & nCurHeight, bool bshow)
{
    bool bShowThird = false, bShowFourth = false;
    if (bshow)
    {
        int nCenterHeight = 0;
        nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_Spread_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_Spread_Static], nCenterHeight);
        MoveCtrlRowWindow(m_hControl[CI_ContractNo_Lock], nCenterHeight);
    }
	::ShowWindow(m_hControl[CI_Spread_Edit], bshow);
	::ShowWindow(m_hControl[CI_Spread_Static], bshow);
    if (bshow)
    {
        ::ShowWindow(m_hControl[CI_ContractNo_Lock], bshow);
    }
}
void CMousewin::MoveArea_OrderQty(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_OrderQty_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderQty_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderQty_Spin], nCenterHeight);

        MoveCtrlRowWindow(m_hControl[CI_OrderRatio_Static], nCenterHeight);

		MoveCtrlRowWindow(m_hControl[CI_ValidStyle_Button], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderLongShort_Static], nCenterHeight);
	}
	::ShowWindow(m_hControl[CI_OrderQty_Edit], bshow);
	::ShowWindow(m_hControl[CI_OrderQty_Static], bshow);
	::ShowWindow(m_hControl[CI_OrderQty_Spin], bshow);

    if (IsSpreadLayOut())
    {
        ::ShowWindow(m_hControl[CI_OrderRatio_Static], bshow);
    }
    else
    {
        ::ShowWindow(m_hControl[CI_OrderRatio_Static], SW_HIDE);
    }
	if (IsLadderLayOut())
		::ShowWindow(m_hControl[CI_OrderLongShort_Static], bshow);
	else if (IsOptionLayOut())
	{
		::ShowWindow(m_hControl[CI_OrderLongShort_Static], bshow);
		if(bshow)
			UpdatePositionQty();//布局变动：应价 行、弃权 
	}
	else
		::ShowWindow(m_hControl[CI_OrderLongShort_Static], SW_HIDE);
}
void CMousewin::MoveArea_ValidType(bool bshow)
{
	::ShowWindow(m_hControl[CI_ValidStyle_Button], bshow&&IsWindow(m_cValidTypeWin.GetHwnd()));
}
void CMousewin::MoveArea_CmbFlag(int &nCurHeight,bool bshow, bool &bdiv)
{
	if (bshow)
	{
		MoveCtrlRowWindow(m_hControl[CI_CmbFlag_Static], nCurHeight + 16);	
		bdiv = true;
	}
	::ShowWindow(m_hControl[CI_CmbFlag_Static], bshow);
}
void CMousewin::MoveArea_OrderQtyRate(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_OrderQtyRate_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderQtyRate_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderQtyRate_Spin], nCenterHeight);
	}
	::ShowWindow(m_hControl[CI_OrderQtyRate_Edit], bshow);
	::ShowWindow(m_hControl[CI_OrderQtyRate_Static], bshow);
	::ShowWindow(m_hControl[CI_OrderQtyRate_Spin], bshow);
}
void CMousewin::MoveArea_OrderPrice(int &nCurHeight, bool bshow, bool bshowsub)
{
	int Start2 = 108;
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_OrderPrice_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderPrice_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_BuyPrice_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderPrice_Spin], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderPrice_Static_Cal], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderPrice_Edit_Mole], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OrderPrice_Static_Div], nCenterHeight + 1);
		MoveCtrlRowWindow(m_hControl[CI_OrderPrice_Edit_Dem], nCenterHeight + 1);

		MoveCtrlRowWindow(m_hControl[CI_OverPoint_Static], nCenterHeight - 1);
		MoveCtrlRowWindow(m_hControl[CI_OverPoint_Edit_Win], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_OverPoint_Spin], nCenterHeight);

		MoveCtrlRowWindow(m_hControl[CI_OrderSellPrice_Static], nCenterHeight - 6);
		MoveCtrlRowWindow(m_hControl[CI_OrderBuyPrice_Static], nCenterHeight + 11);

		MoveCtrlRowWindow(m_hControl[CI_SpreadSwitch_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_SpreadTrigMode_Combox2], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_SpreadTrigMode_Combox], nCenterHeight);

		//价格编辑框位置移动复原
		int nStart = 0;
		RECT rect;
		GetWindowRect(m_hControl[CI_OrderPrice_Edit], &rect);
		ScreenToClient(m_Hwnd, LPPOINT(&rect));
		ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);
		if (bshowsub&&rect.left < Start2)
			nStart = Start2;
		else if (!bshowsub&&rect.left > Start2)
			nStart = -Start2;
		if (nStart != 0)
		{
			MoveCtrlColWindowEx(m_hControl[CI_OrderPrice_Edit], nStart);
			MoveCtrlColWindowEx(m_hControl[CI_OrderPrice_Spin], nStart);
		}

		//买卖行情位置移动复原
		int nWStart1 = 178;
		int nWStart2 = 178;
		if (m_CommodityInfo.bIfFractionPrice|| bshowsub)
		{
			nWStart1 = 300;
			nWStart2 = 300;
		}
		MoveCtrlColWindow(m_hControl[CI_OrderSellPrice_Static], nWStart1);
		MoveCtrlColWindow(m_hControl[CI_OrderBuyPrice_Static], nWStart2);

		if (IsStopLayOut() || IsOfferLayOut())
			;
		else
			nCurHeight += 2;
	}
	if (wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)) == 0 || wcscmp(m_sPricetype, LoadResWchar_t(IDS_MARKET_PRICE)) == 0)//限价 市价
	{
		::ShowWindow(m_hControl[CI_OrderPrice_Edit], bshow);
		::ShowWindow(m_hControl[CI_OrderPrice_Spin], bshow);
		if (bshow&&m_CommodityInfo.bIfFractionPrice)
		{
			bool bshow1 = wcscmp(m_sPricetype, LoadResWchar_t(IDS_LIMIT_PRICE)) == 0;

			::ShowWindow(m_hControl[CI_OrderPrice_Static_Cal], bshow1);
			::ShowWindow(m_hControl[CI_OrderPrice_Edit_Mole], bshow1);
			::ShowWindow(m_hControl[CI_OrderPrice_Static_Div], bshow1);
			::ShowWindow(m_hControl[CI_OrderPrice_Edit_Dem], bshow1);
			SetWindowInt(m_hControl[CI_OrderPrice_Edit_Dem], m_CommodityInfo.iPrecision);
		}
		else
		{
			::ShowWindow(m_hControl[CI_OrderPrice_Static_Cal], SW_HIDE);
			::ShowWindow(m_hControl[CI_OrderPrice_Edit_Mole], SW_HIDE);
			::ShowWindow(m_hControl[CI_OrderPrice_Static_Div], SW_HIDE);
			::ShowWindow(m_hControl[CI_OrderPrice_Edit_Dem], SW_HIDE);
		}
		::ShowWindow(m_hControl[CI_OverPoint_Static], FALSE);
		::ShowWindow(m_hControl[CI_OverPoint_Edit_Win], FALSE);
		::ShowWindow(m_hControl[CI_OverPoint_Spin], FALSE);
	}
	else//超价
	{
		::ShowWindow(m_hControl[CI_OverPoint_Static], bshow);
		::ShowWindow(m_hControl[CI_OverPoint_Edit_Win], bshow);
		::ShowWindow(m_hControl[CI_OverPoint_Spin], bshow);
		::ShowWindow(m_hControl[CI_OrderPrice_Edit], FALSE);
		::ShowWindow(m_hControl[CI_OrderPrice_Static], FALSE);
		::ShowWindow(m_hControl[CI_OrderPrice_Spin], FALSE);
		::ShowWindow(m_hControl[CI_OrderPrice_Static_Cal], FALSE);
		::ShowWindow(m_hControl[CI_OrderPrice_Edit_Mole], FALSE);
		::ShowWindow(m_hControl[CI_OrderPrice_Static_Div], FALSE);
		::ShowWindow(m_hControl[CI_OrderPrice_Edit_Dem], FALSE);
	}
	
	if (IsSpreadLayOut())
		SetWindowText(m_hControl[CI_BuyPrice_Static], LoadResWchar_t(IDS_STRING1093));
	else if (IsOfferLayOut())
		SetWindowText(m_hControl[CI_BuyPrice_Static], LoadResWchar_t(IDS_STRING1091));
	::ShowWindow(m_hControl[CI_SpreadTrigMode_Combox], bshowsub);
	::ShowWindow(m_hControl[CI_SpreadTrigMode_Combox2], bshowsub);
	::ShowWindow(m_hControl[CI_OrderPrice_Static], bshow && !IsOfferLayOut() && !IsSpreadLayOut());
	::ShowWindow(m_hControl[CI_BuyPrice_Static], bshow&&(IsOfferLayOut() || (IsSpreadLayOut()&& m_nCurClickItem.nCurChildType != CHS_SyncSpread)));
	::ShowWindow(m_hControl[CI_SpreadSwitch_Static], bshow&&IsSpreadLayOut()&& m_nCurClickItem.nCurChildType == CHS_SyncSpread);
	::ShowWindow(m_hControl[CI_OrderSellPrice_Static], bshow);
	::ShowWindow(m_hControl[CI_OrderBuyPrice_Static], bshow);
}
void CMousewin::MoveArea_TimePicker(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval - 1;
		MoveCtrlWindow(m_hControl[CI_SpreadBeginTime_Picker], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_SpreadBeignTime_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_TimeValid_Check], nCenterHeight);
		
        nCenterHeight = 0;
        nCurHeight += CtrlInterval - 1;
        MoveCtrlWindow(m_hControl[CI_SpreadEndTime_Picker], nCurHeight, nCenterHeight);
        MoveCtrlRowWindow(m_hControl[CI_SpreadEndTime_Static], nCenterHeight);
	}
	::ShowWindow(m_hControl[CI_SpreadBeginTime_Picker], bshow);
	::ShowWindow(m_hControl[CI_SpreadBeignTime_Static], bshow);
    ::ShowWindow(m_hControl[CI_SpreadEndTime_Picker], bshow);
    ::ShowWindow(m_hControl[CI_SpreadEndTime_Static], bshow);
	::ShowWindow(m_hControl[CI_TimeValid_Check], bshow);
}
void CMousewin::MoveArea_SellPrice(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_SellPrice_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_SellPrice_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_SellPrice_Spin], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_SellPrice_Static_Cal], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_SellPrice_Edit_Mole], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_SellPrice_Static_Div], nCenterHeight + 1);
		MoveCtrlRowWindow(m_hControl[CI_SellPrice_Edit_Dem], nCenterHeight + 1);
	}
	::ShowWindow(m_hControl[CI_SellPrice_Edit], bshow);
	::ShowWindow(m_hControl[CI_SellPrice_Static], bshow);
	::ShowWindow(m_hControl[CI_SellPrice_Spin], bshow);
	::ShowWindow(m_hControl[CI_SellPrice_Static_Cal], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_SellPrice_Edit_Mole], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_SellPrice_Static_Div], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_SellPrice_Edit_Dem], bshow&&m_CommodityInfo.bIfFractionPrice);
	if (bshow&&m_CommodityInfo.bIfFractionPrice)	
		SetWindowInt(m_hControl[CI_SellPrice_Edit_Dem], m_CommodityInfo.iPrecision);
}
void CMousewin::MoveArea_Enquriy(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_EnquriyNo_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_EnquriyNo_Static], nCenterHeight);
	}
	::ShowWindow(m_hControl[CI_EnquriyNo_Static], bshow);
	::ShowWindow(m_hControl[CI_EnquriyNo_Edit], bshow);
}
void CMousewin::MoveArea_LocalStop(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_StopLoss_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_StopLoss_Spin], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_StopLoss_Static], nCenterHeight);
		if (m_CommodityInfo.bIfFractionPrice)
		{
			MoveCtrlRowWindow(m_hControl[CI_StopLoss_Static_Cal], nCenterHeight);
			MoveCtrlRowWindow(m_hControl[CI_StopLoss_Edit_Mole], nCenterHeight);
			MoveCtrlRowWindow(m_hControl[CI_StopLoss_Static_Div], nCenterHeight + 1);
			MoveCtrlRowWindow(m_hControl[CI_StopLoss_Edit_Dem], nCenterHeight + 1);
			SetWindowInt(m_hControl[CI_StopLoss_Edit_Dem], m_CommodityInfo.iPrecision);
		}

		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_StopProfit_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_StopProfit_Spin], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_StopProfit_Static], nCenterHeight);
		if (m_CommodityInfo.bIfFractionPrice)
		{
			MoveCtrlRowWindow(m_hControl[CI_StopProfit_Static_Cal], nCenterHeight);
			MoveCtrlRowWindow(m_hControl[CI_StopProfit_Edit_Mole], nCenterHeight);
			MoveCtrlRowWindow(m_hControl[CI_StopProfit_Static_Div], nCenterHeight + 1);
			MoveCtrlRowWindow(m_hControl[CI_StopProfit_Edit_Dem], nCenterHeight + 1);
			SetWindowInt(m_hControl[CI_StopProfit_Edit_Dem], m_CommodityInfo.iPrecision);
		}
	}
	::ShowWindow(m_hControl[CI_StopLoss_Edit], bshow);
	::ShowWindow(m_hControl[CI_StopLoss_Spin], bshow);
	::ShowWindow(m_hControl[CI_StopLoss_Static], bshow);
	::ShowWindow(m_hControl[CI_StopProfit_Edit], bshow);
	::ShowWindow(m_hControl[CI_StopProfit_Spin], bshow);
	::ShowWindow(m_hControl[CI_StopProfit_Static], bshow);
	::ShowWindow(m_hControl[CI_StopLoss_Static_Cal], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_StopLoss_Edit_Mole], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_StopLoss_Static_Div], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_StopLoss_Edit_Dem], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_StopProfit_Static_Cal], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_StopProfit_Edit_Mole], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_StopProfit_Static_Div], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_StopProfit_Edit_Dem], bshow&&m_CommodityInfo.bIfFractionPrice);
}
void CMousewin::MoveArea_MaxQty(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_MaxQty_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_MaxQty_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_MaxQty_Spin], nCenterHeight);
	}
	::ShowWindow(m_hControl[CI_MaxQty_Edit], bshow);
	::ShowWindow(m_hControl[CI_MaxQty_Static], bshow);
	::ShowWindow(m_hControl[CI_MaxQty_Spin], bshow);
}
void CMousewin::MoveArea_MinQty(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_MinQty_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_MinQty_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_MinQty_Spin], nCenterHeight);
	}
	::ShowWindow(m_hControl[CI_MinQty_Edit], bshow);
	::ShowWindow(m_hControl[CI_MinQty_Static], bshow);
	::ShowWindow(m_hControl[CI_MinQty_Spin], bshow);
}
void CMousewin::MoveArea_TrigPrice(int &nCurHeight, bool bshow, bool bshowsub)
{
	int nStart2 = 108;
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_TriggerPrice_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_TriggerPrice_Static], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_TriggerPrice_Spin], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_TriggerPrice_Static_Cal], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_TriggerPrice_Edit_Mole], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_TriggerPrice_Static_Div], nCenterHeight + 1);
		MoveCtrlRowWindow(m_hControl[CI_TriggerPrice_Edit_Dem], nCenterHeight + 1);
		MoveCtrlRowWindow(m_hControl[CI_TrigMode_Combox2], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_TrigMode_Combox], nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_TriggerPrice_Static], nCenterHeight);
		//还原触发价格原始位置
		RECT rect;
		GetWindowRect(m_hControl[CI_TriggerPrice_Edit], &rect);
		ScreenToClient(m_Hwnd, LPPOINT(&rect));
		ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);
		int nStart = 0;
		if (bshowsub && rect.left < nStart2)
			nStart = nStart2;
		else if (!bshowsub && rect.left > nStart2)
			nStart = -nStart2;
		
		if (nStart != 0)
		{
			MoveCtrlColWindowEx(m_hControl[CI_TriggerPrice_Edit], nStart);
			MoveCtrlColWindowEx(m_hControl[CI_TriggerPrice_Spin], nStart);
			MoveCtrlColWindowEx(m_hControl[CI_TriggerPrice_Static_Cal], nStart);
			MoveCtrlColWindowEx(m_hControl[CI_TriggerPrice_Edit_Mole], nStart);
			MoveCtrlColWindowEx(m_hControl[CI_TriggerPrice_Static_Div], nStart);
			MoveCtrlColWindowEx(m_hControl[CI_TriggerPrice_Edit_Dem], nStart);
		}
	}
	::ShowWindow(m_hControl[CI_TriggerPrice_Edit], bshow);
	::ShowWindow(m_hControl[CI_TriggerPrice_Static], bshow);
	::ShowWindow(m_hControl[CI_TriggerPrice_Spin], bshow);
	::ShowWindow(m_hControl[CI_TrigMode_Combox2], bshowsub);
	::ShowWindow(m_hControl[CI_TrigMode_Combox], bshowsub);
	::ShowWindow(m_hControl[CI_TriggerPrice_Static_Cal], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_TriggerPrice_Edit_Mole], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_TriggerPrice_Static_Div], bshow&&m_CommodityInfo.bIfFractionPrice);
	::ShowWindow(m_hControl[CI_TriggerPrice_Edit_Dem], bshow&&m_CommodityInfo.bIfFractionPrice);
	if (bshow&&m_CommodityInfo.bIfFractionPrice)
		SetWindowInt(m_hControl[CI_TriggerPrice_Edit_Dem], m_CommodityInfo.iPrecision);
}
void CMousewin::MoveArea_Note(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nCenterHeight = 0;
		nCurHeight += CtrlInterval;
		MoveCtrlWindow(m_hControl[CI_NoteInfo_Edit], nCurHeight, nCenterHeight);
		MoveCtrlRowWindow(m_hControl[CI_NoteInfo_Static], nCenterHeight);
	}
	::ShowWindow(m_hControl[CI_NoteInfo_Static], bshow);
	::ShowWindow(m_hControl[CI_NoteInfo_Edit], bshow);
}
void CMousewin::MoveArea_Check(int nID,int &nCurHeight, int &nCurWidth, bool bshow, bool &bdiv)
{
	if (bshow)
	{
		nCurWidth += CtrlInterval;
		MoveCtrlRowWindow(m_hControl[nID], nCurHeight + 15);
		MoveCtrlColWindow(m_hControl[nID], nCurWidth);
		bdiv = true;
	}
	::ShowWindow(m_hControl[nID], bshow);
}
void CMousewin::MoveArea_Picture(int &nCurHeight, bool bshow, bool bdiv)
{
	if (bshow)
	{
		if (bdiv)
			nCurHeight += CtrlInterval + 22;
		else
			nCurHeight += CtrlInterval * 2;
		MoveCtrlWindow(m_cOrderBtn.GetHwnd(), nCurHeight);
		
		int nHeight = nCurHeight - 40;
		MoveCtrlWindow(m_hControl[CI_ShowTip_Static], nHeight);
		UpdatePositionQty();//三键布局变动：刷新可开可平
	}
	::ShowWindow(m_cOrderBtn.GetHwnd(), bshow);
}
void CMousewin::MoveArea_List(int &nCurHeight, bool bshow, bool bdiv)
{
	if (bshow)
	{
		if (bdiv)
			nCurHeight += CtrlInterval + 26;
		else
			nCurHeight += CtrlInterval * 2;
		RECT rect;
		GetClientRect(m_Hwnd, &rect);
		int nHeight = (rect.bottom - rect.top - nCurHeight) / N_HEIGHT_ITEM_LIST * N_HEIGHT_ITEM_LIST;
		nHeight = nHeight > N_HEIGHT_LIST_MIN ? nHeight : N_HEIGHT_LIST_MIN;
		nHeight = nHeight < N_HEIGHT_LIST ? nHeight : N_HEIGHT_LIST;
		SetWindowPos(m_OrderList.GetHwnd(), NULL, 0, 0, N_WIDTH_LIST, nHeight, SWP_NOZORDER | SWP_NOMOVE);
		MoveCtrlWindow(m_OrderList.GetHwnd(), nCurHeight);
		UpdatePositionQty();//点价布局变动：刷新持仓
	}
	::ShowWindow(m_OrderList.GetHwnd(), bshow);	
}
void CMousewin::MoveArea_Option(int &nCurHeight, bool bshow, bool bdiv)
{
	if (bshow)
	{
		if (bdiv)
			nCurHeight += CtrlInterval + 26;
		else
			nCurHeight += CtrlInterval * 2;
		MoveCtrlWindow(m_OptionView.GetHwnd(), nCurHeight);
		m_OptionView.UpdatePrice();
	}
	::ShowWindow(m_OptionView.GetHwnd(), bshow);
}
void CMousewin::MoveArea_Ok(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nHeight = nCurHeight + CtrlInterval * 2;
		MoveCtrlWindow(m_hControl[CI_Ok_Button], nHeight);
	}
	::ShowWindow(m_hControl[CI_Ok_Button], bshow);
}
void CMousewin::MoveArea_Modify(int &nCurHeight, int &nCurWidth, bool bshow)
{
	if (bshow)
	{	
		int nHeight = nCurHeight + CtrlInterval + ((IsSpreadApplyLayOut()|| IsHedgeApplyLayOut())? CtrlInterval:20);
		int nCenterHeight = 0;
		MoveCtrlWindow(m_hControl[CI_Modify_Button], nHeight, nCenterHeight);
		if (IsOptionExAbnLayOut())
		{
			nCurWidth += CtrlInterval;
			MoveCtrlRowWindow(m_hControl[CI_ShowTip_Option], nCurHeight + 15);
			MoveCtrlColWindow(m_hControl[CI_ShowTip_Option], nCurWidth);
			nCurWidth += CtrlInterval;
			MoveCtrlRowWindow(m_hControl[CI_CanExcute_Option], nCurHeight + 15);
			MoveCtrlColWindow(m_hControl[CI_CanExcute_Option], nCurWidth);
		}
	}
	::ShowWindow(m_hControl[CI_ShowTip_Option], bshow&&IsOptionExAbnLayOut());
	::ShowWindow(m_hControl[CI_CanExcute_Option], bshow&&IsOptionExAbnLayOut());
	::ShowWindow(m_hControl[CI_Modify_Button], bshow);
}
void CMousewin::MoveArea_Cancel(int &nCurHeight, bool bshow)
{
	if (bshow)
	{
		int nHeight = nCurHeight + CtrlInterval + ((IsSpreadApplyLayOut() || IsHedgeApplyLayOut()) ? CtrlInterval : 20);
		MoveCtrlWindow(m_hControl[CI_Cancel_Button], nHeight);
	}
	::ShowWindow(m_hControl[CI_Cancel_Button], bshow);
}
void CMousewin::InitTotalInfo()
{
	TClickHeadInfo info;
	int nIDS_ID = 0;
	//MHI_Normal
	info.nCurHeadType = MHI_Normal;
	for (int i= CHN_Vert;i<= CHN_Option;i++)
	{
		info.nCurChildType = i;
		switch (i)
		{
		case CHN_Vert:
			nIDS_ID = IDS_VertOrder;
			break;
		case CHN_Ladder:
			nIDS_ID = IDS_LadderOrder;
			break;
		case CHN_Option:
			nIDS_ID = IDS_OptionHead;
			break;
		}
		m_TotalItemInfo.insert(pair<wstring, TClickHeadInfo>(LoadResWchar_t(nIDS_ID), info));
	}
	info.nCurHeadType = MHI_Advanced;
	for (int i = CHA_Condition; i < CHA_END; i++)
	{
		info.nCurChildType = i;
		switch (i)
		{
			case CHA_Condition:
				nIDS_ID = IDS_Condition;
				break;
			case CHA_PreOrder:
				nIDS_ID = IDS_PreOrder;
				break;
			case CHA_AutoOrder:
				nIDS_ID = IDS_AutoOrder;
				break;
			case CHA_Swap:
				nIDS_ID = IDS_SWAP;
				break;
			case CHA_LimtStop:
				nIDS_ID = IDS_LIMIT_STOP_PRICE;
				break;
			case CHA_Stop:
				nIDS_ID = IDS_STOP_PRICE;
				break;
			case CHA_Iceberg:
				nIDS_ID = IDS_ICEBERG;
				break;
			case CHA_Shadow:
				nIDS_ID = IDS_SHADOW;
				break;
			case CHA_Enquiry:
				nIDS_ID = IDS_ASK_PRICE;
				break;
			case CHA_Offer:
				nIDS_ID = IDS_REPLY_PRICE;
				break;
			case CHA_ExecAban:
				nIDS_ID = IDS_EXECUTE_ABANDON;
				break;
			case CHA_SpreadApply:
				nIDS_ID = IDS_ARBAPP;
				break;
			case CHA_HedgeApply:
				nIDS_ID = IDS_HEDGEAPP;
				break;
            case CHA_AutoClose:
                nIDS_ID = IDS_Auto_Close;
                break;
		}
		m_TotalItemInfo.insert(pair<wstring, TClickHeadInfo>(LoadResWchar_t(nIDS_ID), info));
	}
	info.nCurHeadType = MHI_Strategys;
	for (int i = CHS_PCStop; i <= CHS_SequSpread; i++)
	{
		info.nCurChildType = i;
		switch (i)
		{
		case CHS_PCStop:
			nIDS_ID = IDS_LocalStop;
			break;
		case CHS_SyncSpread:
			nIDS_ID = IDS_STRING1178;
			break;
		case CHS_SequSpread:
			nIDS_ID = IDS_STRING1179;
			break;
		}
		m_TotalItemInfo.insert(pair<wstring, TClickHeadInfo>(LoadResWchar_t(nIDS_ID), info));
	}
	info.nCurHeadType = MHI_BatchOrder;
	info.nCurChildType = 0;
	m_TotalItemInfo.insert(pair<wstring, TClickHeadInfo>(LoadResWchar_t(IDS_BatchOrder), info));

	if (m_LoginType != TYPE_Both)
		m_LoginType = m_LoginType | CConvert::ReadVersionType();
}
bool CMousewin::GetShowItemByStr(TClickHeadInfo& item, const TMouPriceTypeStrW str)
{
	map<wstring, TClickHeadInfo>::iterator iter = m_TotalItemInfo.find(str);
	if (iter != m_TotalItemInfo.end())
	{
		memcpy(&item, &iter->second, sizeof(TClickHeadInfo));
		return true;
	}
	else
		return false;
}
bool CMousewin::UpdateShowItemByOrderType(char cOrderType, char cStrategyType)
{
	if (cStrategyType == stCondition)
	{
		m_nCurClickItem.nCurHeadType = MHI_Advanced;
		m_nCurClickItem.nCurChildType = CHA_Condition;
		if (cOrderType == otLimit || m_CheckFastOrder.GetCheck())
			memcpy_s(m_sPricetype, sizeof(m_sPricetype), LoadResWchar_t(IDS_LIMIT_PRICE), sizeof(m_sPricetype));
		else
			memcpy_s(m_sPricetype, sizeof(m_sPricetype), LoadResWchar_t(IDS_MARKET_PRICE), sizeof(m_sPricetype));
		m_StaticPriceSwitch.CxSetWindowText(m_sPricetype);
		UpdatePriceStyleControl();
	}
	else
	{
		switch (cOrderType)
		{
		case otIceberg:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_Iceberg;
			break;
		case otMarket:
		case otLimit:
			m_nCurClickItem.nCurHeadType = MHI_Normal;
			m_nCurClickItem.nCurChildType = CHN_Vert;
			if (cOrderType == otLimit || m_CheckFastOrder.GetCheck())
				memcpy_s(m_sPricetype, sizeof(m_sPricetype), LoadResWchar_t(IDS_LIMIT_PRICE), sizeof(m_sPricetype));
			else		
				memcpy_s(m_sPricetype, sizeof(m_sPricetype), LoadResWchar_t(IDS_MARKET_PRICE), sizeof(m_sPricetype));
			m_StaticPriceSwitch.CxSetWindowText(m_sPricetype);	
			UpdatePriceStyleControl();
			break;
		case otLimitStop:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_LimtStop;
			break;
		case otMarketStop:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_Stop;
			break;
		case otGhost:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_Shadow;
			break;
		case otOffer:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_Offer;
			break;
		case otEnquiry:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_Enquiry;
			break;
		case otExecute:
		case otAbandon:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_ExecAban;
			break;
		case otSpreadApply:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_SpreadApply;
			break;
		case otHedgApply:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_HedgeApply;
			break;
		case otSwap:
			m_nCurClickItem.nCurHeadType = MHI_Advanced;
			m_nCurClickItem.nCurChildType = CHA_Swap;
			break;
        case otFutureAutoClose:
        case otOptionAutoClose:
            m_nCurClickItem.nCurHeadType = MHI_Advanced;
            m_nCurClickItem.nCurChildType = CHA_AutoClose;
            break;
		default:
			break;
		}
	}
	UpdateCurHeadStr();
	LayOut();
	return true;
}
bool CMousewin::GetShowStrByItem(const TClickHeadInfo& item, TMouPriceTypeStrW str)
{
	for (map<wstring, TClickHeadInfo>::iterator iter = m_TotalItemInfo.begin(); iter != m_TotalItemInfo.end(); iter++)
	{
		if (iter->second.nCurHeadType == item.nCurHeadType&&iter->second.nCurChildType == item.nCurChildType)
		{
			memcpy(str, iter->first.c_str(), sizeof(TMouPriceTypeStrW));
			return true;
		}
	}	
	return false;
}
bool CMousewin::GetActiveTime(TSpreadSendOrder &Sspread)
{
	if (IsWindowVisible(m_hControl[CI_SpreadBeginTime_Picker]) && IsWindowVisible(m_hControl[CI_SpreadEndTime_Picker])
        && m_CheckTimePicker.GetCheck() == BST_CHECKED)
	{
		GetWindowTextA(m_hControl[CI_SpreadBeginTime_Picker], Sspread.BeginTime, sizeof(TStrategyDateTime));
        GetWindowTextA(m_hControl[CI_SpreadEndTime_Picker], Sspread.EndTime, sizeof(TStrategyDateTime));
        
        if (strcmp(Sspread.BeginTime, Sspread.EndTime) >= 0)
        {
            if (MessageBox(m_Hwnd, LoadResWchar_t(IDS_End_Time_Invalid), LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDCANCEL)
                return false;
            memset(Sspread.BeginTime, 0, sizeof(Sspread.BeginTime));
            memset(Sspread.EndTime, 0, sizeof(Sspread.EndTime));

            Sspread.bTimeWaitForTrig = false;
            return true;
        }

		struct tm Tm;
		memset(&Tm, 0, sizeof(tm));
		strptime(Sspread.EndTime, "%Y-%m-%d %H:%M", &Tm);
		time_t set = mktime(&Tm);
		time_t now = time(NULL);//取本地日期还是交易所日期？
		int ndif = HoursDiff(set/60,now/60);
		if (ndif <= 0)
		{
			if (MessageBox(m_Hwnd,LoadResWchar_t(IDS_BeforeTime), LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDCANCEL)
				return false;
			memset(Sspread.BeginTime, 0,sizeof(Sspread.BeginTime));
            memset(Sspread.EndTime, 0, sizeof(Sspread.EndTime));
            Sspread.bTimeWaitForTrig = false;
		}
		else if (ndif > 24 * 60)
		{
			if (MessageBox(m_Hwnd, LoadResWchar_t(IDS_TimeLate), LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDCANCEL)
				return false;
			Sspread.bTimeWaitForTrig = true;
		}
		else
			Sspread.bTimeWaitForTrig = true;
	}


	return true;
}

bool CMousewin::UpdateOffsetOfMovePosition(TSpreadSendOrder & sOrder)
{
    SSpreadContract sQuote;
    if (g_pStarApi && g_pStarApi->GetSpreadInfo(m_sSpreadCode.c_str(), &sQuote, 1, false) == 0)
    {
        return false;
    }

    TOffset offset = sOrder.Offset[0];
    if (offset == oNone)
    {
        return true;
    }

    std::string strCodeNoPos = "";

    for (int i = 1; i < sQuote.ValidLegs; i++)
    {
        if (sOrder.Offset[i] == oNone)
        {
            continue;
        }
        if (sQuote.RateMode[i-1] == S_SPREADMODE_DIFF || sQuote.RateMode[i-1] == S_SPREADMODE_RATIO)
        {
            if (offset == oOpen)
            {
                sOrder.Offset[i] = oCover;

                int nPos = 0;

                TContractKey key;
                memset(&key, 0, sizeof(key));
                uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;

                CConvert::SContractNoTypeToTradeContract(m_sSign[i].c_str(), sQuote.Contract[i]->ContractNo, key);
                g_pTradeData->GetPositionVol(m_sUserNo[i].c_str(), &key, LongCount, ShortCount, DayLongCount, DayShortCount);//单笔持仓

                if (sOrder.Direct[0] == dSell)
                {
                    nPos = (sQuote.RateMode[i - 1] != S_SPREADMODE_SUM ? (ShortCount / (uint)sQuote.SpreadQty[i]) : (LongCount / (uint)sQuote.SpreadQty[i]));
                }
                else
                {
                    nPos = (sQuote.RateMode[i - 1] != S_SPREADMODE_SUM ? (LongCount / (uint)sQuote.SpreadQty[i]) : (ShortCount / (uint)sQuote.SpreadQty[i]));
                }

                if (nPos == 0)
                {
                    std::string strCode = "";
                    CConvert::PosContractToStr(strCode, sOrder.Contract[i]);
                    strCodeNoPos += strCode + " ";
                }
            }
            else
            {
                sOrder.Offset[i] = oOpen;
            }
        }
        else if (sQuote.RateMode[i] == S_SPREADMODE_SUM)
        {
            sOrder.Offset[i] = offset;
        }
    }
	
    if (!strCodeNoPos.empty())
    {
        wchar_t text[400] = { 0 };
        swprintf_s(text, LoadResWchar_t(IDS_Cover_No_Position), CConvert::UTF8ToUnicode(strCodeNoPos).c_str());
        return MessageBox(m_Hwnd, text, LoadResWchar_t(IDS_TIP), MB_OKCANCEL) == IDOK;
    }

    return true;
}

bool CMousewin::IsOneOfComboCode(std::string strCode, std::string strCombCode)
{
    if (strCode.empty() || strCombCode.empty())
    {
        return false;
    }
    TContractKey combKey;
    memset(&combKey, 0, sizeof(TContractKey));
    if (!(CConvert::TContractNoToTContractKey(strCombCode.c_str(), combKey)))
    {
        return false;
    }
    TContractSingleKey key[2];
    memset(&key[0], 0, sizeof(TContractSingleKey));
    memset(&key[1], 0, sizeof(TContractSingleKey));
    CConvert::CombTContractKeyToTContractSingleKey(combKey, key[0], key[1]);

    string sCode1, sCode2;
    CConvert::PosContractToStr(sCode1, key[0]);
    CConvert::PosContractToStr(sCode2, key[1]);

    return strCode == sCode1 || strCode == sCode2;
}
