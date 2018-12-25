#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//�ױ�ƽ̨
extern G_COLOREF			g_ColorRefData;				//ɫ��
extern G_UIFONT				g_FontData;					//����
extern IStarApi            *g_pStarApi;					//���� 
#define V_TITLE				45							//̧ͷ�߶�	
	
#define LEFT_TOP			"��"
#define LEFT_BOTTOM			"��"
#define RIGHT_TOP			"��"
#define RIGHT_BOTTOM		"��"
#define LATERAL				"��"
#define VERTICAL			"��"
#define LEFT_CENTER			"��"
#define RIGHT_CENTER		"��"
#define TOP_CENTER			"��"
#define BOTTOM_CENTER		"��"
#define CENTER				"��"
#define SPACE				"  "


// AutoWrap() - Automation helper function...
HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...) {
	// Begin variable-argument list...
	va_list marker;
	va_start(marker, cArgs);

	if (!pDisp) {
		MessageBoxA(NULL, "NULL IDispatch passed to AutoWrap()", "Error", 0x10010);
		_exit(0);
	}

	// Variables used...
	DISPPARAMS dp = { NULL, NULL, 0, 0 };
	DISPID dispidNamed = DISPID_PROPERTYPUT;
	DISPID dispID;
	HRESULT hr;
	char buf[200];
	char szName[200];

	// Convert down to ANSI
	WideCharToMultiByte(CP_ACP, 0, ptName, -1, szName, 256, NULL, NULL);

	// Get DISPID for name passed...
	hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
	if (FAILED(hr)) {
		sprintf(buf, "IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx", szName, hr);
		MessageBoxA(NULL, buf, "AutoWrap()", 0x10010);
		_exit(0);
		return hr;
	}

	// Allocate memory for arguments...
	VARIANT *pArgs = new VARIANT[cArgs + 1];
	// Extract arguments...
	for (int i = 0; i < cArgs; i++) {
		pArgs[i] = va_arg(marker, VARIANT);
	}

	// Build DISPPARAMS
	dp.cArgs = cArgs;
	dp.rgvarg = pArgs;

	// Handle special-case for property-puts!
	if (autoType & DISPATCH_PROPERTYPUT) {
		dp.cNamedArgs = 1;
		dp.rgdispidNamedArgs = &dispidNamed;
	}

	// Make the call!
	hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
	if (FAILED(hr)) {
		sprintf(buf, "IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx", szName, dispID, hr);
		MessageBoxA(NULL, buf, "AutoWrap()", 0x10010);
		_exit(0);
		return hr;
	}
	// End variable-argument section...
	va_end(marker);

	delete[] pArgs;
	return hr;
}

IntegrationWnd::IntegrationWnd()
	:m_System(nullptr), m_spotAllProfit(0.0), m_allFee(0.0), m_matchAllProfit(0.0),
	m_tradeDot(0.0), m_minorAgreeQty(0), m_minorAgreeTotal(0.0), m_spotTotalProfit(0.0),
	m_futureQty(0), m_openMatchTotal(0.0), m_closeMatchTotal(0.0), m_totalFee(0.0), m_matchTotalProfit(0.0)
{
	m_hQryMatch = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hQrySpecPrice = CreateEvent(NULL, TRUE, FALSE, NULL);
	//��ʼ�������ָ���
	InitSplit();
}

IntegrationWnd::~IntegrationWnd()
{
	m_RowinfoContainer.clear();
}

void IntegrationWnd::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void IntegrationWnd::Register(SystemFrame * pdata)
{
	if (!pdata)
		return;
	m_System = pdata;
}

LRESULT IntegrationWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void IntegrationWnd::OnCreate()
{
	InitCommonControls(); 
	m_hInstance = GetModuleHandle(NULL);

	//��ʼ����
	m_labBeginDate.Create(m_Hwnd, L"��ʼ����", DT_RIGHT, ID_LabBeginDate);
	m_beginTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtBeginDate, m_hInstance, NULL);
	//��������
	m_labEndDate.Create(m_Hwnd, L"��������", DT_RIGHT, ID_LabEndDate);
	m_endTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtEndDate, m_hInstance, NULL);
	//��ͬ
	m_labAgree.Create(m_Hwnd, L"��ͬ���", DT_CENTER, ID_LabAgree);
	m_cbAgree.Create(m_Hwnd, ID_CbAgree);
	//�˺�
	m_labUserno.Create(m_Hwnd, L"�û��˺�", DT_CENTER, ID_LabUserno);
	m_cbUserno.Create(m_Hwnd, ID_CbUserno);
	//��ѯ
	m_btnQry.Create(m_Hwnd, "��  ѯ", ID_BtnQry);
	//����
	m_btnSave.Create(m_Hwnd, "��  ��", ID_BtnSave);
	//���˵�
	m_richeditAll.Create(m_Hwnd, ES_LEFT | ES_READONLY, 0, ID_EditAll);
	//EnableWindow(m_richeditAll.GetHwnd(), false);
	//���˱�
	m_listAll.Create(m_Hwnd, L"AccountBillListWnd", AccountCol);

	m_labBeginDate.SetFont(g_FontData.GetFontWord13());
	m_labEndDate.SetFont(g_FontData.GetFontWord13());
	m_labAgree.SetFont(g_FontData.GetFontWord13());
	m_labUserno.SetFont(g_FontData.GetFontWord13());


	m_cbAgree.SetFont(g_FontData.GetFontNum15());
	m_cbUserno.SetFont(g_FontData.GetFontNum15());
	m_richeditAll.SetFont(g_FontData.GetFontWordRichEdit());

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
	pt.x += editWidth + htap;
	m_labAgree.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbAgree.MoveWindow(pt.x, pt.y, editWidth, Height * 8);
	pt.x += editWidth + htap;
	m_labUserno.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + htap;
	m_cbUserno.MoveWindow(pt.x, pt.y, editWidth, Height * 8);

	pt.x += editWidth + htap * 3;
	m_btnQry.MoveWindow(pt.x, pt.y, btnWidth, Height);
	pt.x += btnWidth + htap * 3;
	m_btnSave.MoveWindow(pt.x, pt.y, btnWidth, Height);

	pt.x = 0; pt.y = V_TITLE;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	m_listAll.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
	//m_richeditAll.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
}

void IntegrationWnd::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());

	HPEN  hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	SelectObject(mdc.GetHdc(), hPen);
	MoveToEx(mdc.GetHdc(), rect.left, V_TITLE-1, 0);
	LineTo(mdc.GetHdc(), rect.right, V_TITLE-1);
	DeleteObject(hPen);
}

void IntegrationWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == ID_CbAgree && HIWORD(wParam) == CBN_SELCHANGE)
	{
		int index = m_cbAgree.GetSelect();
		if (index == 0)
		{
			EnableWindow(m_labBeginDate.GetHwnd(), TRUE);
			EnableWindow(m_cbtBeginDate.GetHwnd(), TRUE);
			EnableWindow(m_labEndDate.GetHwnd(), TRUE);
			EnableWindow(m_cbtEndDate.GetHwnd(), TRUE);
			EnableWindow(m_labUserno.GetHwnd(), TRUE);
			EnableWindow(m_cbUserno.GetHwnd(), TRUE);
		}
		else
		{
			EnableWindow(m_labBeginDate.GetHwnd(), FALSE);
			EnableWindow(m_cbtBeginDate.GetHwnd(), FALSE);
			EnableWindow(m_labEndDate.GetHwnd(), FALSE);
			EnableWindow(m_cbtEndDate.GetHwnd(), FALSE);
			EnableWindow(m_labUserno.GetHwnd(), FALSE);
			EnableWindow(m_cbUserno.GetHwnd(), FALSE);
			m_cbUserno.SetSelect(0);
		}
	}

}

LRESULT IntegrationWnd::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.GetColorBackground();
}

void IntegrationWnd::OnNcDestroy()
{
}

void IntegrationWnd::OnDestroy()
{
	//if (IsWindow(m_addIntegrationWnd.GetHwnd()))
	//	DestroyWindow(m_addIntegrationWnd.GetHwnd());
}

void IntegrationWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = 0; pt.y = V_TITLE;
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	//m_richeditAll.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
	m_listAll.MoveWindow(pt.x, pt.y, rect.right - rect.left, rect.bottom - rect.top - V_TITLE);
}

void IntegrationWnd::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BtnQry:
		OnBtnQry();
		break;
	case ID_BtnSave:
		OnBtnSave();
		break;
	default:
		break;
	}
}

void IntegrationWnd::OnBtnQry()
{	
	if (!m_System)return;

	//HFONT hfont =  CreateFont(-14, 0, 0, 0, 0, FALSE, FALSE, FALSE, GB2312_CHARSET, 0, 0, 0, 0, L"����");
	//m_richeditAll.SetFont(hfont);

	//��ɽ�
	SmsMatchQryReq matchReq;
	memset(&matchReq, 0, sizeof(SmsMatchQryReq));
	//���ֻ��۸�
	SmsSpecPriceQryReq specPriceReq;
	memset(&specPriceReq, 0, sizeof(SmsSpecPriceQryReq));
	strncpy_s(specPriceReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	SYSTEMTIME	curdate;
	memset(&curdate, 0, sizeof(SYSTEMTIME));
	GetSystemTime(&curdate);
	char date[21] = { 0 };
	sprintf_s(date, "%d-%02d-%02d", curdate.wYear, curdate.wMonth, curdate.wDay);
	strncpy_s(specPriceReq.EndDate, date);
	//strncpy_s(specPriceReq.BeginDate, "2018-01-01");
	
	//������
	m_map_Agree.clear();
	m_map_mAgreeMatch.clear();
	m_map_sAgreeRel.clear();
	m_map_specPrice.clear();

	//���к�
	matchReq.SerialId = 0;
	//��˾
	strncpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	//�û���
	wchar_t	userno[MAX_PATH] = { 0 };
	m_cbUserno.GetText(userno);
	//strncpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);
	strncpy_s(matchReq.UserNo,LoadRC::unicode_to_ansi(userno).c_str());

	//������
	if (IsWindowEnabled(m_labBeginDate.GetHwnd()))
	{
		//��ʼ����
		wstring beginDate;
		m_cbtBeginDate.CxGetWindowText(beginDate);
		//��ֹ����
		wstring endDate;
		m_cbtEndDate.CxGetWindowText(endDate);

		int begin = DateStrToInt(LoadRC::unicode_to_ansi(beginDate));
		int end = DateStrToInt(LoadRC::unicode_to_ansi(endDate));

		string key = m_System->get_curkey();
		
		//ָ������ȫ����ͬ
		vector<SmsAgreementDataRsp>		t_vec_buyAgree;
		vector<SmsAgreementDataRsp>		t_vec_sellAgree;
		t_vec_buyAgree.clear();
		t_vec_sellAgree.clear();

		for (auto it : m_System->m_map_mapAgreement[key])
		{
			if (strcmp(matchReq.UserNo, it.second.UserNo) != 0 && strcmp(matchReq.UserNo, m_System->m_curLogin.UserNo) != 0)
			{
				continue;
			}
			if (DateStrToInt(it.second.SignDate) >= begin && DateStrToInt(it.second.SignDate) <= end)
			{
				if (it.second.AgreementType == SMS_AGREEMENT_PURCHASE)
					t_vec_buyAgree.push_back(it.second);
				else
					t_vec_sellAgree.push_back(it.second);
			}
		}
		//�ҵ���Ҫ��ͬ
		set<string>							m_set_mainAgreeno;
		m_set_mainAgreeno.clear();

		for (auto it_buy : t_vec_buyAgree)
		{
			bool found = false;
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (strcmp(it_buy.AgreementNo, it_rel.BuyAgreementNo) == 0)
				{
					found = true;
					if (m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo].SpotQty >= m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo].SpotQty)
						m_set_mainAgreeno.insert(it_rel.BuyAgreementNo);
					else
						m_set_mainAgreeno.insert(it_rel.SellAgreementNo);
					break;
				}
			}
			if (!found)
				m_set_mainAgreeno.insert(it_buy.AgreementNo);
		}
		for (auto it_sell : t_vec_sellAgree)
		{
			bool found = false;
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (strcmp(it_sell.AgreementNo, it_rel.SellAgreementNo) == 0)
				{
					found = true;
					if (m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo].SpotQty >= m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo].SpotQty)
						m_set_mainAgreeno.insert(it_rel.SellAgreementNo);
					else
						m_set_mainAgreeno.insert(it_rel.BuyAgreementNo);
					break;
				}
			}
			if (!found)
				m_set_mainAgreeno.insert(it_sell.AgreementNo);
		}

		//������Ҫ��ͬ����
		for (auto it_main_agree : m_set_mainAgreeno)
		{
			m_map_Agree[it_main_agree] = m_System->m_map_mapAgreement[key][it_main_agree];
		}

		//��Ҫ��ͬ��Ӧ�����д�Ҫ��ͬ
		for (auto it_main_agreeno : m_set_mainAgreeno)
		{
			set<SmsAgreementDataRsp> m_setMinorAgree;
			m_setMinorAgree.clear();
			//һ�����ֺ�ͬ�������ƽ�ֺ�ͬ
			FindMinorAgree(it_main_agreeno, m_setMinorAgree);
			//<�������ݣ�<ƽ������>>
			m_map_sAgreeRel[m_map_Agree[it_main_agreeno]] = m_setMinorAgree;
		}
		//��ѯ�ɽ�
		for (auto it_req : m_map_Agree)
		{
			ResetEvent(m_hQryMatch);
			strncpy_s(matchReq.AgreementNo, it_req.first.c_str());

			//m_System->m_matchWnd->m_listMatch.DeleteAllItems();
			m_System->m_mapMatch.clear();

			g_pSpotSystem->QryMatch(&matchReq);
			DWORD signal = WaitForSingleObject(m_hQryMatch, 5000);
			if (signal == WAIT_TIMEOUT)
			{
				_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ѯ�ɽ���ʱ", TMB_OK);
				return;
			}

			if (!m_System->m_mapMatch.empty())
			{
				m_map_mAgreeMatch[matchReq.AgreementNo] = m_System->m_mapMatch;
			}
		}
		//��ѯ�ֻ��۸�
		for (auto it_req : m_set_mainAgreeno)
		{
			string specNo("");
			specNo = m_map_Agree[it_req].SpecificationNo;

			ResetEvent(m_hQrySpecPrice);
			strncpy_s(specPriceReq.SpecificationNo, specNo.c_str());

			//m_System->m_specSpriceWnd->m_listSpecPrice.DeleteAllItems();

			g_pSpotSystem->QrySpecPrice(&specPriceReq);
			DWORD signal = WaitForSingleObject(m_hQrySpecPrice, 5000);
			if (signal == WAIT_TIMEOUT)
			{
				_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ѯ�ֻ��۸�ʱ", TMB_OK);
				return;
			}
		}
		m_map_specPrice = m_System->m_mapSpecPrice;
		//�����˵�
		//GeneratedBill();
		GeneratedListBill();
		m_listAll.SetData(&m_RowinfoContainer);
		
	}
	else {	//����ͬ=================================================================================================

		//��ͬ��
		wchar_t	agreeno[MAX_PATH] = { 0 };
		m_cbAgree.GetText(agreeno);
		strncpy_s(matchReq.AgreementNo, LoadRC::unicode_to_ansi(agreeno).c_str());

		//��ȡ��ͬ��
		string key = m_System->get_curkey();
		bool isBuyAgree = false;

		isBuyAgree = m_System->m_map_mapAgreement[key][matchReq.AgreementNo].AgreementType == SMS_AGREEMENT_PURCHASE ? true : false;
		m_map_Agree[matchReq.AgreementNo] = m_System->m_map_mapAgreement[key][matchReq.AgreementNo];

		//�ҵ���Ҫ��ͬ
		string mainAgreeno("");
		if (isBuyAgree)
		{
			bool found = false;
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (strcmp(matchReq.AgreementNo, it_rel.BuyAgreementNo) == 0)
				{
					found = true;
					if (m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo].SpotQty >= m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo].SpotQty)
						mainAgreeno = it_rel.BuyAgreementNo;
					else
						mainAgreeno = it_rel.SellAgreementNo;
					break;
				}
			}
			if (!found)
				mainAgreeno = matchReq.AgreementNo;
		}
		else
		{
			bool found = false;
			for (auto it_rel : m_System->m_mapAgreementRel[key])
			{
				if (strcmp(matchReq.AgreementNo, it_rel.SellAgreementNo) == 0)
				{
					found = true;
					if (m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo].SpotQty >= m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo].SpotQty)
						mainAgreeno = it_rel.SellAgreementNo;
					else
						mainAgreeno = it_rel.BuyAgreementNo;
					break;
				}
			}
			if (!found)
				mainAgreeno = matchReq.AgreementNo;
		}

		//������Ҫ��ͬ����
		m_map_Agree[mainAgreeno] = m_System->m_map_mapAgreement[key][mainAgreeno];

		//��ȡ������Ĵ�Ҫ��ͬ��
		set<SmsAgreementDataRsp> m_setMinorAgree;
		m_setMinorAgree.clear();
		//һ����Ҫ��ͬ������Ĵ�Ҫ��ͬ
		FindMinorAgree(mainAgreeno, m_setMinorAgree);
		//<�������ݣ�<ƽ������>>
		m_map_sAgreeRel[m_map_Agree[mainAgreeno]] = m_setMinorAgree;

		//��ѯ�ɽ�
		for (auto it_req : m_map_Agree)
		{
			//if (strlen(matchReq.UserNo) != 0 && strcmp(matchReq.UserNo, it_req.second.UserNo)!=0)
			//{
			//	continue;
			//}
			ResetEvent(m_hQryMatch);
			strncpy_s(matchReq.AgreementNo, it_req.first.c_str());

			//m_System->m_matchWnd->m_listMatch.DeleteAllItems();
			m_System->m_mapMatch.clear();

			g_pSpotSystem->QryMatch(&matchReq);

			DWORD signal = WaitForSingleObject(m_hQryMatch, 2000);
			if (signal == WAIT_TIMEOUT)
			{
				_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ѯ�ɽ���ʱ", TMB_OK);
				return;
			}

			if (!m_System->m_mapMatch.empty())
			{
				m_map_mAgreeMatch[matchReq.AgreementNo] = m_System->m_mapMatch;
			}
		}
		//��ѯ�ֻ��۸�
		for (auto it_req : m_map_Agree)
		{
			//if (it_req.second.PositionOffset != SMS_O_OPEN)
			//	continue;
			ResetEvent(m_hQrySpecPrice);
			strncpy_s(specPriceReq.SpecificationNo, it_req.second.SpecificationNo);

			//m_System->m_specSpriceWnd->m_listSpecPrice.DeleteAllItems();

			g_pSpotSystem->QrySpecPrice(&specPriceReq);

			DWORD signal = WaitForSingleObject(m_hQrySpecPrice, 2000);
			if (signal == WAIT_TIMEOUT)
			{
				_TMessageBox_Domodal(m_Hwnd, "��ʾ", L"��ѯ�ֻ��۸�ʱ", TMB_OK);
				return;
			}
		}
		m_map_specPrice = m_System->m_mapSpecPrice;

		//��ѯ�ڻ�����۸�--ʹ�����¼ۻ���������
		//�����˵�
		//GeneratedBill();
		GeneratedListBill();
		m_listAll.SetData(&m_RowinfoContainer);
	}
}

