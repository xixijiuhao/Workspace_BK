#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_V    405
#define cFRAME_H    310
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
AddAgreementWnd::AddAgreementWnd()
	:m_title("录入合同"),
	m_insertTime("")
{
	m_bMouseTrack = false;
}

//AddAgreementWnd::~AddAgreementWnd()
//{
//	
//}

void AddAgreementWnd::ShowFrm(HWND hParent)
{
	CreateFrm(L"TAddAgreementWnd", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_H, cFRAME_V, SWP_SHOWWINDOW);
}

void AddAgreementWnd::Register(SystemFrame * pdata)
{
	if (!pdata)return;
	m_subSystem = pdata;
}

void AddAgreementWnd::InitWnd()
{
	m_title = "录入合同";
	EnableWindow(m_labAgreementNo.GetHwnd(), TRUE);
	EnableWindow(m_editAgreementNo.GetHwnd(), TRUE);
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);

	if (!m_subSystem)return;

	m_editAgreementNo.SetText(L"");
	m_cbAgreementType.SetSelect(0);
	//m_cbPositionOffset.SetSelect(0);
	m_spineditSpotQty.SetWindowNumber(0);

	if (!m_subSystem)return;
	m_cbUserNo.Clear();
	m_cbUserNo.AddString(LoadRC::ansi_to_unicode(m_subSystem->m_curLogin.UserNo).c_str());
	m_cbUserNo.SetSelect(0);

	m_cbSpecificationNo.Clear();
	for (auto it : m_subSystem->m_mapSpec)
	{
		m_cbSpecificationNo.AddString(LoadRC::ansi_to_unicode(it.second.SpecificationNo).c_str());
	}
	m_cbSpecificationNo.SetSelect(0);
}

void AddAgreementWnd::InitModifyWnd(SmsAgreementDataRsp & agree)
{
	m_title = "修改合同";
	EnableWindow(m_labAgreementNo.GetHwnd(), FALSE);
	EnableWindow(m_editAgreementNo.GetHwnd(), FALSE);
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);

	if (!m_subSystem)return;
	m_cbUserNo.Clear();
	m_cbUserNo.AddString(LoadRC::ansi_to_unicode(m_subSystem->m_curLogin.UserNo).c_str());
	m_cbUserNo.SetSelect(0);

	m_cbSpecificationNo.Clear();
	for (auto it : m_subSystem->m_mapSpec)
	{
		m_cbSpecificationNo.AddString(LoadRC::ansi_to_unicode(it.second.SpecificationNo).c_str());
	}
	m_cbSpecificationNo.SetSelect(0);

	m_editAgreementNo.SetText(agree.AgreementNo);
	m_cbUserNo.SetSelect(LoadRC::ansi_to_unicode(agree.UserNo).c_str());
	m_editSpotPrice.SetDoubleData(agree.SpotPrice, 2);
	m_spineditSpotQty.SetWindowNumber(agree.SpotQty);

	vector<string> vdate;
	vdate = m_subSystem->SplitString(agree.SignDate, '-');
	if (vdate.size() < 3)
		return;
	SYSTEMTIME time;
	memset(&time, 0, sizeof(SYSTEMTIME));
	time.wYear = atoi(vdate.at(0).c_str());
	time.wMonth = atoi(vdate.at(1).c_str());
	time.wDay = atoi(vdate.at(2).c_str());
	DateTime_SetSystemtime(m_cbtSignDate.GetHwnd(), GDT_VALID, &time);

	if (agree.AgreementType == SMS_AGREEMENT_PURCHASE)
		m_cbAgreementType.SetSelect(0);
	else
		m_cbAgreementType.SetSelect(1);

	m_cbSpecificationNo.SetSelect(LoadRC::ansi_to_unicode(agree.SpecificationNo).c_str());

	//if (agree.PositionOffset == SMS_O_OPEN)
	//	m_cbPositionOffset.SetSelect(0);
	//else
	//	m_cbPositionOffset.SetSelect(1);

	m_insertTime = agree.InsertTime;
}


LRESULT AddAgreementWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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



