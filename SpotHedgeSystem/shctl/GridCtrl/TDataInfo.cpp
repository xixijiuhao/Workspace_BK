#include "PreInclude.h"

TDataInfo::TDataInfo()
{
	m_pgrid = NULL;
	//m_pscrollbar = NULL;
	m_npredatacount = 0;
	m_bfirst = true;

	//m_bshowfilter = AfxConfig().m_bshowfilter;
	m_bclearfilter = false;
	m_emfont = Font_Small;
}

TDataInfo::~TDataInfo()
{
	TDeleteMem(m_pgrid);
	DestroyMenu(m_menu);
}
//property
bool TDataInfo::Create(HWND hparent,LPCTSTR pstr,int nCol)
{
	CreateFrm(pstr, hparent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	SetWindowTextA(GetHwnd(), "TDataInfo");

	m_pgrid = new TGridCtrl;
	m_pgrid->Create(m_Hwnd);

	m_pgrid->SetBdColor(RGB(243, 243, 243));
	m_pgrid->SetBkColor(RGB(243, 243, 243));
	m_pgrid->SetFixRow(1);
	m_pgrid->SetColumnCount(nCol);
	m_pgrid->SetRowCount(1);
	m_pgrid->EnableExpandHorz(true);

	InitLayout();
	return true;
}

void TDataInfo::SetFontSize(Font_Size fsize)
{
	m_emfont = fsize;
	m_pgrid->SetGridFont(GetCurFont());
}

void TDataInfo::MoveWindow(int nleft, int ntop, int nright, int nbottom)
{
	::MoveWindow(m_Hwnd, nleft, ntop, nright - nleft, nbottom - ntop, TRUE);
}

DWORD WINAPI AutoWidthProc(LPVOID pParam)
{
	TDataInfo* pinfo = (TDataInfo*)pParam;

	pinfo->SetAutoWidth();
	if (pinfo->GetHwnd())
		::PostMessage(pinfo->GetHwnd(), SSWM_GRIDCTRL_AUTO_COL_WIDTH, 0, 0);
	return 0;
}
void TDataInfo::AutoWidthServer()
{
	m_handle = CreateThread(NULL, 0, AutoWidthProc, this, 0, 0);
}

void TDataInfo::CreateList()
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		ConfigData stcofig;
		item.cellid.ncol = ncol;
		item.cellid.nrow = 0;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.dwformat = DT_VCENTER;
		item.emtype = CellType_Filter;

		stcofig = m_stlconfig[ncol];
		if (stcofig.nformat == 0) item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1) item.dwformat |= DT_CENTER;
		else item.dwformat |= DT_RIGHT;

		strcpy_s(item.szparam, stcofig.szcol.c_str());
		m_pgrid->SetItem(&item);
	}
}

void TDataInfo::SetColumnWidth()
{
	std::vector<ConfigData>::iterator itor;
	if (!m_pgrid) return;
	int ncol = 0;
	for (itor = m_stlconfig.begin(); itor != m_stlconfig.end(); itor++, ncol++)
	{
		int ncolwidth = 0;
		if (itor->bauthor&&!itor->bignore&&itor->bshow)
		/*if (itor->bshow) */ncolwidth = itor->ncolwidth;
		m_pgrid->SetColWidth(ncol, ncolwidth);
	}

	//外部的滚动条
	SCROLLINFO stinfo;
	m_pgrid->GetHorzInfo(&stinfo);
	//if (m_pscrollbar)
	//	m_pscrollbar->SetScrollInfo(&stinfo);
}

void TDataInfo::Resize(HWND hwnd/* = NULL*/)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	int nbottom = rect.top;

	int nvaliduser = GetValidAccount();
	if (m_pgrid)
	{
		if (1 <= nvaliduser)
			SetListHeader();
 		else
 			CreateList();
	}

	rect.top = nbottom;
	if (m_pgrid)
	{
		int npos = m_pgrid->GetVertPos();
		m_pgrid->MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		UpdateList(npos);

		//外部的滚动条
		if (m_Hwnd == hwnd)
		{
			SCROLLINFO stinfo;
			m_pgrid->GetHorzInfo(&stinfo);
			//if (m_pscrollbar)
			//	m_pscrollbar->SetScrollInfo(&stinfo);
		}
	}
}

