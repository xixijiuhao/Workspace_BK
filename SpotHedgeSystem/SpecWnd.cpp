#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����

#define cCAPTION_H			45

SpecWnd::SpecWnd()
{

}

SpecWnd::~SpecWnd()
{

}

void SpecWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void SpecWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

void SpecWnd::AddSpecToList(SmsSpecDataRsp & agree)
{
	int row = 0/*m_listSpec.GetItemCount()*/;
	//���к�
	m_listSpec.AddItem((double)agree.SerialId, row, 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	//��˾���
	//m_listSpec.AddItem(agree.CompanyNo, 0, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�����
	m_listSpec.AddItem(agree.SpecificationNo, row, 1, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�������
	wstring specName = LoadRC::ansi_to_unicode(agree.SpecName, CP_UTF8);
	m_listSpec.AddItem(LoadRC::unicode_to_ansi(specName).c_str(), row, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//Ʒ�ֱ��
	m_listSpec.AddItem(agree.CommodityNo, row, 3, DT_CENTER, g_ColorRefData.GetColorBlack());
	//����Ա
	m_listSpec.AddItem(agree.OperatorNo, row, 4, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�޸�ʱ��
	m_listSpec.AddItem(agree.OperateTime, row, 5, DT_CENTER, g_ColorRefData.GetColorBlack());
}

LRESULT SpecWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void SpecWnd::OnCreate()
{
	//���к�
	//m_labSerialId.Create(m_Hwnd, L"���к���", DT_RIGHT, ID_labSerialID);
	//m_editSerialId.Create(m_Hwnd, 0, 0, ID_editSerialID);
	//�����
	m_labSpecNo.Create(m_Hwnd, L"�����", DT_RIGHT, ID_labSpecNo);
	m_editSpecNo.Create(m_Hwnd, 0, 0, ID_editSpecNo);
	//�������
	m_labSpecName.Create(m_Hwnd, L"�������", DT_RIGHT, ID_labSpecName);
	m_editSpecName.Create(m_Hwnd, 0, 0, ID_editSpecName);
	//Ʒ�ֱ��
	m_labCommNo.Create(m_Hwnd, L"Ʒ�ֱ��", DT_RIGHT, ID_labCommNo);
	m_editCommNo.Create(m_Hwnd, 0, 0, ID_editCommNo);
	//����
	m_bthAdd.Create(m_Hwnd, "��  ��", ID_btnAdd);
	//�޸�
	//m_bthModify.Create(m_Hwnd, "��  ��", ID_btnModify);
	//����
	m_bthReset.Create(m_Hwnd, "��  ��", ID_btnReset);
	//���
	m_listSpec.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_Spec);

	//============================�����б�=============================
	m_listSpec.AddColumn(L"���к�", 60, -1);
	//m_listSpec.AddColumn(L"��˾���", 120, -1);
	m_listSpec.AddColumn(L"�����", 140, -1);
	m_listSpec.AddColumn(L"�������", 200, -1);
	m_listSpec.AddColumn(L"Ʒ�ֱ��", 200, -1);
	m_listSpec.AddColumn(L"����Ա", 200, -1);
	m_listSpec.AddColumn(L"�޸�ʱ��", 150, -1);
	m_menuSpec = CreatePopupMenu();
	//AppendMenu(m_menuSpec, MF_STRING, IDM_AddSpec, L"¼����");
	//AppendMenu(m_menuSpec, MF_SEPARATOR, IDM_Line, L"");
	//AppendMenu(m_menuSpec, MF_STRING, IDM_ModSpec, L"�޸Ĺ��");
	AppendMenu(m_menuSpec, MF_STRING, IDM_DelSpec, L"ɾ�����");

	//EnableWindow(m_labSerialId.GetHwnd(), FALSE);
	//EnableWindow(m_editSerialId.GetHwnd(), FALSE);

	//m_labSerialId.SetFont(g_FontData.GetFontWord13());
	m_labSpecNo.SetFont(g_FontData.GetFontWord13());
	m_labSpecName.SetFont(g_FontData.GetFontWord13());
	m_labCommNo.SetFont(g_FontData.GetFontWord13());

	//m_editSerialId.SetFont(g_FontData.GetFontNum15());
	m_editSpecNo.SetFont(g_FontData.GetFontNum15());
	m_editSpecName.SetFont(g_FontData.GetFontNum15());
	m_editCommNo.SetFont(g_FontData.GetFontNum15());

	m_bthAdd.SetFont(g_FontData.GetFontWord12());
	//m_bthModify.SetFont(g_FontData.GetFontWord12());
	m_bthReset.SetFont(g_FontData.GetFontWord12());

	////////////////////POS//////////////////////	
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
	//m_labSerialId.MoveWindow(pt.x, pt.y, labWidth, Height);
	//pt.x += labWidth + htap;
	//m_editSerialId.MoveWindow(pt.x, pt.y, editWidth, Height);
	//pt.x += editWidth + htap;
	m_labSpecNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSpecNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labSpecName.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editSpecName.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap;
	m_labCommNo.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_editCommNo.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_bthAdd.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	//m_bthModify.MoveWindow(pt.x, pt.y, btnWidth, Height);
	//pt.x += btnWidth + htap * 3;
	m_bthReset.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x = 0; pt.y = cCAPTION_H;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listSpec.MoveWindow(rect.left, pt.y, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);

}

void SpecWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void SpecWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//�����ǲ˵�
	{
		switch (LOWORD(wParam))
		{
		case IDM_DelSpec://ɾ����ͬ
			DealDelSpec();
			break;
		default:
			break;
		}
	}


}

void SpecWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_Spec:
		m_listSpec.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void SpecWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_Spec:
		DealSpecList(lParam);
		break;
	default:
		break;
	}
}

