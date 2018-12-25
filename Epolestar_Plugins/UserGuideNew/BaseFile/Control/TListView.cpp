#include "BaseInclude.h"

#define dafaultColor   RGB(10,10,10)

#define maxFolatData   1.0e80
#define minFloatData   1.0e-15
TListView::TListView()
{
	m_hbr[0] = CreateSolidBrush(g_ColorRefData.g_ColorBlue);
	m_hbr[1] = CreateSolidBrush(g_ColorRefData.g_ColorWhite);
	m_hbr[2] = CreateSolidBrush(g_ColorRefData.g_ColorRGB150);

	m_vItemColor.push_back(dafaultColor);

	m_bVirtualMode = false;
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
	style |= WS_CHILD | WS_VISIBLE | LVS_NOSORTHEADER | LVS_SINGLESEL | LVS_SHOWSELALWAYS|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	exStyle |= LVS_EX_DOUBLEBUFFER|LVS_EX_GRIDLINES;
	m_hwndList = CreateWindowEx(0, WC_LISTVIEW, 0, style, 0, 0, 0, 0, 
		hParent, HMENU(index), GetModuleHandle(NULL), 0);
	ListView_SetExtendedListViewStyleEx(m_hwndList, exStyle, exStyle);
	m_hwndHeader = ListView_GetHeader(m_hwndList);
	m_iHeaderHeight = headerHeight;
	m_iItemHeight = itemHeight;

	if (!(style&LVS_NOCOLUMNHEADER))
		SetWindowSubclass(m_hwndHeader, TListView::NewHeaderProc, 0, DWORD_PTR(this));

	SetWindowSubclass(m_hwndList, TListView::NewListProc, 0, DWORD_PTR(this));

	m_bVirtualMode = (style&LVS_OWNERDATA) == LVS_OWNERDATA;
}
LRESULT CALLBACK TListView::NewListProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data)
{
	TListView *p = (TListView*)data;
	switch (msg)
	{
	case WM_ERASEBKGND:
		return p->OnEraseBK(wParam, lParam);
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
}
BOOL TListView::OnEraseBK(WPARAM wParam, LPARAM lParam)
{
	//只刷新没有数据的部分
	HDC hdc = (HDC)wParam;
	RECT rect = { 0 };
	RECT headRect = { 0 };
	GetClientRect(m_hwndList, &rect);
	Header_GetItemRect(m_hwndHeader, Header_GetItemCount(m_hwndHeader) - 1, &headRect);
	rect.top = headRect.bottom + 2;
	rect.right = headRect.right - 1;
	int itemCount = GetItemCount();
	if (itemCount > 0)
	{
		rect.top -= 1;
		RECT itemRect = { 0 };
		ListView_GetItemRect(m_hwndList, 0, &itemRect, LVIR_BOUNDS);
		int nTop = ListView_GetTopIndex(m_hwndList);
		int nHeight = itemRect.bottom - itemRect.top;
		rect.bottom = min(rect.bottom, rect.top + (itemCount - nTop) * nHeight) /*+ 1*/;
	}
	else
		rect.bottom = min(rect.bottom, headRect.bottom) + 2;
	POINT LTpoint, RBPoint;
	LTpoint.x = rect.left;
	LTpoint.y = rect.top;
	RBPoint.x = rect.right;
	RBPoint.y = rect.bottom;
	ClientToScreen(m_hwndList, &LTpoint);
	ClientToScreen(m_hwndList, &RBPoint);
	rect.left = LTpoint.x;
	rect.top = LTpoint.y;
	rect.right = RBPoint.x;
	rect.bottom = RBPoint.y;
	RECT bgRect = { 0 };
	GetWindowRect(m_hwndList, &bgRect);
	HRGN bgRgn = CreateRectRgnIndirect(&bgRect);
	HRGN noEraseRgn = CreateRectRgnIndirect(&rect);
	if (CombineRgn(bgRgn, bgRgn, noEraseRgn, RGN_XOR) == ERROR)
	{
		DeleteObject(noEraseRgn);
		return FALSE;
	}
	DeleteObject(noEraseRgn);
	HBRUSH brush = CreateSolidBrush(ListView_GetBkColor(m_hwndList));
	POINT saveOrg;
	SetWindowOrgEx(hdc, bgRect.left, bgRect.top, &saveOrg);
	FillRgn(hdc, bgRgn, brush);
	SetWindowOrgEx(hdc, saveOrg.x, saveOrg.y, NULL);
	DeleteObject(bgRgn);
	DeleteObject(brush);
	return TRUE;
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
		FillRect(pDIS->hDC, &pDIS->rcItem, m_hbr[0]); //选中时的颜色
		SetTextColor(pDIS->hDC, RGB(255, 255, 255));
		bSetTextColor = false;
	}
	else
	{
		FillRect(pDIS->hDC, &pDIS->rcItem, m_hbr[1]); //未选中时的颜色
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

	//	FrameRect(pDIS->hDC, &rTemp, m_hbr[2]); //矩形框
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
			DrawTextA(pDIS->hDC, szText, -1, &rTemp, textFormat);
		}

	}
}