void TDataInfo::_GetHScrollInfo(LPSCROLLINFO stinfo)
{
	m_pgrid->GetHorzInfo(stinfo);
}

void TDataInfo::_SetHScrollInfo(LPSCROLLINFO stinfo)
{
	m_pgrid->SetHorzInfo(*stinfo);
}

void TDataInfo::GetSpreadId(TMatch* pmatch, char* sztext, int nlength)
{
	memset(sztext, 0, nlength);
	if (pmatch == NULL)
		return;
	if (pmatch->pOrder == NULL)
		return;
	int nretid = 0;
	nretid = g_strateapi->GetSpreadIdbyOrderId(pmatch->pOrder->OrderID);
	if (nretid != 0)
		sprintf_s(sztext, nlength, "%d", nretid);
}

//msg
void TDataInfo::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT rect;
	GetClientRect(m_Hwnd,&rect);

	HBRUSH hbr = CreateSolidBrush(g_color_listbk);
	FillRect(memdc.GetHdc(), &rect, hbr);
	DeleteObject(hbr);

	int nvaliduser = GetValidAccount();
	if (1 <= nvaliduser)
		m_layout.MoveLayout(rect.left, rect.top, rect.right, &memdc);
}

void TDataInfo::OnSize()
{
	Resize();
}

void TDataInfo::OnNotify(WPARAM wParam, LPARAM lParam)
{
	//UpdateList(pinfo->nbeginrow);
	UpdateList(-1);
}

void TDataInfo::OnShowMenu(WPARAM wParam, LPARAM lParam)
{
	HMENU hmenu = m_menu;
	hmenu = CreatePopupMenu();

	if(m_bclearfilter)
		AppendMenuW(hmenu, MF_STRING, 100, g_langapi->LangText(500));
	for (int i = 0; i < 3; i++)
	{
		DWORD dwsize = MAX_PATH; wchar_t sztext[MAX_PATH];
		MByteToWChar(GetMenutextString(i), sztext, dwsize);
		AppendMenuW(hmenu, MF_STRING, 101+i, sztext);
	}

	int nlast = 103;
	ConstructMenu(hmenu, nlast);

	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_Hwnd, NULL);

	DestroyMenu(hmenu);
}

void TDataInfo::OnGridCtrlColWidth(WPARAM wParam, LPARAM lParam)
{
	int ncol = (int)wParam;
	int nwidth = (int)lParam;

	int nindex = 0;
	std::vector<ConfigData>::iterator itor;
	for (itor = m_stlconfig.begin(); itor != m_stlconfig.end(); itor++, nindex++)
	{
		if (nindex == ncol)
		{
			itor->ncolwidth = nwidth;
			break;
		}
	}

	SetColumnWidth();
	SaveConfigWidth();
}

void TDataInfo::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int ntype = HIWORD(wParam);
	if (ntype != 0) return;

	int nmenuid = LOWORD(wParam);

	if (nmenuid == 101)
	{
		TConfigCol dlg(m_Hwnd, GetMenutextString(0),m_pgrid->GetFixColumn());
		dlg.SetData(&m_stlconfig,&m_emfont);
		dlg.Domodal();
		SaveConfigWidth();
	}
	if (nmenuid == 102)
	{
		AutoWidthServer();
	}
}

void TDataInfo::OnGridCtrlSwapCol(WPARAM wParam, LPARAM lParam)
{
	int nfixcol = m_pgrid->GetFixColumn();
	int nsrccol = (int)wParam; int ndescol = (int)lParam;
	if (nsrccol < 0 || ndescol < 0) return;
	if (nsrccol < nfixcol || ndescol < nfixcol) return;
	SwapCol(nsrccol, ndescol);
	SetColumnWidth();
	CreateList();
	SetListHeader();
	UpdateList(m_pgrid->GetVertPos());
	SaveConfigWidth();
}
LRESULT TDataInfo::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_SIZE:
		OnSize();
		break;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		break;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		break;
	case SSWM_DATA_INFO_SHOW_MENU:
		OnShowMenu(wParam, lParam);
		break;
	case SSWM_CONFIG_GRIDLIST:
		SendMessage(GetParent(), SSWM_CONFIG_GRIDLIST, 0, (LPARAM)this);
		SetColumnWidth();
		CreateList();
		SetListHeader();
		UpdateList(m_pgrid->GetVertPos());
		break;
	case SSWM_GRIDCTRL_COL_WIDTH:
		OnGridCtrlColWidth(wParam, lParam);
		break;
	case SSWM_GRIDCTRL_AUTO_COL_WIDTH:
		SetColumnWidth();
		UpdateList(m_pgrid->GetVertPos());
		break;
	case SSWM_GRIDCTRL_SWAP_COL:
		OnGridCtrlSwapCol(wParam, lParam);
		break;
	case SSWM_AUTO_HSCROLL:
		if (m_pscrollbar)
		{
			if (wParam == VK_LEFT) m_pscrollbar->ScrollLeft();
			if (wParam == VK_RIGHT) m_pscrollbar->ScrollRight();
		}
		break;
	default:
		return -1;
	}

	return 0;
}

