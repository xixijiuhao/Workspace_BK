#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_V    405
#define cFRAME_H    310
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
AddAgreementWnd::AddAgreementWnd()
	:m_title("¼���ͬ"),
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
	m_title = "¼���ͬ";
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
	m_title = "�޸ĺ�ͬ";
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
	//��ͬ���
	m_labAgreementNo.Create(m_Hwnd, L"��ͬ���", DT_CENTER, ID_labAgreementNo);
	m_editAgreementNo.Create(m_Hwnd, 0, 0, ID_editAgreementNo);
	//�û����
	m_labUserNo.Create(m_Hwnd, L"�û����", DT_CENTER, ID_labUserNo);
	m_cbUserNo.Create(m_Hwnd, ID_editUserNo);
	//��ͬ����
	m_labAgreementType.Create(m_Hwnd, L"��ͬ����", DT_CENTER, ID_labAgreementType);
	m_cbAgreementType.Create(m_Hwnd, ID_cbAgreementType);
	//�ֻ��۸�
	m_labSpotPrice.Create(m_Hwnd, L"�ֻ��۸�", DT_CENTER, ID_labSpotPrice);
	m_editSpotPrice.Create(m_Hwnd, 0, 0, ID_editSpotPrice);
	//�ֻ�����
	m_labSpotQty.Create(m_Hwnd, L"�ֻ�����", DT_CENTER, ID_labSpotQty);
	m_spineditSpotQty.Create(m_Hwnd, 0, ID_editSpotQty, ID_spinSpotQty);
	//��ͬ����
	m_labSignDate.Create(m_Hwnd, L"��ͬ����", DT_CENTER, ID_labSignDate);
	//�����
	m_labSpecificationNo.Create(m_Hwnd, L"�����", DT_CENTER, ID_labSpecificationNo);
	m_cbSpecificationNo.Create(m_Hwnd, ID_cbSpecificationNo);
	//��ƽ����
	//m_labPositionOffset.Create(m_Hwnd, L"��ƽ����", DT_CENTER, ID_labPositionOffset);
	//m_cbPositionOffset.Create(m_Hwnd, ID_cbPositionOffset);
	//��ͬ״̬
	//m_labAgreementState.Create(m_Hwnd, L"��ͬ״̬", DT_CENTER, ID_labAgreementState);
	//m_cbAgreementState.Create(m_Hwnd, ID_cbAgreementState);

	m_btnOk.Create(m_Hwnd, "ȷ  ��", ID_btnOK);
	m_btnCancel.Create(m_Hwnd, "ȡ  ��", ID_btnCancel);
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
	m_cbAgreementType.AddString(L"�ɹ���ͬ");
	m_cbAgreementType.AddString(L"���ۺ�ͬ");

	m_cbSpecificationNo.SetFont(g_FontData.GetFontNum15());

	//m_cbPositionOffset.SetFont(g_FontData.GetFontNum15());
	//m_cbPositionOffset.AddString(L"����");
	//m_cbPositionOffset.AddString(L"ƽ��");

	//m_cbAgreementState.SetFont(g_FontData.GetFontNum15());
	//m_cbAgreementState.AddString(L"��ͬ��Ч");
	//m_cbAgreementState.AddString(L"��ͬ����");
}