void AddAgreementWnd::OnCreate()
{
	InitCommonControls();
	CreateWidget();
	InitWidget();
	SetWidgetPos();
}

void AddAgreementWnd::OnPaint()
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

void AddAgreementWnd::OnSize(WPARAM wParam, LPARAM lParam)
{

}

void AddAgreementWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

void AddAgreementWnd::OnMouseLeave()
{
	m_bMouseTrack = false;

}

void AddAgreementWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddAgreementWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddAgreementWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
}

void AddAgreementWnd::KeyDown(WPARAM wParam, LPARAM lParam)
{
}

void AddAgreementWnd::OnNcDestroy()
{
	//delete this;
}

void AddAgreementWnd::CreateWidget()
{
	//合同编号
	m_labAgreementNo.Create(m_Hwnd, L"合同编号", DT_CENTER, ID_labAgreementNo);
	m_editAgreementNo.Create(m_Hwnd, 0, 0, ID_editAgreementNo);
	//用户编号
	m_labUserNo.Create(m_Hwnd, L"用户编号", DT_CENTER, ID_labUserNo);
	m_cbUserNo.Create(m_Hwnd, ID_editUserNo);
	//合同类型
	m_labAgreementType.Create(m_Hwnd, L"合同类型", DT_CENTER, ID_labAgreementType);
	m_cbAgreementType.Create(m_Hwnd, ID_cbAgreementType);
	//现货价格
	m_labSpotPrice.Create(m_Hwnd, L"现货价格", DT_CENTER, ID_labSpotPrice);
	m_editSpotPrice.Create(m_Hwnd, 0, 0, ID_editSpotPrice);
	//现货数量
	m_labSpotQty.Create(m_Hwnd, L"现货数量", DT_CENTER, ID_labSpotQty);
	m_spineditSpotQty.Create(m_Hwnd, 0, ID_editSpotQty, ID_spinSpotQty);
	//合同日期
	m_labSignDate.Create(m_Hwnd, L"合同日期", DT_CENTER, ID_labSignDate);
	//规格编号
	m_labSpecificationNo.Create(m_Hwnd, L"规格编号", DT_CENTER, ID_labSpecificationNo);
	m_cbSpecificationNo.Create(m_Hwnd, ID_cbSpecificationNo);
	//开平类型
	//m_labPositionOffset.Create(m_Hwnd, L"开平类型", DT_CENTER, ID_labPositionOffset);
	//m_cbPositionOffset.Create(m_Hwnd, ID_cbPositionOffset);
	//合同状态
	//m_labAgreementState.Create(m_Hwnd, L"合同状态", DT_CENTER, ID_labAgreementState);
	//m_cbAgreementState.Create(m_Hwnd, ID_cbAgreementState);

	m_btnOk.Create(m_Hwnd, "确  定", ID_btnOK);
	m_btnCancel.Create(m_Hwnd, "取  消", ID_btnCancel);
}

void AddAgreementWnd::InitWidget()
{
	m_labAgreementNo.SetFont(g_FontData.GetFontWord15());
	m_labUserNo.SetFont(g_FontData.GetFontWord15());
	m_labAgreementType.SetFont(g_FontData.GetFontWord15());
	m_labSpotPrice.SetFont(g_FontData.GetFontWord15());
	m_labSpotQty.SetFont(g_FontData.GetFontWord15());
	m_labSignDate.SetFont(g_FontData.GetFontWord15());
	m_labSpecificationNo.SetFont(g_FontData.GetFontWord15());
	//m_labPositionOffset.SetFont(g_FontData.GetFontWord15());
	//m_labAgreementState.SetFont(g_FontData.GetFontWord15());


	m_editAgreementNo.SetFont(g_FontData.GetFontNum15());
	m_editSpotPrice.SetFont(g_FontData.GetFontNum15());
	m_spineditSpotQty.SetFont(g_FontData.GetFontNum15());

	m_cbUserNo.SetFont(g_FontData.GetFontNum15());

	m_cbAgreementType.SetFont(g_FontData.GetFontNum15());
	m_cbAgreementType.AddString(L"采购合同");
	m_cbAgreementType.AddString(L"销售合同");

	m_cbSpecificationNo.SetFont(g_FontData.GetFontNum15());

	//m_cbPositionOffset.SetFont(g_FontData.GetFontNum15());
	//m_cbPositionOffset.AddString(L"开仓");
	//m_cbPositionOffset.AddString(L"平仓");

	//m_cbAgreementState.SetFont(g_FontData.GetFontNum15());
	//m_cbAgreementState.AddString(L"合同有效");
	//m_cbAgreementState.AddString(L"合同结束");
}

