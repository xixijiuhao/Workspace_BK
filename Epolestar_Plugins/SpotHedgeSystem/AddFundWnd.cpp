#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_H    610
#define cFRAME_V    360

extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
AddFundWnd::AddFundWnd()
	:m_title("¼���ʽ�"),
	m_serialId(0)
{
	m_bMouseTrack = false;
}

//AddFundWnd::~AddFundWnd()
//{
//	
//}

void AddFundWnd::ShowFrm(HWND hParent)
{
	CreateFrm(L"TAddFundWnd", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_H, cFRAME_V, SWP_SHOWWINDOW);
}

void AddFundWnd::Register(SystemFrame * pdata)
{
	if (!pdata)return;
	m_subSystem = pdata;
}

void AddFundWnd::InitWnd()
{
	m_title = "¼���ʽ�";
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);

	EnableWindow(m_labCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_editCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_labUserNo.GetHwnd(), FALSE);
	EnableWindow(m_editUserNo.GetHwnd(), FALSE);
	EnableWindow(m_labSettleDate.GetHwnd(), TRUE);
	EnableWindow(m_cbtSettleDate.GetHwnd(), TRUE);

	if (!m_subSystem)return;
	//����
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);
	//��˾
	m_editCompanyNo.SetText(m_subSystem->m_curLogin.CompanyNo);
	//�˺�
	m_editUserNo.SetText(m_subSystem->m_curLogin.UserNo);
	
	m_editFee.SetText("");
	m_editEquity.SetText("");
	m_editPreEquity.SetText("");
	m_editAvailable.SetText("");
	m_editPreAvailable.SetText("");
	m_editMargin.SetText("");
	m_editPositionProfit.SetText("");
	m_editCloseProfit.SetText("");
	m_editDeliveryProfit.SetText("");
	m_editCashIn.SetText("");
	m_editCashOut.SetText("");
}

void AddFundWnd::InitModifyWnd(SmsFundDataRsp & fund)
{
	m_title = "�޸��ʽ�";
	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);

	EnableWindow(m_labSettleDate.GetHwnd(), FALSE);
	EnableWindow(m_cbtSettleDate.GetHwnd(), FALSE);
	EnableWindow(m_labCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_editCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_labUserNo.GetHwnd(), FALSE);
	EnableWindow(m_editUserNo.GetHwnd(), FALSE);

	if (!m_subSystem)return;

	m_serialId = fund.SerialId;
	//��������
	vector<string> vdate;
	vdate = m_subSystem->SplitString(fund.SettleDate, '-');
	if (vdate.size() < 3)
		return;
	SYSTEMTIME time;
	memset(&time, 0, sizeof(SYSTEMTIME));
	time.wYear = atoi(vdate.at(0).c_str());
	time.wMonth = atoi(vdate.at(1).c_str());
	time.wDay = atoi(vdate.at(2).c_str());
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);
	
	m_editCompanyNo.SetText(fund.CompanyNo);
	m_editUserNo.SetText(fund.UserNo);
	m_editFee.SetDoubleData(fund.Fee, 2);
	m_editEquity.SetDoubleData(fund.Equity, 2);
	m_editPreEquity.SetDoubleData(fund.PreEquity, 2);
	m_editAvailable.SetDoubleData(fund.Available, 2);
	m_editPreAvailable.SetDoubleData(fund.PreAvailable, 2);
	m_editMargin.SetDoubleData(fund.Margin, 2);
	m_editPositionProfit.SetDoubleData(fund.PositionProfit, 2);
	m_editCloseProfit.SetDoubleData(fund.CloseProfit, 2);
	m_editDeliveryProfit.SetDoubleData(fund.DeliveryProfit, 2);
	m_editCashIn.SetDoubleData(fund.CashIn, 2);
	m_editCashOut.SetDoubleData(fund.CashOut, 2);

}

LRESULT AddFundWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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



void AddFundWnd::OnCreate()
{
	InitCommonControls();
	CreateWidget();
	InitWidget();
	SetWidgetPos();
}

void AddFundWnd::OnPaint()
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

void AddFundWnd::OnSize(WPARAM wParam, LPARAM lParam)
{

}

void AddFundWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

void AddFundWnd::OnMouseLeave()
{
	m_bMouseTrack = false;

}

void AddFundWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddFundWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddFundWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
}

void AddFundWnd::KeyDown(WPARAM wParam, LPARAM lParam)
{
}

void AddFundWnd::OnNcDestroy()
{
	//delete this;
}

void AddFundWnd::CreateWidget()
{
	//��������
	m_labSettleDate.Create(m_Hwnd, L"��������", DT_LEFT, ID_labSettleDAate);
	m_settleTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_cbtSettleDAate, m_hInstance, NULL);
	//��˾
	m_labCompanyNo.Create(m_Hwnd, L"��˾��", DT_LEFT, ID_labCompanyNo);
	m_editCompanyNo.Create(m_Hwnd, 0, 0, ID_editCompanyNo);
	//�˺�
	m_labUserNo.Create(m_Hwnd, L"�˺�", DT_LEFT, ID_labUserNo);
	m_editUserNo.Create(m_Hwnd, 0, 0, ID_editUserNo);
	//������
	m_labFee.Create(m_Hwnd, L"������", DT_LEFT, ID_labFee);
	m_editFee.Create(m_Hwnd, 0, 0, ID_editFee);
	//Ȩ��
	m_labEquity.Create(m_Hwnd, L"Ȩ��", DT_LEFT, ID_labEquity);
	m_editEquity.Create(m_Hwnd, 0, 0, ID_editEquity);
	//��Ȩ��
	m_labPreEquity.Create(m_Hwnd, L"��Ȩ��", DT_LEFT, ID_labPreEquity);
	m_editPreEquity.Create(m_Hwnd, 0, 0, ID_editPreEquity);
	//����
	m_labAvailable.Create(m_Hwnd, L"����", DT_LEFT, ID_labAvailable);
	m_editAvailable.Create(m_Hwnd, 0, 0, ID_editAvailable);
	//�����
	m_labPreAvailable.Create(m_Hwnd, L"�����", DT_LEFT, ID_labPreAvailable);
	m_editPreAvailable.Create(m_Hwnd, 0, 0, ID_editPreAvailable);
	//��֤��
	m_labMargin.Create(m_Hwnd, L"��֤��", DT_LEFT, ID_labMargin);
	m_editMargin.Create(m_Hwnd, 0, 0, ID_editMargin);
	//�ֲ�����
	m_labPositionProfit.Create(m_Hwnd, L"�ֲ�����", DT_LEFT, ID_labPositionProfit);
	m_editPositionProfit.Create(m_Hwnd, 0, 0, ID_editPositionProfit);
	//ƽ������
	m_labCloseProfit.Create(m_Hwnd, L"ƽ������", DT_LEFT, ID_labCloseProfit);
	m_editCloseProfit.Create(m_Hwnd, 0, 0, ID_editCloseProfit);
	//��������
	m_labDeliveryProfit.Create(m_Hwnd, L"��������", DT_LEFT, ID_labDeliveryProfit);
	m_editDeliveryProfit.Create(m_Hwnd, 0, 0, ID_editDeliveryProfit);
	//���
	m_labCashIn.Create(m_Hwnd, L"���", DT_LEFT, ID_labCashIn);
	m_editCashIn.Create(m_Hwnd, 0, 0, ID_editCashIn);
	//����
	m_labCashOut.Create(m_Hwnd, L"����", DT_LEFT, ID_labCashOut);
	m_editCashOut.Create(m_Hwnd, 0, 0, ID_editCashOut);

	m_btnOk.Create(m_Hwnd, "ȷ  ��", ID_btnOK);
	m_btnCancel.Create(m_Hwnd, "ȡ  ��", ID_btnCancel);
}

