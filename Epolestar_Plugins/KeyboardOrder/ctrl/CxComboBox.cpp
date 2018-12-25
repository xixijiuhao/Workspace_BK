#include "..\PreInclude.h"

extern G_COLOREF g_ColorRefData;

//Combobox的Edit子控件的消息处理方法WNDPROC
LRESULT CxComboBox::EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool bChange = false;
	switch (message)
	{
		case WM_PASTE:
		case WM_CUT:
			if (m_bNoDelAndBack || m_bReadOnly)
				message = 0;
			break;
		case WM_UNDO:
			if (m_bNoUndo)
				message = 0;
			break;
		case WM_CONTEXTMENU:
			if (m_bNoPopupMenu)
				message = 0;
			break;
		case WM_KEYDOWN:
		{
			if (::IsWindow(m_ref.GetHwnd()) && m_ref.DoLeftRightUpDownKey(wParam,true))
				return 1;
			bool bShiftDown = (0x8000 & GetKeyState(VK_SHIFT))!=0;
			if (::IsWindow(m_ref.GetHwnd()))
			{
				if (bShiftDown)
					m_ref.SetIndexFocus(m_ref.GetLastFocusIndex(GetDlgCtrlID(GetParent(hWnd))), true, false);
				else
					m_ref.SetIndexFocus(m_ref.GetNextFocusIndex(GetDlgCtrlID(GetParent(hWnd))), true);
				return 1;
			}
		}
		if (m_bNoDelAndBack && (wParam == VK_DELETE))
			message = 0;
		break;
		case WM_CHAR:
			if (m_bReadOnly || m_bNoDelAndBack && (wParam == VK_BACK))
				message = 0;
			if (::IsWindow(m_ref.GetHwnd()))
			{
				if (m_ref.DoWndKeyDown(GetParent(hWnd), wParam))
					return 0;
			}
			break;
		case WM_LBUTTONDOWN:			//只读时鼠标按下即显示下拉框
			if (m_bReadOnly)
			{
				SendMessage(GetHwnd(),WM_SETFOCUS,0,0);
				ComboBox_ShowDropdown(GetHwnd(), TRUE);
				return 0;
			}
			break;
		case WM_SETTEXT:
			if (m_bAutoComplete && !m_bTextShowValue)
			{
				TCHAR * p = wcschr((TCHAR*)lParam, m_SplitChar);
				if (p != nullptr)
				{
					*p = 0;
					bChange = true;
				}
			}
			break;
		case EM_SETSEL:
			if (m_bReadOnly )
				wParam = lParam;
			break;
		case WM_NCPAINT:
		case WM_PAINT:
		{
			CallWindowProc(m_OldEditWndProc, hWnd, message, wParam, lParam);
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
			return 1;
		}
		break;
		case WM_MOUSEMOVE:
		{
			if (!m_bTracking)
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = hWnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = 50;
				m_bTracking = _TrackMouseEvent(&tme) == TRUE;
			}
		}
		break;
		case WM_MOUSELEAVE:
		{
			m_bTracking = FALSE;
			m_bHover = FALSE;
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
		}
		break;
		case WM_MOUSEHOVER:
		{
			m_bHover = TRUE;
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
		}
		break;
		case WM_SETFOCUS:
		{
			m_bFocus = TRUE;
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
		}
		break;
		case WM_KILLFOCUS:
		{
			m_bFocus = FALSE;
			DrawHelper::DrawFrame(GetHwnd(), m_bHover, m_bFocus, m_HotOrderBrush1, m_HotOrderBrush2, m_ClientBrush, m_hOrderBrush);
		}
		break;
	}


	//调用原始的处理函数
	LRESULT Result = CallWindowProc(m_OldEditWndProc, hWnd, message, wParam, lParam);


	switch (message)
	{
		case WM_GETTEXT:
			if (m_bTextShowValue)//获取有效文字 Name
			{
				TCHAR * p = wcschr((TCHAR*)lParam, m_SplitChar);
				if (p != nullptr)
					*p = 0;
				Result = wcslen((TCHAR*)lParam);
			}
			break;
		case WM_SETTEXT:
			if (bChange)
			{
				SetEditSel(wcslen((TCHAR*)lParam) + 1, wcslen((TCHAR*)lParam) + 1);
				SendMessage(GetParent(GetHwnd()),WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(GetHwnd()), CBN_EDITCHANGE), (LONG)this->m_hWnd);
			}
			break;
	}

	return Result;
}