void TListView::FindBeginEnd(int left, int right, int &iBegin, int &iEnd)
{
	RECT rcHeader = { 0 };
	Header_GetItemRect(m_hwndHeader, iBegin, &rcHeader);
	while (rcHeader.right < abs(left))
		Header_GetItemRect(m_hwndHeader, ++iBegin, &rcHeader); //找到最左边应该显示的位置
	RECT clientRect = { 0 };
	GetClientRect(m_hwndList, &clientRect);
	iEnd = iBegin;
	if (clientRect.right > right)
		iEnd = Header_GetItemCount(m_hwndHeader) - 1; //循环中是小于等于，所以减一
	else
		while (rcHeader.right < clientRect.right + abs(left) - 1) //找到最右边要显示的位置
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
	if (abs(data) > maxFolatData||(data!=0 &&abs(data)<minFloatData))
		strcpy_s(szText, "错误");
	else
		sprintf_s(szText, "%.2f", data);
	AddItem(szText, iRow, iCol, textFormat, textColor);
}
void TListView::AddItem(double data, int iRow, int iCol,const char* priceFormat, UINT textFormat, COLORREF textColor)
{
	char szText[100] = { 0 };
	if (abs(data) > maxFolatData || (data != 0 && abs(data) < minFloatData))
		strcpy_s(szText, "错误");
	else
		sprintf_s(szText, priceFormat, data);
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
	if (!m_bVirtualMode)
	{
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
	}
	else
	{
		AddItemVM(pszText, iRow, iCol);
	}

	SetItemState(iRow, iCol, textFormat, textColor);
}
void TListView::ModifyItem(const char* pszText, int iRow, int iCol, UINT textFormat, COLORREF textColor)
{
    if (!pszText)return;
    LVITEMA item;
    memset(&item, 0, sizeof(LVITEMA));
    item.mask = LVIF_TEXT;
    item.cColumns = Header_GetItemCount(m_hwndHeader);
    item.iItem = iRow;
    item.iSubItem = iCol;
    item.pszText = const_cast<char*>(pszText);
    SendMessageA(m_hwndList, LVM_SETITEMA, 0, (LPARAM)(&item));
    SetItemState(iRow, iCol, textFormat, textColor);
}
void TListView::AddItemVM(const char *_pchar, int iRow, int iCols)
{
	if (iRow < m_vecRowData.size())
	{
		if (iCols < m_vecRowData[iRow].size())
			m_vecRowData[iRow][iCols] = LoadRC::ansi_to_unicode(_pchar);
	}
	else
	{
		int iColumns = Header_GetItemCount(m_hwndHeader);
		vector<wstring>vecOneRow(iColumns);
		if (iCols<iColumns)
			vecOneRow[iCols] = LoadRC::ansi_to_unicode(_pchar);
		m_vecRowData.push_back(vecOneRow);
	}
}
void TListView::OnGetDispInfo(LPARAM lParam)
{
	NMLVDISPINFO *plvdi = (NMLVDISPINFO*)lParam;
	if (plvdi->item.mask&LVFIF_TEXT)
	{
		if (plvdi->item.iItem >= 0 && plvdi->item.iItem < m_vecRowData.size() && plvdi->item.iSubItem < m_vecRowData[plvdi->item.iItem].size())
			lstrcpy(plvdi->item.pszText, m_vecRowData[plvdi->item.iItem][plvdi->item.iSubItem].c_str());
	}
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
	if (iCol == 0 && iRow < m_vItemState.size())//在已经存在的行上添加，后续会的依次往后移动
	{ //如果m_vItemState大于0，说明已经进行过插入，现在在第0行插入，其他行会依次往后面移动，所以这个的style要放在最前面
		vector<unsigned short>oneRow;
		oneRow.push_back(style);
		m_vItemState.insert(m_vItemState.begin() + iRow, oneRow); //插入到对应的位置上
		return;
	}
	if (iRow < m_vItemState.size()) //行已经存在
	{
		if (iCol < m_vItemState[iRow].size()) //列已经存在
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
	case WM_ERASEBKGND:
		return 1;
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
	HBRUSH hbrBorder = CreateSolidBrush(RGB(130, 130, 130));
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
	pWpos->cy = m_iHeaderHeight; //header高度
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
	if (iItem < m_vItemState.size())
		m_vItemState.erase(m_vItemState.begin() + iItem);
	if (iItem < m_vecRowData.size())
		m_vecRowData.erase(m_vecRowData.begin() + iItem);
	SetItemCount(m_vecRowData.size());
}
void TListView::DeleteAllItems()
{
	ListView_DeleteAllItems(m_hwndList);
	m_vItemColor.clear();
	m_vItemState.clear();
	m_vItemColor.push_back(dafaultColor);

	m_vecRowData.clear();

	SetItemCount(0);
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

void TListView::SetItemCount(int count)
{
	ListView_SetItemCount(m_hwndList, count);
}