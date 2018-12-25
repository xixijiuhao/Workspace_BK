#include "PreInclude.h"

#define CtrlHInterval 6

using namespace std;

extern G_UIFONT g_FontData;			//字体
extern G_COLOREF g_ColorRefData;	//色彩
extern ITradeData* g_pTradeData;

#define MaxQtyLimit		9
#define MaxQtyLimitLen	1

uchar StrToVk(char cKey[5])
{
	if (cKey[0] != '\0')
	{
		char c = cKey[0];
		SHORT vk = VkKeyScan(c);
		return  vk & 0xFF;
	}
	else
		return '\0';
}

wstring FormatHotKey(int key)
{
	wchar_t cc[101] = { 0 };
	char c = MapVirtualKey(key, MAPVK_VK_TO_CHAR);
	swprintf_s(cc, L"%c", MapVirtualKey(key, MAPVK_VK_TO_CHAR));
	return cc;	
}
int FormatStrKey(HWND hEdit)
{
	char cKey[20] = { 0 };
	GetWindowTextA(hEdit, cKey, 20);
	if (strlen(cKey) > 2)
		return '\0';
	else if (cKey[0] != '\0')
	{
		char c = cKey[0];
		SHORT vk = VkKeyScan(c);
		return  vk & 0xFF;
	}
	return '\0';
}
LRESULT CfgEditWndProc(HWND hwnd, UINT message, WPARAM w, LPARAM l)
{
	int id = GetWindowLong(hwnd, GWL_ID);
	KeyBoardSet *p_KeyBoardSet = (KeyBoardSet *)GetWindowLong(hwnd, GWL_USERDATA);
	if (!p_KeyBoardSet)
		return FALSE;

	if (message == WM_KEYDOWN)
	{
		switch (id)
		{
		case KB_Buy_Edit:
		case KB_Sell_Edit:
		case KB_Open_Edit:
		case KB_Cover_Edit:
		case KB_CoverT_Edit:
			if (w == VK_PROCESSKEY)
				w = ImmGetVirtualKey(hwnd);
			{
				char c = MapVirtualKey(w, MAPVK_VK_TO_CHAR);
				if ((c >= 48 && c <= 57)||(c >= 65 && c <= 90)||(c>=97&&c<=122))
				{
					char cc[101] = { 0 };
					if (0 != c && c > 32)//可见字符
						sprintf_s(cc, ("%c"), c);
					SetWindowTextA(hwnd, cc);
					if (!p_KeyBoardSet->CheckValidKey(id))
					{
						SetWindowTextA(hwnd, (""));
						return FALSE;
					}
				}
				else
				{
					SetWindowTextA(hwnd, (""));
					return FALSE;
				}				
			}
		default:
			break;
		}
	}
	else if (message == WM_CHAR)
		return FALSE;
	map<int, WNDPROC>::iterator it = p_KeyBoardSet->m_SetEdit.find(id);
	if (it == p_KeyBoardSet->m_SetEdit.end())
		return FALSE;
	else
		return CallWindowProc(it->second, hwnd, message, w, l);
}
KeyBoardSet::KeyBoardSet()
{
	LoadParam();
}
void KeyBoardSet::Regist(IKeyOrderCfg* p)
{
	if (p)
	{
		m_vNotify.insert(p);
		p->OnKeyBoardCfgChange(&m_KeyBoardCfg);
		p->OnKeyBoardCfgFontChange(&m_KyeBoardFontCfg);
	}	
}
void KeyBoardSet::UnRegist(IKeyOrderCfg* p)
{
	if (m_vNotify.size() == 0)
		return;
	set<IKeyOrderCfg*>::iterator it = m_vNotify.find(p);
	if (it != m_vNotify.end())
		m_vNotify.erase(p);
}
LRESULT KeyBoardSet::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:					OnCreateWindow();
		break;
	case WM_NOTIFY:					OnNotify(wParam,lParam);
		break;
	case WM_MOUSEWHEEL:				OnMouseWheel(wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:			return (LONG)m_hStaticBKBrush;
	case WM_COMMAND:				OnCommand(wParam, lParam);
		break;
	case WM_DESTROY:
		SaveParam();
		break;
	case WM_MEASUREITEM:
		OnMeasureListView(lParam);
		break;
	case WM_DRAWITEM:
		OnDrawListView(wParam, lParam);
		break;
	default:
		return NOT_PROCESSED;
	}
	return PROCESSED;
}

void KeyBoardSet::Create(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	CreateFrm(classname, parent, style, exstyle);
}

void KeyBoardSet::LayOut()
{
	int nCurHeight = 0;
	int nRightStart = 300;
	int nCenterHeight = 0;
	RECT rect;
	rect.left = 0;
	rect.right = 586;
	rect.top = 0;

	nCurHeight += CtrlHInterval * 3;
	
	MoveCtrlWindow(m_hControl[KB_FocusPos_Check], nCurHeight, nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_FocusPos_Edit], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_FocusPos_Spin], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_FocusPos_Static2], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_UpDownEqualTab_Check], nCenterHeight);
	MoveCtrlColWindow(m_hControl[KB_UpDownEqualTab_Check], nRightStart);

	nCurHeight += CtrlHInterval;

	MoveCtrlWindow(m_hControl[KB_KeyBoardStyle_Static], nCurHeight, nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_KeyBoardStyle_Combox], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_RightLeftEqualTab_Check], nCenterHeight);
	MoveCtrlColWindow(m_hControl[KB_RightLeftEqualTab_Check], nRightStart);

	nCurHeight += CtrlHInterval;

	MoveCtrlWindow(m_hControl[KB_KeyBoardFont_Static], nCurHeight, nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_KeyBoardFont_Combox], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_ShowOrderValidType_Check], nCenterHeight);
	MoveCtrlColWindow(m_hControl[KB_ShowOrderValidType_Check], nRightStart);

	nCurHeight += CtrlHInterval;
	MoveCtrlWindow(m_hControl[KB_DefaultPrice_Static], nCurHeight, nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_DefaultPrice_Combox	], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_DefaultPrice_Static2], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_DefaultPrice_Edit	], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_DefaultPrice_Spin	], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_DefaultPrice_Static3], nCenterHeight);

	MoveCtrlRowWindow(m_hControl[KB_EnterSend_Check], nCenterHeight);
	MoveCtrlColWindow(m_hControl[KB_EnterSend_Check], nRightStart);
	if (!CConvert::IsTraderLogined())
		ShowWindow(m_hControl[KB_EnterSend_Check], SW_SHOW);
	else
		ShowWindow(m_hControl[KB_EnterSend_Check], SW_HIDE);

	//追单助手开启
	nCurHeight += CtrlHInterval;
	MoveCtrlWindow(m_hControl[KB_Start_Order_Assistant_Check], nCurHeight, nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_Start_Order_Assistant_Check], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_AssistType_Combox], nCenterHeight);
	int sencondStart = nRightStart - 190;
	MoveCtrlColWindow(m_hControl[KB_AssistType_Combox], sencondStart);

	rect.bottom = nCurHeight + CtrlHInterval * 2;
	MoveWindow(m_hControl[KB_Set_GroupBox], rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	rect.top = rect.bottom + CtrlHInterval * 2;

	nCurHeight = rect.top + CtrlHInterval * 3;
	MoveCtrlWindow(m_hControl[KB_Buy_Static], nCurHeight, nCenterHeight);
	for (int n = KB_Buy_Edit; n <= KB_Cover_Edit; n ++)
		MoveCtrlRowWindow(m_hControl[n], nCenterHeight);

	rect.bottom = nCurHeight + CtrlHInterval * 2;
	MoveWindow(m_hControl[KB_Normal_GroupBox], rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);

	rect.top = rect.bottom + CtrlHInterval * 2;
	nCurHeight = rect.top + CtrlHInterval * 3;
	m_ptPopup.y = nCurHeight + 20 + 5;
	MoveCtrlWindow(m_hControl[KB_HK_Contract_Hot_Static], nCurHeight, nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_HK_Contract_Hot_Edit], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_HK_Contract_Name_Static], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_HK_Contract_Name_Edit], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_HK_Contract_Name_Btn], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_HK_Add_Btn], nCenterHeight);
	MoveCtrlRowWindow(m_hControl[KB_HK_Del_Btn], nCenterHeight);


	m_ListCHotKey.MoveWindow(12, nCurHeight+10 , 220, 100);
	InvalidateRect(m_ListCHotKey.GetListHwnd(), nullptr, true);
	nCurHeight += CtrlHInterval * 2 + 100 ;

	rect.bottom = nCurHeight + CtrlHInterval * 2 + 10;
	MoveWindow(m_hControl[KB_HK_GroupBox], rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

RECT KeyBoardSet::MoveCtrlWindow(HWND hwnd, int &nHeightStart, int &nHeightCenter)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	ScreenToClient(m_Hwnd, LPPOINT(&rect));
	ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);

	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	rect.top = nHeightStart;
	rect.bottom = rect.top + nHeight;
	nHeightCenter = rect.top + nHeight / 2;
	nHeightStart += nHeight;
	MoveWindow(hwnd, rect.left, rect.top, nWidth, nHeight, TRUE);
	return rect;
}