LRESULT SpecWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void SpecWnd::OnNcDestroy()
{
}

void SpecWnd::OnDestroy()
{
	//if (IsWindow(m_addSpecWnd.GetHwnd()))
	//	DestroyWindow(m_addSpecWnd.GetHwnd());
}

void SpecWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listSpec.MoveWindow(rect.left, cCAPTION_H, rect.right - rect.left, rect.bottom - rect.top - cCAPTION_H);
}


void SpecWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listSpec.OnMeasureItem(p->itemHeight);
}

void SpecWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_btnAdd:
		OnBtnAdd();
		break;
	//case ID_btnModify:
	//	OnBtnMod();
	//	break;
	case ID_btnReset:
		OnBtnReset();
		break;
	default:
		break;
	}
}

void SpecWnd::DealSpecList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listSpec.GetItemCount() || p->iItem == -1)
		{
			//EnableMenuItem(m_menuSpec, IDM_ModSpec, MF_GRAYED);
			EnableMenuItem(m_menuSpec, IDM_DelSpec, MF_GRAYED);
		}
		ShowMenu(m_menuSpec);
	}
	else if (p->hdr.code == NM_DBLCLK)
	{
		if (p->iItem >= m_listSpec.GetItemCount() || p->iItem == -1)
			return;
		//˫��������Ϣ
		int index = m_listSpec.GetSelectItem();

		char temp[MAX_PATH] = { 0 };
		//m_listSpec.GetSubItem(index, 0, temp, sizeof(temp));
		//m_editSerialId.SetText(temp);
		m_listSpec.GetSubItem(index, 1, temp, sizeof(temp));
		m_editSpecNo.SetText(temp);
		m_listSpec.GetSubItem(index, 2, temp, sizeof(temp));
		m_editSpecName.SetText(temp);
		m_listSpec.GetSubItem(index, 3, temp, sizeof(temp));
		m_editCommNo.SetText(temp);

		//����ûҡ���ťת��
		EnableWindow(m_editSpecNo.GetHwnd(), FALSE);
		m_bthAdd.SetButtonText("��  ��");
	}
}

void SpecWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void SpecWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_DelSpec; i <= IDM_DelSpec; i++)
		EnableMenuItem(m_menuSpec, i, flage);

}

void SpecWnd::DealDelSpec()
{
	if (!_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ȷ��ɾ���˹��", TMB_OKCANCEL))
		return;
	if (!m_System)return;

	SmsSpecDelReq delReq;
	memset(&delReq, 0, sizeof(delReq));

	int row = m_listSpec.GetSelectItem();
	strncpy_s(delReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strncpy_s(delReq.OperatorNo, m_System->m_curLogin.UserNo);
	m_listSpec.GetSubItem(row, 1, delReq.SpecificationNo, sizeof(SmsSpecificationNoType));
	
	g_pSpotSystem->DeleteSpec(&delReq);
}

void SpecWnd::OnBtnAdd()
{
	if (!m_System)return;

	if (IsWindowEnabled(m_editSpecNo.GetHwnd()))	//����
	{
		SmsSpecAddReq req;
		memset(&req, 0, sizeof(SmsSpecAddReq));

		char temp[64] = { 0 };
		memset(&temp, 0, sizeof(temp));
		m_editCommNo.GetText(temp, sizeof(temp));
		strncpy_s(req.CommodityNo, temp);

		if (!CConvert::IsValidQuoteCommodity(req.CommodityNo))
		{
			_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ЧƷ��", TMB_OK);
			return;
		}

		req.SerialId = 0;
		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);


		m_editSpecNo.GetText(temp, sizeof(temp));
		strncpy_s(req.SpecificationNo, temp);

		memset(&temp, 0, sizeof(temp));
		m_editSpecName.GetText(temp, sizeof(temp));
		wstring w_specName = LoadRC::ansi_to_unicode(temp);
		string s_specName = LoadRC::unicode_to_ansi(w_specName, CP_UTF8);
		strncpy_s(req.SpecName, s_specName.c_str());

	

		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);
		g_pSpotSystem->AddSpec(&req);
	}
	else {	//�޸�
		SmsSpecModReq req;
		memset(&req, 0, sizeof(SmsSpecModReq));

		char temp[64] = { 0 };
		memset(&temp, 0, sizeof(temp));
		m_editCommNo.GetText(temp, sizeof(temp));
		strncpy_s(req.CommodityNo, temp);

		if (!CConvert::IsValidQuoteCommodity(req.CommodityNo))
		{
			_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ЧƷ��", TMB_OK);
			return;
		}

		strncpy_s(req.CompanyNo, m_System->m_curLogin.CompanyNo);

		m_editSpecNo.GetText(temp, sizeof(temp));
		strncpy_s(req.SpecificationNo, temp);

		//���к�
		SpecMapType::iterator it = m_System->m_mapSpec.find(req.SpecificationNo);
		if (it != m_System->m_mapSpec.end())
		{
			req.SerialId = it->second.SerialId;
		}

		memset(&temp, 0, sizeof(temp));
		m_editSpecName.GetText(temp, sizeof(temp));
		wstring w_specName = LoadRC::ansi_to_unicode(temp);
		string s_specName = LoadRC::unicode_to_ansi(w_specName, CP_UTF8);
		strncpy_s(req.SpecName, s_specName.c_str());



		strncpy_s(req.OperatorNo, m_System->m_curLogin.UserNo);

		g_pSpotSystem->ModifySpec(&req);
	}


}

void SpecWnd::OnBtnReset()
{
	//m_editSerialId.SetText("");
	m_editSpecNo.SetText("");
	m_editSpecName.SetText("");
	m_editCommNo.SetText("");
	//��񡢰�ťת��
	EnableWindow(m_editSpecNo.GetHwnd(), TRUE);
	m_bthAdd.SetButtonText("��  ��");
}


