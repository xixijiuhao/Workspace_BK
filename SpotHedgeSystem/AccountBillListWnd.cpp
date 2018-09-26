#include "PreInclude.h"



static int g_nTitleSize[AccountCol] = {
	100, 90, 100, 90, 100, 100, 110, 90, 90, 100,
	90, 100, 110, 100, 100, 100, 110, 90, 90
};
static bool g_bColShow[AccountCol] = {
	true, true, true, true, true, true, true, true, true, true,
	true, true, true, true, true, true, true, true, true
};
static int g_nColFormat[AccountCol] = {
	1, 1, 1, 1, 2, 1, 1, 1, 2, 1,
	1, 1, 1, 1, 1, 2, 1, 1, 1
};

//static string g_sTitleTxt0[AccountCol] = {
//	"", "", "", "", "", "", "", "", "", "期现对账单",
//	"", "", "", "", "", "", "", "", ""
//};
static string g_sTitleTxt0[AccountCol] = {
	"", "", "采购", "", "", "", "", "销售", "", "",
	"", "", "", "期货", "", "", "", "", ""
};
static string g_sTitleTxt1[AccountCol] = {
	"账号", "日期", "合同编号", "单价", "数量", "日期", "合同编号", "单价", "数量", "不含税盈亏",
	"合约编号", "开仓日期", "开仓价格", "平仓日期", "平仓价格", "数量", "手续费", "盈亏", "期现盈亏"
};

extern G_COLOREF			g_ColorRefData;				//色彩
extern G_UIFONT				g_FontData;					//字体

AccountBillListWnd::AccountBillListWnd()
{
	m_pcontainer = NULL;
}

AccountBillListWnd::~AccountBillListWnd()
{
	//TDeleteMem(m_pcontainer);
}

bool AccountBillListWnd::Create(HWND hparent, LPCTSTR pstr, int nCol)
{
	CreateFrm(pstr, hparent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	SetWindowTextA(GetHwnd(), "AccountBillListWnd");

	m_pgrid = new TGridCtrl;
	m_pgrid->Create(m_Hwnd);

	m_pgrid->SetBdColor(RGB(243, 243, 243));
	m_pgrid->SetBkColor(RGB(243, 243, 243));
	m_pgrid->SetFixRow(2);
	m_pgrid->SetFixColumn(1);
	m_pgrid->SetColumnCount(nCol);
	m_pgrid->SetRowCount(2);
	m_pgrid->EnableExpandHorz(false);
	m_pgrid->EnableSwapColumn(false);

	InitLayout();

	m_pcontainer = new TRowInfoContainer;
	LoadConfig();
	CreateList();
	SetColumnWidth();
	return true;
}

void AccountBillListWnd::MoveWindow(int nleft, int ntop, int nright, int nbottom)
{
	::MoveWindow(m_Hwnd, nleft, ntop, nright, nbottom, TRUE);
	//::MoveWindow(m_Hwnd, nleft, ntop, nright - nleft, nbottom - ntop, TRUE);
}

void AccountBillListWnd::Resize(HWND hwnd)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	int nbottom = rect.top;

	if (m_pgrid)
	{
		SetListHeader();		
		CreateList();
	}

	rect.top = nbottom;
	if (m_pgrid)
	{
		int npos = m_pgrid->GetVertPos();
		m_pgrid->MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		UpdateList(npos);

	}
}

void AccountBillListWnd::SetData(TRowInfoContainer * pdata)
{
	m_pcontainer = pdata;

	int npos = m_pgrid->GetVertPos();
	UpdateList(npos);
}

