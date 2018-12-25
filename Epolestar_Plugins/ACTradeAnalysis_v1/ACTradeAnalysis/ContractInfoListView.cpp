#include "BaseInclude.h"

#define COLNUM      9
#define SYMBOLCOL   1
#define PATTERNCOL  4
#define TIMECOL     5

extern ILanguageApi* g_pLanguageApi;

CContractInfoListView::CContractInfoListView() :
	m_itemSelectIndex(0),
	m_iheaderSelectIndex(0),
	m_itemShowBegin(0),
	m_itemHoverIndex(-1),
	m_iheaderHoverIndex(-1),
	m_iHeaderHeight(30),
	m_iItemHeight(25)
{
	m_textColor = RGB(200, 200, 200);
	m_bkColor = RGB(38, 38, 46);
	m_frameColor = RGB(19, 19, 26);
	m_headerColor = RGB(200, 200, 200);
	m_scrollColor = RGB(10, 10, 10);
	m_selectColor = RGB(28, 28, 36);
	m_selectTextColor = RGB(255, 60, 56);
	m_bIsMoveItem = false;

	m_PatternDataMap.emplace(make_pair("Falling Wedge", g_pLanguageApi->LangText(FallingWedge)));
	m_PatternDataMap.emplace(make_pair("Pennant", g_pLanguageApi->LangText(Pennant)));
	m_PatternDataMap.emplace(make_pair("Rising Wedge", g_pLanguageApi->LangText(RisingWedge)));
	m_PatternDataMap.emplace(make_pair("Channel Down", g_pLanguageApi->LangText(ChannelDown)));

	m_PatternDataMap.emplace(make_pair("Triangle", g_pLanguageApi->LangText(Triangle)));
	m_PatternDataMap.emplace(make_pair("Flag", g_pLanguageApi->LangText(Flag)));
	m_PatternDataMap.emplace(make_pair("Rectangle", g_pLanguageApi->LangText(RectanglePatter)));
	m_PatternDataMap.emplace(make_pair("Ascending Triangle", g_pLanguageApi->LangText(AscendingTriangle)));

	m_PatternDataMap.emplace(make_pair("Channel Up", g_pLanguageApi->LangText(ChannelUp)));
	m_PatternDataMap.emplace(make_pair("3 Point Extension", g_pLanguageApi->LangText(ThreePointExtension)));
	m_PatternDataMap.emplace(make_pair("ABCD", g_pLanguageApi->LangText(ABCD)));
	m_PatternDataMap.emplace(make_pair("Gartley", g_pLanguageApi->LangText(Gartley)));

	m_PatternDataMap.emplace(make_pair("Resistance", g_pLanguageApi->LangText(Resistance)));
	m_PatternDataMap.emplace(make_pair("Support", g_pLanguageApi->LangText(Support)));
	m_PatternDataMap.emplace(make_pair("KeyLevelsPattern", g_pLanguageApi->LangText(KeyLevels)));
	m_PatternDataMap.emplace(make_pair("FibonacciPattern", g_pLanguageApi->LangText(FibonacciPattern)));

	m_PatternDataMap.emplace(make_pair("ChartPattern", g_pLanguageApi->LangText(ChartPattern)));
}
CContractInfoListView::~CContractInfoListView()
{

}
void CContractInfoListView::CreateView(HWND hParent, RECT r)
{
	m_hParent = hParent;
	CreateFrm(L"cxp_ContractInfoView", hParent, WS_VISIBLE | WS_CHILD);
	SetWindowPos(m_Hwnd, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);

	m_scrollRect.left = m_frameRect.right - 10;
	m_scrollRect.right = m_frameRect.right - 4;
	m_scrollRect.top = m_iHeaderHeight + 5;
	m_scrollRect.bottom = m_scrollRect.top + 120;
}

void CContractInfoListView::SetTotalData(ACDataMap& acDataMap)
{
	if (acDataMap.size() == 0)
	{
		return;
	}
	PrepareHeaderTotalData();
	PrepareItemTotalData(acDataMap);

	RECT client = {};
	GetClientRect(m_Hwnd, &client);
	PrepareRect(client);
	PrepareSubData();

	InvalidateRect(m_Hwnd, 0, false);
}

void CContractInfoListView::PrepareHeaderTotalData()
{
	m_headerData.clear();
	wstring HeadText[COLNUM] = {
		g_pLanguageApi->LangText(Exchange),
		g_pLanguageApi->LangText(Symbol),
		g_pLanguageApi->LangText(Direction),
		g_pLanguageApi->LangText(Interval),

		g_pLanguageApi->LangText(Pattern),
		g_pLanguageApi->LangText(IdentifiedTime),
		g_pLanguageApi->LangText(Quality),
		g_pLanguageApi->LangText(Type),

		g_pLanguageApi->LangText(Length)
	};

	ContractInfoHeaderData data;
	for (int i = 0; i < COLNUM; i++) 
	{
		data.index = i;
		wcsncpy_s(data.dataName, HeadText[i].c_str(), sizeof(data.dataName));
		m_headerData.push_back(data);
	}
}

