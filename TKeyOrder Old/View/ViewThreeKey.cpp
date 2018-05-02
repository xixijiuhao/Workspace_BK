#include "PreInclude.h"
#define  path "..\\Debug\\config\\ThreeKeyOrderConfig.pri"

extern ITradeData * g_pTradeData;
extern TPluginMgrApi	* g_pmanager;


#define minWidth   250
#define minHeight  250


#define captionHeight 30
#define cbTop        4
#define cbWidth    85
#define textLeft   5
#define textTop	   2
#define textHeight (captionHeight -textTop-textTop)
#define textRight (textLeft + cbWidth)

#define arrowWdith  8
#define arrowLeft   (textRight + arrowWdith + 1)
#define arrowHeight 6
#define arrowBottom 19
#define arrowRight (arrowLeft + arrowWdith)
#define labLeft    (arrowRight + 10)
#define labRightOffset 12

#define clickRegionWidth ( arrowRight)

#define  closeWidth        11

TCHAR szTreeTextChs[][20] = { L"下单板", L"通用下单", L"参数设置", L"选项设置", L"默认手数", L"超价参数", L"追价参数",L"止损参数",L"权益：",L"可用：",L"资金使用率：" };
TCHAR szTreeTextCht[][20] = { L"下單板", L"通用下單", L"參數設置", L"選項設置", L"默認手數", L"超價參數", L"追價參數", L"止損參數", L"權益：", L"可用：", L"資金使用率：" };
TCHAR szTreeTextEnu[][20] = { L"Order", L"Usual Order", L"Settings",L"Options",L"Default Qty",L"Super Price",L"Chase Order",L"Stop Loss",
L"Equity: ",L"Avaliable: ",L"Rate: "};

enum{
	OrderBoard_Index,
	TKeyOrder_Index,
	Settings_Index,
	Options_Index,
	DefaultQty_Index,
	SuperPrice_Index,
	ChaseOrder_Index,
	StopLoss_Index,
	Equite_Index,
	Available_Index,
	Rate_Index,
};

TCHAR * _GetTextW(int i)
{
	if (g_language == ENU)return szTreeTextEnu[i];
	else if (g_language == CHT)return szTreeTextCht[i];
	return szTreeTextChs[i];
}

TViewThreeKey::TViewThreeKey(IMediator* mediator) :IColleague(mediator)
{
	caption_focus_color = RGB(110, 110, 110);//	caption_focus_color = RGB(213, 216, 230);

	m_bMouseTrack = false;


	m_iOffset = 138;
	m_iSelTree = 0;

	m_iFrmTop = 38;

	m_hbrCaption = CreateSolidBrush(caption_focus_color);
	m_PosRect = { 0 };

	m_rcUserNo = { textLeft, textTop, textLeft + cbWidth, textTop + textHeight };

	m_pVecComboxCont = nullptr; 

	m_iCbSelIndex = -1;

	m_bUserChange = false;

	m_rcClose.top = 8;
	m_rcClose.bottom = m_rcClose.top + closeWidth;
	m_bHoverClose = false;
}

