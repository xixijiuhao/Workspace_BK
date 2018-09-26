#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_H    1030+1
#define cFRAME_V    485+2
#define  ccloseWidth 13

extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
AddAgreementRelWnd::AddAgreementRelWnd()
	:m_remain(0)
{
	m_bMouseTrack = false;

	m_rcClose.right = cFRAME_H - 8;
	m_rcClose.left = m_rcClose.right - ccloseWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + ccloseWidth;
}

//AddAgreementRelWnd::~AddAgreementRelWnd()
//{
//	
//}

void AddAgreementRelWnd::ShowFrm(HWND hParent)
{
	CreateFrm(L"TAddAgreementRelWnd", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_H, cFRAME_V, SWP_SHOWWINDOW);
}

void AddAgreementRelWnd::Register(SystemFrame * pdata)
{
	if (!pdata)return;
	m_subSystem = pdata;
}

void AddAgreementRelWnd::InitWnd(SmsAgreementDataRsp& agree, bool isBuy, vector<SmsAgreementDataRsp>& unrel, vector<string>& rel)
{
	m_listAgreeSel.DeleteAllItems();
	m_listAgreeAdd.DeleteAllItems();
	m_mapUnRelAgree.clear();

	//�ѹ�����
	m_remain = 0;
	int relQty = 0;
	for (auto it_rel : rel)
	{
		for (auto it_unrel : unrel)
		{
			if (strcmp(it_unrel.AgreementNo,it_rel.c_str()) == 0)
			{
				relQty += it_unrel.SpotQty;
				break;
			}
		}
	}
	//ʣ����
	m_remain = agree.SpotQty - relQty;
	char tips2[256] = { 0 };
	sprintf_s(tips2, "����:%d", m_remain < 0 ? 0 : m_remain);
	m_labTips2.SetText(tips2);

	m_bisBuy = isBuy;
	char tips[256] = { 0 };
	sprintf_s(tips, "%s  %s  %d  %.2f  %s", agree.AgreementType == SMS_AGREEMENT_PURCHASE ? "�ɹ���ͬ" : "���ۺ�ͬ",
		agree.SpecificationNo, agree.SpotQty, agree.SpotPrice,  agree.SignDate);
	m_labTips.SetText(tips);

	if (isBuy)
	{
		m_labBuyAgreementNo.SetText("*�ɹ���ͬ*");
		m_labSellAgreementNo.SetText("���ۺ�ͬ");
		m_editBuyAgreementNo.SetText(agree.AgreementNo);
		m_editSellAgreementNo.SetText("");
		m_listAgreeAdd.SetColumnText(L"�ѹ������ۺ�ͬ",0);
		OpenLayOut();
	}
	else
	{
		m_labBuyAgreementNo.SetText("�ɹ���ͬ");
		m_labSellAgreementNo.SetText("*���ۺ�ͬ*");
		m_editBuyAgreementNo.SetText("");
		m_editSellAgreementNo.SetText(agree.AgreementNo);
		m_listAgreeAdd.SetColumnText(L"�ѹ����ɹ���ͬ", 0);
		CloseLayOut();
	}

	for (auto it : unrel)
	{
		AddAgreementToListSel(it);
		m_mapUnRelAgree[it.AgreementNo] = it;
	}	
	for (auto it : rel)
	{
		AddAgreementToListAdd(it);
		//m_listAgreeAdd.AddItem(it.c_str(), 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	}
}

void AddAgreementRelWnd::AddAgreementToListSel(SmsAgreementDataRsp & agree)
{
	//�û����
	m_listAgreeSel.AddItem(agree.UserNo, 0, AGREE_UserNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��ͬ���
	m_listAgreeSel.AddItem(agree.AgreementNo, 0, AGREE_AgreementNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��ͬ����
	if (agree.AgreementType == SMS_AGREEMENT_PURCHASE)
		m_listAgreeSel.AddItem("�ɹ���ͬ", 0, AGREE_AgreementType, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	else if (agree.AgreementType == SMS_AGREEMENT_SALE)
		m_listAgreeSel.AddItem("���ۺ�ͬ", 0, AGREE_AgreementType, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	//�����
	m_listAgreeSel.AddItem(agree.SpecificationNo, 0, AGREE_SpecificationNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ֻ�����
	m_listAgreeSel.AddItem((double)agree.SpotQty, 0, AGREE_SpotQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ֻ�������
	m_listAgreeSel.AddItem((double)agree.SpotRemainQty, 0, AGREE_SpotRemainQty, 0, DT_CENTER, agree.SpotRemainQty > 0 ? g_ColorRefData.GetColorBuyRed() : g_ColorRefData.GetColorBlack());
	//�ֲ���
	m_listAgreeSel.AddItem((double)agree.PositionQty, 0, AGREE_PositionQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ɿ���
	m_listAgreeSel.AddItem((double)agree.CanOpenQty, 0, AGREE_CanOpenQty, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��ͬ�۸�
	m_listAgreeSel.AddItem((double)agree.SpotPrice, 0, AGREE_SpotPrice, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	////��ƽ������
	//if (agree.PositionOffset == SMS_O_OPEN)
	//	m_listAgreeSel.AddItem("����", 0, AGREE_PositionOffset, DT_CENTER, g_ColorRefData.GetColorBlack());
	//else if (agree.PositionOffset == SMS_O_COVER)
	//	m_listAgreeSel.AddItem("ƽ��", 0, AGREE_PositionOffset, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��ͬ״̬ 
	if (agree.AgreementState == SMS_AGREEMENT_ACTIVE)
		m_listAgreeSel.AddItem("��ͬ��Ч", 0, AGREE_AgreementState, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (agree.AgreementState == SMS_AGREEMENT_FINISH)
		m_listAgreeSel.AddItem("��ͬ����", 0, AGREE_AgreementState, DT_CENTER, g_ColorRefData.GetColorBlack());
	//¼������
	m_listAgreeSel.AddItem(agree.SignDate, 0, AGREE_SignDate, DT_CENTER, g_ColorRefData.GetColorBlack());
	////�޸�ʱ��
	//m_listAgreeSel.AddItem(agree.InsertTime, 0, AGREE_InsertTime, DT_CENTER, g_ColorRefData.GetColorBlack());
	////��˾���
	//m_listAgreeSel.AddItem(agree.CompanyNo, 0, AGREE_CompanyNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	////���к�
	//m_listAgreeSel.AddItem((double)agree.SerialId, 0, AGREE_SerialId, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	////����Ա
	//m_listAgreeSel.AddItem(agree.OperatorNo, 0, AGREE_OperatorNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	////�޸�ʱ��
	//m_listAgreeSel.AddItem(agree.OperateTime, 0, AGREE_OperateTime, DT_CENTER, g_ColorRefData.GetColorBlack());
}

void AddAgreementRelWnd::AddAgreementToListAdd(string  agreeno)
{
	m_listAgreeAdd.AddItem(agreeno.c_str(), 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
}

void AddAgreementRelWnd::RefreshTips2()
{
	//ʣ����
	char tips2[256] = { 0 };
	sprintf_s(tips2, "����:%d", m_remain < 0 ? 0 : m_remain);
	m_labTips2.SetText(tips2);
}

LRESULT AddAgreementRelWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
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
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM:
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
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

void AddAgreementRelWnd::DrawClose(TMemDC * pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());
	HPEN hPen = NULL;
	if (m_bInCloseRect)
		hPen = CreatePen(PS_SOLID, 2, RGB(220, 20, 60));
	else
		hPen = CreatePen(PS_SOLID, 2, RGB(225, 225, 225));// RGB(142, 146, 153));
	HPEN hOldPen = (HPEN)SelectObject(pMemDC->GetHdc(), hPen);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.top, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right /*+ 1*/, m_rcClose.bottom /*+ 1*/);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.bottom, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right /*+ 1*/, m_rcClose.top /*- 1*/);

	SelectObject(pMemDC->GetHdc(), hOldPen);
	DeleteObject(hPen);
	RestoreDC(pMemDC->GetHdc(), iold);
}

void AddAgreementRelWnd::OpenLayOut()
{
	int leftstart = 15;
	int topstart = 15;
	int editWidth = 150;	//edit�ؼ����
	int labWidth = 80;		//lab�ؼ����
	int btnWidth = 80;		//btn�ؼ����
	int Height = 25;		//�ؼ��߶�
	int htap = 60;			//������
	int vtap = 15;			//������

	POINT pt;
	pt.x = leftstart;
	pt.y = cCAPTION_H + topstart;
	//�ɹ���ͬ���
	m_labBuyAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editBuyAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��ͬ��ʾ
	pt.x += editWidth + htap;
	m_labTips.MoveWindow(pt.x, pt.y, labWidth * 6, Height);
	//��ʾ2
	pt.x += labWidth * 6;
	m_labTips2.MoveWindow(pt.x, pt.y, labWidth * 2, Height);

	ShowWindow(m_labBuyAgreementNo.GetHwnd(), SW_SHOW);
	ShowWindow(m_editBuyAgreementNo.GetHwnd(), SW_SHOW);
	ShowWindow(m_labSellAgreementNo.GetHwnd(), SW_HIDE);
	ShowWindow(m_editSellAgreementNo.GetHwnd(), SW_HIDE);
}

void AddAgreementRelWnd::CloseLayOut()
{
	int leftstart = 15;
	int topstart = 15;
	int editWidth = 150;	//edit�ؼ����
	int labWidth = 80;		//lab�ؼ����
	int btnWidth = 80;		//btn�ؼ����
	int Height = 25;		//�ؼ��߶�
	int htap = 60;			//������
	int vtap = 15;			//������

	POINT pt;
	pt.x = leftstart;
	pt.y = cCAPTION_H + topstart;
	//���ۺ�ͬ���
	m_labSellAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSellAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��ͬ��ʾ
	pt.x += editWidth + htap;
	m_labTips.MoveWindow(pt.x, pt.y, labWidth * 6, Height);
	//��ʾ2
	pt.x += labWidth * 6;
	m_labTips2.MoveWindow(pt.x, pt.y, labWidth * 2, Height);

	ShowWindow(m_labSellAgreementNo.GetHwnd(), SW_SHOW);
	ShowWindow(m_editSellAgreementNo.GetHwnd(), SW_SHOW);
	ShowWindow(m_labBuyAgreementNo.GetHwnd(), SW_HIDE);
	ShowWindow(m_editBuyAgreementNo.GetHwnd(), SW_HIDE);
}

void AddAgreementRelWnd::OnCreate()
{
	InitCommonControls();
	CreateWidget();
	InitWidget();
	SetWidgetPos();
}

void AddAgreementRelWnd::OnPaint()
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
	DrawText(memdc.GetHdc(), L"�����ͬ��ϵ", -1, &rcCaption, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	DrawClose(&memdc);
	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}

void AddAgreementRelWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	m_rcClose.right = cx - 8;
	m_rcClose.left = m_rcClose.right - ccloseWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + ccloseWidth;
}

void AddAgreementRelWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

	POINT pointM;
	pointM.x = LOWORD(lParam);
	pointM.y = HIWORD(lParam);
	//�رհ�Ť
	if (PtInRect(&m_rcClose, pointM))
	{
		if (!m_bInCloseRect)
			InvalidateRect(m_Hwnd, &m_rcClose, TRUE);
		m_bInCloseRect = true;
	}
	else
	{
		if (m_bInCloseRect)
			InvalidateRect(m_Hwnd, &m_rcClose, TRUE);
		m_bInCloseRect = false;
	}

	InvalidateRect(m_Hwnd, 0, false);
}

void AddAgreementRelWnd::OnMouseLeave()
{
	m_bMouseTrack = false;

}

void AddAgreementRelWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.bottom = rect.top + cCAPTION_H;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		//ShowWindow(m_Hwnd, SW_HIDE);
	}
	else 
	if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}

void AddAgreementRelWnd::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
		m_subSystem->m_agreeWnd->ReloadList();
	}
}

void AddAgreementRelWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddAgreementRelWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
}

void AddAgreementRelWnd::KeyDown(WPARAM wParam, LPARAM lParam)
{
}

void AddAgreementRelWnd::OnNcDestroy()
{
	//delete this;
}

void AddAgreementRelWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_AgreeSel:
		m_listAgreeSel.OnDrawItem(lParam);
		break;
	case ID_ListView_AgreeAdd:
		m_listAgreeAdd.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void AddAgreementRelWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listAgreeAdd.OnMeasureItem(p->itemHeight);
	m_listAgreeSel.OnMeasureItem(p->itemHeight);
}

void AddAgreementRelWnd::CreateWidget()
{
	//�ɹ���ͬ���
	m_labBuyAgreementNo.Create(m_Hwnd, L"�ɹ���ͬ", DT_CENTER, ID_labBuyAgreementNo);
	m_editBuyAgreementNo.Create(m_Hwnd, 0, 0, ID_editBuyAgreementNo);
	//���ۺ�ͬ���
	m_labSellAgreementNo.Create(m_Hwnd, L"���ۺ�ͬ", DT_CENTER, ID_labSellAgreementNo);
	m_editSellAgreementNo.Create(m_Hwnd, 0, 0, ID_editSellAgreementNo);
	//��ʾ
	m_labTips.Create(m_Hwnd, L"", DT_LEFT, ID_labTips);
	//��ʾ2
	m_labTips2.Create(m_Hwnd, L"", DT_LEFT, ID_labTips2);

	//�ѹ����б�
	m_listAgreeAdd.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES, ID_ListView_AgreeAdd);
	//��ѡ���б�
	m_listAgreeSel.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES, ID_ListView_AgreeSel);

	//m_btnDelete.Create(m_Hwnd, "ɾ  ��", ID_btnDelete);
	//m_btnAdd.Create(m_Hwnd, "��  ��", ID_btnAdd);

	m_menuAgreementRel = CreatePopupMenu();

	AppendMenu(m_menuAgreementRel, MF_STRING, IDM_deleteAgreeRel, L"ɾ  ��");
}