void CContractInfoListView::InsertSymbolContractMap(string symbol)
{
	for (auto iter = m_clPresenter->m_mapSymbolNameContractInfo.begin(); iter != m_clPresenter->m_mapSymbolNameContractInfo.end(); ++iter)
	{
		int spos = symbol.find(iter->first);
		if (spos == string::npos)
			continue;

		int ypos = symbol.find(iter->second.years);
		if (ypos == string::npos)
			continue;

		int mpos = symbol.find(iter->second.months);
		if (mpos == string::npos)
			continue;
		
		m_clPresenter->m_mapSymbolContract.emplace(make_pair(symbol, iter->second.SubContractNo));
	}
}

string CContractInfoListView::GetSymbolName(string symbol)
{
	string name = "";
	int pos = symbol.find('_');
	if (pos == string::npos)
		return "";

	symbol = symbol.substr(pos + 1, string::npos);
	int rpos = symbol.rfind('_');
	if (rpos == string::npos)
		return "";

	name = symbol.substr(0, rpos);
	return name;
}

void CContractInfoListView::PrepareItemTotalData(ACDataMap& data)
{
	m_itemData.clear();

	ContractInfoViewItemData contractData;
	ACPatternParam paramData;
	int i = 0;
	for (auto iter = data.begin(); iter != data.end(); ++iter,++i)
	{
		paramData = iter->second.m_PatternParam;
		contractData.index = i;

		auto iter2 = m_clPresenter->m_mapSymbolContract.find(paramData.symbol);
		if (iter2 != m_clPresenter->m_mapSymbolContract.end())
		{
			strcpy_s(contractData.contractCode, iter2->second.c_str());
		}
		else
		{
			continue;
		}

		contractData.resultuid = iter->second.m_nResultuid;
		strcpy_s(contractData.mainPattern, iter->second.m_sACPattern);
		strcpy_s(contractData.exchange, paramData.exchange);
		strcpy_s(contractData.symbol, paramData.symbol);
		strcpy_s(contractData.pattern, paramData.pattern);
		strcpy_s(contractData.interval, to_string(paramData.interval).c_str());
		strcpy_s(contractData.length, to_string(paramData.length).c_str());

		contractData.direction = paramData.direction;
		contractData.quality = paramData.quality;
		contractData.identified_time = paramData.identified_time;
		contractData.completed = paramData.completed;

		m_itemData.push_back(contractData);
	}
}

LRESULT CContractInfoListView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void CContractInfoListView::OnDealSetFocus()
{
	InvalidateRect(m_Hwnd, NULL, TRUE);
}

void CContractInfoListView::OnSize(WPARAM wParam, LPARAM lParam)
{
	m_iWidth = GET_X_LPARAM(lParam);
	m_iHeight = GET_Y_LPARAM(lParam);
	RECT client = {};
	GetClientRect(m_Hwnd, &client);
	PrepareRect(client);
	PrepareSubData();
}

