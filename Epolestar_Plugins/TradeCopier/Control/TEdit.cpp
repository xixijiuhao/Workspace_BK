#include "PreInclude.h"

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;

TEdit::TEdit()
{
	m_bmouseover = false;
	m_bHavaPopWin = false;
	m_bmousetrack = false;
	m_blbtndown = false;
	m_benable = true;
	m_sztext = "\0";
}
TEdit::~TEdit()
{
	RemoveWindowSubclass(m_edit, TEdit::EditProc, m_nid);
}
TEdit* TEdit::CreateEdit(HWND hParent, RECT& rect, int id)   //这个函数放哪呢
{
	TEdit* edit = new TEdit;
	edit->m_nid = id;
	edit->Create(hParent);
	edit->SetFont(g_FontData.g_FontWord13);
	edit->MoveWindow(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);

	return edit;
}
void TEdit::CreateEdit(HWND hParent, TEdit &edit, RECT rect, int style, DWORD specialstyle)
{
	edit.m_nid = style;
	edit.Create(hParent, specialstyle);
	edit.SetFont(g_FontData.g_FontNum15);
	edit.MoveWindow(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
	edit.SetFgColor(g_ColorRefData.g_ColorGray);
}

bool TEdit::Create(HWND hparent, DWORD specialstyle /*= 0*/, int nindex /* = 0 */)
{
	CreateFrm(_T("TEdit"), hparent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	//SetWindowLongPtr(m_Hwnd, GWL_ID, nindex);
	m_index = nindex;
	specialstyle |= WS_CHILD |/* WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_TABSTOP;
	if (TEDIT_INT == m_nid)
		specialstyle |= ES_NUMBER;
	m_edit = CreateWindowEx(0, WC_EDIT, 0, specialstyle, 0, 0, 0, 0, m_Hwnd, (HMENU)nindex, GetModuleHandle(NULL), 0);
	SetWindowSubclass(m_edit, TEdit::EditProc, m_nid, (DWORD_PTR)this);
	return true;
}

void TEdit::MoveWindow(const int& x, const int& y, const int& cx, const int& cy)
{
 	SetWindowPos(m_Hwnd, 0, x, y, cx, cy, SWP_NOZORDER);
 	InvalidateRect(m_Hwnd, 0, true);
}

//property
void TEdit::EnableEdit(bool benable /*= true*/)
{
	m_benable = benable;
	EnableWindow(m_Hwnd, benable);
	EnableWindow(m_edit, m_benable);
	InvalidateRect(m_Hwnd, 0, true);
	InvalidateRect(m_edit, 0, true);
}
void TEdit::SetFont(HFONT hfont)
{
	SendMessage(m_edit, WM_SETFONT, (WPARAM)hfont, 0);
	InvalidateRect(m_edit, 0, false);
}
void TEdit::SetWindowText(int num)
{
	char strTemp[50] = { 0 };   //初始化为零
	_itoa_s(num, strTemp, 10);
	SetWindowText(strTemp);
}
void TEdit::SetWindowText(double num , int numOfDots)
{
	char strTemp[50] = { 0 };
	char format[20] = "%.";
	sprintf_s(format, "%%.%dlf", numOfDots);
	sprintf_s(strTemp, format, num);
	SetWindowText(strTemp);
}
void TEdit::SetWindowText(const char* ptext)
{
	SetWindowTextA(m_edit, ptext);
	if (strlen(ptext) && m_benable)
		ShowWindow(m_edit, SW_SHOW);
	//else
	//	ShowWindow(m_edit, SW_HIDE);
	InvalidateRect(m_Hwnd, 0, false);
}
void TEdit::GetWindowNumber(int &number)
{
	char szText[50] = { 0 };
	GetWindowText(szText, 50);
	number = atoi(szText);
}
void TEdit::GetWindowText(char* ptext,int nmaxcount)
{
	GetWindowTextA(m_edit, ptext, nmaxcount);
}
bool TEdit::IsEmpty()
{
	char strTemp[1] = { 0 };
	GetWindowTextA(m_edit, strTemp, 1);
	return strTemp[0]=='\0';
}

void TEdit::SetFgColor(COLORREF fgcolor)
{
	m_fgcolor = fgcolor;
}

//MSG
void TEdit::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	HBRUSH framebrush = CreateSolidBrush(RGB(126, 180, 234)/*RGB(229, 195, 101)*/);
	HBRUSH fgbrush = CreateSolidBrush(m_fgcolor);
	RECT	 rect;
	GetClientRect(m_Hwnd, &rect);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	HBRUSH hframe;
	if (m_bmouseover)
		hframe = framebrush;
	else
		hframe = fgbrush;
	if (m_benable)
	{
    	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_white);
	}
	else
	{
		FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_gray);
	}
	FrameRect(memdc.GetHdc(), &rect, hframe);


//	if (!m_bmouseover)
	SetTextColor(memdc.GetHdc(), g_ColorRefData.g_ColorBuyRed);
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord13);
	InflateRect(&rect, -3,-1);
	DrawTextA(memdc.GetHdc(), m_sztext.c_str(), m_sztext.length(), &rect, DT_LEFT | DT_VCENTER);

	DrawMain(&memdc, rect, framebrush);
	DeleteObject(framebrush);
	DeleteObject(fgbrush);
}