void AddAgreementRelWnd::InitWidget()
{
	m_labBuyAgreementNo.SetFont(g_FontData.GetFontWord15());
	m_labSellAgreementNo.SetFont(g_FontData.GetFontWord15());
	m_labTips.SetFont(g_FontData.GetFontWord15());
	m_labTips2.SetFont(g_FontData.GetFontWord15());

	m_editBuyAgreementNo.SetFont(g_FontData.GetFontNum15());
	m_editSellAgreementNo.SetFont(g_FontData.GetFontNum15());

	EnableWindow(m_editBuyAgreementNo.GetHwnd(), false);
	EnableWindow(m_editSellAgreementNo.GetHwnd(), false);

	m_listAgreeAdd.AddColumn(L"��ͬ���", 300);

	//============================�����б�=============================
	for (int i = AGREE_UserNo; i < AGREE_InsertTime; i++)
	{
		switch (i)
		{
		case AGREE_SerialId:
			m_listAgreeSel.AddColumn(L"���к�", 80, AGREE_SerialId);
			break;
		case AGREE_CompanyNo:
			m_listAgreeSel.AddColumn(L"��˾���", 80, AGREE_CompanyNo);
			break;
		case AGREE_AgreementNo:
			m_listAgreeSel.AddColumn(L"��ͬ���", 100, AGREE_AgreementNo);
			break;
		case AGREE_UserNo:
			m_listAgreeSel.AddColumn(L"�û����", 80, AGREE_UserNo);
			break;
		case AGREE_AgreementType:
			m_listAgreeSel.AddColumn(L"��ͬ����", 80, AGREE_AgreementType);
			break;
		case AGREE_SpotQty:
			m_listAgreeSel.AddColumn(L"�ֻ�����", 80, AGREE_SpotQty);
			break;
		case AGREE_SpotPrice:
			m_listAgreeSel.AddColumn(L"��ͬ�۸�", 80, AGREE_SpotPrice);
			break;
		case AGREE_SignDate:
			m_listAgreeSel.AddColumn(L"��ͬ����", 80, AGREE_SignDate);
			break;
		case AGREE_InsertTime:
			m_listAgreeSel.AddColumn(L"¼��ʱ��", 130, AGREE_InsertTime);
			break;
		case AGREE_SpecificationNo:
			m_listAgreeSel.AddColumn(L"�������", 80, AGREE_SpecificationNo);
			break;
		//case AGREE_PositionOffset:
		//	m_listAgreeSel.AddColumn(L"��ƽ������", 80, AGREE_PositionOffset);
		//	break;
		case AGREE_AgreementState:
			m_listAgreeSel.AddColumn(L"��ͬ״̬", 80, AGREE_AgreementState);
			break;
		case AGREE_OperatorNo:
			m_listAgreeSel.AddColumn(L"����Ա", 80, AGREE_OperatorNo);
			break;
		case AGREE_OperateTime:
			m_listAgreeSel.AddColumn(L"�޸�ʱ��", 130, AGREE_OperateTime);
			break;
		case AGREE_SpotRemainQty:
			m_listAgreeSel.AddColumn(L"������", 80, AGREE_SpotRemainQty);
			break;
		case AGREE_PositionQty:
			m_listAgreeSel.AddColumn(L"�ֲ���", 80, AGREE_PositionQty);
			break;
		case AGREE_CanOpenQty:
			m_listAgreeSel.AddColumn(L"�ɿ���", 80, AGREE_CanOpenQty);
			break;
		default:
			break;
		}
	}
}

