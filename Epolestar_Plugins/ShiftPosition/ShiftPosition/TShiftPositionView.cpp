#include "BaseInclude.h"

ShiftOrderConfig *g_pstConfig;
const int CAPTION_H = 30;
const int FRAME_H = 710;
const int FRAME_V = 950;

#define  closeWidth 9
#define  MiniWidth 10
#define  MiniHeight 14
#define  SetWidth 60

#define  Vap_x   10
#define  Vap_y   5
// #define  WidgetWidth 120
// #define  lab_WidgetWidth  50
#define  WidgetHeight 20

#define listWidth  (FRAME_V - 2 * Vap_x)
#define listHeight  150

#define ConditionOrderID  8
int WidgetWidth = 150;
int lab_WidgetWidth = 50;
int EditWidgetWidth = 100;
int EditSmall = 65;
int spinWidgetWidth = 14;

extern IStarApi* g_pStarApi;
extern ShiftQuoteApi* g_pShiftQuoteApi;
extern TShiftPosition* g_pShiftPosition;
extern ShiftTradeApi* g_pShiftTradeApi;

TShiftPosition::TShiftPosition()
{
	m_rcClose.right = FRAME_V - 8;
	m_rcClose.left = m_rcClose.right - closeWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + closeWidth;

	m_rcMini.right = m_rcClose.left - 12;
	m_rcMini.left = m_rcMini.right - MiniWidth;
	m_rcMini.top = 8;
	m_rcMini.bottom = m_rcMini.top + MiniHeight;

	m_rcSet.right = m_rcMini.left - 10;
	m_rcSet.left = m_rcSet.right - SetWidth;
	m_rcSet.top = 0;
	m_rcSet.bottom = m_rcSet.top + CAPTION_H;

	m_iListShiftBottom = FRAME_H - 470;
	m_bMouseTrack = false;
	m_bFirstContract = false;
	

	m_iShiftOrderId = 0;

	g_pShiftQuoteApi = new ShiftQuoteApi();
	g_pShiftQuoteApi->Regist(&m_mapShiftListInfo, &m_mapShiftConditon);


	g_pShiftTradeApi = new ShiftTradeApi();
	g_pShiftTradeApi->Register(&m_mapShiftConditon);


	m_indexShift = -1;

	m_listShiftSelectNo[51] = { 0 };
	m_bIsFirstOrder = true;
	m_logCount.reSet();
	m_bShowCodeList = true;
	m_bIsTrigger = false;
	m_bFirstStart = true;
	g_pstConfig = new ShiftOrderConfig;
}
TShiftPosition::~TShiftPosition()
{
	g_pStarApi->UnsubAllQuote(this);
}
void TShiftPosition::ShowFrm()
{
	ReadShiftContract();
	HWND hwnd = FindWindow(L"class TMainFrame", NULL);
	//了解属性
	//WS_CLIPCHILDREN 创建父窗口时使用。绘图发生在父窗口时，不包括子窗口占用区域
	CreateFrm(L"TShiftPosition", hwnd, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU , WS_EX_CONTROLPARENT | WS_EX_APPWINDOW, L"移仓");

	int x = (GetSystemMetrics(SM_CXSCREEN) - FRAME_V) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - FRAME_H) / 2;
	//SWP_SHOWWINDOW显示窗口
	SetWindowPos(m_Hwnd, HWND_TOP, x, y, FRAME_V, FRAME_H, SWP_HIDEWINDOW);	

}


void CALLBACK TShiftPosition::LoginTimeProc(HWND, UINT, UINT_PTR, DWORD)
{
	/*KillTimer(NULL, ID_Login_Delay);
	if (m_bNotLogin)
	{
		m_bNotLogin = false;
		g_pMainFrame->Linkage(g_pShiftPosition->GetHwnd(), "", "TradeLogin", NULL);
	}*/
	KillTimer(g_pShiftPosition->GetHwnd(), ID_Login_Delay);
	g_pMainFrame->Linkage(g_pShiftPosition->GetHwnd(), "", "TradeLogin", NULL);
}
void CALLBACK TShiftPosition::FirstStartWarning(HWND, UINT, UINT_PTR, DWORD)
{
	KillTimer(g_pShiftPosition->GetHwnd(), ID_ShiftPosition_Warning);
	_TMessageBox_Domodal(g_pShiftPosition->GetHwnd(), "提示", L"移仓过程中 手动 平仓 会导致移仓结果不准确，请谨慎平仓！", TMB_OK);
}
void TShiftPosition::OnMenuClick(const unsigned int MenuIndex, const HWND PastLife)
{
	
	if (!CConvert::IsUserLogin())
	{
		if (!g_pShiftPosition)
		{
			g_pShiftPosition = new TShiftPosition();
			g_pShiftPosition->ShowFrm();
		}
		ShowWindow(g_pShiftPosition->GetHwnd(),SW_HIDE);
		g_pMainFrame->AddLinkage(g_pShiftPosition->GetHwnd(), 0, OnLinkageAction);

		m_bNotLogin = true;
		SetTimer(g_pShiftPosition->GetHwnd(), ID_Login_Delay, 100, NULL);//参数 窗口句柄 定时器ID  NULL使用OnTimer 
		return;
	}
	else if (!g_pShiftPosition)
	{
		HWND hwnd = FindWindow(L"class TMainFrame", NULL);
		g_pShiftPosition = new TShiftPosition();
		g_pShiftPosition->ShowFrm();
		//_TMessageBox_Domodal(hwnd, "提示", L"移仓过程中 手动 平仓 会导致移仓结果不准确，请谨慎平仓！", TMB_OK);
	}
		
	if (g_pShiftPosition->GetHwnd())
	{	
		if (IsWindowVisible(g_pShiftPosition->GetHwnd()))
			ShowWindow(g_pShiftPosition->GetHwnd(), SW_HIDE);
		else
		{
			ShowWindow(g_pShiftPosition->GetHwnd(), SW_SHOW);
			ShowWindow(g_pShiftPosition->GetHwnd(), SW_RESTORE);
		}
		if (m_bFirstStart)
		{
			m_bFirstStart = false;
			SetTimer(g_pShiftPosition->GetHwnd(), ID_ShiftPosition_Warning, 100, NULL);
			
		}
	}
}

void OnLinkageAction(HWND From, HWND To, const char* Sender, const char* Action, const char* Content)
{
	g_pShiftPosition = new TShiftPosition();
	g_pShiftPosition->ShowFrm();
}

void TShiftPosition::TOnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
	TCHAR name[50] = { 0 };
	GetClassName(source, name, sizeof(name));
	if (lstrcmp(name, WC_EDIT) == 0 )
		return;
	if (Alt == false && Ctrl == false && Shift == false && VKey == VK_F12)
	{
		if (!CConvert::IsUserLogin())
			return;
		if (g_pShiftPosition)
		{
			if (IsWindowVisible(g_pShiftPosition->GetHwnd()))
				ShowWindow(g_pShiftPosition->GetHwnd(), SW_HIDE);
			else
			{
				ShowWindow(g_pShiftPosition->GetHwnd(), SW_SHOW);
				ShowWindow(g_pShiftPosition->GetHwnd(), SW_RESTORE);
			}
		}
		else
		{
			g_pShiftPosition = new TShiftPosition();
			g_pShiftPosition->ShowFrm();
		}
	}
}
LRESULT TShiftPosition::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case SSWM_TABTAG_SELECTED://==1
		OnTapSeleted(wParam, lParam);
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:	//==2
		OnNotify(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN://==3
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_SIZE://==4
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_SIZING://==5
		OnSizing(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_DRAWITEM://==6
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM://==7
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_LBUTTONDOWN://8
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_NCHITTEST://==9   //=========当光标位于顶部或底部时，返回光标位置
		return OnNcHitTest(wParam, lParam);
	case WM_SETCURSOR://==10  //==========改变光标形状
		OnSetCursor(wParam, lParam);
		return PROCESSED;
	case WM_NCLBUTTONDOWN://==11
		OnNcLButtonDown(wParam, lParam);
		return NOT_PROCESSED;
	case WM_ONRESETUSERNO://==12
		SetUserInfo();
		InitOrderWidget();
		ShowAllParOrder();
		ShowAllPosition();
		return PROCESSED;
	case WM_ONORDER://==13                       //交易回调
		g_pShiftTradeApi->OnDealOrder((const TOrder*)wParam);
		UpdateParOrderList(wParam, lParam);
		OnDealLog((TOrder*)wParam);
		return PROCESSED;
	case WM_ONPOSITION://==14						//持仓变化回调
		UpdatePositionList(wParam, lParam);
		return PROCESSED;
	case WM_NCDESTROY://==15  //==========
		OnNcDestroy();
		break;
	case WM_DESTROY://==16
		OnDestroy();
		break;
	case WM_ONQUOTE://==17
		g_pShiftQuoteApi->DealOnQuote(wParam);
		SetMarketValue();
		UpdateMarketValue();		
		break;
	case WM_LIST_LBUTTONDOWN://==18	
		if (lParam == ID_LISTSHIFT)
		{
			m_indexShift = LOWORD(wParam) + HIWORD(wParam);
			m_listShift.SelectItem(LOWORD(wParam) + HIWORD(wParam));
		}	
		break;
	case WM_MOUSEWHEEL://==19  //=========滚轮。合约EDIT
		OnMouseWheel(wParam, lParam);
		return PROCESSED;
	case SSWM_EDIT_PRESS_ENTER://==20
		if ((HWND)lParam == m_editFirstContract.GetHwnd())
		{
			m_bFirstContract = true;
			m_popContractList.OnReturn();
		}
		else if ((HWND)lParam == m_editSecondContract.GetHwnd())
		{
			m_bFirstContract = false;
			m_popContractList.OnReturn();
		}
		return PROCESSED;
	case WM_KEYDOWN://==21			
		KeyDown(wParam, lParam);
		break;
	case WM_TIMER:
		OnTimer(wParam);
		return PROCESSED;
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
		{
			ShowWindow(g_pShiftPosition->GetHwnd(), SW_HIDE);
			return PROCESSED;
		}
	default:
		return NOT_PROCESSED;
	}
	return NOT_PROCESSED;
}
void TShiftPosition::OnQuote(const SContract* cont, const SQuoteUpdate* quote)
{
	PostMessage(m_Hwnd, WM_ONQUOTE, (WPARAM)cont, 0);
}
void TShiftPosition::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BTN_FIRSTCONTRACT:
	case ID_BTN_SECONDCONTRACT:
		OnSearchCode(lParam);
		break;
	case ID_BTN_OK:
		OnBtnOk();
		break;
	case ID_BTN_CONDITIONORDER:
		OnBtnCondition();
		break;
	default:
		break;
	}
}
void TShiftPosition::OnBtnCondition()
{
	//要针对界面的合约信息;


	if (!IsWindow(m_ShiftCondition.GetHwnd()))
	{
		m_ShiftCondition.Regist(&m_mapShiftListInfo,&m_mapShiftConditon, &m_mapUserInfo);
		m_ShiftCondition.ShowFrm(m_Hwnd, L"TShiftCondition", "移仓条件单");
		POINT pt;
		GetCursorPos(&pt);
		m_ShiftCondition.MoveWindow(pt.x, pt.y, 400, 240);
	}
	else
	{
		if (!IsWindowVisible(m_ShiftCondition.GetHwnd()))
			ShowWindow(m_ShiftCondition.GetHwnd(), SW_SHOW);
		else 
			ShowWindow(m_ShiftCondition.GetHwnd(), SW_HIDE);
	}
}
void TShiftPosition::OnBtnOk()
{
	ShiftSendOrder stShift;
	//获取UserNo
	memcpy(stShift.UserNo, m_stUserInfo1.UserNo, sizeof(stShift.UserNo));
	memcpy(stShift.Sign, m_stUserInfo1.Sign, sizeof(stShift.Sign));

	memcpy(stShift.UserNo2, m_stUserInfo2.UserNo, sizeof(stShift.UserNo2));
	memcpy(stShift.Sign2, m_stUserInfo2.Sign, sizeof(stShift.Sign2));


	//获取合约号
	m_editFirstContract.GetText(stShift.ContractCode1, sizeof(stShift.ContractCode1));
	m_editSecondContract.GetText(stShift.ContractCode2, sizeof(stShift.ContractCode2));

	//获取数量配比
	stShift.OrderQty1 = m_sEditFirstOrderQty.GetWindowNumber();
	stShift.OrderQty2 = m_sEditSecondOrderQty.GetWindowNumber();


	//获取价格类型
	stShift.OrderPriceType1 = m_cbFirstOrderType.GetSelect();
	stShift.OrderPriceType2 = m_cbSecondOrderType.GetSelect();

	//获取投保类型
	if (g_pstConfig->m_stChasePrice.Hedge == cxSpeculate)
		stShift.Hedge = hSpeculate;
	else
		stShift.Hedge = hHedge;

	//获取超价点数
	stShift.OrderPriceSuperPot1 = m_sEditFirstSuperPot.GetWindowNumber();
	stShift.OrderPriceSuperPot2 = m_sEditSecondSuperPot.GetWindowNumber();

	if (g_pstConfig->m_bSingle)
	{
		stShift.TotalQty = m_sEditTotalQty.GetWindowNumber();
		stShift.RecordTotal = stShift.TotalQty;
	}
	if (!QtyCheck(stShift.OrderQty1, stShift.OrderQty2, stShift.TotalQty, stShift.FirstColse))
		return;
	if (m_cbDirect.GetSelect() < 2)
		stShift.FirstColse = true;
	else
		stShift.FirstColse = false;
	if (!(m_cbDirect.GetSelect()%2))
	{
		stShift.Direct1 = dBuy;
		stShift.Direct2 = dSell;
	}
	else
	{
		stShift.Direct1 = dSell;
		stShift.Direct2 = dBuy;
	}
	//记录是否要追价
	stShift.bChasePrice = m_ckChasePrice.GetCheck();

	g_pShiftTradeApi->SendShiftOrder(stShift);
}