void TViewThreeKey::ShowFrm()
{
	HWND hwnd = FindWindow(MainFrmName, NULL);
	CreateFrm(L"ThreeKeyOrder", hwnd, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	m_PosRect = { (GetSystemMetrics(SM_CXSCREEN) - frame_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - frame_height) / 2 + 100,
		(GetSystemMetrics(SM_CXSCREEN) - frame_width) / 2 + frame_width, (GetSystemMetrics(SM_CYSCREEN) - frame_height) / 2 + 100 + frame_height };
	SetWindowPos(m_Hwnd, HWND_TOP, m_PosRect.left, m_PosRect.top, m_PosRect.right - m_PosRect.left, m_PosRect.bottom - m_PosRect.top, SWP_SHOWWINDOW);

	if (g_pMainFrm)
	{
		g_pMainFrm->AddLinkage(m_Hwnd, 0, TPresenter::OnReceiveNotice);
	}


}

void TViewThreeKey::Send(unsigned int CommandID, const char* content)
{
	m_Mediator->Send(CommandID, content, this);
}

TViewThreeKey::~TViewThreeKey()
{
	if (m_hbrCaption)
		DeleteObject(m_hbrCaption);
}
void TViewThreeKey::OnCreate()
{
	m_treeSetting.CreateTree(m_Hwnd, 10, 38, 112, 338);
	m_treeSetting.AddNode(_GetTextW(OrderBoard_Index), 0, 0);
	m_treeSetting.AddNode(_GetTextW(TKeyOrder_Index), 1, 0);
	m_treeSetting.AddNode(_GetTextW(Settings_Index), 0, 0);
	m_treeSetting.AddNode(_GetTextW(Options_Index), 1, 1);
	m_treeSetting.AddNode(_GetTextW(DefaultQty_Index), 1, 1);
	m_treeSetting.AddNode(_GetTextW(SuperPrice_Index), 1, 1);
	m_treeSetting.AddNode(_GetTextW(ChaseOrder_Index), 1, 1);
	m_treeSetting.AddNode(_GetTextW(StopLoss_Index), 1, 1);

	m_treeSetting.Expand(0);
	m_treeSetting.Expand(1);

	m_sFundInfo.Create(m_Hwnd, L"");
	m_sFundInfo.MoveWindow(labLeft, textTop, frame_width - labLeft -labRightOffset, textHeight);
	m_sFundInfo.SetFont(g_FontData.g_FontWord13);

    ShowWindow(m_cblistUserNo.GetHwnd(), SW_HIDE);

    //m_cblistUserNo.SetFont(g_FontData.g_FontNum13);


	m_Mediator->Get(CMD_TView_GetComboxCont, (void*&)m_pVecComboxCont, this);
	m_Mediator->Get(CMD_TView_GetCurrencyNo, (void*&)m_pMapCurrencyNo, this);
}

void TViewThreeKey::OnUserLogin(int index)
{
	if (!m_pVecComboxCont)return;

    std::vector<ComboxStruct> stldata;

	for (UINT i = 0; i < m_pVecComboxCont->size(); i++)
	{
        ComboxStruct stcombox;
        stcombox.nindex = i;
        stcombox.sztext = m_pVecComboxCont->at(i);
        stldata.push_back(stcombox);
        m_cblistUserNo.SetData(stldata);

		m_bUserChange = true;
	}

	InvalidateRect(m_Hwnd, &m_rcUserNo, false);
}
void TViewThreeKey::UpdateMoney(const TMoney* pMoney)
{
	if (!pMoney) return;

	//auto iter = m_pMapCurrencyNo->find(pMoney->UserNo);

	//if (iter->second.size()==1)

	if (strcmp(pMoney->UserNo, g_pGlobalModel->m_strUserNo.c_str()) != 0) return;
	TCHAR szText[200];

	swprintf_s(szText, L"%s%.0f, %s%.0f, %s%.2f%%", _GetTextW(Equite_Index), pMoney->Equity, _GetTextW(Available_Index), pMoney->Available,
		_GetTextW(Rate_Index), (pMoney->Equity - pMoney->Available) * 100 / pMoney->Equity
		);
	m_sFundInfo.SetText(szText);
}
LRESULT TViewThreeKey::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
    case SSWM_COMBOX_SELECT:
        OnComboxSelect(wParam, lParam);
        return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_NCDESTROY:
		OnNcDestroy();
		return PROCESSED;
	case WM_DESTROY:
		OnDestroy();
		return PROCESSED;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_SIZING:
		OnSizing(wParam, lParam);
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_NCHITTEST: //鼠标的所有消息都有这个产生
		return OnNcHitTest(wParam, lParam); //OnNcHiTest处理鼠标所在区域，返回相应消息
	case WM_SETCURSOR:
		OnSetCursor(wParam, lParam); //设置鼠标形态，根据鼠标所在边框
		return PROCESSED;
	case WM_NCLBUTTONDOWN:          //发送消息，实现窗口的缩放，消息由DefWindowProc处理
		OnNcLButtonDown(wParam, lParam);
		return NOT_PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
void TViewThreeKey::OnSizing(WPARAM wParam, LPARAM lParam)
{
	LPRECT pRect = (LPRECT)lParam;
	if (pRect->bottom - pRect->top < minHeight)
	{
		if (wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT)
			pRect->bottom = pRect->top + minHeight;
		else
			pRect->top = pRect->bottom - minHeight;
	}
	if (pRect->right - pRect->left < minWidth)
	{
		if (wParam == WMSZ_RIGHT || wParam == WMSZ_TOPRIGHT || wParam == WMSZ_BOTTOMRIGHT)
			pRect->right = pRect->left + minWidth;
		else
			pRect->left = pRect->right - minWidth;
	}
}
LRESULT TViewThreeKey::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == m_sFundInfo.GetHwnd())
	{
		SetTextColor((HDC)wParam, RGB(240, 240, 240));
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)m_hbrCaption;
	}
	return NOT_PROCESSED;
}

