#include "PreInclude.h"

#define COMBOX_PAGER			10
#define COMBOX_CHECK_WIDTH		15

extern G_COLOREF			g_ColorRefData;				//É«²Ê
extern G_UIFONT				g_FontData;					//×ÖÌå

TLoginPopWnd::TLoginPopWnd()
{
	m_font = g_FontData.GetFontNum15();
	m_nmouseover = -1;
	m_nleftalign = 10;
	m_bVscrollVisable = false;
	m_clrbk = g_ColorRefData.GetColorWhite();
	m_clrfg = RGB(51, 153, 255);
	m_btrack = false;
}

TLoginPopWnd::~TLoginPopWnd()
{
	DestroyWindow(m_Hwnd);
}

bool TLoginPopWnd::Create(HWND hparent)
{
	m_parent = hparent;
	CreateFrm(_T("TComboxList"), hparent, WS_POPUP /*| WS_VISIBLE */ | WS_CLIPCHILDREN | WS_VSCROLL, WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE);

	m_nmaxheight = COMBOX_PAGER * 25;
	return true;
}

void TLoginPopWnd::SetData(const std::vector<SmsLoginRsp>& stldata)
{
	m_stldata = stldata;
	m_nmouseover = -1;
}

void TLoginPopWnd::GetData(char * sztext, SmsLoginRsp & stdata)
{
	//MByteToWChar(sztext, stdata.sztext, 40);

	std::vector<SmsLoginRsp>::iterator itor;
	itor = find(m_stldata.begin(), m_stldata.end(), stdata);
	if (itor != m_stldata.end())
		stdata = *itor;
}

int TLoginPopWnd::GetListHeight()
{
	int nheight;
	nheight = GetLineHeight() * m_stldata.size();
	nheight = min(nheight, m_nmaxheight);
	return nheight;
}

void TLoginPopWnd::MoveWindow(const int & x, const int & y, const int & cx, const int & cy)
{
	int nwidth, nheight;
	nheight = GetLineHeight() * m_stldata.size();
	nheight = min(nheight, m_nmaxheight);
	if (nheight <= 0) nheight = cy;

	nwidth = max(GetLineWidth(), cx);
	SetWindowPos(m_Hwnd, 0, x, y, nwidth, nheight, SWP_NOZORDER | SWP_NOACTIVATE);
	InvalidateRect(m_Hwnd, 0, true);
}

LRESULT TLoginPopWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_MOUSELEAVE:
		OnMouseLeave();
		break;
	default:
		return __super::WndProc(message, wParam, lParam);
	}

	return PROCESSED;
}

void TLoginPopWnd::DrawMain(TMemDC * pmemdc, RECT & rect)
{
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT);
	SelectObject(pmemdc->GetHdc(), m_font);
	std::vector<SmsLoginRsp>::iterator itor;

	int nindex = 0;
	int nSrcollPos = 0;
	int nlineheight = GetLineHeight();
	char szout[100];

	if (m_bVscrollVisable)
	{
		SCROLLINFO stinfo;
		memset(&stinfo, 0, sizeof(SCROLLINFO));
		stinfo.fMask = SIF_ALL;
		stinfo.cbSize = sizeof(SCROLLINFO);
		GetScrollInfo(m_Hwnd, SB_VERT, &stinfo);
		nSrcollPos = max(stinfo.nPos, 0);
	}

	if (nSrcollPos > 0)
		int a = 0;
	for (itor = m_stldata.begin(); itor != m_stldata.end(); itor++, nindex++)
	{
		if (nindex<nSrcollPos || nindex >= nSrcollPos + COMBOX_PAGER)
			continue;
		RECT rctemp = rect;
		rctemp.left += m_nleftalign;
		rctemp.top += nlineheight * (nindex - nSrcollPos) + m_nlinegap / 2; rctemp.bottom = rctemp.top + nlineheight;

		std::string sztext, szvalue;
		sztext = itor->UserNo;
	/*	if (strlen(itor->UserNo) > 0)
			szvalue.append(itor->UserNo);*/

		sprintf_s(szout, "%-20s", sztext.c_str());
		sztext = szout;
		//sprintf_s(szout,"%-10s", szvalue.c_str());
		//sztext.append(szout);

		DrawTextA(pmemdc->GetHdc(), sztext.c_str(), sztext.length(), &rctemp, DT_LEFT | DT_TOP | DT_SINGLELINE);
	}
}

