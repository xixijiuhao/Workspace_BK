#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern CPlugLoad g_PlugLoad;
extern IStarApi * g_StarApi;

extern KeyBoardConfig *g_KeyBoardConfig;
extern wstring LoadResString(int strID);

#define KQty_Limit6			6
#define KQty_Max6			999999
#define KQty_Limit4			4
#define KQty_Max4			9999
#define KQty_Min			0


enum OF_SysNoticeInfo
{
	OF_USER,	
	OF_CODE,	
	OF_PRICE,
	OF_QTY,		//qty qty_auto qty_default
	OF_DIRECT,
	OF_OFFSET,
	OF_STRATEGY,
	OF_OPENQTY,
	OF_COVERQTY,
	OF_CLICKTYPE
};
//decimal表示四舍五入保留的位置，1-十位，0-个位，-1十分位，其他以此类推
double MyRound(double a, short decimal)
{
	return ((int)(a / pow(10.0, decimal) + 0.5))*pow(10.0, decimal);
}
struct Less_Quote
{
	double					dPrice;
	SQtyType				nQty;
};
bool SortByPriceH(const Less_Quote &v1, const Less_Quote &v2)
{
	return v1.dPrice < v2.dPrice;//升序排列  
}
bool SortByPriceL(const Less_Quote &v1, const Less_Quote &v2)
{
	return v1.dPrice > v2.dPrice;//降序排列  
}
void GetPriceQty(const SQuoteSnapShot& QuoteSnap, double& dbuy, double& dsell, SQtyType& nbuy, SQtyType& nsell)
{
	vector<Less_Quote> lbidquote;
	if (QuoteSnap.Data[S_FID_IMPLIEDBIDPRICE].FidAttr != S_FIDATTR_NONE)
	{
		Less_Quote quote;
		quote.dPrice = QuoteSnap.Data[S_FID_IMPLIEDBIDPRICE].Price;
		if (!CConvert::DoubleSameValue(0, quote.dPrice, FLOAT_SAME_ERRORDIFF))
		{
			quote.nQty = QuoteSnap.Data[S_FID_IMPLIEDBIDQTY].FidAttr != S_FIDATTR_NONE ? QuoteSnap.Data[S_FID_IMPLIEDBIDQTY].Qty : 0;
			if (quote.nQty)
				lbidquote.push_back(quote);
		}
	}

	if (QuoteSnap.Data[S_FID_BESTBIDPRICE].FidAttr != S_FIDATTR_NONE)
	{
		Less_Quote quote;
		quote.dPrice = QuoteSnap.Data[S_FID_BESTBIDPRICE].Price;
		if (!CConvert::DoubleSameValue(0, quote.dPrice, FLOAT_SAME_ERRORDIFF))
		{
			quote.nQty = QuoteSnap.Data[S_FID_BESTBIDQTY].FidAttr != S_FIDATTR_NONE ? QuoteSnap.Data[S_FID_BESTBIDQTY].Qty : 0;
			if (quote.nQty)
				lbidquote.push_back(quote);
		}
	}

	sort(lbidquote.begin(), lbidquote.end(), SortByPriceL);
	vector<Less_Quote>::iterator iter = lbidquote.begin();
	if (iter != lbidquote.end())
	{
		dbuy = iter->dPrice;
		nbuy = iter->nQty;
	}

	vector<Less_Quote> laskquote;
	if (QuoteSnap.Data[S_FID_IMPLIEDASKPRICE].FidAttr != S_FIDATTR_NONE)
	{
		Less_Quote quote;
		quote.dPrice = QuoteSnap.Data[S_FID_IMPLIEDASKPRICE].Price;
		if (!CConvert::DoubleSameValue(0, quote.dPrice, FLOAT_SAME_ERRORDIFF))
		{
			quote.nQty = QuoteSnap.Data[S_FID_IMPLIEDASKQTY].FidAttr != S_FIDATTR_NONE ? QuoteSnap.Data[S_FID_IMPLIEDASKQTY].Qty : 0;
			if (quote.nQty)
				laskquote.push_back(quote);
		}
	}

	if (QuoteSnap.Data[S_FID_BESTASKPRICE].FidAttr != S_FIDATTR_NONE)
	{
		Less_Quote quote;
		quote.dPrice = QuoteSnap.Data[S_FID_BESTASKPRICE].Price;
		if (!CConvert::DoubleSameValue(0, quote.dPrice, FLOAT_SAME_ERRORDIFF))
		{
			quote.nQty = QuoteSnap.Data[S_FID_BESTASKQTY].FidAttr != S_FIDATTR_NONE ? QuoteSnap.Data[S_FID_BESTASKQTY].Qty : 0;
			if (quote.nQty)
				laskquote.push_back(quote);
		}
	}

	sort(laskquote.begin(), laskquote.end(), SortByPriceH);
	vector<Less_Quote>::iterator iter2 = laskquote.begin();
	if (iter2 != laskquote.end())
	{
		dsell = iter2->dPrice;
		nsell = iter2->nQty;
	}
}
COFInputDlg::COFInputDlg(HWND hPrent, WinProperty wProperty) :
m_LockUserNoBtn(IDB_BITMAP_LOCK)
, m_LockCodeBtn(IDB_BITMAP_LOCK)
, m_OrderPriceEdit(m_FillOrderAssist.uiPricePos, IDC_EDIT_Code, m_CommodityInf, *this)
, m_OrderPriceEditFZ(*this)
, m_TriggerPriceEditFZ(*this)
, m_TriggerPriceEdit(m_FillOrderAssist.uiPricePos, IDC_EDIT_Code, m_CommodityInf, *this)
, m_CodeEdit(*this)
, m_DirectEdit(*this)
, m_OffsetEdit(*this)
, m_OrderQtyEdit(*this)
, m_QtyMultiEdit(*this)
, m_TrigCondiEdit(*this)
, m_MaxQtyEdit(*this)
, m_MinQtyEdit(*this)
, m_QtyCountEdit(*this)
, m_IncreQtyEdit(*this)
, m_IncrePriceEdit(*this)
, m_OrderButton(this)
, m_TrigModeCmb(*this)
, m_OrderTypeCmb(*this)
, m_ValidTypeCmb(*this)
, m_UserNO(*this)
, m_ValidDataCtrl(*this)
, m_bHotKeyAltActive(false)
, m_nInitReady(false)
, m_BuyBtn(*this)
, m_SellBtn(*this)
, m_FillOrderAssist({0})
, m_OrderDealInfo({0})
, m_OrderReturn({0})
, m_fastOrderInfo({0})
, m_bTabling(false)
, m_bLoginTrader(false)
{
	memcpy(&m_wProperty, &wProperty, sizeof(WinProperty));
	memset(&m_FillOrderInfo, 0, sizeof(m_FillOrderInfo));
	Init();
	SetCanLayout(false);
	if (g_PlugLoad.GetCommonModule())
	{
		g_PlugLoad.GetCommonModule()->Regist((ICommonInteract*)this);
		g_PlugLoad.GetCommonModule()->Regist((ICommonInteractFast*)this, InteractType_FastOrder);
	}
	if (g_KeyBoardConfig)
		g_KeyBoardConfig->Regist(this);
	SetCanLayout(true);
}
COFInputDlg::~COFInputDlg()
{
	DeleteObject(m_ctlBrush);
	DeleteObject(m_ctlBrushKS);
	DeleteObject(m_BkBrushDlg);

	DeleteObject(m_fontBtn);
	DeleteObject(m_fontCombo);
	DeleteObject(m_fontEdit);	
	DeleteObject(m_fontCheck);
	DeleteObject(m_fontStatic1);	
	DeleteObject(m_fontStatic2);	
	DeleteObject(m_fontPosition);

	if (g_PlugLoad.GetCommonModule())
	{
		g_PlugLoad.GetCommonModule()->UnRegist((ICommonInteract*)this);
		g_PlugLoad.GetCommonModule()->UnRegist(dynamic_cast<ICommonInteractFast*>(this), InteractType_FastOrder);
	}
	if (g_KeyBoardConfig)
		g_KeyBoardConfig->UnRegist(this);
	if (g_PlugLoad.GetStarApi())
		g_PlugLoad.GetStarApi()->UnsubAllQuote(this);

}
//创建非模态对话框
HWND COFInputDlg::ShowDlg(HINSTANCE hInstance, int idDlg, HWND hWndParent)
{
	if (!hWndParent)
		hWndParent = GetDesktopWindow();
	m_hDlg = CreateDlg(hInstance, idDlg, hWndParent);
	m_hParent = hWndParent;
	::ShowWindow(m_hDlg, SW_HIDE);
	return m_hDlg;
}
//初始化
void COFInputDlg::Init()
{
	//是否能够layout
	SetCanLayout(true);
	//字体
	m_fontBtn = NULL;
	m_fontCheck = NULL;
	m_fontStatic1 = NULL;
	m_fontStatic2 = NULL;
	m_fontCombo = NULL;
	m_fontEdit = NULL;
	m_fontPosition = NULL;
	memset(&m_LastBuyQty, 0, sizeof(TTradableQty));
	memset(&m_LastSellQty, 0, sizeof(TTradableQty));
	memset(&m_LastBuyFreeze, 0, sizeof(TPreFreeze));
	memset(&m_LastSellFreeze, 0, sizeof(TPreFreeze));
	m_iOrginBtnWidth = 0;
	m_iOrginBtnFontWidth = 0;
	m_ctlBrush = NULL;
	m_ctlBrushKS = NULL;
	m_BkBrushDlg = NULL;
	SetValidFillTrigger(false);
	m_MaxOrdID = 0;
	m_bValidGroup = false;
	m_bModifyOrderOper = false;
	m_bModifyOrderOperBatch = false;
	memset(&m_ModifyOrder, 0, sizeof(TModifyOrder));
}
//行情通知
void __stdcall COFInputDlg::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
	if (m_hDlg)
		PostMessage(m_hDlg, WM_QUOTCHG, (WPARAM)0, 0);
}
//TraderSpi
void __cdecl COFInputDlg::OnOrder(const TOrder * pOrder)
{
	string str = "";
	if (pOrder->ErrorCode != 0)
		str = pOrder->ErrorText;
	if (m_nInitReady)//有资金账号初始化成功 再更新显示信息
		ShowTip(LoadRC::ansi_to_unicode(str));

	if (pOrder)//最新定单号更新	
		m_MaxOrdID = pOrder->OrderID;
}
//TraderSpi
void __cdecl COFInputDlg::OnDisconnect(const int ErrorCode, const TLogin * pLogin)
{
	if (::IsWindow(m_hDlg))
	{
		LoadVersionType();
		SetCtrlFont();
		SetUser();//获得用户名设置 
		Layout();
	}
}
//TraderSpi
void __cdecl COFInputDlg::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
	if (CurrProgress == tipReady)
	{
		if (::IsWindow(m_hDlg))
		{
			LoadVersionType();
			SetCtrlFont();
			SetUser();//获得用户名设置	
			SetOrderTypeCmbAtrribute();//定单类型
			bool bOldState = m_bLoginTrader;
			m_bLoginTrader = CConvert::IsTraderLogined();
			if (bOldState != m_bLoginTrader)
				ResetTJCheckBox(false,false);
			Layout();
			
			m_thread.SetVersion(VersionType());
			m_thread.SetSign(GetLoginSign());
			m_thread.Start();
		}
	}
}
//TraderSpi
void __cdecl COFInputDlg::OnPosition(const TPosition * pPosition)//只要是这个账户的的下属客户有持仓变化就会触发该函数 通过GetPositionVol获取最新的 pPosition中的量不用管 还有启动时肯定也要通过GetPositionVol获取一下
{
	UpdatePositionInf();
}
//TraderSpi
void COFInputDlg::OnLogin(const int ErrorCode, const TLogin * pLogin)
{
	
}
//TraderSpi
void COFInputDlg::OnPreFrozen(const TPreFreeze * pPreFreeze)		//收到当前定单占用保证金回调
{
	CMyLock::Guard lock(m_lock);

	if (pPreFreeze == NULL || pPreFreeze->pCurrency == NULL)
		return;
	if (pPreFreeze->Direct == dBuy)
	{
		memcpy_s(&m_BuyPreFreezetemp, sizeof(TPreFreeze), pPreFreeze, sizeof(TPreFreeze));
		PostMessage(m_hDlg, WM_DEALPREFREEZE, m_BuyPreFreezetemp.Direct, 0);
	}
	else if (pPreFreeze->Direct == dSell)
	{
		memcpy_s(&m_SellPreFreezetemp, sizeof(TPreFreeze), pPreFreeze, sizeof(TPreFreeze));
		PostMessage(m_hDlg, WM_DEALPREFREEZE, m_SellPreFreezetemp.Direct, 0);
	}
}
//TraderSpi
void COFInputDlg::OnTradableQty(const TTradableQty * pTradableQty)         	//收到当前可下单数量回调
{
	CMyLock::Guard lock(m_lock);

	if (!pTradableQty)
		return;
	if (pTradableQty->Direct == dBuy)
	{
		memcpy_s(&m_BuyTradableQtytemp, sizeof(TTradableQty), pTradableQty, sizeof(TTradableQty));
		//账号
		if (strcmp(m_CurUserNo.c_str(), m_BuyTradableQtytemp.UserNo))
			return;
		PostMessage(m_hDlg, WM_DEALTRADABLEQTY, m_BuyTradableQtytemp.Direct, 0);
	}
	else if (pTradableQty->Direct == dSell)
	{
		memcpy_s(&m_SellTradableQtytemp, sizeof(TTradableQty), pTradableQty, sizeof(TTradableQty));
		//账号
		if (strcmp(m_CurUserNo.c_str(), m_SellTradableQtytemp.UserNo))
			return;
		PostMessage(m_hDlg, WM_DEALTRADABLEQTY, m_SellTradableQtytemp.Direct, 0);
	}
}
void COFInputDlg::OnDealPreFreeze(char cDirect)							//处理预冻结
{
	CMyLock::Guard lock(m_lock);

	if (cDirect == dBuy)
	{
		TCurrency tcurrency;
		memcpy_s(&tcurrency, sizeof(TCurrency), m_BuyPreFreezetemp.pCurrency, sizeof(TCurrency));
		const TCurrency* tCur = g_pTradeData->GetCurrency(tcurrency.CurrencyNo, tcurrency.Sign);
		if (tCur == NULL)
			return;
		
		if (m_CurUserNo != m_BuyPreFreezetemp.UserNo)
			return;
		//合约
		char strCode[51] = {0};
		GetWindowTextA(GetDlgItem(m_hDlg, IDC_EDIT_Code), strCode,51);

		string sCode = "";
		CConvert::TContractKeyToTContractNo((TContractKey&)m_BuyPreFreezetemp, sCode);
		if (sCode != strCode)
			return;
		//获取委托数量
		uint nQty = 0;
		if (!GetCtrlOrderNum(nQty, false, false))
			return;
		else if (nQty != m_BuyPreFreezetemp.OrderQty)
			return;
		memcpy_s(&m_LastBuyFreeze, sizeof(TPreFreeze), &m_BuyPreFreezetemp, sizeof(TPreFreeze));
		ShowOrderFrozen(&m_BuyPreFreezetemp);
	}
	else if (cDirect == dSell)
	{
		TCurrency tcurrency;
		memcpy_s(&tcurrency, sizeof(TCurrency), m_SellPreFreezetemp.pCurrency, sizeof(TCurrency));
		const TCurrency* tCur = g_pTradeData->GetCurrency(tcurrency.CurrencyNo, tcurrency.Sign);
		if (tCur == NULL)
			return;

		if (m_CurUserNo != m_SellPreFreezetemp.UserNo)
			return;
		char strCode[51] = { 0 };
		GetWindowTextA(GetDlgItem(m_hDlg, IDC_EDIT_Code), strCode, 51);

		string sCode = "";
		CConvert::TContractKeyToTContractNo((TContractKey&)m_SellPreFreezetemp, sCode);
		if (sCode != strCode)
			return;
		//获取委托数量
		uint nQty = 0;
		if (!GetCtrlOrderNum(nQty, false, false))
			return;
		else if (nQty != m_SellPreFreezetemp.OrderQty)
			return;
		memcpy_s(&m_LastSellFreeze, sizeof(TPreFreeze), &m_SellPreFreezetemp, sizeof(TPreFreeze));
		ShowOrderFrozen(&m_SellPreFreezetemp);
	}
}
void COFInputDlg::OnDealTradableQty(char cDirect)							//处理可开可冲
{
	CMyLock::Guard lock(m_lock);

	char cdirect = dNone;
	GetDirect(cdirect, false, false);
	if (cdirect != cDirect)
		return;
	if (cDirect == dBuy)
	{
		if (m_BuyTradableQtytemp.OpenQty < 0 || m_BuyTradableQtytemp.OpenQty>100000000 || m_BuyTradableQtytemp.CoverQty < 0 || m_BuyTradableQtytemp.CoverQty>10000000)
			return;
		//保存最后一次数据
		memcpy_s(&m_LastBuyQty, sizeof(TTradableQty), &m_BuyTradableQtytemp, sizeof(TTradableQty));
		//显示数量
		ShowCanOrderVol(&m_BuyTradableQtytemp);
	}
	else if (cDirect == dSell)
	{
		if (m_SellTradableQtytemp.OpenQty < 0 || m_SellTradableQtytemp.OpenQty>100000000 || m_SellTradableQtytemp.CoverQty < 0 || m_SellTradableQtytemp.CoverQty>10000000)
			return;
		memcpy_s(&m_LastSellQty, sizeof(TTradableQty), &m_SellTradableQtytemp, sizeof(TTradableQty));
		//显示数量
		ShowCanOrderVol(&m_SellTradableQtytemp);
	}
}
void COFInputDlg::OnCommonCfgChanged(CM_SUBID PartID, char* pdata)
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
	}
}
bool COFInputDlg::GetFastInfo(sFastInfo& sInfo)
{
	if (m_CheckKS.GetCheck() && IsWindowVisible(m_CheckKS.GetHwnd()))//勾选快速 快速在顶层窗口可见/弹出窗口可见
	{
		TradeLoginLinkage();		
		//HWND hwnd = GetFocus();
		//if (hwnd)
		//{
		//	wchar_t w[20] = { 0 };
		//	GetClassName(hwnd, w, 20);
		//	if (!wcscmp(L"Edit", w) || !wcscmp(L"SysDateTimePick32", w))
		//		return false;
		//}
		//else
		//	return false;

		sInfo.bFastOrder = true;
		//合约
		string sCode = "";
		sCode = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
		GetContractStr(sCode);//
		TContractKey key;
		memset(&key, 0, sizeof(key));
		memcpy(key.Sign, m_CurLoginNo.c_str(), sizeof(key.Sign));
		if (CConvert::TContractNoToTContractKey(sCode.c_str(), key) && CConvert::IsValidTradeCommodity(key))//有效交易合约
		{
			//有效资金账号
			if (!GetCtrlUser(sInfo.sFastUserNo))
				memset(sInfo.sFastUserNo, 0, sizeof(sInfo.sFastUserNo));
			//有效类型
			sInfo.ValidType = GetValidType();
			if (sInfo.ValidType == vtGTD)
			{
				string str = GetValidTime();
				if (!str.empty())
					strcpy_s(sInfo.ValidTime, str.c_str());//有效日期
			}
		
			//数量倍率
			wchar_t sMulti[5] = {0};
		 	GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti), sMulti,5);
			double nMutil = _wtof(sMulti);
			sInfo.dQtyRatio = (nMutil >= 0 ? nMutil : 0);
			//服务器标识 合约代码
			strncpy_s(sInfo.sFastSign, m_CurLoginNo.c_str(), sizeof(sInfo.sFastSign));
			strncpy_s(sInfo.sFastCode, sCode.c_str(), sizeof(sInfo.sFastCode));
			
			//反手
			sInfo.bBackHand = IsCheck(IDC_CHECK_PK);
			//T+1
			sInfo.bAddOne = IsCheck(IDC_CHECK_T1);
			//批次
			if (sInfo.bBatchTimes = IsCheck(IDC_Check_BatchCount))
			{
				//下单次数
				bool bvalid = GetPCQtyCount(sInfo.uiOrderTimes, false, false);
				if (!bvalid)
				{
					ShowTip(LoadResString(IDS_InValidOrderCounts));
					return false;
				}
					

				//数量增量
				bvalid = GetPCQtyIncre(sInfo.uiQtyInc, false, false);
				if (!bvalid)
				{
					ShowTip(LoadResString(IDS_InValidQtyInc));
					return false;
				}

				//价格增量
				double dIncrePrice = 0;
				bvalid = GetPCPriceIncre(sInfo.dPriceInc, false, false);
				if (!bvalid)
				{
					ShowTip(LoadResString(IDS_InValidPriceInc));
					return false;
				}
			}
			
			//条件 
			if (sInfo.bcondition = IsCheck(IDC_CHECK_TJ))
			{
				wchar_t str[20] = {0};
				//条件
				GetWindowTextW(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), str,20);
				if (wcslen(str)!=0)
					sInfo.cTrigMode = CTradeConst::StrToTriggerMode(str);
				else
				{
					ShowTip(LoadResString(IDS_STR_TirgModeEmpty));
					return false;// 
				}
								

				//触发条件
				GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), str,20);
				if (wcslen(str) != 0)
					sInfo.cTrigCondition = CTradeConst::StrToTriggerCondition(str);
				else
				{
					ShowTip(LoadResString(IDS_STR_TirgCondiEmpty));
					return false;//
				}		

				//触发价格
				if (!GetCtrlPrice(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, sInfo.dTrigPrice, false, false,fptTrig))
				{
					ShowTip(LoadResString(IDS_STR_InvalidPrice));
					return false;
				}				
			}

			//批量
			sInfo.bBatchQuantities = IsCheck(IDC_Check_BatchOrder);
			sInfo.bRecove = false;

			ShowTip(TEXT(""));
			return true;
		}
		else
		{
			ShowTip(LoadResString(IDS_STR_InvalidTradeCode));
			return false;
		}
			
	}
	return false;
}
void COFInputDlg::OnFastError(const ErrorReflectInfo* eInfo)
{
	if (eInfo)
		ShowTip(eInfo->error_text);
}
void  COFInputDlg::OnFastOrderInfo(const TSendOrder* TSOrder, double dRatio)
{
	//买卖//开平//价格//数量
	if (TSOrder)
	{
		//买卖  开平 双击买价填卖出，双击卖价填买入
		wchar_t wstr[10] = { 0 };
		CTradeConst::DirectToStr(TSOrder->Direct, wstr, 10);
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Direct), wstr);
		CTradeConst::OffsetToStr(TSOrder->Offset, wstr, 10);
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Offset), wstr);
		//委托价格
		if (IsShowBuySellBtn() && m_SwitchFollow.GetSwitchLevel() == SwitchLevel2)
			;
		else
			SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, TSOrder->OrderPrice);
		
		SetWindowInt(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), TSOrder->OrderQty);

		//需要确认，通用模块不发单，那么就设置焦点到确定Button上
		if (m_OrderDealInfo.bFastOrderConfirm)
		{
			SetFocus(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder));
		}
	}
	//数量倍率
	if (!CConvert::DoubleSameValue(dRatio,0,FLOAT_SAME_ERRORDIFF))
		SetWindowDouble(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti),1,dRatio);	
}
void COFInputDlg::OnFastStateInfo(int nstate)
{
	//取消勾选 同OnBnClickedCheckKS()
	if (m_CheckKS.GetCheck()==BST_CHECKED)
	{
		m_CheckKS.SetCheck(BST_UNCHECKED);
		ClearTipBar();
		//定单类型显示 
		if (1 == VersionType() || 2 == VersionType())
			SetOrderTypeCmbAtrribute();
		DateTime_SetMonthCalColor(GetDlgItem(m_hDlg, IDC_DATETIMEPICKER_ValidData), (WPARAM)MCSC_TEXT, (LPARAM)g_ColorRefData.GetColorTextRed());		
		if (nstate == 1)
		{	
			EnableWindow(m_SwitchFollow.GetHwnd(), false);
			m_CheckKS.SetColor(g_ColorRefData.GetColorBuyRed());
		}	
		else
		{
			EnableWindow(m_SwitchFollow.GetHwnd(), true);
			m_CheckKS.SetColor(g_ColorRefData.GetColorBlack());
		}
			
		Layout();
	}
	else
	{
		//只更新 控件状态
		if (nstate == 1)
		{
			if (m_CheckKS.GetColor() == g_ColorRefData.GetColorBuyRed())
				return;
			EnableWindow(m_SwitchFollow.GetHwnd(), false);
			m_CheckKS.SetColor(g_ColorRefData.GetColorBuyRed());
		}	
		else
		{
			if (m_CheckKS.GetColor() == g_ColorRefData.GetColorBlack())
				return;
			EnableWindow(m_SwitchFollow.GetHwnd(), true);
			m_CheckKS.SetColor(g_ColorRefData.GetColorBlack());
		}	
		Layout();
	}
}

//IOperator
void COFInputDlg::SetContractNo(string sCode, bool brepaint, bool bnutton, bool bgetfreeze)
{
	string strcode = sCode;
	ModifyContractStr(strcode);
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Code), LoadRC::ansi_to_unicode(strcode).c_str());
	//更新行情报价
	char contractid[101] = { 0 };
	if (CConvert::StrToQContractNoType(sCode.c_str(), contractid))
	{
		string str = contractid;
		str = "contractid=" + str;
		g_PlugLoad.GetMainFrm()->Linkage(m_hDlg, "MouseOrder", W_SETCONTRACT, str.c_str());
	}
}

