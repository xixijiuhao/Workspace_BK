#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
#define V_TITLE				45							//̧ͷ�߶�	

PartAccountWnd::PartAccountWnd()
	:m_System(nullptr)
{
	m_hQryFund = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hQryPosition = CreateEvent(NULL, TRUE, FALSE, NULL);
}

PartAccountWnd::~PartAccountWnd()
{

}

void PartAccountWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void PartAccountWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

LRESULT PartAccountWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
		//case WM_NOTIFY:
		//	OnNotify(wParam, lParam);
		//	return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_LIST_LBUTTONDBCLK:
		//OnListDoubleClick(wParam, lParam);
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_DRAWITEM:
		OnDrawItem(wParam, lParam);
		return PROCESSED;
	case WM_MEASUREITEM://==7
		OnMeasureItem(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_PAINT:
		OnPaint();
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

void PartAccountWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);

	//
	m_labDate.Create(m_Hwnd, L"ָ������", DT_RIGHT, ID_LabDate);
	m_TimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtDate, m_hInstance, NULL);

	//��ѯ
	m_btnQry.Create(m_Hwnd, "��  ѯ", ID_BtnQry);
	//������˵�
	m_btnRead.Create(m_Hwnd, "������˵�", ID_BtnRead);
	//���
	m_listPartAccount.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListPart);

	/////////////////////////Init///////////////////////////
	m_listPartAccount.AddColumn(L"�˶���Ŀ", 200, -1);
	m_listPartAccount.AddColumn(L"�ױ�ϵͳ", 200, -1);
	m_listPartAccount.AddColumn(L"�ڻ�ϵͳ", 200, -1);
	m_listPartAccount.AddItem("����Ȩ��", 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("���տ���", 1, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("ӯ���ܼ�", 2, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("ƽ��ӯ��", 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());

	m_labDate.SetFont(g_FontData.GetFontWord13());
	//m_richeditAll.SetFont(g_FontData.GetFontWord15());

	m_cbtDate.Init(m_TimePick);
	SendMessage(m_TimePick, WM_SETFONT, (WPARAM)g_FontData.GetFontNum15(), (LPARAM)TRUE);
	DateTime_SetFormat(m_TimePick, L"yyyy-MM-dd");
	SYSTEMTIME time;
	GetLocalTime(&time);
	DateTime_SetSystemtime(m_cbtDate.GetHwnd(), GDT_VALID, &time);

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
	m_labDate.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbtDate.MoveWindow(pt.x, pt.y, editWidth, Height);
	pt.x += editWidth + htap * 3;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnRead.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = 0; pt.y = V_TITLE;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listPartAccount.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
}

void PartAccountWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());

	HPEN  hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	SelectObject(mdc.GetHdc(), hPen);
	MoveToEx(mdc.GetHdc(), rect.left, V_TITLE - 1, 0);
	LineTo(mdc.GetHdc(), rect.right, V_TITLE - 1);
	DeleteObject(hPen);
}

void PartAccountWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

LRESULT PartAccountWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void PartAccountWnd::OnNcDestroy()
{
}

void PartAccountWnd::OnDestroy()
{
	//if (IsWindow(m_addPartAccountWnd.GetHwnd()))
	//	DestroyWindow(m_addPartAccountWnd.GetHwnd());
}

void PartAccountWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = 0; pt.y = V_TITLE;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listPartAccount.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);

}

void PartAccountWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	case ID_BtnRead:
		OnBtnRead();
		break;
	default:
		break;
	}
}

void PartAccountWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListPart:
		m_listPartAccount.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void PartAccountWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listPartAccount.OnMeasureItem(p->itemHeight);
}

