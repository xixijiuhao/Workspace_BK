#include "BaseInclude.h"

#define cCAPTION_H  30
#define cFRAME_V    400
#define cFRAME_H    360

#define cleft       15
#define cVap_x      10
#define cVap_y       5

#define  ccloseWidth 9

extern ShiftQuoteApi* g_pShiftQuoteApi;
extern TShiftPosition* g_pShiftPosition;
TAddContractView::TAddContractView()
{
	m_rcClose.right = cFRAME_V - 8;
	m_rcClose.left = m_rcClose.right - ccloseWidth;
	m_rcClose.top = 10;
	m_rcClose.bottom = m_rcClose.top + ccloseWidth;
	m_bMouseTrack = false;

	m_bFirstContract = false;
	m_bShowCodeList = true;
}
void TAddContractView::ShowFrm(HWND hParent)
{
	CreateFrm(L"TShiftPosition", hParent, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU, WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT);
	POINT pt;
	GetCursorPos(&pt);
	SetWindowPos(m_Hwnd, HWND_TOP, pt.x, pt.y, cFRAME_V, cFRAME_H, SWP_SHOWWINDOW);
}
void TAddContractView::Regist(char* sign, char* sign2)
{
	if (sign)
		memcpy(m_Sign, sign, sizeof(m_Sign));
	if (sign2)
		memcpy(m_Sign2, sign2, sizeof(m_Sign2));
}
void TAddContractView::InitWnd()
{
	m_editFirstContract.SetText(L"");
	m_editSecondContract.SetText(L"");
	m_cbCondition.SetSelect(0);
	m_cbFirstCondition.SetSelect(0);
	m_cbSecondCondition.SetSelect(0);

	m_spinEditFirstQty.SetWindowNumber(1);
	m_spinEditSecondQty.SetWindowNumber(1);
}
LRESULT TAddContractView::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return PROCESSED;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return PROCESSED;
	case SSWM_STATIC_BUTTON_CLICKDOWN:
		OnBtnClickDown(wParam, lParam);
		return PROCESSED;
	case WM_CREATE:
		OnCreate();
		return PROCESSED;
	case WM_PAINT:
		OnPaint();
		return PROCESSED;
	case WM_SIZE:
		OnSize(wParam, lParam);
		return PROCESSED;
	case WM_CTLCOLORSTATIC:
		return OnCtlColor(wParam, lParam);
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, lParam);
		return PROCESSED;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return PROCESSED;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam, lParam);
		return PROCESSED;
	case WM_NCLBUTTONDOWN:
		OnNcLButtonDown(wParam, lParam);
		return NOT_PROCESSED;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, lParam);
		return PROCESSED;
	case SSWM_EDIT_PRESS_ENTER:
		if ((HWND)lParam == m_editFirstContract.GetHwnd())
		{
			m_bFirstContract = true;
			m_popContractList.OnReturn();
		}
		else if ((HWND)lParam == m_editSecondContract.GetHwnd())
		{
			m_bFirstContract = false;
			m_popContractList.OnReturn();
		}		
		return PROCESSED;
	case WM_KEYDOWN:
		KeyDown(wParam, lParam);
		break;
	case SSWM_TAB_SWITCH_FOCUS:
		if (GetWindowLong(GetFocus(), GWL_ID) == ID_EDIT_FIRSTCONTRACT)
			SetFocus(m_editSecondContract.GetHwnd());
		else if (GetWindowLong(GetFocus(), GWL_ID) == ID_EDIT_SECONDCONTRACT)
			SetFocus(m_editFirstContract.GetHwnd());
		return PROCESSED;
	default:
		return NOT_PROCESSED;
	}
}
extern bool g_MouseTrack;
void TAddContractView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	if ((id == ID_EDIT_FIRSTCONTRACT || id== ID_EDIT_SECONDCONTRACT) && HIWORD(wParam) == EN_CHANGE)
	{
		if (m_bShowCodeList)
		{
			if (!IsWindow(m_popContractList.GetHwnd()))
			{
				DWORD dwStyle = WS_CLIPSIBLINGS /*| WS_CHILD*/ | WS_BORDER | WS_POPUP;
				DWORD dwStyleeEx = WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE;
				m_popContractList.CreatePopWin(this, L"popupcodeList", m_Hwnd, dwStyle, dwStyleeEx, 0, 0, 120);
				if (id == ID_EDIT_FIRSTCONTRACT)
					m_popContractList.UpdateData(m_Sign);
				else if (id == ID_EDIT_SECONDCONTRACT)
					m_popContractList.UpdateData(m_Sign2);
			}
			POINT pt = { 0, 0 };
			char sCode[100] = { 0 };
			if (id == ID_EDIT_FIRSTCONTRACT)
			{
				m_bFirstContract = true;
				ClientToScreen(m_editFirstContract.GetHwnd(), &pt);
				m_editFirstContract.GetText(sCode, sizeof(sCode));
			}
			else if (id == ID_EDIT_SECONDCONTRACT)
			{
				m_bFirstContract = false;
				ClientToScreen(m_editSecondContract.GetHwnd(), &pt);
				m_editSecondContract.GetText(sCode, sizeof(sCode));
			}
			pt.y += 20;
			if (!IsWindowVisible(m_popContractList.GetHwnd()))
				SetWindowPos(m_popContractList.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			m_popContractList.EditChange(sCode, m_Sign);
		}
	}
	else if ((id == ID_EDIT_FIRSTCONTRACT || id == ID_EDIT_SECONDCONTRACT) && HIWORD(wParam) == EN_KILLFOCUS)
	{
		if (IsWindowVisible(m_popContractList.GetHwnd()) && g_MouseTrack)
			ShowWindow(m_popContractList.GetHwnd(), SW_HIDE);
	}
}
void TAddContractView::OnNotify(WPARAM wParam, LPARAM lParam)
{

}
void TAddContractView::OnBtnClickDown(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case ID_BTN_FIRSTCONTRACT:
	case ID_BTN_SECONDCONTRACT:
		OnSearchCode(lParam);
		break;
	case ID_BTNOK:
		OnBtnOk();
		break;
	case ID_BTNCANCEL:
		OnBtnCancel();
		break;
	default:
		break;
	}
}
void TAddContractView::OnBtnOk()
{
	ShiftListInfo shiftContract;
	//memset(&shiftContract, 0, sizeof(ShiftListInfo));
	//获取合约
	m_editFirstContract.GetText(shiftContract.Code1, sizeof(shiftContract.Code1));
	m_editSecondContract.GetText(shiftContract.Code2, sizeof(shiftContract.Code2));
	//获取第一腿配比条件、数量
	shiftContract.LinkContract1 = m_cbFirstCondition.GetSelect();
	shiftContract.OrderQty1 = m_spinEditFirstQty.GetWindowNumber();
	//获取第二腿配比条件、数量
	shiftContract.LinkContract2 = m_cbSecondCondition.GetSelect();
	shiftContract.OrderQty2 = m_spinEditSecondQty.GetWindowNumber();
	//价格图表类型
	if (!m_cbCondition.GetSelect())
		shiftContract.LinkCondition = LcSub;
	else
		shiftContract.LinkCondition = LcDiv;

 	if (g_pShiftPosition->GetShiftSize() >= 6)
 	{
 		_TMessageBox_Domodal(m_Hwnd, "提示", L"自设移仓合约最多为6个", TMB_OK);
 		return;
 	}

	g_pShiftQuoteApi->QuoteShiftContract(shiftContract, m_Sign, m_Sign2);

	OnBtnCancel();
}
void TAddContractView::OnBtnCancel()
{
	if (IsWindowVisible(m_Hwnd))
		ShowWindow(m_Hwnd, SW_HIDE);
	else
		ShowWindow(m_Hwnd, SW_SHOW);
}
void TAddContractView::OnSearchCode(LPARAM lParam)
{
	if (!IsWindow(m_popContractCode.GetHwnd()))
		m_popContractCode.CreatePopWin(this, L"popupCode", m_Hwnd, WS_CLIPSIBLINGS, 0);

	POINT pt = { 0, 0 };
	if ((int)lParam == ID_BTN_FIRSTCONTRACT)
	{
		m_popContractCode.UpdateData(m_Sign);
		m_bFirstContract = true;
		ClientToScreen(m_editFirstContract.GetHwnd(), &pt);
	}
	else
	{
		m_popContractCode.UpdateData(m_Sign2);
		m_bFirstContract = false;
		ClientToScreen(m_editSecondContract.GetHwnd(), &pt);
	}
	pt.y += 20;
	SetWindowPos(m_popContractCode.GetHwnd(), 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);


}
void TAddContractView::OnPaint()
{
	TMemDC memdc(m_Hwnd);
	int oldDc = SaveDC(memdc.GetHdc());
	SetBkMode(memdc.GetHdc(), TRANSPARENT);

	RECT rcClient;
	GetClientRect(m_Hwnd, &rcClient);

	HBRUSH hbrBorder = CreateSolidBrush(RGB(100, 100, 100));
	InflateRect(&rcClient, -1, -1);
	FrameRect(memdc.GetHdc(), &rcClient, hbrBorder);
	InflateRect(&rcClient, -1, -1);
	FrameRect(memdc.GetHdc(), &rcClient, hbrBorder);

	RECT rcCaption = { rcClient.left, rcClient.top, rcClient.right, cCAPTION_H };

	HBRUSH hbrCaption = CreateSolidBrush(RGB(100, 100, 100));
	FillRect(memdc.GetHdc(), &rcCaption, hbrCaption);

	rcClient.top = cCAPTION_H;
	FillRect(memdc.GetHdc(), &rcClient, g_ColorRefData.g_brush_white);

	rcCaption.left = 8;
	SelectObject(memdc.GetHdc(), g_FontData.GetFontWord13());
	SetTextColor(memdc.GetHdc(), RGB(238, 238, 238));
	DrawText(memdc.GetHdc(), L"新增移仓合约", -1, &rcCaption, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	DrawClose(&memdc);


	DeleteObject(hbrBorder);
	DeleteObject(hbrCaption);
	RestoreDC(memdc.GetHdc(), oldDc);
}
void TAddContractView::DrawClose(TMemDC *pMemDC)
{
	int iold = SaveDC(pMemDC->GetHdc());
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(238, 238, 238));
	HPEN hOldPen = (HPEN)SelectObject(pMemDC->GetHdc(), hPen);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.top, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right + 1, m_rcClose.bottom + 1);

	MoveToEx(pMemDC->GetHdc(), m_rcClose.left, m_rcClose.bottom, 0);
	LineTo(pMemDC->GetHdc(), m_rcClose.right + 1, m_rcClose.top - 1);

	SelectObject(pMemDC->GetHdc(), hOldPen);
	DeleteObject(hPen);
	RestoreDC(pMemDC->GetHdc(), iold);
}
void TAddContractView::OnCreate()
{
	CreateWidget();
	InitWidget();
	SetWidgetPos();
}
LRESULT TAddContractView::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	SetBkMode((HDC)wParam, TRANSPARENT);
	return (LRESULT)g_ColorRefData.g_brush_white;
}
void TAddContractView::OnSize(WPARAM wParam, LPARAM lParam)
{

}
void TAddContractView::CreateWidget()
{
	//下单面板
	m_labFirstContract.Create(m_Hwnd, L"第一腿", 0, ID_LAB_FIRSTORDER);
	m_labSecondContract.Create(m_Hwnd, L"第二腿", 0, ID_LAB_SECONDORDER);
	m_editFirstContract.Create(m_Hwnd, 0, ES_UPPERCASE, ID_EDIT_FIRSTCONTRACT);
	m_editSecondContract.Create(m_Hwnd, 0, ES_UPPERCASE, ID_EDIT_SECONDCONTRACT);
	m_btnFirstContract.Create(m_Hwnd, "..", ID_BTN_FIRSTCONTRACT);
	m_btnSecondContract.Create(m_Hwnd, "..", ID_BTN_SECONDCONTRACT);

	m_labLine.Create(m_Hwnd, L"--------------------------------------------------------------------------------------------", 0, ID_LABLINE);
	m_labSet.Create(m_Hwnd, L"配比参数设置:", 0, ID_LABSET);


  	//第一腿
  	m_labFContract.Create(m_Hwnd, L"第一腿:", 0, ID_LABFIRSTCONTRACT);
  	m_cbFirstCondition.Create(m_Hwnd, ID_CBFIRSTCONDITION);
  	m_spinEditFirstQty.Create(m_Hwnd, 0, ID_SPINEDITFIRSTEDIT, ID_SPINEDITFIRSTSPIN);
  	//条件
  	m_labCondition.Create(m_Hwnd, L"价格图表", 0, ID_LABCONDITION);
	m_cbCondition.Create(m_Hwnd, ID_CBCONDITION);
  	//第二腿
  	m_labSContract.Create(m_Hwnd, L"第二腿:", 0, ID_LABSECONDCONTRACT);
  	m_cbSecondCondition.Create(m_Hwnd, ID_CBSECONDCONDITION);
  	m_spinEditSecondQty.Create(m_Hwnd, 0, ID_SPINEDITSECONDEDIT, ID_SPINEDITSECONDSPIN);

	m_btnOk.Create(m_Hwnd, "确定", ID_BTNOK);
	m_btnCancel.Create(m_Hwnd, "取消", ID_BTNCANCEL);

}
void TAddContractView::InitWidget()
{
	m_editFirstContract.SetFont(g_FontData.GetFontWord15());
	m_editSecondContract.SetFont(g_FontData.GetFontWord15());

	m_spinEditFirstQty.SetFont(g_FontData.GetFontNum13());
	m_spinEditSecondQty.SetFont(g_FontData.GetFontNum13());

	//m_cbCondition.SetFont(g_FontData.GetFontWord15());
	m_cbFirstCondition.SetFont(g_FontData.GetFontWord15());
	m_cbSecondCondition.SetFont(g_FontData.GetFontWord15());

	m_cbFirstCondition.AddString(L"*");
	m_cbFirstCondition.AddString(L"+");
	m_cbFirstCondition.AddString(L"-");
	m_cbFirstCondition.AddString(L"/");

	m_cbSecondCondition.AddString(L"*");
	m_cbSecondCondition.AddString(L"+");
	m_cbSecondCondition.AddString(L"-");
	m_cbSecondCondition.AddString(L"/");

	m_cbCondition.AddString(L"第一腿 - 第二腿");
	m_cbCondition.AddString(L"第一腿 / 第二腿");

	m_spinEditSecondQty.SetRange(0, 1000000);

	m_cbFirstCondition.SetSelect(0);
	m_cbSecondCondition.SetSelect(0);
	m_cbCondition.SetSelect(0);

	m_spinEditFirstQty.SetWindowNumber(1);
	m_spinEditSecondQty.SetWindowNumber(1);
}
void TAddContractView::SetWidgetPos()
{
	int EditWidth = 100;//一般控件宽度
	int EditContract = 150;//合约编辑控件
	int labWidth = 50;//lab控件宽度
	int Height = 20;//控件高度
	int btnContractWidth = 15;//合约选择按钮
	int ContractVap = 40;//两个合约之间的间隔

	POINT pt;
	pt.x = cleft; pt.y = cCAPTION_H + Height + cVap_y;
	//合约lab;
	m_labFirstContract.MoveWindow(pt.x, pt.y, EditWidth, Height);
	pt.x += EditContract + ContractVap + btnContractWidth;
	m_labSecondContract.MoveWindow(pt.x, pt.y, EditWidth, Height);
	//合约edit
	pt.x = cleft; pt.y += Height + cVap_y;
	m_editFirstContract.MoveWindow(pt.x, pt.y, EditContract, Height);
	//合约选择btn
	pt.x += EditContract;
	m_btnFirstContract.MoveWindow(pt.x, pt.y, btnContractWidth, Height);
	pt.x += btnContractWidth + ContractVap;
	m_editSecondContract.MoveWindow(pt.x, pt.y, EditContract, Height);
	//合约选择btn
	pt.x += EditContract;
	m_btnSecondContract.MoveWindow(pt.x, pt.y, btnContractWidth, Height);

	pt.x = cleft; pt.y += Height + cVap_y;
	m_labLine.MoveWindow(pt.x, pt.y, cFRAME_V - 2 * cleft, Height);

	//参数配置：
	pt.x = cleft; pt.y +=  2*Height;
	m_labSet.MoveWindow(pt.x, pt.y, EditWidth, Height);
	//第一腿(75为了使其居中显示)
	pt.x = cleft; pt.y += 2*Height;
	m_labFContract.MoveWindow(pt.x, pt.y, labWidth, Height);
	//第一腿条件
	pt.x += labWidth + cVap_x; 
	m_cbFirstCondition.MoveWindow(pt.x, pt.y, labWidth, 5 * Height);
	//第一腿手数edit
	pt.x += labWidth + cVap_x;
	m_spinEditFirstQty.MoveWindow(pt.x, pt.y, EditWidth, Height);
	//第二腿
	pt.x = cleft; pt.y += 2 * Height;
	m_labSContract.MoveWindow(pt.x, pt.y, labWidth, Height);
	//第二腿条件
	pt.x += labWidth + cVap_x;
	m_cbSecondCondition.MoveWindow(pt.x, pt.y, labWidth, 5 * Height);
	//第二腿手数Edit
	pt.x += labWidth + cVap_x;
	m_spinEditSecondQty.MoveWindow(pt.x, pt.y, EditWidth, Height);

	//价格图表
	pt.x = cleft; pt.y += 3 * Height;
	m_labCondition.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += labWidth + cVap_x;
	m_cbCondition.MoveWindow(pt.x, pt.y, labWidth + cVap_x + EditWidth, 4 * Height);

	//确定取消
	pt.x += labWidth + cVap_x +EditWidth + cVap_x; pt.y += cVap_y + Height;
	m_btnOk.MoveWindow(pt.x, pt.y, labWidth, Height);
	pt.x += cVap_x + labWidth; 
	m_btnCancel.MoveWindow(pt.x, pt.y, labWidth, Height);
}
void TAddContractView::OnMouseMove(WPARAM wParam, LPARAM lParam)
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
	InvalidateRect(m_Hwnd, 0, false);
}
void TAddContractView::OnMouseLeave()
{
	m_bMouseTrack = false;
}
void TAddContractView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);
	rect.bottom = rect.top + cCAPTION_H;
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	if (PtInRect(&m_rcClose, pt))
	{
		ShowWindow(m_Hwnd, SW_HIDE);
	}
	else if (PtInRect(&rect, pt))
	{
		PostMessage(m_Hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}
void TAddContractView::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case HTTOP:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, lParam);
		break;
	case HTBOTTOM:
		SendMessage(m_Hwnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, lParam);
		break;
	default:
		break;
	}
}
void TAddContractView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT id = GetWindowLong(GetFocus(), GWL_ID);
	if (id == ID_EDIT_FIRSTCONTRACT ||  id == ID_EDIT_SECONDCONTRACT)
	{
		if (!IsWindowVisible(m_popContractList.GetHwnd()))
			return;
		int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (nDelta > 0)
			m_popContractList.AddNum();
		else
			m_popContractList.DelNum();
	}
}
void TAddContractView::KeyDown(WPARAM wParam, LPARAM lParam)
{
	UINT id = GetWindowLong(GetFocus(), GWL_ID);
	if (id == ID_EDIT_FIRSTCONTRACT || id == ID_EDIT_SECONDCONTRACT)
	{
		if (!IsWindowVisible(m_popContractList.GetHwnd()))
			return;
		if (wParam == VK_DOWN)
			m_popContractList.AddNum();
		else if(wParam == VK_UP)
			m_popContractList.DelNum();
	}
}

void TAddContractView::SetContractNo(string sCode)
{
// 	string::size_type pos = sCode.find(" ");
// 	if (pos != string::npos)
// 	{
// 		string sContract = sCode.substr(++pos, string::npos);
// 		if (m_bFirstContract)
// 			m_editFirstContract.SetText(sContract.c_str());
// 		else
// 			m_editSecondContract.SetText(sContract.c_str());
// 	}

	m_bShowCodeList = false;
	if (m_bFirstContract)
		m_editFirstContract.SetText(sCode.c_str());
	else
		m_editSecondContract.SetText(sCode.c_str());
	m_bShowCodeList = true;
}