//处理消息
INT_PTR COFInputDlg::DlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (EN_SETFOCUS == (UINT)HIWORD(wParam) && !m_bTabling)
	{
		//ime输入法更改为英文状态
		LoadKeyboardLayout(_T("0x0409"), KLF_ACTIVATE | KLF_SETFORPROCESS);
	}

	switch (message)
	{
		case WM_INITDIALOG:		
			return OnInitDialog();		
		case WM_COMMAND:		
			return OnCommandCOFInputDlg(wParam, lParam);		
		case WM_CTLCOLORSTATIC:
		{
			HDC hDC = (HDC)wParam;
			SetBkMode(hDC, TRANSPARENT);
			return (INT_PTR)m_BkBrushDlg;
		}	
		case WM_CTLCOLOREDIT:
		{
			HDC hDC = (HDC)wParam;
			SetBkMode(hDC, TRANSPARENT);
			HBRUSH hbrush = OnCtlColor(wParam, lParam);
			return (INT_PTR)hbrush;
		}
		case WM_CTLCOLORDLG:	
			return (INT_PTR)m_BkBrushDlg;	
		case WM_QUOTCHG:
			OnQuotInfo(wParam, lParam);
			break;
		case WM_DRAWITEM:
		{
			int id = (int)wParam;
			DRAWITEMSTRUCT *pDraw = (DRAWITEMSTRUCT*)(lParam);
			if (id == IDC_ComUserNo)
			{
				m_UserNO.DrawItem(pDraw);
				return TRUE;
			}
			else if (id == IDC_Btn_Buy || id == IDC_Btn_Sell)
			{
				if (id == IDC_Btn_Buy)
					m_BuyBtn.DrawItem(pDraw);
				else
					m_SellBtn.DrawItem(pDraw);
				return TRUE;
			}
		}
		break;
		case WM_MEASUREITEM:
		{
			int id = (int)wParam;
			MEASUREITEMSTRUCT*pMeasure = (MEASUREITEMSTRUCT*)(lParam);
			if (id == IDC_ComUserNo)
			{
				m_UserNO.MeasureItem(pMeasure);
				return TRUE;
			}
		}
		break;
		case WM_DELETEITEM:
		{
			int id = (int)wParam;
			MEASUREITEMSTRUCT*pMeasure = (MEASUREITEMSTRUCT*)(lParam);
			if (id == IDC_ComUserNo)
			{
				return TRUE;
			}
		}
		break;
		case WM_NOTIFY:
			return OnNotifyCOFInputDlg(wParam, lParam);
		case WM_MOUSEWHEEL:				
			OnMouseWheel(wParam, lParam);
			break;
		case WM_OFHOTKEY:
			OnDealHotKey(wParam, lParam);
			break;
		case WM_NCDESTROY:
			GetOFInputDlg()->DeleteObject(this);
			return FALSE;
		case WM_BATCHORDER:
			OnBatchOrderInfoCHG();//批量配置更新
			break;
		case WM_POSITIONSHOW:
			UpdatePositionInf();//持仓显示配置 更新
			break;
		case WM_CONDITION:
			UpdateTJState();
			break;
		case WM_DEALTRADABLEQTY:
			OnDealTradableQty(wParam);
			break;
		case WM_DEALPREFREEZE:
			OnDealPreFreeze(wParam);
			break;	
		case WM_LBUTTONDOWN:
			if (m_KeyBoardSet.bSetDefaultPos)
				SetIndexFocus(m_KeyBoardSet.uiFocusPos - 1);
			else
				SetFocus(m_hDlg);
			break;
		case WM_DEALQUOTECOMMON:
			OnDealQuoteCommon();
			break;
		case WM_DEALPOSCOMMON:
		case WM_DEALORDERCOMMON:
			OnDealPositionOrderCommon();
			break;
		case WM_DEALKLINECONTRACT:
			OnDealQuoteKLine();
			break;
		default:
			return FALSE;
	}
	return TRUE;
}
//处理命令消息
BOOL COFInputDlg::OnCommandCOFInputDlg(WPARAM wParam, LPARAM lParam)
{
	int id = (int)(LOWORD(wParam));
	HWND hwndCtl = (HWND)(lParam);
	UINT codeNotify = (UINT)HIWORD(wParam);
		switch (id)
	{
		case IDC_Btn_SelCode:
			OnBnClickedBtnSelcode();
			break;
		case IDC_CHECK_TJ:
			OnBnClickedCheckTJ();
			break;
		case IDC_CHECK_PK:
			OnBnClickedCheckPK();
			break;
		case IDC_CHECK_KS:
			OnBnClickedCheckKS();
			break;
		case IDC_Check_BatchCount:
			OnBnClickedCheckBatchCount();
			break;
		case IDC_Check_BatchOrder:
			OnBnClickedCheckBatchOrder();
			break;
		case IDC_CHECK_T1:
			OnBnClickedCheckT1();
			break;
		case IDC_CHECK_ForCover:
			OnBnClickedCheckForCover();
			break;
		case IDC_BUTTON_DoOrder:
			OnBnClickedButtonDoOrder();
			break;
		case IDC_BUTTON_PayOrder:
			OnBnClickedButtonPayOrder();
			break;
		case IDC_BUTTON_AutoOrder:
			OnBnClickedButtonAutoOrder();
			break;
		case IDC_Btn_Seq:
			OnBnClickedButtonSeq();
			break;
		case IDC_Btn_Buy:
			OnBnClickedBtnBuy();
			break;
		case IDC_Btn_Sell:
			OnBnClickedBtnSell();
			break;
		case IDC_Btn_LockUserNo:
			OnBnClickedBtnLockuserno();
			break;
		case IDC_Btn_LockCode:
			OnBnClickedBtnLockcode();
			break;
		case IDC_ComUserNo:
		{
			if (codeNotify == CBN_EDITUPDATE)
			{
				m_UserNO.OnCbnEditupdate();
				return TRUE;
			}
			else if (CBN_SELCHANGE == codeNotify)
			{
				OnCbnSelChangeUserNo();
				return TRUE;
			}
			else if (CBN_EDITCHANGE == codeNotify)
			{
				OnCbnEditchangeComuserno();
				return TRUE;
			}
		}
		break;
		case IDC_STATIC_Direct:
		{
			if (codeNotify == STN_CLICKED)
				OnStnClickedStaticDirect();
		}
			break;
		case IDC_STATIC_Offset:
		{
			if (codeNotify == STN_CLICKED)
				OnStnClickedStaticOffset();
		}
			break;
		case IDC_EDIT_Direct:
		{
			if (EN_SETFOCUS == codeNotify)
				OnEnSetfocusEditDirect();
			else if (EN_KILLFOCUS == codeNotify)
				ShowTip(_T(""));
		}
		break;
		case IDC_EDIT_Offset:
		{
			if (EN_SETFOCUS == codeNotify)
				OnEnSetfocusEditOffset();
			else if (EN_KILLFOCUS == codeNotify)
				ShowTip(_T(""));
		}
			break;
		case IDC_COMBO_TirgMode:
		{
			if (codeNotify == CBN_SETFOCUS)
				OnCbnSetfocusComboTirgMode();
			else if (EN_KILLFOCUS == codeNotify)
				ShowTip(_T(""));
		}
		break;
		case IDC_EDIT_OrderPrice:
		{
			FormatStaticDepositDisplay();
			if (EN_SETFOCUS == codeNotify)
				//OnEnSetfocusEditOrderPrice();
			{
			}
			else if (EN_KILLFOCUS == codeNotify)
				ShowTip(_T(""));
		}
			break;
		case IDC_EDIT_OrderQty:
		{
			FormatStaticDepositDisplay();
			if (EN_SETFOCUS == codeNotify)
			{
				OnEnSetfocusEditOrderQty();
				
			}
			else if (EN_KILLFOCUS == codeNotify)
				ShowTip(_T(""));
		}
			break;
		case IDC_EDIT_TrigPrice:
		{
			if (EN_SETFOCUS == codeNotify)
				OnEnSetfocusEditTrigPrice();
			else if (EN_UPDATE == codeNotify)
				OnEnUpdateEditOrderQty();
			else if (EN_KILLFOCUS == codeNotify)
				ShowTip(_T(""));
		}
			break;
		case IDC_EDIT_QtyMulti:
		{
			if (EN_SETFOCUS == codeNotify)
				OnEnSetfocusEditQtyMulti();
			//else if (EN_UPDATE == codeNotify)
				//OnEnUpdateEditQtyMulti();
		}
			break;
		case IDC_EDIT_Code:
		{
			if (EN_CHANGE == codeNotify)
				OnEnChangeEditCode();
			else if (EN_KILLFOCUS == codeNotify)
				OnEnKillFocusEditCode();
			else if (EN_SETFOCUS == codeNotify)
				::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_Code), EM_SETSEL, 0, -1);
		}
		break;
		case IDC_COMBO_OrderType:
		{
			if (CBN_SELCHANGE == codeNotify)
				OnCbnSelchangeComboOrdertype();
			if (codeNotify == CBN_SETFOCUS)
				OnCbnSetfocusComboOrderType();
		}
		break;
		case IDC_COMBO_ValidType:
		{
			if (CBN_SELCHANGE == codeNotify)
				OnCbnSelchangeComboValidtype();
		}
		break;
		case IDC_CHECK_AutoPrice:
			OnBnClickedCheckAutoprice();
		break;
		case IDC_EDIT_TrigCondi:
			OnEnSetfocusEditTrigType();
		break;
		default:
			return FALSE;
	}
	return TRUE;
}
//处理通告消息
BOOL COFInputDlg::OnNotifyCOFInputDlg(WPARAM wParam, LPARAM lParam)
{
	NMHDR*pNmhdr = (NMHDR*)lParam;
	if (UDN_DELTAPOS == pNmhdr->code)
	{
		LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNmhdr);
		if (pNMUpDown->hdr.idFrom == IDC_SPIN_OrderPrice)
			OnDeltaposSpinOrderPrice(pNMUpDown->iDelta);
		else if (pNMUpDown->hdr.idFrom == IDC_SPIN_OrderQty)
			OnDeltaposSpinOrderQty(pNMUpDown->iDelta);
		else if (pNMUpDown->hdr.idFrom == IDC_SPIN_QtyMulti)
			OnDeltaposSpinQtyMulti(pNMUpDown->iDelta);
		else if (pNMUpDown->hdr.idFrom == IDC_SPIN_TrigPrice)
			OnDeltaposSpinTrigPrice(pNMUpDown->iDelta);
		else if (pNMUpDown->hdr.idFrom == IDC_SPIN_QtyCount)
			OnDeltaposSpinQtycount(pNMUpDown->iDelta);
		else if (pNMUpDown->hdr.idFrom == IDC_SPIN_IncreQty)
			OnDeltaposSpinIncreqty(pNMUpDown->iDelta);
		else if (pNMUpDown->hdr.idFrom == IDC_SPIN_IncrePrice)
			OnDeltaposSpinIncreprice(pNMUpDown->iDelta);
		else if (pNMUpDown->hdr.idFrom == IDC_SPIN_MaxQty)
			OnDeltaposSpinMaxqty(pNMUpDown->iDelta);
		else if (pNMUpDown->hdr.idFrom == IDC_SPIN_MinQty)
			OnDeltaposSpinMinqty(pNMUpDown->iDelta);
		return TRUE;
	}
	return FALSE;
}
//处理条件check
void COFInputDlg::OnBnClickedCheckTJ()
{
	(m_CheckTJ.GetCheck() == BST_CHECKED) ? m_CheckTJ.SetCheck(BST_UNCHECKED) : m_CheckTJ.SetCheck(BST_CHECKED);
	//条件单与定单类型（止损、限价止损不能同时存在） 依条件为准 选中条件后 就把定单类型改为限价类型 并且不能切换
	ClearTipBar();
	if (m_CheckTJ.GetCheck()==BST_CHECKED)
		UpdateTJState();
	//定单类型显示 
	if (1 == VersionType() || 2 == VersionType())
		SetOrderTypeCmbAtrribute();
	Layout();//yw add
}
//处理平开check
void COFInputDlg::OnBnClickedCheckPK()
{
	(m_CheckPK.GetCheck() == BST_CHECKED) ? m_CheckPK.SetCheck(BST_UNCHECKED) : m_CheckPK.SetCheck(BST_CHECKED);
	ClearTipBar();
}
//处理快速check
void COFInputDlg::OnBnClickedCheckKS()
{
	(m_CheckKS.GetCheck() == BST_CHECKED) ? m_CheckKS.SetCheck(BST_UNCHECKED) : m_CheckKS.SetCheck(BST_CHECKED);

	ClearTipBar();
	//定单类型显示 
	if (1 == VersionType() || 2 == VersionType())
		SetOrderTypeCmbAtrribute();
	DateTime_SetMonthCalColor(GetDlgItem(m_hDlg, IDC_DATETIMEPICKER_ValidData), (WPARAM)MCSC_TEXT, (LPARAM)g_ColorRefData.GetColorTextRed());
	
	if (m_CheckKS.GetCheck() == BST_CHECKED)
	{
		m_CheckKS.SetColor(g_ColorRefData.GetColorBuyRed());
		m_SwitchFollow.SetSwitchLevel(0);
		EnableWindow(m_SwitchFollow.GetHwnd(), false);
		g_PlugLoad.GetCommonModule()->StartFastOrder(this);
	}	
	else
	{
		m_CheckKS.SetColor(g_ColorRefData.GetColorBlack());
		EnableWindow(m_SwitchFollow.GetHwnd(), true);
		if (g_PlugLoad.GetCommonModule() != nullptr) {
			g_PlugLoad.GetCommonModule()->StopFastOrder(this);
		}

	}
	Layout();
}
//处理批次check
void COFInputDlg::OnBnClickedCheckBatchCount()
{
	(m_CheckPC.GetCheck() == BST_CHECKED) ? m_CheckPC.SetCheck(BST_UNCHECKED) : m_CheckPC.SetCheck(BST_CHECKED);
	if (IsCheck(IDC_Check_BatchCount) && IsCheck(IDC_Check_BatchOrder))
	{
		m_CheckPL.SetCheck(BST_UNCHECKED);
		ModifyStaticUserName();
		SetUser(m_bValidGroup);
	}
	ClearTipBar();
	Layout();
}
//处理批量check
void COFInputDlg::OnBnClickedCheckBatchOrder()
{
	(m_CheckPL.GetCheck() == BST_CHECKED) ? m_CheckPL.SetCheck(BST_UNCHECKED) : m_CheckPL.SetCheck(BST_CHECKED);
	if (IsCheck(IDC_Check_BatchCount) && IsCheck(IDC_Check_BatchOrder))
	{
		m_CheckPC.SetCheck(BST_UNCHECKED);
		Layout();
	}
	ClearTipBar();
	ModifyStaticUserName();
	//清空资金账号信息
	if (IsCheck(IDC_Check_BatchOrder))
		UpdateLoginSign("", "");
	SetUser(m_bValidGroup);
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), L"");
}
//处理T1check
void COFInputDlg::OnBnClickedCheckT1()
{
	(m_CheckT1.GetCheck() == BST_CHECKED) ? m_CheckT1.SetCheck(BST_UNCHECKED) : m_CheckT1.SetCheck(BST_CHECKED);
	UpdatePropertyCfg();
}
//处理强平check
void COFInputDlg::OnBnClickedCheckForCover()
{
	(m_CheckQP.GetCheck() == BST_CHECKED) ? m_CheckQP.SetCheck(BST_UNCHECKED) : m_CheckQP.SetCheck(BST_CHECKED);
}
//处理ctrlcolor
HBRUSH COFInputDlg::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	HDC hDC = (HDC)wParam;
	HWND hWnd = (HWND)lParam;
	//if (IsCheck(IDC_CHECK_KS))
	//{
	//	SetWindowLong(m_hDlg, DWL_MSGRESULT, (LONG)m_ctlBrush);
	//	return m_ctlBrushKS;
	//}
	wchar_t str[10] = {0};
	if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_Direct) /*&& m_KeyBoardSet.bDirectHighLight*/)//买卖框
	{
		GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), str,10);
		if (str == LoadResString(IDS_Direct_Buy))
			SetTextColor(hDC, g_ColorRefData.GetColorBuyRed());
		else
			SetTextColor(hDC, g_ColorRefData.GetColorTextGreen());
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_Offset) /*&& m_KeyBoardSet.bDirectHighLight*/)//开平框
	{
		GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Offset), str, 10);
		if (str == LoadResString(IDS_Offset_Open))
			SetTextColor(hDC, g_ColorRefData.GetColorBuyRed());
		else
			SetTextColor(hDC, g_ColorRefData.GetColorTextGreen());
	}
	SetWindowLong(m_hDlg, DWL_MSGRESULT, (LONG)m_ctlBrush);
	return m_ctlBrush;
}
//处理确定按钮
void COFInputDlg::OnBnClickedButtonDoOrder()
{
	if (GetModifyOrderOper())
	{
		if (!GetDlgCtrlInput(m_ModifyOrder, true, true))	return;
		
		if (m_ModifyOrder.OrderQty < m_ModifyOrder.MatchQty)
		{
			MessageBox(m_hDlg, LoadResString(IDS_OrderQtyInvalid).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
			return;
		}

		bool bDeal = false;
		if (m_bModifyOrderOperBatch)
			bDeal = g_PlugLoad.GetCommonModule()->BatchModify(&m_ModifyOrder);
		else
			bDeal = g_PlugLoad.GetCommonModule()->ModifyOrder(m_ModifyOrder);
		if (GetModifyOrderOper() && bDeal)
			OnDealModifyRecover();
		return;
	}
	DoOrder('\0');
	if (m_KeyBoardSet.bSetDefaultPos)
		SetIndexFocus(m_KeyBoardSet.uiFocusPos - 1);
}
//处理埋单按钮
void COFInputDlg::OnBnClickedButtonPayOrder()
{
	DoOrder(stPreOrder);// 埋单
	if (m_KeyBoardSet.bSetDefaultPos)
		SetIndexFocus(m_KeyBoardSet.uiFocusPos - 1);
}
//处理自动弹按钮
void COFInputDlg::OnBnClickedButtonAutoOrder()
{
	DoOrder(stAutoOrder);// 自动单
	if (m_KeyBoardSet.bSetDefaultPos)
		SetIndexFocus(m_KeyBoardSet.uiFocusPos - 1);
}
//处理顺序按钮
void COFInputDlg::OnBnClickedButtonSeq()
{
	if (GetModifyOrderOper())
	{
		SetModifyOrderOper(false);
		Layout();
		if (g_PlugLoad.GetMainFrm())
			g_PlugLoad.GetMainFrm()->Linkage(m_hDlg, "CommonModule", WM_MODIFY_RECOVER, "");
		return;
	}
	if (g_KeyBoardConfig)//顺序配置窗口
	{
		if (!IsWindow(g_KeyBoardConfig->GetKeyBoardSeqHWND()))//窗口资源是否被销毁 销毁则重新创建
			g_KeyBoardConfig->CreateKeyBoardSeqWnd(TEXT("SequenceSet"));
		else
			ShowWindow(g_KeyBoardConfig->GetKeyBoardSeqHWND(),SW_SHOW);
	}
}
//处理买入按钮
void COFInputDlg::OnBnClickedBtnBuy()
{
	DoOrder('\0', dBuy);
	if (m_KeyBoardSet.bSetDefaultPos)
		SetIndexFocus(m_KeyBoardSet.uiFocusPos - 1);
}
//处理卖出按钮
void COFInputDlg::OnBnClickedBtnSell()
{
	DoOrder('\0', dSell);
	if (m_KeyBoardSet.bSetDefaultPos)
		SetIndexFocus(m_KeyBoardSet.uiFocusPos - 1);
}
//处理点击静态控件方向
void COFInputDlg::OnStnClickedStaticDirect()
{
	//点击了买/卖标签,计算下点击的位置
	POINT pos;
	GetCursorPos(&pos);
	::ScreenToClient(GetDlgItem(m_hDlg, IDC_STATIC_Direct), &pos);
	wchar_t wstr[10] = {0};
	if (pos.x < GetMiddlePos(IDC_STATIC_Direct))
		CTradeConst::DirectToStr(dBuy, wstr, 10);
	else
		CTradeConst::DirectToStr(dSell, wstr, 10);
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), wstr);
}
//处理点击静态控件开平
void COFInputDlg::OnStnClickedStaticOffset()
{
	POINT pos;
	GetCursorPos(&pos);
	::ScreenToClient(GetDlgItem(m_hDlg, IDC_STATIC_Offset), &pos);
	wchar_t wstr[10] = { 0 };
	if (pos.x < GetMiddlePos(IDC_STATIC_Offset))
		CTradeConst::OffsetToStr(oOpen,wstr,10);
	else
		CTradeConst::OffsetToStr(oCoverT, wstr, 10);
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Offset), wstr);
}
//买卖框焦点
void COFInputDlg::OnEnSetfocusEditDirect()
{
	wchar_t str[101] = { 0 };
	wchar_t wdirect1[10] = { 0 };
	wchar_t wdirect2[10] = { 0 };
	CTradeConst::DirectToStr(dBuy, wdirect1,10);
	CTradeConst::DirectToStr(dSell, wdirect2,10);
	swprintf_s(str, _T("%s:%c %s:%c"), wdirect1, m_KeyBoardSet.ucBuy,
		wdirect2, m_KeyBoardSet.ucSell);
	ShowTip(str);
}
//开平焦点
void COFInputDlg::OnEnSetfocusEditOffset()
{
	wchar_t str[101] = { 0 };
	wchar_t woffset1[10] = { 0 };
	wchar_t woffset2[10] = { 0 };
	wchar_t woffset3[10] = { 0 };
	CTradeConst::OffsetToStr(oOpen, woffset1, 10);	
	CTradeConst::OffsetToStr(oCoverT, woffset2, 10);
	CTradeConst::OffsetToStr(oCover, woffset3, 10);
	swprintf_s(str, _T("%s:%c %s:%c %s:%c"), woffset1, m_KeyBoardSet.ucOpen,
		woffset2, m_KeyBoardSet.ucCoverT,
		woffset3, m_KeyBoardSet.ucCover);
	ShowTip(str);
}
//设置组框条件
void COFInputDlg::OnCbnSetfocusComboTirgMode()
{
	wchar_t str[101] = { 0 };
	swprintf_s(str, LoadResString(IDS_STRING_TirgModeKeyTip).c_str(), KeyTrigLast, KeyTrigBuy, KeyTrigSell);
	ShowTip(str);
}
//委托价格焦点
void COFInputDlg::OnEnSetfocusEditOrderPrice()
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	GetContractStr(str);

	//获取涨停价 跌停价
	SQuoteSnapShot temp;
	ShowTip(_T(""));
	if (GetQuotInfo(str.c_str(), temp))							//有效合约
	{
		SContractNoType contract_no;
		CConvert::StrToQContractNoType(str.c_str(), contract_no);

		string strZTJ, strDTJ;
		TContractKey tempContractKey;
		CConvert::SContractNoTypeToTradeContract(m_CurLoginNo.c_str(), contract_no, tempContractKey);
		if (temp.Data[S_FID_LIMITDOWNPRICE].FidAttr != S_FIDATTR_NONE && temp.Data[S_FID_LIMITUPPRICE].FidAttr != S_FIDATTR_NONE)
		{
			CGeneralGlobal::DecimalsToStr(tempContractKey, temp.Data[S_FID_LIMITDOWNPRICE].Price, strDTJ);
			CGeneralGlobal::DecimalsToStr(tempContractKey, temp.Data[S_FID_LIMITUPPRICE].Price, strZTJ);
			wchar_t cc[51] = { 0 };
			swprintf_s(cc, LoadResString(IDS_STRING_UpDownLimit).c_str(), LoadRC::ansi_to_unicode(strDTJ).c_str(), LoadRC::ansi_to_unicode(strZTJ).c_str());
			wstring wstr = TEXT("");
			wstr = wstr + cc + TEXT("  ") + LoadResString(IDS_Market_Zero);
			ShowTip(wstr);
		}
	}
	else
		ShowTip(LoadResString(IDS_Market_Zero));

	//委托价格整数部分得到焦点时 如果上一次得到焦点的窗口是分子部分 那就不需要填价格
	if (GetDlgItem(m_hDlg, IDC_EDITORDERPRICE_FenZi) != m_OrderPriceEdit.GetLastFocusWnd())
		SetPriceSel();

	FormatStaticDepositDisplay();
}
//委托数量焦点
void COFInputDlg::OnEnSetfocusEditOrderQty()
{
	ShowTip(_T(""));
	string strText;
	strText = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty));
	Edit_SetSel(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), 0, (int)strText.length());
	FormatStaticNumDisplay();
}
//触发价格焦点
void COFInputDlg::OnEnSetfocusEditTrigPrice()
{
	ShowTip(_T(""));
	//设置当前合约代码的价格
	if (IsNeedFillTrigger() && !GetValidFillTrigger())//外盘 止损 限价止损有效
	{
		string str;
		str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
		GetContractStr(str);
		string strDirect;
		strDirect = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Direct));
		bool bValidPrice = false;
		SQuoteSnapShot QuoteData;
		memset(&QuoteData, 0, sizeof(SQuoteSnapShot));
		double Price = 0;
		if (true == GetQuotInfo(str.c_str(), QuoteData, m_FillOrderAssist.uiTrigPriceMode, m_FillOrderAssist.uiTrigOverPoint, CTradeConst::StrToDirect(LoadRC::ansi_to_unicode(strDirect).c_str()), Price, Price)
			&& Price != 0)
		{
			SetPriceToDlg(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, IDC_Sta_TriggerPrice_FenMu, Price);
			bValidPrice = true;
		}
		//若无效价格 则填充委托价格
		if (!bValidPrice || strDirect.empty())
		{
			double Price = 0;
			if (GetCtrlPrice(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, Price, false))
				SetPriceToDlg(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, IDC_Sta_TriggerPrice_FenMu, Price);
		}
		SetValidFillTrigger(true);
	}
}
//触发条件
void COFInputDlg::OnEnSetfocusEditTrigType()
{
	wchar_t str[101] = { 0 };
	swprintf_s(str, L">=:%c  <=:%c", KeyGreater, KeyLesser);
	ShowTip(str);
}
//数量倍率焦点
void COFInputDlg::OnEnSetfocusEditQtyMulti()
{
	ShowTip(_T(""));
}
//账户组框选择
void COFInputDlg::OnCbnSelChangeUserNo()
{
	wchar_t strSelectItem[51] = { 0 };
	int index = ComboBox_GetCurSel(GetDlgItem(m_hDlg, IDC_ComUserNo));
	if (CB_ERR == index)	return;
	ComboBox_GetLBText(GetDlgItem(m_hDlg, IDC_ComUserNo), index, strSelectItem);

	if (!IsCheck(IDC_Check_BatchOrder))
	{
		//多个资金账户和单个资金账户
		if (m_vecUserNo.size() > 1)
		{
			if (index!=0)
			{
				if (0 != strcmp(LoadRC::unicode_to_ansi(strSelectItem).c_str(), m_vecUserNo[index-1][0].c_str()))
					MessageBox(m_hDlg, _T("ERROR"), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				//当前登录账户
				SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo),LoadRC::ansi_to_unicode(m_vecUserNo[index-1][1]).c_str());
				UpdateLoginSign(m_vecUserNo[index-1][0].c_str(), m_vecUserNo[index-1][2].c_str());
				SetWindowTextW(GetDlgItem(m_hDlg, IDC_ComUserNo), LoadRC::ansi_to_unicode(m_vecUserNo[index - 1][0]).c_str());
			}
			else
			{
				SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), L"");
				UpdateLoginSign("", "");
				SetWindowTextW(GetDlgItem(m_hDlg, IDC_ComUserNo), L"");
			}
		}
	}
	else//批量
	{
		map<string, BatchGroupInfo>::iterator iter = m_batchGroupVec.find(LoadRC::unicode_to_ansi(strSelectItem).c_str());
		if (iter != m_batchGroupVec.end())
		{
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), LoadRC::ansi_to_unicode(iter->second.BatchGroupName).c_str());
			if (GetSignByUserNo(iter->second.BaseUser, index))
				UpdateLoginSign(m_vecUserNo[index][0].c_str(), m_vecUserNo[index][2].c_str());	
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_ComUserNo), LoadRC::ansi_to_unicode(iter->first).c_str());
		}	
	}
	UpdatePositionInf();
	CheckCodeInput();
	FormatStaticNumDisplay();
	FormatStaticDepositDisplay();

	if (g_PlugLoad.GetMainFrm())
	{
		if (m_vecUserNo.size()>1)
		{
			if (index==0)
				g_PlugLoad.GetMainFrm()->Linkage(m_hDlg,"KeyboardOrder", W_USER_CHANGED, "");
			else
			{
				for (vector<CStringsVec>::size_type i = 0; i < m_vecUserNo.size(); i++)
				{
					if (0 == strcmp(LoadRC::unicode_to_ansi(strSelectItem).c_str(), m_vecUserNo[i][0].c_str()))
					{
						char content[50] = { 0 };
						sprintf_s(content, "user=%s", m_vecUserNo[i][0].c_str());
						g_PlugLoad.GetMainFrm()->Linkage(m_hDlg, "KeyboardOrder", W_USER_CHANGED, content);
						//"KeyboardOrder","UserNo_Update","user=UserNo"
					}
				}
			}		
		}
	}
}
//定单类型组框选择
void COFInputDlg::OnCbnSelchangeComboOrdertype()
{
	char preType = GetOrderType();
	int sel = m_OrderTypeCmb.GetCurSel();
	_ASSERT(sel != -1);
	wstring str;
	m_OrderTypeCmb.GetLBText(sel, str);
	char curType = CTradeConst::StrToOrderType(str.c_str());
	if (preType != curType)
		Layout();
}
//设置定单类型组框
void COFInputDlg::OnCbnSetfocusComboOrderType()
{
	wchar_t str[101] = { 0 };
	if (CConvert::ReadVersionType() == 1 || CConvert::ReadVersionType() == 2)
		swprintf_s(str, LoadResString(IDS_STR_OrderType_Show2).c_str(), KeyLimit, KeyMarket, KeyLimitStop, KeyStop,KeyIceberg,KeyGhost);
	else
		swprintf_s(str, LoadResString(IDS_STR_OrderType_Show).c_str(), KeyLimit, KeyMarket, KeyLimitStop, KeyStop);
	ShowTip(str);
}
//有效类型组框选择
void COFInputDlg::OnCbnSelchangeComboValidtype()
{
	char preType = GetValidType();
	int sel = m_ValidTypeCmb.GetCurSel();
	_ASSERT(sel != -1);
	wstring str;
	str = m_ValidTypeCmb.GetName(sel);
	char curType = CTradeConst::StrToValidType(str.c_str());
	if ((preType == vtGTD &&preType != curType) || (preType != vtGTD &&curType == vtGTD))
		Layout();
}
//合约代码内容改变
void COFInputDlg::OnEnChangeEditCode()
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" ") + 1);
	}
	if (str.length() > m_CodeLengthStr.length() && !m_CurLoginNo.empty())
	{
		m_CodeLengthStr = str;
		TContractKey key;
		memset(&key, 0, sizeof(TContractKey));
		memcpy_s(key.Sign, sizeof(key.Sign), m_CurLoginNo.c_str(), sizeof(key.Sign));
		if (CConvert::TContractNoToTContractKey(m_CodeLengthStr.c_str(), key) && CConvert::IsValidTradeCommodity(key))
		{
			SIZE sz;
			SetEditCtrlParam(IAF_COMMODITY, LoadRC::ansi_to_unicode(m_CodeLengthStr), IDC_EDIT_Code, sz);		//合约代码的宽度和高度
			Layout();
		}
	}
	GetContractStr(str);

	UpdateFillTriggerValid(str);

	//港交所
	EnableT1(str.c_str());

	UpdateTradeLayOut();
	FormatStaticCodeDisplay();
}
void COFInputDlg::OnEnKillFocusEditCode()
{
	if (m_KeyBoardSet.mapContractHotkey.size() == 0)return;
	string strA;
	strA = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	wstring str = LoadRC::ansi_to_unicode(strA);
	if (!str.empty())
	{
		std::map<std::wstring, std::wstring>::iterator iter = m_KeyBoardSet.mapContractHotkey.find(str);
		if (iter != m_KeyBoardSet.mapContractHotkey.end())
		{
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Code), iter->second.c_str());
		}
	}
}
//更新交易数据并重新布局
bool COFInputDlg::UpdateTradeLayOut()
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	GetContractStr(str);

	//价格动态变化
	char preCoveMode = m_CommodityInf.cCoverMode;
	bool bFractionPrice = m_CommodityInf.bIfFractionPrice;
	if (RequestCommodity(str.c_str()))
		UpdateCommodityTick(str.c_str());
	else
	{
		TContractKey	TradeContract;	//交易合约
		memset(&TradeContract, 0, sizeof(TradeContract));
		SetLoginSign(TradeContract.Sign);
		CConvert::TContractNoToTContractKey(str.c_str(), TradeContract);
		char contractID[101];
		CConvert::StrToQContractNoType(str.c_str(), contractID);
		if (CConvert::IsValidQuotContract(contractID))//合法的交易合约
			CConvert::GetCommodityInf(TradeContract, m_CommodityInf);
	}
	if (m_CommodityInf.cCoverMode != preCoveMode || bFractionPrice != m_CommodityInf.bIfFractionPrice)//两者不等 重新布局
		Layout();

	//更新持仓
	UpdatePositionInf();
	return true;
}
//买卖方向内容改变
void COFInputDlg::OnEnChangeEditDirect()
{
	if (IsCheck(IDC_CHECK_TJ))
		UpdateTJState();
}
//委托价格spin
void COFInputDlg::OnDeltaposSpinOrderPrice(int nDelta)
{
	SpinUpOrDown(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, nDelta> 0 ? TRUE : FALSE);
}
//委托数量spin
void COFInputDlg::OnDeltaposSpinOrderQty(int nDelta)
{
	DoWndWheel(IDC_EDIT_OrderQty, nDelta > 0 ? TRUE : FALSE);
}
//数量倍率spin
void COFInputDlg::OnDeltaposSpinQtyMulti(int nDelta)
{
	DoWndWheel(IDC_EDIT_QtyMulti, nDelta> 0 ? TRUE : FALSE);
}
//触发价格spin
void COFInputDlg::OnDeltaposSpinTrigPrice(int nDelta)
{
	SpinUpOrDown(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, nDelta > 0 ? TRUE : FALSE);
}
//下单次数spin
void  COFInputDlg::OnDeltaposSpinQtycount(int nDelta)
{
	DoWndWheel(IDC_EDIT_QtyCount, nDelta > 0 ? TRUE : FALSE);
}
//数量增加spin
void  COFInputDlg::OnDeltaposSpinIncreqty(int nDelta)
{
	DoWndWheel(IDC_EDIT_IncreQty, nDelta > 0 ? TRUE : FALSE);
}
//价格增量spin
void  COFInputDlg::OnDeltaposSpinIncreprice(int nDelta)
{
	SpinUpOrDown(IDC_EDIT_IncrePrice, nDelta > 0 ? TRUE : FALSE);
}
//最大量spin
void  COFInputDlg::OnDeltaposSpinMaxqty(int nDelta)
{
	DoWndWheel(IDC_EDIT_MaxQty, nDelta > 0 ? TRUE : FALSE);
}
//最小量spin
void  COFInputDlg::OnDeltaposSpinMinqty(int nDelta)
{
	DoWndWheel(IDC_EDIT_MinQty, nDelta > 0 ? TRUE : FALSE);
}
//账户edit更改
void COFInputDlg::OnCbnEditchangeComuserno()
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_ComUserNo));

	int index = -1;
	//这句位置不能放到此函数开头 因为NotifyUserNoChange会回调到此函数 把m_bShowFuzzyQuery置为true 导致显示出下来选合约
	if (!IsCheck(IDC_Check_BatchOrder))
	{
		if (GetIndexUserNo(m_vecUserNo,str, index))
		{
			//当前登录账户
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), LoadRC::ansi_to_unicode(m_vecUserNo[index][1]).c_str());
			UpdateLoginSign(m_vecUserNo[index][0].c_str(), m_vecUserNo[index][2].c_str());
		}
		else
		{
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), L"");
			UpdateLoginSign("", "");
		}		
	}
	else//批量
	{
		map<string, BatchGroupInfo>::iterator iter = m_batchGroupVec.find(str);
		if (iter != m_batchGroupVec.end())
		{
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), LoadRC::ansi_to_unicode(iter->second.BatchGroupName).c_str());
			if (GetSignByUserNo(iter->second.BaseUser, index))
				UpdateLoginSign(m_vecUserNo[index][0].c_str(), m_vecUserNo[index][2].c_str());
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_ComUserNo), LoadRC::ansi_to_unicode(iter->first).c_str());
		}
		else
		{
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), _T(""));
			UpdateLoginSign("", "");
		}
	}
	UpdatePositionInf();
	CheckCodeInput();

	if (g_PlugLoad.GetMainFrm())
	{
		if (m_vecUserNo.size() > 1)
		{
			if (str=="")
				g_PlugLoad.GetMainFrm()->Linkage(m_hDlg, "KeyboardOrder", W_USER_CHANGED, "");
			else
			{
				for (vector<CStringsVec>::size_type i = 0; i < m_vecUserNo.size(); i++)
				{
					if (str==m_vecUserNo[i][0])
					{
						char content[50] = { 0 };
						sprintf_s(content, "user=%s", str.c_str());
						g_PlugLoad.GetMainFrm()->Linkage(m_hDlg, "KeyboardOrder", W_USER_CHANGED, content);
						//"KeyboardOrder","UserNo_Update","user=UserNo"
					}
				}
			}
		}
	}
}
//倍率更新
void COFInputDlg::OnEnUpdateEditQtyMulti()
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti));
	//关于这个价格是否合理的判断都在这个函数实现算了
	//使用正则表达式判断这个串是否正在录入数值型
	BOOL bChange = FALSE;
	if (!CGeneralGlobal::MatchPattern("^(\\d{1})?(\\.)?(\\d{1})?$", str.c_str()))
	{
		str = m_LastBLStr;
		bChange = TRUE;
	}
	//判断值超出范围没
	if (!bChange)
	{
		double value = atof(str.c_str());
		if (value > 100)
		{
			char tt[16] = { 0 };
			value = 100;
			bChange = TRUE;
			sprintf_s(tt,"%.1f", value);
			str = tt;
		}
		if (value <= 0)
		{
			char tt[16] = { 0 };
			value = m_fastOrderInfo.dChangePoint > 0 ? m_fastOrderInfo.dChangePoint : 1;
			bChange = TRUE;
			sprintf_s(tt, "%.1f", value);
			str = tt;
		}
	}
	if (bChange)
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti),LoadRC::ansi_to_unicode(str).c_str());
	//记录现在的值
	m_LastBLStr = str;
}
//数量更新
void COFInputDlg::OnEnUpdateEditOrderQty()
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty));
	//使用正则表达式判断这个串是否正在录入数值型
	BOOL bChange = FALSE;
	if (!CGeneralGlobal::MatchPattern("^([1-9]\\d*|0)?$",str.c_str()))
	{
		str = m_LastVolStr;
		bChange = TRUE;
	}
	//判断值超出范围没
	if (bChange)
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), LoadRC::ansi_to_unicode(str).c_str());
	//记录现在的值
	m_LastVolStr = str;
}
//价格增量更新
void COFInputDlg::OnEnUpdateEditIncreprice()
{
	//使用正则表达式判断这个串是否正在录入数值型
	BOOL bChange = FALSE, bIsValidNum = false;
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice));
	bIsValidNum = CGeneralGlobal::IsValidNumber(str.c_str(), false, true);
	if (!bIsValidNum)
	{
		str = "0";
		bChange = TRUE;
	}
	//判断值超出范围没
	if (bChange)
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice), LoadRC::ansi_to_unicode(str).c_str());
}
//下单次数增量
void COFInputDlg::OnEnUpdateEditIncreqty()
{
	//使用正则表达式判断这个串是否正在录入数值型
	BOOL bChange = FALSE, bIsValidNum = false;
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_IncreQty));
	bIsValidNum = CGeneralGlobal::IsValidNumber(str.c_str(), false, true);
	if (!bIsValidNum)
	{
		str = "0";
		bChange = TRUE;
	}
	//判断值超出范围没
	if (bChange)
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_IncreQty), LoadRC::ansi_to_unicode(str).c_str());
}
//选合约按钮
void COFInputDlg::OnBnClickedBtnSelcode()
{
	RECT rectvalid;
	GetWindowRect(GetDlgItem(m_hDlg, IDC_EDIT_Code), &rectvalid);
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	if (rectvalid.bottom + 400 >= rect.bottom)
		rectvalid.bottom = rectvalid.top - 400;
	if (!IsWindow(m_cCodeWin.GetHwnd()))
	{
		m_cCodeWin.CreatePopWin(this, TEXT("m_cCodeWin"), m_hDlg, WS_CLIPCHILDREN, 0, rectvalid.left, rectvalid.bottom);
		m_cCodeWin.UpdateData(m_CommodityInf.Sign);
		::ShowWindow(m_cCodeWin.GetHwnd(), SW_SHOWNORMAL);
	}
	else
	{
		m_cCodeWin.UpdateData(m_CommodityInf.Sign);
		SetWindowPos(m_cCodeWin.GetHwnd(), NULL, rectvalid.left, rectvalid.bottom, 0, 0, SWP_NOSIZE);
		::ShowWindow(m_cCodeWin.GetHwnd(), SW_SHOWNORMAL);
	}
}
//跟盘价格
void COFInputDlg::OnBnClickedCheckAutoprice()
{
	bool bCheck = false;
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice), L"");
	if (m_SwitchFollow.GetSwitchLevel() == 0)
	{
		bCheck = true;
		m_SwitchFollow.SetSwitchLevel(1);
		PostMessage(m_hDlg, WM_QUOTCHG, 0, 0);
	}	
	else
	{
		bCheck = false;	
		m_SwitchFollow.SetSwitchLevel(0);
	}		
	//价格框
	EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice), (BOOL)!bCheck);
	EnableWindow(GetDlgItem(m_hDlg, IDC_SPIN_OrderPrice), (BOOL)!bCheck);
	if (IsWindowVisible(GetDlgItem(m_hDlg, IDC_EDITORDERPRICE_FenZi)))
		EnableWindow(GetDlgItem(m_hDlg, IDC_EDITORDERPRICE_FenZi), (BOOL)!bCheck);
	
	//设置buysell按钮
	SetBuySellBtnStyle(bCheck);	
}

