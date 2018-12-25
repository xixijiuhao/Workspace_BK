#include "PreInclude.h"

extern vector<SystemFrame*>	g_FrameVec;
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern ISpotSystemApi		*g_pSpotSystem;				//套保平台
extern string				g_specNoPrice;

SystemFrame::SystemFrame()
	: m_main_bar_height(38)//30
	, m_main_bar_width(80)
	, m_sub_bar_height(35)//25
	, m_sub_bar_width(80)
	, m_frame_weight(0)
	, m_sub_text_left(8)
	, m_dlg_gap(8)
	, m_bMouseTrack(false)

	, m_current_main_index(0)
	, m_current_sub_index(-1)

	, m_main_bar_back_color(0x585858)//BFBFBF)//0x373333)//D4B99E//858F1F
	, m_main_bar_text_color(0xFFFFFF)//000000)//0xEEEEEE)//969696
	, m_main_bar_back_sel_color(0xF3F3F3)//B97130)//0xCC7A00)//
	, m_main_bar_text_sel_color(0x606060)//FFFFFF)//0xEEEEEE)//606060

	, m_sub_bar_back_color(0x9B9B9B)//E7E6E6)//0x463F3F)//0x686868//E7E6E6//E6D8D5
	, m_sub_bar_text_color(0xFFFFFF)//000000)//0xEEEEEE)//0xEEEEEE
	, m_sub_bar_back_sel_color(0xF3F3F3)//FFFFFF)//0xF0F0F0)//FFFFFF
	, m_sub_bar_text_sel_color(0x000000)//000000)//0x303030)//0xDCDC00

	, m_main_bar_font({ -14, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET, 0,0,0,0, L"Microsoft YaHei UI" })
	, m_sub_bar_font({ -13, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET, 0,0,0,0, L"Microsoft YaHei UI" })
{
	memset(&m_curLogin, 0, sizeof(SmsLoginRsp));

	m_sendOrderWnd = new SendOrderWnd;

	m_agreeWnd = new AgreementWnd;
	m_specWnd = new SpecWnd;
	m_specSpriceWnd = new SpecPriceWnd;
	m_taxWnd = new TaxWnd;

	m_orderWnd = new OrderWnd;
	m_matchWnd = new MatchWnd;
	m_instantPositionWnd = new InstantPositionWnd;
	m_positionWnd = new PositionWnd;
	m_fundWnd = new FundWnd;
	m_cashInoutWnd = new CashInOutWnd;
	m_settleSpriceWnd = new SettlePriceWnd;

	m_integrationWnd = new IntegrationWnd;
	m_partAccountWnd = new PartAccountWnd;
	m_allAccountWnd = new AllAccountWnd;
	m_exposureWnd = new ExposureWnd;

	init();
	if (g_pSpotSystem)
	{
		g_pSpotSystem->Regist(this);
	}
	m_sendOrderWnd->Register(this);

	m_agreeWnd->Register(this);
	m_specSpriceWnd->Register(this);
	m_specWnd->Register(this);
	m_taxWnd->Register(this);

	m_orderWnd->Register(this);
	m_matchWnd->Register(this);
	m_instantPositionWnd->Register(this);

	m_positionWnd->Register(this);
	m_fundWnd->Register(this);
	m_cashInoutWnd->Register(this);
	m_settleSpriceWnd->Register(this);

	m_integrationWnd->Register(this);
	m_partAccountWnd->Register(this);
	m_allAccountWnd->Register(this);
	m_exposureWnd->Register(this);
}

SystemFrame::~SystemFrame()
{
	m_frame_weight = 2;
	if (g_pSpotSystem)
	{
		g_pSpotSystem->UnRegist(this);
	}
	if (m_agreeWnd)	delete m_agreeWnd;
	if (m_sendOrderWnd)	delete m_sendOrderWnd;
	if (m_specSpriceWnd)	delete m_specSpriceWnd;
	if (m_settleSpriceWnd)	delete m_settleSpriceWnd;
	if (m_cashInoutWnd)	delete m_cashInoutWnd;
	if (m_instantPositionWnd)	delete m_instantPositionWnd;
	if (m_positionWnd)	delete m_positionWnd;
	if (m_orderWnd)	delete m_orderWnd;
	if (m_matchWnd)	delete m_matchWnd;
	if (m_fundWnd)	delete m_fundWnd;
	if (m_specWnd)	delete m_specWnd;
	if (m_taxWnd)	delete m_taxWnd;
	if (m_integrationWnd)	delete m_integrationWnd;
	if (m_partAccountWnd)	delete m_partAccountWnd;
	if (m_allAccountWnd)	delete m_allAccountWnd;
	if (m_exposureWnd)	delete m_exposureWnd;
}