void IntegrationWnd::OnBtnSave()
{
	///////////////////////////////����///////////////////////////////
	//m_RowinfoContainer.clear();
	//for (int i = 0; i < 18; i++)
	//{
	//	TRowInfo tempRow;
	//	m_RowinfoContainer.push_back(&tempRow);
	//}


	//m_listAll.SetData(&m_RowinfoContainer);

	//return;
	//////////////////////////////////////////////////////////////////

	SaveToExcel(L"1");
	return;

	//OPENFILENAME ofn = { 0 };
	//TCHAR strFilename[MAX_PATH] = { 0 };															//���ڽ����ļ���
	//ofn.lStructSize = sizeof(OPENFILENAME);															//�ṹ���С
	//ofn.hwndOwner = m_Hwnd;																			//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	//ofn.lpstrFilter = TEXT("Excel�ļ�(*.xlsx)\0*.xlsx\0\0");										//���ù���
	//ofn.nFilterIndex = 1;																			//����������
	//ofn.lpstrFile = strFilename;																	//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
	//ofn.nMaxFile = sizeof(strFilename);																//����������
	//ofn.lpstrInitialDir = NULL;																		//��ʼĿ¼ΪĬ��
	//ofn.lpstrTitle = TEXT("���浽");														//ʹ��ϵͳĬ�ϱ������ռ���
	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;							//Ŀ¼������ڣ������ļ�ǰ��������
	//ofn.lpstrDefExt = TEXT("xlsx");//Ĭ��׷�ӵ���չ��

	//if (GetSaveFileName(&ofn))
	//{
	//	//���浽excel
	//	SaveToExcel(strFilename);
	//}
	//else
	//	return;
}

void IntegrationWnd::GeneratedBill()
{
	//���
	m_richeditAll.Clear();

	//key
	string key = m_System->get_curkey();
	//�ɽ�������ƽ����ÿ�ʶԳ�
	for (auto& it_agree : m_map_mAgreeMatch)
	{
		for (auto & it_match : it_agree.second)
		{
			it_match.second.MatchFee = it_match.second.MatchFee / it_match.second.MatchQty;
		}
	}

	//============================================��ͷ========================================================
	char temp_txt[1024] = { 0 };
	m_richeditAll.AddString(row_title_start);		//1
	sprintf_s(temp_txt, "��%124s%124s��", "���ֶ��˵�"," ");
	m_richeditAll.AddString(temp_txt);		//2
	m_richeditAll.AddString(row_title_middle1);		//3
	sprintf_s(temp_txt, "��%-14s��%29s%25s��%35s%31s��%49s%47s��%7s%3s��", "", "�ɹ�", "", "����", "", "�ڻ�", "", "", "");
	m_richeditAll.AddString(temp_txt);		//4
	m_richeditAll.AddString(row_title_middle2);		//5
	//sprintf_s(temp_txt, "��%-4s��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%14s��", "", "�˺�", "����", "��ͬ���", "����", "����",
	//	"����","��ͬ���", "����", "����", "����˰����", "��Լ���", "��������", "���ּ۸�", "ƽ������", "ƽ�ּ۸�", "����", "������", "ӯ��", "����ӯ��");
	sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%10s��",  "�˺�", "����", "��ͬ���", "����", "����",
		"����","��ͬ���", "����", "����", "����˰ӯ��", "��Լ���", "��������", "���ּ۸�", "ƽ������", "ƽ�ּ۸�", "����", "������", "ӯ��", "����ӯ��");
	m_richeditAll.AddString(temp_txt);		//5

	//���п��ֺ�ͬӯ���ϼ�
	m_spotAllProfit = 0.0;
	m_allFee = 0.0;
	m_matchAllProfit = 0.0;

	bool b_isFirstMain = true;
	//����ÿ����Ҫ��ͬ
	for (auto it_main : m_map_sAgreeRel)
	{
		bool b_isFirst = true;
	
		m_tradeDot = 0.000000001;		//ÿ�ֳ���
		m_minorAgreeQty = 0;			//��Ҫ��ͬ�����ܼ�
		m_minorAgreeTotal = 0.0;		//��Ҫ��ͬ��ֵ�ܼ�
		m_spotTotalProfit = 0.0;		//����ϼ�

		m_futureQty = 0;				//�ѶԳ��ڻ������ϼ�
		m_futureRemainQty = 0;			//δ�Գ��ڻ������ϼ�
		m_openMatchTotal = 0.0;			//���ּ۸�ϼ�
		m_closeMatchTotal = 0.0;		//ƽ�ּ۸�ϼ�
		m_totalFee = 0.0;				//�����Ѻϼ�
		m_matchTotalProfit = 0.0;		//�ڻ�ӯ��

		//��ͬ��Ӧ-���--ÿ�ֳ���
		string scommod = m_System->m_mapSpec[it_main.first.SpecificationNo].CommodityNo;
		SCommodity* commod[1] = { 0 };
		g_pStarApi->GetCommodityData("", scommod.c_str(), commod, 1, false);
		if (commod[0])
			m_tradeDot = commod[0]->TradeDot;
		
		//1.��Ҫ��ͬ���ԶԳ壨���˹�ƽ�֣���2.�˹�ƽ�� 3.Ȼ�������Ҫ��ͬ(�ȴ����˹�ƽ�֣����ԶԳ�,Ȼ����ƽ��Ҫ��ͬ���ٿ���Ҫ��ͬ)
		vector<HedgeInfo>				v_selfHedge;	//�ԶԳ�
		v_selfHedge.clear();
		vector<HedgeInfo>				v_manHedge;		//һ��Ŀ���ͬ��ͬ����ָ��ƽ�ֶԳ��ϵ
		v_manHedge.clear();

		//1.���ԶԳ�Ŀ���ͬ�����˹�ƽ�֣�
		{
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//ƽ����ϸû��ƽ��
					while (it_closeMatch.second.MatchQty > 0)
					{
						//ƽ�˺�ͬ���ֳɽ�
						for (auto &it_openMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_openMatch.second.ContractNo, it_closeMatch.second.ContractNo) != 0)
								continue;
							if (it_openMatch.second.Offset != SMS_O_OPEN)
								continue;

							if (it_openMatch.second.MatchQty > 0)
							{
								HedgeInfo tempinfo;

								if (it_openMatch.second.MatchQty >= it_closeMatch.second.MatchQty)
								{
									tempinfo.CloseQty = it_closeMatch.second.MatchQty;
									it_openMatch.second.MatchQty -= it_closeMatch.second.MatchQty;
									it_closeMatch.second.MatchQty = 0;
								}
								else
								{
									tempinfo.CloseQty = it_openMatch.second.MatchQty;
									it_closeMatch.second.MatchQty -= it_openMatch.second.MatchQty;
									it_openMatch.second.MatchQty = 0;
								}

								strncpy_s(tempinfo.OpenAgreementNo, it_main.first.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_main.first.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_selfHedge.push_back(tempinfo);
								break;
							}
						}
						//���Ŀ���ͬ���ֳɽ�������Ϊ��Ϳ���break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_openMatch_b.second.ContractNo, it_closeMatch.second.ContractNo) == 0 && it_openMatch_b.second.Offset == SMS_O_OPEN)
								if (it_openMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}
						if (b_canBreak) break;
					}
				}
			}
			//���ԶԳ��ϵд������
			GeneratedSelfOffsetItem(it_main.first, v_selfHedge, b_isFirst, b_isFirstMain);
			if (v_selfHedge.size() != 0)
				if (b_isFirst)
					b_isFirst = !b_isFirst;
		}
		//2.������Ҫ��ִͬ���˹�ƽ��
		vector<HedgeInfo>				v_temp_manHedge;
		v_temp_manHedge.clear();
		//��Ҫ��ͬ
		{
			//����ָ��ƽ��
			for (auto it_manual : m_System->m_mapManualClose[key])
			{
				if (strcmp(it_manual.CloseAgreementNo, it_main.first.AgreementNo) == 0)
				{
					HedgeInfo tempinfo;
					strncpy_s(tempinfo.OpenAgreementNo, "");
					for (auto it_minor : it_main.second)
					{
						auto it_openNo = m_map_mAgreeMatch[it_minor.AgreementNo].find(it_manual.OpenNo);
						if (it_openNo != m_map_mAgreeMatch[it_minor.AgreementNo].end())
						{
							strncpy_s(tempinfo.OpenAgreementNo, it_minor.AgreementNo);
							break;
						}
					}
					if (strlen(tempinfo.OpenAgreementNo) != 0)
					{
						strncpy_s(tempinfo.CloseAgreementNo, it_main.first.AgreementNo);
						strncpy_s(tempinfo.OpenNo, it_manual.OpenNo);
						strncpy_s(tempinfo.CloseNo, it_manual.CloseNo);
						tempinfo.CloseQty = it_manual.CloseQty;
						v_manHedge.push_back(tempinfo);
						v_temp_manHedge.push_back(tempinfo);
					}
				}
			}
			//����Ŀ���ͬ�ţ�������ͬ�ţ�������Ӧ�ĳɽ�����
			for (auto it_reduceNum : v_temp_manHedge)
			{
				//��Ŀ���ͬ�ɽ�
				auto it_openAgree = m_map_mAgreeMatch.find(it_main.first.AgreementNo);
				if (it_openAgree != m_map_mAgreeMatch.end())
				{
					auto it_openMatchNo = m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo].find(it_reduceNum.OpenNo);
					if (it_openMatchNo != m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo].end())
						m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo][it_reduceNum.OpenNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
				//�ҹ�����ͬ�ɽ�
				auto it_closeAgree = m_map_mAgreeMatch.find(it_reduceNum.CloseAgreementNo);
				if (it_closeAgree != m_map_mAgreeMatch.end())
				{
					auto it_closeMatchNo = m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo].find(it_reduceNum.CloseNo);
					if (it_closeMatchNo != m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo].end())
						m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo][it_reduceNum.CloseNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
			}
		}
		//��Ҫ��ͬ
		for (auto it_minor : it_main.second)
		{
			v_temp_manHedge.clear();
			//����ָ��ƽ��
			for (auto it_manual : m_System->m_mapManualClose[key])
			{
				if (strcmp(it_manual.CloseAgreementNo, it_minor.AgreementNo) == 0)
				{
					HedgeInfo tempinfo;
					strncpy_s(tempinfo.OpenAgreementNo, it_main.first.AgreementNo);
					strncpy_s(tempinfo.CloseAgreementNo, it_minor.AgreementNo);
					strncpy_s(tempinfo.OpenNo, it_manual.OpenNo);
					strncpy_s(tempinfo.CloseNo, it_manual.CloseNo);
					tempinfo.CloseQty = it_manual.CloseQty;
					v_manHedge.push_back(tempinfo);
					v_temp_manHedge.push_back(tempinfo);
				}
			}
			//����Ŀ���ͬ�ţ�������ͬ�ţ�������Ӧ�ĳɽ�����
			for (auto it_reduceNum : v_temp_manHedge)
			{
				//��Ŀ���ͬ�ɽ�
				auto it_openAgree = m_map_mAgreeMatch.find(it_reduceNum.OpenAgreementNo);
				if (it_openAgree != m_map_mAgreeMatch.end())
				{
					auto it_openMatchNo = m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo].find(it_reduceNum.OpenNo);
					if (it_openMatchNo != m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo].end())
						m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo][it_reduceNum.OpenNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
				//�ҹ�����ͬ�ɽ�
				auto it_closeAgree = m_map_mAgreeMatch.find(it_reduceNum.CloseAgreementNo);
				if (it_closeAgree != m_map_mAgreeMatch.end())
				{
					auto it_closeMatchNo = m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo].find(it_reduceNum.CloseNo);
					if (it_closeMatchNo != m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo].end())
						m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo][it_reduceNum.CloseNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
			}
		}

		//3.������Ҫ��ͬ�����˹�ƽ�֣����ԶԳ壩
		for (auto it_minor : it_main.second)
		{
			//�Գ�
			vector<HedgeInfo>								v_matchNum;							//�Գ�����
			v_matchNum.clear();

			//��ָ��ƽ�ֹ�ϵ��ӵ���ʾ����
			for (auto it_manclose : v_manHedge)
			{
				if (strcmp(it_manclose.OpenAgreementNo, it_main.first.AgreementNo) == 0 && strcmp(it_manclose.CloseAgreementNo, it_minor.AgreementNo) == 0)
					v_matchNum.push_back(it_manclose);
				if (strcmp(it_manclose.CloseAgreementNo, it_main.first.AgreementNo) == 0 && strcmp(it_manclose.OpenAgreementNo, it_minor.AgreementNo) == 0)
					v_matchNum.push_back(it_manclose);

			}

			//���ԶԳ�
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//ƽ����ϸû��ƽ��
					while (it_closeMatch.second.MatchQty > 0)
					{
						//ƽ�˺�ͬ���ֳɽ�
						for (auto &it_openMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
						{
							if (strcmp(it_openMatch.second.ContractNo, it_closeMatch.second.ContractNo) != 0)
								continue;
							if (it_openMatch.second.Offset != SMS_O_OPEN)
								continue;

							if (it_openMatch.second.MatchQty > 0)
							{
								HedgeInfo tempinfo;
								if (it_openMatch.second.MatchQty >= it_closeMatch.second.MatchQty)
								{
									tempinfo.CloseQty = it_closeMatch.second.MatchQty;
									it_openMatch.second.MatchQty -= it_closeMatch.second.MatchQty;
									it_closeMatch.second.MatchQty = 0;
								}
								else
								{
									tempinfo.CloseQty = it_openMatch.second.MatchQty;
									it_closeMatch.second.MatchQty -= it_openMatch.second.MatchQty;
									it_openMatch.second.MatchQty = 0;
								}

								strncpy_s(tempinfo.OpenAgreementNo, it_minor.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_minor.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_matchNum.push_back(tempinfo);
								break;
							}
						}
						//���Ŀ���ͬ���ֳɽ�������Ϊ��Ϳ���break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_minor.AgreementNo])
						{
							if (strcmp(it_openMatch_b.second.ContractNo, it_closeMatch.second.ContractNo) == 0 && it_openMatch_b.second.Offset == SMS_O_OPEN)
								if (it_openMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}

						if (b_canBreak) break;
					}
				}
			}


			//������Ҫ��ͬ�ɽ�(��Ҫ��ͬ���֣���Ҫ��ͬƽ��)
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//����û��ƽ��
					while (it_closeMatch.second.MatchQty > 0)
					{
						//ƽ���ֺ�ͬ�ɽ�
						for (auto &it_openMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_openMatch.second.ContractNo, it_closeMatch.second.ContractNo) != 0)
								continue;
							if (it_openMatch.second.Offset != SMS_O_OPEN)
								continue;

							if (it_openMatch.second.MatchQty > 0)
							{
								HedgeInfo tempinfo;

								if (it_openMatch.second.MatchQty >= it_closeMatch.second.MatchQty)
								{
									tempinfo.CloseQty = it_closeMatch.second.MatchQty;
									it_openMatch.second.MatchQty -= it_closeMatch.second.MatchQty;
									it_closeMatch.second.MatchQty = 0;
								}
								else
								{
									tempinfo.CloseQty = it_openMatch.second.MatchQty;
									it_closeMatch.second.MatchQty -= it_openMatch.second.MatchQty;
									it_openMatch.second.MatchQty = 0;
								}

								strncpy_s(tempinfo.OpenAgreementNo, it_main.first.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_minor.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_matchNum.push_back(tempinfo);
								break;
							}
						}
						//������ֺ�ͬ�ɽ�������Ϊ��Ϳ���break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_openMatch_b.second.ContractNo, it_closeMatch.second.ContractNo) == 0 && it_openMatch_b.second.Offset == SMS_O_OPEN)
								if (it_openMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}
						if (b_canBreak) break;
					}
				}
			}

			//������Ҫ��ͬ�ɽ�(��Ҫ��ͬƽ�֣���Ҫ��ͬ����)
			for (auto& it_mopenMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_mopenMatch.second.Offset == SMS_O_OPEN)
				{
					//����û��ƽ��
					while (it_mopenMatch.second.MatchQty > 0)
					{
						//�Գ���Ҫ��ͬƽ�ֳɽ�
						for (auto &it_mcloseMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_mcloseMatch.second.ContractNo, it_mopenMatch.second.ContractNo) != 0)
								continue;
							if (it_mcloseMatch.second.Offset != SMS_O_COVER)
								continue;

							if (it_mcloseMatch.second.MatchQty > 0)
							{
								HedgeInfo tempinfo;

								if (it_mcloseMatch.second.MatchQty >= it_mopenMatch.second.MatchQty)
								{
									tempinfo.CloseQty = it_mopenMatch.second.MatchQty;
									it_mcloseMatch.second.MatchQty -= it_mopenMatch.second.MatchQty;
									it_mopenMatch.second.MatchQty = 0;
								}
								else
								{
									tempinfo.CloseQty = it_mcloseMatch.second.MatchQty;
									it_mopenMatch.second.MatchQty -= it_mcloseMatch.second.MatchQty;
									it_mcloseMatch.second.MatchQty = 0;
								}

								strncpy_s(tempinfo.CloseAgreementNo, it_main.first.AgreementNo);
								strncpy_s(tempinfo.OpenAgreementNo, it_minor.AgreementNo);
								strncpy_s(tempinfo.CloseNo, it_mcloseMatch.second.MatchNo);
								strncpy_s(tempinfo.OpenNo, it_mopenMatch.second.MatchNo);
								v_matchNum.push_back(tempinfo);
								break;
							}
						}
						//������ֺ�ͬ�ɽ�������Ϊ��Ϳ���break
						bool b_canBreak = true;
						for (auto &it_closeMatch_b : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_closeMatch_b.second.ContractNo, it_mopenMatch.second.ContractNo) == 0 && it_closeMatch_b.second.Offset == SMS_O_COVER)
								if (it_closeMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}
						if (b_canBreak) break;
					}
				}
			}

			//�ж��Ƿ��Ҫ��ͬ�ֻ����Ѿ��Գ�����
			int sumCover = 0;
			int close_remain = 0;
			for (auto it_match_num : v_matchNum)
			{
				sumCover += it_match_num.CloseQty;
			}
			//ƽ�ֺ�ͬ��Ӧ��ƽ�ֳɽ�û�����ֻ꣬��ʾ��ͬʣ�������������ڻ����ֳɽ�
			if (sumCover < it_minor.SpotQty / m_tradeDot)
			{
				close_remain = int(it_minor.SpotQty / m_tradeDot - sumCover);
			}

			//һ����Ҫ��ͬ���� ���� �Գ��ϵ
			GeneratedItem(it_main.first, it_minor, v_matchNum, b_isFirst,close_remain,b_isFirstMain);
			if (b_isFirst) b_isFirst = !b_isFirst;
		}

		
		//===================һ����Ҫ��ͬ��Ӧһ��С�� �� һ������ͳ��=================================
		//�����Ƿ��г���
		CalculateSpotOpen(it_main.first, b_isFirst,b_isFirstMain);
		//С��
		int ifutureQty = 0;
		ifutureQty = m_futureQty + m_futureRemainQty;
		m_richeditAll.AddString(row_text_sumtop);
		sprintf_s(temp_txt, "��%-14s  %-10s  %-20s  %10.2f  %8d��%-10s  %-20s  %10.2f  %8d  %10.2f��%-14s  %-10s  %10.2f  %-10s  %10.2f  %8d  %10.2f  %10.2f��%10.2f��",
			 "С��"/*it_main.first.UserNo*/, ""/*it_main.first.SignDate*/, ""/*it_main.first.AgreementNo*/, it_main.first.SpotPrice,
			it_main.first.SpotQty, "", "",  m_minorAgreeQty == 0 ? 0.0 : m_minorAgreeTotal / m_minorAgreeQty, it_main.first.SpotQty, m_spotTotalProfit,
			"","", ifutureQty == 0 ? 0.0 : m_openMatchTotal / ifutureQty, "", ifutureQty == 0 ? 0.0 : m_closeMatchTotal/ ifutureQty, m_futureQty * 2 + m_futureRemainQty, m_totalFee, m_matchTotalProfit, m_spotTotalProfit + m_matchTotalProfit - m_totalFee);
		m_richeditAll.AddString(temp_txt);
		//m_richeditAll.AddString(row_text_start);
		//�ۼӵ��ϼ�
		m_spotAllProfit += m_spotTotalProfit;
		m_allFee += m_totalFee;
		m_matchAllProfit += m_matchTotalProfit;

		if (b_isFirstMain)
			b_isFirstMain = !b_isFirstMain;

	}
	//���п��ֺ�ͬ �ϼ�ӯ��
	if (!m_map_sAgreeRel.empty())
	{
		sprintf_s(temp_txt, "��%-14s  %-10s  %-20s  %10s  %8s  %-10s  %-20s  %10s  %8s  %10.2f��%-14s  %-10s  %10s  %-10s  %10s  %8s  %10.2f  %10.2f��%10.2f��",
			"�ܼ�", "", "", "", "",
			 "", "", "", "", m_spotAllProfit,
			"", "", "", "", "", "", m_allFee, m_matchAllProfit, m_spotAllProfit + m_matchAllProfit - m_allFee);
		m_richeditAll.AddString(row_text_totalsumtop);
		m_richeditAll.AddString(temp_txt);
		m_richeditAll.AddString(row_title_end);
	}
}

