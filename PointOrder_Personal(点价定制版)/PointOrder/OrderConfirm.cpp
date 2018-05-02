#include "PreInclude.h"

#define MESSAGE_BOTTOM			58
#define  WndFrmW  330
#define  WndFrmH  200
TOrderConfirm::TOrderConfirm(HWND hparent, UINT ntype):TModalWnd(hparent, "提示")
{
	//m_parent = hparent;
	//m_sztitle = "提示";
	//memset(m_szcontent, 0, sizeof(m_szcontent));
	m_ntype = ntype;
	m_bFocusOk = true;
	m_bFocusNo = false;
	memset(&m_rcOk, 0, sizeof(RECT));
	memset(&m_rcNo, 0, sizeof(RECT));
	m_iOrderQty = NULL;
}

TOrderConfirm::~TOrderConfirm()
{

}

extern TCHAR szmsgCHS[2][20];
extern  TCHAR szmsgCHT[2][20];
extern  TCHAR szmsgENU[2][20];
TCHAR* OrderGetMessagebox(int i)
{
	switch (g_LanguageID)
	{
	case NONE:
		break;
	case CHT:
		return szmsgCHT[i];
	case CHS:
		return szmsgCHS[i];
	case ENU:
		return szmsgENU[i];
	default:
		break;
	}

}
void TOrderConfirm::CreateSubCtrl()
{
	//创建子控件
	m_scContract.Create(m_Hwnd, L"合约：", id_scContract);
	m_scContractInfo.Create(m_Hwnd, LoadRC::ansi_to_unicode(m_szContractInfo).c_str(), id_scContractInfo , SS_LEFT);
	m_scOrderQty.Create(m_Hwnd, L"手数:", id_scOrderQty);
	m_scOrderPrice.Create(m_Hwnd, L"价格：", id_scOrderPrice);
	m_scOrderPriceInfo.Create(m_Hwnd, LoadRC::ansi_to_unicode(m_szOrderPrice).c_str(), id_scOrderPriceInfo, SS_LEFT);

	HWND hedit;
	HWND hSpin;
	CreateSpinHwnd(hedit, hSpin, id_editOrderQty, id_spinOrderQty);
	m_editOrderQty.Init(hedit);
	m_spinOrderQty.Init(hSpin);

	SetWindowInt(m_editOrderQty.GetHwnd(), *m_iOrderQty);
	//m_scOrderPriceInfo.SetAlign(SS_LEFT);
	//m_scContractInfo.SetAlign(SS_LEFT);


	//设置控件属性
	m_scContract.SetFont(g_FontData.g_FontWord13);
	m_scContractInfo.SetFont(g_FontData.g_FontWord13);
	m_scOrderQty.SetFont(g_FontData.g_FontWord13);
	m_scOrderPrice.SetFont(g_FontData.g_FontWord13);
	m_scOrderPriceInfo.SetFont(g_FontData.g_FontWord13);
	m_editOrderQty.SetFont(g_FontData.g_FontWord13);

	
	//设置控件位置
	int WidgetW = 40;
	int infoW = 80;
	int WidgetH = 20;
	int spinW = 19;
	int editW = 40;
	int leftVap = 20;
	int vap = 10;


	//合约
	POINT pt = { leftVap, CAPTION_HEIGHT + vap };
	m_scContract.MoveWindow(pt.x, pt.y, WidgetW, WidgetH);
	pt.x += WidgetW + vap;
	m_scContractInfo.MoveWindow(pt.x, pt.y, 2*infoW, WidgetH);

	//手数
	pt.x = leftVap; pt.y += WidgetH + vap;
	m_scOrderQty.MoveWindow(pt.x, pt.y, WidgetW, WidgetH);
	pt.x += WidgetW + vap;
	SetWindowPos(m_editOrderQty.GetHwnd(), 0, pt.x, pt.y, editW, WidgetH, 0);
	pt.x += editW + 1;
	SetWindowPos(m_spinOrderQty.GetHwnd(), 0, pt.x, pt.y, spinW, WidgetH, 0);

	//价格
	pt.x = leftVap; pt.y += WidgetH + vap;
	m_scOrderPrice.MoveWindow(pt.x, pt.y, WidgetW, WidgetH);
	pt.x += WidgetW + vap;
	m_scOrderPriceInfo.MoveWindow(pt.x, pt.y, 2 * infoW, WidgetH);
}