void COFInputDlg::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int nID = GetWindowLong(GetFocus(), GWL_ID);
	switch (nID)
	{	
	case IDC_EDIT_OrderPrice:
	case IDC_EDITORDERPRICE_FenZi:
		OnDeltaposSpinOrderPrice(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case  IDC_EDIT_OrderQty:
		OnDeltaposSpinOrderQty(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case IDC_EDIT_QtyMulti:
		OnDeltaposSpinQtyMulti(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case IDC_EDIT_TrigPrice:
	case IDC_EDITTriggerPRICE_FenZi:
		OnDeltaposSpinTrigPrice(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case IDC_EDIT_QtyCount:
		OnDeltaposSpinQtycount(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case IDC_EDIT_IncreQty:
		OnDeltaposSpinIncreqty(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case IDC_EDIT_IncrePrice:
		OnDeltaposSpinIncreprice(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case IDC_EDIT_MaxQty:
		OnDeltaposSpinMaxqty(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case IDC_EDIT_MinQty:
		OnDeltaposSpinMinqty(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case IDC_EDIT_Direct:
		DoWndWheel(IDC_EDIT_Direct, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? TRUE : FALSE);
		break;
	case IDC_EDIT_Offset:
		DoWndWheel(IDC_EDIT_Offset, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? TRUE : FALSE);
		break;
	case IDC_EDIT_TrigCondi:
		DoWndWheel(IDC_EDIT_TrigCondi, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? TRUE : FALSE);
		break;
	}
	return;
}

void COFInputDlg::OnDealHotKey(WPARAM wParam, LPARAM lParam)
{
	switch (char(wParam))
	{
		case Vk_Condition://条件
			if (IsWindowVisible(GetDlgItem(m_hDlg, IDC_CHECK_TJ)))
				OnBnClickedCheckTJ();
			break;
		case Vk_CloseOpen://平开
			if (IsWindowVisible(GetDlgItem(m_hDlg, IDC_CHECK_PK)))
				OnBnClickedCheckPK();
			break;
		case Vk_Fast://快速
			if (IsWindowVisible(GetDlgItem(m_hDlg, IDC_CHECK_KS)))
				OnBnClickedCheckKS();
			break;
		case Vk_BatchTimes://批次
			if (IsWindowVisible(GetDlgItem(m_hDlg, IDC_Check_BatchCount)))
				OnBnClickedCheckBatchCount();
			break;
		case Vk_BatchQty://批量
			if (IsWindowVisible(GetDlgItem(m_hDlg, IDC_Check_BatchOrder)))
				OnBnClickedCheckBatchOrder();
			break;
		case Vk_OK://确定
			if (IsWindowVisible(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder)))
				OnBnClickedButtonDoOrder();
			break;
		case Vk_PreOrder://埋单
			if (IsWindowVisible(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder)))
				OnBnClickedButtonPayOrder();
			break;
		default:
			break;
	}
}

void COFInputDlg::OnBatchOrderInfoCHG()
{
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), L"");
	SetUser();
}
//初始化对话框
BOOL COFInputDlg::OnInitDialog()
{
	//判断内外盘信息
	LoadVersionType();

	//资金账户匹配方式
	m_UserNO.SetReverFind(m_FillOrderAssist.bBackMatchUserNo);

	//控件重绘 子类化 
	InitCtrl();

	//控件静态文本居中
	IniStaticTextVer();

	//修改控件遮挡重绘属性
	ModifyCtrlStyle();

	//设置下单按钮原始尺寸
	SetOrginBtnWidth();

	//初始化提示信息空
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_Tip), L"");
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_Deposit), L"");
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_CanOpenCover), L"");

	//创建画刷
	m_ctlBrush = CreateSolidBrush(g_ColorRefData.GetColorWhite());
	m_ctlBrushKS = CreateSolidBrush(g_ColorRefData.GetColorBackground());
	m_BkBrushDlg = CreateSolidBrush(g_ColorRefData.GetColorBackground());

	//对编辑框的内容长度约束一下
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_Code), EM_SETLIMITTEXT, 27, 0);
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_Offset), EM_SETLIMITTEXT, 4, 0);
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_Direct), EM_SETLIMITTEXT, 4, 0);
	

	//::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice), EM_SETLIMITTEXT, 6, 0);		
	//::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_TriggerPrice), EM_SETLIMITTEXT, 6, 0);
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), EM_SETLIMITTEXT, 2, 0);

	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), EM_SETLIMITTEXT, 6, 0);
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_MaxQty), EM_SETLIMITTEXT, 6, 0);
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_MinQty), EM_SETLIMITTEXT, 6, 0);
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti), EM_SETLIMITTEXT, 4, 0);
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_QtyCount), EM_SETLIMITTEXT, 4, 0);
	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_IncreQty), EM_SETLIMITTEXT, 4, 0);

	::SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice), EM_SETLIMITTEXT, 4, 0);

	//绑定编辑框 旋转按钮
	SetNumSpin(IDC_EDIT_OrderQty, IDC_SPIN_OrderQty, false);
	SetNumSpin(IDC_EDIT_QtyMulti, IDC_SPIN_QtyMulti, false);
	SetNumSpin(IDC_EDIT_QtyCount, IDC_SPIN_QtyCount, false);
	SetNumSpin(IDC_EDIT_IncreQty, IDC_SPIN_IncreQty, false);
	SetNumSpin(IDC_EDIT_MaxQty, IDC_SPIN_MaxQty, false);
	SetNumSpin(IDC_EDIT_MinQty, IDC_SPIN_MinQty, false);

	//更新持仓文本信息
	m_BuyPositionStatic.UpdateText(0, LoadResString(IDS_STR_LongPosition), m_OrderFont.lgPositionFont, g_ColorRefData.GetColorBackground(), m_OrderFont.rgbBuy);
	m_SellPositionStatic.UpdateText(0, LoadResString(IDS_STR_ShortPosition), m_OrderFont.lgPositionFont, g_ColorRefData.GetColorBackground(), m_OrderFont.rgbSell);

	//设置控件字体
	SetCtrlFont();

	//设置用户信息
	SetUser();

	//简化合约代码(内盘) 线程
	m_thread.SetVersion(VersionType());
	m_thread.SetSign(GetLoginSign());
	m_thread.Start();
	//重新布局
	Layout();

	//设置定单类型
	SetOrderTypeCmbAtrribute();

	//设置有效类型
	SetValidTypeCmbAtrribute();

	//设置条件单触发类型
	wchar_t wMode[20] = { 0 };
	CTradeConst::TriggerModeToStr(tmLatest,wMode,20);
	ComboBox_AddString(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), wMode);
	CTradeConst::TriggerModeToStr(tmBid, wMode, 20);
	ComboBox_AddString(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), wMode);
	CTradeConst::TriggerModeToStr(tmAsk, wMode, 20);
	ComboBox_AddString(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), wMode);

	ComboBox_SetCurSel(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), 0);

	//按钮显示内容更新 买入卖出
	m_BuyBtn.SetBitmapID(LoadResString(IDS_STR_Buy).c_str());
	m_SellBtn.SetBitmapID(LoadResString(IDS_STR_SELL).c_str());

	//更新窗口属性
	UpdatePropertyControl();
	//持仓显示更新
	UpdatePositionInf();

	return TRUE;  
}
//设置spin
void COFInputDlg::SetNumSpin(int ID, int IDSpin, bool bCanNat)
{
	if (bCanNat)
		SendDlgItemMessage(m_hDlg, IDSpin, UDM_SETRANGE32, 0, MAKELPARAM(10000000, -10000000));
	else
		SendDlgItemMessage(m_hDlg, IDSpin, UDM_SETRANGE32, 0, MAKELPARAM(10000000, 0));
	::SendMessage(GetDlgItem(m_hDlg, IDSpin), UDM_SETPOS, 0, MAKELPARAM(0, 0));
}
//初始化静态空间文本垂直居中
void COFInputDlg::IniStaticTextVer()
{
	ModifyStyle(GetDlgItem(m_hDlg, IDC_Sta_OrderPrice_ChuHao), 0, SS_CENTERIMAGE);
	ModifyStyle(GetDlgItem(m_hDlg, IDC_Sta_OrderPrice_FenMu), 0, SS_CENTERIMAGE);
	ModifyStyle(GetDlgItem(m_hDlg, IDC_Sta_TriggerPrice_ChuHao), 0, SS_CENTERIMAGE);
	ModifyStyle(GetDlgItem(m_hDlg, IDC_Sta_InvrePrice_ChuHao), 0, SS_CENTERIMAGE);
	ModifyStyle(GetDlgItem(m_hDlg, IDC_Sta_IncrePrice_FenMu), 0, SS_CENTERIMAGE);
	ModifyStyle(GetDlgItem(m_hDlg, IDC_Sta_TriggerPrice_FenMu), 0, SS_CENTERIMAGE);
	ModifyStyle(GetDlgItem(m_hDlg, IDC_Sta_OrderPrice_Add), 0, SS_CENTERIMAGE);
	ModifyStyle(GetDlgItem(m_hDlg, IDC_Sta_TriggerPrice_Add), 0, SS_CENTERIMAGE);
}
//修改控件属性
void COFInputDlg::ModifyStyle(HWND hCtrl, DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwStyle = ::GetWindowLong(hCtrl, GWL_STYLE);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if (dwStyle == dwNewStyle)
		return;
	::SetWindowLong(hCtrl, GWL_STYLE, dwNewStyle);
}
//修改控件属性
void COFInputDlg::ModifyCtrlStyle()
{
	HWND hWnd = GetTopWindow(m_hDlg);
	while (hWnd != NULL)
	{
		ModifyStyle(hWnd, NULL, WS_CLIPSIBLINGS);
		hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
	}
}
//根据字体计算控件大小
bool COFInputDlg::UseFontSetCtrlSize(eCtrlType eType, HFONT &font, SIZE & sz, wstring& str)
{
	HDC hDC = GetDC(m_hDlg);
	SelectObject(hDC, font);
	::GetTextExtentPoint32(hDC, str.c_str(), (int)str.length(), &sz);
	DeleteDC(hDC);
	switch (eType)
	{
		case EDIT_TYPE:
		case CHECK_TYPE:
		case BTN_TYPE:
		case STATIC1_TYPE:
		case STATIC2_TYPE://针对状态提示
			return true;
		break;
		case COMBO_TYPE://根据组框的字体和所有交易账户来计算组框的大小
		{
			int iWidth = -1;
			int iIndex = -1;
			if (str!=TEXT(" "))//非空说明不是账户组框
				return true;
			TDataContainer tempDataCon;
			g_pTradeData->GetAllUser(tempDataCon);
			string strUser;
			if (tempDataCon.size() > 1)//多客户
			{
				for (uint i = 0; i < tempDataCon.size(); i++)   //对每一个客户处理
				{
					if (m_hDlg)   //
					{
						strUser = (((TUserInfo*)(tempDataCon.at(i)))->UserNo);
						int iLength = strUser.length();
						if (iLength > iWidth)
						{
							iIndex = i;
							iWidth = iLength;
						}
					}
				}
			}
			else if (1 == tempDataCon.size())//单客户
			{
				if (m_hDlg)
				{
					strUser = (((TUserInfo*)(tempDataCon.at(0)))->UserNo);
					iWidth = strUser.length();
				}
			}
			if (-1 != iWidth)
			{
				HDC hDC = GetDC(m_hDlg);
				SelectObject(hDC, font);
				string tempstr;
				if (-1 != iIndex)//说明多账户
					tempstr = ((TUserInfo*)(tempDataCon.at(iIndex)))->UserNo;
				else
				{
					if (1 == tempDataCon.size())//单客户
						tempstr = ((TUserInfo*)(tempDataCon.at(0)))->UserNo;
				}
				::GetTextExtentPoint32(hDC, LoadRC::ansi_to_unicode(tempstr).c_str(), (int)tempstr.length(), &sz);
				DeleteDC(hDC);
			}
			return true;
		}
		break;
		default:
			break;
	}
	return false;
}
//设置编辑框参数
bool COFInputDlg::SetEditCtrlParam(FutureInputArea eInputArea, wstring str, int iEditID, SIZE &sz, bool bExtraWidth)
{
	if (UseFontSetCtrlSize(EDIT_TYPE,/*m_fontCombo*/m_fontEdit, sz, str))//合约代码的宽度和高度
	{
		RECT rectCombo;
		GetWindowRect(GetDlgItem(m_hDlg, IDC_ComUserNo), &rectCombo);

		RECT rectEdit;
		GetWindowRect(GetDlgItem(m_hDlg, iEditID), &rectEdit);
		ScreenToClient(rectEdit);
		if (true == bExtraWidth)
			rectEdit.right = rectEdit.left + sz.cx + EDITBORDERWIDTH;
		else
			rectEdit.right = rectEdit.left + sz.cx;
		rectEdit.bottom = rectEdit.top + sz.cy + EDITBORDERHEIGHT;

		for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
		{
			if (eInputArea == m_InputCfgArray[i].id)
			{
				m_InputCfgArray[i].width = rectEdit.right - rectEdit.left;
				m_InputCfgArray[i].height = rectEdit.bottom - rectEdit.top;
			}
		}
		return true;
	}
	return false;
}
//设置组框框参数
bool COFInputDlg::SetComboCtrlParam(FutureInputArea eInputArea, wstring str, int iComboID, SIZE &sz)
{
	if (UseFontSetCtrlSize(COMBO_TYPE, m_fontCombo, sz, str))
	{
		COMBOBOXINFO coboInfo;
		memset(&coboInfo, 0, sizeof(coboInfo));
		coboInfo.cbSize = sizeof(COMBOBOXINFO);
		::GetComboBoxInfo(GetDlgItem(m_hDlg, iComboID), &coboInfo);
		int iBtnWidth = coboInfo.rcButton.right - coboInfo.rcButton.left + 2;
		RECT rectCombo;
		GetWindowRect(GetDlgItem(m_hDlg, iComboID), &rectCombo);
		ScreenToClient(rectCombo);
		rectCombo.right = rectCombo.left + sz.cx + iBtnWidth + 16;
		rectCombo.bottom = rectCombo.top + sz.cy/*+iBtnWidth*/;
		MoveWindow(GetDlgItem(m_hDlg, iComboID), rectCombo.left, rectCombo.top, rectCombo.right - rectCombo.left, rectCombo.bottom - rectCombo.top, TRUE);

		for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
		{
			if (eInputArea == m_InputCfgArray[i].id)
			{
				m_InputCfgArray[i].width = rectCombo.right - rectCombo.left;
				m_InputCfgArray[i].height = rectCombo.bottom - rectCombo.top;
				break;
			}
		}
		return true;
	}
	return false;
}
//设置静态框参数
bool COFInputDlg::SetStaticCtrlParam(HFONT &font, wstring str, int iStaticID, SIZE &sz, int *pDefaultMaxWidht, bool bExtraWidth)
{
	if (UseFontSetCtrlSize(STATIC1_TYPE, font, sz, str))//
	{
		RECT rectEdit;
		GetWindowRect(GetDlgItem(m_hDlg, iStaticID), &rectEdit);
		ScreenToClient(rectEdit);
		if (bExtraWidth)
			rectEdit.right = rectEdit.left + sz.cx + EDITBORDERWIDTH;
		else
			rectEdit.right = rectEdit.left + sz.cx + 0;
		rectEdit.bottom = rectEdit.top + sz.cy + 2/*+EDITBORDERHEIGHT*/;
		MoveWindow(GetDlgItem(m_hDlg, iStaticID), rectEdit.left, rectEdit.top, rectEdit.right - rectEdit.left, rectEdit.bottom - rectEdit.top, TRUE);
		if (pDefaultMaxWidht)//静态控件+锁定按钮的宽度（按钮的高度与静态文本一样、并且按钮是个正方形）
		{
			*pDefaultMaxWidht = (rectEdit.right - rectEdit.left) + (rectEdit.bottom - rectEdit.top);
			SetWindowPos(GetDlgItem(m_hDlg, IDC_Btn_LockUserNo), NULL, 0, 0, (rectEdit.bottom - rectEdit.top), (rectEdit.bottom - rectEdit.top), SWP_NOMOVE);
			SetWindowPos(GetDlgItem(m_hDlg, IDC_Btn_LockCode), NULL, 0, 0, (rectEdit.bottom - rectEdit.top), (rectEdit.bottom - rectEdit.top), SWP_NOMOVE);
		}
		return true;
	}
	return false;
}
//设置按钮框参数
bool COFInputDlg::SetBtnCtrlParam(wstring str, int iBtnID, SIZE &sz, int *pwidth)
{
	if (UseFontSetCtrlSize(BTN_TYPE, m_fontBtn, sz, str))//按钮的宽度和高度
	{
		RECT rectBtn;
		GetWindowRect(GetDlgItem(m_hDlg, iBtnID), &rectBtn);

		int temp = sz.cx*m_iOrginBtnWidth / m_iOrginBtnFontWidth;
		ScreenToClient(rectBtn);
		rectBtn.bottom = rectBtn.top + sz.cy + BTNBORDERHEIGHT;
		rectBtn.right = rectBtn.left + temp + BTNBORDERWIDTH;
		MoveWindow(GetDlgItem(m_hDlg, iBtnID), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		if (pwidth)	*pwidth = rectBtn.right - rectBtn.left;
		return true;
	}
	return false;
}
//设置check框参数
bool COFInputDlg::SetCheckCtrlParam(FutureInputArea eInputArea, wstring str, int iCheckID, SIZE &sz)
{
	if (UseFontSetCtrlSize(CHECK_TYPE, m_fontCheck, sz, str))//check的宽度和高度
	{
		RECT rectEdit;
		GetWindowRect(GetDlgItem(m_hDlg, iCheckID), &rectEdit);
		ScreenToClient(rectEdit);
		rectEdit.right = rectEdit.left + sz.cx + EDITBORDERWIDTH + 1;
		rectEdit.bottom = rectEdit.top + sz.cy;
		MoveWindow(GetDlgItem(m_hDlg, iCheckID), rectEdit.left, rectEdit.top, rectEdit.right - rectEdit.left, rectEdit.bottom - rectEdit.top, TRUE);
		for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
		{
			if (eInputArea == m_InputCfgArray[i].id)
			{
				m_InputCfgArray[i].width = rectEdit.right - rectEdit.left;
				m_InputCfgArray[i].height = rectEdit.bottom - rectEdit.top;
			}
		}
		return true;
	}
	return false;
}
bool COFInputDlg::SetCheckCtrlParam2(wstring str, int iCheckID, SIZE &sz)
{
	if (UseFontSetCtrlSize(CHECK_TYPE, m_fontCheck, sz, str))//check的宽度和高度
	{
		RECT rectEdit;
		GetWindowRect(GetDlgItem(m_hDlg, iCheckID), &rectEdit);
		ScreenToClient(rectEdit);
		rectEdit.right = rectEdit.left + sz.cx + EDITBORDERWIDTH*2 + 1;
		rectEdit.bottom = rectEdit.top + sz.cy;
		MoveWindow(GetDlgItem(m_hDlg, iCheckID), rectEdit.left, rectEdit.top, rectEdit.right - rectEdit.left, rectEdit.bottom - rectEdit.top, TRUE);
		return true;
	}
	return false;
}
//设置日期控件参数
bool COFInputDlg::SetDateTimeCtrlParam()
{
	HDC hDC = GetDC(m_hDlg);
	SelectObject(hDC, m_fontEdit);
	wstring str(_T("88-88-88 "));
	SIZE sz;
	::GetTextExtentPoint32(hDC, str.c_str(), (int)str.length(), &sz);
	sz.cx += 34;
	DeleteDC(hDC);
	ModifyCtrlWidth(IAF_VALIDTIME, sz.cx);

	//显示格式
	::SendMessage(GetDlgItem(m_hDlg, IDC_DATETIMEPICKER_ValidData), DTM_SETFORMAT, 0, (LPARAM)_T("yy-MM-dd"));
	return true;
}
//修改控件宽度
void COFInputDlg::ModifyCtrlWidth(int id, int iNewWidth)
{
	for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
	{
		C_OrderField &of = m_InputCfgArray.at(i);
		if (id == of.id)
		{
			of.width = iNewWidth;
			break;
		}
	}
}
//设置字体
bool COFInputDlg::SetCtrlFont()
{
	m_UserNO.ResetContent();//这句很重要，要不账户下拉宽度就为0
	DeleteObject(m_fontBtn);
	m_fontBtn = CreateFontIndirect(&m_OrderFont.lgBtnFont);
	DeleteObject(m_fontCombo);
	m_fontCombo = CreateFontIndirect(&m_OrderFont.lgComboFont);
	DeleteObject(m_fontEdit);
	LOGFONT tmpFont;
	memcpy_s(&tmpFont, sizeof(LOGFONT), &m_OrderFont.lgComboFont, sizeof(LOGFONT));
	tmpFont.lfHeight -= 2;
	m_fontEdit = CreateFontIndirect(&tmpFont);
	DeleteObject(m_fontCheck);
	m_fontCheck = CreateFontIndirect(&m_OrderFont.lgCheckFont);
	DeleteObject(m_fontStatic1);
	m_fontStatic1 = CreateFontIndirect(&m_OrderFont.lgStaticFont1);
	DeleteObject(m_fontStatic2);
	m_fontStatic2 = CreateFontIndirect(&m_OrderFont.lgStaticFont2);
	DeleteObject(m_fontPosition);
	m_fontPosition = CreateFontIndirect(&m_OrderFont.lgPositionFont);
	LOGFONT lfCodeName;
	memcpy_s(&lfCodeName, sizeof(LOGFONT), &m_OrderFont.lgStaticFont1, sizeof(LOGFONT));
	lfCodeName.lfHeight = abs(lfCodeName.lfHeight) - 1;

	SendMessage(GetDlgItem(m_hDlg, IDC_ComUserNo), WM_SETFONT, (WPARAM)m_fontCombo, TRUE);

	//这句很重要，要不账户下拉宽度就为0 调用此SendMessage(GetDlgItem(m_hDlg, IDC_ComUserNo), WM_SETFONT, (WPARAM)m_fontCombo, TRUE);这句才生效 否则getwindowrect不准确
	//m_UserNO.AddRow(_T(" "));

	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_Code), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_Direct), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_Offset), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDITORDERPRICE_FenZi), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);	//委托价格分子
	SendMessage(GetDlgItem(m_hDlg, IDC_Sta_OrderPrice_Add), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);		//委托价格+
	SendMessage(GetDlgItem(m_hDlg, IDC_Sta_OrderPrice_ChuHao), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);	//委托价格 除号
	SendMessage(GetDlgItem(m_hDlg, IDC_Sta_OrderPrice_FenMu), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);	//委托价格 分母
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_TrigPrice), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDITTriggerPRICE_FenZi), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);	//触发价格分子
	SendMessage(GetDlgItem(m_hDlg, IDC_Sta_TriggerPrice_Add), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);	//触发价格+
	SendMessage(GetDlgItem(m_hDlg, IDC_Sta_TriggerPrice_ChuHao), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);	//触发价格 除号
	SendMessage(GetDlgItem(m_hDlg, IDC_Sta_TriggerPrice_FenMu), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);	//触发价格 分母
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_QtyCount), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_IncreQty), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_Sta_InvrePrice_ChuHao), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);//委托价格 除号
	SendMessage(GetDlgItem(m_hDlg, IDC_Sta_IncrePrice_FenMu), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);//委托价格 分母
	SendMessage(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_COMBO_OrderType), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_COMBO_ValidType), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_DATETIMEPICKER_ValidData), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_MaxQty), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_EDIT_MinQty), WM_SETFONT, (WPARAM)m_fontEdit, TRUE);

	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_Tip), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);//状态提示
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_Code), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_Direct), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_Offset), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_OrderPrice), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_OrderQty), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_QtyMulti), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_UserNo), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_TrigMode), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_TrigCondi), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_TrigPrice), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_QtyCount), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_IncreQty), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_IncrePrice), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_StaticBuyPos), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_StaticSellPos), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	
	m_StaticCodeName.SetTextFont(m_OrderFont.lgStaticFont1/*m_OrderFont.lgStaticFont2*/);
	m_StaticCanOpenCover.SetTextFont(m_OrderFont.lgStaticFont2);
	m_StaticDeposit.SetTextFont(m_OrderFont.lgStaticFont2, DT_RIGHT);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_CanOpenCover), WM_SETFONT, (WPARAM)m_fontStatic2, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_Deposit), WM_SETFONT, (WPARAM)m_fontStatic2, TRUE);


	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_OrderType), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_ValidType), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_ValidData), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_MaxQty), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_STATIC_MinQty), WM_SETFONT, (WPARAM)m_fontStatic1, TRUE);

	SendMessage(GetDlgItem(m_hDlg, IDC_CHECK_TJ), WM_SETFONT, (WPARAM)m_fontCheck, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_CHECK_PK), WM_SETFONT, (WPARAM)m_fontCheck, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_CHECK_KS), WM_SETFONT, (WPARAM)m_fontCheck, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_Check_BatchCount), WM_SETFONT, (WPARAM)m_fontCheck, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_Check_BatchOrder), WM_SETFONT, (WPARAM)m_fontCheck, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_CHECK_T1), WM_SETFONT, (WPARAM)m_fontCheck, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_CHECK_ForCover), WM_SETFONT, (WPARAM)m_fontCheck, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_CHECK_AutoPrice), WM_SETFONT, (WPARAM)m_fontCheck, TRUE);

	SendMessage(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), WM_SETFONT, (WPARAM)m_fontBtn, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), WM_SETFONT, (WPARAM)m_fontBtn, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), WM_SETFONT, (WPARAM)m_fontBtn, TRUE);
	SendMessage(GetDlgItem(m_hDlg, IDC_Btn_Seq), WM_SETFONT, (WPARAM)m_fontBtn, TRUE);

	//针对编辑框
	SIZE sz;
	m_CodeLengthStr = "EUREX DAX 0803/0806";
	if (0 == VersionType())
		m_CodeLengthStr = " TF 1603";
	SetEditCtrlParam(IAF_COMMODITY, LoadRC::ansi_to_unicode(m_CodeLengthStr), IDC_EDIT_Code, sz);		//合约代码的宽度和高度
	SetEditCtrlParam(IAF_OFFSET, LoadResString(IDS_Offset_CoverT), IDC_EDIT_Offset, sz);				//开平的宽度和高度
	SetEditCtrlParam(IAF_DIRECT, LoadResString(IDS_Direct_Buy), IDC_EDIT_Direct, sz);					//买卖的宽度

	SetEditCtrlParam(IAF_PRICE, _T("88888888"), IDC_EDIT_OrderPrice, sz);								//委托价格	
	//Edit_LimitText(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice),12);
	SetEditCtrlParam(IAF_PRICE_ADD, _T(" + "), IDC_EDITORDERPRICE_FenZi, sz, false);					//委托价格 +			
	SetEditCtrlParam(IAF_PRICE_FENZI, _T("12345"), IDC_Sta_OrderPrice_Add, sz, false);					//委托价格 分子			 
	SetEditCtrlParam(IAF_PRICE_CHUHAO, _T("/"), IDC_Sta_OrderPrice_ChuHao, sz, false);					//委托价格 除号			 
	SetEditCtrlParam(IAF_PRICE_FENMU, _T("12345"), IDC_Sta_OrderPrice_FenMu, sz, false);				//委托价格 分母

	SetEditCtrlParam(IAF_VOL, _T("99999"), IDC_EDIT_OrderQty, sz);										//委托数量
	SetEditCtrlParam(IAF_VOLBL, _T("9999"), IDC_EDIT_QtyMulti, sz);										//数量倍率

	SetEditCtrlParam(IAF_TRIGGERPRICE, _T("88888888"), IDC_EDIT_TrigPrice, sz);							//触发价格	
	//Edit_LimitText(GetDlgItem(m_hDlg, IDC_EDIT_TrigPrice), 12);
	SetEditCtrlParam(IAF_TRIGGERPRICE_ADD, _T(" + "), IDC_EDITTriggerPRICE_FenZi, sz, false);			//触发价格 +			
	SetEditCtrlParam(IAF_TRIGGERPRICE_FENZI, _T("12345"), IDC_Sta_TriggerPrice_Add, sz, false);			//触发价格 分子			 
	SetEditCtrlParam(IAF_TRIGGERPRICE_CHUHAO, _T("/"), IDC_Sta_TriggerPrice_ChuHao, sz, false);			//触发价格 除号			 
	SetEditCtrlParam(IAF_TRIGGERPRICE_FENMU, _T("12345"), IDC_Sta_TriggerPrice_FenMu, sz, false);		//触发价格 分母
	SetEditCtrlParam(IAF_TRIGGERCONDITION, _T(">="), IDC_EDIT_TrigCondi, sz);

	SetEditCtrlParam(IAF_VOLCOUNT, _T("8888"), IDC_EDIT_QtyCount, sz);
	SetEditCtrlParam(IAF_INCREVOL, _T("8888"), IDC_EDIT_IncreQty, sz);
	SetEditCtrlParam(IAF_INCREPRICE, _T("8888"), IDC_EDIT_IncrePrice, sz);
	SetEditCtrlParam(IAF_INCRERPRICE_CHUHAO, _T("/"), IDC_Sta_InvrePrice_ChuHao, sz, false);			// 除号			 
	SetEditCtrlParam(IAF_INCREPRICE_FENMU, _T("12345"), IDC_Sta_IncrePrice_FenMu, sz, false);			// 分母

	SetEditCtrlParam(IAF_MAXQTY, _T("9999"), IDC_EDIT_MaxQty, sz, false);
	SetEditCtrlParam(IAF_MINQTY, _T("9999"), IDC_EDIT_MinQty, sz, false);

	//针对组框 还有条件单的要加
	SetComboCtrlParam(IAF_ACCOUNT, _T(" "), IDC_ComUserNo, sz);							//账号
	SetComboCtrlParam(IAF_TRIGGERMODE, LoadResString(IDS_STR_TirgMode_LastSpace), IDC_COMBO_TirgMode, sz);//条件单
	SetComboCtrlParam(IAF_ORDERTYPE, LoadResString(IDS_STRING_OrderTypeLStopKey), IDC_COMBO_OrderType, sz);//定单类型
	SetComboCtrlParam(IAF_VALIDTYPE, LoadResString(IDS_STR_ValidType_GTD), IDC_COMBO_ValidType, sz);//有效类型

	//针对状态提示静态框
	UseFontSetCtrlSize(STATIC2_TYPE, m_fontStatic2, sz, wstring(_T("123456789123456789123456789")));
	RECT rectStatic2;
	GetWindowRect(GetDlgItem(m_hDlg, IDC_STATIC_Tip), &rectStatic2);
	ScreenToClient(rectStatic2);
	rectStatic2.right = rectStatic2.left + sz.cx + EDITBORDERWIDTH;
	rectStatic2.bottom = rectStatic2.top + sz.cy + 2;
	MoveWindow(GetDlgItem(m_hDlg, IDC_STATIC_Tip), rectStatic2.left, rectStatic2.top, rectStatic2.right - rectStatic2.left, rectStatic2.bottom - rectStatic2.top, TRUE);

	//针对静态文本
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_Code), IDC_STATIC_Code, sz, NULL);	//合约代码
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_Direct), IDC_STATIC_Direct, sz, NULL);		//买/卖
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_Offset), IDC_STATIC_Offset, sz, NULL);		//开/平
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_OrderPrice), IDC_STATIC_OrderPrice, sz, NULL);	//委托价格
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_OrderQty), IDC_STATIC_OrderQty, sz, NULL);	//委托数量
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_MultiQty), IDC_STATIC_QtyMulti, sz, NULL);	//数量倍率
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_TrigMode), IDC_STATIC_TrigMode, sz, NULL);	//触发方式
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_TrigCon), IDC_STATIC_TrigCondi, sz, NULL);		//条件
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_TrigPrice), IDC_STATIC_TrigPrice, sz, NULL);	//触发价格
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_QtyCount), IDC_STATIC_QtyCount, sz, NULL);	//下单次数
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_IncreQty), IDC_STATIC_IncreQty, sz, NULL);	//数量增量
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_IncrePrice), IDC_STATIC_IncrePrice, sz, NULL);	//价格增量

	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_OrderType), IDC_STATIC_OrderType, sz, NULL);	//定单类型
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_ValidType), IDC_STATIC_ValidType, sz, NULL);	//有效类型
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_ValidDate), IDC_STATIC_ValidData, sz, NULL);	//有效日期
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_MaxQty), IDC_STATIC_MaxQty, sz, NULL);	//最大量
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_MinQty), IDC_STATIC_MinQty, sz, NULL);	//最小量

	int iStaticMaxWidth = -1;
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_UserNo), IDC_STATIC_UserNo, sz, &iStaticMaxWidth);	//客户账号

	SetStaticCtrlParam(m_fontPosition, LoadResString(IDS_STR_STATIC_BuyPos), IDC_StaticBuyPos, sz, NULL, false);	//多头持仓
	SetStaticCtrlParam(m_fontPosition, LoadResString(IDS_STR_STATIC_SellPos), IDC_StaticSellPos, sz, NULL, false);	//空头持仓

	//修改跟盘的窗口
	SetStaticCtrlParam(m_fontStatic1, LoadResString(IDS_STR_STATIC_OrderPrice), IDC_CHECK_AutoPrice, sz, NULL);
	//check 仅需要最长的就行了
	SetCheckCtrlParam(IAF_TJCHECK, LoadResString(IDS_STR_BatchOrder), IDC_Check_BatchOrder, sz);
	
	//Btn  大小按最宽的来算
	SetBtnCtrlParam(LoadResString(IDS_STR_DoOrder), IDC_BUTTON_DoOrder, sz);		//
	SetBtnCtrlParam(LoadResString(IDS_STR_PayOrder), IDC_BUTTON_PayOrder, sz);
	SetBtnCtrlParam(LoadResString(IDS_STR_AutoOrder), IDC_BUTTON_AutoOrder, sz);
	SetBtnCtrlParam(LoadResString(IDS_STR_Seq), IDC_Btn_Seq, sz);
	int selcodebtnwidth = 0;
	SetBtnCtrlParam(_T("....."), IDC_Btn_SelCode, sz, &selcodebtnwidth);

	//日期宽度
	SetDateTimeCtrlParam();

	//静态文本和可编辑框要选择最大长度的
	if (iStaticMaxWidth != -1)
	{
		int iComboMaxWidth = -1;
		for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
		{
			if (IAF_ACCOUNT == m_InputCfgArray[i].id)
			{
				iComboMaxWidth = m_InputCfgArray[i].width;
				if (iStaticMaxWidth > iComboMaxWidth)
					m_InputCfgArray[i].width = iStaticMaxWidth;
			}
			else if (IAF_COMMODITY == m_InputCfgArray[i].id)
			{
				ModifyCtrlWidth(IAF_SELECODE, m_InputCfgArray[i].height * 3 / 4);
			}
		}
	}
	m_BuyPositionStatic.UpdateText(0, LoadResString(IDS_STR_LongPosition).c_str(), m_OrderFont.lgPositionFont, g_ColorRefData.GetColorBackground(), m_OrderFont.rgbBuy);
	m_SellPositionStatic.UpdateText(0, LoadResString(IDS_STR_ShortPosition).c_str(), m_OrderFont.lgPositionFont, g_ColorRefData.GetColorBackground(), m_OrderFont.rgbSell);
	return true;
}
//读取进来列表的配置信息
void COFInputDlg::ReadFieldConfig()
{
	m_InputCfgArray.clear();
	for (int i = 0; i < m_OrderFiled.nFieldCount; i++)
		m_InputCfgArray.push_back(m_OrderFiled.pOrderField[i]);
	//遍历把选合约放到合约代码后面
	C_OrderField tempSeleCode;
	bool bFind = false;
	for (int i = 0; i < m_OrderFiled.nFieldCount; i++)
	{
		if (m_InputCfgArray.at(i).id == IAF_SELECODE)
		{
			tempSeleCode = m_InputCfgArray.at(i);
			m_InputCfgArray.erase(m_InputCfgArray.begin() + i);
			bFind = true;
			break;
		}
	}
	if (bFind)
	{
		for (int i = 0; i < m_OrderFiled.nFieldCount; i++)
		{
			if (m_InputCfgArray.at(i).id == IAF_COMMODITY)
			{
				m_InputCfgArray.insert(m_InputCfgArray.begin() + i + 1, tempSeleCode);
				break;
			}
		}
	}
	return;
}
//获取下单按钮原始大小
void COFInputDlg::SetOrginBtnWidth()
{
	RECT rectOrgin;
	GetWindowRect(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), &rectOrgin);
	m_iOrginBtnWidth = rectOrgin.right - rectOrgin.left;
	HFONT hfont = (HFONT)::SendMessage(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), WM_GETFONT, 0, 0);
	LOGFONT lgfont;
	::GetObject(hfont, sizeof(LOGFONT), &lgfont);
	SIZE szOrgin;
	UseFontSetCtrlSize(BTN_TYPE, hfont, szOrgin, LoadResString(IDS_STR_DoOrder));
	m_iOrginBtnFontWidth = szOrgin.cx;
}
//设置可见
void  COFInputDlg::SetItemVisible(int id, BOOL bShow)
{
	for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
	{
		C_OrderField &of = m_InputCfgArray.at(i);
		if (of.id == id)
		{
			of.bShow = bShow ? 1 : 0;
			break;
		}
	}
}
//check布局
void COFInputDlg::CheckLayout(int id, bool bShow, int height, RECT &ctrlRect)
{
	if (bShow)
	{
		MoveWindow(GetDlgItem(m_hDlg, id), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);
		ctrlRect.top = ctrlRect.bottom;
		ctrlRect.bottom = ctrlRect.top + height;
	}
	::ShowWindow(GetDlgItem(m_hDlg, id), bShow ? SW_SHOW : SW_HIDE);
}
//条件区布局
void COFInputDlg::TJCheck(C_OrderField &of, RECT &ctrlRect)
{
	//仅一个单客户		最多显示 【条件 平开 快速 批次】 
	//多个单客户		最多显示 【条件 平开 快速 批次 批量】
	//若存在交易员		最多显示 【条件 平开 T+1 强平】 
	ctrlRect.top = 0;
	ctrlRect.bottom = ctrlRect.top + of.height;
	//条件 
	CheckLayout(IDC_CHECK_TJ, IsShowTJ(), of.height, ctrlRect);
	//平开 始终显示
	CheckLayout(IDC_CHECK_PK, IsShowPK(), of.height, ctrlRect);
	//快速 根据是否有交易员登陆决定
	CheckLayout(IDC_CHECK_KS, IsShowKS(), of.height, ctrlRect);
	//批次 根据是否有交易员登陆决定
	CheckLayout(IDC_Check_BatchCount, IsShowPC(), of.height, ctrlRect);
	//批量 根据是否有交易员登陆决定
	CheckLayout(IDC_Check_BatchOrder, IsShowPL(), of.height, ctrlRect);
	//T+1 根据是否有交易员登陆和配置决定
	CheckLayout(IDC_CHECK_T1, IsShowT1(), of.height, ctrlRect);
	//强平 根据是否有交易员登陆决定
	CheckLayout(IDC_CHECK_ForCover, IsShowForceCover(), of.height, ctrlRect);
}
//更新状态
void COFInputDlg::UpdateItemStates()
{
	//先把全部的置于false，然后根据情况确定出来显示的
	for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
	{
		C_OrderField &of = m_InputCfgArray.at(i);
		of.bShow = FALSE;
	}

	if (!IsOnlyOneUser())//非单用户
		SetItemVisible(IAF_ACCOUNT, TRUE);
	else
		SetItemVisible(IAF_ACCOUNT, FALSE);

	SetItemVisible(IAF_TJCHECK, TRUE);

	SetItemVisible(IAF_COMMODITY, TRUE);
	//if (VersionType()!=0)
		SetItemVisible(IAF_SELECODE, TRUE);

	//只有显示买卖按钮才会隐藏 改单不隐藏
	if (!IsShowBuySellBtn() || GetModifyOrderOper())
		SetItemVisible(IAF_DIRECT, TRUE);

	//是否需要显示开平
	if (cmNone == m_CommodityInf.cCoverMode || cmUnfinish == m_CommodityInf.cCoverMode || 1 == VersionType())//不区分开平 外盘 
		SetItemVisible(IAF_OFFSET, FALSE);
	else if (cmCover == m_CommodityInf.cCoverMode || cmCoverToday == m_CommodityInf.cCoverMode || (0 == VersionType() || 2 == VersionType()))
		SetItemVisible(IAF_OFFSET, TRUE);

	char ordertype = GetOrderTypeEx();
	//委托价格 只要是有外盘 就始终显示 不过委托价格要根据定单类型变灰
	SetItemVisible(IAF_PRICE, TRUE);
	if (m_CommodityInf.bIfFractionPrice)
	{
		SetItemVisible(IAF_PRICE_ADD, TRUE);
		SetItemVisible(IAF_PRICE_FENZI, TRUE);
		SetItemVisible(IAF_PRICE_CHUHAO, TRUE);
		SetItemVisible(IAF_PRICE_FENMU, TRUE);
	}

	//设置滚轮增减价格
	SetSpinCtrlBuddy(IDC_SPIN_OrderPrice, IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu);
	SetSpinCtrlBuddy(IDC_SPIN_TrigPrice, IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, IDC_Sta_TriggerPrice_FenMu);
	SetSpinCtrlBuddy(IDC_SPIN_IncrePrice, -1, -1, IDC_Sta_IncrePrice_FenMu);

	SetItemVisible(IAF_VOL, TRUE);

	//触发模式
	if (IsCheck(IDC_CHECK_TJ) && IsShowTJ())
	{
		SetItemVisible(IAF_TRIGGERMODE, TRUE);
		SetItemVisible(IAF_TRIGGERCONDITION, TRUE);
	}

	//选中了条件选项 或者订单类型为止损或者限价止损
	if (IsShowTrigPrice())
	{
		SetItemVisible(IAF_TRIGGERPRICE, TRUE);
		if (m_CommodityInf.bIfFractionPrice)
		{
			SetItemVisible(IAF_TRIGGERPRICE_ADD, TRUE);
			SetItemVisible(IAF_TRIGGERPRICE_FENZI, TRUE);
			SetItemVisible(IAF_TRIGGERPRICE_CHUHAO, TRUE);
			SetItemVisible(IAF_TRIGGERPRICE_FENMU, TRUE);
		}
	}
	Button_Enable(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), BOOL(!IsCheck(IDC_CHECK_TJ)));
	Button_Enable(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), BOOL(!IsCheck(IDC_CHECK_TJ)));

	//选中了快速选项
	if (IsCheck(IDC_CHECK_KS) && IsShowKS())
		SetItemVisible(IAF_VOLBL, TRUE);
	if (IsCheck(IDC_Check_BatchCount) && IsShowPC())
	{
		SetItemVisible(IAF_VOLCOUNT, TRUE);
		SetItemVisible(IAF_INCREVOL, TRUE);
		SetItemVisible(IAF_INCREPRICE, TRUE);
		if (m_CommodityInf.bIfFractionPrice)
		{
			SetItemVisible(IAF_INCRERPRICE_CHUHAO, TRUE);
			SetItemVisible(IAF_INCREPRICE_FENMU, TRUE);
		}
	}
	//
	if (IsShowComboType())
	{
		SetItemVisible(IAF_ORDERTYPE, TRUE);
		SetItemVisible(IAF_VALIDTYPE, TRUE);
	}
	//限期有效才显示
	if (IsShowComboType() && vtGTD == GetValidTypeEx())
		SetItemVisible(IAF_VALIDTIME, TRUE);
	//冰山单才显示
	if (IsShowComboType() && otIceberg == ordertype)
	{
		SetItemVisible(IAF_MAXQTY, TRUE);
		SetItemVisible(IAF_MINQTY, TRUE);
	}
}
//移动静态文本
void COFInputDlg::MoveStaticCtrl(bool bShow, int idStatic, RECT &ctrlRect)
{
	if (bShow)
	{
		RECT CurStaticRect;//静态文本的宽高
		GetWindowRect(GetDlgItem(m_hDlg, idStatic), &CurStaticRect);
		ctrlRect.bottom = ctrlRect.top + CurStaticRect.bottom - CurStaticRect.top;
		MoveWindow(GetDlgItem(m_hDlg, idStatic), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);
	}
	::ShowWindow(GetDlgItem(m_hDlg, idStatic), bShow ? SW_SHOW : SW_HIDE);
}
//移动控件
void COFInputDlg::MoveCtrl(bool bShow, int id, int iVerInterval1, int height, RECT &ctrlRect, HWND &LastWnd)
{
	if (bShow)
	{
		ctrlRect.top = ctrlRect.bottom + iVerInterval1;
		ctrlRect.bottom = ctrlRect.top + height;
		MoveWindow(GetDlgItem(m_hDlg, id), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);
		SetWindowPos(GetDlgItem(m_hDlg, id), LastWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE/*|SWP_NOACTIVATE*/);
		LastWnd = GetDlgItem(m_hDlg, id);
	}
	::ShowWindow(GetDlgItem(m_hDlg, id), bShow ? SW_SHOW : SW_HIDE);
}
//移动spin
void COFInputDlg::MoveSpinCtrl(bool bShow, int idspin, int idStatic, const RECT &InputRect, RECT &ctrlRect, int &iExtraSpinWidht)
{
	if (bShow)
	{
		ctrlRect.left = ctrlRect.right+1;
		ctrlRect.right = InputRect.right+1;
		ctrlRect.right += ((ctrlRect.bottom - ctrlRect.top) * 2 / 3);//spin控件的宽度
		iExtraSpinWidht = ((ctrlRect.bottom - ctrlRect.top) * 2 / 3)+1;
		ctrlRect.top = ctrlRect.top;
		ctrlRect.bottom = ctrlRect.bottom;
		MoveWindow(GetDlgItem(m_hDlg, idspin), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);
		RECT rectS,rectSC;
		GetWindowRect(GetDlgItem(m_hDlg, idStatic), &rectS);
		CopyRect(&rectSC, &rectS);
		ScreenToClient(rectSC);
		MoveWindow(GetDlgItem(m_hDlg, idStatic), rectSC.left, rectSC.top, ctrlRect.right - rectSC.left, rectSC.bottom - rectSC.top, TRUE);
	}
	::ShowWindow(GetDlgItem(m_hDlg, idspin), bShow ? SW_SHOW : SW_HIDE);
}
//移动分数报价部分
void  COFInputDlg::MoveFractionCtrl(bool bShow, int idStatic, int idFraction, int interval, int iVerInterval1, int height, RECT &InputRect, RECT &ctrlRect, HWND &LastWnd)
{
	if (bShow)
	{
		RECT CurStaticRect;
		GetWindowRect(GetDlgItem(m_hDlg, idStatic), &CurStaticRect);
		ctrlRect.bottom = ctrlRect.top + (CurStaticRect.bottom - CurStaticRect.top);
		ctrlRect.left -= interval;
		ctrlRect.right -= interval;
		InputRect.left -= interval;
		ctrlRect.top = ctrlRect.bottom + iVerInterval1;
		ctrlRect.bottom = ctrlRect.top + height;
		MoveWindow(GetDlgItem(m_hDlg, idFraction), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);
		SetWindowPos(GetDlgItem(m_hDlg, idFraction), LastWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE/*|SWP_NOACTIVATE*/);
		LastWnd = GetDlgItem(m_hDlg, idFraction);
	}
	::ShowWindow(GetDlgItem(m_hDlg, idFraction), bShow ? SW_SHOW : SW_HIDE);
}
//移动按钮、持仓 布局
void COFInputDlg::MoveBtnCtrl(RECT &rectBtn, RECT &InputRect, int interval, HWND&LastWnd, int iStateBarLeft, int &iSeqBtnLeft)
{
	int iTopBotMargin = 5;
	SIZE szDlg = GetKbDlgSize();
	InputRect.left += (int)(interval*1.6);//与最后一个的间距是普通的1.6倍
	RECT rectOrginBtn;

	//改单界面
	if (GetModifyOrderOper())
	{
		ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Buy), SW_HIDE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Sell), SW_HIDE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), SW_HIDE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), SW_HIDE);

		RECT rectCode;
		GetWindowRect(GetDlgItem(m_hDlg, IDC_EDIT_Code), &rectCode);
		GetWindowRect(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), &rectOrginBtn);
		ScreenToClient(rectCode);
		rectBtn.left = InputRect.left;
		rectBtn.top = rectCode.top;
		rectBtn.right = rectBtn.left + rectOrginBtn.right - rectOrginBtn.left;	//按钮宽度
		rectBtn.bottom = rectBtn.top + rectCode.bottom - rectCode.top;	//按钮高度
		MoveWindow(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		SetWindowPos(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), LastWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), SW_SHOW);
		LastWnd = GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder);

		iSeqBtnLeft = rectBtn.left - iStateBarLeft;

		GetWindowRect(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), &rectOrginBtn);
		rectBtn.left = rectBtn.right + (int)(interval*1.6);
		rectBtn.right = rectBtn.left + rectOrginBtn.right - rectOrginBtn.left;	//按钮宽度
		MoveWindow(GetDlgItem(m_hDlg, IDC_Btn_Seq), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Seq), SW_SHOW);
		return;
	}

	//非改单
	if (IsShowBuySellBtn())
	{
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), SW_HIDE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Seq), SW_HIDE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), SW_HIDE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), SW_HIDE);

		//按钮的宽度与价格编辑框的宽度相关 因为按钮上需要显示跟盘价格 
		GetWindowRect(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice), &rectOrginBtn);

		rectBtn.left = InputRect.left;
		rectBtn.top = iTopBotMargin - 3;
		rectBtn.right = rectBtn.left + rectOrginBtn.right - rectOrginBtn.left;
		rectBtn.bottom = szDlg.cy - iTopBotMargin;
		MoveWindow(GetDlgItem(m_hDlg, IDC_Btn_Buy), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		SetWindowPos(GetDlgItem(m_hDlg, IDC_Btn_Buy), LastWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Buy), SW_SHOW);
		LastWnd = GetDlgItem(m_hDlg, IDC_Btn_Buy);
		iSeqBtnLeft = rectBtn.left - iStateBarLeft;

		rectBtn.left = rectBtn.right + (int)(interval*1.6);
		rectBtn.right = rectBtn.left + rectOrginBtn.right - rectOrginBtn.left;
		MoveWindow(GetDlgItem(m_hDlg, IDC_Btn_Sell), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		SetWindowPos(GetDlgItem(m_hDlg, IDC_Btn_Sell), LastWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Sell), SW_SHOW);
		LastWnd = GetDlgItem(m_hDlg, IDC_Btn_Sell);
		return;
	}
	ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Buy), SW_HIDE);
	ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Sell), SW_HIDE);

	//移动一下下单几个按钮
	GetWindowRect(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), &rectOrginBtn);		//这是最长字符的btn 其他的都按这个来

	//水平显示按钮 begin
	RECT rectCode;
	GetWindowRect(GetDlgItem(m_hDlg, IDC_EDIT_Code), &rectCode);
	ScreenToClient(rectCode);
	if (IsShowBtn())//确定 自动单 埋单
	{
		rectBtn.left = InputRect.left;
		rectBtn.top = iTopBotMargin;
		rectBtn.right = rectBtn.left + rectOrginBtn.right - rectOrginBtn.left;	//按钮宽度
		rectBtn.bottom = rectBtn.top + rectOrginBtn.bottom - rectOrginBtn.top;	//按钮高度
		MoveWindow(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), SW_SHOW);
		LastWnd = GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder);

		iSeqBtnLeft = rectBtn.left - iStateBarLeft;
		rectBtn.bottom = szDlg.cy - iTopBotMargin;
		rectBtn.top = rectBtn.bottom - (rectOrginBtn.bottom - rectOrginBtn.top);
		MoveWindow(GetDlgItem(m_hDlg, IDC_Btn_Seq), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Seq), SW_SHOW );

		GetWindowRect(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), &rectOrginBtn);
		rectBtn.left = rectBtn.right + (int)(interval*1.6);
		rectBtn.right = rectBtn.left + rectOrginBtn.right - rectOrginBtn.left;	//按钮宽度
		rectBtn.top = iTopBotMargin;
		rectBtn.bottom = rectBtn.top + rectOrginBtn.bottom - rectOrginBtn.top;	//按钮高度
		MoveWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), SW_SHOW);

		rectBtn.bottom = szDlg.cy - iTopBotMargin;
		rectBtn.top = rectBtn.bottom - (rectOrginBtn.bottom - rectOrginBtn.top);
		MoveWindow(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder),SW_SHOW);
	}
	else//仅确定按钮
	{
		GetWindowRect(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice), &rectOrginBtn);//取按钮宽度

		RECT rectStatic;
		GetWindowRect(GetDlgItem(m_hDlg, IDC_STATIC_OrderQty), &rectStatic);//取按钮宽度

		rectBtn.left = InputRect.left;
		rectBtn.right = rectBtn.left + (rectOrginBtn.right - rectOrginBtn.left) * 5 / 4;	//按钮宽度
		rectBtn.bottom = (rectStatic.bottom - rectStatic.top) * 3 / 2 + rectOrginBtn.bottom - rectOrginBtn.top; //按钮高度
		rectBtn.top = (rectStatic.bottom - rectStatic.top) / 2;

		iSeqBtnLeft = rectBtn.left - iStateBarLeft;
		MoveWindow(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), rectBtn.left, rectBtn.top, rectBtn.right - rectBtn.left, rectBtn.bottom - rectBtn.top, TRUE);
		LastWnd = GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder);

		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), SW_SHOW);
		ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_Seq), SW_HIDE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), SW_HIDE);
		ShowWindow(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), SW_HIDE);
	}
}
//移动状态提示
void COFInputDlg::MoveStatusTip(int iStateBarLeft, int iTempHeight, int iVerInterval2, int iDepoRight)
{
	//调整消息提示static的位置
	RECT rectStatic2;
	GetWindowRect(GetDlgItem(m_hDlg, IDC_STATIC_Tip), &rectStatic2);
	ScreenToClient(rectStatic2);

	int nFirst = 62, nSecond = 1, nThird = 1;
	if (VersionType()!=0)//外盘
	{
		nFirst = 24;
		nSecond = 15;
		nThird = 25;
	}
	
	RECT InfoRect;
	InfoRect.left = iStateBarLeft +/*interval*/0;
	InfoRect.top = iTempHeight + iVerInterval2;
	InfoRect.right = InfoRect.left + iDepoRight * nFirst / 64;
	InfoRect.bottom = InfoRect.top + rectStatic2.bottom - rectStatic2.top;
	MoveWindow(GetDlgItem(m_hDlg, IDC_STATIC_Tip), InfoRect.left, InfoRect.top, InfoRect.right - InfoRect.left, InfoRect.bottom - InfoRect.top, TRUE);
	InfoRect.left = InfoRect.right;
	InfoRect.right = InfoRect.left + iDepoRight * nSecond / 64;//首先分8份 然后剩下的5/8再分8份 保证金占5份
	MoveWindow(GetDlgItem(m_hDlg, IDC_STATIC_CanOpenCover), InfoRect.left, InfoRect.top, InfoRect.right - InfoRect.left, InfoRect.bottom - InfoRect.top, TRUE);
	InfoRect.left = InfoRect.right;
	InfoRect.right = InfoRect.left + iDepoRight * nThird / 64;
	MoveWindow(GetDlgItem(m_hDlg, IDC_STATIC_Deposit), InfoRect.left, InfoRect.top, InfoRect.right - InfoRect.left, InfoRect.bottom - InfoRect.top, TRUE);
}
//移动持仓
void COFInputDlg::MovePosCtrl(int interval, RECT &ctrlRect)
{
	//持仓信息显示 与相关
	RECT rectPos, rectEdit;
	GetWindowRect(GetDlgItem(m_hDlg, IDC_StaticBuyPos), &rectPos);
	GetWindowRect(GetDlgItem(m_hDlg, IDC_EDIT_Code), &rectEdit);
	ScreenToClient(rectEdit);
	int show = (m_FillOrderAssist.bShowPosition) ? SW_SHOW : SW_HIDE;
	ShowWindow(GetDlgItem(m_hDlg, IDC_StaticBuyPos), show);
	ShowWindow(GetDlgItem(m_hDlg, IDC_StaticSellPos), show);

	if (m_FillOrderAssist.bShowPosition)
	{
		SetWindowPos(GetDlgItem(m_hDlg, IDC_StaticBuyPos), NULL, 0, 0, rectPos.right - rectPos.left, rectEdit.bottom - rectEdit.top - 2, SWP_NOMOVE);
		SetWindowPos(GetDlgItem(m_hDlg, IDC_StaticSellPos), NULL, 0, 0, rectPos.right - rectPos.left, rectEdit.bottom - rectEdit.top - 2, SWP_NOMOVE);

		//持仓大小需要调整 只有一个按钮时调整为
		RECT btnRect;
		if (IsShowBuySellBtn())
			GetWindowRect(GetDlgItem(m_hDlg, IDC_Btn_Sell), &btnRect);
		else
		{
			if (IsShowBtn())
				GetWindowRect(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), &btnRect);
			else
				GetWindowRect(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), &btnRect);
		}
		//改单
		if (GetModifyOrderOper())
			GetWindowRect(GetDlgItem(m_hDlg, IDC_Btn_Seq), &btnRect);

		ScreenToClient(btnRect);
		ctrlRect.left = btnRect.right + interval;
		ctrlRect.right = ctrlRect.left + rectPos.right - rectPos.left;
		ctrlRect.top = rectEdit.top;
		ctrlRect.bottom = ctrlRect.top + rectEdit.bottom - rectEdit.top - 2;
		MoveWindow(GetDlgItem(m_hDlg, IDC_StaticBuyPos), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);

		ctrlRect.left = ctrlRect.right + interval;
		ctrlRect.right = ctrlRect.left + rectPos.right - rectPos.left;
		MoveWindow(GetDlgItem(m_hDlg, IDC_StaticSellPos), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);
	}
}
//布局
int COFInputDlg::Layout()
{
	if (!GetCanLayout())
		return 0;
	Button_Enable(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), TRUE);
	//根据当前选择，更新一下各项的状态
	UpdateItemStates();

	//这里根据m_InputCfgArray的配置及选项设置自动排列窗口
	int interval = m_OrderFont.iHorCtrlInterval;			//水平控件之间间隔
	int iVerInterval1 = m_OrderFont.iVerCtrlInterval1;	//垂直控件间距1 静态文本与水平控件的间距

	RECT InputRect;											//当前输入区域rect
	InputRect.left = m_OrderFont.iLeftMargin - interval;//起始左边界
	InputRect.top = 3;										//上边界
	InputRect.bottom = 50;									//下边界
	InputRect.right = 0;

	HWND LastWnd = HWND_TOP;
	RECT ctrlRect;				//当前控件rect
	int flag = SW_HIDE;

	RECT CurStaticRect;		//静态文本的宽高
	int   iTempHeight = -1;
	int	  iMaxWidth = -1;		//根据这个和iMaxHeight调整对话框的大小
	int   iStateBarLeft = 0;	//状态提示的左起点
	int   iExtraSpinWidht = 0;//spin width

	for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
	{
		iExtraSpinWidht = 0;
		C_OrderField &of = m_InputCfgArray.at(i);
		if (of.bShow)
		{
			InputRect.left += interval;
			InputRect.right = InputRect.left + of.width;
			ctrlRect = InputRect;
		}
		flag = of.bShow ? SW_SHOW : SW_HIDE;
		switch (of.id)
		{
			case IAF_TJCHECK://条件选择区
				TJCheck(of, ctrlRect);
				break;
			case IAF_ACCOUNT://客户账号 静态文本的宽度要of.width-锁定按钮的宽度
				MoveStaticCtrl(of.bShow, IDC_STATIC_UserNo, ctrlRect);
				SetWindowPos(GetDlgItem(m_hDlg, IDC_Btn_LockUserNo), NULL, ctrlRect.right - (ctrlRect.bottom - ctrlRect.top), ctrlRect.top - 1, 0, 0, SWP_NOSIZE);
				MoveCtrl(of.bShow, IDC_ComUserNo, iVerInterval1, of.height, ctrlRect, LastWnd);
				if (of.bShow)
				{
					RECT rectcombo;
					GetWindowRect(GetDlgItem(m_hDlg, IDC_Static_LoginNo), &CurStaticRect);
					GetWindowRect(GetDlgItem(m_hDlg, IDC_ComUserNo), &rectcombo);
					ScreenToClient(rectcombo);
					ctrlRect.top = rectcombo.bottom + 3;
					ctrlRect.bottom = ctrlRect.top + (CurStaticRect.bottom - CurStaticRect.top);
					MoveWindow(GetDlgItem(m_hDlg, IDC_Static_LoginNo), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);
				}
				ShowWindow(GetDlgItem(m_hDlg, IDC_Static_LoginNo), flag);
				ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_LockUserNo), flag);
				ShowWindow(GetDlgItem(m_hDlg, IDC_Static_LoginNo), flag);
				SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), L"");
				break;
			case IAF_COMMODITY://合约代码
				if (of.bShow)
				{
					RECT rectStatic;
					GetWindowRect(GetDlgItem(m_hDlg, IDC_STATIC_Code), &CurStaticRect);
					ctrlRect.bottom = ctrlRect.top + (CurStaticRect.bottom - CurStaticRect.top);
					rectStatic = ctrlRect;
					//rectStatic.right = rectStatic.left + (CurStaticRect.right - CurStaticRect.left);
					MoveWindow(GetDlgItem(m_hDlg, IDC_STATIC_Code), rectStatic.left, rectStatic.top, rectStatic.right - rectStatic.left, rectStatic.bottom - rectStatic.top, TRUE);

					RECT rectStaticCode;
					rectStaticCode.left = rectStatic.right + 0;
					rectStaticCode.right = ctrlRect.right;
					rectStaticCode.top = rectStatic.top;
					rectStaticCode.bottom = rectStatic.bottom;
					MoveWindow(GetDlgItem(m_hDlg, IDC_STATIC_CodeName), rectStaticCode.left, rectStaticCode.top, rectStaticCode.right - rectStaticCode.left, rectStaticCode.bottom - rectStaticCode.top, TRUE);

					MoveCtrl(of.bShow, IDC_EDIT_Code, iVerInterval1, of.height, ctrlRect, LastWnd);
					iStateBarLeft = ctrlRect.left;
					iTempHeight = ctrlRect.bottom;	//目前高度 还要加上间隔和状态栏高度
				}
				ShowWindow(GetDlgItem(m_hDlg, IDC_STATIC_Code), flag);
				break;
			case IAF_SELECODE://
				if (of.bShow)
				{
					GetWindowRect(GetDlgItem(m_hDlg, IDC_EDIT_Code), &CurStaticRect);
					ScreenToClient(CurStaticRect);
					ctrlRect.top = CurStaticRect.top;
					ctrlRect.left -= interval-1;
					InputRect.left -= interval-1;
					ctrlRect.right -= interval-1;
					ctrlRect.bottom = ctrlRect.top + CurStaticRect.bottom - CurStaticRect.top;
					MoveWindow(GetDlgItem(m_hDlg, IDC_Btn_SelCode), ctrlRect.left, ctrlRect.top, ctrlRect.right - ctrlRect.left, ctrlRect.bottom - ctrlRect.top, TRUE);
					RECT rectTep;
					GetWindowRect(GetDlgItem(m_hDlg, IDC_STATIC_Code), &rectTep);
					ScreenToClient(rectTep);
					SetWindowPos(GetDlgItem(m_hDlg, IDC_Btn_LockCode), NULL, ctrlRect.right - (rectTep.bottom - rectTep.top) - 2, ctrlRect.top - (rectTep.bottom - rectTep.top) - 1, 0, 0, SWP_NOSIZE);
				}
				ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_SelCode), flag);
				ShowWindow(GetDlgItem(m_hDlg, IDC_Btn_LockCode), flag);
				break;
			case IAF_DIRECT://买卖方向
				MoveStaticCtrl(of.bShow, IDC_STATIC_Direct, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_Direct, iVerInterval1, of.height, ctrlRect, LastWnd);
				break;
			case IAF_OFFSET://开平
				MoveStaticCtrl(of.bShow, IDC_STATIC_Offset, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_Offset, iVerInterval1, of.height, ctrlRect, LastWnd);
				break;
			case IAF_PRICE://委托价格  要自动判断是否显示分数报价
				MoveStaticCtrl(of.bShow, IDC_STATIC_OrderPrice, ctrlRect);
				if (of.bShow)
				{
					RECT rectTemp;
					GetWindowRect(GetDlgItem(m_hDlg, IDC_CHECK_AutoPrice), &rectTemp);
					MoveWindow(GetDlgItem(m_hDlg, IDC_CHECK_AutoPrice), ctrlRect.left, ctrlRect.top, rectTemp.right - rectTemp.left, rectTemp.bottom - rectTemp.top, TRUE);
				}
				MoveCtrl(of.bShow, IDC_EDIT_OrderPrice, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_OrderPrice, IDC_STATIC_OrderPrice, InputRect, ctrlRect, iExtraSpinWidht);
				
				break;
			case IAF_PRICE_ADD:
				MoveFractionCtrl(of.bShow, IDC_STATIC_OrderPrice, IDC_Sta_OrderPrice_Add, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_PRICE_FENZI:
				MoveFractionCtrl(of.bShow, IDC_STATIC_OrderPrice, IDC_EDITORDERPRICE_FenZi, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_PRICE_CHUHAO:
				MoveFractionCtrl(of.bShow, IDC_STATIC_OrderPrice, IDC_Sta_OrderPrice_ChuHao, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_PRICE_FENMU:
				MoveFractionCtrl(of.bShow, IDC_STATIC_OrderPrice, IDC_Sta_OrderPrice_FenMu, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_VOL://委托数量
				MoveStaticCtrl(of.bShow, IDC_STATIC_OrderQty, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_OrderQty, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_OrderQty, IDC_STATIC_OrderQty,InputRect, ctrlRect, iExtraSpinWidht);
				break;
			case IAF_TRIGGERMODE://触发方式
				MoveStaticCtrl(of.bShow, IDC_STATIC_TrigMode, ctrlRect);
				MoveCtrl(of.bShow, IDC_COMBO_TirgMode, iVerInterval1, of.height, ctrlRect, LastWnd);
				break;
			case IAF_ORDERTYPE:
				MoveStaticCtrl(of.bShow, IDC_STATIC_OrderType, ctrlRect);
				MoveCtrl(of.bShow, IDC_COMBO_OrderType, iVerInterval1, of.height, ctrlRect, LastWnd);
				if (of.bShow)
					ComboBox_SetEditSel(GetDlgItem(m_hDlg, IDC_COMBO_OrderType), -1, -1);
				break;
			case IAF_VALIDTYPE:
				MoveStaticCtrl(of.bShow, IDC_STATIC_ValidType, ctrlRect);
				MoveCtrl(of.bShow, IDC_COMBO_ValidType, iVerInterval1, of.height, ctrlRect, LastWnd);
				if (of.bShow)
					ComboBox_SetEditSel(GetDlgItem(m_hDlg, IDC_COMBO_ValidType), -1, -1);
				break;
			case IAF_VALIDTIME://有效日期
			{
				MoveStaticCtrl(of.bShow, IDC_STATIC_ValidData, ctrlRect);
				RECT rectComUser;
				GetWindowRect(GetDlgItem(m_hDlg, IDC_EDIT_Code), &rectComUser);
				ctrlRect.right = ctrlRect.left + of.width;
				MoveCtrl(of.bShow, IDC_DATETIMEPICKER_ValidData, iVerInterval1,rectComUser.bottom - rectComUser.top + 1, ctrlRect, LastWnd);
			}
			break;
			case IAF_TRIGGERCONDITION://条件
				MoveStaticCtrl(of.bShow, IDC_STATIC_TrigCondi, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_TrigCondi, iVerInterval1, of.height, ctrlRect, LastWnd);
				break;
			case IAF_TRIGGERPRICE://触发价格
				MoveStaticCtrl(of.bShow, IDC_STATIC_TrigPrice, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_TrigPrice, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_TrigPrice, IDC_STATIC_TrigPrice,InputRect, ctrlRect, iExtraSpinWidht);
				break;
			case IAF_TRIGGERPRICE_ADD:
				MoveFractionCtrl(of.bShow, IDC_STATIC_TrigPrice, IDC_Sta_TriggerPrice_Add, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_TRIGGERPRICE_FENZI:
				MoveFractionCtrl(of.bShow, IDC_STATIC_TrigPrice, IDC_EDITTriggerPRICE_FenZi, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_TRIGGERPRICE_CHUHAO:
				MoveFractionCtrl(of.bShow, IDC_STATIC_TrigPrice, IDC_Sta_TriggerPrice_ChuHao, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_TRIGGERPRICE_FENMU:
				MoveFractionCtrl(of.bShow, IDC_STATIC_TrigPrice, IDC_Sta_TriggerPrice_FenMu, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_VOLBL://数量倍率
				MoveStaticCtrl(of.bShow, IDC_STATIC_QtyMulti, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_QtyMulti, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_QtyMulti, IDC_STATIC_QtyMulti, InputRect, ctrlRect, iExtraSpinWidht);
				break;
			case IAF_INCREVOL://数量增量
				MoveStaticCtrl(of.bShow, IDC_STATIC_IncreQty, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_IncreQty, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_IncreQty, IDC_STATIC_IncreQty, InputRect, ctrlRect, iExtraSpinWidht);
				break;
			case IAF_INCREPRICE://价格增量
				MoveStaticCtrl(of.bShow, IDC_STATIC_IncrePrice, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_IncrePrice, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_IncrePrice, IDC_STATIC_IncrePrice, InputRect, ctrlRect, iExtraSpinWidht);
				break;
			case IAF_INCRERPRICE_CHUHAO:
				MoveFractionCtrl(of.bShow, IDC_STATIC_IncrePrice, IDC_Sta_InvrePrice_ChuHao, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_INCREPRICE_FENMU:
				MoveFractionCtrl(of.bShow, IDC_STATIC_IncrePrice, IDC_Sta_IncrePrice_FenMu, interval, iVerInterval1, of.height, InputRect, ctrlRect, LastWnd);
				break;
			case IAF_VOLCOUNT://下单次数
				MoveStaticCtrl(of.bShow, IDC_STATIC_QtyCount, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_QtyCount, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_QtyCount, IDC_STATIC_QtyCount, InputRect, ctrlRect, iExtraSpinWidht);
				break;
			case IAF_MAXQTY://最大量
				MoveStaticCtrl(of.bShow, IDC_STATIC_MaxQty, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_MaxQty, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_MaxQty, IDC_STATIC_MaxQty, InputRect, ctrlRect, iExtraSpinWidht);
				break;
			case IAF_MINQTY://最小量
				MoveStaticCtrl(of.bShow, IDC_STATIC_MinQty, ctrlRect);
				MoveCtrl(of.bShow, IDC_EDIT_MinQty, iVerInterval1, of.height, ctrlRect, LastWnd);
				MoveSpinCtrl(of.bShow, IDC_SPIN_MinQty, IDC_STATIC_MinQty, InputRect, ctrlRect, iExtraSpinWidht);
				break;
		}
		if (of.bShow)
			InputRect.left += (of.width + iExtraSpinWidht);
	}
	
	//按钮
	RECT rectBtn;
	int iSeqBtnLeft = 0;
	MoveBtnCtrl(rectBtn, InputRect, interval, LastWnd, iStateBarLeft, iSeqBtnLeft);

	//调整消息提示static的位置
	MoveStatusTip(iStateBarLeft, iTempHeight, m_OrderFont.iVerCtrlInterval2, iSeqBtnLeft);

	//持仓
	MovePosCtrl(interval, ctrlRect);

	iMaxWidth = rectBtn.right + m_OrderFont.iLeftMargin;
	RECT rectDlg;
	rectDlg.left = 0;
	rectDlg.top = 0;
	rectDlg.right = iMaxWidth > (ctrlRect.right + m_OrderFont.iLeftMargin) ? iMaxWidth : (ctrlRect.right + m_OrderFont.iLeftMargin);
	rectDlg.bottom = GetKbDlgSize().cy;

	ReInvalidateCtrl();						//恢复可用状态

	ModifyBtnName();						//改单按钮显示修改

	EnableModifyOrder(m_bModifyOrderOper);	//改单控件状态修改

	UpdateAltHotKeyShowState();				//Alt快捷键响应控件状态显示修改

	SetCanLayout(true);

	InitItemPos();

	if (!GetModifyOrderOper())
		SetFastControlState();//快速勾选与恢复

	UpdatePropertyCfg();
	return 0;
}
//修改按钮名字
void COFInputDlg::ModifyBtnName()
{
	if (GetModifyOrderOper())
		m_OrderButton.SetButtonText(m_bModifyOrderOperBatch ? LoadResString(IDS_BatchModify).c_str() : LoadResString(IDS_STR_ModifyOrder).c_str());
	else
		m_OrderButton.SetButtonText(LoadResString(IDS_STR_DoOrder).c_str());
	m_SeqButton.SetButtonText(GetModifyOrderOper() ? LoadResString(IDS_STR_Cancel).c_str() : LoadResString(IDS_STR_Seq).c_str());
}
//修改按钮名字
void COFInputDlg::ModifyStaticUserName()
{
	if (IsCheck(IDC_Check_BatchOrder))
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_UserNo), LoadResString(IDS_STR_STATIC_PLGroupNo).c_str());
	else
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_UserNo), LoadResString(IDS_STR_STATIC_UserNo).c_str());
}
//改单控制ctrl可用
void COFInputDlg::EnableModifyOrder(bool bModify)
{
	BOOL bEnable = bModify ? FALSE : TRUE;
	HWND hWnd = GetTopWindow(m_hDlg);
	while (hWnd != NULL)
	{
		if (IsWindowVisible(hWnd))
			EnableWindow(hWnd, bEnable);
		hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
	}
	
	Button_Enable(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), BOOL(!IsCheck(IDC_CHECK_TJ)));
	Button_Enable(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), BOOL(!IsCheck(IDC_CHECK_TJ)));
	//能改变的控件
	if (bModify)
	{
		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), TRUE);
		EnableWindow(GetDlgItem(m_hDlg, IDC_SPIN_OrderQty), TRUE);

		char OrderType = GetOrderType();
		if (otLimit == OrderType || otLimitStop == OrderType || otIceberg == OrderType || otGhost == OrderType)
		{
			EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice), TRUE);
			EnableWindow(GetDlgItem(m_hDlg, IDC_SPIN_OrderPrice), TRUE);
			EnableWindow(GetDlgItem(m_hDlg, IDC_CHECK_AutoPrice), TRUE);
		}
		if (otMarketStop == OrderType || otLimitStop == OrderType)
		{
			EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_TrigPrice), TRUE);
			EnableWindow(GetDlgItem(m_hDlg, IDC_SPIN_TrigPrice), TRUE);
		}
		EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), TRUE);
		EnableWindow(GetDlgItem(m_hDlg, IDC_Btn_Seq), TRUE);

		if (m_bModifyOrderOperBatch)
		{
			EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), FALSE);
			EnableWindow(GetDlgItem(m_hDlg, IDC_SPIN_OrderQty), FALSE);
		}	
	}
	//
	IniAutoPriceCheck();

	//T1
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	GetContractStr(str);
	EnableT1(str.c_str());
}
//T1
void COFInputDlg::EnableT1(const char *szCode)
{
	//港交所  
	BOOL able = IsShowT1() && CConvert::IsHK(szCode);
	EnableWindow(GetDlgItem(m_hDlg, IDC_CHECK_T1), able);
	if (!able)
		m_CheckT1.SetCheck(BST_UNCHECKED);

	//如果是改单 不可用
	if (GetModifyOrderOper())
		EnableWindow(GetDlgItem(m_hDlg, IDC_CHECK_T1), able);
}
//委托价格是否能用
BOOL COFInputDlg::OrderPriceIsCanAble()
{
	//价格框 可用 显示cmb（1（显示买卖按钮（1，0）），0（显示买卖按钮（1，0））） 四种情况
	char ordertype = GetOrderTypeEx();
	return
		(IsShowComboType() && IsShowBuySellBtn() && m_SwitchFollow.GetSwitchLevel() == 0 &&otMarket != ordertype&&otMarketStop != ordertype) ||
		(IsShowComboType() && !IsShowBuySellBtn() && otMarket != ordertype&&otMarketStop != ordertype) ||
		(!IsShowComboType() && IsShowBuySellBtn() && m_SwitchFollow.GetSwitchLevel() == 0) ||
		(!IsShowComboType() && !IsShowBuySellBtn());
}
//触发价格是否能用
BOOL COFInputDlg::TriggerPriceIsCanAble()
{
	char ordertype = GetOrderTypeEx();
	return (IsShowComboType() && (otMarketStop == ordertype || otLimitStop == ordertype))
		|| (IsCheck(IDC_CHECK_TJ) ? TRUE : FALSE);
}
//委托价格 触发价格 enable 
void COFInputDlg::EnablePrice(int ID, int IDAdd, int IDFenzi, int IDChu, int IDFenmu, int IDSpin, BOOL bEanble)
{
	EnableWindow(GetDlgItem(m_hDlg, ID), bEanble);
	EnableWindow(GetDlgItem(m_hDlg, IDAdd), bEanble);
	EnableWindow(GetDlgItem(m_hDlg, IDFenzi), bEanble);
	EnableWindow(GetDlgItem(m_hDlg, IDChu), bEanble);
	EnableWindow(GetDlgItem(m_hDlg, IDFenmu), bEanble);
	EnableWindow(GetDlgItem(m_hDlg, IDSpin), bEanble);
}
//初始化跟盘价check
void COFInputDlg::IniAutoPriceCheck()
{
	//跟盘是否可用
	int check = IsShowBuySellBtn() ? SW_SHOW:SW_HIDE;
	ShowWindow(GetDlgItem(m_hDlg, IDC_CHECK_AutoPrice), check);
	ShowWindow(GetDlgItem(m_hDlg, IDC_STATIC_OrderPrice), !check);

	if (check)//显示跟盘
	{
		//跟盘价 可用否
		BOOL bEnable = TRUE;
		char ordertype = GetOrderTypeEx();
		if (IsShowBuySellBtn() && (ordertype == otMarket || ordertype == otMarketStop))
			bEnable = FALSE;
		if (!bEnable)
			m_SwitchFollow.SetSwitchLevel(0);
		
		EnableWindow(GetDlgItem(m_hDlg, IDC_CHECK_AutoPrice), bEnable);

		EnablePrice(IDC_EDIT_OrderPrice, IDC_Sta_OrderPrice_Add, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_ChuHao, IDC_Sta_OrderPrice_FenMu, IDC_SPIN_OrderPrice, bEnable&&OrderPriceIsCanAble());
	}
	else
		EnablePrice(IDC_EDIT_OrderPrice, IDC_Sta_OrderPrice_Add, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_ChuHao, IDC_Sta_OrderPrice_FenMu, IDC_SPIN_OrderPrice, OrderPriceIsCanAble());
	
	//跟盘状态
	SetBuySellBtnStyle(m_SwitchFollow.GetSwitchLevel() == SwitchLevel2);
	
	//触发价格可用
	EnablePrice(IDC_EDIT_TrigPrice, IDC_Sta_TriggerPrice_Add, IDC_EDITTriggerPRICE_FenZi, IDC_Sta_TriggerPrice_ChuHao, IDC_Sta_TriggerPrice_FenMu, IDC_SPIN3,TriggerPriceIsCanAble());
}
//设置按钮风格 是只显示买入卖出 还是有价格
void COFInputDlg::SetBuySellBtnStyle(bool bShowPrice)
{
	m_BuyBtn.SetShowPrice(bShowPrice);
	m_SellBtn.SetShowPrice(bShowPrice);
}
//重绘子窗口
void COFInputDlg::ReInvalidateCtrl()
{
	HWND hWnd = GetTopWindow(m_hDlg);
	while (hWnd != NULL)
	{
		InvalidateRect(hWnd, NULL, TRUE);
		hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
	}
	//InvalidateRect(hWnd, NULL, TRUE);
}
//获取控件高度
int  COFInputDlg::FindCtrlHeight(int id)
{
	for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
	{
		C_OrderField &of = m_InputCfgArray.at(i);
		if (id == of.id)
			return of.height;
	}
	return -1;
}
//获取对话框大小
SIZE COFInputDlg::GetKbDlgSize()
{
	SIZE  sz;
	int tempHeight = 0, tempHeigh2 = 0;
	//check的高度
	tempHeight = FindCtrlHeight(IAF_TJCHECK);
	if (-1 != tempHeight&&m_vecUserNo.size() > 1 && !CConvert::IsTraderLogined() && IsShowTJ())
		tempHeight = tempHeight * 5;
	//静态文本高度、编辑框高度、提示信息高度
	RECT tempRect;
	GetWindowRect(GetDlgItem(m_hDlg, IDC_STATIC_Code), &tempRect);
	tempHeigh2 = tempRect.bottom - tempRect.top;
	int iEditHeight = FindCtrlHeight(IAF_COMMODITY);
	tempHeigh2 += iEditHeight;
	tempHeigh2 += m_OrderFont.iVerCtrlInterval2;
	GetWindowRect(GetDlgItem(m_hDlg, IDC_STATIC_Tip), &tempRect);
	tempHeigh2 += tempRect.bottom - tempRect.top;
	sz.cy = tempHeigh2 > tempHeight ? tempHeigh2 : tempHeight;
	return sz;
}
//转化屏幕坐标
void COFInputDlg::ScreenToClient(RECT&rt)
{
	POINT pt;
	pt.x = rt.left;
	pt.y = rt.top;
	::ScreenToClient(m_hDlg, &pt);
	rt.right = rt.right - rt.left + pt.x;
	rt.bottom = rt.bottom - rt.top + pt.y;
	rt.left = pt.x;
	rt.top = pt.y;
}
//判断是内外盘
void COFInputDlg::LoadVersionType()
{
	//登陆交易后 取遍历交易所信息 0==m_iVersionType表示内盘 1==m_iVersionType表示外盘 2==m_iVersionType表示内外盘 
	//修改
	TDataContainer tUsers;
	g_pTradeData->GetAllUser(tUsers, "");
	uint count = tUsers.size();
	bool bInner = false, bFore = false;
	for (uint i = 0; i < count; i++)
	{
		const TUserInfo *pUser = (const TUserInfo *)tUsers.at(i);
		if (!(pUser->pLogin && (bYes == pUser->pLogin->Logined)))	continue;//判断是否登录
		if (!strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_FOREIGNTRADEAPI_NAME).c_str()) || !strcmp(pUser->pLogin->LoginClassName, LoadRC::unicode_to_ansi(PLUG_DIPPERTRADEAPI_NAME).c_str()))//外盘 默认不显示开平
			bFore = true;
		else
			bInner = true;	
	}
	if (bInner)
		m_iVersionType = 0;
	if (bFore)
		m_iVersionType = 1;
	if (bInner&&bFore)
		m_iVersionType = 2;
}
//设置分数报价和普通报价滚轮滚动增减
void COFInputDlg::SetSpinCtrlBuddy(int IDSpin, int ID, int IDFenZi, int IDFenMu)
{
	SendDlgItemMessage(m_hDlg, IDSpin, UDM_SETRANGE32, 0, MAKELPARAM(10000000, -10000000));
	SendDlgItemMessage(m_hDlg, IDSpin, UDM_SETPOS, 0, 0);
	if (m_CommodityInf.bIfFractionPrice)//分数报价
	{
		wchar_t nTemp[10] = { 0 };
		swprintf_s(nTemp, sizeof(nTemp) / sizeof(nTemp[0]), L"%d", m_CommodityInf.iPrecision);
		SetWindowTextW(GetDlgItem(m_hDlg, IDFenMu), nTemp);
	}
}
//设置价格框的自动选中
void COFInputDlg::SetPriceSel()
{
	//设置当前合约代码的价格
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	GetContractStr(str);
	string strDirect;
	strDirect = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Direct));
	
	SContractNoType contractId;
	contractId[0] = '\0';
	SQuoteSnapShot QuoteData;
	memset(&QuoteData, 0, sizeof(SQuoteSnapShot));
	CConvert::StrToQContractNoType(str.c_str(), contractId);
	if (GetQuotInfo(str.c_str(), QuoteData))//有效合约
	{
		double Price = 0;
		if (m_KeyBoardSet.OutPriceType != fopm_Nodefault && 
			GetQuotInfo(str.c_str(), QuoteData, m_KeyBoardSet.OutPriceType, m_KeyBoardSet.OutPriceTick, CTradeConst::StrToDirect(LoadRC::ansi_to_unicode(strDirect).c_str()), Price, Price))
			SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, Price);
	}
}
//所有编辑框spin的mousewheel，上下键，spinclick消息的处理
BOOL COFInputDlg::DoWndWheel(int nID, BOOL bUP)
{
	switch (nID)
	{
	case IDC_EDIT_QtyMulti:
		UpdateNumEdit(bUP, IDC_EDIT_QtyMulti, m_fastOrderInfo.dChangePoint, false, true);
		break;
	case IDC_EDIT_OrderQty:
		UpdateNumEdit(bUP, IDC_EDIT_OrderQty, 1, false);
		break;
	case IDC_EDIT_QtyCount:
		UpdateNumEdit(bUP, IDC_EDIT_QtyCount, 1, false);
		break;
	case IDC_EDIT_IncreQty:
		UpdateNumEdit(bUP, IDC_EDIT_IncreQty, 1, true);
		break;
	case IDC_EDIT_MaxQty:
		UpdateNumEdit(bUP, IDC_EDIT_MaxQty, 1, false);
		break;
	case IDC_EDIT_MinQty:
		UpdateNumEdit(bUP, IDC_EDIT_MinQty, 1, false);
		break;
	case IDC_EDIT_OrderPrice:
	case IDC_EDITORDERPRICE_FenZi:
		SpinUpOrDown(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, bUP);
		break;
	case IDC_EDIT_TriggerPrice:
	case IDC_EDITTriggerPRICE_FenZi:
		SpinUpOrDown(IDC_EDIT_TriggerPrice, IDC_EDITTriggerPRICE_FenZi, bUP);
		break;
	case IDC_SPIN_IncrePrice:
	case IDC_EDIT_IncrePrice:
		SpinUpOrDown(IDC_SPIN_IncrePrice, IDC_EDIT_IncrePrice, bUP);
		break;
	case IDC_EDIT_Direct:
		SwitchDirect();
		break;
	case IDC_EDIT_Offset:
		SwitchOffset();
		break;
	case IDC_EDIT_TrigCondi:
		SwitchTJ();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
//按键
BOOL COFInputDlg::DoWndKeyDown(HWND hWnd, WPARAM w)
{
	//处理WM_CHAR
	DWORD key(w);
	if (key >= 97 && key <= 122)
		key -= 32;
	if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_Direct))//买卖框
	{
		wchar_t wdirect[10] = { 0 };	
		if (key == m_KeyBoardSet.ucBuy&&m_KeyBoardSet.ucBuy!=0)
			CTradeConst::DirectToStr(dBuy, wdirect, 10);
		else if (key == m_KeyBoardSet.ucSell&&m_KeyBoardSet.ucSell != 0)
			CTradeConst::DirectToStr(dSell, wdirect, 10);
		else
			return TRUE;
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), wdirect);
		return TRUE;
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_Offset))//开平框
	{
		wchar_t woffset[10] = { 0 };
		if (key == m_KeyBoardSet.ucOpen&&m_KeyBoardSet.ucOpen != 0)
			CTradeConst::OffsetToStr(oOpen, woffset,10);
		else if (key == m_KeyBoardSet.ucCover&&m_KeyBoardSet.ucCover != 0)
			CTradeConst::OffsetToStr(oCover, woffset, 10);
		else if (key == m_KeyBoardSet.ucCoverT&&m_KeyBoardSet.ucCoverT != 0)
			CTradeConst::OffsetToStr(oCoverT, woffset, 10);
		else
			return TRUE;
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Offset), woffset);
		return TRUE;
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi))//条件框
	{
		wchar_t wGL[10] = { 0 };
		if (key == KeyGreater)
			CTradeConst::TriggerConditionToStr(tcGreaterEqual, wGL,10);
		else if (key == KeyLesser)
			CTradeConst::TriggerConditionToStr(tcLessEqual, wGL, 10);
		else
			return TRUE;
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), wGL);
		return TRUE;
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_COMBO_OrderType))
	{
		char preType = GetOrderType();
		wchar_t wtype[15] = { 0 };
		
		if (key == KeyLimit)
			CTradeConst::OrderTypeToStr(otLimit, wtype, 15);
		else if (key == KeyMarket)
			CTradeConst::OrderTypeToStr(otMarket, wtype, 15);
		else if (key == KeyLimitStop)
			CTradeConst::OrderTypeToStr(otLimitStop, wtype, 15);
		else if (key == KeyStop)
			CTradeConst::OrderTypeToStr(otMarketStop, wtype, 15);
		else if (CConvert::ReadVersionType() == 1 || CConvert::ReadVersionType() == 2)
		{
			if (key == KeyIceberg)
				CTradeConst::OrderTypeToStr(otIceberg, wtype, 15);
			else if (key == KeyGhost)
				CTradeConst::OrderTypeToStr(otGhost, wtype, 15);
		}
		else
			return TRUE;
		SendMessage(hWnd, CB_SELECTSTRING, -1, (LPARAM)(LPWSTR)wtype);
		char curType = GetOrderType();
		if (preType != curType)
			Layout();

		return TRUE;
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_COMBO_TirgMode))
	{
		if (key == KeyTrigLast)
			SendMessage(hWnd, CB_SELECTSTRING, -1, (LPARAM)(LPWSTR)LoadResString(IDS_STRING_TJTriggerModeLast).c_str());
		else if (key == KeyTrigBuy)
			SendMessage(hWnd, CB_SELECTSTRING, -1, (LPARAM)(LPWSTR)LoadResString(IDS_STRING_TJTriggerModeBuy).c_str());
		else if (key == KeyTrigSell)
			SendMessage(hWnd, CB_SELECTSTRING, -1, (LPARAM)(LPWSTR)LoadResString(IDS_STRING_TJTriggerModeSell).c_str());
		return TRUE;
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_OrderQty))
	{
		if (!m_KeyBoardSet.bUpDownEqualTab)
		{
			if (key == VK_UP)
				UpdateNumEdit(TRUE, IDC_EDIT_OrderQty, 1, false);
			else if (key == VK_DOWN)
				UpdateNumEdit(FALSE, IDC_EDIT_OrderQty, 1, false);
		}	
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice))
	{
		if (!m_KeyBoardSet.bUpDownEqualTab)
		{
			if (key == VK_UP)
				SpinUpOrDown(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, TRUE);
			else if (key == VK_DOWN)
				SpinUpOrDown(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, FALSE);
		}
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_TrigPrice))
	{
		if (!m_KeyBoardSet.bUpDownEqualTab)
		{
			if (key == VK_UP)
				SpinUpOrDown(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, TRUE);
			else if (key == VK_DOWN)
				SpinUpOrDown(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, FALSE);
		}
	}
	return FALSE;
}
//bDoubleType是否是带小数 像委托数量就不能带小数  数量倍率可以带小数
void COFInputDlg::UpdateNumEdit(BOOL bUp, int ID, double iChangePoint, bool bCanNat, bool bDoubleType)
{
	wchar_t str[20];
	GetWindowTextW(GetDlgItem(m_hDlg, ID), str,20);

	double value = _wtof(str);
	if (bUp)//按了向上按钮//倍率
	{
		switch (ID)
		{
		case IDC_EDIT_OrderQty:
		case IDC_EDIT_MaxQty:
		case IDC_EDIT_MinQty:
			if (value < KQty_Max6)
				value += iChangePoint;
			break;
		case IDC_EDIT_QtyMulti:
		case IDC_EDIT_QtyCount:
		case IDC_EDIT_IncreQty:
			if (value < KQty_Max4)
				value += iChangePoint;
			break;
		}	
	}
	else//按了向下按钮
		value -= iChangePoint;
	if (value < KQty_Min && !bCanNat) value = KQty_Min;
	if (bDoubleType)
		swprintf_s(str, L"%0.*lf", 1, value);
	else
		swprintf_s(str, L"%d", (int)value);
	SetWindowTextW(GetDlgItem(m_hDlg, ID), str);
}
//针对价格
void COFInputDlg::SpinUpOrDown(int ID, int IDFenzi, BOOL bUp)
{
	wchar_t strTemp[20] = {0};
	wchar_t nTemp[20] = { 0 };
	HWND hwnd = NULL;
	if (m_CommodityInf.bIfFractionPrice)
		hwnd = GetDlgItem(m_hDlg, IDFenzi);
	else
		hwnd = GetDlgItem(m_hDlg, ID);
	GetWindowTextW(hwnd, strTemp,20);
	double InputPrice = _wtof(strTemp);
	if (m_CommodityInf.bIfFractionPrice)
	{
		if (bUp)                                //Up Spin
		{
			int TempMinCPrice = (int)(InputPrice + (int)m_CommodityInf.dMiniChangePrice);//
			GetWindowTextW(GetDlgItem(m_hDlg, ID), strTemp,20);
			if ((TempMinCPrice / m_CommodityInf.iPrecision) >= 1)//判断是否分子已经大于分母了
			{
				int IntTempPrice = TempMinCPrice / m_CommodityInf.iPrecision;
				IntTempPrice = IntTempPrice + _wtoi(strTemp);
				TempMinCPrice = TempMinCPrice%m_CommodityInf.iPrecision;
				swprintf_s(nTemp, sizeof(nTemp) / sizeof(nTemp[0]), L"%d", IntTempPrice);
				SetWindowTextW(GetDlgItem(m_hDlg, ID), nTemp);
			}
			if (wcslen(strTemp)==0)
				SetWindowTextW(GetDlgItem(m_hDlg, ID), L"0");
			swprintf_s(nTemp, sizeof(nTemp) / sizeof(nTemp[0]), L"%d", TempMinCPrice);
			SetWindowTextW(hwnd, nTemp);
		}
		else                               //Down Spin
		{
			int TempMinCPrice = (int)(InputPrice - (int)m_CommodityInf.dMiniChangePrice);//
			if (TempMinCPrice < 0)
			{
				GetWindowTextW(GetDlgItem(m_hDlg, ID), strTemp,20);
				int IntPrice = _wtoi(strTemp);
				if (0 == IntPrice)
					TempMinCPrice = 0;
				else if (IntPrice>0)
				{
					IntPrice--;
					swprintf_s(nTemp, sizeof(nTemp) / sizeof(nTemp[0]), L"%d", IntPrice);
					SetWindowTextW(GetDlgItem(m_hDlg, ID), nTemp);
					TempMinCPrice = m_CommodityInf.iPrecision + TempMinCPrice;
					swprintf_s(nTemp, sizeof(nTemp) / sizeof(nTemp[0]), L"%d", TempMinCPrice);
					SetWindowTextW(hwnd, nTemp);
				}
			}
			else
			{
				GetWindowTextW(GetDlgItem(m_hDlg, ID), strTemp,20);
				if (wcslen(strTemp) == 0)
					SetWindowTextW(GetDlgItem(m_hDlg, ID), L"");
				swprintf_s(nTemp, sizeof(nTemp) / sizeof(nTemp[0]), L"%d", TempMinCPrice);
				SetWindowTextW(hwnd, nTemp);
			}
		}
	}
	else//小数报价
	{
		CGeneralGlobal::IfEffectivelyPrice(m_CommodityInf, InputPrice);       //输入值四舍五入后调整到最小变动价的倍数
		double TempMinCPrice = 0.0;
		if (bUp)									//Up Spin
			TempMinCPrice = InputPrice + m_CommodityInf.dMiniChangePrice;//atof("100.1234")
		else									//Down Spin
			TempMinCPrice = InputPrice - m_CommodityInf.dMiniChangePrice;//atof("100.1234")
		if ((0 > TempMinCPrice) && (false == m_CommodityInf.bCanNegative))	TempMinCPrice = 0;
		SetWindowTextW(hwnd,LoadRC::ansi_to_unicode(FormatDoubleValue(m_CommodityInf.iPrecision, TempMinCPrice, FALSE)).c_str());
	}
}
//针对价格增量
void COFInputDlg::SpinUpOrDown(int IDIncre, BOOL bUp)
{
	wchar_t strTemp[20] = {0};
	wchar_t nTemp[20] = { 0 };
	HWND hwnd = GetDlgItem(m_hDlg, IDIncre);
	GetWindowTextW(hwnd, strTemp,20);
	double InputPrice = _wtof(strTemp);

	if (m_CommodityInf.bIfFractionPrice)
	{
		if (bUp)                                //Up Spin
		{
			int TempMinCPrice = (int)(InputPrice + (int)m_CommodityInf.dMiniChangePrice);//
			if (abs((TempMinCPrice / m_CommodityInf.iPrecision)) >= 1)//判断是否分子已经大于分母了
				TempMinCPrice = 240;
			if (wcslen(strTemp))
				SetWindowTextW(GetDlgItem(m_hDlg, IDIncre), L"0");
			swprintf_s(nTemp, sizeof(nTemp) / sizeof(nTemp[0]), L"%d", TempMinCPrice);
			SetWindowTextW(hwnd, nTemp);
		}
		else                               //Down Spin
		{
			int TempMinCPrice = (int)(InputPrice - (int)m_CommodityInf.dMiniChangePrice);//
			if (abs((TempMinCPrice / m_CommodityInf.iPrecision)) >= 1)
				TempMinCPrice = -240;
			swprintf_s(nTemp, sizeof(nTemp) / sizeof(nTemp[0]), L"%d", TempMinCPrice);
			SetWindowTextW(hwnd, nTemp);
		}
	}
	else//小数报价
	{
		CGeneralGlobal::IfEffectivelyPrice(m_CommodityInf, InputPrice);       //输入值四舍五入后调整到最小变动价的倍数
		double TempMinCPrice = 0.0;
		if (bUp)									//Up Spin
			TempMinCPrice = InputPrice + m_CommodityInf.dMiniChangePrice;//atof("100.1234")
		else									//Down Spin
			TempMinCPrice = InputPrice - m_CommodityInf.dMiniChangePrice;//atof("100.1234")
		SetWindowTextW(hwnd, LoadRC::ansi_to_unicode(FormatDoubleValue(m_CommodityInf.iPrecision, TempMinCPrice, FALSE)).c_str());
	}
}
//格式化double
string COFInputDlg::FormatDoubleValue(int precision, double value, BOOL bZeroEmpty)
{
	char str[16] = { 0 };
	if (value == 0 && bZeroEmpty)
		str[0] = '\0';
	else
	{
		if (precision < 0)
			precision = 0;
		char fmt[16] = { 0 };
		sprintf_s(fmt, "%%.%df", precision);
		sprintf_s(str, fmt, value);
	}
	return str;
}
//格式化整形
string COFInputDlg::FormatIntValue(int value, BOOL bZeroEmpty)
{
	char str[16] = { 0 };
	if (value == 0 && bZeroEmpty)
		str[0] = '\0';
	else
		sprintf_s(str, "%d", value);
	return str;
}
//是否显示 触发价格
bool COFInputDlg::IsShowTrigPrice()
{
	//选中了条件选项 或者订单类型为止损或者限价止损
	char cOrderType = GetOrderTypeEx();
	return  (IsCheck(IDC_CHECK_TJ)&&IsShowTJ()) || (IsShowComboType() && (cOrderType == otLimitStop || cOrderType == otMarketStop));
}

