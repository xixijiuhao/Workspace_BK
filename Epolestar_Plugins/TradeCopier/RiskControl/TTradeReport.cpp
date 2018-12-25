#include "PreInclude.h"

TTradeReport * g_pTradeReport = NULL;

#define  captionHeight  28

#define  captionColor   RGB(76,112,177)

#define  textLeft    12
#define  textWidth   80
#define  textVgap    40
#define  textHeight  20
#define  textTop     (captionHeight+90)

#define  chartWidth  350
#define  chartHeight 240
#define  chartTop    (captionHeight+ 45)
#define  chartLeft   165

#define  labelLeft   (textLeft+textWidth)
#define  labelTop    textTop
#define  labelWidth  65
#define  labelHeight textHeight
#define  labelNum    4

#define  lvLeft      18
#define  lvTop       (chartTop+chartHeight + 10)
#define  lvWidth     504
#define  lvHeight    378

#define  widgetLeft  20
#define  widgetTop   (captionHeight + 15)
#define  widgetHeight 20
#define  labelTimeWidth  60
#define  cbWidth     150
#define  ckWidth     75
#define  btnWidth    120

#define  frameWidth  550
#define  frameHeight 710


#define  closeWidth  9

#define MainFrmName L"class TMainFrame"
TTradeReport::TTradeReport()
{
	m_dProfit = 0.0;
	m_hbrStatic = CreateSolidBrush(RGB(255, 255, 255));
	m_iTimeRange = 8; //all

	m_bIsFullScreen = false;
	m_bTrackMouse = false;

	m_boxState[0] = boxStateNormal;
	m_boxState[1] = boxStateNormal;
}
TTradeReport::~TTradeReport()
{
	DeleteObject(m_hbrStatic);
}
void TTradeReport::ShowFrm()
{
	CreateFrm(L"TTradeRepoerFrm", FindWindow(MainFrmName,NULL), WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN/*|WS_SYSMENU*/ | WS_CLIPSIBLINGS, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	RECT rect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	SetWindowPos(m_Hwnd, 0, rect.left + (rect.right - rect.left - frameWidth) / 2, rect.top + (rect.bottom - rect.top - frameHeight) / 2, frameWidth, frameHeight, SWP_SHOWWINDOW);
	SetSize();
}

void TTradeReport::MoveWindow()
{
	RECT rect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	if (m_bIsFullScreen)
	{
		SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
	}
	else
	{
		SetWindowPos(m_Hwnd, 0, rect.left + (rect.right - rect.left - frameWidth) / 2, rect.top + (rect.bottom - rect.top - frameHeight) / 2, frameWidth, frameHeight, SWP_SHOWWINDOW);
	}
	SetSize();

}
void TTradeReport::SetSize()
{
	RECT rect = { 0 };
	GetClientRect(m_Hwnd, &rect);
	m_rcClose.right = rect.right - 8;
	m_rcClose.left = m_rcClose.right - closeWidth;
	m_rcClose.top = 9;
	m_rcClose.bottom = m_rcClose.top + closeWidth;

	m_rcMaxMinBox.right = m_rcClose.left - 8;
	m_rcMaxMinBox.left = m_rcMaxMinBox.right - 11;
	m_rcMaxMinBox.top = m_rcClose.top;
	m_rcMaxMinBox.bottom = m_rcMaxMinBox.top + 11;

	if (m_bIsFullScreen)
	{		
		m_lvData.MoveWindow(textLeft, lvTop - 20, lvWidth, rect.bottom - lvTop);
		m_frmChart.MoveWindow(textLeft + lvWidth + 20, captionHeight + 30, rect.right - textLeft - lvWidth - 35, rect.bottom - captionHeight - 50);
	}
	else
	{
		m_frmChart.MoveWindow(chartLeft, chartTop, chartWidth, chartHeight);
		m_lvData.MoveWindow(textLeft, lvTop, lvWidth, lvHeight);

	}
}
void TTradeReport::OnCreate()
{
	m_labelTime.Create(m_Hwnd, L"报告范围");
	m_labelTime.SetFont(g_FontData.g_FontWord12);
	int left = widgetLeft;
	int top = widgetTop;
	m_labelTime.MoveWindow(left, top, labelTimeWidth, widgetHeight);
	m_cbTime.Create(m_Hwnd);
	left += labelTimeWidth + 5;
	m_cbTime.MoveWindow(left, top, cbWidth, widgetHeight);

	left += cbWidth + 20;
	m_btnReport.CreateButton(m_Hwnd, "生成报告", 0);
	m_btnReport.MoveWindow(left, top, cbWidth, widgetHeight);

	for (int i = 0; i < labelNum; i++)
	{
		top = labelTop + i*textVgap;
		m_labelMoney[i].Create(m_Hwnd, L"", i, SS_LEFT);
		m_labelMoney[i].MoveWindow(labelLeft, top, labelWidth, labelHeight);
	}
	m_lvData.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 21, 18, LVS_EX_FULLROWSELECT);
	m_lvData.MoveWindow(lvLeft, lvTop, lvWidth, lvHeight);
	ShowWindow(m_lvData.GetListHwnd(), SW_SHOW);
	InitLV();

	m_pVecContractNo = m_frmData.GetCbString();
	m_pVecTradeData = m_frmData.GetTradeData();

	m_frmChart.RegistData(m_frmData.GetTradeData());
	m_frmChart.Create(m_Hwnd);
	m_frmChart.MoveWindow(chartLeft, chartTop, chartWidth, chartHeight);

	char szContent[][30] = { "第一节(21:00--22:00)", "第二节(22:00至夜盘收盘）", "第三节(09:00--10:15)",
		"第四节(10:15--11:30)", "第五节(13:00--14:15)", "第六节(14:15--15:15)", "白天(09:00--15:15)", "晚上(21:00--02:30)", "全部(21:00--15:15)" };
	for (int i = 0; i < 9; i++)
		m_cbTime.AddString(szContent[i]);
	m_cbTime.SetSelect(8);

	if (g_pTradeStatistic)
		g_pTradeStatistic->Ts_Init();
}
void TTradeReport::InitLV()
{
	m_lvData.AddColumn(L"统计指标", 150);
	m_lvData.AddColumn(L"全部", 118);
	m_lvData.AddColumn(L"做多", 118);
	m_lvData.AddColumn(L"做空", 118);

	char szText[][30] = { "净盈亏", "手续费", "盈亏额(不含手续费)", "盈利金额", "亏损金额", 
		"交易次数", "盈利次数", "亏损次数","原价进出次数","最大连盈次数", "最大连亏次数","胜率(总)", "盈亏比(总)",
		"平均每笔盈利", "平均每笔亏损", "最大连盈金额","最大连亏金额", "最大单笔盈利", "最大单笔亏损", 
		"均持仓时间", "持盈单平均时间", "持亏单平均时间", "持盈单最长时间", "持亏单最长时间" };
	int iCount = sizeof(szText) / sizeof(szText[0]);
	for (int i = 0; i < iCount; i++)
	{
		m_lvData.AddItem(szText[i], i, 0);
	}
}
void TTradeReport::SetLVData()
{
	if (m_iTimeRange == -1 || g_pTradeStatistic==NULL)
		return;
	int iRow = 0;
	TIME_PERIOD time_period = (TIME_PERIOD)m_iTimeRange;

	m_lvData.AddItem(g_pTradeStatistic->Ts_Get_Total_Profit(WHOLE_CONTRACT, WHOLE, time_period), iRow++, 1);
	m_lvData.AddItem(g_pTradeStatistic->Ts_Get_Total_Commision(WHOLE_CONTRACT, WHOLE, time_period), iRow++, 1);

	m_lvData.AddItem(g_pTradeStatistic->Ts_Get_Loss(WHOLE_CONTRACT, WHOLE, time_period) + g_pTradeStatistic->Ts_Get_Profit(WHOLE_CONTRACT, WHOLE, time_period), iRow++, 1);

	for (int i = WHOLE; i <= SELL; i++)
	{
		ST_TYPE type = (ST_TYPE)i;
		iRow = 3;
		m_lvData.AddItem(g_pTradeStatistic->Ts_Get_Profit(WHOLE_CONTRACT, type, time_period), iRow++, i);
		m_lvData.AddItem((int)g_pTradeStatistic->Ts_Get_Loss(WHOLE_CONTRACT, type, time_period), iRow++, i);
	}
	

	for (UINT i = 0; i < m_pVecContractNo->size(); i++)
	{
		m_lvData.AddItem(m_pVecContractNo->at(i).c_str(), iRow, 0);
		double dData = g_pTradeStatistic->Ts_Get_Profit(m_pVecContractNo->at(i).c_str(), WHOLE, time_period) +
			g_pTradeStatistic->Ts_Get_Loss(m_pVecContractNo->at(i).c_str(), WHOLE, time_period);
		m_lvData.AddItem(dData, iRow, 1);
		dData = g_pTradeStatistic->Ts_Get_Profit(m_pVecContractNo->at(i).c_str(), BUY, time_period) +
			g_pTradeStatistic->Ts_Get_Loss(m_pVecContractNo->at(i).c_str(), BUY, time_period);
		m_lvData.AddItem(dData, iRow, 2);
		dData = g_pTradeStatistic->Ts_Get_Profit(m_pVecContractNo->at(i).c_str(), SELL, time_period) +
			g_pTradeStatistic->Ts_Get_Loss(m_pVecContractNo->at(i).c_str(), SELL, time_period);
		m_lvData.AddItem(dData, iRow, 3);
		iRow++;
	}
	int iCount = iRow;
	for (int i = WHOLE; i <= SELL; i++)
	{
		iRow = iCount;
		ST_TYPE type = (ST_TYPE)i;
		int tradeCount = g_pTradeStatistic->Ts_Get_Count(WHOLE_CONTRACT, type, time_period);
		int winCount = g_pTradeStatistic->Ts_Get_Profit_Loss_Count(true, WHOLE_CONTRACT, type, time_period);
		int lossCount = g_pTradeStatistic->Ts_Get_Profit_Loss_Count(false,WHOLE_CONTRACT, type, time_period);
		m_lvData.AddItem(tradeCount, iRow++, i);
		m_lvData.AddItem(winCount, iRow++, i);
		m_lvData.AddItem(lossCount, iRow++, i);	
		int iCount = tradeCount - winCount - lossCount;
		iCount = iCount > 0 ? iCount : 0;
		m_lvData.AddItem(iCount, iRow++, i);

		m_lvData.AddItem((int)g_pTradeStatistic->Ts_Get_Consecutive_Win_count(WHOLE_CONTRACT, type, time_period), iRow++, i);
		m_lvData.AddItem((int)g_pTradeStatistic->Ts_Get_Consecutive_Loss_count(WHOLE_CONTRACT, type, time_period), iRow++, i);

		m_lvData.AddItem(g_pTradeStatistic->Ts_Get_WinRate(WHOLE_CONTRACT, type, time_period), iRow++, i);
		m_lvData.AddItem(g_pTradeStatistic->Ts_Get_PLRatio(WHOLE_CONTRACT, type, time_period), iRow++, i);

		double dMoney = g_pTradeStatistic->Ts_Get_Profit(WHOLE_CONTRACT, type, time_period);
		dMoney = winCount > 0 ? dMoney / winCount : 0;
		m_lvData.AddItem(dMoney, iRow++, i);

		dMoney = g_pTradeStatistic->Ts_Get_Loss(WHOLE_CONTRACT, type, time_period);
		dMoney = lossCount > 0 ? dMoney / lossCount : 0;
		m_lvData.AddItem(dMoney, iRow++, i);

		m_lvData.AddItem(g_pTradeStatistic->Ts_Get_Consecutive_Win_Money(WHOLE_CONTRACT, type, time_period), iRow++, i);
		m_lvData.AddItem(g_pTradeStatistic->Ts_Get_Consecutive_Loss_Money(WHOLE_CONTRACT, type, time_period), iRow++, i);

		m_lvData.AddItem(g_pTradeStatistic->Ts_Get_Biggest_Profit(WHOLE_CONTRACT, type, time_period), iRow++, i);
		m_lvData.AddItem(g_pTradeStatistic->Ts_Get_Biggest_Loss(WHOLE_CONTRACT, type, time_period), iRow++, i);


	}
	char strTime[50] = { 0 };
	FormatTime(strTime, sizeof(strTime), g_pTradeStatistic->Ts_Get_Pos_Hold_Time(WHOLE_CONTRACT, time_period));
	m_lvData.AddItem(strTime, iRow++, 1);
	FormatTime(strTime, sizeof(strTime), g_pTradeStatistic->Ts_Get_Win_Pos_Hold_Time(WHOLE_CONTRACT, time_period));
	m_lvData.AddItem(strTime, iRow++, 1);
	FormatTime(strTime, sizeof(strTime), g_pTradeStatistic->Ts_Get_Loss_Pos_Hold_Time(WHOLE_CONTRACT, time_period));
	m_lvData.AddItem(strTime, iRow++, 1);
	FormatTime(strTime, sizeof(strTime), g_pTradeStatistic->Ts_Get_Profit_Max_Pos_Hold_Time(WHOLE_CONTRACT, time_period));
	m_lvData.AddItem(strTime, iRow++, 1);
	FormatTime(strTime, sizeof(strTime), g_pTradeStatistic->Ts_Get_Loss_Max_Pos_Hold_Time(WHOLE_CONTRACT, time_period));
	m_lvData.AddItem(strTime, iRow++, 1);
}
LRESULT TTradeReport::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave(wParam, lParam);
		return PROCESSED;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawLV(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM:
		OnMeasureLV(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtrlStatic(wParam, lParam);
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case SSWM_COMBOX_SELECT:
		OnComboxSelect(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnClickBtn(wParam, lParam);
		return PROCESSED;
	case WM_NCDESTROY:
		if (g_pTradeStatistic)
			g_pTradeStatistic->Ts_UnInit();	
		delete this;
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
void TTradeReport::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bTrackMouse)
	{
		TRACKMOUSEEVENT t;
		t.cbSize = sizeof(TRACKMOUSEEVENT);
		t.dwFlags = TME_LEAVE;
		t.dwHoverTime = 0;
		t.hwndTrack = m_Hwnd;
		m_bTrackMouse = (TRUE == TrackMouseEvent(&t));
	}
	if (MK_LBUTTON == wParam)
		return;
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	if (PtInRect(&m_rcMaxMinBox, pt))
	{
		m_boxState[0] = boxStateHover;
		m_boxState[1] = boxStateNormal;
	}
	else
	{
		m_boxState[0] = boxStateNormal;
	}
	if (PtInRect(&m_rcClose, pt))
	{
		m_boxState[0] = boxStateNormal;
		m_boxState[1] = boxStateHover;
	}
	else
	{
		m_boxState[1] = boxStateNormal;
	}
	Redraw();
}
void TTradeReport::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_boxState[0] = boxStateNormal;
	m_boxState[1] = boxStateNormal;
	Redraw();
}
void TTradeReport::OnDrawLV(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = LPDRAWITEMSTRUCT(lParam);
	if (pDIS->CtlType == ODT_LISTVIEW)
		m_lvData.OnDrawItem(lParam);
}
void TTradeReport::OnMeasureLV(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT pmi = LPMEASUREITEMSTRUCT(lParam);
	if (pmi->CtlType == ODT_LISTVIEW)
		m_lvData.OnMeasureItem(pmi->itemHeight);
}
LRESULT TTradeReport::OnCtrlStatic(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)m_hbrStatic;
}
void TTradeReport::OnComboxSelect(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == m_cbTime.GetHwnd())
	{
		m_iTimeRange = wParam;
		m_frmData.SetTimeRange(m_iTimeRange);
	}
}


