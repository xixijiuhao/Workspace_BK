#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_H    610
#define cFRAME_V    325

extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
AddPositionWnd::AddPositionWnd()
	:m_title("¼��ֲ�"),
	m_serialId(0)
{
	m_bMouseTrack = false;
}

//AddPositionWnd::~AddPositionWnd()
//{
//	
//}

void AddPositionWnd::ShowFrm(HWND hParent)
{
	CreateFrm(L"TAddPositionWnd", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_H, cFRAME_V, SWP_SHOWWINDOW);
}

void AddPositionWnd::Register(SystemFrame * pdata)
{
	if (!pdata)return;
	m_subSystem = pdata;
}

void AddPositionWnd::InitWnd()
{
	m_title = "¼��ֲ�";
	EnableWindow(m_labCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_editCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_labUserNo.GetHwnd(), FALSE);
	EnableWindow(m_editUserNo.GetHwnd(), FALSE);

	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);

	if (!m_subSystem)return;

	//���º�ͬ
	m_cbAgreementNo.Clear();
	string key = m_subSystem->get_curkey();
	m_cbAgreementNo.AddString(L"");
	for (auto it : m_subSystem->m_map_mapAgreement[key])
	{
		m_cbAgreementNo.AddString(LoadRC::ansi_to_unicode(it.second.AgreementNo).c_str());
	}

	//������
	m_editCompanyNo.SetText(m_subSystem->m_curLogin.CompanyNo);
	m_editUserNo.SetText(m_subSystem->m_curLogin.UserNo);
	m_cbAgreementNo.SetSelect(0);
	m_editContractNo.SetText("");
	m_editPositionNo.SetText("");
	m_editPositionQty.SetText("");
	
	//m_editOffset.SetText("����");
	//m_editHedge.SetText("Ͷ��");

	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);


	m_editPositionPrice.SetText("");
	m_editPositionProfit.SetText("");

}

void AddPositionWnd::InitModifyWnd(SmsPositionDataRsp & position)
{
	m_title = "�޸ĳֲ�";
	EnableWindow(m_labCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_editCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_labUserNo.GetHwnd(), FALSE);
	EnableWindow(m_editUserNo.GetHwnd(), FALSE);

	InvalidateRect(m_Hwnd, NULL, TRUE);
	UpdateWindow(m_Hwnd);

	//���º�ͬ
	m_cbAgreementNo.Clear();
	string key = m_subSystem->get_curkey();
	m_cbAgreementNo.AddString(L"");
	for (auto it : m_subSystem->m_map_mapAgreement[key])
	{
		m_cbAgreementNo.AddString(LoadRC::ansi_to_unicode(it.second.AgreementNo).c_str());
	}

	m_serialId = position.SerialId;

	m_editCompanyNo.SetText(position.CompanyNo);
	m_editUserNo.SetText(position.UserNo);

	//bool bfound = m_cbAgreementNo.SetSelect(LoadRC::ansi_to_unicode(position.AgreementNo).c_str());
	//if (!bfound)
	//	m_cbAgreementNo.SetSelect(0);

	string sCode = position.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	m_editContractNo.SetText(tCode.c_str());

	m_editPositionNo.SetText(position.PositionNo);
	m_editPositionQty.SetDoubleData(position.PositionQty);

	if (position.Direct == SMS_DIRECT_BUY)
		m_editDirect.SetText("��");
	else
		m_editDirect.SetText("��");
	//if (position.Offset == SMS_O_OPEN)
	//	m_editOffset.SetText("����");
	//else
	//	m_editOffset.SetText("ƽ��");
	if (position.Hedge == SMS_HEDGE_SPECULATE)
		m_editHedge.SetText("Ͷ��");
	else
		m_editHedge.SetText("�ױ�");

	vector<string> vdate;
	vdate = m_subSystem->SplitString(position.SettleDate, '-');
	if (vdate.size() < 3)
		return;
	SYSTEMTIME time;
	memset(&time, 0, sizeof(SYSTEMTIME));
	time.wYear = atoi(vdate.at(0).c_str());
	time.wMonth = atoi(vdate.at(1).c_str());
	time.wDay = atoi(vdate.at(2).c_str());
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);

	m_editPositionPrice.SetDoubleData(position.PositionPrice, 2);
	m_editPositionProfit.SetDoubleData(position.PositionProfit, 2);

}