void SystemFrame::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void SystemFrame::SetWindow()
{
	m_sendOrderWnd->Create(L"SendOrderWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[0].second[0].second.sub_hwnd = m_sendOrderWnd->GetHwnd();

	m_agreeWnd->Create(L"AgreementWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[1].second[0].second.sub_hwnd = m_agreeWnd->GetHwnd();

	m_specSpriceWnd->Create(L"SpecPriceWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[1].second[1].second.sub_hwnd = m_specSpriceWnd->GetHwnd();

	m_specWnd->Create(L"SpecWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[1].second[2].second.sub_hwnd = m_specWnd->GetHwnd();

	m_taxWnd->Create(L"TaxWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[1].second[3].second.sub_hwnd = m_taxWnd->GetHwnd();

	m_orderWnd->Create(L"OrderWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[2].second[0].second.sub_hwnd = m_orderWnd->GetHwnd();

	m_matchWnd->Create(L"MatchWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[2].second[1].second.sub_hwnd = m_matchWnd->GetHwnd();

	m_instantPositionWnd->Create(L"InstantPositionWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[2].second[2].second.sub_hwnd = m_instantPositionWnd->GetHwnd();

	m_positionWnd->Create(L"PositionWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[2].second[3].second.sub_hwnd = m_positionWnd->GetHwnd();

	m_fundWnd->Create(L"FundWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[2].second[4].second.sub_hwnd = m_fundWnd->GetHwnd();

	m_cashInoutWnd->Create(L"CashInOutWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[2].second[5].second.sub_hwnd = m_cashInoutWnd->GetHwnd();

	m_settleSpriceWnd->Create(L"SettlePriceWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[2].second[6].second.sub_hwnd = m_settleSpriceWnd->GetHwnd();

	m_integrationWnd->Create(L"IntegrationWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[3].second[0].second.sub_hwnd = m_integrationWnd->GetHwnd();

	m_exposureWnd->Create(L"ExposureWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[3].second[1].second.sub_hwnd = m_exposureWnd->GetHwnd();

	m_partAccountWnd->Create(L"PartAccountWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[3].second[2].second.sub_hwnd = m_partAccountWnd->GetHwnd();

	m_allAccountWnd->Create(L"AllAccountWnd", m_Hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	m_main_bars[3].second[3].second.sub_hwnd = m_allAccountWnd->GetHwnd();
}

LRESULT SystemFrame::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		break;
	case WM_PAINT:
		OnPaint();
		break;
	case WM_SIZE:
		OnSize();
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(LOWORD(lParam), HIWORD(lParam));
		break;
	case SSWM_COMBOX_SELECT:
		OnComboxSelect(wParam, lParam);
		break;
	case WM_NCDESTROY:				OnNcDestory();//加锁
		break;
	//case WM_DESTROY:				OnDestroy();
	//	break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}

void SystemFrame::OnCreate()
{
	m_loginWnd.Create(m_Hwnd);
	m_loginWnd.SetAutoHide(true);
}

void SystemFrame::OnPaint()
{
	TMemDC mdc(m_Hwnd);
	int oldDc = SaveDC(mdc.GetHdc());
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(mdc.GetHdc(), &rect, g_ColorRefData.GetHbrBackground());
	SetBkMode(mdc.GetHdc(), TRANSPARENT);
	draw_main_tab(mdc.GetHdc());
	draw_sub_tab(mdc.GetHdc());
	draw_login_tab(mdc.GetHdc());
	RestoreDC(mdc.GetHdc(), oldDc);
}

void SystemFrame::OnSize()
{
	RECT rect;

	if (IsWindow(m_current_dlg))
	{
		get_dlg_rect(rect);
		if (m_main_bars[m_current_main_index].second.size() == 1)
			rect.left -= m_sub_bar_width;
		::MoveWindow(m_current_dlg, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	}

	m_rcLogin.left = m_main_bars.size() * m_main_bar_width;
	m_rcLogin.right = m_rcLogin.left + m_main_bar_width;
	m_rcLogin.top = 0;
	m_rcLogin.bottom = m_rcLogin.top + m_main_bar_height ;

	m_rcUserNo.left = m_rcLogin.right;
	m_rcUserNo.right = m_rcUserNo.left + m_main_bar_width + 20;
	m_rcUserNo.top = 0;
	m_rcUserNo.bottom = m_rcUserNo.top + m_main_bar_height;

	::GetClientRect(m_Hwnd, &rect);
	::PostMessage(m_Hwnd, WM_PAINT, 0, MAKELPARAM(rect.right - rect.left, rect.bottom - rect.top));
}

void SystemFrame::InitMenuItem()
{
}

void SystemFrame::OnLButtonDown(const int x, const int y)
{
	POINT pt = { x,y };

	int	ind = -1;
	if (pt_in_main_bar(pt, ind))
		switch_index(ind, m_sub_indexs[ind]);
	else if (pt_in_sub_bar(pt, ind))
		switch_index(m_current_main_index, ind);
	//else if (pt_in_login(pt))
	//{
	//	m_loginWnd.ShowWindow(SW_SHOW);
	//	RECT rt;
	//	GetWindowRect(m_Hwnd, &rt);
	//	m_loginWnd.MoveWindow(rt.left+m_rcLogin.left, rt.top +m_rcLogin.bottom, 130, 0);
	//}
}

void SystemFrame::OnLButtonUp(const int x, const int y)
{
}

void SystemFrame::UpdateSwitchWindow()
{
}

void SystemFrame::OnNcDestory()
{
	vector<SystemFrame*>::iterator iter = find(g_FrameVec.begin(), g_FrameVec.end(), this);
	if (iter != g_FrameVec.end())
	{
		g_FrameVec.erase(iter);
		delete this;
	}
}

void SystemFrame::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMouseTrack = true;
	}
	InvalidateRect(m_Hwnd, 0, false);


	POINT pointM;
	pointM.x = LOWORD(lParam);
	pointM.y = HIWORD(lParam);
	//账号
	if (PtInRect(&m_rcLogin, pointM))
	{
		if (!m_bInLoginRect)
			InvalidateRect(m_Hwnd, &m_rcLogin, TRUE);
		m_bInLoginRect = true;
	}
	else
	{
		if (m_bInLoginRect)
			InvalidateRect(m_Hwnd, &m_rcLogin, TRUE);
		m_bInLoginRect = false;
	}
}

void SystemFrame::OnMouseLeave()
{
	m_bMouseTrack = false;
	m_bInLoginRect = false;
}

void SystemFrame::OnComboxSelect(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)wParam;
	int pos = (int)lParam;
	m_curLogin = m_vecLogin.at(pos);

}

void SystemFrame::init()
{
	for (size_t i = 0; i <= shEnd; i++)
	{
		wstring main_title;
		sub_vector subs;
		switch (i)
		{
		case 0:
		{
			main_title = L"下单";
			subs.clear();
			TSub sub0 = { NULL, 0 };
			subs.push_back(sub_vector::value_type(L"", sub0));
			m_main_bars.push_back(make_pair(main_title, subs));
			break;
		}
		case 1:
		{
			main_title = L"现货";
			subs.clear();
			TSub sub0 = { NULL, 0 };
			subs.push_back(sub_vector::value_type(L"合同管理", sub0));
			TSub sub1 = { NULL, 1 };
			subs.push_back(sub_vector::value_type(L"现货价格", sub1));
			TSub sub2 = { NULL, 2 };
			subs.push_back(sub_vector::value_type(L"规格管理", sub2));
			TSub sub3 = { NULL, 3 };
			subs.push_back(sub_vector::value_type(L"税率管理", sub3));
			m_main_bars.push_back(make_pair(main_title, subs));
			break;
		}
		case 2: 
		{
			main_title = L"期货";
			subs.clear();
			TSub sub0 = { NULL, 0 };
			subs.push_back(sub_vector::value_type(L"委托数据", sub0));
			TSub sub1 = { NULL, 1 };
			subs.push_back(sub_vector::value_type(L"成交数据", sub1));
			TSub sub2 = { NULL, 2 };
			subs.push_back(sub_vector::value_type(L"实时持仓", sub2));
			TSub sub3 = { NULL, 3 };
			subs.push_back(sub_vector::value_type(L"历史持仓", sub3));
			TSub sub4 = { NULL, 4 };
			subs.push_back(sub_vector::value_type(L"历史资金", sub4));
			TSub sub5 = { NULL, 5 };
			subs.push_back(sub_vector::value_type(L"出入金额", sub5));
			TSub sub6 = { NULL, 6 };
			subs.push_back(sub_vector::value_type(L"结算价格", sub6));
			m_main_bars.push_back(make_pair(main_title, subs));
			break;
		}
		case 3: 
		{	
			main_title = L"综合";
			subs.clear();
			TSub sub0 = { NULL, 0 };
			subs.push_back(sub_vector::value_type(L"期现账单", sub0));
			TSub sub1 = { NULL, 1 };
			subs.push_back(sub_vector::value_type(L"敞口统计", sub1));
			TSub sub2 = { NULL, 2 };
			subs.push_back(sub_vector::value_type(L"期货分账", sub2));
			TSub sub3 = { NULL, 3 };
			subs.push_back(sub_vector::value_type(L"期货总账", sub3));
			m_main_bars.push_back(make_pair(main_title, subs));
			break;
		}
		//case 4:
		//{
		//	main_title = L"设置";
		//	subs.clear();
		//	TSub sub0 = { NULL, 0 };
		//	subs.push_back(sub_vector::value_type(L"规格", sub0));
		//	TSub sub1 = { NULL, 1 };
		//	subs.push_back(sub_vector::value_type(L"现货价格", sub1));
		//	TSub sub2 = { NULL, 2 };
		//	subs.push_back(sub_vector::value_type(L"税率", sub2));

		//	m_main_bars.push_back(make_pair(main_title, subs));
		//	break;
		//}
		default: 
			continue;
		}
		m_sub_indexs.push_back(0);
	}
}

void SystemFrame::get_main_bar_rect(RECT & rect)
{
	GetClientRect(m_Hwnd, &rect);
	::InflateRect(&rect, -m_frame_weight, -m_frame_weight);

	rect.bottom = rect.top + m_main_bar_height;
}

void SystemFrame::get_sub_bar_rect(RECT & rect)
{
	GetClientRect(m_Hwnd, &rect);
	::InflateRect(&rect, -m_frame_weight, -m_frame_weight);

	rect.top += m_main_bar_height;
	rect.right = rect.left + m_sub_bar_width;
}

void SystemFrame::get_dlg_rect(RECT & rect)
{
	GetClientRect(m_Hwnd, &rect);
	::InflateRect(&rect, -m_frame_weight, -m_frame_weight);

	rect.left += m_sub_bar_width;
	rect.top += m_main_bar_height;
	//rect.bottom -= m_dlg_gap;

	//::InflateRect(&rect, -m_dlg_gap, -m_dlg_gap);
}

void SystemFrame::draw_main_tab(HDC dc)
{
	RECT _rect, rect;
	get_main_bar_rect(_rect);
	rect = _rect;
	rect.right = rect.left + m_main_bar_width;

	HBRUSH brush = ::CreateSolidBrush(m_main_bar_back_color);
	HBRUSH brush_sel = ::CreateSolidBrush(m_main_bar_back_sel_color);
	HFONT font = ::CreateFontIndirect(&m_main_bar_font);
	HGDIOBJ old_font = ::SelectObject(dc, font);

	::SetBkMode(dc, TRANSPARENT);
	for (size_t i = 0; i < m_main_bars.size(); i++)
	{
		if (i == m_current_main_index)
		{
			::FillRect(dc, &rect, brush_sel);
			::SetTextColor(dc, m_main_bar_text_sel_color);
		}
		else
		{
			::FillRect(dc, &rect, brush);
			::SetTextColor(dc, m_main_bar_text_color);
		}
		::DrawTextW(dc, m_main_bars[i].first.c_str(), m_main_bars[i].first.size(), &rect, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		::OffsetRect(&rect, m_main_bar_width, 0);
	}
	rect.right = _rect.right;
	::FillRect(dc, &rect, brush);

	::SelectObject(dc, old_font);
	::DeleteObject(font);
	::DeleteObject(brush);
	::DeleteObject(brush_sel);
}

void SystemFrame::draw_sub_tab(HDC dc)
{
	RECT _rect, rect, text_rect;
	get_sub_bar_rect(_rect);
	rect = _rect;
	rect.bottom = rect.top + m_sub_bar_height;

	HBRUSH brush = ::CreateSolidBrush(m_sub_bar_back_color);
	HBRUSH brush_sel = ::CreateSolidBrush(m_sub_bar_back_sel_color);
	HFONT font = ::CreateFontIndirect(&m_sub_bar_font);
	HGDIOBJ old_font = ::SelectObject(dc, font);

	::SetBkMode(dc, TRANSPARENT);
	sub_vector & subs = m_main_bars[m_current_main_index].second;
	for (size_t i = 0; i < subs.size(); i++)
	{
		if (i == m_current_sub_index)
		{
			::FillRect(dc, &rect, brush_sel);
			::SetTextColor(dc, m_sub_bar_text_sel_color);
		}
		else
		{
			::FillRect(dc, &rect, brush);
			::SetTextColor(dc, m_sub_bar_text_color);
		}
		text_rect = rect;
		text_rect.left += m_sub_text_left;
		::DrawTextW(dc, subs[i].first.c_str(), subs[i].first.size(), &text_rect, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);

		::OffsetRect(&rect, 0, m_sub_bar_height);
	}
	rect.bottom = _rect.bottom;
	::FillRect(dc, &rect, brush);

	::SelectObject(dc, old_font);
	::DeleteObject(font);
	::DeleteObject(brush);
	::DeleteObject(brush_sel);
}

void SystemFrame::draw_login_tab(HDC dc)
{
	SetTextColor(dc, RGB(255, 255, 255));
	HFONT font = ::CreateFontIndirect(&m_main_bar_font);
	//if (m_bInLoginRect)
	//{
	//	SetTextColor(dc, RGB(0, 220, 220));
	//}
	SelectObject(dc, font);
	//DrawText(dc, L"账号▽", -1, &m_rcLogin, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	if (strlen(m_curLogin.CompanyNo) != 0 && strlen(m_curLogin.UserNo) != 0)
	{
		SelectObject(dc, g_FontData.GetFontWordEx13());
		SetTextColor(dc, RGB(255, 255, 255));
		DrawTextA(dc, m_curLogin.UserNo, -1, &m_rcUserNo, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	::DeleteObject(font);
}

bool SystemFrame::pt_in_main_bar(const POINT & pt, int & ind)
{
	RECT rect;
	get_main_bar_rect(rect);
	if (::PtInRect(&rect, pt))
	{
		int pos = pt.x / m_main_bar_width;
		for (ind = 0; ind < (int)m_main_bars.size(); ind++)
		{
			pos--;
			if (pos < 0)
				return true;
		}
	}
	ind = -1;
	return false;
}

bool SystemFrame::pt_in_sub_bar(const POINT & pt, int & ind)
{
	RECT rect;
	get_sub_bar_rect(rect);
	if (::PtInRect(&rect, pt))
	{
		ind = (pt.y - rect.top) / m_sub_bar_height;
		sub_vector & subs = m_main_bars[m_current_main_index].second;
		if (ind < (int)subs.size())
			return true;
	}
	ind = -1;
	return false;
}

bool SystemFrame::pt_in_login(const POINT & pt)
{
	return ::PtInRect(&m_rcLogin, pt);
}

string SystemFrame::get_key(const SmsCompanyNoType companyno, const SmsUserNoType userno)
{
	string str;
	str = companyno;
	str.append("^");
	str.append(userno);
	return str;
}

string SystemFrame::get_curkey()
{
	return get_key(m_curLogin.CompanyNo, m_curLogin.UserNo);
}

void SystemFrame::Init_User(const SmsCompanyNoType companyno, const SmsUserNoType userno)
{
	string key = get_key(companyno, userno);
	UserMapType::iterator it = m_mapUser.find(key);
	if (it != m_mapUser.end())
	{
		m_mapUser[key].clear();
	}

	int count = g_pSpotSystem->GetUserCount(companyno, userno);
	SmsUserDataRsp** out;
	out = new SmsUserDataRsp*[count];
	g_pSpotSystem->GetUser(companyno, userno, out);
 
	UserVecType vecuser;
	vecuser.clear();
	for (int i = 0; i < count; i++)
	{
		if (!out[i])
			continue;
		SmsUserDataRsp temp;
		memset(&temp, 0, sizeof(SmsUserDataRsp));
		memcpy(&temp, out[i], sizeof(SmsUserDataRsp));
		vecuser.push_back(temp);
	}
	m_mapUser[key] = vecuser;

	int may = 5;

	for (auto it : m_mapUser[key])
	{
		if (it.UserType == SMS_USER_TRADER)
		{
			m_agreeWnd->m_cbUserno.AddString(LoadRC::ansi_to_unicode(it.UserNo).c_str());
			m_integrationWnd->m_cbUserno.AddString(LoadRC::ansi_to_unicode(it.UserNo).c_str());
		}
	}
}

void SystemFrame::Init_Agreement(const SmsCompanyNoType companyno, const SmsUserNoType userno)
{	
	m_agreeWnd->m_listAgree.DeleteAllItems();

	string key = get_key(companyno, userno);
	AgreementMapMapType::iterator it = m_map_mapAgreement.find(key);
	if (it != m_map_mapAgreement.end())
	{
		m_map_mapAgreement[key].clear();
		//m_agreeWnd->m_listCloseAgree.DeleteAllItems();

	}

	int count = g_pSpotSystem->GetAgreementCount(companyno, userno);
	SmsAgreementDataRsp** out;
	out = new SmsAgreementDataRsp*[count];
	g_pSpotSystem->GetAgreement(companyno, userno, out);

	AgreementMapType mapagree;
	mapagree.clear();
	for (int i = 0; i < count; i++)
	{
		if (!out[i])
			continue;
		SmsAgreementDataRsp temp;
		memset(&temp, 0, sizeof(SmsAgreementDataRsp));
		memcpy(&temp, out[i], sizeof(SmsAgreementDataRsp));
		mapagree[temp.AgreementNo] = temp;
	}
	m_map_mapAgreement[key] = mapagree;

	m_sendOrderWnd->m_cbAgree.Clear();
	m_sendOrderWnd->m_cbAgree.AddString(L"");

	m_instantPositionWnd->m_cbAgreementNo.Clear();
	m_instantPositionWnd->m_cbAgreementNo.AddString(L"");

	m_agreeWnd->m_cbAgree.Clear();
	m_agreeWnd->m_cbAgree.AddString(L"指定日期");
	m_agreeWnd->m_cbAgree.AddString(L"全部合同");
	m_agreeWnd->m_cbAgree.AddString(L"未关联合同");
	m_agreeWnd->m_cbAgree.SetSelect(0);

	m_integrationWnd->m_cbAgree.Clear();
	m_integrationWnd->m_cbAgree.AddString(L"");

	m_matchWnd->m_cbAgreementNo.Clear();
	m_matchWnd->m_cbAgreementNo.AddString(L"");

	for (auto it : m_map_mapAgreement[key])
	{
		m_agreeWnd->m_cbAgree.AddString(LoadRC::ansi_to_unicode(it.second.AgreementNo).c_str());

		m_integrationWnd->m_cbAgree.AddString(LoadRC::ansi_to_unicode(it.second.AgreementNo).c_str());

		m_matchWnd->m_cbAgreementNo.AddString(LoadRC::ansi_to_unicode(it.second.AgreementNo).c_str());

		string atype = { 0 };
		if (it.second.AgreementType == SMS_AGREEMENT_PURCHASE)
			atype = "采购";
		else
			atype = "销售";

		char	info[MAX_PATH] = { 0 };
		sprintf_s(info, "%-13s %-4s %-8s %-10d", it.second.AgreementNo, atype.c_str(), it.second.SpecificationNo, it.second.SpotQty/*, it.SpotPrice, ptype.c_str()*/);
		m_sendOrderWnd->m_cbAgree.AddString(LoadRC::ansi_to_unicode(info).c_str());

		sprintf_s(info, "%-20s%-5s%-10s%-6d%-10.2f", it.second.AgreementNo, atype.c_str(), it.second.SpecificationNo, it.second.SpotQty, it.second.SpotPrice);
		//m_exposureWnd->m_cbOpenAgreementNo.AddString(LoadRC::ansi_to_unicode(info).c_str());
		//if (it.PositionOffset == SMS_O_OPEN)
		m_instantPositionWnd->m_cbAgreementNo.AddString(LoadRC::ansi_to_unicode(info).c_str());
	}

	//释放
	delete[]out;
}

void SystemFrame::Init_AgreementRel(const SmsCompanyNoType companyno, const SmsUserNoType userno)
{
	string key = get_key(companyno, userno);
	AgreementRelMapType::iterator it = m_mapAgreementRel.find(key);
	if (it != m_mapAgreementRel.end())
	{
		m_mapAgreementRel[key].clear();
		//m_agreeWnd->m_listOpenAgree.DeleteAllItems();
		//m_agreeWnd->m_listCloseAgree.DeleteAllItems();
	}
	int count = g_pSpotSystem->GetAgreementRelCount(companyno, userno);
	SmsAgreementRelDataRsp** out;
	out = new SmsAgreementRelDataRsp*[count];
	g_pSpotSystem->GetAgreementRel(companyno, userno, out);

	AgreementRelVecType vecagreeRel;
	vecagreeRel.clear();
	for (int i = 0; i < count; i++)
	{
		if (!out[i])
			continue;
		SmsAgreementRelDataRsp temp;
		memset(&temp, 0, sizeof(SmsAgreementRelDataRsp));
		memcpy(&temp, out[i], sizeof(SmsAgreementRelDataRsp));
		vecagreeRel.push_back(temp);
	}
	m_mapAgreementRel[key] = vecagreeRel;
	//释放
	delete[]out;
}

void SystemFrame::Init_ManualClose(const SmsCompanyNoType companyno, const SmsUserNoType userno)
{
	string key = get_key(companyno, userno);
	ManualCloseMapType::iterator it = m_mapManualClose.find(key);
	if (it != m_mapManualClose.end())
	{
		m_mapManualClose[key].clear();
		///指定平仓列表清空？
		//m_agreeWnd->m_listOpenAgree.DeleteAllItems();
		//m_agreeWnd->m_listCloseAgree.DeleteAllItems();
	}
	int count = g_pSpotSystem->GetManualCloseCount(companyno, userno);
	SmsManualCloseDataRsp** out;
	out = new SmsManualCloseDataRsp*[count];
	g_pSpotSystem->GetManualClose(companyno, userno, out);

	ManualCloseVecType vecmanualClose;
	vecmanualClose.clear();
	for (int i = 0; i < count; i++)
	{
		if (!out[i])
			continue;
		SmsManualCloseDataRsp temp;
		memset(&temp, 0, sizeof(SmsManualCloseDataRsp));
		memcpy(&temp, out[i], sizeof(SmsManualCloseDataRsp));
		vecmanualClose.push_back(temp);
	}
	m_mapManualClose[key] = vecmanualClose;
	//释放
	delete[]out;

}

void SystemFrame::Init_Spec(const SmsCompanyNoType companyno, const SmsUserNoType userno)
{
	int count = g_pSpotSystem->GetSpecCount(companyno, userno);
	SmsSpecDataRsp** out;
	out = new SmsSpecDataRsp*[count];
	g_pSpotSystem->GetSpec(companyno, userno, out);

	m_specWnd->m_listSpec.DeleteAllItems();

	for (int i = 0; i < count; i++)
	{
		if (!out[i])
			continue;
		m_mapSpec[out[i]->SpecificationNo] = *(out[i]);
		m_specWnd->AddSpecToList(*out[i]);
	}
	//释放
	delete[]out;

	//添加到combox中
	m_specSpriceWnd->m_cbSpecificationNo.Clear();
	m_specSpriceWnd->m_cbSpecificationNo.AddString(L"");
	m_specSpriceWnd->m_cbSpecNo.Clear();
	m_specSpriceWnd->m_cbSpecNo.AddString(L"");
	for (auto it : m_mapSpec)
	{
		m_specSpriceWnd->m_cbSpecificationNo.AddString(LoadRC::ansi_to_unicode(it.second.SpecificationNo).c_str());
		m_specSpriceWnd->m_cbSpecNo.AddString(LoadRC::ansi_to_unicode(it.second.SpecificationNo).c_str());
	}
}

void SystemFrame::Init_Tax(const SmsCompanyNoType companyno, const SmsUserNoType userno)
{
	int count = g_pSpotSystem->GetTaxCount(companyno, userno);
	SmsTaxDataRsp** out;
	out = new SmsTaxDataRsp*[count];
	g_pSpotSystem->GetTax(companyno, userno, out);

	m_taxWnd->m_listTax.DeleteAllItems();
	m_setTax.clear();

	for (int i = 0; i < count; i++)
	{
		if (!out[i])
			continue;
		m_setTax.insert( *(out[i]));
		m_taxWnd->AddTaxToList(*out[i]);
	}
	//释放
	delete[]out;
}

void SystemFrame::OnInitComplete(const SmsLoginRsp login)
{
	if (!g_pSpotSystem)
		return;

	m_vecLogin.push_back(login);
	m_curLogin = login;
	InvalidateRect(m_Hwnd, 0, false);

	m_loginWnd.SetData(m_vecLogin);

	m_agreeWnd->m_cbUserno.Clear();
	m_integrationWnd->m_cbUserno.Clear();
	//m_integrationWnd->m_cbUserno.AddString(L"");

	for (auto it : m_vecLogin)
	{
		//if(it.UserType != SMS_USER_MANAGER)
		m_agreeWnd->m_cbUserno.AddString(LoadRC::ansi_to_unicode(it.UserNo).c_str());
		m_integrationWnd->m_cbUserno.AddString(LoadRC::ansi_to_unicode(it.UserNo).c_str());
	}
	m_agreeWnd->m_cbUserno.SetSelect(0);
	m_integrationWnd->m_cbUserno.SetSelect(0);

	//初始化下属交易员
	if (login.UserType == SMS_USER_MANAGER)
		Init_User(login.CompanyNo, login.UserNo);
	//初始化合同
	Init_Agreement(login.CompanyNo, login.UserNo);
	//合同关系
	Init_AgreementRel(login.CompanyNo, login.UserNo);
	//人工平仓
	Init_ManualClose(login.CompanyNo, login.UserNo);
	//规格
	Init_Spec(login.CompanyNo, login.UserNo);
	//税率
	Init_Tax(login.CompanyNo, login.UserNo);

	PostMessage(m_agreeWnd->GetHwnd(), WM_AGREEEMENT_REFRESH, 0, 0);
}

void SystemFrame::OnTradeDisconnect(const SmsLoginRsp login)
{
	for (vector<SmsLoginRsp>::iterator it = m_vecLogin.begin(); it != m_vecLogin.end(); it++)
	{
		if (/*strcmp(it->CompanyNo, login.CompanyNo) == 0 &&*/ strcmp(it->UserNo, login.UserNo) == 0)
		{
			m_vecLogin.erase(it);
			break;
		}
	}
	//清理下属交易员
	m_mapUser.clear();
	//清理合同
	m_map_mapAgreement.clear();
	//清理合同关系
	m_mapAgreementRel.clear();
	//清理人工平仓
	m_mapManualClose.clear();
}

void SystemFrame::OnAddAgreement(SmsAgreementAddRsp * rsp)
{
	if (!rsp)return;
	string key = get_key(rsp->CompanyNo, rsp->UserNo);
	AgreementMapMapType::iterator it = m_map_mapAgreement.find(key);
	if (it != m_map_mapAgreement.end())
	{
		m_map_mapAgreement[key][rsp->AgreementNo] = *rsp;
	}

	m_agreeWnd->AddAgreementToList(*rsp,0);
	m_agreeWnd->m_listAgree.SelectItem(0);
	//m_agreeWnd->m_listAgree.DeleteAllItems();
	//for (auto it : m_mapAgreement[key])
	//{
		//m_agreeWnd->AddAgreementToList(it);
	//}

	string atype = { 0 };
	if (rsp->AgreementType == SMS_AGREEMENT_PURCHASE)
		atype = "采购";
	else
		atype = "销售";

	char	info[MAX_PATH] = { 0 };
	sprintf_s(info, "%-13s %-4s %-8s %-10d", rsp->AgreementNo, atype.c_str(), rsp->SpecificationNo, rsp->SpotQty/*, it.SpotPrice, ptype.c_str()*/);
	
	m_sendOrderWnd->m_cbAgree.AddString(LoadRC::ansi_to_unicode(info).c_str());
	m_integrationWnd->m_cbAgree.AddString(LoadRC::ansi_to_unicode(rsp->AgreementNo).c_str());
}

void SystemFrame::OnDeleteAgreement(SmsAgreementDelRsp * rsp)
{
	if (!rsp)
		return;
	//删除容器
	string key = get_curkey();
	m_map_mapAgreement[key].erase(rsp->AgreementNo);
	//for (AgreementMapMapType::iterator m_it = m_map_mapAgreement.begin(); m_it != m_map_mapAgreement.end(); m_it++)
	//{
	//	bool found = false;
	//	for (AgreementMapType::iterator v_it = m_it->second.begin(); v_it != m_it->second.end(); v_it++)
	//	{
	//		if (strcmp(v_it->second.AgreementNo, rsp->AgreementNo) == 0)
	//		{
	//			m_it->second.erase(v_it);
	//			found = true;
	//			break;
	//		}
	//	}
	//	if (found)	break;
	//}

	//删除表格
	int sum = m_agreeWnd->m_listAgree.GetItemCount();
	for (int i = 0; i < sum; i++)
	{
		SmsAgreementNoType temp = { 0 };
		m_agreeWnd->m_listAgree.GetSubItem(i, AGREE_AgreementNo, temp, sizeof(SmsAgreementNoType));
		if (strcmp(temp, rsp->AgreementNo) == 0)
		{
			m_agreeWnd->m_listAgree.DeleteItem(i);
			break;
		}
	}
	//sum = m_agreeWnd->m_listCloseAgree.GetItemCount();
	//for (int i = 0; i < sum; i++)
	//{
	//	SmsAgreementNoType temp = { 0 };
	//	m_agreeWnd->m_listCloseAgree.GetSubItem(i, AGREE_AgreementNo, temp, sizeof(SmsAgreementNoType));
	//	if (strcmp(temp, rsp->AgreementNo) == 0)
	//	{
	//		m_agreeWnd->m_listCloseAgree.DeleteItem(i);
	//		break;
	//	}
	//}
}

void SystemFrame::OnModifyAgreement(SmsAgreementModRsp * rsp)
{
	if (!rsp)return;

	string key = get_curkey();
	m_map_mapAgreement[key][rsp->AgreementNo] = *rsp;
	//for (AgreementMapType::iterator it = m_map_mapAgreement[key].begin(); it != m_map_mapAgreement[key].end(); it++)
	//{
	//	if (strcmp(it->AgreementNo, rsp->AgreementNo) == 0)
	//	{
	//		memcpy(&(*it), rsp, sizeof(SmsAgreementModRsp));
	//		break;
	//	}
	//}

	int count = m_agreeWnd->m_listAgree.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char agreeno[32] = { 0 };
		m_agreeWnd->m_listAgree.GetSubItem(i, AGREE_AgreementNo, agreeno, sizeof(agreeno));
		if (strcmp(agreeno, rsp->AgreementNo) == 0)
		{
			m_agreeWnd->m_listAgree.DeleteItem(i);
			m_agreeWnd->AddAgreementToList(*rsp, 0);
			m_agreeWnd->m_listAgree.SelectItem(0);
			break;
		}
	}

	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改合同成功！", TMB_OK);
}

void SystemFrame::OnAddAgreementRel(SmsAgreementRelAddRsp * rsp)
{
	if (!rsp)return;

	string key = get_curkey();
	AgreementRelMapType::iterator it = m_mapAgreementRel.find(key);
	if (it != m_mapAgreementRel.end())
	{
		m_mapAgreementRel[key].push_back(*rsp);
	}

	if (m_agreeWnd->m_addAgreementRelWnd.m_bisBuy)
	{
		m_agreeWnd->m_addAgreementRelWnd.AddAgreementToListAdd(rsp->SellAgreementNo);	

		m_agreeWnd->m_addAgreementRelWnd.m_remain -= m_map_mapAgreement[key][rsp->SellAgreementNo].SpotQty;
		m_agreeWnd->m_addAgreementRelWnd.RefreshTips2();
		
		m_map_mapAgreement[key][rsp->BuyAgreementNo].SpotRemainQty = m_agreeWnd->m_addAgreementRelWnd.m_remain;

		//for (auto it : m_mapAgreement[key])
		//{
		//	if (strcmp(it.AgreementNo, rsp->SellAgreementNo) == 0)
		//	{
		//		m_agreeWnd->m_addAgreementRelWnd.m_remain -= it.SpotQty;
		//		m_agreeWnd->m_addAgreementRelWnd.RefreshTips2();
		//		break;
		//	}
		//}
	}
	else
	{
		m_agreeWnd->m_addAgreementRelWnd.AddAgreementToListAdd(rsp->BuyAgreementNo);

		m_agreeWnd->m_addAgreementRelWnd.m_remain -= m_map_mapAgreement[key][rsp->BuyAgreementNo].SpotQty;
		m_agreeWnd->m_addAgreementRelWnd.RefreshTips2();

		m_map_mapAgreement[key][rsp->SellAgreementNo].SpotRemainQty = m_agreeWnd->m_addAgreementRelWnd.m_remain < 0 ? 0 : m_agreeWnd->m_addAgreementRelWnd.m_remain;
		//for (auto it : m_mapAgreement[key])
		//{
		//	if (strcmp(it.AgreementNo, rsp->BuyAgreementNo) == 0)
		//	{
		//		m_agreeWnd->m_addAgreementRelWnd.m_remain -= it.SpotQty;
		//		m_agreeWnd->m_addAgreementRelWnd.RefreshTips2();
		//		break;
		//	}
		//}
	}

	_TMessageBox_Domodal(m_agreeWnd->m_addAgreementRelWnd.GetHwnd(), "提示", L"增加合同关系成功！", TMB_OK);
}

void SystemFrame::OnDeleteAgreementRel(SmsAgreementRelDelRsp * rsp)
{
	if (!rsp)return;

	string key = get_curkey();
	AgreementRelMapType::iterator it = m_mapAgreementRel.find(key);
	if (it != m_mapAgreementRel.end())
	{
		AgreementRelVecType::iterator it_rel = m_mapAgreementRel[key].begin();
		for (; it_rel != m_mapAgreementRel[key].end(); ++it_rel)
		{
			if (strcmp(it_rel->BuyAgreementNo,rsp->BuyAgreementNo)==0 && strcmp(it_rel->SellAgreementNo, rsp->SellAgreementNo) == 0)
			{
				m_mapAgreementRel[key].erase(it_rel);
				break;
			}		
		}
	}
	if (m_agreeWnd->m_addAgreementRelWnd.m_bisBuy)
	{
		int count = m_agreeWnd->m_addAgreementRelWnd.m_listAgreeAdd.GetItemCount();
		for (int i = 0; i < count; i++)
		{
			char	agreeno[51] = { 0 };
			m_agreeWnd->m_addAgreementRelWnd.m_listAgreeAdd.GetSubItem(i, 0, agreeno, sizeof(agreeno));
			if (strcmp(agreeno, rsp->SellAgreementNo) == 0)
			{
				m_agreeWnd->m_addAgreementRelWnd.m_listAgreeAdd.DeleteItem(i);
				break;
			}
		}

		m_agreeWnd->m_addAgreementRelWnd.m_remain += m_map_mapAgreement[key][rsp->SellAgreementNo].SpotQty;
		m_agreeWnd->m_addAgreementRelWnd.RefreshTips2();

		m_map_mapAgreement[key][rsp->BuyAgreementNo].SpotRemainQty = m_agreeWnd->m_addAgreementRelWnd.m_remain;
		//for (auto it : m_mapAgreement[key])
		//{
		//	if (strcmp(it.AgreementNo, rsp->SellAgreementNo) == 0)
		//	{
		//		m_agreeWnd->m_addAgreementRelWnd.m_remain += it.SpotQty;
		//		m_agreeWnd->m_addAgreementRelWnd.RefreshTips2();
		//		break;
		//	}
		//}
	}
	else {
		int count = m_agreeWnd->m_addAgreementRelWnd.m_listAgreeAdd.GetItemCount();
		for (int i = 0; i < count; i++)
		{
			char	agreeno[51] = { 0 };
			m_agreeWnd->m_addAgreementRelWnd.m_listAgreeAdd.GetSubItem(i, 0, agreeno, sizeof(agreeno));
			if (strcmp(agreeno, rsp->BuyAgreementNo) == 0)
			{
				m_agreeWnd->m_addAgreementRelWnd.m_listAgreeAdd.DeleteItem(i);
				break;
			}
		}

		m_agreeWnd->m_addAgreementRelWnd.m_remain += m_map_mapAgreement[key][rsp->BuyAgreementNo].SpotQty;
		m_agreeWnd->m_addAgreementRelWnd.RefreshTips2();

		m_map_mapAgreement[key][rsp->SellAgreementNo].SpotRemainQty = m_agreeWnd->m_addAgreementRelWnd.m_remain;
		//for (auto it : m_mapAgreement[key])
		//{
		//	if (strcmp(it.AgreementNo, rsp->BuyAgreementNo) == 0)
		//	{
		//		m_agreeWnd->m_addAgreementRelWnd.m_remain += it.SpotQty;
		//		m_agreeWnd->m_addAgreementRelWnd.RefreshTips2();
		//		break;
		//	}
		//}
	}
	_TMessageBox_Domodal(m_agreeWnd->m_addAgreementRelWnd.GetHwnd(), "提示", L"删除合同关系成功！", TMB_OK);
}

void SystemFrame::OnModifyAgreementRel(SmsAgreementRelModRsp * rsp)
{
}

void SystemFrame::OnAddSpec(SmsSpecAddRsp * rsp)
{
	if (!rsp)return;

	m_mapSpec[rsp->SpecificationNo] = *rsp;

	m_specWnd->AddSpecToList(*rsp);
	//m_specWnd->m_listSpec.DeleteAllItems();
	//for (auto it : m_mapSpec)
	//{
	//	m_specWnd->AddSpecToList(it.second);
	//}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"增加规格成功！", TMB_OK);
}

void SystemFrame::OnDeleteSpec(SmsSpecDelRsp * rsp)
{
	if (!rsp)return;

	m_mapSpec.erase(rsp->SpecificationNo);

	int count = m_specWnd->m_listSpec.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char specNo[32] = { 0 };
		m_specWnd->m_listSpec.GetSubItem(i, 1, specNo, sizeof(specNo));
		if (strcmp(specNo, rsp->SpecificationNo) == 0)
		{
			m_specWnd->m_listSpec.DeleteItem(i);
			break;
		}
	}
	//m_specWnd->m_listSpec.DeleteAllItems();
	//for (auto it : m_mapSpec)
	//{
	//	m_specWnd->AddSpecToList(it.second);
	//}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除规格成功！", TMB_OK);
}

void SystemFrame::OnModifySpec(SmsSpecModRsp * rsp)
{
	if (!rsp)return;

	m_mapSpec[rsp->SpecificationNo] = *rsp;
	
	int count = m_specWnd->m_listSpec.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char specNo[32] = { 0 };
		m_specWnd->m_listSpec.GetSubItem(i, 1, specNo, sizeof(specNo));
		if (strcmp(specNo, rsp->SpecificationNo) == 0)
		{
			m_specWnd->m_listSpec.DeleteItem(i);
			m_specWnd->AddSpecToList(*rsp);
			break;
		}
	}
	//m_specWnd->m_listSpec.DeleteAllItems();
	//for (auto it : m_mapSpec)
	//{
	//	m_specWnd->AddSpecToList(it.second);
	//}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改规格成功！", TMB_OK);
}

void SystemFrame::OnAddSpecPrice(SmsSpecPriceAddRsp * rsp)
{
	if (!rsp)return;

	_TMessageBox_Domodal(m_Hwnd, "提示", L"增加现货价格成功！", TMB_OK);

	m_specSpriceWnd->AddSpecPriceToList(*rsp);
}

void SystemFrame::OnDeleteSpecPrice(SmsSpecPriceDelRsp * rsp)
{
	if (!rsp)return;
	for (SpecPriceSetType::iterator it = m_mapSpecPrice[rsp->SpecificationNo].begin(); it != m_mapSpecPrice[rsp->SpecificationNo].end(); it++)
	{
		if (strcmp(it->CompanyNo, rsp->CompanyNo) == 0 && strcmp(it->SettleDate, rsp->SettleDate) == 0 && strcmp(it->SpecificationNo, rsp->SpecificationNo) == 0)
		{
			m_mapSpecPrice[rsp->SpecificationNo].erase(it);
			break;
		}
	}

	//for (vector<SmsSpecPriceDataRsp>::iterator it = m_vecSpecPrice.begin(); it != m_vecSpecPrice.end(); ++it)
	//{
	//	if (strcmp(it->CompanyNo, rsp->CompanyNo) == 0 && strcmp(it->SettleDate, rsp->SettleDate) == 0 && strcmp(it->SpecificationNo, rsp->SpecificationNo) == 0)
	//	{
	//		m_vecSpecPrice.erase(it);
	//		break;
	//	}
	//}

	int count = m_specSpriceWnd->m_listSpecPrice.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char date[21] = { 0 };
		char specNo[32] = { 0 };
		m_specSpriceWnd->m_listSpecPrice.GetSubItem(i, 1, date, sizeof(date));
		m_specSpriceWnd->m_listSpecPrice.GetSubItem(i, 2, specNo, sizeof(specNo));
		if (strcmp(specNo, rsp->SpecificationNo) == 0 && strcmp(date, rsp->SettleDate) == 0)
		{
			m_specSpriceWnd->m_listSpecPrice.DeleteItem(i);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除现货价格成功！", TMB_OK);
}

void SystemFrame::OnModifySpecPrice(SmsSpecPriceModRsp * rsp)
{
	if (!rsp)return;

	for (SpecPriceSetType::iterator it = m_mapSpecPrice[rsp->SpecificationNo].begin(); it != m_mapSpecPrice[rsp->SpecificationNo].end(); it++)
	{
		if (strcmp(it->CompanyNo, rsp->CompanyNo) == 0 && strcmp(it->SettleDate, rsp->SettleDate) == 0 && strcmp(it->SpecificationNo, rsp->SpecificationNo) == 0)
		{
			m_mapSpecPrice[rsp->SpecificationNo].erase(it);
			break;
		}
	}
	m_mapSpecPrice[rsp->SpecificationNo].insert(*rsp);
	//for (auto &it : m_vecSpecPrice)
	//{
	//	if (strcmp(it.CompanyNo, rsp->CompanyNo) == 0 && strcmp(it.SettleDate, rsp->SettleDate) == 0 && strcmp(it.SpecificationNo, rsp->SpecificationNo) == 0)
	//	{
	//		it.SerialId = rsp->SerialId;
	//		it.SpecificationPirce = rsp->SpecificationPirce;
	//		strncpy_s(it.OperatorNo, rsp->OperatorNo);
	//		break;
	//	}
	//}

	int count = m_specSpriceWnd->m_listSpecPrice.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char date[21] = { 0 };
		char specNo[32] = { 0 };
		m_specSpriceWnd->m_listSpecPrice.GetSubItem(i, 1, date, sizeof(date));
		m_specSpriceWnd->m_listSpecPrice.GetSubItem(i, 2, specNo, sizeof(specNo));
		if (strcmp(specNo, rsp->SpecificationNo) == 0 && strcmp(date, rsp->SettleDate) == 0)
		{
			m_specSpriceWnd->m_listSpecPrice.DeleteItem(i);
			m_specSpriceWnd->AddSpecPriceToList(*rsp);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改现货价格成功！", TMB_OK);
}

void SystemFrame::OnQrySpecPrice(SmsSpecPriceQryRsp * rsp)
{
	if (!rsp)
		return;
	SetEvent(m_integrationWnd->m_hQrySpecPrice);
	SetEvent(m_instantPositionWnd->m_hQrySpecPrice);

	g_specNoPrice = rsp->SpecificationNo;
	PostMessage(m_specSpriceWnd->GetHwnd(), WM_SPECPRICE_REFRESH, 0, 0);

	//for (auto it : m_mapSpecPrice[rsp->SpecificationNo])
	//{
	//	m_specSpriceWnd->AddSpecPriceToList(it);
	//}

	//for (auto it : m_vecSpecPrice)
	//{
	//	m_specSpriceWnd->AddSpecPriceToList(it);
	//}
	//m_vecSpecPrice.clear();
}

void SystemFrame::OnQrySpecPriceData(SmsSpecPriceDataRsp * rsp)
{
	if (!rsp)
		return;
	auto it = m_mapSpecPrice.find(rsp->SpecificationNo);
	if (it == m_mapSpecPrice.end())
	{
		set<SmsSpecPriceDataRsp> set_specprice;
		set_specprice.clear();
		set_specprice.insert(*rsp);
		m_mapSpecPrice[rsp->SpecificationNo] = set_specprice;
	}
	else
	{
		m_mapSpecPrice[rsp->SpecificationNo].insert(*rsp);
	}
	//m_vecSpecPrice.push_back(*rsp);
}

void SystemFrame::OnAddSettlePrice(SmsSettlePriceAddRsp * rsp)
{
	if (!rsp)return;

	_TMessageBox_Domodal(m_Hwnd, "提示", L"增加结算价成功！", TMB_OK);

}

void SystemFrame::OnDeleteSettlePrice(SmsSettlePriceDelRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsSettlePriceDataRsp>::iterator it = m_vecSettlePrice.begin(); it != m_vecSettlePrice.end(); ++it)
	{
		if (strcmp(it->SettleDate, rsp->SettleDate) == 0 && strcmp(it->ContractNo, rsp->ContractNo) == 0)
		{
			m_vecSettlePrice.erase(it);
			break;
		}
	}

	int count = m_settleSpriceWnd->m_listSettlePrice.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char date[21] = { 0 };
		char con[32] = { 0 };
		m_settleSpriceWnd->m_listSettlePrice.GetSubItem(i, 1, date, sizeof(date));
		m_settleSpriceWnd->m_listSettlePrice.GetSubItem(i, 2, con, sizeof(con));

		string sCode = rsp->ContractNo;
		string tCode;
		CConvert::SContractNoToTContractNo(sCode, tCode);
		if (strcmp(date, rsp->SettleDate) == 0 && strcmp(con, tCode.c_str()) == 0)
		{
			m_settleSpriceWnd->m_listSettlePrice.DeleteItem(i);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除结算价成功！", TMB_OK);
}

void SystemFrame::OnModifySettlePrice(SmsSettlePriceModRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsSettlePriceDataRsp>::iterator it = m_vecSettlePrice.begin(); it != m_vecSettlePrice.end(); ++it)
	{
		if (strcmp(it->SettleDate, rsp->SettleDate) == 0 && strcmp(it->ContractNo, rsp->ContractNo) == 0)
		{
			it->SerialId = rsp->SerialId;
			it->SettlePrice = rsp->SettlePrice;
			break;
		}
	}

	int count = m_settleSpriceWnd->m_listSettlePrice.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char date[21] = { 0 };
		char con[32] = { 0 };
		m_settleSpriceWnd->m_listSettlePrice.GetSubItem(i, 1, date, sizeof(date));
		m_settleSpriceWnd->m_listSettlePrice.GetSubItem(i, 2, con, sizeof(con));

		string sCode = rsp->ContractNo;
		string tCode;
		CConvert::SContractNoToTContractNo(sCode, tCode);
		if (strcmp(date, rsp->SettleDate) == 0 && strcmp(con, tCode.c_str()) == 0)
		{
			m_settleSpriceWnd->m_listSettlePrice.DeleteItem(i);
			m_settleSpriceWnd->AddSettlePriceToList(*rsp);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改结算价成功！", TMB_OK);
}

void SystemFrame::OnQrySettlePriceData(SmsSettlePriceDataRsp * rsp)
{
	if (!rsp)
		return;
	m_vecSettlePrice.push_back(*rsp);
}

void SystemFrame::OnQrySettlePrice(SmsSettlePriceQryRsp * rsp)
{
	if (!rsp)
		return;
	for (auto it : m_vecSettlePrice)
	{
		m_settleSpriceWnd->AddSettlePriceToList(it);
	}
	//m_vecSettlePrice.clear();
}

void SystemFrame::OnAddManualClose(SmsManualCloseAddRsp * rsp)
{
	if (!rsp)return;

	string key = get_curkey();
	ManualCloseMapType::iterator it = m_mapManualClose.find(key);
	if (it != m_mapManualClose.end())
	{
		m_mapManualClose[key].push_back(*rsp);
	}

	m_matchWnd->m_addManualCloseWnd.AddToAddList(*rsp);

	_TMessageBox_Domodal(m_Hwnd, "提示", L"增加指定平仓成功！", TMB_OK);
}

void SystemFrame::OnDeleteManualClose(SmsManualCloseDelRsp * rsp)
{
	if (!rsp)return;

	string key = get_curkey();
	ManualCloseMapType::iterator it = m_mapManualClose.find(key);
	if (it != m_mapManualClose.end())
	{
		ManualCloseVecType::iterator it_rel = m_mapManualClose[key].begin();
		for (; it_rel != m_mapManualClose[key].end(); ++it_rel)
		{
			if (strcmp(it_rel->CloseAgreementNo, rsp->CloseAgreementNo) == 0 && strcmp(it_rel->UserNo, rsp->UserNo) == 0 && strcmp(it_rel->CloseNo, rsp->CloseNo) == 0 && strcmp(it_rel->OpenNo, rsp->OpenNo) == 0)
			{
				m_mapManualClose[key].erase(it_rel);
				break;
			}
		}
	}

	int count = m_matchWnd->m_addManualCloseWnd.m_listAddMatch.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char	openMatchNo[51] = { 0 };
		m_matchWnd->m_addManualCloseWnd.m_listAddMatch.GetSubItem(i, 0, openMatchNo, sizeof(openMatchNo));
		if (strcmp(openMatchNo, rsp->OpenNo) == 0)
		{
			m_matchWnd->m_addManualCloseWnd.m_listAddMatch.DeleteItem(i);
			break;
		}
	}

	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除指定平仓成功！", TMB_OK);
}



void SystemFrame::OnAddCashInout(SmsCashInoutAddRsp * rsp)
{
	if (!rsp)return;

	_TMessageBox_Domodal(m_Hwnd, "提示", L"增加出入金成功！", TMB_OK);

}

void SystemFrame::OnDeleteCashInout(SmsCashInoutDelRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsCashInoutDataRsp>::iterator it = m_vecCashInout.begin(); it != m_vecCashInout.end(); ++it)
	{
		if (it->SerialId == rsp->SerialId)
		{
			m_vecCashInout.erase(it);
			break;
		}
	}

	int count = m_cashInoutWnd->m_listCashInOut.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char serial[21] = { 0 };
		m_cashInoutWnd->m_listCashInOut.GetSubItem(i, 0, serial, sizeof(serial));
		int serialId = atoi(serial);
		if (serialId == rsp->SerialId)
		{
			m_cashInoutWnd->m_listCashInOut.DeleteItem(i);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除出入金成功！", TMB_OK);
}

void SystemFrame::OnModifyCashInout(SmsCashInoutModRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsCashInoutDataRsp>::iterator it = m_vecCashInout.begin(); it != m_vecCashInout.end(); ++it)
	{
		if (it->SerialId == rsp->SerialId)
		{
			memcpy(&(*it), rsp, sizeof(SmsCashInoutDataRsp));
			break;
		}
	}

	int count = m_cashInoutWnd->m_listCashInOut.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char serial[21] = { 0 };
		m_cashInoutWnd->m_listCashInOut.GetSubItem(i, 0, serial, sizeof(serial));
		int serialId = atoi(serial);
		if (serialId == rsp->SerialId)
		{
			m_cashInoutWnd->m_listCashInOut.DeleteItem(i);
			m_cashInoutWnd->AddCashInOutToList(*rsp);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改出入金成功！", TMB_OK);
}

void SystemFrame::OnQryCashInoutData(SmsCashInoutDataRsp * rsp)
{
	if (!rsp)
		return;
	m_vecCashInout.push_back(*rsp);
}

void SystemFrame::OnQryCashInout(SmsCashInoutQryRsp * rsp)
{
	if (!rsp)
		return;
	for (auto it : m_vecCashInout)
	{
		m_cashInoutWnd->AddCashInOutToList(it);
	}
	//m_vecCashInout.clear();
}

void SystemFrame::OnAddFund(SmsFundAddRsp * rsp)
{
	if (!rsp)return;

	m_vecFund.push_back(*rsp);

	m_fundWnd->AddFundToList(*rsp, 0);

	_TMessageBox_Domodal(m_Hwnd, "提示", L"增加资金成功！", TMB_OK);
}

void SystemFrame::OnDeleteFund(SmsFundDelRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsFundDataRsp>::iterator it = m_vecFund.begin(); it != m_vecFund.end(); ++it)
	{
		if (strcmp(it->SettleDate, rsp->SettleDate) == 0 && strcmp(it->CompanyNo, rsp->CompanyNo) == 0 && strcmp(it->UserNo, rsp->UserNo) == 0)
		{
			m_vecFund.erase(it);
			break;
		}
	}

	int count = m_fundWnd->m_listFund.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char settleDate[21] = { 0 };
		char companyNo[21] = { 0 };
		char userNo[21] = { 0 };
		m_fundWnd->m_listFund.GetSubItem(i, 1, settleDate, sizeof(settleDate));
		m_fundWnd->m_listFund.GetSubItem(i, 2, companyNo, sizeof(companyNo));
		m_fundWnd->m_listFund.GetSubItem(i, 3, userNo, sizeof(userNo));
		if (strcmp(settleDate, rsp->SettleDate) == 0 && strcmp(companyNo, rsp->CompanyNo) == 0 && strcmp(userNo, rsp->UserNo) == 0)
		{
			m_fundWnd->m_listFund.DeleteItem(i);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改资金成功！", TMB_OK);
}

void SystemFrame::OnModifyFund(SmsFundModRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsFundDataRsp>::iterator it = m_vecFund.begin(); it != m_vecFund.end(); ++it)
	{
		if (strcmp(it->SettleDate,  rsp->SettleDate)==0 && strcmp(it->CompanyNo, rsp->CompanyNo) == 0&& strcmp(it->UserNo, rsp->UserNo) == 0)
		{
			memcpy(&(*it), rsp, sizeof(SmsFundDataRsp));
			break;
		}
	}

	int count = m_fundWnd->m_listFund.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char serial[21] = { 0 };
		m_fundWnd->m_listFund.GetSubItem(i, 0, serial, sizeof(serial));
		int serialId = atoi(serial);
		if (serialId == rsp->SerialId)
		{
			m_fundWnd->m_listFund.DeleteItem(i);
			m_fundWnd->AddFundToList(*rsp,0);
			m_fundWnd->m_listFund.SelectItem(0);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改资金成功！", TMB_OK);
}

void SystemFrame::OnQryFundData(SmsFundDataRsp * rsp)
{
	if (!rsp)
		return;
	m_vecFund.push_back(*rsp);
}

void SystemFrame::OnQryFund(SmsFundQryRsp * rsp)
{
	if (!rsp)
		return;

	SetEvent(m_partAccountWnd->m_hQryFund);
	SetEvent(m_allAccountWnd->m_hQryFund);
	PostMessage(m_fundWnd->GetHwnd(), WM_FUND_REFRESH, 0, 0);
	//for (auto it : m_vecFund)
	//{
	//	m_fundWnd->AddFundToList(it);
	//}

	//m_vecFund.clear();
	

}

void SystemFrame::OnAddPosition(SmsPositionAddRsp * rsp)
{
	if (!rsp)return;

	m_vecPosition.push_back(*rsp);

	m_positionWnd->AddPositionToList(*rsp, 0);

	//_TMessageBox_Domodal(m_Hwnd, "提示", L"增加持仓成功！", TMB_OK);
}

void SystemFrame::OnDeletePosition(SmsPositionDelRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsPositionDataRsp>::iterator it = m_vecPosition.begin(); it != m_vecPosition.end(); ++it)
	{
		if (it->SerialId == rsp->SerialId)
		{
			m_vecPosition.erase(it);
			break;
		}
	}

	int count = m_positionWnd->m_listPosition.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char serial[21] = { 0 };
		m_positionWnd->m_listPosition.GetSubItem(i, 0, serial, sizeof(serial));
		int serialId = atoi(serial);
		if (serialId == rsp->SerialId)
		{
			m_positionWnd->m_listPosition.DeleteItem(i);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除持仓成功！", TMB_OK);
}

void SystemFrame::OnModifyPosition(SmsPositionModRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsPositionDataRsp>::iterator it = m_vecPosition.begin(); it != m_vecPosition.end(); ++it)
	{
		if (it->SerialId == rsp->SerialId)
		{
			memcpy(&(*it), rsp, sizeof(SmsPositionDataRsp));
			break;
		}
	}

	int count = m_positionWnd->m_listPosition.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char serial[21] = { 0 };
		m_positionWnd->m_listPosition.GetSubItem(i, 0, serial, sizeof(serial));
		int serialId = atoi(serial);
		if (serialId == rsp->SerialId)
		{
			m_positionWnd->m_listPosition.DeleteItem(i);
			m_positionWnd->AddPositionToList(*rsp, 0);
			m_positionWnd->m_listPosition.SelectItem(0);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改持仓成功！", TMB_OK);
}

void SystemFrame::OnQryPositionData(SmsPositionDataRsp * rsp)
{
	if (!rsp)
		return;
	m_vecPosition.push_back(*rsp);
}

void SystemFrame::OnQryPosition(SmsPositionQryRsp * rsp)
{
	if (!rsp)
		return;
	SetEvent(m_partAccountWnd->m_hQryPosition);
	SetEvent(m_allAccountWnd->m_hQryPosition);
	PostMessage(m_positionWnd->GetHwnd(), WM_POSITION_REFRESH, 0, 0);
	
	//for (auto it : m_vecPosition)
	//{
	//	m_positionWnd->AddPositionToList(it);
	//}
	//m_vecPosition.clear();


}

void SystemFrame::OnDeleteOrder(SmsOrderDelRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsOrderDataRsp>::iterator it = m_vecOrder.begin(); it != m_vecOrder.end(); ++it)
	{
		if (strcmp(it->OrderNo,rsp->OrderNo)==0 && strcmp(it->SystemNo,rsp->SystemNo) == 0)
		{
			m_vecOrder.erase(it);
			break;
		}
	}

	int count = m_orderWnd->m_listOrder.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char OrderNo[51] = { 0 };
		char SystemNo[51] = { 0 };
		m_orderWnd->m_listOrder.GetSubItem(i, ORDER_OrderNo, OrderNo, sizeof(OrderNo));
		m_orderWnd->m_listOrder.GetSubItem(i, ORDER_SystemNo, SystemNo, sizeof(SystemNo));
		if (strcmp(OrderNo, rsp->OrderNo) == 0 && strcmp(SystemNo, rsp->SystemNo) == 0)
		{
			m_orderWnd->m_listOrder.DeleteItem(i);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除委托成功！", TMB_OK);
}

void SystemFrame::OnModifyOrder(SmsOrderModRsp * rsp)
{
	if (!rsp)return;
	for (vector<SmsOrderDataRsp>::iterator it = m_vecOrder.begin(); it != m_vecOrder.end(); ++it)
	{
		if (strcmp(it->SystemNo, rsp->SystemNo)==0)
		{
			memcpy(&(*it), rsp, sizeof(SmsOrderDataRsp));
			break;
		}
	}

	int count = m_orderWnd->m_listOrder.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char systemno[51] = { 0 };
		m_orderWnd->m_listOrder.GetSubItem(i, ORDER_SystemNo, systemno, sizeof(systemno));
		
		if (strcmp(systemno,rsp->SystemNo)==0)
		{
			m_orderWnd->m_listOrder.DeleteItem(i);
			m_orderWnd->AddOrderToList(*rsp);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改委托成功！", TMB_OK);
}

void SystemFrame::OnQryOrderData(SmsOrderDataRsp * rsp)
{
	if (!rsp)
		return;
	m_vecOrder.push_back(*rsp);
}

void SystemFrame::OnQryOrder(SmsOrderQryRsp * rsp)
{
	if (!rsp)
		return;

	PostMessage(m_orderWnd->GetHwnd(), WM_ORDER_REFRESH, 0, 0);
}

void SystemFrame::OnAddMatch(SmsMatchAddRsp * rsp)
{
	if (!rsp)return;


	m_mapMatch[rsp->MatchNo] =  (*rsp);

	m_matchWnd->AddMatchToList(*rsp, 0);

	_TMessageBox_Domodal(m_Hwnd, "提示", L"增加成交成功！", TMB_OK);
}

void SystemFrame::OnDeleteMatch(SmsMatchDelRsp * rsp)
{
	if (!rsp)return;
	auto it_del = m_mapMatch.find(rsp->MatchNo);
	if (it_del == m_mapMatch.end())
		return;
	else
	{
		m_mapMatch.erase(it_del);
	}

	//for (vector<SmsMatchDataRsp>::iterator it = m_vecMatch.begin(); it != m_vecMatch.end(); ++it)
	//{
	//	if (strcmp(it->SystemNo, rsp->SystemNo) == 0)
	//	{
	//		m_vecMatch.erase(it);
	//		break;
	//	}
	//}

	int count = m_matchWnd->m_listMatch.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char matchno[64] = { 0 };
		m_matchWnd->m_listMatch.GetSubItem(i, MATCH_SystemNo, matchno, sizeof(matchno));
		if (strcmp(matchno, rsp->SystemNo) == 0)
		{
			m_matchWnd->m_listMatch.DeleteItem(i);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除成交成功！", TMB_OK);
}

void SystemFrame::OnModifyMatch(SmsMatchModRsp * rsp)
{
	if (!rsp)return;
	m_mapMatch[rsp->MatchNo] = (*rsp);
	//for (vector<SmsMatchDataRsp>::iterator it = m_vecMatch.begin(); it != m_vecMatch.end(); ++it)
	//{
	//	if (strcmp(it->SystemNo ,rsp->SystemNo)==0)
	//	{
	//		memcpy(&(*it), rsp, sizeof(SmsMatchDataRsp));
	//		break;
	//	}
	//}

	int count = m_matchWnd->m_listMatch.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		char matchno[64] = { 0 };
		m_matchWnd->m_listMatch.GetSubItem(i, MATCH_SystemNo, matchno, sizeof(matchno));
		if (strcmp(matchno, rsp->SystemNo) == 0)
		{
			m_matchWnd->m_listMatch.DeleteItem(i);
			m_matchWnd->AddMatchToList(*rsp, 0);
			m_matchWnd->m_listMatch.SelectItem(0);
			break;
		}
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改成交成功！", TMB_OK);
}

void SystemFrame::OnQryMatchData(SmsMatchDataRsp * rsp)
{
	if (!rsp)
		return;
	//m_vecMatch.push_back(*rsp);
	m_mapMatch[rsp->MatchNo] =  (*rsp);
}

void SystemFrame::OnQryMatch(SmsMatchQryRsp * rsp)
{
	if (!rsp)
		return;

	SetEvent(m_integrationWnd->m_hQryMatch);
	SetEvent(m_sendOrderWnd->m_hQryMatch);
	SetEvent(m_exposureWnd->m_hQryMatch);
	SetEvent(m_instantPositionWnd->m_hQryMatch);

	if (strlen(rsp->BeginDate) == 0 && strlen(rsp->EndDate) == 0)
	{
		PostMessage(m_matchWnd->GetHwnd(), WM_MATCH_REFRESH, 0, 0);
	}
	else
		PostMessage(m_matchWnd->GetHwnd(), WM_MATCH_REFRESH, 0, 0);
	//for (auto it : m_mapMatch)
	//{
	//	m_matchWnd->AddMatchToList(it.second);
	//}
}

void SystemFrame::OnAddTax(SmsTaxAddRsp * rsp)
{
	if (!rsp)return;

	m_setTax.insert(*rsp);

	m_taxWnd->m_listTax.DeleteAllItems();
	for (auto it : m_setTax)
	{
		m_taxWnd->AddTaxToList(it);
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"增加税率成功！", TMB_OK);
}

void SystemFrame::OnDeleteTax(SmsTaxDelRsp * rsp)
{
	if (!rsp)return;
	for (TaxSetType::iterator it = m_setTax.begin(); it != m_setTax.end(); ++it)
	{
		if (strcmp(it->BeginDate, rsp->BeginDate) == 0)
		{
			m_setTax.erase(it);
			break;
		}
	}

	m_taxWnd->m_listTax.DeleteAllItems();
	for (auto it : m_setTax)
	{
		m_taxWnd->AddTaxToList(it);
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"删除税率成功！", TMB_OK);

}

void SystemFrame::OnModifyTax(SmsTaxModRsp * rsp)
{
	if (!rsp)return;
	for (TaxSetType::iterator it = m_setTax.begin(); it != m_setTax.end(); ++it)
	{
		if (strcmp(it->BeginDate, rsp->BeginDate) == 0)
		{
			m_setTax.erase(it);
			break;
		}
	}
	m_setTax.insert(*rsp);

	m_taxWnd->m_listTax.DeleteAllItems();
	for (auto it : m_setTax)
	{
		m_taxWnd->AddTaxToList(it);
	}
	_TMessageBox_Domodal(m_Hwnd, "提示", L"修改税率成功！", TMB_OK);
}

void SystemFrame::switch_index(const int main_index, const int sub_index)
{
	if (main_index == -1 || sub_index == -1)
		return;
	if (main_index == m_current_main_index && sub_index == m_current_sub_index)
		return;

	//隐藏老窗口
	HWND old_dlg = NULL;
	if (m_current_main_index != -1 && m_current_sub_index != -1)
		old_dlg = m_main_bars[m_current_main_index].second[m_current_sub_index].second.sub_hwnd;


	//显示新窗口
	HWND dlg = m_main_bars[main_index].second[sub_index].second.sub_hwnd;
	//dlg_key = get_dlg_key(main_index, sub_index);
	//it = m_dlgs.find(dlg_key);
	//if (it != m_dlgs.end())
	//	dlg = it->second;
	//else if (m_main_bars[main_index].second.size() > 0)
	//{
	//	sub_pair sub = m_main_bars[main_index].second[sub_index];
	//	try
	//	{
	//		dlg = sub.second.create_func(sub.first.c_str(), m_hwnd);
	//	}
	//	catch (...)
	//	{
	//	}

	//	IS_OUT(dlg == nullptr, "dlg create faild, main: %d sub: %d", main_index, sub_index);
	//	if (dlg != nullptr)
	//	{
	//		dlg_init(dlg);
	//		m_dlgs.insert(dlg_map::value_type(dlg_key, dlg));
	//	}
	//}

	if (::IsWindow(dlg))
	{

		if (m_main_bars[main_index].second.size() == 1)
		{
			RECT rect;
			get_dlg_rect(rect);
			rect.left -= m_sub_bar_width;
			::MoveWindow(dlg, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
			::ShowWindow(dlg, SW_SHOW);
		}
		else
		{
			RECT rect;
			get_dlg_rect(rect);
			::MoveWindow(dlg, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
			::ShowWindow(dlg, SW_SHOW);
		}
	}

	if (old_dlg)
		::ShowWindow(old_dlg, SW_HIDE);

	m_current_dlg = dlg;


	//更新索引
	m_current_main_index = main_index;
	m_current_sub_index = sub_index;
	m_sub_indexs[main_index] = sub_index;

	//重绘
	RECT client;
	::GetClientRect(m_Hwnd, &client);
	::InvalidateRect(m_Hwnd, &client, FALSE);
}

std::vector<std::string> SystemFrame::SplitString(const std::string & str, char cSplit)
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
		else//没找到' '但有内容  类似abc;这是仍然要保存
		{
			iterator_traits<string::iterator>::value_type dis = distance(tempit, tempstr.end());
			vecstr.push_back(tempstr);
			break;
		}
	}
	return vecstr;
}

void SystemFrame::StrTimeToSysTime(string str, SYSTEMTIME & time)
{
	vector<string> datetime;
	datetime = SplitString(str, ' ');
	if (datetime.size() < 2)
		return;

	vector<string> vdate;
	vdate = SplitString(datetime.at(0), '-');
	if (vdate.size() < 3)
		return;
	time.wYear = atoi(vdate.at(0).c_str());
	time.wMonth = atoi(vdate.at(1).c_str());
	time.wDay = atoi(vdate.at(2).c_str());

	vector<string> vtime;
	vtime = SplitString(datetime.at(1), ':');
	if (vtime.size() < 3)
		return;
	time.wHour = atoi(vtime.at(0).c_str());
	time.wMinute = atoi(vtime.at(1).c_str());
	time.wSecond = atoi(vtime.at(2).c_str());
}