void TTradeReport::OnHotKeyDown(const bool Alt, const bool Ctrl, const bool Shift, const unsigned char VKey, const bool Exclusive, HWND source)
{
	TCHAR name[50] = { 0 };
	GetClassName(source, name, sizeof(name) / sizeof(TCHAR));
	if (lstrcmp(name, WC_EDIT) == 0)
		return;
	if (VKey == VK_F4)
	{
		if (!g_pTradeReport->IsAuthent()) return;
		if (g_pTradeReport == NULL)
		{
			TDataContainer users;
			g_pTradeData->GetAllUser(users);
			int ret = 0;
			if (users.size() == 0)
			{
				_TMessageBox_Domodal(GetActiveWindow(), "提示", L"用户未登录，请先登录！", TMB_OK);
				ILoginFrm *pLogin = (ILoginFrm*)g_pPluginMgrApi->CreatePluginApi(PLUG_LOGINFRM_NAME, PLUG_LOGINFRM_VERSION);
				ret = pLogin->ShowLogin();
			}

			if (ret == 0)
			{
				g_pTradeReport = new TTradeReport;
				g_pTradeReport->ShowFrm();
			}
			else
				_TMessageBox_Domodal(GetActiveWindow(), "提示", L"用户登录失败，请重试！", TMB_OK);
			return;
		}
		if (IsWindowVisible(g_pTradeReport->GetHwnd()))
			ShowWindow(g_pTradeReport->GetHwnd(), SW_HIDE);
		else
			ShowWindow(g_pTradeReport->GetHwnd(), SW_SHOW);
	}
}
void TTradeReport::OnClickBtn(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0)
	{
		ShowWindow(m_lvData.GetListHwnd(), SW_HIDE);
		if (g_pTradeStatistic)
		{
			for (UINT i = 0; i < m_pVecContractNo->size(); i++)
				m_lvData.DeleteItem(5);
		}
		m_dProfit = 0.0;
		m_dFee = 0.0;
		m_frmData.GetData();
		m_dFee = m_frmData.GetFee();
		m_dProfit = m_frmData.GetProfit();
		m_frmChart.SetData(m_pVecTradeData);
		SetLabelText();//显示初始，期末金额，亏损值
		SetLVData();
		ShowWindow(m_lvData.GetListHwnd(), SW_SHOW);
	}//if(lParam==0)
}