LRESULT AddPositionWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void AddPositionWnd::SetContractNo(string sCode)
{
	m_editContractNo.SetText(sCode.c_str());
}

void AddPositionWnd::OnCreate()
{
	InitCommonControls();
	CreateWidget();
	InitWidget();
	SetWidgetPos();
}

void AddPositionWnd::OnPaint()
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

void AddPositionWnd::OnSize(WPARAM wParam, LPARAM lParam)
{

}

void AddPositionWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

void AddPositionWnd::OnMouseLeave()
{
	m_bMouseTrack = false;

}

void AddPositionWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddPositionWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddPositionWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
}

void AddPositionWnd::KeyDown(WPARAM wParam, LPARAM lParam)
{
}

void AddPositionWnd::OnNcDestroy()
{
	//delete this;
}

void AddPositionWnd::OnLDoubleClick(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_editDirect://˫���л�
	{
		char	temp[21] = { 0 };
		m_editDirect.GetText(temp, sizeof(temp));
		if (strcmp(temp, "��") == 0)
			m_editDirect.SetText("��");
		else
			m_editDirect.SetText("��");
		break;
	}
	case ID_editOffset://˫���л�
	{
		char	temp[21] = { 0 };
		m_editOffset.GetText(temp, sizeof(temp));
		if (strcmp(temp, "����") == 0)
			m_editOffset.SetText("ƽ��");
		else
			m_editOffset.SetText("����");
		break;
	}
	case ID_editHedge://˫���л�
	{
		char	temp[21] = { 0 };
		m_editHedge.GetText(temp, sizeof(temp));
		if (strcmp(temp, "Ͷ��") == 0)
			m_editHedge.SetText("�ױ�");
		else
			m_editHedge.SetText("Ͷ��");
		break;
	}
	default:
		break;
	}
}

void AddPositionWnd::CreateWidget()
{
	//��˾
	m_labCompanyNo.Create(m_Hwnd, L"��˾", DT_LEFT, ID_labCompanyNo);
	m_editCompanyNo.Create(m_Hwnd, 0, 0, ID_editCompanyNo);
	//�˺�
	m_labUserNo.Create(m_Hwnd, L"�˺�", DT_LEFT, ID_labUserNo);
	m_editUserNo.Create(m_Hwnd, 0, 0, ID_editUserNo);
	//��ͬ���
	m_labAgreementNo.Create(m_Hwnd, L"��ͬ���", DT_LEFT, ID_labAgreementNo);
	m_cbAgreementNo.Create(m_Hwnd, ID_cbAgreementNo);
	//��Լ
	m_labContractNo.Create(m_Hwnd, L"��Լ", DT_LEFT, ID_labContractNo);
	m_editContractNo.Create(m_Hwnd, 0, 0, ID_editContractNo);
	m_btnContractNo.Create(m_Hwnd, "...", ID_btnContractNo);
	//�ֱֲ��
	m_labPositionNo.Create(m_Hwnd, L"�ֱֲ��", DT_LEFT, ID_labPositionNo);
	m_editPositionNo.Create(m_Hwnd, 0, 0, ID_editPositionNo);
	//�ֲ�����
	m_labPositionQty.Create(m_Hwnd, L"�ֲ�����", DT_LEFT, ID_labPositionQty);
	m_editPositionQty.Create(m_Hwnd, 0, 0, ID_editPositionQty);
	//����
	m_labDirect.Create(m_Hwnd, L"����", DT_LEFT, ID_labDirect);
	m_editDirect.Create(m_Hwnd, 0, ES_READONLY, ID_editDirect);
	//��ƽ
	m_labOffset.Create(m_Hwnd, L"��ƽ", DT_LEFT, ID_labOffset);
	m_editOffset.Create(m_Hwnd, 0, ES_READONLY, ID_editOffset);
	//Ͷ��
	m_labHedge.Create(m_Hwnd, L"Ͷ��", DT_LEFT, ID_labHedge);
	m_editHedge.Create(m_Hwnd, 0, ES_READONLY, ID_editHedge);
	//��������
	m_labSettleDate.Create(m_Hwnd, L"��������", DT_LEFT, ID_labSettleDAate);
	m_settleTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_cbtSettleDAate, m_hInstance, NULL);
	//�ֲּ۸�
	m_labPositionPrice.Create(m_Hwnd, L"�ֲּ۸�", DT_LEFT, ID_labPositionPrice);
	m_editPositionPrice.Create(m_Hwnd, 0, 0, ID_editPositionPrice);
	//�ֲ�����
	m_labPositionProfit.Create(m_Hwnd, L"�ֲ�����", DT_LEFT, ID_labPositionProfit);
	m_editPositionProfit.Create(m_Hwnd, 0, 0, ID_editPositionProfit);

	m_btnOk.Create(m_Hwnd, "ȷ  ��", ID_btnOK);
	m_btnCancel.Create(m_Hwnd, "ȡ  ��", ID_btnCancel);
}

