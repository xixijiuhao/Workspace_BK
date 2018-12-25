#include "PreInclude.h"

#define MiniHeight				6
extern G_UIFONT				g_FontData;					//字体
void VFilter_Clear(VFilter* stfilter)
{
	VFilter::iterator itor;
	for (itor = stfilter->begin(); itor != stfilter->end(); itor++)
		itor->bcheck = false;
}

void VFilter_Add(VFilter* stfilter, const GFilter& cpfilter)
{
	if (stfilter == NULL)
		return;
	VFilter::iterator itor;
	itor = find(stfilter->begin(), stfilter->end(), cpfilter.szvalue);
	if (itor == stfilter->end())
		stfilter->push_back(cpfilter);
}

void VFilter_SelectItem(VFilter* stfilter, const char* ptext, bool bsingle/* = false*/)
{
	VFilter::iterator itor;
	for (itor = stfilter->begin(); itor != stfilter->end(); itor++)
		itor->bcheck = false;
	itor = std::find(stfilter->begin(), stfilter->end(), ptext);
	if(itor!=stfilter->end())
		itor->bcheck = true;
}

int VFilter_GetSelectItem(VFilter* stfilter, string& szstring, bool bhaveall/* = true*/, VInt* stlint/* = NULL*/)//构造筛选条件
{
	int nindex = 0;
	int nret = -1;
	bool bfirst = true;
	VFilter::iterator itor;

	for (itor = stfilter->begin(); itor != stfilter->end(); itor++, nindex++)
	{
		if(bhaveall&&itor == stfilter->begin()&&itor->bcheck)
		{
			szstring.clear();
			nret = -1;
			break;
		}
		else if (itor->bcheck)
		{
			if (bfirst)
			{
				szstring = "^";
				bfirst = false;
			}
			szstring += itor->szvalue;
			szstring += "^";
			if(stlint)stlint->push_back(nindex);
		}
	}
	if (stlint) nret = stlint->size();
	return nret;
}
//////////////////////////////////////////////////////////////////////////
TFilterList::TFilterList()
{
//m_font = g_font_15;
	ResetParam();
}
TFilterList::~TFilterList()
{

}

//property
bool TFilterList::_Create(HWND hparent)
{
	m_parent = hparent;
	__super::Create(hparent);
  
	m_edit.Create(m_Hwnd);
	m_edit.SetFont(g_FontData.GetFontNum15());
	return true;
}

void TFilterList::_MoveWindow(int xpos, int ypos, int nalign/* = ListPos_LeftBottom*/)
{
	int nleft, ntop;
	switch (nalign)
	{
	case ListPos_LeftTop:
		nleft = xpos;
		ntop = ypos;
		break;
	case ListPos_LeftBottom:
		nleft = xpos;
		ntop = ypos - m_nwndheight;
		break;
	case ListPos_RightTop:
		nleft = xpos - m_nwndheight;
		ntop = ypos;
		break;
	}

	int nheight = m_nwndheight;
	int nwidth = m_nlistwidth;
	switch (m_ntype)
	{
	case Filter_Edit:
		m_bshowedit = true;
		if (m_cpfilter)
			m_edit.SetText(m_cpfilter->wsztext);
		else
			m_edit.SetText(L"");
		m_edit.SetRect(2, 2, m_nlistwidth - 4, 21);
		nheight = 25;
		break;
	case Filter_EditList_Single:
	case Filter_EditList:
		m_nvisablerow = 6;
		m_nlistheight = m_nwndheight - 25;
		m_edit.SetText(L"");
		SubFilterProc();
		CalFilterWidth(nwidth);
		m_bshowedit = (GetFilterSize() > 0);
		if(m_bshowedit)
			m_edit.SetRect(5, 2, nwidth - 10, 21);
		else
			m_edit.SetRect(0, 0, 0, 0);
		break;
	case Filter_Combox:
		m_nvisablerow = 3;
		m_bshowedit = false;
		nheight = 77;
		nwidth = 100;
		m_nlistheight = nheight;
		m_edit.SetRect(0, 0, 0, 0);
		break;
	default:
		m_nvisablerow = 7;
		m_nlistheight = m_nwndheight;
		m_bshowedit = false;
		m_edit.SetRect(0, 0, 0, 0);
		break;
	}
	Resize();
	SetWindowPos(m_Hwnd, 0, nleft, ntop, nwidth, nheight, SWP_NOZORDER | SWP_NOACTIVATE);
	InvalidateRect(m_Hwnd, 0, true);
}

