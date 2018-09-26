#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_H    610
#define cFRAME_V    400

extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
AddMatchWnd::AddMatchWnd()
	:m_title("录入持仓")
{
	m_bMouseTrack = false;
}

//AddMatchWnd::~AddMatchWnd()
//{
//	
//}

void AddMatchWnd::ShowFrm(HWND hParent)
{
	CreateFrm(L"TAddMatchWnd", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_H, cFRAME_V, SWP_SHOWWINDOW);
}

void AddMatchWnd::Register(SystemFrame * pdata)
{
	if (!pdata)return;
	m_subSystem = pdata;
}

void AddMatchWnd::InitWnd()
{
	m_title = "录入成交";
	EnableWindow(m_labCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_editCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_labUserNo.GetHwnd(), FALSE);
	EnableWindow(m_editUserNo.GetHwnd(), FALSE);
	EnableWindow(m_labSettleDate.GetHwnd(), TRUE);
	EnableWindow(m_cbtSettleDate.GetHwnd(), TRUE);
	EnableWindow(m_labContractNo.GetHwnd(), TRUE);
	EnableWindow(m_editContractNo.GetHwnd(), TRUE);
	EnableWindow(m_btnContractNo.GetHwnd(), TRUE);
	EnableWindow(m_labMatchNo.GetHwnd(), TRUE);
	EnableWindow(m_editMatchNo.GetHwnd(), TRUE);
	EnableWindow(m_labSystemNo.GetHwnd(), TRUE);
	EnableWindow(m_editSystemNo.GetHwnd(), TRUE);

	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);

	if (!m_subSystem)return;

	//填数据
	m_editCompanyNo.SetText(m_subSystem->m_curLogin.CompanyNo);
	m_editUserNo.SetText(m_subSystem->m_curLogin.UserNo);

	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);
	DateTime_SetSystemtime(m_cbtMatchTime.GetHwnd(), GDT_VALID, &time);

	m_editContractNo.SetText("");
	m_editMatchNo.SetText("");
	m_editSystemNo.SetText("");
	m_editOrderNo.SetText("");
	m_editLocalNo.SetText("");
	//m_editDirect.SetText("");
	//m_editOffset.SetText("");
	//m_editHedge.SetText("");
	m_editMatchPrice.SetText("");
	m_editMatchQty.SetText("");
	m_editMatchFee.SetText("");
}

void AddMatchWnd::InitModifyWnd(SmsMatchDataRsp & match)
{
	m_title = "修改成交";
	EnableWindow(m_labCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_editCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_labUserNo.GetHwnd(), FALSE);
	EnableWindow(m_editUserNo.GetHwnd(), FALSE);
	EnableWindow(m_labSettleDate.GetHwnd(), FALSE);
	EnableWindow(m_cbtSettleDate.GetHwnd(), FALSE);
	EnableWindow(m_labContractNo.GetHwnd(), FALSE);
	EnableWindow(m_editContractNo.GetHwnd(), FALSE);
	EnableWindow(m_btnContractNo.GetHwnd(), FALSE);
	EnableWindow(m_labMatchNo.GetHwnd(), FALSE);
	EnableWindow(m_editMatchNo.GetHwnd(), FALSE);
	EnableWindow(m_labSystemNo.GetHwnd(), FALSE);
	EnableWindow(m_editSystemNo.GetHwnd(), FALSE);

	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);

	//填数据
	m_editCompanyNo.SetText(match.CompanyNo);
	m_editUserNo.SetText(match.UserNo);

	vector<string> vdate;
	vdate = m_subSystem->SplitString(match.SettleDate, '-');
	if (vdate.size() < 3)
		return;
	SYSTEMTIME time;
	memset(&time, 0, sizeof(SYSTEMTIME));
	time.wYear = atoi(vdate.at(0).c_str());
	time.wMonth = atoi(vdate.at(1).c_str());
	time.wDay = atoi(vdate.at(2).c_str());
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);

	string sCode = match.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	m_editContractNo.SetText(tCode.c_str());

	m_editMatchNo.SetText(match.MatchNo);
	m_editSystemNo.SetText(match.SystemNo);
	m_editOrderNo.SetText(match.OrderNo);
	m_editLocalNo.SetText(match.LocalNo);

	if (match.Direct == SMS_DIRECT_BUY)
		m_editDirect.SetText("买");
	else
		m_editDirect.SetText("卖");
	if (match.Offset == SMS_O_OPEN)
		m_editOffset.SetText("开仓");
	else
		m_editOffset.SetText("平仓");
	if (match.Hedge == SMS_HEDGE_SPECULATE)
		m_editHedge.SetText("投机");
	else
		m_editHedge.SetText("套保");
	
	SYSTEMTIME matchtime;
	memset(&matchtime, 0, sizeof(SYSTEMTIME));
	m_subSystem->StrTimeToSysTime(match.MatchTime, matchtime);
	DateTime_SetSystemtime(m_cbtMatchTime.GetHwnd(), GDT_VALID, &matchtime);

	m_editMatchPrice.SetDoubleData(match.MatchPrice, 2);
	m_editMatchQty.SetDoubleData(match.MatchQty);
	m_editMatchFee.SetDoubleData(match.MatchFee, 2);

}