void TLoginPopWnd::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT rect;
	HBRUSH hbr;
	GetClientRect(m_Hwnd, &rect);
	hbr = CreateSolidBrush(m_clrbk);
	FillRect(memdc.GetHdc(), &rect, hbr);
	FrameRect(memdc.GetHdc(), &rect, g_ColorRefData.GetHbrGray());
	DeleteObject(hbr);

	if (m_nmouseover >= 0)
	{
		HBRUSH hbr;
		RECT rctemp = rect;
		rctemp.top = GetLineHeight()*m_nmouseover;
		rctemp.bottom = rctemp.top + GetLineHeight();

		hbr = CreateSolidBrush(m_clrfg);
		FillRect(memdc.GetHdc(), &rctemp, hbr);
		DeleteObject(hbr);
	}
	DrawMain(&memdc, rect);
}

void TLoginPopWnd::OnSize()
{
	ResetScrollBars();
}

void TLoginPopWnd::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
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
			memset(&stinfo, 0, sizeof(SCROLLINFO));
			stinfo.fMask = SIF_ALL;
			stinfo.cbSize = sizeof(SCROLLINFO);
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
			{
				//if (m_stldata[nindex + nSrcollPos].bshow)
					SendMessage(m_parent, SSWM_COMBOX_SELECT, (WPARAM)m_Hwnd, nindex + nSrcollPos);
				ShowWindow(SW_HIDE);
			}
		}
	}
}

void TLoginPopWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	if (!m_btrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
	}

	if (PtInRect(&rect, pt))
	{
		int nindex = 0;
		int nheight = rect.bottom - rect.top;
		nindex = pt.y / GetLineHeight();
		m_nmouseover = nindex;
		InvalidateRect(m_Hwnd, 0, true);
	}
}

void TLoginPopWnd::OnVScroll(WPARAM wParam, LPARAM lParam)
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
		memset(&info, 0, sizeof(SCROLLINFO));
		info.fMask = SIF_TRACKPOS;
		info.cbSize = sizeof(SCROLLINFO);
		GetScrollInfo(m_Hwnd, SB_VERT, &info);
		SetScrollPos(m_Hwnd, SB_VERT, info.nTrackPos, true);
		break;
	}

	if (GetScrollPos(m_Hwnd, SB_VERT) == scrollPos)
		return;
	RefreshData();
}

void TLoginPopWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	if (!m_bVscrollVisable) return;
	short nsize = (short)HIWORD(wParam);

	SCROLLINFO stinfo;
	memset(&stinfo, 0, sizeof(SCROLLINFO));
	stinfo.fMask = SIF_POS | SIF_RANGE;
	stinfo.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(m_Hwnd, SB_VERT, &stinfo);

	if (nsize < 0)
		SetScrollPos(m_Hwnd, SB_VERT, min(stinfo.nPos + 2, stinfo.nMax), true);
	else
		SetScrollPos(m_Hwnd, SB_VERT, max(stinfo.nPos - 2, stinfo.nMin), true);

	RefreshData();
}

void TLoginPopWnd::OnMouseLeave()
{
	m_btrack = false;
	m_nmouseover = -1;
	InvalidateRect(m_Hwnd, 0, false);
}

int TLoginPopWnd::GetLineHeight()
{
	return 25;
}

int TLoginPopWnd::GetLineWidth()
{
	HDC hdc;
	hdc = GetDC(NULL);
	std::vector<SmsLoginRsp>::iterator itor;

	int nwidth = 0;
	char szout[100];
	SelectObject(hdc, m_font);
	for (itor = m_stldata.begin(); itor != m_stldata.end(); itor++)
	{
		SIZE sizetext;
		std::string sztext, szvalue;
		sztext = itor->UserNo;
		//if (strlen(itor->UserNo) > 0)
		//	szvalue.append(itor->UserNo);

		sprintf_s(szout, "%s", sztext.c_str());
		sztext = szout;
		/*if (szvalue.length() > 0)
		{
			sprintf_s(szout, "%-15s", szvalue.c_str());
			sztext.append(szout);
		}*/
		GetTextExtentPoint32A(hdc, sztext.c_str(), sztext.length(), &sizetext);
		nwidth = max(nwidth, sizetext.cx + m_nleftalign * 2);
	}

	if (m_stldata.size()>COMBOX_PAGER)
		nwidth += 20;
	ReleaseDC(NULL, hdc);
	return nwidth;
}

void TLoginPopWnd::ResetScrollBars(bool brestpos)
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

void TLoginPopWnd::RefreshData()
{
	InvalidateRect(m_Hwnd, 0, false);
}

bool TLoginPopWnd::IsOnlyShow()
{
	int nindex = 0;
	std::vector<SmsLoginRsp>::iterator itor;
	for (itor = m_stldata.begin(); itor != m_stldata.end(); itor++)
	{
		//if (itor->bshow) nindex++;
	}

	if (nindex > 1) return false;
	return true;
}