void TFilterList::ShowWindow(int nCmdShow)
{
	if (nCmdShow == SW_SHOW)
		SetCapture(m_Hwnd);
	else
	{
		ReleaseCapture();
		SetFocus(m_parent);
	}
	__super::ShowWindow(nCmdShow);
}

//////////////////////////////////////////////////////////////////////////
void TFilterList::SetData(void* pdata, TCell cell, int ntype/* = Filter_EditList*/)
{
	m_stldata = NULL;
	m_cpfilter = NULL;
	m_ntype = ntype;
	m_subdata.clear();
	if (ntype != Filter_Edit)
		m_stldata = (VFilter*)pdata;
	else
		m_cpfilter = (FilterEdit*)pdata;
	m_cell = cell;

	//EditList List
	if (GetFilterSize() <= 0)
	{
		m_nselect = -1;
		m_nbeginrow = 0;
	}
	else
	{
		m_nselect = 0;
		m_nbeginrow = 0;
	}

	//Edit
	if (m_cpfilter)
		m_edit.SetText(m_cpfilter->wsztext);
	Resize();
	InvalidateRect(m_Hwnd, 0, false);
}
//msg
void TFilterList::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	HBRUSH hbr = CreateSolidBrush(RGB(231, 231, 231));
	HBRUSH	hframe = CreateSolidBrush(RGB(96,96,96));
	RECT rect; GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, hbr);
	FrameRect(memdc.GetHdc(), &rect, hframe);

	//Edit
	if (m_bshowedit)
		m_edit.OnDraw(&memdc);
	RECT rclist = rect;
	rclist.top = rect.bottom - m_nlistheight;
	DrawMain(&memdc, rclist);
	DeleteObject(hbr);
	DeleteObject(hframe);
}

