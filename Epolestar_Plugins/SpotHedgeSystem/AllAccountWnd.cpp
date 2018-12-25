#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
#define V_TITLE				45							//̧ͷ�߶�	

enum
{
	CTP_ClientID,
	CTP_SettleDate,
	CTP_ClientEquity,
	CTP_ClientPreEquity,
	CTP_Fund_Avail,
	CTP_Margin_Occupied,
	CTP_Commission,
	CTP_MTM_PL,
	CTP_Realized_PL,
	CTP_Deposit_Withdrawal,
	CTP_End,
};
const char* cBillCtp[] =
{
	"�ͻ��� Client ID��",
	"���� Date��",
	"�ͻ�Ȩ�� Client Equity����",		//��Ȩ��
	"�ڳ���� Balance b/f��",			//��Ȩ��
	"�����ʽ� Fund Avail.��",
	"��֤��ռ�� Margin Occupied��",
	"�� �� �� Commission��",
	"�ֲֶ���ӯ�� MTM P/L��",
	"ƽ��ӯ�� Realized P/L��",
	"�� �� �� Deposit/Withdrawal��",
};



AllAccountWnd::AllAccountWnd()
	:m_System(nullptr)
{
	m_hQryFund = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hQryPosition = CreateEvent(NULL, TRUE, FALSE, NULL);
}

AllAccountWnd::~AllAccountWnd()
{

}

void AllAccountWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void AllAccountWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

LRESULT AllAccountWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void AllAccountWnd::OnCreate()
{
	InitCommonControls();
	m_hInstance = GetModuleHandle(NULL);

	//
	m_labDate.Create(m_Hwnd, L"ָ������", DT_RIGHT, ID_LabDate);
	m_TimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtDate, m_hInstance, NULL);

	//��ѯ
	m_btnQry.Create(m_Hwnd, "��  ѯ", ID_BtnQry);
	//�������˵�
	m_btnRead.Create(m_Hwnd, "�������˵�", ID_BtnRead);
	//�ı�
	m_listAllAccount.CreateList(m_Hwnd, LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER, 25, 25, LVS_EX_FULLROWSELECT, ID_ListAll);

	////////////////////////Init///////////////////////////////////
	m_listAllAccount.AddColumn(L"�˶���Ŀ", 200, -1);
	m_listAllAccount.AddColumn(L"�ױ�ϵͳ", 200, -1);
	m_listAllAccount.AddColumn(L"�ڻ�ϵͳ", 200, -1);
	m_listAllAccount.AddItem("����Ȩ��", 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem("���տ���", 1, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem("ӯ���ܼ�", 2, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem("ƽ��ӯ��", 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());

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
	m_listAllAccount.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
}

void AllAccountWnd::OnPaint()
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

void AllAccountWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{

}

LRESULT AllAccountWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void AllAccountWnd::OnNcDestroy()
{
}

void AllAccountWnd::OnDestroy()
{
	//if (IsWindow(m_addAllAccountWnd.GetHwnd()))
	//	DestroyWindow(m_addAllAccountWnd.GetHwnd());
}

void AllAccountWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = 0; pt.y = V_TITLE;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listAllAccount.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
}

void AllAccountWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
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

void AllAccountWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT p = (LPDRAWITEMSTRUCT)lParam;
	switch (p->CtlID)
	{
	case ID_ListAll:
		m_listAllAccount.OnDrawItem(lParam);
		break;
	default:
		break;
	}
}

void AllAccountWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT p = (LPMEASUREITEMSTRUCT)lParam;
	m_listAllAccount.OnMeasureItem(p->itemHeight);
}

void AllAccountWnd::OnBtnQry()
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

void AllAccountWnd::OnBtnRead()
{
	if (!m_System || strlen(m_System->m_curLogin.UserNo) == 0)
	{
		_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"���ȵ�¼�˺ţ�", TMB_OK);
		return;
	}

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };											//���ڽ����ļ���
	ofn.lStructSize = sizeof(OPENFILENAME);											//�ṹ���С
	ofn.hwndOwner = m_Hwnd;															//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	ofn.lpstrFilter = TEXT("�ı��ļ�(*.txt)\0*.txt\0\0");							//���ù���    "�����ļ�\0*.*\0�ı��ļ�(*.txt)\0*.txt\0\0"
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
			MessageBox(m_Hwnd, strFilename, TEXT("���ļ�ʧ��"), 0);
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
		//��������
		ParseAccount(v_tempTxt, fund, v_position);
		//�����ʽ�ͳֲ���ϸ
		g_pSpotSystem->AddFund(&fund);
		for (auto it : v_position)
		{
			g_pSpotSystem->AddPosition(&it);
		}
		//�ر��ļ������� 
		infile.close();    
		//_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"���ӳֲֳɹ���", TMB_OK);
	}
}