bool TShiftPosition::QtyCheck(int Qty1, int Qty2, int Total, bool firstcolse)
{
	if (Qty1 <= 0 || Qty2 <= 0)
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"下单手数不能为0!", TMB_OK);
		return false;
	}
	if (firstcolse)
	{
		if (Qty1 > 1000 || Total > 1000)
		{
			_TMessageBox_Domodal(m_Hwnd, "提示", L"平仓手数不能大于1000!", TMB_OK);
			return false;
		}
	}
	else
	{
		int temptotal = (Qty2 / Qty1) * Total;
		if (Qty2 > 1000 || temptotal > 1000)
		{
			_TMessageBox_Domodal(m_Hwnd, "提示", L"平仓手数不能大于1000!", TMB_OK);
			return false;
		}
	}
	if (Total != 0 && Total < Qty1)
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"总手数不能小于单笔手数!", TMB_OK);
		return false;
	}
	return true;
}

void TShiftPosition::OnNcDestroy()
{
	delete this;
}
void TShiftPosition::OnDestroy()
{
	WriteShiftContract();
}
void TShiftPosition::DealCbChange(WPARAM wParam)
{
	int id = LOWORD(wParam);
	if (id == ID_CB_USERNO1) {
		DealChangeUserNo(ID_CB_USERNO1);
	}
	else if (id == ID_CB_USERNO2)
	{
		DealChangeUserNo(ID_CB_USERNO2);
	}
	else if (id == ID_CB_FIRSTORDERTYPE) {
		if (m_cbFirstOrderType.GetSelect() == SuperPrice) {
			m_sEditFirstSuperPot.Enable(true);
			m_sEditFirstSuperPot.SetWindowNumber(1);
		}	
		else {
			m_sEditFirstSuperPot.Enable(false);
			m_sEditFirstSuperPot.SetWindowNumber(0);
		}
	}
	else if (id == ID_CB_SECONDORDERTYPE) {
		if (m_cbSecondOrderType.GetSelect() == SuperPrice) {
			m_sEditSecondSuperPot.Enable(true);
			m_sEditSecondSuperPot.SetWindowNumber(1);
		}
		else {
			m_sEditSecondSuperPot.Enable(false);
			m_sEditSecondSuperPot.SetWindowNumber(0);
		}
	}
	else if (id == ID_CB_DIRECT)
	{
		/*int index = m_cbDirect.GetSelect();
		if (index < 2)
		{
			EnableWindow(GetDlgItem(m_Hwnd, ID_CK_CHASEPRICE), true);
		}
		else
		{
			m_ckChasePrice.SetCheck(BST_UNCHECKED);
			EnableWindow(GetDlgItem(m_Hwnd, ID_CK_CHASEPRICE), false);
		}*/
	}
}
void TShiftPosition::DealChangeUserNo(int id)
{
	TCHAR UserNo[42];
	if (id == ID_CB_USERNO1)
		m_cbUserNo1.GetText(UserNo);
	else
		m_cbUserNo2.GetText(UserNo);
	string cUserNo = LoadRC::unicode_to_ansi(UserNo);
	std::map<string, TUserInfo*>::iterator iter = m_mapUserInfo.find(cUserNo);
	if (iter != m_mapUserInfo.end())
	{
		if (id == ID_CB_USERNO1)
		{
			memcpy(&m_stUserInfo1, iter->second, sizeof(m_stUserInfo1));
			m_popContractCode.UpdateData(m_stUserInfo1.Sign);
			InitTradeData();
			ShowAllParOrder();
			ShowAllPosition();
		}
		else
		{
			memcpy(&m_stUserInfo2, iter->second, sizeof(m_stUserInfo2));
			m_popContractCode.UpdateData(m_stUserInfo2.Sign);
		}
	}
}
void TShiftPosition::WriteShiftContract()
{
	char Name[10] = "移仓合约";
	int iKey = 0;
	char cKey[5] = { 0 };
	char ShiftContent[150] = { 0 };
	TCHAR fileName[150] = L"\\Config\\ShiftPosition.pri";
	TCHAR filePath[150] = { 0 };
	GetFilePath(filePath, sizeof(filePath)/2, fileName);
	std::map<std::string, ShiftListInfo>::iterator iter = m_mapShiftListInfo.begin();

	WritePrivateProfileStringA(Name, NULL, NULL, LoadRC::unicode_to_ansi(filePath).c_str());

	for (; iter != m_mapShiftListInfo.end(); iter++)
	{
		sprintf_s(cKey, "%d", iKey);
		GetShiftContent(ShiftContent, iter->second);
		WritePrivateProfileStringA(Name, cKey, ShiftContent, LoadRC::unicode_to_ansi(filePath).c_str());
		iKey++;
	}
}
void TShiftPosition::GetShiftContent(char* ShiftContent, ShiftListInfo stShift)
{
	sprintf_s(ShiftContent, 150, "%s|%s|%d|%d|%d|%d|%d|%s", stShift.Code1, stShift.Code2, stShift.OrderQty1, stShift.OrderQty2,
		stShift.LinkContract1, stShift.LinkContract2, stShift.LinkCondition, stShift.ShiftContractNo);
}
void TShiftPosition::InitShiftMap(char* buffer)
{
	if (strcmp(buffer, "Over") == 0)
		return;
	std::string PriBuffer = buffer;
	std::vector<std::string> vtInfo = CConvert::SplitString(PriBuffer, '|');
	if (vtInfo.size() != 8)
		return;
	ShiftListInfo stShift;
	int i = 0;
	memcpy(stShift.Code1, vtInfo[i++].c_str(), sizeof(stShift.Code1));
	memcpy(stShift.Code2, vtInfo[i++].c_str(), sizeof(stShift.Code2));
	stShift.OrderQty1 = atoi(vtInfo[i++].c_str());
	stShift.OrderQty2 = atoi(vtInfo[i++].c_str());

	stShift.LinkContract1 = atoi(vtInfo[i++].c_str());
	stShift.LinkContract2 = atoi(vtInfo[i++].c_str());
	stShift.LinkCondition = atoi(vtInfo[i++].c_str());

	memcpy(stShift.ShiftContractNo, vtInfo[i++].c_str(), sizeof(stShift.ShiftContractNo));
	m_mapShiftListInfo.insert(make_pair(stShift.ShiftContractNo, stShift));
}
void TShiftPosition::ReadShiftContract()
{
	char Name[10] = "移仓合约";
	int iKey = 0;
	char buffer[250] = { 0 };
	char cKey[10] = { 0 };
	TCHAR fileName[150] = L"\\Config\\ShiftPosition.pri";
	TCHAR filePath[150] = { 0 };
	GetFilePath(filePath, sizeof(filePath)/2, fileName);
	while (strcmp(buffer, "Over") != 0)
	{
		sprintf_s(cKey, "%d", iKey);
		if (!GetPrivateProfileStringA(Name, cKey, "Over", buffer, sizeof(buffer), LoadRC::unicode_to_ansi(filePath).c_str()))
			return;
		//初始化Map
		InitShiftMap(buffer);
		iKey++;
	}
}
void TShiftPosition::SetContractNo(string Code, int iFirstOrSecond)
{
	m_bShowCodeList = false;
	if (iFirstOrSecond == 1)
		m_editFirstContract.SetText(Code.c_str());
	else
		m_editSecondContract.SetText(Code.c_str());
	SubQuote(Code,iFirstOrSecond);
	SetMarketValue();
	m_bShowCodeList = true;
}
void TShiftPosition::OnTimer(WPARAM wParam)
{
	if (wParam == ID_Login_Delay)
		LoginTimeProc(NULL, 0, 0, NULL);
	else if (wParam == ID_ShiftPosition_Warning)
	{
		
		FirstStartWarning(NULL, 0, 0, NULL);
	}
	else
		g_pShiftTradeApi->OnDealTimer();
}
void TShiftPosition::SubQuote(string Code,int num)
{
	TContractKey stContract;
	memset(&stContract, 0, sizeof(TContractKey));
	if (num == 1)
		memcpy(stContract.Sign, m_stUserInfo1.Sign, sizeof(stContract.Sign));
	else if(num == 2)
		memcpy(stContract.Sign, m_stUserInfo2.Sign, sizeof(stContract.Sign));
	CConvert::TContractNoToTContractKey(Code.c_str(), stContract);
	char sQuote[101] = { 0 };
	CConvert::TradeContractToQContractNoType(stContract, sQuote);
	if (g_pStarApi)
		g_pStarApi->SubQuote(&sQuote, 1, this);
}
void TShiftPosition::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT id = GetWindowLong(GetFocus(), GWL_ID);
	if (id == ID_EDIT_FIRSTCONTRACT || id == ID_EDIT_SECONDCONTRACT)
	{
		if (!IsWindowVisible(m_popContractList.GetHwnd()))
			return;
		int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (nDelta > 0)
			m_popContractList.AddNum();
		else
			m_popContractList.DelNum();
	}
}
void TShiftPosition::KeyDown(WPARAM wParam, LPARAM lParam)
{
	UINT id = GetWindowLong(GetFocus(), GWL_ID);
	if (id == ID_EDIT_FIRSTCONTRACT || id == ID_EDIT_SECONDCONTRACT)
	{
		if (!IsWindowVisible(m_popContractList.GetHwnd()))
			return;
		if (wParam == VK_DOWN)
			m_popContractList.AddNum();
		else if (wParam == VK_UP)
			m_popContractList.DelNum();
	}

}
void TShiftPosition::CloseOrder(int OrderType)
{	
	double OrderPrice = 0.0;
	TDirect direct = dNone;
	int index = m_listPosition.GetSelectItem();
	if (index < 0 && index < m_UserPosition.size())
		return;
	TPosition* pst = (TPosition*)m_UserPosition.at(index);
	if (!pst)
		return;
	pst->Direct == dBuy ? direct = dSell : direct = dBuy;
	if (OrderType == IDM_POSITTIONCOUNTERCLOSE)
		CConvert::GetPrice(pst, direct, CounterPrice, OrderPrice);
	else if (OrderType == IDM_POSITIONMARKETCLOSE)
		CConvert::GetPrice(pst, direct, MarketPrice, OrderPrice);

	g_pShiftTradeApi->CloseOrder(pst, OrderPrice);

}
void TShiftPosition::DealAddCondition()
{
	if (!IsWindow(m_ShiftCondition.GetHwnd()))
		m_ShiftCondition.Regist(&m_mapShiftListInfo, &m_mapShiftConditon, &m_mapUserInfo);
	char ShiftContractNo[51] = { 0 };
	m_listShift.GetSubItem(m_indexShift, 0, ShiftContractNo, sizeof(ShiftContractNo));
	TCHAR UserNo[21] = { 0 };
	TCHAR UserNo2[21] = { 0 };
	m_cbUserNo1.GetText(UserNo);
	m_cbUserNo2.GetText(UserNo2);
	m_ShiftCondition.ShowCondition(LoadRC::unicode_to_ansi(UserNo).c_str(), LoadRC::unicode_to_ansi(UserNo2).c_str(), ShiftContractNo);
}
void TShiftPosition::DeleteShiftContract()
{
	if (m_indexShift == -1)
		return;
	char shiftContract[51] = { 0 };
	m_listShift.GetSubItem(m_indexShift, 0, shiftContract, sizeof(shiftContract));
	std::map<string, ShiftListInfo>::iterator iter = m_mapShiftListInfo.find(shiftContract);
	if (_TMessageBox_Domodal(m_Hwnd, "提示", L"是否确定删除自设移仓合约？（删除之后相应的条件单也会删除）", TMB_OKCANCEL))
	{
		if (iter != m_mapShiftListInfo.end())
		{
			for (int i = 0; i < iter->second.OrderId.size(); i++)
			{
				std::map<int, ShiftCondition>::iterator Coniter = m_mapShiftConditon.find(iter->second.OrderId.at(i));
				if (Coniter != m_mapShiftConditon.end())
					m_mapShiftConditon.erase(Coniter);
			}
			m_mapShiftListInfo.erase(iter);
		}		
		UpdateShiftList();
	}
}
void TShiftPosition::CanCelOrder()
{
	int index = m_listParOrder.GetSelectItem();
	if (index < 0 || index > m_UserParOrder.size())
		return;
	TOrder* pst = (TOrder*)m_UserParOrder.at(index);
	g_pTradeApi->CancelOrder(pst->OrderID);
}
void TShiftPosition::ModiftyOrder(int PriceType)
{
	int index = m_listParOrder.GetSelectItem();
	if (index < 0 || index > m_UserParOrder.size())
		return;
	TOrder* pst = (TOrder*)m_UserParOrder.at(index);
	TSendOrder stOrder;
	memcpy(&stOrder, pst, sizeof(TSendOrder));
	stOrder.OrderQty = pst->OrderQty - pst->MatchQty;
	
	if (PriceType == IDM_PARORDERCOUNTERFOLLOW)
		CConvert::GetPrice(&stOrder, stOrder.Direct, CounterPrice, stOrder.OrderPrice);
	else if (PriceType == IDM_PARORDERMARKETFOLLOW)
		CConvert::GetPrice(&stOrder, stOrder.Direct, MarketPrice, stOrder.OrderPrice);
	g_pShiftTradeApi->ModifyOrder(pst->OrderID, stOrder);
}
void TShiftPosition::OnTapSeleted(WPARAM wParam, LPARAM lParam)
{
	ShowTabList(m_tabShiftPosition.GetSelIndex());
}
void TShiftPosition::ShowTabList(int index)
{
	if (index == 0)
	{
		ShowWindow(m_listCondition.GetListHwnd(), SW_HIDE);
		ShowWindow(m_listShift.GetListHwnd(), SW_SHOW);		
	}
	else
	{
		ShowWindow(m_listShift.GetListHwnd(), SW_HIDE);
		ShowWindow(m_listCondition.GetListHwnd(), SW_SHOW);	
// 		CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_TRIGGER, MF_UNCHECKED);
// 		CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_NOTTRIGGER, MF_CHECKED);
	}
	ShowAllCondition(m_bIsTrigger);
}
extern bool g_MouseTrack;
void TShiftPosition::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
		case IDM_SHIFTADDNEWSHIFT:
			if (!IsWindow(m_AddContract.GetHwnd()))
			{
				m_AddContract.ShowFrm(m_Hwnd);
			}
			else
			{
				if (!IsWindowVisible(m_AddContract.GetHwnd()))
					ShowWindow(m_AddContract.GetHwnd(), SW_SHOW);
				m_AddContract.InitWnd();
			}
			m_AddContract.Regist(m_stUserInfo1.Sign, m_stUserInfo2.Sign);
			break;
		case IDM_SHIFTDELETESHIFT://删除移仓合约
			DeleteShiftContract();
			break;
		case IDM_SHIFTADDCONDITION://增加条件单
			DealAddCondition();
			break;
		case IDM_POSITTIONCOUNTERCLOSE:
		case IDM_POSITIONMARKETCLOSE:
			CloseOrder(LOWORD(wParam));
			break;
		case IDM_POSITIONALLCLOSE:
			if (_TMessageBox_Domodal(m_Hwnd, "提示", L"是否确认清仓", TMB_OKCANCEL))
				g_pShiftTradeApi->CloseAll(m_UserPosition, m_UserParOrder);
			break;
		case IDM_POSITIONFRESH://持仓刷新
			ShowAllPosition();
			break;
		case IDM_PARORDERCANCEL:
			CanCelOrder();
			break;
		case IDM_PARORDERCOUNTERFOLLOW:		//市价、对价跟
		case IDM_PARORDERMARKETFOLLOW:
			ModiftyOrder(LOWORD(wParam));
			break;
		case IDM_PARORDERALLCANCEL:
			g_pShiftTradeApi->CanCelAll(m_UserParOrder);
			break;
		case IDM_PARORDERFRESH://挂单刷新
			ShowAllParOrder();
			break;
		case IDM_CONDITIONNO_SEND:
			DealConditionSend();
			break;
		case IDM_CONDITIONNO_MODIFY:
			DealConditionModify();
			break;
		case IDM_CONDITIONNO_DELETE:
			DealConditionDelete();
			break;
		case IDM_CONDITIONNO_PAUSE:
			DealConditionPause();
			break;
		case IDM_CONDITIONNO_START:
			DealConditionStart();
			break;
		case IDM_CONDITIONNO_ALLPAUSE:
			DealConditionAllPause();
			break;
		case IDM_CONDITIONNO_ALLSTART:
			DealConditionAllStart();
			break;
		case IDM_CONDITIONNO_ALLDELETE:
			DealConditionAllDelete();
			break;
		case IDM_CONDITIONNO_TRIGGER:
			DealShowTrigger();
			break;
		case IDM_CONDITIONNO_NOTTRIGGER:
			DealShowNoTrigger();
			break;
		default:
			break;
		}
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		DealCbChange(wParam);
	}
	else if (LOWORD(wParam) == ID_EDIT_FIRSTORDERQTY && HIWORD(wParam) == EN_CHANGE)
	{
		if (m_sEditFirstOrderQty.GetWindowNumber() < 1)
			m_sEditFirstOrderQty.SetWindowNumber(1);
		SetMarketValue();
	}
	else if (LOWORD(wParam) == ID_EDIT_SECONDORDERQTY && HIWORD(wParam) == EN_CHANGE)
	{
		if (m_sEditSecondOrderQty.GetWindowNumber() < 1)
			m_sEditSecondOrderQty.SetWindowNumber(1);
		SetMarketValue();
	}
	else if (LOWORD(wParam) == ID_EDIT_TOTALQTY && HIWORD(wParam) == EN_CHANGE)
	{
		if (m_sEditTotalQty.GetWindowNumber() < 1)
			m_sEditTotalQty.SetWindowNumber(1);
	}
	else if ((LOWORD(wParam) == ID_EDIT_FIRSTCONTRACT || LOWORD(wParam) == ID_EDIT_SECONDCONTRACT) && HIWORD(wParam) == EN_CHANGE)
	{
		SetMarketValue();
		int id = LOWORD(wParam);
		if (m_bShowCodeList)
		{
			if (!IsWindow(m_popContractList.GetHwnd()))
			{ 
				DWORD dwStyle = WS_CLIPSIBLINGS /*| WS_CHILD*/ | WS_BORDER | WS_POPUP;
				DWORD dwStyleeEx = WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE;
				m_popContractList.CreatePopWin(this, L"popupcodeList", m_Hwnd, dwStyle, dwStyleeEx, 0, 0, 120);
				if (id == ID_EDIT_FIRSTCONTRACT)
					m_popContractList.UpdateData(m_stUserInfo1.Sign);
				else if (id == ID_EDIT_SECONDCONTRACT)
					m_popContractList.UpdateData(m_stUserInfo2.Sign);
			}
			POINT pt = { 0, 0 };
			char sCode[100] = { 0 };
			if (id == ID_EDIT_FIRSTCONTRACT)
			{
				m_bFirstContract = true;
				ClientToScreen(m_editFirstContract.GetHwnd(), &pt);
				m_editFirstContract.GetText(sCode, sizeof(sCode));
			}	
			else if (id == ID_EDIT_SECONDCONTRACT)
			{
				m_bFirstContract = false;
				ClientToScreen(m_editSecondContract.GetHwnd(), &pt);
				m_editSecondContract.GetText(sCode, sizeof(sCode));
			}			
			pt.y += 20;	
			if (!IsWindowVisible(m_popContractList.GetHwnd()))
				SetWindowPos(m_popContractList.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			if (id == ID_EDIT_FIRSTCONTRACT)
				m_popContractList.EditChange(sCode, m_stUserInfo1.Sign);
			else if(id == ID_EDIT_SECONDCONTRACT)
				m_popContractList.EditChange(sCode, m_stUserInfo2.Sign);
		}
	}
	else if ((LOWORD(wParam) == ID_EDIT_FIRSTCONTRACT || LOWORD(wParam) == ID_EDIT_SECONDCONTRACT) && HIWORD(wParam) == EN_KILLFOCUS)
	{
		if (IsWindowVisible(m_popContractList.GetHwnd()) && g_MouseTrack)
			ShowWindow(m_popContractList.GetHwnd(), SW_HIDE);
	}
}
void TShiftPosition::DealConditionSend()
{
	int iRow = m_listCondition.GetSelectItem();
	char cOrderId[21] = { 0 };
	m_listCondition.GetSubItem(iRow, ConditionOrderID, cOrderId, sizeof(cOrderId));
	int iOrderId = atoi(cOrderId);
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.find(iOrderId);
	if (iter == m_mapShiftConditon.end())
		return;
	if (g_pShiftQuoteApi->SendConditionOrder(iter->second))
	{
		iter->second.OrderState = osTrigger;
	}
	ShowAllCondition(m_bIsTrigger);
}
void TShiftPosition::DealConditionPause()
{
	int iRow = m_listCondition.GetSelectItem();
	char cOrderId[21] = { 0 };
	m_listCondition.GetSubItem(iRow, ConditionOrderID, cOrderId, sizeof(cOrderId));
	int iOrderId = atoi(cOrderId);
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.find(iOrderId);
	if (iter == m_mapShiftConditon.end())
		return;
	iter->second.OrderState = osPause;
	ShowAllCondition(m_bIsTrigger);
}
void TShiftPosition::DealConditionStart()
{
	int iRow = m_listCondition.GetSelectItem();
	char cOrderId[21] = { 0 };
	m_listCondition.GetSubItem(iRow, ConditionOrderID, cOrderId, sizeof(cOrderId));
	int iOrderId = atoi(cOrderId);
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.find(iOrderId);
	if (iter == m_mapShiftConditon.end())
		return;
	iter->second.OrderState = osRun;
	ShowAllCondition(m_bIsTrigger);
}
void TShiftPosition::DealConditionDelete()
{
	int iRow = m_listCondition.GetSelectItem();
	char cOrderId[21] = { 0 };
	m_listCondition.GetSubItem(iRow, ConditionOrderID, cOrderId, sizeof(cOrderId));
	int iOrderId = atoi(cOrderId);
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.find(iOrderId);
	if (iter == m_mapShiftConditon.end())
		return;
	m_mapShiftConditon.erase(iter);
	ShowAllCondition(m_bIsTrigger);
}
void TShiftPosition::DealConditionModify()
{
	if (!IsWindow(m_ShiftCondition.GetHwnd()))
		m_ShiftCondition.Regist(&m_mapShiftListInfo, &m_mapShiftConditon, &m_mapUserInfo);
	int iRow = m_listCondition.GetSelectItem();
	char cOrderId[21] = { 0 };
	m_listCondition.GetSubItem(iRow, ConditionOrderID, cOrderId, sizeof(cOrderId));
	int iOrderId = atoi(cOrderId);
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.find(iOrderId);
	if (iter == m_mapShiftConditon.end())
		return;
	iter->second.OrderState = osPause;
	m_ShiftCondition.ModifyCondition(iOrderId);
}
void TShiftPosition::DealConditionAllPause()
{
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.begin();
	for (; iter != m_mapShiftConditon.end(); iter++)
		iter->second.OrderState = osPause;
	ShowAllCondition(m_bIsTrigger);
}
void TShiftPosition::DealConditionAllStart()
{
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.begin();
	for (; iter != m_mapShiftConditon.end(); iter++)
		iter->second.OrderState = osRun;
	ShowAllCondition(m_bIsTrigger);
}
void TShiftPosition::DealConditionAllDelete()
{
	bool IsTrigger = false;
	if (GetMenuState(m_menuConditon, IDM_CONDITIONNO_NOTTRIGGER, MF_BYCOMMAND) == MF_CHECKED)
		IsTrigger = false;
	else
		IsTrigger = true;
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.begin();
	for (; iter != m_mapShiftConditon.end(); )
	{
		std::map<string, ShiftListInfo>::iterator listIter = m_mapShiftListInfo.find(iter->second.ShiftContractNo);
		if (iter->second.OrderState == osTrigger && IsTrigger)
		{
			if (listIter == m_mapShiftListInfo.end() || listIter->second.OrderId.size() == 0)
			{
				m_mapShiftConditon.erase(iter++);
				continue;
			}
// 			//删除移仓列表中的条件单信息：
// 			std::vector<int>::iterator Orderiter = listIter->second.OrderId.begin();
// 			while (Orderiter != listIter->second.OrderId.end())
// 			{
// 				if (*Orderiter == iter->second.OrderId)
// 					Orderiter = listIter->second.OrderId.erase(Orderiter);
// 				else
// 					Orderiter++;
// 			}
			m_mapShiftConditon.erase(iter++);
		}
		else if (IsTrigger == false && (iter->second.OrderState == osRun || iter->second.OrderState == osPause))
		{
			if (listIter == m_mapShiftListInfo.end() || listIter->second.OrderId.size() == 0)
			{

				m_mapShiftConditon.erase(iter++);
				continue;
			}
// 			//删除移仓列表中的条件单信息：
// 			std::vector<int>::iterator Orderiter = listIter->second.OrderId.begin();
// 			while (Orderiter != listIter->second.OrderId.end())
// 			{
// 				if (*Orderiter == iter->first)
// 					Orderiter = listIter->second.OrderId.erase(Orderiter);
// 				else
// 					Orderiter++;
// 			}
// 			std::vector<int>::iterator Orderiter = std::find(listIter->second.OrderId.begin(), listIter->second.OrderId.end(), iter->second.OrderId);
// 			if (Orderiter != listIter->second.OrderId.end())
// 				listIter->second.OrderId.erase(Orderiter);
			m_mapShiftConditon.erase(iter++);
		}
		else
			iter++;
	}	
	ShowAllCondition(m_bIsTrigger);
}
void TShiftPosition::DealShowNoTrigger()
{
	CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_NOTTRIGGER, MF_CHECKED);
	CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_TRIGGER, MF_UNCHECKED);
	m_bIsTrigger = false;
	ShowAllCondition(m_bIsTrigger);
}
void TShiftPosition::DealShowTrigger()
{
	CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_NOTTRIGGER, MF_UNCHECKED);
	CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_TRIGGER, MF_CHECKED);
	m_bIsTrigger = true;
	ShowAllCondition(m_bIsTrigger);
}

