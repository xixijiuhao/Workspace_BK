#include "PreInclude.h"

TCombox::TCombox()
{
	m_bmouseover = false;
	m_bmousetrack = false;
	m_blbtndown = false;
	m_buserdef = false;
	m_font = g_FontData.g_FontWord12;

	m_pcomboxlist = NULL;
}

TCombox::~TCombox()
{
	ReleaseComboxList();
}

bool TCombox::Create(HWND hwnd, int nindex/* = 0*/)
{
	m_nindex = nindex;
	CreateFrm(_T("TCombox"), hwnd, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	return true;
}

void TCombox::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, m_nComboxHeight, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, 0, true);
}

//property
void TCombox::SetUserComboxList(TPopupWnd* pwnd)
{
	ReleaseComboxList();
	m_pcomboxlist = pwnd;
	m_buserdef = true;
}

void TCombox::SetFont(HFONT hfont)
{
	m_font = hfont;
	InvalidateRect(m_Hwnd, 0, false);
}

void TCombox::SetText(const char* ptext)
{
	m_sztext = ptext;
	InvalidateRect(m_Hwnd, 0, false);
}

void TCombox::SetSelect(int nindex)
{
	if (nindex < 0 || nindex >= (int)m_stldata.size()) return;
	ComboxStruct stcom = m_stldata.at(nindex);
	SetText(stcom.sztext.c_str());
}
//action
void TCombox::AddString(const char* ptext, const char* pvalue)
{
	ComboxStruct stdata;
	stdata.nindex = m_stldata.size();
	stdata.sztext = ptext;
	if (pvalue)stdata.szvalue = pvalue;

	m_stldata.push_back(stdata);
}

void TCombox::ReleaseComboxList()
{
	if (!m_buserdef&&m_pcomboxlist)
	{
		delete m_pcomboxlist;
		m_pcomboxlist = NULL;
	}
}

void TCombox::Clear()
{
	m_sztext = "";
	m_stldata.clear();
	CreateDropList();
	((TComboxList*)m_pcomboxlist)->SetData(m_stldata);
}
void	TCombox::ResizeData()
{
	CreateDropList();
	((TComboxList*)m_pcomboxlist)->SetData(m_stldata);
}
int  TCombox::GetSameTextIndex(string sText)
{

	for (int i = 0; i < m_stldata.size(); i++)
	{
		if (sText == m_stldata.at(i).sztext)
			return m_stldata.at(i).nindex;
	}
	return -1;
}
//MSG
LRESULT TCombox::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave(wParam, lParam);
		break;
	case SSWM_COMBOX_SELECT:
		OnComboxSelect(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void TCombox::CreateDropList()
{
	if (m_pcomboxlist) return;
	m_pcomboxlist = new TComboxList;
	m_pcomboxlist->Create(m_Hwnd);
	m_pcomboxlist->MoveWindow(0, 0, 150, 200);
	((TComboxList*)m_pcomboxlist)->SetData(m_stldata);
}

void TCombox::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT	 rect;
	GetClientRect(m_Hwnd, &rect);

	HBRUSH framebrush = CreateSolidBrush(RGB(126, 180, 234));
	HBRUSH hframe;
	if (m_bmouseover)
		hframe = framebrush;
	else
		hframe = g_ColorRefData.g_brush_gray;

	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_white);
	FrameRect(memdc.GetHdc(), &rect, hframe);

	DrawDrop(&memdc, rect, framebrush);

	RECT	 rctext;
	rctext = rect;
	rctext.right -= m_nDropWidth;
	InflateRect(&rctext, -4, -2);
	SelectObject(memdc.GetHdc(), m_font);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	DrawWideText(memdc.GetHdc(), m_sztext.c_str(), rctext, DT_LEFT | DT_VCENTER);
	DeleteObject(framebrush);
}

void TCombox::DrawDrop(TMemDC* pmemdc, const RECT& rect, HBRUSH framebrush)
{
	HDC hdc = pmemdc->GetHdc();
	HBRUSH hoverbrush = CreateSolidBrush(RGB(253, 244, 191));

	RECT rcdrop;
	rcdrop = rect;
	rcdrop.left = rcdrop.right - m_nDropWidth;

	if (m_bmouseover)
	{
		FrameRect(hdc, &rcdrop, framebrush);
	}

	if (m_blbtndown)
		FillRect(hdc, &rcdrop, framebrush);

	SelectObject(hdc, GetStockObject(NULL_PEN));
	SelectObject(hdc, g_ColorRefData.g_brush_gray);
	POINT gpt[3];
	gpt[0].x = rcdrop.left + 2;
	gpt[0].y = rcdrop.top + (rcdrop.bottom - rcdrop.top - 3) / 2;
	gpt[2].x = rcdrop.right - 3;
	gpt[2].y = gpt[0].y;
	gpt[1].x = (gpt[0].x + gpt[2].x) / 2;
	gpt[1].y = gpt[0].y + 5;
	Polygon(hdc, gpt, sizeof(gpt) / sizeof(POINT));
	DeleteObject(hoverbrush);
}

void TCombox::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	RECT rccombox, rect;
	GetClientRect(m_Hwnd, &rect);
	rccombox = rect;

	if (PtInRect(&rect, pt))
	{
		CreateDropList();
		pt.x = rect.left; pt.y = rect.bottom - 1;
		::ClientToScreen(m_Hwnd, &pt);
		GetClientRect(m_pcomboxlist->GetHwnd(), &rect);
		m_pcomboxlist->MoveWindow(pt.x, pt.y, rccombox.right - rccombox.left, rect.bottom - rect.top);
		m_pcomboxlist->ShowWindow(SW_SHOW);
	}
}

void TCombox::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	m_bmouseover = true;
	if (!m_bmousetrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
	}
	InvalidateRect(m_Hwnd, 0, false);
}

void TCombox::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bmousetrack = false;
	m_bmouseover = false;

	InvalidateRect(m_Hwnd, 0, false);
}

void TCombox::OnComboxSelect(WPARAM wParam, LPARAM lParam)
{
	int nindex = (int)lParam;
	if (nindex >= 0 && nindex < (int)m_stldata.size())
	{
		std::string sztext = m_stldata[nindex].sztext;
		SetText(sztext.c_str());
	}

	PostMessage(GetParent(), SSWM_COMBOX_SELECT, nindex, (LPARAM)m_Hwnd);
}