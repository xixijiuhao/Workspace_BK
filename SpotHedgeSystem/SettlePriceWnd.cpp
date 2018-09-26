#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
#define cCAPTION_H			80
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����

SettlePriceWnd::SettlePriceWnd()
	:m_System(nullptr)
{
}

SettlePriceWnd::~SettlePriceWnd()
{
}

void SettlePriceWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void SettlePriceWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void SettlePriceWnd::AddSettlePriceToList(SmsSettlePriceDataRsp & specprice)
{
	//���к�
	m_listSettlePrice.AddItem((double)specprice.SerialId, 0, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��������
	m_listSettlePrice.AddItem(specprice.SettleDate, 0, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��Լ���
	string sCode = specprice.ContractNo;
	string tCode;
	CConvert::SContractNoToTContractNo(sCode, tCode);
	m_listSettlePrice.AddItem(tCode.c_str(), 0, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�����
	m_listSettlePrice.AddItem(specprice.SettlePrice, 0, 3, 4, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT SettlePriceWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void SettlePriceWnd::SetContractNo(string sCode)
{
	if (IsWindowVisible(m_popCode.GetHwnd()))
		m_EditContractNo.SetText(sCode.c_str());
	else if(IsWindowVisible(m_popCode2.GetHwnd()))
		m_EditContractNo2.SetText(sCode.c_str());
}

void SettlePriceWnd::OnCreate()
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
	//��Լ����
	m_labContractNo.Create(m_Hwnd, L"��Լ����", DT_RIGHT, ID_LabContractno);
	m_EditContractNo.Create(m_Hwnd, ID_EditContractno);
	m_btnConntractNo.Create(m_Hwnd, "...", ID_BtnContractno);
	//��ѯ
	m_btnQry.Create(m_Hwnd, "��  ѯ", ID_BtnQry);

	//��������
	m_labSettleDate.Create(m_Hwnd, L"��������", DT_RIGHT, ID_LabSettleDate);
	m_settleTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 120, 25, m_Hwnd, (HMENU)ID_CbtSettleDate, m_hInstance, NULL);
	//��Լ����
	m_labContractNo2.Create(m_Hwnd, L"��Լ����", DT_RIGHT, ID_LabContractno2);
	m_EditContractNo2.Create(m_Hwnd, ID_EditContractno2);
	m_btnConntractNo2.Create(m_Hwnd, "...", ID_BtnContractno2);
	//����۸�
	m_labSettlePrice.Create(m_Hwnd, L"����۸�", DT_RIGHT, ID_LabSettlePrice);
	m_EditSettlePrice.Create(m_Hwnd, 0, 0, ID_EditSettlePrice);
	//���ӡ�ɾ��
	m_btnAdd.Create(m_Hwnd, "��  ��", ID_BtnAdd);
	//���
	m_btnReset.Create(m_Hwnd, "��  ��", ID_BtnReset);
	
	//���
	m_listSettlePrice.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_SettlePrice);


	//////////////////////////////Init////////////////////////////////////
	m_listSettlePrice.AddColumn(L"���к�", 60, -1);
	m_listSettlePrice.AddColumn(L"��������", 160, -1);
	m_listSettlePrice.AddColumn(L"��Լ����", 220, -1);
	m_listSettlePrice.AddColumn(L"�����", 220, -1);

	m_menuSettlePrice = CreatePopupMenu();

	AppendMenu(m_menuSettlePrice, MF_STRING, IDM_DelSettlePrice, L"ɾ�������");

	m_labBeginDate.SetFont(g_FontData.GetFontWord13());
	m_labEndDate.SetFont(g_FontData.GetFontWord13());
	m_labContractNo.SetFont(g_FontData.GetFontWord13());
	m_labSettleDate.SetFont(g_FontData.GetFontWord13());
	m_labContractNo2.SetFont(g_FontData.GetFontWord13());
	m_labSettlePrice.SetFont(g_FontData.GetFontWord13());

	m_EditContractNo.SetFont(g_FontData.GetFontNumEx15());
	m_EditContractNo2.SetFont(g_FontData.GetFontNumEx15());
	m_EditSettlePrice.SetFont(g_FontData.GetFontNum15());

	m_btnConntractNo.SetFont(g_FontData.GetFontNum13());
	m_btnQry.SetFont(g_FontData.GetFontWord12());
	m_btnConntractNo2.SetFont(g_FontData.GetFontNum13());
	m_btnAdd.SetFont(g_FontData.GetFontWord12());
	m_btnReset.SetFont(g_FontData.GetFontWord12());

	m_cbtBeginDate.Init(m_beginTimePick);
	SendMessage(m_beginTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_beginTimePick, L"yyyy-MM-dd");

	m_cbtEndDate.Init(m_endTimePick);
	SendMessage(m_endTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_endTimePick, L"yyyy-MM-dd");

	m_cbtSettleDate.Init(m_settleTimePick);
	SendMessage(m_settleTimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_settleTimePick, L"yyyy-MM-dd");

	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtEndDate.GetHwnd(), GDT_VALID, &time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);
	time.wDay = 1;
	DateTime_SetSystemtime(m_cbtBeginDate.GetHwnd(), GDT_VALID, &time);

	/////////////////////POS///////////////////////////////////
	int leftstart = 10;
	int topstart = 10;
	int editWidth = 140;	//edit�ؼ����
	int labWidth = 60;		//lab�ؼ����
	int btnWidth = 80;		//btn�ؼ����
	int Height = 25;		//�ؼ��߶�
	int htap = 10;			//������
	int vtap = 10;			//������

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
	pt.x += editWidth + htap;
	m_labContractNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_EditContractNo.MoveWindow(pt.x, pt.y, editWidth - 20 , Height);
	pt.x += editWidth -20 + 1;
	m_btnConntractNo.MoveWindow(pt.x, pt.y, 19, Height);
	pt.x += 19 + htap * 3;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = leftstart;
	pt.y += Height+ vtap;
	m_labSettleDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtSettleDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labContractNo2.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_EditContractNo2.MoveWindow(pt.x, pt.y, editWidth - 20, Height);
	pt.x += editWidth -20 + 1;
	m_btnConntractNo2.MoveWindow(pt.x, pt.y, 19, Height);
	pt.x += 19 + htap;
	m_labSettlePrice.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_EditSettlePrice.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_btnAdd.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnReset.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listSettlePrice.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void SettlePriceWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void SettlePriceWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//�����ǲ˵�
	{
		switch (LOWORD(wParam))
		{
		case IDM_DelSettlePrice://ɾ�������
			DealDelSettlePrice();
			break;
		default:
			break;
		}
	}
}

void SettlePriceWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_SettlePrice:
		m_listSettlePrice.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void SettlePriceWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_SettlePrice:
		DealSettlePriceList(lParam);
		break;
	default:
		break;
	}
}

