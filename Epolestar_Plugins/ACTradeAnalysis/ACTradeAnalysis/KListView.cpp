#include "BaseInclude.h"

extern ILanguageApi* g_pLanguageApi;

KListView::KListView() :
	m_itemSelectIndex(0),
	m_iheaderSelectIndex(0),
	m_itemShowBegin(0),
	m_itemHoverIndex(-1),
	m_iheaderHoverIndex(-1),
	m_iHeaderHeight(40),
	m_iItemHeight(30)
{
	m_textColor = RGB(200, 200, 200);
	m_bkColor = RGB(38, 38, 46);
	m_frameColor = RGB(19, 19, 26);
	m_headerColor = RGB(200, 200, 200);
	m_scrollColor = RGB(10, 10, 10);
	m_selectColor = RGB(28, 28, 36);
	m_selectTextColor = RGB(255, 60, 56);
	m_bIsMoveItem = false;

	m_ExchangeData.emplace(make_pair("CZCE", g_pLanguageApi->LangText(CZCE)));
	m_ExchangeData.emplace(make_pair("DCE", g_pLanguageApi->LangText(DCE)));
	m_ExchangeData.emplace(make_pair("SHFE", g_pLanguageApi->LangText(SHFE)));
	m_ExchangeData.emplace(make_pair("CFFEX", g_pLanguageApi->LangText(CFFEX)));
}
KListView::~KListView()
{

}
void KListView::CreateView(HWND hParent, RECT r)
{
	m_hParent = hParent;
	PrepareRect(r);
	PrepareSubData();
	CreateFrm(L"cxp_KListView", hParent, WS_VISIBLE | WS_CHILD);
	SetWindowPos(m_Hwnd, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);

	m_scrollRect.left = m_frameRect.right - 10;
	m_scrollRect.right = m_frameRect.right - 4;
	m_scrollRect.top = m_iHeaderHeight + 5;
	m_scrollRect.bottom = m_scrollRect.top + 120;
}

void KListView::SetTotalData(ListTotalData& data)
{
	if (m_totalData.size() > 0) {
		return;
	}
	m_totalData = data;
	PrepareHeaderTotalData();
	PrepareItemTotalData();
}

void KListView::PrepareHeaderTotalData()
{
	KListViewHeaderData data;
	data.index = 0;
	wcsncpy_s(data.plateName, g_pLanguageApi->LangText(InternalID),sizeof(data.plateName));
	m_headerData.push_back(data);

	data.index = 1;
	wcsncpy_s(data.plateName, g_pLanguageApi->LangText(ExternalID), sizeof(data.plateName));
	m_headerData.push_back(data);
}
void KListView::PrepareItemTotalData()
{
	if (m_iheaderSelectIndex < m_totalData.size()) 
	{
		m_itemData = m_totalData[m_iheaderSelectIndex];
	}
}

LRESULT KListView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		OnSize(wParam, lParam);
		break;
	case WM_PAINT:
		OnPaint();
		break;
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
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		break;
	case WM_SETFOCUS:
		OnDealSetFocus();
		break;
	default:
		break;
	}
	return NOT_PROCESSED;
}

void KListView::OnDealSetFocus()
{
	InvalidateRect(m_Hwnd, NULL, TRUE);
}

void KListView::OnSize(WPARAM wParam, LPARAM lParam)
{
	m_iWidth = GET_X_LPARAM(lParam);
	m_iHeight = GET_Y_LPARAM(lParam);
	RECT client = {};
	GetClientRect(m_Hwnd, &client);
	PrepareRect(client);
	PrepareSubData();
}