void TShiftPosition::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_LISTSHIFT:
		DealShiftList(lParam);
		break;
	case ID_LISTPOSITION:
		DealPositionList(lParam);
		break;
	case ID_LISTPARORDER:
		DealParOrderList(lParam);
		break;
	case ID_LISTCONDITION:
		DealCondition(lParam);
		break;
	default:
		break;
	}
}
void TShiftPosition::DealPositionList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem == -1)
			EnableMenu(p->hdr.idFrom, false);
		ShowMenu(m_menuPosition);
	}
	else if (p->hdr.code == NM_CLICK)
	{
		char Code[51];
		if (m_listPosition.GetSelectItem() == -1)
			return;
		TPosition* pst = (TPosition*)m_UserPosition.at(m_listPosition.GetSelectItem());
		sprintf_s(Code, "%s %s %s", pst->ExchangeNo, pst->CommodityNo, pst->ContractNo);
		int AvaQty = 0, TAvaQty = 0, iRow = 0;
		CConvert::GetAvailableQty(pst, AvaQty, TAvaQty);
		m_sEditFirstOrderQty.SetWindowNumber(AvaQty);//设置数量开平量
		SetContractNo(Code, 1);

		if (pst->Direct == dBuy)
			m_cbDirect.SetSelect(1);
		else
			m_cbDirect.SetSelect(0);
	}
}
void TShiftPosition::DealShiftList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_mapShiftListInfo.size() || p->iItem == -1)
			EnableMenuItem(m_menuShift, IDM_SHIFTADDCONDITION, MF_GRAYED);
		ShowMenu(m_menuShift);
	}	
	else if (p->hdr.code == NM_CLICK)
	{
		char shiftNo[51] = { 0 };
		m_listShift.GetSubItem(m_indexShift, 0, shiftNo, sizeof(shiftNo));
		std::map<string, ShiftListInfo>::iterator iter = m_mapShiftListInfo.find(shiftNo);
		if (iter != m_mapShiftListInfo.end())
		{
			m_bShowCodeList = false;
			m_editFirstContract.SetText(iter->second.Code1);
			m_editSecondContract.SetText(iter->second.Code2);
			m_sEditFirstOrderQty.SetWindowNumber(iter->second.OrderQty1);
			m_sEditSecondOrderQty.SetWindowNumber(iter->second.OrderQty2);
			m_bShowCodeList = true;
		}
	}
}
void TShiftPosition::DealParOrderList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem == -1)
			EnableMenu(p->hdr.idFrom, false);
		ShowMenu(m_menuParOrder);
	}
}
void TShiftPosition::DealCondition(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem == -1)
			EnableMenu(p->hdr.idFrom, false);
		ShowMenu(m_menuConditon);
	}
}