LRESULT SettlePriceWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void SettlePriceWnd::OnNcDestroy()
{
}

void SettlePriceWnd::OnDestroy()
{
}

void SettlePriceWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listSettlePrice.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}

void SettlePriceWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listSettlePrice.OnMeasureItem(p->itemHeight);
}

void SettlePriceWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnContractno:
		OnBtnCode();
		break;
	case ID_BtnContractno2:
		OnBtnCode2();
		break;
	case ID_BtnQry:
		OnBtnQry();
		break;
	case ID_BtnAdd:
		OnBtnAdd();
		break;
	case ID_BtnReset:
		OnBtnReset();
		break;
	default:
		break;
	}
}

void SettlePriceWnd::DealSettlePriceList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listSettlePrice.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuSettlePrice, IDM_DelSettlePrice, MF_GRAYED);
		}
		ShowMenu(m_menuSettlePrice);
	}
	else if (p->hdr.code == NM_DBLCLK)
	{
		if (p->iItem >= m_listSettlePrice.GetItemCount() || p->iItem == -1)
			return;
		if (!m_System)return;

		//˫��������Ϣ
		int index = m_listSettlePrice.GetSelectItem();

		//��������
		char temp[MAX_PATH] = { 0 };
		m_listSettlePrice.GetSubItem(index, 1, temp, sizeof(temp));
		vector<string> vdate;
		vdate = m_System->SplitString(temp, '-');
		if (vdate.size() < 3)
			return;
		SYSTEMTIME time;
		memset(&time, 0, sizeof(SYSTEMTIME));
		time.wYear = atoi(vdate.at(0).c_str());
		time.wMonth = atoi(vdate.at(1).c_str());
		time.wDay = atoi(vdate.at(2).c_str());
		DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);
		//��Լ����
		memset(&temp, 0, sizeof(temp));
		m_listSettlePrice.GetSubItem(index, 2, temp, sizeof(temp));
		m_EditContractNo2.SetText(LoadRC::ansi_to_unicode(temp).c_str());
		//�����
		memset(&temp, 0, sizeof(temp));
		m_listSettlePrice.GetSubItem(index, 3, temp, sizeof(temp));
		m_EditSettlePrice.SetText(temp);

		//����ûҡ���ťת��
		EnableWindow(m_cbtSettleDate.GetHwnd(), FALSE);
		EnableWindow(m_EditContractNo2.GetHwnd(), FALSE);
		ShowWindow(m_btnConntractNo2.GetHwnd(), SW_HIDE);
		m_btnAdd.SetButtonText("��  ��");
	}
}

void SettlePriceWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void SettlePriceWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_DelSettlePrice; i <= IDM_DelSettlePrice; i++)
		EnableMenuItem(m_menuSettlePrice, i, flage);
}

void SettlePriceWnd::OnBtnQry()
{
	SmsSettlePriceQryReq qryReq;
	memset(&qryReq, 0, sizeof(SmsSettlePriceQryReq));
	char temp[MAX_PATH] = { 0 };
	//���к�
	qryReq.SerialId = 0;

	//��Լ
	char con[MAX_PATH] = { 0 };
	m_EditContractNo.GetText(con, sizeof(con));
	TContractKey conKey;
	memset(&conKey, 0, sizeof(TContractKey));
	CConvert::TContractNoToTContractKey(con, conKey);
	char sCode[101] = { 0 };
	CConvert::TContractKeyToSContractNo(conKey, sCode);
	strcpy_s(qryReq.ContractNo, sCode);

	//��ʼ����
	wstring tempDate1;
	m_cbtBeginDate.CxGetWindowText(tempDate1);
	strcpy_s(qryReq.BeginDate, LoadRC::unicode_to_ansi(tempDate1).c_str());
	//��ֹ����
	wstring tempDate2;
	m_cbtEndDate.CxGetWindowText(tempDate2);
	strcpy_s(qryReq.EndDate, LoadRC::unicode_to_ansi(tempDate2).c_str());


	//����б�
	m_listSettlePrice.DeleteAllItems();

	//����ظ�����
	m_System->m_vecSettlePrice.clear();
	g_pSpotSystem->QrySettlePrice(&qryReq);

}