void KListView::OnPaint()
{
	TMemDC dc(m_Hwnd);
	KLineSaveDC Save(dc);
	SetBkMode(dc.GetHdc(), TRANSPARENT);
	HBRUSH bkBrush = CreateSolidBrush(m_bkColor);
	HBRUSH frameBrush = CreateSolidBrush(m_frameColor);
	HBRUSH selectBrush = CreateSolidBrush(m_selectColor);
	HPEN framePen = CreatePen(PS_SOLID, 1, m_frameColor);

	HBRUSH scrollBrush = CreateSolidBrush(m_scrollColor);

	SelectObject(dc.GetHdc(), framePen);
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord13());

	//frame
	FillRect(dc.GetHdc(), &m_frameRect, bkBrush);

	//Select
	if (m_itemSelectIndex < m_vtItemRect.size()) 
	{
		FillRect(dc.GetHdc(), &m_vtItemRect[m_itemSelectIndex], selectBrush);
		SetTextColor(dc.GetHdc(), m_selectTextColor);

		RECT r = m_vtItemRect[m_itemSelectIndex];
		r.left += 15;

		auto IterEx = m_ExchangeData.find(m_vtShowItemData[m_itemSelectIndex]);
		if (IterEx != m_ExchangeData.end())
		{
			DrawText(dc.GetHdc(), IterEx->second.c_str(), wcslen(IterEx->second.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
	}

	if (m_iheaderSelectIndex < m_vtHeaderRect.size()) 
	{
		FillRect(dc.GetHdc(), &m_vtHeaderRect[m_iheaderSelectIndex], selectBrush);
		SetTextColor(dc.GetHdc(), m_selectTextColor);
		DrawText(dc.GetHdc(), m_headerData[m_iheaderSelectIndex].plateName, wcslen(m_headerData[m_iheaderSelectIndex].plateName), 
			&m_vtHeaderRect[m_iheaderSelectIndex], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	//header;
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord14());
	SetTextColor(dc.GetHdc(), m_headerColor);
	for (int index = 0; index < m_vtHeaderRect.size(); index++) 
	{
		if (m_iheaderSelectIndex == index)
		{
			continue;
		}
		else
		{
			FrameRect(dc.GetHdc(), &m_vtHeaderRect[index], frameBrush);
			DrawText(dc.GetHdc(), m_headerData[index].plateName, wcslen(m_headerData[index].plateName), &m_vtHeaderRect[index], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	//item
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord13());
	SetTextColor(dc.GetHdc(), m_textColor);
	for (int index = 0 ; index < m_vtItemRect.size(); index++)
	{
		if (index < m_vtShowItemData.size()) 
		{
			if (m_itemSelectIndex == index)
			{
				continue;
			}
			RECT r = m_vtItemRect[index];
			r.left += 15;
			FrameRect(dc.GetHdc(), &m_vtItemRect[index], frameBrush);
			auto IterEx = m_ExchangeData.find(m_vtShowItemData[index]);
			if (IterEx != m_ExchangeData.end())
			{
				DrawText(dc.GetHdc(), IterEx->second.c_str(), wcslen(IterEx->second.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			}
			
		}
	}

	//Hover
	if (m_itemHoverIndex >= 0) 
	{
		DrawFocusRect(dc.GetHdc(), &m_vtItemRect[m_itemHoverIndex]);
	}

	if (m_iheaderHoverIndex >= 0) 
	{
		DrawFocusRect(dc.GetHdc(), &m_vtHeaderRect[m_iheaderHoverIndex]);
	}

	//scroll
	if (m_itemData.size() > m_iShowCount)
	{
		m_scrollRect.left = m_frameRect.right - 10;
		m_scrollRect.right = m_frameRect.right - 4;

		FillRect(dc.GetHdc(), &m_scrollRect, scrollBrush);
	}

	DeleteObject(bkBrush);
	DeleteObject(frameBrush);
	DeleteObject(framePen);
	DeleteObject(scrollBrush);
	DeleteObject(selectBrush);	
}

void KListView::PrepareRect(RECT r) 
{
	m_iHeight = r.bottom - r.top;
	m_iWidth = r.right - r.left;
	m_frameRect = { 0, 0, m_iWidth, m_iHeight };
	m_iItemTotalHeight = m_iHeight - m_iHeaderHeight;

	PrepareHeaderRect();
	PrepareItemRect();
}

void KListView::PrepareItemRect()
{
	//初始化ItemRect
	m_vtItemRect.clear();
	m_iShowCount = m_iItemTotalHeight / m_iItemHeight > m_itemData.size() ? m_itemData.size() : m_iItemTotalHeight / m_iItemHeight;
	m_itemRect = { 0 ,m_iHeaderHeight, m_iWidth, m_iHeight };
	RECT r = { 0, m_iHeaderHeight, m_iWidth , m_iItemHeight + m_iHeaderHeight };
	for (int index = 0; index < m_iShowCount; index++)
	{
		m_vtItemRect.push_back(r);
		r.top = r.bottom;
		r.bottom = r.top + m_iItemHeight;
	}
}
void KListView::PrepareHeaderRect()
{
	//初始化HeaderRect
	m_vtHeaderRect.clear();
	m_headerRect = { 0, 0 , m_iWidth, m_iHeaderHeight };
	int iHeaderWidth = m_iWidth / m_headerData.size();
	RECT r = { 0, 0, iHeaderWidth, m_iHeaderHeight };
	for (int index = 0; index < m_headerData.size(); index++)
	{
		m_vtHeaderRect.push_back(r);
		r.left = r.right;
		r.right = r.left + iHeaderWidth;
	}
}
bool KListView::GetShowIndex(int change)
{
	int begin = m_itemShowBegin - change;
	if (begin < 0) 
	{
		return false;
	}
	else if ((begin + m_iShowCount) > m_itemData.size()) 
	{
		begin = m_itemData.size() - m_iShowCount;
		m_itemShowBegin = begin;
	}
	else 
	{
		m_itemShowBegin = begin;
	}
	return true;
}
void KListView::PrepareSubData()
{
	m_vtShowItemData.clear();
	for (int index = m_itemShowBegin ; index < (m_iShowCount + m_itemShowBegin) && index < m_itemData.size(); index++)
	{
		m_vtShowItemData.push_back(m_itemData[index]);
	}
}

void KListView::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	TrackMouse();
	SetFocus(m_Hwnd);
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	if (GetHoverIndex(pt)) 
	{
		if (m_itemHoverIndex >= 0 || m_iheaderHoverIndex >= 0) 
		{
			InvalidateRect(m_Hwnd, 0, false);
		}
	}
}

void KListView::OnMouseLeave()
{
	m_bMousetrack = false;
	m_iheaderHoverIndex = -1;
	m_itemHoverIndex = -1;
	InvalidateRect(m_Hwnd, 0, false);
}

void KListView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);

	if (PtInRect(&m_headerRect, point)) 
	{
		m_iheaderSelectIndex = m_iheaderHoverIndex;
		m_itemShowBegin = 0;
		PrepareItemTotalData();
		PrepareItemRect();
		PrepareSubData();
		
		//切换Tab之后默认选择第一个item
		m_itemSelectIndex = 0;
		m_ClickItem(m_vtShowItemData[m_itemSelectIndex]);

		//针对滑块做处理
		m_scrollRect.left = m_frameRect.right - 10;
		m_scrollRect.right = m_frameRect.right - 4;
		m_scrollRect.top = m_iHeaderHeight + 5;
		m_scrollRect.bottom = m_scrollRect.top + 120;
		m_bIsMoveItem = false;

		InvalidateRect(m_Hwnd, NULL, false);
	}
	else if (PtInRect(&m_itemRect, point)) 
	{
		m_itemSelectIndex = m_itemHoverIndex;
		InvalidateRect(m_Hwnd, NULL, false);
		if (m_itemSelectIndex < m_vtShowItemData.size()) 
		{
			m_ClickItem(m_vtShowItemData[m_itemSelectIndex]);
		}
	}
}

void KListView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
}

void KListView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	int line = delta / 120;

	InvalidateRect(m_Hwnd, &m_itemRect, false);
}

void KListView::TrackMouse()
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

bool KListView::GetHoverIndex(POINT point)
{
	bool update = false;
	if (PtInRect(&m_headerRect, point)) 
	{
		for (int index = 0 ; index < m_vtHeaderRect.size(); index++)
		{
			if (PtInRect(&m_vtHeaderRect[index], point)) 
			{
				m_iheaderHoverIndex = index;
				update = true;
				break;
			}
		}
	}
	else if (PtInRect(&m_itemRect, point))
	{
		for (int index = 0; index < m_vtItemRect.size(); index++)
		{
			if (PtInRect(&m_vtItemRect[index], point)) 
			{
				m_itemHoverIndex = index;
				update = true;
				break;
			}
		}
	}
	return update;
}