void TOrderConfirm::SetModalRect()
{
	POINT pt;
	GetCursorPos(&pt);
	MoveWindow(m_Hwnd, pt.x - WndFrmW, pt.y - WndFrmH, WndFrmW, WndFrmH, true);
}
//MSG
void TOrderConfirm::DrawMain(TMemDC* pmemdc)
{
	RECT rect; GetClientRect(m_Hwnd, &rect);
	SelectObject(pmemdc->GetHdc(), g_FontData.g_FontWord15);

	rect.top += m_rctitle.bottom;
	rect.bottom -= MESSAGE_BOTTOM;
	InflateRect(&rect, -1, 0);
	HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(pmemdc->GetHdc(), &rect, hbr);
	rect.left += 20; rect.right -= 20;
	rect.top += 23;
	SetTextColor(pmemdc->GetHdc(), RGB(0, 0, 0));
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT);
	//DrawText(pmemdc->GetHdc(), m_szcontent, -1, &rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK/*DT_CENTER | DT_VCENTER | DT_SINGLELINE*/);
	DeleteObject(hbr);

	//画按钮按钮：
	HBRUSH btnHbr = CreateSolidBrush(RGB(0, 0, 0));
	UINT uformat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

	if (m_ntype == TMB_OK)
	{
		FrameRect(pmemdc->GetHdc(), &m_rcOk, btnHbr);
		if (m_bFocusOk)
			DrawFocusBtn(pmemdc, ID_OK);
		else
			DrawFocusBtn(pmemdc, ID_NULL);
	}
	else if (m_ntype == TMB_OKCANCEL)
	{
		FrameRect(pmemdc->GetHdc(), &m_rcOk, btnHbr);
		FrameRect(pmemdc->GetHdc(), &m_rcNo, btnHbr);
		if (m_bFocusOk)
			DrawFocusBtn(pmemdc, ID_OK);
		else if (m_bFocusNo)
			DrawFocusBtn(pmemdc, ID_NO);
		else
			DrawFocusBtn(pmemdc, ID_NULL);
	}
}
void TOrderConfirm::DrawFocusBtn(TMemDC* pmemdc, int id)
{
	int ihc = SaveDC(pmemdc->GetHdc());
	UINT uformat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	HBRUSH hbr = CreateSolidBrush(RGB(25, 121, 202));
	HBRUSH Framehbr = CreateSolidBrush(RGB(126, 180, 234));
	if (id == ID_OK)
	{
		FillRect(pmemdc->GetHdc(), &m_rcOk, hbr);
		FrameRect(pmemdc->GetHdc(), &m_rcOk, Framehbr);
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorWhite);
		DrawText(pmemdc->GetHdc(), OrderGetMessagebox(0), -1, &m_rcOk, uformat);
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorBlack);
		DrawText(pmemdc->GetHdc(), OrderGetMessagebox(1), -1, &m_rcNo, uformat);
	}
	else if (id == ID_NO)
	{
		FillRect(pmemdc->GetHdc(), &m_rcNo, hbr);
		FrameRect(pmemdc->GetHdc(), &m_rcNo, Framehbr);
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorWhite);
		DrawText(pmemdc->GetHdc(), OrderGetMessagebox(1), -1, &m_rcNo, uformat);
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorBlack);
		DrawText(pmemdc->GetHdc(), OrderGetMessagebox(0), -1, &m_rcOk, uformat);
	}
	else
	{
		SetTextColor(pmemdc->GetHdc(), g_ColorRefData.g_ColorBlack);
		DrawText(pmemdc->GetHdc(), OrderGetMessagebox(1), -1, &m_rcNo, uformat);
		DrawText(pmemdc->GetHdc(), OrderGetMessagebox(0), -1, &m_rcOk, uformat);
	}
	DeleteObject(hbr);
	DeleteObject(Framehbr);
}
void TOrderConfirm::OnMouseMove()
{
	__super::OnMouseMove();
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);
	if (PtInRect(&m_rcOk, pt))
		SetBtnFocus(ID_OK);
	else if (PtInRect(&m_rcNo, pt))
		SetBtnFocus(ID_NO);
	else
	{
		if (m_bFocusNo)
			SetBtnFocus(ID_NO);
		else if (m_bFocusOk)
			SetBtnFocus(ID_OK);
	}
}
void TOrderConfirm::OnLbuttonDown(WPARAM wParam, LPARAM lParam)
{
	__super::OnLbuttonDown(wParam, lParam);
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_Hwnd, &pt);

	if (PtInRect(&m_rcOk, pt))
		Result(ID_OK);
	else if (PtInRect(&m_rcNo, pt))
		Result(ID_NO);

}
void TOrderConfirm::Result(int id)
{
	if (id == ID_NO)
		m_nret = 0;
	else if (id == ID_OK)
		m_nret = 1;
	DestroyWindow(m_Hwnd);
}
void TOrderConfirm::CreateSpinHwnd(HWND& hedit, HWND& hSpin, int idEdit, int idSpin)
{
	hedit = CreateWindowEx(0, WC_EDIT, TEXT(""), ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NUMBER | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS,
		0, 0, 0, 0, m_Hwnd, (HMENU)idEdit, GetModuleHandle(NULL), NULL);
	hSpin = CreateWindowEx(0, UPDOWN_CLASS, TEXT(""), WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | WS_CLIPSIBLINGS,
		0, 0, 0, 0, m_Hwnd, (HMENU)idSpin, GetModuleHandle(NULL), NULL);
	SendMessage(hSpin, UDM_SETBUDDY, (WPARAM)hedit, 0);
	SendMessage(hSpin, UDM_SETRANGE32, 0, (LPARAM)MAKELPARAM(65535, 0));
	//SendMessage(hSpin, UDM_SETBASE, 10, 0);
	SendMessage(hedit, EM_SETLIMITTEXT, 10, 0);
	SendMessage(hSpin, UDM_SETRANGE, 0, (LPARAM)MAKELONG((short)1000, (short)1));
}
void TOrderConfirm::OnSize()
{
	__super::OnSize();
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	RECT rcok, rcno;
	rcok.right = rect.right - 20; rcok.left = rcok.right - 100;
	rcok.bottom = rect.bottom - 13; rcok.top = rcok.bottom - 31;
	rcno = rcok; rcno.right = rcno.left - 13;
	rcno.left = rcno.right - 100;

	if (m_ntype == TMB_OK)
	{
		m_rcOk = { (rect.right - rect.left - 100) / 2, rcok.top, (rect.right - rect.left - 100) / 2 + 100, rcok.top + 31 };
	}
	if (m_ntype == TMB_OKCANCEL)
	{
		m_rcOk = { rcno.left, rcno.top, rcno.left + 100, rcno.top + 31 };
		m_rcNo = { rcok.left, rcok.top, rcok.left + 100, rcok.top + 31 };
	}
}