LRESULT AddMatchWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case SSWM_EDIT_DBLCLK:
		OnLDoubleClick(wParam, lParam);
		return PROCESSED;
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_NCLBUTTONDOWN:
		OnNcLButtonDown(wParam, lParam);
		return NOT_PROCESSED;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		return PROCESSED;
	case WM_KEYDOWN:
		KeyDown(wParam, lParam);
		break;
	case WM_NCDESTROY:
		OnNcDestroy();
		return NOT_PROCESSED;
	case WM_DESTROY:
		return NOT_PROCESSED;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}

void AddMatchWnd::SetContractNo(string sCode)
{
	m_editContractNo.SetText(sCode.c_str());
}



void AddMatchWnd::OnCreate()
{
	InitCommonControls();
	CreateWidget();
	InitWidget();
	SetWidgetPos();
}

void AddMatchWnd::OnPaint()
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

	RECT rcCaption = { rcClient.left, rcClient.top, rcClient.right, cCAPTION_H };

	HBRUSH hbrCaption = CreateSolidBrush(RGB(100, 100, 100));
	FillRect(memdc.GetHdc(), &rcCaption, hbrCaption);

	rcClient.top = cCAPTION_H;
	FillRect(memdc.GetHdc(), &rcClient, g_ColorRefData.GetHbrWhite());

	rcCaption.left = 8;
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord15());
	SetTextColor(memdc.GetHdc(), RGB(238, 238, 238));
	DrawTextA(memdc.GetHdc(), m_title.c_str(), -1, &rcCaption, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	//DrawClose(&memdc);
	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}

void AddMatchWnd::OnSize(WPARAM wParam, LPARAM lParam)
{

}

void AddMatchWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

void AddMatchWnd::OnMouseLeave()
{
	m_bMouseTrack = false;

}

void AddMatchWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.bottom = rect.top + cCAPTION_H;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	//if (PtInRect(&m_rcClose, pt))
	//{
	//	ShowWindow(m_Hwnd, SW_HIDE);
	//}
	//else 
	if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}

void AddMatchWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddMatchWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
}

void AddMatchWnd::KeyDown(WPARAM wParam, LPARAM lParam)
{
}

void AddMatchWnd::OnNcDestroy()
{
	//delete this;
}

