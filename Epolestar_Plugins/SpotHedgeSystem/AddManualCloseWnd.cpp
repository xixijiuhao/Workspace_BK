#include "PreInclude.h"

#define cCAPTION_H  30
#define cFRAME_H    865
#define cFRAME_V    455+1
#define  ccloseWidth 13

extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
AddManualCloseWnd::AddManualCloseWnd()
{
	m_bMouseTrack = false;

	m_rcClose.right = cFRAME_H - 8;
	m_rcClose.left = m_rcClose.right - ccloseWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + ccloseWidth;
}

//AddManualCloseWnd::~AddManualCloseWnd()
//{
//	
//}

void AddManualCloseWnd::ShowFrm(HWND hParent)
{
	CreateFrm(L"TAddManualCloseWnd", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_H, cFRAME_V, SWP_SHOWWINDOW);
}

void AddManualCloseWnd::Register(SystemFrame * pdata)
{
	if (!pdata)return;
	m_subSystem = pdata;
}

void AddManualCloseWnd::InitWnd(SmsMatchDataRsp closeMatch, string closeAgreeNo, vector<SmsMatchDataRsp>& unrel, vector<SmsManualCloseDataRsp>& rel)
{
	//��ʼ��
	m_editCloseMatchNo.SetText("");
	m_editCloseAgreeNo.SetText("");
	m_editOpenMatchNo.SetText("");
	m_editCloseQty.SetText("");

	m_listAddMatch.DeleteAllItems();
	m_listSelMatch.DeleteAllItems();

	//������
	m_editCloseMatchNo.SetText(closeMatch.MatchNo);
	m_editCloseAgreeNo.SetText(closeAgreeNo.c_str());

	char	tip[MAX_PATH] = { 0 };
	string sCode = closeMatch.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	sprintf_s(tip, "%s   %s   %s   %.2f   %d   %.2f", tCode.c_str(), closeMatch.Direct == SMS_DIRECT_BUY ? "��" : "��", closeMatch.Offset == SMS_O_OPEN ? "����" : "ƽ��",
		closeMatch.MatchPrice, closeMatch.MatchQty, closeMatch.MatchFee);
	m_labCloseMatchTip.SetText(tip);

	for (auto it : unrel)
	{
		AddToSelList(it);
	}

	for (auto it : rel)
	{
		AddToAddList(it);
	}
}

