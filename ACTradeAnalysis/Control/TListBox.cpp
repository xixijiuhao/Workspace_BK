#include "BaseInclude.h"

TListBox::TListBox()
{
	m_iItemHeight = 21;
}
TListBox::~TListBox()
{
}

void TListBox::Create(HWND hParent, UINT style, int id)
{
	UINT lbStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_HASSTRINGS | style;
	m_hListBox = CreateWindowEx(0, WC_LISTBOX, 0, lbStyle, 0, 0, 0, 0, \
		hParent, (HMENU)id, GetModuleHandle(NULL), 0);
	SetWindowSubclass(m_hListBox, ListBoxProc, id, DWORD_PTR(this));
	SendMessage(m_hListBox, WM_SETFONT, (WPARAM)g_FontData.g_FontWord13, 0);
	SendMessage(m_hListBox, LB_SETITEMHEIGHT, 0, 22);
}

LRESULT CALLBACK TListBox::ListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	if (msg == WM_LBUTTONUP)
	{
		int index = SendMessage(hwnd, LB_GETCURSEL, 0, 0);
		if (index != LB_ERR)
		{
			PostMessage(::GetParent(hwnd), SDWM_CLICK_LISTBOX_ITEM, (WPARAM)index, (LPARAM)hwnd);
		}
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
}

void TListBox::AddItem(const char* pszText)
{
	if (!pszText)
		return;
	SendMessageA(m_hListBox, LB_ADDSTRING, 0, (LPARAM)pszText);
}
void TListBox::AddItem(const WCHAR* pszText)
{
	if (!pszText)
		return;
	SendMessage(m_hListBox, LB_ADDSTRING, 0, (LPARAM)pszText);
}

void TListBox::DeleteItem(int Index)
{
	SendMessage(m_hListBox, LB_DELETESTRING, Index, 0);
}

void TListBox::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
	SetBkMode(pDIS->hDC, TRANSPARENT);
	HBRUSH hbrSelected = CreateSolidBrush(g_ColorRefData.g_ColorBlue);
	HBRUSH hbrUnselected = CreateSolidBrush(g_ColorRefData.g_ColorWhite);

	if (pDIS->itemState&ODS_SELECTED || pDIS->itemState&ODS_FOCUS)
		FillRect(pDIS->hDC, &pDIS->rcItem, hbrSelected);
	else
		FillRect(pDIS->hDC, &pDIS->rcItem, hbrUnselected);

	TCHAR szText[200] = { 0 };
	SendMessage(m_hListBox, LB_GETTEXT, pDIS->itemID, (LPARAM)szText);
	RECT rect = pDIS->rcItem;
	int iTextStartLeft = 12;        //文字左边起始位置
	rect.left += iTextStartLeft;
	DrawText(pDIS->hDC, szText, -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	DeleteObject(hbrSelected);
	DeleteObject(hbrUnselected);
}
void TListBox::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT pMIS = (LPMEASUREITEMSTRUCT)lParam;
	pMIS->itemHeight = m_iItemHeight;
	m_iItemHeight = pMIS->itemHeight;
}

void TListBox::Clear()
{
	int count = ListBox_GetCount(m_hListBox);
	for (int i = 0; i < count; i++)
	{
		ListBox_DeleteString(m_hListBox, i);
	}
}