void AddMatchWnd::CreateWidget()
{
	//公司
	m_labCompanyNo.Create(m_Hwnd, L"公司", DT_LEFT, ID_labCompanyNo);
	m_editCompanyNo.Create(m_Hwnd, 0, 0, ID_editCompanyNo);
	//账号
	m_labUserNo.Create(m_Hwnd, L"账号", DT_LEFT, ID_labUserNo);
	m_editUserNo.Create(m_Hwnd, 0, 0, ID_editUserNo);
	//结算日期
	m_labSettleDate.Create(m_Hwnd, L"结算日期", DT_LEFT, ID_labSettleDAate);
	m_settleTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_cbtSettleDAate, m_hInstance, NULL);
	//合约
	m_labContractNo.Create(m_Hwnd, L"合约", DT_LEFT, ID_labContractNo);
	m_editContractNo.Create(m_Hwnd, 0, 0, ID_editContractNo);
	m_btnContractNo.Create(m_Hwnd, "...", ID_btnContractNo);
	//成交号
	m_labMatchNo.Create(m_Hwnd, L"成交号", DT_LEFT, ID_labMatchNo);
	m_editMatchNo.Create(m_Hwnd, 0, 0, ID_editMatchNo);
	//系统号
	m_labSystemNo.Create(m_Hwnd, L"系统号", DT_LEFT, ID_labSystemNo);
	m_editSystemNo.Create(m_Hwnd, 0, 0, ID_editSystemNo);
	//委托号
	m_labOrderNo.Create(m_Hwnd, L"委托号", DT_LEFT, ID_labOrderNo);
	m_editOrderNo.Create(m_Hwnd, 0, 0, ID_editOrderNo);
	//本地号
	m_labLocalNo.Create(m_Hwnd, L"本地号", DT_LEFT, ID_labLocalNo);
	m_editLocalNo.Create(m_Hwnd, 0, 0, ID_editLocalNo);
	//买卖
	m_labDirect.Create(m_Hwnd, L"买卖", DT_LEFT, ID_labDirect);
	m_editDirect.Create(m_Hwnd, ES_READONLY, 0, ID_editDirect);
	//开平
	m_labOffset.Create(m_Hwnd, L"开平", DT_LEFT, ID_labOffset);
	m_editOffset.Create(m_Hwnd, ES_READONLY, 0, ID_editOffset);
	//投保
	m_labHedge.Create(m_Hwnd, L"投保", DT_LEFT, ID_labHedge);
	m_editHedge.Create(m_Hwnd, ES_READONLY, 0, ID_editHedge);
	//成交时间
	m_labMatchTime.Create(m_Hwnd, L"成交时间", DT_LEFT, ID_labSettleDAate);
	m_matchTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_cbtMatchTime, m_hInstance, NULL);
	//成交价格
	m_labMatchPrice.Create(m_Hwnd, L"成交价格", DT_LEFT, ID_labMatchPrice);
	m_editMatchPrice.Create(m_Hwnd, 0, 0, ID_editMatchPrice);
	//成交数量
	m_labMatchQty.Create(m_Hwnd, L"成交数量", DT_LEFT, ID_labMatchQty);
	m_editMatchQty.Create(m_Hwnd, 0, 0, ID_editMatchQty);
	//手续费
	m_labMatchFee.Create(m_Hwnd, L"手续费", DT_LEFT, ID_labMatchFee);
	m_editMatchFee.Create(m_Hwnd, 0, 0, ID_editMatchFee);

	m_btnOk.Create(m_Hwnd, "确  定", ID_btnOK);
	m_btnCancel.Create(m_Hwnd, "取  消", ID_btnCancel);
}

