#include "PreInclude.h"
#pragma comment(lib, "MSIMG32.LIB")

TStaticButton::TStaticButton()
{
	m_btnclick = false;
	m_bmouseover = false;
	m_bmousetrack = false;
	m_benable = true;
	m_clrbk = RGB(240, 240, 240);//RGB(75,92,116);//COLOR_IMAGE_WHITE;
	m_clrfrm = RGB(150, 150, 150);
	m_textColor = RGB(34, 34, 34);
	m_DTStyle = DT_CENTER;
	m_font = g_FontData.g_FontWord13;
	m_sztext = "È·¶¨";
	m_gradient = NULL;
	_GradientFill = NULL;
	m_bfocus = false;
	m_hCursor = LoadCursor(NULL, IDC_ARROW);
	m_bClkChange = true;
}

TStaticButton::~TStaticButton()
{
	if (m_gradient)
		::FreeLibrary(m_gradient);
	m_gradient = NULL;
}
TStaticButton* TStaticButton::CreateStaticButton(HWND hParent, RECT& rect, const char* ptext, int index)
{
	TStaticButton* btn = new TStaticButton;
	btn->Create(hParent, index);
	btn->SetFont(g_FontData.g_FontWord12);
	btn->SetButtonText(ptext);
	btn->MoveWindow(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
	return btn;
}
void TStaticButton::CreateButton(HWND hParent, TStaticButton &btn, const char *pText, int index)
{
	btn.Create(hParent, index);
	btn.SetFont(g_FontData.g_FontWord12);
	btn.SetButtonText(pText);
}
bool TStaticButton::Create(HWND hparent, int nindex /* = 0 */)
{
	m_nindex = nindex;
	CreateFrm(_T("TStaticButton"), hparent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	SetClassLong(m_Hwnd, GCL_STYLE, CS_HREDRAW | CS_VREDRAW);
	//	m_gradient = ::LoadLibraryA("msimg32.dll");
	//
	//	if (m_gradient)
	//		_GradientFill = (LPGNDLLFUNC)::GetProcAddress(m_gradient, "GradientFill");
	return true;
}

void TStaticButton::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
	InvalidateRect(m_Hwnd, 0, true);
}

void TStaticButton::EnableButton(bool benable /*= true*/)
{
	m_benable = benable;
	InvalidateRect(m_Hwnd, 0, false);
	EnableWindow(m_Hwnd, m_benable);
	if (m_benable) SetFocus(m_Hwnd);
}

//property
void TStaticButton::SetButtonText(const char* ptext)
{
	m_sztext = ptext;
	SetWindowTextA(m_Hwnd, m_sztext.c_str());
	InvalidateRect(m_Hwnd, 0, false);
}

void TStaticButton::SetBkColor(COLORREF clrbk)
{
	m_clrbk = clrbk;
	InvalidateRect(m_Hwnd, 0, false);
}

void TStaticButton::SetFrameColor(COLORREF clrfrm)
{
	m_clrfrm = clrfrm;
	InvalidateRect(m_Hwnd, 0, false);
}

void TStaticButton::SetDTstyle(UINT DTstyle)
{
	m_DTStyle = DTstyle;
	InvalidateRect(m_Hwnd, 0, false);
}

void TStaticButton::SetFont(HFONT hfont)
{
	m_font = hfont;
	InvalidateRect(m_Hwnd, 0, true);
}
void TStaticButton::SetbtnTextColor(COLORREF color)
{
	m_textColor = color;
	InvalidateRect(m_Hwnd, 0, false);
}
void  TStaticButton::SetbtnCursorStyle(LPCTSTR CursorStyle)
{
	m_bClkChange = false;
	m_hCursor = LoadCursor(NULL, CursorStyle);
	InvalidateRect(m_Hwnd, 0, false);
}
//MSG

void TStaticButton::OnPaint()
{
	TMemDC memdc(m_Hwnd);

	RECT	 rect;
	GetClientRect(m_Hwnd, &rect);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	HBRUSH hbr;
	hbr = CreateSolidBrush(m_clrbk);
	FillRect(memdc.GetHdc(), &rect, hbr);
	DeleteObject(hbr);
	if (m_btnclick&&m_bmouseover)
	{
		//rect.top += 2;
		//rect.bottom += 2;
		OffsetRect(&rect, 0, 2);
	}
	SIZE sizetext;
	HPEN pen = CreatePen(PS_SOLID, 1, m_textColor);
	HPEN oldPen = (HPEN)SelectObject(memdc.GetHdc(), pen);
	SetTextColor(memdc.GetHdc(), m_textColor);
	SelectObject(memdc.GetHdc(), m_font);
	if (m_bClkChange)
	{
		hbr = CreateSolidBrush(m_clrfrm);
		FrameRect(memdc.GetHdc(), &rect, hbr);
		DeleteObject(hbr);
		if (m_bmouseover)
		{

			hbr = CreateSolidBrush(RGB(25, 121, 202)/*RGB(126, 180, 234)*/);
			//		DrawBtnBk(&memdc, rect);
			FillRect(memdc.GetHdc(), &rect, hbr);
			DeleteObject(hbr);
			SetTextColor(memdc.GetHdc(), g_ColorRefData.g_ColorWhite);
			hbr = CreateSolidBrush(RGB(126, 180, 234));
			FrameRect(memdc.GetHdc(), &rect, hbr);
			DeleteObject(hbr);
		}
		if (!m_benable)
		{
			hbr = CreateSolidBrush(RGB(170, 175, 186));
			FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_gray);
			DeleteObject(hbr);
			SetTextColor(memdc.GetHdc(), g_ColorRefData.g_ColorGray);
		}

		//if (m_benable&&m_bfocus)
		//{
		//	InflateRect(&rect, -3, -3);
		//	memdc.DotRectangle(rect, g_ColorRefData.g_ColorLightBlue, 1);
		//	InflateRect(&rect, 3, 3);
		//	
		//}
	}
	GetTextExtentPoint32A(memdc.GetHdc(), m_sztext.c_str(), m_sztext.length(), &sizetext);
	rect.top += (rect.bottom - rect.top - sizetext.cy) / 2;
	DrawText(memdc.GetHdc(), LoadRC::ansi_to_unicode(m_sztext).c_str(), -1, &rect, m_DTStyle);

	DeleteObject(pen);
	DeleteObject(oldPen);
}