//deal title length
void TViewThreeKey::OnSize(WPARAM wParam, LPARAM lParam)
{
	int x = LOWORD(lParam) - 12;
	int y = HIWORD(lParam) - 48;
	m_treeSetting.MoveWindow(10, 38, 112, y);
	m_sFundInfo.MoveWindow(labLeft, textTop, x -labLeft-8-closeWidth,  textHeight);

	RECT rect = GetShowRect();
	m_Mediator->Send(CMD_TView_SizeChange, &rect, this);

	m_rcClose.right = LOWORD(lParam) - 8;
	m_rcClose.left = m_rcClose.right - closeWidth;
}
void TViewThreeKey::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	rect.bottom = caption_height;
	
	RECT rcClick = m_rcUserNo;
	rcClick.right = arrowRight;

	RECT rc = m_rcClose;
	InflateRect(&rc, 2, 2);
	if (PtInRect(&rcClick, pt))
	{
        POINT pt;
        pt.x = 0;
        pt.y = captionHeight;
        ::ClientToScreen(m_Hwnd, &pt);
		if (!m_cblistUserNo.GetHwnd())
		  m_cblistUserNo.Create(m_Hwnd);
		m_cblistUserNo.MoveWindow(pt.x, pt.y, clickRegionWidth, 20 * 5);
        m_cblistUserNo.ShowWindow(SW_SHOW);   
	}
	else if (PtInRect(&rc, pt))
	{
		//
		ShowWindow(m_Hwnd, SW_HIDE);
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}


void TViewThreeKey::SetToOriginalPos()
{
	RECT rect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	MoveWindow(m_Hwnd, rect.left, rect.bottom - frame_height, frame_width, frame_height, true);
}

void TViewThreeKey::ShowHide(int iShow)
{
	if (iShow == 0)
	{
		if (IsWindowVisible(m_Hwnd))
		{
			ShowWindow(m_Hwnd, SW_HIDE);
		}
		else
		{
			ShowWindow(m_Hwnd, SW_SHOW);
		}
	}
	else if (iShow == 1)
		ShowWindow(m_Hwnd, SW_SHOW);
	else if (iShow == 2)
		ShowWindow(m_Hwnd, SW_HIDE);

}
void TViewThreeKey::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	RECT rc = rect;
	rc.bottom = caption_height;
	FillRect(memdc.GetHdc(), &rc, m_hbrCaption);   //标题栏颜色
	rect.top = rc.bottom;
	FillRect(memdc.GetHdc(), &rect, g_ColorRefData.g_brush_bk);
	rc.bottom = rect.bottom;
	FrameRect(memdc.GetHdc(), &rc, m_hbrCaption);
	InflateRect(&rc, -1, -1);
	FrameRect(memdc.GetHdc(), &rc, m_hbrCaption);

	SetTextColor(memdc.GetHdc(), g_ColorRefData.g_ColorWhite);
	SelectObject(memdc.GetHdc(), g_FontData.g_FontWord13);
	DrawText(memdc.GetHdc(), ANSIToUnicode(g_pGlobalModel->m_strUserNo).c_str(), -1, &m_rcUserNo, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	DraeLeftArrow(memdc.GetHdc(), arrowLeft, arrowBottom);

	DrawClose(memdc.GetHdc());
}