LRESULT AccountBillListWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	//case SSWM_DATA_INFO_SHOW_MENU:
	//	OnShowMenu(wParam, lParam);
	//	break;
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
	//case SSWM_GRIDCTRL_SWAP_COL:
	//	OnGridCtrlSwapCol(wParam, lParam);
	//	break;
	//case SSWM_AUTO_HSCROLL:
	//	if (m_pscrollbar)
	//	{
	//		if (wParam == VK_LEFT) m_pscrollbar->ScrollLeft();
	//		if (wParam == VK_RIGHT) m_pscrollbar->ScrollRight();
	//	}
	//	break;
	case WM_NCDESTROY:
		return NOT_PROCESSED;
	case WM_DESTROY:
		return NOT_PROCESSED;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void AccountBillListWnd::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	HBRUSH hbr = CreateSolidBrush(g_ColorRefData.GetColorBackground());
	FillRect(memdc.GetHdc(), &rect, hbr);
	DeleteObject(hbr);

}

void AccountBillListWnd::OnSize()
{
	Resize();
}

void AccountBillListWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	UpdateList(-1);
}

void AccountBillListWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
}

void AccountBillListWnd::OnGridCtrlColWidth(WPARAM wParam, LPARAM lParam)
{
	int ncol = (int)wParam;
	int nwidth = (int)lParam;

	int nindex = 0;
	std::vector<ConfigData>::iterator itor;
	for (itor = m_stlconfig0.begin(); itor != m_stlconfig0.end(); itor++, nindex++)
	{
		if (nindex == ncol)
		{
			itor->ncolwidth = nwidth;
			break;
		}
	}
	nindex = 0;
	for (itor = m_stlconfig1.begin(); itor != m_stlconfig1.end(); itor++, nindex++)
	{
		if (nindex == ncol)
		{
			itor->ncolwidth = nwidth;
			break;
		}
	}

	SetColumnWidth();

}

void AccountBillListWnd::UpdateList(int nstartrow)
{
	if (!m_pgrid) return;
	m_pgrid->SetRowCount(m_pcontainer->size() + 2);

	DataRange strange;
	m_pgrid->GetNonfixDataRange(strange, (nstartrow == 0));
	for (_uint nrow = strange.nbeginrow; nrow <= strange.nendrow; nrow++)
	{
		if (m_pcontainer->size() == 0)
			break;
		TRowInfo* pinfo = NULL;
		pinfo = (TRowInfo*)m_pcontainer->at(nrow - 2);
		if (!pinfo) break;

		switch (pinfo->RowInfoFormat)
		{
		case SMS_ROWINFOFORMAT_BASE:
			SetBaseRowItem(nrow, pinfo);
			break;
		case SMS_ROWINFOFORMAT_BUY1:
			SetBuy1RowItem(nrow, pinfo);
			break;
		case SMS_ROWINFOFORMAT_BUY2:
			SetBuy2RowItem(nrow, pinfo);
			break;
		case SMS_ROWINFOFORMAT_SELL1:
			SetSell1RowItem(nrow, pinfo);
			break;
		case SMS_ROWINFOFORMAT_SELL2:
			SetSell2RowItem(nrow, pinfo);
			break;
		case SMS_ROWINFOFORMAT_SUBTOTAL:
			SetSubTotalRowItem(nrow, pinfo);
			break;
		case SMS_ROWINFOFORMAT_SUMTOTAL:
			SetSumTotalRowItem(nrow, pinfo);
			break;
		default:
			SetBaseRowItem(nrow, pinfo);
			break;
		}


	}
}