RECT KeyBoardSet::MoveCtrlRowWindow(HWND hwnd, int nHeightCenter)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	ScreenToClient(m_Hwnd, LPPOINT(&rect));
	ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	rect.top = nHeightCenter - nHeight / 2;
	rect.bottom = rect.top + nHeight;
	MoveWindow(hwnd, rect.left, rect.top, nWidth, nHeight, TRUE);
	return rect;
}

RECT KeyBoardSet::MoveCtrlColWindow(HWND hwnd, int &nWidthMove)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	ScreenToClient(m_Hwnd, LPPOINT(&rect));
	ScreenToClient(m_Hwnd, LPPOINT(&rect) + 1);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	if (rect.left < nWidthMove)
		rect.left = rect.left + nWidthMove;
	rect.right = rect.left + nHeight;

	MoveWindow(hwnd, rect.left, rect.top, nWidth, nHeight, TRUE);
	return rect;
}

void KeyBoardSet::OnCreateWindow()
{
	m_hStaticBKBrush = g_ColorRefData.GetHbrWhite();//背景色
	const int nPitch = 1;
	const int nLeftStart = 12;			//统一左侧起点
	const int nShortText = 92;			//短文文本宽度
	const int nNormalHeight = 20;		//统一高度
	const int nEditWidth = 20;			//统一高度
	const int nSpinWidth = 16;
	const int nLeftCheckWidth = 261;	//统一的Check宽度
	
	const int nStartStatic = nLeftStart + 156 + nPitch;					//Combox
	const int nRightAlign = nStartStatic + nShortText;

	HINSTANCE hInstance = GetModuleHandle(NULL);

	m_hControl[KB_Set_GroupBox] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Setting).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, nLeftStart - 2, 6, 580, nNormalHeight * 8, m_Hwnd, (HMENU)KB_Set_GroupBox, hInstance, NULL);
	m_hControl[KB_Normal_GroupBox] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_FastOperator).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, nLeftStart - 2, 6, 580, nNormalHeight * 8, m_Hwnd, (HMENU)KB_Normal_GroupBox, hInstance, NULL);
	
	m_hControl[KB_FocusPos_Check] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_DefaultFocus).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_FLAT, nLeftStart, 31, nRightAlign - nEditWidth - nSpinWidth - 2, nNormalHeight, m_Hwnd, (HMENU)KB_FocusPos_Check, hInstance, NULL);
	m_hControl[KB_FocusPos_Edit] = CreateWindowEx(0, TEXT("edit"), TEXT("0"), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER, nRightAlign - nEditWidth- nSpinWidth - 2, 6, nEditWidth, nNormalHeight, m_Hwnd, (HMENU)KB_FocusPos_Edit, hInstance, NULL);
	m_hControl[KB_FocusPos_Spin] = CreateWindowEx(0, TEXT("msctls_updown32"), TEXT(""), WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_NOTHOUSANDS, nRightAlign-nSpinWidth-1, 6, nSpinWidth, nNormalHeight, m_Hwnd, (HMENU)KB_FocusPos_Spin, hInstance, NULL);
	m_hControl[KB_FocusPos_Static2] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_Item).c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, nRightAlign, 31, nEditWidth + 4, nNormalHeight, m_Hwnd, (HMENU)KB_FocusPos_Static2, hInstance, NULL);
	SendMessage(m_hControl[KB_FocusPos_Edit], EM_SETLIMITTEXT, MaxQtyLimitLen, 0);

	m_hControl[KB_UpDownEqualTab_Check] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_UpDownEqualTab).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_FLAT, nLeftStart, 6, nLeftCheckWidth, nNormalHeight, m_Hwnd, (HMENU)KB_UpDownEqualTab_Check, hInstance, NULL);
	m_hControl[KB_RightLeftEqualTab_Check] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_LeftRightEqualTab).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_FLAT, nLeftStart, 6, nLeftCheckWidth, nNormalHeight, m_Hwnd, (HMENU)KB_RightLeftEqualTab_Check, hInstance, NULL);
	
	m_hControl[KB_KeyBoardStyle_Static] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_KeyboardStyle).c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, nLeftStart, 31, nShortText+15, nNormalHeight, m_Hwnd, (HMENU)KB_KeyBoardStyle_Static, hInstance, NULL);
	m_hControl[KB_KeyBoardStyle_Combox] = CreateWindowEx(0, TEXT("COMBOBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP, nStartStatic - 45, 31, nShortText + 45, 132, m_Hwnd, (HMENU)KB_KeyBoardStyle_Combox, hInstance, NULL);
	SendMessage(m_hControl[KB_KeyBoardStyle_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_OnlyOkButton).c_str());
	SendMessage(m_hControl[KB_KeyBoardStyle_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_OnlyBuySell).c_str());
	SendMessage(m_hControl[KB_KeyBoardStyle_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_ConfirmPayAuto).c_str());
	
	m_hControl[KB_KeyBoardFont_Static] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_FontSize).c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, nLeftStart, 31, nShortText+15, nNormalHeight, m_Hwnd, (HMENU)KB_KeyBoardFont_Static, hInstance, NULL);
	m_hControl[KB_KeyBoardFont_Combox] = CreateWindowEx(0, TEXT("COMBOBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP, nStartStatic, 31, nShortText, 132, m_Hwnd, (HMENU)KB_KeyBoardFont_Combox, hInstance, NULL);
	SendMessage(m_hControl[KB_KeyBoardFont_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_FontSmall).c_str());
	SendMessage(m_hControl[KB_KeyBoardFont_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_FontMid).c_str());
	SendMessage(m_hControl[KB_KeyBoardFont_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_FontBig).c_str());

	m_hControl[KB_ShowOrderValidType_Check] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_ShowOrderValidType).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_FLAT, nLeftStart, 31, nLeftCheckWidth, nNormalHeight, m_Hwnd, (HMENU)KB_ShowOrderValidType_Check, hInstance, NULL);
	m_hControl[KB_EnterSend_Check] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_StartHEnterSend).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_FLAT, nLeftStart, 6, nLeftCheckWidth, nNormalHeight, m_Hwnd, (HMENU)KB_EnterSend_Check, hInstance, NULL);
	m_hControl[KB_Start_Order_Assistant_Check] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Use_Assistant).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_FLAT, nLeftStart, 6, nLeftCheckWidth, nNormalHeight, m_Hwnd, (HMENU)KB_Start_Order_Assistant_Check, hInstance, NULL);
	m_hControl[KB_AssistType_Combox] = CreateWindowExW(0, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP, 5 /*+ nLeftStart*/ + nShortText, 0, nShortText + 100, 200, m_Hwnd, (HMENU)KB_AssistType_Combox, hInstance, NULL);
	SendMessage(m_hControl[KB_AssistType_Combox], CB_ADDSTRING, 0, (LPARAM)LoadResString(IDS_Chase_Open_Close).c_str());
	SendMessage(m_hControl[KB_AssistType_Combox], CB_ADDSTRING, 0, (LPARAM)LoadResString(IDS_Chase_Open).c_str());
	SendMessage(m_hControl[KB_AssistType_Combox], CB_ADDSTRING, 0, (LPARAM)LoadResString(IDS_Chase_Close).c_str());

	m_hControl[KB_DefaultPrice_Static	] = CreateWindowExW(0, L"static", LoadResString(IDS_DefaultPrice).c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, nLeftStart, 0, nShortText, nNormalHeight, m_Hwnd, (HMENU)KB_DefaultPrice_Static, hInstance, NULL);
	m_hControl[KB_DefaultPrice_Combox	] = CreateWindowExW(0, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP, 3+nLeftStart + nShortText, 0, nShortText, 200, m_Hwnd, (HMENU)KB_DefaultPrice_Combox, hInstance, NULL);
	m_hControl[KB_DefaultPrice_Static2	] = CreateWindowExW(0, L"static", L"+", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,  3+nLeftStart + nShortText + nShortText + 4, 0, nSpinWidth, nNormalHeight, m_Hwnd, (HMENU)KB_DefaultPrice_Static2, hInstance, NULL);
	m_hControl[KB_DefaultPrice_Edit		] = CreateWindowExW(0, L"edit", L"0", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER, 3+nLeftStart + nShortText + nShortText + nSpinWidth + nPitch, 0, nEditWidth + 6, nNormalHeight, m_Hwnd, (HMENU)KB_DefaultPrice_Edit, hInstance, NULL);
	m_hControl[KB_DefaultPrice_Spin		] = CreateWindowExW(0, L"msctls_updown32", L"", WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_NOTHOUSANDS, 3+nLeftStart + nShortText + nShortText + nSpinWidth + nPitch + nEditWidth + 7, 0, nSpinWidth, nNormalHeight, m_Hwnd, (HMENU)KB_DefaultPrice_Spin, hInstance, NULL);
	m_hControl[KB_DefaultPrice_Static3	] = CreateWindowExW(0, L"static", LoadResString(IDS_Tick).c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE, 3+nLeftStart + nShortText + nShortText + nSpinWidth + nPitch + nEditWidth + 8+nSpinWidth, 0, 30, nNormalHeight, m_Hwnd, (HMENU)KB_DefaultPrice_Static3, hInstance, NULL);
	SendMessage(m_hControl[KB_DefaultPrice_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_STRING_DPLastPrice).c_str());
	SendMessage(m_hControl[KB_DefaultPrice_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_STRING_DPOppoPrice).c_str());
	SendMessage(m_hControl[KB_DefaultPrice_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_STRING_DPQuenePrice).c_str());
	SendMessage(m_hControl[KB_DefaultPrice_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_STRING_DPNoPrice).c_str());
	SendMessage(m_hControl[KB_DefaultPrice_Combox], CB_ADDSTRING, 0, (LPARAM)(LPWSTR)LoadResString(IDS_STRING_GetPrice).c_str());
	SendMessage(m_hControl[KB_DefaultPrice_Edit], EM_SETLIMITTEXT, 3, 0);


	SetWindowPos(m_hControl[KB_FocusPos_Spin], NULL, 0, 0, nSpinWidth, nNormalHeight, SWP_NOMOVE | SWP_NOZORDER);

	for (int n = KB_Buy_Static; n <= KB_Cover_Static/*KB_Lesser_Static*/; n = n + 2)
	{
		m_hControl[n] = CreateWindowEx(0, TEXT("static"), LoadResString(IDS_Direct_Buy + (n - KB_Buy_Static) / 2).c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, nLeftStart + (n - KB_Buy_Static) / 2 % 5 * 110, 6, 45, nNormalHeight, m_Hwnd, (HMENU)n, hInstance, NULL);
		m_hControl[n + 1] = CreateWindowEx(0, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, nLeftStart + (n - KB_Buy_Static) / 2 % 5 * 110 + 45, 6, 35, nNormalHeight, m_Hwnd, (HMENU)(n + 1), hInstance, NULL);
		m_Edit[(n - KB_Buy_Static) / 2].Init(m_hControl[n + 1]);
	}
	m_ComboxFont.Init(m_hControl[KB_KeyBoardFont_Combox]);
	m_ComboxStyle.Init(m_hControl[KB_KeyBoardStyle_Combox]);
	m_EditDefaultPos.Init(m_hControl[KB_FocusPos_Edit]);
	m_SpinDefaultPos.Init(m_hControl[KB_FocusPos_Spin]);
	
	m_ComboxDefPrice.Init(m_hControl[KB_DefaultPrice_Combox]);
	m_EditDefPrice.Init(m_hControl[KB_DefaultPrice_Edit]);
	m_SpinDefPrice.Init(m_hControl[KB_DefaultPrice_Spin]);

	/*
	合约快捷键2017-5-25
	*/
	int xlayout = nLeftStart;
	m_ptPopup = { 0,0 };
	m_hControl[KB_HK_Contract_Hot_Static] = CreateWindowExW(0, L"static", LoadResString(IDS_Shortcut_Key).c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, xlayout, 0, nShortText-10, nNormalHeight, m_Hwnd, (HMENU)KB_HK_Contract_Hot_Static, hInstance, NULL);
	xlayout += nShortText -10 + nPitch*2;
	m_hControl[KB_HK_Contract_Hot_Edit] = CreateWindowExW(0, L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_UPPERCASE,  xlayout, 0, nShortText+10, nNormalHeight, m_Hwnd, (HMENU)KB_HK_Contract_Hot_Edit, hInstance, NULL);
	xlayout += nShortText + 10 + nPitch * 2;
	m_hControl[KB_HK_Contract_Name_Static] = CreateWindowExW(0, L"static", LoadResString(IDS_STRING_ChangeCode).c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, xlayout, 0, nShortText-10, nNormalHeight, m_Hwnd, (HMENU)KB_HK_Contract_Name_Static, hInstance, NULL);
	xlayout += nShortText - 10 + nPitch * 2;
	m_hControl[KB_HK_Contract_Name_Edit] = CreateWindowExW(0, L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_UPPERCASE,xlayout, 0, nShortText+10, nNormalHeight, m_Hwnd, (HMENU)KB_HK_Contract_Name_Edit, hInstance, NULL);
	m_ptPopup.x = xlayout;
	xlayout += nShortText + 10 + nPitch * 2;
	m_hControl[KB_HK_Contract_Name_Btn] = CreateWindowExW(0, L"button", LoadResString(1).c_str(), WS_CHILD | WS_VISIBLE,xlayout, 0, 25, nNormalHeight, m_Hwnd, (HMENU)KB_HK_Contract_Name_Btn, hInstance, NULL);
	xlayout += 25 + nPitch * 7;
	m_hControl[KB_HK_Add_Btn] = CreateWindowExW(0, L"button", LoadResString(IDS_Add).c_str(), WS_CHILD | WS_VISIBLE, xlayout, 0, 56, nNormalHeight+5, m_Hwnd, (HMENU)KB_HK_Add_Btn, hInstance, NULL);
	xlayout += 56 + nPitch * 2;
	m_hControl[KB_HK_Del_Btn] = CreateWindowExW(0, L"button", LoadResString(IDS_Delete).c_str(), WS_CHILD | WS_VISIBLE, xlayout, 0, 56, nNormalHeight+5, m_Hwnd, (HMENU)KB_HK_Del_Btn, hInstance, NULL);

	m_EditCHotKey.Init(m_hControl[KB_HK_Contract_Hot_Edit]);
	m_EditCName.Init(m_hControl[KB_HK_Contract_Name_Edit]);
	::SendMessage(m_hControl[KB_HK_Contract_Hot_Edit], EM_SETLIMITTEXT, 10, 0);
	::SendMessage(m_hControl[KB_HK_Contract_Name_Edit], EM_SETLIMITTEXT, 20, 0);
	m_BtnContract.Init(m_hControl[KB_HK_Contract_Name_Btn]);
	m_BtnContract.SetButtonText(L"..");
	m_BtnAdd.Init(m_hControl[KB_HK_Add_Btn]);
	m_BtnAdd.SetButtonText(LoadResString(IDS_Add).c_str());
	m_BtnDel.Init(m_hControl[KB_HK_Del_Btn]);
	m_BtnDel.SetButtonText(LoadResString(IDS_Delete).c_str());
	m_ListCHotKey.CreateList(m_Hwnd, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED | WS_BORDER, 20, 18, LVS_EX_FULLROWSELECT, KB_HK_List);
	m_hControl[KB_HK_List] = m_ListCHotKey.GetListHwnd();
	m_ListCHotKey.MoveWindow(nLeftStart, 0, 220, 100);
	m_ListCHotKey.AddColumn(LoadResString(IDS_Shortcut_Key).c_str(), 110);
	m_ListCHotKey.AddColumn(LoadResString(IDS_STRING_ChangeCode).c_str(), 110);
	
	m_hControl[KB_HK_GroupBox] = CreateWindowEx(0, TEXT("button"), LoadResString(IDS_Contract_Shortcut_Key).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, nLeftStart - 2, 6, 580, nNormalHeight * 8, m_Hwnd, (HMENU)KB_HK_GroupBox, hInstance, NULL);
	/*
	End
	*/

	for (int n = KB_Buy_Edit; n <= KB_Cover_Edit/*KB_Lesser_Edit*/; n = n + 2)
	{
		WNDPROC w = (WNDPROC)SetWindowLong(m_hControl[n], GWL_WNDPROC, (LONG)CfgEditWndProc);	//默认价格
		m_SetEdit.insert(pair<int, WNDPROC>(n, w));
		SetWindowLong(m_hControl[n], GWL_USERDATA, LONG(this));//用户数据
	}
	
	//设置控件字体
	for (int i = KB_START; i <= KB_END; i++)
	{
		if (m_hControl[i] != 0)
		{
			HWND h = m_hControl[i];
			SendMessage(m_hControl[i], WM_SETFONT, (WPARAM)g_FontData.GetFontWord13(), (LPARAM)TRUE);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	m_CheckFocusPos.Init(m_hControl[KB_FocusPos_Check]);
	m_CheckUpDownEqualTab.Init(m_hControl[KB_UpDownEqualTab_Check]);
	m_CheckRightLeftEqualTab.Init(m_hControl[KB_RightLeftEqualTab_Check]);
	m_CheckShowOrderValidType.Init(m_hControl[KB_ShowOrderValidType_Check]);
	m_CheckEnterSendOrder.Init(m_hControl[KB_EnterSend_Check]);
	m_CheckStartOrderAssistant.Init(m_hControl[KB_Start_Order_Assistant_Check]);
	m_ComboxAssistStyle.Init(m_hControl[KB_AssistType_Combox]);
	//控件布局
	LayOut();

	LoadParam(true);	
}

void KeyBoardSet::OnSpinDeltaUpDownNormal(int nDelta, int nID, int nMax, int nMin)
{
	if (nDelta > 0)
	{
		string sInt = "";
		sInt = GetWindowTextAnsic(m_hControl[nID]);
		int a = atoi(sInt.c_str());
		a++;
		a = a > nMax ? nMax : a;
		SetWindowInt(m_hControl[nID], a);
	}
	else
	{
		string sInt = "";
		sInt = GetWindowTextAnsic(m_hControl[nID]);
		int a = atoi(sInt.c_str());
		a--;
		a = a < nMin ? nMin : a;
		SetWindowInt(m_hControl[nID], a);
	}
}

void KeyBoardSet::OnNotify(WPARAM wParam, LPARAM lParam)
{	
	if (LOWORD(wParam) == KB_FocusPos_Spin && ((LPNMHDR)lParam)->code == UDN_DELTAPOS)
		OnSpinDeltaUpDownNormal(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1, MaxQtyLimit);
	else if (LOWORD(wParam) == KB_DefaultPrice_Spin)
		OnSpinDeltaUpDownNormal(-((LPNMUPDOWN)lParam)->iDelta, LOWORD(wParam) - 1, 999, -99);

	LPNMHDR p = (LPNMHDR)lParam;
	if (p->hwndFrom == m_ListCHotKey.GetListHwnd() && p->code == NM_CLICK && m_ListCHotKey.GetSelectItem() != -1)
	{
		char temp[21];
		m_ListCHotKey.GetSelectItem(temp,20,0);
		wstring strHotKey = LoadRC::ansi_to_unicode(temp);
		std::map<std::wstring,std::wstring>::iterator iter = m_stdmapContract.find(strHotKey);
		if (iter != m_stdmapContract.end())
		{
			SetWindowText(m_EditCHotKey.GetHwnd(), strHotKey.c_str());
			SetWindowText(m_EditCName.GetHwnd(), iter->second.c_str());
		}

	}
}

void KeyBoardSet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (LOWORD(wParam))
	{
	case KB_KeyBoardFont_Combox:
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			ChangeFacadeFont(m_KyeBoardFontCfg, true, true);
		}
		break;
	case KB_DefaultPrice_Combox:
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			ChangeDefaultPrice();
		}
		break;
	case KB_HK_Add_Btn:
	{
		wstring strHotKey;
		wstring strCode;
		wchar_t tempStr[21];
		GetWindowTextW(m_EditCHotKey.GetHwnd(), tempStr, 20);
		strHotKey = wstring(tempStr);
		GetWindowTextW(m_EditCName.GetHwnd(),tempStr,20);
		strCode = wstring(tempStr);
		if (strHotKey == L"" || strCode == L"")break;
		else
		{
			std::map<std::wstring, std::wstring>::iterator iter = m_stdmapContract.find(strHotKey);
			if (iter != m_stdmapContract.end())
			{
				m_stdmapContract.erase(strHotKey);
				m_stdmapContract.insert(std::make_pair(strHotKey, strCode));
			}
			else
			{
				m_stdmapContract.insert(std::make_pair(strHotKey, strCode));
			}
			UpdateHKList();
		}
	}
		break;
	case KB_HK_Del_Btn:
	{
		char temp[21];
		if (-1 != m_ListCHotKey.GetSelectItem(temp, 20, 0))
		{
			wstring strHotKey = LoadRC::ansi_to_unicode(temp);
			std::map<std::wstring, std::wstring>::iterator iter = m_stdmapContract.find(strHotKey);
			if (iter != m_stdmapContract.end())
			{
				m_stdmapContract.erase(iter);
			}
			UpdateHKList();
		}

	}
		break;
	case KB_HK_Contract_Name_Btn:
	{
		POINT ptTemp = { 0,0 };
		ptTemp.x = m_ptPopup.x;
		ptTemp.y = m_ptPopup.y;
		ClientToScreen(m_Hwnd, &ptTemp);
		if (!IsWindow(m_PopupContractCode.GetHwnd()))
		{
			m_PopupContractCode.CreatePopWin(this, TEXT("m_PopupContractCode"), m_Hwnd, WS_CLIPCHILDREN, 0, ptTemp.x, ptTemp.y);
			m_PopupContractCode.UpdateData();
			::ShowWindow(m_PopupContractCode.GetHwnd(), SW_SHOWNORMAL);
		}
		else
		{
			m_PopupContractCode.UpdateData();
			SetWindowPos(m_PopupContractCode.GetHwnd(), NULL, ptTemp.x, ptTemp.y, 0, 0, SWP_NOSIZE);
			::ShowWindow(m_PopupContractCode.GetHwnd(), SW_SHOWNORMAL);
		}
	}
		break;
	}
}
void KeyBoardSet::OnMouseWheel(WPARAM wParam, LPARAM lParam)					//鼠标滚轮
{
	LONG ID = GetWindowLong(GetFocus(), GWL_ID);
	if (ID == KB_FocusPos_Edit)
		OnSpinDeltaUpDownNormal(GET_WHEEL_DELTA_WPARAM(wParam), ID, MaxQtyLimit);
	else if (ID == KB_DefaultPrice_Edit)
		OnSpinDeltaUpDownNormal(GET_WHEEL_DELTA_WPARAM(wParam), ID, 999, -99);
}