void CContractInfoListView::OnPaint()
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

	//If Selected
	if (m_itemSelectIndex < m_vtItemRect.size())
	{
		FillRect(dc.GetHdc(), &m_vtItemRect[m_itemSelectIndex], selectBrush);
	}

	//header;
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord14());
	//FrameRect(dc.GetHdc(), &m_headerRect, frameBrush);
	SetTextColor(dc.GetHdc(), m_headerColor);
	for (int index = 0; index < m_vtHeaderRect.size(); index++)
	{
		FrameRect(dc.GetHdc(), &m_vtHeaderRect[index], frameBrush);
		DrawText(dc.GetHdc(), m_headerData[index].dataName, wcslen(m_headerData[index].dataName), &m_vtHeaderRect[index], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	//item
	SelectObject(dc.GetHdc(), g_FontData.GetFontWord13());
	SetTextColor(dc.GetHdc(), m_textColor);
	for (int index = 0; index < m_vtItemRect.size(); index++)
	{
		if (index < m_vtShowItemData.size())
		{
			DrawItemText(dc, index, frameBrush);
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

void CContractInfoListView::DrawItemText(TMemDC &dc, int &index, HBRUSH &frameBrush)
{
	FrameRect(dc.GetHdc(), &m_vtItemRect[index], frameBrush);

	RECT r = m_vtItemRect[index];
	string itemText = "";

	r.left = m_vtHeaderRect[0].left + 5;
	r.right = m_vtHeaderRect[0].right;
	DrawTextA(dc.GetHdc(), m_vtShowItemData[index].exchange, strlen(m_vtShowItemData[index].exchange), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	r.left = m_vtHeaderRect[1].left + 5;
	r.right = m_vtHeaderRect[1].right;
	
	SContractNameType tempName = { 0 };
	SContractNoType contractNo = { 0 };
	auto IterC = m_clPresenter->m_mapSymbolContract.find(m_vtShowItemData[index].symbol);
	if (IterC != m_clPresenter->m_mapSymbolContract.end())
	{
		strcpy(contractNo, IterC->second.c_str());
		g_pQuoteApi->GetContractName(contractNo, tempName);
		wstring wContractName = CConvert::UTF8ToUnicode(tempName);
		DrawText(dc.GetHdc(), wContractName.c_str(), wcslen(wContractName.c_str()), &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		//DrawTextA(dc.GetHdc(), m_vtShowItemData[index].symbol, strlen(m_vtShowItemData[index].symbol), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	
	r.left = m_vtHeaderRect[2].left + 5;
	r.right = m_vtHeaderRect[2].right;

	HPEN greenPen = CreatePen(PS_SOLID, 1, KLineGreenColor);
	HPEN redPen = CreatePen(PS_SOLID, 1, KLineRedColor);
	
	if (m_vtShowItemData[index].direction == 1)
	{
		SelectObject(dc.GetHdc(), redPen);
		itemText = "     up";
		HBRUSH bRedBrush = CreateSolidBrush(KLineRedColor);
		SelectBrush(dc.GetHdc(), bRedBrush);
		POINT gPoint[4] = { { r.left + 5, r.top + 5 }, { r.left, r.top + 15 }, { r.left + 10, r.top + 15 }, { r.left + 5, r.top + 5 } };
		Polygon(dc.GetHdc(), gPoint, 4);
		DeleteObject(bRedBrush);
	}
	else
	{
		SelectObject(dc.GetHdc(), greenPen);
		itemText = "     down";
		HBRUSH bGreenBrush = CreateSolidBrush(KLineGreenColor);
		SelectBrush(dc.GetHdc(), bGreenBrush);
		POINT gPoint[4] = { { r.left , r.top + 5 }, { r.left + 10, r.top + 5 }, { r.left + 5, r.top + 15 }, { r.left, r.top + 5 } };
		Polygon(dc.GetHdc(), gPoint, 4);
		DeleteObject(bGreenBrush);
	}
	DrawTextA(dc.GetHdc(), itemText.c_str(), strlen(itemText.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	r.left = m_vtHeaderRect[3].left + 5;
	r.right = m_vtHeaderRect[3].right;
	itemText = m_vtShowItemData[index].interval;
	DrawTextA(dc.GetHdc(), itemText.c_str(), strlen(itemText.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	r.left = m_vtHeaderRect[4].left + 5;
	r.right = m_vtHeaderRect[4].right;
	auto mainPatternIter = m_PatternDataMap.find(m_vtShowItemData[index].mainPattern);
	auto sonPatternIter = m_PatternDataMap.find(m_vtShowItemData[index].pattern);
	wstring partternText = L"";
	if (sonPatternIter != m_PatternDataMap.end())
	{
		partternText = mainPatternIter->second + L" | " + sonPatternIter->second;
	}
	else
	{
		partternText = mainPatternIter->second + L" | " + LoadRC::ansi_to_unicode(m_vtShowItemData[index].pattern);
	}
	DrawText(dc.GetHdc(), partternText.c_str(), wcslen(partternText.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	r.left = m_vtHeaderRect[5].left + 5;
	r.right = m_vtHeaderRect[5].right;
	itemText = KLineUtil::UnixTime2Str(m_vtShowItemData[index].identified_time);
	DrawTextA(dc.GetHdc(), itemText.c_str(), strlen(itemText.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	r.left = m_vtHeaderRect[6].left + 5;
	r.right = m_vtHeaderRect[6].right;

	itemText = "                " + to_string(m_vtShowItemData[index].quality);
	HBRUSH bBuleBrush = CreateSolidBrush(KlineBlueColor);
	SelectBrush(dc.GetHdc(), bBuleBrush);
	RECT qr = { r.left + 5, r.top + 10 , r.left + 10 , r.top + 18 };
	for (int i = 0 ;i < m_vtShowItemData[index].quality;++i)
	{
		FillRect(dc.GetHdc(), &qr, bBuleBrush);
		qr.left += 6;
		qr.right = qr.left + 5;
	}
	DrawTextA(dc.GetHdc(), itemText.c_str(), strlen(itemText.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	r.left = m_vtHeaderRect[7].left + 5;
	r.right = m_vtHeaderRect[7].right;
	wstring completedText = L"";
	if (m_vtShowItemData[index].completed == 1)
	{
		completedText = g_pLanguageApi->LangText(Completed);
	}
	else
	{
		completedText = g_pLanguageApi->LangText(Emerging);
	}
	DrawText(dc.GetHdc(), completedText.c_str(), wcslen(completedText.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	r.left = m_vtHeaderRect[8].left + 5;
	r.right = m_vtHeaderRect[8].right;
	itemText = m_vtShowItemData[index].length;
	DrawTextA(dc.GetHdc(), itemText.c_str(), strlen(itemText.c_str()), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	DeleteObject(bBuleBrush);
	DeleteObject(greenPen);
	DeleteObject(redPen);
}

void CContractInfoListView::PrepareRect(RECT r)
{
	m_iHeight = r.bottom - r.top;
	m_iWidth = r.right - r.left;
	m_frameRect = { 0, 0, m_iWidth, m_iHeight };
	m_iItemTotalHeight = m_iHeight - m_iHeaderHeight;

	PrepareHeaderRect();
	PrepareItemRect();
}

void CContractInfoListView::PrepareItemRect()
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
void CContractInfoListView::PrepareHeaderRect()
{
	//初始化HeaderRect
	m_vtHeaderRect.clear();
	m_headerRect = { 0, 0 , m_iWidth, m_iHeaderHeight };
	int iMidWidth = 120;
	int iNarrowWidth = 80;
	int iWideWidth = (m_iWidth - iMidWidth * 3) / (m_headerData.size() - 3);;

	RECT r = { 0, 0, 0, m_iHeaderHeight };
	//RECT r = { 0, 0, iMidWidth, m_iHeaderHeight };
	//m_vtHeaderRect.push_back(r);
	for (int index = 0; index < m_headerData.size(); ++index)
	{
		r.left = r.right;
		if (index == SYMBOLCOL || index == PATTERNCOL || index == TIMECOL)
		{
			r.right = r.left + iWideWidth;
		}
		else
		{
			r.right = r.left + iMidWidth;
		}	
		m_vtHeaderRect.push_back(r);
	}
}
bool CContractInfoListView::GetShowIndex(int change)
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
void CContractInfoListView::PrepareSubData()
{
	m_vtShowItemData.clear();
	for (int index = m_itemShowBegin; index < (m_iShowCount + m_itemShowBegin) && index < m_itemData.size(); index++) 
	{
		m_vtShowItemData.push_back(m_itemData[index]);
	}
}

void CContractInfoListView::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

void CContractInfoListView::OnMouseLeave()
{
	m_bMousetrack = false;
	m_iheaderHoverIndex = -1;
	m_itemHoverIndex = -1;
	InvalidateRect(m_Hwnd, 0, false);
}

void CContractInfoListView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);

	if (PtInRect(&m_itemRect, point)) 
	{
		m_itemSelectIndex = m_itemHoverIndex;
		if (m_itemSelectIndex < m_vtShowItemData.size()) 
		{
			m_ClickItem(m_vtShowItemData[m_itemSelectIndex]);
		}
	}
}

void CContractInfoListView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
}

void CContractInfoListView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	int line = delta / 120;
	if (GetShowIndex(line))
	{
		PrepareSubData();

		if ((m_iShowCount + m_itemShowBegin) < m_itemData.size())
		{
			m_itemSelectIndex += line;

			double tmpMidDiv = (double)(m_itemData.size() * m_iItemHeight - m_iItemTotalHeight) / (m_iItemTotalHeight - 120);
			int scrollOffset = (int)((double)delta / (tmpMidDiv * 4) + 0.5);

			m_scrollRect.top -= scrollOffset;
			m_scrollRect.bottom -= scrollOffset;
			m_bIsMoveItem = true;
		}
		if ((m_iShowCount + m_itemShowBegin) == m_itemData.size())
		{
			if (m_bIsMoveItem)
			{
				m_itemSelectIndex += line;
				m_bIsMoveItem = false;
			}
			m_scrollRect.bottom = m_frameRect.bottom;
			m_scrollRect.top = m_scrollRect.bottom - 120;
		}
	}
	else
	{
		m_scrollRect.top = m_iHeaderHeight + 5;
		m_scrollRect.bottom = m_scrollRect.top + 120;
	}

	InvalidateRect(m_Hwnd, &m_itemRect, false);
}

void CContractInfoListView::TrackMouse()
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

bool CContractInfoListView::GetHoverIndex(POINT point)
{
	bool update = false;
	if (PtInRect(&m_headerRect, point))
	{
		for (int index = 0; index < m_vtHeaderRect.size(); index++)
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