void TOrderConfirm::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 1)
		m_nret = 0;
	else
		m_nret = 1;
	DestroyWindow(m_Hwnd);
}
void TOrderConfirm::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_LEFT:
	case VK_RIGHT:
		if (m_ntype == TMB_OKCANCEL)
			SetBtnFocus();
		break;
	case VK_RETURN:
	{
		if (m_bFocusOk)
			m_nret = 1;
		else
			m_nret = 0;
		DestroyWindow(m_Hwnd);
	}
	default:
		break;
	}
}
void TOrderConfirm::SetBtnFocus(int id)
{
	if (id == ID_OK)
	{
		m_bFocusOk = true;
		m_bFocusNo = false;
	}
	else if (id == ID_NO)
	{
		m_bFocusOk = false;
		m_bFocusNo = true;
	}
	else if (id == ID_NULL)
	{
		m_bFocusOk = false;
		m_bFocusNo = false;
	}
	InvalidateRect(m_Hwnd, NULL, TRUE);
}
void TOrderConfirm::SetBtnFocus()
{
	if (m_bFocusNo)
		SetBtnFocus(ID_OK);
	else if (m_bFocusOk)
		SetBtnFocus(ID_NO);
	else
		SetBtnFocus(ID_NULL);
}

LRESULT TOrderConfirm::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == id_editOrderQty && HIWORD(wParam) == EN_CHANGE)
		{
			char OrderQty[20] = { 0 };
			GetWindowTextA(m_editOrderQty.GetHwnd(), OrderQty, sizeof(OrderQty));
			*m_iOrderQty = atoi(OrderQty);
		}
	}
		break;
	case WM_SIZE:
		OnSize();
		break;
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove();
		break;
	case WM_CTLCOLORSTATIC:
	{
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)g_ColorRefData.g_brush_white;
	}
	default:
		break;
	}

	return __super::WndProc(message, wParam, lParam);
}

// bool OrderConfirm_Demodal(HWND hwnd, const  char* sztitle, const wchar_t* sztext, UINT ntype)
// {
// 	TOrderConfirm tmsgbox(hwnd, ntype);;
// 	return tmsgbox.Domodal();
// }
bool OrderConfirm_Domodal(HWND hwnd, char* ContractInfo, uint* OrderQty, char* OrderPriceInfo, UINT ntype)
{
	TOrderConfirm box(hwnd, ntype);
	box.setContractInfo(ContractInfo);
	box.setOrderQty(OrderQty);
	box.setOrderPrice(OrderPriceInfo);
	return box.Domodal();
}