void TShiftPosition::OnDealLog(TOrder* p)
{
	bool flag = false;

	if (p->OrderState == osQueued)
	{
		if (m_logCount.Orderid != p->OrderID)
		{
			m_logCount.Orderid = p->OrderID;
			m_logCount.iCount = 0;
			flag = true;
		}
		else if (m_logCount.Orderid == p->OrderID)
		{
			if (m_logCount.iCount)
				flag = false;
			else
				flag = true;
		}
	}
	else if (p->OrderState == osFilled || p->OrderState == osFail)
	{
		for (vector<int>::iterator iter = m_vectorOrderId.begin(); iter != m_vectorOrderId.end(); )
		{
			if (*iter == p->OrderID)
			{
				m_logCount.iCount = 0;
				flag = true;
				iter = m_vectorOrderId.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	else
		flag = true;
	
	if (flag)
	{
		char loginfo[100] = { 0 };
		char szText[20] = { 0 };
		char szText2[20] = { 0 };
		char szPrice[20] = { 0 };
		char szMatchPrice[20] = { 0 };
		if (g_pTradeData)
		{
			const SCommodity* pCom = g_pTradeData->GetCommodity(p);
			if (pCom == NULL)
				return;
			DoubleToChar(pCom->PricePrec, p->OrderPrice, szPrice);
			DoubleToChar(pCom->PricePrec, p->MatchPrice, szMatchPrice);
		}
		if (p->Direct == dBuy && p->Offset == oOpen)
			sprintf_s(szText, "买开%d手", p->OrderQty);
		else if (p->Direct == dSell && p->Offset == oOpen)
			sprintf_s(szText, "卖开%d手", p->OrderQty);
		else if (p->Direct == dBuy && p->Offset == oCover)
			sprintf_s(szText, "买平%d手", p->OrderQty);
		else if (p->Direct == dSell && p->Offset == oCover)
			sprintf_s(szText, "卖平%d手", p->OrderQty);
		else if (p->Direct == dSell && p->Offset == oCoverT)
			sprintf_s(szText, "卖平今%d手", p->OrderQty);
		else if (p->Direct == dBuy && p->Offset == oCoverT)
			sprintf_s(szText, "买平今%d手", p->OrderQty);
		TOrderState2Char(p->OrderState, p->StrategyType, szText2, sizeof(szText2));

		char Time[20] = { 0 };
		vector<string> vstrTime = CConvert::SplitString(p->InsertDateTime, ' ');
		if (vstrTime.size() > 1)
			sprintf_s(Time, "%s", vstrTime.at(1).c_str());
		sprintf_s(loginfo, "%s%s:%s%s %s 委托价:%s (%s)", p->UserNo, "合约", p->CommodityNo, p->ContractNo, szText, szPrice, Time);
		m_RichEdit.AddString(loginfo, sizeof(loginfo), g_ColorRefData.g_ColorBlack);
		if (p->OrderState == osAccept)
		{
			sprintf_s(loginfo, "状态:%s", szText2);
			if (p->Direct == dBuy)
				m_RichEdit.AddString(loginfo, sizeof(loginfo), g_ColorRefData.g_ColorTextRed);
			else if (p->Direct == dSell)
				m_RichEdit.AddString(loginfo, sizeof(loginfo), g_ColorRefData.g_ColorTextGreen);
			m_logCount.iCount++;
			return;
		}
		if (p->ErrorCode == 0)
		{
			sprintf_s(loginfo, "状态:%s  成交手数：%d  成交价:%s", szText2, p->MatchQty, szMatchPrice);
			if (p->Direct == dBuy)
				m_RichEdit.AddString(loginfo, sizeof(loginfo), g_ColorRefData.g_ColorTextRed);
			else if (p->Direct == dSell)
				m_RichEdit.AddString(loginfo, sizeof(loginfo), g_ColorRefData.g_ColorTextGreen);
		}
		else
		{
			sprintf_s(loginfo, "%s:%s", szText2, p->ErrorText);
			m_RichEdit.AddString(loginfo, sizeof(loginfo), g_ColorRefData.GetColorBlack());
		}

		m_logCount.iCount++;
	}	
}
//合约按钮
void TShiftPosition::OnSearchCode(LPARAM lParam)
{
	if (!IsWindow(m_popContractCode.GetHwnd()))
		m_popContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0);

	POINT pt = { 0, 0 };
	if ((int)lParam == ID_BTN_FIRSTCONTRACT)
	{
		m_popContractCode.UpdateData(m_stUserInfo1.Sign);
		m_bFirstContract = true;
		ClientToScreen(m_editFirstContract.GetHwnd(), &pt);
	}		
	else
	{
		m_popContractCode.UpdateData(m_stUserInfo2.Sign);
		m_bFirstContract = false;
		ClientToScreen(m_editSecondContract.GetHwnd(), &pt);
	}	
	pt.y += WidgetHeight;
	SetWindowPos(m_popContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}
void TShiftPosition::SetContractNo(string sCode)
{
// 	string::size_type pos = sCode.find(" ");
// 	if (pos != string::npos)
// 	{
// 		string sContract = sCode.substr(++pos, string::npos);
// 		if (m_bFirstContract)
// 			m_editFirstContract.SetText(sContract.c_str());
// 		else
// 			m_editSecondContract.SetText(sContract.c_str());
// 	}	
	if (m_bFirstContract)
		SetContractNo(sCode, 1);
	else
		SetContractNo(sCode, 2);
}
void __cdecl TShiftPosition::OnDisconnect(const TSocketDisconnectReason ErrorCode, const TLogin * pLogin)
{
	PostMessage(m_Hwnd, WM_ONRESETUSERNO, 0, 0);
}
void __cdecl TShiftPosition::OnInitProgress(const int ErrorCode, const TradeInitProgress CurrProgress, const TLoginBase * pLoginBase)
{
	if (CurrProgress == tipReady)
		PostMessage(m_Hwnd, WM_ONRESETUSERNO, 0, 0);
}
void __cdecl TShiftPosition::OnOrder(const TOrder * pOrder)
{
	if (pOrder->OrderState == osSended)
	{
		m_vectorOrderId.push_back(pOrder->OrderID);
	}
	PostMessage(m_Hwnd, WM_ONORDER, (WPARAM)pOrder, 0);
}
void __cdecl TShiftPosition::OnMatch(const TMatch * pMatch)
{
	PostMessage(m_Hwnd, WM_ONMATCH, (WPARAM)pMatch, 0);
}
void __cdecl TShiftPosition::OnPositionTotal(const TTotalPosition * pPosition)
{
	PostMessage(m_Hwnd, WM_ONPOSITION, (WPARAM)pPosition, 0);
}
void TShiftPosition::InitUserNo()
{
	
}
void TShiftPosition::SetUserInfo()
{
	m_mapUserInfo.clear();

	TContainer ctUser;
	g_pTradeData->GetAllUser(ctUser, "");
	uint iCount = ctUser.size();
	for (uint i = 0; i < iCount; i++)
	{
		TUserInfo* pUser = (TUserInfo*)ctUser.at(i);
		if (!pUser->pLogin)
			continue;

		//保存客户信息
		std::map<std::string, TUserInfo*>::iterator iter = m_mapUserInfo.find(pUser->UserNo);
		if (iter == m_mapUserInfo.end())
			m_mapUserInfo.insert(make_pair(pUser->UserNo, pUser));
		if (i == 0)
		{
			memcpy(&m_stUserInfo1, pUser, sizeof(TUserInfo));
			memcpy(&m_stUserInfo2, pUser, sizeof(TUserInfo));
		}	
	}
}
void TShiftPosition::InitTradeData()
{
	m_PositionContainer.clear();
	m_UserParOrder.clear();
	if (g_pTradeData)
	{
		g_pTradeData->GetAllPositionTotal(m_PositionContainer);
		g_pTradeData->GetParOrder(m_ParOrderContainer, ftNone);
		GetMaxOrderId();								//过滤日志多次返回的消息
	}
}
int TShiftPosition::GetMaxOrderId()
{
	TContainer Order;
	TContainer UserOrder;
	g_pTradeData->GetAllOrder(Order, ftNone, "");
	for (int i = 0; i < Order.size(); i++)
	{
		TOrder* pst = (TOrder*)Order.at(i);
		if (!pst) continue;
		if (IsUserNoInfo(pst->UserNo, pst->Sign))
			UserOrder.push_back(pst);
	}
	if (UserOrder.size() == 0)
		m_logCount.Orderid = 0;
	else
	{
		TOrder* pst = (TOrder*)UserOrder.at(UserOrder.size() - 1);
		m_logCount.Orderid = pst->OrderID;
	}
	return 0;
}
void TShiftPosition::ShowAllParOrder()
{
	//默认挂单信息为第一个账号信息
	ShowWindow(m_listParOrder.GetListHwnd(), SW_HIDE);
	GetParOrder(m_stUserInfo1.UserNo, m_stUserInfo1.Sign);
	m_listParOrder.DeleteAllItems();
	for (int i = 0; i < m_UserParOrder.size(); i++)
	{
		//显示全部挂单信息；
		TOrder* pst = (TOrder*)m_UserParOrder.at(i);
		UpdateOneParOrder(i, pst);
	}
	ShowWindow(m_listParOrder.GetListHwnd(), SW_SHOW);
}
void TShiftPosition::ShowAllPosition()
{
	//默认持仓信息为第一个账号信息
	GetPosition(m_stUserInfo1.UserNo, m_stUserInfo1.Sign);
	m_listPosition.DeleteAllItems();
	for (int i = 0; i < m_UserPosition.size(); i++)
	{
		//显示全部持仓信息；
		TPosition* pst = (TPosition*)m_UserPosition.at(i);
		UpdateOnePosition(i, pst);
		//行情订阅也要改为第一个账户信息
		SubQuoteFromPos(pst);
	}
}
void TShiftPosition::SubQuoteFromPos(const TPosition* pst)
{
	char Code[50] = { 0 };
	sprintf_s(Code, "%s %s %s", pst->ExchangeNo, pst->CommodityNo, pst->ContractNo);
	TContractKey stContract;
	memset(&stContract, 0, sizeof(TContractKey));
	memcpy(stContract.Sign, m_stUserInfo1.Sign, sizeof(stContract.Sign));
	CConvert::TContractNoToTContractKey(Code, stContract);
	char sQuote[101] = { 0 };
	CConvert::TradeContractToQContractNoType(stContract, sQuote);
	if (g_pStarApi)
		g_pStarApi->SubQuote(&sQuote,1, this);												//行情订阅
}
void TShiftPosition::SubQuoteFromShift()
{
	//移仓单行情订阅也为第一个账户的sign
	std::map<string, ShiftListInfo>::iterator iter = m_mapShiftListInfo.begin();
	for (; iter != m_mapShiftListInfo.end(); iter++)
	{
		g_pShiftQuoteApi->QuoteShiftContract(iter->second, m_stUserInfo1.Sign, m_stUserInfo2.Sign);
	}
}
void TShiftPosition::UpdateOnePosition(int i ,TPosition* pst)
{
	if (!pst ||pst->PositionQty ==0)
		return;
	int iPresicion = 0;
	int iCol = 0;
	char szText[51] = { 0 };
	sprintf_s(szText, "%s %s %s", pst->ExchangeNo, pst->CommodityNo, pst->ContractNo);
	iPresicion = GetMiniPricePresicion(szText);
	//品种名称
	//CConvert::UTF8ToUnicode(pst->pCommodity->CommodityName).c_str;
	m_listPosition.AddItem(CConvert::WcharToChar(CConvert::UTF8ToUnicode(pst->pCommodity->CommodityName).c_str()), i, iCol++);
	//合约号
	sprintf_s(szText, "%s%s", pst->CommodityNo, pst->ContractNo);
	m_listPosition.AddItem(szText, i, iCol++);
	//买入方向
	TDirect2CharByPosition(pst->Direct, szText, sizeof(szText));
	if (strcmp(szText, "多头") == 0)
		m_listPosition.AddItem(szText, i, iCol++, DT_LEFT, g_ColorRefData.GetColorTextRed());
	else
		m_listPosition.AddItem(szText, i, iCol++, DT_CENTER, g_ColorRefData.GetColorTextGreen());
	//手数
	m_listPosition.AddItem(int(pst->PositionQty), i, iCol++);
	//可用
	int AvaQty = 0;
	int TAvaQty = 0;
	CConvert::GetAvailableQty(pst, AvaQty, TAvaQty);
	m_listPosition.AddItem(AvaQty, i, iCol++);
	//持仓均价
	m_listPosition.AddItem(pst->PositionPrice, i, iCol++, DT_CENTER,-1, 0 );
	//逐笔浮盈
	if (pst->FloatProfitTBT >= 0)
		m_listPosition.AddItem(pst->FloatProfitTBT, i, iCol++, iPresicion, DT_CENTER, g_ColorRefData.GetColorTextRed());
	else
		m_listPosition.AddItem(pst->FloatProfitTBT, i, iCol++, iPresicion, DT_CENTER, g_ColorRefData.GetColorTextGreen());
	//价值
	const SCommodity* pCommodity = g_pTradeData->GetCommodity(pst);
	if (!pCommodity)
		return;
	double Price = 0.0;
	CConvert::GetPrice(pst, pst->Direct, LastPrice, Price);
	double MarketValue = pCommodity->TradeDot * Price * pst->PositionQty;
	m_listPosition.AddItem(MarketValue, i, iCol++, pCommodity->PricePrec,DT_CENTER, -1);

	//保证金
	m_listPosition.AddItem(pst->Deposit, i, iCol++, iPresicion, DT_CENTER);
	//盯市浮盈
	if (pst->FloatProfit >= 0)
		m_listPosition.AddItem(pst->FloatProfit, i, iCol++, iPresicion, DT_CENTER, g_ColorRefData.GetColorTextRed());
	else
		m_listPosition.AddItem(pst->FloatProfit, i, iCol++, iPresicion, DT_CENTER, g_ColorRefData.GetColorTextGreen());
	//今手数
	m_listPosition.AddItem(int(pst->PositionQty - pst->PrePositionQty), i, iCol++);
	//今可用
	m_listPosition.AddItem(TAvaQty, i, iCol++);
	
}
void TShiftPosition::UpdateOneParOrder(int i, TOrder* pst)
{
	if (!pst)
		return;
	int iCol = 0;
	char szText[51] = { 0 };
	//时间
	m_listParOrder.AddItem(pst->InsertDateTime, i, iCol++);
	//合约号
	sprintf_s(szText, "%s%s", pst->CommodityNo, pst->ContractNo);
	m_listParOrder.AddItem(szText, i, iCol++);
	//状态
	TOrderState2Char(pst->OrderState, pst->StrategyType, szText, sizeof(szText));
	m_listParOrder.AddItem(szText, i, iCol++);
	//买卖
	TDirect2Char(pst->Direct, szText, sizeof(szText));
	m_listParOrder.AddItem(szText, i, iCol++);
	//开平
	TOffset2Char(pst->Offset, szText, sizeof(szText));
	m_listParOrder.AddItem(szText, i, iCol++);
	//委托价
	m_listParOrder.AddItem(pst->OrderPrice, i, iCol++);
	//委托量
	m_listParOrder.AddItem((int)pst->OrderQty, i, iCol++);
	//可撤
	m_listParOrder.AddItem((int)(pst->OrderQty - pst->MatchQty), i, iCol++);
	//已成交
	m_listParOrder.AddItem((int)pst->MatchQty, i, iCol++);
}
void TShiftPosition::ShowAllShift()
{
	SubQuoteFromShift();
}
void TShiftPosition::ShowOneShiftList(ShiftListInfo shift)
{
	//找到更新的行数;
	int i = 0;
	std::map<string, ShiftListInfo>::iterator iter = m_mapShiftListInfo.begin();
	for (; iter != m_mapShiftListInfo.end(); iter++, i++)
	{
		if (strcmp(shift.ShiftContractNo, iter->second.ShiftContractNo) == 0)
			break;
	}
	//刷新数据；
	int iCol = 1;
	int iPresicion = 0;
	if (shift.LinkCondition == LcDiv)
		iPresicion = 4;
	else
	{
		int iPresicion1 = GetMiniPricePresicion(shift.Code1);
		int iPresicion2 = GetMiniPricePresicion(shift.Code2);
		iPresicion = iPresicion1 > iPresicion2 ? iPresicion1 : iPresicion2;
	}
	if (shift.Long_CounterDif < 0)
		m_listShift.AddItem(shift.Long_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Long_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
	if (shift.Long_QueueDif < 0)
		m_listShift.AddItem(shift.Long_QueueDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Long_QueueDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
	if (shift.Short_CounterDif < 0)
		m_listShift.AddItem(shift.Short_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Short_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
	if (shift.Short_QueueDif < 0)
		m_listShift.AddItem(shift.Short_QueueDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Short_QueueDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.LastDif < 0)
		m_listShift.AddItem(shift.LastDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.LastDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.AverageDif < 0)
		m_listShift.AddItem(shift.AverageDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.AverageDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.Long_CounterDif < 0)
		m_listShift.AddItem(shift.Long_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Long_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
	if (shift.OpenDif < 0)
		m_listShift.AddItem(shift.OpenDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.OpenDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.Long_MarketDif < 0)
		m_listShift.AddItem(shift.Long_MarketDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Long_MarketDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.Short_MarketDif < 0)
		m_listShift.AddItem(shift.Short_MarketDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Short_MarketDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
}
void TShiftPosition::ShowAllCondition(bool IsTrigger)
{
	m_listCondition.DeleteAllItems();
	std::map<int, ShiftCondition>::iterator iter = m_mapShiftConditon.begin();
	int iRow = 0;
	char szText[51] = { 0 };
	for (; iter != m_mapShiftConditon.end(); iter++)
	{
		if (iter->second.OrderState == osTrigger && IsTrigger)
		{
			UpdateOneCondition(iRow, iter->second);
			iRow++;
		}
		else if ((iter->second.OrderState == osRun || iter->second.OrderState == osPause) && IsTrigger == false)
		{
			UpdateOneCondition(iRow, iter->second);
			iRow++;
		}
		else
			continue;
	}
	if (IsTrigger)
	{
 		 CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_TRIGGER, MF_CHECKED);
		 CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_NOTTRIGGER, MF_UNCHECKED);
	}
	else
	{
		CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_TRIGGER, MF_UNCHECKED);
		CheckMenuItem(m_menuConditon, IDM_CONDITIONNO_NOTTRIGGER, MF_CHECKED);
	}
}
void TShiftPosition::UpdateOneCondition(int iRow, ShiftCondition condition)
{
	int iCol = 0;
	char szText[51] = { 0 };
	m_listCondition.AddItem(condition.UserNo, iRow, iCol++, DT_CENTER);
	m_listCondition.AddItem(condition.UserNo2, iRow, iCol++, DT_CENTER);

	OrderStateToChar(condition.OrderState, szText, sizeof(szText));
	m_listCondition.AddItem(szText, iRow, iCol++, DT_CENTER);

	m_listCondition.AddItem(condition.ShiftContractNo, iRow, iCol++, DT_CENTER);

	DiffPriceTypeToChar(condition.iPriceCondition, szText, sizeof(szText));
	m_listCondition.AddItem(szText, iRow, iCol++, DT_CENTER);

	PriceConditionToChar(condition.iTriggerCondition, szText, sizeof(szText));
	m_listCondition.AddItem(szText, iRow, iCol++,DT_CENTER);

	vector<string> vt = CConvert::SplitString(condition.ShiftContractNo, ' ');
	if (vt.size() < 7)
		return;
	int iPresicion = 0;
	if (atoi(vt.at(3).c_str()) == LcDiv)
		iPresicion = 4;
	else
		GetMiniPrice(condition.ShiftContractNo, iPresicion, m_stUserInfo1.UserNo, m_stUserInfo2.UserNo);
	if (condition.OrderPrice >=0)
		m_listCondition.AddItem(condition.OrderPrice, iRow, iCol++, iPresicion, DT_LEFT, g_ColorRefData.GetColorTextRed());
	else
		m_listCondition.AddItem(condition.OrderPrice, iRow, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());

	OrderTypeToChar(condition.OrderType1, condition.OrderType2, szText, sizeof(szText));
	m_listCondition.AddItem(szText, iRow, iCol++, DT_CENTER);

	m_listCondition.AddItem(condition.OrderId, iRow, iCol++);

	if (condition.iDirect == BuyCoverSellOpen)
		strcpy_s(szText, sizeof(szText), "买平-卖开");
	else if(condition.iDirect == SellCoverBuyOpen)
		strcpy_s(szText, sizeof(szText), "卖平-买开");
	else if(condition.iDirect == BuyOpenSellCover)
		strcpy_s(szText, sizeof(szText), "买开-卖平");
	else
		strcpy_s(szText, sizeof(szText), "卖开-买平");
	m_listCondition.AddItem(szText, iRow, iCol++, DT_CENTER);

	if(condition.TotalQty == 0)
		strcpy_s(szText, sizeof(szText), "默认策略");
	else
		strcpy_s(szText, sizeof(szText), "螺旋策略");
	m_listCondition.AddItem(szText, iRow, iCol++, DT_CENTER);
}
int TShiftPosition::GetMiniPricePresicion(char* code1)
{
	//不同账户的最小变动价是一样的
	TContractKey stContract;
	memset(&stContract, 0, sizeof(TContractKey));
	memcpy(&stContract.Sign, m_stUserInfo1.Sign, sizeof(stContract.Sign));
	CConvert::TContractNoToTContractKey(code1, stContract);
	const SCommodity* pst = g_pTradeData->GetCommodity(&stContract);
	if (!pst)
		return 0;
	return pst->PricePrec;
}
double TShiftPosition::GetMiniPrice(char* ShiftContracNo, int& iPresicion, char* UserNo, char* UserNo2)
{
	vector<string> vtCode = CConvert::SplitString(ShiftContracNo, ' ');
	if (vtCode.size() < 7)
		return 0;
	if (strcmp(vtCode.at(3).c_str(), "/") == 0)
	{
		iPresicion = 4;
		return 0.0001;
	}

	std::map<string, ShiftListInfo>::iterator iter = m_mapShiftListInfo.find(ShiftContracNo);
	if (iter == m_mapShiftListInfo.end())
	{
		_TMessageBox_Domodal(m_Hwnd, "提示", L"移仓列表没有该合约！", TMB_OK);
		return 1;
	}

	TContractKey stContract[2];
	memset(&stContract[0], 0, sizeof(TContractKey));
	memset(&stContract[1], 0, sizeof(TContractKey));
	std::map<std::string, TUserInfo*>::iterator UserIter = m_mapUserInfo.find(UserNo);
	std::map<std::string, TUserInfo*>::iterator UserIter2 = m_mapUserInfo.find(UserNo2);
	if (UserIter == m_mapUserInfo.end() || UserIter2 == m_mapUserInfo.end())
	{
		WCHAR msgText[60] = { 0 };
		if (UserIter == m_mapUserInfo.end())
			swprintf_s(msgText,L"没有账户为 %s 的用户", UserNo);
		else if(UserIter2==m_mapUserInfo.end())
			swprintf_s(msgText, L"没有账户为 %s 的用户", UserNo2);
		_TMessageBox_Domodal(m_Hwnd, "提示", msgText, TMB_OK);
		return 1;
	}
	memcpy(stContract[0].Sign, UserIter->second->Sign, sizeof(stContract[0].Sign));
	CConvert::TContractNoToTContractKey(iter->second.Code1, stContract[0]);
	const SCommodity* pst = g_pTradeData->GetCommodity(&stContract[0]);
	if (!pst)
		return 0;
	double MiniPrice1 = 0.0;
	if (pst->PriceDeno == 0)//最小变动价是1
		MiniPrice1 = 1;
	else if (pst->PriceDeno == 1)//最小变动价是UpperTick
		MiniPrice1 = pst->PriceNume;
	int iPresicion1 = pst->PricePrec;

	memcpy(stContract[1].Sign, UserIter->second->Sign, sizeof(stContract[1].Sign));
	CConvert::TContractNoToTContractKey(iter->second.Code2, stContract[1]);
	pst = g_pTradeData->GetCommodity(&stContract[1]);
	if (!pst)
		return 0;
	double MiniPrice2 = 0.0;
	if (pst->PriceDeno == 0)//最小变动价是1
		MiniPrice2 = 1;
	else if (pst->PriceDeno == 1)//最小变动价是UpperTick
		MiniPrice2 = pst->PriceNume;
	int iPresicion2 = pst->PricePrec;
	iPresicion = iPresicion1 > iPresicion2 ? iPresicion1 : iPresicion2;
	return (MiniPrice1) < (MiniPrice2) ? (MiniPrice1) : (MiniPrice2);
}
void TShiftPosition::UpdateOneShiftList(int i, ShiftListInfo shift)
{
	int iCol = 0;
	int iPresicion = 0;
	if (shift.LinkCondition == LcDiv)
	{
		iPresicion = 4;
	}
	else
	{
		int iPresicion1 = GetMiniPricePresicion(shift.Code1);
		int iPresicion2 = GetMiniPricePresicion(shift.Code2);
		iPresicion = iPresicion1 > iPresicion2 ? iPresicion1 : iPresicion2;
	}
	m_listShift.AddItem(shift.ShiftContractNo, i, iCol++);
	if (shift.Long_CounterDif < 0)
		m_listShift.AddItem(shift.Long_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Long_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
	if (shift.Long_QueueDif < 0)
		m_listShift.AddItem(shift.Long_QueueDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Long_QueueDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
	if (shift.Short_CounterDif < 0)
		m_listShift.AddItem(shift.Short_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Short_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
	if (shift.Short_QueueDif < 0)
		m_listShift.AddItem(shift.Short_QueueDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Short_QueueDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.LastDif < 0)
		m_listShift.AddItem(shift.LastDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.LastDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.AverageDif < 0)
		m_listShift.AddItem(shift.AverageDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.AverageDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.Long_CounterDif < 0)
		m_listShift.AddItem(shift.Long_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Long_CounterDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
	if (shift.OpenDif < 0)
		m_listShift.AddItem(shift.OpenDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.OpenDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.Long_MarketDif < 0)
		m_listShift.AddItem(shift.Long_MarketDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Long_MarketDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());

	if (shift.Short_MarketDif < 0)
		m_listShift.AddItem(shift.Short_MarketDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorTextGreen());
	else
		m_listShift.AddItem(shift.Short_MarketDif, i, iCol++, iPresicion, DT_RIGHT, g_ColorRefData.GetColorBlack());
}
void TShiftPosition::GetShowShiftName(ShiftListInfo shift, string& name, int size)
{
	
}
void TShiftPosition::UpdateParOrderList(WPARAM wParam, LPARAM lParam)
{
	UpdateAvaQty(wParam);
	m_ParOrderContainer.clear();
	g_pTradeData->GetParOrder(m_ParOrderContainer);
	ShowAllParOrder();
}
void TShiftPosition::UpdatePositionList(WPARAM wParam, LPARAM lParam)
{	
	TPosition* pst = (TPosition*)wParam;
	if (pst == NULL) return;
	bool bUpdate = false;
	if (pst->PositionQty)							//（新品种）买进的时候持仓列表增加
	{
		uint size = m_UserPosition.size();//保存之前的size
		m_PositionContainer.push_back(pst);
		if (IsUserNoInfo(pst->UserNo, pst->Sign))
			m_UserPosition.push_back(pst);

		if (size == m_UserPosition.size() && IsAddOrUpdate(pst) || size < m_UserPosition.size())
			bUpdate = true;
	}
	else                                            //删除已经平仓的数量或一行
	{
		m_PositionContainer.del(pst);
		if (IsUserNoInfo(pst->UserNo, pst->Sign))
			m_UserPosition.del(pst);
		bUpdate = true;
	}
	if (bUpdate)
	{
		ShowWindow(m_listPosition.GetListHwnd(), SW_HIDE);
		ShowWindow(m_listPosition.GetHeaderHwnd(), SW_HIDE);
		//刷新列表
		m_listPosition.DeleteAllItems();
		for (int i = 0; i < m_UserPosition.size(); i++)
		{
			pst = (TPosition*)m_UserPosition.at(i);
			UpdateOnePosition(i, pst);
		}
		ShowWindow(m_listPosition.GetListHwnd(), SW_SHOW);
		ShowWindow(m_listPosition.GetHeaderHwnd(), SW_SHOW);
	}
}
void TShiftPosition::UpdateShiftList()
{
	m_listShift.SelectItem(m_indexShift);
	m_listShift.DeleteAllItems();
	std::map<string, ShiftListInfo>::iterator iter = m_mapShiftListInfo.begin();
	int i = 0;
	for (; iter != m_mapShiftListInfo.end(); iter++)
	{
		UpdateOneShiftList(i++, iter->second);
	}
	m_listShift.SelectItem(m_indexShift);
}
LRESULT TShiftPosition::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_white;
}
void TShiftPosition::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	int oldDc = SaveDC(memdc.GetHdc());
	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	RECT rcClient;
	GetClientRect(m_Hwnd, &rcClient);

	HBRUSH hbrBorder = CreateSolidBrush(RGB(100, 100, 100));
	InflateRect(&rcClient, -1, -1);
	FrameRect(memdc.GetHdc(), &rcClient, hbrBorder);
	InflateRect(&rcClient, -1, -1);
	FrameRect(memdc.GetHdc(), &rcClient, hbrBorder);

	RECT rcCaption = { rcClient.left, rcClient.top, rcClient.right, CAPTION_H };

	HBRUSH hbrCaption = CreateSolidBrush(RGB(100, 100, 100));
	FillRect(memdc.GetHdc(), &rcCaption, hbrCaption);

	rcClient.top = CAPTION_H;
	FillRect(memdc.GetHdc(), &rcClient, g_ColorRefData.g_brush_white);

	rcCaption.left = 8;
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord13());
	SetTextColor(memdc.GetHdc(), RGB(238, 238, 238));
	DrawText(memdc.GetHdc(), L"移仓", -1, &rcCaption, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	DrawClose(&memdc);
	DrawMini(&memdc);
	DrawText(memdc.GetHdc(), L"设置", -1, &m_rcSet, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);


	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}
void TShiftPosition::DrawClose(TMemDC *pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(238, 238, 238));
	HPEN hOldPen = (HPEN)SelectObject(pMemDC->GetHdc(), hPen);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.top, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right + 1, m_rcClose.bottom + 1);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.bottom, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right + 1, m_rcClose.top - 1);

	SelectObject(pMemDC->GetHdc(), hOldPen);
	DeleteObject(hPen);
	RestoreDC(pMemDC->GetHdc(), iold);
}

void TShiftPosition::DrawMini(TMemDC *pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(238, 238, 238));
	HPEN hOldPen = (HPEN)SelectObject(pMemDC->GetHdc(), hPen);

	MoveToEx(pMemDC->GetHdc(), m_rcMini.left, m_rcMini.top + (m_rcMini.bottom - m_rcMini.top) / 2, 0);
	LineTo(pMemDC->GetHdc(), m_rcMini.right, m_rcMini.top + (m_rcMini.bottom - m_rcMini.top) / 2);


	SelectObject(pMemDC->GetHdc(), hOldPen);
	DeleteObject(hPen);
	RestoreDC(pMemDC->GetHdc(), iold);
}

void TShiftPosition::OnCreate()
{
	CreateWidget();
	SetWidgetPos();
	InitWidget();

	SetTimer(m_Hwnd, 0, 1000, NULL);
}


void TShiftPosition::CreateWidget()
{
	//日志Edit;
	m_RichEdit.Create(m_Hwnd, WS_BORDER, 0, ID_RICHEDIT);
	m_RichEdit.SetFont(g_FontData.g_FontWord13);

	//TabControl
	std::vector<std::string> tabstr;
	tabstr.push_back("移仓合约");
	tabstr.push_back("条件单");
	m_tabShiftPosition.SetData(tabstr);
	m_tabShiftPosition.Create(m_Hwnd);


	//创建列表
	m_listShift.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_LISTSHIFT);
	m_listParOrder.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_LISTPARORDER);	//挂单
	m_listPosition.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_LISTPOSITION);	//持仓
	m_listCondition.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, ID_LISTCONDITION);


	//创建列表右键
	m_menuPosition = CreatePopupMenu();
	m_menuParOrder = CreatePopupMenu();
	m_menuShift = CreatePopupMenu();
	m_menuConditon = CreatePopupMenu();

	//下单面板
	m_labUserNo1.Create(m_Hwnd, L"账 号：", 0, ID_LAB_USERNO1);
	m_cbUserNo1.Create(m_Hwnd, ID_CB_USERNO1);
	m_labUserNo2.Create(m_Hwnd, L"账 号：", 0, ID_LAB_USERNO2);
	m_cbUserNo2.Create(m_Hwnd, ID_CB_USERNO2);
	m_labOrderContract.Create(m_Hwnd, L"合约", 0, ID_LAB_CONTRACT);
	m_labOrderQty.Create(m_Hwnd, L"手数", 0, ID_LAB_ORDERQTY);
	m_labOrderType.Create(m_Hwnd, L"价格类型", 0, ID_LAB_ORDERTYPE);
	m_labFirstContract.Create(m_Hwnd, L"第一腿", 0, ID_LAB_FIRSTORDER);
	m_labSecondContract.Create(m_Hwnd, L"第二腿", 0, ID_LAB_SECONDORDER);
	m_editFirstContract.Create(m_Hwnd, 0, ES_UPPERCASE, ID_EDIT_FIRSTCONTRACT);
	m_editSecondContract.Create(m_Hwnd, 0, ES_UPPERCASE, ID_EDIT_SECONDCONTRACT);
	m_editFirstOrderQty.Create(m_Hwnd, 0, 0, ID_EDIT_FIRSTORDERQTY);
	m_editSecondOrderQty.Create(m_Hwnd, 0, 0, ID_EDIT_SECONDORDERQTY);
	m_spinFirstOrderQty.Create(m_Hwnd, ID_SPIN_FIRSTORDERQTY, 0);
	m_spinSecondOrderQty.Create(m_Hwnd, ID_SPIN_SECONDORDERQTY, 0);
	m_cbFirstOrderType.Create(m_Hwnd, ID_CB_FIRSTORDERTYPE);
	m_cbSecondOrderType.Create(m_Hwnd, ID_CB_SECONDORDERTYPE);
	m_btnFirstContract.Create(m_Hwnd, "..", ID_BTN_FIRSTCONTRACT);
	m_btnSecondContract.Create(m_Hwnd, "..", ID_BTN_SECONDCONTRACT);
	m_labDirect.Create(m_Hwnd, L"方向型", 0, ID_LAB_DIRECT);
	m_cbDirect.Create(m_Hwnd, ID_CB_DIRECT);
	m_btnInsertOrder.Create(m_Hwnd, "下单", ID_BTN_OK);
	m_btnConditionOrder.Create(m_Hwnd, "条件单", ID_BTN_CONDITIONORDER);										

	m_sEditFirstOrderQty.Create(m_Hwnd, 0,ID_EDIT_FIRSTORDERQTY, ID_SPIN_FIRSTORDERQTY);
	m_sEditSecondOrderQty.Create(m_Hwnd, 0, ID_EDIT_SECONDORDERQTY, ID_SPIN_SECONDORDERQTY);

	//总手数（未添加设置控制）
	m_labTotalQty.Create(m_Hwnd, L"总手数", 0, ID_STATIC_TOTALQTY);
	m_sEditTotalQty.Create(m_Hwnd, 0,ID_EDIT_TOTALQTY, ID_SPIN_TOTALQTY);

	//超价设置
	m_labFirstSuperPot.Create(m_Hwnd, L"+");
	m_labSecondSuperPot.Create(m_Hwnd, L"+");
	m_sEditFirstSuperPot.Create(m_Hwnd, 0, ID_EDIT_FIRSTSUPERPOT, ID_SPIN_FIRSTSUPERPOT);
	m_sEditSecondSuperPot.Create(m_Hwnd, 0, ID_EDIT_SECONDSUPERPOT, ID_SPIN_SECONDSUPERPOT);
	//追价
	m_ckChasePrice.Create(m_Hwnd, ID_CK_CHASEPRICE, L"自动追价");

	m_labMarketValue.Create(m_Hwnd, L"价值");
	m_labFirstMarketValue.Create(m_Hwnd, L"0.000");
	m_labSecondMarketValue.Create(m_Hwnd, L"0.000");

}
void TShiftPosition::SetWidgetPos()
{
	POINT pt;
	pt.x = Vap_x;
	pt.y = CAPTION_H + Vap_y;
	m_tabShiftPosition.MoveWindow(pt.x, pt.y, FRAME_V - 2 * Vap_x, WidgetHeight);

	pt.y += WidgetHeight;
	m_listCondition.MoveWindow(Vap_x, pt.y, listWidth, m_iListShiftBottom - CAPTION_H - Vap_y - WidgetHeight);
	m_listShift.MoveWindow(Vap_x, pt.y, listWidth, m_iListShiftBottom - CAPTION_H - Vap_y - WidgetHeight);
	ShowTabList(0);

	int ilabWidth = 10;
	int iSuperPotWidth = 50;

	//账号
	pt.x = Vap_x;
	pt.y = m_iListShiftBottom + Vap_y;
	m_labUserNo1.MoveWindow(pt.x, pt.y + 2, lab_WidgetWidth, WidgetHeight);
	//第一腿资金账号选择
	pt.x += Vap_x + lab_WidgetWidth;
	m_cbUserNo1.MoveWindow(pt.x - 1, pt.y, WidgetWidth, 6 * WidgetHeight);

	//账号
	pt.x += Vap_x * 2 + 5 + WidgetWidth;
	m_cbUserNo2.MoveWindow(pt.x - 1, pt.y, WidgetWidth, 6 * WidgetHeight);

	//合约
	pt.x = Vap_x + lab_WidgetWidth + Vap_x; 
	pt.y += WidgetHeight + Vap_y;
	m_labOrderContract.MoveWindow(pt.x, pt.y + 1, EditSmall, WidgetHeight);
	//手数
	pt.x += EditWidgetWidth + Vap_x + 14;//14为合约btn的宽度和间隔的总和
	m_labOrderQty.MoveWindow(pt.x, pt.y, EditSmall, WidgetHeight);
	//价格类型
	pt.x += EditSmall + Vap_x;
	m_labOrderType.MoveWindow(pt.x, pt.y, EditSmall, WidgetHeight);
	//价值
	pt.x += EditSmall + Vap_x + ilabWidth + iSuperPotWidth + Vap_x;
	m_labMarketValue.MoveWindow(pt.x, pt.y, EditWidgetWidth, WidgetHeight);

	//第一腿
	pt.x = Vap_x; pt.y += WidgetHeight + Vap_y;
	m_labFirstContract.MoveWindow(pt.x, pt.y, lab_WidgetWidth, WidgetHeight);
	//第一腿合约edit
	pt.x += lab_WidgetWidth + Vap_x; 
	m_editFirstContract.MoveWindow(pt.x, pt.y, EditWidgetWidth, WidgetHeight);
	pt.x += EditWidgetWidth + 2;//btn合约按钮
	m_btnFirstContract.MoveWindow(pt.x, pt.y, 12, WidgetHeight);
	//第一腿合约手数
	pt.x += Vap_x + 12;
	m_sEditFirstOrderQty.MoveWindow(pt.x, pt.y, EditSmall, WidgetHeight);
	//第一腿价格类型
	pt.x += EditSmall + Vap_x;
	m_cbFirstOrderType.MoveWindow(pt.x, pt.y, EditSmall, 5 * WidgetHeight);
	
	//超价设置
	pt.x += EditSmall;
	m_labFirstSuperPot.MoveWindow(pt.x, pt.y, ilabWidth, WidgetHeight);
	pt.x += ilabWidth;
	m_sEditFirstSuperPot.MoveWindow(pt.x, pt.y, iSuperPotWidth, WidgetHeight);
	
	//第一腿价值
	pt.x += ilabWidth +iSuperPotWidth + Vap_x;
	m_labFirstMarketValue.MoveWindow(pt.x, pt.y, EditWidgetWidth, WidgetHeight);

	//日志显示
	POINT RichPt = pt;
	RichPt.x += EditWidgetWidth + ilabWidth + Vap_x;
	RichPt.y = m_iListShiftBottom + Vap_y;
	m_RichEdit.MoveWindow(RichPt.x, RichPt.y, FRAME_V - RichPt.x - Vap_x, WidgetHeight * 6 + 3);

	//第二腿
	pt.x = Vap_x ;
	pt.y += WidgetHeight + Vap_y;
	m_labSecondContract.MoveWindow(pt.x, pt.y, lab_WidgetWidth, WidgetHeight);
	//第二腿合约edit
	pt.x += lab_WidgetWidth + Vap_x;
	m_editSecondContract.MoveWindow(pt.x, pt.y, EditWidgetWidth, WidgetHeight);
	pt.x += EditWidgetWidth + 2;//btn合约按钮
	m_btnSecondContract.MoveWindow(pt.x, pt.y, 12, WidgetHeight);
	//第二腿合约手数

	pt.x += Vap_x + 12;
	m_sEditSecondOrderQty.MoveWindow(pt.x, pt.y, EditSmall, WidgetHeight);
	//第二腿价格类型
	pt.x += EditSmall + Vap_x;
	m_cbSecondOrderType.MoveWindow(pt.x, pt.y, EditSmall, 5 * WidgetHeight);

	//超价设置
	pt.x += EditSmall;
	m_labSecondSuperPot.MoveWindow(pt.x, pt.y, ilabWidth, WidgetHeight);
	pt.x += ilabWidth;
	m_sEditSecondSuperPot.MoveWindow(pt.x, pt.y, iSuperPotWidth, WidgetHeight);

	//第二腿价值
	pt.x += ilabWidth + iSuperPotWidth + Vap_x;
	m_labSecondMarketValue.MoveWindow(pt.x, pt.y, EditWidgetWidth, WidgetHeight);


	//方向型
	pt.x = Vap_x; pt.y += Vap_y + WidgetHeight;
	m_labDirect.MoveWindow(pt.x, pt.y, lab_WidgetWidth, WidgetHeight);
	pt.x += lab_WidgetWidth + Vap_x;
	m_cbDirect.MoveWindow(pt.x - 1, pt.y, EditWidgetWidth, 5 * WidgetHeight);


	//追价
	pt.x += EditWidgetWidth + 14 + Vap_x;
	m_ckChasePrice.MoveWindow(pt.x, pt.y, EditSmall, WidgetHeight);

	//下单
	pt.x += EditSmall + Vap_x;
	m_btnInsertOrder.MoveWindow(pt.x, pt.y + 1, EditSmall + ilabWidth + iSuperPotWidth, WidgetHeight);

	//总手数
	pt.x += EditSmall + ilabWidth + iSuperPotWidth + Vap_x + ilabWidth;
	m_labTotalQty.MoveWindow(pt.x, pt.y, lab_WidgetWidth - 10, WidgetHeight);
	pt.x += Vap_x + lab_WidgetWidth - 10;;
	m_sEditTotalQty.MoveWindow(pt.x, pt.y, EditSmall, WidgetHeight);
	TotalQtyWidget(g_pstConfig->m_bSingle);
	

	//持仓列表和挂单列表
	pt.x = Vap_x; 
	pt.y += WidgetHeight + Vap_y + 2;
	m_listPosition.MoveWindow(pt.x, pt.y, listWidth, listHeight);

	pt.x = Vap_x; 
	pt.y += listHeight + Vap_y;
	m_listParOrder.MoveWindow(pt.x, pt.y, listWidth, listHeight);
	

	//ShowWindow(m_btnConditionOrder.GetHwnd(), SW_HIDE);
}

