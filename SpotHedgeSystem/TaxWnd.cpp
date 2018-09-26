#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����

#define cCAPTION_H			45	

TaxWnd::TaxWnd()
{

}

TaxWnd::~TaxWnd()
{

}

void TaxWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void TaxWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void TaxWnd::AddTaxToList(SmsTaxDataRsp & agree)
{
	//���к�
	m_listTax.AddItem((double)agree.SerialId, 0, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��ʼ����
	m_listTax.AddItem(agree.BeginDate, 0, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//˰��
	m_listTax.AddItem((double)agree.Tax, 0, 2, 4, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����Ա
	m_listTax.AddItem(agree.OperatorNo, 0, 3, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�޸�ʱ��
	m_listTax.AddItem(agree.OperateTime, 0, 4, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT TaxWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
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

void TaxWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);
	//��ʼ����
	m_labBeginDate.Create(m_Hwnd, L"��ʼ����", DT_RIGHT, ID_labBeginDate);
	m_beginTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 120, 25, m_Hwnd, (HMENU)ID_cbtBeginDate, m_hInstance, NULL);
	//˰��
	m_labTax.Create(m_Hwnd, L"˰�ʱ���", DT_RIGHT, ID_labTax);
	m_editTax.Create(m_Hwnd, 0, 0, ID_editTax);
	//���ӡ��޸�
	m_btnAdd.Create(m_Hwnd, "��  ��", ID_btnAdd);
	m_btnReset.Create(m_Hwnd, "��  ��", ID_btnReset);

	//���
	m_listTax.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Tax);

	//============================�����б�=============================
	m_listTax.AddColumn(L"���к�", 60, -1);
	m_listTax.AddColumn(L"��ʼ����", 140, -1);
	m_listTax.AddColumn(L"˰��", 200, -1);
	m_listTax.AddColumn(L"����Ա", 200, -1);
	m_listTax.AddColumn(L"�޸�ʱ��", 200, -1);

	m_menuTax = CreatePopupMenu();
	AppendMenu(m_menuTax, MF_STRING, IDM_DelTax, L"ɾ��˰��");

	m_labBeginDate.SetFont(g_FontData.GetFontWord13());
	m_labTax.SetFont(g_FontData.GetFontWord13());


	m_editTax.SetFont(g_FontData.GetFontNum15());


	m_btnAdd.SetFont(g_FontData.GetFontWord12());
	m_btnReset.SetFont(g_FontData.GetFontWord12());

	m_cbtBeginDate.Init(m_beginTimePick);
	SendMessage(m_beginTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_beginTimePick, L"yyyy-MM-dd");



	////////////////////POS/////////////////////////
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
	m_labTax.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editTax.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_btnAdd.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnReset.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x = 0; pt.y = cCAPTION_H;

	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listTax.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void TaxWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void TaxWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//�����ǲ˵�
	{
		switch (LOWORD(wParam))
		{
		case IDM_DelTax://ɾ��˰��
			DealDelTax();
			break;
		default:
			break;
		}
	}


}

void TaxWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Tax:
		m_listTax.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void TaxWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Tax:
		DealTaxList(lParam);
		break;
	default:
		break;
	}
}

LRESULT TaxWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void TaxWnd::OnNcDestroy()
{
}

void TaxWnd::OnDestroy()
{
	//if (IsWindow(m_addTaxWnd.GetHwnd()))
	//	DestroyWindow(m_addTaxWnd.GetHwnd());
}

void TaxWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listTax.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}


void TaxWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listTax.OnMeasureItem(p->itemHeight);
}

void TaxWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_btnAdd:
		OnBtnAdd();
		break;
	case ID_btnReset:
		OnBtnReset();
		break;
	default:
		break;
	}
}

void TaxWnd::DealTaxList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listTax.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuTax, IDM_DelTax, MF_GRAYED);
		}
		ShowMenu(m_menuTax);
	}
	else if (p->hdr.code == NM_DBLCLK)
	{
		if (p->iItem >= m_listTax.GetItemCount() || p->iItem == -1)
			return;
		//˫��������Ϣ
		int index = m_listTax.GetSelectItem();

		char temp[MAX_PATH] = { 0 };
		//m_listTax.GetSubItem(index, 0, temp, sizeof(temp));
		//m_editSerialId.SetText(temp);
		m_listTax.GetSubItem(index, 1, temp, sizeof(temp));
		vector<string> vdate;
		vdate = m_System->SplitString(temp, '-');
		if (vdate.size() < 3)
			return;
		SYSTEMTIME time;
		memset(&time, 0, sizeof(SYSTEMTIME));
		time.wYear = atoi(vdate.at(0).c_str());
		time.wMonth = atoi(vdate.at(1).c_str());
		time.wDay = atoi(vdate.at(2).c_str());
		DateTime_SetSystemtime(m_cbtBeginDate.GetHwnd(), GDT_VALID, &time);


		m_listTax.GetSubItem(index, 2, temp, sizeof(temp));
		m_editTax.SetText(temp);

		//����ûҡ���ťת��
		EnableWindow(m_cbtBeginDate.GetHwnd(), FALSE);
		m_btnAdd.SetButtonText("��  ��");
	}
}

void TaxWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void TaxWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_DelTax; i <= IDM_DelTax; i++)
		EnableMenuItem(m_menuTax, i, flage);

}

void TaxWnd::DealDelTax()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ȷ��ɾ����˰�ʣ�", TMB_OKCANCEL))
		return;
	SmsTaxDelReq delReq;
	memset(&delReq, 0, sizeof(delReq));

	strcpy_s(delReq.CompanyNo, m_System->m_curLogin.CompanyNo);

	int row = m_listTax.GetSelectItem();
	m_listTax.GetSubItem(row, 1, delReq.BeginDate, sizeof(SmsDateType));

	strcpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);

	g_pSpotSystem->DeleteTax(&delReq);
}

void TaxWnd::OnBtnAdd()
{
	if (!m_System)return;
	if (IsWindowEnabled(m_cbtBeginDate.GetHwnd()))	//����
	{
		SmsTaxAddReq req;
		memset(&req, 0, sizeof(SmsTaxAddReq));

		req.SerialId = 0;

		wstring tempDate;
		m_cbtBeginDate.CxGetWindowText(tempDate);
		strncpy_s(req.BeginDate, LoadRC::unicode_to_ansi(tempDate).c_str());

		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);

		char temp[64] = { 0 };
		m_editTax.GetText(temp, sizeof(temp));
		req.Tax = atof(temp);

		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);

		g_pSpotSystem->AddTax(&req);
	}
	else//�޸�
	{
		SmsTaxModReq req;
		memset(&req, 0, sizeof(SmsTaxModReq));

		wstring tempDate;
		m_cbtBeginDate.CxGetWindowText(tempDate);
		strncpy_s(req.BeginDate, LoadRC::unicode_to_ansi(tempDate).c_str());

		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);

		char temp[64] = { 0 };
		m_editTax.GetText(temp, sizeof(temp));
		req.Tax = atof(temp);

		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);

		g_pSpotSystem->ModifyTax(&req);
	}
}

void TaxWnd::OnBtnReset()
{

	m_editTax.SetText("");

	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtBeginDate.GetHwnd(), GDT_VALID, &time);

	//��񡢰�ťת��
	EnableWindow(m_cbtBeginDate.GetHwnd(), TRUE);
	m_btnAdd.SetButtonText("��  ��");
}


