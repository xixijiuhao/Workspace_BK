#include "PreInclude.h"



TPinEdit::TPinEdit()
{
	m_bHoverUpPin = false;
	m_bHoverDownPin = false;
	m_bClickupPin = false;
	m_bClickdownPin = false;
	m_benable = true;
	m_nid = TEDIT_FLOAT;
	m_dStep = 1;

	m_clrPinClk = RGB(50, 50, 50);
	m_clrPinNormal = RGB(200, 200, 200);
	m_clrPinHover = RGB(100, 100, 100);
	m_clrPinUnable = RGB(150, 150, 150);

	m_sztext.assign("0");

	m_drangeMax = 100000.0;
	m_drangeMin = 0;

	m_bSetCharacter = false;
	m_dSetChValue = 0;
	m_iMillSecPress = 0;
}

TPinEdit::~TPinEdit()
{

}

LRESULT TPinEdit::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		OnSize();
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLbuttonup(wParam, lParam);
		break;
	case WM_TIMER:
		OnTimer(wParam, lParam);
		break;
	default:
		return __super::WndProc(message, wParam, lParam);
	}
	return PROCESSED;
}

void TPinEdit::OnSize()
{
	RECT rect; GetClientRect(m_Hwnd, &rect);
	m_rcUpPin = rect; m_rcUpPin.left = m_rcUpPin.right - rect.bottom + rect.top + 4;
	m_rcDownPin = m_rcUpPin;
	m_rcUpPin.bottom = m_rcUpPin.top + (m_rcUpPin.bottom - m_rcUpPin.top) / 2;
	m_rcDownPin.top = m_rcUpPin.bottom;
	::MoveWindow(m_edit, m_gapsize, m_gapsize, m_rcUpPin.left - rect.left - 2 * m_gapsize, rect.bottom -  2 * m_gapsize, false);//为什么-rect.left没看懂啊
}

void TPinEdit::DrawMain(TMemDC* pmemdc, const RECT& rect, HBRUSH framebrush)
{
	RECT rctemp;
	COLORREF PinColor;
	HBRUSH hbr;
	HPEN hpen;

	rctemp = m_rcUpPin;
	rctemp.bottom = m_rcDownPin.bottom;

	if (!m_bHoverUpPin&&!m_bHoverDownPin)
	{
		FrameRect(pmemdc->GetHdc(), &rctemp, g_ColorRefData.g_brush_gray);
	}
	else
	{
		FrameRect(pmemdc->GetHdc(), &rctemp, framebrush);
	}

	//draw uppin
	POINT pttemp[3];
	int icount[1] = { 3 };
	rctemp = m_rcUpPin;
	pttemp[0].x = (rctemp.right - rctemp.left) / 2 + rctemp.left;
	pttemp[0].y = (rctemp.bottom - rctemp.top) / 4 + rctemp.top;
	pttemp[1].x = (rctemp.right - rctemp.left) / 3 + rctemp.left;
	pttemp[1].y = (rctemp.bottom - rctemp.top) * 3 / 4 + rctemp.top;
	pttemp[2].x = (rctemp.right - rctemp.left) * 2 / 3 + rctemp.left;
	pttemp[2].y = (rctemp.bottom - rctemp.top) * 3 / 4 + rctemp.top;
	if (!m_benable)
	{
		PinColor = m_clrPinUnable;
	}
	else if (m_bClickupPin)
	{
		for (int i = 0; i < 3; i++)
		{
			pttemp[i].y += 2;
		}
		PinColor = m_clrPinClk;
	}
	else if (m_bHoverUpPin)
	{
		PinColor = m_clrPinHover;
	}
	else
	{
		PinColor = m_clrPinNormal;
	}
	hbr = CreateSolidBrush(PinColor);
	hpen = CreatePen(PS_SOLID, 1, PinColor);
	SelectObject(pmemdc->GetHdc(), hbr);
	SelectObject(pmemdc->GetHdc(), hpen);
	PolyPolygon(pmemdc->GetHdc(), pttemp, icount, 1);
	DeleteObject(hbr);
	DeleteObject(hpen);

	//draw downpin
	rctemp = m_rcDownPin;
	pttemp[0].x = (rctemp.right - rctemp.left) / 2 + rctemp.left;
	pttemp[0].y = (rctemp.bottom - rctemp.top) * 3 / 4 + rctemp.top - 1;
	pttemp[1].x = (rctemp.right - rctemp.left) * 2 / 3 + rctemp.left;
	pttemp[1].y = (rctemp.bottom - rctemp.top) / 4 + rctemp.top - 1;
	pttemp[2].x = (rctemp.right - rctemp.left) / 3 + rctemp.left;
	pttemp[2].y = (rctemp.bottom - rctemp.top) / 4 + rctemp.top - 1;

	if (!m_benable)
	{
		PinColor = m_clrPinUnable;
	}
	else if (m_bClickdownPin)
	{
		for (int i = 0; i < 3; i++)
		{
			pttemp[i].y += 2;
		}
		PinColor = m_clrPinClk;
	}
	else if (m_bHoverDownPin)
	{
		PinColor = m_clrPinHover;
	}
	else
	{
		PinColor = m_clrPinNormal;
	}
	hbr = CreateSolidBrush(PinColor);
	hpen = CreatePen(PS_SOLID, 1, PinColor);
	SelectObject(pmemdc->GetHdc(), hbr);
	SelectObject(pmemdc->GetHdc(), hpen);
	PolyPolygon(pmemdc->GetHdc(), pttemp, icount, 1);
	DeleteObject(hbr);
	DeleteObject(hpen);
}


