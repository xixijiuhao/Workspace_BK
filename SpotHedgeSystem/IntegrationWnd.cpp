#include "PreInclude.h"

extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern IStarApi            *g_pStarApi;					//行情 
#define V_TITLE				45							//抬头高度	
	
#define LEFT_TOP			"┌"
#define LEFT_BOTTOM			"└"
#define RIGHT_TOP			"┐"
#define RIGHT_BOTTOM		"┘"
#define LATERAL				"─"
#define VERTICAL			"│"
#define LEFT_CENTER			"├"
#define RIGHT_CENTER		"┤"
#define TOP_CENTER			"┬"
#define BOTTOM_CENTER		"┴"
#define CENTER				"┼"
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
	//初始化基础分隔符
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

	//开始日期
	m_labBeginDate.Create(m_Hwnd, L"开始日期", DT_RIGHT, ID_LabBeginDate);
	m_beginTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtBeginDate, m_hInstance, NULL);
	//结束日期
	m_labEndDate.Create(m_Hwnd, L"结束日期", DT_RIGHT, ID_LabEndDate);
	m_endTimePick = CreateWindowEx(0, DATETIMEPICK_CLASS, TEXT("DateTime"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | DTS_SHORTDATEFORMAT,
		0, 0, 150, 25, m_Hwnd, (HMENU)ID_CbtEndDate, m_hInstance, NULL);
	//合同
	m_labAgree.Create(m_Hwnd, L"合同编号", DT_CENTER, ID_LabAgree);
	m_cbAgree.Create(m_Hwnd, ID_CbAgree);
	//账号
	m_labUserno.Create(m_Hwnd, L"用户账号", DT_CENTER, ID_LabUserno);
	m_cbUserno.Create(m_Hwnd, ID_CbUserno);
	//查询
	m_btnQry.Create(m_Hwnd, "查  询", ID_BtnQry);
	//保存
	m_btnSave.Create(m_Hwnd, "保  存", ID_BtnSave);
	//对账单
	m_richeditAll.Create(m_Hwnd, ES_LEFT | ES_READONLY, 0, ID_EditAll);
	//EnableWindow(m_richeditAll.GetHwnd(), false);
	//对账表
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
	int editWidth = 120;	//edit控件宽度
	int labWidth = 60;		//lab控件宽度
	int btnWidth = 80;		//btn控件宽度
	int Height = 25;		//控件高度
	int htap = 10;			//横向间隔

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

	//HFONT hfont =  CreateFont(-14, 0, 0, 0, 0, FALSE, FALSE, FALSE, GB2312_CHARSET, 0, 0, 0, 0, L"黑体");
	//m_richeditAll.SetFont(hfont);

	//查成交
	SmsMatchQryReq matchReq;
	memset(&matchReq, 0, sizeof(SmsMatchQryReq));
	//查现货价格
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
	
	//清数据
	m_map_Agree.clear();
	m_map_mAgreeMatch.clear();
	m_map_sAgreeRel.clear();
	m_map_specPrice.clear();

	//序列号
	matchReq.SerialId = 0;
	//公司
	strncpy_s(matchReq.CompanyNo, m_System->m_curLogin.CompanyNo);
	//用户名
	wchar_t	userno[MAX_PATH] = { 0 };
	m_cbUserno.GetText(userno);
	//strncpy_s(matchReq.UserNo, m_System->m_curLogin.UserNo);
	strncpy_s(matchReq.UserNo,LoadRC::unicode_to_ansi(userno).c_str());

	//按日期
	if (IsWindowEnabled(m_labBeginDate.GetHwnd()))
	{
		//开始日期
		wstring beginDate;
		m_cbtBeginDate.CxGetWindowText(beginDate);
		//终止日期
		wstring endDate;
		m_cbtEndDate.CxGetWindowText(endDate);

		int begin = DateStrToInt(LoadRC::unicode_to_ansi(beginDate));
		int end = DateStrToInt(LoadRC::unicode_to_ansi(endDate));

		string key = m_System->get_curkey();
		
		//指定日期全部合同
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
		//找到主要合同
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

		//保存主要合同数据
		for (auto it_main_agree : m_set_mainAgreeno)
		{
			m_map_Agree[it_main_agree] = m_System->m_map_mapAgreement[key][it_main_agree];
		}

		//主要合同对应的所有次要合同
		for (auto it_main_agreeno : m_set_mainAgreeno)
		{
			set<SmsAgreementDataRsp> m_setMinorAgree;
			m_setMinorAgree.clear();
			//一个开仓合同相关联的平仓合同
			FindMinorAgree(it_main_agreeno, m_setMinorAgree);
			//<开仓数据，<平仓数据>>
			m_map_sAgreeRel[m_map_Agree[it_main_agreeno]] = m_setMinorAgree;
		}
		//查询成交
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
				_TMessageBox_Domodal(m_Hwnd, "提示", L"查询成交超时", TMB_OK);
				return;
			}

			if (!m_System->m_mapMatch.empty())
			{
				m_map_mAgreeMatch[matchReq.AgreementNo] = m_System->m_mapMatch;
			}
		}
		//查询现货价格
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
				_TMessageBox_Domodal(m_Hwnd, "提示", L"查询现货价格超时", TMB_OK);
				return;
			}
		}
		m_map_specPrice = m_System->m_mapSpecPrice;
		//生成账单
		//GeneratedBill();
		GeneratedListBill();
		m_listAll.SetData(&m_RowinfoContainer);
		
	}
	else {	//按合同=================================================================================================

		//合同号
		wchar_t	agreeno[MAX_PATH] = { 0 };
		m_cbAgree.GetText(agreeno);
		strncpy_s(matchReq.AgreementNo, LoadRC::unicode_to_ansi(agreeno).c_str());

		//获取合同号
		string key = m_System->get_curkey();
		bool isBuyAgree = false;

		isBuyAgree = m_System->m_map_mapAgreement[key][matchReq.AgreementNo].AgreementType == SMS_AGREEMENT_PURCHASE ? true : false;
		m_map_Agree[matchReq.AgreementNo] = m_System->m_map_mapAgreement[key][matchReq.AgreementNo];

		//找到主要合同
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

		//保存主要合同数据
		m_map_Agree[mainAgreeno] = m_System->m_map_mapAgreement[key][mainAgreeno];

		//获取相关联的次要合同号
		set<SmsAgreementDataRsp> m_setMinorAgree;
		m_setMinorAgree.clear();
		//一个主要合同相关联的次要合同
		FindMinorAgree(mainAgreeno, m_setMinorAgree);
		//<开仓数据，<平仓数据>>
		m_map_sAgreeRel[m_map_Agree[mainAgreeno]] = m_setMinorAgree;

		//查询成交
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
				_TMessageBox_Domodal(m_Hwnd, "提示", L"查询成交超时", TMB_OK);
				return;
			}

			if (!m_System->m_mapMatch.empty())
			{
				m_map_mAgreeMatch[matchReq.AgreementNo] = m_System->m_mapMatch;
			}
		}
		//查询现货价格
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
				_TMessageBox_Domodal(m_Hwnd, "提示", L"查询现货价格超时", TMB_OK);
				return;
			}
		}
		m_map_specPrice = m_System->m_mapSpecPrice;

		//查询期货结算价格--使用最新价或者昨结算价
		//生成账单
		//GeneratedBill();
		GeneratedListBill();
		m_listAll.SetData(&m_RowinfoContainer);
	}
}