//初始化控件顺序和ID
void COFInputDlg::InitItemPos()
{
	m_vecItemPos.clear();
	int j = 0;
	ItemPos itPos;
	for (int i = 0; i < (int)m_InputCfgArray.size(); i++)
	{
		C_OrderField &of = m_InputCfgArray.at(i);
		if (!of.bShow || of.id == IAF_TJCHECK || of.id == IAF_SELECODE)
			continue;
		switch (of.id)
		{
			case IAF_ACCOUNT://客户账号
				itPos.iPosIndex = j;
				itPos.hID = IDC_ComUserNo;
				break;
			case IAF_COMMODITY://合约代码
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_Code;
				break;
			case IAF_DIRECT://买卖方向
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_Direct;
				break;
			case IAF_OFFSET://开平
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_Offset;
				break;
			case IAF_PRICE://委托价格
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_OrderPrice;
				break;
			case IAF_VOL://委托数量
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_OrderQty;
				break;
			case IAF_TRIGGERMODE://触发方式
				itPos.iPosIndex = j;
				itPos.hID = IDC_COMBO_TirgMode;
			break;
			case IAF_VALIDTIME://有效日期
				itPos.iPosIndex = j;
				itPos.hID = IDC_DATETIMEPICKER_ValidData;
				break;
			case IAF_TRIGGERCONDITION://条件
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_TrigCondi;
				break;
			case IAF_TRIGGERPRICE://触发价格
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_TrigPrice;
				break;
			case IAF_VOLBL://数量倍率
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_QtyMulti;
				break;
			case IAF_INCREVOL://数量增量
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_IncreQty;
				break;
			case IAF_INCREPRICE://价格增量
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_IncrePrice;
				break;
			case IAF_VOLCOUNT://下单次数
				itPos.iPosIndex = j;
				itPos.hID = IDC_EDIT_QtyCount;
				break;
			case IAF_ORDERTYPE://定单类型
				itPos.iPosIndex = j;
				itPos.hID = IDC_COMBO_OrderType;
			break;
			case IAF_VALIDTYPE://有效类型
				itPos.iPosIndex = j;
				itPos.hID = IDC_COMBO_ValidType;
			break;
		}
		m_vecItemPos.push_back(itPos);
		j++;
	}
	//最后添加上按钮
	if (m_KeyBoardSet.uiDoOrderStyle!=1)
	{
		itPos.iPosIndex = j;
		itPos.hID = IDC_BUTTON_DoOrder;
		m_vecItemPos.push_back(itPos);
	}
	else
	{
		itPos.iPosIndex = j;
		itPos.hID = IDC_Btn_Buy;
		m_vecItemPos.push_back(itPos);
		itPos.iPosIndex = j+1;
		itPos.hID = IDC_Btn_Sell;
		m_vecItemPos.push_back(itPos);
	}
	
}
//设置索引焦点
void COFInputDlg::SetIndexFocus(int index, bool btabling, bool bAdd)
{
	int count = (int)m_vecItemPos.size();
	if (-1 == index || count <= index)
		return;
	HWND hwnd = GetDlgItem(m_hDlg, m_vecItemPos[index].hID);
	m_bTabling = btabling;
	int nNext = index;
	int nCount = 0;
	while (!IsWindowEnabled(hwnd))
	{
		nCount++;
		if (nCount > (int)m_vecItemPos.size())
			return;

		if (bAdd)
		{
			nNext++;
			if (count <= nNext)
				nNext = 0;
		}	
		else
		{
			nNext--;
			if (-1 == nNext)
				nNext = m_vecItemPos.size() - 1;
		}
		if (-1 == index || count <= index)
			return;
		
		hwnd = GetDlgItem(m_hDlg, m_vecItemPos[nNext].hID);

	}
	SetFocus(hwnd);
	m_bTabling = false;
}
//获取上一个焦点位置索引
int COFInputDlg::GetLastFocusIndex(int ID)
{
	for (vecItemPos::size_type i = m_vecItemPos.size() - 1; i >= 0; i--)
	{
		if (m_vecItemPos[i].hID == ID)
		{
			if (0 == i)
				return m_vecItemPos[m_vecItemPos.size() - 1].iPosIndex;
			else
				return m_vecItemPos[i - 1].iPosIndex;
		}
	}
	return -1;
}
//获取下一个焦点位置索引
int COFInputDlg::GetNextFocusIndex(int ID)
{
	for (vecItemPos::size_type i = 0; i < m_vecItemPos.size(); i++)
	{
		if (m_vecItemPos[i].hID == ID)
		{
			if (m_vecItemPos.size() - 1 == i)
				return 0;
			else
				return m_vecItemPos[i + 1].iPosIndex;
		}
	}
	return -1;
}
//子类化edit
void COFInputDlg::InitCtrl()
{
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_CHECK_PK), LoadResString(IDS_CHECK_PK).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_CHECK_KS), LoadResString(IDS_CHECK_KS).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_CHECK_TJ), LoadResString(IDS_CHECK_TJ).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_Check_BatchOrder), LoadResString(IDS_Check_BatchOrder).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_Check_BatchCount), LoadResString(IDS_Check_BatchCount).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_CHECK_T1), LoadResString(IDS_CHECK_T1).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_CHECK_ForCover), LoadResString(IDS_CHECK_ForCover).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_UserNo), LoadResString(IDS_STATIC_UserNo).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), LoadResString(IDS_Static_LoginNo).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_Code), LoadResString(IDS_STATIC_Code).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_Direct), LoadResString(IDS_STATIC_Direct).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_Offset), LoadResString(IDS_STATIC_Offset).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_OrderPrice), LoadResString(IDS_STATIC_OrderPrice).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_OrderQty), LoadResString(IDS_STATIC_OrderQty).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_QtyMulti), LoadResString(IDS_STATIC_QtyMulti).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_TrigMode), LoadResString(IDS_STATIC_TrigMode).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_TrigCondi), LoadResString(IDS_STATIC_TrigCondi).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_TrigPrice), LoadResString(IDS_STATIC_TrigPrice).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_OrderType), LoadResString(IDS_STATIC_OrderType).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_ValidType), LoadResString(IDS_STATIC_ValidType).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_ValidData), LoadResString(IDS_STATIC_ValidData).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_QtyCount), LoadResString(IDS_STATIC_QtyCount).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_IncreQty), LoadResString(IDS_STATIC_IncreQty).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_IncrePrice), LoadResString(IDS_STATIC_IncrePrice).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder), LoadResString(IDS_BUTTON_DoOrder).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder), LoadResString(IDS_BUTTON_PayOrder).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_Btn_Seq), LoadResString(IDS_Btn_Seq).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder), LoadResString(IDS_BUTTON_AutoOrder).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_StaticBuyPos), LoadResString(IDS_StaticBuyPos).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_StaticSellPos), LoadResString(IDS_StaticSellPos).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_MaxQty), LoadResString(IDS_STATIC_MaxQty).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_MinQty), LoadResString(IDS_STATIC_MinQty).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_CHECK_AutoPrice), LoadResString(IDS_CHECK_AutoPrice).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_Btn_Buy), LoadResString(IDS_Btn_Buy).c_str());
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_Btn_Sell), LoadResString(IDS_Btn_Sell).c_str());

	Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti), _T("1"));
	//初始化控件对象
	m_LockUserNoBtn.Init(GetDlgItem(m_hDlg, IDC_Btn_LockUserNo));
	m_LockUserNoBtn.SetToolTips(LoadResString(IDS_LockTip).c_str());
	m_LockCodeBtn.Init(GetDlgItem(m_hDlg, IDC_Btn_LockCode));
	m_UserNO.InitComboBox(GetDlgItem(m_hDlg, IDC_ComUserNo));

	m_CodeEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_Code));//合约代码
	m_DirectEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_Offset));//买卖
	m_OffsetEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_Direct));//开平
	m_OrderPriceEdit.InitEdit(GetDlgItem(m_hDlg, IDC_EDIT_OrderPrice));//委托价格
	m_OrderPriceEditFZ.Init(GetDlgItem(m_hDlg, IDC_EDITORDERPRICE_FenZi));//分子

	m_OrderQtyEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty));//委托数量
	m_QtyMultiEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti));//数量倍率	
	m_TrigCondiEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi));//条件
	m_TriggerPriceEdit.InitEdit(GetDlgItem(m_hDlg, IDC_EDIT_TrigPrice));////触发价格
	m_TriggerPriceEditFZ.Init(GetDlgItem(m_hDlg, IDC_EDITTriggerPRICE_FenZi));//分子

	m_MaxQtyEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_MaxQty));//最大量
	m_MinQtyEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_MinQty));//最小量
	m_QtyCountEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_QtyCount));//下单次数
	m_IncreQtyEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_IncreQty));//数量增量
	m_IncrePriceEdit.Init(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice));//价格增量

	//子类化combo droplist和dropdown不同  droplist下edit和combo 句柄相同
	m_TrigModeCmb.InitCombo(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode));
	m_TrigModeCmb.ReadOnly(true);

	m_OrderButton.Init(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder));//子类化按钮
	m_PayOrderButton.Init(GetDlgItem(m_hDlg, IDC_BUTTON_PayOrder));
	m_AutoOrderButton.Init(GetDlgItem(m_hDlg, IDC_BUTTON_AutoOrder));
	m_SeqButton.Init(GetDlgItem(m_hDlg, IDC_Btn_Seq));
	m_SeleCodeButton.Init(GetDlgItem(m_hDlg, IDC_Btn_SelCode));
	m_OrderButton.SetButtonText(LoadResString(IDS_STR_DoOrder));
	m_PayOrderButton.SetButtonText(LoadResString(IDS_STR_PayOrder));
	m_AutoOrderButton.SetButtonText(LoadResString(IDS_STR_AutoOrder));
	m_SeqButton.SetButtonText(LoadResString(IDS_STR_Seq));
	m_SeleCodeButton.SetButtonText(TEXT("..."));

	m_BuyPositionStatic.Init(GetDlgItem(m_hDlg, IDC_StaticBuyPos));
	m_SellPositionStatic.Init(GetDlgItem(m_hDlg, IDC_StaticSellPos));

	m_OrderTypeCmb.InitCombo(GetDlgItem(m_hDlg, IDC_COMBO_OrderType),true);
	m_OrderTypeCmb.ReadOnly(true);

	m_ValidTypeCmb.InitCombo(GetDlgItem(m_hDlg, IDC_COMBO_ValidType));
	m_ValidTypeCmb.ReadOnly(true);

	m_BuyBtn.InitButton(GetDlgItem(m_hDlg, IDC_Btn_Buy));
	m_SellBtn.InitButton(GetDlgItem(m_hDlg,IDC_Btn_Sell));

	//spin
	m_OrderPriceSpin.Init(GetDlgItem(m_hDlg, IDC_SPIN_OrderPrice));
	m_OrderQtySpin.Init(GetDlgItem(m_hDlg, IDC_SPIN_OrderQty));
	m_TrigPriceSpin.Init(GetDlgItem(m_hDlg,IDC_SPIN_TrigPrice ));
	m_MaxQtySpin.Init(GetDlgItem(m_hDlg, IDC_SPIN_MaxQty));
	m_MinQtySpin.Init(GetDlgItem(m_hDlg, IDC_SPIN_MinQty));
	m_QtyMultiSpin.Init(GetDlgItem(m_hDlg, IDC_SPIN_QtyMulti));
	m_QtyCountSpin.Init(GetDlgItem(m_hDlg, IDC_SPIN_QtyCount));
	m_IncreQtySpin.Init(GetDlgItem(m_hDlg, IDC_SPIN_IncreQty));
	m_IncrePriceSpin.Init(GetDlgItem(m_hDlg, IDC_SPIN_IncrePrice));
	//日期控件
	m_ValidDataCtrl.Init(GetDlgItem(m_hDlg, IDC_DATETIMEPICKER_ValidData));

	//check
	m_CheckTJ.InitBtn(GetDlgItem(m_hDlg, IDC_CHECK_TJ));
	m_CheckPK.InitBtn(GetDlgItem(m_hDlg, IDC_CHECK_PK));
	m_CheckKS.InitBtn(GetDlgItem(m_hDlg, IDC_CHECK_KS));
	m_CheckPC.InitBtn(GetDlgItem(m_hDlg, IDC_Check_BatchCount));
	m_CheckPL.InitBtn(GetDlgItem(m_hDlg, IDC_Check_BatchOrder));
	m_CheckT1.InitBtn(GetDlgItem(m_hDlg, IDC_CHECK_T1));
	m_CheckQP.InitBtn(GetDlgItem(m_hDlg, IDC_CHECK_ForCover));
	
	m_SwitchFollow.InitButton(GetDlgItem(m_hDlg, IDC_CHECK_AutoPrice), LoadResString(IDS_STR_STATIC_OrderPrice).c_str(), LoadResString(IDS_FollowPrice).c_str(), LoadResString(IDS_OrderFollowSwitch).c_str());
	//static
	m_StaticCodeName.InitStatic(GetDlgItem(GetHwnd(), IDC_STATIC_Code));
	m_StaticCodeName.SetStaticText(LoadResString(IDS_STRING_ChangeCode).c_str());
	m_StaticCanOpenCover.InitStatic(GetDlgItem(GetHwnd(), IDC_STATIC_CanOpenCover));
	m_StaticDeposit.InitStatic(GetDlgItem(GetHwnd(), IDC_STATIC_Deposit));
	
}
//是价格edit吗
bool COFInputDlg::IsPriceEdit(HWND hwnd)
{
	return hwnd && (GetDlgCtrlID(hwnd) == IDC_EDIT_OrderPrice ||
		GetDlgCtrlID(hwnd) == IDC_EDIT_TrigPrice);
}
//处理双击
BOOL COFInputDlg::DoWndDbClick(HWND hWnd, LPARAM lparam)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lparam);
	pt.y = GET_Y_LPARAM(lparam);
	RECT rect;
	if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_Direct))//买卖框
	{
		//在几种值之间循环
		GetClientRect(GetDlgItem(m_hDlg, IDC_EDIT_Direct), &rect);
		if (PtInRect(&rect, pt))
			return SwitchDirect();
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_Offset))//开平框
	{
		//在几种值之间循环
		GetClientRect(GetDlgItem(m_hDlg, IDC_EDIT_Offset), &rect);
		if (PtInRect(&rect, pt))
			return SwitchOffset();
	}
	else if (hWnd == GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi))//条件框
	{
		//在几种值之间循环
		GetClientRect(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), &rect);
		if (PtInRect(&rect, pt))
			return SwitchTJ();
	}
	return FALSE;
}
//切换买卖方向
BOOL COFInputDlg::SwitchDirect()
{
	wchar_t str[10] = {0};
	GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), str, 10);
	if (CTradeConst::StrToDirect(str) == dBuy)
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), LoadResString(IDS_Direct_Sell).c_str());
	else
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), LoadResString(IDS_Direct_Buy).c_str());
	return TRUE;
}
//切换开平
BOOL COFInputDlg::SwitchOffset()
{
	wchar_t str[10] = { 0 };
	GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Offset), str,10);
	if (str == LoadResString(IDS_Offset_Open))
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Offset), LoadResString(IDS_Offset_CoverT).c_str());
	else if (str == LoadResString(IDS_Offset_CoverT))
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Offset), LoadResString(IDS_Offset_Cover).c_str());
	else
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Offset), LoadResString(IDS_Offset_Open).c_str());
	return TRUE;
}
//切换条件
BOOL COFInputDlg::SwitchTJ()
{
	wchar_t str[10] = { 0 };
	GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), str,10);
	if (!wcscmp(L">=", str))
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), L"<=");
	else
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), L">=");
	return TRUE;
}
//获取中间位置
int  COFInputDlg::GetMiddlePos(int ID)
{
	wstring str;
	str = GetWindowTextUnicode(GetDlgItem(m_hDlg, ID));
	int index = str.find('/');
	str = str.substr(0, index);
	HDC hdc = ::GetDC(GetDlgItem(m_hDlg, ID));
	SIZE sz;
	::GetTextExtentPoint32(hdc, str.c_str(), (int)str.length(), &sz);
	::ReleaseDC(GetDlgItem(m_hDlg, ID), hdc);
	return sz.cx;
}

