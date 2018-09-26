#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_H    610
#define cFRAME_V    485

extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
ModifyOrderWnd::ModifyOrderWnd()
{
	m_bMouseTrack = false;
}

//ModifyOrderWnd::~ModifyOrderWnd()
//{
//	
//}

void ModifyOrderWnd::ShowFrm(HWND hParent)
{
	CreateFrm(L"TModifyOrderWnd", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_H, cFRAME_V, SWP_SHOWWINDOW);
}

void ModifyOrderWnd::Register(SystemFrame * pdata)
{
	if (!pdata)return;
	m_subSystem = pdata;
}

void ModifyOrderWnd::InitWnd(SmsOrderDataRsp & order)
{
	//���º�ͬ
	m_cbAgreementNo.Clear();
	string key = m_subSystem->get_curkey();
	m_cbAgreementNo.AddString(L"");
	for (auto it : m_subSystem->m_map_mapAgreement[key])
	{
		m_cbAgreementNo.AddString(LoadRC::ansi_to_unicode(it.second.AgreementNo).c_str());
	}
	//������
	m_editSerialId.SetDoubleData(order.SerialId);
	m_editCompanyNo.SetText(order.CompanyNo);
	m_editUserNo.SetText(order.UserNo);

	string sCode = order.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	m_editContractNo.SetText(tCode.c_str());

	m_editOrderNo.SetText(order.OrderNo);
	m_editSystemNo.SetText(order.SystemNo);

	if(order.Direct == SMS_DIRECT_BUY)
		m_editDirect.SetText("��");
	else
		m_editDirect.SetText("��");
	if(order.Offset == SMS_O_OPEN)
		m_editOffset.SetText("����");
	else
		m_editOffset.SetText("ƽ��");
	if(order.Hedge == SMS_HEDGE_SPECULATE)
		m_editHedge.SetText("Ͷ��");
	else
		m_editHedge.SetText("�ױ�");
	m_editSettleDate.SetText(order.SettleDate);

	bool bfound = m_cbAgreementNo.SetSelect(LoadRC::ansi_to_unicode(order.AgreementNo).c_str());
	if (!bfound)
		m_cbAgreementNo.SetSelect(0);

	if (order.OrderState == SMS_OS_QUEUED)
		m_cbOrderState.SetSelect(0);
	else if (order.OrderState == SMS_OS_PARTFILLED)
		m_cbOrderState.SetSelect(1);
	else if(order.OrderState == SMS_OS_FILLED)
		m_cbOrderState.SetSelect(2);
	else if (order.OrderState == SMS_OS_PARTCANCELED)
		m_cbOrderState.SetSelect(3);
	else if (order.OrderState == SMS_OS_CANCELED)
		m_cbOrderState.SetSelect(4);
	else
		m_cbOrderState.SetSelect(5);
	m_editOrderPrice.SetDoubleData(order.OrderPrice, 2);
	m_editOrderQty.SetDoubleData(order.OrderQty);
	m_editMatchPrice.SetDoubleData(order.MatchPrice, 2);
	m_editMatchQty.SetDoubleData(order.MatchQty);
	m_editOrderTime.SetText(order.OrderTime);
	m_editLocalNo.SetText(order.LocalNo);
	m_editErrorCode.SetDoubleData(order.ErrorCode);
	m_editErrorText.SetText(order.ErrorText);
}

LRESULT ModifyOrderWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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



void ModifyOrderWnd::OnCreate()
{
	InitCommonControls();
	CreateWidget();
	InitWidget();
	SetWidgetPos();
}

void ModifyOrderWnd::OnPaint()
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
	DrawText(memdc.GetHdc(), L"�޸�ί��", -1, &rcCaption, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	//DrawClose(&memdc);
	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}

void ModifyOrderWnd::OnSize(WPARAM wParam, LPARAM lParam)
{

}

void ModifyOrderWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

void ModifyOrderWnd::OnMouseLeave()
{
	m_bMouseTrack = false;

}

void ModifyOrderWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
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

void ModifyOrderWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
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

void ModifyOrderWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
}

void ModifyOrderWnd::KeyDown(WPARAM wParam, LPARAM lParam)
{
}

void ModifyOrderWnd::OnNcDestroy()
{
	//delete this;
}

