#include "PreInclude.h"

TCheckBox::TCheckBox()
{
	m_bfocus = false;
	m_font = g_FontData.g_FontWord13;
	m_bkclr = g_ColorRefData.g_ColorWhite;
	m_fgclr = g_ColorRefData.g_ColorBlack;
}
TCheckBox::~TCheckBox()
{

}

bool TCheckBox::Create(HWND hwnd, const char* ptext, int ntype/* = TCS_CHECK*/, int nindex/* = 0*/)
{
	m_stcheck.sztext = ptext;
	m_nindex = nindex;
	m_checktype = ntype;
	CreateFrm(_T("TCheckbox"), hwnd,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN);

	return true;
}

//action
void TCheckBox::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, 0, true);
}

//Checkbox property
void TCheckBox::SetFont(HFONT hfont)
{
	m_font = hfont;
	if (!IsWindow(m_Hwnd)) return;

	InvalidateRect(m_Hwnd, 0, false);
}

void TCheckBox::SetValue(bool bvalue)
{
	m_bvalue = bvalue;
	if (!IsWindow(m_Hwnd)) return;

	InvalidateRect(m_Hwnd, 0, false);
}

void TCheckBox::SetText(const char* ptext)
{
	if (!ptext) return;
	m_stcheck.sztext = ptext;
	if (!IsWindow(m_Hwnd)) return;

	InvalidateRect(m_Hwnd, 0, false);
}

void TCheckBox::SetTextEX(const CheckStruct& stcheck)
{
	m_stcheck = stcheck;
	if (!IsWindow(m_Hwnd)) return;

	InvalidateRect(m_Hwnd, 0, false);
}

void TCheckBox::SetBkColor(COLORREF bkclr)
{
	m_bkclr = bkclr;
	InvalidateRect(m_Hwnd, 0, false);
}

void TCheckBox::SetFgColor(COLORREF fgclr)
{
	m_fgclr = fgclr;
	InvalidateRect(m_Hwnd, 0, false);
}
//MSG
void TCheckBox::OnPaint()
{
	RECT rect;
	TMemDC memdc(m_Hwnd);
	HBRUSH hbkbrush;

	GetClientRect(m_Hwnd,&rect);

	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	hbkbrush = CreateSolidBrush(m_bkclr);
	FillRect(memdc.GetHdc(), &rect, hbkbrush);

	RECT rcsel;
	rcsel.left = 2; rcsel.top = (rect.bottom - rect.top - m_checksize) / 2;
	rcsel.right = rcsel.left + m_checksize; rcsel.bottom = rcsel.top + m_checksize;

	HPEN pen;
	pen = CreatePen(PS_USERSTYLE, 1, m_fgclr);
	SelectObject(memdc.GetHdc(), pen);

	//»æÖÆcheckbox
	if (m_checktype == TCS_CHECK)
		DrawCheckbox(&memdc, rcsel);
	else if (m_checktype == TCS_RADIO)
		DrawRadio(&memdc, rcsel);
	else 
		DrawCheckHook(&memdc, rcsel);

	//text
	SelectObject(memdc.GetHdc(), m_font);
	SetTextColor(memdc.GetHdc(), m_fgclr);
	rcsel.left = rcsel.right + 10; rcsel.right = rect.right - 2;

	std::string sztext;

	sztext.append(m_stcheck.sztext);
	if (m_stcheck.sztextex.length() > 0)
	{
		sztext.append("("); sztext.append(m_stcheck.sztextex);
		sztext.append(")");
	}
	if (m_bfocus)
	{
		RECT rcdot;
		SIZE sizetext;
		rcdot = rect;
		GetTextExtentPoint32A(memdc.GetHdc(), sztext.c_str(), sztext.length(), &sizetext);
		rcdot.left = rcsel.left - 5; rcdot.right = rcdot.left + sizetext.cx + 10;

		memdc.DotRectangle(rcdot, m_fgclr, 1);
	}

	DrawTextA(memdc.GetHdc(), sztext.c_str(), sztext.length(), &rcsel, DT_LEFT | DT_VCENTER);

	DrawTextA(memdc.GetHdc(), m_stcheck.sztext2.c_str(), m_stcheck.sztext2.length(), &rcsel, DT_RIGHT | DT_VCENTER);

	DeleteObject(hbkbrush);
	DeleteObject(pen);
}
void TCheckBox::DrawCheckbox(TMemDC* pmemdc, RECT& rect)
{
	//»æÖÆcheckbox
	HBRUSH holdbr = (HBRUSH)SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_BRUSH));

	Rectangle(pmemdc->GetHdc(), rect.left, rect.top, rect.right, rect.bottom);
	m_rccheckbox = rect;

	if (m_bvalue)
	{
		InflateRect(&rect, -3, -3);
		HBRUSH hbr = CreateSolidBrush(m_fgclr);
		FillRect(pmemdc->GetHdc(), &rect, hbr);
		DeleteObject(hbr);
		InflateRect(&rect, 3, 3);
	}

	SelectObject(pmemdc->GetHdc(), holdbr);
}
void TCheckBox::DrawRadio(TMemDC* pmemdc, RECT& rect)
{
	HBRUSH holdbr = (HBRUSH)SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_BRUSH));

	Ellipse(pmemdc->GetHdc(), rect.left, rect.top, rect.right, rect.bottom);
	m_rccheckbox = rect;

	if (m_bvalue)
	{
		InflateRect(&rect, -2, -2);
		SelectObject(pmemdc->GetHdc(), g_ColorRefData.g_brush_gray);
		Ellipse(pmemdc->GetHdc(), rect.left, rect.top, rect.right, rect.bottom);
		InflateRect(&rect, 2, 2);
	}

	SelectObject(pmemdc->GetHdc(), holdbr);
}