void AddMatchWnd::InitWidget()
{
	m_labCompanyNo.SetFont(g_FontData.GetFontWord15());
	m_labUserNo.SetFont(g_FontData.GetFontWord15());
	m_labSettleDate.SetFont(g_FontData.GetFontWord15());
	m_labContractNo.SetFont(g_FontData.GetFontWord15());
	m_labMatchNo.SetFont(g_FontData.GetFontWord15());
	m_labSystemNo.SetFont(g_FontData.GetFontWord15());
	m_labOrderNo.SetFont(g_FontData.GetFontWord15());
	m_labLocalNo.SetFont(g_FontData.GetFontWord15());
	m_labDirect.SetFont(g_FontData.GetFontWord15());
	m_labOffset.SetFont(g_FontData.GetFontWord15());
	m_labHedge.SetFont(g_FontData.GetFontWord15());
	m_labMatchTime.SetFont(g_FontData.GetFontWord15());
	m_labMatchPrice.SetFont(g_FontData.GetFontWord15());
	m_labMatchQty.SetFont(g_FontData.GetFontWord15());
	m_labMatchFee.SetFont(g_FontData.GetFontWord15());

	m_editCompanyNo.SetFont(g_FontData.GetFontNum15());
	m_editUserNo.SetFont(g_FontData.GetFontNum15());
	m_editContractNo.SetFont(g_FontData.GetFontNum15());
	m_editMatchNo.SetFont(g_FontData.GetFontNum15());
	m_editSystemNo.SetFont(g_FontData.GetFontNum15());
	m_editOrderNo.SetFont(g_FontData.GetFontNum15());
	m_editLocalNo.SetFont(g_FontData.GetFontNum15());
	m_editDirect.SetFont(g_FontData.GetFontNum15());
	m_editOffset.SetFont(g_FontData.GetFontNum15());
	m_editHedge.SetFont(g_FontData.GetFontNum15());
	m_editMatchPrice.SetFont(g_FontData.GetFontNum15());
	m_editMatchQty.SetFont(g_FontData.GetFontNum15());
	m_editMatchFee.SetFont(g_FontData.GetFontNum15());

	m_editDirect.SetText("买");
	m_editOffset.SetText("开仓");
	m_editHedge.SetText("套保");

	m_btnContractNo.SetFont(g_FontData.GetFontNum13());

	m_cbtSettleDate.Init(m_settleTimePick);
	SendMessage(m_settleTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_settleTimePick, L"yyyy-MM-dd");

	m_cbtMatchTime.Init(m_matchTimePick);
	SendMessage(m_matchTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_matchTimePick, L"yyyy-MM-dd HH:mm:ss");

	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);
	DateTime_SetSystemtime(m_cbtMatchTime.GetHwnd(), GDT_VALID, &time);
}

void AddMatchWnd::SetWidgetPos()
{
	int leftstart = 30;
	int topstart = 15;
	int editWidth = 160;	//edit控件宽度
	int labWidth = 80;		//lab控件宽度
	int btnWidth = 80;		//btn控件宽度
	int Height = 25;		//控件高度
	int htap = 30;			//横向间隔
	int vtap = 15;			//纵向间隔
	m_hInstance = GetModuleHandle(NULL);
	POINT pt;
	pt.x = leftstart;
	pt.y = cCAPTION_H + topstart;
	//公司号
	m_labCompanyNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCompanyNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//账号
	pt.x += editWidth + htap;
	m_labUserNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editUserNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//结算
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labSettleDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtSettleDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	//合约
	pt.x += editWidth + htap;
	m_labContractNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editContractNo.MoveWindow(pt.x, pt.y, editWidth-20, Height);
	pt.x += editWidth - 20 + 1;
	m_btnContractNo.MoveWindow(pt.x, pt.y, 19, Height);
	//成交号
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labMatchNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editMatchNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//系统号
	pt.x += editWidth + htap;
	m_labSystemNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSystemNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//委托号
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labOrderNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editOrderNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//本地号
	pt.x += editWidth + htap;
	m_labLocalNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editLocalNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//买卖
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labDirect.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editDirect.MoveWindow(pt.x, pt.y, editWidth, Height);
	//开平
	pt.x += editWidth + htap;
	m_labOffset.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editOffset.MoveWindow(pt.x, pt.y, editWidth, Height);
	//投保
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labHedge.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editHedge.MoveWindow(pt.x, pt.y, editWidth, Height);
	//成交时间
	pt.x += editWidth + htap;
	m_labMatchTime.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtMatchTime.MoveWindow(pt.x, pt.y, editWidth, Height);
	//成交价
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labMatchPrice.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editMatchPrice.MoveWindow(pt.x, pt.y, editWidth, Height);
	//成交数量
	pt.x += editWidth + htap;
	m_labMatchQty.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editMatchQty.MoveWindow(pt.x, pt.y, editWidth, Height);
	//手续费
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labMatchFee.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editMatchFee.MoveWindow(pt.x, pt.y, editWidth, Height);
	
	//确定
	//取消
	pt.x = cFRAME_H / 2 - 60 - btnWidth;
	pt.y += Height + vtap;
	m_btnOk.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + 120;
	m_btnCancel.MoveWindow(pt.x, pt.y, btnWidth, Height);
}

