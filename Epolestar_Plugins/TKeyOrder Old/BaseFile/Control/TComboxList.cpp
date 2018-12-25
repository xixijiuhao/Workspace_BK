#include "PreInclude.h"

#define TEXT_VALUE_GAP			"    "
#define COMBOX_PAGER			10
#define COMBOX_CHECK_WIDTH		15

TComboxList::TComboxList()
{
	m_font = g_FontData.g_FontWord12;
	m_nmouseover = -1;
	m_nleftalign = 10;
	m_bVscrollVisable = false;
}

TComboxList::~TComboxList()
{
	DestroyWindow(m_Hwnd);
}

bool TComboxList::Create(HWND hparent, int nstyle)
{
	m_parent = hparent;
	m_nstyle = nstyle;
	CreateFrm(_T("TComboxList"), NULL, WS_POPUP /*| WS_VISIBLE */ | WS_CLIPCHILDREN | WS_VSCROLL, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
	return true;
}

void TComboxList::SetData(const std::vector<ComboxStruct>& stldata)
{
	m_stldata = stldata;
	m_nmouseover = -1;
}

//action
int TComboxList::GetListHeight()
{
	int nheight;
	nheight = GetLineHeight() * m_stldata.size();
	nheight = min(nheight, m_nmaxheight);
	return nheight;
}
void TComboxList::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	int nwidth,nheight;
	nheight = GetLineHeight() * m_stldata.size();
	nheight = min(nheight, m_nmaxheight);
	if (nheight <= 0) nheight = cy;

	nwidth = max(GetLineWidth(), cx);
	SetWindowPos(m_Hwnd, 0, x, y, nwidth, nheight, SWP_NOZORDER|SWP_NOACTIVATE);
	InvalidateRect(m_Hwnd, 0, true);
}
//MSG
LRESULT TComboxList::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_SIZE:
		OnSize();
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_VSCROLL:
		OnVScroll(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		break;
	default:
		return __super::WndProc(message,wParam,lParam);
	}

	return PROCESSED;
}

void TComboxList::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_white);
	FrameRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_gray);

	if (m_nmouseover >= 0)
	{
		HBRUSH hbr;
		RECT rctemp = rect;
		rctemp.top = GetLineHeight()*m_nmouseover;
		rctemp.bottom = rctemp.top + GetLineHeight();

		hbr = CreateSolidBrush(RGB(51, 153, 255));
		FillRect(memdc.GetHdc(), &rctemp, hbr);
		DeleteObject(hbr);
	}
	DrawMain(&memdc,rect);
}

void TComboxList::DrawMain(TMemDC* pmemdc,RECT& rect)
{
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT);
	SelectObject(pmemdc->GetHdc(), m_font);
	std::vector<ComboxStruct>::iterator itor;

	int nindex = 0;
	int nSrcollPos = 0;
	int nlineheight = GetLineHeight();

	if (m_bVscrollVisable)
	{
		SCROLLINFO stinfo;
		stinfo.fMask = SIF_ALL;
		GetScrollInfo(m_Hwnd, SB_VERT, &stinfo);
		nSrcollPos = max(stinfo.nPos , 0);
	}

	if (nSrcollPos > 0)
		int a = 0;
	for (itor = m_stldata.begin(); itor != m_stldata.end(); itor++, nindex++)
	{
		if (nindex<nSrcollPos||nindex>=nSrcollPos+COMBOX_PAGER)
			continue;
		RECT rctemp = rect,rccheck;
		rctemp.left += m_nleftalign;
		rctemp.top += nlineheight*(nindex - nSrcollPos) + m_nlinegap / 2; rctemp.bottom = rctemp.top + nlineheight;
		if (m_nstyle == Combox_Check)
		{
			rccheck = rctemp;
			rccheck.right = rccheck.left + COMBOX_CHECK_WIDTH;
			rctemp.left += COMBOX_CHECK_WIDTH + m_nleftalign;
			rccheck.bottom = rccheck.top + COMBOX_CHECK_WIDTH;

			HBRUSH hcheck = CreateSolidBrush(RGB(186, 186, 186));
			SelectObject(pmemdc->GetHdc(), hcheck);
			FillRect(pmemdc->GetHdc(), &rccheck, hcheck);
			DeleteObject(hcheck);

			if (itor->bshow)
			{
				HPEN hpen = CreatePen(PS_SOLID, 2, RGB(255,255,255));
				SelectObject(pmemdc->GetHdc(), hpen);
				InflateRect(&rccheck, -2, -2);

				POINT pp[3];
				pp[0].x = rccheck.left + 1;
				pp[0].y = rccheck.top + (rccheck.bottom - rccheck.top) / 2;
				pp[1].x = pp[0].x + 2;
				pp[1].y = rccheck.bottom - 2;
				pp[2].x = rccheck.right - 1;
				pp[2].y = rccheck.top + 2;

				MoveToEx(pmemdc->GetHdc(), pp[0].x, pp[0].y, NULL);
				LineTo(pmemdc->GetHdc(), pp[1].x, pp[1].y);
				MoveToEx(pmemdc->GetHdc(), pp[1].x, pp[1].y, NULL);
				LineTo(pmemdc->GetHdc(), pp[2].x, pp[2].y);

				InflateRect(&rccheck, 2, 2);
				DeleteObject(hpen);
			}
		}

		std::string sztext;
		sztext = itor->sztext;

		DrawText(pmemdc->GetHdc(), LoadRC::ansi_to_unicode(sztext).c_str(), -1, &rctemp, DT_LEFT | DT_VCENTER);
	//	DrawWideText(pmemdc->GetHdc(), sztext.c_str(), rctemp, DT_LEFT | DT_VCENTER);
	}
}