//用户未登录时，scode不能转换正常，在这里处理一下
void COFInputDlg::DealUserNotLoginSwitch(string &scode)
{
	vector<string> vecContent;
	int count = m_vecUpDownSwitchContract.size();

	for (int i=0; i< count;i++)
	{
		string strTemp = m_vecUpDownSwitchContract[i];
		ModifyContractStr(strTemp);
		if (strTemp == scode)
		{
			scode = m_vecUpDownSwitchContract[i];
			break;
		}
	}	
}

string COFInputDlg::GetOneSwitchContract(bool bNext)
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	string transStr = str;
	GetContractStr(transStr);
	if (transStr == str) //如果GetContractStr没有转换
	{
		DealUserNotLoginSwitch(transStr);
	}

	int findIndex = -1;
	int count = m_vecUpDownSwitchContract.size();
	for (int i=0; i<count;i++)
	{
		if (transStr == m_vecUpDownSwitchContract[i])
		{
			findIndex = i;
			break;
		}
	}

	if (findIndex != -1)//找到了
	{
		if (bNext)//下一个
		{
			if (findIndex + 1 < count)//有下一个
			{
				return m_vecUpDownSwitchContract[findIndex + 1];
			}
			else//没有下一个
			{
				return m_vecUpDownSwitchContract[0];
			}
		}
		else
		{
			if (findIndex != 0)//有前一个
			{
				return m_vecUpDownSwitchContract[findIndex - 1];
			}
			else//没有前一个
			{
				return m_vecUpDownSwitchContract[count - 1];
			}
		}
	}

	if (count) //没有找到说明不再切换合约内，那么返回第一个
	{
		return m_vecUpDownSwitchContract[0];
	}
	return "";

}

