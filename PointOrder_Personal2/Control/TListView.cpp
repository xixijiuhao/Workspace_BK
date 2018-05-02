#include "PreInclude.h"

#define dafaultColor   RGB(10,10,10)
TListView::TListView()
{
	m_hbr[0] = CreateSolidBrush(g_ColorRefData.g_ColorBlue);
	m_hbr[1] = CreateSolidBrush(g_ColorRefData.g_ColorWhite);
	m_hbr[2] = CreateSolidBrush(g_ColorRefData.g_ColorRGB150);

	m_vItemColor.push_back(dafaultColor);
}
TListView::~TListView()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_hbr[i])
			DeleteObject(m_hbr[i]);
	}
}
void TListView::CreateList(HWND hParent, UINT style, int itemHeight, int headerHeight, UINT exStyle, int index)
{
	style |= WS_CHILD | WS_VISIBLE | LVS_NOSORTHEADER | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	exStyle |= LVS_EX_DOUBLEBUFFER;
	m_hwndList = CreateWindowEx(0, WC_LISTVIEW, 0, style, 0, 0, 0, 0, 
		hParent, HMENU(index), GetModuleHandle(NULL), 0);
	ListView_SetExtendedListViewStyleEx(m_hwndList, exStyle, exStyle);
	m_hwndHeader = ListView_GetHeader(m_hwndList);
	m_iHeaderHeight = headerHeight;
	m_iItemHeight = itemHeight;
	SetWindowSubclass(m_hwndHeader, TListView::NewHeaderProc, 0, DWORD_PTR(this));
}

void TListView::MoveWindow(int x, int y, int cx, int cy)
{
	SetWindowPos(m_hwndList, 0, x, y, cx, cy, SWP_NOZORDER);
}
void TListView::OnDrawItem(LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = LPDRAWITEMSTRUCT(lParam);
	bool bSetTextColor = true;
	if (pDIS->itemState&(ODS_SELECTED | ODS_FOCUS))
	{
		FillRect(pDIS->hDC, &pDIS->rcItem, m_hbr[0]); //ѡ��ʱ����ɫ
		SetTextColor(pDIS->hDC, RGB(255, 255, 255));
		bSetTextColor = false;
	}
	else
	{
		FillRect(pDIS->hDC, &pDIS->rcItem, m_hbr[1]); //δѡ��ʱ����ɫ
	}
	LVITEMA item = { 0 };
	char szText[260] = { 0 };
	item.mask = LVIF_TEXT | LVIF_IMAGE;
	item.cchTextMax = 260;
	item.pszText = szText;
	item.iItem = pDIS->itemID;

	RECT rcHeader = { 0 };
	int iBegin = 0;
	int iEnd = 0;
	FindBeginEnd(pDIS->rcItem.left, pDIS->rcItem.right, iBegin, iEnd);

	RECT rTemp = pDIS->rcItem;
	if (pDIS->itemID != 0)
		rTemp.top -= 1;

	for (int i = iBegin; i <= iEnd; i++)
	{
		item.iSubItem = i;
		SendMessageA(m_hwndList, LVM_GETITEMA, 0, (LPARAM)(LV_ITEMA*)(&item));
		Header_GetItemRect(m_hwndHeader, i, &rcHeader);
		rTemp.left = pDIS->rcItem.left + rcHeader.left;
		rTemp.right = rTemp.left + rcHeader.right - rcHeader.left;
		//if (i != 0)
		rTemp.left -= 1;

		FrameRect(pDIS->hDC, &rTemp, m_hbr[2]); //���ο�
		rTemp.left += 2;
		if (strlen(szText))
		{
			UINT colorIndex = 0;
			UINT textFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
			if (pDIS->itemID < m_vItemState.size() && i < m_vItemState[pDIS->itemID].size())
			{
				colorIndex = m_vItemState[pDIS->itemID][i] & 0x00FF;
				textFormat |= (m_vItemState[pDIS->itemID][i] & 0xFF00) >> 8;
			}
			if (bSetTextColor)
				SetTextColor(pDIS->hDC, m_vItemColor[colorIndex]);
			DrawText(pDIS->hDC, LoadRC::ansi_to_unicode(szText).c_str(), -1, &rTemp, textFormat);
		}

	}
}

