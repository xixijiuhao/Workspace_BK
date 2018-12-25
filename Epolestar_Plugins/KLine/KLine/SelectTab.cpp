#include "BaseInclude.h"
extern ILanguageApi* g_pLanguageApi;

KLineSelectTab::KLineSelectTab():m_iCurrIndex(0), m_bMousetrack(false), m_iHoverIndex(-1), m_bInitRect(false)
{
	//m_wtext.push_back(L"高频15Min");
	m_wtext.push_back(g_pLanguageApi->LangText(HighFrequencyStrID));
	m_wtext.push_back(g_pLanguageApi->LangText(IntradayStrID));
	m_wtext.push_back(g_pLanguageApi->LangText(ShortTermStrID));
	m_wtext.push_back(g_pLanguageApi->LangText(MidTermStrID));
}

KLineSelectTab::~KLineSelectTab()
{
	DeleteObject(m_BackgroundBrush);
}
void KLineSelectTab::CreateView(HWND hParent, RECT r)
{
	m_hParent = hParent;
	CreateFrm(L"cxp_SelectTab", hParent, WS_VISIBLE | WS_CHILD);
	//问题没有收到WM_PAINT消息，设置位置在界面以外了
	SetWindowPos(m_Hwnd, 0, r.left, r.top, 340, r.bottom - r.top, 0);
}
LRESULT KLineSelectTab::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return NOT_PROCESSED;
	case WM_SIZE:
	{
		int width = GET_X_LPARAM(lParam);
		int height = GET_Y_LPARAM(lParam);
	}
		return PROCESSED;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam, lParam);
		break;
	default:
		break;
	}
	return NOT_PROCESSED;
}
void KLineSelectTab::OnPaint()
{
	TMemDC dc(m_Hwnd);
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	m_BackgroundBrush = CreateSolidBrush(RGB(38, 38, 46));
	HBRUSH RectFrameBr = CreateSolidBrush(KLineFrameColor);
	HBRUSH SelectBr = CreateSolidBrush(RGB(201, 51, 39));
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord14());
	
	//填充背景色
	RECT r;
	GetClientRect(m_Hwnd, &r);
	FillRect(dc.GetHdc(), &r, m_BackgroundBrush);

	//初始化Rect
	if (!m_bInitRect) {
		InitRect(dc);
	}
	//画悬浮Rect
	if (m_iHoverIndex >=0 && m_iHoverIndex <= m_vtRect.size() -1){
		DrawFocusRect(dc.GetHdc(), &m_vtRect[m_iHoverIndex]);
	}
	//画选中Rect
	//if (m_iCurrIndex >= 0 && m_iCurrIndex <= m_vtRect.size() - 1) {
	//	FillRect(dc.GetHdc(), &m_vtRect[m_iCurrIndex], SelectBr);
	//}

	for (int index = 0; index < m_wtext.size(); index++)
	{
		if (m_iCurrIndex == index)
		{
			SetTextColor(dc.GetHdc(), RGB(255, 60, 56));
			DrawText(dc.GetHdc(), m_wtext[index].c_str(), wcslen(m_wtext[index].c_str()), &m_vtRect[index], DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			continue;
		}
		SetTextColor(dc.GetHdc(), g_ColorRefData.GetColorWhite());
		DrawText(dc.GetHdc(), m_wtext[index].c_str(), wcslen(m_wtext[index].c_str()), &m_vtRect[index], DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	DeleteObject(SelectBr);
	DeleteObject(RectFrameBr);
	DeleteObject(SelectBr);
}

void KLineSelectTab::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	TrackMouse();
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	m_iHoverIndex = GetTabIndex(pt);
	if (m_iHoverIndex >= 0 ){
		InvalidateRect(m_Hwnd, 0, false);
	}
}
void KLineSelectTab::OnMouseLeave()
{
	m_bMousetrack = false;
	m_iHoverIndex = -1;
	InvalidateRect(m_Hwnd, 0, false);
}
void KLineSelectTab::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{

}
void KLineSelectTab::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	m_iCurrIndex = GetTabIndex(pt);
	InvalidateRect(m_Hwnd, NULL, false);
	m_func(m_iCurrIndex);
}
void KLineSelectTab::TrackMouse()
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
int  KLineSelectTab::GetTabIndex(POINT pt)
{
	for (int index = 0; index < m_vtRect.size(); index++)
	{
		if (pt.x <= m_vtRect[index].right && pt.x > m_vtRect[index].left && pt.y >= m_vtRect[index].top
			&& pt.y <= m_vtRect[index].bottom)
		{
			return index;
		}
	}
	return m_iCurrIndex;
}
void KLineSelectTab::InitRect(TMemDC dc)
{
	SIZE size;
	RECT rText = { 0 };
	GetClientRect(m_Hwnd, &rText);
	for (int index = 0; index < m_wtext.size(); index++)
	{
		GetTextExtentPoint(dc.GetHdc(), m_wtext[index].c_str(), wcslen(m_wtext[index].c_str()), &size);
		rText.right = rText.left + size.cx + KLineView_TextVap;
		m_vtRect.push_back(rText);
		rText.left = rText.right;
	}
	m_bInitRect = true;
}