void AddAgreementWnd::SetWidgetPos()
{
	int leftstart = 15;
	int topstart = 15;
	int editWidth = 150;	//edit控件宽度
	int labWidth = 100;		//lab控件宽度
	int btnWidth = 80;		//btn控件宽度
	int Height = 25;		//控件高度
	int htap = 10;			//横向间隔
	int vtap = 15;			//纵向间隔
	m_hInstance = GetModuleHandle(NULL);
	POINT pt;
	pt.x = leftstart; 
	pt.y = cCAPTION_H + topstart;
	//合同编号
	m_labAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//用户编号
	pt.x = leftstart; pt.y += Height + vtap;
	m_labUserNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbUserNo.MoveWindow(pt.x, pt.y, editWidth, Height*5);
	//现货价格
	pt.x = leftstart; pt.y += Height + vtap;
	m_labSpotPrice.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSpotPrice.MoveWindow(pt.x, pt.y, editWidth, Height);
	//现货数量
	pt.x = leftstart; pt.y += Height + vtap;
	m_labSpotQty.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_spineditSpotQty.MoveWindow(pt.x, pt.y, editWidth, Height);
	//签名日期
	pt.x = leftstart; pt.y += Height + vtap;
	m_labSignDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;

	m_timepick =  CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		pt.x, pt.y, editWidth, Height, m_Hwnd, (HMENU)55, m_hInstance, NULL);
	SendMessage(m_timepick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	m_cbtSignDate.Init(m_timepick);

	DateTime_SetFormat(m_timepick, L"yyyy-MM-dd");
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSignDate.GetHwnd(), GDT_VALID, &time);
	
	//合同类型
	pt.x = leftstart; pt.y += Height + vtap;
	m_labAgreementType.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbAgreementType.MoveWindow(pt.x, pt.y, editWidth, Height * 5);
	//规格编号
	pt.x = leftstart; pt.y += Height + vtap;
	m_labSpecificationNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbSpecificationNo.MoveWindow(pt.x, pt.y, editWidth, Height* 5);
	//开平仓类型
	//pt.x = leftstart; pt.y += Height + vtap;
	//m_labPositionOffset.MoveWindow(pt.x, pt.y, labWidth, Height);
	//pt.x += labWidth + htap;
	//m_cbPositionOffset.MoveWindow(pt.x, pt.y, editWidth, Height*5);
	//合同状态
	//pt.x = leftstart; pt.y += Height + vtap;
	//m_labAgreementState.MoveWindow(pt.x, pt.y, labWidth, Height);
	//pt.x += labWidth + htap;
	//m_cbAgreementState.MoveWindow(pt.x, pt.y, editWidth, Height*5);
	//ok cancel

	pt.x = cFRAME_H/2 - 40 - btnWidth; 
	pt.y += Height + vtap;
	m_btnOk.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + 80;
	m_btnCancel.MoveWindow(pt.x, pt.y, btnWidth, Height);
}

LRESULT AddAgreementWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetHbrWhite();
}

void AddAgreementWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

void AddAgreementWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
}

