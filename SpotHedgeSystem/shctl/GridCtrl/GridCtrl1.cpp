#include "PreInclude.h"

//
TGridCtrl::TGridCtrl()
{
	m_ncolumncount = 0;
	m_nrowcount = 0;
	m_nfixcolumn = 0;
	m_nfixrow = 0;
	m_nvirtualwidth = 0;
	m_npagecount = 0;
	m_bexpand = false;
	m_emstate = GridSort::Header_Normal;
	m_nsortcol = -1;
	m_nprecol = -1;

	m_bmulselect = false;
	m_bresizecol = true;
	m_bswapcol = true;
	m_nselectrow = -1;
	m_stmousemode = MouseMode_Normal;
	m_clrhighlight = RGB(71, 70, 70);

	memset(&m_scrollshow.rcvert, 0, sizeof(RECT));
	memset(&m_vertscroll, 0, sizeof(SCROLLINFO));
	memset(&m_horzscroll, 0, sizeof(SCROLLINFO));

	m_pfilterlist = NULL;
	m_bexpandfixcol = false;
	m_bheadershowid = false;
	m_bdragging = false;
	m_blockupdate = false;
	m_bexpandcheck = false;
	m_btrack = false;
}

TGridCtrl::~TGridCtrl()
{
	if (m_pfilterlist)
		delete m_pfilterlist;
	m_pfilterlist = NULL;

	DeleteObject(m_font);
}

//Property
bool TGridCtrl::Create(HWND hparent)
{
	CreateFrm(_T("TGridCtrl"), hparent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	SetFocus(m_Hwnd);

	m_swapcursor.Create(m_Hwnd);

	m_font = CreateFont(17, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Microsoft Yahei"/*TEXT("Microsoft Yahei")*/);

	m_pfilterlist = new TFilterList;
	m_pfilterlist->_Create(m_Hwnd);
	m_pfilterlist->SetFont(m_font);

	m_miniedit.Create(m_Hwnd);
	m_miniedit.UseOwnerProc(TGridEditCell::EditProc);
	m_miniedit.SetBackgroundColor(m_clrbk);
	m_miniedit.SetText(0,L"347.55");
	return true;
}

void TGridCtrl::MoveWindow(int x, int y, int cx, int cy)
{
	::MoveWindow(m_Hwnd, x, y, cx, cy, true);
}
//窗口函数
LRESULT TGridCtrl::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT: OnPaint(); break;
	case WM_SIZE:  OnSize(); break;
	case WM_LBUTTONDOWN: SetFocus(m_Hwnd); OnLButtonDown(wParam, lParam); break;
	case WM_LBUTTONUP: OnLButtonUp(wParam, lParam); break;
	case WM_RBUTTONDOWN: OnRButtonDown(wParam,lParam); break;
	case WM_LBUTTONDBLCLK: OnDbClickDown(wParam, lParam); break;
	case WM_MOUSEMOVE: OnMouseMove(wParam, lParam); break;
	case WM_MOUSELEAVE:OnMouseLeave(); break;
	case WM_MOUSEWHEEL: OnMouseWheel(wParam,lParam); break;
	case WM_SETCURSOR: OnSetCursor(); break;
	case WM_KEYDOWN: OnKeyDown(wParam, lParam);break;
	case SSWM_FILTER_EDIT_RETURN:
	case SSWM_FILTER_LIST_SELECT:
	case SSWM_TREE_EXPAND:
		PostMessage(GetParent(), message, wParam, lParam);
		break;
	case SSWM_GRID_MINIEDIT_MODITY:
	case SSWM_GRID_CELLEDIT_WHEEL:
		SendMessage(GetParent(), message, wParam, lParam);
		break;
	case SSWM_FILTER_COMBOX_SELECT:
		OnComboxSel(wParam, lParam);
		break;
	default:return NOT_PROCESSED;
	}

	return PROCESSED;
}

void TGridCtrl::OnPaint()
{
	if (m_blockupdate) return;
	RECT rect;
	GetClientRect(m_Hwnd,&rect);

	//资源
	HBRUSH bkbrush,bdframe;
	bkbrush = CreateSolidBrush(m_clrbk);
	bdframe = CreateSolidBrush(m_clrbd);

	TMemDC memdc(m_Hwnd);
	FillRect(memdc.GetHdc(), &rect, bkbrush);
	//绘制数据
	OnDraw(&memdc);

	//绘制滚动条
	bool bvert = IsVertVisable();
	bool bhorz = IsHorzVisable()&&!m_bexpand;
	if (bvert || bhorz)
	{
		HBRUSH scrollbrush;
		scrollbrush = CreateSolidBrush(RGB(232,232,236));
		if(bvert)
			FillRect(memdc.GetHdc(), &m_scrollshow.rcvert, scrollbrush);
		if(bhorz)
			FillRect(memdc.GetHdc(), &m_scrollshow.rchorz, scrollbrush);
		if(bvert&&bhorz)
			FillRect(memdc.GetHdc(), &m_scrollshow.rcunion, scrollbrush);
		DeleteObject(scrollbrush);

		RECT rcthumb;
		rcthumb = m_scrollshow.rcvert; /*InflateRect(&rcthumb, -2, 0);*/
		rcthumb.top = m_scrollshow.vertbegin; rcthumb.bottom = rcthumb.top + m_scrollshow.vertthumb;
		scrollbrush = CreateSolidBrush(RGB(194,195,201));
		if(bvert)
			FillRect(memdc.GetHdc(), &rcthumb, scrollbrush);

		rcthumb = m_scrollshow.rchorz; /*InflateRect(&rcthumb, 0, -2);*/
		rcthumb.left = m_scrollshow.horzbegin; rcthumb.right = rcthumb.left + m_scrollshow.horzthumb;
		if (bhorz)
			FillRect(memdc.GetHdc(), &rcthumb, scrollbrush);
		DeleteObject(scrollbrush);
	}

	//绘制边框
	FrameRect(memdc.GetHdc(), &rect, bdframe);

	//绘制Resizing分割线
	RECT rcsize; GetValidRect(rcsize);
 	_uint nvisablerow = min(GetMaxVisableCount(), (_uint)m_vertscroll.nMax - m_vertscroll.nPos);
 	rcsize.bottom = min(GetVirtualHeight(nvisablerow),(_uint)rcsize.bottom);
	if (MouseMode_Resizing == m_stmousemode)
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(m_Hwnd, &point);

		MoveToEx(memdc.GetHdc(), point.x, rcsize.top, 0);
		LineTo(memdc.GetHdc(), point.x, rcsize.bottom);
	}
	//释放资源
	DeleteObject(bkbrush);
	DeleteObject(bdframe);
}

void TGridCtrl::OnDraw(TMemDC* pmemdc)
{
	RECT rect;
	GetValidRect(rect);
	//数据
	DrawMain(pmemdc, rect);
}