void KeyBoardSet::get_param_file(char * buf, size_t len)
{
	GetModuleFileNameA(nullptr, buf, len);
	char * p = strrchr(buf, '\\');
	len = buf + len - p;
	strncpy_s(++p, len - 1, "config\\KeyBoardOrder.Default.pri", len);
}
int KeyBoardSet::read_param(const char * ident, const int def)
{
	char file[MAX_PATH];
	get_param_file(file, sizeof(file));

	char buf[21];
	int count = GetPrivateProfileStringA("Layout", ident, "0", buf, sizeof(buf), file);
	if (count == 0)
		return def;
	else
	{
		int ret = atoi(buf);
		return ret;
	} 
}
void KeyBoardSet::write_param(const char * ident, const int value)
{
	char file[MAX_PATH];
	get_param_file(file, sizeof(file));

	char buf[21];
	_itoa_s(value, buf, 10);
	if (!WritePrivateProfileStringA("Layout", ident, buf, file))
		_ASSERT(0);
}

int KeyBoardSet::read_paramHK(uint ident, wstring& key, wstring& value)
{
	char file[MAX_PATH];
	get_param_file(file, sizeof(file));

	char keybuf[21];
	_itoa_s(ident, keybuf, 10);
	char buf[31];
	int count = GetPrivateProfileStringA("ContractHotkey", keybuf, "0", buf, sizeof(buf), file);
	if (count == 0)
		return 0;
	else
	{
		std::vector<std::string> vecPriBuffer;
		vecPriBuffer = CConvert::SplitString(buf, '|');
		if (vecPriBuffer.size() != 2)return 0;
		else
		{
			key =LoadRC::ansi_to_unicode(vecPriBuffer[0]);
			value = LoadRC::ansi_to_unicode(vecPriBuffer[1]);
		}
		return count;
	}
}
void KeyBoardSet::write_paramHK(uint ident, const wchar_t* value)
{
	char file[MAX_PATH];
	get_param_file(file, sizeof(file));
	char buf[21];
	_itoa_s(ident, buf, 10);
	if (!WritePrivateProfileStringA("ContractHotkey",buf ,LoadRC::unicode_to_ansi(value).c_str(), file))
		_ASSERT(0);
}