CxComboBox::CxComboBox(COFInputDlg&ref) :m_bReadOnly(false)
, m_SplitChar(' ')
, m_bAutoComplete(true)
, m_bCanAutoComplete(true)
, m_bTimerEnabled(false)
, m_bMouseClicked(false)
, m_bNoDelAndBack(false)
, m_bNoPasteCut(false)
, m_bNoPopupMenu(false)
, m_bNoUndo(false)
, m_bTextShowValue(false)
, m_ref(ref)
{
	m_bTracking = m_bHover = m_bFocus = FALSE;
	m_ClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorWhite()/*GetSysColor(COLOR_BTNFACE)*/);
	m_hOrderBrush = ::CreateSolidBrush(/*GetSysColor(COLOR_WINDOWFRAME)*/g_ColorRefData.GetColorRGB150());
	m_HotOrderBrush1 = ::CreateSolidBrush(RGB(96, 200, 253));
	m_HotOrderBrush2 = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());
}
CxComboBox::~CxComboBox()
{
	if (m_ClientBrush)
		DeleteObject(m_ClientBrush);
	if (m_hOrderBrush)
		DeleteObject(m_hOrderBrush);
	if (m_HotOrderBrush1)
		DeleteObject(m_HotOrderBrush1);
	if (m_HotOrderBrush2)
		DeleteObject(m_HotOrderBrush2);
}
void CxComboBox::InitCombo(HWND hwnd,bool bShowValue)
{
	Init(hwnd);
	m_bTextShowValue = bShowValue;
	//重定向子控件Edit的WndProc到CxComboBox的成员函数EditWndProc
	m_EditHandle = ::FindWindowEx(hwnd, 0, _T("Edit"), 0);
	if (m_EditHandle != 0)
	{
		//保存Edit的原始处理函数
		m_OldEditWndProc = (WNDPROC) ::GetWindowLong(m_EditHandle, GWL_WNDPROC);

		//转成员函数EditWndProc为回调函数
		m_dEditWndProc.Assign(this, &CxComboBox::EditWndProc);

		//设置Edit的回调函数为EditWndProc
		::SetWindowLong(m_EditHandle, GWL_WNDPROC, (LONG)(WNDPROC)m_dEditWndProc);
	}
}
LRESULT CALLBACK CxComboBox::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//鼠标单双击判断
	switch (message)
	{
		case WM_PAINT:
		{
			COMBOBOXINFO Info;
			memset(&Info, 0, sizeof(COMBOBOXINFO));
			Info.cbSize = sizeof(COMBOBOXINFO);
			GetComboBoxInfo(GetHwnd(), &Info);

			RECT rect;
			CopyRect(&rect, &Info.rcButton);
			InflateRect(&rect,1,0);

			PAINTSTRUCT ps;
			HDC hdc=BeginPaint(GetHwnd(), &ps);

			RECT rc;
			GetClientRect(GetHwnd(), &rc);
			FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));
			HBRUSH hBkBrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
			if (m_ref.IsKS())
				FillRect(hdc, &rect, hBkBrush);
			else
				FillRect(hdc, &rect, GetStockBrush(WHITE_BRUSH));

			//画倒三角
			int pos = rect.top + (rect.bottom - rect.top) / 2;
			POINT ptLeft;
			ptLeft.x = rect.right - 5 - 8;
			ptLeft.y = pos - 2;
			POINT ptCenter;
			ptCenter.x = rect.right - 5 - 4;
			ptCenter.y=pos + 2;
			POINT ptRight;
			ptRight.x = rect.right - 5;
			ptRight.y=pos - 2;
			HPEN pen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
			HPEN OldPen = (HPEN)SelectObject(hdc, pen);
			MoveToEx(hdc,ptLeft.x, ptLeft.y,NULL);
			LineTo(hdc,ptCenter.x, ptCenter.y);
			LineTo(hdc,ptRight.x + 1, ptRight.y - 1);
			SelectObject(hdc, OldPen);

			
			DeleteObject(pen);
			DeleteObject(hBkBrush);
			EndPaint(GetHwnd(), &ps);
		}
			return 1;
		case WM_LBUTTONDOWN:
			SetIsClick(true);					//单击判断激活
			break;
		case WM_LBUTTONUP:
		{
			RECT Rect;
			GetClientRect(GetHwnd(),&Rect);
			POINT point;

			point.x = lParam & 0xFFFF;//<<16>>16;
			point.y = lParam >> 16;

			//鼠标按键弹起时的位置不在本控件之内，则取消单击事件
			if (PtInRect(&Rect,point) && m_bTimerEnabled)
			{
				m_bMouseClicked = true;
				MouseClk.wParam = wParam;
				MouseClk.lParam = lParam;
			}
			else
				SetIsClick(false);
		}
		break;
		case WM_MOUSELEAVE:
		case WM_LBUTTONDBLCLK:
			SetIsClick(false);			          //触发双击事件时关闭时钟，使单击事件失效
			break;
		case WM_KEYDOWN:
			m_bCanAutoComplete = wParam != VK_DELETE && wParam != VK_BACK;
			break;
		case WM_TIMER:
		{
			//在一个鼠标双击最大时间间隔之后才触发单击事件
			if (m_bMouseClicked)
			{
				POINT pt;
				pt.x = MouseClk.lParam & 0xFFFF;//<<16>>16;
				pt.y = MouseClk.lParam >> 16;
				DoLBtnClick(MouseClk.wParam, pt);
			}
		}
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void CxComboBox::DoLBtnClick(UINT nFlags, POINT point)
{
	SetIsClick(false);
	SendMessage(GetHwnd(),WM_LBUTTONCLK, nFlags, point.y << 16 || point.x);//发送单击消息
}
bool CxComboBox::IsReadOnly()
{
	return m_bReadOnly;
}
void CxComboBox::ReadOnly(bool Value)
{
	if (m_bReadOnly == Value)
		return;
	m_bReadOnly = Value;
}
wstring CxComboBox::GetName(int Index)
{
	if (Index<0)
		return _T("");
	wstring ItemText;
	GetLBText(Index, ItemText);
	if (!m_bAutoComplete)
		return ItemText;

	int pos = ItemText.find(m_SplitChar);
	if (pos != CB_ERR)
		return ItemText.substr(0,pos);
	else
		return ItemText;
}
int CxComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
{
	return ComboBox_SelectString(GetHwnd(), nStartAfter, lpszString);
}
int CxComboBox::GetCurSel()
{
	return ComboBox_GetCurSel(GetHwnd());
}
int CxComboBox::SetCurSel(int nSelect)
{
	return ComboBox_SetCurSel(GetHwnd(), nSelect);
}
void CxComboBox::GetLBText(int nIndex, wstring& rString) 
{
	int len=ComboBox_GetLBTextLen(GetHwnd(), nIndex);
	wchar_t *p = new wchar_t[len + 1];
	p[0] = '\0';
	ComboBox_GetLBText(GetHwnd(), nIndex, p);
	rString = p;
	delete[]p;
}
int CxComboBox::FindString(_In_ int nStartAfter, _In_z_ LPCTSTR lpszString) 
{
	return ComboBox_FindString(GetHwnd(), nStartAfter, lpszString);
}
void CxComboBox::ResetContent()
{
	ComboBox_ResetContent(GetHwnd());
}
int CxComboBox::AddString(LPCTSTR lpszItem)
{
	return ComboBox_AddString(GetHwnd(), lpszItem);
}
void CxComboBox::SetIsClick(bool Value)
{
	m_bTimerEnabled = Value;

	if (m_bTimerEnabled)
		SetTimer(GetHwnd(),1, ::GetDoubleClickTime(), NULL);	//打开时钟，并设置时钟回调的间隔为鼠标双击最大时间间隔
	else
	{
		KillTimer(GetHwnd(), 1);								//关闭时钟
		m_bMouseClicked = false;
	}
}
int CxComboBox::AddString(wstring Name, wstring Value)
{
	wchar_t ItemText[21] = { 0 };
	swprintf_s(ItemText,_T("%s%c%s"), Name.c_str(), m_SplitChar, Value.c_str());
	return AddString(ItemText);
}
BOOL CxComboBox::SetEditSel(_In_ int nStartChar, _In_ int nEndChar)
{
	return (BOOL)::SendMessage(GetHwnd(), CB_SETEDITSEL, 0, MAKELONG(nStartChar, nEndChar));
}
void CxComboBox::SetSplitChar(char Value)
{
	m_SplitChar = Value;
}
//Value对齐, AuotListWidth:List自适应文本宽度
void CxComboBox::AlighValue(bool AuotListWidth)
{
	if (!m_bAutoComplete)
		return;

	int MaxNameLeng = 0;
	wstring ItemText;
	int pos;
	//获得Name最大长度
	for (int i = 0; i<ComboBox_GetCount(m_hWnd); i++)
	{
		GetLBText(i, ItemText);
		pos = ItemText.find(m_SplitChar);
		if (CB_ERR == pos)
			pos = ItemText.length();
		if (pos > MaxNameLeng)
			MaxNameLeng = pos;
	}

	int ItemMaxWidth = 0;
	HDC hdc = GetDC(m_hWnd);
	HFONT hfont=(HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hfont);

	wchar_t * pSplit = new wchar_t[MaxNameLeng + 2];
	memset(pSplit, m_SplitChar, MaxNameLeng + 2);
	int SplitNum = 1;

	//对齐Value，获得最大文本长度
	for (int i = 0; i<ComboBox_GetCount(m_hWnd); i++)
	{
		GetLBText(i, ItemText);
		pos = ItemText.find(m_SplitChar);

		if (CB_ERR != pos && MaxNameLeng != pos)
		{
			SplitNum = MaxNameLeng - pos;
			pSplit[SplitNum + 1] = 0;

			wchar_t text[51] = { 0 };
			swprintf_s(text, _T("%s%s%s"), GetName(i).c_str(), pSplit, GetValue(i).c_str());
			ItemText = text;
			::SendMessage(m_hWnd, LB_DELETESTRING, i, 0);
			::SendMessage(m_hWnd, LB_INSERTSTRING, i, (LPARAM)ItemText.c_str());

			pSplit[SplitNum + 1] = m_SplitChar;
		};

		if (AuotListWidth)
		{
			SIZE sz;
			GetTextExtentPoint(hdc, ItemText.c_str(), ItemText.length(), &sz);
			ItemMaxWidth = max(sz.cx,ItemMaxWidth);
		}
	}
	delete[] pSplit;

	SelectObject(hdc,hOldFont);
	ReleaseDC(m_hWnd, hdc);

	if (!AuotListWidth)
		return;

	ItemMaxWidth += ::GetSystemMetrics(SM_CYBORDER) * 2;
	if (SendMessage(m_hWnd, CB_GETMINVISIBLE, 0, 0) < ComboBox_GetCount(m_hWnd))
		ItemMaxWidth += ::GetSystemMetrics(SM_CXVSCROLL);
	::SendMessage(m_hWnd, CB_SETDROPPEDWIDTH, ItemMaxWidth + 2, 0);
}
wstring CxComboBox::GetValue(int Index) 
{
	wstring ItemText;
	GetLBText(Index, ItemText);
	if (ItemText.empty() || !m_bAutoComplete)
		return _T("");

	int pos = ItemText.find(m_SplitChar);
	if (pos == CB_ERR)
		return _T("");
	else
		return ItemText.substr(pos + 1, string::npos);
	
}