void ModifyOrderWnd::CreateWidget()
{
	//���к�
	m_labSerialId.Create(m_Hwnd, L"���к�", DT_LEFT, ID_labSerialId);
	m_editSerialId.Create(m_Hwnd, 0, 0, ID_editSerialId);
	//��˾
	m_labCompanyNo.Create(m_Hwnd, L"��˾", DT_LEFT, ID_labCompanyNo);
	m_editCompanyNo.Create(m_Hwnd, 0, 0, ID_editCompanyNo);
	//�˺�
	m_labUserNo.Create(m_Hwnd, L"�˺�", DT_LEFT, ID_labUserNo);
	m_editUserNo.Create(m_Hwnd, 0, 0, ID_editUserNo);
	//��Լ
	m_labContractNo.Create(m_Hwnd, L"��Լ", DT_LEFT, ID_labContractNo);
	m_editContractNo.Create(m_Hwnd, 0, 0, ID_editContractNo);
	//ί�к�
	m_labOrderNo.Create(m_Hwnd, L"ί�к�", DT_LEFT, ID_labOrderNo);
	m_editOrderNo.Create(m_Hwnd, 0, 0, ID_editOrderNo);
	//ϵͳ��
	m_labSystemNo.Create(m_Hwnd, L"ϵͳ��", DT_LEFT, ID_labSystemNo);
	m_editSystemNo.Create(m_Hwnd, 0, 0, ID_editSystemNo);
	//����
	m_labDirect.Create(m_Hwnd, L"����", DT_LEFT, ID_labDirect);
	m_editDirect.Create(m_Hwnd, 0, 0, ID_editDirect);
	//��ƽ
	m_labOffset.Create(m_Hwnd, L"��ƽ", DT_LEFT, ID_labOffset);
	m_editOffset.Create(m_Hwnd, 0, 0, ID_editOffset);
	//Ͷ��
	m_labHedge.Create(m_Hwnd, L"Ͷ��", DT_LEFT, ID_labHedge);
	m_editHedge.Create(m_Hwnd, 0, 0, ID_editHedge);
	//��������
	m_labSettleDate.Create(m_Hwnd, L"��������", DT_LEFT, ID_labSettleDAate);
	m_editSettleDate.Create(m_Hwnd, 0, 0, ID_editSettleDAate);

	//��ͬ���
	m_labAgreementNo.Create(m_Hwnd, L"��ͬ���", DT_LEFT, ID_labAgreementNo);
	m_cbAgreementNo.Create(m_Hwnd, ID_cbAgreementNo);
	//����״̬
	m_labOrderState.Create(m_Hwnd, L"����״̬", DT_LEFT, ID_labOrderState);
	m_cbOrderState.Create(m_Hwnd, ID_cbOrderState);
	//ί�м۸�
	m_labOrderPrice.Create(m_Hwnd, L"ί�м۸�", DT_LEFT, ID_labOrderPrice);
	m_editOrderPrice.Create(m_Hwnd, 0, 0, ID_editOrderPrice);
	//ί������
	m_labOrderQty.Create(m_Hwnd, L"ί������", DT_LEFT, ID_labOrderQty);
	m_editOrderQty.Create(m_Hwnd, 0, 0, ID_editOrderQty);
	//�ɽ��۸�
	m_labMatchPrice.Create(m_Hwnd, L"�ɽ��۸�", DT_LEFT, ID_labMatchPrice);
	m_editMatchPrice.Create(m_Hwnd, 0, 0, ID_editMatchPrice);
	//�ɽ�����
	m_labMatchQty.Create(m_Hwnd, L"�ɽ�����", DT_LEFT, ID_labMatchQty);
	m_editMatchQty.Create(m_Hwnd, 0, 0, ID_editMatchQty);
	//���غ�
	m_labLocalNo.Create(m_Hwnd, L"���غ�", DT_LEFT, ID_labLocalNo);
	m_editLocalNo.Create(m_Hwnd, 0, 0, ID_editLocalNo);
	//�µ�ʱ��
	m_labOrderTime.Create(m_Hwnd, L"�µ�ʱ��", DT_LEFT, ID_labOrderTime);
	m_editOrderTime.Create(m_Hwnd, 0, 0, ID_editOrderTime);
	//������
	m_labErrorCode.Create(m_Hwnd, L"������", DT_LEFT, ID_labErrorCode);
	m_editErrorCode.Create(m_Hwnd, 0, 0, ID_editErrorCode);
	//������Ϣ
	m_labErrorText.Create(m_Hwnd, L"������Ϣ", DT_LEFT, ID_labErrorText);
	m_editErrorText.Create(m_Hwnd, 0, 0, ID_editErrorText);
	
	m_btnOk.Create(m_Hwnd, "ȷ  ��", ID_btnOK);
	m_btnCancel.Create(m_Hwnd, "ȡ  ��", ID_btnCancel);
}