bool KeyBoardSet::LoadParam(bool bUpdateToUI)
{		
	//横向下单配置
	m_KeyBoardCfg.bSetDefaultPos		= read_param("bSetDefaultPos"		, 0) != 0;
	m_KeyBoardCfg.bShowOrderValidType	= read_param("bShowOrderValidType"	, 0) != 0;
	m_KeyBoardCfg.bEnterSendOrder		= read_param("bEnterSendOrder"		, 0) != 0;
	m_KeyBoardCfg.bUpDownEqualTab		= read_param("bUpDownEqualTab"		, 0) != 0; 
	m_KeyBoardCfg.bRightLeftEqualTab	= read_param("bRightLeftEqualTab"	, 0) != 0;
	m_KeyBoardCfg.uiFocusPos			= read_param("uiFocusPos"			, 1);
	m_KeyBoardCfg.uiDoOrderStyle		= read_param("uiDoOrderStyle"		, 2);
	m_KeyBoardCfg.uiKeyBoardFont		= read_param("uiKeyBoardFont"		, 0);
	m_KeyBoardCfg.ucBuy					= read_param("ucBuy"				, 1);
	m_KeyBoardCfg.ucSell				= read_param("ucSell"				, 3);
	m_KeyBoardCfg.ucOpen				= read_param("ucOpen"				, 1);
	m_KeyBoardCfg.ucCover				= read_param("ucCover"				, 2);
	m_KeyBoardCfg.ucCoverT				= read_param("ucCoverT"				, 3);

	m_KeyBoardCfg.OutPriceTick			= read_param("OutPriceTick"			, 0);
	m_KeyBoardCfg.OutPriceType			= read_param("OutPriceType"			, 2);
	m_KeyBoardCfg.bStartOrderAssistant  = read_param("bStartOrderAssistant"	, 0);
	m_KeyBoardCfg.uiAssistType			= read_param("uiAssistType"			, 0);
	
	//字体配置
	ChangeFacadeFont(m_KyeBoardFontCfg);
	
	if (!bUpdateToUI)
		return true;
	
	//数据刷新到界面
	wchar_t cc[101] = { 0 };
	//默认焦点位置
	m_CheckFocusPos.SetCheck(m_KeyBoardCfg.bSetDefaultPos ? BST_CHECKED : BST_UNCHECKED);
	swprintf_s(cc, TEXT("%d"), m_KeyBoardCfg.uiFocusPos);
	Edit_SetText(m_hControl[KB_FocusPos_Edit], cc);

	//上下键 tab
	m_CheckUpDownEqualTab.SetCheck(m_KeyBoardCfg.bUpDownEqualTab ? BST_CHECKED : BST_UNCHECKED);
	//左右键 tab
	m_CheckRightLeftEqualTab.SetCheck(m_KeyBoardCfg.bRightLeftEqualTab ? BST_CHECKED : BST_UNCHECKED);
	
	//横向下单风格
	if (0 == m_KeyBoardCfg.uiDoOrderStyle)
		ComboBox_SelectString(m_hControl[KB_KeyBoardStyle_Combox], -1, LoadResString(IDS_OnlyOkButton).c_str());
	else if (1 == m_KeyBoardCfg.uiDoOrderStyle)
		ComboBox_SelectString(m_hControl[KB_KeyBoardStyle_Combox], -1, LoadResString(IDS_OnlyBuySell).c_str());
	else
		ComboBox_SelectString(m_hControl[KB_KeyBoardStyle_Combox], -1, LoadResString(IDS_ConfirmPayAuto).c_str());

	//横向字体
	if(0==m_KeyBoardCfg.uiKeyBoardFont)
		ComboBox_SelectString(m_hControl[KB_KeyBoardFont_Combox], -1, LoadResString(IDS_FontSmall).c_str());
	else if (1 == m_KeyBoardCfg.uiKeyBoardFont)
		ComboBox_SelectString(m_hControl[KB_KeyBoardFont_Combox], -1, LoadResString(IDS_FontMid).c_str());
	else
		ComboBox_SelectString(m_hControl[KB_KeyBoardFont_Combox], -1, LoadResString(IDS_FontBig).c_str());
	
	ComboBox_SetCurSel(m_hControl[KB_DefaultPrice_Combox], m_KeyBoardCfg.OutPriceType);
	swprintf_s(cc, TEXT("%d"), m_KeyBoardCfg.OutPriceTick);
	Edit_SetText(m_hControl[KB_DefaultPrice_Edit], cc);

	//显示 定单类型有效类型
	m_CheckShowOrderValidType.SetCheck(m_KeyBoardCfg.bShowOrderValidType ? BST_CHECKED : BST_UNCHECKED);
	//Enter发单
	m_CheckEnterSendOrder.SetCheck(m_KeyBoardCfg.bEnterSendOrder ? BST_CHECKED : BST_UNCHECKED);
	//追单助手
	m_CheckStartOrderAssistant.SetCheck(m_KeyBoardCfg.bStartOrderAssistant ? BST_CHECKED : BST_UNCHECKED);
	//追单风格
	if (0 == m_KeyBoardCfg.uiAssistType)
		ComboBox_SelectString(m_hControl[KB_AssistType_Combox], -1, LoadResString(IDS_Chase_Open_Close).c_str());
	else if (1 == m_KeyBoardCfg.uiAssistType)
		ComboBox_SelectString(m_hControl[KB_AssistType_Combox], -1, LoadResString(IDS_Chase_Open).c_str());
	else
		ComboBox_SelectString(m_hControl[KB_AssistType_Combox], -1, LoadResString(IDS_Chase_Close).c_str());

	Edit_SetText(m_hControl[KB_Buy_Edit], FormatHotKey(m_KeyBoardCfg.ucBuy).c_str());
	Edit_SetText(m_hControl[KB_Sell_Edit], FormatHotKey(m_KeyBoardCfg.ucSell).c_str());
	Edit_SetText(m_hControl[KB_Open_Edit], FormatHotKey(m_KeyBoardCfg.ucOpen).c_str());
	Edit_SetText(m_hControl[KB_Cover_Edit], FormatHotKey(m_KeyBoardCfg.ucCover).c_str());
	Edit_SetText(m_hControl[KB_CoverT_Edit], FormatHotKey(m_KeyBoardCfg.ucCoverT).c_str());
	/*
	合约快捷键2017-5-25
	*/
	uint ncount = 0;
	wstring value;
	wstring key;
	while (read_paramHK(ncount, key, value) != 0)
	{
		m_stdmapContract.insert(make_pair(key, value));
		ncount++;
	}
	UpdateHKList();
	//--
	return true;
}
	