void TGridCtrl::DrawMain(TMemDC* pmemdc, RECT& rect)
{
	int ntop = rect.top;
	RECT rctemp = rect;
	//可见行
	int nexpandrow = 0;
	_uint nvisablerow = min(GetMaxVisableCount(), (_uint)m_vertscroll.nMax);
	for (_uint nrow = 0; nrow < m_nfixrow&&nrow < (_uint)m_vertscroll.nMax; nrow++)
	{
		RECT rcrow = rctemp;
		rcrow.top = ntop;
		rcrow.bottom = ntop + m_nrowheight;
		DrawRow(pmemdc, rcrow, nrow);
		ntop = rcrow.bottom;
		nexpandrow++;
	}
	for (_uint nrow = m_vertscroll.nPos+m_nfixrow; nrow <m_vertscroll.nPos + nvisablerow&&nrow < (_uint)m_vertscroll.nMax; nrow++)
	{
		RECT rcrow = rctemp;
		rcrow.top = ntop;
		rcrow.bottom = ntop + m_nrowheight;
		DrawRow(pmemdc, rcrow,nrow);
		ntop = rcrow.bottom;
		nexpandrow++;
	}

	//绘制ExpandCol
	if (IsShowExpandCol())
	{
		RECT rcexpand;
		InflateRect(&rect, 0, 1);
		rcexpand = rect;
		rcexpand.left = 0;
		rcexpand.right = 1 + GetExpandWidth();
		DrawExpand(pmemdc, rcexpand, nexpandrow);
	}
}

void TGridCtrl::DrawRow(TMemDC* pmemdc, RECT& rcrow, int nrow)
{
	TCell cellid;
	int nleft,nfixwidth,nhorzscroll;

	//绘制非固定列
	cellid.nrow = nrow;
	nleft = rcrow.left;
	nfixwidth = GetFixColumnWidth();
	nleft = rcrow.left + nfixwidth;
	nhorzscroll = m_horzscroll.nPos;
	for (int ncol = m_nfixcolumn; ncol < (int)m_ncolumncount; ncol++)
	{
		RECT rccell = rcrow;
		int ncolwidth = GetColWidth(ncol);
		rccell.left = nleft - nhorzscroll;
		rccell.right = rccell.left + ncolwidth;

		if ((rccell.left < nfixwidth && rccell.right < nfixwidth)
			|| (rccell.left > rcrow.right&&rccell.right > rcrow.right))
		{
			nleft += ncolwidth;
			continue;
		}
		cellid.ncol = ncol;
		DrawCell(pmemdc, rccell,cellid);
		nleft += ncolwidth;
	}

	//绘制固定列
	nleft = rcrow.left;
	for (_uint ncol = 0; ncol < m_nfixcolumn; ncol++)
	{
		RECT rccell = rcrow;
		int npos = GetScrollPos(m_Hwnd, SB_VERT);
		int nwidth = GetColWidth(ncol);

		rccell.left = nleft;
		rccell.right = rccell.left + nwidth;

		cellid.ncol = ncol;
		DrawCell(pmemdc, rccell, cellid);
		nleft += nwidth;
	}
}

void TGridCtrl::DrawCell(TMemDC* pmemdc, RECT&rccell, const TCell& cellid)
{
	DWORD dwstate = ~GVIS_SELECTED;
	if (IsSelectRow(cellid.nrow))
		dwstate = GVIS_SELECTED;
	GV_ITEM* pitem;
	pitem = GetGridCell(cellid);
// #ifndef _DEBUG
// 	if (!pitem)
// 	{
// 		TGridCellBase cpcell;
// 		cpcell.SetItem(&cpitem);
// 		cpitem.clrbk = RGB(131, 131, 131);
// 		cpitem.dwstate = dwstate;
// 		cpcell.Draw(pmemdc, rccell);
// 		return;
// 	}
// #else
	if (!pitem) return;
//#endif
	switch (pitem->emtype)
	{
	case CellType_Base:
	{
		TGridCellBase cpcell;
		cpcell.SetItem(pitem);
		cpcell.SetHighColor(m_clrhighlight);
		cpcell.SetState(dwstate);
		cpcell.Draw(pmemdc, rccell);
		break;
	}
	case CellType_Header:
		break;
	case CellType_Edit:
	case CellType_Filter:
	case CellType_EditFilter:
	case CellType_EditFilter1:
	{
		TGridHeaderCell cpcell(m_pfilterlist);
		cpcell.SetItem(pitem);
		cpcell.SetHighColor(m_clrhighlight);
		cpcell.SetState(dwstate);
// 		if (m_nsortcol == cellid.ncol)
// 			cpcell.SetSortState(m_emstate);
		cpcell.Draw(pmemdc, rccell);
		break;
	}
	case CellType_Tree:
	{
		TGridTreeCell cpcell;
		cpcell.SetItem(pitem);
		cpcell.SetState(dwstate);
		cpcell.Draw(pmemdc, rccell);
		break;
	}
	case CellType_Commbox:
	{
		TGridComboxCell cpcell(m_pfilterlist);
		cpcell.SetItem(pitem);
		cpcell.SetState(dwstate);
		cpcell.Draw(pmemdc,rccell);
		break;
	}
	case CellType_CheckBox:
	{
		TGridCheckCell cpcell;
		cpcell.SetItem(pitem);
		cpcell.SetState(dwstate);
		cpcell.Draw(pmemdc, rccell);
		break;
	}
	case CellType_CellEdit:
	{
		TGridEditCell cpcell(&m_miniedit);
		cpcell.SetItem(pitem);
		cpcell.SetState(dwstate);
		cpcell.Draw(pmemdc, rccell);
		break;
	}
	default:break;
	}
}

void TGridCtrl::DrawExpand(TMemDC* pmemdc, RECT rect, int nvisablerow)
{
	HBRUSH brbk,brframe;
	brbk = CreateSolidBrush(RGB(165, 165, 165));
	brframe = CreateSolidBrush(RGB(243, 243, 243));
	rect.bottom = rect.top + nvisablerow * m_nrowheight + 1;
	FillRect(pmemdc->GetHdc(), &rect, brframe);
	FrameRect(pmemdc->GetHdc(), &rect, brbk);
	DeleteObject(brbk);
	DeleteObject(brframe);
	m_rcexpandfixcol = rect;

 	HPEN hpen;
 	int ntop = rect.top;
 	RECT rcexpand = rect;
 	hpen = CreatePen(PS_SOLID, 1, RGB(135, 135, 135));

	int nbegincol = 1;
	if (m_bheadershowid)
		nbegincol = 0;

	RECT rccheck = rect;
	HDC htempdc = CreateCompatibleDC(pmemdc->GetDVdc());
	for (int i = nbegincol; i < nvisablerow/* - 1*/; i++)
	{
		SelectObject(pmemdc->GetHdc(), hpen);
		ntop = rect.top + i * 25; rcexpand.top = ntop;
		MoveToEx(pmemdc->GetHdc(), rect.left, ntop, NULL);
		LineTo(pmemdc->GetHdc(), rect.right, ntop);
		rcexpand.bottom = ntop + 25;

		int npngleft = rcexpand.left;
		if (m_bexpandcheck)
		{
			GetExpandCheck(rcexpand, rccheck);
			npngleft = rccheck.right;
		}
		//if (m_nid != -1)
		//{
		//	Graphics gc(pmemdc->GetHdc());
		//	int nleftmargin = 12, ntopmargin = 5;
		//	pmemdc->DrawPng(&gc, m_nid, npngleft + nleftmargin, rcexpand.top + ntopmargin/*5*/);
		//}

		if (m_bexpandcheck)
		{
			int ntoppos = GetVertPos();
			int nrow = i + ntoppos;
			MCheck::iterator itor = m_stlcheck.find(nrow);
			if(itor == m_stlcheck.end()) continue;

			DrawCheck(pmemdc, rccheck, itor->second);
		}
	}

	DeleteObject(hpen);
	DeleteDC(htempdc);
}