LRESULT AddMatchWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetHbrWhite();
}

void AddMatchWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

void AddMatchWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
}

void AddMatchWnd::OnBtnOk()
{
	if (!m_subSystem)return;

	if (strcmp(m_title.c_str(), "录入成交") == 0)
	{
		SmsMatchAddReq req;
		memset(&req, 0, sizeof(SmsMatchAddReq));

		char		atemp[MAX_PATH] = { 0 };
		wchar_t		wtemp[MAX_PATH] = { 0 };

		//序列号
		req.SerialId = 0;
		//公司编号
		memset(&atemp, 0, sizeof(atemp));
		m_editCompanyNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.CompanyNo, atemp);
		//交易员编号
		memset(&atemp, 0, sizeof(atemp));
		m_editUserNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.UserNo, atemp);
		//结算日期
		wstring date;
		m_cbtSettleDate.CxGetWindowText(date);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(date).c_str());
		//合约编号
		memset(&atemp, 0, sizeof(atemp));
		m_editContractNo.GetText(atemp, sizeof(atemp));
		TContractKey conKey;
		memset(&conKey, 0, sizeof(TContractKey));
		CConvert::TContractNoToTContractKey(atemp, conKey);
		char sCode[101] = { 0 };
		CConvert::TContractKeyToSContractNo(conKey, sCode);
		strncpy_s(req.ContractNo, sCode);
		//成交编号
		memset(&atemp, 0, sizeof(atemp));
		m_editMatchNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.MatchNo, atemp);
		//系统编号
		memset(&atemp, 0, sizeof(atemp));
		m_editSystemNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.SystemNo, atemp);
		//委托编号
		memset(&atemp, 0, sizeof(atemp));
		m_editOrderNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.OrderNo, atemp);
		//本地编号
		memset(&atemp, 0, sizeof(atemp));
		m_editLocalNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.LocalNo, atemp);
		//买卖方向
		memset(&atemp, 0, sizeof(atemp));
		m_editDirect.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "买") == 0)
			req.Direct = SMS_DIRECT_BUY;
		else
			req.Direct = SMS_DIRECT_SELL;
		//开平
		memset(&atemp, 0, sizeof(atemp));
		m_editOffset.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "开仓") == 0)
			req.Offset = SMS_O_OPEN;
		else
			req.Offset = SMS_O_COVER;
		//投保类型
		memset(&atemp, 0, sizeof(atemp));
		m_editHedge.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "投机") == 0)
			req.Hedge = SMS_HEDGE_SPECULATE;
		else
			req.Hedge = SMS_HEDGE_HEDGE;
		//成交时间
		wstring matchtime;
		m_cbtMatchTime.CxGetWindowText(matchtime);
		strncpy_s(req.MatchTime, LoadRC::unicode_to_ansi(matchtime).c_str());
		//成交价格
		req.MatchPrice = m_editMatchPrice.GetDoubleData();
		//成交数量
		req.MatchQty = (int)m_editMatchQty.GetDoubleData();
		//持仓数量
		req.MatchFee = m_editMatchFee.GetDoubleData();
		//操作员
		strncpy_s(req.OperatorNo, m_subSystem->m_curLogin.UserNo);

		g_pSpotSystem->AddMatch(&req);
		OnBtnCancel();
	}
	else
	{
		SmsMatchModReq req;
		memset(&req, 0, sizeof(SmsMatchModReq));

		char		atemp[MAX_PATH] = { 0 };
		wchar_t		wtemp[MAX_PATH] = { 0 };

		//序列号
		req.SerialId = 0;
		//公司编号
		memset(&atemp, 0, sizeof(atemp));
		m_editCompanyNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.CompanyNo, atemp);
		//交易员编号
		memset(&atemp, 0, sizeof(atemp));
		m_editUserNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.UserNo, atemp);
		//结算日期
		wstring date;
		m_cbtSettleDate.CxGetWindowText(date);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(date).c_str());
		//合约编号
		memset(&atemp, 0, sizeof(atemp));
		m_editContractNo.GetText(atemp, sizeof(atemp));
		TContractKey conKey;
		memset(&conKey, 0, sizeof(TContractKey));
		CConvert::TContractNoToTContractKey(atemp, conKey);
		char sCode[101] = { 0 };
		CConvert::TContractKeyToSContractNo(conKey, sCode);
		strncpy_s(req.ContractNo, sCode);
		//成交编号
		memset(&atemp, 0, sizeof(atemp));
		m_editMatchNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.MatchNo, atemp);
		//系统编号
		memset(&atemp, 0, sizeof(atemp));
		m_editSystemNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.SystemNo, atemp);
		//委托编号
		memset(&atemp, 0, sizeof(atemp));
		m_editOrderNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.OrderNo, atemp);
		//本地编号
		memset(&atemp, 0, sizeof(atemp));
		m_editLocalNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.LocalNo, atemp);
		//买卖方向
		memset(&atemp, 0, sizeof(atemp));
		m_editDirect.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "买") == 0)
			req.Direct = SMS_DIRECT_BUY;
		else
			req.Direct = SMS_DIRECT_SELL;
		//开平
		memset(&atemp, 0, sizeof(atemp));
		m_editOffset.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "开仓") == 0)
			req.Offset = SMS_O_OPEN;
		else
			req.Offset = SMS_O_COVER;
		//投保类型
		memset(&atemp, 0, sizeof(atemp));
		m_editHedge.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "投机") == 0)
			req.Hedge = SMS_HEDGE_SPECULATE;
		else
			req.Hedge = SMS_HEDGE_HEDGE;
		//成交时间
		wstring matchtime;
		m_cbtMatchTime.CxGetWindowText(matchtime);
		strncpy_s(req.MatchTime, LoadRC::unicode_to_ansi(matchtime).c_str());
		//成交价格
		req.MatchPrice = m_editMatchPrice.GetDoubleData();
		//成交数量
		req.MatchQty = (int)m_editMatchQty.GetDoubleData();
		//持仓数量
		req.MatchFee = m_editMatchFee.GetDoubleData();
		//操作员
		strncpy_s(req.OperatorNo, m_subSystem->m_curLogin.UserNo);

		g_pSpotSystem->ModifyMatch(&req);
		OnBtnCancel();
	}

}