void AddAgreementWnd::SetWidgetPos()
{
	int leftstart = 15;
	int topstart = 15;
	int editWidth = 150;	//edit�ؼ����
	int labWidth = 100;		//lab�ؼ����
	int btnWidth = 80;		//btn�ؼ����
	int Height = 25;		//�ؼ��߶�
	int htap = 10;			//������
	int vtap = 15;			//������
	m_hInstance = GetModuleHandle(NULL);
	POINT pt;
	pt.x = leftstart; 
	pt.y = cCAPTION_H + topstart;
	//��ͬ���
	m_labAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�û����
	pt.x = leftstart; pt.y += Height + vtap;
	m_labUserNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbUserNo.MoveWindow(pt.x, pt.y, editWidth, Height*5);
	//�ֻ��۸�
	pt.x = leftstart; pt.y += Height + vtap;
	m_labSpotPrice.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSpotPrice.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�ֻ�����
	pt.x = leftstart; pt.y += Height + vtap;
	m_labSpotQty.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_spineditSpotQty.MoveWindow(pt.x, pt.y, editWidth, Height);
	//ǩ������
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
	
	//��ͬ����
	pt.x = leftstart; pt.y += Height + vtap;
	m_labAgreementType.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbAgreementType.MoveWindow(pt.x, pt.y, editWidth, Height * 5);
	//�����
	pt.x = leftstart; pt.y += Height + vtap;
	m_labSpecificationNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbSpecificationNo.MoveWindow(pt.x, pt.y, editWidth, Height* 5);
	//��ƽ������
	//pt.x = leftstart; pt.y += Height + vtap;
	//m_labPositionOffset.MoveWindow(pt.x, pt.y, labWidth, Height);
	//pt.x += labWidth + htap;
	//m_cbPositionOffset.MoveWindow(pt.x, pt.y, editWidth, Height*5);
	//��ͬ״̬
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
		//��˾
		strcpy_s(addReq.CompanyNo, m_subSystem->m_curLogin.CompanyNo);
		//��ͬ
		m_editAgreementNo.GetText(temp, sizeof(temp));
		strcpy_s(addReq.AgreementNo, temp);
		//�û�
		wchar_t userno[MAX_PATH] = { 0 };
		m_cbUserNo.GetText(userno);
		strcpy_s(addReq.UserNo, LoadRC::unicode_to_ansi(userno).c_str());
		//��ͬ����
		if (m_cbAgreementType.GetSelect() == 0)
		{
			addReq.AgreementType = SMS_AGREEMENT_PURCHASE;
		}
		else
		{
			addReq.AgreementType = SMS_AGREEMENT_SALE;
		}
		//�ֻ��۸�
		addReq.SpotPrice = m_editSpotPrice.GetDoubleData();
		//�ֻ�����
		addReq.SpotQty = m_spineditSpotQty.GetWindowNumber();
		//��ͬ����
		wstring tempDate;
		m_cbtSignDate.CxGetWindowText(tempDate);
		strcpy_s(addReq.SignDate, LoadRC::unicode_to_ansi(tempDate).c_str());
		//�����
		wchar_t spec[MAX_PATH] = { 0 };
		m_cbSpecificationNo.GetText(spec);
		strcpy_s(addReq.SpecificationNo, LoadRC::unicode_to_ansi(spec).c_str());
		//��ƽ��
		//if (m_cbPositionOffset.GetSelect() == 0)
		//{
		//	addReq.PositionOffset = SMS_O_OPEN;
		//}
		//else
		//{
		//	addReq.PositionOffset = SMS_O_COVER;
		//}
		//��ͬ״̬
		addReq.AgreementState = SMS_AGREEMENT_ACTIVE;
		//����Ա
		strcpy_s(addReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

		g_pSpotSystem->AddAgreement(&addReq);
		OnBtnCancel();
	}
	else
	{
		SmsAgreementModReq modReq;
		memset(&modReq, 0, sizeof(SmsAgreementAddReq));
		char temp[MAX_PATH] = { 0 };
		//��˾
		strcpy_s(modReq.CompanyNo, m_subSystem->m_curLogin.CompanyNo);
		//��ͬ
		m_editAgreementNo.GetText(temp, sizeof(temp));
		strcpy_s(modReq.AgreementNo, temp);
		//�û�
		wchar_t userno[MAX_PATH] = { 0 };
		m_cbUserNo.GetText(userno);
		strcpy_s(modReq.UserNo, LoadRC::unicode_to_ansi(userno).c_str());
		//��ͬ����
		if (m_cbAgreementType.GetSelect() == 0)
		{
			modReq.AgreementType = SMS_AGREEMENT_PURCHASE;
		}
		else
		{
			modReq.AgreementType = SMS_AGREEMENT_SALE;
		}
		//�ֻ��۸�
		modReq.SpotPrice = m_editSpotPrice.GetDoubleData();
		//�ֻ�����
		modReq.SpotQty = m_spineditSpotQty.GetWindowNumber();
		//��ͬ����
		wstring tempDate;
		m_cbtSignDate.CxGetWindowText(tempDate);
		strcpy_s(modReq.SignDate, LoadRC::unicode_to_ansi(tempDate).c_str());
		//¼��ʱ��
		strcpy_s(modReq.InsertTime, m_insertTime.c_str());
		//�����
		wchar_t spec[MAX_PATH] = { 0 };
		m_cbSpecificationNo.GetText(spec);
		strcpy_s(modReq.SpecificationNo, LoadRC::unicode_to_ansi(spec).c_str());
		//��ƽ��
		//if (m_cbPositionOffset.GetSelect() == 0)
		//{
		//	modReq.PositionOffset = SMS_O_OPEN;
		//}
		//else
		//{
		//	modReq.PositionOffset = SMS_O_COVER;
		//}
		//��ͬ״̬
		modReq.AgreementState = SMS_AGREEMENT_ACTIVE;
		//����Ա
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