void AccountBillListWnd::UpdateListText(int nrealindex, TRowInfo * pinfo, GV_ITEM & item)
{
	switch (nrealindex) 
	{
	case 0:	strcpy_s(item.szparam, pinfo->UserNo);
		break;
	case 1:strcpy_s(item.szparam, pinfo->BuyAgreeDate);
		break;
	case 2:strcpy_s(item.szparam, pinfo->BuyAgreeNo);
		break;
	case 3:strcpy_s(item.szparam, pinfo->BuyPrice);
		break;
	case 4:strcpy_s(item.szparam, pinfo->BuyQty);
		break;
	case 5:strcpy_s(item.szparam, pinfo->SellAgreeDate);
		break;
	case 6:strcpy_s(item.szparam, pinfo->SellAgreeNo);
		break;
	case 7:strcpy_s(item.szparam, pinfo->SellPrice);
		break;
	case 8:strcpy_s(item.szparam, pinfo->SellQty);
		break;
	case 9:strcpy_s(item.szparam, pinfo->SpotProfit);
		break;
	case 10:strcpy_s(item.szparam, pinfo->ContractNo);
		break;
	case 11:strcpy_s(item.szparam, pinfo->OpenDate);
		break;
	case 12:strcpy_s(item.szparam, pinfo->OpenPrice);
		break;
	case 13:strcpy_s(item.szparam, pinfo->CoverDate);
		break;
	case 14:strcpy_s(item.szparam, pinfo->CoverPrice);
		break;
	case 15:strcpy_s(item.szparam, pinfo->FutureQty);
		break;
	case 16:strcpy_s(item.szparam, pinfo->FutureFee);
		break;
	case 17:strcpy_s(item.szparam, pinfo->FutureProfit);
		break;
	case 18:strcpy_s(item.szparam, pinfo->TotalProfit);
		break;
	}
}



void AccountBillListWnd::CreateList()
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
		item.emtype = CellType_Base;
		stcofig = m_stlconfig0[ncol];
		if (stcofig.nformat == 0) 
			item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1) 
			item.dwformat |= DT_CENTER;
		else 
			item.dwformat |= DT_RIGHT;

		if (ncol >= 1 && ncol <= 3)
			item.b_right = false;
		if (ncol >= 5 && ncol <= 8)
			item.b_right = false;
		if (ncol >= 10 && ncol <= 16)
			item.b_right = false;

		strcpy_s(item.szparam, stcofig.szcol.c_str());
		m_pgrid->SetItem(&item);
	}
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		ConfigData stcofig;
		item.cellid.ncol = ncol;
		item.cellid.nrow = 1;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.dwformat = DT_VCENTER;
		item.emtype = CellType_Base;

		stcofig = m_stlconfig1[ncol];
		if (stcofig.nformat == 0) item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1) item.dwformat |= DT_CENTER;
		else item.dwformat |= DT_RIGHT;

		strcpy_s(item.szparam, stcofig.szcol.c_str());
		m_pgrid->SetItem(&item);
	}
}

void AccountBillListWnd::SetColumnWidth()
{
	std::vector<ConfigData>::iterator itor;
	if (!m_pgrid) return;

	int ncol = 0;
	for (itor = m_stlconfig1.begin(); itor != m_stlconfig1.end(); itor++, ncol++)
	{
		int ncolwidth = 0;
		if (itor->bauthor && !itor->bignore&&itor->bshow)
			ncolwidth = itor->ncolwidth;
		m_pgrid->SetColWidth(ncol, ncolwidth);
	}
}

void AccountBillListWnd::SetBaseRowItem(_uint nrow, TRowInfo* pinfo)
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		memset(&item, 0, sizeof(GV_ITEM));
		item.cellid.ncol = ncol;
		item.cellid.nrow = nrow;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.nvmargin = 5;
		item.nhmargin = 3;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
		item.dwformat = DT_VCENTER;

		ConfigData stcofig = m_stlconfig1[ncol];
		if (stcofig.nformat == 0)
			item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1)
			item.dwformat |= DT_CENTER;
		else
			item.dwformat |= DT_RIGHT;
		int ntmpcol = stcofig.nrealindex;
		item.clrfg = g_ColorRefData.GetColorBlack();

		item.b_top = true;
		item.b_bottom = false;
		item.b_left = false;
		item.b_right = true;

		UpdateListText(ntmpcol, pinfo, item);
		m_pgrid->SetItem(&item);
	}
}