void AddFundWnd::InitWidget()
{
	m_labSettleDate.SetFont(g_FontData.GetFontWord15());
	m_labCompanyNo.SetFont(g_FontData.GetFontWord15());
	m_labUserNo.SetFont(g_FontData.GetFontWord15());
	m_labFee.SetFont(g_FontData.GetFontWord15());
	m_labEquity.SetFont(g_FontData.GetFontWord15());
	m_labPreEquity.SetFont(g_FontData.GetFontWord15());
	m_labAvailable.SetFont(g_FontData.GetFontWord15());
	m_labPreAvailable.SetFont(g_FontData.GetFontWord15());
	m_labMargin.SetFont(g_FontData.GetFontWord15());
	m_labPositionProfit.SetFont(g_FontData.GetFontWord15());
	m_labCloseProfit.SetFont(g_FontData.GetFontWord15());
	m_labDeliveryProfit.SetFont(g_FontData.GetFontWord15());
	m_labCashIn.SetFont(g_FontData.GetFontWord15());
	m_labCashOut.SetFont(g_FontData.GetFontWord15());

	m_cbtSettleDate.Init(m_settleTimePick);
	SendMessage(m_settleTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_settleTimePick, L"yyyy-MM-dd");
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);

	m_editCompanyNo.SetFont(g_FontData.GetFontNum15());
	m_editUserNo.SetFont(g_FontData.GetFontNum15());
	m_editFee.SetFont(g_FontData.GetFontNum15());
	m_editEquity.SetFont(g_FontData.GetFontNum15());
	m_editPreEquity.SetFont(g_FontData.GetFontNum15());
	m_editAvailable.SetFont(g_FontData.GetFontNum15());
	m_editPreAvailable.SetFont(g_FontData.GetFontNum15());
	m_editMargin.SetFont(g_FontData.GetFontNum15());
	m_editPositionProfit.SetFont(g_FontData.GetFontNum15());
	m_editCloseProfit.SetFont(g_FontData.GetFontNum15());
	m_editDeliveryProfit.SetFont(g_FontData.GetFontNum15());
	m_editCashIn.SetFont(g_FontData.GetFontNum15());
	m_editCashOut.SetFont(g_FontData.GetFontNum15());


}

void AddFundWnd::SetWidgetPos()
{
	int leftstart = 30;
	int topstart = 15;
	int editWidth = 150;	//edit�ؼ����
	int labWidth = 80;		//lab�ؼ����
	int btnWidth = 80;		//btn�ؼ����
	int Height = 25;		//�ؼ��߶�
	int htap = 30;			//������
	int vtap = 15;			//������
	m_hInstance = GetModuleHandle(NULL);
	POINT pt;
	pt.x = leftstart;
	pt.y = cCAPTION_H + topstart;
	//��˾��
	m_labCompanyNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCompanyNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�˺�
	pt.x += editWidth + htap;
	m_labUserNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editUserNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��������
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labSettleDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtSettleDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	//������
	pt.x += editWidth + htap;
	m_labFee.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editFee.MoveWindow(pt.x, pt.y, editWidth, Height);
	//Ȩ��
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labEquity.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editEquity.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��Ȩ��
	pt.x += editWidth + htap;
	m_labPreEquity.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editPreEquity.MoveWindow(pt.x, pt.y, editWidth, Height);
	//����
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labAvailable.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editAvailable.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�����
	pt.x += editWidth + htap;
	m_labPreAvailable.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editPreAvailable.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��֤��
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labMargin.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editMargin.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�ֲ�����
	pt.x += editWidth + htap;
	m_labPositionProfit.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editPositionProfit.MoveWindow(pt.x, pt.y, editWidth, Height);
	//ƽ������
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labCloseProfit.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCloseProfit.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��������
	pt.x += editWidth + htap;
	m_labDeliveryProfit.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editDeliveryProfit.MoveWindow(pt.x, pt.y, editWidth, Height);
	//���
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labCashIn.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCashIn.MoveWindow(pt.x, pt.y, editWidth, Height);
	//����
	pt.x += editWidth + htap;
	m_labCashOut.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCashOut.MoveWindow(pt.x, pt.y, editWidth, Height);

	//ȷ��
	//ȡ��
	pt.x = cFRAME_H / 2 - 60 - btnWidth;
	pt.y += Height + vtap;
	m_btnOk.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + 120;
	m_btnCancel.MoveWindow(pt.x, pt.y, btnWidth, Height);
}

LRESULT AddFundWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetHbrWhite();
}

void AddFundWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

void AddFundWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
}