void IntegrationWnd::GeneratedListBill()
{
	//���
	m_RowinfoContainer.clear();

	//key
	string key = m_System->get_curkey();
	//�ɽ�������ƽ����ÿ�ʶԳ�
	for (auto& it_agree : m_map_mAgreeMatch)
	{
		for (auto & it_match : it_agree.second)
		{
			it_match.second.MatchFee = it_match.second.MatchFee / it_match.second.MatchQty;
		}
	}

	//���п��ֺ�ͬӯ���ϼ�
	m_spotAllProfit = 0.0;
	m_allFee = 0.0;
	m_matchAllProfit = 0.0;

	bool b_isFirstMain = true;
	//����ÿ����Ҫ��ͬ
	for (auto it_main : m_map_sAgreeRel)
	{
		bool b_isFirst = true;

		m_tradeDot = 0.000000001;		//ÿ�ֳ���
		m_minorAgreeQty = 0;			//��Ҫ��ͬ�����ܼ�
		m_minorAgreeTotal = 0.0;		//��Ҫ��ͬ��ֵ�ܼ�
		m_spotTotalProfit = 0.0;		//����ϼ�

		m_futureQty = 0;				//�ѶԳ��ڻ������ϼ�
		m_futureRemainQty = 0;			//δ�Գ��ڻ������ϼ�
		m_openMatchTotal = 0.0;			//���ּ۸�ϼ�
		m_closeMatchTotal = 0.0;		//ƽ�ּ۸�ϼ�
		m_totalFee = 0.0;				//�����Ѻϼ�
		m_matchTotalProfit = 0.0;		//�ڻ�ӯ��

										//��ͬ��Ӧ-���--ÿ�ֳ���
		string scommod = m_System->m_mapSpec[it_main.first.SpecificationNo].CommodityNo;
		SCommodity* commod[1] = { 0 };
		g_pStarApi->GetCommodityData("", scommod.c_str(), commod, 1, false);
		if (commod[0])
			m_tradeDot = commod[0]->TradeDot;

		//1.��Ҫ��ͬ���ԶԳ壨���˹�ƽ�֣���2.�˹�ƽ�� 3.Ȼ�������Ҫ��ͬ(�ȴ����˹�ƽ�֣����ԶԳ�,Ȼ����ƽ��Ҫ��ͬ���ٿ���Ҫ��ͬ)
		vector<HedgeInfo>				v_selfHedge;	//�ԶԳ�
		v_selfHedge.clear();
		vector<HedgeInfo>				v_manHedge;		//һ��Ŀ���ͬ��ͬ����ָ��ƽ�ֶԳ��ϵ
		v_manHedge.clear();

		//1.���ԶԳ�Ŀ���ͬ�����˹�ƽ�֣�
		{
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//ƽ����ϸû��ƽ��
					while (it_closeMatch.second.MatchQty > 0)
					{
						//ƽ�˺�ͬ���ֳɽ�
						for (auto &it_openMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_openMatch.second.ContractNo, it_closeMatch.second.ContractNo) != 0)
								continue;
							if (it_openMatch.second.Offset != SMS_O_OPEN)
								continue;

							if (it_openMatch.second.MatchQty > 0)
							{
								HedgeInfo tempinfo;

								if (it_openMatch.second.MatchQty >= it_closeMatch.second.MatchQty)
								{
									tempinfo.CloseQty = it_closeMatch.second.MatchQty;
									it_openMatch.second.MatchQty -= it_closeMatch.second.MatchQty;
									it_closeMatch.second.MatchQty = 0;
								}
								else
								{
									tempinfo.CloseQty = it_openMatch.second.MatchQty;
									it_closeMatch.second.MatchQty -= it_openMatch.second.MatchQty;
									it_openMatch.second.MatchQty = 0;
								}

								strncpy_s(tempinfo.OpenAgreementNo, it_main.first.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_main.first.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_selfHedge.push_back(tempinfo);
								break;
							}
						}
						//���Ŀ���ͬ���ֳɽ�������Ϊ��Ϳ���break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_openMatch_b.second.ContractNo, it_closeMatch.second.ContractNo) == 0 && it_openMatch_b.second.Offset == SMS_O_OPEN)
								if (it_openMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}
						if (b_canBreak) break;
					}
				}
			}
			//���ԶԳ��ϵ�浽rowinfo
			GeneratedSelfOffsetListItem(it_main.first, v_selfHedge, b_isFirst, b_isFirstMain);
			if (v_selfHedge.size() != 0)
				if (b_isFirst)
					b_isFirst = !b_isFirst;
		}
		//2.������Ҫ��ִͬ���˹�ƽ��
		vector<HedgeInfo>				v_temp_manHedge;
		v_temp_manHedge.clear();
		//��Ҫ��ͬ
		{
			//����ָ��ƽ��
			for (auto it_manual : m_System->m_mapManualClose[key])
			{
				if (strcmp(it_manual.CloseAgreementNo, it_main.first.AgreementNo) == 0)
				{
					HedgeInfo tempinfo;
					strncpy_s(tempinfo.OpenAgreementNo, "");
					for (auto it_minor : it_main.second)
					{
						auto it_openNo = m_map_mAgreeMatch[it_minor.AgreementNo].find(it_manual.OpenNo);
						if (it_openNo != m_map_mAgreeMatch[it_minor.AgreementNo].end())
						{
							strncpy_s(tempinfo.OpenAgreementNo, it_minor.AgreementNo);
							break;
						}
					}
					if (strlen(tempinfo.OpenAgreementNo) != 0)
					{
						strncpy_s(tempinfo.CloseAgreementNo, it_main.first.AgreementNo);
						strncpy_s(tempinfo.OpenNo, it_manual.OpenNo);
						strncpy_s(tempinfo.CloseNo, it_manual.CloseNo);
						tempinfo.CloseQty = it_manual.CloseQty;
						v_manHedge.push_back(tempinfo);
						v_temp_manHedge.push_back(tempinfo);
					}
				}
			}
			//����Ŀ���ͬ�ţ�������ͬ�ţ�������Ӧ�ĳɽ�����
			for (auto it_reduceNum : v_temp_manHedge)
			{
				//��Ŀ���ͬ�ɽ�
				auto it_openAgree = m_map_mAgreeMatch.find(it_main.first.AgreementNo);
				if (it_openAgree != m_map_mAgreeMatch.end())
				{
					auto it_openMatchNo = m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo].find(it_reduceNum.OpenNo);
					if (it_openMatchNo != m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo].end())
						m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo][it_reduceNum.OpenNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
				//�ҹ�����ͬ�ɽ�
				auto it_closeAgree = m_map_mAgreeMatch.find(it_reduceNum.CloseAgreementNo);
				if (it_closeAgree != m_map_mAgreeMatch.end())
				{
					auto it_closeMatchNo = m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo].find(it_reduceNum.CloseNo);
					if (it_closeMatchNo != m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo].end())
						m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo][it_reduceNum.CloseNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
			}
		}
		//��Ҫ��ͬ
		for (auto it_minor : it_main.second)
		{
			v_temp_manHedge.clear();
			//����ָ��ƽ��
			for (auto it_manual : m_System->m_mapManualClose[key])
			{
				if (strcmp(it_manual.CloseAgreementNo, it_minor.AgreementNo) == 0)
				{
					HedgeInfo tempinfo;
					strncpy_s(tempinfo.OpenAgreementNo, it_main.first.AgreementNo);
					strncpy_s(tempinfo.CloseAgreementNo, it_minor.AgreementNo);
					strncpy_s(tempinfo.OpenNo, it_manual.OpenNo);
					strncpy_s(tempinfo.CloseNo, it_manual.CloseNo);
					tempinfo.CloseQty = it_manual.CloseQty;
					v_manHedge.push_back(tempinfo);
					v_temp_manHedge.push_back(tempinfo);
				}
			}
			//����Ŀ���ͬ�ţ�������ͬ�ţ�������Ӧ�ĳɽ�����
			for (auto it_reduceNum : v_temp_manHedge)
			{
				//��Ŀ���ͬ�ɽ�
				auto it_openAgree = m_map_mAgreeMatch.find(it_reduceNum.OpenAgreementNo);
				if (it_openAgree != m_map_mAgreeMatch.end())
				{
					auto it_openMatchNo = m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo].find(it_reduceNum.OpenNo);
					if (it_openMatchNo != m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo].end())
						m_map_mAgreeMatch[it_reduceNum.OpenAgreementNo][it_reduceNum.OpenNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
				//�ҹ�����ͬ�ɽ�
				auto it_closeAgree = m_map_mAgreeMatch.find(it_reduceNum.CloseAgreementNo);
				if (it_closeAgree != m_map_mAgreeMatch.end())
				{
					auto it_closeMatchNo = m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo].find(it_reduceNum.CloseNo);
					if (it_closeMatchNo != m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo].end())
						m_map_mAgreeMatch[it_reduceNum.CloseAgreementNo][it_reduceNum.CloseNo].MatchQty -= it_reduceNum.CloseQty;
					else
						continue;
				}
				else
					continue;
			}
		}

		//3.������Ҫ��ͬ�����˹�ƽ�֣����ԶԳ壩
		for (auto it_minor : it_main.second)
		{
			//�Գ�
			vector<HedgeInfo>								v_matchNum;							//�Գ�����
			v_matchNum.clear();

			//��ָ��ƽ�ֹ�ϵ��ӵ���ʾ����
			for (auto it_manclose : v_manHedge)
			{
				if (strcmp(it_manclose.OpenAgreementNo, it_main.first.AgreementNo) == 0 && strcmp(it_manclose.CloseAgreementNo, it_minor.AgreementNo) == 0)
					v_matchNum.push_back(it_manclose);
				if (strcmp(it_manclose.CloseAgreementNo, it_main.first.AgreementNo) == 0 && strcmp(it_manclose.OpenAgreementNo, it_minor.AgreementNo) == 0)
					v_matchNum.push_back(it_manclose);

			}

			//���ԶԳ�
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//ƽ����ϸû��ƽ��
					while (it_closeMatch.second.MatchQty > 0)
					{
						//ƽ�˺�ͬ���ֳɽ�
						for (auto &it_openMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
						{
							if (strcmp(it_openMatch.second.ContractNo, it_closeMatch.second.ContractNo) != 0)
								continue;
							if (it_openMatch.second.Offset != SMS_O_OPEN)
								continue;

							if (it_openMatch.second.MatchQty > 0)
							{
								HedgeInfo tempinfo;
								if (it_openMatch.second.MatchQty >= it_closeMatch.second.MatchQty)
								{
									tempinfo.CloseQty = it_closeMatch.second.MatchQty;
									it_openMatch.second.MatchQty -= it_closeMatch.second.MatchQty;
									it_closeMatch.second.MatchQty = 0;
								}
								else
								{
									tempinfo.CloseQty = it_openMatch.second.MatchQty;
									it_closeMatch.second.MatchQty -= it_openMatch.second.MatchQty;
									it_openMatch.second.MatchQty = 0;
								}

								strncpy_s(tempinfo.OpenAgreementNo, it_minor.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_minor.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_matchNum.push_back(tempinfo);
								break;
							}
						}
						//���Ŀ���ͬ���ֳɽ�������Ϊ��Ϳ���break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_minor.AgreementNo])
						{
							if (strcmp(it_openMatch_b.second.ContractNo, it_closeMatch.second.ContractNo) == 0 && it_openMatch_b.second.Offset == SMS_O_OPEN)
								if (it_openMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}

						if (b_canBreak) break;
					}
				}
			}


			//������Ҫ��ͬ�ɽ�(��Ҫ��ͬ���֣���Ҫ��ͬƽ��)
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//����û��ƽ��
					while (it_closeMatch.second.MatchQty > 0)
					{
						//ƽ���ֺ�ͬ�ɽ�
						for (auto &it_openMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_openMatch.second.ContractNo, it_closeMatch.second.ContractNo) != 0)
								continue;
							if (it_openMatch.second.Offset != SMS_O_OPEN)
								continue;

							if (it_openMatch.second.MatchQty > 0)
							{
								HedgeInfo tempinfo;

								if (it_openMatch.second.MatchQty >= it_closeMatch.second.MatchQty)
								{
									tempinfo.CloseQty = it_closeMatch.second.MatchQty;
									it_openMatch.second.MatchQty -= it_closeMatch.second.MatchQty;
									it_closeMatch.second.MatchQty = 0;
								}
								else
								{
									tempinfo.CloseQty = it_openMatch.second.MatchQty;
									it_closeMatch.second.MatchQty -= it_openMatch.second.MatchQty;
									it_openMatch.second.MatchQty = 0;
								}

								strncpy_s(tempinfo.OpenAgreementNo, it_main.first.AgreementNo);
								strncpy_s(tempinfo.CloseAgreementNo, it_minor.AgreementNo);
								strncpy_s(tempinfo.OpenNo, it_openMatch.second.MatchNo);
								strncpy_s(tempinfo.CloseNo, it_closeMatch.second.MatchNo);
								v_matchNum.push_back(tempinfo);
								break;
							}
						}
						//������ֺ�ͬ�ɽ�������Ϊ��Ϳ���break
						bool b_canBreak = true;
						for (auto &it_openMatch_b : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_openMatch_b.second.ContractNo, it_closeMatch.second.ContractNo) == 0 && it_openMatch_b.second.Offset == SMS_O_OPEN)
								if (it_openMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}
						if (b_canBreak) break;
					}
				}
			}

			//������Ҫ��ͬ�ɽ�(��Ҫ��ͬƽ�֣���Ҫ��ͬ����)
			for (auto& it_mopenMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_mopenMatch.second.Offset == SMS_O_OPEN)
				{
					//����û��ƽ��
					while (it_mopenMatch.second.MatchQty > 0)
					{
						//�Գ���Ҫ��ͬƽ�ֳɽ�
						for (auto &it_mcloseMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_mcloseMatch.second.ContractNo, it_mopenMatch.second.ContractNo) != 0)
								continue;
							if (it_mcloseMatch.second.Offset != SMS_O_COVER)
								continue;

							if (it_mcloseMatch.second.MatchQty > 0)
							{
								HedgeInfo tempinfo;

								if (it_mcloseMatch.second.MatchQty >= it_mopenMatch.second.MatchQty)
								{
									tempinfo.CloseQty = it_mopenMatch.second.MatchQty;
									it_mcloseMatch.second.MatchQty -= it_mopenMatch.second.MatchQty;
									it_mopenMatch.second.MatchQty = 0;
								}
								else
								{
									tempinfo.CloseQty = it_mcloseMatch.second.MatchQty;
									it_mopenMatch.second.MatchQty -= it_mcloseMatch.second.MatchQty;
									it_mcloseMatch.second.MatchQty = 0;
								}

								strncpy_s(tempinfo.CloseAgreementNo, it_main.first.AgreementNo);
								strncpy_s(tempinfo.OpenAgreementNo, it_minor.AgreementNo);
								strncpy_s(tempinfo.CloseNo, it_mcloseMatch.second.MatchNo);
								strncpy_s(tempinfo.OpenNo, it_mopenMatch.second.MatchNo);
								v_matchNum.push_back(tempinfo);
								break;
							}
						}
						//������ֺ�ͬ�ɽ�������Ϊ��Ϳ���break
						bool b_canBreak = true;
						for (auto &it_closeMatch_b : m_map_mAgreeMatch[it_main.first.AgreementNo])
						{
							if (strcmp(it_closeMatch_b.second.ContractNo, it_mopenMatch.second.ContractNo) == 0 && it_closeMatch_b.second.Offset == SMS_O_COVER)
								if (it_closeMatch_b.second.MatchQty > 0)
									b_canBreak = false;
						}
						if (b_canBreak) break;
					}
				}
			}

			//�ж��Ƿ��Ҫ��ͬ�ֻ����Ѿ��Գ�����
			int sumCover = 0;
			int close_remain = 0;
			for (auto it_match_num : v_matchNum)
			{
				sumCover += it_match_num.CloseQty;
			}
			//ƽ�ֺ�ͬ��Ӧ��ƽ�ֳɽ�û�����ֻ꣬��ʾ��ͬʣ�������������ڻ����ֳɽ�
			if (sumCover < it_minor.SpotQty / m_tradeDot)
			{
				close_remain = int(it_minor.SpotQty / m_tradeDot - sumCover);
			}

			//һ����Ҫ��ͬ���� ���� �Գ��ϵ
			GeneratedListItem(it_main.first, it_minor, v_matchNum, b_isFirst, close_remain, b_isFirstMain);
			if (b_isFirst) b_isFirst = !b_isFirst;
		}


		//===================һ����Ҫ��ͬ��Ӧһ��С�� �� һ������ͳ��=================================
		//�����Ƿ��г���
		CalculateSpotOpenList(it_main.first, b_isFirst, b_isFirstMain);
		//С��
		int ifutureQty = 0;
		ifutureQty = m_futureQty + m_futureRemainQty;

		TRowInfo  rowinfo_subtotal;
		//m_richeditAll.AddString(row_text_sumtop);
		//sprintf_s(temp_txt, "��%-14s  %-10s  %-20s  %10.2f  %8d��%-10s  %-20s  %10.2f  %8d  %10.2f��%-14s  %-10s  %10.2f  %-10s  %10.2f  %8d  %10.2f  %10.2f��%10.2f��",
		//	"С��"/*it_main.first.UserNo*/, ""/*it_main.first.SignDate*/, ""/*it_main.first.AgreementNo*/, it_main.first.SpotPrice,
		//	it_main.first.SpotQty, "", "", m_minorAgreeQty == 0 ? 0.0 : m_minorAgreeTotal / m_minorAgreeQty, it_main.first.SpotQty, m_spotTotalProfit,
		//	"", "", ifutureQty == 0 ? 0.0 : m_openMatchTotal / ifutureQty, "", ifutureQty == 0 ? 0.0 : m_closeMatchTotal / ifutureQty, m_futureQty * 2 + m_futureRemainQty, m_totalFee, m_matchTotalProfit, m_spotTotalProfit + m_matchTotalProfit - m_totalFee);
		//m_richeditAll.AddString(temp_txt);
		//m_richeditAll.AddString(row_text_start);

		strncpy_s(rowinfo_subtotal.UserNo, "С��");
		strncpy_s(rowinfo_subtotal.BuyAgreeDate, "");
		strncpy_s(rowinfo_subtotal.BuyAgreeNo, "");
		sprintf_s(rowinfo_subtotal.BuyPrice, "%10.2f", it_main.first.SpotPrice);
		sprintf_s(rowinfo_subtotal.BuyQty, "%8d", it_main.first.SpotQty);
		strncpy_s(rowinfo_subtotal.SellAgreeDate, "");
		strncpy_s(rowinfo_subtotal.SellAgreeNo, "");
		sprintf_s(rowinfo_subtotal.SellPrice, "%10.2f", m_minorAgreeQty == 0 ? 0.0 : m_minorAgreeTotal / m_minorAgreeQty);
		sprintf_s(rowinfo_subtotal.SellQty, "%8d", it_main.first.SpotQty);
		sprintf_s(rowinfo_subtotal.SpotProfit, "%10.2f", m_spotTotalProfit);
		strncpy_s(rowinfo_subtotal.ContractNo, "");
		strncpy_s(rowinfo_subtotal.OpenDate, "");
		sprintf_s(rowinfo_subtotal.OpenPrice, "%10.2f", ifutureQty == 0 ? 0.0 : m_openMatchTotal / ifutureQty);
		strncpy_s(rowinfo_subtotal.CoverDate, "");
		sprintf_s(rowinfo_subtotal.CoverPrice, "%10.2f", ifutureQty == 0 ? 0.0 : m_closeMatchTotal / ifutureQty);
		sprintf_s(rowinfo_subtotal.FutureQty, "%8d", m_futureQty * 2 + m_futureRemainQty);
		sprintf_s(rowinfo_subtotal.FutureFee, "%10.2f", m_totalFee);
		sprintf_s(rowinfo_subtotal.FutureProfit, "%10.2f", m_matchTotalProfit);
		sprintf_s(rowinfo_subtotal.TotalProfit, "%10.2f", m_spotTotalProfit + m_matchTotalProfit - m_totalFee);

		rowinfo_subtotal.RowInfoFormat = SMS_ROWINFOFORMAT_SUBTOTAL;
		m_RowinfoContainer.push_back(&rowinfo_subtotal);


		//�ۼӵ��ϼ�
		m_spotAllProfit += m_spotTotalProfit;
		m_allFee += m_totalFee;
		m_matchAllProfit += m_matchTotalProfit;

		if (b_isFirstMain)
			b_isFirstMain = !b_isFirstMain;

	}
	//���п��ֺ�ͬ �ϼ�ӯ��
	if (!m_map_sAgreeRel.empty())
	{
		//sprintf_s(temp_txt, "��%-14s  %-10s  %-20s  %10s  %8s  %-10s  %-20s  %10s  %8s  %10.2f��%-14s  %-10s  %10s  %-10s  %10s  %8s  %10.2f  %10.2f��%10.2f��",
		//	"�ܼ�", "", "", "", "",
		//	"", "", "", "", m_spotAllProfit,
		//	"", "", "", "", "", "", m_allFee, m_matchAllProfit, m_spotAllProfit + m_matchAllProfit - m_allFee);
		//m_richeditAll.AddString(row_text_totalsumtop);
		//m_richeditAll.AddString(temp_txt);
		//m_richeditAll.AddString(row_title_end);
		TRowInfo  rowinfo_sumtotal;
		strncpy_s(rowinfo_sumtotal.UserNo, "�ܼ�");
		strncpy_s(rowinfo_sumtotal.BuyAgreeDate, "");
		strncpy_s(rowinfo_sumtotal.BuyAgreeNo, "");
		strncpy_s(rowinfo_sumtotal.BuyPrice, "");
		strncpy_s(rowinfo_sumtotal.BuyQty, "");
		strncpy_s(rowinfo_sumtotal.SellAgreeDate, "");
		strncpy_s(rowinfo_sumtotal.SellAgreeNo, "");
		strncpy_s(rowinfo_sumtotal.SellPrice, "");
		strncpy_s(rowinfo_sumtotal.SellQty, "");
		sprintf_s(rowinfo_sumtotal.SpotProfit, "%10.2f", m_spotAllProfit);
		strncpy_s(rowinfo_sumtotal.ContractNo, "");
		strncpy_s(rowinfo_sumtotal.OpenDate, "");
		strncpy_s(rowinfo_sumtotal.OpenPrice, "");
		strncpy_s(rowinfo_sumtotal.CoverDate, "");
		strncpy_s(rowinfo_sumtotal.CoverPrice, "");
		strncpy_s(rowinfo_sumtotal.FutureQty, "");
		sprintf_s(rowinfo_sumtotal.FutureFee, "%10.2f", m_allFee);
		sprintf_s(rowinfo_sumtotal.FutureProfit, "%10.2f", m_matchAllProfit);
		sprintf_s(rowinfo_sumtotal.TotalProfit, "%10.2f", m_spotAllProfit + m_matchAllProfit - m_allFee);

		rowinfo_sumtotal.RowInfoFormat = SMS_ROWINFOFORMAT_SUMTOTAL;
		m_RowinfoContainer.push_back(&rowinfo_sumtotal);
	}
}