void TShiftPosition::InitWidget()
{
	InitMenu();
	InitListView();
	InitListData();
	SetUserInfo();

	InitOrderWidget();
	

	//默认是显示第一个账号的交易信息；
	ShowAllParOrder();						//挂单
	ShowAllPosition();						//持仓
	ShowAllShift();

}
void TShiftPosition::InitOrderWidget()
{

	m_cbFirstOrderType.Clear();
	m_cbFirstOrderType.AddString(L"排队价");
	m_cbFirstOrderType.AddString(L"对手价");
	m_cbFirstOrderType.AddString(L"超价");
	m_cbFirstOrderType.AddString(L"市价");
	m_cbFirstOrderType.AddString(L"最新价");

	m_cbSecondOrderType.Clear();
	m_cbSecondOrderType.AddString(L"排队价");
	m_cbSecondOrderType.AddString(L"对手价");
	m_cbSecondOrderType.AddString(L"超价");
	m_cbSecondOrderType.AddString(L"市价");
	m_cbSecondOrderType.AddString(L"最新价");
	m_cbFirstOrderType.SetSelect(1);
	m_cbSecondOrderType.SetSelect(1);

	m_cbDirect.Clear();
	m_cbDirect.AddString(L"买平-卖开");
	m_cbDirect.AddString(L"卖平-买开");
	m_cbDirect.AddString(L"买开-卖平"); 
	m_cbDirect.AddString(L"卖开-买平");
	m_cbDirect.SetSelect(0);

	m_sEditFirstOrderQty.SetWindowNumber(1);
	m_sEditSecondOrderQty.SetWindowNumber(1);
	m_sEditFirstOrderQty.SetFont(g_FontData.GetFontNum13());
	m_sEditSecondOrderQty.SetFont(g_FontData.GetFontNum13());

	m_sEditTotalQty.SetWindowNumber(1);
	m_sEditTotalQty.SetFont(g_FontData.GetFontNum13());

	m_sEditFirstSuperPot.SetFont(g_FontData.GetFontNum13());
	m_sEditSecondSuperPot.SetFont(g_FontData.GetFontNum13());
	m_labFirstSuperPot.SetFont(g_FontData.GetFontNum15());
	m_labSecondSuperPot.SetFont(g_FontData.GetFontNum15());

	m_editFirstContract.SetFont(g_FontData.GetFontWord13());
	m_editSecondContract.SetFont(g_FontData.GetFontWord13());

	m_labFirstMarketValue.SetFont(g_FontData.GetFontNum13());
	m_labSecondMarketValue.SetFont(g_FontData.GetFontNum13());


	m_cbUserNo1.SetFont(g_FontData.GetFontWord15());
	m_cbUserNo2.SetFont(g_FontData.GetFontWord15());

	m_sEditFirstOrderQty.SetRange(1, 100000);
	m_sEditSecondOrderQty.SetRange(1, 100000);
	m_sEditFirstSuperPot.SetRange(0, 100);
	m_sEditSecondSuperPot.SetRange(0, 100);
	m_sEditTotalQty.SetRange(1, 100000);

	m_sEditSecondSuperPot.SetWindowNumber(0);
	m_sEditFirstSuperPot.SetWindowNumber(0);

	m_sEditFirstSuperPot.Enable(false);
	m_sEditSecondSuperPot.Enable(false);

	m_cbUserNo1.Clear();
	m_cbUserNo2.Clear();
	std::map<std::string, TUserInfo*>::iterator iter = m_mapUserInfo.begin();
	for (; iter != m_mapUserInfo.end(); iter++)
	{	
		m_cbUserNo1.AddString(LoadRC::ansi_to_unicode(iter->second->UserNo).c_str());
		m_cbUserNo2.AddString(LoadRC::ansi_to_unicode(iter->second->UserNo).c_str());
	}
	m_cbUserNo1.SetSelect(0);
	m_cbUserNo2.SetSelect(0);
}