void TFilterList::DrawMain(TMemDC* pmemdc, RECT rect)
{
	int nindex = m_nbeginrow;
	InflateRect(&rect, -1, -1);

	RECT rcselect;
	RECT rcthumb = rect;
	rcselect = rect;
	rect.left += m_leftmargin;
	rect.right -= m_rightmargin;
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT);
	SelectObject(pmemdc->GetHdc(), m_font);

	HBRUSH hselect;
	hselect = CreateSolidBrush(RGB(36, 35, 35));
	if (m_nvertheight > 0)
	{
		rect.right -= m_nvertwidth;
		rcselect.right -= m_nvertwidth;
	}

	if ((int)GetFilterSize() > m_nvisablerow)
	{
		HBRUSH hthumb,hbkthumb;
		RECT rcbkthumb = rcthumb;
		rcthumb.left = rcthumb.right - m_nvertwidth;
		rcthumb.top += (int)(m_nbeginrow * (m_nlistheight - 2/*MiniHeight*/ + 0.0) / GetFilterSize());
		rcthumb.bottom = rcthumb.top + m_nvertheight;
		OffsetRect(&rcthumb, -1, 1);
		hthumb = CreateSolidBrush(RGB(194, 195, 201));
		hbkthumb = CreateSolidBrush(RGB(232, 232, 236));
		rcbkthumb.left = rcthumb.left;
		rcbkthumb.right = rcthumb.right;
		FillRect(pmemdc->GetHdc(), &rcbkthumb, hbkthumb);
		FillRect(pmemdc->GetHdc(), &rcthumb, hthumb);
		m_rcthumb = rcthumb;
		DeleteObject(hthumb);
		DeleteObject(hbkthumb);
	}
	//
	bool bpartcheck = IsPartCheck();
	//Text
	char sztext[1000];
	HPEN hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN hselpen = CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
	for (; nindex <m_nvisablerow+m_nbeginrow&&nindex < (int)GetFilterSize()&&GetFilterSize() > 0; nindex++)
	{
		RECT rctemp = rect;
		rctemp.top = (nindex - m_nbeginrow) * m_nrowheight + rctemp.top;
		rctemp.bottom = rctemp.top + m_nrowheight;
		if (m_nselect == nindex)
		{
			rcselect.top = rctemp.top;
			rcselect.bottom = rctemp.bottom;
			FillRect(pmemdc->GetHdc(), &rcselect, hselect);
			SetTextColor(pmemdc->GetHdc(), RGB(220, 220, 220));
			SelectObject(pmemdc->GetHdc(), hselpen);
		}
		else
		{
			SetTextColor(pmemdc->GetHdc(), RGB(0, 0, 0));
			SelectObject(pmemdc->GetHdc(), hpen);
		}
		int nretorder;
		GFilter stdata; nretorder = GetFilterData(nindex, stdata);

		if(stdata.bshowvalue)
			sprintf_s(sztext, "%s%s", stdata.sztext, stdata.szvalue);
		else
			sprintf_s(sztext, "%s", stdata.sztext);
		if (m_ntype != Filter_Combox)
			DrawCheck(pmemdc, rctemp, stdata.bcheck, (m_nselect == nindex),(bpartcheck&&nretorder == 0));

		wchar_t wsztext[1024];	\
		MByteToWChar(sztext, wsztext, 1024);
		DrawText(pmemdc->GetHdc(), wsztext, wcslen(wsztext), &rctemp, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	}
	DeleteObject(hpen);
	DeleteObject(hselpen);
	DeleteObject(hselect);
}

void TFilterList::DrawCheck(TMemDC* pmdc, RECT& rect, bool bvalue, bool bselect, bool bpartcheck)
{
	int nleftgap = 5;

	RECT rccheck = rect;
	rccheck.left += nleftgap; rccheck.top += (rccheck.bottom - rccheck.top - m_checksize) / 2;
	rccheck.bottom = rccheck.top + m_checksize; rccheck.right = rccheck.left + m_checksize;

	HBRUSH holdbr = (HBRUSH)SelectObject(pmdc->GetHdc(), GetStockObject(NULL_BRUSH));
	Rectangle(pmdc->GetHdc(), rccheck.left, rccheck.top, rccheck.right, rccheck.bottom);


	if (bpartcheck)
	{
		HBRUSH hfill;
		if(!bselect) hfill = CreateSolidBrush(RGB(0, 0, 0));
		else hfill = CreateSolidBrush(RGB(220, 220, 220));
		RECT rcfill = rccheck;
		InflateRect(&rcfill, -2, -2);
		FillRect(pmdc->GetHdc(), &rcfill, hfill);
		DeleteObject(hfill);
	}
	else if (bvalue)
	{
		InflateRect(&rccheck, -1, -2);

		POINT pp[3];
		pp[0].x = rccheck.left;
		pp[0].y = rccheck.top + (rccheck.bottom - rccheck.top) / 2 - 1;
		pp[1].x = pp[0].x + 4;
		pp[1].y = pp[0].y + 5;
		pp[2].x = pp[1].x + 7;// rect.right - 1;
		pp[2].y = pp[1].y - 8;//rect.top + 2;

		MoveToEx(pmdc->GetHdc(), pp[0].x +1, pp[0].y+1, NULL);
		LineTo(pmdc->GetHdc(), pp[1].x + 1, pp[1].y);
 		MoveToEx(pmdc->GetHdc(), pp[1].x, pp[1].y - 1, NULL);
 		LineTo(pmdc->GetHdc(), pp[2].x, pp[2].y);

		InflateRect(&rccheck, 2, 2);
	}

	rect.left = rccheck.right + 5;
	SelectObject(pmdc->GetHdc(), holdbr);
}