void IntegrationWnd::GeneratedItem(SmsAgreementDataRsp mainAgree, SmsAgreementDataRsp minorAgree, vector<HedgeInfo>& v_hedgeInfo, bool isfirst, int close_remain, bool isfirstmain)
{
	//if (v_hedgeInfo.empty())
	//	return;

	bool bMainBuy = true;
	bMainBuy = mainAgree.AgreementType == SMS_AGREEMENT_PURCHASE;

	string userno("");
	string opendate("");
	string openagree("");
	double openprice = 0.0;

	//˰��
	double tax = 1.16;
	if (!m_System->m_setTax.empty())
	{
		auto it = m_System->m_setTax.end();
		tax = (*(--it)).Tax;
	}

	//��Ҫ��ͬ���ɹ�����Ҫ��ͬ�����ۣ�====================================
	if (bMainBuy)
	{	
		//�ֻ�����
		double spotprofit = (minorAgree.SpotPrice - mainAgree.SpotPrice)*minorAgree.SpotQty / tax;
		m_minorAgreeQty += minorAgree.SpotQty;
		m_minorAgreeTotal += minorAgree.SpotQty * minorAgree.SpotPrice;
		m_spotTotalProfit += spotprofit;

		if (v_hedgeInfo.size() == 0)
		{
			if (isfirst && isfirstmain)
			{
				userno = mainAgree.UserNo;
				opendate = mainAgree.SignDate;
				openagree = mainAgree.AgreementNo;
				openprice = mainAgree.SpotPrice;

				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo,  minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (isfirst)
			{
				userno = mainAgree.UserNo;
				opendate = mainAgree.SignDate;
				openagree = mainAgree.AgreementNo;
				openprice = mainAgree.SpotPrice;

				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo,  minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					"","", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					 "", "", "", "", minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo,  minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_middle_l);
				m_richeditAll.AddString(temp_txt);
			}
			return;
		}

		for (int i = 0; i < v_hedgeInfo.size(); i++)
		{
			SmsMatchDataRsp openmatch = { 0 };
			auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
			if (it_open_agree != m_map_mAgreeMatch.end())
			{
				auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
				if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
				{
					memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
				}
			}

			SmsMatchDataRsp closematch = { 0 };
			auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
			if (it_close_agree != m_map_mAgreeMatch.end())
			{
				auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
				if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
				{
					memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
				}
			}

			//ÿ�ֳ���
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//����һ�ʶԳ� ������ �ڻ�����
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);
			
			double futureprofit = 0.0;
			if(openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else 
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//ÿ�ʶԳ����ϼ�
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//�����Լת���׺�Լ
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			if (i == 0 && isfirst && isfirstmain)
			{
				userno = mainAgree.UserNo;
				opendate = mainAgree.SignDate;
				openagree = mainAgree.AgreementNo;
				openprice = mainAgree.SpotPrice;

				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo, minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0 && isfirst)
			{
				userno = mainAgree.UserNo;
				opendate = mainAgree.SignDate;
				openagree = mainAgree.AgreementNo;
				openprice = mainAgree.SpotPrice;

				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo,  minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate,closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 "", "", "", "", minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo,  minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_middle_l);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 "", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate,closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);
			}
		}
	}
	//��Ҫ��ͬ�����ۣ���Ҫ��ͬ���ɹ���
	else
	{
		//�ֻ�����
		double spotprofit = (mainAgree.SpotPrice - minorAgree.SpotPrice)*minorAgree.SpotQty / tax;
		m_minorAgreeQty += minorAgree.SpotQty;
		m_minorAgreeTotal += minorAgree.SpotQty * minorAgree.SpotPrice;
		m_spotTotalProfit += spotprofit;

		userno = minorAgree.UserNo;
		opendate = minorAgree.SignDate;
		openagree = minorAgree.AgreementNo;
		openprice = minorAgree.SpotPrice;

		if (v_hedgeInfo.size() == 0)
		{
			if (isfirst && isfirstmain)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (isfirst)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10s��%8s��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					"", "", "", "", spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_middle_r);
				m_richeditAll.AddString(temp_txt);
			}
			return;
		}

		for (int i = 0; i < v_hedgeInfo.size(); i++)
		{
			SmsMatchDataRsp openmatch = { 0 };
			auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
			if (it_open_agree != m_map_mAgreeMatch.end())
			{
				auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
				if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
				{
					memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
				}
			}

			SmsMatchDataRsp closematch = { 0 };
			auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
			if (it_close_agree != m_map_mAgreeMatch.end())
			{
				auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
				if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
				{
					memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
				}
			}

			//ÿ�ֳ���
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//����һ�ʶԳ� ������ �ڻ�����
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//ÿ�ʶԳ����ϼ�
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//�����Լת���׺�Լ
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			if (i == 0 && isfirst && isfirstmain)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, mainAgree.SpotQty, spotprofit,
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0 && isfirst)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, spotprofit,
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10s��%8s��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					"", "", "", "", spotprofit,
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_middle_r);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 "", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);
			}
		}
	}
}