void TPinEdit::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	__super::OnMouseMove(wParam, lParam);
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);
	m_bHoverUpPin = false;
	m_bHoverDownPin = false;
	if (PtInRect(&m_rcUpPin, pt))
		m_bHoverUpPin = true;
	if (PtInRect(&m_rcDownPin, pt))
		m_bHoverDownPin = true;
	if (!PtInRect(&m_rcUpPin, pt)&&m_bClickupPin)
	{
		m_bClickupPin = false;
		KillTimer(m_Hwnd, 0);
	}
	if (!PtInRect(&m_rcDownPin, pt)&&m_bClickdownPin)
	{
		m_bClickdownPin = false;
		KillTimer(m_Hwnd, 0);
	}
	InvalidateRect(m_Hwnd, 0, false);
}

void TPinEdit::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	__super::OnMouseLeave(wParam, lParam);
	KillTimer(m_Hwnd, 0);
	m_bHoverUpPin = false;
	m_bHoverDownPin = false;
	m_bClickdownPin = false;
	m_bClickupPin = false;
	InvalidateRect(m_Hwnd, 0, false);
}


void TPinEdit::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	__super::OnLbuttonDown(wParam, lParam);
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);
	if (PtInRect(&m_rcUpPin, pt))
	{
		m_bClickupPin = true;
		OnPinUp();
		m_iMillSecPress = 0;
		SetTimer(m_Hwnd, 0, 100, NULL);
	}
	else if (PtInRect(&m_rcDownPin, pt))
	{
		m_bClickdownPin = true;
		OnPinDown();
		m_iMillSecPress = 0;
		SetTimer(m_Hwnd, 0, 100, NULL);
	}
	InvalidateRect(m_Hwnd, 0, false);
}
void TPinEdit::OnLbuttonup(WPARAM wParam, LPARAM lParam)
{
	KillTimer(m_Hwnd, 0);
	m_bClickdownPin = false;
	m_bClickupPin = false;
	InvalidateRect(m_Hwnd, 0, false);
}