void TShiftPosition::TotalQtyWidget(bool single)
{
	if (single)
	{
		ShowWindow(GetDlgItem(m_Hwnd, ID_STATIC_TOTALQTY), SW_SHOW);
		ShowWindow(GetDlgItem(m_Hwnd, ID_EDIT_TOTALQTY), SW_SHOW);
		ShowWindow(GetDlgItem(m_Hwnd, ID_SPIN_TOTALQTY), SW_SHOW);
	}
	else
	{
		ShowWindow(GetDlgItem(m_Hwnd, ID_STATIC_TOTALQTY), SW_HIDE);
		ShowWindow(GetDlgItem(m_Hwnd, ID_EDIT_TOTALQTY), SW_HIDE);
		ShowWindow(GetDlgItem(m_Hwnd, ID_SPIN_TOTALQTY), SW_HIDE);
	}
}

void TShiftPosition::InitListData()
{
	if (g_pTradeData)
	{
		g_pTradeData->GetParOrder(m_ParOrderContainer, ftNone);
		g_pTradeData->GetAllPositionTotal(m_PositionContainer);
	}
}
void TShiftPosition::InitMenu()
{
	AppendMenu(m_menuPosition, MF_STRING, IDM_POSITTIONCOUNTERCLOSE, L"对价平仓");
	AppendMenu(m_menuPosition, MF_STRING, IDM_POSITIONMARKETCLOSE, L"市价平仓");
	AppendMenu(m_menuPosition, MF_STRING, IDM_POSITIONALLCLOSE, L"清仓");
	AppendMenu(m_menuPosition, MF_STRING, IDM_POSITIONFRESH, L"刷新");

	AppendMenu(m_menuParOrder, MF_STRING, IDM_PARORDERCANCEL, L"撤单");
	AppendMenu(m_menuParOrder, MF_STRING, IDM_PARORDERCOUNTERFOLLOW, L"对价跟");
	AppendMenu(m_menuParOrder, MF_STRING, IDM_PARORDERMARKETFOLLOW, L"市价跟");
	AppendMenu(m_menuParOrder, MF_STRING, IDM_PARORDERALLCANCEL, L"撤所有挂单");
	AppendMenu(m_menuParOrder, MF_STRING, IDM_PARORDERFRESH, L"刷新");

	AppendMenu(m_menuShift, MF_STRING, IDM_SHIFTADDNEWSHIFT, L"新增移仓合约");
	AppendMenu(m_menuShift, MF_STRING, IDM_SHIFTDELETESHIFT, L"删除移仓合约");
	AppendMenu(m_menuShift, MF_SEPARATOR, IDM_SHIFTLINE, L"");
	AppendMenu(m_menuShift, MF_STRING, IDM_SHIFTADDCONDITION, L"新增条件单");

	AppendMenu(m_menuConditon, MF_STRING, IDM_CONDITIONNO_SEND, L"立即触发");
	AppendMenu(m_menuConditon, MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuConditon, MF_STRING, IDM_CONDITIONNO_MODIFY, L"修改");
	AppendMenu(m_menuConditon, MF_STRING, IDM_CONDITIONNO_DELETE, L"删除");
	AppendMenu(m_menuConditon, MF_STRING, IDM_CONDITIONNO_PAUSE, L"暂停");
	AppendMenu(m_menuConditon, MF_STRING, IDM_CONDITIONNO_START, L"启动");

	AppendMenu(m_menuConditon, MF_MENUBARBREAK | MF_STRING, IDM_CONDITIONNO_ALLPAUSE, L"全部暂停");
	AppendMenu(m_menuConditon, MF_STRING, IDM_CONDITIONNO_ALLSTART, L"全部启动");
	AppendMenu(m_menuConditon, MF_STRING, IDM_CONDITIONNO_ALLDELETE, L"全部删除");
	AppendMenu(m_menuConditon, MF_SEPARATOR, 0, 0);
	AppendMenu(m_menuConditon, MF_STRING | MF_CHECKED, IDM_CONDITIONNO_NOTTRIGGER, L"未触发");
	AppendMenu(m_menuConditon, MF_STRING | MF_UNCHECKED, IDM_CONDITIONNO_TRIGGER, L"已触发");
}