void TGridCtrl::DrawCheck(TMemDC* pmdc, RECT& rccheck, bool bvalue)
{
	HBRUSH holdbr = (HBRUSH)SelectObject(pmdc->GetHdc(), GetStockObject(NULL_BRUSH));
	Rectangle(pmdc->GetHdc(), rccheck.left, rccheck.top, rccheck.right, rccheck.bottom);

	if (bvalue)
	{
		HPEN hpen = CreatePen(PS_SOLID, 1, RGB(85, 85, 85));
		InflateRect(&rccheck, -1, -2);
		SelectObject(pmdc->GetHdc(), hpen);
		POINT pp[3];
		pp[0].x = rccheck.left;
		pp[0].y = rccheck.top + (rccheck.bottom - rccheck.top) / 2 - 1;
		pp[1].x = pp[0].x + 4;
		pp[1].y = pp[0].y + 5;
		pp[2].x = pp[1].x + 7;// rect.right - 1;
		pp[2].y = pp[1].y - 8;//rect.top + 2;

		MoveToEx(pmdc->GetHdc(), pp[0].x + 1, pp[0].y + 1, NULL);
		LineTo(pmdc->GetHdc(), pp[1].x + 1, pp[1].y);
		MoveToEx(pmdc->GetHdc(), pp[1].x, pp[1].y - 1, NULL);
		LineTo(pmdc->GetHdc(), pp[2].x, pp[2].y);

		InflateRect(&rccheck, 2, 2);
		DeleteObject(hpen);
	}

	SelectObject(pmdc->GetHdc(), holdbr);
}

void TGridCtrl::OnSize()
{
	RefreshScrollBar();
	m_stgriddata.InitGridData(m_ncolumncount, GetMaxVisableCount());
}

void TGridCtrl::OnLButtonDown(WPARAM wparam, LPARAM lparam)
{
	POINT pt;
	pt.x = (int)(short)LOWORD(lparam);
	pt.y = (int)(short)HIWORD(lparam);

	//ExpandCol
	if (IsShowExpandCol())
		ClickExpandCol(pt);
	//Drag
	RECT vertthumb = m_scrollshow.rcvert;
	RECT horzthumb = m_scrollshow.rchorz;
	vertthumb.top = m_scrollshow.vertbegin;
	vertthumb.bottom = vertthumb.top + m_scrollshow.vertthumb;
	horzthumb.left = m_scrollshow.horzbegin;
	horzthumb.right = horzthumb.left + m_scrollshow.horzthumb;
	if (IsVertVisable()&&PtInRect(&vertthumb,pt))
	{
		SetCapture(m_Hwnd);
		m_dragoffset.x = pt.x - (_Width(vertthumb) / 2 + vertthumb.left);
		m_dragoffset.y = pt.y - (_Height(vertthumb) / 2 + vertthumb.top);
		m_bdragging = true;
		m_dragbar = GSB_VERT;
	}
	if (IsHorzVisable() && PtInRect(&horzthumb, pt))
	{
		SetCapture(m_Hwnd);
		m_dragoffset.x = pt.x - (_Width(horzthumb) / 2 + horzthumb.left);
		m_dragoffset.y = pt.y - (_Height(horzthumb) / 2 + horzthumb.top);
		m_bdragging = true;
		m_dragbar = GSB_HORZ;
	}

	if (MouseMode_ResizeCol == m_stmousemode)
	{
		m_stmousemode = MouseMode_Resizing;
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		m_lastclickcell = m_hovercell;
	}
	else if (MouseMode_Normal == m_stmousemode)
	{
		m_lastmousept = pt;
		m_swapsrcclick = GetCellFromPt(pt);
		m_blbtndown = true;

		//点击非GridCellBase的单元格
		bool bret = false;
		GV_ITEM* pgvitem = GetGridCell(m_swapsrcclick);
		if (pgvitem&&pgvitem->emtype != CellType_Base)
		{
			RECT rccell;
			GetCellRect(m_swapsrcclick, rccell);
			if(m_swapsrcclick.ncol >= m_nfixcolumn)
				OffsetRect(&rccell, -m_horzscroll.nPos, 0);
			if (m_swapsrcclick.nrow >= m_nfixrow)
				OffsetRect(&rccell, 0, -m_vertscroll.nPos*m_nrowheight);
			switch (pgvitem->emtype)
			{
			case CellType_Tree:
			{
				TGridTreeCell stcell;
				stcell.SetItem(pgvitem);
				bret = stcell.ClickCell(rccell, pt,m_Hwnd);
				break;
			}
			case CellType_Header:
				break;
			case CellType_Edit:
			case CellType_Filter:
			case CellType_EditFilter:
			case CellType_EditFilter1:
			{
				TGridHeaderCell stcell(m_pfilterlist);
				stcell.SetItem(pgvitem);
				bret = stcell.ClickCell(rccell, pt, m_Hwnd);
				break;
			}
			case CellType_CheckBox:
			{
				TGridCheckCell stcell;
				stcell.SetItem(pgvitem);
				bret = stcell.ClickCell(rccell, pt, m_Hwnd);
				break;
			}
			case CellType_Commbox:
			{
				TGridComboxCell stcell(m_pfilterlist);
				stcell.SetItem(pgvitem);
				bret = stcell.ClickCell(rccell, pt, m_Hwnd);
				break;
			}
			case CellType_CellEdit:
			{
				TGridEditCell stcell(&m_miniedit);
				stcell.SetItem(pgvitem);
				m_miniedit.SetText(0,pgvitem->szparam);
				bret = stcell.ClickCell(rccell, pt, m_Hwnd);
				break;
			}
			default:break;
			}//end switch
		}
		if(bret)
			m_blbtndown = false;
	}
	RefreshGrid();
}

void TGridCtrl::OnMouseMove(WPARAM wparam, LPARAM lparam)
{
	POINT pt;
	pt.x = (int)(short)LOWORD(lparam);
	pt.y = (int)(short)HIWORD(lparam);

	if (!m_btrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
	}

	RECT rect;
	GetValidRect(rect);

	m_hovercell.ncol = -1;m_hovercell.nrow = -1;

	//Expand Col
	if (IsShowExpandCol())
		MoveOverExpandCol(pt);

	// drag thumb
	if (m_bdragging)
	{
		POINT ptdrag = pt;
		ptdrag.x += -m_dragoffset.x;
		ptdrag.y += -m_dragoffset.y;
		ScrollThumb(ptdrag, m_dragbar);
	}

	if (PtInRect(&rect, pt))
	{
		TCell cellid = GetCellFromPt(pt);
		m_hovercell = cellid;

		if (IsValidCell(cellid) && (_uint)cellid.nrow < m_nfixrow && GetResizeColumn(pt, m_hovercell.ncol))
		{
			if (m_stmousemode == MouseMode_Normal && m_bresizecol)
			{
				m_stmousemode = MouseMode_ResizeCol;
				::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
			}
		}
		else if (m_stmousemode == MouseMode_ResizeCol)
		{
			m_stmousemode = MouseMode_Normal;
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		}
		else if (m_stmousemode == MouseMode_Normal
				&& m_blbtndown && m_bswapcol && (_uint)m_swapsrcclick.ncol >= m_nfixcolumn
				&& (_uint)m_swapsrcclick.nrow < m_nfixrow && abs(m_lastmousept.x - pt.x) > 2)
		{
			SetCapture(m_Hwnd);
			HBITMAP hbmp;
			m_stmousemode = MouseMode_SwapCol;
			hbmp = CreateAlphaCursor(m_swapsrcclick);
			m_swapcursor.SetBitmap(hbmp);
		} 
 		::SendMessage(GetParent(), SSWM_GRIDCTRL_HOVER, 0, (LPARAM)&m_hovercell);
	}

	if (m_stmousemode == MouseMode_SwapCol)
	{
		POINT pttemp;
		GetCursorPos(&pttemp);
		pttemp.y += 5; pttemp.x += 5;
		m_swapcursor.SetBitmap(NULL, &pttemp);
		m_swapcursor.ShowWindow(SW_SHOW);
	}

//	if (m_stmousemode == MouseMode_Resizing|| m_hovercell.nrow == 0)
	RefreshGrid();
}