void ModifyOrderWnd::InitWidget()
{
	m_labSerialId.SetFont(g_FontData.GetFontWord15());
	m_labSerialId.SetFont(g_FontData.GetFontWord15());
	m_labCompanyNo.SetFont(g_FontData.GetFontWord15());
	m_labContractNo.SetFont(g_FontData.GetFontWord15());
	m_labOrderNo.SetFont(g_FontData.GetFontWord15());
	m_labSystemNo.SetFont(g_FontData.GetFontWord15());
	m_labDirect.SetFont(g_FontData.GetFontWord15());
	m_labOffset.SetFont(g_FontData.GetFontWord15());
	m_labHedge.SetFont(g_FontData.GetFontWord15());
	m_labSettleDate.SetFont(g_FontData.GetFontWord15());

	m_labAgreementNo.SetFont(g_FontData.GetFontWord15());
	m_labOrderState.SetFont(g_FontData.GetFontWord15());
	m_labOrderPrice.SetFont(g_FontData.GetFontWord15());
	m_labOrderQty.SetFont(g_FontData.GetFontWord15());
	m_labMatchPrice.SetFont(g_FontData.GetFontWord15());
	m_labMatchQty.SetFont(g_FontData.GetFontWord15());
	m_labLocalNo.SetFont(g_FontData.GetFontWord15());
	m_labOrderTime.SetFont(g_FontData.GetFontWord15());
	m_labErrorCode.SetFont(g_FontData.GetFontWord15());
	m_labErrorText.SetFont(g_FontData.GetFontWord15());


	m_editSerialId.SetFont(g_FontData.GetFontNum15());
	m_editCompanyNo.SetFont(g_FontData.GetFontNum15());
	m_editUserNo.SetFont(g_FontData.GetFontNum15());
	m_editContractNo.SetFont(g_FontData.GetFontNum15());
	m_editOrderNo.SetFont(g_FontData.GetFontNum15());
	m_editSystemNo.SetFont(g_FontData.GetFontNum15());
	m_editDirect.SetFont(g_FontData.GetFontNum15());
	m_editOffset.SetFont(g_FontData.GetFontNum15());
	m_editHedge.SetFont(g_FontData.GetFontNum15());
	m_editSettleDate.SetFont(g_FontData.GetFontNum15());

	m_cbAgreementNo.SetFont(g_FontData.GetFontNum15());
	m_cbOrderState.SetFont(g_FontData.GetFontNum15());
	m_editOrderPrice.SetFont(g_FontData.GetFontNum15());
	m_editOrderQty.SetFont(g_FontData.GetFontNum15());
	m_editMatchPrice.SetFont(g_FontData.GetFontNum15());
	m_editMatchQty.SetFont(g_FontData.GetFontNum15());
	m_editLocalNo.SetFont(g_FontData.GetFontNum15());
	m_editOrderTime.SetFont(g_FontData.GetFontNum15());
	m_editErrorCode.SetFont(g_FontData.GetFontNum15());
	m_editErrorText.SetFont(g_FontData.GetFontNum15());

	m_cbOrderState.SetFont(g_FontData.GetFontNum15());
	m_cbOrderState.AddString(L"���Ŷ�");
	m_cbOrderState.AddString(L"���ֳɽ�");
	m_cbOrderState.AddString(L"��ȫ�ɽ�");
	m_cbOrderState.AddString(L"�ѳ��൥");
	m_cbOrderState.AddString(L"�ѳ���");
	m_cbOrderState.AddString(L"��Ч��");

	EnableWindow(m_labSerialId.GetHwnd(), FALSE);
	EnableWindow(m_editSerialId.GetHwnd(), FALSE);
	EnableWindow(m_labCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_editCompanyNo.GetHwnd(), FALSE);
	EnableWindow(m_labUserNo.GetHwnd(), FALSE);
	EnableWindow(m_editUserNo.GetHwnd(), FALSE);
	EnableWindow(m_labContractNo.GetHwnd(), FALSE);
	EnableWindow(m_editContractNo.GetHwnd(), FALSE);
	EnableWindow(m_labOrderNo.GetHwnd(), FALSE);
	EnableWindow(m_editOrderNo.GetHwnd(), FALSE);
	EnableWindow(m_labSystemNo.GetHwnd(), FALSE);
	EnableWindow(m_editSystemNo.GetHwnd(), FALSE);
	EnableWindow(m_labDirect.GetHwnd(), FALSE);
	EnableWindow(m_editDirect.GetHwnd(), FALSE);
	EnableWindow(m_labOffset.GetHwnd(), FALSE);
	EnableWindow(m_editOffset.GetHwnd(), FALSE);
	EnableWindow(m_labHedge.GetHwnd(), FALSE);
	EnableWindow(m_editHedge.GetHwnd(), FALSE);
	EnableWindow(m_labSettleDate.GetHwnd(), FALSE);
	EnableWindow(m_editSettleDate.GetHwnd(), FALSE);
}