void TComboxList::OnSize()
{
	ResetScrollBars();
}
void TComboxList::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if (!PtInRect(&rect, pt) && m_bauto)
		ShowWindow(SW_HIDE);
	else
	{
		int nSrcollPos = 0;
		if (m_bVscrollVisable)
		{
			SCROLLINFO stinfo;
			stinfo.fMask = SIF_ALL;
			GetScrollInfo(m_Hwnd, SB_VERT, &stinfo);
			nSrcollPos = max(stinfo.nPos, 0);
		}
		
		
		int nindex = 0;
		int nheight = rect.bottom - rect.top;
		nindex = pt.y / GetLineHeight();
		if (nindex >= 0 && nindex < (int)m_stldata.size())
		{
			RECT rccheck = rect;
			rccheck.left += m_nleftalign; rccheck.right = rccheck.left + COMBOX_CHECK_WIDTH;
			rccheck.top += GetLineHeight()*nindex + m_nlinegap / 2; rccheck.bottom = rccheck.top + GetLineHeight();
			if (m_nstyle == Combox_Check&&PtInRect(&rccheck, pt))
			{
				if (!IsOnlyShow() || !m_stldata[nindex + nSrcollPos].bshow)
				{
					m_stldata[nindex + nSrcollPos].bshow = !m_stldata[nindex + nSrcollPos].bshow;
					InvalidateRect(m_Hwnd, &rccheck, false);
					PostMessage(m_parent, SSWM_COMBOX_CHECK_CHANAGE, (WPARAM)m_Hwnd, nindex + nSrcollPos);
				}
			}
			else
			{
				if (m_stldata[nindex + nSrcollPos].bshow)
					PostMessage(m_parent, SSWM_COMBOX_SELECT, (WPARAM)m_Hwnd, nindex + nSrcollPos);
				ShowWindow(SW_HIDE);
			}
		}
	}
}

void TComboxList::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	if (PtInRect(&rect, pt))
	{
		int nindex = 0;
		int nheight = rect.bottom - rect.top;
		nindex = pt.y / GetLineHeight();
		m_nmouseover = nindex;
		InvalidateRect(m_Hwnd, 0, true);
	}
}