void TGridCtrl::OnMouseLeave()
{
	PostMessage(GetParent(), SSWM_GRIDLIST_EXPAND_HOVER, 0, 0);
}

void TGridCtrl::OnLButtonUp(WPARAM wparam, LPARAM lparam)
{
	POINT pt;
	pt.x = (int)(short)LOWORD(lparam);
	pt.y = (int)(short)HIWORD(lparam);

	//重置部分状态值
	m_hovercell.ncol = -1; m_hovercell.nrow = -1;

	if (MouseMode_Normal == m_stmousemode/*&&m_blbtndown*/)
	{
		int nfirstshift = -1;
		GV_CLKINFO clkinfo;
		clkinfo.naction = GVA_LCLK;
		clkinfo.cellid = GetCellFromPt(pt);

		if (!m_bmulselect || !((wparam &MK_CONTROL) || (wparam &MK_SHIFT)))
			m_stlselect.clear();
		else if (wparam &MK_SHIFT)
		{
			if (m_stlselect.size() > 0)
			{
				nfirstshift = m_stlselect.at(0);
				m_stlselect.clear(); AddSelectRow(nfirstshift);
			}
			else
				nfirstshift = -1;
		}

		if ((_uint)clkinfo.cellid.nrow >= m_nfixrow)
		{
			m_nselectrow = clkinfo.cellid.nrow;
			if (!IsSelectRow(m_nselectrow))
			{
				if (!(wparam &MK_SHIFT) || nfirstshift == -1)
					AddSelectRow(m_nselectrow);
				else//MK_SHIFT
				{
					if (m_nselectrow > nfirstshift)
					{
						for (int nindex = nfirstshift + 1; nindex <= m_nselectrow; nindex++)
							AddSelectRow(nindex);
					}
					else
					{
						for (int nindex = nfirstshift - 1; nindex >= m_nselectrow; nindex--)
							AddSelectRow(nindex);
					}
				}
			}
		}

		if(m_blbtndown||clkinfo.cellid.nrow!=0)SendMessage(GetParent(), SSWM_GRID_CTRL_ACTION, 0, (LPARAM)&clkinfo);
	}
	else if (m_stmousemode == MouseMode_Resizing)
	{
		int nhpos = m_horzscroll.nPos;//GetScrollPos(m_Hwnd, SB_HORZ);
		nhpos = max(nhpos, 0);
		pt.x += nhpos;

		POINT start;
		if (!GetCellOriginPt(m_lastclickcell, start))
			return;
		if (start.x != pt.x && (pt.x != 0 || pt.y != 0))
		{
			int ncolwidth = max(pt.x - start.x, 1);

			ModifyColumnWidth(m_lastclickcell.ncol, ncolwidth);
			RefreshScrollInfo();
			StandardScrollInfo(GSB_HORZ);
			RefreshScrollRect();
			PostMessage(GetParent(), SSWM_GRIDCTRL_COL_WIDTH, m_lastclickcell.ncol, ncolwidth);
		}
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	else if (MouseMode_SwapCol == m_stmousemode)
	{
		TCell cellid = GetCellFromPt(pt);
		ReleaseCapture();
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		m_swapcursor.ShowWindow(SW_HIDE);
		if ((_uint)m_swapsrcclick.nrow < m_nfixrow
			&& (_uint)cellid.nrow < m_nfixrow && (_uint)cellid.ncol >= m_nfixcolumn)
			SwapColumn(m_swapsrcclick.ncol, cellid.ncol);
 			PostMessage(GetParent(), SSWM_GRIDCTRL_SWAP_COL, m_swapsrcclick.ncol, cellid.ncol);
	}

	if (m_bdragging)
	{
		POINT ptdrag = pt;
		ptdrag.x += -m_dragoffset.x;
		ptdrag.y += -m_dragoffset.y;
		ScrollThumb(ptdrag, m_dragbar);
		ReleaseCapture();
	}

	m_bdragging = false;
	m_blbtndown = false;
	m_dragbar = GSB_NULL;
	m_stmousemode = MouseMode_Normal;
	RefreshGrid();
}

void TGridCtrl::OnRButtonDown(WPARAM wparam, LPARAM lparam)
{
	POINT pt;
	pt.x = (int)(short)LOWORD(lparam);
	pt.y = (int)(short)HIWORD(lparam);

	TCell cellid = GetCellFromPt(pt);

	GV_CLKINFO clkinfo;
	clkinfo.cellid = cellid;
	clkinfo.naction = GVA_RCLK;

	if ((_uint)clkinfo.cellid.nrow >= m_nfixrow && !IsSelectRow(cellid.nrow))
	{
		if (!m_bmulselect || !(wparam &MK_CONTROL))
		{
			m_stlselect.clear();
			m_nselectrow = cellid.nrow;
		}
		else
			clkinfo.cellid.nrow = m_nselectrow;
		if (!IsSelectRow(m_nselectrow))
			AddSelectRow(m_nselectrow);
	}
	SendMessage(GetParent(), SSWM_GRID_CTRL_ACTION, 0, (LPARAM)&clkinfo);
	RefreshGrid();
}

void TGridCtrl::OnDbClickDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	GetCursorPos(&pt);
	ClientToScreen(m_Hwnd, &pt); // 转屏幕座标

	TCell cellid = GetCellFromPt(pt);

	if (IsValidCell(cellid))
	{
		GV_CLKINFO clkinfo;
		clkinfo.cellid = cellid;
		clkinfo.naction = GVA_DBCLK;

		m_nselectrow = cellid.nrow;
		InvalidateRect(m_Hwnd, 0, true);
		SendMessage(GetParent(), SSWM_GRID_CTRL_ACTION, 0, (LPARAM)&clkinfo);
	}
}

