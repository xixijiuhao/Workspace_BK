

/*******************************************************************************
* 文件名称:    BaseControl.cpp
* 创建日期:    
* 创 建 人:    
* 说    明:    
*******************************************************************************/

#include "Include.h"

HWND BaseControl::CreateControl(CreateWndStruct &controlStruct)
{
	return ::CreateWindowEx(
		controlStruct.dwExStyle,
		controlStruct.lpszClass,
		controlStruct.lpszName,
		controlStruct.style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		controlStruct.hwndParent,
		controlStruct.hMenu,
		controlStruct.hInstance,
		controlStruct.lpCreateParams);
}

// 构造函数
BaseControl::BaseControl()
{
	m_bTracking = FALSE;
	m_bHover = FALSE;
	m_bFocus = FALSE;
	m_OldProc = NULL;
}

// 析构函数
BaseControl::~BaseControl()
{
}

// 设置控件的位置
void BaseControl::OnMove()
{
	::MoveWindow(this->m_hWnd, 
		m_Rect.left, 
		m_Rect.top,
		m_Rect.right - m_Rect.left,
		m_Rect.bottom - m_Rect.top, 
		true);
}


void BaseControl::MoveWindow(int x, int y, int cx, int cy,UINT uFlags)
{
	SetWindowPos(m_hWnd, 0, x, y, cx, cy, uFlags);
}

void BaseControl::MoveWindow(HWND tmpHwnd, int x, int y, int cx, int cy, UINT uFlags)
{
	SetWindowPos(tmpHwnd, 0, x, y, cx, cy, uFlags);
}

HWND BaseControl::CreateLable(HWND hParent, const TCHAR* pszText, UINT style, UINT id)
{
	style |= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_NOTIFY;
	HWND hWndLable = CreateWindowEx(0, 
		WC_STATIC, 
		pszText, 
		style, 
		0, 
		0, 
		0, 
		0,
		hParent, 
		HMENU(id), 
		GetModuleHandle(NULL), 0);
	SetFont(hWndLable,g_FontData.g_FontWord12);
	return hWndLable;
}

void BaseControl::SetFont(HWND &hWndTmp,HFONT hfont)
{
	SendMessage(hWndTmp, WM_SETFONT, (WPARAM)hfont, TRUE);
}

// 创建控件的位置
void BaseControl::CreateControl(char* chValue, char* key, HWND hParentWnd)
{

}

// 画出控件
void BaseControl::DrawItem(WPARAM &wParam, LPARAM &lParam)
{
	

}

// 设置控件文本
void BaseControl::SetText(HWND &hWndTmp, const TCHAR * strText)
{
	SetWindowText(hWndTmp, strText);
}



TCHAR* BaseControl::GetText(HWND &hWndTmp)
{
	TCHAR chInput[MAX_PATH] = { 0 };
	::GetWindowText(hWndTmp, chInput, MAX_PATH);
	return chInput;
}


BOOL BaseControl::Init(HWND hWnd)
{
	if (IsWindow(hWnd))
	{
		m_hWnd = hWnd;

		m_hParenthWnd = ::GetParent(m_hWnd);
		if (SetProp(m_hWnd, _T("BaseControl"), (HANDLE)this) == 0)
		{
			return FALSE;
		}
		m_OldProc = SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)stdProcCtrl);//改变窗口回调过程

	}
	return FALSE;
}

HWND BaseControl::CreateTextEdit(HWND hParent, UINT style, UINT specialStyle, UINT id)
{
	m_iExStyle = style;
	UINT editStyle = ES_AUTOVSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS | specialStyle;
	if (style == FLOATSTYLE)
		editStyle |= ES_NUMBER;
	HWND hedit = CreateWindowEx(0, WC_EDIT, TEXT(""), editStyle, 0, 0, 0, 0, hParent, (HMENU)(id), GetModuleHandle(NULL), NULL);
	Init(hedit);
	return hedit;
}
void BaseControl::SetLimitTextLen(HWND &hWndTmp,UINT length)
{
	SendMessage(hWndTmp, EM_SETLIMITTEXT, length, 0);
}
void BaseControl::SetWindowNumber(HWND &hWndTmp,int number)
{
	TCHAR szText[100] = { 0 };
	wsprintf(szText, L"%d", number);
	SetWindowText(hWndTmp, szText);
}
void BaseControl::SetDoubleData(HWND &hWndTmp,double data)
{
	char szText[100] = { 0 };
	sprintf_s(szText, "%.2lf", data);
	SetWindowTextA(hWndTmp, szText);
}
int BaseControl::GetWindowNumber(HWND &hWndTmp)
{
	TCHAR szText[100] = { 0 };
	GetWindowText(hWndTmp, szText, sizeof(szText) / sizeof(szText[0]));

	return _wtoi(szText);
}
double BaseControl::GetDoubleData(HWND &hWndTmp)
{
	TCHAR szText[100] = { 0 };
	GetWindowText(hWndTmp, szText, sizeof(szText) / sizeof(szText[0]));
	return _wtof(szText);
}