void TFilterList::OnArrowUpDown(WPARAM wparam, LPARAM lparam)
{
	int nkey = (int)wparam;
	int nsize = GetFilterSize();
	if (nsize <= 0)return;
	switch (nkey)
	{
	case VK_UP:
		if (m_nselect <= 0)
			m_nselect = 0;
		else
			m_nselect--;
		break;
	case VK_DOWN:
		m_nselect++;
		if (m_nselect >= nsize)
			m_nselect = nsize - 1;
		break;
	}

	if (m_nbeginrow > m_nselect)
	{
		m_nbeginrow = m_nselect;
		m_nbeginrow = max(m_nbeginrow, 0);
	}
	else if (m_nselect > m_nbeginrow + m_nvisablerow - 1)
	{
		m_nbeginrow = m_nselect - m_nvisablerow + 1;
		m_nbeginrow = max(m_nbeginrow, 0);
	}
	InvalidateRect(m_Hwnd, 0, false);
}

void TFilterList::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int ncount = GetFilterSize();
	if (ncount <= m_nvisablerow)return;
	short nsize = (short)HIWORD(wParam);

	if (nsize < 0)
		m_nbeginrow++;
	else if (nsize > 0)
		m_nbeginrow--;

	if (m_nbeginrow < 0)
		m_nbeginrow = 0;
	else if (m_nbeginrow + m_nvisablerow >= ncount)
		m_nbeginrow = ncount - m_nvisablerow;

	InvalidateRect(m_Hwnd, 0, false);
}

void TFilterList::OnLButtonDown(WPARAM wparam, LPARAM lparam)
{
	__super::OnLbuttonDown(wparam, lparam);
	POINT pt;
	pt.x = (int)(short)LOWORD(lparam);
	pt.y = (int)(short)HIWORD(lparam);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	RECT rclist = rect;
	rclist.top = rect.bottom - m_nlistheight;
	if (PtInRect(&m_rcthumb, pt))
	{
		m_dragoffset.x = pt.x - (_Width(m_rcthumb) / 2 + m_rcthumb.left)/*m_rtThumb.CenterPoint()*/;
		m_dragoffset.y = pt.y - (_Height(m_rcthumb) / 2 + m_rcthumb.top)/*m_rtThumb.CenterPoint()*/;
		m_bdragging = true;
		return;
	}
	else if (PtInRect(&rclist, pt))
	{
		int nselect;
		int noffrow = (pt.y - rect.top) / m_nrowheight;
		nselect = m_nbeginrow + noffrow;
		nselect = min(m_nselect, (int)GetFilterSize() - 1);

		if (PtInRect(&rclist, pt))
		{
			if (m_ntype != Filter_Combox)
			{
				ModifyFilterCheck(nselect);
				::PostMessage(m_parent, SSWM_FILTER_LIST_SELECT, m_cell.nrow, m_cell.ncol);
			}
			else
				::PostMessage(m_parent, SSWM_FILTER_COMBOX_SELECT, nselect,(LPARAM)&m_cell);
		}

		InvalidateRect(m_Hwnd, 0, false);
	}
}

void TFilterList::OnLButtonUp(WPARAM wparam,LPARAM lparam)
{
	POINT point;
	point.x = (int)(short)LOWORD(lparam);
	point.y = (int)(short)HIWORD(lparam);

	BOOL bDrag = m_bdragging;
	m_bdragging = false;
	if (bDrag)
	{
		point.x += -m_dragoffset.x;
		point.y += -m_dragoffset.y;
		ScrollTo(point);
	}

	InvalidateRect(m_Hwnd, NULL, FALSE);
}