void AddAgreementRelWnd::SetWidgetPos()
{
	int leftstart = 15;
	int topstart = 15;
	int editWidth = 150;	//edit�ؼ����
	int labWidth = 80;		//lab�ؼ����
	int btnWidth = 80;		//btn�ؼ����
	int Height = 25;		//�ؼ��߶�
	int htap = 60;			//������
	int vtap = 15;			//������
	m_hInstance = GetModuleHandle(NULL);
	POINT pt;
	pt.x = leftstart;
	pt.y = cCAPTION_H + topstart;
	//���ֺ�ͬ���
	m_labBuyAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editBuyAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//ƽ�ֺ�ͬ���
	pt.x += editWidth + htap;
	m_labSellAgreementNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSellAgreementNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//��ʾ
	pt.x += editWidth + htap;
	m_labTips.MoveWindow(pt.x, pt.y, labWidth * 6, Height);
	//��ʾ2
	pt.x += labWidth * 6 ;
	m_labTips2.MoveWindow(pt.x, pt.y, labWidth * 2, Height);
	//ɾ��
	//pt.x += editWidth + htap;
	//m_btnDelete.MoveWindow(pt.x, pt.y, btnWidth, Height);
	//����
	//pt.x += editWidth + htap;
	//m_btnAdd.MoveWindow(pt.x, pt.y, btnWidth, Height);
	//��ѡ���б�
	pt.x = 1;
	pt.y += Height + vtap;
	m_listAgreeSel.MoveWindow(pt.x, pt.y, 720, (Height+ vtap) * 10);
	//�ѹ�����ͬ
	pt.x = 730;
	m_listAgreeAdd.MoveWindow(pt.x, pt.y, 300, (Height + vtap) * 10);

}