void AccountBillListWnd::SetBuy1RowItem(_uint nrow, TRowInfo * pinfo)
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		memset(&item, 0, sizeof(GV_ITEM));
		item.cellid.ncol = ncol;
		item.cellid.nrow = nrow;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.nvmargin = 5;
		item.nhmargin = 3;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
		item.dwformat = DT_VCENTER;

		ConfigData stcofig = m_stlconfig1[ncol];
		if (stcofig.nformat == 0)
			item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1)
			item.dwformat |= DT_CENTER;
		else
			item.dwformat |= DT_RIGHT;
		int ntmpcol = stcofig.nrealindex;
		item.clrfg = g_ColorRefData.GetColorBlack();

		if(ncol>=0 && ncol<=3)
			item.b_top = false;
		else
			item.b_top = true;
		item.b_bottom = false;
		item.b_left = false;
		item.b_right = true;

		UpdateListText(ntmpcol, pinfo, item);
		m_pgrid->SetItem(&item);
	}
}

void AccountBillListWnd::SetBuy2RowItem(_uint nrow, TRowInfo * pinfo)
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		memset(&item, 0, sizeof(GV_ITEM));
		item.cellid.ncol = ncol;
		item.cellid.nrow = nrow;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.nvmargin = 5;
		item.nhmargin = 3;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
		item.dwformat = DT_VCENTER;

		ConfigData stcofig = m_stlconfig1[ncol];
		if (stcofig.nformat == 0)
			item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1)
			item.dwformat |= DT_CENTER;
		else
			item.dwformat |= DT_RIGHT;
		int ntmpcol = stcofig.nrealindex;
		item.clrfg = g_ColorRefData.GetColorBlack();


		item.b_top = false;
		item.b_bottom = false;
		item.b_left = false;
		item.b_right = true;

		UpdateListText(ntmpcol, pinfo, item);
		m_pgrid->SetItem(&item);
	}
}

void AccountBillListWnd::SetSell1RowItem(_uint nrow, TRowInfo * pinfo)
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		memset(&item, 0, sizeof(GV_ITEM));
		item.cellid.ncol = ncol;
		item.cellid.nrow = nrow;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.nvmargin = 5;
		item.nhmargin = 3;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
		item.dwformat = DT_VCENTER;

		ConfigData stcofig = m_stlconfig1[ncol];
		if (stcofig.nformat == 0)
			item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1)
			item.dwformat |= DT_CENTER;
		else
			item.dwformat |= DT_RIGHT;
		int ntmpcol = stcofig.nrealindex;
		item.clrfg = g_ColorRefData.GetColorBlack();

		if (ncol >= 5 && ncol <= 8)
			item.b_top = false;
		else
			item.b_top = true;
		item.b_bottom = false;
		item.b_left = false;
		item.b_right = true;

		UpdateListText(ntmpcol, pinfo, item);
		m_pgrid->SetItem(&item);
	}
}

void AccountBillListWnd::SetSell2RowItem(_uint nrow, TRowInfo * pinfo)
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		memset(&item, 0, sizeof(GV_ITEM));
		item.cellid.ncol = ncol;
		item.cellid.nrow = nrow;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.nvmargin = 5;
		item.nhmargin = 3;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
		item.dwformat = DT_VCENTER;

		ConfigData stcofig = m_stlconfig1[ncol];
		if (stcofig.nformat == 0)
			item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1)
			item.dwformat |= DT_CENTER;
		else
			item.dwformat |= DT_RIGHT;
		int ntmpcol = stcofig.nrealindex;
		item.clrfg = g_ColorRefData.GetColorBlack();


		item.b_top = false;
		item.b_bottom = false;
		item.b_left = false;
		item.b_right = true;

		UpdateListText(ntmpcol, pinfo, item);
		m_pgrid->SetItem(&item);
	}
}