void BaseControl::DrawItemFrame(bool bHover, bool bFocus)
{
	HBRUSH hClientBrush = ::CreateSolidBrush(g_ColorRefData.GetColorRGB150());
	HBRUSH hHotBKBrush = ::CreateSolidBrush(g_ColorRefData.GetColorLightBlue());


	HDC hDC = ::GetWindowDC(m_hWnd);
	RECT rect, rectClient;
	::GetWindowRect(m_hWnd, &rect);
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = rect.right - rect.left;
	rectClient.bottom = rect.bottom - rect.top;

	if (bHover || bFocus)
		::FrameRect(hDC, &rectClient, hHotBKBrush);
	else
	{
		::FrameRect(hDC, &rectClient, hClientBrush);
	}
	::ReleaseDC(m_hWnd, hDC);

	DeleteObject(hClientBrush);
	DeleteObject(hHotBKBrush);
}
bool BaseControl::DealFloat(WPARAM wParam, LPARAM lParam)
{
	static bool dotFlag = true;
	TCHAR szText[50] = { 0 };
	GetWindowText(m_hWnd, szText, sizeof(szText));
	if (lstrlen(szText) == 0)
		dotFlag = true;
	TCHAR * pC = wcsrchr(szText, L'.');
	if (pC == NULL)
		dotFlag = true;
	if (wParam == '.')
	{
		if (!dotFlag)
			return false;
		dotFlag = false;
	}
	else if (wParam == '+' || wParam == '-')
	{
		if (lstrlen(szText) != 0)
			return false;
	}
	else if ((wParam<'0' || wParam>'9') && wParam != VK_BACK)
		return false;
	return true;
}

LRESULT BaseControl::WndProc(UINT message, WPARAM w, LPARAM l)
{
	if (WM_NCPAINT == message || WM_PAINT == message)
	{
		CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l);
		DrawItemFrame(m_bHover, m_bFocus);
		return 1;
	}
	else if (WM_MOUSEMOVE == message)
	{
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 50;
			m_bTracking = TrackMouseEvent(&tme) == TRUE;
		}
	}
	else if (WM_MOUSELEAVE == message)
	{
		m_bTracking = FALSE;
		m_bHover = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_MOUSEHOVER == message)
	{
		m_bHover = TRUE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_SETFOCUS == message)
	{
		m_bFocus = TRUE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_KILLFOCUS == message)
	{
		m_bFocus = FALSE;
		DrawItemFrame(m_bHover, m_bFocus);
	}
	else if (WM_CHAR == message)
	{
		if (m_iExStyle == FLOATSTYLE && !DealFloat(w, l))
			return 0;
	}
	else if (WM_KEYDOWN == message)
	{
		if (w == VK_RETURN)
			SendMessage(::GetParent(m_hWnd), SSWM_EDIT_PRESS_ENTER, GetWindowLongPtr(m_hWnd, GWL_ID), LPARAM(m_hWnd));
		else if (w == VK_TAB)
			SendMessage(::GetParent(m_hWnd), SSWM_TAB_SWITCH_FOCUS, GetWindowLongPtr(m_hWnd, GWL_ID), LPARAM(m_hWnd));
		else if (VK_UP == w)
			SendMessage(::GetParent(m_hWnd), WM_MOUSEWHEEL, 1 << 16, (LPARAM)m_hWnd);
		else if (VK_DOWN == w)
			SendMessage(::GetParent(m_hWnd), WM_MOUSEWHEEL, -1 << 16, (LPARAM)m_hWnd);
		//return 0;
	}

	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, w, l); //默认回调过程
}

LRESULT WINAPI BaseControl::stdProcCtrl(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam)
{
	BaseControl* bc = (BaseControl*)GetProp(hWnd, _T("BaseControl"));
	return bc->WndProc(uMsg, wParam, lParam);
}

void BaseControl::CreateLableTextEdit(HWND hParent, const TCHAR* pszText, ControlPos pos, UINT id, UINT style, UINT specialStyle)
{
	m_LabelWithEdit.Create(hParent, pszText);
	m_LabelWithEdit.MoveWindow(pos.LEFT, pos.TOP, pos.WIDGETV, pos.WIDGETH);

	//合约号TEdit
	RECT rect = { pos.LEFT, pos.BOTTOM + pos.Y_VAP, pos.RIGHT, pos.BOTTOM + pos.Y_VAP + pos.WIDGETH };
	m_EditWithLabel.Create(hParent, 0, ES_UPPERCASE, id);
	m_EditWithLabel.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_EditWithLabel.SetFont(g_FontData.g_FontNum15);
}