TCombox* TDataInfo::CreateCombox(int nwidth, int nheight, bool bedit/* = false*/)
{
	TCombox* pcombox = NULL;
	pcombox = new TCombox;
	pcombox->Create(m_Hwnd, 0, bedit);
	pcombox->MoveWindow(0, 0, nwidth, nheight);
	return pcombox;
}

TEdit* TDataInfo::CreateEdit(int nwidth, int nheight)
{
	TEdit* pedit;
	pedit = new TEdit;
	pedit->Create(m_Hwnd);
	pedit->MoveWindow(0, 0, nwidth, nheight);
	pedit->SetFont(g_font_15);
	pedit->SetDefText("");
	return pedit;
}

bool TDataInfo::SaveAs(LPCTSTR ptitle, std::wstring& szfilename, wchar_t* pfileformat/* = NULL*/,HWND hwnd/* = NULL*/)
{
	USES_CONVERSION;
	TCHAR strPath[MAX_PATH] = _T("");
	TCHAR szTempName[MAX_PATH] = _T("");
	TCHAR szfileformat[MAX_PATH] = _T("");

	if (g_language != ENU)
		memcpy(szfileformat, _T("文本文件(*.txt)\0*.txt\0Excel文件(*.csv)\0*.csv\0\0"), 90);
	else
		memcpy(szfileformat, _T("TextFile(*.txt)\0*.txt\0ExcelFile(*.csv)\0*.csv\0\0"), 90);
	OPENFILENAME ofn = { sizeof(OPENFILENAME), hwnd, NULL,
		szfileformat, NULL,
		0, 1, szTempName, MAX_PATH, NULL, 0, strPath,
		ptitle,
		OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST |
		OFN_HIDEREADONLY | OFN_NOREADONLYRETURN,
		0, 0, _T(".txt"), 0, NULL, NULL };
	if (GetSaveFileName(&ofn))
	{
		szfilename = szTempName;// ConvertLPWSTRToLPSTR(szTempName);
		return true;
	}

	return false;
}

int TDataInfo::GetValidAccount()
{
	int nvaliduser = 0;

	if (m_bshowfilter)
	{
		for (int i = 0; i < (int)AfxGetUserInfo().size(); i++)
		{
			TUserInfo* pinfo = (TUserInfo*)AfxGetUserInfo().at(i);
			if (pinfo&&pinfo->pLogin&&pinfo->pLogin->Logined == bYes)
				nvaliduser++;
		}
	}

	return nvaliduser;
}
bool TDataInfo::IsTraderMenu()
{
	bool badd = false;
	for (int i = 0; i < (int)AfxGetUserInfo().size(); i++)
	{
		TUserInfo* pdata = (TUserInfo*)AfxGetUserInfo().at(i);
		if (pdata&&IsLoginning(pdata) && pdata->pLogin->UserType == uiTrader)
		{
			badd = true;
			break;
		}
	}

	return badd;
}

bool TDataInfo::IsUserTrays()//内盘用户
{
	bool btrays = false;
	for (int i = 0; i < (int)AfxGetUserInfo().size(); i++)
	{
		TUserInfo* pdata = (TUserInfo*)AfxGetUserInfo().at(i);
		if (pdata&&IsLoginning(pdata)&& (strcmp(pdata->pLogin->LoginClassName, "ForeignTradeApi") != 0
			&& strcmp(pdata->pLogin->LoginClassName, "DipperTradeApi") != 0))
		{
			btrays = true;
			break;
		}
	}

	return btrays;
}

