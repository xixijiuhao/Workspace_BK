#include "PreInclude.h"

#define MESSAGE_BOTTOM			58

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;


TMessageBox::TMessageBox(HWND hparent, const char* sztext, UINT ntype)
	:TModalWnd(hparent, sztext)
{
	memset(m_szcontent, 0, sizeof(m_szcontent));
	m_pbtnok = NULL;
	m_pbtnno = NULL;
	m_ntype = ntype;
}

TMessageBox::~TMessageBox()
{
	if (m_pbtnok) delete m_pbtnok; m_pbtnok = NULL;
	if (m_pbtnno) delete m_pbtnno; m_pbtnno = NULL;
}
//Property
void TMessageBox::SetContent(const wchar_t* szcontent)
{
//	wcscpy_s(m_szcontent, sizeof(m_szcontent), szcontent);
	wsprintf(m_szcontent, _T("%s"), szcontent);
}

char szmsgchs[2][20] = {"确定", "取消"};
char* GetMessagebox(int i)
{
	return szmsgchs[i];
}
void TMessageBox::CreateSubCtrl()
{
	m_pbtnok = new TStaticButton;
	m_pbtnok->Create(m_Hwnd);
	m_pbtnok->SetBkColor(RGB(255, 255, 255));
	m_pbtnok->SetButtonText(GetMessagebox(0));

	m_pbtnno = new TStaticButton;
	m_pbtnno->Create(m_Hwnd,1);
	m_pbtnno->SetBkColor(RGB(255, 255, 255));
	m_pbtnno->SetButtonText(GetMessagebox(1));
}

void TMessageBox::SetModalRect()
{
	HDC hdc = GetDC(m_Hwnd); SIZE st;
	SelectObject(hdc, g_FontData.g_FontWord13);
	GetTextExtentPoint32(hdc, m_szcontent, wcslen(m_szcontent), &st);
	double /*dbcx,*/ dbcy; int nwidth;
	dbcy = sqrt(47 * 47 + st.cx*st.cy / 1.05) - 47; nwidth = (int)((dbcy + 149)*1.05);
	if (TMB_OKCANCEL == m_ntype) nwidth = max(nwidth, 253);
	if (TMB_OK == m_ntype) nwidth = max(nwidth, 140);
	MoveWindow(m_Hwnd, (GetSystemMetrics(SM_CXSCREEN) - 314) / 2, (GetSystemMetrics(SM_CYSCREEN) - 412) / 2, nwidth, (int)(dbcy + 149), true);
	ReleaseDC(m_Hwnd,hdc);
}
//MSG
void TMessageBox::DrawMain(TMemDC* pmemdc)
{
	RECT rect; GetClientRect(m_Hwnd, &rect);
	SelectObject(pmemdc->GetHdc(), g_FontData.g_FontWord13);

	rect.top += m_rctitle.bottom;
	rect.bottom -= MESSAGE_BOTTOM;
	InflateRect(&rect, -1, 0);
	HBRUSH hbr = CreateSolidBrush(RGB(255,255,255));
	FillRect(pmemdc->GetHdc(), &rect, hbr);
	rect.left += 20; rect.right -= 20;
	rect.top += 23;
	SetTextColor(pmemdc->GetHdc(), RGB(0, 0, 0));
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT);
	DrawText(pmemdc->GetHdc(), m_szcontent, wcslen(m_szcontent), &rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK/*DT_CENTER | DT_VCENTER | DT_SINGLELINE*/);
	DeleteObject(hbr);
}

void TMessageBox::OnSize()
{
	__super::OnSize();
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	RECT rcok, rcno;
	rcok.right = rect.right - 20; rcok.left = rcok.right - 100;
	rcok.bottom = rect.bottom - 13; rcok.top = rcok.bottom - 31;
	rcno = rcok; rcno.right = rcno.left - 13;
	rcno.left = rcno.right - 100;
	if (m_ntype==TMB_OK)
	{
		if (m_pbtnok) m_pbtnok->MoveWindow((rect.right - rect.left - 100) / 2, rcok.top, 100, 31);
	}
	if (m_ntype==TMB_OKCANCEL)
	{
		if (m_pbtnno) m_pbtnno->MoveWindow(rcok.left, rcok.top, 100, 31);
		if (m_pbtnok) m_pbtnok->MoveWindow(rcno.left, rcno.top, 100, 31);
	}
}

void TMessageBox::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 1)
		m_nret = 0;
	else
		m_nret = 1;
	DestroyWindow(m_Hwnd);
}
LRESULT TMessageBox::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		OnSize();
		break;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnButtonDown(wParam, lParam);
		break;
	default:
		return __super::WndProc(message, wParam, lParam);
	}

	return PROCESSED;
}

bool _TMessageBox_Domodal(HWND hwnd, char* sztitle, wchar_t* sztext, UINT ntype)
{
	TMessageBox tmsgbox(hwnd, sztitle,ntype);
	tmsgbox.SetContent(sztext);
	return tmsgbox.Domodal();
}