void IntegrationWnd::GeneratedListItem(SmsAgreementDataRsp mainAgree, SmsAgreementDataRsp minorAgree, vector<HedgeInfo>& v_hedgeInfo, bool isfirst, int close_remain, bool isfirstmain)

{
	//if (v_hedgeInfo.empty())
	//	return;

	bool bMainBuy = true;
	bMainBuy = mainAgree.AgreementType == SMS_AGREEMENT_PURCHASE;

	string userno("");
	string opendate("");
	string openagree("");
	double openprice = 0.0;

	//˰��
	double tax = 1.16;
	if (!m_System->m_setTax.empty())
	{
		auto it = m_System->m_setTax.end();
		tax = (*(--it)).Tax;
	}

	//��Ҫ��ͬ���ɹ�����Ҫ��ͬ�����ۣ�====================================
	if (bMainBuy)
	{
		//�ֻ�����
		double spotprofit = (minorAgree.SpotPrice - mainAgree.SpotPrice)*minorAgree.SpotQty / tax;
		m_minorAgreeQty += minorAgree.SpotQty;
		m_minorAgreeTotal += minorAgree.SpotQty * minorAgree.SpotPrice;
		m_spotTotalProfit += spotprofit;

		TRowInfo	tempinfo;

		if (v_hedgeInfo.size() == 0)
		{
			if (isfirst && isfirstmain)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", minorAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (isfirst)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", minorAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					"", "", "", "", minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo, minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_middle_l);
				m_richeditAll.AddString(temp_txt);

				strncpy_s(tempinfo.UserNo, "");
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", minorAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			return;
		}

		for (int i = 0; i < v_hedgeInfo.size(); i++)
		{
			SmsMatchDataRsp openmatch = { 0 };
			auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
			if (it_open_agree != m_map_mAgreeMatch.end())
			{
				auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
				if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
				{
					memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
				}
			}

			SmsMatchDataRsp closematch = { 0 };
			auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
			if (it_close_agree != m_map_mAgreeMatch.end())
			{
				auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
				if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
				{
					memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
				}
			}

			//ÿ�ֳ���
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//����һ�ʶԳ� ������ �ڻ�����
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//ÿ�ʶԳ����ϼ�
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//�����Լת���׺�Լ
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			if (i == 0 && isfirst && isfirstmain)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", minorAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (i == 0 && isfirst)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", minorAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (i == 0)
			{
				strncpy_s(tempinfo.UserNo, "");
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", minorAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					"", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);

				strncpy_s(tempinfo.UserNo, "");
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY2;
				m_RowinfoContainer.push_back(&tempinfo);
			}
		}
	}
	//��Ҫ��ͬ�����ۣ���Ҫ��ͬ���ɹ���
	else
	{
		//�ֻ�����
		double spotprofit = (mainAgree.SpotPrice - minorAgree.SpotPrice)*minorAgree.SpotQty / tax;
		m_minorAgreeQty += minorAgree.SpotQty;
		m_minorAgreeTotal += minorAgree.SpotQty * minorAgree.SpotPrice;
		m_spotTotalProfit += spotprofit;

		userno = minorAgree.UserNo;
		opendate = minorAgree.SignDate;
		openagree = minorAgree.AgreementNo;
		openprice = minorAgree.SpotPrice;

		TRowInfo tempinfo;

		if (v_hedgeInfo.size() == 0)
		{
			if (isfirst && isfirstmain)
			{
				strncpy_s(tempinfo.UserNo, minorAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", mainAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (isfirst)
			{
				strncpy_s(tempinfo.UserNo, minorAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", mainAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else
			{
				strncpy_s(tempinfo.UserNo, minorAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_SELL1;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			return;
		}

		for (int i = 0; i < v_hedgeInfo.size(); i++)
		{
			SmsMatchDataRsp openmatch = { 0 };
			auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
			if (it_open_agree != m_map_mAgreeMatch.end())
			{
				auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
				if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
				{
					memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
				}
			}

			SmsMatchDataRsp closematch = { 0 };
			auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
			if (it_close_agree != m_map_mAgreeMatch.end())
			{
				auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
				if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
				{
					memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
				}
			}

			//ÿ�ֳ���
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//����һ�ʶԳ� ������ �ڻ�����
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//ÿ�ʶԳ����ϼ�
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//�����Լת���׺�Լ
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			if (i == 0 && isfirst && isfirstmain)
			{
				strncpy_s(tempinfo.UserNo, minorAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", mainAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (i == 0 && isfirst)
			{
				strncpy_s(tempinfo.UserNo, minorAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", mainAgree.SpotQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (i == 0)
			{
				strncpy_s(tempinfo.UserNo, minorAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, minorAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, minorAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", minorAgree.SpotPrice);
				sprintf_s(tempinfo.BuyQty, "%8d", minorAgree.SpotQty);
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_SELL1;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					"", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);

				strncpy_s(tempinfo.UserNo, "");
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_SELL2;
				m_RowinfoContainer.push_back(&tempinfo);
			}
		}
	}
}

void IntegrationWnd::GeneratedSelfOffsetItem(SmsAgreementDataRsp mainAgree, vector<HedgeInfo>& v_hedgeInfo, bool isfirst, bool isfirstmain)
{	
	if (v_hedgeInfo.empty())
		return;

	bool bMainBuy = true;
	bMainBuy = mainAgree.AgreementType == SMS_AGREEMENT_PURCHASE;

	string userno("");
	string opendate("");
	string openagree("");
	double openprice = 0.0;

	//˰��
	//double tax = 1.16;
	//if (!m_System->m_setTax.empty())
	//{
	//	auto it = m_System->m_setTax.end();
	//	tax = (*(--it)).Tax;
	//}

	//��Ҫ��ͬ���ɹ�����Ҫ��ͬ�����ۣ�====================================
	if (bMainBuy)
	{
		//�ֻ�����
		double spotprofit = 0;
		//double spotprofit = (minorAgree.SpotPrice - mainAgree.SpotPrice)*minorAgree.SpotQty / tax;
		//m_minorAgreeQty += minorAgree.SpotQty;
		//m_minorAgreeTotal += minorAgree.SpotQty * minorAgree.SpotPrice;
		m_spotTotalProfit += spotprofit;

	/*	if (v_hedgeInfo.size() == 0)
		{
			if (isfirst)
			{
				userno = mainAgree.UserNo;
				opendate = mainAgree.SignDate;
				openagree = mainAgree.AgreementNo;
				openprice = mainAgree.SpotPrice;

				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-4s��%-14s��%-10s��%-20s��%10.2f��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					"", userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo, minorAgree.SpotQty, minorAgree.SpotPrice, spotprofit,
					"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-4s��%-14s��%-10s��%-20s��%10s��%8d��%-10s��%-20s��%10.2f��%8d��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					"", "", "", "", "", minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo, minorAgree.SpotQty, minorAgree.SpotPrice, spotprofit,
					"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
				m_richeditAll.AddString(row_text_middle_l);
				m_richeditAll.AddString(temp_txt);
			}
			return;
		}*/

		for (int i = 0; i < v_hedgeInfo.size(); i++)
		{
			SmsMatchDataRsp openmatch = { 0 };
			auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
			if (it_open_agree != m_map_mAgreeMatch.end())
			{
				auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
				if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
				{
					memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
				}
			}

			SmsMatchDataRsp closematch = { 0 };
			auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
			if (it_close_agree != m_map_mAgreeMatch.end())
			{
				auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
				if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
				{
					memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
				}
			}

			//ÿ�ֳ���
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//����һ�ʶԳ� ������ �ڻ�����
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//ÿ�ʶԳ����ϼ�
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//�����Լת���׺�Լ
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			if (i == 0 && isfirst && isfirstmain)
			{
				userno = mainAgree.UserNo;
				opendate = mainAgree.SignDate;
				openagree = mainAgree.AgreementNo;
				openprice = mainAgree.SpotPrice;

				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, "",
					"", "",  "", "", "",
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0 && isfirst)
			{
				userno = mainAgree.UserNo;
				opendate = mainAgree.SignDate;
				openagree = mainAgree.AgreementNo;
				openprice = mainAgree.SpotPrice;

				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, "",
					"", "", "", "", "",
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_middle_l);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 "", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);
			}
		}
	}
	//��Ҫ��ͬ�����ۣ���Ҫ��ͬ���ɹ���
	else
	{
		//�ֻ�����
		double spotprofit = 0;
		//double spotprofit = (mainAgree.SpotPrice - minorAgree.SpotPrice)*minorAgree.SpotQty / tax;
		//m_minorAgreeQty += minorAgree.SpotQty;
		//m_minorAgreeTotal += minorAgree.SpotQty * minorAgree.SpotPrice;
		m_spotTotalProfit += spotprofit;

		userno = mainAgree.UserNo;
		opendate = mainAgree.SignDate;
		openagree = mainAgree.AgreementNo;
		openprice = mainAgree.SpotPrice;

		//if (v_hedgeInfo.size() == 0)
		//{
		//	if (isfirst)
		//	{
		//		char temp_txt[1024] = { 0 };
		//		sprintf_s(temp_txt, "��%-4s��%-14s��%-10s��%-20s��%10.2f��%8d����%-10s��%-20s��%10.2f��%8d��%10.2f����%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
		//			"", userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
		//			mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotQty, mainAgree.SpotPrice, spotprofit,
		//			"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
		//		m_richeditAll.AddString(row_text_start);
		//		m_richeditAll.AddString(temp_txt);
		//	}
		//	else
		//	{
		//		char temp_txt[1024] = { 0 };
		//		sprintf_s(temp_txt, "��%-4s��%-14s��%-10s��%-20s��%10.2f��%8d����%-10s��%-20s��%10s��%8s��%10.2f����%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
		//			"", userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
		//			"", "", "", "", spotprofit,
		//			"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
		//		m_richeditAll.AddString(row_text_middle_r);
		//		m_richeditAll.AddString(temp_txt);
		//	}
		//	return;
		//}

		for (int i = 0; i < v_hedgeInfo.size(); i++)
		{
			SmsMatchDataRsp openmatch = { 0 };
			auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
			if (it_open_agree != m_map_mAgreeMatch.end())
			{
				auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
				if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
				{
					memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
				}
			}

			SmsMatchDataRsp closematch = { 0 };
			auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
			if (it_close_agree != m_map_mAgreeMatch.end())
			{
				auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
				if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
				{
					memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
				}
			}

			//ÿ�ֳ���
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//����һ�ʶԳ� ������ �ڻ�����
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//ÿ�ʶԳ����ϼ�
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//�����Լת���׺�Լ
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			if (i == 0 && isfirst && isfirstmain)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10.2f��%8d��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), "", "", "", "",
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, "",
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0 && isfirst)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10.2f��%8d��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), "", "", "", "",
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, "",
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10.2f��%8d��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 userno.c_str(), "", "", "", "",
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_middle_r);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 "", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);
			}
		}
	}
}

void IntegrationWnd::GeneratedSelfOffsetListItem(SmsAgreementDataRsp mainAgree, vector<HedgeInfo>& v_hedgeInfo, bool isfirst, bool isfirstmain)
{
	if (v_hedgeInfo.empty())
		return;

	bool bMainBuy = true;
	bMainBuy = mainAgree.AgreementType == SMS_AGREEMENT_PURCHASE;

	string userno("");
	string opendate("");
	string openagree("");
	double openprice = 0.0;

	//��Ҫ��ͬ���ɹ�����Ҫ��ͬ�����ۣ�====================================
	if (bMainBuy)
	{
		//�ֻ�����
		double spotprofit = 0;
		//double spotprofit = (minorAgree.SpotPrice - mainAgree.SpotPrice)*minorAgree.SpotQty / tax;
		//m_minorAgreeQty += minorAgree.SpotQty;
		//m_minorAgreeTotal += minorAgree.SpotQty * minorAgree.SpotPrice;
		m_spotTotalProfit += spotprofit;

		for (int i = 0; i < v_hedgeInfo.size(); i++)
		{
			SmsMatchDataRsp openmatch = { 0 };
			auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
			if (it_open_agree != m_map_mAgreeMatch.end())
			{
				auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
				if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
				{
					memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
				}
			}

			SmsMatchDataRsp closematch = { 0 };
			auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
			if (it_close_agree != m_map_mAgreeMatch.end())
			{
				auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
				if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
				{
					memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
				}
			}

			//ÿ�ֳ���
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//����һ�ʶԳ� ������ �ڻ�����
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//ÿ�ʶԳ����ϼ�
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//�����Լת���׺�Լ
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			TRowInfo	tempinfo;

			if (i == 0 && isfirst && isfirstmain)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f",closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d",v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (i == 0 && isfirst)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (i == 0)
			{
				//char temp_txt[1024] = { 0 };
				//sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
				//	userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, "",
				//	"", "", "", "", "",
				//	tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				//m_richeditAll.AddString(row_text_middle_l);
				//m_richeditAll.AddString(temp_txt);
				strncpy_s(tempinfo.UserNo, userno.c_str());
				strncpy_s(tempinfo.BuyAgreeDate, opendate.c_str());
				strncpy_s(tempinfo.BuyAgreeNo, openagree.c_str());
				sprintf_s(tempinfo.BuyPrice, "%10.2f", openprice);
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					"", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);
				strncpy_s(tempinfo.UserNo, "");
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY2;
				m_RowinfoContainer.push_back(&tempinfo);
			}
		}
	}
	//��Ҫ��ͬ�����ۣ���Ҫ��ͬ���ɹ���
	else
	{
		//�ֻ�����
		double spotprofit = 0;
		//double spotprofit = (mainAgree.SpotPrice - minorAgree.SpotPrice)*minorAgree.SpotQty / tax;
		//m_minorAgreeQty += minorAgree.SpotQty;
		//m_minorAgreeTotal += minorAgree.SpotQty * minorAgree.SpotPrice;
		m_spotTotalProfit += spotprofit;

		userno = mainAgree.UserNo;
		opendate = mainAgree.SignDate;
		openagree = mainAgree.AgreementNo;
		openprice = mainAgree.SpotPrice;

		for (int i = 0; i < v_hedgeInfo.size(); i++)
		{
			SmsMatchDataRsp openmatch = { 0 };
			auto it_open_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).OpenAgreementNo);
			if (it_open_agree != m_map_mAgreeMatch.end())
			{
				auto it_open_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].find(v_hedgeInfo.at(i).OpenNo);
				if (it_open_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo].end())
				{
					memcpy(&openmatch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).OpenAgreementNo][v_hedgeInfo.at(i).OpenNo], sizeof(SmsMatchDataRsp));
				}
			}

			SmsMatchDataRsp closematch = { 0 };
			auto it_close_agree = m_map_mAgreeMatch.find(v_hedgeInfo.at(i).CloseAgreementNo);
			if (it_close_agree != m_map_mAgreeMatch.end())
			{
				auto it_close_match = m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].find(v_hedgeInfo.at(i).CloseNo);
				if (it_close_match != m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo].end())
				{
					memcpy(&closematch, &m_map_mAgreeMatch[v_hedgeInfo.at(i).CloseAgreementNo][v_hedgeInfo.at(i).CloseNo], sizeof(SmsMatchDataRsp));
				}
			}

			//ÿ�ֳ���
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//����һ�ʶԳ� ������ �ڻ�����
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//ÿ�ʶԳ����ϼ�
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//�����Լת���׺�Լ
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			TRowInfo	tempinfo;

			if (i == 0 && isfirst && isfirstmain)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty,"%8d", mainAgree.SpotQty);
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (i == 0 && isfirst)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", mainAgree.SpotQty);
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (i == 0)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.SellAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.SellPrice, "%10.2f", mainAgree.SpotPrice);
				sprintf_s(tempinfo.SellQty, "%8d", mainAgree.SpotQty);
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_SELL1;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else
			{
				strncpy_s(tempinfo.UserNo, "");
				strncpy_s(tempinfo.BuyAgreeDate, "");
				strncpy_s(tempinfo.BuyAgreeNo, "");
				strncpy_s(tempinfo.BuyPrice, "");
				strncpy_s(tempinfo.BuyQty, "");
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				strncpy_s(tempinfo.SellPrice, "");
				strncpy_s(tempinfo.SellQty, "");
				strncpy_s(tempinfo.SpotProfit, "");
				strncpy_s(tempinfo.ContractNo, tcode.c_str());
				strncpy_s(tempinfo.OpenDate, openmatch.SettleDate);
				sprintf_s(tempinfo.OpenPrice, "%10.2f", openmatch.MatchPrice);
				strncpy_s(tempinfo.CoverDate, closematch.SettleDate);
				sprintf_s(tempinfo.CoverPrice, "%10.2f", closematch.MatchPrice);
				sprintf_s(tempinfo.FutureQty, "%8d", v_hedgeInfo.at(i).CloseQty);
				sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
				sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_SELL2;
				m_RowinfoContainer.push_back(&tempinfo);
			}
		}
	}
}