void PartAccountWnd::OnBtnQry()
{
	ResetEvent(m_hQryFund);
	ResetEvent(m_hQryPosition);
	//ResetEvent(m_hQry[0]);
	//ResetEvent(m_hQry[1]);

	//��ɽ�
	SmsMatchQryReq matchReq;
	memset(&matchReq, 0, sizeof(SmsMatchQryReq));
	//���ʽ�
	SmsFundQryReq fundReq;
	memset(&fundReq, 0, sizeof(SmsFundQryReq));
	//��ֲ�
	SmsPositionQryReq positionReq;
	memset(&positionReq, 0, sizeof(SmsPositionQryReq));

	char temp[MAX_PATH] = { 0 };
	//���к�
	matchReq.SerialId = 0;
	fundReq.SerialId = 0;
	positionReq.SerialId = 0;

	if (!m_System)return;

	//��˾
	strncpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strncpy_s(fundReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	strncpy_s(positionReq.CompanyNo, m_System->m_curLogin.CompanyNo);

	//�û�
	strncpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);
	strncpy_s(fundReq.UserNo, m_System->m_curLogin.UserNo);
	strncpy_s(positionReq.UserNo, m_System->m_curLogin.UserNo);

	wstring tempDate;
	m_cbtDate.CxGetWindowText(tempDate);
	//��ʼ����
	strncpy_s(matchReq.BeginDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	strncpy_s(fundReq.BeginDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	strncpy_s(positionReq.BeginDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	//��ֹ����
	strncpy_s(matchReq.EndDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	strncpy_s(fundReq.EndDate, LoadRC::unicode_to_ansi(tempDate).c_str());
	strncpy_s(positionReq.EndDate, LoadRC::unicode_to_ansi(tempDate).c_str());

	m_System->m_matchWnd->m_listMatch.DeleteAllItems();
	m_System->m_fundWnd->m_listFund.DeleteAllItems();
	m_System->m_positionWnd->m_listPosition.DeleteAllItems();

	
	m_System->m_mapMatch.clear();

	m_System->m_vecFund.clear();
	m_System->m_vecPosition.clear();


	//g_pSpotSystem->QryMatch(&matchReq);
	//WaitForMultipleObjects(2, m_hQry, TRUE, 10000);
	g_pSpotSystem->QryFund(&fundReq);
	WaitForSingleObject(m_hQryFund, 2000);
	if (m_System->m_vecFund.empty())
	{
		_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ָ�������ʽ𲻴��ڣ�", TMB_OK);
		return;
	}

	g_pSpotSystem->QryPosition(&positionReq);
	WaitForSingleObject(m_hQryPosition, 2000);
	if (m_System->m_vecPosition.empty())
	{
		_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"ָ�����ڳֲֲ����ڣ�", TMB_OK);
		return;
	}

	//���ɱ��
	GenerateList();
}

void PartAccountWnd::OnBtnRead()
{
	if (!m_System || strlen(m_System->m_curLogin.UserNo) == 0)
	{
		_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"���ȵ�¼�˺ţ�", TMB_OK);
		return;
	}

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };											//���ڽ����ļ���
	ofn.lStructSize = sizeof(OPENFILENAME);											//�ṹ���С
	ofn.hwndOwner = NULL;															//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	ofn.lpstrFilter = TEXT("�ı��ļ�(*.txt)\0*.txt\0\0");			//���ù���    "�����ļ�\0*.*\0�ı��ļ�(*.txt)\0*.txt\0\0"
	ofn.nFilterIndex = 1;															//����������
	ofn.lpstrFile = strFilename;													//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
	ofn.nMaxFile = sizeof(strFilename);												//����������
	ofn.lpstrInitialDir = NULL;														//��ʼĿ¼ΪĬ��
	ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");										//ʹ��ϵͳĬ�ϱ������ռ���
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;			//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
	if (GetOpenFileName(&ofn))
	{
		//MessageBox(NULL, strFilename, TEXT("ѡ����ļ�"), 0);
		//����CTP���˵�
		std::ifstream infile;
		infile.open(strFilename);   //���ļ����������ļ��������� 
									//assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 
		if (!infile)
		{
			MessageBox(NULL, strFilename, TEXT("���ļ�ʧ��"), 0);
			return;
		}
		vector<string> v_tempTxt;
		string s;
		while (getline(infile, s))
		{
			//OutputDebugStringA(s.c_str());.
			v_tempTxt.push_back(s);
		}
		SmsFundDataRsp fund;
		memset(&fund, 0, sizeof(SmsFundDataRsp));
		vector<SmsPositionDataRsp>	v_position;
		v_position.clear();
		////��������
		//ParseAccount(v_tempTxt, fund, v_position);
		////�����ʽ�ͳֲ���ϸ
		//g_pSpotSystem->AddFund(&fund);
		//for (auto it : v_position)
		//{
		//	g_pSpotSystem->AddPosition(&it);
		//}
		//�ر��ļ������� 
		infile.close();
	}
}

void PartAccountWnd::GenerateList()
{
	//����
	m_listPartAccount.DeleteAllItems();
	m_listPartAccount.AddItem("����Ȩ��", 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("���տ���", 1, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("ӯ���ܼ�", 2, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem("ƽ��ӯ��", 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());

	//�ױ��ʽ�
	m_listPartAccount.AddItem(0.1, 0, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(0.2, 1, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(0.3, 2, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(0.4, 3, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ױ��ֲ�

	//�ڻ��ʽ�
	SmsFundDataRsp	tempFund = m_System->m_vecFund.at(0);
	m_listPartAccount.AddItem(tempFund.Equity, 0, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(tempFund.Available, 1, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(tempFund.CloseProfit + tempFund.PositionProfit, 2, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listPartAccount.AddItem(tempFund.CloseProfit, 3, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());

	int index = 3;
	//�ڻ��ֲ�
	for (auto it : m_System->m_vecPosition)
	{
		string sCode = it.ContractNo;
		string tCode;
		CConvert::SContractNoToTContractNo(sCode, tCode);
		m_listPartAccount.AddItem(tCode.c_str(), ++index, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
		m_listPartAccount.AddItem(it.PositionProfit, index, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
		m_listPartAccount.AddItem(it.PositionProfit, index, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	}

	
}