bool COFInputDlg::OnKeyUpDown(bool isDown, HWND hFocus)
{
	//上下键等于Tab键
	if (m_KeyBoardSet.bUpDownEqualTab)
	{
		SetIndexFocus(isDown ? GetNextFocusIndex(GetDlgCtrlID(hFocus)) : 
			GetLastFocusIndex(GetDlgCtrlID(hFocus)), true, isDown);
		return true;
	}
	else 
	{  //上下键不等于Tab键
		if (hFocus == GetDlgItem(m_hDlg, IDC_EDIT_Code))
		{
			string sCode = GetOneSwitchContract(isDown);
			if (sCode.length())
			{
				SetContractNo(sCode);
				return true;
			}
		}
		else if (hFocus == GetDlgItem(m_hDlg, IDC_EDIT_Direct))
		{
			SwitchDirect();
			return true;
		}
		else if (hFocus == GetDlgItem(m_hDlg, IDC_EDIT_Offset))
		{
			SwitchOffset();
			return true;
		}

	}
	return false;
}

//处理上下键、左右键等于tab键
bool COFInputDlg::DoLeftRightUpDownKey(char key, bool Combo)
{	
	HWND hFocus = GetFocus();
	if (Combo)
		hFocus = ::GetParent(hFocus);

	switch (key)
	{
	case VK_LEFT:
		if (m_KeyBoardSet.bRightLeftEqualTab)
		{
			SetIndexFocus(GetLastFocusIndex(GetDlgCtrlID(hFocus)), true, false);
		}
		return true;
	case VK_RIGHT:
		if (m_KeyBoardSet.bRightLeftEqualTab)
		{
			SetIndexFocus(GetNextFocusIndex(GetDlgCtrlID(hFocus)), true);
		}
		return true;
	case VK_UP:
		return OnKeyUpDown(false,hFocus);
	case VK_DOWN:
		return OnKeyUpDown(true, hFocus);
	}

	return false;
}
//处理enter
BOOL COFInputDlg::DisposeEnterMsg(DWORD key)
{
	if (key == VK_RETURN)
	{
		HWND hWnd = GetFocus();
		int id = -1;
		if (hWnd == GetDlgItem(m_hDlg, IDC_Btn_Buy))
			id = IDC_Btn_Buy;
		else if (hWnd == GetDlgItem(m_hDlg, IDC_Btn_Sell))
			id = IDC_Btn_Sell;
		else if (hWnd == GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder))
			id = IDC_BUTTON_DoOrder;
		if (id != -1)
		{
			SendMessage(m_hDlg, WM_COMMAND, MAKEWPARAM(id, BN_CLICKED), 0);
			return TRUE;
		}
	}
	return FALSE;
}
//处理enter发单
bool COFInputDlg::DealEnterSendOrder(DWORD key)
{
	if (key == VK_RETURN&&m_KeyBoardSet.bEnterSendOrder&&IsWindowVisible(GetDlgItem(m_hDlg, IDC_BUTTON_DoOrder)))
	{
		if (CConvert::IsTraderLogined())//交易员无效
			return false;
		
		SendMessage(m_hDlg, WM_COMMAND, MAKEWPARAM(IDC_BUTTON_DoOrder, BN_CLICKED), 0);
		return true;
	}
	else
		return false;
}
//清空提示框
void COFInputDlg::ClearTipBar()
{
	//清空可开可平、保证金占用
	m_StaticCanOpenCover.SetWindowText(_T(""));
	m_StaticCanOpenCover.Invalidate();
	m_StaticDeposit.SetWindowText(_T(""));
	m_StaticDeposit.Invalidate();
}
//设置定单类型组框
void COFInputDlg::SetOrderTypeCmbAtrribute()
{
	//定单类型
	m_OrderTypeCmb.ResetContent();
	if (IsShowKS()&&IsCheck(IDC_CHECK_KS))
	{
		m_OrderTypeCmb.AddString(LoadResString(IDS_STRING_OrderTypeLimitKey).c_str());
		m_OrderTypeCmb.ReadOnly(true);
		m_OrderTypeCmb.SetCurSel(0);
		return;
	}

	m_OrderTypeCmb.AddString(LoadResString(IDS_STRING_OrderTypeLimitKey).c_str());//限价
	m_OrderTypeCmb.AddString(LoadResString(IDS_STRING_OrderTypeMarketKey).c_str());//市价
	if (!IsCheck(IDC_CHECK_TJ))
	{
		m_OrderTypeCmb.AddString(LoadResString(IDS_STRING_OrderTypeLStopKey).c_str());//限价止损 
		m_OrderTypeCmb.AddString(LoadResString(IDS_STRING_OrderTypeMStopKey).c_str());//止损 
		if (1 == VersionType() || 2 == VersionType())
		{
			m_OrderTypeCmb.AddString(LoadResString(IDS_STRING_OrderTypeIceKey).c_str());//外盘 冰山
			m_OrderTypeCmb.AddString(LoadResString(IDS_STRING_OrderTypeGhostKey).c_str());//影子
		}
	}
	
	m_OrderTypeCmb.ReadOnly(true);
	m_OrderTypeCmb.SetCurSel(0);
}
//设置有效类型类型组框
void COFInputDlg::SetValidTypeCmbAtrribute()
{
	//有效类型
	m_ValidTypeCmb.ResetContent();
	m_ValidTypeCmb.AddString(LoadResString(IDS_STR_ValidType_FOK), _T("FOK"));
	m_ValidTypeCmb.AddString(LoadResString(IDS_STR_ValidType_IOC), _T("IOC"));
	m_ValidTypeCmb.AddString(LoadResString(IDS_STR_ValidType_GFD), _T("GFD"));
	m_ValidTypeCmb.AddString(LoadResString(IDS_STR_ValidType_GTC), _T("GTC"));
	m_ValidTypeCmb.AddString(LoadResString(IDS_STR_ValidType_GTD), _T("GTD"));
	m_ValidTypeCmb.ReadOnly(true);
	m_ValidTypeCmb.SetSplitChar();
	m_ValidTypeCmb.AlighValue();
	m_ValidTypeCmb.SetCurSel(2);
}
//显示提示
void COFInputDlg::ShowTip(const wstring& msg)
{
	ShowWindow(GetDlgItem(m_hDlg, IDC_STATIC_Tip), SW_SHOW);
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_STATIC_Tip), msg.c_str());
}
//条件单状态
void COFInputDlg::UpdateTJState()
{
	wchar_t wstr[20] = {0};
	CTradeConst::TJTriggerModeToStr(m_FillOrderAssist.uiTrigMode, wstr, 20);
	//触发方式
	ComboBox_SelectString(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), -1, wstr);
	//触发条件  与买卖、触发方式相关  如果止损：买入(<=),卖出(>=)  止赢：买入(>=),卖出(<=)
	FillTConditionByDirect();
}
//填充条件状态根据配置
void COFInputDlg::FillTConditionByDirect()
{
	//触发条件  与买卖、触发方式相关  如果止损：买入(<=),卖出(>=)  止赢：买入(>=),卖出(<=)
	wchar_t strDirect[10] = {0};
	GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), strDirect,10);
	if ((tcStopLossMode == m_FillOrderAssist.uiTrigCondition&& strDirect == LoadResString(IDS_Direct_Buy)) ||
		(tcStopProfit == m_FillOrderAssist.uiTrigCondition&&strDirect == LoadResString(IDS_Direct_Sell)))
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), L">=");
	else if ((tcStopLossMode == m_FillOrderAssist.uiTrigCondition&& strDirect == LoadResString(IDS_Direct_Sell)) ||
		(tcStopProfit == m_FillOrderAssist.uiTrigCondition&&strDirect == LoadResString(IDS_Direct_Buy)))
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), L"<=");
	
}
//条件单触发价格更新
void COFInputDlg::UpdateTrigPriceFill(string sCode, char cDirect)
{	
	if (cDirect != dNone)
	{
		double price = 0;
		SQuoteSnapShot dc_QuoteData;
		memset(&dc_QuoteData, 0, sizeof(SQuoteSnapShot));
		bool bValid = GetQuotInfo(sCode.c_str(), dc_QuoteData, m_FillOrderAssist.uiTrigPriceMode, m_FillOrderAssist.uiTrigOverPoint, cDirect, price, price);
		//更新价格框的内容 委托价
		if (bValid)
			SetPriceToDlg(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, IDC_Sta_TriggerPrice_FenMu, price);
	}
}
//更新账户和sign
void COFInputDlg::UpdateLoginSign(const char *szUserNo, const char *sign)
{
	m_CurUserNo = szUserNo;
	if (strcmp(sign, ""))
	{
		m_CurLoginNo = sign;
		strncpy_s(m_CommodityInf.Sign, sign, sizeof(m_CommodityInf.Sign));
	}
}
//获取sign
void COFInputDlg::SetLoginSign(char(&Sign)[21])
{
	strncpy_s(Sign, GetLoginSign(), _TRUNCATE);
}
//获取sign
const char * COFInputDlg::GetLoginSign()
{
	if (!m_CurLoginNo.empty())
		return m_CurLoginNo.c_str();
	else
		return m_CommodityInf.Sign;
	return "";
}
//处理锁定按钮
void COFInputDlg::OnBnClickedBtnLockuserno()
{
	m_LockUserNoBtn.SetLockState(!m_LockUserNoBtn.GetLockState());
	EnableWindow(GetDlgItem(m_hDlg, IDC_ComUserNo), m_LockUserNoBtn.GetLockState() ? FALSE : TRUE);
}
//处理锁定按钮
void COFInputDlg::OnBnClickedBtnLockcode()
{
	m_LockCodeBtn.SetLockState(!m_LockCodeBtn.GetLockState());
	EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_Code), m_LockCodeBtn.GetLockState() ? FALSE : TRUE);
	EnableWindow(GetDlgItem(m_hDlg, IDC_Btn_SelCode), m_LockCodeBtn.GetLockState() ? FALSE : TRUE);
}
//处理行情回调
LRESULT	COFInputDlg::OnQuotInfo(WPARAM wparam, LPARAM lparam)
{
	string strCode;
	strCode = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	GetContractStr(strCode);
	SQuoteSnapShot tmpQuoteSnap;
	memset(&tmpQuoteSnap, 0, sizeof(SQuoteSnapShot));
	SContractNoType contract_no;
	CConvert::StrToQContractNoType(strCode.c_str(), contract_no);
	if (m_regQuotId != contract_no)
		return 0;

	//下面是跟随行情动态填充的部分
	if (IsShowBuySellBtn() && m_SwitchFollow.GetSwitchLevel() == SwitchLevel2)
	{
		//改单时买入卖出会显示出来 所以要排除改单的情况
		if (!GetModifyOrderOper())
		{
			double dBuyPrice = 0, dSellPrice = 0;
			GetQuotInfo(strCode.c_str(), tmpQuoteSnap, fopm_Follow, 0, dBuy, dBuyPrice, dBuyPrice);
			SetPriceToBtn(IDC_Btn_Buy, dBuyPrice);
			GetQuotInfo(strCode.c_str(), tmpQuoteSnap, fopm_Follow, 0, dSell, dSellPrice, dSellPrice);
			SetPriceToBtn(IDC_Btn_Sell, dSellPrice);
		}
	}
	//else if (m_KeyBoardSet.bPriceSynQuote)
	//{
	//	//改单时买入卖出会显示出来 所以要排除改单的情况
	//	if (IsShowBuySellBtn() && !GetModifyOrderOper())
	//	{
	//		double dBuyPrice = 0, dSellPrice = 0;
	//		GetQuotInfo(strCode.c_str(), tmpQuoteSnap, m_FillOrderAssist.uiDefaultPriceMode, m_FillOrderAssist.uiOverPoints, dBuy, dBuyPrice, dBuyPrice);
	//		SetPriceToBtn(IDC_Btn_Buy, dBuyPrice);
	//		GetQuotInfo(strCode.c_str(), tmpQuoteSnap, m_FillOrderAssist.uiDefaultPriceMode, m_FillOrderAssist.uiOverPoints, dSell, dSellPrice, dSellPrice);
	//		SetPriceToBtn(IDC_Btn_Sell, dSellPrice);	
	//	}
	//	else
	//	{
	//		double price = 0;
	//		GetQuotInfo(strCode.c_str(), tmpQuoteSnap, m_FillOrderAssist.uiDefaultPriceMode, m_FillOrderAssist.uiOverPoints, CTradeConst::StrToDirect(strDirect.c_str()), price, price);
	//		SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, price);
	//	}
	//}
	//条件单 触发价格
	/*if (IsCheck(IDC_CHECK_TJ) && m_FillOrderAssist.bTrigSynQuote)
	{
		wchar_t strDirect[10] = { 0 };
		GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), strDirect, 10);
		double price = 0;
		GetQuotInfo(strCode.c_str(), tmpQuoteSnap, m_FillOrderAssist.uiTrigPriceMode, m_FillOrderAssist.uiTrigOverPoint, CTradeConst::StrToDirect(strDirect), price, price);
		SetPriceToDlg(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, IDC_Sta_TriggerPrice_FenMu, price);
	}*/
	return 1;
}
//填充界面
void COFInputDlg::FillKeyBoardOrderDlg(const TSendOrder *pSendOrder, bool bFastOrder, bool bQuot, bool bTradeData, bool bModifyOrder)//填充界面
{
	//条件单
	if (pSendOrder->StrategyType == stCondition&& !IsCheck(IDC_CHECK_TJ))
		OnBnClickedCheckTJ();

	//改单
	if (bModifyOrder)
	{
		FillModifyOrder(*pSendOrder);
		return;
	}
}
// bool COFInputDlg::IsLME(TExchangeKey&ExchangeKey)
// {
// 	const SExchange*pExchange = (const SExchange*)g_pTradeData->GetExchange(&ExchangeKey);
// 	if (pExchange)
// 		return pExchange->CoverMode == cmUnfinish;
// 	else
// 	{
// 		string str(ExchangeKey.ExchangeNo);
// 		transform(str.begin(), str.end(), str.begin(), toupper);
// 		return !str.compare("LME");
// 	}
// 	return false;
// }
//填充改单
void COFInputDlg::FillModifyOrder(const TSendOrder &SendOrder)
{
	//账户
	if (SendOrder.UserNo[0] != '\0'&&!m_LockUserNoBtn.GetLockState())
	{
		ComboBox_SetText(GetDlgItem(m_hDlg, IDC_ComUserNo), LoadRC::ansi_to_unicode(SendOrder.UserNo).c_str());
		UpdateLoginSign(SendOrder.UserNo, SendOrder.Sign);
	}

	//填充合约 获取最小变动价等信息 必填
	string strCode;
	CConvert::TContractKeyToTContractNo(SendOrder, strCode);
	//UpdateShowFuzzyQuery(false);
	ModifyContractStr(strCode);
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Code), LoadRC::ansi_to_unicode(strCode).c_str());//此时会触发合约编辑框 文本更新消息
	//订单类型
	ModifyOrderType(SendOrder.OrderType);

	//有效类型
	ModifyValidType(SendOrder.ValidType);
	if (vtGTD == SendOrder.ValidType)
		ModifyValidType(SendOrder.ValidTime);

	//委托数量
	wchar_t strQty[21] = { 0 };
	swprintf_s(strQty, _T("%ld"), SendOrder.OrderQty);
	Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), strQty);

	//委托价格
	string strTemp;
	switch (SendOrder.OrderType)
	{
		case otLimit:
			SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, SendOrder.OrderPrice);
			break;
		case otLimitStop:
			SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, SendOrder.OrderPrice);
			SetPriceToDlg(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, IDC_Sta_TriggerPrice_FenMu, SendOrder.TriggerPrice);
			break;
		case otMarketStop:
			SetPriceToDlg(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, IDC_Sta_TriggerPrice_FenMu, SendOrder.TriggerPrice);
			break;
		case otIceberg:
		case otGhost:
			SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, SendOrder.OrderPrice);
			swprintf_s(strQty, _T("%ld"), SendOrder.MaxOrderQty);
			Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_MaxQty), strQty);
			swprintf_s(strQty, _T("%ld"), SendOrder.MinOrderQty);
			Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_MinQty), strQty);
			break;
		default:
			break;
	}

	//买卖 开平
	wchar_t wdirect[10] = { 0 };
	wchar_t woffset[10] = { 0 };
	CTradeConst::DirectToStr(SendOrder.Direct, wdirect, 10);
	CTradeConst::OffsetToStr(SendOrder.Offset, woffset, 10);
	Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Direct), wdirect);
	Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Offset), woffset);
}

//设置价格到界面
void COFInputDlg::SetPriceToDlg(int ID, int IDMolecules, int IDDenominator, double dInputPriceParma)
{
	wchar_t str[21] = { 0 };
	if (m_CommodityInf.bIfFractionPrice)
	{
		SFractionPrice temp;
		CGeneralGlobal::DecimalsToFractions(dInputPriceParma, m_CommodityInf.iPrecision, temp);
		swprintf_s(str, _T("%d"), temp.iInteger);
		Edit_SetText(GetDlgItem(m_hDlg, ID), str);
		swprintf_s(str, _T("%d"), temp.iMolecules);
		Edit_SetText(GetDlgItem(m_hDlg, IDMolecules), str);
		swprintf_s(str, _T("%d"), temp.idenominator);
		Edit_SetText(GetDlgItem(m_hDlg, IDDenominator), str);
	}
	else
	{
		swprintf_s(str, _T("%0.*lf"), m_CommodityInf.iPrecision, dInputPriceParma);
		Edit_SetText(GetDlgItem(m_hDlg, ID), str);
	}
}
//设置价格到买卖按钮上
void COFInputDlg::SetPriceToBtn(int IDBtn, double dPrice)
{
	wchar_t str[51] = { 0 };
	string sPrice = CConvert::FormatPriceToStr(dPrice, m_CommodityInf);
	swprintf_s(str, L"%s", LoadRC::ansi_to_unicode(sPrice).c_str());
	if (IDC_Btn_Buy == IDBtn)
		m_BuyBtn.SetPrice(str);
	else
		m_SellBtn.SetPrice(str);
}
//获取行情信息但并不订阅 取到就取到 取不到就算了
bool COFInputDlg::GetQuotInfo(const char *szCode, SQuoteSnapShot&QuotInfo)
{
	SContractNoType contract_no;
	CConvert::StrToQContractNoType(szCode, contract_no);
	if (!g_PlugLoad.GetStarApi())
		return false;
	return g_PlugLoad.GetStarApi()->GetSnapshot(contract_no, &QuotInfo);
}
//获取行情信息但并不订阅 取到就取到 取不到就算了 返回true表示获取成功 false 表示失败
bool COFInputDlg::GetQuotInfo(const char *szCode, SQuoteSnapShot &QuoteData, int iTypePrice, int nDefaultPriceOverStep, char Direct, double &dPrice, double dDefaultPrice)
{
	if (!GetQuotInfo(szCode, QuoteData))
		return false;
	if (0 == m_CommodityInf.iPrecision)
	{
		SOFCommodityInf tempCommodityInf;
		memset(&tempCommodityInf, 0, sizeof(SOFCommodityInf));
		if (CConvert::GetQuoteCommodityInf(szCode, tempCommodityInf))
			m_CommodityInf.iPrecision = tempCommodityInf.iPrecision;
	}

	TContractKey tmpContract;
	memset(&tmpContract, 0, sizeof(TContractKey));
	CConvert::TContractNoToTContractKey(szCode, tmpContract);

	//目前只要是非组合 价格为0就是无效 组合始终有效
	double dSellPrice = 0, dBuyPrice = 0;
	SQtyType nSellQty = 0, nBuyQty = 0;
	GetPriceQty(QuoteData, dBuyPrice, dSellPrice, nBuyQty, nSellQty);

	bool bValidPrice = false;
	double dOverStp = nDefaultPriceOverStep*CConvert::GetMinChangePrice(m_CommodityInf);
	switch (iTypePrice)
	{
	case fopm_Last:
			bValidPrice = IsValidPrice(tmpContract.CommodityType, QuoteData.Data[S_FID_LASTPRICE].FidAttr != S_FIDATTR_NONE ? QuoteData.Data[S_FID_LASTPRICE].Price:0);
			if (dBuy == Direct)
				dPrice = QuoteData.Data[S_FID_LASTPRICE].FidAttr != S_FIDATTR_NONE ? (QuoteData.Data[S_FID_LASTPRICE].Price + dOverStp):0;
			else if (dSell == Direct)
				dPrice = QuoteData.Data[S_FID_LASTPRICE].FidAttr != S_FIDATTR_NONE ? (QuoteData.Data[S_FID_LASTPRICE].Price - dOverStp):0;
			break;
	case fopm_Follow:
			if (dBuy == Direct)
			{
				bValidPrice = IsValidPrice(tmpContract.CommodityType, dSellPrice);
				dPrice = dSellPrice + dOverStp;
			}
			else if (dSell == Direct)
			{
				bValidPrice = IsValidPrice(tmpContract.CommodityType, dBuyPrice);
				dPrice = dBuyPrice - dOverStp;
			}
			break;
	case fopm_Nodefault:
			dPrice = dDefaultPrice;
			bValidPrice = true;
			break;
	case fopm_Work:
			if (dBuy == Direct)
			{
				bValidPrice = IsValidPrice(tmpContract.CommodityType, dBuyPrice);
				dPrice = dBuyPrice + dOverStp;
			}
			else if (dSell == Direct)
			{
				bValidPrice = IsValidPrice(tmpContract.CommodityType, dSellPrice);
				dPrice = dSellPrice - dOverStp;
			}
			break;
		default:
			break;
	}
	if (bValidPrice)
		return true;
	else
		dPrice = 0;
	return false;
}

//下单后取消强平
void COFInputDlg::CancelForceCover()
{
	if (IsShowForceCover() && m_CheckQP.GetCheck())
		m_CheckQP.SetCheck(BST_UNCHECKED);
}
//下单后定单类型改为限价 前提是定单类型显示出来
void COFInputDlg::ModifyOrderType(char OrderType)
{
	if (IsShowComboType())
	{
		int iPreIndex = m_OrderTypeCmb.GetCurSel();

		wchar_t wtype[10] = { 0 };
		CTradeConst::OrderTypeToStr(OrderType, wtype, 10);
		int iCurIndex = m_OrderTypeCmb.FindString(-1, wtype);
		if (iPreIndex != iCurIndex)
		{
			m_OrderTypeCmb.SetCurSel(iCurIndex);
			Layout();
		}
	}
}
//下单后定单类型改为限价 前提是定单类型显示出来
void COFInputDlg::ModifyValidType(const char *szTime)
{
	int nYear = 0, nMonth = 0, nDay = 0;
	sscanf_s(szTime, "%d-%d-%d", &nYear, &nMonth, &nDay);
	SYSTEMTIME systime;
	memset(&systime, 0, sizeof(SYSTEMTIME));
	systime.wYear = nYear;
	systime.wMonth = nMonth;
	systime.wDay = nDay;
	DateTime_SetSystemtime(GetDlgItem(m_hDlg, IDC_DATETIMEPICKER_ValidData), GDT_VALID,&systime);
}
//下单后有效类型改为当日有效 前提是有效类型显示出来
void COFInputDlg::ModifyValidType(char ValidType)
{
	if (IsShowComboType())
	{
		int iPreIndex = m_ValidTypeCmb.GetCurSel();
		wchar_t wtype[10] = { 0 };
		CTradeConst::ValidTypeToStr(ValidType, wtype, 10);
		int iCurIndex = m_ValidTypeCmb.FindString(-1, wtype);
		if (iPreIndex != iCurIndex)
		{
			m_ValidTypeCmb.SetCurSel(iCurIndex);
			Layout();
		}
	}
}
//下单后取消T1
void COFInputDlg::CancelT1()
{
	if (IsShowT1() && m_CheckT1.GetCheck())
		m_CheckT1.SetCheck(BST_UNCHECKED);
}
//清空价格
void COFInputDlg::EmptyPrice(int ID, int IDMolecules)
{
	SetWindowTextW(GetDlgItem(m_hDlg, ID), L"");
	if (m_CommodityInf.bIfFractionPrice)
		SetWindowTextW(GetDlgItem(m_hDlg, IDMolecules), L"");
}
//是否需要填充条件单
bool COFInputDlg::IsNeedFillTrigger()
{
	char cOrderType = GetOrderTypeEx();
	return  (!IsShowTJ() || (IsShowTJ() && !IsCheck(IDC_CHECK_TJ))) &&
		!IsShowBuySellBtn() &&
		(IsShowComboType() && (cOrderType == otLimitStop || cOrderType == otMarketStop));
}
//更新
void COFInputDlg::UpdateFillTriggerValid(const string &strCur)
{
	//与旧的相比
	string strLastCode;
	strLastCode = GetLastTriggerFillCode();
	if (strCur != strLastCode)
	{
		SetValidFillTrigger(false);
		SetLastTriggerFillCode(strCur);
	}
}

//获取账户索引
bool COFInputDlg::GetIndexUserNo(const vector<CStringsVec> &vecStr, const string &str, int &index)
{
	bool bhave = false;
	for (vector<CStringsVec>::size_type i = 0; i < vecStr.size(); i++)
	{
		if (0 == strcmp(vecStr[i][0].c_str(), str.c_str()))
		{
			UpdateLoginSign(vecStr[i][0].c_str(), vecStr[i][2].c_str());
			bhave = true;
			index = (int)i;
			break;
		}
	}
	if (!bhave)	return false;
	return true;
}
//填充账户组框
void COFInputDlg::SetUser(bool bValidData)
{
	m_UserNO.ResetContent();
	
	if (!bValidData)
	{
		m_vecUserNo.clear();
		CConvert::GetAllUserNo(m_vecUserNo);
		//排序
		sort(m_vecUserNo.begin(), m_vecUserNo.end(), UserNoOrder());

		m_nInitReady = m_vecUserNo.size() > 0;
	}
	
	if (!IsCheck(IDC_Check_BatchOrder))
	{
		if (m_vecUserNo.size() > 1)
		{
			sUserInfo user;
			m_UserNO.AddRow(user);
		}
		for (vector<CStringsVec>::size_type i = 0; i < m_vecUserNo.size(); i++)
		{
			sUserInfo user;
			memcpy(user.UserNo, m_vecUserNo[i][0].c_str(), sizeof(user.UserNo));
			memcpy(user.UserName, m_vecUserNo[i][1].c_str(), sizeof(user.UserName));
			m_UserNO.AddRow(user);
		}
		if (m_vecUserNo.size() == 1 && ::IsWindow(m_UserNO.GetHwnd()))
		{
			ComboBox_SetText(GetDlgItem(m_hDlg, IDC_ComUserNo), LoadRC::ansi_to_unicode(m_vecUserNo[0].at(0)).c_str());
			UpdateLoginSign(m_vecUserNo[0][0].c_str(), m_vecUserNo[0][2].c_str());
		}
		else if (m_vecUserNo.size()>0)//
			UpdateLoginSign(m_vecUserNo[0][0].c_str(), m_vecUserNo[0][2].c_str());

	}
	else
	{
		if (m_batchGroupVec.empty())
			return;
		for (map<string, BatchGroupInfo>::iterator iter = m_batchGroupVec.begin(); iter != m_batchGroupVec.end(); iter++)
		{
			sUserInfo user;
			memcpy(user.UserNo, iter->second.BatchGroupNo, sizeof(iter->second.BatchGroupNo));
			memcpy(user.UserName, iter->second.BatchGroupName, sizeof(iter->second.BatchGroupName));
			m_UserNO.AddRow(user);
		}
		if (m_batchGroupVec.size() == 1 && ::IsWindow(m_UserNO.GetHwnd()))
		{
			map<string, BatchGroupInfo>::iterator it = m_batchGroupVec.begin();
			SetWindowTextW(GetDlgItem(m_hDlg, IDC_Static_LoginNo), LoadRC::ansi_to_unicode(it->second.BatchGroupName).c_str());
			for (uint i = 0; i < m_vecUserNo.size();i++)
			{
				if (m_vecUserNo[i][0] == it->second.BaseUser)
				{
					UpdateLoginSign(m_vecUserNo[0][0].c_str(), m_vecUserNo[0][2].c_str());
					break;
				}	
			}
			ComboBox_SetText(GetDlgItem(m_hDlg, IDC_ComUserNo), LoadRC::ansi_to_unicode(it->second.BatchGroupNo).c_str());
		}
	}
}

//有效的价格
bool COFInputDlg::IsValidPrice(char commodityType, double price)
{
	return (!CConvert::IsCombContract(commodityType) && price == 0) ? false : true;
}
//检查合约有效性
void COFInputDlg::CheckCodeInput()
{
	TContractKey ContractKey;
	memset(&ContractKey, 0, sizeof(TContractKey));
	if (!GetKeyCode(ContractKey, FALSE))
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Code), _T(""));
	OnEnChangeEditCode();
}
//iType=0代表显示合约持仓，1代表账户持仓
void COFInputDlg::UpdatePositionInf()
{
	if (!m_FillOrderAssist.bShowPosition)
	{
		if (IsWindowVisible(m_BuyPositionStatic.GetHwnd()))
		{
			ShowWindow(m_BuyPositionStatic.GetHwnd(), SW_HIDE);
			ShowWindow(m_SellPositionStatic.GetHwnd(), SW_HIDE);
		}
		return;
	}
	else
	{
		if (!IsWindowVisible(m_BuyPositionStatic.GetHwnd()))
		{
			ShowWindow(m_BuyPositionStatic.GetHwnd(), SW_SHOW);
			ShowWindow(m_SellPositionStatic.GetHwnd(), SW_SHOW);
		}
	}
	uint LongCount = 0, ShortCount = 0, DayLongCount = 0, DayShortCount = 0;
	if (m_FillOrderAssist.uiShowPosType == fosp_Single)//根据此刷新持仓信息
	{
		//具体合约判断
		TContractKey ContractKey;
		memset(&ContractKey, 0, sizeof(TContractKey));
		strncpy_s(ContractKey.Sign, m_CurLoginNo.c_str(), _TRUNCATE);
		if (!GetKeyCode(ContractKey, false))
		{
			m_BuyPositionStatic.UpdateText(0, LoadResString(IDS_STR_LongPosition), m_OrderFont.lgPositionFont, g_ColorRefData.GetColorBackground(), m_OrderFont.rgbBuy);
			m_SellPositionStatic.UpdateText(0, LoadResString(IDS_STR_ShortPosition), m_OrderFont.lgPositionFont, g_ColorRefData.GetColorBackground(), m_OrderFont.rgbSell);
			return;
		}
		//资金账号判断
		if (!m_CurUserNo.empty() && CConvert::IsValidTradeCommodity(ContractKey))
			CConvert::GetPositionVol(m_CurUserNo.c_str(), &ContractKey, LongCount, ShortCount, DayLongCount, DayShortCount);
	}
	else
		CConvert::GetPositionVol(NULL, NULL, LongCount, ShortCount, DayLongCount, DayShortCount);

	m_BuyPositionStatic.UpdateText(LongCount, LoadResString(IDS_STR_LongPosition), m_OrderFont.lgPositionFont, g_ColorRefData.GetColorBackground(), m_OrderFont.rgbBuy);
	m_SellPositionStatic.UpdateText(ShortCount, LoadResString(IDS_STR_ShortPosition), m_OrderFont.lgPositionFont, g_ColorRefData.GetColorBackground(), m_OrderFont.rgbSell);
}