void IntegrationWnd::OnBtnSave()
{
	///////////////////////////////测试///////////////////////////////
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
	//TCHAR strFilename[MAX_PATH] = { 0 };															//用于接收文件名
	//ofn.lStructSize = sizeof(OPENFILENAME);															//结构体大小
	//ofn.hwndOwner = m_Hwnd;																			//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
	//ofn.lpstrFilter = TEXT("Excel文件(*.xlsx)\0*.xlsx\0\0");										//设置过滤
	//ofn.nFilterIndex = 1;																			//过滤器索引
	//ofn.lpstrFile = strFilename;																	//接收返回的文件名，注意第一个字符需要为NULL
	//ofn.nMaxFile = sizeof(strFilename);																//缓冲区长度
	//ofn.lpstrInitialDir = NULL;																		//初始目录为默认
	//ofn.lpstrTitle = TEXT("保存到");														//使用系统默认标题留空即可
	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;							//目录必须存在，覆盖文件前发出警告
	//ofn.lpstrDefExt = TEXT("xlsx");//默认追加的扩展名

	//if (GetSaveFileName(&ofn))
	//{
	//	//保存到excel
	//	SaveToExcel(strFilename);
	//}
	//else
	//	return;
}

void IntegrationWnd::GeneratedBill()
{
	//清空
	m_richeditAll.Clear();

	//key
	string key = m_System->get_curkey();
	//成交手续费平均到每笔对冲
	for (auto& it_agree : m_map_mAgreeMatch)
	{
		for (auto & it_match : it_agree.second)
		{
			it_match.second.MatchFee = it_match.second.MatchFee / it_match.second.MatchQty;
		}
	}

	//============================================表头========================================================
	char temp_txt[1024] = { 0 };
	m_richeditAll.AddString(row_title_start);		//1
	sprintf_s(temp_txt, "│%124s%124s│", "期现对账单"," ");
	m_richeditAll.AddString(temp_txt);		//2
	m_richeditAll.AddString(row_title_middle1);		//3
	sprintf_s(temp_txt, "│%-14s│%29s%25s│%35s%31s│%49s%47s│%7s%3s│", "", "采购", "", "销售", "", "期货", "", "", "");
	m_richeditAll.AddString(temp_txt);		//4
	m_richeditAll.AddString(row_title_middle2);		//5
	//sprintf_s(temp_txt, "│%-4s│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%14s│", "", "账号", "日期", "合同编号", "单价", "数量",
	//	"日期","合同编号", "单价", "数量", "不含税损益", "合约编号", "开仓日期", "开仓价格", "平仓日期", "平仓价格", "数量", "手续费", "盈亏", "期现盈亏");
	sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%10s│",  "账号", "日期", "合同编号", "单价", "数量",
		"日期","合同编号", "单价", "数量", "不含税盈亏", "合约编号", "开仓日期", "开仓价格", "平仓日期", "平仓价格", "数量", "手续费", "盈亏", "期现盈亏");
	m_richeditAll.AddString(temp_txt);		//5

	//所有开仓合同盈亏合计
	m_spotAllProfit = 0.0;
	m_allFee = 0.0;
	m_matchAllProfit = 0.0;

	bool b_isFirstMain = true;
	//遍历每个主要合同
	for (auto it_main : m_map_sAgreeRel)
	{
		bool b_isFirst = true;
	
		m_tradeDot = 0.000000001;		//每手乘数
		m_minorAgreeQty = 0;			//次要合同数量总计
		m_minorAgreeTotal = 0.0;		//次要合同价值总计
		m_spotTotalProfit = 0.0;		//损益合计

		m_futureQty = 0;				//已对冲期货数量合计
		m_futureRemainQty = 0;			//未对冲期货数量合计
		m_openMatchTotal = 0.0;			//开仓价格合计
		m_closeMatchTotal = 0.0;		//平仓价格合计
		m_totalFee = 0.0;				//手续费合计
		m_matchTotalProfit = 0.0;		//期货盈亏

		//合同对应-规格--每手乘数
		string scommod = m_System->m_mapSpec[it_main.first.SpecificationNo].CommodityNo;
		SCommodity* commod[1] = { 0 };
		g_pStarApi->GetCommodityData("", scommod.c_str(), commod, 1, false);
		if (commod[0])
			m_tradeDot = commod[0]->TradeDot;
		
		//1.主要合同先自对冲（不人工平仓），2.人工平仓 3.然后遍历次要合同(先处理人工平仓，再自对冲,然后先平主要合同，再开主要合同)
		vector<HedgeInfo>				v_selfHedge;	//自对冲
		v_selfHedge.clear();
		vector<HedgeInfo>				v_manHedge;		//一个目标合同合同所有指定平仓对冲关系
		v_manHedge.clear();

		//1.先自对冲目标合同（不人工平仓）
		{
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//平仓明细没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平此合同开仓成交
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
						//如果目标合同开仓成交数量都为零就可以break
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
			//将自对冲关系写到界面
			GeneratedSelfOffsetItem(it_main.first, v_selfHedge, b_isFirst, b_isFirstMain);
			if (v_selfHedge.size() != 0)
				if (b_isFirst)
					b_isFirst = !b_isFirst;
		}
		//2.遍历次要合同执行人工平仓
		vector<HedgeInfo>				v_temp_manHedge;
		v_temp_manHedge.clear();
		//主要合同
		{
			//遍历指定平仓
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
			//根据目标合同号，关联合同号，消除对应的成交数量
			for (auto it_reduceNum : v_temp_manHedge)
			{
				//找目标合同成交
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
				//找关联合同成交
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
		//次要合同
		for (auto it_minor : it_main.second)
		{
			v_temp_manHedge.clear();
			//遍历指定平仓
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
			//根据目标合同号，关联合同号，消除对应的成交数量
			for (auto it_reduceNum : v_temp_manHedge)
			{
				//找目标合同成交
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
				//找关联合同成交
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

		//3.遍历次要合同（先人工平仓，再自对冲）
		for (auto it_minor : it_main.second)
		{
			//对冲
			vector<HedgeInfo>								v_matchNum;							//对冲数据
			v_matchNum.clear();

			//把指定平仓关系添加到显示界面
			for (auto it_manclose : v_manHedge)
			{
				if (strcmp(it_manclose.OpenAgreementNo, it_main.first.AgreementNo) == 0 && strcmp(it_manclose.CloseAgreementNo, it_minor.AgreementNo) == 0)
					v_matchNum.push_back(it_manclose);
				if (strcmp(it_manclose.CloseAgreementNo, it_main.first.AgreementNo) == 0 && strcmp(it_manclose.OpenAgreementNo, it_minor.AgreementNo) == 0)
					v_matchNum.push_back(it_manclose);

			}

			//再自对冲
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//平仓明细没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平此合同开仓成交
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
						//如果目标合同开仓成交数量都为零就可以break
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


			//遍历次要合同成交(主要合同开仓，次要合同平仓)
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//还有没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平开仓合同成交
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
						//如果开仓合同成交数量都为零就可以break
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

			//遍历次要合同成交(主要合同平仓，次要合同开仓)
			for (auto& it_mopenMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_mopenMatch.second.Offset == SMS_O_OPEN)
				{
					//还有没有平完
					while (it_mopenMatch.second.MatchQty > 0)
					{
						//对冲主要合同平仓成交
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
						//如果开仓合同成交数量都为零就可以break
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

			//判断是否次要合同现货都已经对冲完了
			int sumCover = 0;
			int close_remain = 0;
			for (auto it_match_num : v_matchNum)
			{
				sumCover += it_match_num.CloseQty;
			}
			//平仓合同对应的平仓成交没有下完，只显示合同剩余量，不分配期货开仓成交
			if (sumCover < it_minor.SpotQty / m_tradeDot)
			{
				close_remain = int(it_minor.SpotQty / m_tradeDot - sumCover);
			}

			//一个次要合同生成 几行 对冲关系
			GeneratedItem(it_main.first, it_minor, v_matchNum, b_isFirst,close_remain,b_isFirstMain);
			if (b_isFirst) b_isFirst = !b_isFirst;
		}

		
		//===================一个主要合同对应一个小计 和 一个敞口统计=================================
		//计算是否有敞口
		CalculateSpotOpen(it_main.first, b_isFirst,b_isFirstMain);
		//小计
		int ifutureQty = 0;
		ifutureQty = m_futureQty + m_futureRemainQty;
		m_richeditAll.AddString(row_text_sumtop);
		sprintf_s(temp_txt, "│%-14s  %-10s  %-20s  %10.2f  %8d│%-10s  %-20s  %10.2f  %8d  %10.2f│%-14s  %-10s  %10.2f  %-10s  %10.2f  %8d  %10.2f  %10.2f│%10.2f│",
			 "小计"/*it_main.first.UserNo*/, ""/*it_main.first.SignDate*/, ""/*it_main.first.AgreementNo*/, it_main.first.SpotPrice,
			it_main.first.SpotQty, "", "",  m_minorAgreeQty == 0 ? 0.0 : m_minorAgreeTotal / m_minorAgreeQty, it_main.first.SpotQty, m_spotTotalProfit,
			"","", ifutureQty == 0 ? 0.0 : m_openMatchTotal / ifutureQty, "", ifutureQty == 0 ? 0.0 : m_closeMatchTotal/ ifutureQty, m_futureQty * 2 + m_futureRemainQty, m_totalFee, m_matchTotalProfit, m_spotTotalProfit + m_matchTotalProfit - m_totalFee);
		m_richeditAll.AddString(temp_txt);
		//m_richeditAll.AddString(row_text_start);
		//累加到合计
		m_spotAllProfit += m_spotTotalProfit;
		m_allFee += m_totalFee;
		m_matchAllProfit += m_matchTotalProfit;

		if (b_isFirstMain)
			b_isFirstMain = !b_isFirstMain;

	}
	//所有开仓合同 合计盈亏
	if (!m_map_sAgreeRel.empty())
	{
		sprintf_s(temp_txt, "│%-14s  %-10s  %-20s  %10s  %8s  %-10s  %-20s  %10s  %8s  %10.2f│%-14s  %-10s  %10s  %-10s  %10s  %8s  %10.2f  %10.2f│%10.2f│",
			"总计", "", "", "", "",
			 "", "", "", "", m_spotAllProfit,
			"", "", "", "", "", "", m_allFee, m_matchAllProfit, m_spotAllProfit + m_matchAllProfit - m_allFee);
		m_richeditAll.AddString(row_text_totalsumtop);
		m_richeditAll.AddString(temp_txt);
		m_richeditAll.AddString(row_title_end);
	}
}

void IntegrationWnd::GeneratedListBill()
{
	//清空
	m_RowinfoContainer.clear();

	//key
	string key = m_System->get_curkey();
	//成交手续费平均到每笔对冲
	for (auto& it_agree : m_map_mAgreeMatch)
	{
		for (auto & it_match : it_agree.second)
		{
			it_match.second.MatchFee = it_match.second.MatchFee / it_match.second.MatchQty;
		}
	}

	//所有开仓合同盈亏合计
	m_spotAllProfit = 0.0;
	m_allFee = 0.0;
	m_matchAllProfit = 0.0;

	bool b_isFirstMain = true;
	//遍历每个主要合同
	for (auto it_main : m_map_sAgreeRel)
	{
		bool b_isFirst = true;

		m_tradeDot = 0.000000001;		//每手乘数
		m_minorAgreeQty = 0;			//次要合同数量总计
		m_minorAgreeTotal = 0.0;		//次要合同价值总计
		m_spotTotalProfit = 0.0;		//损益合计

		m_futureQty = 0;				//已对冲期货数量合计
		m_futureRemainQty = 0;			//未对冲期货数量合计
		m_openMatchTotal = 0.0;			//开仓价格合计
		m_closeMatchTotal = 0.0;		//平仓价格合计
		m_totalFee = 0.0;				//手续费合计
		m_matchTotalProfit = 0.0;		//期货盈亏

										//合同对应-规格--每手乘数
		string scommod = m_System->m_mapSpec[it_main.first.SpecificationNo].CommodityNo;
		SCommodity* commod[1] = { 0 };
		g_pStarApi->GetCommodityData("", scommod.c_str(), commod, 1, false);
		if (commod[0])
			m_tradeDot = commod[0]->TradeDot;

		//1.主要合同先自对冲（不人工平仓），2.人工平仓 3.然后遍历次要合同(先处理人工平仓，再自对冲,然后先平主要合同，再开主要合同)
		vector<HedgeInfo>				v_selfHedge;	//自对冲
		v_selfHedge.clear();
		vector<HedgeInfo>				v_manHedge;		//一个目标合同合同所有指定平仓对冲关系
		v_manHedge.clear();

		//1.先自对冲目标合同（不人工平仓）
		{
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_main.first.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//平仓明细没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平此合同开仓成交
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
						//如果目标合同开仓成交数量都为零就可以break
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
			//将自对冲关系存到rowinfo
			GeneratedSelfOffsetListItem(it_main.first, v_selfHedge, b_isFirst, b_isFirstMain);
			if (v_selfHedge.size() != 0)
				if (b_isFirst)
					b_isFirst = !b_isFirst;
		}
		//2.遍历次要合同执行人工平仓
		vector<HedgeInfo>				v_temp_manHedge;
		v_temp_manHedge.clear();
		//主要合同
		{
			//遍历指定平仓
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
			//根据目标合同号，关联合同号，消除对应的成交数量
			for (auto it_reduceNum : v_temp_manHedge)
			{
				//找目标合同成交
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
				//找关联合同成交
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
		//次要合同
		for (auto it_minor : it_main.second)
		{
			v_temp_manHedge.clear();
			//遍历指定平仓
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
			//根据目标合同号，关联合同号，消除对应的成交数量
			for (auto it_reduceNum : v_temp_manHedge)
			{
				//找目标合同成交
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
				//找关联合同成交
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

		//3.遍历次要合同（先人工平仓，再自对冲）
		for (auto it_minor : it_main.second)
		{
			//对冲
			vector<HedgeInfo>								v_matchNum;							//对冲数据
			v_matchNum.clear();

			//把指定平仓关系添加到显示界面
			for (auto it_manclose : v_manHedge)
			{
				if (strcmp(it_manclose.OpenAgreementNo, it_main.first.AgreementNo) == 0 && strcmp(it_manclose.CloseAgreementNo, it_minor.AgreementNo) == 0)
					v_matchNum.push_back(it_manclose);
				if (strcmp(it_manclose.CloseAgreementNo, it_main.first.AgreementNo) == 0 && strcmp(it_manclose.OpenAgreementNo, it_minor.AgreementNo) == 0)
					v_matchNum.push_back(it_manclose);

			}

			//再自对冲
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//平仓明细没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平此合同开仓成交
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
						//如果目标合同开仓成交数量都为零就可以break
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


			//遍历次要合同成交(主要合同开仓，次要合同平仓)
			for (auto& it_closeMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_closeMatch.second.Offset == SMS_O_COVER)
				{
					//还有没有平完
					while (it_closeMatch.second.MatchQty > 0)
					{
						//平开仓合同成交
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
						//如果开仓合同成交数量都为零就可以break
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

			//遍历次要合同成交(主要合同平仓，次要合同开仓)
			for (auto& it_mopenMatch : m_map_mAgreeMatch[it_minor.AgreementNo])
			{
				if (it_mopenMatch.second.Offset == SMS_O_OPEN)
				{
					//还有没有平完
					while (it_mopenMatch.second.MatchQty > 0)
					{
						//对冲主要合同平仓成交
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
						//如果开仓合同成交数量都为零就可以break
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

			//判断是否次要合同现货都已经对冲完了
			int sumCover = 0;
			int close_remain = 0;
			for (auto it_match_num : v_matchNum)
			{
				sumCover += it_match_num.CloseQty;
			}
			//平仓合同对应的平仓成交没有下完，只显示合同剩余量，不分配期货开仓成交
			if (sumCover < it_minor.SpotQty / m_tradeDot)
			{
				close_remain = int(it_minor.SpotQty / m_tradeDot - sumCover);
			}

			//一个次要合同生成 几行 对冲关系
			GeneratedListItem(it_main.first, it_minor, v_matchNum, b_isFirst, close_remain, b_isFirstMain);
			if (b_isFirst) b_isFirst = !b_isFirst;
		}


		//===================一个主要合同对应一个小计 和 一个敞口统计=================================
		//计算是否有敞口
		CalculateSpotOpenList(it_main.first, b_isFirst, b_isFirstMain);
		//小计
		int ifutureQty = 0;
		ifutureQty = m_futureQty + m_futureRemainQty;

		TRowInfo  rowinfo_subtotal;
		//m_richeditAll.AddString(row_text_sumtop);
		//sprintf_s(temp_txt, "│%-14s  %-10s  %-20s  %10.2f  %8d│%-10s  %-20s  %10.2f  %8d  %10.2f│%-14s  %-10s  %10.2f  %-10s  %10.2f  %8d  %10.2f  %10.2f│%10.2f│",
		//	"小计"/*it_main.first.UserNo*/, ""/*it_main.first.SignDate*/, ""/*it_main.first.AgreementNo*/, it_main.first.SpotPrice,
		//	it_main.first.SpotQty, "", "", m_minorAgreeQty == 0 ? 0.0 : m_minorAgreeTotal / m_minorAgreeQty, it_main.first.SpotQty, m_spotTotalProfit,
		//	"", "", ifutureQty == 0 ? 0.0 : m_openMatchTotal / ifutureQty, "", ifutureQty == 0 ? 0.0 : m_closeMatchTotal / ifutureQty, m_futureQty * 2 + m_futureRemainQty, m_totalFee, m_matchTotalProfit, m_spotTotalProfit + m_matchTotalProfit - m_totalFee);
		//m_richeditAll.AddString(temp_txt);
		//m_richeditAll.AddString(row_text_start);

		strncpy_s(rowinfo_subtotal.UserNo, "小计");
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


		//累加到合计
		m_spotAllProfit += m_spotTotalProfit;
		m_allFee += m_totalFee;
		m_matchAllProfit += m_matchTotalProfit;

		if (b_isFirstMain)
			b_isFirstMain = !b_isFirstMain;

	}
	//所有开仓合同 合计盈亏
	if (!m_map_sAgreeRel.empty())
	{
		//sprintf_s(temp_txt, "│%-14s  %-10s  %-20s  %10s  %8s  %-10s  %-20s  %10s  %8s  %10.2f│%-14s  %-10s  %10s  %-10s  %10s  %8s  %10.2f  %10.2f│%10.2f│",
		//	"总计", "", "", "", "",
		//	"", "", "", "", m_spotAllProfit,
		//	"", "", "", "", "", "", m_allFee, m_matchAllProfit, m_spotAllProfit + m_matchAllProfit - m_allFee);
		//m_richeditAll.AddString(row_text_totalsumtop);
		//m_richeditAll.AddString(temp_txt);
		//m_richeditAll.AddString(row_title_end);
		TRowInfo  rowinfo_sumtotal;
		strncpy_s(rowinfo_sumtotal.UserNo, "总计");
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

	//税率
	double tax = 1.16;
	if (!m_System->m_setTax.empty())
	{
		auto it = m_System->m_setTax.end();
		tax = (*(--it)).Tax;
	}

	//主要合同（采购）次要合同（销售）====================================
	if (bMainBuy)
	{	
		//现货利润
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo,  minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					"","", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
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

			//每手乘数
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//计算一笔对冲 手续费 期货利润
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);
			
			double futureprofit = 0.0;
			if(openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else 
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//每笔对冲计算合计
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//行情合约转交易合约
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo,  minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate,closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 "", "", "", "", minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo,  minorAgree.SpotPrice, minorAgree.SpotQty, spotprofit,
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_middle_l);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 "", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate,closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);
			}
		}
	}
	//主要合同（销售）次要合同（采购）
	else
	{
		//现货利润
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (isfirst)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10s│%8s│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
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

			//每手乘数
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//计算一笔对冲 手续费 期货利润
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//每笔对冲计算合计
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//行情合约转交易合约
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			if (i == 0 && isfirst && isfirstmain)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, mainAgree.SpotQty, spotprofit,
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0 && isfirst)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, spotprofit,
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10s│%8s│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					"", "", "", "", spotprofit,
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_middle_r);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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

	//税率
	double tax = 1.16;
	if (!m_System->m_setTax.empty())
	{
		auto it = m_System->m_setTax.end();
		tax = (*(--it)).Tax;
	}

	//主要合同（采购）次要合同（销售）====================================
	if (bMainBuy)
	{
		//现货利润
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
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

			//每手乘数
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//计算一笔对冲 手续费 期货利润
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//每笔对冲计算合计
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//行情合约转交易合约
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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
	//主要合同（销售）次要合同（采购）
	else
	{
		//现货利润
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

			//每手乘数
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//计算一笔对冲 手续费 期货利润
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//每笔对冲计算合计
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//行情合约转交易合约
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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

	//税率
	//double tax = 1.16;
	//if (!m_System->m_setTax.empty())
	//{
	//	auto it = m_System->m_setTax.end();
	//	tax = (*(--it)).Tax;
	//}

	//主要合同（采购）次要合同（销售）====================================
	if (bMainBuy)
	{
		//现货利润
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
				sprintf_s(temp_txt, "│%-4s│%-14s│%-10s│%-20s│%10.2f│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					"", userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
					minorAgree.SignDate, minorAgree.AgreementNo, minorAgree.SpotQty, minorAgree.SpotPrice, spotprofit,
					"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-4s│%-14s│%-10s│%-20s│%10s│%8d│%-10s│%-20s│%10.2f│%8d│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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

			//每手乘数
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//计算一笔对冲 手续费 期货利润
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//每笔对冲计算合计
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//行情合约转交易合约
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, "",
					"", "", "", "", "",
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_middle_l);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 "", "", "", "", "",
					"", "", "", "", "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(temp_txt);
			}
		}
	}
	//主要合同（销售）次要合同（采购）
	else
	{
		//现货利润
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
		//		sprintf_s(temp_txt, "│%-4s│%-14s│%-10s│%-20s│%10.2f│%8d││%-10s│%-20s│%10.2f│%8d│%10.2f││%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
		//			"", userno.c_str(), opendate.c_str(), openagree.c_str(), openprice, minorAgree.SpotQty,
		//			mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotQty, mainAgree.SpotPrice, spotprofit,
		//			"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
		//		m_richeditAll.AddString(row_text_start);
		//		m_richeditAll.AddString(temp_txt);
		//	}
		//	else
		//	{
		//		char temp_txt[1024] = { 0 };
		//		sprintf_s(temp_txt, "│%-4s│%-14s│%-10s│%-20s│%10.2f│%8d││%-10s│%-20s│%10s│%8s│%10.2f││%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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

			//每手乘数
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//计算一笔对冲 手续费 期货利润
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//每笔对冲计算合计
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//行情合约转交易合约
			string scode = openmatch.ContractNo;
			string tcode("");
			CConvert::SContractNoToTContractNo(scode, tcode);

			if (i == 0 && isfirst && isfirstmain)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10.2f│%8d│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 userno.c_str(), "", "", "", "",
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, "",
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0 && isfirst)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10.2f│%8d│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 userno.c_str(), "", "", "", "",
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, "",
					tcode.c_str()/*openmatch.ContractNo*/, openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}
			else if (i == 0)
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10.2f│%8d│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 userno.c_str(), "", "", "", "",
					mainAgree.SignDate, mainAgree.AgreementNo,  mainAgree.SpotPrice, mainAgree.SpotQty, "",
					tcode.c_str(), openmatch.SettleDate, openmatch.MatchPrice, closematch.SettleDate, closematch.MatchPrice, v_hedgeInfo.at(i).CloseQty, fee, futureprofit, "");
				m_richeditAll.AddString(row_text_middle_r);
				m_richeditAll.AddString(temp_txt);
			}
			else
			{
				char temp_txt[1024] = { 0 };
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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

	//主要合同（采购）次要合同（销售）====================================
	if (bMainBuy)
	{
		//现货利润
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

			//每手乘数
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//计算一笔对冲 手续费 期货利润
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//每笔对冲计算合计
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//行情合约转交易合约
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
				//sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
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
	//主要合同（销售）次要合同（采购）
	else
	{
		//现货利润
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

			//每手乘数
			if (m_tradeDot == 0.0)
			{
				SContract* commod[1] = { 0 };
				g_pStarApi->GetContractData("", openmatch.ContractNo, commod, 1, false);
				if (commod[0] && commod[0]->Commodity)
					m_tradeDot = commod[0]->Commodity->TradeDot;
			}

			//计算一笔对冲 手续费 期货利润
			double fee = v_hedgeInfo.at(i).CloseQty*(openmatch.MatchFee + closematch.MatchFee);

			double futureprofit = 0.0;
			if (openmatch.Direct == SMS_DIRECT_SELL)
				futureprofit = (openmatch.MatchPrice - closematch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;
			else
				futureprofit = (closematch.MatchPrice - openmatch.MatchPrice) * v_hedgeInfo.at(i).CloseQty * m_tradeDot;

			//每笔对冲计算合计
			m_futureQty += v_hedgeInfo.at(i).CloseQty;
			m_openMatchTotal += openmatch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_closeMatchTotal += closematch.MatchPrice * v_hedgeInfo.at(i).CloseQty;
			m_totalFee += fee;
			m_matchTotalProfit += futureprofit;

			//行情合约转交易合约
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
	//没有次要合同，但要体现出期货头寸
	if (m_minorAgreeQty == 0)
	{
		int spotOpen = mainAgree.SpotQty - m_minorAgreeQty;
		//遍历开仓合同的成交
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end() && !m_map_mAgreeMatch[mainAgree.AgreementNo].empty())
		{
			bool b_first = true;

			//取现货价格,现货收益
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

			//现货总计
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//行情合约转交易合约
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//期货昨结算价，最新价
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;
				if (contract[0] && contract[0]->Commodity)
					m_tradeDot = contract[0]->Commodity->TradeDot;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty* m_tradeDot;;
				//期货合计
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//数量加*号
				char sSpotOpenQty[24] = { 0 };
				char sMatchQty[24] = { 0 };
				sprintf_s(sSpotOpenQty, "*%d", spotOpen);
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				char temp_txt[1024] = { 0 };
				if (b_first && isfirst && isfirstmain)
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10.2f│%8s│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
						 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
						"", "",  spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_start);
					m_richeditAll.AddString(temp_txt);
				}
				else if (b_first && isfirst)
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10.2f│%8s│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
						 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_sumbottom);
					m_richeditAll.AddString(temp_txt);
				}
				else if (b_first)
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10.2f│%8s│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
						 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_middle_l);
					m_richeditAll.AddString(temp_txt);
				}
				else
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
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
			//也没有成交，只显示现货持仓
			//取现货价格,现货收益
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

			//现货总计
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			//数量加*号
			char sSpotOpenQty[24] = { 0 };
			sprintf_s(sSpotOpenQty, "*%d", spotOpen);

			char temp_txt[1024] = { 0 };
			if (isfirst && isfirstmain)
			{
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10.2f│%8s│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
					 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
					"", "", spotprice, sSpotOpenQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (isfirst)
			{
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10.2f│%8s│%10.2f│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
					 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
					"", "", spotprice, sSpotOpenQty, spotprofit,
					"", "", "", "", "", "", "", "", "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}


		}
		return;
	}
	if (m_minorAgreeQty < mainAgree.SpotQty)	//有敞口
	{
		int spotOpen = mainAgree.SpotQty - m_minorAgreeQty;
		//遍历开仓合同的成交
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end())
		{
			bool b_first = true;

			//取现货价格,现货收益
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

			//现货总计
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//行情合约转交易合约
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//期货昨结算价，最新价
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty * m_tradeDot;
				//期货合计
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//数量加*号
				char sSpotOpenQty[24] = { 0 };
				char sMatchQty[24] = { 0 };
				sprintf_s(sSpotOpenQty, "*%d", spotOpen);
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				char temp_txt[1024] = { 0 };
				if (b_first && isfirst && isfirstmain)
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10.2f│%8s│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
						 "", "", "", "", sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_start);
					m_richeditAll.AddString(temp_txt);
				}
				else if (b_first && isfirst)
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10.2f│%8s│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
						 "", "", "", "", sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_sumbottom);
					m_richeditAll.AddString(temp_txt);
				}
				else if (b_first)
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10.2f│%8s│%10.2f│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
						 "", "", "", "", sSpotOpenQty,
						"", "", spotprice, sSpotOpenQty, spotprofit,
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_middle_l);
					m_richeditAll.AddString(temp_txt);
				}
				else
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
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
			//取现货价格,现货收益
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

			//数量加*号
			char sSpotOpenQty[24] = { 0 };
			sprintf_s(sSpotOpenQty, "*%d", spotOpen);

			char temp_txt[1024] = { 0 };
			if (isfirst && isfirstmain)
			{
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s││%-10s│%-20s│%10.2f│%8s│%10.2f││%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
					"", "", spotprice, sSpotOpenQty, spotprofit,
					"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
				m_richeditAll.AddString(row_text_start);
				m_richeditAll.AddString(temp_txt);
			}
			else if (isfirst)
			{
				sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s││%-10s│%-20s│%10.2f│%8s│%10.2f││%-14s│%-10s│%10.2f│%-10s│%10.2f│%8d│%10.2f│%10.2f│%-10s│",
					 mainAgree.UserNo, mainAgree.SignDate, mainAgree.AgreementNo, mainAgree.SpotPrice, sSpotOpenQty,
					"", "", spotprice, sSpotOpenQty, spotprofit,
					"", "", 0.0, "", 0.0, 0, 0.0, 0.0, "");
				m_richeditAll.AddString(row_text_sumbottom);
				m_richeditAll.AddString(temp_txt);
			}


		}
		return;
	}
	//没敞口但是有期货剩余（主次合同的所有开仓都要考虑）
	if(m_minorAgreeQty >= mainAgree.SpotQty)
	{
		int spotOpen = 0;
		//遍历开仓合同的成交
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end())
		{
			bool b_first = true;

			//取现货价格,现货收益
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

			//现货总计
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//行情合约转交易合约
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//期货昨结算价，最新价
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty * m_tradeDot;
				//期货合计
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//数量加*号
				char sMatchQty[24] = { 0 };
				sprintf_s(sMatchQty, "*%d", it_open_match.second.MatchQty);

				char temp_txt[1024] = { 0 };
				if (b_first)
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
						 "", "", "", "", "",
						"", "",  "", "", "",
						tcode.c_str()/*it_open_match.second.ContractNo*/, it_open_match.second.SettleDate, it_open_match.second.MatchPrice, "", settleprice, sMatchQty, fee, futureprofit, "");
					m_richeditAll.AddString(row_text_middle_l);
					m_richeditAll.AddString(temp_txt);
				}
				else
				{
					sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10s│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10.2f│%-10s│%10.2f│%8s│%10.2f│%10.2f│%-10s│",
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
			//取现货价格,现货收益
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
			sprintf_s(temp_txt, "│%-14s│%-10s│%-20s│%10.2f│%8s│%-10s│%-20s│%10s│%8s│%10s│%-14s│%-10s│%10s│%-10s│%10s│%8s│%10s│%10s│%-10s│",
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
	//没有次要合同，但要体现出期货头寸
	if (m_minorAgreeQty == 0)
	{
		int spotOpen = mainAgree.SpotQty - m_minorAgreeQty;
		//遍历开仓合同的成交
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end() && !m_map_mAgreeMatch[mainAgree.AgreementNo].empty())
		{
			bool b_first = true;

			//取现货价格,现货收益
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

			//现货总计
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//行情合约转交易合约
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//期货昨结算价，最新价
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;
				if (contract[0] && contract[0]->Commodity)
					m_tradeDot = contract[0]->Commodity->TradeDot;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty* m_tradeDot;;
				//期货合计
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//数量加*号
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
			//也没有成交，只显示现货持仓
			//取现货价格,现货收益
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

			//现货总计
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			//数量加*号
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
	if (m_minorAgreeQty < mainAgree.SpotQty)	//有敞口
	{
		int spotOpen = mainAgree.SpotQty - m_minorAgreeQty;
		//遍历开仓合同的成交
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end())
		{
			bool b_first = true;

			//取现货价格,现货收益
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

			//现货总计
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//行情合约转交易合约
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//期货昨结算价，最新价
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty * m_tradeDot;
				//期货合计
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//数量加*号
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
			//取现货价格,现货收益
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

			//数量加*号
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
	//没敞口但是有期货剩余（主次合同的所有开仓都要考虑）
	if (m_minorAgreeQty >= mainAgree.SpotQty)
	{
		int spotOpen = 0;
		//遍历开仓合同的成交
		auto it_open_agree = m_map_mAgreeMatch.find(mainAgree.AgreementNo);
		if (it_open_agree != m_map_mAgreeMatch.end())
		{
			bool b_first = true;

			//取现货价格,现货收益
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

			//现货总计
			m_minorAgreeQty += (spotprice == 0.0 ? 0 : spotOpen);
			m_minorAgreeTotal += spotOpen * spotprice;
			m_spotTotalProfit += spotprofit;

			for (auto it_open_match : m_map_mAgreeMatch[mainAgree.AgreementNo])
			{
				if (it_open_match.second.MatchQty == 0 || it_open_match.second.Offset == SMS_O_COVER)
					continue;

				//行情合约转交易合约
				string scode = it_open_match.second.ContractNo;
				string tcode("");
				CConvert::SContractNoToTContractNo(scode, tcode);

				//期货昨结算价，最新价
				double settleprice = 0.0;
				SContract* contract[1] = { 0 };
				g_pStarApi->GetContractData("", it_open_match.second.ContractNo, contract, 1, false);
				if (contract[0] && contract[0]->SnapShot)
					settleprice = contract[0]->SnapShot->Data[S_FID_PRESETTLEPRICE].Price;

				double fee = it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				double futureprofit = (it_open_match.second.MatchPrice - settleprice)*it_open_match.second.MatchQty * m_tradeDot;
				//期货合计
				m_futureRemainQty += it_open_match.second.MatchQty;
				m_openMatchTotal += it_open_match.second.MatchPrice * it_open_match.second.MatchQty;
				m_closeMatchTotal += settleprice * it_open_match.second.MatchQty;
				m_totalFee += it_open_match.second.MatchQty * it_open_match.second.MatchFee;
				m_matchTotalProfit += futureprofit;

				//数量加*号
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
			//取现货价格,现货收益
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
				//<合同号，合同数据>
				m_map_Agree[it_rel.SellAgreementNo] = m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo];
				//<次要合同数据>
				set_minoragree.insert(m_System->m_map_mapAgreement[key][it_rel.SellAgreementNo]);
			}
		}
		else
		{
			if (strcmp(it_rel.SellAgreementNo, maingreeno.c_str()) == 0)
			{
				//<合同号，合同数据>
				m_map_Agree[it_rel.BuyAgreementNo] = m_System->m_map_mapAgreement[key][it_rel.BuyAgreementNo];
				//<次要合同数据>
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
		else//没找到' '但有内容  类似abc;这是仍然要保存
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
	//方便截取最后一段数据
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

	//如果size为4
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
		else//没找到' '但有内容  类似abc;这是仍然要保存
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
		::MessageBoxA(m_Hwnd, "没有数据可保存！", "错误", 0x10010);
		return false;
	}


	//写入到excel
	//------------------写入excel---------------------------
	// 初始化COM库
	CoInitialize(NULL);

	// 获得EXCEL的CLSID
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);

	if (FAILED(hr)) {
		//::MessageBoxA(m_Hwnd, "CLSIDFromProgID() 函数调用失败！", "错误", 0x10010);
		::MessageBoxA(m_Hwnd, "请检查是否已经安装EXCEL！", "错误", 0x10010);
		return false;
	}

	// 创建实例
	IDispatch *pXlApp;
	hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&pXlApp);
	if (FAILED(hr)) {
		::MessageBoxA(m_Hwnd, "请检查是否已经安装EXCEL！", "错误", 0x10010);
		return false;
	}

	// 显示，将Application.Visible属性置1
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 1;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"Visible", 1, x);
	}
	//取消提示框
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"DisplayAlerts", 1, x);
	}

	// 获取Workbooks集合
	IDispatch *pXlBooks;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"Workbooks", 0);
		pXlBooks = result.pdispVal;
	}

	// 调用Workbooks.Add()方法，创建一个新的Workbook
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
		tmp.bstrVal = SysAllocString(L"账号");
		long indices1[] = { h_index,1 };
		SafeArrayPutElement(arr.parray, indices1, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"日期");
		long indices2[] = { h_index,2 };
		SafeArrayPutElement(arr.parray, indices2, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"合同编号");
		long indices3[] = { h_index,3 };
		SafeArrayPutElement(arr.parray, indices3, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"单价");
		long indices4[] = { h_index,4 };
		SafeArrayPutElement(arr.parray, indices4, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"数量");
		long indices5[] = { h_index,5 };
		SafeArrayPutElement(arr.parray, indices5, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"日期");
		long indices6[] = { h_index,6 };
		SafeArrayPutElement(arr.parray, indices6, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"合同编号");
		long indices7[] = { h_index,7 };
		SafeArrayPutElement(arr.parray, indices7, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"单价");
		long indices8[] = { h_index,8 };
		SafeArrayPutElement(arr.parray, indices8, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"数量");
		long indices9[] = { h_index,9 };
		SafeArrayPutElement(arr.parray, indices9, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"不含税盈亏");
		long indices10[] = { h_index,10 };
		SafeArrayPutElement(arr.parray, indices10, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"合约编号");
		long indices11[] = { h_index,11 };
		SafeArrayPutElement(arr.parray, indices11, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"开仓日期");
		long indices12[] = { h_index,12 };
		SafeArrayPutElement(arr.parray, indices12, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"开仓价格");
		long indices13[] = { h_index,13 };
		SafeArrayPutElement(arr.parray, indices13, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"平仓日期");
		long indices14[] = { h_index,14 };
		SafeArrayPutElement(arr.parray, indices14, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"平仓价格");
		long indices15[] = { h_index,15 };
		SafeArrayPutElement(arr.parray, indices15, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"数量");
		long indices16[] = { h_index,16 };
		SafeArrayPutElement(arr.parray, indices16, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"手续费");
		long indices17[] = { h_index,17 };
		SafeArrayPutElement(arr.parray, indices17, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"盈亏");
		long indices18[] = { h_index,18 };
		SafeArrayPutElement(arr.parray, indices18, (void *)&tmp);
		tmp.bstrVal = SysAllocString(L"期现盈亏");
		long indices19[] = { h_index,19 };
		SafeArrayPutElement(arr.parray, indices19, (void *)&tmp);
	}

	bool b_dblSplit = false;
	int index_tablecell = 3;
	map<int, int>  map_tablecell;
	map_tablecell.clear();

	//for (int i = 5; i < v_txt.size(); i++) {
	//	vector<string> v_temp = SplitString(v_txt.at(i), "│");

	//	if (v_temp.size() == 1 || v_temp.size() == 5)
	//		continue;
	//	h_index++;

	//	for (auto& it : v_temp)
	//	{
	//		TrimString(it);
	//	}

	//	if (v_temp.size() == 19)	//正常
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
	//	else if (v_temp.size() == 3)	//合计
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
	//	else if (v_temp.size() == 4)	//小计
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

	//		//记录合并单元格
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
		//写入字符串
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

		//记录合并单元格行数
		if (strcmp(m_RowinfoContainer.GetData(i).UserNo, "小计") == 0)
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


	//根据行数生成range
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

	// 选择一个size*19大小的Range
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
	//设置列宽
	{
		VARIANT parm;
		parm.vt = VT_I4;
		parm.lVal = 11.75;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"ColumnWidth", 1, parm);
	}
	// Set range with our safearray...
	AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, arr);

	//采购合并单元格
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
	//销售合并单元格
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
	//期货合并单元格
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
	//合并单元格
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
	//自适应列宽
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

	//冻结窗格
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


	// 接下来我们该保存文件了，利用Worksheet.SaveAs()方法(我这里忽略了其他所有参数，除了文件名)
	//{
	//	VARIANT filename;
	//	filename.vt = VT_BSTR;
	//	filename.bstrVal = SysAllocString(L"R:\\test.xlsx");
	//	AutoWrap(DISPATCH_METHOD, NULL, pXlSheet, L"SaveAs", 1, filename);
	//}

	//恢复提示框
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




