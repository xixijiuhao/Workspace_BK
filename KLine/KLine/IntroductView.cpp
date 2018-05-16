#include "BaseInclude.h"

#define INTRODUCTTEXTHIGHT 300

KLineIntroductView::KLineIntroductView():m_iYVap(10)
{
	m_bkBrush = g_ColorRefData.g_brush_black;
	m_iHeight = 0;
}
void KLineIntroductView::SetText(std::vector<string>& text)
{
	m_vtFirstColText.clear();
	m_vtSecondColText.clear();
	m_vtText = text;
	bool isFirstCol = true;
	for (int i = 0;i < m_vtText.size();++i)
	{
		if (m_vtText[i] == "")
			continue;
		if (isFirstCol)
		{
			m_vtFirstColText.push_back(m_vtText[i]);
			int spos = m_vtText[i].find("技术点评");
			if (spos != string::npos)
			{
				isFirstCol = false;
			}
		}
		else
		{
			m_vtSecondColText.push_back(m_vtText[i]);
		}
	}
}
void KLineIntroductView::CreateView(HWND hParent, RECT r)
{
	CreateFrm(L"cxp_IntroductView", hParent, WS_VISIBLE | WS_CHILD);
	SetWindowPos(m_Hwnd, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);
	RECT client = {};
	GetClientRect(m_Hwnd, &client);
	m_CurRect = client;
	m_rect = client;
	m_iMaxBottom = m_rect.bottom - (INTRODUCTTEXTHIGHT - m_rect.bottom);

	m_ScrollRect.left = m_rect.right - 36;
	m_ScrollRect.right = m_rect.right - 30;
	m_ScrollRect.top = client.top;
	m_ScrollRect.bottom = client.top + 60;
}
void KLineIntroductView::MoveWindow(RECT rect)
{
	SetWindowPos(m_Hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
	InvalidateRect(m_Hwnd, NULL, false);
}
LRESULT KLineIntroductView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		OnSize(wParam, lParam);
		break;
	case WM_PAINT:
		OnPaint();
		break;
		/*case WM_MOUSEMOVE:
			OnMouseMove(wParam, lParam);
			break;
		case WM_MOUSELEAVE:
			OnMouseLeave();
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			OnLButtonUp(wParam, lParam);*/
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		break;
	default:
		break;
	}
	return NOT_PROCESSED;
}

void KLineIntroductView::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT client = {};
	GetClientRect(m_Hwnd, &client);
	m_rect = client;
	m_CurRect = client;
	m_iMaxBottom = m_rect.bottom - (INTRODUCTTEXTHIGHT - m_rect.bottom);
	m_ScrollRect.left = m_rect.right - 36;
	m_ScrollRect.right = m_rect.right - 30;
	m_ScrollRect.top = client.top;
	m_ScrollRect.bottom = client.top + 60;
}

void KLineIntroductView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	int offset = delta / 12;

	if (m_CurRect.bottom - m_CurRect.top >= INTRODUCTTEXTHIGHT)
		return;
	//控制滚轮，当文字已经到达最顶部则不能往上滚动；
	if (m_CurRect.top > 0 && offset > 0)
	{
		/*m_ScrollRect.top = 20;
		m_ScrollRect.bottom = m_ScrollRect.top + 60;*/
		return;
	}
	//控制滚轮，当文字已经到达最底部则不能往下滚动；
	if (m_CurRect.bottom < m_iMaxBottom && offset < 0)
	{
		return;
	}

	m_CurRect.top += offset;
	m_CurRect.bottom += offset;

	//处理滑动块
	/*double tmpMidDiv = (double)offset / (INTRODUCTTEXTHIGHT - m_rect.bottom);
	int scrollOffset = tmpMidDiv * (m_rect.bottom - 60) ;
	if(scrollOffset == 0)
	{
		scrollOffset = offset > 0 ? 1 : -1;
	}
	else
	{
		int offset2 = (m_iHeight % 2 == 0) ? 1 : -1;
		scrollOffset += offset2;
	}*/

	double tmpMidDiv = (double)(INTRODUCTTEXTHIGHT - m_rect.bottom) / (m_rect.bottom - 60);
	int scrollOffset = delta / (tmpMidDiv * 12);

	m_ScrollRect.top -= scrollOffset;
	m_ScrollRect.bottom -= scrollOffset;
	InvalidateRect(m_Hwnd, NULL, false);
	//++m_iHeight;
}

void KLineIntroductView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);

	if (PtInRect(&m_ScrollRect, point)) {
		//m_ScrollRect.top = 
		m_bMousetrack = false;

		InvalidateRect(m_Hwnd, NULL, false);
	}
}