void TStaticButton::DrawBtnBk(TMemDC* pmemdc, const RECT& rect)
{
	int nMove = 8;
	COLORREF clrtop, clrbottom;
	clrtop = RGB(240, 244, 252);
	clrbottom = RGB(219, 236, 252);
	TRIVERTEX tvert[2];
	tvert[0].x = rect.left;
	tvert[0].y = rect.top;
	tvert[0].Red = GetRValue(clrtop) << nMove;
	tvert[0].Green = GetGValue(clrtop) << nMove;
	tvert[0].Blue = GetBValue(clrtop) << nMove;
	tvert[0].Alpha = 255;


	tvert[1].x = rect.right;
	tvert[1].y = rect.bottom;
	tvert[1].Red = GetRValue(clrbottom) << nMove;
	tvert[1].Green = GetGValue(clrbottom) << nMove;
	tvert[1].Blue = GetBValue(clrbottom) << nMove;
	tvert[1].Alpha = 255;

	GRADIENT_RECT grect;
	grect.UpperLeft = 0;
	grect.LowerRight = 1;
	if (_GradientFill)_GradientFill(pmemdc->GetHdc(), tvert, 2/* sizeof(tvert) / sizeof(TRIVERTEX)*/, &grect, 1, GRADIENT_FILL_RECT_V);
}

void TStaticButton::OnLButtonDown()
{
	if (m_benable)
	{
		m_btnclick = true;
		SetCursor(m_hCursor);
		SetFocus(m_Hwnd);
		InvalidateRect(m_Hwnd, 0, false);
	}
}

void TStaticButton::OnLButtonUp()
{
	if (m_benable && m_btnclick)
	{
		m_btnclick = false;
		InvalidateRect(m_Hwnd, 0, false);
		SendMessage(GetParent(), SSWM_STATIC_BUTTON_CLICKDOWN, 0, m_nindex);
	}
}

void TStaticButton::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	m_bmouseover = true;
	SetCursor(m_hCursor);
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

void TStaticButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bmousetrack = false;
	m_bmouseover = false;
	InvalidateRect(m_Hwnd, 0, false);
}
void TStaticButton::OnEnableWindow(WPARAM wParam, LPARAM lParam)
{
	bool bEnable = (bool)wParam;
	EnableButton(bEnable);
}

void TStaticButton::OnSetFocus()
{
	m_bfocus = true;
	InvalidateRect(m_Hwnd, 0, false);
}

void TStaticButton::OnKillFocus()
{
	m_bfocus = false;
	InvalidateRect(m_Hwnd, 0, false);
}
LRESULT TStaticButton::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown();
		break;
	case WM_LBUTTONUP:
		OnLButtonUp();
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave(wParam, lParam);
		break;
	case WM_SETFOCUS:
		OnSetFocus();
		break;
	case WM_KILLFOCUS:
		OnKillFocus();
		break;
	case WM_ENABLE:
		OnEnableWindow(wParam, lParam);
		break;
	case WM_KEYDOWN:
		PostMessage(GetParent(), SSWM_TAB_SWITCH_FOCUS, 0, (LPARAM)m_Hwnd);
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}