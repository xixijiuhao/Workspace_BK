#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
#define cCAPTION_H			45
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����

PositionWnd::PositionWnd()
{
}

PositionWnd::~PositionWnd()
{
}

void PositionWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void PositionWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void PositionWnd::AddPositionToList(SmsPositionDataRsp & pos, int index)
{
	int count = 0;
	if (index == -1)
		count = m_listPosition.GetItemCount();
	else
		count = index;
	//���к�
	m_listPosition.AddItem((double)pos.SerialId, count, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��������
	m_listPosition.AddItem(pos.SettleDate, count, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��˾���
	m_listPosition.AddItem(pos.CompanyNo, count, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����Ա���
	m_listPosition.AddItem(pos.UserNo, count, 3, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��ͬ���
	//m_listPosition.AddItem(pos.AgreementNo, count, 4, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��Լ���
	string sCode = pos.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	m_listPosition.AddItem(tCode.c_str(), count, 4, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ֱֲ��
	m_listPosition.AddItem(pos.PositionNo, count, 5, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ֲ�����
	m_listPosition.AddItem((double)pos.PositionQty, count, 6, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��������
	if (pos.Direct == SMS_DIRECT_BUY)
		m_listPosition.AddItem("��", count, 7, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	else if (pos.Direct == SMS_DIRECT_SELL)
		m_listPosition.AddItem("��", count, 7, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	else
		m_listPosition.AddItem("˫��", count, 7, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��ƽ
	//if (pos.Offset == SMS_O_OPEN)
	//	m_listPosition.AddItem("����", count, 9, DT_CENTER, g_ColorRefData.GetColorBuyRed());
	//else if (pos.Offset == SMS_O_COVER)
	//	m_listPosition.AddItem("ƽ��", count, 9, DT_CENTER, g_ColorRefData.GetColorSellBlue());
	//Ͷ������
	if (pos.Hedge == SMS_HEDGE_SPECULATE)
		m_listPosition.AddItem("Ͷ��", count, 8, DT_CENTER, g_ColorRefData.GetColorBlack());
	else if (pos.Hedge == SMS_HEDGE_HEDGE)
		m_listPosition.AddItem("�ױ�", count, 8, DT_CENTER, g_ColorRefData.GetColorBlack());
	else
		m_listPosition.AddItem("��", count, 8, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ֲּ۸�
	m_listPosition.AddItem(pos.PositionPrice, count, 9, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ֲ�����
	m_listPosition.AddItem(pos.PositionProfit, count, 10, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����Ա
	m_listPosition.AddItem(pos.OperatorNo, count, 11, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�޸�ʱ��
	m_listPosition.AddItem(pos.OperateTime, count, 12, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT PositionWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_POSITION_REFRESH:
		RefreshList();
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

void PositionWnd::OnCreate()
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
	m_listPosition.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Position);
	//RECT rect;
	//GetClientRect(m_Hwnd, &rect);
	//m_listPosition.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	//////////////////////////////Init////////////////////////////////////
	m_listPosition.AddColumn(L"���к�", 80, -1);
	m_listPosition.AddColumn(L"��������", 80, -1);
	m_listPosition.AddColumn(L"��˾���", 80, -1);
	m_listPosition.AddColumn(L"�˺�", 80, -1);
	//m_listPosition.AddColumn(L"��ͬ���", 80, -1);
	m_listPosition.AddColumn(L"��Լ���", 80, -1);
	m_listPosition.AddColumn(L"�ֱֲ��", 80, -1);
	m_listPosition.AddColumn(L"�ֲ�����", 80, -1);
	m_listPosition.AddColumn(L"����", 80, -1);
	//m_listPosition.AddColumn(L"��ƽ", 80, -1);
	m_listPosition.AddColumn(L"Ͷ��", 80, -1);
	m_listPosition.AddColumn(L"�ֲּ۸�", 80, -1);
	m_listPosition.AddColumn(L"�ֲ�����", 80, -1);
	m_listPosition.AddColumn(L"����Ա", 80, -1);
	m_listPosition.AddColumn(L"�޸�ʱ��", 130, -1);

	m_menuPosition = CreatePopupMenu();

	AppendMenu(m_menuPosition, MF_STRING, IDM_AddPosition, L"¼��ֲ�");
	AppendMenu(m_menuPosition, MF_SEPARATOR, IDM_Line, L"");
	AppendMenu(m_menuPosition, MF_STRING, IDM_ModPosition, L"�޸ĳֲ�");
	AppendMenu(m_menuPosition, MF_STRING, IDM_DelPosition, L"ɾ���ֲ�");

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
	m_listPosition.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top);
}

void PositionWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void PositionWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//�����ǲ˵�
	{
		switch (LOWORD(wParam))
		{
		case IDM_AddPosition:	//¼��ֲ�
			if (!IsWindow(m_addPositionWnd.GetHwnd()))
			{
				m_addPositionWnd.ShowFrm(m_Hwnd);
				m_addPositionWnd.Register(m_System);				
			}
			else
			{
				if (!IsWindowVisible(m_addPositionWnd.GetHwnd()))
					ShowWindow(m_addPositionWnd.GetHwnd(), SW_SHOW);
			}
			m_addPositionWnd.InitWnd();
			break;
		case IDM_ModPosition://�޸ĳֲ�
		{
			if (!IsWindow(m_addPositionWnd.GetHwnd()))
			{
				m_addPositionWnd.ShowFrm(m_Hwnd);
				m_addPositionWnd.Register(m_System);
			}
			else
			{
				if (!IsWindowVisible(m_addPositionWnd.GetHwnd()))
					ShowWindow(m_addPositionWnd.GetHwnd(), SW_SHOW);
			}

			char	serialId[32] = { 0 };
			int index = m_listPosition.GetSelectItem();
			m_listPosition.GetSubItem(index, 0, serialId, sizeof(serialId));
			int id = atoi(serialId);
			for (auto it : m_System->m_vecPosition)
			{
				if (it.SerialId  ==  id)
				{
					m_addPositionWnd.InitModifyWnd(it);
					break;
				}
			}
		}
			break;
		case IDM_DelPosition://ɾ���ֲ�
			DealDelPosition();
			break;
		default:
			break;
		}
	}
}

void PositionWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Position:
		m_listPosition.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void PositionWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Position:
		DealPositionList(lParam);
		break;
	default:
		break;
	}
}

LRESULT PositionWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void PositionWnd::OnNcDestroy()
{

}

void PositionWnd::OnDestroy()
{
}

void PositionWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listPosition.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void PositionWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listPosition.OnMeasureItem(p->itemHeight);
}

void PositionWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
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

void PositionWnd::DealPositionList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listPosition.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuPosition, IDM_ModPosition, MF_GRAYED);
			EnableMenuItem(m_menuPosition, IDM_DelPosition, MF_GRAYED);
		}

		ShowMenu(m_menuPosition);
	}
}

void PositionWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void PositionWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_AddPosition; i <= IDM_DelPosition; i++)
		EnableMenuItem(m_menuPosition, i, flage);
}

void PositionWnd::OnBtnQry()
{
	SmsPositionQryReq qryReq;
	memset(&qryReq, 0, sizeof(SmsPositionQryReq));
	char temp[MAX_PATH] = { 0 };
	if (!m_System)return;
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
	//
	m_listPosition.DeleteAllItems();

	m_System->m_vecPosition.clear();
	g_pSpotSystem->QryPosition(&qryReq);

}

void PositionWnd::DealDelPosition()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ȷ��ɾ���˳ֲ�ô��", TMB_OKCANCEL))
		return;
	if (!m_System)return;

	SmsPositionDelReq delReq;
	memset(&delReq, 0, sizeof(SmsPositionDelReq));

	int row = m_listPosition.GetSelectItem();
	//���к�
	char	serial[24] = { 0 };
	m_listPosition.GetSubItem(row, 0, serial, sizeof(serial));
	delReq.SerialId = atoi(serial);

	//����Ա
	strncpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);

	g_pSpotSystem->DeletePosition(&delReq);
}

void PositionWnd::RefreshList()
{
	if (!m_System)return;
	for (auto it : m_System->m_vecPosition)
	{
		AddPositionToList(it);
	}
}