void AddPositionWnd::InitWidget()
{
	m_labCompanyNo.SetFont(g_FontData.GetFontWord15());
	m_labUserNo.SetFont(g_FontData.GetFontWord15());
	m_labAgreementNo.SetFont(g_FontData.GetFontWord15());
	m_labContractNo.SetFont(g_FontData.GetFontWord15());
	m_labPositionNo.SetFont(g_FontData.GetFontWord15());
	m_labPositionQty.SetFont(g_FontData.GetFontWord15());
	m_labDirect.SetFont(g_FontData.GetFontWord15());
	m_labOffset.SetFont(g_FontData.GetFontWord15());
	m_labHedge.SetFont(g_FontData.GetFontWord15());
	m_labSettleDate.SetFont(g_FontData.GetFontWord15());
	m_labPositionPrice.SetFont(g_FontData.GetFontWord15());
	m_labPositionProfit.SetFont(g_FontData.GetFontWord15());

	m_editCompanyNo.SetFont(g_FontData.GetFontNum15());
	m_editUserNo.SetFont(g_FontData.GetFontNum15());
	m_editContractNo.SetFont(g_FontData.GetFontNum15());
	m_editPositionNo.SetFont(g_FontData.GetFontNum15());
	m_editPositionQty.SetFont(g_FontData.GetFontNum15());
	m_editDirect.SetFont(g_FontData.GetFontNum15());
	m_editOffset.SetFont(g_FontData.GetFontNum15());
	m_editHedge.SetFont(g_FontData.GetFontNum15());
	m_editPositionPrice.SetFont(g_FontData.GetFontNum15());
	m_editPositionProfit.SetFont(g_FontData.GetFontNum15());

	m_editDirect.SetText("��");
	m_editOffset.SetText("����");
	m_editHedge.SetText("�ױ�");

	m_cbtSettleDate.Init(m_settleTimePick);
	SendMessage(m_settleTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_settleTimePick, L"yyyy-MM-dd");
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);

	m_cbAgreementNo.SetFont(g_FontData.GetFontNum15());

	m_btnContractNo.SetFont(g_FontData.GetFontNum13());

	EnableWindow(m_labCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_editCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_labUserNo.GetHwnd(), FALSE);
	EnableWindow(m_editUserNo.GetHwnd(), FALSE);

}

