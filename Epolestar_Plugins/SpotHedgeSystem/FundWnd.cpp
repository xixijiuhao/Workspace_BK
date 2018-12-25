#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
#define cCAPTION_H			45
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����

FundWnd::FundWnd()
	:m_System(nullptr)
{
}

FundWnd::~FundWnd()
{
}

void FundWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void FundWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void FundWnd::AddFundToList(SmsFundDataRsp & order, int index)
{
	int count = 0;
	if (index == -1)
		count = m_listFund.GetItemCount();
	else
		count = index;

	//���к�
	m_listFund.AddItem((double)order.SerialId, count, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��������
	m_listFund.AddItem(order.SettleDate, count, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��˾���
	m_listFund.AddItem(order.CompanyNo, count, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����Ա���
	m_listFund.AddItem(order.UserNo, count, 3, DT_CENTER, g_ColorRefData.GetColorBlack());
	//Ȩ��
	m_listFund.AddItem((double)order.Equity, count, 4, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��Ȩ��
	m_listFund.AddItem((double)order.PreEquity, count, 5, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����
	m_listFund.AddItem((double)order.Available, count, 6, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�����
	m_listFund.AddItem((double)order.PreAvailable, count, 7, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��֤��
	m_listFund.AddItem((double)order.Margin, count, 8, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//������
	m_listFund.AddItem((double)order.Fee, count, 9, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ֲ�����
	m_listFund.AddItem((double)order.PositionProfit, count, 10, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//ƽ������
	m_listFund.AddItem((double)order.CloseProfit, count, 11, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��������
	m_listFund.AddItem((double)order.DeliveryProfit, count, 12, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//���
	m_listFund.AddItem((double)order.CashIn, count, 13, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����
	m_listFund.AddItem((double)order.CashOut, count, 14, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����Ա
	m_listFund.AddItem(order.OperatorNo, count, 15, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�޸�ʱ��
	m_listFund.AddItem(order.OperateTime, count, 16, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT FundWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_FUND_REFRESH:
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

void FundWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	///////////////////////////////Create//////////////////////////////////
	//��ʼ����
	m_labBeginDate.Create(m_Hwnd, L"��ʼ����", DT_RIGHT, ID_LabBeginDate);
	m_beginTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtBeginDate, m_hInstance, NULL);
	//��������
	m_labEndDate.Create(m_Hwnd, L"��������", DT_RIGHT, ID_LabEndDate);
	m_endTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtEndDate, m_hInstance, NULL);
	//�˺�
	//m_labUserno.Create(m_Hwnd, L"�˺�", DT_CENTER, ID_LabUserno);
	//m_cbUserno.Create(m_Hwnd, ID_CbUserno);
	//��ѯ
	m_btnQry.Create(m_Hwnd, "��  ѯ", ID_BtnQry);
	//���
	m_listFund.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Fund);
	//RECT rect;
	//GetClientRect(m_Hwnd, &rect);
	//m_listFund.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	//////////////////////////////Init////////////////////////////////////
	m_listFund.AddColumn(L"���к�", 80, -1);
	m_listFund.AddColumn(L"��������", 80, -1);
	m_listFund.AddColumn(L"��˾���", 80, -1);
	m_listFund.AddColumn(L"�˺�", 80, -1);
	m_listFund.AddColumn(L"Ȩ��", 80, -1);
	m_listFund.AddColumn(L"��Ȩ��", 80, -1);
	m_listFund.AddColumn(L"����", 80, -1);
	m_listFund.AddColumn(L"�����", 80, -1);
	m_listFund.AddColumn(L"��֤��", 80, -1);
	m_listFund.AddColumn(L"������", 80, -1);
	m_listFund.AddColumn(L"�ֲ�����", 80, -1);
	m_listFund.AddColumn(L"ƽ������", 80, -1);
	m_listFund.AddColumn(L"��������", 80, -1);
	m_listFund.AddColumn(L"���", 80, -1);
	m_listFund.AddColumn(L"����", 80, -1);
	m_listFund.AddColumn(L"����Ա", 80, -1);
	m_listFund.AddColumn(L"�޸�ʱ��", 130, -1);

	m_menuFund = CreatePopupMenu();

	AppendMenu(m_menuFund, MF_STRING, IDM_AddFund, L"¼���ʽ�");
	AppendMenu(m_menuFund, MF_SEPARATOR, IDM_Line, L"");
	AppendMenu(m_menuFund, MF_STRING, IDM_ModFund, L"�޸��ʽ�");
	AppendMenu(m_menuFund, MF_STRING, IDM_DelFund, L"ɾ���ʽ�");

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
	int editWidth = 120;	//edit�ؼ����
	int labWidth = 60;		//lab�ؼ����
	int btnWidth = 80;		//btn�ؼ����
	int Height = 25;		//�ؼ��߶�
	int htap = 10;			//������

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
	m_listFund.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top);
}

void FundWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void FundWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//�����ǲ˵�
	{
		switch (LOWORD(wParam))
		{
		case IDM_AddFund:	//¼���ʽ�
			if (!IsWindow(m_addFundWnd.GetHwnd()))
			{
				m_addFundWnd.ShowFrm(m_Hwnd);
				m_addFundWnd.Register(m_System);
			}
			else
			{
				if (!IsWindowVisible(m_addFundWnd.GetHwnd()))
					ShowWindow(m_addFundWnd.GetHwnd(), SW_SHOW);
			}
			m_addFundWnd.InitWnd();
			break;
		case IDM_ModFund://�޸��ʽ�
		{
			if (!IsWindow(m_addFundWnd.GetHwnd()))
			{
				m_addFundWnd.ShowFrm(m_Hwnd);
				m_addFundWnd.Register(m_System);
			}
			else
			{
				if (!IsWindowVisible(m_addFundWnd.GetHwnd()))
					ShowWindow(m_addFundWnd.GetHwnd(), SW_SHOW);
			}
			char	userno[MAX_PATH] = { 0 };
			char	settleDate[MAX_PATH] = { 0 };
			int index = m_listFund.GetSelectItem();
			m_listFund.GetSubItem(index, 3, userno, sizeof(userno));
			m_listFund.GetSubItem(index, 1, settleDate, sizeof(settleDate));
			for (auto it : m_System->m_vecFund)
			{
				if (strcmp(it.UserNo,userno) == 0 && strcmp(it.SettleDate,settleDate) == 0)
				{
					m_addFundWnd.InitModifyWnd(it);
					break;
				}
			}
		}
			break;
		case IDM_DelFund://ɾ���ʽ�
			DealDelFund();
			break;
		default:
			break;
		}
	}
}

void FundWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Fund:
		m_listFund.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void FundWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Fund:
		DealFundList(lParam);
		break;
	default:
		break;
	}
}

LRESULT FundWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void FundWnd::OnNcDestroy()
{

}

void FundWnd::OnDestroy()
{
}

void FundWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listFund.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void FundWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listFund.OnMeasureItem(p->itemHeight);
}

void FundWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
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

void FundWnd::DealFundList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listFund.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuFund, IDM_ModFund, MF_GRAYED);
			EnableMenuItem(m_menuFund, IDM_DelFund, MF_GRAYED);
		}

		ShowMenu(m_menuFund);
	}
}

void FundWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void FundWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_AddFund; i <= IDM_DelFund; i++)
		EnableMenuItem(m_menuFund, i, flage);
}

void FundWnd::OnBtnQry()
{
	SmsFundQryReq qryReq;
	memset(&qryReq, 0, sizeof(SmsFundQryReq));
	char temp[MAX_PATH] = { 0 };
	//���к�
	qryReq.SerialId = 0;
	//��˾
	strcpy_s(qryReq.CompanyNo, m_System->m_curLogin.CompanyNo);

	//�����
	//wchar_t userno[MAX_PATH] = { 0 };
	//m_cbUserno.GetText(userno);
	strcpy_s(qryReq.UserNo, m_System->m_curLogin.UserNo);
	//��ʼ����
	wstring tempDate1;
	m_cbtBeginDate.CxGetWindowText(tempDate1);
	strcpy_s(qryReq.BeginDate, LoadRC::unicode_to_ansi(tempDate1).c_str());
	//��ֹ����
	wstring tempDate2;
	m_cbtEndDate.CxGetWindowText(tempDate2);
	strcpy_s(qryReq.EndDate, LoadRC::unicode_to_ansi(tempDate2).c_str());

	m_listFund.DeleteAllItems();

	m_System->m_vecFund.clear();
	g_pSpotSystem->QryFund(&qryReq);
}

void FundWnd::DealDelFund()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ȷ��ɾ�����ʽ�ô��", TMB_OKCANCEL))
		return;
	if (!m_System)return;

	SmsFundDelRsp delReq;
	memset(&delReq, 0, sizeof(delReq));

	char	temp[MAX_PATH] = { 0 };
	int row = m_listFund.GetSelectItem();

	//�������� 
	memset(&temp, 0, sizeof(temp));
	m_listFund.GetSubItem(row, 1, temp, sizeof(temp));
	strncpy_s(delReq.SettleDate, temp);
	//��˾���
	memset(&temp, 0, sizeof(temp));
	m_listFund.GetSubItem(row, 2, temp, sizeof(temp));
	strncpy_s(delReq.CompanyNo, temp);
	//����Ա��� 
	memset(&temp, 0, sizeof(temp));
	m_listFund.GetSubItem(row, 3, temp, sizeof(temp));
	strncpy_s(delReq.UserNo, temp);
	//����Ա
	strncpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);

	g_pSpotSystem->DeleteFund(&delReq);

}

void FundWnd::RefreshList()
{
	if (!m_System)return;
	for (auto it : m_System->m_vecFund)
	{
		AddFundToList(it);
	}
}