void TCheckBox::DrawCheckHook(TMemDC* pmemdc, RECT& rect)
{
	//»æÖÆcheckbox
	HBRUSH holdbr = (HBRUSH)SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_BRUSH));

	Rectangle(pmemdc->GetHdc(), rect.left, rect.top, rect.right, rect.bottom);
	m_rccheckbox = rect;

	if (m_bvalue)
	{
		HPEN hpen = CreatePen(PS_SOLID, 2, m_fgclr);
		SelectObject(pmemdc->GetHdc(), hpen);
		InflateRect(&rect,-2, -2);

		POINT pp[3];
		pp[0].x = rect.left + 1;
		pp[0].y = rect.top + (rect.bottom - rect.top) / 2;
		pp[1].x = pp[0].x + 2;
		pp[1].y = rect.bottom - 2;
		pp[2].x = rect.right - 1;
		pp[2].y = rect.top + 2;

		MoveToEx(pmemdc->GetHdc(), pp[0].x, pp[0].y, NULL);
		LineTo(pmemdc->GetHdc(), pp[1].x, pp[1].y);
		MoveToEx(pmemdc->GetHdc(), pp[1].x, pp[1].y, NULL);
		LineTo(pmemdc->GetHdc(), pp[2].x, pp[2].y);
		
		InflateRect(&rect, 2, 2);
		DeleteObject(hpen);
	}

	SelectObject(pmemdc->GetHdc(), holdbr);
}

void TCheckBox::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	m_bvalue = !m_bvalue;
	SetFocus(m_Hwnd);
	WPARAM param; param = MAKEWORD(0, BN_CLICKED);
	PostMessage(GetParent(), WM_COMMAND, param, (LPARAM)m_Hwnd);
	InvalidateRect(m_Hwnd, 0, false);
}

void TCheckBox::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	int vkey = (int)wParam;
	if (vkey == VK_TAB)
		PostMessage(GetParent(), SSWM_TAB_SWITCH_FOCUS, 0, (LPARAM)m_Hwnd);
}

LRESULT TCheckBox::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_SETFOCUS:
		m_bfocus = true;
		InvalidateRect(m_Hwnd, 0, false);
		break;
	case WM_KILLFOCUS:
		m_bfocus = false;
		InvalidateRect(m_Hwnd, 0, false);
		break;
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}