void AddPositionWnd::SetWidgetPos()
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
	//��ͬ
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height*10);
	//��Լ
	pt.x += editWidth + htap;
	m_labContractNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editContractNo.MoveWindow(pt.x, pt.y, editWidth - 20, Height);
	pt.x += editWidth- 20 + 1;
	m_btnContractNo.MoveWindow(pt.x, pt.y, 19, Height);
	//�ֱֲ��
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labPositionNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editPositionNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�ֲ�����
	pt.x += editWidth + htap;
	m_labPositionQty.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editPositionQty.MoveWindow(pt.x, pt.y, editWidth, Height);
	//����
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labDirect.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editDirect.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��ƽ
	pt.x += editWidth + htap;
	m_labOffset.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editOffset.MoveWindow(pt.x, pt.y, editWidth, Height);
	//Ͷ��
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labHedge.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editHedge.MoveWindow(pt.x, pt.y, editWidth, Height);
	//����
	pt.x += editWidth + htap;
	m_labSettleDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtSettleDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�ֲּ۸�
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labPositionPrice.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editPositionPrice.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�ֲ�����
	pt.x += editWidth + htap;
	m_labPositionProfit.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editPositionProfit.MoveWindow(pt.x, pt.y, editWidth, Height);

	//ȷ��
	//ȡ��
	pt.x = cFRAME_H / 2 - 60 - btnWidth;
	pt.y += Height + vtap;
	m_btnOk.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + 120;
	m_btnCancel.MoveWindow(pt.x, pt.y, btnWidth, Height);
}

LRESULT AddPositionWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetHbrWhite();
}

void AddPositionWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

void AddPositionWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
}

