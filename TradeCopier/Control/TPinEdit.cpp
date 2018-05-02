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
	ResetTimer();
	m_clrPinClk = RGB(80, 80, 80);
	m_clrPinNormal = RGB(200, 200, 200);
	m_clrPinHover = RGB(150, 150, 150);
	m_clrPinUnable = RGB(255, 255, 255);

	m_sztext.assign("0");

	m_drangeMax = 100000.0;
	m_drangeMin = 0;

	m_bSetCharacter = false;
	m_dSetChValue = 0;
}

TPinEdit::~TPinEdit()
{

}

LRESULT TPinEdit::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hparent = ::GetParent(m_Hwnd);
	switch (message)
	{
	case WM_SIZE:
		OnSize();
		break;
	case WM_TIMER:
		OnTimer(wParam, lParam);
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

void TPinEdit::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if (m_iDelay-- < 0)
	{
		if (wParam == 0)
			OnPinUp();
		else
			OnPinDown();
	}

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
	if (!PtInRect(&m_rcUpPin, pt))
		m_bClickupPin = false;
	if (!PtInRect(&m_rcDownPin, pt))
		m_bClickdownPin = false;
	InvalidateRect(m_Hwnd, 0, false);
}

void TPinEdit::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	__super::OnMouseLeave(wParam, lParam);
	m_bHoverUpPin = false;
	m_bHoverDownPin = false;
	m_bClickdownPin = false;
	m_bClickupPin = false;
	ResetTimer();
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
		SetTimer(m_Hwnd, 0, 100, NULL);
		m_bClickupPin = true;
		OnPinUp();
	}
	else if (PtInRect(&m_rcDownPin, pt))
	{
		SetTimer(m_Hwnd, 1, 100, NULL);
		m_bClickdownPin = true;
		OnPinDown();
	}

	InvalidateRect(m_Hwnd, 0, false);
}
void TPinEdit::OnLbuttonup(WPARAM wParam, LPARAM lParam)
{
	ResetTimer();
	m_bClickdownPin = false;
	m_bClickupPin = false;
	InvalidateRect(m_Hwnd, 0, false);
}

void TPinEdit::OnKeyDown(WPARAM wParam, LPARAM lParam, HWND hparent)
{
	__super::OnKeyDown(wParam, lParam, hparent);
	if (VK_UP == wParam)
	{
		SetTimer(m_Hwnd, 0, 100, NULL);
		OnPinUp();
	}
	if (VK_DOWN == wParam)
	{
		SetTimer(m_Hwnd, 1, 100, NULL);
		OnPinDown();
	}
}

void TPinEdit::OnKeyUp()
{
	ResetTimer();
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
	SendMessage(::GetParent(m_Hwnd), SSWM_PINEDIT_UP, 1, (LPARAM)m_Hwnd);
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
void TPinEdit::ResetTimer()
{
	KillTimer(m_Hwnd, 0);
	KillTimer(m_Hwnd, 1);
	m_iDelay = 5;
}