void TGridCtrl::OnSetCursor()
{
	if (MouseMode_ResizeCol == m_stmousemode 
		|| MouseMode_Resizing == m_stmousemode)
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
	else if (MouseMode_Normal == m_stmousemode)
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

void TGridCtrl::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	bool brefreshdata = false;
	_uint ukey = (_uint)wParam;
	if (ukey == VK_DOWN || ukey == VK_UP)//按上下键
	{
		m_stlselect.clear();
		if (m_nselectrow < 0 || (_uint)m_nselectrow >= m_nrowcount)
			m_nselectrow = 0;
		brefreshdata = true;
	}

	switch (ukey)
	{
	case VK_DOWN:
		m_nselectrow++;
		break;
	case VK_UP:
		m_nselectrow--;
		break;
	case VK_LEFT:
		if (!m_bexpand&&IsHorzVisable())
			m_horzscroll.nPos -= 10;
		else if (m_bexpand)
			::PostMessage(GetParent(), SSWM_AUTO_HSCROLL, ukey, 0);
		StandardScrollInfo(GSB_HORZ);
		break;
	case VK_RIGHT:
		if (!m_bexpand&&IsHorzVisable())
			m_horzscroll.nPos +=10;
		else if (m_bexpand)
			::PostMessage(GetParent(), SSWM_AUTO_HSCROLL, ukey,0);
		StandardScrollInfo(GSB_HORZ);
		break;
	}

	if (brefreshdata)
	{
		if ((_uint)m_nselectrow <= m_nfixrow-1)
			m_nselectrow = m_nfixrow;
		else if ((_uint)m_nselectrow >= m_nrowcount)
			m_nselectrow = m_nrowcount - 1;
		AddSelectRow(m_nselectrow);
		int npos = m_vertscroll.nPos;
		if ((_uint)m_nselectrow <= npos+m_nfixrow)
		{
			m_vertscroll.nPos = m_nselectrow - m_nfixrow;
			StandardScrollInfo(GSB_VERT);
			NotifyDispData();
		}
		else if ((_uint)m_nselectrow >= npos + m_vertscroll.nPage)
		{
			m_vertscroll.nPos = m_nselectrow - m_vertscroll.nPage+1;
			StandardScrollInfo(GSB_VERT);
			NotifyDispData();
		}
	}

	RefreshScrollRect();
	RefreshGrid();
}

void TGridCtrl::OnMouseWheel(WPARAM wparam, LPARAM lparam)//
{
	int ncount = m_vertscroll.nMax;
	int nvisablerow = GetPageCount();
	if (ncount <= nvisablerow)return;
	short nsize = (short)HIWORD(wparam);

	if (nsize < 0)
		m_vertscroll.nPos++;
	else if (nsize > 0)
		m_vertscroll.nPos--;
	else
		return;

	if (m_vertscroll.nPos < 0)
		m_vertscroll.nPos = 0;
	else if (m_vertscroll.nPos + nvisablerow >= ncount)
		m_vertscroll.nPos = ncount - nvisablerow;

	RefreshScrollRect();
	NotifyDispData();
	RefreshGrid();
}

void TGridCtrl::OnComboxSel(WPARAM wparam, LPARAM lparam)
{
	//m_pfilterlist->ShowWindow(SW_HIDE);

	//int nselect = (int)wparam;
	//TCell* pcell = (TCell*)lparam;

	//VFilter* vfilter;
	//GV_ITEM* pitem = GetGridCell(*pcell);
	//vfilter = (VFilter*)pitem->exdata;
	//if (vfilter == NULL || pitem == NULL) return;
	//
 //	GFilter stdata;
	//if (nselect >= vfilter->size())
	//	return;
	//memset(&stdata, 0, sizeof(GFilter));
	//stdata = vfilter->at(nselect);

	//strncpy_s(pitem->szparam,sizeof(pitem->szparam), stdata.szvalue,_TRUNCATE);
	//SendMessage(GetParent(), SSWM_GRIDCELL_COMBOX, (WPARAM)pcell, (LPARAM)stdata.szvalue);
	//OnPaint();
}

/**********************************绘制**************************************/
void TGridCtrl::SetBkColor(COLORREF clrbk)
{
	m_clrbk = clrbk;
	//m_miniedit.SetBackgroundColor(clrbk);
}

void TGridCtrl::SetBdColor(COLORREF clrbd)
{
	m_clrbd = clrbd;
}

void TGridCtrl::SetGridSort(int ncol, GridSort emsort)
{ 
	GV_ITEM* pitem;
	m_emstate = emsort;
	m_nprecol = m_nsortcol;
	m_nsortcol = ncol;
	pitem = m_stgriddata.GetGridCell(m_nprecol, 0);
	if (pitem&&pitem->emsort != Header_Null)
		pitem->emsort = Header_Normal;
	pitem = m_stgriddata.GetGridCell(m_nsortcol, 0);
	if(pitem&&pitem->emsort != Header_Null)
		pitem->emsort = emsort;
}

void TGridCtrl::SetColumnCount(int ncolcount)
{
	m_ncolumncount = ncolcount;
	m_vcolwidth.resize(ncolcount);
	m_stgriddata.InitGridData(m_ncolumncount, GetMaxVisableCount());
}

void TGridCtrl::SetRowCount(int nrowcount)
{
	m_nrowcount = nrowcount;
	m_stgriddata.InitGridData(m_ncolumncount, GetMaxVisableCount());
	RefreshScrollBar();
	RefreshGrid();
}

void TGridCtrl::SetColWidth(int ncol, int nwidth)
{
	if ((_uint)ncol >= m_ncolumncount) return;
	m_vcolwidth[ncol] = nwidth;
	RefreshScrollInfo();
}

_uint TGridCtrl::GetVirtualWidth(int ncolcount)
{
	_uint nvirtual = 0;
	for (_uint ncol = 0; ncol < ncolcount; ncol++)
	{
		_uint nwidth = GetColWidth(ncol);
		nvirtual += nwidth;
	}
	return nvirtual;
}

_uint TGridCtrl::GetVirtualHeight(int nrowcount)
{
	return (nrowcount*m_nrowheight);
}

_uint TGridCtrl::GetPageCount()
{
	_uint npagecount = m_vertscroll.nPage;
	return npagecount;
}

TCell TGridCtrl::GetTopLeftNonFixCell()
{
	TCell cellid;
	return cellid;
}

void TGridCtrl::RefreshScrollBar()
{
	ResizeScrollPage();
	RefreshScrollInfo();
	RefreshScrollRect();
}

void TGridCtrl::ResizeScrollPage()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	//去掉边框大小
	InflateRect(&rect, -1, -1);

	int nheight = _Height(rect);
	if (GetVirtualWidth(m_ncolumncount) > (_uint)_Width(rect) && !m_bexpand)
		nheight -= HorzScroll_Height;
	m_npagecount = (_uint)(nheight / (m_nrowheight + 0.0));

	//恢复大小
	InflateRect(&rect, 1, 1);
}

void TGridCtrl::StandardScrollInfo(GScrollBar embar)
{
	switch (embar)
	{
	case GSB_VERT:
		if (!IsVertVisable())
			memset(&m_vertscroll, 0, sizeof(SCROLLINFO));
		else
		{
			m_vertscroll.nPos = max(0, m_vertscroll.nPos);
			m_vertscroll.nPos = min(m_vertscroll.nMax - m_vertscroll.nPage, (_uint)m_vertscroll.nPos);
		}
		break;
	case GSB_HORZ:
		if (!IsHorzVisable())
			memset(&m_horzscroll, 0, sizeof(SCROLLINFO));
		else
		{
			m_horzscroll.nPos = max(0, m_horzscroll.nPos);
			m_horzscroll.nPos = min(m_horzscroll.nMax - m_horzscroll.nPage, (_uint)m_horzscroll.nPos);
		}
		break;
	}
}
void TGridCtrl::RefreshScrollInfo()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	InflateRect(&rect, -1, -1);
	//竖向滚动条保存数据
	m_vertscroll.nMax = m_nrowcount/* - m_npagecount*/;
	m_vertscroll.nPage = m_npagecount;

	//横向滚动条数据
	m_horzscroll.nMax = GetVirtualWidth(m_ncolumncount) - GetFixColumnWidth() + m_horzbordergap;
	m_horzscroll.nPage = _Width(rect) - GetFixColumnWidth();

	if (IsShowExpandCol())
	{

		m_horzscroll.nPage -= GetExpandWidth();
	}
	if (m_nrowcount > m_npagecount)
		m_horzscroll.nPage -= VertScroll_Width;
	InflateRect(&rect, 1, 1);
}