void AddPositionWnd::OnBtnOk()
{
	if (!m_subSystem)return;

	if (strcmp(m_title.c_str(), "¼��ֲ�") == 0)
	{
		SmsPositionAddReq req;
		memset(&req, 0, sizeof(SmsPositionAddReq));

		char		atemp[MAX_PATH] = { 0 };
		wchar_t		wtemp[MAX_PATH] = { 0 };

		//���к�
		req.SerialId = 0;
		//��˾���
		memset(&atemp, 0, sizeof(atemp));
		m_editCompanyNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.CompanyNo, atemp);
		//����Ա���
		memset(&atemp, 0, sizeof(atemp));
		m_editUserNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.UserNo, atemp);
		//��ͬ���
		//memset(&wtemp, 0, sizeof(wtemp));
		//m_cbAgreementNo.GetText(wtemp);
		//strcpy_s(req.AgreementNo, LoadRC::unicode_to_ansi(wtemp).c_str());
		//��Լ���
		memset(&atemp, 0, sizeof(atemp));
		m_editContractNo.GetText(atemp, sizeof(atemp));
		TContractKey conKey;
		memset(&conKey, 0, sizeof(TContractKey));
		CConvert::TContractNoToTContractKey(atemp, conKey);
		char sCode[101] = { 0 };
		CConvert::TContractKeyToSContractNo(conKey, sCode);
		strncpy_s(req.ContractNo, sCode);
		//�ֱֲ��
		memset(&atemp, 0, sizeof(atemp));
		m_editPositionNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.PositionNo, atemp);
		//�ֲ�����
		req.PositionQty = (int)m_editPositionQty.GetDoubleData();
		//��������
		memset(&atemp, 0, sizeof(atemp));
		m_editDirect.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "��") == 0)
			req.Direct = SMS_DIRECT_BUY;
		else
			req.Direct = SMS_DIRECT_SELL;
		//��ƽ
		//memset(&atemp, 0, sizeof(atemp));
		//m_editOffset.GetText(atemp, sizeof(atemp));
		//if (strcmp(atemp, "����") == 0)
		//	req.Offset = SMS_O_OPEN;
		//else
		//	req.Offset = SMS_O_COVER;
		//Ͷ������
		memset(&atemp, 0, sizeof(atemp));
		m_editHedge.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "Ͷ��") == 0)
			req.Hedge = SMS_HEDGE_SPECULATE;
		else
			req.Hedge = SMS_HEDGE_HEDGE;
		//��������
		wstring date;
		m_cbtSettleDate.CxGetWindowText(date);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(date).c_str());
		//�ֲּ۸�
		req.PositionPrice = m_editPositionPrice.GetDoubleData();
		//�ֲ�����
		req.PositionProfit = m_editPositionProfit.GetDoubleData();

		//����Ա
		strncpy_s(req.OperatorNo, m_subSystem->m_curLogin.UserNo);
	       
		g_pSpotSystem->AddPosition(&req);
		OnBtnCancel();
	}
	else 
	{
		SmsPositionModReq req;
		memset(&req, 0, sizeof(SmsPositionModReq));

		char		atemp[MAX_PATH] = { 0 };
		wchar_t		wtemp[MAX_PATH] = { 0 };

		//���к�
		req.SerialId = m_serialId;
		//��˾���
		memset(&atemp, 0, sizeof(atemp));
		m_editCompanyNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.CompanyNo, atemp);
		//����Ա���
		memset(&atemp, 0, sizeof(atemp));
		m_editUserNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.UserNo, atemp);
		//��ͬ���
		//memset(&wtemp, 0, sizeof(wtemp));
		//m_cbAgreementNo.GetText(wtemp);
		//strcpy_s(req.AgreementNo, LoadRC::unicode_to_ansi(wtemp).c_str());
		//��Լ���
		memset(&atemp, 0, sizeof(atemp));
		m_editContractNo.GetText(atemp, sizeof(atemp));
		TContractKey conKey;
		memset(&conKey, 0, sizeof(TContractKey));
		CConvert::TContractNoToTContractKey(atemp, conKey);
		char sCode[101] = { 0 };
		CConvert::TContractKeyToSContractNo(conKey, sCode);
		strncpy_s(req.ContractNo, sCode);
		//�ֱֲ��
		memset(&atemp, 0, sizeof(atemp));
		m_editPositionNo.GetText(atemp, sizeof(atemp));
		strncpy_s(req.PositionNo, atemp);
		//�ֲ�����
		req.PositionQty = (int)m_editPositionQty.GetDoubleData();
		//��������
		memset(&atemp, 0, sizeof(atemp));
		m_editDirect.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "��") == 0)
			req.Direct = SMS_DIRECT_BUY;
		else
			req.Direct = SMS_DIRECT_SELL;
		//��ƽ
		//memset(&atemp, 0, sizeof(atemp));
		//m_editOffset.GetText(atemp, sizeof(atemp));
		//if (strcmp(atemp, "����") == 0)
		//	req.Offset = SMS_O_OPEN;
		//else
		//	req.Offset = SMS_O_COVER;
		//Ͷ������
		memset(&atemp, 0, sizeof(atemp));
		m_editHedge.GetText(atemp, sizeof(atemp));
		if (strcmp(atemp, "Ͷ��") == 0)
			req.Hedge = SMS_HEDGE_SPECULATE;
		else
			req.Hedge = SMS_HEDGE_HEDGE;
		//��������
		wstring date;
		m_cbtSettleDate.CxGetWindowText(date);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(date).c_str());
		//�ֲּ۸�
		req.PositionPrice = m_editPositionPrice.GetDoubleData();
		//�ֲ�����
		req.PositionProfit = m_editPositionProfit.GetDoubleData();

		//����Ա
		strncpy_s(req.OperatorNo, m_subSystem->m_curLogin.UserNo);

		g_pSpotSystem->ModifyPosition(&req);
		OnBtnCancel();
	}
}

void AddPositionWnd::OnBtnCancel()
{
	if (IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_HIDE);
	else
		ShowWindow(m_Hwnd, SW_SHOW);
}

void AddPositionWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
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

void AddPositionWnd::OnBtnCode()
{
	POINT pt = { 0, 0 };
	ClientToScreen(m_editContractNo.GetHwnd(), &pt);
	pt.y += 25;

	if (!IsWindow(m_popCode.GetHwnd()))
		m_popCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0, 0, pt.x, pt.y);

	if (!m_subSystem)return;
	m_popCode.UpdateData(m_subSystem->m_curLogin.sSign);

	SetWindowPos(m_popCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}