void AddManualCloseWnd::AddToSelList(SmsMatchDataRsp & match)
{
	//����Ա���
	m_listSelMatch.AddItem(match.UserNo, 0, MATCH_UserNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��Լ���
	string sCode = match.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	m_listSelMatch.AddItem(tCode.c_str(), 0, MATCH_ContractNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��������
	if (match.Direct == SMS_DIRECT_BUY)
		m_listSelMatch.AddItem("��", 0, MATCH_Direct, DT_LEFT, g_ColorRefData.GetColorBuyRed());
	else if (match.Direct == SMS_DIRECT_SELL)
		m_listSelMatch.AddItem("��", 0, MATCH_Direct, DT_RIGHT, g_ColorRefData.GetColorSellBlue());
	//��ƽ
	if (match.Offset == SMS_O_OPEN)
		m_listSelMatch.AddItem("����", 0, MATCH_Offset, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	else if (match.Offset == SMS_O_COVER)
		m_listSelMatch.AddItem("ƽ��", 0, MATCH_Offset, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	//�ɽ��۸�
	m_listSelMatch.AddItem((double)match.MatchPrice, 0, MATCH_MatchPrice, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ɽ�����
	m_listSelMatch.AddItem((double)match.MatchQty, 0, MATCH_MatchQty, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//������
	m_listSelMatch.AddItem((double)match.MatchFee, 0, MATCH_MatchFee, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ɽ�ʱ��
	m_listSelMatch.AddItem(match.MatchTime, 0, MATCH_MatchTime, DT_CENTER, g_ColorRefData.GetColorBlack());
	//Ͷ������
	if (match.Hedge == SMS_HEDGE_SPECULATE)
		m_listSelMatch.AddItem("Ͷ��", 0, MATCH_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (match.Hedge == SMS_HEDGE_HEDGE)
		m_listSelMatch.AddItem("�ױ�", 0, MATCH_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	else
		m_listSelMatch.AddItem("��", 0, MATCH_Hedge, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��������
	m_listSelMatch.AddItem(match.SettleDate, 0, MATCH_SettleDate, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��˾���
	m_listSelMatch.AddItem(match.CompanyNo, 0, MATCH_CompanyNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//���к�
	m_listSelMatch.AddItem((double)match.SerialId, 0, MATCH_SerialId, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ɽ����
	m_listSelMatch.AddItem(match.MatchNo, 0, MATCH_MatchNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//ϵͳ��
	m_listSelMatch.AddItem(match.SystemNo, 0, MATCH_SystemNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//ί�б��
	m_listSelMatch.AddItem(match.OrderNo, 0, MATCH_OrderNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//���ر��
	m_listSelMatch.AddItem(match.LocalNo, 0, MATCH_LocalNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����Ա
	m_listSelMatch.AddItem(match.OperatorNo, 0, MATCH_OperatorNo, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�޸�ʱ��
	m_listSelMatch.AddItem(match.OperateTime, 0, MATCH_OperateTime, DT_CENTER, g_ColorRefData.GetColorBlack());
}

void AddManualCloseWnd::AddToAddList(SmsManualCloseDataRsp & data)
{
	//ָ�����ֳɽ���
	m_listAddMatch.AddItem(data.OpenNo, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����
	m_listAddMatch.AddItem((double)data.CloseQty, 0, 1, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT AddManualCloseWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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



void AddManualCloseWnd::OnCreate()
{
	InitCommonControls();
	CreateWidget();
	InitWidget();
	SetWidgetPos();
}

void AddManualCloseWnd::OnPaint()
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
	DrawText(memdc.GetHdc(), L"����ָ��ƽ��", -1, &rcCaption, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	DrawClose(&memdc);
	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}

void AddManualCloseWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	m_rcClose.right = cx - 8;
	m_rcClose.left = m_rcClose.right - ccloseWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + ccloseWidth;
}

void AddManualCloseWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

void AddManualCloseWnd::OnMouseLeave()
{
	m_bMouseTrack = false;

}

void AddManualCloseWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddManualCloseWnd::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
	}
}

void AddManualCloseWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
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

void AddManualCloseWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
}

void AddManualCloseWnd::KeyDown(WPARAM wParam, LPARAM lParam)
{
}

void AddManualCloseWnd::OnNcDestroy()
{
	//delete this;
}

void AddManualCloseWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_AddMatch:
		m_listAddMatch.OnDrawItem(lParam);
		break;
	case ID_ListView_SelMatch:
		m_listSelMatch.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void AddManualCloseWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listAddMatch.OnMeasureItem(p->itemHeight);
}

void AddManualCloseWnd::CreateWidget()
{
	//ƽ�ֳɽ���
	m_labCloseMatchNo.Create(m_Hwnd, L"ƽ�ֳɽ���", DT_CENTER, ID_labCloseMatchNo);
	m_editCloseMatchNo.Create(m_Hwnd, 0, 0, ID_editCloseMatchNo);
	//ƽ�ֺ�ͬ��
	m_labCloseAgreeNo.Create(m_Hwnd, L"ƽ�ֺ�ͬ��", DT_CENTER, ID_labCloseAgreeNo);
	m_editCloseAgreeNo.Create(m_Hwnd, 0, 0, ID_editCloseAgreeNo);
	//�б�
	m_listAddMatch.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_AddMatch);
	//��ʾ
	m_labCloseMatchTip.Create(m_Hwnd, L"", DT_LEFT, ID_labCloseMatchTip);
	//���ֳɽ���
	m_labOpenMatchNo.Create(m_Hwnd, L"���ֳɽ���", DT_CENTER, ID_labOpenMatchNo);
	m_editOpenMatchNo.Create(m_Hwnd, 0, 0, ID_editOpenMatchNo);
	//ƽ������
	m_labCloseQty.Create(m_Hwnd, L"ƽ������", DT_CENTER, ID_labCloseQty);
	m_editCloseQty.Create(m_Hwnd, 0, 0, ID_editCloseQty);
	//�б�
	m_listSelMatch.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_SelMatch);

	//m_btnDelete.Create(m_Hwnd, "ɾ  ��", ID_btnDelete);
	m_btnAdd.Create(m_Hwnd, "��  ��", ID_btnAdd);
}

void AddManualCloseWnd::InitWidget()
{
	m_labCloseMatchNo.SetFont(g_FontData.GetFontWord15());
	m_labCloseAgreeNo.SetFont(g_FontData.GetFontWord15());
	m_labOpenMatchNo.SetFont(g_FontData.GetFontWord15());
	m_labCloseQty.SetFont(g_FontData.GetFontWord15());
	m_labCloseMatchTip.SetFont(g_FontData.GetFontNumEx15());

	m_editCloseMatchNo.SetFont(g_FontData.GetFontNum15());
	m_editCloseAgreeNo.SetFont(g_FontData.GetFontNum15());
	m_editOpenMatchNo.SetFont(g_FontData.GetFontNum15());
	m_editCloseQty.SetFont(g_FontData.GetFontNum15());

	EnableWindow(m_labCloseMatchNo.GetHwnd(), false);
	EnableWindow(m_labCloseAgreeNo.GetHwnd(), false);
	EnableWindow(m_labOpenMatchNo.GetHwnd(), false);
	EnableWindow(m_editCloseMatchNo.GetHwnd(), false);
	EnableWindow(m_editCloseAgreeNo.GetHwnd(), false);
	EnableWindow(m_editOpenMatchNo.GetHwnd(), false);


	m_listAddMatch.AddColumn(L"ָ�����ֳɽ���", 180);
	m_listAddMatch.AddColumn(L"ƽ������", 80);

	m_menuDelete = CreatePopupMenu();

	AppendMenu(m_menuDelete, MF_STRING, IDM_delAddMatch, L"ɾ  ��");

	//============================�����б�=============================
	for (int i = MATCH_UserNo; i < MATCH_OperateTime; i++)
	{
		switch (i)
		{
		case MATCH_SerialId:
			m_listSelMatch.AddColumn(L"���к�", 80, MATCH_SerialId);
			break;
		case MATCH_CompanyNo:
			m_listSelMatch.AddColumn(L"��˾���", 80, MATCH_CompanyNo);
			break;
		case MATCH_UserNo:
			m_listSelMatch.AddColumn(L"�˺�", 80, MATCH_UserNo);
			break;
		case MATCH_SettleDate:
			m_listSelMatch.AddColumn(L"��������", 80, MATCH_SettleDate);
			break;
		case MATCH_ContractNo:
			m_listSelMatch.AddColumn(L"��Լ���", 130, MATCH_ContractNo);
			break;
		case MATCH_MatchNo:
			m_listSelMatch.AddColumn(L"�ɽ����", 80, MATCH_MatchNo);
			break;
		case MATCH_SystemNo:
			m_listSelMatch.AddColumn(L"ϵͳ��", 80, MATCH_SystemNo);
			break;
		case MATCH_OrderNo:
			m_listSelMatch.AddColumn(L"ί�б��", 80, MATCH_OrderNo);
			break;
		case MATCH_LocalNo:
			m_listSelMatch.AddColumn(L"���ر��", 80, MATCH_LocalNo);
			break;
		case MATCH_Direct:
			m_listSelMatch.AddColumn(L"����", 40, MATCH_Direct);
			break;
		case MATCH_Offset:
			m_listSelMatch.AddColumn(L"��ƽ", 80, MATCH_Offset);
			break;
		case MATCH_Hedge:
			m_listSelMatch.AddColumn(L"Ͷ������", 80, MATCH_Hedge);
			break;
		case MATCH_MatchPrice:
			m_listSelMatch.AddColumn(L"�ɽ��۸�", 80, MATCH_MatchPrice);
			break;
		case MATCH_MatchQty:
			m_listSelMatch.AddColumn(L"�ɽ�����", 80, MATCH_MatchQty);
			break;
		case MATCH_MatchFee:
			m_listSelMatch.AddColumn(L"������", 80, MATCH_MatchFee);
			break;
		case MATCH_MatchTime:
			m_listSelMatch.AddColumn(L"�ɽ�ʱ��", 130, MATCH_MatchTime);
			break;
		case MATCH_OperatorNo:
			m_listSelMatch.AddColumn(L"����Ա", 80, MATCH_OperatorNo);
			break;
		case MATCH_OperateTime:
			m_listSelMatch.AddColumn(L"�޸�ʱ��", 130, MATCH_OperateTime);
			break;
		default:
			break;
		}
	}
}

void AddManualCloseWnd::SetWidgetPos()
{
	int leftstart = 15;
	int topstart = 15;
	int editWidth = 170;	//edit�ؼ����
	int labWidth = 90;		//lab�ؼ����
	int btnWidth = 80;		//btn�ؼ����
	int Height = 25;		//�ؼ��߶�
	int htap = 10;			//������
	int vtap = 15;			//������
	m_hInstance = GetModuleHandle(NULL);
	POINT pt;
	pt.x = leftstart;
	pt.y = cCAPTION_H + topstart;
	//ƽ�ֳɽ���
	m_labCloseMatchNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCloseMatchNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//ƽ�ֺ�ͬ��
	pt.x += editWidth + htap;
	m_labCloseAgreeNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCloseAgreeNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//�б�
	pt.x += editWidth + htap*3;
	m_listAddMatch.MoveWindow(pt.x, pt.y, 260, (Height + vtap) * 3 + Height);

	//ɾ��
	pt.x = leftstart + labWidth + htap;
	pt.y += Height + vtap;
	m_labCloseMatchTip.MoveWindow(pt.x, pt.y, editWidth + htap + labWidth + htap + editWidth, Height);

	//���ֳɽ���
	pt.x = leftstart;
	pt.y += Height + vtap;
	m_labOpenMatchNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editOpenMatchNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	//ƽ������
	pt.x += editWidth + htap;
	m_labCloseQty.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCloseQty.MoveWindow(pt.x, pt.y, editWidth, Height);

	//����
	pt.x = leftstart + labWidth + htap + editWidth + htap + labWidth + htap + editWidth - btnWidth;
	pt.y += Height + vtap;
	m_btnAdd.MoveWindow(pt.x, pt.y, btnWidth, Height);

	//�б�
	pt.x = 1;
	pt.y += Height + vtap;
	m_listSelMatch.MoveWindow(pt.x, pt.y, cFRAME_H - 2, Height * 10);
}

void AddManualCloseWnd::DrawClose(TMemDC * pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());
	HPEN hPen = NULL;
	RECT rc = m_rcClose;
	rc.left -= 2;
	rc.right += 3;
	rc.top -= 2;
	rc.bottom += 3;

	if (m_bInCloseRect)
		FillRect(pMemDC->GetHdc(), &rc, g_ColorRefData.GetHbrLightRed());
	else
		FillRect(pMemDC->GetHdc(), &rc, g_ColorRefData.GetHbrRGB100());
	//if (m_bInCloseRect)
	//	hPen = CreatePen(PS_SOLID, 2, RGB(220, 20, 60));
	//else
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

LRESULT AddManualCloseWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetHbrWhite();
}

void AddManualCloseWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//�����ǲ˵�
	{
		switch (LOWORD(wParam))
		{
		case IDM_delAddMatch://ɾ��
			OnBtnDel();
			break;
		default:
			break;
		}
	}
}

void AddManualCloseWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_AddMatch:
		DealAddMatchList(lParam);
		break;
	case ID_ListView_SelMatch:
		DealSelMatchList(lParam);
		break;
	default:
		break;
	}
}

//void AddManualCloseWnd::OnBtnOk()
//{
//	OnBtnCancel();
//}
//
//void AddManualCloseWnd::OnBtnCancel()
//{
//	if (IsWindowVisible(m_Hwnd))
//		ShowWindow(m_Hwnd, SW_HIDE);
//	else
//		ShowWindow(m_Hwnd, SW_SHOW);
//}

void AddManualCloseWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_btnAdd:
		OnBtnAdd();
		break;
	//case ID_btnDelete:
	//	OnBtnDel();
	//	break;
	default:
		break;
	}

}

void AddManualCloseWnd::DealAddMatchList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenuItem(m_menuDelete, IDM_delAddMatch, MF_ENABLED);
		if (p->iItem >= m_listAddMatch.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuDelete, IDM_delAddMatch, MF_GRAYED);
		}

		ShowMenu(m_menuDelete);
	}
}

void AddManualCloseWnd::DealSelMatchList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_CLICK)
	{
		int index = m_listSelMatch.GetSelectItem();
		SmsPositionNoType tempMatch = { 0 };
		m_listSelMatch.GetSubItem(index, MATCH_MatchNo, tempMatch, sizeof(SmsPositionNoType));

		m_editOpenMatchNo.SetText(tempMatch);
	}
}

void AddManualCloseWnd::OnBtnAdd()
{
	if (!m_subSystem)return;

	SmsManualCloseAddReq addReq;
	memset(&addReq, 0, sizeof(SmsManualCloseAddReq));
	char temp[MAX_PATH] = { 0 };

	//���к�
	addReq.SerialId = 0;
	//��˾���
	strncpy_s(addReq.CompanyNo, m_subSystem->m_curLogin.CompanyNo);
	//ƽ�ֺ�ͬ���
	memset(&temp, 0, sizeof(temp));
	m_editCloseAgreeNo.GetText(temp, sizeof(temp));
	strncpy_s(addReq.CloseAgreementNo, temp);
	//����Ա���
	strncpy_s(addReq.UserNo, m_subSystem->m_curLogin.UserNo);
	//ƽ�ֱ��
	memset(&temp, 0, sizeof(temp));
	m_editCloseMatchNo.GetText(temp, sizeof(temp));
	strncpy_s(addReq.CloseNo, temp);
	//���ֱ��
	memset(&temp, 0, sizeof(temp));
	m_editOpenMatchNo.GetText(temp, sizeof(temp));
	strncpy_s(addReq.OpenNo, temp);
	//ƽ������
	addReq.CloseQty =  (int)m_editCloseQty.GetDoubleData();
	//����Ա
	strcpy_s(addReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

	g_pSpotSystem->AddManualClose(&addReq);
}

void AddManualCloseWnd::OnBtnDel()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ȷ��ɾ����ָ��ƽ�֣�", TMB_OKCANCEL))
		return;
	if (!m_subSystem)return;

	SmsManualCloseDelReq delReq;
	memset(&delReq, 0, sizeof(SmsManualCloseDelReq));
	char temp[MAX_PATH] = { 0 };
	//��˾
	strncpy_s(delReq.CompanyNo, m_subSystem->m_curLogin.CompanyNo);
	//����Ա���
	strncpy_s(delReq.UserNo, m_subSystem->m_curLogin.UserNo);
	//ƽ�ֺ�ͬ���
	memset(&temp, 0, sizeof(temp));
	m_editCloseAgreeNo.GetText(temp, sizeof(temp));
	strncpy_s(delReq.CloseAgreementNo, temp);
	//ƽ�ֱ��
	memset(&temp, 0, sizeof(temp));
	m_editCloseMatchNo.GetText(temp, sizeof(temp));
	strncpy_s(delReq.CloseNo, temp);
	//���ֱ��
	memset(&temp, 0, sizeof(temp));
	int index = m_listAddMatch.GetSelectItem();
	m_listAddMatch.GetSubItem(index, 0, temp, sizeof(temp));
	strncpy_s(delReq.OpenNo, temp);
	//����Ա
	strcpy_s(delReq.OperatorNo, m_subSystem->m_curLogin.UserNo);

	g_pSpotSystem->DeleteManualClose(&delReq);

}

void AddManualCloseWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