_uint TGridCtrl::GetColWidth(int ncol)
{
	if ((_uint)ncol >= m_vcolwidth.size()) return 0;
	return m_vcolwidth[ncol];
}

void TGridCtrl::GetValidRect(RECT& rect)
{
	RECT rctemp;
	GetClientRect(m_Hwnd, &rctemp);

	//去掉边框大小
	InflateRect(&rctemp, -1, -1);

	//竖向滚动条
	if (IsVertVisable())
		rctemp.right -= VertScroll_Width;
	//横向滚动条  是否使用外部滚动条
	if (!m_bexpand&&IsHorzVisable())
		rctemp.bottom -= HorzScroll_Height;
	rect = rctemp;

	//zxw
	if(IsShowExpandCol())
		rect.left += GetExpandWidth();
	//恢复大小
	InflateRect(&rctemp, 1, 1);
}

void TGridCtrl::SetHorzInfo(SCROLLINFO& stinfo)
{
	m_horzscroll = stinfo;
	m_horzscroll.nPos = max(m_horzscroll.nPos, 0);
	InvalidateRect(m_Hwnd, 0, TRUE);
}

void TGridCtrl::GetHorzInfo(LPSCROLLINFO lpinfo)
{
	*lpinfo = m_horzscroll;
}

_uint TGridCtrl::GetVertPos()
{
	if (m_vertscroll.nPos < 0 || m_vertscroll.nPos > m_vertscroll.nMax || !IsVertVisable())
		m_vertscroll.nPos = 0;
	return m_vertscroll.nPos;
}

void TGridCtrl::SetMoveVertPos(_uint nrange)
{
	if (m_vertscroll.nPos <= m_vertscroll.nMax && IsVertVisable())
	{
		m_vertscroll.nPos += nrange;
		m_vertscroll.nPos = min(m_vertscroll.nMax, m_vertscroll.nPos);
		m_vertscroll.nPos = max(0, m_vertscroll.nPos);
		InvalidateRect(m_Hwnd, 0, false);
	}
}

_uint TGridCtrl::GetMaxVisableCount()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	//去掉边框大小
	InflateRect(&rect, -1, -1);

	if (m_npagecount*m_nrowheight < (_uint)_Height(rect))
		return (m_npagecount + 1);
	return m_npagecount;
}

void TGridCtrl::RefreshScrollRect()
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	bool bvert = IsVertVisable();
	bool bhorz = IsHorzVisable() && !m_bexpand;

	if (bvert || bhorz)
	{
		m_scrollshow.rcvert = rect;
		InflateRect(&m_scrollshow.rcvert, -1, -1);
		m_scrollshow.rcvert.left = m_scrollshow.rcvert.right - VertScroll_Width;

		m_scrollshow.rchorz = rect;
		InflateRect(&m_scrollshow.rchorz, -1, -1);
		m_scrollshow.rchorz.top = m_scrollshow.rchorz.bottom - HorzScroll_Height;

		if (bvert&&bhorz)
		{
			m_scrollshow.rcunion = m_scrollshow.rcvert;
			m_scrollshow.rcunion.top = m_scrollshow.rchorz.top;
			m_scrollshow.rcvert.bottom -= HorzScroll_Height;
			m_scrollshow.rchorz.right -= VertScroll_Width;
		}
		m_scrollshow.vertbegin = (_uint)((m_vertscroll.nPos + 0.0) / m_vertscroll.nMax * (_Height(m_scrollshow.rcvert) - 12));
		m_scrollshow.vertthumb = (_uint)((m_vertscroll.nPage + 0.0) / m_vertscroll.nMax * (_Height(m_scrollshow.rcvert) - 12) + 12);
		if (m_vertscroll.nPos + m_vertscroll.nPage == m_vertscroll.nMax)
			m_scrollshow.vertbegin = m_scrollshow.rcvert.bottom - m_scrollshow.vertthumb;

		m_scrollshow.horzbegin = (_uint)((m_horzscroll.nPos + 0.0) / m_horzscroll.nMax * _Width(m_scrollshow.rchorz));
		m_scrollshow.horzthumb = (_uint)((m_horzscroll.nPage + 0.0) / m_horzscroll.nMax * _Width(m_scrollshow.rchorz));
		if (m_horzscroll.nPos + m_horzscroll.nPage == m_horzscroll.nMax)
			m_scrollshow.horzbegin = m_scrollshow.rchorz.right - m_scrollshow.horzthumb;
	}
}

void TGridCtrl::ScrollThumb(POINT pt, GScrollBar embar)
{
	if (embar == GSB_VERT)
	{
		double dbpos;
		pt.y -= m_scrollshow.vertthumb/2;
		m_vertscroll.nPos = dbpos = pt.y * (m_vertscroll.nMax - m_vertscroll.nMin) / (_Height(m_scrollshow.rcvert) - 12 + 0.000) + m_vertscroll.nMin;
		if (dbpos - m_vertscroll.nPos > 0.5)
			m_vertscroll.nPos++;
		StandardScrollInfo(embar);
		RefreshScrollRect();
		NotifyDispData();
	}
	if(embar == GSB_HORZ)
	{
		pt.x -= m_scrollshow.horzthumb / 2;
		m_horzscroll.nPos = pt.x * (m_horzscroll.nMax - m_horzscroll.nMin) / (_Width(m_scrollshow.rchorz) + 0.000) + m_horzscroll.nMin;
		StandardScrollInfo(embar);
		RefreshScrollRect();
	}
}

//判断
bool TGridCtrl::IsVertVisable()
{
	if ((int)m_vertscroll.nPage < m_vertscroll.nMax)
		m_bvertvisable = true;
	else
		m_bvertvisable = false;
	return m_bvertvisable;
}

bool TGridCtrl::IsHorzVisable()
{
	if ((int)m_horzscroll.nPage < m_horzscroll.nMax)
		m_bhorzvisable = true;
	else
		m_bhorzvisable = false;
	return m_bhorzvisable;
}
/*********************************************调整列宽(固定列不参与调整)******************************************/
bool TGridCtrl::GetResizeColumn(const POINT& pt, int& nresizingcol)
{
	POINT ptmp = pt;
	int npos = m_horzscroll.nPos; //GetScrollPos(m_Hwnd, SB_HORZ);
	nresizingcol = -1;
	ptmp.x += npos;

	int nwidth = GetFixColumnWidth();
	if (IsShowExpandCol())nwidth += GetExpandWidth();
	for (_uint ncol = m_nfixcolumn; ncol < m_ncolumncount; ncol++)
	{
		nwidth += GetColWidth(ncol);
		if (nwidth - 2 < ptmp.x&&nwidth + 2 > ptmp.x)
		{
			nresizingcol = ncol;
			return true;
		}
	}

	return false;
}