void AddMatchWnd::OnBtnCancel()
{
	if (IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_HIDE);
	else
		ShowWindow(m_Hwnd, SW_SHOW);
}

void AddMatchWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_btnContractNo:
		OnBtnCode();
		break;
	case ID_btnOK:
		OnBtnOk();
		break;
	case ID_btnCancel:
		OnBtnCancel();
		break;
	default:
		break;
	}

}

void AddMatchWnd::OnBtnCode()
{
	POINT pt = { 0, 0 };
	ClientToScreen(m_editContractNo.GetHwnd(), &pt);
	pt.y += 25;

	if (!IsWindow(m_popCode.GetHwnd()))
		m_popCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0, pt.x, pt.y);

	if (!m_subSystem)return;
	m_popCode.UpdateData(m_subSystem->m_curLogin.sSign);

	SetWindowPos(m_popCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

void AddMatchWnd::OnLDoubleClick(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_editDirect://双击切换
	{
		char	temp[21] = { 0 };
		m_editDirect.GetText(temp, sizeof(temp));
		if (strcmp(temp, "买") == 0)
			m_editDirect.SetText("卖");
		else
			m_editDirect.SetText("买");
		break;
	}
	case ID_editOffset://双击切换
	{
		char	temp[21] = { 0 };
		m_editOffset.GetText(temp, sizeof(temp));
		if (strcmp(temp, "开仓") == 0)
			m_editOffset.SetText("平仓");
		else
			m_editOffset.SetText("开仓");
		break;
	}
	case ID_editHedge://双击切换
	{
		char	temp[21] = { 0 };
		m_editHedge.GetText(temp, sizeof(temp));
		if (strcmp(temp, "投机") == 0)
			m_editHedge.SetText("套保");
		else
			m_editHedge.SetText("投机");
		break;
	}
	default:
		break;
	}
}