void ModifyOrderWnd::SetWidgetPos()
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
	//���к�
	m_labSerialId.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSerialId.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��˾��
	pt.x += editWidth + htap;
	m_labCompanyNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCompanyNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�˺�
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labUserNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editUserNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��Լ
	pt.x += editWidth + htap;
	m_labContractNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editContractNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//ί�к�
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labOrderNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editOrderNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//ϵͳ��
	pt.x += editWidth + htap;
	m_labSystemNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSystemNo.MoveWindow(pt.x, pt.y, editWidth, Height);
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
	m_editSettleDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��ͬ���
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height*10);
	//����״̬
	pt.x += editWidth + htap;
	m_labOrderState.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbOrderState.MoveWindow(pt.x, pt.y, editWidth, Height*10);
	//ί�м۸�
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labOrderPrice.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editOrderPrice.MoveWindow(pt.x, pt.y, editWidth, Height);
	//ί������
	pt.x += editWidth + htap;
	m_labOrderQty.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editOrderQty.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�ɽ���
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labMatchPrice.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editMatchPrice.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�ɽ�����
	pt.x += editWidth + htap;
	m_labMatchQty.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editMatchQty.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�µ�ʱ��
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labOrderTime.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editOrderTime.MoveWindow(pt.x, pt.y, editWidth, Height);
	//���غ�
	pt.x += editWidth + htap;
	m_labLocalNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editLocalNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//������
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labErrorCode.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editErrorCode.MoveWindow(pt.x, pt.y, editWidth, Height);
	//������Ϣ
	pt.x += editWidth + htap;
	m_labErrorText.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editErrorText.MoveWindow(pt.x, pt.y, editWidth, Height);

	//ȷ��
	//ȡ��
	pt.x = cFRAME_H / 2 - 60 - btnWidth;
	pt.y += Height + vtap ;
	m_btnOk.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + 120;
	m_btnCancel.MoveWindow(pt.x, pt.y, btnWidth, Height);
}

LRESULT ModifyOrderWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetHbrWhite();
}

void ModifyOrderWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

void ModifyOrderWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
}

