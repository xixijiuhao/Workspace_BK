#include	"PreInclude.h"
extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体
extern ILanguageApi*		g_pLanguage;				//语言资源
TBaseGrid::TBaseGrid()
{
	m_pgrid = NULL;
	m_npredatacount = 0;
	m_bfirst = true;

	//m_bshowfilter = AfxConfig().m_bshowfilter;
	//m_bclearfilter = false;
	m_emfont = Font_Small;
}

TBaseGrid::~TBaseGrid()
{
	TDeleteMem(m_pgrid);
	DestroyMenu(m_menu);
}

void TBaseGrid::SetFontSize(Font_Size fsize)
{
	m_emfont = fsize;
	m_pgrid->SetGridFont(GetCurFont());
}

bool TBaseGrid::Create(HWND hparent, LPCTSTR pstr, int nCol)
{
	CreateFrm(pstr, hparent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	SetWindowTextA(GetHwnd(), "TBaseGrid");

	m_pgrid = new TGridCtrl;
	m_pgrid->Create(m_Hwnd);

	m_pgrid->SetBdColor(RGB(243, 243, 243));
	m_pgrid->SetBkColor(RGB(243, 243, 243));
	m_pgrid->SetFixRow(1);
	m_pgrid->SetColumnCount(nCol);
	m_pgrid->SetRowCount(1);
	//m_pgrid->EnableExpandHorz(true);

	InitLayout();
	return true;
}

void TBaseGrid::MoveWindow(int nleft, int ntop, int nright, int nbottom)
{
	::MoveWindow(m_Hwnd, nleft, ntop, nright - nleft, nbottom - ntop, TRUE);
}

void TBaseGrid::Resize(HWND hwnd)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	int nbottom = rect.top;

	if (m_pgrid)
	{
		CreateList();
	}

	rect.top = nbottom;
	if (m_pgrid)
	{
		int npos = m_pgrid->GetVertPos();
		m_pgrid->MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		UpdateList(npos);

		//外部的滚动条
		//if (m_Hwnd == hwnd)
		//{
		//	SCROLLINFO stinfo;
		//	m_pgrid->GetHorzInfo(&stinfo);
		//	if (m_pscrollbar)
		//		m_pscrollbar->SetScrollInfo(&stinfo);
		//}
	}
}


void TBaseGrid::ConstructFilter(VFilter * vfilter, string & szinfo)
{
	VFilter::iterator itor;
	szinfo.clear();
	for (itor = vfilter->begin(); itor != vfilter->end(); itor++)
	{
		if (strlen(itor->szvalue) <= 0) continue;
		if (itor->bcheck)
		{
			szinfo.append(itor->sztext);
			szinfo.append(" ");
		}
	}
}

LRESULT TBaseGrid::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		//OnGridCtrlSwapCol(wParam, lParam);
		break;
	case SSWM_AUTO_HSCROLL:
		//if (m_pscrollbar)
		//{
		//	if (wParam == VK_LEFT) m_pscrollbar->ScrollLeft();
		//	if (wParam == VK_RIGHT) m_pscrollbar->ScrollRight();
		//}
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void TBaseGrid::SetColumnWidth()
{
	//std::vector<ConfigData>::iterator itor;
	//if (!m_pgrid) return;
	//int ncol = 0;
	//for (itor = m_stlconfig.begin(); itor != m_stlconfig.end(); itor++, ncol++)
	//{
	//	int ncolwidth = 0;
	//	if (itor->bauthor && !itor->bignore&&itor->bshow)
	//		/*if (itor->bshow) */ncolwidth = itor->ncolwidth;
	//	m_pgrid->SetColWidth(ncol, ncolwidth);
	//}

	//外部的滚动条
	//SCROLLINFO stinfo;
	//m_pgrid->GetHorzInfo(&stinfo);
	//if (m_pscrollbar)
	//	m_pscrollbar->SetScrollInfo(&stinfo);
}

void TBaseGrid::CreateList()
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		//ConfigData stcofig;
		item.cellid.ncol = ncol;
		item.cellid.nrow = 0;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.dwformat = DT_VCENTER;
		item.emtype = CellType_Header;
		item.dwformat |= DT_CENTER;
		//stcofig = m_stlconfig[ncol];
		//if (stcofig.nformat == 0) item.dwformat |= DT_LEFT;
		//else if (stcofig.nformat == 1) item.dwformat |= DT_CENTER;
		//else item.dwformat |= DT_RIGHT;

		strcpy_s(item.szparam, "22");
		m_pgrid->SetItem(&item);
	}
}

void TBaseGrid::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	HBRUSH hbr = CreateSolidBrush(g_ColorRefData.GetColorBackground());
	FillRect(memdc.GetHdc(), &rect, hbr);
	DeleteObject(hbr);

}

void TBaseGrid::OnSize()
{
	Resize();
}

void TBaseGrid::OnNotify(WPARAM wParam, LPARAM lParam)
{
	UpdateList(-1);
}

void TBaseGrid::OnShowMenu(WPARAM wParam, LPARAM lParam)
{
	HMENU hmenu = m_menu;
	hmenu = CreatePopupMenu();

	//if (m_bclearfilter)
	//	AppendMenuW(hmenu, MF_STRING, 100, g_pLanguage->LangText(500));
	for (int i = 0; i < 3; i++)
	{
		//DWORD dwsize = MAX_PATH; wchar_t sztext[MAX_PATH];
		//MByteToWChar(GetMenutextString(i), sztext, dwsize);
		//AppendMenuW(hmenu, MF_STRING, 101 + i, sztext);
		AppendMenuW(hmenu, MF_STRING, 101 + i, g_pLanguage->LangText(101+i));
	}

	int nlast = 103;
	ConstructMenu(hmenu, nlast);

	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(hmenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_Hwnd, NULL);

	DestroyMenu(hmenu);
}

void TBaseGrid::OnGridCtrlColWidth(WPARAM wParam, LPARAM lParam)
{

}

void TBaseGrid::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int ntype = HIWORD(wParam);
	if (ntype != 0) return;

	int nmenuid = LOWORD(wParam);

	//if (nmenuid == 101)
	//{
	//	TConfigCol dlg(m_Hwnd, GetMenutextString(0), m_pgrid->GetFixColumn());
	//	dlg.SetData(&m_stlconfig, &m_emfont);
	//	dlg.Domodal();
	//	SaveConfigWidth();
	//}
	if (nmenuid == 102)
	{
		//AutoWidthServer();
	}
}


bool TBaseGrid::SaveAs(LPCTSTR ptitle, std::wstring & szfilename, wchar_t * pfileformat, HWND hwnd)
{
	USES_CONVERSION;
	TCHAR strPath[MAX_PATH] = _T("");
	TCHAR szTempName[MAX_PATH] = _T("");
	TCHAR szfileformat[MAX_PATH] = _T("");

	if (g_pLanguage->LangId() != ENU)
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



void TBaseGrid::AdjustShowPage(const int ntotalrow, int & nstartrow, bool bautosrcoll)
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

LOGFONT TBaseGrid::GetCurFont()
{
	LOGFONT lf = g_FontData.g18_font;
	if (m_emfont == Font_Middle)
		lf.lfHeight += 3;
	if (m_emfont == Font_Large)
		lf.lfHeight += 6;
	return lf;
}