bool TDataInfo::IsDaystarUser()
{
	bool byes = false;
	for (int i = 0; i < (int)AfxGetUserInfo().size(); i++)
	{
		TUserInfo* pdata = (TUserInfo*)AfxGetUserInfo().at(i);
		if (pdata&&IsLoginning(pdata) && (strcmp(pdata->pLogin->LoginClassName, "DaystarTradeApi") == 0))
		{
			byes = true;
			break;
		}
	}

	return byes;
}

bool TDataInfo::IsUserExternal()//外盘用户
{
	bool btrays = false;
	for (int i = 0; i < (int)AfxGetUserInfo().size(); i++)
	{
		TUserInfo* pdata = (TUserInfo*)AfxGetUserInfo().at(i);
		if (pdata&&IsLoginning(pdata)&& (strcmp(pdata->pLogin->LoginClassName, "ForeignTradeApi") == 0 ||
			strcmp(pdata->pLogin->LoginClassName, "DipperTradeApi") == 0))
		{
			btrays = true;
			break;
		}
	}

	return btrays;
}

bool TDataInfo::IsUserExternalSingle()//外盘单客户
{
	bool besingle = false;
	for (int i = 0; i < (int)AfxGetUserInfo().size(); i++)
	{
		TUserInfo* pdata = (TUserInfo*)AfxGetUserInfo().at(i);
		if (pdata&&IsLoginning(pdata) && (strcmp(pdata->pLogin->LoginClassName, "ForeignTradeApi") == 0 ||
			strcmp(pdata->pLogin->LoginClassName, "DipperTradeApi") == 0) && pdata->pLogin->UserType == uiUser)
		{
			besingle = true;
			break;
		}
	}

	return besingle;
}
bool TDataInfo::IsUSerExternalTrader()//外盘交易员
{
	bool betrader = false;
	for (int i = 0; i < (int)AfxGetUserInfo().size(); i++)
	{
		TUserInfo* pdata = (TUserInfo*)AfxGetUserInfo().at(i);
		if (pdata&&IsLoginning(pdata) && (strcmp(pdata->pLogin->LoginClassName, "ForeignTradeApi") == 0 ||
			strcmp(pdata->pLogin->LoginClassName, "DipperTradeApi") == 0) && pdata->pLogin->UserType == uiTrader)
		{
			betrader = true;
			break;
		}
	}

	return betrader;
}

bool TDataInfo::IsExternal_9_0()//仅有外盘9.0
{
	bool bret = true;
	int nsize = (int)AfxGetUserInfo().size();
	if (nsize == 0) bret = false;
	for (int i = 0; i < nsize; i++)
	{
		TUserInfo* pdata = (TUserInfo*)AfxGetUserInfo().at(i);
		if (pdata&&IsLoginning(pdata) && (strcmp(pdata->pLogin->LoginClassName, "DipperTradeApi") != 0))
		{
			bret = false;
			break;
		}
	}

	return bret;
}

BOOL TDataInfo::IsOnlyExternal()//仅有外盘用户
{
	return AfxConfig().IsOnlyExternal();
}

bool TDataInfo::IsDipperUser()//是否包含北斗
{
	bool byes = false;
	for (int i = 0; i < (int)AfxGetUserInfo().size(); i++)
	{
		TUserInfo* pdata = (TUserInfo*)AfxGetUserInfo().at(i);
		if (pdata&&IsLoginning(pdata) && (strcmp(pdata->pLogin->LoginClassName, "DipperTradeApi") == 0))
		{
			byes = true;
			break;
		}
	}

	return byes;
}
bool TDataInfo::IsTraderHaveRight(TradeUserRight emright)
{
	TLoginContainer stcontain;
	if (G_TradeData)G_TradeData->GetAllLogin(stcontain);
	if (0 >= stcontain.size()) return false;
	for (int i = 0; i < (int)stcontain.size(); i++)
	{
		TLogin* plogin = (TLogin*)stcontain.at(i);
		if (plogin->Logined&&IsHaveRight(plogin->Rights, 16, emright))
			return true;
	}
	return false;
}

bool TDataInfo::IsHaveRight(TradeUserRight* pright, int nsize, TradeUserRight emright)
{
	for (int i = 0; i < nsize; i++)
	{
		if (pright[i] == emright)
			return true;
	}
	return false;
}

BOOL TDataInfo::IsLoginning(TUserInfo* puser)
{
	return AfxConfig().IsLoginning(puser);
}