void IntegrationWnd::CalculateSpotOpen(SmsAgreementDataRsp mainAgree, bool isfirst, bool isfirstmain)
{
	//û�д�Ҫ��ͬ����Ҫ���ֳ��ڻ�ͷ��
	if (m_minorAgreeQty == 0)
	{
		int spotOpen = mainAgree.SpotQty - m_minorAgreeQty;
		//�������ֺ�ͬ�ĳɽ�
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end() && !m_map_mAgreeMatch[mainAgree.AgreementNo].empty())
		{
			bool b_first = true;

			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//�ֻ��ܼ�
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//�����Լת���׺�Լ
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//�ڻ������ۣ����¼�
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;
				if (contract[0] && contract[0]->Commodity)
					m_tradeDot = contract[0]->Commodity->TradeDot;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty* m_tradeDot;;
				//�ڻ��ϼ�
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//������*��
				char sSpotOpenQty[24] = { 0 };
				char sMatchQty[24] = { 0 };
				sprintf_s(sSpotOpenQty, "*%d", spotOpen);
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				char temp_txt[1024] = { 0 };
				if (b_first && isfirst && isfirstmain)
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10.2f��%8s��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
						"", "",  spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_start);
					m_richeditAll.AddString(temp_txt);
				}
				else if (b_first && isfirst)
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10.2f��%8s��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_sumbottom);
					m_richeditAll.AddString(temp_txt);
				}
				else if (b_first)
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10.2f��%8s��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_middle_l);
					m_richeditAll.AddString(temp_txt);
				}
				else
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 "", "", "", "", "",
						"", "", "", "", "",
						tcode.c_str(), it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					//m_richeditAll.AddString(row_mid);
					m_richeditAll.AddString(temp_txt);
				}
				if (b_first) b_first = !b_first;
			}
		}
		else
		{
			//Ҳû�гɽ���ֻ��ʾ�ֻ��ֲ�
			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//�ֻ��ܼ�
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			//������*��
			char sSpotOpenQty[24] = { 0 };
			sprintf_s(sSpotOpenQty, "*%d", spotOpen);

			char temp_txt[1024] = { 0 };
			if (isfirst && isfirstmain)
			{
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10.2f��%8s��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
					"", "", spotprice, sSpotOpenQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (isfirst)
			{
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10.2f��%8s��%10.2f��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
					 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
					"", "", spotprice, sSpotOpenQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}


		}
		return;
	}
	if (m_minorAgreeQty < mainAgree.SpotQty)	//�г���
	{
		int spotOpen = mainAgree.SpotQty - m_minorAgreeQty;
		//�������ֺ�ͬ�ĳɽ�
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end())
		{
			bool b_first = true;

			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//�ֻ��ܼ�
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//�����Լת���׺�Լ
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//�ڻ������ۣ����¼�
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty * m_tradeDot;
				//�ڻ��ϼ�
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//������*��
				char sSpotOpenQty[24] = { 0 };
				char sMatchQty[24] = { 0 };
				sprintf_s(sSpotOpenQty, "*%d", spotOpen);
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				char temp_txt[1024] = { 0 };
				if (b_first && isfirst && isfirstmain)
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10.2f��%8s��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 "", "", "", "", sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_start);
					m_richeditAll.AddString(temp_txt);
				}
				else if (b_first && isfirst)
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10.2f��%8s��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 "", "", "", "", sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_sumbottom);
					m_richeditAll.AddString(temp_txt);
				}
				else if (b_first)
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10.2f��%8s��%10.2f��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 "", "", "", "", sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_middle_l);
					m_richeditAll.AddString(temp_txt);
				}
				else
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 "", "", "", "", "",
						"", "", "", "", "",
						tcode.c_str(), it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					//m_richeditAll.AddString(row_mid);
					m_richeditAll.AddString(temp_txt);
				}
				if (b_first) b_first = !b_first;
			}
		}
		else
		{
			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//������*��
			char sSpotOpenQty[24] = { 0 };
			sprintf_s(sSpotOpenQty, "*%d", spotOpen);

			char temp_txt[1024] = { 0 };
			if (isfirst && isfirstmain)
			{
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s����%-10s��%-20s��%10.2f��%8s��%10.2f����%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
					"", "", spotprice, sSpotOpenQty, spotprofit,
					"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (isfirst)
			{
				sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s����%-10s��%-20s��%10.2f��%8s��%10.2f����%-14s��%-10s��%10.2f��%-10s��%10.2f��%8d��%10.2f��%10.2f��%-10s��",
					 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
					"", "", spotprice, sSpotOpenQty, spotprofit,
					"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}


		}
		return;
	}
	//û���ڵ������ڻ�ʣ�ࣨ���κ�ͬ�����п��ֶ�Ҫ���ǣ�
	if(m_minorAgreeQty >= mainAgree.SpotQty)
	{
		int spotOpen = 0;
		//�������ֺ�ͬ�ĳɽ�
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end())
		{
			bool b_first = true;

			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//�ֻ��ܼ�
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//�����Լת���׺�Լ
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//�ڻ������ۣ����¼�
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty * m_tradeDot;
				//�ڻ��ϼ�
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//������*��
				char sMatchQty[24] = { 0 };
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				char temp_txt[1024] = { 0 };
				if (b_first)
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 "", "", "", "", "",
						"", "",  "", "", "",
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_middle_l);
					m_richeditAll.AddString(temp_txt);
				}
				else
				{
					sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10s��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10.2f��%-10s��%10.2f��%8s��%10.2f��%10.2f��%-10s��",
						 "", "", "", "", "",
						"", "", "", "", "",
						tcode.c_str(), it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					//m_richeditAll.AddString(row_mid);
					m_richeditAll.AddString(temp_txt);
				}
				if (b_first) b_first = !b_first;
			}
		}
		else
		{
			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			char temp_txt[1024] = { 0 };
			sprintf_s(temp_txt, "��%-14s��%-10s��%-20s��%10.2f��%8s��%-10s��%-20s��%10s��%8s��%10s��%-14s��%-10s��%10s��%-10s��%10s��%8s��%10s��%10s��%-10s��",
				mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, "",
				"", "",  "", "", "",
				"", "", "", "", "", "", "", "", "");
			m_richeditAll.AddString(row_text_middle_l);
			m_richeditAll.AddString(temp_txt);

		}
		return;
	}
}

void IntegrationWnd::CalculateSpotOpenList(SmsAgreementDataRsp mainAgree, bool isfirst, bool isfirstmain)
{
	//û�д�Ҫ��ͬ����Ҫ���ֳ��ڻ�ͷ��
	if (m_minorAgreeQty == 0)
	{
		int spotOpen = mainAgree.SpotQty - m_minorAgreeQty;
		//�������ֺ�ͬ�ĳɽ�
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end() && !m_map_mAgreeMatch[mainAgree.AgreementNo].empty())
		{
			bool b_first = true;

			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//�ֻ��ܼ�
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//�����Լת���׺�Լ
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//�ڻ������ۣ����¼�
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;
				if (contract[0] && contract[0]->Commodity)
					m_tradeDot = contract[0]->Commodity->TradeDot;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty* m_tradeDot;;
				//�ڻ��ϼ�
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//������*��
				char sSpotOpenQty[24] = { 0 };
				char sMatchQty[24] = { 0 };
				sprintf_s(sSpotOpenQty, "*%d", spotOpen);
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				TRowInfo	tempinfo;
				if (b_first && isfirst && isfirstmain)
				{
					strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
					strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
					strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
					sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
					strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
					strncpy_s(tempinfo.SellQty, sSpotOpenQty);
					sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				else if (b_first && isfirst)
				{
					strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
					strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
					strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
					sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
					strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
					strncpy_s(tempinfo.SellQty, sSpotOpenQty);
					sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				else if (b_first)
				{
					strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
					strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
					strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
					sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
					strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
					strncpy_s(tempinfo.SellQty, sSpotOpenQty);
					sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				else
				{
					strncpy_s(tempinfo.UserNo, "");
					strncpy_s(tempinfo.BuyAgreeDate, "");
					strncpy_s(tempinfo.BuyAgreeNo, "");
					strncpy_s(tempinfo.BuyPrice, "");
					strncpy_s(tempinfo.BuyQty, "");
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					strncpy_s(tempinfo.SellPrice, "");
					strncpy_s(tempinfo.SellQty, "");
					strncpy_s(tempinfo.SpotProfit, "");
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY2;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				if (b_first) b_first = !b_first;
			}
		}
		else
		{
			//Ҳû�гɽ���ֻ��ʾ�ֻ��ֲ�
			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//�ֻ��ܼ�
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			//������*��
			char sSpotOpenQty[24] = { 0 };
			sprintf_s(sSpotOpenQty, "*%d", spotOpen);

			TRowInfo	tempinfo;
			if (isfirst && isfirstmain)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
				strncpy_s(tempinfo.SellQty, sSpotOpenQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (isfirst)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
				strncpy_s(tempinfo.SellQty, sSpotOpenQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BASE;
				m_RowinfoContainer.push_back(&tempinfo);
			}


		}
		return;
	}
	if (m_minorAgreeQty < mainAgree.SpotQty)	//�г���
	{
		int spotOpen = mainAgree.SpotQty - m_minorAgreeQty;
		//�������ֺ�ͬ�ĳɽ�
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end())
		{
			bool b_first = true;

			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//�ֻ��ܼ�
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//�����Լת���׺�Լ
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//�ڻ������ۣ����¼�
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty * m_tradeDot;
				//�ڻ��ϼ�
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//������*��
				char sSpotOpenQty[24] = { 0 };
				char sMatchQty[24] = { 0 };
				sprintf_s(sSpotOpenQty, "*%d", spotOpen);
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				TRowInfo	tempinfo;
				if (b_first && isfirst && isfirstmain)
				{
					strncpy_s(tempinfo.UserNo, "");
					strncpy_s(tempinfo.BuyAgreeDate, "");
					strncpy_s(tempinfo.BuyAgreeNo, "");
					strncpy_s(tempinfo.BuyPrice, "");
					strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
					strncpy_s(tempinfo.SellQty, sSpotOpenQty);
					sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				else if (b_first && isfirst)
				{
					strncpy_s(tempinfo.UserNo, "");
					strncpy_s(tempinfo.BuyAgreeDate, "");
					strncpy_s(tempinfo.BuyAgreeNo, "");
					strncpy_s(tempinfo.BuyPrice, "");
					strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
					strncpy_s(tempinfo.SellQty, sSpotOpenQty);
					sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				else if (b_first)
				{
					strncpy_s(tempinfo.UserNo, "");
					strncpy_s(tempinfo.BuyAgreeDate, "");
					strncpy_s(tempinfo.BuyAgreeNo, "");
					strncpy_s(tempinfo.BuyPrice, "");
					strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
					strncpy_s(tempinfo.SellQty, sSpotOpenQty);
					sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				else
				{
					strncpy_s(tempinfo.UserNo, "");
					strncpy_s(tempinfo.BuyAgreeDate, "");
					strncpy_s(tempinfo.BuyAgreeNo, "");
					strncpy_s(tempinfo.BuyPrice, "");
					strncpy_s(tempinfo.BuyQty, "");
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					strncpy_s(tempinfo.SellPrice, "");
					strncpy_s(tempinfo.SellQty, "");
					strncpy_s(tempinfo.SpotProfit, "");
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY2;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				if (b_first) b_first = !b_first;
			}
		}
		else
		{
			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//������*��
			char sSpotOpenQty[24] = { 0 };
			sprintf_s(sSpotOpenQty, "*%d", spotOpen);

			TRowInfo	tempinfo;
			if (isfirst && isfirstmain)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
				strncpy_s(tempinfo.SellQty, sSpotOpenQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
				m_RowinfoContainer.push_back(&tempinfo);
			}
			else if (isfirst)
			{
				strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
				strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
				strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
				sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
				strncpy_s(tempinfo.BuyQty, sSpotOpenQty);
				strncpy_s(tempinfo.SellAgreeDate, "");
				strncpy_s(tempinfo.SellAgreeNo, "");
				sprintf_s(tempinfo.SellPrice, "%10.2f", spotprice);
				strncpy_s(tempinfo.SellQty, sSpotOpenQty);
				sprintf_s(tempinfo.SpotProfit, "%10.2f", spotprofit);
				strncpy_s(tempinfo.ContractNo, "");
				strncpy_s(tempinfo.OpenDate, "");
				strncpy_s(tempinfo.OpenPrice, "");
				strncpy_s(tempinfo.CoverDate, "");
				strncpy_s(tempinfo.CoverPrice, "");
				strncpy_s(tempinfo.FutureQty, "");
				strncpy_s(tempinfo.FutureFee, "");
				strncpy_s(tempinfo.FutureProfit, "");
				strncpy_s(tempinfo.TotalProfit, "");

				tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
				m_RowinfoContainer.push_back(&tempinfo);
			}


		}
		return;
	}
	//û���ڵ������ڻ�ʣ�ࣨ���κ�ͬ�����п��ֶ�Ҫ���ǣ�
	if (m_minorAgreeQty >= mainAgree.SpotQty)
	{
		int spotOpen = 0;
		//�������ֺ�ͬ�ĳɽ�
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end())
		{
			bool b_first = true;

			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			//�ֻ��ܼ�
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//�����Լת���׺�Լ
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//�ڻ������ۣ����¼�
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty * m_tradeDot;
				//�ڻ��ϼ�
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//������*��
				char sMatchQty[24] = { 0 };
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				TRowInfo	tempinfo;
				if (b_first)
				{
					strncpy_s(tempinfo.UserNo, "");
					strncpy_s(tempinfo.BuyAgreeDate, "");
					strncpy_s(tempinfo.BuyAgreeNo, "");
					strncpy_s(tempinfo.BuyPrice, "");
					strncpy_s(tempinfo.BuyQty, "");
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					strncpy_s(tempinfo.SellPrice, "");
					strncpy_s(tempinfo.SellQty, "");
					strncpy_s(tempinfo.SpotProfit, "");
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				else
				{
					strncpy_s(tempinfo.UserNo, "");
					strncpy_s(tempinfo.BuyAgreeDate, "");
					strncpy_s(tempinfo.BuyAgreeNo, "");
					strncpy_s(tempinfo.BuyPrice, "");
					strncpy_s(tempinfo.BuyQty, "");
					strncpy_s(tempinfo.SellAgreeDate, "");
					strncpy_s(tempinfo.SellAgreeNo, "");
					strncpy_s(tempinfo.SellPrice, "");
					strncpy_s(tempinfo.SellQty, "");
					strncpy_s(tempinfo.SpotProfit, "");
					strncpy_s(tempinfo.ContractNo, tcode.c_str());
					strncpy_s(tempinfo.OpenDate, it_open_match.second.SettleDate);
					sprintf_s(tempinfo.OpenPrice, "%10.2f", it_open_match.second.MatchPrice);
					strncpy_s(tempinfo.CoverDate, "");
					sprintf_s(tempinfo.CoverPrice, "%10.2f", settleprice);
					strncpy_s(tempinfo.FutureQty, sMatchQty);
					sprintf_s(tempinfo.FutureFee, "%10.2f", fee);
					sprintf_s(tempinfo.FutureProfit, "%10.2f", futureprofit);
					strncpy_s(tempinfo.TotalProfit, "");

					tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY2;
					m_RowinfoContainer.push_back(&tempinfo);
				}
				if (b_first) b_first = !b_first;
			}
		}
		else
		{
			//ȡ�ֻ��۸�,�ֻ�����
			double spotprice = 0.0;
			auto it_spec_price = m_map_specPrice.find(mainAgree.SpecificationNo);
			if (it_spec_price != m_map_specPrice.end())
			{
				if (!m_map_specPrice[mainAgree.SpecificationNo].empty())
					spotprice = m_map_specPrice[mainAgree.SpecificationNo].begin()->SpecificationPirce;
			}
			double tax = 1.16;
			if (!m_System->m_setTax.empty())
			{
				auto it = m_System->m_setTax.end();
				tax = (*(--it)).Tax;
			}
			double spotprofit = spotprice == 0.0 ? 0 : (spotprice - mainAgree.SpotPrice) * spotOpen / tax;

			TRowInfo	tempinfo;
			strncpy_s(tempinfo.UserNo, mainAgree.UserNo);
			strncpy_s(tempinfo.BuyAgreeDate, mainAgree.SignDate);
			strncpy_s(tempinfo.BuyAgreeNo, mainAgree.AgreementNo);
			sprintf_s(tempinfo.BuyPrice, "%10.2f", mainAgree.SpotPrice);
			strncpy_s(tempinfo.BuyQty, "");
			strncpy_s(tempinfo.SellAgreeDate, "");
			strncpy_s(tempinfo.SellAgreeNo, "");
			strncpy_s(tempinfo.SellPrice, "");
			strncpy_s(tempinfo.SellQty, "");
			strncpy_s(tempinfo.SpotProfit, "");
			strncpy_s(tempinfo.ContractNo, "");
			strncpy_s(tempinfo.OpenDate, "");
			strncpy_s(tempinfo.OpenPrice, "");
			strncpy_s(tempinfo.CoverDate, "");
			strncpy_s(tempinfo.CoverPrice, "");
			strncpy_s(tempinfo.FutureQty, "");
			strncpy_s(tempinfo.FutureFee, "");
			strncpy_s(tempinfo.FutureProfit, "");
			strncpy_s(tempinfo.TotalProfit, "");

			tempinfo.RowInfoFormat = SMS_ROWINFOFORMAT_BUY1;
			m_RowinfoContainer.push_back(&tempinfo);

		}
		return;
	}

}

void IntegrationWnd::FindMinorAgree(string maingreeno, set<SmsAgreementDataRsp>& set_minoragree)
{
	string key = m_System->get_curkey();
	bool isBuy = m_map_Agree[maingreeno].AgreementType == SMS_AGREEMENT_PURCHASE ? true : false;

	for (auto it_rel : m_System->m_mapAgreementRel[key])
	{
		if (isBuy)
		{
			if (strcmp(it_rel.BuyAgreementNo, maingreeno.c_str()) == 0)
			{
				//<��ͬ�ţ���ͬ����>
				m_map_Agree[it_rel.SellAgreementNo] = m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo];
				//<��Ҫ��ͬ����>
				set_minoragree.insert(m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo]);
			}
		}
		else
		{
			if (strcmp(it_rel.SellAgreementNo, maingreeno.c_str()) == 0)
			{
				//<��ͬ�ţ���ͬ����>
				m_map_Agree[it_rel.BuyAgreementNo] = m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo];
				//<��Ҫ��ͬ����>
				set_minoragree.insert(m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo]);
			}
		}
	}
}