void TFilterList::OnMouseMove(WPARAM wparam, LPARAM lparam)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags =TME_LEAVE;
	tme.hwndTrack = m_Hwnd;
	tme.dwHoverTime = 1;
	_TrackMouseEvent(&tme);

	POINT point;
	point.x = (int)(short)LOWORD(lparam);
	point.y = (int)(short)HIWORD(lparam);

	RECT rect; GetClientRect(m_Hwnd, &rect);
	RECT rclist = rect;
	rclist.top = rect.bottom - m_nlistheight;
	// drag thumb
	if (m_bdragging)
	{
		point.x += -m_dragoffset.x;
		point.y += -m_dragoffset.y;
		ScrollTo(point);
		InvalidateRect(m_Hwnd, NULL, FALSE);
	}
	else if (PtInRect(&rclist, point))
		{
			int noffrow = (point.y - rclist.top) / m_nrowheight;
			m_nselect = m_nbeginrow + noffrow;
			m_nselect = min(m_nselect, (int)GetFilterSize() - 1);

			InvalidateRect(m_Hwnd, 0, false);
		}
}

void TFilterList::OnMouseLeave()
{
	m_bdragging = false;
	InvalidateRect(m_Hwnd, NULL, FALSE);
}

void TFilterList::OnArrowReturn()
{
	m_edit.GetText(m_cpfilter->wsztext, sizeof(m_cpfilter->wsztext) / 2);
	ShowWindow(SW_HIDE);
	::PostMessage(m_parent, SSWM_FILTER_EDIT_RETURN, 0, 0);
}

void TFilterList::OnEditFilterReturn()
{
	if (m_ntype == Filter_EditList)
	{
		VInt::iterator itor;
		itor = m_subdata.begin();
		for (; itor != m_subdata.end(); itor++)
		{
			GFilter& stfilter = m_stldata->at(*itor);
			stfilter.bcheck = !stfilter.bcheck;
		}
		InvalidateRect(m_Hwnd, 0, FALSE);
		::PostMessage(m_parent, SSWM_FILTER_LIST_SELECT, m_cell.nrow, m_cell.ncol);
	}
}

LRESULT TFilterList::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam,lParam);
		m_edit.OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		m_edit.OnLButtonUp(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		m_edit.OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		m_edit.OnMouseLeave();
		break;
	case WM_KEYDOWN:
		m_edit.OnKeyDown(wParam, lParam);
		if (VK_DOWN == (int)wParam || VK_UP == (int)wParam)
			OnArrowUpDown(wParam, lParam);
		break;
	case WM_CHAR:
		m_edit.OnChar(wParam, lParam);
		if (VK_RETURN == (int)wParam && m_ntype == Filter_Edit)
			OnArrowReturn();
		if (VK_RETURN == (int)wParam && m_ntype == Filter_EditList)
			OnEditFilterReturn();
		break;
	case WM_MOUSEWHEEL:/*SSWM_FILTER_LIST_WHEEL*/
		OnMouseWheel(wParam, lParam);
		break;
	case SSWM_MINIEDIT_CHANGE:
		SubFilterProc(/*wParam*/);
		Resize();
		break;
//////////////////////////////////////////////////////////////////////////
 	case WM_SETFOCUS:
 		m_edit.OnSetFocus();
 		break;
	case WM_KILLFOCUS:
		m_edit.OnKillFocus();
		break;
	default:
		return __super::WndProc(message,wParam,lParam);
	}

	return PROCESSED;
}

void TFilterList::Resize()
{
	int nsize = GetFilterSize();
	if (nsize > m_nvisablerow)
	{
		int nheight = m_nlistheight - MiniHeight;
		m_nvertheight = (int)((m_nvisablerow+0.0) / nsize * nheight);
		m_nvertheight = max(m_nvertheight, MiniHeight);
	}
	else m_nvertheight = 0;
}

void TFilterList::ResetParam()
{
	m_nselect = -1;
	m_nbeginrow = 0;
	m_nvertheight = 0;
	m_bdragging = false;
	m_bauto = true;
	m_stldata = NULL;
	m_subdata.clear();
	m_ntype = Filter_EditList;
}