LRESULT AddAgreementRelWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	int i = GetWindowLong((HWND)lParam, GWL_ID);
	if (i == ID_labTips2)
		SetTextColor((HDC)wParam, g_ColorRefData.GetColorTextRed());
	return (LRESULT)g_ColorRefData.GetHbrWhite();
}

void AddAgreementRelWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//�����ǲ˵�
	{
		switch (LOWORD(wParam))
		{
		case IDM_deleteAgreeRel://ɾ��
			OnBtnDelete();
			break;
		default:
			break;
		}
	}
}

void AddAgreementRelWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_AgreeSel:
		DealAgreeSelList(lParam);
		break;
	case ID_ListView_AgreeAdd:
		DealAgreeAddList(lParam);
		break;
	default:
		break;
	}
}

void AddAgreementRelWnd::OnBtnAdd()
{
	SmsAgreementRelAddReq addReq;
	memset(&addReq, 0, sizeof(SmsAgreementRelAddReq));
	char temp[MAX_PATH] = { 0 };
	//���к�
	addReq.SerialId = 0;
	//��˾
	strcpy_s(addReq.CompanyNo, m_subSystem->m_curLogin.CompanyNo);
	//�ɹ���ͬ���
	memset(&temp, 0, sizeof(temp));
	m_editBuyAgreementNo.GetText(temp, sizeof(temp));
	strncpy_s(addReq.BuyAgreementNo, temp);
	//���ۺ�ͬ���
	memset(&temp, 0, sizeof(temp));
	m_editSellAgreementNo.GetText(temp, sizeof(temp));
	strncpy_s(addReq.SellAgreementNo, temp);
	//�û����
	map<string, SmsAgreementDataRsp>::iterator it = m_mapUnRelAgree.find(addReq.SellAgreementNo);
	if(it != m_mapUnRelAgree.end())
		strncpy_s(addReq.UserNo, it->second.UserNo);
	else
		strncpy_s(addReq.UserNo, m_subSystem->m_curLogin.UserNo);
	//����Ա
	strcpy_s(addReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

	g_pSpotSystem->AddAgreementRel(&addReq);
}

void AddAgreementRelWnd::OnBtnDelete()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ȷ��ɾ���˺�ͬ��ϵô��", TMB_OKCANCEL))
		return;
	if (!m_subSystem)return;

	SmsAgreementRelDelReq delReq;
	memset(&delReq, 0, sizeof(SmsAgreementRelDelReq));
	char temp[MAX_PATH] = { 0 };
	//��˾
	strncpy_s(delReq.CompanyNo, m_subSystem->m_curLogin.CompanyNo);

	if (m_bisBuy)
	{
		//�ɹ���ͬ���
		memset(&temp, 0, sizeof(temp));
		m_editBuyAgreementNo.GetText(temp, sizeof(temp));
		strncpy_s(delReq.BuyAgreementNo, temp);
		//���ۺ�ͬ���
		memset(&temp, 0, sizeof(temp));
		int index = m_listAgreeAdd.GetSelectItem();
		m_listAgreeAdd.GetSubItem(index, 0, temp, sizeof(temp));
		strncpy_s(delReq.SellAgreementNo, temp);
	}
	else
	{
		//���ۺ�ͬ���
		memset(&temp, 0, sizeof(temp));
		m_editSellAgreementNo.GetText(temp, sizeof(temp));
		strncpy_s(delReq.SellAgreementNo, temp);
		//�ɹ���ͬ���
		memset(&temp, 0, sizeof(temp));
		int index = m_listAgreeAdd.GetSelectItem();
		m_listAgreeAdd.GetSubItem(index, 0, temp, sizeof(temp));
		strncpy_s(delReq.BuyAgreementNo, temp);
	}

	//����Ա
	strcpy_s(delReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

	g_pSpotSystem->DeleteAgreementRel(&delReq);
}

void AddAgreementRelWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	//switch (lParam)
	//{
	////case ID_btnAdd:
	////	OnBtnAdd();
	////	break;
	////case ID_btnDelete:
	////	OnBtnDelete();
	////	break;
	//default:
	//	break;
	//}

}

void AddAgreementRelWnd::DealAgreeSelList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_CLICK)
	{
		int index = m_listAgreeSel.GetSelectItem();
		SmsAgreementNoType tempAgree = {0};
		m_listAgreeSel.GetSubItem(index, AGREE_AgreementNo, tempAgree, sizeof(SmsAgreementNoType));

		if (m_bisBuy)
			m_editSellAgreementNo.SetText(tempAgree);
		else
			m_editBuyAgreementNo.SetText(tempAgree);
	}
	else if (p->hdr.code == NM_DBLCLK)
	{
		OnBtnAdd();
	}
}

void AddAgreementRelWnd::DealAgreeAddList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenuItem(m_menuAgreementRel, IDM_deleteAgreeRel, MF_ENABLED);
		if (p->iItem >= m_listAgreeAdd.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuAgreementRel, IDM_deleteAgreeRel, MF_GRAYED);
		}

		ShowMenu(m_menuAgreementRel);
	}
}

void AddAgreementRelWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}