std::vector<std::string> IntegrationWnd::SplitString(const std::string & str, char cSplit)
{
	vector<string> vecstr;
	string tempstr(str);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), cSplit);
		if (tempit != tempstr.end())
		{
			/*iterator_traits<string::iterator>::value_type*/int dis = distance(tempstr.begin(), tempit);
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
			/*iterator_traits<string::iterator>::value_type*/int dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}

std::vector<std::string> IntegrationWnd::SplitString(const std::string & str, const string sSplit)
{
	std::vector<std::string> resVec;
	resVec.clear();

	if ("" == str)
	{
		return resVec;
	}
	//�����ȡ���һ������
	std::string strs = str + sSplit;

	size_t pos = strs.find(sSplit);
	size_t size = strs.size();
	size_t size_split = sSplit.size();

	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		if (x.length() != 0)
		{
			resVec.push_back(x);
		}
		strs = strs.substr(pos + size_split, size);
		pos = strs.find(sSplit);
	}

	//���sizeΪ4
	if (resVec.size() == 4)
	{
		TrimString(resVec.at(1));
		if (resVec[1].at(0) == ' ' && resVec[0].at(0) == ' ')
			resVec.push_back("left");
		else if(resVec[1].at(0) == ' ' && resVec[0].at(0) != ' ')
			resVec.push_back("right");
	}

	return resVec;
}