void TGridCtrl::ModifyColumnWidth(const _uint ncol, const _uint nwidth)
{
	if (ncol >= m_vcolwidth.size()) return;
	m_vcolwidth[ncol] = nwidth;
}
/*********************************************交换列(固定列不参与调整)********************************************/
HBITMAP TGridCtrl::CreateAlphaCursor(TCell cellid)
{
	RECT rect;
	DWORD dwWidth, dwHeight;
	GetCellRect(cellid, rect);
	dwWidth = _Width(rect) - 1;
	dwHeight = _Height(rect) - 1;

	HDC hdc, memdc;
	hdc = GetDC(m_Hwnd);
	HBITMAP hBitmap;

	memdc = CreateCompatibleDC(hdc);
	PrintWindow(m_Hwnd, hdc, PW_CLIENTONLY);

	void *lpBits;
	BITMAPV5HEADER bi;
	memset(&bi,0, sizeof(BITMAPV5HEADER));
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = dwWidth;
	bi.bV5Height = dwHeight;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	bi.bV5AlphaMask = 0x5F000000;

	hBitmap = CreateDIBSection(memdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
		(void **)&lpBits, NULL, (DWORD)0);

	::SelectObject(memdc, hBitmap);
	::BitBlt(memdc, 0, 0, dwWidth, dwHeight, hdc, rect.left - m_horzscroll.nPos + 1, rect.top + 1, SRCCOPY);

	DWORD *lpdwPixel;
	lpdwPixel = (DWORD *)lpBits;
	int x, y;
	for (x = 0; x < (int)dwWidth; x++)
		for (y = 0; y < (int)dwHeight; y++)
		{
			*lpdwPixel &= 0x00FFFFFF;
			*lpdwPixel |= 0xCF000000;
			lpdwPixel++;
		}

	ReleaseDC(m_Hwnd, hdc);
	DeleteDC(memdc);

	return hBitmap;
}

void TGridCtrl::SwapColumn(_uint nsrc, _uint ndes)
{
	RefreshGrid();
}
/*********************************************固定行固定列********************************************************/
void TGridCtrl::SetFixRow(int nfix)
{
	m_nfixrow = nfix;
}

void TGridCtrl::SetFixColumn(int nfix)
{
	m_nfixcolumn = nfix;
}

_uint TGridCtrl::GetFixColumnWidth()
{
	return GetVirtualWidth(m_nfixcolumn);
}
/*********************************************MouseMode相关*******************************************************/
_uint TGridCtrl::GetColFromPt(const POINT& pt)
{
	int nwidth = 0;
	if (m_vcolwidth.size() <= 0) return -1;
	if (IsShowExpandCol()) nwidth += GetExpandWidth();

	_uint ncol = 0;
	_uint noffset = m_horzscroll.nPos;
	for (_uint ncol = 0; ncol < m_vcolwidth.size(); ncol++)
	{
		int ncolwidth = GetColWidth(ncol);
		if (ncol < m_nfixcolumn && pt.x > nwidth
			&& pt.x <= nwidth + ncolwidth)
			return ncol;
		if (ncol >= m_nfixcolumn&& pt.x > (int)(nwidth - noffset)
			&& pt.x <= (int)(nwidth + ncolwidth - noffset))
			return ncol;
		nwidth += ncolwidth;
	}

	return -1;
}

TCell TGridCtrl::GetCellFromPt(const POINT& pt)
{
	RECT rect;
	TCell cellid;
	GetValidRect(rect);
	if (PtInRect(&rect, pt))
	{
		int nheight = pt.y - rect.top;
		int noffsetrow = nheight / m_nrowheight;
		if ((_uint)noffsetrow < m_nfixrow)
			cellid.nrow = noffsetrow;
		else
			cellid.nrow = noffsetrow + m_vertscroll.nPos;

		cellid.ncol = GetColFromPt(pt);
	}

	IsValidCell(cellid);
	return cellid;
}

bool TGridCtrl::GetCellOriginPt(const TCell& cellid, POINT& pt)
{
	int nwidth = 0, nheight = 0;
	if (m_ncolumncount <= 0) return false;

	pt.x = GetVirtualWidth(cellid.ncol);
	pt.y = GetVirtualHeight(cellid.nrow);

	if (IsShowExpandCol()) pt.x += GetExpandWidth();
	return true;
}

void TGridCtrl::GetCellRect(const TCell& cellid, RECT& rect)
{
	POINT pt;
	GetCellOriginPt(cellid, pt);
	memset(&rect, 0, sizeof(RECT));
	rect.left = pt.x;rect.right = rect.left + GetColWidth(cellid.ncol);
	rect.top = pt.y; rect.bottom = rect.top + m_nrowheight;
}

bool TGridCtrl::IsValidCell(TCell& cellid)
{
	bool bret = true;
	if (cellid.nrow < 0 || cellid.nrow >= (int)m_nrowcount)
		bret = false;
	if (cellid.ncol < 0 || cellid.ncol >= (int)m_ncolumncount)
		bret = false;

	if (!bret)
		cellid.ncol = cellid.nrow = -1;
	return bret;
}

/**********************************************选中行相关****************************************************/
void TGridCtrl::SelectAll()
{
	m_stlselect.clear();
	for (_uint nindex = 0; nindex <= m_nrowcount; nindex++)
		AddSelectRow(nindex);
	RefreshGrid();
}

void TGridCtrl::SetSelectRow(int nselect)
{
	if (nselect <= m_nfixrow)//小于固定行
		return;
	m_nselectrow = nselect;
	m_stlselect.clear();
	AddSelectRow(nselect);
	RefreshGrid();
}

void TGridCtrl::AddSelectRow(int nselect)
{
	if (nselect < m_nfixrow || (_uint)nselect >= m_nrowcount)
		return;
	m_stlselect.push_back(nselect);
}

void TGridCtrl::RefreshSelectRow(int nrange)
{
	if (m_nselectrow > m_nfixrow)//zxw
		m_nselectrow += nrange;
	if (m_nselectrow < m_nfixrow)//zxw
		m_nselectrow = -1;
	VInt::iterator itor;
	for (itor = m_stlselect.begin(); itor != m_stlselect.end(); )
	{
		*itor += nrange;
		if (*itor < 0)
			itor = m_stlselect.erase(itor);
		else
			itor++;
	}
}

bool TGridCtrl::IsSelectRow(int nrow)
{
	if (std::find(m_stlselect.begin(), m_stlselect.end(), nrow) == m_stlselect.end())
		return false;
	return true;
}

/*********************************************数据相关*******************************************************/
void TGridCtrl::NotifyDispData()
{
	PostMessage(GetParent(), WM_NOTIFY, GN_DISPINFO, 0);
}

GV_ITEM* TGridCtrl::GetGridCell(const TCell& cellid)
{//仅可取到当前页面显示的数据
	//将row转换为数据存储位置
	int nrowindex = -1;
	int ncolindex = cellid.ncol;
	nrowindex = cellid.nrow;
	if(IsVertVisable()&&(_uint)nrowindex >= m_nfixrow)
		nrowindex -= m_vertscroll.nPos;

	return (m_stgriddata.GetGridCell(ncolindex, nrowindex));
}

void TGridCtrl::SetGridFont(LOGFONT lf)
{
	MGridData* pdata = m_stgriddata.GetGridData();
	if (!pdata) return;

	vector<MRowData>::iterator itor;
	for (itor = pdata->begin(); itor != pdata->end(); itor++)
	{
		vector<GV_ITEM*>::iterator gitor;
		for (gitor = (*itor).begin(); gitor != (*itor).end(); gitor++)
			(*gitor)->lgfont = lf;
	}
}