void AllAccountWnd::GenerateList()
{
	//����
	m_listAllAccount.DeleteAllItems();
	m_listAllAccount.AddItem("����Ȩ��", 0, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem("���տ���", 1, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem("ӯ���ܼ�", 2, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem("ƽ��ӯ��", 3, 0, DT_CENTER, g_ColorRefData.GetColorBlack());

	//�ױ��ʽ�
	m_listAllAccount.AddItem(0.1, 0, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem(0.2, 1, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem(0.3, 2, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem(0.4, 3, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	//�ױ��ֲ�

	//�ڻ��ʽ�
	SmsFundDataRsp	tempFund = m_System->m_vecFund.at(0);
	m_listAllAccount.AddItem(tempFund.Equity, 0, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem(tempFund.Available, 1, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem(tempFund.CloseProfit + tempFund.PositionProfit, 2, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	m_listAllAccount.AddItem(tempFund.CloseProfit, 3, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());

	int index = 3;
	//�ڻ��ֲ�
	for (auto it : m_System->m_vecPosition)
	{
		string sCode = it.ContractNo;
		string tCode;
		CConvert::SContractNoToTContractNo(sCode, tCode);
		m_listAllAccount.AddItem(tCode.c_str(), ++index, 0, DT_CENTER, g_ColorRefData.GetColorBlack());
		m_listAllAccount.AddItem(it.PositionProfit, index, 1, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
		m_listAllAccount.AddItem(it.PositionProfit, index, 2, 2, DT_CENTER, g_ColorRefData.GetColorBlack());
	}


}

void AllAccountWnd::ParseAccount(vector<string>& v_CtpTxt, SmsFundDataRsp & fund, vector<SmsPositionDataRsp>& v_pos)
{
	//////////////////////////////////�����ʽ�/////////////////////////////////////////
	//���к�
	fund.SerialId = 0;
	//��������
	string settleDate;
	getFundString(v_CtpTxt.at(5), CTP_SettleDate, settleDate);
	if (settleDate.length() < 8)
	{
		_TMessageBox_Domodal(m_Hwnd, "����", L"�ʽ��������", TMB_OK);
		return;
	}
	settleDate.insert(6, "-");
	settleDate.insert(4, "-");
	strncpy_s(fund.SettleDate, settleDate.c_str());
	//��˾���
	strncpy_s(fund.CompanyNo, m_System->m_curLogin.CompanyNo);
	//�ͻ���
	string userNo;
	getFundString(v_CtpTxt.at(4), CTP_ClientID, userNo);
	strncpy_s(fund.UserNo,userNo.c_str());
	//Ȩ��
	string equity;
	getFundString(v_CtpTxt.at(15), CTP_ClientEquity, equity);
	fund.Equity = atof(equity.c_str());
	//��Ȩ��
	string preEquity;
	getFundString(v_CtpTxt.at(12), CTP_ClientPreEquity, preEquity);
	fund.PreEquity = atof(preEquity.c_str());
	//����
	string available;
	getFundString(v_CtpTxt.at(22), CTP_Fund_Avail, available);
	fund.Available = atof(available.c_str());
	//�����?
	//��֤��
	string margin;
	getFundString(v_CtpTxt.at(17), CTP_Margin_Occupied, margin);
	fund.Margin = atof(margin.c_str());
	//������
	string fee;
	getFundString(v_CtpTxt.at(17), CTP_Commission, fee);
	fund.Fee = atof(fee.c_str());
	//�ֲ�����
	string positionProfit;
	getFundString(v_CtpTxt.at(15), CTP_MTM_PL, positionProfit);
	fund.PositionProfit = atof(positionProfit.c_str());
	//ƽ������
	string closeProfit;
	getFundString(v_CtpTxt.at(14), CTP_Realized_PL, closeProfit);
	fund.CloseProfit = atof(closeProfit.c_str());
	//��������?
	//�����
	string cash;
	getFundString(v_CtpTxt.at(13), CTP_Deposit_Withdrawal, cash);
	double cashInOut = atof(cash.c_str());
	if (cashInOut >= 0)
		fund.CashIn = cashInOut;
	else
		fund.CashOut = cashInOut;
	//����Ա
	strncpy_s(fund.OperatorNo, m_System->m_curLogin.UserNo);

	//////////////////////////////////�����ֲ���ϸ/////////////////////////////////////////
	int index_begin = 25;						//�ʽ����һ��
	int index_end = v_CtpTxt.size();
	for (; index_begin < index_end; ++index_begin)
	{
		string::size_type pos = v_CtpTxt.at(index_begin).find("�ֲ���ϸ Positions Detail");
		if (pos != string::npos)
			break;
	}
	int  pos_begin = index_begin + 5;
	if (pos_begin >= index_end)
	{
		_TMessageBox_Domodal(m_Hwnd, "����", L"�ֲ���ϸ��������", TMB_OK);
		return;
	}
	for (; pos_begin < index_end;)
	{
		if (dealPositionString(v_CtpTxt.at(pos_begin), v_pos, userNo, settleDate))
			++pos_begin;
		else
			break;
	}
}

void AllAccountWnd::TrimString(string & str)
{
	string::size_type pos = str.find_first_not_of(' ');
	if (pos == string::npos)
	{
		return;
	}
	string::size_type pos2 = str.find_last_not_of(' ');
	if (pos2 != string::npos)
	{
		str = str.substr(pos, pos2 - pos + 1);
		return;
	}
	str = str.substr(pos);
	return;
}

std::vector<std::string> AllAccountWnd::SplitString(const std::string & str, char cSplit)
{
	vector<string> vecstr;
	string tempstr(str);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), cSplit);
		if (tempit != tempstr.end())
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempstr.begin(), tempit);
			vecstr.push_back(tempstr.substr(0, dis));
			if (++tempit != tempstr.end())
			{
				tempstr.erase(tempstr.begin(), tempit);
				tempit = tempstr.begin();
			}
			else
				break;
		}
		else//û�ҵ�' '��������  ����abc;������ȻҪ����
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}

void AllAccountWnd::getFundString(string & src, int nID, string & out)
{
	string::size_type pos = src.find(cBillCtp[nID]);
	if (pos == string::npos)
		return;
	string s_sub = src.substr(pos + strlen(cBillCtp[nID]));
	string::size_type pos_r = s_sub.find("\r");
	if (pos_r == string::npos)
		return;
	s_sub = s_sub.substr(0, pos_r);
	TrimString(s_sub);
	string::size_type pos_nop = s_sub.find_first_of(' ');
	if (pos_nop == string::npos)
	{
		out = s_sub;
	}
	else
	{
		s_sub = s_sub.substr(0, pos_nop);
		out = s_sub;
	}
}

bool AllAccountWnd::dealPositionString(string & src, vector<SmsPositionDataRsp>& v_pos, string& userno, string& settleDate)
{
	vector<string> v_tPosition;
	v_tPosition.clear();
	v_tPosition = SplitString(src, '|');
	if (v_tPosition.size() <= 1)
		return false;

	for (int i = 0; i < v_tPosition.size(); i++)
	{
		TrimString(v_tPosition.at(i));
	}

	SmsPositionDataRsp tempPos;
	memset(&tempPos, 0, sizeof(SmsPositionDataRsp));
	//���к�
	tempPos.SerialId = 0;
	//��������
	//v_tPosition.at(4).insert(6, "-");
	//v_tPosition.at(4).insert(4, "-");
	strncpy_s(tempPos.SettleDate, settleDate.c_str());
	//��˾���
	strncpy_s(tempPos.CompanyNo, m_System->m_curLogin.CompanyNo);
	//����Ա���
	strncpy_s(tempPos.UserNo, userno.c_str());

	//��Լ���
	getContractNo(v_tPosition.at(1), v_tPosition.at(3));
	TContractKey conKey;
	memset(&conKey, 0, sizeof(TContractKey));
	CConvert::TContractNoToTContractKey(v_tPosition.at(3).c_str(), conKey);
	char sCode[101] = { 0 };
	CConvert::TContractKeyToSContractNo(conKey, sCode);
	strncpy_s(tempPos.ContractNo, sCode);

	//�ֲ�����		
	tempPos.PositionQty = atoi(v_tPosition.at(7).c_str());

	//��������		
	if (strcmp("��", v_tPosition.at(6).c_str()) == 0)
		tempPos.Direct = SMS_DIRECT_BUY;
	else if (strcmp("��", v_tPosition.at(6).c_str()) == 0)
		tempPos.Direct = SMS_DIRECT_SELL;
	else
		tempPos.Hedge = SMS_DIRECT_both;

	//Ͷ������		
	if (strcmp("Ͷ", v_tPosition.at(5).c_str()) == 0)
		tempPos.Hedge = SMS_HEDGE_SPECULATE;
	else if (strcmp("��", v_tPosition.at(5).c_str()) == 0)
		tempPos.Hedge = SMS_HEDGE_HEDGE;
	else
		tempPos.Hedge = SMS_HEDGE_NONE;

	//�ֲּ۸�		
	tempPos.PositionPrice = atof(v_tPosition.at(8).c_str());
    //�ֲ�����		
	tempPos.PositionProfit = atof(v_tPosition.at(11).c_str());
	//����Ա		
	strncpy_s(tempPos.OperatorNo, m_System->m_curLogin.UserNo);

	v_pos.push_back(tempPos);
	return true;
}

void AllAccountWnd::getContractNo(string & exchange, string & instrument)
{
	for (int i = 0; i < instrument.size(); i++)
	{
		char ch = instrument.at(i);
		if (ch >= 'a' && ch <= 'z')
			instrument.at(i) = toupper(ch);
		else if (ch >= 'A' && ch <= 'Z')
			continue;
		else
		{
			instrument.insert(i, 1, ' ');
			break;
		}
	}
	if (strcmp(exchange.c_str(), "֣����") == 0)
		instrument.insert(0, "ZCE ");
	else if (strcmp(exchange.c_str(), "��Դ����") == 0)
		instrument.insert(0,"INE ");
	else if (strcmp(exchange.c_str(), "������") == 0)
		instrument.insert(0,"SHFE ");
	else if (strcmp(exchange.c_str(), "�н���") == 0)
		instrument.insert(0,"CFFEX ");
	else if (strcmp(exchange.c_str(), "������") == 0)
		instrument.insert(0,"DCE ");
}