std::vector<std::string> IntegrationWnd::SplitStringWithoutNone(const std::string & str, char cSplit)
{
	vector<string> vecstr;
	string tempstr(str);
	string::iterator tempit = tempstr.begin();
	while (tempit != tempstr.end())
	{
		tempit = find<string::iterator, char>(tempstr.begin(), tempstr.end(), cSplit);
		if (tempit != tempstr.end())
		{
			int dis = distance(tempstr.begin(), tempit);
			string realString = tempstr.substr(0, dis);
			if(realString.length()>0)
				vecstr.push_back(realString);
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
			int dis = distance(tempit, tempstr.end());
			if(tempstr.length()>0)
				vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}

void IntegrationWnd::TrimString(string & str)
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

int IntegrationWnd::DateStrToInt(const std::string & str)
{
	vector<string> tempvec = SplitString(str, '-');
	if (tempvec.size() < 3)
		return 0;
	int year = atoi(tempvec.at(0).c_str());
	int month = atoi(tempvec.at(1).c_str());
	int day = atoi(tempvec.at(2).c_str());
	return year * 10000 + month * 100 + day;
}

bool IntegrationWnd::SaveToExcel(wstring filename)
{
	//vector<string> v_str;
	//v_str.clear();
	//wstring wtxt = m_richeditAll.GetAllText();
	//string txt = LoadRC::unicode_to_ansi(wtxt);

	//vector<string> v_txt = SplitString(txt, '\r');
	//if (v_txt.size() < 4)
	//	return false;
	if (m_RowinfoContainer.size() == 0)
	{
		::MessageBoxA(m_Hwnd, "û�����ݿɱ��棡", "����", 0x10010);
		return false;
	}


	//д�뵽excel
	//------------------д��excel---------------------------
	// ��ʼ��COM��
	CoInitialize(NULL);

	// ���EXCEL��CLSID
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);

	if (FAILED(hr)) {
		//::MessageBoxA(m_Hwnd, "CLSIDFromProgID() ��������ʧ�ܣ�", "����", 0x10010);
		::MessageBoxA(m_Hwnd, "�����Ƿ��Ѿ���װEXCEL��", "����", 0x10010);
		return false;
	}

	// ����ʵ��
	IDispatch *pXlApp;
	hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&pXlApp);
	if (FAILED(hr)) {
		::MessageBoxA(m_Hwnd, "�����Ƿ��Ѿ���װEXCEL��", "����", 0x10010);
		return false;
	}

	// ��ʾ����Application.Visible������1
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 1;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"Visible", 1, x);
	}
	//ȡ����ʾ��
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"DisplayAlerts", 1, x);
	}

	// ��ȡWorkbooks����
	IDispatch *pXlBooks;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"Workbooks", 0);
		pXlBooks = result.pdispVal;
	}

	// ����Workbooks.Add()����������һ���µ�Workbook
	IDispatch *pXlBook;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlBooks, L"Add", 0);
		pXlBook = result.pdispVal;
	}

	// Create a 15x15 safearray of variants...
	VARIANT arr;
	arr.vt = VT_ARRAY | VT_VARIANT;
	{
		SAFEARRAYBOUND sab[2];
		sab[0].lLbound = 1; sab[0].cElements = m_RowinfoContainer.size() + 2;
		sab[1].lLbound = 1; sab[1].cElements = 19;
		arr.parray = SafeArrayCreate(VT_VARIANT, 2, sab);
	}

	// Fill safearray with some values...
	int h_index = 1;
	{
		VARIANT tmp;
		tmp.vt = VT_BSTR;
		tmp.bstrVal = SysAllocString(L"");
		long indices1[] = { h_index,2 };
		SafeArrayPutElement(arr.parray, indices1, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"");
		long indices2[] = { h_index,6 };
		SafeArrayPutElement(arr.parray, indices2, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"");
		long indices3[] = { h_index,11 };
		SafeArrayPutElement(arr.parray, indices3, (void *)&tmp);
	}
	h_index = 2;
	{
		VARIANT tmp;
		tmp.vt = VT_BSTR;
		tmp.bstrVal = SysAllocString(L"�˺�");
		long indices1[] = { h_index,1 };
		SafeArrayPutElement(arr.parray, indices1, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����");
		long indices2[] = { h_index,2 };
		SafeArrayPutElement(arr.parray, indices2, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"��ͬ���");
		long indices3[] = { h_index,3 };
		SafeArrayPutElement(arr.parray, indices3, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����");
		long indices4[] = { h_index,4 };
		SafeArrayPutElement(arr.parray, indices4, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����");
		long indices5[] = { h_index,5 };
		SafeArrayPutElement(arr.parray, indices5, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����");
		long indices6[] = { h_index,6 };
		SafeArrayPutElement(arr.parray, indices6, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"��ͬ���");
		long indices7[] = { h_index,7 };
		SafeArrayPutElement(arr.parray, indices7, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����");
		long indices8[] = { h_index,8 };
		SafeArrayPutElement(arr.parray, indices8, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����");
		long indices9[] = { h_index,9 };
		SafeArrayPutElement(arr.parray, indices9, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����˰ӯ��");
		long indices10[] = { h_index,10 };
		SafeArrayPutElement(arr.parray, indices10, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"��Լ���");
		long indices11[] = { h_index,11 };
		SafeArrayPutElement(arr.parray, indices11, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"��������");
		long indices12[] = { h_index,12 };
		SafeArrayPutElement(arr.parray, indices12, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"���ּ۸�");
		long indices13[] = { h_index,13 };
		SafeArrayPutElement(arr.parray, indices13, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"ƽ������");
		long indices14[] = { h_index,14 };
		SafeArrayPutElement(arr.parray, indices14, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"ƽ�ּ۸�");
		long indices15[] = { h_index,15 };
		SafeArrayPutElement(arr.parray, indices15, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����");
		long indices16[] = { h_index,16 };
		SafeArrayPutElement(arr.parray, indices16, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"������");
		long indices17[] = { h_index,17 };
		SafeArrayPutElement(arr.parray, indices17, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"ӯ��");
		long indices18[] = { h_index,18 };
		SafeArrayPutElement(arr.parray, indices18, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"����ӯ��");
		long indices19[] = { h_index,19 };
		SafeArrayPutElement(arr.parray, indices19, (void *)&tmp);
	}

	bool b_dblSplit = false;
	int index_tablecell = 3;
	map<int, int>  map_tablecell;
	map_tablecell.clear();

	//for (int i = 5; i < v_txt.size(); i++) {
	//	vector<string> v_temp = SplitString(v_txt.at(i), "��");

	//	if (v_temp.size() == 1 || v_temp.size() == 5)
	//		continue;
	//	h_index++;

	//	for (auto& it : v_temp)
	//	{
	//		TrimString(it);
	//	}

	//	if (v_temp.size() == 19)	//����
	//	{
	//		for (int v_index = 0; v_index < 19; v_index++)
	//		{
	//			int excelIndex = v_index + 1;
	//			VARIANT tmp;
	//			tmp.vt = VT_BSTR;
	//			tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(v_temp.at(v_index)).c_str());
	//			 //Add to safearray...
	//			long indices[] = { h_index,excelIndex };
	//			SafeArrayPutElement(arr.parray, indices, (void *)&tmp);
	//		}
	//	}
	//	else if (v_temp.size() == 3)	//�ϼ�
	//	{
	//		vector<string> v_totalsum;
	//		v_totalsum.clear();
	//		for (int i3 = 0; i3 < v_temp.size(); i3++)
	//		{
	//			vector<string> v_totalsum_temp = SplitStringWithoutNone(v_temp[i3], ' ');
	//			for (auto it : v_totalsum_temp)
	//			{
	//				v_totalsum.push_back(it);
	//			}
	//		}

	//		for (int index_totalsum = 0; index_totalsum < v_totalsum.size(); index_totalsum++)
	//		{
	//			int excelIndex = 0;
	//			switch (index_totalsum)
	//			{
	//			case 0: excelIndex = 1; break;
	//			case 1: excelIndex = 10; break;
	//			case 2: excelIndex = 17; break;
	//			case 3: excelIndex = 18; break;
	//			case 4: excelIndex = 19; break;
	//			default:
	//				break;
	//			}
	//			VARIANT tmp;
	//			tmp.vt = VT_BSTR;
	//			tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(v_totalsum.at(index_totalsum)).c_str());
	//			 //Add to safearray...
	//			long indices[] = { h_index,excelIndex };
	//			SafeArrayPutElement(arr.parray, indices, (void *)&tmp);
	//		}
	//	}
	//	else if (v_temp.size() == 4)	//С��
	//	{
	//		vector<string> v_subsum;
	//		v_subsum.clear();
	//		for (int i4 = 0; i4 < v_temp.size(); i4++)
	//		{
	//			vector<string> v_subsum_temp = SplitStringWithoutNone(v_temp[i4], ' ');
	//			for (auto it : v_subsum_temp)
	//			{
	//				v_subsum.push_back(it);
	//			}
	//		}

	//		for (int index_subsum = 0; index_subsum < v_subsum.size(); index_subsum++)
	//		{
	//			int excelIndex = 0;
	//			switch (index_subsum)
	//			{
	//			case 0: excelIndex = 1; break;
	//			case 1: excelIndex = 4; break;
	//			case 2: excelIndex = 5; break;
	//			case 3: excelIndex = 8; break;
	//			case 4: excelIndex = 9; break;
	//			case 5: excelIndex = 10; break;
	//			case 6: excelIndex = 13; break;
	//			case 7: excelIndex = 15; break;
	//			case 8: excelIndex = 16; break;
	//			case 9: excelIndex = 17; break;
	//			case 10: excelIndex = 18; break;
	//			case 11: excelIndex = 19; break;
	//			default:
	//				break;
	//			}
	//			
	//			VARIANT tmp;
	//			tmp.vt = VT_BSTR;
	//			tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(v_subsum.at(index_subsum)).c_str());
	//			 //Add to safearray...
	//			long indices[] = { h_index,excelIndex };
	//			SafeArrayPutElement(arr.parray, indices, (void *)&tmp);
	//		}

	//		//��¼�ϲ���Ԫ��
	//		index_tablecell;
	//		int end = h_index - 1;
	//		if (end > index_tablecell)
	//		{
	//			map_tablecell[index_tablecell] = end;
	//			index_tablecell = h_index + 1;
	//		}
	//		else
	//		{
	//			index_tablecell = h_index + 1;
	//		}

	//	}
	//}


	for (int i = 0; i < m_RowinfoContainer.size(); i++)
	{
		//д���ַ���
		VARIANT tmp;
		tmp.vt = VT_BSTR;

		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).UserNo).c_str());
		long indices1[] = { i + 3, 1};
		SafeArrayPutElement(arr.parray, indices1, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).BuyAgreeDate).c_str());
		long indices2[] = { i + 3, 2 };
		SafeArrayPutElement(arr.parray, indices2, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).BuyAgreeNo).c_str());
		long indices3[] = { i + 3, 3 };
		SafeArrayPutElement(arr.parray, indices3, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).BuyPrice).c_str());
		long indices4[] = { i + 3, 4 };
		SafeArrayPutElement(arr.parray, indices4, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).BuyQty).c_str());
		long indices5[] = { i + 3, 5 };
		SafeArrayPutElement(arr.parray, indices5, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).SellAgreeDate).c_str());
		long indices6[] = { i + 3, 6 };
		SafeArrayPutElement(arr.parray, indices6, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).SellAgreeNo).c_str());
		long indices7[] = { i + 3, 7 };
		SafeArrayPutElement(arr.parray, indices7, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).SellPrice).c_str());
		long indices8[] = { i + 3, 8 };
		SafeArrayPutElement(arr.parray, indices8, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).SellQty).c_str());
		long indices9[] = { i + 3, 9 };
		SafeArrayPutElement(arr.parray, indices9, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).SpotProfit).c_str());
		long indices10[] = { i + 3, 10 };
		SafeArrayPutElement(arr.parray, indices10, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).ContractNo).c_str());
		long indices11[] = { i + 3, 11 };
		SafeArrayPutElement(arr.parray, indices11, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).OpenDate).c_str());
		long indices12[] = { i + 3, 12 };
		SafeArrayPutElement(arr.parray, indices12, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).OpenPrice).c_str());
		long indices13[] = { i + 3, 13 };
		SafeArrayPutElement(arr.parray, indices13, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).CoverDate).c_str());
		long indices14[] = { i + 3, 14 };
		SafeArrayPutElement(arr.parray, indices14, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).CoverPrice).c_str());
		long indices15[] = { i + 3, 15 };
		SafeArrayPutElement(arr.parray, indices15, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).FutureQty).c_str());
		long indices16[] = { i + 3, 16 };
		SafeArrayPutElement(arr.parray, indices16, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).FutureFee).c_str());
		long indices17[] = { i + 3, 17 };
		SafeArrayPutElement(arr.parray, indices17, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).FutureProfit).c_str());
		long indices18[] = { i + 3, 18 };
		SafeArrayPutElement(arr.parray, indices18, (void *)&tmp);
		tmp.bstrVal = SysAllocString(LoadRC::ansi_to_unicode(m_RowinfoContainer.GetData(i).TotalProfit).c_str());
		long indices19[] = { i + 3, 19 };
		SafeArrayPutElement(arr.parray, indices19, (void *)&tmp);

		//��¼�ϲ���Ԫ������
		if (strcmp(m_RowinfoContainer.GetData(i).UserNo, "С��") == 0)
		{
			int end = i + 3;
			if (end - 1 > index_tablecell)
			{
				map_tablecell[index_tablecell] = end - 1;
				index_tablecell = end + 1;
			}
			else
				index_tablecell = end + 1;
		}
	}


	//������������range
	wchar_t		real_range[24] = { 0 };
	swprintf_s(real_range, L"A1:S%d", 2 + m_RowinfoContainer.size());

	// Get ActiveSheet object
	IDispatch *pXlSheet;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"ActiveSheet", 0);
		pXlSheet = result.pdispVal;
	}

	// ѡ��һ��size*19��С��Range
	IDispatch *pXlRange;
	{
		VARIANT parm;
		parm.vt = VT_BSTR;
		//parm.bstrVal = ::SysAllocString(L"A1:R588");
		parm.bstrVal = ::SysAllocString(real_range);

		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
		VariantClear(&parm);

		pXlRange = result.pdispVal;
	}
	//�����п�
	{
		VARIANT parm;
		parm.vt = VT_I4;
		parm.lVal = 11.75;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"ColumnWidth", 1, parm);
	}
	// Set range with our safearray...
	AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, arr);

	//�ɹ��ϲ���Ԫ��
	{
		IDispatch *pXlRangeTitle1;
		{
			VARIANT parm;
			parm.vt = VT_BSTR;
			parm.bstrVal = ::SysAllocString(L"B1:E1");

			VARIANT result;
			VariantInit(&result);
			AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
			VariantClear(&parm);

			pXlRangeTitle1 = result.pdispVal;
		}
		AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"Merge", 0);
		pXlRangeTitle1->Release();
	}
	//���ۺϲ���Ԫ��
	{
		IDispatch *pXlRangeTitle1;
		{
			VARIANT parm;
			parm.vt = VT_BSTR;
			parm.bstrVal = ::SysAllocString(L"F1:J1");

			VARIANT result;
			VariantInit(&result);
			AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
			VariantClear(&parm);

			pXlRangeTitle1 = result.pdispVal;
		}
		AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"Merge", 0);
		pXlRangeTitle1->Release();
	}
	//�ڻ��ϲ���Ԫ��
	{
		IDispatch *pXlRangeTitle1;
		{
			VARIANT parm;
			parm.vt = VT_BSTR;
			parm.bstrVal = ::SysAllocString(L"K1:R1");

			VARIANT result;
			VariantInit(&result);
			AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
			VariantClear(&parm);

			pXlRangeTitle1 = result.pdispVal;
		}
		AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"Merge", 0);
		pXlRangeTitle1->Release();
	}
	//�ϲ���Ԫ��
	for (auto it : map_tablecell)
	{
		char range1[24] = { 0 };
		char range2[24] = { 0 };
		char range3[24] = { 0 };
		char range4[24] = { 0 };
		sprintf_s(range1, "A%d:A%d", it.first, it.second);
		sprintf_s(range2, "B%d:B%d", it.first, it.second);
		sprintf_s(range3, "C%d:C%d", it.first, it.second);
		sprintf_s(range4, "D%d:D%d", it.first, it.second);
		{
			IDispatch *pXlRangeTitle1;
			{
				VARIANT parm;
				parm.vt = VT_BSTR;
				parm.bstrVal = ::SysAllocString(LoadRC::ansi_to_unicode(range1).c_str());

				VARIANT result;
				VariantInit(&result);
				AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
				VariantClear(&parm);

				pXlRangeTitle1 = result.pdispVal;
			}
			AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"Merge", 0);
			pXlRangeTitle1->Release();
		}
		{
			IDispatch *pXlRangeTitle1;
			{
				VARIANT parm;
				parm.vt = VT_BSTR;
				parm.bstrVal = ::SysAllocString(LoadRC::ansi_to_unicode(range2).c_str());

				VARIANT result;
				VariantInit(&result);
				AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
				VariantClear(&parm);

				pXlRangeTitle1 = result.pdispVal;
			}
			AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"Merge", 0);
			pXlRangeTitle1->Release();
		}
		{
			IDispatch *pXlRangeTitle1;
			{
				VARIANT parm;
				parm.vt = VT_BSTR;
				parm.bstrVal = ::SysAllocString(LoadRC::ansi_to_unicode(range3).c_str());

				VARIANT result;
				VariantInit(&result);
				AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
				VariantClear(&parm);

				pXlRangeTitle1 = result.pdispVal;
			}
			AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"Merge", 0);
			pXlRangeTitle1->Release();
		}
		{
			IDispatch *pXlRangeTitle1;
			{
				VARIANT parm;
				parm.vt = VT_BSTR;
				parm.bstrVal = ::SysAllocString(LoadRC::ansi_to_unicode(range4).c_str());

				VARIANT result;
				VariantInit(&result);
				AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
				VariantClear(&parm);

				pXlRangeTitle1 = result.pdispVal;
			}
			AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"Merge", 0);
			pXlRangeTitle1->Release();
		}
	}
	//����Ӧ�п�
	//{
	//	IDispatch *pXlRangeTitle1;
	//	{
	//		VARIANT parm;
	//		parm.vt = VT_BSTR;
	//		parm.bstrVal = ::SysAllocString(L"A2:S2");

	//		VARIANT result;
	//		VariantInit(&result);
	//		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
	//		VariantClear(&parm);

	//		pXlRangeTitle1 = result.pdispVal;
	//	}
	//	AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"AutoFit", 0);
	//	pXlRangeTitle1->Release();
	//}

	//���ᴰ��
	{
		IDispatch *pXlRangeTitle1;
		{
			VARIANT parm;
			parm.vt = VT_BSTR;
			parm.bstrVal = ::SysAllocString(L"B3:B3");

			VARIANT result;
			VariantInit(&result);
			AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
			VariantClear(&parm);

			pXlRangeTitle1 = result.pdispVal;
		}
		AutoWrap(DISPATCH_METHOD, NULL, pXlRangeTitle1, L"Select", 0);
		pXlRangeTitle1->Release();

		IDispatch *pXlWindow;
		{
			VARIANT result;
			VariantInit(&result);
			AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"ActiveWindow", 0);
			pXlWindow = result.pdispVal;
		}

		{
			VARIANT x;
			x.vt = VT_BOOL;
			x.boolVal = -1;
			AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlWindow, L"FreezePanes", 1, x);
		}
	}


	// Wait for user...
	//::MessageBoxA(m_Hwnd, "All done.", "Notice", 0x10000);

	// Set .Saved property of workbook to TRUE so we aren't prompted
	// to save when we tell Excel to quit...
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlBook, L"Saved", 1, x);
	}


	// ���������Ǹñ����ļ��ˣ�����Worksheet.SaveAs()����(������������������в����������ļ���)
	//{
	//	VARIANT filename;
	//	filename.vt = VT_BSTR;
	//	filename.bstrVal = SysAllocString(L"R:\\test.xlsx");
	//	AutoWrap(DISPATCH_METHOD, NULL, pXlSheet, L"SaveAs", 1, filename);
	//}

	//�ָ���ʾ��
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 1;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"DisplayAlerts", 1, x);
	}

	// Tell Excel to quit (i.e. App.Quit)
	AutoWrap(DISPATCH_METHOD, NULL, pXlApp, L"Quit", 0);

	// Release references...
	pXlRange->Release();
	pXlSheet->Release();
	pXlBook->Release();
	pXlBooks->Release();
	pXlApp->Release();
	VariantClear(&arr);

	// Uninitialize COM for this thread...
	CoUninitialize();

	return true;
}


void IntegrationWnd::InitSplit()
{
	for (int i = 0; i < 126; i++)
	{
		switch (i)
		{
		//case 0:
		//	row_title_start.append(LEFT_TOP);
		//	row_title_middle1.append(LEFT_CENTER);
		//	row_title_middle2.append(LEFT_CENTER);
		//	row_text_start.append(LEFT_CENTER);
		//	row_text_middle_l.append(VERTICAL);
		//	row_text_middle_r.append(LEFT_CENTER);
		//	row_title_end.append(LEFT_BOTTOM);
		//	break;
		case 0:
			row_title_start.append(LEFT_TOP);
			row_title_middle1.append(LEFT_CENTER);
			row_title_middle2.append(LEFT_CENTER);
			row_text_start.append(LEFT_CENTER);
			row_text_middle_l.append(VERTICAL);
			row_text_middle_r.append(LEFT_CENTER);
			row_title_end.append(LEFT_BOTTOM);
			row_text_sumtop.append(LEFT_CENTER);
			row_text_sumbottom.append(LEFT_CENTER);
			row_text_totalsumtop.append(LEFT_CENTER);
			break;
		case 8:
			row_title_start.append(LATERAL);
			row_title_middle1.append(TOP_CENTER);
			row_title_middle2.append(CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(VERTICAL);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 14:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(VERTICAL);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 25:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(VERTICAL);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 31:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(LEFT_CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 36:
			row_title_start.append(LATERAL);
			row_title_middle1.append(TOP_CENTER);
			row_title_middle2.append(CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(RIGHT_CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(CENTER);
			row_text_sumbottom.append(CENTER);
			row_text_totalsumtop.append(BOTTOM_CENTER);
			break;
		case 42:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(VERTICAL);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 53:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(VERTICAL);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 59:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(VERTICAL);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 64:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(LEFT_CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 70:
			row_title_start.append(LATERAL);
			row_title_middle1.append(TOP_CENTER);
			row_title_middle2.append(CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(BOTTOM_CENTER);
			row_text_sumtop.append(CENTER);
			row_text_sumbottom.append(CENTER);
			row_text_totalsumtop.append(CENTER);
			break;
		case 78:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 84:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 90:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 96:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 102:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 107:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 113:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(TOP_CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(BOTTOM_CENTER);
			row_text_sumbottom.append(TOP_CENTER);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 119:
			row_title_start.append(LATERAL);
			row_title_middle1.append(TOP_CENTER);
			row_title_middle2.append(CENTER);
			row_text_start.append(CENTER);
			row_text_middle_l.append(CENTER);
			row_text_middle_r.append(CENTER);
			row_title_end.append(BOTTOM_CENTER);
			row_text_sumtop.append(CENTER);
			row_text_sumbottom.append(CENTER);
			row_text_totalsumtop.append(CENTER);
			break;
		case 125:
			row_title_start.append(RIGHT_TOP);
			row_title_middle1.append(RIGHT_CENTER);
			row_title_middle2.append(RIGHT_CENTER);
			row_text_start.append(RIGHT_CENTER);
			row_text_middle_l.append(RIGHT_CENTER);
			row_text_middle_r.append(RIGHT_CENTER);
			row_title_end.append(RIGHT_BOTTOM);
			row_text_sumtop.append(RIGHT_CENTER);
			row_text_sumbottom.append(RIGHT_CENTER);
			row_text_totalsumtop.append(RIGHT_CENTER);
			break;
		case 1:case 2:case 3:case 4:case 5:case 6:case 7:
		case 9:case 10:case 11:case 12:case 13:case 15:case 16:case 17:case 18:case 19:case 20:case 21:
		case 22:case 23:case 24:case 26:case 27:case 28:case 29:case 30:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(LATERAL);
			row_text_start.append(LATERAL);
			row_text_middle_l.append(SPACE);
			row_text_middle_r.append(LATERAL);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(LATERAL);
			row_text_sumbottom.append(LATERAL);
			row_text_totalsumtop.append(LATERAL);
			break;
		case 37:case 38:case 39:case 40:case 41:case 43:case 44:case 45:case 46:case 47:case 48:case 49:
		case 50:case 51:case 52:case 54:case 55:case 56:case 57:case 58:case 60:case 61:case 62:case 63:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(LATERAL);
			row_text_start.append(LATERAL);
			row_text_middle_l.append(LATERAL);
			row_text_middle_r.append(SPACE);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(LATERAL);
			row_text_sumbottom.append(LATERAL);
			row_text_totalsumtop.append(LATERAL);
			break;
		default:
			row_title_start.append(LATERAL);
			row_title_middle1.append(LATERAL);
			row_title_middle2.append(LATERAL);
			row_text_start.append(LATERAL);
			row_text_middle_l.append(LATERAL);
			row_text_middle_r.append(LATERAL);
			row_title_end.append(LATERAL);
			row_text_sumtop.append(LATERAL);
			row_text_sumbottom.append(LATERAL);
			row_text_totalsumtop.append(LATERAL);
			break;
		}
	}

	//row_split = row_title_start;
	//row_dblv = row_split;
	//row_mid_l = row_dblv;
	//row_mid_r = row_dblv;
}