void TComboxList::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	int nsbcode = LOWORD(wParam);

	SCROLLINFO info;
	int scrollPos = GetScrollPos(m_Hwnd, SB_VERT);
	switch (nsbcode)
	{
	case SB_PAGEDOWN:
		SetScrollPos(m_Hwnd, SB_VERT, scrollPos + COMBOX_PAGER, true);
		break;
	case SB_PAGEUP:
		SetScrollPos(m_Hwnd, SB_VERT, scrollPos - COMBOX_PAGER, true);
		break;
	case SB_LINEDOWN:
		SetScrollPos(m_Hwnd, SB_VERT, scrollPos + 1, true);
		break;
	case SB_LINEUP:
		SetScrollPos(m_Hwnd, SB_VERT, scrollPos - 1, true);
		break;
	case SB_THUMBPOSITION:
		info.fMask = SIF_TRACKPOS;
		GetScrollInfo(m_Hwnd, SB_VERT, &info);
		SetScrollPos(m_Hwnd, SB_VERT, info.nTrackPos, true);
		break;
	}

	if (GetScrollPos(m_Hwnd, SB_VERT) == scrollPos)
		return;
	RefreshData();
}
void TComboxList::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	if (!m_bVscrollVisable) return;
	short nsize = (short)HIWORD(wParam);

	SCROLLINFO stinfo;
	stinfo.fMask = SIF_POS | SIF_RANGE;
	GetScrollInfo(m_Hwnd, SB_VERT, &stinfo);

	if (nsize < 0)
		SetScrollPos(m_Hwnd, SB_VERT, min(stinfo.nPos + 2, stinfo.nMax), true);
	else
		SetScrollPos(m_Hwnd, SB_VERT, max(stinfo.nPos - 2, stinfo.nMin), true);

	RefreshData();
}

//
int TComboxList::GetLineHeight()
{
	if (!m_Hwnd)return 0;

	LOGFONT lf;
	HDC hdc = GetDC(m_Hwnd);

	GetObject(m_font, sizeof(LOGFONT), &lf);
	ReleaseDC(m_Hwnd, hdc);
	return (abs(lf.lfHeight) + m_nlinegap);
}

int TComboxList::GetLineWidth()
{
	HDC hdc;
	hdc = GetDC(m_Hwnd);
	std::vector<ComboxStruct>::iterator itor;

	int nwidth = 0;
	char szout[100];
	SelectObject(hdc, m_font);
	for (itor = m_stldata.begin(); itor != m_stldata.end(); itor++)
	{
		SIZE sizetext;
		std::string sztext,szvalue;
		sztext = itor->sztext;
		if (itor->szvalue.length() > 0)
			szvalue.append(itor->szvalue);

		sprintf_s(szout, "%s", sztext.c_str());
		sztext = szout;
		if (szvalue.length() > 0)
		{
			sprintf_s(szout, "%-15s", szvalue.c_str());
			sztext.append(szout);
		}
		GetTextExtentPoint32A(hdc, sztext.c_str(), sztext.length(), &sizetext);
		nwidth = max(nwidth, sizetext.cx + m_nleftalign*2);
	}

	if (m_stldata.size()>COMBOX_PAGER)
		nwidth += 20;
	if (m_nstyle == Combox_Check)
		nwidth += m_nleftalign + COMBOX_CHECK_WIDTH;
	ReleaseDC(m_Hwnd, hdc);
	return nwidth;
}

void TComboxList::ResetScrollBars(bool brestpos/* = false*/)
{
	int nvertzpos = GetScrollPos(m_Hwnd, SB_VERT);

	int nheight = GetLineHeight() * m_stldata.size();
	if (nheight > m_nmaxheight)
	{
		EnableScrollBar(m_Hwnd, SB_VERT, TRUE);
		ShowScrollBar(m_Hwnd, SB_VERT, TRUE);
		m_bVscrollVisable = true;
	}
	else
	{
		EnableScrollBar(m_Hwnd, SB_VERT, FALSE);
		ShowScrollBar(m_Hwnd, SB_VERT, FALSE);
		m_bVscrollVisable = false;
	}

	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE | SIF_DISABLENOSCROLL | SIF_POS;

	si.nPage = COMBOX_PAGER;//m_nsinglepage;
	si.nMin = 0;
	si.nMax = m_stldata.size() - 1;
	si.nPos = 0;//brestpos ? 0 : nvertzpos;
	SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE);
}

void TComboxList::RefreshData()
{
	InvalidateRect(m_Hwnd, 0, false);
}

bool TComboxList::IsOnlyShow()
{
	int nindex = 0;
	std::vector<ComboxStruct>::iterator itor;
	for (itor = m_stldata.begin(); itor != m_stldata.end(); itor++)
	{
		if (itor->bshow) nindex++;
	}

	if (nindex > 1) return false;
	return true;
}