void TEdit::OnSize()
{
	RECT rect; GetClientRect(m_Hwnd, &rect);
	::MoveWindow(m_edit, m_gapsize, m_gapsize, rect.right - rect.left - 2 * m_gapsize, rect.bottom - rect.left - 2 * m_gapsize, false);
}

void TEdit::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	if (m_benable)ShowWindow(m_edit, SW_SHOW);
	SetFocus(m_edit);
}
void TEdit::OnEnableWindow(WPARAM wParam, LPARAM lParam)
{
	bool bEnable = (bool)wParam;
	EnableEdit(bEnable);
}
void TEdit::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
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

void TEdit::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	if (!PtInRect(&rect, pt)&&GetFocus()!=m_edit)
	{
		m_bmousetrack = false;
		m_bmouseover = false;
	}

	InvalidateRect(m_Hwnd, 0, false);
}

void TEdit::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	char sztext[21];

	HWND hwnd = GetFocus();
	GetWindowTextA(m_edit, sztext, sizeof(sztext));
	if (strlen(sztext) == 0 && hwnd!=m_edit)
		ShowWindow(m_edit,SW_HIDE);
	m_bmousetrack = false;
	m_bmouseover = false;
	InvalidateRect(m_Hwnd, 0, false);
}

void TEdit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = GetParent();
	SendMessage(hwnd, WM_COMMAND, wParam, lParam);
}

LRESULT TEdit::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_MOUSELEAVE:
		OnMouseLeave(wParam, lParam);
		break;
	case WM_ENABLE:
		OnEnableWindow(wParam, lParam);
		break;
	case WM_SETFOCUS:
		m_bmouseover = true;
		InvalidateRect(m_Hwnd, 0, false);
		PostMessage(m_Hwnd,WM_LBUTTONDOWN, 0, 0);
		break;
	case WM_KILLFOCUS:
		OnKillFocus(wParam, lParam);
		break;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		break;
	case WM_CTLCOLOREDIT:
		return OnCtlColor(wParam, lParam);
	case SSWM_TAB_SWITCH_FOCUS:
		PostMessage(GetParent(), SSWM_TAB_SWITCH_FOCUS, 0, (LPARAM)m_Hwnd);
		break;
	default:
		return NOT_PROCESSED;
	}

	return PROCESSED;
}
LRESULT TEdit::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	if (m_benable)
	{
		SetBkColor(HDC(wParam), g_ColorRefData.g_ColorWhite);
		return (LRESULT)g_ColorRefData.g_brush_white;
	}
	SetBkColor(HDC(wParam), g_ColorRefData.g_ColorGray);
	return (LRESULT)g_ColorRefData.g_brush_gray;
}

void  TEdit::OnKeyDown(WPARAM wParam, LPARAM lParam, HWND hparent)
{
	if (VK_TAB == (int)wParam)
		SendMessage(hparent, SSWM_TAB_SWITCH_FOCUS, 0, 0);
	else if (VK_UP==wParam)
		SendMessage(hparent, WM_MOUSEWHEEL, 1<<16, (LPARAM)m_Hwnd);
	else if (VK_DOWN==wParam)
		SendMessage(hparent, WM_MOUSEWHEEL, -1<<16, (LPARAM)m_Hwnd);
}
LRESULT TEdit::EditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	HWND hparent = ::GetParent(hwnd);
	char strTemp[30];
	static bool dotFlag = true;
	switch (message)
	{
	case WM_KEYDOWN:
		((TEdit*)dwRefData)->OnKeyDown(wParam, lParam, hparent);
		break;
	case WM_KEYUP:
		((TEdit*)dwRefData)->OnKeyUp();
		break;
	case WM_KILLFOCUS:
	case WM_ACTIVATE:
		SendMessage(hparent, message, wParam, lParam);
		break;
	case WM_CHAR:  //限制输入浮点数据,暂时先这样处理，后面在优化，重点是画界面

		if (uIdSubclass == TEdit::TEDIT_FLOAT)
		{
			GetWindowTextA(hwnd, strTemp, 30);
			if (strlen(strTemp) == 0)
				dotFlag = true;
			if (wParam == '.')
			{
				if (!dotFlag)
					return 0;
				dotFlag = false;
			}
		/*	else if (wParam == '+' || wParam == '-')  //可以带正负号
			{
				if (strlen(strTemp) != 0)
					return 0;
			}*/
			else if ((wParam<'0' || wParam>'9') && wParam != VK_BACK)
				return 0;	
		}
		else if (uIdSubclass == TEdit::TEDIT_IP)
		{
			if ((wParam<'0' || wParam>'9') && wParam != VK_BACK&&wParam !='.')
			return 0;
		}
		else if (uIdSubclass == TEdit::TEDIT_SINT)
		{
			if (wParam == '+' || wParam == '-')
			{
				GetWindowTextA(hwnd, strTemp, 30);
				if (strlen(strTemp) != 0)
					return 0;
			}
			else if ((wParam<'0' || wParam>'9') && wParam != VK_BACK)
				return 0;
		}
		else if (uIdSubclass == TEdit::TEDIT_NUMCHAR)
		{
			if (!((wParam <= '9'&&wParam >= '0') || (wParam <= 'z'&&wParam >= 'a') || (wParam <= 'Z'&&wParam >= 'A') || wParam == VK_BACK))
				return 0;
		}
		break;
	}
	return DefSubclassProc(hwnd, message, wParam, lParam);
}