void TPinEdit::OnKeyDown(WPARAM wParam, LPARAM lParam, HWND hparent)
{
	__super::OnKeyDown(wParam, lParam, hparent);
	if (VK_UP == wParam)
	{
		OnPinUp();
	}
	if (VK_DOWN == wParam)
	{
		OnPinDown();
	}
}
void TPinEdit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	__super::OnCommand(wParam, lParam);
    if (HIWORD(wParam) == EN_KILLFOCUS)
	{
		if (m_bSetCharacter)return;
		char sztext[50];
		this->GetWindowTextW(sztext, sizeof(sztext));
		double dValue = atof(sztext);
		if (dValue < m_drangeMin)
		{
 			this->SetWindowTextW(m_drangeMin, m_iNumOfDot);
		}
		else if (dValue >m_drangeMax)
		{
			this->SetWindowTextW(m_drangeMax, m_iNumOfDot);
		}
	}
}

void TPinEdit::OnTimer(WPARAM wParam, LPARAM lParam)
{
	m_iMillSecPress++;
	int acclevel = 0;
	if (m_iMillSecPress >= 4 && m_iMillSecPress < 12)
	{
		acclevel = 1;
	}
	else if (m_iMillSecPress >= 12)
	{
		acclevel = 2;
	}
	else
	{
		return;
	}
	if (m_bClickupPin)
	{
		if (acclevel == 1 && m_iMillSecPress % 2 == 0)
		{
			OnPinUp();
		}
		else if (acclevel == 2)
		{
			OnPinUp();
		}
	}
	if (m_bClickdownPin)
	{
		if (acclevel == 1 && m_iMillSecPress % 2 == 0)
		{
			OnPinDown();
		}
		else if (acclevel == 2)
		{
			OnPinDown();
		}
	}
}

void TPinEdit::SetCharacter(const char *ptext)
{
	char tempchar[50];
	this->GetWindowTextW(tempchar, 50);
	m_dSetChValue = atof(tempchar);
	m_bSetCharacter = true;
	SetWindowTextW(ptext);
}

void TPinEdit::ValueMinus(double Value)
{
	if (Value < m_drangeMin)return;
	this->SetWindowTextW(Value, m_iNumOfDot);
	SendMessage(::GetParent(m_Hwnd), SSWM_PINEDIT_DOWN, 0, (LPARAM)m_Hwnd);
}

void TPinEdit::ValueAdd(double Value)
{
	if (Value > m_drangeMax)return;
	this->SetWindowTextW(Value, m_iNumOfDot);
	SendMessage(::GetParent(m_Hwnd), SSWM_PINEDIT_UP, 0, (LPARAM)m_Hwnd);
}

void TPinEdit::OnPinDown()
{
	if (m_bSetCharacter)//如果前面填了汉字
	{
		this->SetWindowTextW(m_dSetChValue, m_iNumOfDot);
		double tempdouble = m_dSetChValue - m_dStep;
		ValueMinus(tempdouble);
		m_bSetCharacter = false;
		return;
	}
	char tempchar[50];
	this->GetWindowTextW(tempchar, 50);
	double tempdouble = atof(tempchar);
	tempdouble -= m_dStep;
	ValueMinus(tempdouble);
}
void TPinEdit::OnPinUp()
{
	if (m_bSetCharacter)//如果前面填了汉字
	{
		this->SetWindowTextW(m_dSetChValue, m_iNumOfDot);
		double tempdouble = m_dSetChValue + m_dStep;
		ValueAdd(tempdouble);
		m_bSetCharacter = false;
		return;
	}
	char tempchar[50];
	this->GetWindowTextW(tempchar, 50);
	double tempdouble = atof(tempchar);
	tempdouble += m_dStep;
	ValueAdd(tempdouble);
}

void TPinEdit::RemoveCharacter()
{
	if (true == m_bSetCharacter)
	{
		m_bSetCharacter = false;
		this->SetWindowTextW(m_dSetChValue, m_iNumOfDot);
	}
}