void TViewThreeKey::OnNcDestroy()
{
	
	delete this;
}

void TViewThreeKey::OnDestroy()
{
}

void TViewThreeKey::UpdateMoney()
{
	TDataContainer stMoney;
	g_pTradeData->GetAllMoney(stMoney);
	for (UINT i = 0; i < stMoney.size(); i++)
	{
		const TMoney* pMoney = (const TMoney*)stMoney.at(i);
		if (strcmp(g_pGlobalModel->m_strUserNo.c_str(), pMoney->UserNo) == 0)
		{
			UpdateMoney(pMoney);
			return;
		}
	}
}
void TViewThreeKey::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == m_sFundInfo.GetHwnd() && HIWORD(wParam) == STN_CLICKED)
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}

}
void TViewThreeKey::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
	if (pnmtv->hdr.hwndFrom == m_treeSetting.GetHwnd() && pnmtv->hdr.code == TVN_SELCHANGED)
	{
		TVITEMW item;
		item.hItem = pnmtv->itemNew.hItem;
		item.mask = TVIF_TEXT;
		wchar_t a[101] = { 0 };
		item.pszText = a;
		item.cchTextMax = sizeof(a) / sizeof(a[0]);
		TreeView_GetItem(m_treeSetting.GetHwnd(), &item);
		ShowHideTree(item.pszText);
	}

}
void TViewThreeKey::ShowHideTree(LPWSTR lpsztext)
{

	if (!lstrcmp(lpsztext, _GetTextW(OrderBoard_Index)) || !lstrcmp(lpsztext, _GetTextW(Settings_Index)))
		return;
	//隐藏之前界面
	int iSelTreeFormer = m_iSelTree;
	//显示当前界面
	if (!lstrcmp(lpsztext, _GetTextW(TKeyOrder_Index)))
	{
		m_iSelTree = ID_frmTKeyOrder;
	}
	else if (!lstrcmp(lpsztext, _GetTextW(Options_Index)))
	{
		m_iSelTree = ID_frmOptionConfig;
	}
	else if (!lstrcmp(lpsztext, _GetTextW(DefaultQty_Index)))
	{
		m_iSelTree = ID_frmDefaultQty;
	}
	else if (!lstrcmp(lpsztext, _GetTextW(SuperPrice_Index)))
	{
		m_iSelTree = ID_frmSuperPrice;
	}
	else if (!lstrcmp(lpsztext, _GetTextW(ChaseOrder_Index)))
	{
		m_iSelTree = ID_frmChasePrice;
	}
	else if (!lstrcmp(lpsztext, _GetTextW(StopLoss_Index)))
	{
		m_iSelTree = ID_frmStopLoss;
	}

	if (iSelTreeFormer != m_iSelTree)
	{
		WPARAM data = (m_iSelTree & 0xFFFF) | ((iSelTreeFormer & 0xFFFF) << 16);  //LOWORD nowSel, HIWORD formerSel
		m_Mediator->Send(CMD_TView_SwitchFrm, (const void*)data , this);
	}
}
void TViewThreeKey::OnComboxSelect(WPARAM wParam, LPARAM lParam)
{
    HWND hwnd = (HWND)wParam;
    int nselect = (int)lParam;
    if (hwnd == m_cblistUserNo.GetHwnd())
    {
		if (nselect == m_iCbSelIndex && !m_bUserChange)
			return;
		m_bUserChange = false;

		m_iCbSelIndex = nselect;
        m_Mediator->Send(CMD_TView_UserChange, (void*)nselect, this);
		RECT rc = { 0, 0, textRight, captionHeight };
        InvalidateRect(m_Hwnd, &rc, false);

    }
}
void TViewThreeKey::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_Hwnd;
		tme.dwHoverTime = 0;
		::TrackMouseEvent(&tme);
		m_bMouseTrack = true;
	}
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	RECT rc = m_rcClose;
	InflateRect(&rc, 2, 2);
	m_bHoverClose = false;
	if (PtInRect(&rc, pt))
	{
		m_bHoverClose = true;
	}

	InvalidateRect(m_Hwnd, &m_rcClose, false);
}
void TViewThreeKey::OnMouseLeave()
{
	m_bMouseTrack = false;
	m_bHoverClose = false;
	InvalidateRect(m_Hwnd, &m_rcClose, false);
}