void TGridCtrl::SetItem(GV_ITEM* pitem)
{
	if (!pitem)
		return;
	GV_ITEM* cpdata = GetGridCell(pitem->cellid);
	if (!cpdata) return;

	deletevoidpointer(cpdata);
	memset(cpdata, 0, sizeof(cpdata));
	cpdata->cellid = pitem->cellid;
	cpdata->emtype = pitem->emtype;
	cpdata->dwmask = pitem->dwmask;
	cpdata->exdata = pitem->exdata;
	cpdata->emsort = pitem->emsort;
	if (cpdata->emsort == Header_Down 
		|| cpdata->emsort == Header_Upper)
		m_nsortcol = pitem->cellid.ncol;

	if (pitem->dwmask & GVIF_BKCLR)
		cpdata->clrbk = pitem->clrbk;
	if (pitem->dwmask & GVIF_FGCLR)
		cpdata->clrfg = pitem->clrfg;
	if (pitem->dwmask& GVIF_FGCLR2)
		cpdata->clrfg2 = pitem->clrfg2;
	if (pitem->dwmask & GVIF_FONT)
		cpdata->lgfont = pitem->lgfont;
	if (pitem->dwmask & GVIF_VMARGIN)
		cpdata->nvmargin = pitem->nvmargin;
	if (pitem->dwmask & GVIF_HMARGIN)
		cpdata->nhmargin = pitem->nhmargin;
	if (pitem->dwmask & GVIF_FORMAT)
		cpdata->dwformat = pitem->dwformat;
	if (pitem->dwmask & GVIF_TEXT)
		strncpy_s(cpdata->szparam, sizeof(cpdata->szparam), pitem->szparam, _TRUNCATE);
	return;
}

void TGridCtrl::GetNonfixDataRange(DataRange& strange, bool bresetpos/* = false*/)
{
	_uint nvisablerow = min(GetMaxVisableCount(), (_uint)m_vertscroll.nMax);

	if (bresetpos|| GetMaxVisableCount() >m_vertscroll.nMax)
	{
		m_vertscroll.nPos = 0;
		RefreshScrollRect();
	}
	strange.nbeginrow = m_vertscroll.nPos + m_nfixrow;
	strange.nendrow = min(m_vertscroll.nPos + nvisablerow, (_uint)m_vertscroll.nMax - m_nfixrow);
	strange.nbegincol = 0;
	strange.nendcol = 7;
}

void TGridCtrl::ModifyItemExdata(int ncol, int nrow, GridCellType sttype, void* pdata)
{
	TCell cell;
	cell.ncol = ncol; cell.nrow = nrow;
	GV_ITEM* cpdata = GetGridCell(cell);
	if (!cpdata) return;

	deletevoidpointer(cpdata);
	cpdata->exdata = pdata;
	cpdata->emtype = sttype;
}

void TGridCtrl::SetExpandCol(bool bexpand, int nid/* = -1*/, bool bheader/* = true*/,bool bcheck/* = false*/)
{
	m_bexpandfixcol = bexpand;
	m_nid = nid;
	m_bheadershowid = bheader;
	m_bexpandcheck = bcheck;
}

void TGridCtrl::GetExpandColRect(POINT& pt, RECT& rcinfo)
{
	int nindex = (pt.y - m_rcexpandfixcol.top) / 25;

	RECT rccheck;
	rcinfo = m_rcexpandfixcol; rcinfo.top += nindex * 25;

	if (m_nid != -1 && m_bexpandcheck)
	{
		GetExpandCheck(rcinfo, rccheck);
		rcinfo.left = rccheck.right + 2;
	}
	else
		InflateRect(&rcinfo, -3, -2);
}

void TGridCtrl::GetExpandCheck(POINT& pt, RECT& rccheck)
{
	int nindex = (pt.y - m_rcexpandfixcol.top) / 25;

	RECT rcinfo;
	memset(&rccheck, 0, sizeof(RECT));
	rcinfo = m_rcexpandfixcol; rcinfo.top += nindex * 25;
	if (IsShowExpandCol())
		GetExpandCheck(rcinfo, rccheck);
}

void TGridCtrl::GetExpandCheck(RECT rect, RECT& rccheck)
{
	int nleftgap = 10;
	rccheck = rect;
	rccheck.left += nleftgap; rccheck.top += 7;
	rccheck.bottom = rccheck.top + m_checksize; rccheck.right = rccheck.left + m_checksize;
}

_uint TGridCtrl::GetExpandWidth()
{
	if (m_nid != -1 && m_bexpandcheck)
		return EXPAND_WIDTH_EX;
	else
		return EXPAND_WIDTH;
}
void TGridCtrl::MoveOverExpandCol(POINT pt)
{
	bool bhover = false;
	int nindex = (pt.y - m_rcexpandfixcol.top) / 25;
	if (m_nid!=-1&&PtInRect(&m_rcexpandfixcol, pt) && nindex > 0)
	{
		RECT rcexpand;
		GetExpandColRect(pt, rcexpand);
		int ntoppos = GetVertPos();

		int nrow = nindex + ntoppos - 1;
		if (PtInRect(&rcexpand, pt))
		{
			bhover = true;
			PostMessage(GetParent(), SSWM_GRIDLIST_EXPAND_HOVER, 1, nrow);
		}
	}

	if (!bhover)
		PostMessage(GetParent(), SSWM_GRIDLIST_EXPAND_HOVER, 0, 0);
}

void TGridCtrl::ClickExpandCol(POINT pt)
{
	//int nindex = (pt.y - m_rcexpandfixcol.top) / 25;

	//if (PtInRect(&m_rcexpandfixcol, pt) && nindex >= 0)
	//{
	//	RECT rcexpand,rccheck;
	//	GetExpandColRect(pt, rcexpand);
	//	GetExpandCheck(pt, rccheck);
	//	if (PtInRect(&rcexpand, pt))
	//	{
	//		if (nindex > 0)
	//		{
	//			int ntoppos = GetVertPos();
	//			int nrow = nindex + ntoppos - 1;
	//			PostMessage(GetParent(), SSWM_GRIDLIST_EXPAND_Click, 0, nrow);
	//		}
	//		else if (nindex == 0 && m_nid == IDB_PNG_CLOSE)
	//		{
	//			PostMessage(GetParent(), SSWM_GRIDLIST_EXPAND_CloseAll, 0, 0);
	//		}
	//	}

	//	if (PtInRect(&rccheck, pt))
	//	{
	//		int ntoppos = GetVertPos();
	//		int nrow = nindex + ntoppos;
	//		MCheck::iterator itor;
	//		itor = m_stlcheck.find(nrow);
	//		if (itor != m_stlcheck.end())
	//			PostMessage(GetParent(), SSWM_GRIDCTRL_EXPAND_CHECK, itor->second, nrow);
	//	}
	//}
}

void TGridCtrl::SetExpandCheck(int nrow, bool bcheck)
{
	MCheck::iterator itor;
	itor = m_stlcheck.find(nrow);
	if (itor != m_stlcheck.end())
		itor->second = bcheck;
	else
		m_stlcheck.insert(std::make_pair(nrow, bcheck));
}