void TTradeReport::SetLabelText()
{	
	if (m_pVecTradeData->size())
	{
		char szText[100] = { 0 };
		long long dMoney[labelNum] = { m_pVecTradeData->at(0), m_dProfit, m_dFee + 0.5, m_pVecTradeData->at(m_pVecTradeData->size() - 1) };
		if (m_dProfit > 0)
			dMoney[1] += 0.5;
		else
			dMoney[1] -= 0.5;
		for (int i = 0; i < labelNum; i++)
		{
			sprintf_s(szText, "%ld", dMoney[i]);
			m_labelMoney[i].SetText(szText);
		}
	}
	else
	{
		for (int i = 0; i < labelNum; i++)
			m_labelMoney[i].SetText("");
	}
}
void TTradeReport::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	if (PtInRect(&m_rcMaxMinBox, pt))
	{
		m_bIsFullScreen = !m_bIsFullScreen;
		memset(&m_boxState, 0, sizeof(m_boxState));
		MoveWindow();
	}
	if (PtInRect(&m_rcClose, pt))
	{
		memset(&m_boxState, 0, sizeof(m_boxState));
		ShowWindow(m_Hwnd, SW_HIDE);
	}

}
void TTradeReport::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	rect.bottom = captionHeight;
	if (PtInRect(&m_rcClose, pt))
	{
		m_boxState[1] = boxStateClicked;
		m_boxState[0] = boxStateNormal;
		Redraw();
	}
	else if (PtInRect(&m_rcMaxMinBox,pt))
	{
		m_boxState[0] = boxStateClicked;
		m_boxState[1] = boxStateNormal;
		Redraw();
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}
void TTradeReport::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord13);
	DrawFrame(&memdc);
	DrawClose(&memdc);
	DrawText(&memdc);
}
void TTradeReport::DrawMaxMinBox(TMemDC *pMemDC)
{
	HBRUSH hbr = CreateSolidBrush(RGB(230, 230, 230));
	RECT rect = { 0 };
	int offset = m_boxState[0] == boxStateClicked ? 1 : 0;
	if (m_bIsFullScreen)
	{
		rect = { m_rcMaxMinBox.left + offset, m_rcMaxMinBox.top + 3 + offset, m_rcMaxMinBox.right - 2 + offset, m_rcMaxMinBox.bottom + offset };
		MoveToEx(pMemDC->GetHdc(), m_rcMaxMinBox.left + 3 + offset, rect.top + offset, 0);
		LineTo(pMemDC->GetHdc(), m_rcMaxMinBox.left + 3 + offset, m_rcMaxMinBox.top + offset);

		MoveToEx(pMemDC->GetHdc(), m_rcMaxMinBox.left + 3 + offset, m_rcMaxMinBox.top + offset, 0);
		LineTo(pMemDC->GetHdc(), m_rcMaxMinBox.right + offset, m_rcMaxMinBox.top + offset);
		MoveToEx(pMemDC->GetHdc(), m_rcMaxMinBox.left + 3+offset , m_rcMaxMinBox.top + 1 + offset, 0);
		LineTo(pMemDC->GetHdc(), m_rcMaxMinBox.right + offset, m_rcMaxMinBox.top + 1 + offset);

		MoveToEx(pMemDC->GetHdc(), m_rcMaxMinBox.right + offset, m_rcMaxMinBox.top + offset, 0);
		LineTo(pMemDC->GetHdc(), m_rcMaxMinBox.right + offset, m_rcMaxMinBox.bottom - 3 + offset);

		MoveToEx(pMemDC->GetHdc(), m_rcMaxMinBox.right + offset, m_rcMaxMinBox.bottom - 3 + offset, 0);
		LineTo(pMemDC->GetHdc(), rect.right - 1 + offset, m_rcMaxMinBox.bottom - 3 + offset);

	}
	else
	{
		rect = m_rcMaxMinBox;
		OffsetRect(&rect, offset, offset);
	}
	FrameRect(pMemDC->GetHdc(), &rect, hbr);
	MoveToEx(pMemDC->GetHdc(), rect.left, rect.top + 1, 0);
	LineTo(pMemDC->GetHdc(), rect.right, rect.top + 1);
	DeleteObject(hbr);
}
void TTradeReport::DrawClose(TMemDC *pMemDC)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(230, 230, 230));
	HPEN hOldPen = (HPEN)SelectObject(pMemDC->GetHdc(), hPen);

	int offset = m_boxState[1] == boxStateClicked ? 1 : 0;
	POINT ptCloseBegin[4] = { 0 };
	POINT ptCloseEnd[4] = { 0 };
	ptCloseBegin[0] = { m_rcClose.left + offset, m_rcClose.top + offset };
	ptCloseBegin[1] = { m_rcClose.left + 1 + offset, m_rcClose.top + offset };
	ptCloseBegin[2] = { m_rcClose.left + offset, m_rcClose.bottom + offset };
	ptCloseBegin[3] = { m_rcClose.left + 1 + offset, m_rcClose.bottom + offset };

	ptCloseEnd[0] = { m_rcClose.right + 1 + offset, m_rcClose.bottom + 1 + offset };
	ptCloseEnd[1] = { m_rcClose.right + 2 + offset, m_rcClose.bottom + 1 + offset };
	ptCloseEnd[2] = { m_rcClose.right + 1 + offset, m_rcClose.top - 1 + offset };
	ptCloseEnd[3] = { m_rcClose.right + 2 + offset, m_rcClose.top - 1 + offset };
	for (int i = 0; i < 4; i++)
	{
		MoveToEx(pMemDC->GetHdc(), ptCloseBegin[i].x, ptCloseBegin[i].y, 0);
		LineTo(pMemDC->GetHdc(), ptCloseEnd[i].x, ptCloseEnd[i].y);
	}

	DrawMaxMinBox(pMemDC);

	SelectObject(pMemDC->GetHdc(), hOldPen);
	DeleteObject(hPen);
}
void TTradeReport::DrawFrame(TMemDC *pMemDC)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 255));
	int bottom = rect.bottom;
	rect.bottom = captionHeight;
	HBRUSH hbrCaption = CreateSolidBrush(/*RGB(6, 157, 213)*/captionColor);
	FillRect(pMemDC->GetHdc(), &rect, hbrCaption);
	RECT rcHead = rect;
	rcHead.left = 8;
	SetTextColor(pMemDC->GetHdc(), RGB(255, 255, 255));
	DrawTextA(pMemDC->GetHdc(), "交易统计分析", -1, &rcHead, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	rect.top = rect.bottom - 2;
	rect.bottom = bottom;
	FrameRect(pMemDC->GetHdc(), &rect, hbrCaption);
	InflateRect(&rect, -1, -1);
	FillRect(pMemDC->GetHdc(), &rect, hbr);

	DeleteObject(hbr);
	DeleteObject(hbrCaption);
}