void AccountBillListWnd::SetSubTotalRowItem(_uint nrow, TRowInfo * pinfo)
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		memset(&item, 0, sizeof(GV_ITEM));
		item.cellid.ncol = ncol;
		item.cellid.nrow = nrow;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.nvmargin = 5;
		item.nhmargin = 3;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
		item.dwformat = DT_VCENTER;

		ConfigData stcofig = m_stlconfig1[ncol];
		if (stcofig.nformat == 0)
			item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1)
			item.dwformat |= DT_CENTER;
		else
			item.dwformat |= DT_RIGHT;
		int ntmpcol = stcofig.nrealindex;
		item.clrfg = g_ColorRefData.GetColorBlack();


		item.b_top = true;
		item.b_bottom = false;
		item.b_left = false;
		if (ncol >= 0 && ncol <= 3)
			item.b_right = false;
		else if (ncol >= 5 && ncol <= 8)
			item.b_right = false;
		else if (ncol >= 10 && ncol <= 16)
			item.b_right = false;
		else 
			item.b_right = true;

		UpdateListText(ntmpcol, pinfo, item);
		m_pgrid->SetItem(&item);
	}
}

void AccountBillListWnd::SetSumTotalRowItem(_uint nrow, TRowInfo * pinfo)
{
	for (_uint ncol = 0; ncol < m_pgrid->GetColumnCount(); ncol++)
	{
		GV_ITEM item;
		memset(&item, 0, sizeof(GV_ITEM));
		item.cellid.ncol = ncol;
		item.cellid.nrow = nrow;
		item.lgfont = GetCurFont()/*g18_font*/;
		item.nvmargin = 5;
		item.nhmargin = 3;
		item.dwmask = GVIF_TEXT | GVIF_FONT | GVIF_FORMAT | GVIF_FGCLR | GVIF_VMARGIN | GVIF_HMARGIN;
		item.dwformat = DT_VCENTER;

		ConfigData stcofig = m_stlconfig1[ncol];
		if (stcofig.nformat == 0)
			item.dwformat |= DT_LEFT;
		else if (stcofig.nformat == 1)
			item.dwformat |= DT_CENTER;
		else
			item.dwformat |= DT_RIGHT;
		int ntmpcol = stcofig.nrealindex;
		item.clrfg = g_ColorRefData.GetColorBlack();


		item.b_top = true;
		item.b_bottom = true;
		item.b_left = false;
		if (ncol >= 0 && ncol <= 8)
			item.b_right = false;
		else if (ncol >= 10 && ncol <= 16)
			item.b_right = false;
		else
			item.b_right = true;

		UpdateListText(ntmpcol, pinfo, item);
		m_pgrid->SetItem(&item);
	}
}

LOGFONT AccountBillListWnd::GetCurFont()
{
	LOGFONT lf = { -13,0,0,0,FW_NORMAL ,0,0,0,ANSI_CHARSET ,OUT_DEFAULT_PRECIS ,OUT_DEFAULT_PRECIS ,DEFAULT_QUALITY ,DEFAULT_PITCH | FF_ROMAN,L"黑体" };
	if (m_emfont == Font_Middle)
		lf.lfHeight += 3;
	if (m_emfont == Font_Large)
		lf.lfHeight += 6;
	return lf;
}

void AccountBillListWnd::LoadConfig()
{
	int nsize = m_stlconfig0.size();
	std::vector<ConfigData>::iterator itor = m_stlconfig0.begin();

	for (int i = 0; i < AccountCol; i++)
	{
		ConfigData stconfig;
		stconfig.bshow = g_bColShow[i];
		stconfig.szcol = g_sTitleTxt0[i];
		stconfig.nformat = g_nColFormat[i];
		stconfig.sztype = "居*"; 
		stconfig.nrealindex = i;
		stconfig.ncolwidth = g_nTitleSize[i];
		stconfig.bignore = false;
		m_stlconfig0.push_back(stconfig);

		stconfig.szcol = g_sTitleTxt1[i];
		m_stlconfig1.push_back(stconfig);
	}
}