//获取倍率
double COFInputDlg::GetMultiQty()
{
	wchar_t cc[16] = { 0 };
	Edit_GetText(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti), cc, sizeof(cc) / sizeof(cc[0]));
	return _wtof(cc);
}
//获取参数信息
bool COFInputDlg::GetDlgCtrlInput(TSendOrder &SendOrder, bool bShowTip, bool bShowMsgBox,
	bool bFastOrder, char key, const char * ContractID)
{
	//更新sign
	memcpy_s(SendOrder.Sign, sizeof(SendOrder.Sign), m_CurLoginNo.c_str(), sizeof(SendOrder.Sign));
	SendOrder.OrderWay = owHorz;

	//有效类型 有效日期
	SendOrder.ValidType = GetValidType();
	string str = GetValidTime();
	if (!str.empty())
		strcpy_s(SendOrder.ValidTime, str.c_str());//有效日期

	//风险报单 TBool
	SendOrder.IsRiskOrder = (IsShowForceCover() && IsCheck(IDC_CHECK_ForCover)) ? bYes : bNo;

	//T+1
	SendOrder.AddOneIsValid = (IsShowT1() && IsCheck(IDC_CHECK_T1)) ? tsNight : tsDay;

	//用户账号
	if (!GetCtrlUser(SendOrder.UserNo, bShowTip, bShowMsgBox))	return false;

	//获取商品信息
	if (!GetKeyCode(SendOrder, bShowTip, bShowMsgBox))
	{
		SetFocus(GetDlgItem(m_hDlg, IDC_EDIT_Code));
		return false;
	}

	//获取定单类型 肯定不会是空
	SendOrder.OrderType = GetOrderType();

	//委托价格
	if (SendOrder.Direct != dNone && IsShowBuySellBtn() && m_SwitchFollow.GetSwitchLevel() == SwitchLevel2)//跟盘
	{
		SQuoteSnapShot QuoteData;
		memset(&QuoteData, 0, sizeof(SQuoteSnapShot));
		string strCode;
		CConvert::TContractKeyToTContractNo(SendOrder, strCode);
		double dPrice = 0;
		if (false == GetQuotInfo(strCode.c_str(), QuoteData, fopm_Follow, 0, SendOrder.Direct, dPrice, dPrice))
			return false;
		SendOrder.OrderPrice = dPrice;
	}
	//else if (SendOrder.Direct != dNone && !IsShowBuySellBtn()&&m_KeyBoardSet.bPriceSynQuote)//仅确定 委托价格同步刷新
	//{
	//	SQuoteSnapShot QuoteData;
	//	memset(&QuoteData, 0, sizeof(SQuoteSnapShot));
	//	string strCode;
	//	CConvert::TContractKeyToTContractNo(SendOrder, strCode);
	//	double dPrice = 0;
	//	if (false == GetQuotInfo(strCode.c_str(), QuoteData, m_FillOrderAssist.uiDefaultPriceMode, m_FillOrderAssist.uiOverPoints, SendOrder.Direct, dPrice, dPrice))
	//		return false;
	//	SendOrder.OrderPrice = dPrice;
	//}
	else
	{
		if (otLimitStop == SendOrder.OrderType || otLimit == SendOrder.OrderType || otIceberg == SendOrder.OrderType || otGhost == SendOrder.OrderType)
		{
			if (!GetCtrlPrice(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, SendOrder.OrderPrice, bShowMsgBox))
				return false;
		}
	}

	//触发价格
	if (otLimitStop == SendOrder.OrderType || otMarketStop == SendOrder.OrderType)
	{
		if (!GetCtrlPrice(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, SendOrder.TriggerPrice, bShowMsgBox, true, fptTrig))
			return false;
	}

	//委托数量
	if (!GetCtrlOrderNum(SendOrder.OrderQty, bShowMsgBox))	return false;

	//最大量 最小量
	if (otIceberg == SendOrder.OrderType)
	{
		if (!GetCtrlMaxNum(SendOrder.MaxOrderQty, bShowMsgBox))	return false;
		if (!GetCtrlMinNum(SendOrder.MinOrderQty, bShowMsgBox))	return false;
		//检查数据有效性，委托数量大于等于最大两，最大量大于等于最小量
		if (!((SendOrder.MinOrderQty <= SendOrder.MaxOrderQty) && (SendOrder.MaxOrderQty <= SendOrder.OrderQty)))
		{
			wchar_t strError[51] = { 0 };
			if (SendOrder.MaxOrderQty > SendOrder.OrderQty)
				swprintf_s(strError, LoadResString(IDS_STR_IceMaxTip).c_str());
			if (SendOrder.MinOrderQty > SendOrder.MaxOrderQty)
				swprintf_s(strError, LoadResString(IDS_STR_IceMinTip).c_str());
			if (bShowMsgBox)
			{
				MessageBox(m_hDlg, strError, LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				return false;
			}
		}
	}

	//买卖方向  有买卖按钮时是通过参数传进来 不需要获取
	if (SendOrder.Direct == dNone )
	{
		if (!GetDirect(SendOrder.Direct, bShowMsgBox))
			return false;
	}

	//开平仓  
	if (!GetOffset(SendOrder.Offset, bShowMsgBox))			return false;

	//条件
	if (IsCheck(IDC_CHECK_TJ) && IsShowTJ() && !GetTJParam(SendOrder, bShowTip, bShowMsgBox)) return false;
	return true;
}

//得到用户并判断有效性 合法返回ture  bShowTip是否显示提示信息 为ture时是弹出提示还是在界面上显示
bool COFInputDlg::GetCtrlUser(char(&UserNo)[21], bool bShowTip, bool bShowMsgBox)
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_ComUserNo));
	int index = -1;
	if (str.empty())
	{
		if (bShowTip)
		{
			if (!IsCheck(IDC_Check_BatchOrder))
			{
				if (bShowMsgBox)
					MessageBox(m_hDlg, LoadResString(IDS_STR_InvalidUserNo).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				else
					g_FastError = LoadResString(IDS_STR_InvalidUserNo);
			}
			else
			{
				if (bShowMsgBox)
					MessageBox(m_hDlg, LoadResString(IDS_STR_InvalidGroupNo).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				else
					g_FastError = LoadResString(IDS_STR_InvalidGroupNo);
			}
		}
		return false;
	}
	else
	{
		if (!IsCheck(IDC_Check_BatchOrder))
		{
			const TUserInfo *pInfo = g_pTradeData->GetUser(str.c_str(), m_CurLoginNo.c_str());
			//判断该账户是否登陆过
			if (pInfo && pInfo->pLogin)
			{
				if (pInfo->pLogin->Logined == bNo)//登录不成功
				{
					wchar_t str[51] = { 0 };
					swprintf_s(str,LoadResString(IDS_STR_NoLoginUser).c_str(), pInfo->LoginUser);
					if (bShowTip)
					{
						if (bShowMsgBox)	MessageBox(m_hDlg, str, LoadResString(IDS_STRING_Error).c_str(), MB_OK);
						else			g_FastError = str;
					}
					return false;
				}
				strncpy_s(UserNo, str.c_str(), _TRUNCATE);
				return true;
			}
			if (!GetIndexUserNo(m_vecUserNo, str, index))
			{
				if (bShowTip)
				{
					if (bShowMsgBox)	MessageBox(m_hDlg, LoadResString(IDS_STR_IllUserNo).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
					else				g_FastError = LoadResString(IDS_STR_IllUserNo);
				}
				return false;
			}
		}
		else//批量
		{
			map<string, BatchGroupInfo>::iterator iter = m_batchGroupVec.find(str);
			if (iter == m_batchGroupVec.end())
			{
				if (bShowTip)
				{
					if (bShowMsgBox)	MessageBox(m_hDlg, LoadResString(IDS_STR_IllGroupNo).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
					else			g_FastError = LoadResString(IDS_STR_IllGroupNo);
				}
				return false;
			}
			strncpy_s(UserNo, str.c_str(), _TRUNCATE);
		}
	}
	return true;
}
//SContractKey输出
bool COFInputDlg::GetKeyCode(TContractKey &key, bool bShowTip, bool bShowMsgBox)//SContractKey
{
	string strCode;
	strCode = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	GetContractStr(strCode);

	SetLoginSign(key.Sign);
	if (!CConvert::TContractNoToTContractKey(strCode.c_str(), key) || !CConvert::IsValidTradeCommodity(key))
	{
		if (bShowTip)
		{
			if (bShowMsgBox)	MessageBox(m_hDlg, LoadResString(IDS_STR_InvalidCode).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
			else			g_FastError = LoadResString(IDS_STR_InvalidCode);
		}
		return false;
	}
	return true;
}
//获取定单类型 
char COFInputDlg::GetOrderType(char *pType)
{
	if (!IsShowComboType())
	{
		char cOrderType = otLimit;
		if (pType)	*pType = cOrderType;
		return cOrderType;
	}
	else
	{
		wchar_t wstr[10] = {0};
		GetWindowTextW(m_OrderTypeCmb.GetHwnd(), wstr, 10);
		char cOrderType = CTradeConst::StrToOrderType(wstr);
		if (0 == cOrderType)	cOrderType = otLimit;
		if (pType)	*pType = cOrderType;
		return cOrderType;
	}
	return otLimit;
}
//获取定单类型 针对selchange
char COFInputDlg::GetOrderTypeEx()
{
	int sel = m_OrderTypeCmb.GetCurSel();
	if (CB_ERR == sel)
		return GetOrderType();
	wstring str;
	m_OrderTypeCmb.GetLBText(sel, str);
	return CTradeConst::StrToOrderType(str.c_str());
}
//获取有效类型 
char COFInputDlg::GetValidType()
{
	if (IsShowComboType())
	{
		wchar_t wstr[10] = { 0 };
		GetWindowTextW(m_ValidTypeCmb.GetHwnd(), wstr, 10);
		return CTradeConst::StrToValidType(wstr);
	}
	return vtGFD;
}
//获取有效类型 针对selchange
char COFInputDlg::GetValidTypeEx()
{
	int sel = m_ValidTypeCmb.GetCurSel();
	if (CB_ERR == sel)
		return GetValidType();
	wstring str;
	str = m_ValidTypeCmb.GetName(sel);
	return CTradeConst::StrToValidType(str.c_str());
}
//获取有效日期
string COFInputDlg::GetValidTime()
{
	wchar_t str[51] = { 0 };
	if (IsShowComboType() && vtGTD == GetValidType())
	{
		SYSTEMTIME sysTime;
		if (GDT_VALID == ::SendMessage(GetDlgItem(m_hDlg, IDC_DATETIMEPICKER_ValidData), DTM_GETSYSTEMTIME, 0, (LPARAM)&sysTime))
			swprintf_s(str, TEXT("%04d-%02d-%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	}
	return LoadRC::unicode_to_ansi(str);
}
//获取买卖方向
bool COFInputDlg::GetDirect(char &Direct, bool bShowMsgBox, bool bSetFocus)
{
	wchar_t str[10] = {0};
	GetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Direct), str,10);
	if (LoadResString(IDS_Direct_Buy) == str)		Direct = dBuy;
	else if (LoadResString(IDS_Direct_Sell) == str)	Direct = dSell;
	else
	{
		if (bShowMsgBox)	MessageBox(m_hDlg, LoadResString(IDS_STR_InvalidDirect).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
		if (bSetFocus)		SetFocus(GetDlgItem(m_hDlg,IDC_EDIT_Direct));
		return false;
	}
	return true;
}
//获取快速下单价格
bool COFInputDlg::GetCtrlPrice(int id, int idMolecular, double &outPrice, bool bShowMsgBox, bool bSetFocus, FPriceType type)
{
	string strInteger, strMolecular;
	strInteger = GetWindowTextAnsic(GetDlgItem(m_hDlg, id));
	string strText, strTitle;
	if (strInteger.empty())
	{
		if (bShowMsgBox)
		{
			switch (type)
			{
			case fptTrig:
				MessageBox(m_hDlg, LoadResString(IDS_InvalidTrigPrice).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				break;
			default:
				MessageBox(m_hDlg, LoadResString(IDS_InvalidOrderPrice).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				break;
			}
		}
		if (bSetFocus)			SetFocus(GetDlgItem(m_hDlg, id));
		return false;
	}
	if (m_CommodityInf.bIfFractionPrice)//分数
	{
		strMolecular = GetWindowTextAnsic(GetDlgItem(m_hDlg, idMolecular));
		if (strMolecular.empty())
		{
			if (bShowMsgBox)	
			{
				switch (type)
				{
				case fptTrig:
					MessageBox(m_hDlg, LoadResString(IDS_InvalidTrigPrice).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
					break;
				default:
					MessageBox(m_hDlg, LoadResString(IDS_InvalidOrderPrice).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
					break;
				}
			}
			if (bSetFocus)	SetFocus(GetDlgItem(m_hDlg, idMolecular)); 
			return false;
		}
	}
	//判断是否是合法的价格
	if (m_CommodityInf.bIfFractionPrice)
	{
		if (!CGeneralGlobal::IfEffectivelyInput(m_CommodityInf, atof(strMolecular.c_str())))
		{
			if (bShowMsgBox == false)	return false;
			switch (type)
			{
			case fptTrig:
				MessageBox(m_hDlg, (LoadResString(IDS_InvalidTrigPrice) + L":" + LoadResString(IDS_STR_InvalidMolecularPrice)).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				break;
			default:
				MessageBox(m_hDlg, (LoadResString(IDS_InvalidOrderPrice) + L":" + LoadResString(IDS_STR_InvalidMolecularPrice)).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				break;
			}		
			if (bSetFocus)	SetFocus(GetDlgItem(m_hDlg, idMolecular));
			return false;
		}
		outPrice = atof(strMolecular.c_str()) / m_CommodityInf.iPrecision + atoi(strInteger.c_str());
	}
	else
	{
		if (!CGeneralGlobal::IfEffectivelyInput(m_CommodityInf, atof(strInteger.c_str())))
		{
			if (bShowMsgBox == false)	return false;
			switch (type)
			{
			case fptTrig:
				MessageBox(m_hDlg, (LoadResString(IDS_InvalidTrigPrice) + L":" + LoadResString(IDS_STR_NoMiniChangePrice)).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				break;
			default:
				MessageBox(m_hDlg, (LoadResString(IDS_InvalidOrderPrice) + L":" + LoadResString(IDS_STR_NoMiniChangePrice)).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
				break;
			}
			if (bSetFocus)	SetFocus(GetDlgItem(m_hDlg, id));
			return false;
		}
		outPrice = atof(strInteger.c_str());
	}
	return true;
}
//获取条件单参数
bool COFInputDlg::GetTJParam(TSendOrder &SendOrder, bool bShowTip, bool bShowMsgBox, bool bSetFocus)
{
	SendOrder.StrategyType = stCondition;

	if (!CConvert::IsInnerDiskExchange(SendOrder.ExchangeNo))
	{
		if(bShowMsgBox) MessageBox(m_hDlg, LoadResString(IDS_InnerSupport).c_str(), LoadResString(IDS_STRING_Error).c_str(), NULL);
		return false;
	}
	wstring str;
	//条件
	str = GetWindowTextUnicode(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode) );
	SendOrder.TriggerMode = CTradeConst::StrToTriggerMode(str.c_str());

	//触发方式
	str = GetWindowTextUnicode(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi) );
	if (str.empty())
	{
		if (bShowTip)	g_FastError ==  LoadResString(IDS_STR_TirgCondiEmpty);
		if (bShowMsgBox)	MessageBox(m_hDlg, LoadResString(IDS_STR_TirgCondiEmpty).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
		if (bSetFocus)	SetFocus(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi));
		return false;
	}
	SendOrder.TriggerCondition = CTradeConst::StrToTriggerCondition(str.c_str());

	//触发价格
	if (!GetCtrlPrice(IDC_EDIT_TrigPrice, IDC_EDITTriggerPRICE_FenZi, SendOrder.TriggerPrice, bShowMsgBox, bSetFocus, fptTrig)) return false;
	return true;
}
//获取批次次数
bool COFInputDlg::GetPCQtyCount(uint &QtyCount, bool bShowMsgBox, bool bSetFocus)//获取批次次数
{
	string strQtyCount;
	strQtyCount = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_QtyCount));
	if (strQtyCount.empty())
	{
		if (bShowMsgBox)		MessageBox(m_hDlg, LoadResString(IDS_STR_OrderQtyInvalid).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
		if (bSetFocus)
			SetFocus(GetDlgItem(m_hDlg, IDC_EDIT_QtyCount));
		return false;
	}
	QtyCount = (uint)atoi(strQtyCount.c_str());
	return true;
}
//获取批次数量增量
bool COFInputDlg::GetPCQtyIncre(int &QtyIncre, bool bShowMsgBox, bool bSetFocus)//获取批次数量增量
{
	string strQtyIncre;
	strQtyIncre = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_IncreQty));
	QtyIncre = (uint)atoi(strQtyIncre.c_str());
	return true;
}
//获取批次价格增量
bool COFInputDlg::GetPCPriceIncre(double &PriceIncre, bool bShowMsgBox, bool bSetFocus)//获取批次价格增量
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice));
	if (m_CommodityInf.bIfFractionPrice)//分数
		PriceIncre = atof(str.c_str()) / m_CommodityInf.iPrecision;
	else
	{
		if (!CGeneralGlobal::IfEffectivelyInput(m_CommodityInf, atof(str.c_str())))
		{
			if (bShowMsgBox == false)	return false;
			MessageBox(m_hDlg, LoadResString(IDS_STR_PriceIncreInvalid).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
			if (bSetFocus)	 SetFocus(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice));
			return false;
		}
		PriceIncre = atof(str.c_str());
	}
	return true;
}
//获取开平
bool COFInputDlg::GetOffset(char &Offset, bool bShowMsgBox, bool bSetFoucus)
{
	if (cmNone == m_CommodityInf.cCoverMode || cmUnfinish == m_CommodityInf.cCoverMode)//不区分开平
	{
		Offset = oNone;
		return true;
	}
	wstring str;
	str = GetWindowTextUnicode(GetDlgItem(m_hDlg, IDC_EDIT_Offset));
	if (str == LoadResString(IDS_Offset_Open))			Offset = oOpen;
	else if (str == LoadResString(IDS_Offset_CoverT))	Offset = oCoverT;
	else if (str == LoadResString(IDS_Offset_Cover))	Offset = oCover;
	else
	{
		if (bShowMsgBox)		MessageBox(m_hDlg, LoadResString(IDS_STR_InvalidOffset).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
		if (bSetFoucus)			SetFocus(GetDlgItem(m_hDlg, IDC_EDIT_Offset));
		return false;
	}
	return true;
}
//获取数量
bool COFInputDlg::GetCtrlOrderNum(uint & Qty, bool bShowMsgBox, bool bSetFocus)
{
	string strQty, strRatio;
	strQty = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty));
	strRatio = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti));
	int iQtyTemp = atoi(strQty.c_str());
	if (strQty.empty() || iQtyTemp == 0)
	{
		if (bShowMsgBox)		MessageBox(m_hDlg, LoadResString(IDS_STR_InvalidOrderQty).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
		if (bSetFocus)
			SetFocus(GetDlgItem(m_hDlg,IDC_EDIT_OrderQty));
		return false;
	}
	double dRatioTemp = atof(strRatio.c_str());
	if (IsCheck(IDC_CHECK_KS)&&IsShowKS())												//应该乘以倍率
		Qty = (unsigned long)MyRound(iQtyTemp*dRatioTemp, 0);
	else
		Qty = iQtyTemp;										//委托数量 
	return true;
}
//获取冰山单最大数量
bool COFInputDlg::GetCtrlMaxNum(uint & Qty, bool bShowMsgBox, bool bSetFocus)
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_MaxQty));
	if (str.empty()|| (0 == atoi(str.c_str())))
	{
		if (bShowMsgBox == false)
			return false;
		MessageBox(m_hDlg, LoadResString(IDS_STRING_IceMaxInvalid).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
		if (bSetFocus)
			SetFocus(GetDlgItem(m_hDlg, IDC_EDIT_MaxQty));
		return false;
	}
	Qty = atoi(str.c_str());
	return true;
}
//获取冰山单最小数量
bool COFInputDlg::GetCtrlMinNum(uint & Qty, bool bShowMsgBox, bool bSetFocus)
{
	string str;
	str = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_MinQty));
	if (str.empty() || (0 == atoi(str.c_str())))
	{
		if (bShowMsgBox == false)
			return false;
		MessageBox(m_hDlg, LoadResString(IDS_STRING_IceMinInvalid).c_str(), LoadResString(IDS_STRING_Error).c_str(), MB_OK);
		if (bSetFocus)
			SetFocus(GetDlgItem(m_hDlg, IDC_EDIT_MinQty));
		return false;
	}
	Qty = atoi(str.c_str());
	return true;
}
//显示可开可平
void COFInputDlg::ShowCanOrderVol(const TTradableQty * pTradableQty, bool bAutoCover)
{
	//过滤是不是需要显示
	if (!pTradableQty)	return;
	if (m_CurUserNo != pTradableQty->UserNo)			return;
	TContractKey tempContractKey;
	memset(&tempContractKey, 0, sizeof(tempContractKey));
	if (!GetKeyCode(tempContractKey, FALSE))		return;
	if (memcmp(&tempContractKey, pTradableQty, sizeof(TContractKey)))	return;

	wchar_t str[101] = { 0 };
	//内盘 自动开平
	if (bAutoCover)
	{
		//有持仓 则显示可平 没有持仓则显示可开 还要根据配置平仓自动撤单、平今平昨自适应、自动拆分等有关？
	}
	else
		swprintf_s(str, LoadResString(IDS_STR_CanOrderFore).c_str(), pTradableQty->OpenQty + pTradableQty->CoverQty, pTradableQty->CoverQty, pTradableQty->OpenQty);
	m_StaticCanOpenCover.SetStaticText(str);
}

//显示冻结保证金
void COFInputDlg::ShowOrderFrozen(const TPreFreeze * pPreFreeze)
{
	//过滤是不是需要显示
	if (IsShowBuySellBtn() || !pPreFreeze || !pPreFreeze->pCurrency)
		return;
	
	if (m_CurUserNo != pPreFreeze->UserNo)			return;

	TContractKey tempContractKey;
	memset(&tempContractKey, 0, sizeof(tempContractKey));
	if (!GetKeyCode(tempContractKey, false))		return;
	
	if (memcmp(&tempContractKey, pPreFreeze, sizeof(TContractKey)))	return;
	
	double dUserDeposit = 0, dBaseDeposit = 0;//客户币种 基币 
	dBaseDeposit = pPreFreeze->pCurrency->ExchangeRate * pPreFreeze->Deposit;
	dUserDeposit = pPreFreeze->Deposit;
	wchar_t strTextBase[101] = { 0 };
	swprintf_s(strTextBase, LoadResString(IDS_STR_PreDepositFore).c_str(), 2, dBaseDeposit, LoadRC::ansi_to_unicode(pPreFreeze->pCurrency->CurrencyNo).c_str(), 2, dUserDeposit);
	
	m_StaticDeposit.SetStaticText(strTextBase);
}
//得到可用资金
bool COFInputDlg::GetAvailableMoney(double &dAvailabe)
{
	const TMoney *pMoney = g_pTradeData->GetMoney(m_CurUserNo.c_str(), nullptr, Currency_Base, m_CurLoginNo.c_str());
	if (!pMoney)
	{
		pMoney = g_pTradeData->GetMoney(m_CurUserNo.c_str(), nullptr, Currency_RMB, m_CurLoginNo.c_str());
		if (!pMoney)
			return false;
	}
	dAvailabe = pMoney->Available;
	return true;
}
//格式化保证金
void COFInputDlg::FormatStaticDepositDisplay()
{
	//得到焦点时调用、客户账号更改时调用、合约代码改变时调用 
	bool bValidData = true;
	TSendOrder tempSendOrder;
	CConvert::DefaultSendOrder(tempSendOrder);
	//更新sign
	memcpy_s(tempSendOrder.Sign, sizeof(tempSendOrder.Sign), m_CurLoginNo.c_str(), sizeof(tempSendOrder.Sign));
	//用户账号 如果是批量则改为基准客户
	if (!GetCtrlUser(tempSendOrder.UserNo, false))	bValidData = false;
	
	//获取商品信息
	if (!GetKeyCode(tempSendOrder, false))			bValidData = false;
	//获取委托数量
	if (!GetCtrlOrderNum(tempSendOrder.OrderQty, false, false))			bValidData = false;
	//有的后台需要委托价格 有的不需要  全填让API过滤
	GetCtrlPrice(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, tempSendOrder.OrderPrice, false, false);
	//买卖方向
	if (!GetDirect(tempSendOrder.Direct, false, false))	bValidData = false;
	//跟盘 买卖按钮显示
	if (IsShowBuySellBtn() && m_SwitchFollow.GetSwitchLevel() == SwitchLevel2) bValidData = false;
	
	if (!bValidData)
	{
		m_StaticDeposit.SetStaticText(TEXT(""));
		return;
	}
	double dAvalibale = 0;
	GetAvailableMoney(dAvalibale);
	if (!IsVaild(&tempSendOrder, GetMaxOrderID(), dAvalibale))//请求的数据已经有了 直接显示
		g_pTradeApi->QryPreFreeze(tempSendOrder);
	else
	{
		if (tempSendOrder.Direct == dBuy)			ShowOrderFrozen(&m_LastBuyFreeze);
		else if (tempSendOrder.Direct == dSell)		ShowOrderFrozen(&m_LastSellFreeze);
	}
}
//格式化可开可平
void COFInputDlg::FormatStaticNumDisplay()
{
	bool bValidData = true;
	TSendOrder tempSendOrder;
	CConvert::DefaultSendOrder(tempSendOrder);

	//目前参数是查一次服务器全给了 所以可以先不查询参数了 若以后服务器按合约发送参数 则可改为再合约编辑框内容改变时查询参数
	//查询参数 账户+合约+订单类型+开仓 
	//更新sign
	memcpy_s(tempSendOrder.Sign, sizeof(tempSendOrder.Sign), m_CurLoginNo.c_str(), sizeof(tempSendOrder.Sign));
	//用户账号 如果是批量则改为基准客户
	if (!GetCtrlUser(tempSendOrder.UserNo, false))	bValidData = false;
	
	//获取商品信息
	if (!GetKeyCode(tempSendOrder, false))			bValidData = false;
	//定单类型 内盘用 外盘不用
	tempSendOrder.OrderType = GetOrderType();

	//有的后台需要委托价格 有的不需要  全填让API过滤
	GetCtrlPrice(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, tempSendOrder.OrderPrice, false, false);
	//买卖方向
	if (!GetDirect(tempSendOrder.Direct, false, false))	bValidData = false;

	if (!bValidData)
	{
		m_StaticCanOpenCover.SetStaticText(TEXT(""));
		return;
	}
	double dAvalibale = 0;
	GetAvailableMoney(dAvalibale);

	//内盘 外盘分开
	if (m_CommodityInf.cCoverMode == cmCover || m_CommodityInf.cCoverMode == cmCoverToday)
	{
		tempSendOrder.Offset = oOpen;
		wchar_t strCanOpen[101] = { 0 };
		//可开
		int iret = CConvert::GetCanOpenQty(m_CommodityInf.cCoverMode, tempSendOrder, m_OrderDealInfo.bOpenAutoCancel);
		//可平
		CanCoverQty canCover;
		CConvert::GetCanCoverQty(m_CommodityInf.cCoverMode, tempSendOrder, tempSendOrder.UserNo, tempSendOrder.Direct, canCover, m_OrderDealInfo.bCoverAutoCancel);
		if (iret < 0)
			strCanOpen;/*=LoadRedString(IDS_STRING_CanOpenParam);*/
		else
		{
			if (m_CommodityInf.cCoverMode == cmCover)
				swprintf_s(strCanOpen, LoadResString(IDS_STR_CanOpenCover).c_str(), iret, canCover.uCanCoverQty);
			else
				swprintf_s(strCanOpen, LoadResString(IDS_STR_CanOpenCoverT).c_str(), iret, canCover.uCanCoverTQty, canCover.uCanCoverQty);
		}
		//内盘显示可开可平的位置与外盘不一样
		ShowTip(strCanOpen);
	}
	else
	{
		if (!IsVaild(&tempSendOrder, GetMaxOrderID(), dAvalibale))
			g_pTradeApi->QryTradableQty(tempSendOrder);
		else//请求的数据已经有了 直接显示
		{
			if (tempSendOrder.Direct == dBuy)
				ShowCanOrderVol(&m_LastBuyQty);
			else if (tempSendOrder.Direct == dSell)
				ShowCanOrderVol(&m_LastSellQty);
		}
	}
}
//格式化合约
void COFInputDlg::FormatStaticCodeDisplay()
{
	bool bValid = true;
	string strCode;
	strCode = GetWindowTextAnsic(GetDlgItem(m_hDlg, IDC_EDIT_Code));
	GetContractStr(strCode);

	TContractKey tempContractKey;
	memset(&tempContractKey, 0, sizeof(TContractKey));
	if (!CConvert::TContractNoToTContractKey(strCode.c_str(), tempContractKey))	bValid = false;
	SetLoginSign(tempContractKey.Sign);
	if (!g_StarApi)	bValid = false;
	char ContractId[101] = {0};
	if(!CConvert::StrToQContractNoType(strCode.c_str(), ContractId)) bValid = false;
	SContract* pContractInfo[1];
	//g_StarApi->GetContractData(strCode.c_str(), "", pContractInfo, 1, false);
	g_StarApi->GetContractData("", ContractId, pContractInfo, 1, false);
	if (NULL == pContractInfo[0])														bValid = false;
	if (!bValid)
	{
		m_StaticCodeName.SetStaticText(LoadResString(IDS_STRING_ChangeCode).c_str());
		return;
	}

	TExchangeKey tempExchangeKey;
	memset(&tempExchangeKey, 0, sizeof(tempExchangeKey));
	strcpy_s(tempExchangeKey.ExchangeNo, tempContractKey.ExchangeNo);
	SetLoginSign(tempExchangeKey.Sign);
	SExchange* pExcangeInfo[1] = { 0 };
	g_StarApi->GetExchangeData(tempExchangeKey.ExchangeNo, pExcangeInfo, 1, false);
	if (!pExcangeInfo[0])	return;

	TCommodityKey tempCommodityKey;
	memset(&tempCommodityKey, 0, sizeof(tempCommodityKey));
	strcpy_s(tempCommodityKey.ExchangeNo, tempContractKey.ExchangeNo);
	SetLoginSign(tempCommodityKey.Sign);
	tempCommodityKey.CommodityType = tempContractKey.CommodityType;
	strcpy_s(tempCommodityKey.CommodityNo, tempContractKey.CommodityNo);
	const SCommodity *pCommodityInfo = g_pTradeData->GetCommodity(&tempCommodityKey);
	if (!pCommodityInfo)	return;
	char contractName[101];
	g_StarApi->GetContractName(pContractInfo[0]->ContractNo, contractName);
	//string strtmp = contractName;
	//wstring strtmp2 = CConvert::UTF8ToUnicode(pCommodityInfo->CommodityName);
	wstring strDisplay;
	if (pCommodityInfo->CoverMode == cmNone || pCommodityInfo->CoverMode==cmUnfinish)
		strDisplay = strDisplay + CConvert::UTF8ToUnicode(pExcangeInfo[0]->ExchangeName) + TEXT("\\") + CConvert::UTF8ToUnicode(pCommodityInfo->CommodityName) + TEXT("\\")
		+ CConvert::UTF8ToUnicode(contractName);
	else
		strDisplay = strDisplay + CConvert::UTF8ToUnicode(pCommodityInfo->CommodityName);
	m_StaticCodeName.SetStaticText(strDisplay.c_str());
}
//可开可平、保证金占用是否还有效，是不是需要重新请求 ifpOrder->OrderQty有效为可开可平
bool COFInputDlg::IsVaild(TSendOrder * pOrder, int AMaxOrdID, double AAvailable)
{
	string strKey1, strKey2;
	CConvert::FormatTradeContract(*pOrder, strKey1);
	if (pOrder->Direct == dBuy)
	{
		if (pOrder->OrderQty == 0)//可开可平
		{
			CConvert::FormatTradeContract(m_LastBuyQty, strKey2);
			return (m_LastBuyQty.MaxOrdID == AMaxOrdID) && CGeneralGlobal::DoubleSameValue(AAvailable, m_LastBuyQty.Available, FLOAT_SAME_ERRORDIFF) &&
				(strcmp(m_LastBuyQty.UserNo, pOrder->UserNo) == 0) && (strKey1 == strKey2) && (pOrder->Direct == m_LastBuyQty.Direct) && (pOrder->Offset == m_LastBuyQty.Offset)
				&& (pOrder->Hedge == m_LastBuyQty.Hedge);
		}
		else//保证金占用
		{
			CConvert::FormatTradeContract(m_LastBuyFreeze, strKey2);
			return (m_LastBuyFreeze.MaxOrdID == AMaxOrdID) && CGeneralGlobal::DoubleSameValue(AAvailable, m_LastBuyFreeze.Available, FLOAT_SAME_ERRORDIFF) &&
				(strcmp(m_LastBuyFreeze.UserNo, pOrder->UserNo) == 0) && (strKey1 == strKey2) && (pOrder->Direct == m_LastBuyFreeze.Direct) && (pOrder->Offset == m_LastBuyFreeze.Offset)
				&& (pOrder->Hedge == m_LastBuyFreeze.Hedge) && (pOrder->OrderQty == m_LastBuyFreeze.OrderQty)
				&& CGeneralGlobal::DoubleSameValue(pOrder->OrderPrice, m_LastBuyFreeze.OrderPrice, FLOAT_SAME_ERRORDIFF);
		}
	}
	else
	{
		if (pOrder->OrderQty == 0)//可开可平
		{
			CConvert::FormatTradeContract(m_LastSellQty, strKey2);
			return (m_LastSellQty.MaxOrdID == AMaxOrdID) && CGeneralGlobal::DoubleSameValue(AAvailable, m_LastSellQty.Available, FLOAT_SAME_ERRORDIFF) &&
				(strcmp(m_LastSellQty.UserNo, pOrder->UserNo) == 0) && (strKey1 == strKey2) && (pOrder->Direct == m_LastSellQty.Direct) && (pOrder->Offset == m_LastSellQty.Offset)
				&& (pOrder->Hedge == m_LastSellQty.Hedge);
		}
		else//保证金占用
		{
			CConvert::FormatTradeContract(m_LastSellFreeze, strKey2);
			return (m_LastSellFreeze.MaxOrdID == AMaxOrdID) && CGeneralGlobal::DoubleSameValue(AAvailable, m_LastSellFreeze.Available, FLOAT_SAME_ERRORDIFF) &&
				(strcmp(m_LastSellFreeze.UserNo, pOrder->UserNo) == 0) && (strKey1 == strKey2) && (pOrder->Direct == m_LastSellFreeze.Direct) && (pOrder->Offset == m_LastSellFreeze.Offset)
				&& (pOrder->Hedge == m_LastSellFreeze.Hedge) && (pOrder->OrderQty == m_LastSellFreeze.OrderQty)
				&& CGeneralGlobal::DoubleSameValue(pOrder->OrderPrice, m_LastSellFreeze.OrderPrice, FLOAT_SAME_ERRORDIFF);
		}
	}
	return true;
}
//转换
bool COFInputDlg::TransKey(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//首先判断是否是小键盘区域的虚拟码表，有两种情况：一（wParam>=96&&wParam<=105）此范围说明是NumLock打开，按下0-9 
	//二 NumLock关闭 虚拟吗为上、下、左、右、End、home、pgup、pugdn、ins、del等键 但(0==(pMsg->lParam&0x01000000))此标志位为0（非扩展，对应正常的上下左右等键为扩展、扩展此位为1）
	//这两种情况 第一种要转为正常曲艺0-9的ascii码，第二种情况要丢掉
	if (wParam == VK_NUMLOCK)
		return 1;
	SHORT keyState = GetKeyState(VK_NUMLOCK);
	if (keyState & 1)//低位为真 NumLock打开l 
	{
		if (wParam >= 96 && wParam <= 105)//小键盘的数字
		{
			wParam = wParam - 48;
		}
	}
	else//NumLock关闭情况下 判断虚拟码是否是小键盘上的十个键(上下左右键等)，是的话就应该判断扩展位是否为1不是改为1
	{
		if (wParam == 45 || wParam == 35 || wParam == 40 || wParam == 34 || wParam == 37 ||
			wParam == 12 || wParam == 39 || wParam == 36 || wParam == 38 || wParam == 33)//
		{
			if (0 == (lParam & 0x01000000))
				return 1;
		}
	}
	return 0;
}

bool COFInputDlg::RequestCommodity(const char *szCode)
{
	SContractNoType typeId;
	typeId[0] = '\0';
	CConvert::StrToQContractNoType(szCode, typeId);
	g_PlugLoad.GetStarApi()->UnsubQuote(&typeId, 1, this);

	bool bsuc = false;
	if (g_PlugLoad.GetStarApi())
	{
		if (CConvert::IsValidQuotContract(typeId))
		{
			g_PlugLoad.GetStarApi()->SubQuote(&typeId, 1,this);
			m_regQuotId = typeId;
			bsuc = true;
		}
	}
	return bsuc;
}
void COFInputDlg::UpdateCommodityTick(const char *szCode)
{
	//根据合约代码设置spin最小变动价位？
	//先在在交易合约里找
	TContractKey TradeContract;	//交易合约
	memset(&TradeContract, 0, sizeof(TradeContract));
	strcpy_s(TradeContract.Sign, GetLoginSign());
	CConvert::TContractNoToTContractKey(szCode, TradeContract);
	char contractID[101];
	CConvert::StrToQContractNoType(szCode, contractID);
	if (CConvert::IsValidQuotContract(contractID))//合法的交易合约
	{
		CConvert::GetCommodityInf(TradeContract, m_CommodityInf);
		return;
	}

	SContractNoType typeId;
	typeId[0] = '\0';
	CConvert::TradeContractToQContractNoType(TradeContract, typeId);
	//交易找不到 去行情中找
	CConvert::GetQuoteCommodityInf(szCode, m_CommodityInf);
}