void AddFundWnd::OnBtnOk()
{
	if (!m_subSystem)return;
	if (IsWindowEnabled(m_cbtSettleDate.GetHwnd()))
	{
		SmsFundAddReq addReq;
		memset(&addReq, 0, sizeof(SmsFundAddReq));
		char temp[MAX_PATH] = { 0 };
		//���к�
		addReq.SerialId = 0;
		//��������
		wstring tempDate;
		m_cbtSettleDate.CxGetWindowText(tempDate);
		strcpy_s(addReq.SettleDate, LoadRC::unicode_to_ansi(tempDate).c_str());
		//��˾���
		memset(&temp, 0, sizeof(temp));
		m_editCompanyNo.GetText(temp, sizeof(temp));
		strcpy_s(addReq.CompanyNo, temp);
		//����Ա���
		memset(&temp, 0, sizeof(temp));
		m_editUserNo.GetText(temp, sizeof(temp));
		strcpy_s(addReq.UserNo, temp);
		//������
		addReq.Fee = m_editFee.GetDoubleData();
		//Ȩ��
		addReq.Equity = m_editEquity.GetDoubleData();
		//��Ȩ��
		addReq.PreEquity = m_editPreEquity.GetDoubleData();
		//����
		addReq.Available = m_editAvailable.GetDoubleData();
		//�����
		addReq.PreAvailable = m_editPreAvailable.GetDoubleData();
		//��֤��
		addReq.Margin = m_editMargin.GetDoubleData();
		//�ֲ�����
		addReq.PositionProfit = m_editPositionProfit.GetDoubleData();
		//ƽ������
		addReq.CloseProfit = m_editCloseProfit.GetDoubleData();
		//��������
		addReq.DeliveryProfit = m_editDeliveryProfit.GetDoubleData();
		//���
		addReq.CashIn = m_editCashIn.GetDoubleData();
		//����
		addReq.CashOut = m_editCashOut.GetDoubleData();
		//����Ա
		strcpy_s(addReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

		g_pSpotSystem->AddFund(&addReq);
		OnBtnCancel();
	}
	else
	{
		SmsFundModReq modReq;
		memset(&modReq, 0, sizeof(SmsFundModReq));
		char temp[MAX_PATH] = { 0 };
		//��������
		wstring tempDate;
		m_cbtSettleDate.CxGetWindowText(tempDate);
		strcpy_s(modReq.SettleDate, LoadRC::unicode_to_ansi(tempDate).c_str());
		//��˾���
		memset(&temp, 0, sizeof(temp));
		m_editCompanyNo.GetText(temp, sizeof(temp));
		strcpy_s(modReq.CompanyNo, temp);
		//����Ա���
		memset(&temp, 0, sizeof(temp));
		m_editUserNo.GetText(temp, sizeof(temp));
		strcpy_s(modReq.UserNo, temp);
		//������
		modReq.Fee = m_editFee.GetDoubleData();
		//Ȩ��
		modReq.Equity = m_editEquity.GetDoubleData();
		//��Ȩ��
		modReq.PreEquity = m_editPreEquity.GetDoubleData();
		//����
		modReq.Available = m_editAvailable.GetDoubleData();
		//�����
		modReq.PreAvailable = m_editPreAvailable.GetDoubleData();
		//��֤��
		modReq.Margin = m_editMargin.GetDoubleData();
		//�ֲ�����
		modReq.PositionProfit = m_editPositionProfit.GetDoubleData();
		//ƽ������
		modReq.CloseProfit = m_editCloseProfit.GetDoubleData();
		//��������
		modReq.DeliveryProfit = m_editDeliveryProfit.GetDoubleData();
		//���
		modReq.CashIn = m_editCashIn.GetDoubleData();
		//����
		modReq.CashOut = m_editCashOut.GetDoubleData();
		//����Ա
		strcpy_s(modReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

		g_pSpotSystem->ModifyFund(&modReq);
		OnBtnCancel();
	}
}

void AddFundWnd::OnBtnCancel()
{
	if (IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_HIDE);
	else
		ShowWindow(m_Hwnd, SW_SHOW);
}

void AddFundWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
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
