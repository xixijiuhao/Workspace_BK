#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;

CMultiColComboBox::CMultiColComboBox(COFInputDlg&ref)
	:m_bReverFind(false)
	, m_bAutoComplete(true)
	, m_Edit(*this,ref)
	, m_bEditHeightSet(false)
	,m_iSelectedRow(-1)
	, m_ref(ref)
{
	m_vecEndPixels[0] = 150;
	m_vecEndPixels[1] = 150;
}
CMultiColComboBox::~CMultiColComboBox()
{
	
}
void CMultiColComboBox::InitComboBox(HWND hComboBox)
{
	Init(hComboBox);
	COMBOBOXINFO info;
	info.cbSize = sizeof(COMBOBOXINFO);
	::GetComboBoxInfo(m_hWnd,&info);
	m_Edit.InitEdit(info.hwndItem);
}
LRESULT CALLBACK CMultiColComboBox::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_PAINT)
	{
		COMBOBOXINFO Info;
		memset(&Info, 0, sizeof(COMBOBOXINFO));
		Info.cbSize = sizeof(COMBOBOXINFO);
		GetComboBoxInfo(GetHwnd(), &Info);

		RECT rect;
		CopyRect(&rect, &Info.rcButton);
		InflateRect(&rect, 1, 0);

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(GetHwnd(), &ps);

		RECT rc;
		GetClientRect(GetHwnd(), &rc);
		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));
		//HBRUSH hBkBrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
		//if (m_ref.IsKS())
		//	FillRect(hdc, &rect, hBkBrush);
		//else
			FillRect(hdc, &rect, GetStockBrush(WHITE_BRUSH));

		//画倒三角
		int pos = rect.top + (rect.bottom - rect.top) / 2;
		POINT ptLeft;
		ptLeft.x = rect.right - 5 - 8;
		ptLeft.y = pos - 2;
		POINT ptCenter;
		ptCenter.x = rect.right - 5 - 4;
		ptCenter.y = pos + 2;
		POINT ptRight;
		ptRight.x = rect.right - 5;
		ptRight.y = pos - 2;
		HPEN pen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
		HPEN OldPen = (HPEN)SelectObject(hdc, pen);
		MoveToEx(hdc, ptLeft.x, ptLeft.y, NULL);
		LineTo(hdc, ptCenter.x, ptCenter.y);
		LineTo(hdc, ptRight.x + 1, ptRight.y - 1);
		SelectObject(hdc, OldPen);


		DeleteObject(pen);
		//DeleteObject(hBkBrush);
		EndPaint(GetHwnd(), &ps);
		return 1;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CMultiColComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	TEXTMETRIC tm;
	HDC dc = GetDC(m_hWnd);

	SelectObject(dc,(HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0));
	GetTextMetrics(dc, &tm);
	lpMeasureItemStruct->itemHeight = tm.tmHeight + tm.tmExternalLeading;
	if (!m_bEditHeightSet)
	{
		m_bEditHeightSet = true;
		ComboBox_SetItemHeight(m_hWnd, -1, lpMeasureItemStruct->itemHeight + 2);
		ComboBox_SetItemHeight(m_hWnd, 0, lpMeasureItemStruct->itemHeight + 2);
	}

	ReleaseDC(m_hWnd, dc);
}
void CMultiColComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC hDC = lpDrawItemStruct->hDC;
	//CWnd* pWndDropDown = pDC->GetWindow();
	//_ASSERT(pWndDropDown != this);

	RECT rcText = lpDrawItemStruct->rcItem;
	int iItemID = lpDrawItemStruct->itemID;
	int iItemAction = lpDrawItemStruct->itemAction;
	int iItemState = lpDrawItemStruct->itemState;
	wchar_t sText[51] = { 0 };
	UINT iCol = 0;
	if (iItemID == CB_ERR) return;
	RECT rcField = rcText;

	switch (iItemAction)
	{
		case ODA_DRAWENTIRE:
		case ODA_SELECT:
			ComboBox_GetLBText(m_hWnd, iItemID, sText);
			if (iItemState & ODS_SELECTED)
			{
				SetBkColor(hDC, GetSysColor(COLOR_HIGHLIGHT));
				SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
			else
			{
				SetBkColor(hDC, GetSysColor(COLOR_WINDOW));
				SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
			}

			ExtTextOut(hDC, rcText.left, rcText.top, ETO_OPAQUE, &rcText
				, sText, wstring(sText).length(), NULL);
		
			wcscpy_s(sText, LoadRC::ansi_to_unicode(m_vUserInfo[iItemID].UserName).c_str());
			rcField.left = m_vecEndPixels[0];

			ExtTextOut(hDC, rcField.left, rcField.top, ETO_OPAQUE, &rcField
					, sText, wstring(sText).length(), NULL);
			
			if (iItemState & ODS_FOCUS)
				DrawFocusRect(hDC, &rcText);
			break;
		case ODA_FOCUS:
			DrawFocusRect(hDC, &rcText); 
			break;
	}
}
void CMultiColComboBox::ResetContent()
{
	ComboBox_ResetContent(m_hWnd);
	m_bEditHeightSet = false;
	::SendMessage(m_hWnd, CB_SETDROPPEDWIDTH, 300 + GetSystemMetrics(SM_CXVSCROLL), 0);
	m_vUserInfo.clear();
}

