#include "PreInclude.h"
extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
extern G_COLOREF			g_ColorRefData;				//ɫ��
AgreementRelWnd::AgreementRelWnd()
{

}

AgreementRelWnd::~AgreementRelWnd()
{

}

void AgreementRelWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

LRESULT AgreementRelWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void AgreementRelWnd::OnCreate()
{
	//���
	m_listAgreementRel.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListView_AgreeementRel);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listAgreementRel.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	//============================�����б�=============================
	m_listAgreementRel.AddColumn(L"���к�", 80, -1);
	m_listAgreementRel.AddColumn(L"��˾���", 80, -1);
	m_listAgreementRel.AddColumn(L"���ֺ�ͬ���", 130, -1);
	m_listAgreementRel.AddColumn(L"ƽ�ֺ�ͬ���", 130, -1);
	m_listAgreementRel.AddColumn(L"�û����", 80, -1);
	m_listAgreementRel.AddColumn(L"����Ա", 80, -1);
	m_listAgreementRel.AddColumn(L"�޸�ʱ��", 130, -1);

	m_menuAgreementRel = CreatePopupMenu();

	AppendMenu(m_menuAgreementRel, MF_STRING, IDM_AddAgreementRel, L"¼���ͬ��ϵ");
	AppendMenu(m_menuAgreementRel, MF_SEPARATOR, IDM_Line, L"");
	AppendMenu(m_menuAgreementRel, MF_STRING, IDM_ModAgreementRel, L"�޸ĺ�ͬ��ϵ");
	AppendMenu(m_menuAgreementRel, MF_STRING, IDM_DelAgreementRel, L"ɾ����ͬ��ϵ");
}

void AgreementRelWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
}

void AgreementRelWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == 0 && lParam == 0)//�����ǲ˵�
	{
		switch (LOWORD(wParam))
		{
		case IDM_AddAgreementRel:	//¼���ͬ��ϵ
			//if (!IsWindow(m_addAgreementWnd.GetHwnd()))
			//{
			//	m_addAgreementWnd.ShowFrm(m_Hwnd);
			//}
			//else
			//{
			//	if (!IsWindowVisible(m_addAgreementWnd.GetHwnd()))
			//		ShowWindow(m_addAgreementWnd.GetHwnd(), SW_SHOW);
			//	m_addAgreementWnd.InitWnd();
			//}
			//m_addAgreementWnd.Regist(m_stUserInfo1.Sign, m_stUserInfo2.Sign);
			break;
		case IDM_ModAgreementRel://�޸ĺ�ͬ��ϵ
			//if (!IsWindow(m_addAgreementWnd.GetHwnd()))
			//{
			//	m_addAgreementWnd.ShowFrm(m_Hwnd);
			//}
			//else
			//{
			//	if (!IsWindowVisible(m_addAgreementWnd.GetHwnd()))
			//		ShowWindow(m_addAgreementWnd.GetHwnd(), SW_SHOW);
			//	m_addAgreementWnd.InitWnd();
			//}
			break;
		case IDM_DelAgreementRel://ɾ����ͬ��ϵ
			//DealDelAgreement();
			break;
		default:
			break;
		}
	}
}

void AgreementRelWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListView_AgreeementRel:
		m_listAgreementRel.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void AgreementRelWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ListView_AgreeementRel:
		DealAgreeList(lParam);
		break;
	default:
		break;
	}
}

LRESULT AgreementRelWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void AgreementRelWnd::OnNcDestroy()
{
}

void AgreementRelWnd::OnDestroy()
{
	//if (IsWindow(m_addAgreementWnd.GetHwnd()))
	//	DestroyWindow(m_addAgreementWnd.GetHwnd());
}

void AgreementRelWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listAgreementRel.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

void AgreementRelWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listAgreementRel.OnMeasureItem(p->itemHeight);
}

void AgreementRelWnd::DealAgreeList(LPARAM lParam)
{
	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE)lParam;
	if (p->hdr.code == NM_RCLICK)
	{
		EnableMenu(p->hdr.idFrom, true);
		if (p->iItem >= m_listAgreementRel.GetItemCount() || p->iItem == -1)
		{
			EnableMenuItem(m_menuAgreementRel, IDM_ModAgreementRel, MF_GRAYED);
			EnableMenuItem(m_menuAgreementRel, IDM_DelAgreementRel, MF_GRAYED);
		}

		ShowMenu(m_menuAgreementRel);
	}
}

void AgreementRelWnd::ShowMenu(HMENU hmenu)
{
	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_Hwnd, NULL);
}

void AgreementRelWnd::EnableMenu(uint id, bool bEnable)
{
	UINT flage = MF_GRAYED;
	if (bEnable)
		flage = MF_ENABLED;

	for (uint i = IDM_AddAgreementRel; i <= IDM_DelAgreementRel; i++)
		EnableMenuItem(m_menuAgreementRel, i, flage);
}
