#include "BaseInclude.h"

KLineIntroductView::KLineIntroductView():m_iYVap(5)
{
	m_bkBrush = g_ColorRefData.g_brush_black;
}
void KLineIntroductView::SetText(std::vector<string>& text)
{
	m_vtText = text;
}
void KLineIntroductView::CreateView(HWND hParent, RECT r)
{
	m_rect = r;
	CreateFrm(L"cxp_IntroductView", hParent, WS_VISIBLE | WS_CHILD);
	SetWindowPos(m_Hwnd, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);
}
void KLineIntroductView::MoveWindow(RECT rect)
{
	SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
}
LRESULT KLineIntroductView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		OnPaint();
		break;
	default:
		break;
	}
	return NOT_PROCESSED;
}
void KLineIntroductView::OnPaint()
{
	int iHeight = m_rect.bottom - m_rect.top;
	TMemDC dc(m_Hwnd);
	KLineSaveDC Save(dc);
	HBRUSH hbrFrame = CreateSolidBrush(KLineFrameColor);
	HPEN   penLine = CreatePen(PS_SOLID, 1, KLineFrameColor);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	SelectObject(dc.GetHdc(), g_FontData.g_FontWord13);
	SelectObject(dc.GetHdc(), penLine);
	RECT client = {};
	GetClientRect(m_Hwnd, &client);
	FillRect(dc.GetHdc(), &client, m_bkBrush);
	FrameRect(dc.GetHdc(), &client, hbrFrame);
	MoveToEx(dc.GetHdc(), (client.right - client.left)/ 2, client.top, NULL);
	LineTo(dc.GetHdc(), (client.right - client.left) / 2, client.bottom);


	if (!m_vtText.size()) {
		return;
	}
	RECT r = client;
	r.right = (client.right - client.left) / 2;
	SIZE size;
	int largeSize = 0;
	GetTextExtentPointA(dc.GetHdc(), m_vtText[0].c_str(), m_vtText[0].length(), &size);
	r.bottom = r.top + size.cy + m_iYVap;
	for (int index = 0; index < m_vtText.size(); index++)
	{
		if (m_vtText[index].length()) {
			GetTextExtentPointA(dc.GetHdc(), m_vtText[index].c_str(), m_vtText[index].length(), &size);
			if (m_vtText.size() == 1) {
				SelectObject(dc.GetHdc(), g_FontData.g_FontWord15);
				SetTextColor(dc.GetHdc(), KLineFontNameColor);
			}else {
				//都采用黄色
				SetTextColor(dc.GetHdc(), KLineFontNameColor);
				//SetTextColor(dc.GetHdc(), KLineFontNumberColor);
			}
			
			if (size.cx > (client.right - client.left)/2) {
				r.bottom = r.bottom + size.cy +  m_iYVap;
				DrawTextA(dc.GetHdc(), m_vtText[index].c_str(), m_vtText[index].length(), &r, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
				r.top = r.bottom;
				r.bottom = r.top + size.cy + m_iYVap;
			}
			else {
				DrawTextA(dc.GetHdc(), m_vtText[index].c_str(), m_vtText[index].length(), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				r.top = r.bottom;
				r.bottom = r.top + size.cy + m_iYVap;
			}
		
			
			if (r.bottom > iHeight) {
				r.right = client.right;
				r.left = (client.right - client.left) / 2;
				r.top = client.top;
				r.bottom = r.top + size.cy + m_iYVap;
			}
		}
	}
	DeleteObject(hbrFrame);
	DeleteObject(penLine);
}