void TShiftPosition::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}
void TShiftPosition::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;
	switch (id)
	{
	case ID_LISTPOSITION:
		for (uint i = IDM_POSITTIONCOUNTERCLOSE; i <= IDM_POSITIONALLCLOSE; i++)
			EnableMenuItem(m_menuPosition, i, flage);
		break;
	case ID_LISTPARORDER:
		for (uint i = IDM_PARORDERCANCEL; i <= IDM_PARORDERALLCANCEL; i++)
			EnableMenuItem(m_menuParOrder, i, flage);
		break;
	case ID_LISTCONDITION:
		for (uint i = IDM_CONDITIONNO_SEND; i <= IDM_CONDITIONNO_ALLDELETE; i++)
		{
			if (bEnable == false)
				EnableMenuItem(m_menuConditon, i, flage); 
			else
			{
				if (m_bIsTrigger)
				{
					if (i == IDM_CONDITIONNO_ALLDELETE)
						EnableMenuItem(m_menuConditon, i, MF_ENABLED);
					else
						EnableMenuItem(m_menuConditon, i, MF_GRAYED);
				}
				else
					EnableMenuItem(m_menuConditon, i, flage);
			}
		}	
		EnableMenuItem(m_menuConditon, IDM_CONDITIONNO_TRIGGER, MF_ENABLED);
		EnableMenuItem(m_menuConditon, IDM_CONDITIONNO_NOTTRIGGER, MF_ENABLED);
		break;
	case ID_LISTSHIFT:
		for (uint i = IDM_SHIFTADDNEWSHIFT; i <= IDM_SHIFTADDCONDITION; i++)
			EnableMenuItem(m_menuShift, i, flage);
		break; 
	default:
		break;
	}
}
void TShiftPosition::InitListView()
{

	m_listShift.AddColumn(L"合约名称", 150, -1);
	m_listShift.AddColumn(L"做多对价差", 80, -1);
	m_listShift.AddColumn(L"做多挂价差", 80, -1);
	m_listShift.AddColumn(L"做空对价差", 80, -1);
	m_listShift.AddColumn(L"做空挂价差", 80, -1);
	m_listShift.AddColumn(L"最新", 80, -1);
	m_listShift.AddColumn(L"均价", 80, -1);
	m_listShift.AddColumn(L"开盘", 80, -1);
	m_listShift.AddColumn(L"最高价", 80, -1);
	m_listShift.AddColumn(L"最低价", 80, -1);


	//m_listCondition.AddColumn(L"设置时间", 120, -1);
	m_listCondition.AddColumn(L"交易账号", 120, -1);
	m_listCondition.AddColumn(L"交易账号", 120, -1);
	m_listCondition.AddColumn(L"状态", 40, -1);
	m_listCondition.AddColumn(L"合约号", 160, -1);
	//m_listCondition.AddColumn(L"最新价", 80, -1);
	m_listCondition.AddColumn(L"价差方式", 80, -1);
	m_listCondition.AddColumn(L"条件", 60, -1);
	m_listCondition.AddColumn(L"触发价", 100, -1);
	m_listCondition.AddColumn(L"下单方式", 100, -1);
	m_listCondition.AddColumn(L"委托号", 80, -1);
	m_listCondition.AddColumn(L"下单方向", 100, -1);
	m_listCondition.AddColumn(L"下单策略", 100, -1);


	m_listPosition.AddColumn(L"品种", 80);
	m_listPosition.AddColumn(L"合约号", 80);
	m_listPosition.AddColumn(L"多空", 40);
	m_listPosition.AddColumn(L"手数", 40);
	m_listPosition.AddColumn(L"可用", 40);
	m_listPosition.AddColumn(L"持仓均价", 80);
	m_listPosition.AddColumn(L"逐笔浮盈", 80);
	m_listPosition.AddColumn(L"价值", 80);
	m_listPosition.AddColumn(L"保证金", 80);
	m_listPosition.AddColumn(L"盯市浮盈", 80);
	m_listPosition.AddColumn(L"今手数", 40);
	m_listPosition.AddColumn(L"今可用", 40);


	m_listParOrder .AddColumn(L"时间", 160, -1);
	m_listParOrder.AddColumn(L"合约号", 80, -1);
	m_listParOrder.AddColumn(L"状态", 40, -1);
	m_listParOrder.AddColumn(L"买卖", 40, -1);
	m_listParOrder.AddColumn(L"开平", 40, -1);
	m_listParOrder.AddColumn(L"委托价", 80, -1);
	m_listParOrder.AddColumn(L"委托量", 40, -1);
	m_listParOrder.AddColumn(L"可撤", 40, -1);
	m_listParOrder.AddColumn(L"已成交", 40, -1);
}
void TShiftPosition::OnSize(WPARAM wParam, LPARAM lParam)
{
	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	m_iListShiftBottom = cy - 470;
	SetWidgetPos();
}
void TShiftPosition::OnSizing(WPARAM wParam, LPARAM lParam)
{
	LPRECT pRect = (LPRECT)lParam;
	if (pRect->bottom - pRect->top < FRAME_H)
	{
		if (wParam == WMSZ_BOTTOM)
			pRect->bottom = pRect->top + FRAME_H;
		else
			pRect->top = pRect->bottom - FRAME_H;
	}
}
void TShiftPosition::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_LISTSHIFT:
		m_listShift.OnDrawItem(lParam);
		break;
	case ID_LISTPOSITION:
		m_listPosition.OnDrawItem(lParam);
		break;
	case ID_LISTPARORDER:
		m_listParOrder.OnDrawItem(lParam);
		break;
	case ID_LISTCONDITION:
		m_listCondition.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}