void AddAgreementWnd::OnBtnOk()
{
	if (!m_subSystem)return;
	if (IsWindowEnabled(m_editAgreementNo.GetHwnd()))
	{
		SmsAgreementAddReq addReq;
		memset(&addReq, 0, sizeof(SmsAgreementAddReq));
		char temp[MAX_PATH] = { 0 };
		//公司
		strcpy_s(addReq.CompanyNo, m_subSystem->m_curLogin.CompanyNo);
		//合同
		m_editAgreementNo.GetText(temp, sizeof(temp));
		strcpy_s(addReq.AgreementNo, temp);
		//用户
		wchar_t userno[MAX_PATH] = { 0 };
		m_cbUserNo.GetText(userno);
		strcpy_s(addReq.UserNo, LoadRC::unicode_to_ansi(userno).c_str());
		//合同类型
		if (m_cbAgreementType.GetSelect() == 0)
		{
			addReq.AgreementType = SMS_AGREEMENT_PURCHASE;
		}
		else
		{
			addReq.AgreementType = SMS_AGREEMENT_SALE;
		}
		//现货价格
		addReq.SpotPrice = m_editSpotPrice.GetDoubleData();
		//现货数量
		addReq.SpotQty = m_spineditSpotQty.GetWindowNumber();
		//合同日期
		wstring tempDate;
		m_cbtSignDate.CxGetWindowText(tempDate);
		strcpy_s(addReq.SignDate, LoadRC::unicode_to_ansi(tempDate).c_str());
		//规格编号
		wchar_t spec[MAX_PATH] = { 0 };
		m_cbSpecificationNo.GetText(spec);
		strcpy_s(addReq.SpecificationNo, LoadRC::unicode_to_ansi(spec).c_str());
		//开平仓
		//if (m_cbPositionOffset.GetSelect() == 0)
		//{
		//	addReq.PositionOffset = SMS_O_OPEN;
		//}
		//else
		//{
		//	addReq.PositionOffset = SMS_O_COVER;
		//}
		//合同状态
		addReq.AgreementState = SMS_AGREEMENT_ACTIVE;
		//操作员
		strcpy_s(addReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

		g_pSpotSystem->AddAgreement(&addReq);
		OnBtnCancel();
	}
	else
	{
		SmsAgreementModReq modReq;
		memset(&modReq, 0, sizeof(SmsAgreementAddReq));
		char temp[MAX_PATH] = { 0 };
		//公司
		strcpy_s(modReq.CompanyNo, m_subSystem->m_curLogin.CompanyNo);
		//合同
		m_editAgreementNo.GetText(temp, sizeof(temp));
		strcpy_s(modReq.AgreementNo, temp);
		//用户
		wchar_t userno[MAX_PATH] = { 0 };
		m_cbUserNo.GetText(userno);
		strcpy_s(modReq.UserNo, LoadRC::unicode_to_ansi(userno).c_str());
		//合同类型
		if (m_cbAgreementType.GetSelect() == 0)
		{
			modReq.AgreementType = SMS_AGREEMENT_PURCHASE;
		}
		else
		{
			modReq.AgreementType = SMS_AGREEMENT_SALE;
		}
		//现货价格
		modReq.SpotPrice = m_editSpotPrice.GetDoubleData();
		//现货数量
		modReq.SpotQty = m_spineditSpotQty.GetWindowNumber();
		//合同日期
		wstring tempDate;
		m_cbtSignDate.CxGetWindowText(tempDate);
		strcpy_s(modReq.SignDate, LoadRC::unicode_to_ansi(tempDate).c_str());
		//录入时间
		strcpy_s(modReq.InsertTime, m_insertTime.c_str());
		//规格编号
		wchar_t spec[MAX_PATH] = { 0 };
		m_cbSpecificationNo.GetText(spec);
		strcpy_s(modReq.SpecificationNo, LoadRC::unicode_to_ansi(spec).c_str());
		//开平仓
		//if (m_cbPositionOffset.GetSelect() == 0)
		//{
		//	modReq.PositionOffset = SMS_O_OPEN;
		//}
		//else
		//{
		//	modReq.PositionOffset = SMS_O_COVER;
		//}
		//合同状态
		modReq.AgreementState = SMS_AGREEMENT_ACTIVE;
		//操作员
		strcpy_s(modReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

		g_pSpotSystem->ModifyAgreement(&modReq);
		OnBtnCancel();
	}

}

void AddAgreementWnd::OnBtnCancel()
{
	if (IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_HIDE);
	else
		ShowWindow(m_Hwnd, SW_SHOW);
}

void AddAgreementWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
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