void ModifyOrderWnd::OnBtnOk()
{
	if (!m_subSystem)return;

	SmsOrderModReq req;
	memset(&req, 0, sizeof(SmsOrderModReq));
	char		atemp[MAX_PATH] = { 0 };
	wchar_t		wtemp[MAX_PATH] = { 0 };
	//���к� �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editSerialId.GetText(atemp, sizeof(atemp));
	req.SerialId = atoi(atemp);
	//��˾��� �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editCompanyNo.GetText(atemp, sizeof(atemp));
	strncpy_s(req.CompanyNo, atemp);
	//����Ա��� �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editUserNo.GetText(atemp, sizeof(atemp));
	strncpy_s(req.UserNo, atemp);
	//��Լ��� �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editContractNo.GetText(atemp, sizeof(atemp));
	TContractKey conKey;
	memset(&conKey, 0, sizeof(TContractKey));
	CConvert::TContractNoToTContractKey(atemp, conKey);
	char sCode[101] = { 0 };
	CConvert::TContractKeyToSContractNo(conKey, sCode);
	strncpy_s(req.ContractNo, sCode);
	//ί�б�� �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editOrderNo.GetText(atemp, sizeof(atemp));
	strncpy_s(req.OrderNo, atemp);
	//ϵͳ��   �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editSystemNo.GetText(atemp, sizeof(atemp));
	strncpy_s(req.SystemNo, atemp);
	//��������  �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editDirect.GetText(atemp, sizeof(atemp));
	if (strcmp(atemp, "��") == 0)
		req.Direct = SMS_DIRECT_BUY;
	else
		req.Direct = SMS_DIRECT_SELL;
	//��ƽ    �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editOffset.GetText(atemp, sizeof(atemp));
	if (strcmp(atemp, "����") == 0)
		req.Offset = SMS_O_OPEN;
	else
		req.Offset = SMS_O_COVER;
	//Ͷ������  �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editHedge.GetText(atemp, sizeof(atemp));
	if (strcmp(atemp, "Ͷ��") == 0)
		req.Hedge = SMS_HEDGE_SPECULATE;
	else
		req.Hedge = SMS_HEDGE_HEDGE;
	//�������� �����޸�
	memset(&atemp, 0, sizeof(atemp));
	m_editSettleDate.GetText(atemp, sizeof(atemp));
	strncpy_s(req.SettleDate, atemp);
	//��ͬ��� 
	memset(&wtemp, 0, sizeof(wtemp));
	m_cbAgreementNo.GetText(wtemp);
	strcpy_s(req.AgreementNo, LoadRC::unicode_to_ansi(wtemp).c_str());
	//����״̬
	memset(&wtemp, 0, sizeof(wtemp));
	m_cbOrderState.GetText(wtemp);
	string orderState = LoadRC::unicode_to_ansi(wtemp);
	if (strcmp(orderState.c_str(), "���Ŷ�") == 0)
		req.OrderState = SMS_OS_QUEUED;
	else if (strcmp(orderState.c_str(), "���ֳɽ�") == 0)
		req.OrderState = SMS_OS_PARTFILLED;
	else if (strcmp(orderState.c_str(), "��ȫ�ɽ�") == 0)
		req.OrderState = SMS_OS_FILLED;
	else if (strcmp(orderState.c_str(), "�ѳ��൥") == 0)
		req.OrderState = SMS_OS_PARTCANCELED;
	else if (strcmp(orderState.c_str(), "�ѳ���") == 0)
		req.OrderState = SMS_OS_CANCELED;
	else
		req.OrderState = SMS_OS_INVALID;

	//ί�м۸�
	memset(&atemp, 0, sizeof(atemp));
	m_editOrderPrice.GetText(atemp, sizeof(atemp));
	req.OrderPrice = atof(atemp);
	//ί������
	memset(&atemp, 0, sizeof(atemp));
	m_editOrderQty.GetText(atemp, sizeof(atemp));
	req.OrderQty = atoi(atemp);
	//�ɽ��۸�
	memset(&atemp, 0, sizeof(atemp));
	m_editMatchPrice.GetText(atemp, sizeof(atemp));
	req.MatchPrice = atof(atemp);
	//�ɽ�����
	memset(&atemp, 0, sizeof(atemp));
	m_editMatchQty.GetText(atemp, sizeof(atemp));
	req.MatchQty = atoi(atemp);
	//�µ�ʱ��
	memset(&atemp, 0, sizeof(atemp));
	m_editOrderTime.GetText(atemp, sizeof(atemp));
	strncpy_s(req.OrderTime, atemp);
	//���ر��
	memset(&atemp, 0, sizeof(atemp));
	m_editLocalNo.GetText(atemp, sizeof(atemp));
	strncpy_s(req.LocalNo, atemp);
	//������
	memset(&atemp, 0, sizeof(atemp));
	m_editErrorCode.GetText(atemp, sizeof(atemp));
	req.ErrorCode = atoi(atemp);
	//������Ϣ
	memset(&atemp, 0, sizeof(atemp));
	m_editErrorText.GetText(atemp, sizeof(atemp));
	strncpy_s(req.ErrorText, atemp);
	//����Ա
	strncpy_s(req.OperatorNo, m_subSystem->m_curLogin.UserNo);

	g_pSpotSystem->ModifyOrder(&req);
	OnBtnCancel();
}

void ModifyOrderWnd::OnBtnCancel()
{
	if (IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_HIDE);
	else
		ShowWindow(m_Hwnd, SW_SHOW);
}

void ModifyOrderWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
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