void BaseControl::CreateLableTextEditBtn(HWND hParent, const TCHAR* pszText, ControlPos pos, RECT &rect, IOperator *ip, 
	CPopWinCode &tmpPopWinCode, CPopWinCodeList &tmpPopupCodeList, CxEdit &tmpEdit, UINT editId, UINT btnId, UINT style, UINT specialStyle)
{
	//Label
	m_LabelWithEditBtn.Create(hParent, pszText);
	m_LabelWithEditBtn.MoveWindow(pos.LEFT, pos.TOP, pos.WIDGETV, pos.WIDGETH);

	//Edit
	rect = { pos.LEFT, pos.BOTTOM + pos.Y_VAP, pos.RIGHT, pos.BOTTOM + pos.Y_VAP + pos.WIDGETH };
	m_EditWithLabelBtn.Create(hParent, 0, ES_UPPERCASE, editId);
	m_EditWithLabelBtn.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_EditWithLabelBtn.SetFont(g_FontData.g_FontNum15);

	//Button按钮
	rect.left = rect.right + 1;
	rect.right = rect.right + 14;
	m_BtnWithEditLabel.Create(hParent, " ", btnId);
	m_BtnWithEditLabel.SetFont(g_FontData.g_FontWord13);
	m_BtnWithEditLabel.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

	//合约筛选下拉框
	m_PopupContractCode2.CreatePopWin(ip, L"popupCode", hParent, WS_CLIPSIBLINGS, 0);
	ShowWindow(m_PopupContractCode2.GetHwnd(), SW_HIDE);
	//合约List控件
	DWORD dwStyle = WS_CLIPSIBLINGS | WS_BORDER | WS_POPUP;
	DWORD dwStyleeEx = WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE;
	m_PopupCodeList2.CreatePopWin(ip, L"popupcodeList", hParent, dwStyle, dwStyleeEx, 0, 0);
	ShowWindow(m_PopupCodeList2.GetHwnd(), SW_HIDE);
	tmpEdit = m_EditWithLabelBtn;
	tmpPopWinCode = m_PopupContractCode2;
	tmpPopupCodeList = m_PopupCodeList2;
}

CxSpinEdit BaseControl::CreateLableSpin(HWND hParent, const TCHAR* pszText, ControlPos pos, RECT &rect,UINT id , UINT style, UINT specialStyle)
{
	//买超价
	rect.left = pos.LEFT;
	rect.top = rect.bottom; 
	rect.right = rect.left + pos.WIDGETV;
	rect.bottom = rect.top + pos.WIDGETH;

	m_LabelWithSpin.Create(hParent, pszText);
	m_LabelWithSpin.MoveWindow(pos.LEFT, rect.top, pos.WIDGETV, pos.WIDGETH);
	//买超价spin
	rect.left = pos.LEFT;
	rect.top = rect.bottom;
	rect.right = rect.left + pos.WIDGETV;
	rect.bottom = rect.top + pos.WIDGETH;
	m_SpinWithLable.Create(hParent, CxEdit::intStyle);
	m_SpinWithLable.SetWindowNumber(1);
	m_SpinWithLable.SetRange(-100, 100);
	m_SpinWithLable.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	return m_SpinWithLable;
}

TStaticButton BaseControl::CreateSingleButton(HWND hParent, const char* pszText, ControlPos pos, RECT &rect, UINT id)
{
	m_SingleButton.Create(hParent, pszText, id);
	m_SingleButton.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	return m_SingleButton;
}

TListView BaseControl::CreateListView(HWND &hParent,map<string,int> &tmpMap, ControlPos pos, RECT &rect, TListView &tmpListView , UINT id,
	UINT style, int itemHeight, int headerHeight, UINT exStyle)
{
	//TListView listView;
	m_ListView.CreateList(hParent, style, itemHeight, headerHeight, exStyle, id);
	m_ListView.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	for (auto iter = tmpMap.begin();iter != tmpMap.end();++iter)
	{
		m_ListView.AddColumn(ANSIToUnicode(iter->first).c_str(), iter->second);
	}
	//tmpListView = m_ListView;
	return m_ListView;
}
CxCombox BaseControl::CreateLableComboBox(HWND hParent, const TCHAR* pszText, vector<string> &tmpVec, RECT &rect, UINT id)
{
	//Label
	//TStatic tmpLabel;
	tmpLabelWithCb.Create(hParent, pszText);
	tmpLabelWithCb.MoveWindow(rect.left, rect.top, (rect.right - rect.left) / 2, 20);

	//CxCombox cbWithLable;
	m_cbWithLable.Create(hParent,id);
	m_cbWithLable.MoveWindow(((rect.left + rect.right - rect.left) / 2 + 5), rect.top, (rect.right - rect.left) / 2, rect.bottom - rect.top);

	for(auto iter = tmpVec.begin();iter != tmpVec.end();++iter)
	{
		//m_cbWithLable.AddString(ANSIToUnicode(*iter).c_str());
		SendMessage(m_cbWithLable.GetHwnd(), CB_INSERTSTRING, (WPARAM)(-1), (LPARAM)ANSIToUnicode(*iter).c_str());
	}
	return m_cbWithLable;
}