void SettlePriceWnd::OnBtnCode()
{
	POINT pt = { 0, 0 };
	ClientToScreen(m_EditContractNo.GetHwnd(), &pt);
	pt.y += 25;

	if (!IsWindow(m_popCode.GetHwnd()))
		m_popCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0,pt.x,pt.y);

	m_popCode.UpdateData(m_System->m_curLogin.sSign);

	SetWindowPos(m_popCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

void SettlePriceWnd::OnBtnCode2()
{
	POINT pt = { 0, 0 };
	ClientToScreen(m_EditContractNo2.GetHwnd(), &pt);
	pt.y += 25;

	if (!IsWindow(m_popCode2.GetHwnd()))
		m_popCode2.CreatePopWin(this, L"popupCode2", m_Hwnd, WS_CLIPSIBLINGS, 0, pt.x, pt.y);

	m_popCode2.UpdateData(m_System->m_curLogin.sSign);

	SetWindowPos(m_popCode2.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

void SettlePriceWnd::DealDelSettlePrice()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ȷ��ɾ���˽���ۣ�", TMB_OKCANCEL))
		return;
	SmsSettlePriceDelReq delReq;
	memset(&delReq, 0, sizeof(delReq));

	int row = m_listSettlePrice.GetSelectItem();
	//���к�
	//char	serial[24] = { 0 };
	//m_listSettlePrice.GetSubItem(row, 0, serial, sizeof(serial));
	//delReq.SerialId = atoi(serial);
	//��������
	m_listSettlePrice.GetSubItem(row, 1, delReq.SettleDate, sizeof(SmsDateType));
	//��Լ
	char con[MAX_PATH] = { 0 };
	m_listSettlePrice.GetSubItem(row, 2, con, sizeof(con));
	TContractKey conKey;
	memset(&conKey, 0, sizeof(TContractKey));
	CConvert::TContractNoToTContractKey(con, conKey);
	char sCode[101] = { 0 };
	CConvert::TContractKeyToSContractNo(conKey, sCode);
	strncpy_s(delReq.ContractNo, sCode);
	//�����
	//char	price[24] = { 0 };
	//m_listSettlePrice.GetSubItem(row, 3, price, sizeof(price));
	//delReq.SettlePrice = atof(price);

	g_pSpotSystem->DeleteSettlePrice(&delReq);
}

void SettlePriceWnd::OnBtnAdd()
{
	if (!m_System)return;
	if (IsWindowEnabled(m_cbtSettleDate.GetHwnd()))	//����
	{
		SmsSettlePriceAddReq req;
		memset(&req, 0, sizeof(SmsSettlePriceAddReq));

		req.SerialId = 0;

		wstring tempDate;
		m_cbtSettleDate.CxGetWindowText(tempDate);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(tempDate).c_str());

		char con[MAX_PATH] = { 0 };
		m_EditContractNo2.GetText(con, sizeof(con));
		TContractKey conKey;
		memset(&conKey, 0, sizeof(TContractKey));
		CConvert::TContractNoToTContractKey(con, conKey);
		char sCode[101] = { 0 };
		CConvert::TContractKeyToSContractNo(conKey, sCode);
		strncpy_s(req.ContractNo, sCode);

		char temp[64] = { 0 };
		m_EditSettlePrice.GetText(temp, sizeof(temp));
		req.SettlePrice = atof(temp);

		g_pSpotSystem->AddSettlePrice(&req);
	}
	else//�޸�
	{
		SmsSettlePriceModReq req;
		memset(&req, 0, sizeof(SmsSettlePriceModReq));

		wstring tempDate;
		m_cbtSettleDate.CxGetWindowText(tempDate);
		strncpy_s(req.SettleDate, LoadRC::unicode_to_ansi(tempDate).c_str());

		char con[MAX_PATH] = { 0 };
		m_EditContractNo2.GetText(con, sizeof(con));
		TContractKey conKey;
		memset(&conKey, 0, sizeof(TContractKey));
		CConvert::TContractNoToTContractKey(con, conKey);
		char sCode[101] = { 0 };
		CConvert::TContractKeyToSContractNo(conKey, sCode);
		strncpy_s(req.ContractNo, sCode);

		char temp[64] = { 0 };
		m_EditSettlePrice.GetText(temp, sizeof(temp));
		req.SettlePrice = atof(temp);

		g_pSpotSystem->ModifySettlePrice(&req);
	}
}

void SettlePriceWnd::OnBtnReset()
{
	m_EditContractNo2.SetText("");
	m_EditSettlePrice.SetText("");

	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtSettleDate.GetHwnd(), GDT_VALID, &time);

	//����ûҡ���ťת��
	EnableWindow(m_cbtSettleDate.GetHwnd(), TRUE);
	EnableWindow(m_EditContractNo2.GetHwnd(), TRUE);
	ShowWindow(m_btnConntractNo2.GetHwnd(), SW_SHOW);
	m_btnAdd.SetButtonText("��  ��");
}
