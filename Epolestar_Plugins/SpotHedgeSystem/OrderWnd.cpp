#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
#define cCAPTION_H			45
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体

OrderWnd::OrderWnd()
{
}

OrderWnd::~OrderWnd()
{
}

void OrderWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void OrderWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void OrderWnd::AddOrderToList(SmsOrderDataRsp & order, int index)
{
	int count = 0;
	if (index == -1)
		count = m_listOrder.GetItemCount();
	else
		count = index;
	//交易员编号
	m_listOrder.AddItem(order.UserNo, count, ORDER_UserNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合同编号
	m_listOrder.AddItem(order.AgreementNo, count, ORDER_AgreementNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//合约编号
	string sCode = order.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	m_listOrder.AddItem(tCode.c_str(), count, ORDER_ContractNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//买卖方向
	if (order.Direct == SMS_DIRECT_BUY)
		m_listOrder.AddItem("买", count, ORDER_Direct, DT_LEFT, g_ColorRefData.GetColorBuyRed());
	else if (order.Direct == SMS_DIRECT_SELL)
		m_listOrder.AddItem("卖", count, ORDER_Direct, DT_RIGHT, g_ColorRefData.GetColorSellBlue());
	else
		m_listOrder.AddItem("双边", count, ORDER_Direct, DT_CENTER, g_ColorRefData.GetColorBlack());
	//开平
	if (order.Offset == SMS_O_OPEN)
		m_listOrder.AddItem("开仓", count, ORDER_Offset, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	else if (order.Offset == SMS_O_COVER)
		m_listOrder.AddItem("平仓", count, ORDER_Offset, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	//委托价格
	m_listOrder.AddItem((double)order.OrderPrice, count, ORDER_OrderPrice, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//委托数量
	m_listOrder.AddItem((double)order.OrderQty, count, ORDER_OrderQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//成交价格
	m_listOrder.AddItem((double)order.MatchPrice, count, ORDER_MatchPrice, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//成交数量
	m_listOrder.AddItem((double)order.MatchQty, count, ORDER_MatchQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//订单状态
	if (order.OrderState == SMS_OS_QUEUED)
		m_listOrder.AddItem("已排队", count, ORDER_OrderState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (order.OrderState == SMS_OS_PARTFILLED)
		m_listOrder.AddItem("部分成交", count, ORDER_OrderState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (order.OrderState == SMS_OS_FILLED)
		m_listOrder.AddItem("完全成交", count, ORDER_OrderState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (order.OrderState == SMS_OS_CANCELED)
		m_listOrder.AddItem("已撤单", count, ORDER_OrderState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (order.OrderState == SMS_OS_ACCEPT)
		m_listOrder.AddItem("已受理", count, ORDER_OrderState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (order.OrderState == SMS_OS_FAIL)
		m_listOrder.AddItem("指令失败", count, ORDER_OrderState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (order.OrderState == SMS_OS_SENDED)
		m_listOrder.AddItem("已发送", count, ORDER_OrderState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else
		m_listOrder.AddItem("无", count, ORDER_OrderState, DT_CENTER, g_ColorRefData.GetColorBlack());
	//投保类型
	if (order.Hedge == SMS_HEDGE_SPECULATE)
		m_listOrder.AddItem("投机", count, ORDER_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (order.Hedge == SMS_HEDGE_HEDGE)
		m_listOrder.AddItem("套保", count, ORDER_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	else
		m_listOrder.AddItem("无", count, ORDER_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	//结算日期
	m_listOrder.AddItem(order.SettleDate, count, ORDER_SettleDate, DT_CENTER, g_ColorRefData.GetColorBlack());
	//公司编号
	m_listOrder.AddItem(order.CompanyNo, count, ORDER_CompanyNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//序列号
	m_listOrder.AddItem((double)order.SerialId, count, ORDER_SerialId, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//委托编号
	m_listOrder.AddItem(order.OrderNo, count, ORDER_OrderNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//系统号
	m_listOrder.AddItem(order.SystemNo, count, ORDER_SystemNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//本地编号
	m_listOrder.AddItem(order.LocalNo, count, ORDER_LocalNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//错误码
	m_listOrder.AddItem((double)order.ErrorCode, count, ORDER_ErrorCode, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//错误信息
	m_listOrder.AddItem(order.ErrorText, count, ORDER_ErrorText, DT_CENTER, g_ColorRefData.GetColorBlack());
	//下单时间
	m_listOrder.AddItem(order.OrderTime, count, ORDER_OrderTime, DT_CENTER, g_ColorRefData.GetColorBlack());
	//操作员
	m_listOrder.AddItem(order.OperatorNo, count, ORDER_OperatorNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//修改时间
	m_listOrder.AddItem(order.OperateTime, count, ORDER_OperateTime, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT OrderWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_ORDER_REFRESH:
		RefreshList();
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM://==7
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_NCDESTROY:
		OnNcDestroy();
		break;
	case WM_DESTROY:
		OnDestroy();
		break;
	default:
		return NOT_PROCESSED;
	}
	return NOT_PROCESSED;
}

void OrderWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	///////////////////////////////Create//////////////////////////////////
	//开始日期
	m_labBeginDate.Create(m_Hwnd, L"开始日期", DT_RIGHT, ID_LabBeginDate);
	m_beginTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtBeginDate, m_hInstance, NULL);
	//结束日期
	m_labEndDate.Create(m_Hwnd, L"结束日期", DT_RIGHT, ID_LabEndDate);
	m_endTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtEndDate, m_hInstance, NULL);
	//账号
	//m_labUserno.Create(m_Hwnd, L"账号", DT_CENTER, ID_LabUserno);
	//m_cbUserno.Create(m_Hwnd, ID_CbUserno);
	//查询
	m_btnQry.Create(m_Hwnd, "查  询", ID_BtnQry);
	//表格
	m_listOrder.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Order);
	//RECT rect;
	//GetClientRect(m_Hwnd, &rect);
	//m_listOrder.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	//////////////////////////////Init////////////////////////////////////
	for (int i = ORDER_UserNo; i < ORDER_OperateTime; i++)
	{
		switch (i)
		{
		case ORDER_SerialId:
			m_listOrder.AddColumn(L"序列号", 80, ORDER_SerialId);
			//m_listCloseOrder.AddColumn(L"平仓委托", 80, ORDER_SerialId);
			break;
		case ORDER_CompanyNo:
			m_listOrder.AddColumn(L"公司编号", 80, ORDER_CompanyNo);
			//m_listCloseOrder.AddColumn(L"公司编号", 80, ORDER_CompanyNo);
			break;
		case ORDER_UserNo:
			m_listOrder.AddColumn(L"账号", 80, ORDER_UserNo);
			//m_listCloseOrder.AddColumn(L"账号", 80, ORDER_UserNo);
			break;
		case ORDER_SettleDate:
			m_listOrder.AddColumn(L"结算日期", 80, ORDER_SettleDate);
			//m_listCloseOrder.AddColumn(L"结算日期", 80, ORDER_SettleDate);
			break;
		case ORDER_AgreementNo:
			m_listOrder.AddColumn(L"合同编号", 80, ORDER_AgreementNo);
			//m_listCloseOrder.AddColumn(L"合同编号", 80, ORDER_AgreementNo);
			break;
		case ORDER_ContractNo:
			m_listOrder.AddColumn(L"合约编号", 130, ORDER_ContractNo);
			//m_listCloseOrder.AddColumn(L"合约编号", 80, ORDER_ContractNo);
			break;
		case ORDER_OrderNo:
			m_listOrder.AddColumn(L"委托编号", 80, ORDER_OrderNo);
			//m_listCloseOrder.AddColumn(L"委托编号", 80, ORDER_OrderNo);
			break;
		case ORDER_SystemNo:
			m_listOrder.AddColumn(L"系统号", 80, ORDER_SystemNo);
			//m_listCloseOrder.AddColumn(L"系统号", 80, ORDER_SystemNo);
			break;
		case ORDER_Direct:
			m_listOrder.AddColumn(L"买卖", 40, ORDER_Direct);
			//m_listCloseOrder.AddColumn(L"买卖", 80, ORDER_Direct);
			break;
		case ORDER_Offset:
			m_listOrder.AddColumn(L"开平", 80, ORDER_Offset);
			//m_listCloseOrder.AddColumn(L"开平", 80, ORDER_Offset);
			break;
		case ORDER_Hedge:
			m_listOrder.AddColumn(L"投保", 80, ORDER_Hedge);
			//m_listCloseOrder.AddColumn(L"投保", 80, ORDER_Hedge);
			break;
		case ORDER_LocalNo:
			m_listOrder.AddColumn(L"本地编号", 80, ORDER_LocalNo);
			//m_listCloseOrder.AddColumn(L"本地编号", 80, ORDER_LocalNo);
			break;
		case ORDER_OrderPrice:
			m_listOrder.AddColumn(L"委托价格", 80, ORDER_OrderPrice);
			//m_listCloseOrder.AddColumn(L"委托价格", 80, ORDER_OrderPrice);
			break;
		case ORDER_OrderQty:
			m_listOrder.AddColumn(L"委托数量", 80, ORDER_OrderQty);
			//m_listCloseOrder.AddColumn(L"委托数量", 80, ORDER_OrderQty);
			break;
		case ORDER_MatchPrice:
			m_listOrder.AddColumn(L"成交价格", 80, ORDER_MatchPrice);
			//m_listCloseOrder.AddColumn(L"成交价格", 80, ORDER_MatchPrice);
			break;
		case ORDER_MatchQty:
			m_listOrder.AddColumn(L"成交数量", 80, ORDER_MatchQty);
			//m_listCloseOrder.AddColumn(L"成交数量", 80, ORDER_MatchQty);
			break;
		case ORDER_OrderState:
			m_listOrder.AddColumn(L"订单状态", 80, ORDER_OrderState);
			//m_listCloseOrder.AddColumn(L"订单状态", 80, ORDER_OrderState);
			break;
		case ORDER_ErrorCode:
			m_listOrder.AddColumn(L"错误码", 80, ORDER_ErrorCode);
			//m_listCloseOrder.AddColumn(L"错误码", 80, ORDER_ErrorCode);
			break;
		case ORDER_ErrorText:
			m_listOrder.AddColumn(L"错误信息", 80, ORDER_ErrorText);
			//m_listCloseOrder.AddColumn(L"错误信息", 80, ORDER_ErrorText);
			break;
		case ORDER_OrderTime:
			m_listOrder.AddColumn(L"下单时间", 130, ORDER_OrderTime);
			//m_listCloseOrder.AddColumn(L"下单时间", 130, ORDER_OrderTime);
			break;
		case ORDER_OperatorNo:
			m_listOrder.AddColumn(L"操作员", 80, ORDER_OperatorNo);
			//m_listCloseOrder.AddColumn(L"操作员", 80, ORDER_OperatorNo);
			break;
		case ORDER_OperateTime:
			m_listOrder.AddColumn(L"修改时间", 130, ORDER_OperateTime);
			//m_listCloseOrder.AddColumn(L"修改时间", 130, ORDER_OperateTime);
			break;
		default:
			break;
		}
	}

	m_menuOrder = CreatePopupMenu();

	//AppendMenu(m_menuOrder, MF_STRING, IDM_AddOrder, L"录入定单");

	AppendMenu(m_menuOrder, MF_STRING, IDM_ModOrder, L"期现新配");
	AppendMenu(m_menuOrder, MF_SEPARATOR, IDM_Line, L"");
	AppendMenu(m_menuOrder, MF_STRING, IDM_DelOrder, L"删除定单");

	m_labBeginDate.SetFont(g_FontData.GetFontWord13());
	m_labEndDate.SetFont(g_FontData.GetFontWord13());
	//m_labUserno.SetFont(g_FontData.GetFontWord15());

	//m_cbUserno.SetFont(g_FontData.GetFontWord15());
	//m_cbUserno.AddString(L"2222");

	m_cbtBeginDate.Init(m_beginTimePick);
	SendMessage(m_beginTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_beginTimePick, L"yyyy-MM-dd");
	m_cbtEndDate.Init(m_endTimePick);
	SendMessage(m_endTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_endTimePick, L"yyyy-MM-dd");
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtEndDate.GetHwnd(), GDT_VALID, &time);
	time.wDay = 1;
	DateTime_SetSystemtime(m_cbtBeginDate.GetHwnd(), GDT_VALID, &time);
	/////////////////////POS///////////////////////////////////
	int leftstart = 10;
	int topstart = 10;
	int editWidth = 120;	//edit控件宽度
	int labWidth = 60;		//lab控件宽度
	int btnWidth = 80;		//btn控件宽度
	int Height = 25;		//控件高度
	int htap = 10;			//横向间隔

	POINT pt;
	pt.x = leftstart;
	pt.y = topstart;
	m_labBeginDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtBeginDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labEndDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtEndDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	//m_labUserno.MoveWindow(pt.x, pt.y, labWidth, Height);
	//pt.x += labWidth + htap;
	//m_cbUserno.MoveWindow(pt.x, pt.y, editWidth, Height * 5);
	//pt.x += editWidth + htap;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listOrder.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top);
}

void OrderWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void OrderWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//表明是菜单
	{
		switch (LOWORD(wParam))
		{
		//case IDM_AddOrder:	
		//	break;
		case IDM_ModOrder://修改委托
		{
			int index = m_listOrder.GetSelectItem();
			char	temp[MAX_PATH] = { 0 };
			m_listOrder.GetSubItem(index, ORDER_SystemNo, temp, sizeof(temp));

			for (auto it : m_System->m_vecOrder)
			{
				if (strcmp(it.SystemNo, temp) == 0)
				{
					if (!IsWindow(m_modifyOdrerWnd.GetHwnd()))
					{
						m_modifyOdrerWnd.ShowFrm(m_Hwnd);
						m_modifyOdrerWnd.Register(m_System);
						m_modifyOdrerWnd.InitWnd(it);
					}
					else
					{
						if (!IsWindowVisible(m_modifyOdrerWnd.GetHwnd()))
							ShowWindow(m_modifyOdrerWnd.GetHwnd(), SW_SHOW);
						m_modifyOdrerWnd.InitWnd(it);
					}
					break;
				}
			}
		}
			break;
		case IDM_DelOrder://删除委托
			DealDelOrder();
			break;
		default:
			break;
		}
	}
}

void OrderWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Order:
		m_listOrder.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void OrderWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Order:
		DealOrderList(lParam);
		break;
	default:
		break;
	}
}

LRESULT OrderWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void OrderWnd::OnNcDestroy()
{

}

void OrderWnd::OnDestroy()
{
}

void OrderWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listOrder.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void OrderWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listOrder.OnMeasureItem(p->itemHeight);
}

void OrderWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	default:
		break;
	}
}

void OrderWnd::DealOrderList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listOrder.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuOrder, IDM_ModOrder, MF_GRAYED);
			EnableMenuItem(m_menuOrder, IDM_DelOrder, MF_GRAYED);
		}

		ShowMenu(m_menuOrder);
	}
}

void OrderWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void OrderWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_ModOrder; i <= IDM_DelOrder; i++)
		EnableMenuItem(m_menuOrder, i, flage);
}

void OrderWnd::OnBtnQry()
{
	if (!m_System)return;

	SmsOrderQryReq qryReq;
	memset(&qryReq, 0, sizeof(SmsOrderQryReq));
	char temp[MAX_PATH] = { 0 };
	//序列号
	qryReq.SerialId = 0;
	//公司
	strcpy_s(qryReq.CompanyNo, m_System->m_curLogin.CompanyNo);

	//用户
	strcpy_s(qryReq.UserNo, m_System->m_curLogin.UserNo);
	//开始日期
	wstring tempDate1;
	m_cbtBeginDate.CxGetWindowText(tempDate1);
	strcpy_s(qryReq.BeginDate, LoadRC::unicode_to_ansi(tempDate1).c_str());
	//终止日期
	wstring tempDate2;
	m_cbtEndDate.CxGetWindowText(tempDate2);
	strcpy_s(qryReq.EndDate, LoadRC::unicode_to_ansi(tempDate2).c_str());

	m_listOrder.DeleteAllItems();

	m_System->m_vecOrder.clear();
	g_pSpotSystem->QryOrder(&qryReq);
}

void OrderWnd::DealDelOrder()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "提示", L"确定删除此委托么？", TMB_OKCANCEL))
		return;
	if (!m_System)return;

	SmsOrderDelRsp delReq;
	memset(&delReq, 0, sizeof(delReq));

	char	temp[MAX_PATH] = { 0 };
	int row = m_listOrder.GetSelectItem();
	//公司编号
	strncpy_s(delReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	//交易员编号
	memset(&temp, 0, sizeof(temp));
	m_listOrder.GetSubItem(row, ORDER_UserNo, temp, sizeof(temp));
	strncpy_s(delReq.UserNo, temp);
	//结算日期
	memset(&temp, 0, sizeof(temp));
	m_listOrder.GetSubItem(row, ORDER_SettleDate, temp, sizeof(temp));
	strncpy_s(delReq.SettleDate, temp);
	//合同编号
	//memset(&temp, 0, sizeof(temp));
	//m_listOrder.GetSubItem(row, ORDER_AgreementNo, temp, sizeof(temp));
	//strncpy_s(delReq.AgreementNo, temp);
	//合约编号
	memset(&temp, 0, sizeof(temp));
	m_listOrder.GetSubItem(row, ORDER_ContractNo, temp, sizeof(temp));
	TContractKey conKey;
	memset(&conKey, 0, sizeof(TContractKey));
	CConvert::TContractNoToTContractKey(temp, conKey);
	char sCode[101] = { 0 };
	CConvert::TContractKeyToSContractNo(conKey, sCode);
	strncpy_s(delReq.ContractNo, sCode);
	//委托编号
	memset(&temp, 0, sizeof(temp));
	m_listOrder.GetSubItem(row, ORDER_OrderNo, temp, sizeof(temp));
	strncpy_s(delReq.OrderNo, temp);
	//系统号
	memset(&temp, 0, sizeof(temp));
	m_listOrder.GetSubItem(row, ORDER_SystemNo, temp, sizeof(temp));
	strncpy_s(delReq.SystemNo, temp);
	//买卖方向
	memset(&temp, 0, sizeof(temp));
	m_listOrder.GetSubItem(row, ORDER_Direct, temp, sizeof(temp));
	if (strcmp(temp, "买") == 0)
		delReq.Direct = SMS_DIRECT_BUY;
	else
		delReq.Direct = SMS_DIRECT_SELL;
	//开平
	memset(&temp, 0, sizeof(temp));
	m_listOrder.GetSubItem(row, ORDER_Offset, temp, sizeof(temp));
	if (strcmp(temp, "开仓") == 0)
		delReq.Offset = SMS_O_OPEN;
	else
		delReq.Offset = SMS_O_COVER;
	//投保类型
	memset(&temp, 0, sizeof(temp));
	m_listOrder.GetSubItem(row, ORDER_Hedge, temp, sizeof(temp));
	if (strcmp(temp, "套保") == 0)
		delReq.Hedge = SMS_HEDGE_HEDGE;
	else
		delReq.Hedge = SMS_HEDGE_SPECULATE;
	 //操作员
	strncpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);

	g_pSpotSystem->DeleteOrder(&delReq);
	 
}

void OrderWnd::RefreshList()
{
	if (!m_System)return;

	SYSTEMTIME curtm;
	memset(&curtm, 0, sizeof(SYSTEMTIME));
	GetSystemTime(&curtm);

	for (auto it : m_System->m_vecOrder)
	{
		if (it.OrderState != SMS_OS_FILLED && it.OrderState != SMS_OS_PARTFILLED)
		{
			SYSTEMTIME tm;
			memset(&tm, 0, sizeof(SYSTEMTIME));

			vector<string> vdate;
			vdate = m_System->SplitString(it.SettleDate, '-');
			if (vdate.size() < 3)
				continue;
			tm.wYear = atoi(vdate.at(0).c_str());
			tm.wMonth = atoi(vdate.at(1).c_str());
			tm.wDay = atoi(vdate.at(2).c_str());

			if (curtm.wYear != tm.wYear || curtm.wDay != tm.wDay || curtm.wMonth != tm.wMonth)
				continue;
		}
		AddOrderToList(it);
	}
}