//委托价格为0下市价单
bool COFInputDlg::IsOrderTypeToMarket(const TSendOrder &SendOrder)
{
	//组合 价格为0 限价单 配置选中
	bool bCombo = CConvert::IsCombContract(SendOrder.CommodityType);
	return !bCombo&&CConvert::DoubleSameValue(SendOrder.OrderPrice,0,FLOAT_SAME_ERRORDIFF)&&
		SendOrder.OrderType == otLimit;
}
//上期所
bool COFInputDlg::IsSHFE(const char *szExchange)
{
	return 0 == strcmp(szExchange, "SHFE");
}
//修改上期所订单类型
void COFInputDlg::ModifySHFEOrderTyep(TSendOrder &SendOrder)
{
	if (SendOrder.OrderType == otMarket && IsSHFE(SendOrder.ExchangeNo))
	{
		SQuoteSnapShot temp;
		string str;
		CConvert::TContractKeyToTContractNo(SendOrder,str);
		if (GetQuotInfo(str.c_str(), temp))
		{
			SendOrder.OrderType = otLimit;
			if (dBuy == SendOrder.Direct)
				SendOrder.OrderPrice = temp.Data[S_FID_LIMITUPPRICE].FidAttr!=S_FIDATTR_NONE ? temp.Data[S_FID_LIMITUPPRICE].Price:0;
			else
				SendOrder.OrderPrice = temp.Data[S_FID_LIMITDOWNPRICE].FidAttr!=S_FIDATTR_NONE ? temp.Data[S_FID_LIMITDOWNPRICE].Price:0;
		}
	}
}
bool COFInputDlg::TradeLoginLinkage()
{
	if (m_vecUserNo.size() == 0 && g_PlugLoad.GetMainFrm())
	{
		g_PlugLoad.GetMainFrm()->Linkage(m_hDlg, _KeyBoardOrder, W_USER_LOGIN, "");
		return true;
	}
	else
		return false;
}
//CommitMode应该是为埋单和自动单使用的
void COFInputDlg::DoOrder(char CommitMode, char Direct)//此函数应该是用来下普通单（非快速）
{
	if (TradeLoginLinkage())
		return;
	TSendOrder	SendOrder;
	CConvert::DefaultSendOrder(SendOrder);
	if (Direct != dNone)
		SendOrder.Direct = Direct;

	if (!GetDlgCtrlInput(SendOrder, true, true))	return;

	//自动单、埋单
	if (CommitMode != '\0')	SendOrder.StrategyType = CommitMode;

	//组合委托价格本来就能为0 
	if (IsOrderTypeToMarket(SendOrder))
		SendOrder.OrderType = otMarket;

	//上期所 市价单 自动转为限价单 以涨停跌停发出
	ModifySHFEOrderTyep(SendOrder);

	//开满撤单(内盘) 此功能目前分析无法精确实现
	//if( (m_CommodityInf.cCoverMode==cmCover||m_CommodityInf.cCoverMode==cmCoverToday) &&SendOrder.Offset==oOpen){}

	//通用下单
	OFSendOrder(&SendOrder); 
		
	ClearTipBar();

	RecoverState();
}
////下单之后操作 比如 取消强平 定单改为限价 有效类型等
void COFInputDlg::RecoverState()
{
	//下单清空买卖
	if (m_OrderReturn.bClearDirect)
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Direct), _T(""));

	//取消强平
	if (m_OrderReturn.bCancelForceCover)
		CancelForceCover();

	//订单类型改为限价
	if (m_OrderReturn.bOrderTypeLimit)
		ModifyOrderType(otLimit);

	//有效类型改为当日有效
	if (m_OrderReturn.bValidTypeGFD)
		ModifyValidType(vtGFD);

	//取消T1
	if (m_OrderReturn.bCancelAddOne)
		CancelT1();
}

//改单
void COFInputDlg::ModifyOrderOper(const TOrder *pOrder,bool bBatch)
{
	if (m_LockUserNoBtn.GetLockState())//)
		return;
	memcpy_s(&m_ModifyOrder, sizeof(TOrder), pOrder, sizeof(TOrder));
	SetCanLayout(false);//此函数可以控制布局在ModifyOrderOper只布局一次 所以要放在最开始
	ResetTJCheckBox(pOrder->IsRiskOrder, pOrder->AddOneIsValid);
	SetModifyOrderOper(true);
	m_bModifyOrderOperBatch = bBatch;
	FillKeyBoardOrderDlg(pOrder, false, false, false,true);
	SetCanLayout(true);
	Layout();
}
//改单时布局前重置条件选择区域
void COFInputDlg::ResetTJCheckBox(TBool IsRiskOrder, TTradeSection AddOneIsValid)
{
	if (IsCheck(IDC_CHECK_TJ))
		OnBnClickedCheckTJ();
	if (IsCheck(IDC_CHECK_PK))
		OnBnClickedCheckPK();
	if (IsCheck(IDC_CHECK_KS))
		OnBnClickedCheckKS();
	if (IsCheck(IDC_Check_BatchCount))
		OnBnClickedCheckBatchCount();
	if (IsCheck(IDC_Check_BatchOrder))
		OnBnClickedCheckBatchOrder();

	//这两个需要根据改单信息来复制
	if (IsShowT1())
	{
		int check = BST_UNCHECKED;
		if (AddOneIsValid == tsNight)
			check = BST_CHECKED;
		m_CheckT1.SetCheck(check);
	}

	//强平
	int check = BST_UNCHECKED;
	if (IsShowForceCover() && IsRiskOrder == bYes)
		check = BST_CHECKED;
	m_CheckQP.SetCheck(check);
}
void COFInputDlg::OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
	if (Alt)//横向下单组合件控制
	{
		switch (VKey)
		{
		case Vk_Condition://条件
			PostMessage(m_hDlg, WM_OFHOTKEY, Vk_Condition, NULL);
			break;
		case Vk_CloseOpen://平开
			PostMessage(m_hDlg, WM_OFHOTKEY, Vk_CloseOpen, NULL);
			break;
		case Vk_Fast://快速
			PostMessage(m_hDlg, WM_OFHOTKEY, Vk_Fast, NULL);
			break;
		case Vk_BatchTimes://批次
			PostMessage(m_hDlg, WM_OFHOTKEY, Vk_BatchTimes, NULL);
			break;
		case Vk_BatchQty://批量
			PostMessage(m_hDlg, WM_OFHOTKEY, Vk_BatchQty, NULL);
			break;
		case Vk_OK://确定
			PostMessage(m_hDlg, WM_OFHOTKEY, Vk_OK, NULL);
			break;
		case Vk_PreOrder://埋单
			PostMessage(m_hDlg, WM_OFHOTKEY, Vk_PreOrder, NULL);
			break;
		default:
			break;
		}
		return;
	}
	if (!IsCheck(IDC_CHECK_KS))
		return;
	
}
//简化合约转化
void COFInputDlg::GetContractStr(string &str)
{
	if (0 == VersionType())
	{
		SContract *p = m_thread.GetContractPtr(str);
		if (p)
		{
			string contractNo(p->ContractNo);
			CConvert::QContractNoTypeToStr(contractNo, str);
		}
			
	}
}
//修改通过双击行情持仓等方式填充code的内容 比如把ZCE SR 501转为SR1501||SR501
void COFInputDlg::ModifyContractStr(string&str)
{
	if (0 == VersionType())
	{
		TContractKey contractKey;
		memset(&contractKey, 0, sizeof(TContractKey));
		CConvert::TContractNoToTContractKey(str.c_str(), contractKey);
		if (ctFutures == contractKey.CommodityType)
		{
			char cc[101] = { 0 };
			sprintf_s(cc, "%s%s", contractKey.CommodityNo, contractKey.ContractNo);
			str = cc;
		}
	}
}
bool COFInputDlg::IsCheck(int check)
{
	CxKeyBoardCheck *p = NULL;
	if (check == IDC_CHECK_TJ) p = &m_CheckTJ;
	else if (check == IDC_CHECK_PK) p = &m_CheckPK;
	else if (check == IDC_CHECK_KS) p = &m_CheckKS;
	else if (check == IDC_Check_BatchCount) p = &m_CheckPC;
	else if (check == IDC_Check_BatchOrder) p = &m_CheckPL;
	else if (check == IDC_CHECK_T1) p = &m_CheckT1;
	else if (check == IDC_CHECK_ForCover) p = &m_CheckQP;
	return p->GetCheck() == BST_CHECKED ? true : false;
}
void COFInputDlg::OnDealQuoteKLine()
{
	//改单恢复
	if (GetModifyOrderOper())
	{
		SetModifyOrderOper(false);
		Layout();
	}
	//锁合约
	if (m_LockCodeBtn.GetLockState())
		return;
	//填充合约 获取最小变动价等信息 必填
	string sCode = "";
	CConvert::QContractNoTypeToStr(m_sQuoteKline, sCode);
	string  strCode = sCode;
	ModifyContractStr(strCode);
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Code), LoadRC::ansi_to_unicode(strCode).c_str());//此时会触发合约编辑框 文本更新消息
	GetContractStr(strCode);

	char sUser[21] = { 0 };
	TContractKey key;
	memset(&key, 0, sizeof(key));
	memcpy(key.Sign, m_CurLoginNo.c_str(), sizeof(key.Sign));
	if (CConvert::TContractNoToTContractKey(strCode.c_str(), key)
		&& CConvert::IsValidTradeCommodity(key) && GetCtrlUser(sUser, FALSE))
	{}
	else
	{
		//交易找不到 去行情中找
		CConvert::GetQuoteCommodityInf(sCode.c_str(), m_CommodityInf);
		Layout();
	}	
	//SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, 0);
}
void COFInputDlg::OnDealQuoteCommon()
{
	//改单恢复
	if (GetModifyOrderOper())
	{
		SetModifyOrderOper(false);
		Layout();
	}
	//锁合约
	if (m_LockCodeBtn.GetLockState())
		return;

	//填充合约 获取最小变动价等信息 必填
	string strCode = m_FillOrderInfo.sCode;
	ModifyContractStr(strCode);
	SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Code), LoadRC::ansi_to_unicode(strCode).c_str());//此时会触发合约编辑框 文本更新消息
	GetContractStr(strCode);

	char cDirect = m_FillOrderInfo.cDirect;
	//买卖 双击买价填卖出，双击卖价填买入
	if (cDirect != dNone)
	{
		wchar_t wdirect[10] = { 0 };
		CTradeConst::DirectToStr(cDirect, wdirect, 10);
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Direct), wdirect);
	}

	//数量
	if (m_FillOrderInfo.nQtyType == 0)
		SetWindowInt(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), m_FillOrderInfo.nQty);//默认平仓量

	char sUser[21] = { 0 };
	TContractKey key;
	memset(&key, 0, sizeof(key));
	memcpy(key.Sign, m_CurLoginNo.c_str(), sizeof(key.Sign));
	if (CConvert::TContractNoToTContractKey(m_FillOrderInfo.sCode, key)
		&& CConvert::IsValidTradeCommodity(key) && GetCtrlUser(sUser, FALSE))
	{
		//开平
		char tOffset = CConvert::GetOffset(sUser, &key, m_FillOrderInfo.cDirect);
		wchar_t woffset[10] = { 0 };
		CTradeConst::OffsetToStr(tOffset, woffset, 10);
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Offset), woffset);
		//默认数量
		if (m_FillOrderInfo.nQtyType == 2)
		{
			if (tOffset == oCoverT || tOffset == oCover)
				SetWindowInt(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), m_FillOrderInfo.nCoverQty);//默认平仓量
			else
				SetWindowInt(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), m_FillOrderInfo.nOpenQty);//默认开仓量
		}

		if (IsCheck(IDC_CHECK_TJ))
		{
			UpdateTJState();//条件状态更新
			UpdateTrigPriceFill(m_FillOrderInfo.sCode, cDirect);//触发价格更新
		}

		FormatStaticCodeDisplay();
		m_StaticCanOpenCover.SetStaticText(TEXT(""));
		m_StaticDeposit.SetStaticText(TEXT(""));
	}
	else
	{
		SetWindowInt(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), m_FillOrderInfo.nOpenQty);//默认开仓量
																					  //交易找不到 去行情中找
		string sCode = m_FillOrderInfo.sCode;
		CConvert::GetQuoteCommodityInf(sCode.c_str(), m_CommodityInf);
		if (IsCheck(IDC_CHECK_TJ))
		{
			UpdateTJState();//条件状态更新
			UpdateTrigPriceFill(sCode, cDirect);//触发价格更新
		}
		Layout();
	}
	//价格
	if (IsShowBuySellBtn() && m_SwitchFollow.GetSwitchLevel() == SwitchLevel2/*m_KeyBoardSet.bPriceSynQuote*/)
		PostMessage(m_hDlg, WM_QUOTCHG, 0, 0);
	else
		SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, m_FillOrderInfo.dPrice);

	if (m_bHotKeyAltActive&&m_KeyBoardSet.bSetDefaultPos&&IsWindowVisible(m_hDlg))
	{
		SetFocus(GetDlgItem(m_hDlg, IDC_STATIC_OrderQty));
		SetIndexFocus(m_KeyBoardSet.uiFocusPos - 1, true);
	}
}


void COFInputDlg::OnDealPositionOrderCommon()
{
	if (GetModifyOrderOper())
	{
		SetModifyOrderOper(false);
		Layout();
	}
	//更新资金账号组框
	//非批量
	if (!IsCheck(IDC_Check_BatchOrder) && !m_LockUserNoBtn.GetLockState())
	{
		vector<CStringsVec> vecUserNo;
		if (CConvert::GetAllUserNo(vecUserNo) && vecUserNo.size() > 0)
		{
			for (vector<CStringsVec>::iterator iter = vecUserNo.begin(); iter != vecUserNo.end(); iter++)
			{
				if ((*iter)[0] == m_FillOrderInfo.sUserNo)
				{
					ComboBox_SetText(GetDlgItem(m_hDlg, IDC_ComUserNo), LoadRC::ansi_to_unicode(m_FillOrderInfo.sUserNo).c_str());
					UpdateLoginSign(m_FillOrderInfo.sUserNo, (*iter)[2].c_str());
				}
			}
		}
	}	
	
	//条件单
	if (m_FillOrderInfo.cStrategy == stCondition&& !IsCheck(IDC_CHECK_TJ))
		OnBnClickedCheckTJ();
	//填充合约
	if (!m_LockCodeBtn.GetLockState())
	{
		string sCode = m_FillOrderInfo.sCode;
		ModifyContractStr(sCode);
		SetWindowTextW(GetDlgItem(m_hDlg, IDC_EDIT_Code), LoadRC::ansi_to_unicode(sCode).c_str());//此时会触发合约编辑框 文本更新消息
		
		//更新行情报价
		char contractid[101] = { 0 };
		if (CConvert::StrToQContractNoType(m_FillOrderInfo.sCode, contractid))
		{
			string str = contractid;
			str = "contractid=" + str;
			g_PlugLoad.GetMainFrm()->Linkage(m_hDlg, "MouseOrder", W_SETCONTRACT, str.c_str());
		}
		//买卖  开平 双击买价填卖出，双击卖价填买入
		wchar_t wCon[10] = { 0 };
		CTradeConst::DirectToStr(m_FillOrderInfo.cDirect, wCon, 10);
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Direct), wCon);
		CTradeConst::OffsetToStr(m_FillOrderInfo.cOffset, wCon, 10);
		Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT_Offset), wCon);

		//委托价格
		if (IsShowBuySellBtn() && m_SwitchFollow.GetSwitchLevel() == SwitchLevel2/*m_KeyBoardSet.bPriceSynQuote*/)
			PostMessage(m_hDlg, WM_QUOTCHG, 0, 0);			
		else
			SetPriceToDlg(IDC_EDIT_OrderPrice, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_FenMu, m_FillOrderInfo.dPrice);
			
		if (m_FillOrderInfo.nQtyType==0)
			SetWindowInt(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), m_FillOrderInfo.nQty);

		if (IsCheck(IDC_CHECK_TJ))
		{
			UpdateTJState();//条件状态更新
			UpdateTrigPriceFill(m_FillOrderInfo.sCode, m_FillOrderInfo.cDirect);//触发价格更新
		}
	}
	
	UpdateTradeLayOut();
	FormatStaticCodeDisplay();
	m_StaticCanOpenCover.SetStaticText(TEXT(""));
	m_StaticDeposit.SetStaticText(TEXT(""));
	if (m_bHotKeyAltActive&&m_KeyBoardSet.bSetDefaultPos&&IsWindowVisible(m_hDlg)/* && m_FillOrderInfo.nClickType*/)
		SetIndexFocus(m_KeyBoardSet.uiFocusPos - 1,true);	
}
void COFInputDlg::OnDealModifyRecover()
{
	if (GetModifyOrderOper())
	{
		SetModifyOrderOper(false);
		Layout();
		RecoverState();
		if (g_PlugLoad.GetMainFrm())
			g_PlugLoad.GetMainFrm()->Linkage(m_hDlg, "CommonModule", WM_MODIFY_RECOVER, "");
	}	
}
void COFInputDlg::OFSendOrder(TSendOrder* order)
{
	vector<int> nVectId;
	int nStartID = 0, nCounts = 0;
	ErrorReflectInfo eError;
	memset(&eError, 0, sizeof(ErrorReflectInfo));
	BatchCountsInfo bCountInfo = { 0 };

	//批次
	if (IsCheck(IDC_Check_BatchCount)&&IsShowPC())
	{
		//下单次数
		bool bvalid = GetPCQtyCount(bCountInfo.uiCounts, false);
		if (!bvalid)
		{
			MessageBox(m_hDlg, LoadResString(IDS_InValidOrderCounts).c_str(), LoadResString(IDS_STRING_Error).c_str(), NULL);
			return;
		}

		//数量增量
		bvalid = GetPCQtyIncre(bCountInfo.nQtyInc, false);
		if (!bvalid)
		{
			MessageBox(m_hDlg, LoadResString(IDS_InValidQtyInc).c_str(), LoadResString(IDS_STRING_Error).c_str(), NULL);
			return;
		}

		//价格增量
		double dIncrePrice = 0;
		bvalid = GetPCPriceIncre(bCountInfo.dPriceInc, false);
		if (!bvalid)
		{
			MessageBox(m_hDlg, LoadResString(IDS_InValidPriceInc).c_str(), LoadResString(IDS_STRING_Error).c_str(), NULL);
			return;
		}
	}
	CM_PARAM param = { 0 };	
	param.cmParam |= ((IsShowPL() && IsCheck(IDC_Check_BatchOrder)) ? CM_BATCHQTY : CM_NORMAL);
	param.ctParam |= ((IsShowPK() && IsCheck(IDC_CHECK_PK)) ? CT_COVEROPEN : CT_NONE);
	if (IsShowPC() && IsCheck(IDC_Check_BatchCount))
	{
		param.cmParam |= CM_BATCHCOUNT;
		memcpy_s(&param.bcinfo, sizeof(BatchCountsInfo), &bCountInfo, sizeof(BatchCountsInfo));
	}
	TVecInt vOrderIds;
	if (!g_PlugLoad.GetCommonModule()->InsertOrder(*order, eError, vOrderIds, &param))
	{
		if (eError.nErrorLevel == 1)
			MessageBox(m_hDlg, eError.error_text, LoadResString(IDS_STRING_Error).c_str(), NULL);
	}
	else {
		//追单
		if (m_KeyBoardSet.bStartOrderAssistant && (g_PlugLoad.GetStrategyTradeAPI()->GetStartegyWorkState() | SWS_Assist))
		{
			if (m_KeyBoardSet.uiAssistType == 0 || (m_KeyBoardSet.uiAssistType == 2 && (order->Offset == oCover|| order->Offset == oCoverT)) || (m_KeyBoardSet.uiAssistType == 1 && order->Offset == oOpen))
			{
				for (int i = 0; i < vOrderIds.size(); i++)
				{
					TAssistInfo assistInfo;
					assistInfo.OrderID = vOrderIds.at(i);
					assistInfo.p = this;
					g_PlugLoad.GetStrategyTradeAPI()->AddAssistOrder(assistInfo);
				}
			}
		}
	}
}

bool COFInputDlg::GetSignByUserNo(string sUser,int& nIndex)
{
	bool bhave = false;
	for (vector<CStringsVec>::size_type i = 0; i < m_vecUserNo.size(); i++)
	{
		if (0 == strcmp(m_vecUserNo[i][0].c_str(), sUser.c_str()))
		{
			UpdateLoginSign(m_vecUserNo[i][0].c_str(), m_vecUserNo[i][2].c_str());
			nIndex = i;
			bhave = true;
			break;
		}
	}
	if (!bhave)	return false;
	return true;
}
void COFInputDlg::OnKeyBoardCfgChange(const C_KeyBoardOrderCfg* kbcfg)
{
	if (!kbcfg)
		return;
	if (m_KeyBoardSet.bStartOrderAssistant && !kbcfg->bStartOrderAssistant)
	{
		TAssistInfo info;
		info.OrderID = 0;//停止所有追单
		info.p = this;
		g_PlugLoad.GetStrategyTradeAPI()->StopAssistOrder(info);
	}
	memcpy_s(&m_KeyBoardSet, sizeof(C_KeyBoardOrderCfg) - sizeof(std::map<std::wstring, std::wstring>), kbcfg, sizeof(C_KeyBoardOrderCfg) - sizeof(std::map<std::wstring, std::wstring>));
	m_KeyBoardSet.mapContractHotkey = kbcfg->mapContractHotkey;
	if (m_hDlg)
		Layout();
}
void COFInputDlg::OnKeyBoardCfgFontChange(const C_KeyBoardFontInfoCfg* kbcfg)
{
	if (!kbcfg)
		return;
	memcpy_s(&m_OrderFont, sizeof(C_KeyBoardFontInfoCfg), kbcfg, sizeof(C_KeyBoardFontInfoCfg));
	if (m_hDlg)
	{
		SetCtrlFont();
		Layout();
		SetUser();
	}	
}
void COFInputDlg::OnKeyBoardSeqChange(const C_KeyBoardSeqCfg* kbcfg)
{
	if (!kbcfg)
		return;
	m_OrderFiled.nFieldCount = kbcfg->nFieldCount;
	m_OrderFiled.pOrderField = kbcfg->pOrderField;
	ReadFieldConfig();
	if (m_hDlg)
	{
		SetCtrlFont();
		Layout();
		SetUser();
	}
}

void COFInputDlg::OnAssistInfoRef(uint OrderIdOld, uint OrderIdNew)
{

	//追单回调
}

void COFInputDlg::OnDealAssistFill(const C_FillOrderAssistInfo* pdata)
{
	bool bUpdatePosition = false,bUpdateCondition = false;
	if (m_FillOrderAssist.bShowPosition != pdata->bShowPosition
		|| m_FillOrderAssist.uiShowPosType != pdata->uiShowPosType)
	{
		bUpdatePosition = true;
	}	
	if (IsCheck(IDC_CHECK_TJ)&&(m_FillOrderAssist.uiTrigCondition != pdata->uiTrigCondition
		|| m_FillOrderAssist.uiTrigMode != pdata->uiTrigMode
		|| m_FillOrderAssist.uiTrigOverPoint != pdata->uiTrigOverPoint
		|| m_FillOrderAssist.uiTrigPriceMode != pdata->uiTrigPriceMode))
	{
		bUpdateCondition = true;
	}
	if (sizeof(C_FillOrderAssistInfo) > pdata->uSize)
	{
		memset(&m_FillOrderAssist, 0, sizeof(C_FillOrderAssistInfo));
		memcpy_s(&m_FillOrderAssist, pdata->uSize, pdata, pdata->uSize);
	}
	else
		memcpy_s(&m_FillOrderAssist, sizeof(C_FillOrderAssistInfo), pdata, sizeof(C_FillOrderAssistInfo));

	if (m_hDlg)
	{
		if (bUpdatePosition)
			PostMessage(m_hDlg, WM_POSITIONSHOW, 0, 0);//持买持卖更新
		if (bUpdateCondition)
			PostMessage(m_hDlg, WM_CONDITION, 0, 0);//条件更新
	}	
	m_UserNO.SetReverFind(m_FillOrderAssist.bBackMatchUserNo);

	//获取上下键切换合约
	m_vecUpDownSwitchContract.clear();
	//g_PlugLoad.GetCommonModule()->GetSwicthContracts(m_vecUpDownSwitchContract);
	std::vector<std::string> UpDownSwitchContract;
	ICommonModule *pCommonModule = g_PlugLoad.GetCommonModule();
	if(pCommonModule != nullptr)
		pCommonModule->GetSwicthContracts(m_vecUpDownSwitchContract);
}
void COFInputDlg::OnDealOrderDeal(const C_OrderDealInfo* pdata)
{
	if (sizeof(C_OrderDealInfo) > pdata->uSize)
	{
		memset(&m_OrderDealInfo, 0, sizeof(C_OrderDealInfo));
		memcpy_s(&m_OrderDealInfo, pdata->uSize, pdata, pdata->uSize);
	}
	else
		memcpy_s(&m_OrderDealInfo, sizeof(C_OrderDealInfo), pdata, sizeof(C_OrderDealInfo));
}
void COFInputDlg::OnDealOrderReturn(const C_OrderReturnInfo* pdata)
{
	if (sizeof(C_OrderReturnInfo) > pdata->uSize)
	{
		memset(&m_OrderReturn, 0, sizeof(C_OrderReturnInfo));
		memcpy_s(&m_OrderReturn, pdata->uSize, pdata, pdata->uSize);
	}
	else
		memcpy_s(&m_OrderReturn, sizeof(C_OrderReturnInfo), pdata, sizeof(C_OrderReturnInfo));
	
}
void COFInputDlg::OnDealFastOrderInfo(const C_FastOrderInfo* pdata)
{
	if (sizeof(C_FastOrderInfo) > pdata->uSize)
	{
		memset(&m_fastOrderInfo, 0, sizeof(C_FastOrderInfo));
		memcpy_s(&m_fastOrderInfo, pdata->uSize, pdata, pdata->uSize);
	}
	else
		m_fastOrderInfo.dChangePoint = pdata->dChangePoint;
}
void COFInputDlg::OnDealBatchGroupInfo(const C_BatchGroupInfo* pdata)
{
	m_batchGroupVec.clear();
	if (pdata->uMember == 0)
		return;
	
	unsigned int nStructSize = ((const BatchGroupInfo*)pdata->pBatchGroupInfo)->uSize;
	if (sizeof(BatchGroupInfo) > nStructSize)
	{
		ShowTip(TEXT("UnMatched Version"));
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
	PostMessage(m_hDlg, WM_BATCHORDER, NULL, NULL);
}
void COFInputDlg::SetFastControlState()
{
	bool bcheck = IsShowKS()&& m_CheckKS.GetCheck() == BST_CHECKED ? false : true;
	bool bwarn = IsShowKS()&&m_CheckKS.GetColor() == g_ColorRefData.GetColorBuyRed() ? false : true;

	bcheck = bwarn && bcheck;//选中 或者 红色（有快速选中）
	
	char ordertype = GetOrderTypeEx();
	bool benable = otMarket != ordertype&&otMarketStop != ordertype;//市价 止损
	if (!bwarn || !benable)
		EnableWindow(m_SwitchFollow.GetHwnd(), false);
	else
		EnableWindow(m_SwitchFollow.GetHwnd(), true);

	//资金账号锁
	EnableWindow(m_LockUserNoBtn.GetHwnd(), bcheck);
	if (m_LockUserNoBtn.GetLockState())
		EnableWindow(GetDlgItem(m_hDlg, IDC_ComUserNo), false);
	else
	{
		EnableWindow(GetDlgItem(m_hDlg, IDC_ComUserNo), bcheck);
		if (!bcheck)
			Edit_SetSel(m_UserNO.m_Edit.GetHwnd(), 0, 0);
	}		
	//合约代码锁
	EnableWindow(m_LockCodeBtn.GetHwnd(), bcheck);
	if (m_LockCodeBtn.GetLockState())
		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_Code), false);
	else
		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_Code), true);
		//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_Code), bcheck);
	
	//买卖 开平 委托数量
	EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_Direct), true);
	EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_Offset), true);
	EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), true);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_Direct), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_Offset), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_OrderQty), bcheck);
	
	//价格 spin不控制
	//if (bcheck)
		EnablePrice(IDC_EDIT_OrderPrice, IDC_Sta_OrderPrice_Add, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_ChuHao, IDC_Sta_OrderPrice_FenMu, 0, OrderPriceIsCanAble());
	//else
	//	EnablePrice(IDC_EDIT_OrderPrice, IDC_Sta_OrderPrice_Add, IDC_EDITORDERPRICE_FenZi, IDC_Sta_OrderPrice_ChuHao, IDC_Sta_OrderPrice_FenMu, 0, FALSE);

		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti), true);
		EnableWindow(GetDlgItem(m_hDlg, IDC_COMBO_OrderType), true);
		EnableWindow(GetDlgItem(m_hDlg, IDC_COMBO_ValidType), true);

		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_TrigPrice), true);
		EnableWindow(GetDlgItem(m_hDlg, IDC_EDITTriggerPRICE_FenZi), true);
		EnableWindow(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), true);
		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), true);

		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_QtyCount), true);
		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_IncreQty), true);
		EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice), true);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_QtyMulti), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_COMBO_OrderType), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_COMBO_ValidType), bcheck);

	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_TrigPrice), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDITTriggerPRICE_FenZi), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_COMBO_TirgMode), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_TrigCondi), bcheck);

	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_QtyCount), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_IncreQty), bcheck);
	//EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_IncrePrice), bcheck);
}
void COFInputDlg::SetHotKeyActive(bool bActive)
{ 
	m_bHotKeyAltActive = bActive;
	UpdateAltHotKeyShowState();
}
void COFInputDlg::UpdateAltHotKeyShowState()
{
	if (GetModifyOrderOper() || !IsWindow(m_hDlg))
		return;
	if (!m_bHotKeyAltActive)
	{
		SetWindowTextW(m_CheckTJ.GetHwnd(), LoadResString(IDS_Condition).c_str());
		InvalidateRect(m_CheckTJ.GetHwnd(), NULL, TRUE);
		SetWindowTextW(m_CheckPK.GetHwnd(), LoadResString(IDS_CoverOpen).c_str());
		InvalidateRect(m_CheckPK.GetHwnd(), NULL, TRUE);
		SetWindowTextW(m_CheckKS.GetHwnd(), LoadResString(IDS_FastOrder).c_str());
		InvalidateRect(m_CheckKS.GetHwnd(), NULL, TRUE);
		SetWindowTextW(m_CheckPL.GetHwnd(), LoadResString(IDS_BatchOrder).c_str());
		InvalidateRect(m_CheckPL.GetHwnd(), NULL, TRUE);
		SetWindowTextW(m_CheckPC.GetHwnd(), LoadResString(IDS_BatchCount).c_str());
		InvalidateRect(m_CheckPC.GetHwnd(), NULL, TRUE);
		m_OrderButton.SetButtonText(LoadResString(IDS_OK));
		m_PayOrderButton.SetButtonText(LoadResString(IDS_PayOrder));
	}
	else
	{
		SetWindowTextW(m_CheckTJ.GetHwnd(), LoadResString(IDS_ConditionEx).c_str());
		InvalidateRect(m_CheckTJ.GetHwnd(), NULL, TRUE);
		SetWindowTextW(m_CheckPK.GetHwnd(), LoadResString(IDS_CoverOpenEx).c_str());
		InvalidateRect(m_CheckPK.GetHwnd(), NULL, TRUE);
		SetWindowTextW(m_CheckKS.GetHwnd(), LoadResString(IDS_FastOrderEx).c_str());
		InvalidateRect(m_CheckKS.GetHwnd(), NULL, TRUE);
		SetWindowTextW(m_CheckPL.GetHwnd(), LoadResString(IDS_STR_BatchOrder).c_str());
		InvalidateRect(m_CheckPL.GetHwnd(), NULL, TRUE);
		SetWindowTextW(m_CheckPC.GetHwnd(), LoadResString(IDS_BatchCountEx).c_str());
		InvalidateRect(m_CheckPC.GetHwnd(), NULL, TRUE);
		m_OrderButton.SetButtonText(LoadResString(IDS_STR_DoOrder));
		m_PayOrderButton.SetButtonText(LoadResString(IDS_STR_PayOrder));
	}
}
void COFInputDlg::SetCodeFromQuote(const char * sKey, const char* cType)	//双击 单击 行情填单
{
	string strContent = sKey;
	vector<NoticeContent> vecNoticeContent;
	if (!CConvert::SplitNotice(strContent, ';','=', vecNoticeContent))
		return;
	if ((vecNoticeContent.size() != ID_LEVEL + 1)&&(vecNoticeContent.size() != ID_LEVEL))
		return;

	SSpreadRateModeType RateMode = S_SPREADMODE_DIFF;
	size_t npos = vecNoticeContent[ID_CODE].strContent.find(RateMode);
	if (string::npos == npos)
	{
		RateMode = S_SPREADMODE_RATIO;
		npos = vecNoticeContent[ID_CODE].strContent.find(RateMode);
	}
	if (npos != string::npos)
		return;

	if (vecNoticeContent[ID_SRC].strContent == W_SRC_GRID)
	{
		if (!strcmp(cType, W_LButtonDown))
		{
			C_ExcInfo info = { m_KeyBoardSet.OutPriceType, m_KeyBoardSet.OutPriceTick };
			memcpy_s(m_FillOrderInfo.sUserNo, sizeof(m_FillOrderInfo.sUserNo), m_CurUserNo.c_str(), sizeof(m_FillOrderInfo.sUserNo));
			if (g_PlugLoad.GetCommonModule() && g_PlugLoad.GetCommonModule()->SetCodeFromQuote(vecNoticeContent[ID_CODE].strContent.c_str(), vecNoticeContent[ID_FIELD].strContent.c_str(), 0,cType, GetLoginSign(), m_FillOrderInfo, &info))
				PostMessage(m_hDlg, WM_DEALQUOTECOMMON, 0, 0);
		}
		else if (!strcmp(cType, W_VK_UP) || !strcmp(cType, W_VK_DOWN))
			SetCodeFromQuoteKline(vecNoticeContent[0].strContent.c_str());
	}
	else if (vecNoticeContent[ID_SRC].strContent == W_SRC_PANEL)
	{
		if (!strcmp(cType, W_LButtonDown))
		{
			C_ExcInfo info = { m_KeyBoardSet.OutPriceType, m_KeyBoardSet.OutPriceTick };
			memcpy_s(m_FillOrderInfo.sUserNo, sizeof(m_FillOrderInfo.sUserNo), m_CurUserNo.c_str(), sizeof(m_FillOrderInfo.sUserNo));
			
			
			if (g_PlugLoad.GetCommonModule() && g_PlugLoad.GetCommonModule()->SetCodeFromQuote(vecNoticeContent[ID_CODE].strContent.c_str(), vecNoticeContent[ID_FIELD].strContent.c_str(),atoi(vecNoticeContent[ID_LEVEL].strContent.c_str()), cType, GetLoginSign(), m_FillOrderInfo, &info))
				PostMessage(m_hDlg, WM_DEALQUOTECOMMON, 0, 0);
		}
	}
	else if (vecNoticeContent[2].strContent == W_SRC_KLINE)
		SetCodeFromQuoteKline(vecNoticeContent[0].strContent.c_str());
}

void COFInputDlg::SetCodeFromPosTotal(const char * sKey, const char* cType)						//点击持仓合计
{
	C_ExcInfo info = { m_KeyBoardSet.OutPriceType,m_KeyBoardSet.OutPriceTick};
	if (g_PlugLoad.GetCommonModule() && g_PlugLoad.GetCommonModule()->SetCodeFromPosTotal(sKey, cType, m_FillOrderInfo, &info))
		PostMessage(m_hDlg, WM_DEALPOSCOMMON, 0, 0);
}
void COFInputDlg::SetCodeFromPosSub(const char * sKey, const char* cType)						//点击持仓明细
{
	C_ExcInfo info = {m_KeyBoardSet.OutPriceType, m_KeyBoardSet.OutPriceTick};
	if (g_PlugLoad.GetCommonModule() && g_PlugLoad.GetCommonModule()->SetCodeFromPosSub(sKey, cType, m_FillOrderInfo, &info))
		PostMessage(m_hDlg, WM_DEALPOSCOMMON, 0, 0);
}
void COFInputDlg::SetCodeFromOrder(const char *sOrderID, const char* cType)						//双击 单击 委托填单	
{
	C_ExcInfo info = {m_KeyBoardSet.OutPriceType, m_KeyBoardSet.OutPriceTick };
	if (g_PlugLoad.GetCommonModule() && g_PlugLoad.GetCommonModule()->SetCodeFromOrder(sOrderID, cType, m_FillOrderInfo, &info))
		PostMessage(m_hDlg, WM_DEALORDERCOMMON, 0, 0);
}
void COFInputDlg::SetCodeFromQuoteKline(const char *sContent)
{
	m_sQuoteKline = sContent;
	SContract * refOut = g_PlugLoad.GetStarApi()->GetContractUnderlay(sContent);
	if (refOut)
		m_sQuoteKline = refOut->ContractNo;

	PostMessage(m_hDlg, WM_DEALKLINECONTRACT, 0, 0);
}

void COFInputDlg::UpdatePropertyCfg()
{
	char cProperty[100] = { 0 };
	sprintf_s(cProperty, "ConditionCheck=%d;BackHandCheck=%d;FastCheck=%d;BatchCountCheck=%d;AddOne=%d", m_CheckTJ.GetCheck() == BST_CHECKED ? 1 : 0, m_CheckPK.GetCheck() == BST_CHECKED ? 1 : 0,
		m_CheckKS.GetCheck() == BST_CHECKED ? 1 : 0, m_CheckPC.GetCheck() == BST_CHECKED ? 1 : 0,m_CheckT1.GetCheck()==BST_CHECKED?1:0);
	if (g_PlugLoad.GetMainFrm())
		g_PlugLoad.GetMainFrm()->SetProperty(m_hDlg, cProperty);
}
void COFInputDlg::UpdatePropertyControl()
{
	if (m_wProperty.bConditionCheck)
		OnBnClickedCheckTJ();
	if (m_wProperty.bBackHandCheck)
		OnBnClickedCheckPK();
	if (m_wProperty.bFastCheck)
		OnBnClickedCheckKS();
	if (m_wProperty.bBatchCountCheck)
		OnBnClickedCheckBatchCount();
	if (m_wProperty.bBatchCountCheck)
		OnBnClickedCheckT1();
}