void KLineIntroductView::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	TrackMouse();
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	if (m_bMousetrack) {
		//m_ScrollRect.top = 
	}
}

void KLineIntroductView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_bMousetrack = false;

	InvalidateRect(m_Hwnd, 0, false);
}

void KLineIntroductView::TrackMouse()
{
	if (!m_bMousetrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMousetrack = true;
	}
}

void KLineIntroductView::OnMouseLeave()
{
	m_bMousetrack = false;
	InvalidateRect(m_Hwnd, 0, false);
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
	HBRUSH scrollBrush = CreateSolidBrush(RGB(20,20,20));

	RECT client ={ };
	GetClientRect(m_Hwnd, &client);
	FillRect(dc.GetHdc(), &client, m_bkBrush);
	if (!m_vtFirstColText.size() && !m_vtSecondColText.size()) {
		return;
	}
	//对换合约之后进行控制
	//client = (m_strCurFirstText != m_vtFirstColText[0]) ? client : m_CurRect;
	if (m_strCurFirstText != m_vtFirstColText[0])
	{
		m_ScrollRect.left = m_rect.right - 36;
		m_ScrollRect.right = m_rect.right - 30;
		m_ScrollRect.top = client.top + 10;
		m_ScrollRect.bottom = client.top + 70;
	}
	else
	{
		client = m_CurRect;
	}
	RECT r = client;
	r.top += 10;
	r.right = (client.right - client.left) / 2 - 50;
	SIZE size;
	m_strCurFirstText = m_vtFirstColText[0];
	GetTextExtentPointA(dc.GetHdc(), m_strCurFirstText.c_str(), m_strCurFirstText.length(), &size);
	r.bottom = r.top + size.cy + m_iYVap;
	for (int index = 0; index < m_vtFirstColText.size(); index++)
	{
		if (m_vtFirstColText[index].length()) 
		{
			GetTextExtentPointA(dc.GetHdc(), m_vtFirstColText[index].c_str(), m_vtFirstColText[index].length(), &size);
			if (m_vtFirstColText.size() == 1) {
				SelectObject(dc.GetHdc(), g_FontData.g_FontWord15);
				SetTextColor(dc.GetHdc(), KLineFontNameColor);
			}else {
				SetTextColor(dc.GetHdc(), KLineFontNameColor);
				//SetTextColor(dc.GetHdc(), KLineFontNumberColor);
			}
			
			int oneLineWith = (client.right - client.left) / 2 - 50;
			if (size.cx > oneLineWith )
			{
				r.bottom = r.bottom + size.cy +  m_iYVap;
				DrawTextA(dc.GetHdc(), m_vtFirstColText[index].c_str(), m_vtFirstColText[index].length(), &r, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
				r.top = r.bottom;
				r.bottom = r.top + size.cy + m_iYVap;
			}
			else 
			{
				string tmpStr = m_vtFirstColText[index];
				DrawTextA(dc.GetHdc(), tmpStr.c_str(), m_vtFirstColText[index].length(), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				r.top = r.bottom;
				r.bottom = r.top + size.cy + m_iYVap;
			}
		}
	}

	r.right = client.right;
	r.left = (client.right - client.left) / 2;
	r.top = client.top + 10;
	r.bottom = r.top + size.cy + m_iYVap;

	for (int index = 0; index < m_vtSecondColText.size(); index++)
	{
		if (m_vtSecondColText[index].length()) {
			GetTextExtentPointA(dc.GetHdc(), m_vtSecondColText[index].c_str(), m_vtSecondColText[index].length(), &size);

			int oneLineWith = (client.right - client.left) / 2 - 50;
			if (size.cx > oneLineWith)
			{
				r.bottom = r.bottom + size.cy + m_iYVap;
				DrawTextA(dc.GetHdc(), m_vtSecondColText[index].c_str(), m_vtSecondColText[index].length(), &r, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
				r.top = r.bottom;
				r.bottom = r.top + size.cy + m_iYVap;
			}
			else
			{
				string tmpStr = m_vtSecondColText[index];
				DrawTextA(dc.GetHdc(), tmpStr.c_str(), m_vtSecondColText[index].length(), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				r.top = r.bottom;
				r.bottom = r.top + size.cy + m_iYVap;
			}
		}
	}

	if (m_CurRect.bottom - m_CurRect.top < INTRODUCTTEXTHIGHT )
	{
		FillRect(dc.GetHdc(), &m_ScrollRect, scrollBrush);
	}

	DeleteObject(scrollBrush);
	DeleteObject(hbrFrame);
	DeleteObject(penLine);
}