void TFilterList::ScrollTo(POINT point)
{
	int ndatasize = GetFilterSize();
	RECT rect;
	GetClientRect(m_Hwnd,&rect);
	RECT rctemp = m_rcthumb;
	rctemp.top = point.y - _Height(rctemp) / 2;
	if (rctemp.top < rect.top)
		rctemp.top = rect.top;
	rctemp.bottom = rctemp.top + _Height(m_rcthumb);
	if (rctemp.bottom > rect.bottom)
	{
		rctemp.bottom = rect.bottom;
		rctemp.top = rctemp.bottom - _Height(m_rcthumb);
	}
	m_rcthumb = rctemp;

	double dbbegin = (int)(rctemp.top - rect.top)*ndatasize / (_Height(rect) + 0.01);
	m_nbeginrow = (int)dbbegin;
	if (dbbegin - m_nbeginrow > 0.0000001)
		m_nbeginrow++;


 	if (m_nbeginrow > ndatasize - m_nvisablerow)
 		m_nbeginrow = ndatasize - m_nvisablerow;
	InvalidateRect(m_Hwnd, NULL, FALSE);
}

_uint TFilterList::GetFilterSize()
{
	_uint nsize = 0;
	if (m_ntype == Filter_EditList||m_ntype == Filter_EditList_Single)
		nsize = m_subdata.size();
	else if (m_stldata)
		nsize = m_stldata->size();

	return nsize;
}

int TFilterList::GetFilterData(const _uint nindex, GFilter& stdata)
{
	if (nindex >= GetFilterSize())
		return -1;
	memset(&stdata, 0, sizeof(GFilter));
	if (m_ntype == Filter_EditList || m_ntype == Filter_EditList_Single)
	{
		int norder = m_subdata.at(nindex);
		stdata = m_stldata->at(norder);
		return norder;
	}
	else
		stdata = m_stldata->at(nindex);
	return nindex;
}

void TFilterList::ModifyFilterCheck(const _uint nindex)
{
	if (nindex >= GetFilterSize())
		return;
	int norder = nindex;
	if (m_ntype == Filter_EditList || m_ntype == Filter_EditList_Single)
		norder = m_subdata.at(nindex);
	if (m_ntype == Filter_EditList_Single)
	{
		for (_uint index = 0; index < m_stldata->size(); index++)
		{
			if(index != norder) m_stldata->at(index).bcheck = false;
		}
	}
	GFilter& gfilter = m_stldata->at(norder);
	gfilter.bcheck = !gfilter.bcheck;
	if (norder == 0 && strlen(gfilter.szvalue) <= 0)
	{
		for (_uint index = 1; index < m_stldata->size(); index++)
			m_stldata->at(index).bcheck = gfilter.bcheck;
	}

	if(strlen(m_stldata->at(0).szvalue)==0)
		ModifyAllCheck();
}

void TFilterList::ModifyAllCheck()
{
	bool balltrue = true;
	for (_uint index = 1; index < m_stldata->size(); index++)
	{
		balltrue = balltrue && m_stldata->at(index).bcheck;
		if (!balltrue) break;
	}
	m_stldata->at(0).bcheck = balltrue;
}

void TFilterList::CalFilterWidth(int& nwidth)
{
	if (m_ntype != Filter_EditList && m_ntype != Filter_List && m_ntype != Filter_EditList_Single)
		return;
	if (!m_stldata) return;

	int nleft = 0;
	if (m_ntype == Filter_EditList||m_ntype == Filter_EditList_Single) nleft = 30;
	else nleft = 10;
	HDC hdc = GetDC(m_Hwnd);
	SelectObject(hdc, m_font);

	char sztext[1000];
	VFilter::iterator itor;
	for (itor = m_stldata->begin(); itor!= m_stldata->end(); itor++)
	{
		if (itor->bshowvalue)
			sprintf_s(sztext, "%s%s", itor->sztext, itor->szvalue);
		else
			sprintf_s(sztext, "%s", itor->sztext);

		SIZE size;
		GetTextExtentPoint32A(hdc, sztext, strlen(sztext), &size);
		nwidth = max(nwidth, size.cx + nleft);
	}

	ReleaseDC(m_Hwnd, hdc);
}