bool KeyBoardSet::SaveParam()
{
	wchar_t cc[101] = { 0 };
	int nValue = 0;

	m_KeyBoardCfg.bSetDefaultPos = m_CheckFocusPos .GetCheck()== BST_CHECKED ? true : false;
	Edit_GetText(m_hControl[KB_FocusPos_Edit], cc, sizeof(cc) / sizeof(cc[0]));
	nValue = _wtoi(cc);
	m_KeyBoardCfg.uiFocusPos = nValue < 0 ? 0 : nValue;

	
	Edit_GetText(m_hControl[KB_DefaultPrice_Edit], cc, sizeof(cc) / sizeof(cc[0]));
	nValue = _wtoi(cc);
	m_KeyBoardCfg.OutPriceTick = nValue;
	m_KeyBoardCfg.OutPriceType = ComboBox_GetCurSel(m_hControl[KB_DefaultPrice_Combox]);
	if (m_KeyBoardCfg.OutPriceType < 0)
		m_KeyBoardCfg.OutPriceType = 2;


	m_KeyBoardCfg.bUpDownEqualTab = m_CheckUpDownEqualTab.GetCheck() == BST_CHECKED ? true : false;
	m_KeyBoardCfg.bRightLeftEqualTab = m_CheckRightLeftEqualTab.GetCheck() == BST_CHECKED ? true : false;


	ComboBox_GetLBText(m_hControl[KB_KeyBoardStyle_Combox], ComboBox_GetCurSel(m_hControl[KB_KeyBoardStyle_Combox]), cc);
	if (cc == LoadResString(IDS_OnlyOkButton))
		m_KeyBoardCfg.uiDoOrderStyle = 0;
	else if (cc == LoadResString(IDS_OnlyBuySell))
		m_KeyBoardCfg.uiDoOrderStyle = 1;
	else
		m_KeyBoardCfg.uiDoOrderStyle = 2;

	m_KeyBoardCfg.bShowOrderValidType = m_CheckShowOrderValidType.GetCheck() == BST_CHECKED ? true : false;

	m_KeyBoardCfg.bEnterSendOrder = m_CheckEnterSendOrder.GetCheck() == BST_CHECKED ? true : false;

	m_KeyBoardCfg.bStartOrderAssistant = m_CheckStartOrderAssistant.GetCheck() == BST_CHECKED ? true : false;

	char cKey[5] = { 0 };
	GetWindowTextA(m_hControl[KB_Buy_Edit], cKey, 5);
	m_KeyBoardCfg.ucBuy = StrToVk(cKey);
	GetWindowTextA(m_hControl[KB_Sell_Edit], cKey, 5);
	m_KeyBoardCfg.ucSell = StrToVk(cKey);
	GetWindowTextA(m_hControl[KB_Open_Edit], cKey, 5);
	m_KeyBoardCfg.ucOpen = StrToVk(cKey);
	GetWindowTextA(m_hControl[KB_Cover_Edit], cKey, 5);
	m_KeyBoardCfg.ucCover = StrToVk(cKey);
	GetWindowTextA(m_hControl[KB_CoverT_Edit], cKey, 5);
	m_KeyBoardCfg.ucCoverT = StrToVk(cKey);

	ComboBox_GetLBText(m_hControl[KB_KeyBoardFont_Combox], ComboBox_GetCurSel(m_hControl[KB_KeyBoardFont_Combox]), cc);
	if (cc == LoadResString(IDS_FontSmall))
		m_KeyBoardCfg.uiKeyBoardFont = 0;
	else if (cc == LoadResString(IDS_FontMid))
		m_KeyBoardCfg.uiKeyBoardFont = 1;
	else
		m_KeyBoardCfg.uiKeyBoardFont = 2;
	
	ComboBox_GetLBText(m_hControl[KB_AssistType_Combox], ComboBox_GetCurSel(m_hControl[KB_AssistType_Combox]), cc);
	if (!strcmp(LoadRC::unicode_to_ansi(cc).c_str() , LoadRC::unicode_to_ansi(LoadResString(IDS_Chase_Open_Close)).c_str()))
		m_KeyBoardCfg.uiAssistType = 0;
	else if (!strcmp(LoadRC::unicode_to_ansi(cc).c_str(), LoadRC::unicode_to_ansi(LoadResString(IDS_Chase_Open)).c_str()))
		m_KeyBoardCfg.uiAssistType = 1;
	else
		m_KeyBoardCfg.uiAssistType = 2;

	write_param("bSetDefaultPos"		, m_KeyBoardCfg.bSetDefaultPos		? 1 : 0);
	write_param("bShowOrderValidType"	, m_KeyBoardCfg.bShowOrderValidType ? 1 : 0);
	write_param("bEnterSendOrder"		, m_KeyBoardCfg.bEnterSendOrder		? 1 : 0);
	write_param("bUpDownEqualTab"		, m_KeyBoardCfg.bUpDownEqualTab		? 1 : 0); 
	write_param("bRightLeftEqualTab"	, m_KeyBoardCfg.bRightLeftEqualTab	? 1 : 0);
	write_param("uiFocusPos"			, m_KeyBoardCfg.uiFocusPos			);
	write_param("uiDoOrderStyle"		, m_KeyBoardCfg.uiDoOrderStyle		);
	write_param("uiKeyBoardFont"		, m_KeyBoardCfg.uiKeyBoardFont		);
	write_param("ucBuy"					, m_KeyBoardCfg.ucBuy				);
	write_param("ucSell"				, m_KeyBoardCfg.ucSell				);
	write_param("ucOpen"				, m_KeyBoardCfg.ucOpen				);
	write_param("ucCover"				, m_KeyBoardCfg.ucCover				);
	write_param("ucCoverT"				, m_KeyBoardCfg.ucCoverT			);
	write_param("OutPriceType"			, m_KeyBoardCfg.OutPriceType		);
	write_param("OutPriceTick"			, m_KeyBoardCfg.OutPriceTick		);
	write_param("bStartOrderAssistant"	, m_KeyBoardCfg.bStartOrderAssistant);
	write_param("uiAssistType"			, m_KeyBoardCfg.uiAssistType		);
	/*
	合约快捷键2017-5-25
	*/
	m_KeyBoardCfg.mapContractHotkey = m_stdmapContract;
	uint nfieldcount = 0;
	for (std::map<std::wstring, std::wstring>::iterator iter = m_stdmapContract.begin(); iter != m_stdmapContract.end(); iter++,nfieldcount++)
	{
		wchar_t temp[31];
		wsprintf(temp,L"%s|%s", iter->first.c_str(), iter->second.c_str());
		write_paramHK(nfieldcount, temp);
	}
	//--

	for (set<IKeyOrderCfg*>::iterator it = m_vNotify.begin(); it != m_vNotify.end(); it++)
		(*it)->OnKeyBoardCfgChange(&m_KeyBoardCfg);
	return true;
}