void TShiftPosition::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_LISTSHIFT:
		m_listShift.OnMeasureItem(p->itemHeight);
		break;
	case ID_LISTPOSITION:
		m_listPosition.OnMeasureItem(p->itemHeight);
		break;
	case ID_LISTPARORDER:
		m_listParOrder.OnMeasureItem(p->itemHeight);
		break;
	case ID_LISTCONDITION:
		m_listCondition.OnMeasureItem(p->itemHeight);
		break;
	default:
		break;
	}
}
void TShiftPosition::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMouseTrack = true;
	}
	InvalidateRect(m_Hwnd, 0, false);
}
void TShiftPosition::OnMouseLeave()
{
	m_bMouseTrack = false;
}
void TShiftPosition::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.bottom = rect.top + CAPTION_H;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
	}
	if (PtInRect(&m_rcMini, pt))
	{
		ShowWindow(m_Hwnd, SW_SHOWMINIMIZED);
	}
	if (PtInRect(&m_rcSet, pt))
	{
		m_clOrderConfig.RegistConfig(g_pstConfig);
		m_clOrderConfig.Create(m_Hwnd);
		m_clOrderConfig.MoveWindow(500, 500);
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}
UINT TShiftPosition::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = GET_X_LPARAM(lParam);
	point.y = GET_Y_LPARAM(lParam);
	RECT rect;
	GetWindowRect(m_Hwnd, &rect);
	if (point.y <= rect.top + 5)
		return HTTOP;
	else if (point.y >= rect.bottom - 5)
		return HTBOTTOM;
	return NOT_PROCESSED;
}
void TShiftPosition::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam != m_Hwnd)
		return;
	UINT nHitTest = LOWORD(lParam);
	switch (nHitTest)
	{
	case HTTOP:
	case HTBOTTOM:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		break;
	default:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		break;
	}
}
void TShiftPosition::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case HTTOP:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, lParam);
		break;
	case HTBOTTOM:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, lParam);
		break;
	default:
		break;
	}
}

void TShiftPosition::GetParOrder(char* UserNo, char* sign)
{
	m_UserParOrder.clear();
	uint iCount = m_ParOrderContainer.size();
	for (int i = 0; i < iCount; i++)
	{
		TOrder* pst = (TOrder*)m_ParOrderContainer.at(i);
		if (IsUserNoInfo(pst->UserNo, pst->Sign))
			m_UserParOrder.push_back(pst);
	}
}
void TShiftPosition::GetPosition(char* UserNo, char* sign)
{
	m_UserPosition.clear();
	uint iCount = m_PositionContainer.size();
	for (int i = 0; i < iCount; i++)
	{
		TPosition* pst = (TPosition*)m_PositionContainer.at(i);
		if (IsUserNoInfo(pst->UserNo, pst->Sign))
			m_UserPosition.push_back(pst);
	}
}
bool TShiftPosition::IsUserNoInfo(const char* UserNo, const char* sign)
{
	//判断合约资金账号与选中的资金账号是否一致。列表默认为第一个资金账户信息
	if (strcmp(m_stUserInfo1.UserNo, UserNo) == 0 && strcmp(m_stUserInfo1.Sign, sign) == 0)
		return true;
	else
		return false;
}
bool TShiftPosition::IsAddOrUpdate(const TPosition* pPosition)
{
	for (UINT i = 0; i < m_UserPosition.size(); i++)
	{
		if (pPosition == m_UserPosition.at(i))
		{
			char szText[100] = { 0 };
			m_listPosition.GetSubItem(i, 3, szText, sizeof(szText));//获取手数
			if (atoi(szText) == pPosition->PositionQty)
				return false;
			return true;
		}
	}
	return true;
}
void TShiftPosition::UpdateAvaQty(WPARAM wParam)//更新可用  4,11 为可用和今可用
{
	const TOrder* pOrder = (TOrder*)wParam;
	TContractSingleKey Contract;
	memcpy(&Contract, pOrder, sizeof(TContractSingleKey));
	TDirect direct;
	if (pOrder->Direct == dBuy)
		direct = dSell;
	else
		direct = dBuy;
	const TPosition* pst = g_pTradeData->GetPositionTotal(pOrder->UserNo, &Contract, direct);
	int AvaQty = 0, TAvaQty = 0, iRow = 0;
	if (!pst)return;
	CConvert::GetAvailableQty(pst, AvaQty, TAvaQty);
	if (IsUserNoInfo(pst->UserNo, pst->Sign))
	{
		m_UserPosition.FindItem(pst, iRow);
		m_listPosition.AddItem(AvaQty, iRow, 4);
		m_listPosition.AddItem(TAvaQty, iRow, 11);
	}	
}
void TShiftPosition::UpdateMarketValue()
{
	for (int i = 0; i < m_UserPosition.size(); i++)
	{
		TPosition* pst = (TPosition*)m_UserPosition.at(i);
		if (!pst || pst->PositionQty == 0) continue;	
		const SCommodity* pCommodity = g_pTradeData->GetCommodity(pst);
		if (!pCommodity) continue;
		double Price = 0.0;
		CConvert::GetPrice(pst, pst->Direct, LastPrice, Price);
		double MarketValue = pCommodity->TradeDot *Price * pst->PositionQty;
		m_listPosition.AddItem(MarketValue, i, 7, pCommodity->PricePrec, DT_CENTER);
	}
}
void TShiftPosition::SetMarketValue()
{
	char Contract[2][51] = { 0 };
	char szText[21] = { 0 };
	int  iOrderQty = 0;
	TContractKey stContract[2];
	for (int i = 0; i < 2; i++)
	{
		memset(&stContract[i], 0, sizeof(TContractKey));
		if (i == 0)
			memcpy(stContract[i].Sign, m_stUserInfo1.Sign, sizeof(stContract[i].Sign));
		else
			memcpy(stContract[i].Sign, m_stUserInfo2.Sign, sizeof(stContract[i].Sign));
	}
	double MarValue = 0.0;
	double Price = 0.0;
	m_editFirstContract.GetText(Contract[0], sizeof(Contract[0]));
	m_editSecondContract.GetText(Contract[1], sizeof(Contract[0]));
	for (int i = 0; i < 2; i++)
	{
		if (strcmp(Contract[i], "") ==0) continue;		
		CConvert::TContractNoToTContractKey(Contract[i], stContract[i]);
		const SCommodity* pst = g_pTradeData->GetCommodity(&stContract[i]);
		if (!pst) continue;
		CConvert::GetPrice(&stContract[i], dBuy, LastPrice, Price);	
		if (!i)
		{
			MarValue = pst->TradeDot * Price * m_sEditFirstOrderQty.GetWindowNumber();
			DoubleToChar(pst->PricePrec, MarValue, szText);
			m_labFirstMarketValue.SetText(szText);
		}
		else
		{
			MarValue = pst->TradeDot * Price * m_sEditSecondOrderQty.GetWindowNumber();
			DoubleToChar(pst->PricePrec, MarValue, szText);
			m_labSecondMarketValue.SetText(szText);
		}
	}
}
//===================================Mark
bool TShiftPosition::FindItem(int& item, char* ShiftContractNo)
{
	std::map<string, ShiftListInfo>::iterator iter;
	int Index = 0;
	for (iter = m_mapShiftListInfo.begin(); iter != m_mapShiftListInfo.end(); iter++)
	{
		if (strcmp(iter->first.c_str(), ShiftContractNo) == 0)
		{
			item = Index;
			return true;
		}		
		Index++;
	}
	return false;
}
void TShiftPosition::OrderStateToChar(int iOrderState, char* OrderState, int iSize)
{
	switch (iOrderState)
	{
	case osRun:
		strcpy_s(OrderState, iSize, "运行");
		break;
	case osPause:
		strcpy_s(OrderState, iSize, "暂停");
		break;
	case osTrigger:
		strcpy_s(OrderState, iSize, "触发");
		break;
	default:
		strcpy_s(OrderState, iSize, "");
		break;
	}
}
void TShiftPosition::DiffPriceTypeToChar(int iPriceType, char* cPriceType, int iSize)
{
	switch (iPriceType)
	{
	case pcLastPrice:
		strcpy_s(cPriceType, iSize, "最新差价");
		break;
	case pcCounterPrice:
		strcpy_s(cPriceType, iSize, "对手差价");
		break;
	case pcQueuePrice:
		strcpy_s(cPriceType, iSize, "排队价差");
		break;
	default:
		strcpy_s(cPriceType, iSize, "");
		break;
	}
}
void TShiftPosition::PriceConditionToChar(int iPriceCondition, char* cPriceCondition, int iSize)
{
	switch (iPriceCondition)
	{
	case tcEqualLarger:
		strcpy_s(cPriceCondition, iSize, "大于等于");
		break;
	case tcEqualLess:
		strcpy_s(cPriceCondition, iSize, "小于等于");
		break;
	default:
		strcpy_s(cPriceCondition, iSize, "");
		break;
	}
}
void TShiftPosition::OrderTypeToChar(int iFirstType, int iSecondType, char* cOrderType, int iSize)
{
	int PriceType[3] = { iFirstType, iSecondType };
	char OrderType[3][51] = { 0 };
	for (int i = 0; i < 2; )
	{
		switch (PriceType[i])
		{
		case 0:
			strcpy_s(OrderType[i], iSize, "排队价");
			break;
		case 1:
			strcpy_s(OrderType[i], iSize, "对手价");
			break;
		case 2:
			strcpy_s(OrderType[i], iSize, "超价");
			break;
		case 3:
			strcpy_s(OrderType[i], iSize, "市价");
			break;
		case 4:
			strcpy_s(OrderType[i], iSize, "最新价");
			break;
		default:
			strcpy_s(OrderType[i], iSize, "");
			break;
		}
		i++;
	}
	sprintf_s(cOrderType, iSize,"%s-%s", OrderType[0], OrderType[1]);
}