RECT TViewThreeKey::GetShowRect()
{
	RECT rect = { 0 };
	GetClientRect(m_Hwnd, &rect);
	rect.left = m_iOffset;
	rect.top = m_iFrmTop;
	rect.right -= 12;
	rect.bottom -= 10;
	return rect;
}

UINT TViewThreeKey::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = GET_X_LPARAM(lParam);
	point.y = GET_Y_LPARAM(lParam);
	RECT rect;
	GetWindowRect(m_Hwnd, &rect);
	if (point.x <= rect.left + 10 && point.y <= rect.top + 10)
	{
		return HTTOPLEFT;
	}
	else if (point.x >= rect.right - 10 && point.y <= rect.top + 10)
	{
		return HTTOPRIGHT;
	}
	else if (point.x <= rect.left + 10 && point.y >= rect.bottom - 10)
	{
		return HTBOTTOMLEFT;
	}
	else if (point.x >= rect.right - 10 && point.y >= rect.bottom - 10)
	{
		return HTBOTTOMRIGHT;
	}
	else if (point.x <= rect.left + 5)
	{
		return HTLEFT;
	}
	else if (point.x >= rect.right - 5)
	{
		return HTRIGHT;
	}
	else if (point.y <= rect.top + 5)
	{
		return HTTOP;
	}
	else if (point.y >= rect.bottom - 5)
	{
		return HTBOTTOM;
	}

	return NOT_PROCESSED;
}
void TViewThreeKey::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam != m_Hwnd)
		return;
	UINT nHitTest = LOWORD(lParam);
	switch (nHitTest)
	{
	case HTTOP:
	case HTBOTTOM:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		break;
	case HTLEFT:
	case HTRIGHT:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
		break;
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
		break;
	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
		break;
	case HTCAPTION:
	case HTSYSMENU:
	case HTMENU:
	case HTCLIENT:
	default:
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		break;
	}
}
void TViewThreeKey::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case HTTOP:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, lParam);
		break;
	case HTBOTTOM:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, lParam);
		break;
	case HTLEFT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, lParam);
		break;
	case HTRIGHT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, lParam);
		break;
	case HTTOPLEFT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, lParam);
		break;
	case HTTOPRIGHT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, lParam);
		break;
	case HTBOTTOMLEFT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, lParam);
		break;
	case HTBOTTOMRIGHT:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, lParam);
		break;
	default:
		break;
	}
}


void TViewThreeKey::DraeLeftArrow(HDC hdc, int left, int bottom)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, g_ColorRefData.g_ColorWhite);
	SelectObject(hdc, hPen);
	int top = bottom - arrowHeight;
	MoveToEx(hdc, left -arrowHeight, top, 0);
	LineTo(hdc, left,  bottom);
	LineTo(hdc, left + arrowHeight + 1, top - 1);
	DeleteObject(hPen);
}

void TViewThreeKey::DrawClose(HDC hdc)
{
	COLORREF colseColor = RGB(150, 150, 150);
	if (m_bHoverClose)
		colseColor = RGB(231, 231, 231);
	HPEN hPen = CreatePen(PS_SOLID, 1, colseColor);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

	//int offset = m_bCloseClick ? 1 : 0;
	POINT ptCloseBegin[2] = { 0 };
	POINT ptCloseEnd[2] = { 0 };
	ptCloseBegin[0] = { m_rcClose.left , m_rcClose.top  };
	ptCloseBegin[1] = { m_rcClose.left , m_rcClose.bottom  };


	ptCloseEnd[0] = { m_rcClose.right + 1 , m_rcClose.bottom + 1  };
	ptCloseEnd[1] = { m_rcClose.right + 1 , m_rcClose.top - 1  };
	for (int i = 0; i < 2; i++)
	{
		MoveToEx(hdc, ptCloseBegin[i].x, ptCloseBegin[i].y, 0);
		LineTo(hdc, ptCloseEnd[i].x, ptCloseEnd[i].y);
	}

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}