bool CMultiColComboBox::IsNeedForbidMosueWheel()
{
	return !ComboBox_GetDroppedState(m_hWnd);
}
int  CMultiColComboBox::AddRow(const sUserInfo& sInfo)
{
	m_vUserInfo.push_back(sInfo);
	int iRow = ComboBox_AddString(m_hWnd, LoadRC::ansi_to_unicode(sInfo.UserNo).c_str());
	return iRow;
}
int  CMultiColComboBox::GetSelectedRow()
{
	int iCount = ComboBox_GetCount(m_hWnd);
	if (m_iSelectedRow >= iCount)
		m_iSelectedRow = -1;
	return m_iSelectedRow;
}
void CMultiColComboBox::GetLBText(int nIndex, wstring& rString) const
{
	if (ComboBox_GetLBTextLen(m_hWnd, nIndex) == 0)
	{
		rString = L"";
		return;
	}
	else
	{
		wchar_t text[51] = { 0 };
		ComboBox_GetLBText(m_hWnd, nIndex, text);
		rString = text;
		return;
	}
}
void CMultiColComboBox::ResetSelection()
{
	ComboBox_SetCurSel(m_hWnd, -1);
	m_iSelectedRow = -1;
}
bool CMultiColComboBox::LineInStringLeft(const string& rsLine, const string& rsString) const
{
	if (rsLine.length() > rsString.length())
		return false;
	string sPart;
	sPart = rsString.substr(0,rsLine.length());
	return !rsLine.compare(sPart);
}
bool CMultiColComboBox::LineInStringRight(const string& rsLine, const string& rsString) const
{
	if (rsLine.length() > rsString.length())
		return false;
	string sPart;
	sPart = rsString.substr(rsString.length() - rsLine.length(), string::npos);
	return !rsLine.compare(sPart);
}
int  CMultiColComboBox::FindUniqueMatchedItem(const string &line, string &sMatchedLine)
{
	sMatchedLine.clear();
	int iMatchedItem = CB_ERR;
	wchar_t sItem[51] = { 0 };
	//输入大写
	string sLine(line);
	transform(sLine.begin(), sLine.end(), sLine.begin(), toupper);

	if (!GetReverFind())
	{
		for (int iItem = 0; iItem < ComboBox_GetCount(m_hWnd); ++iItem)
		{
			ComboBox_GetLBText(m_hWnd, iItem, sItem);
			string strItem(LoadRC::unicode_to_ansi(sItem));
			string sorigin(strItem);
			transform(strItem.begin(), strItem.end(), strItem.begin(), toupper);
			
			if (!strItem.compare(sLine) || LineInStringLeft(sLine, strItem))//完全 部分匹配的
			{
				sMatchedLine = sorigin;
				return iItem;
			}
		}
	}
	else
	{
		for (int iItem = 0; iItem < ComboBox_GetCount(m_hWnd); ++iItem)
		{
			ComboBox_GetLBText(m_hWnd, iItem, sItem);
			string strItem(LoadRC::unicode_to_ansi(sItem));
			string sorigin(strItem);
			transform(strItem.begin(), strItem.end(), strItem.begin(), toupper);
			if (!strItem.compare(sLine) || LineInStringRight(sLine, strItem))//完全 部分匹配的
			{
				sMatchedLine = sorigin;
				return iItem;
			}
		}
	}
	return iMatchedItem;
}
void CMultiColComboBox::OnCbnEditupdate()
{
	string line;
	string sMatchedText;
	line = GetWindowTextAnsic(m_hWnd);

	int iHiLightStart = line.length();
	if (line.length() == 0)
	{
		ComboBox_ShowDropdown(m_hWnd,FALSE);
		SetWindowTextW(m_hWnd,L"");
		m_iSelectedRow = -1;
		if (!m_bAutoComplete)	m_bAutoComplete = true;
		return;
	}
	if (!m_bAutoComplete)
	{
		m_bAutoComplete = true;
		return;
	}
	m_iSelectedRow = FindUniqueMatchedItem(line, sMatchedText);
	if (m_iSelectedRow >= 0)
	{
		//这句需要有 要不就会就会自动选中一些项 不清楚原因
		if (GetReverFind())
			SendMessage(m_hWnd, CB_SETCURSEL, -1, 0);
		ComboBox_ShowDropdown(m_hWnd, TRUE);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		//正向查询时自动选择项目 反向时不自动选中
		if (!GetReverFind())
			SendMessage(m_hWnd, CB_SETCURSEL, m_iSelectedRow, 0);
		else
		{
			//需要用postMessage  若用SendMessage则不行
			PostMessage(m_hWnd, CB_SETTOPINDEX, m_iSelectedRow, 0);
			COMBOBOXINFO info;
			info.cbSize = sizeof(COMBOBOXINFO);
			GetComboBoxInfo(m_hWnd, &info);
			::PostMessage(info.hwndList, LB_SETCURSEL, m_iSelectedRow, 0);
		}

		//因为调用SendMessage(CB_SETCURSEL, -1, 0);所以需要设置文本 要不就被清空了
		if (GetReverFind())
			SetWindowTextW(m_hWnd, LoadRC::ansi_to_unicode(line).c_str());

		int iStartChar = line.length();
		int iEndChar = -1;
		PostMessage(m_hWnd, CB_SETEDITSEL, 0, MAKELPARAM(iStartChar, iEndChar));
	}
	else
	{
		ComboBox_ShowDropdown(m_hWnd, FALSE);
		SetWindowTextW(m_hWnd, LoadRC::ansi_to_unicode(line).c_str());
		PostMessage(m_hWnd, CB_SETEDITSEL, 0, MAKELPARAM(line.length(), line.length()));
	}
}