void TListView::FindBeginEnd(int left, int right, int &iBegin, int &iEnd)
{
	RECT rcHeader = { 0 };
	Header_GetItemRect(m_hwndHeader, iBegin, &rcHeader);
	while (rcHeader.right < abs(left))
		Header_GetItemRect(m_hwndHeader, ++iBegin, &rcHeader); //�ҵ������Ӧ����ʾ��λ��
	RECT clientRect = { 0 };
	GetClientRect(m_hwndList, &clientRect);
	iEnd = iBegin;
	if (clientRect.right > right)
		iEnd = Header_GetItemCount(m_hwndHeader) - 1; //ѭ������С�ڵ��ڣ����Լ�һ
	else
		while (rcHeader.right < clientRect.right + abs(left) - 1) //�ҵ����ұ�Ҫ��ʾ��λ��
			Header_GetItemRect(m_hwndHeader, ++iEnd, &rcHeader);
}
void TListView::OnMeasureItem(UINT &height)
{
	height = m_iItemHeight;
}
void TListView::AddColumn(const TCHAR *pszText, int width, int index)
{
	LVCOLUMN stColumn;
	memset(&stColumn, 0, sizeof(LVCOLUMN));
	stColumn.mask = LVCF_TEXT |LVCF_WIDTH;
	stColumn.pszText = const_cast<TCHAR*>(pszText);
	stColumn.cx = width;
	if (index == -1)
		index = Header_GetItemCount(m_hwndHeader);
	ListView_InsertColumn(m_hwndList, index, &stColumn);
}
void TListView::AddItem(double data, int iRow, int iCol, UINT textFormat, COLORREF textColor)
{
	char szText[100] = { 0 };
	sprintf_s(szText, "%.2f", data);
	AddItem(szText, iRow, iCol, textFormat, textColor);
}
void TListView::AddItem(int data, int iRow, int iCol, UINT textFormat, COLORREF textColor)
{
	char szText[100] = { 0 };
	sprintf_s(szText, "%d", data);
	AddItem(szText, iRow, iCol, textFormat, textColor);
}
void TListView::AddItem(const char *pszText, int iRow, int iCol, UINT textFormat, COLORREF textColor)
{
	if (!pszText)return;
	LVITEMA item;
	memset(&item, 0, sizeof(LVITEMA));
	item.mask = LVIF_TEXT;	
	item.cColumns = Header_GetItemCount(m_hwndHeader);
	item.iItem = iRow;
	item.iSubItem = iCol;
	item.pszText = const_cast<char*>(pszText);
	if (iCol == 0)
		SendMessageA(m_hwndList, LVM_INSERTITEMA, 0, (LPARAM)(&item));
	else
		SendMessageA(m_hwndList, LVM_SETITEMA, 0, (LPARAM)(&item));

	SetItemState(iRow, iCol, textFormat, textColor);
}
void TListView::SetItemState(int iRow, int iCol, UINT textFormat, COLORREF textColor)
{
	int colorIndex = 0;
	if (textColor != -1)
	{
		bool colorExisted = false;
		for (UINT i = 0; i < m_vItemColor.size(); i++)
		{
			if (textColor == m_vItemColor[i])
			{
				colorIndex = i;
				colorExisted = true;
				break;
			}
		}
		if (!colorExisted)
		{
			colorIndex = m_vItemColor.size();
			m_vItemColor.push_back(textColor);
		}
	}
	unsigned short style = (textFormat << 8) | colorIndex;
	if (iCol == 0 && iRow < m_vItemState.size())//���Ѿ����ڵ�������ӣ�����������������ƶ�
	{ //���m_vItemState����0��˵���Ѿ����й����룬�����ڵ�0�в��룬�����л������������ƶ������������styleҪ������ǰ��
		vector<unsigned short>oneRow;
		oneRow.push_back(style);
		m_vItemState.insert(m_vItemState.begin() + iRow, oneRow); //���뵽��Ӧ��λ����
		return;
	}
	if (iRow < m_vItemState.size()) //���Ѿ�����
	{
		if (iCol < m_vItemState[iRow].size()) //���Ѿ�����
			m_vItemState[iRow][iCol] = style;
		else
			m_vItemState[iRow].push_back(style);
	}
	else
	{
		vector<unsigned short>oneRow;
		oneRow.push_back(style);
		m_vItemState.push_back(oneRow);
	}
}
void TListView::SelectItem(int index)
{
	ListView_Scroll(m_hwndList, 0, index*m_iItemHeight);
	ListView_SetItemState(m_hwndList, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}
LRESULT TListView::NewHeaderProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data)
{
	TListView *p = (TListView*)data;
	switch (msg)
	{
	case HDM_LAYOUT:
		p->OnHeaderLayout(wParam, lParam);
		return 0;
	case WM_PAINT:
		p->OnHeaderPaint();
		return 0;
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
}

void TListView::OnHeaderPaint()
{
	TMemDC memdc(m_hwndHeader);
	RECT rect = { 0 };
	LVCOLUMN col = { 0 };
	TCHAR strTemp[50] = { 0 };
	col.mask = LVCF_TEXT;
	col.pszText = strTemp;
	col.cchTextMax = sizeof(strTemp)/sizeof(TCHAR);
	int count = Header_GetItemCount(m_hwndHeader);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);
	SetTextColor(memdc.GetHdc(), RGB(255, 255, 255));
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord12);

	GetClientRect(m_hwndHeader, &rect);
	HBRUSH hbrBorder = CreateSolidBrush(g_ColorRefData.g_ColorRGB150);
	FillRect(memdc.GetHdc(), &rect, hbrBorder);
	HPEN  hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	SelectObject(memdc.GetHdc(), hPen);
	
	for (int i = 0; i < count; i++)
	{
		Header_GetItemRect(m_hwndHeader, i, &rect);
		ListView_GetColumn(m_hwndList, i, &col);
		if (i > 0)
		{
			MoveToEx(memdc.GetHdc(), rect.left -1, rect.top, 0);
			LineTo(memdc.GetHdc(), rect.left -1, rect.bottom);
		}
		DrawText(memdc.GetHdc(), col.pszText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	DeleteObject(hbrBorder);
	DeleteObject(hPen);
}
void TListView::OnHeaderLayout(WPARAM wParam, LPARAM lParam)
{
	LPHDLAYOUT pHdl = LPHDLAYOUT(lParam);
	LPRECT pRect = pHdl->prc;
	LPWINDOWPOS pWpos = pHdl->pwpos;
	DefSubclassProc(m_hwndHeader, HDM_LAYOUT, wParam, lParam);
	pWpos->cy = m_iHeaderHeight; //header�߶�
	pRect->top = pWpos->cy;
}
void TListView::SetColor(COLORREF selectColor, COLORREF unSelectColor, COLORREF gridColor)
{
	for (int i = 0; i < 3; i++)
	{
		if (m_hbr[i])
			DeleteObject(m_hbr[i]);
	}
	m_hbr[0] = CreateSolidBrush(selectColor);
	m_hbr[1] = CreateSolidBrush(unSelectColor);
	m_hbr[2] = CreateSolidBrush(gridColor);
}
void TListView::SetTxtColor(COLORREF textColor)
{
	if (m_vItemColor.size())
		m_vItemColor[0] = textColor;
	else
		m_vItemColor.push_back(textColor);
}
int TListView::GetItemCount()
{
	return ListView_GetItemCount(m_hwndList);
}
void TListView::DeleteItem(int iItem)
{
	ListView_DeleteItem(m_hwndList, iItem);
	if (iItem<m_vItemState.size())
		m_vItemState.erase(m_vItemState.begin() + iItem);
}
void TListView::DeleteAllItems()
{
	ListView_DeleteAllItems(m_hwndList);
	m_vItemColor.clear();
	m_vItemState.clear();
	m_vItemColor.push_back(dafaultColor);
}

int TListView::GetSelectItem(char* ptext, int nlength, int iSubitem)
{
	LV_ITEMA item;
	int index = ListView_GetSelectionMark(m_hwndList);
	if (index < 0)
		return -1;
	memset(&item, 0, sizeof(LV_ITEMA));
	item.pszText = ptext;
	item.cchTextMax = nlength - 1;
	item.mask = LVIF_TEXT;
	item.iItem = index;
	item.iSubItem = iSubitem;
	SendMessageA(m_hwndList, LVM_GETITEMA, 0, (LPARAM)(LV_ITEMA*)(&item));
	return index;
}
int TListView::GetSelectItem()
{
	return ListView_GetSelectionMark(m_hwndList);
}