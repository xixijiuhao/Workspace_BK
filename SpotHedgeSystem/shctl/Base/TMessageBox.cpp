#include "TMessageBox.h"
#include "UIDesign.h"
#include <tchar.h>

extern G_COLOREF g_ColorRefData;
extern G_UIFONT  g_FontData;

#define MESSAGE_BOTTOM			58
TMessageBox::TMessageBox(HWND hparent, const char* sztext, UINT ntype)
	:TModalWnd(hparent, sztext)
{
	memset(m_szcontent, 0, sizeof(m_szcontent));
	m_ntype = ntype;
	m_bFocusOk = true;
	m_bFocusNo = false;
	memset(&m_rcOk, 0, sizeof(RECT));
	memset(&m_rcNo, 0, sizeof(RECT));
}

TMessageBox::~TMessageBox()
{
	
}
//Property
void TMessageBox::SetContent(const wchar_t* szcontent)
{
	wsprintf(m_szcontent, _T("%s"), szcontent);
}

char szmsgchs[2][20] = {"确定", "取消"};
char* GetMessagebox(int i)
{
	return szmsgchs[i];
}
void TMessageBox::CreateSubCtrl()
{

}

void TMessageBox::SetModalRect()
{
	HDC hdc = GetDC(m_Hwnd); SIZE st;
	SelectObject(hdc, g_FontData.GetFontWord13());
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
	SelectObject(pmemdc->GetHdc(), g_FontData.GetFontWord13());

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

	//画按钮按钮：
	HBRUSH btnHbr = CreateSolidBrush(RGB(0, 0, 0));
	UINT uformat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

	if (m_ntype == TMB_OK)
	{	
		FrameRect(pmemdc->GetHdc(), &m_rcOk, btnHbr);		
		if (m_bFocusOk)
			DrawFocusBtn(pmemdc, ID_OK);
		else
			DrawFocusBtn(pmemdc, ID_NULL);
	}
	else if (m_ntype == TMB_OKCANCEL)
	{
		FrameRect(pmemdc->GetHdc(), &m_rcOk, btnHbr);
		FrameRect(pmemdc->GetHdc(), &m_rcNo, btnHbr);
		if (m_bFocusOk)
			DrawFocusBtn(pmemdc, ID_OK);
		else if (m_bFocusNo)
			DrawFocusBtn(pmemdc, ID_NO);
		else
			DrawFocusBtn(pmemdc, ID_NULL);
	}
}
void TMessageBox::DrawFocusBtn(TMemDC* pmemdc, int id)
{
	int ihc = SaveDC(pmemdc->GetHdc());
	UINT uformat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	HBRUSH hbr = CreateSolidBrush(RGB(25, 121, 202));
	HBRUSH Framehbr = CreateSolidBrush(RGB(126, 180, 234));
	if (id == ID_OK)
	{
		FillRect(pmemdc->GetHdc(), &m_rcOk, hbr);
		FrameRect(pmemdc->GetHdc(), &m_rcOk, Framehbr);
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.GetColorWhite());
		DrawTextA(pmemdc->GetHdc(), "确定", -1, &m_rcOk, uformat);
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.GetColorBlack());
		DrawTextA(pmemdc->GetHdc(), "取消", -1, &m_rcNo, uformat);
	}
	else if (id == ID_NO)
	{
		FillRect(pmemdc->GetHdc(), &m_rcNo, hbr);
		FrameRect(pmemdc->GetHdc(), &m_rcNo, Framehbr);
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.GetColorWhite());
		DrawTextA(pmemdc->GetHdc(), "取消", -1, &m_rcNo, uformat);
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.GetColorBlack());
		DrawTextA(pmemdc->GetHdc(), "确定", -1, &m_rcOk, uformat);
	}
	else
	{
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.GetColorBlack());
		DrawTextA(pmemdc->GetHdc(), "取消", -1, &m_rcNo, uformat);
		DrawTextA(pmemdc->GetHdc(), "确定", -1, &m_rcOk, uformat);
	}
	DeleteObject(hbr);
	DeleteObject(Framehbr);
}
void TMessageBox::OnMouseMove()
{
	__super::OnMouseMove();
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);
	if (PtInRect(&m_rcOk, pt))
		SetBtnFocus(ID_OK);
	else if (PtInRect(&m_rcNo, pt))
		SetBtnFocus(ID_NO);
	else
	{
		if (m_bFocusNo)
			SetBtnFocus(ID_NO);
		else if (m_bFocusOk)
			SetBtnFocus(ID_OK);
	}	
}
void TMessageBox::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	__super::OnLbuttonDown(wParam, lParam);
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);

	if (PtInRect(&m_rcOk, pt))
		Result(ID_OK);
	else if (PtInRect(&m_rcNo, pt))
		Result(ID_NO);

}
void TMessageBox::Result(int id)
{
	if (id == ID_NO)
		m_nret = 0;
	else if (id == ID_OK)
		m_nret = 1;
	DestroyWindow(m_Hwnd);
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
		m_rcOk = { (rect.right - rect.left - 100) / 2, rcok.top, (rect.right - rect.left - 100) / 2 + 100, rcok.top + 31 };			
	}
	if (m_ntype==TMB_OKCANCEL)
	{
		m_rcOk = { rcno.left, rcno.top, rcno.left  + 100, rcno.top + 31 };
		m_rcNo = { rcok.left, rcok.top, rcok.left + 100, rcok.top + 31 };
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
void TMessageBox::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_LEFT:
	case VK_RIGHT:
		if (m_ntype == TMB_OKCANCEL)
			SetBtnFocus();
		break;
	case VK_RETURN:
	{
		if (m_bFocusOk)
			 m_nret = 1;
		else
			 m_nret = 0;
		DestroyWindow(m_Hwnd);
	}
	default:
		break;
	}
}
void TMessageBox::SetBtnFocus(int id)
{
	if (id == ID_OK)
	{
		m_bFocusOk = true;
		m_bFocusNo = false;
	}
	else if (id == ID_NO)
	{
		m_bFocusOk = false;
		m_bFocusNo = true;
	}
	else if (id == ID_NULL)
	{
		m_bFocusOk = false;
		m_bFocusNo = false;
	}
	InvalidateRect(m_Hwnd, NULL, TRUE);
}
void TMessageBox::SetBtnFocus()
{
	if (m_bFocusNo)
		SetBtnFocus(ID_OK);
	else if (m_bFocusOk)
		SetBtnFocus(ID_NO);
	else
		SetBtnFocus(ID_NULL);
}

LRESULT TMessageBox::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		OnSize();
		break;
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove();
		break;
	default:
		break;
	}

	return __super::WndProc(message, wParam, lParam);
}

bool _TMessageBox_Domodal(HWND hwnd, char* sztitle, const wchar_t* sztext, UINT ntype)
{
	TMessageBox tmsgbox(hwnd, sztitle,ntype);
	tmsgbox.SetContent(sztext);
	return tmsgbox.Domodal();
}