void KeyBoardSet::OnMeasureListView(LPARAM lParam)
{
	LPMEASUREITEMSTRUCT pmi = LPMEASUREITEMSTRUCT(lParam);
	if (pmi->CtlType == ODT_LISTVIEW && pmi->CtlID == KB_HK_List)
		m_ListCHotKey.OnMeasureItem(pmi->itemHeight);
}
void KeyBoardSet::OnDrawListView(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = LPDRAWITEMSTRUCT(lParam);
	if (pDIS->CtlType == ODT_LISTVIEW && pDIS->CtlID == KB_HK_List)
		m_ListCHotKey.OnDrawItem(lParam);
}
//设置字体
void KeyBoardSet::SetFontDefault(int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCTSTR lpszFace, LOGFONT &lf)
{
	lf.lfHeight 			= nHeight;
	lf.lfWidth 				= nWidth;
	lf.lfEscapement 		= nEscapement;
	lf.lfOrientation 		= nOrientation;
	lf.lfWeight 			= fnWeight;
	lf.lfItalic 			= (BYTE)fdwItalic;
	lf.lfUnderline 			= (BYTE)fdwUnderline;
	lf.lfStrikeOut 			= (BYTE)fdwStrikeOut;
	lf.lfCharSet 			= (BYTE)fdwCharSet;
	lf.lfOutPrecision 		= (BYTE)fdwOutputPrecision;
	lf.lfClipPrecision 		= (BYTE)fdwClipPrecision;
	lf.lfQuality 			= (BYTE)fdwQuality;
	lf.lfPitchAndFamily		= (BYTE)fdwPitchAndFamily;
	wcscpy_s(lf.lfFaceName	, lpszFace);
}
void KeyBoardSet::ChangeDefaultPrice()
{
	wchar_t txt[16];
	GetWindowTextW(m_hControl[KB_DefaultPrice_Combox], txt, sizeof(txt)/sizeof(txt[0]));
	if (LoadResString(IDS_STRING_DPNoPrice) == txt)
	{
		SetWindowTextW(m_hControl[KB_DefaultPrice_Edit], L"");
		::EnableWindow(m_hControl[KB_DefaultPrice_Static2], FALSE);
		::EnableWindow(m_hControl[KB_DefaultPrice_Edit], FALSE);
		::EnableWindow(m_hControl[KB_DefaultPrice_Spin], FALSE);
		::EnableWindow(m_hControl[KB_DefaultPrice_Static3], FALSE);
	}
	else
	{
		::EnableWindow(m_hControl[KB_DefaultPrice_Static2], TRUE);
		::EnableWindow(m_hControl[KB_DefaultPrice_Edit], TRUE);
		::EnableWindow(m_hControl[KB_DefaultPrice_Spin], TRUE);
		::EnableWindow(m_hControl[KB_DefaultPrice_Static3], TRUE);
	}

}
void KeyBoardSet::ChangeFacadeFont(C_KeyBoardFontInfoCfg &cfg, bool bfromui, bool bNotify)
{
	if (bfromui)
	{
		wchar_t cc[101] = { 0 };
		ComboBox_GetLBText(m_hControl[KB_KeyBoardFont_Combox], ComboBox_GetCurSel(m_hControl[KB_KeyBoardFont_Combox]), cc);
		if (cc == LoadResString(IDS_FontSmall))
			m_KeyBoardCfg.uiKeyBoardFont = 0;
		else if (cc == LoadResString(IDS_FontMid))
			m_KeyBoardCfg.uiKeyBoardFont = 1;
		else
			m_KeyBoardCfg.uiKeyBoardFont = 2;
	}
	if (0 == m_KeyBoardCfg.uiKeyBoardFont)
	{
		SetFontDefault(-12, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, TEXT("Arial Black"	), cfg.lgStaticFont1);
		SetFontDefault(-15, 0, 0, 0, FW_BOLD	, 0, 0, 0, DEFAULT_CHARSET	, 0				   , 0				   , 0			  , 0			 , TEXT("Arial"			), cfg.lgComboFont);
		SetFontDefault(-11, 0, 0, 0, FW_HEAVY	, 0, 0, 0, ANSI_CHARSET		, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, TEXT("Arial"			), cfg.lgStaticFont2);
		SetFontDefault(-11, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, TEXT("Arial"			), cfg.lgBtnFont);
		SetFontDefault(-11, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, TEXT("Arial"			), cfg.lgCheckFont);
		SetFontDefault(-16, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, TEXT("Arial"			), cfg.lgPositionFont);
		cfg.iLeftMargin			= 5;
		cfg.iTopMargin			= 5;
		cfg.iHorCtrlInterval	= 12;
		cfg.iVerCtrlInterval1	= 0;
		cfg.iVerCtrlInterval2	= 1;
	}
	else if (1 == m_KeyBoardCfg.uiKeyBoardFont)
	{
		SetFontDefault(-15, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"	), cfg.lgStaticFont1);
		SetFontDefault(-21, 0, 0, 0, FW_BOLD	, 0, 0, 0, GB2312_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Arial"		), cfg.lgComboFont);
		SetFontDefault(-14, 0, 0, 0, FW_HEAVY	, 0, 0, 0, ANSI_CHARSET		, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, 34			  , TEXT("Arial"		), cfg.lgStaticFont2);
		SetFontDefault(-19, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Arial"		), cfg.lgBtnFont);
		SetFontDefault(-15, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, LoadResString(IDS_STRING_SongTiFont).c_str(), cfg.lgCheckFont);
		SetFontDefault(-21, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, LoadResString(IDS_STRING_SongTiFont).c_str(), cfg.lgPositionFont);
		cfg.iLeftMargin			= 5;
		cfg.iTopMargin			= 5;
		cfg.iHorCtrlInterval	= 5;
		cfg.iVerCtrlInterval1	= 0;
		cfg.iVerCtrlInterval2	= 5;

	}
	else if (2 == m_KeyBoardCfg.uiKeyBoardFont)
	{
		SetFontDefault(-19, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Arial Black"	), cfg.lgStaticFont1);
		SetFontDefault(-24, 0, 0, 0, FW_BOLD	, 0, 0, 0, GB2312_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Arial"		), cfg.lgComboFont);
		SetFontDefault(-18, 0, 0, 0, FW_HEAVY	, 0, 0, 0, ANSI_CHARSET		, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Arial"		), cfg.lgStaticFont2);
		SetFontDefault(-21, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, TEXT("Arial"		), cfg.lgBtnFont);
		SetFontDefault(-18, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, LoadResString(IDS_STRING_SongTiFont).c_str(), cfg.lgCheckFont);
		SetFontDefault(-24, 0, 0, 0, FW_REGULAR	, 0, 0, 0, DEFAULT_CHARSET	, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH, LoadResString(IDS_STRING_SongTiFont).c_str(), cfg.lgPositionFont);
		cfg.iLeftMargin			= 5;
		cfg.iTopMargin			= 5;
		cfg.iHorCtrlInterval	= 10;
		cfg.iVerCtrlInterval1	= 0;
		cfg.iVerCtrlInterval2	= 5;
	}
	cfg.rgbBkPosition = g_ColorRefData.GetColorBlack();
	cfg.rgbBuy = g_ColorRefData.GetColorTextRed();
	cfg.rgbSell = g_ColorRefData.GetColorTextGreen();

	if (bNotify)
		for (set<IKeyOrderCfg*>::iterator it = m_vNotify.begin(); it != m_vNotify.end(); it++)
			(*it)->OnKeyBoardCfgFontChange(&cfg);
}
void KeyBoardSet::UpdateHKList()
{
	if (!IsWindow(m_ListCHotKey.GetListHwnd()))return;
	m_ListCHotKey.DeleteAllItems();
	std::map<std::wstring, std::wstring>::iterator iter;
	int iRow = 0;
	int iCol = 0;
	for (iter = m_stdmapContract.begin(); iter != m_stdmapContract.end(); iter++)
	{
		iCol = 0;
		m_ListCHotKey.AddItem(iter->first.c_str(), iRow, iCol++);
		m_ListCHotKey.AddItem(iter->second.c_str(), iRow, iCol++);
		iRow++;
	}
}
void KeyBoardSet::SetContractNo(string sCode, bool brepaint, bool bnutton, bool bgetfreeze)
{
	string code = sCode;
	//if (0 == VersionType())//如果是内盘
	//{
		TContractKey contractKey;
		memset(&contractKey, 0, sizeof(TContractKey));
		CConvert::TContractNoToTContractKey(code.c_str(), contractKey);
		if (ctFutures == contractKey.CommodityType)
		{
			char cc[101] = { 0 };
			sprintf_s(cc, "%s%s", contractKey.CommodityNo, contractKey.ContractNo);
			code = cc;
		}
	//}
	SetWindowText(m_EditCName.GetHwnd(), LoadRC::ansi_to_unicode(code).c_str());
}
bool KeyBoardSet::CheckValidKey(uint ID)
{
	switch (ID)
	{
	case KB_Buy_Edit:
	case KB_Sell_Edit:
	{
		map<wstring, int> VkeyMap;
		VkeyMap.insert(pair<wstring, int>(LoadResString(IDS_Direct_Buy), FormatStrKey(m_hControl[KB_Buy_Edit])));//撤买入快捷键
		VkeyMap.insert(pair<wstring, int>(LoadResString(IDS_Direct_Sell), FormatStrKey(m_hControl[KB_Sell_Edit])));//撤买入快捷键

		int nCur = FormatStrKey(m_hControl[ID]);
		if (nCur == 0)//空
			return true;
		wstring sInfo = L"";
		int ncount = 0;
		for (map<wstring, int>::iterator iter = VkeyMap.begin(); iter != VkeyMap.end(); iter++)
		{
			if (nCur == iter->second)
			{
				ncount++;
				sInfo = sInfo + iter->first.c_str() + L" ";
			}
		}
		if (ncount >= 2)
		{
			sInfo += LoadResString(IDS_SameHotKey);
			MessageBoxW(m_Hwnd, sInfo.c_str(), LoadResString(IDS_STRING_Error).c_str(), NULL);
			return false;
		}
		return true;
	}
	case KB_Open_Edit:
	case KB_Cover_Edit:
	case KB_CoverT_Edit:
	{
		map<wstring, int> VkeyMap;
		VkeyMap.insert(pair<wstring, int>(LoadResString(IDS_Offset_Open), FormatStrKey(m_hControl[KB_Open_Edit])));//撤买入快捷键
		VkeyMap.insert(pair<wstring, int>(LoadResString(IDS_Offset_Cover), FormatStrKey(m_hControl[KB_Cover_Edit])));//撤买入快捷键
		VkeyMap.insert(pair<wstring, int>(LoadResString(IDS_Offset_CoverT), FormatStrKey(m_hControl[KB_CoverT_Edit])));//撤买入快捷键

		int nCur = FormatStrKey(m_hControl[ID]);
		if (nCur == 0)//空
			return true;
		wstring sInfo = L"";
		int ncount = 0;
		for (map<wstring, int>::iterator iter = VkeyMap.begin(); iter != VkeyMap.end(); iter++)
		{
			if (nCur == iter->second)
			{
				ncount++;
				sInfo = sInfo + iter->first.c_str() + L" ";
			}
		}
		if (ncount >= 2)
		{
			sInfo += LoadResString(IDS_SameHotKey);
			MessageBoxW(m_Hwnd, sInfo.c_str(), LoadResString(IDS_STRING_Error).c_str(), NULL);
			return false;
		}
		return true;
	}
	default:
		return true;
	}
}