bool TFilterList::IsPartCheck()
{
	if (!m_stldata) return false;

	bool bhaveall = false;
	VFilter::iterator itor;
	itor = m_stldata->begin();
	if (itor == m_stldata->end()) return false;
	if (strlen(itor->szvalue) <= 0) bhaveall = true;
	itor++;

	bool bhavecheck = false; bool bhavencheck = false;
	for (; itor != m_stldata->end(); itor++)
	{
		if (itor->bcheck)
			bhavecheck = true;
		else
			bhavencheck = true;
	}

	if (bhavecheck&&bhavencheck&&bhaveall)
		return true;
	return false;
}
/////////////////////////////////嵌入Edit//////////////////////////////////////////////////////////////////
void TFilterList::SubFilterProc(int nkey/* = 0*/)
{
	if (m_ntype != Filter_EditList&& m_ntype != Filter_EditList_Single)
		return;
	char sztext[49];
	FilterEdit stedit;

	m_edit.GetText(stedit.wsztext, sizeof(m_cpfilter->wsztext) / 2);
	WCharToMByte(stedit.wsztext, sztext, 49);
	if (strlen(sztext)>0&&(wcslen(m_wszpretext)>0 && _wcsstr(stedit.wsztext, m_wszpretext)!=NULL))
	{
		VInt::iterator itor;
		for (itor = m_subdata.begin(); itor != m_subdata.end();)
		{
			GFilter stfilter = m_stldata->at(*itor);
			if (!_strstr(stfilter.sztext, sztext)/* == NULL*/)//if (strstr(stfilter.sztext, sztext) == NULL)
				itor = m_subdata.erase(itor);
			else
				itor++;
		}
	}
	else
	{
		_uint index = 0;
		VFilter::iterator itor;
		m_subdata.clear();
		for (itor = m_stldata->begin(); itor!=m_stldata->end(); itor++, index++)
		{
			if (_strstr(itor->sztext, sztext)!=NULL||strlen(sztext)==0)
				m_subdata.push_back(index);
		}
	}

	//VK_RETURN ZXW
// 	if (nkey == VK_RETURN)
// 	{
// 		for (int i = 0; i < m_subdata.size(); i++)
// 		{
// 			int index = m_subdata.at(i);
// 			m_stldata->at(index).bcheck = true;
// 		}
// 		ShowWindow(SW_HIDE);
// 		::PostMessage(m_parent, SSWM_FILTER_EDIT_RETURN, 0, 0);
// 	}

	m_nbeginrow = 0;
	m_nselect = 0;
	wcsncpy_s(m_wszpretext,stedit.wsztext,_TRUNCATE);
	InvalidateRect(m_Hwnd, 0, false);
}

void WToUpper(wstring & str)
{
	for (uint i = 0; i < str.length(); i++)
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] -= 32;
}

void AToUpper(string & str)
{
	for (uint i = 0; i < str.length(); i++)
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] -= 32;
}

bool TFilterList::_wcsstr(const wchar_t* scdes, const wchar_t* scsrc)
{
	wstring wszdes = scdes;
	wstring wszsrc = scsrc;
	WToUpper(wszdes); WToUpper(wszdes);

	if (wszdes.find(wszsrc) == 0)
		return true;
	return false;
}

bool TFilterList::_strstr(const char* scdes, const char* scsrc)
{
	string szdes = scdes;
	string szsrc = scsrc;
	AToUpper(szdes); AToUpper(szsrc);

	if (szdes.find(szsrc) != string::npos)
		return true;

	int sizepos = szdes.find(" ");
	if (sizepos != string::npos)
	{
		szdes.replace(sizepos, 1, "");
		if (szdes.find(szsrc) != string::npos)
			return true;
	}
	
	return false;
}