void TTradeReport::DrawTextW(TMemDC *pMemDC)
{
	SetTextColor(pMemDC->GetHdc(), RGB(10, 10, 10));
	SelectObject(pMemDC->GetHdc(), g_FontData.g_FontWord12);
	RECT rcText = { textLeft, textTop, textWidth + textLeft , textHeight+textTop };
	DrawTextA(pMemDC->GetHdc(), "期初账户净值", -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	rcText.top += textVgap;
	rcText.bottom += textVgap;
	DrawTextA(pMemDC->GetHdc(), "交易盈亏", -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	rcText.top += textVgap;
	rcText.bottom += textVgap;
	DrawTextA(pMemDC->GetHdc(), "手续费", -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	rcText.top += textVgap;
	rcText.bottom += textVgap;
	DrawTextA(pMemDC->GetHdc(), "期末账户净值", -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void TTradeReport::FormatTime(char *pTimeStr, int length, double dSecond)
{
	if (!pTimeStr) return;
	if (dSecond < 60)
		sprintf_s(pTimeStr, length, "%.2fs", dSecond);
	else
	{
		int iSecond = (int)dSecond;
		pTimeStr[0] = '\0';
		char strTime[10] = { 0 };
		if (iSecond >= 86400)
		{
			int day = iSecond / 86400;
			iSecond = iSecond % 86400;
			sprintf_s(pTimeStr,length, "%d天 ", day);
		}
		strTime[0] = iSecond / 36000 + 0x30;
		strTime[1] = iSecond % 36000 / 3600 + 0x30;
		strTime[2] = ':';
		strTime[3] = iSecond % 3600 / 600 + 0x30;
		strTime[4] = iSecond % 600 / 60 + 0x30;
		strTime[5] = ':';
		strTime[6] = iSecond % 60 / 10 + 0x30;
		strTime[7] = iSecond % 10 + 0x30;
		strTime[8] = '\0';
		strcat_s(pTimeStr,length, strTime);
	}
}

void TTradeReport::Redraw()
{
	RECT rect = m_rcMaxMinBox; 
	rect.right = m_rcClose.right;
	InflateRect(&rect, 2, 2);
	::RedrawWindow(m_Hwnd, &rect, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
}

bool TTradeReport::IsAuthent()
{
	if (!ReadVersion()) return true;
	if (!g_pAuthent)
	{
		_TMessageBox_Domodal(FindWindow(MainFrmName, NULL), "提示", L"认证出错！", TMB_OK);
		return false;
	}

	UserAuthentEx stAuthent;
	memset(&stAuthent, 0, sizeof(UserAuthentEx));
	bool bRet = g_pAuthent->GetAuthent("GDLH", "tf1685", stAuthent);

	if (stAuthent.serverdate < stAuthent.auth.value[0].date)
		return true;
	TAccessAlarm alarmFrm(FindWindow(MainFrmName, NULL), "认证未通过");
	alarmFrm.Domodal();
	return false;
}


//true:Need Authent
//false:No Need Authent
bool TTradeReport::ReadVersion()
{
	TCHAR szFilePath[200] = { 0 };
	GetModuleFileName(0, szFilePath, sizeof(szFilePath)/sizeof(szFilePath[0]));
	WCHAR* cp_pos = wcsrchr(szFilePath, '\\');
	if (0 != cp_pos)
		cp_pos[0] = 0;
	wcscat_s(szFilePath, L"\\config\\VersionInfo.pri");

	FILE *pFile = nullptr;
	pFile = _wfsopen(szFilePath, L"r", _SH_DENYWR);
	if (pFile)
	{
		fclose(pFile);
	}
	return pFile == nullptr;
}