void TDataInfo::ConstructFilter(VFilter* vfilter, string& szinfo)
{
	VFilter::iterator itor;
	szinfo.clear();
	for (itor = vfilter->begin(); itor != vfilter->end(); itor++)
	{
		if(strlen(itor->szvalue) <= 0) continue;
		if (itor->bcheck)
		{
			szinfo.append(itor->sztext);
			szinfo.append(" ");
		}
	}
}

void TDataInfo::SwapCol(int nsrc, int ndes)
{
 	ConfigData stsrc = m_stlconfig.at(nsrc);

	int nindex = 0;
	if (nsrc < ndes)
	{
		for (std::vector<ConfigData>::iterator itor = m_stlconfig.begin(); itor != m_stlconfig.end(); nindex++)
		{
			if (nindex == nsrc)
				itor = m_stlconfig.erase(itor);
			else
			{
				itor++;
				if (nindex == ndes)
				{
					m_stlconfig.insert(itor, stsrc);
					break;
				}
			}
		}
	}
	else if (nsrc > ndes)
	{
		for (std::vector<ConfigData>::iterator itor = m_stlconfig.begin(); itor != m_stlconfig.end(); itor++, nindex++)
		{
			if (nindex == ndes) 
				itor = m_stlconfig.insert(itor, stsrc);
			else
			{
				if (nindex == (nsrc+1))
				{
					m_stlconfig.erase(itor);
					break;
				}
			}
		}
	}
}

void TDataInfo::AdjustShowPage(const int ntotalrow, int& nstartrow, bool bautosrcoll)//zxw
{
	if (ntotalrow != m_npredatacount)
	{
		int nrange = ntotalrow - m_npredatacount;
		if (nstartrow == 0 || !bautosrcoll/*!IsTraderMenu()*/)
		{
			nstartrow = 0;
			int ncurpos = m_pgrid->GetVertPos();
			m_pgrid->SetSelectRow(1);
			m_pgrid->SetMoveVertPos(-ncurpos);
		}
		else if (nstartrow > 0)
		{
			nstartrow += nrange;
			nstartrow = min(ntotalrow - 1, nstartrow);
			nstartrow = max(0, nstartrow);
			m_pgrid->SetMoveVertPos(nrange);
			m_pgrid->RefreshSelectRow(nrange);
		}
	}
	m_npredatacount = ntotalrow;
}

void TDataInfo::SaveConfigWidth()
{
	TListConfig cpconfig;
	cpconfig.SaveConfig(m_szconfig, m_stlconfig);
}

void TDataInfo::StaticsFormat(wchar_t* ptext,int ntextsize,int nline,int nbuy,int nsell,int nopen,int ncover)
{
	int nlength;
	if (g_language == ENU) nlength = 17;
	else nlength = 10;
	wchar_t szformat[100];
	swprintf_s(szformat,100, _T("%%%ds %%7d\r\n%%%ds %%7d\r\n%%%ds %%7d\r\n%%%ds %%7d\r\n%%%ds %%7d\r\n%%%ds %%7d"),
		nlength, nlength, nlength, nlength, nlength, nlength);
	swprintf_s(ptext, ntextsize, szformat, GetStatisticsSign(0), nline, GetStatisticsSign(1), nbuy, GetStatisticsSign(2), nsell,
		GetStatisticsSign(3), nopen, GetStatisticsSign(4), ncover, GetStatisticsSign(5), nbuy + nsell);
}

_uint TDataInfo::GetListColumn(_uint nrealcol)
{
	_uint nindex = 0;
	std::vector<ConfigData>::iterator itor;
	for (itor = m_stlconfig.begin(); itor != m_stlconfig.end();itor++, nindex++)
	{
		if (itor->nrealindex == nrealcol)
			return nindex;
	}

	nindex = 0;
	return nindex;
}

_uint TDataInfo::GetRealColumn(_uint ncol)
{
	ConfigData stdata = m_stlconfig.at(ncol);
	return stdata.nrealindex;
}

void TDataInfo::GetFilterInfo(string& szinfo)
{ 
	ConFilterInfo();
	szinfo = m_szfilterinfo; 
}

LOGFONT TDataInfo::GetCurFont()
{
	LOGFONT lf = g18_font;
	if (m_emfont == Font_Middle)
		lf.lfHeight += 3;
	if (m_emfont == Font_Large)
		lf.lfHeight += 6;
	return lf;
}