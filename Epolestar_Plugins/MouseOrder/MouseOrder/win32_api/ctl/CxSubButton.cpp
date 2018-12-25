#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;

CxSubButton::CxSubButton():
m_nWidth(0),
m_nHeight(0),
m_nState(BTN_S_NORMAL),
m_bMouseTrack(TRUE),
m_bInRect(false)
{

}

CxSubButton::~CxSubButton()
{

}

void CxSubButton::CreatePopWin(LPCTSTR classname, HWND parent, DWORD style, RECT rect, bool btriangle, COLORREF color, DWORD exstyle)
{
	m_reClient = rect;
	m_ColorEdge = color;
	m_bTriangle = btriangle;

	CreateFrm(classname, parent, style, exstyle);
}
LRESULT CxSubButton::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:				OnCreate(wParam,lParam);
		break;
	case WM_SIZE:				OnSize(wParam,lParam);
		break;
	case WM_PAINT:				OnPaint(wParam, lParam);
		break;
	case WM_MOUSEMOVE:			OnMouseMove(wParam, lParam);
		break;
	case WM_MOUSEHOVER:			OnMouseHover(wParam, lParam);
		break;
	case WM_MOUSELEAVE:			OnMouseLeave(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:		OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONDBLCLK://双击转单击
		SendMessage(m_Hwnd, WM_LBUTTONDOWN, wParam, lParam);
		break;
	case WM_LBUTTONUP:			OnLButtonUp(wParam, lParam);
		break;
	case WM_SETFOCUS:			OnSetFocus(wParam, lParam);
		break;
	case WM_KILLFOCUS:			OnKillFocus(wParam, lParam);
		break;
	case WM_DESTROY:			OnDestory(wParam, lParam);
		break;
	case WM_ERASEBKGND:
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}

void CxSubButton::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	SetFocus(m_Hwnd);
	m_nState = BTN_S_DOWN;
	InvalidateRect(m_Hwnd, NULL, FALSE);
	if (!m_bInRect)
		m_bInRect = true;
}

void CxSubButton::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_nState = BTN_S_UP;
	if (!m_bInRect)
		return;
	InvalidateRect(m_Hwnd, NULL, FALSE);
}

void CxSubButton::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (m_bMouseTrack)    //若允许追踪，则。
	{
		TRACKMOUSEEVENT csTME;
		csTME.cbSize = sizeof(csTME);
		csTME.dwFlags = TME_LEAVE | TME_HOVER;
		csTME.hwndTrack = m_Hwnd;//指定要追踪的窗口
		csTME.dwHoverTime = 10;  //鼠标在按钮上停留超过10ms，才认为状态为HOVER
		::TrackMouseEvent(&csTME); //开启Windows的WM_MOUSELEAVE，WM_MOUSEHOVER事件支持
		m_bMouseTrack = FALSE;   //若已经追踪，则停止追踪
	}
}

void CxSubButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_nState = BTN_S_OVER;
	InvalidateRect(m_Hwnd, NULL, FALSE);
}

void CxSubButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseTrack = TRUE;
	{
		HWND h = GetFocus();
		if (h != m_Hwnd)
		{
			m_nState = BTN_S_NORMAL;
			InvalidateRect(m_Hwnd, NULL, FALSE);
		}
		else
		{
			m_nState = BTN_S_FOCUS;
			InvalidateRect(m_Hwnd, NULL, FALSE);
		}
	}
	m_bInRect = false;
}

void CxSubButton::OnCreate(WPARAM wParam, LPARAM lParam)
{	
	//初始化dc 画刷
	HDC			hdc;
	hdc = GetDC(m_Hwnd);
	m_HdcMemNormal = CreateCompatibleDC(hdc);// bitmap
	m_HdcMemSpecial = CreateCompatibleDC(hdc);// bitmap
	m_HdcMemInvalid = CreateCompatibleDC(hdc);// bitmap
	m_hdctemp = CreateCompatibleDC(hdc);
	
	m_nWidth = m_reClient.right - m_reClient.left;
	m_nHeight = m_reClient.bottom - m_reClient.top;//获取窗口大小客户区
	
	RECT rect;
	rect.left = 0;
	rect.right = m_nWidth;
	rect.top = 0;
	rect.bottom = m_nHeight;

	//选入位图
	m_hBitmapNormal = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
	m_hBitmapInvalid = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
	m_hBitmapSpecial = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
	m_hbmptemp = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
	GetObject(m_hBitmapNormal, sizeof(BITMAP), &m_bBitmapInfoNormal);
	GetObject(m_hBitmapSpecial, sizeof(BITMAP), &m_bBitmapInfoSpecial);
	SelectObject(m_HdcMemNormal, m_hBitmapNormal);
	SelectObject(m_HdcMemInvalid, m_hBitmapInvalid);
	SelectObject(m_HdcMemSpecial, m_hBitmapSpecial);
	SelectObject(m_hdctemp, m_hbmptemp);
	FillRect(m_HdcMemNormal, &rect, g_ColorRefData.GetHbrWhite());
	FillRect(m_HdcMemInvalid, &rect, g_ColorRefData.GetHbrBackground());
	FillRect(m_HdcMemSpecial, &rect, g_ColorRefData.GetHbrBlue());
	if (m_bTriangle)
	{
		HPEN    hGrayPen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());	//黑色画刷
		HPEN hWhitePen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorWhite());

		SelectObject(m_HdcMemNormal, hGrayPen);
		SelectObject(m_HdcMemInvalid, hGrayPen);
		SelectObject(m_HdcMemSpecial, hWhitePen);

		DrawTriangle(m_HdcMemNormal, m_nWidth * 4 / 5, m_nHeight / 2 - 3, 9);
		DrawTriangle(m_HdcMemInvalid, m_nWidth * 4 / 5, m_nHeight / 2 - 3, 9);
		DrawTriangle(m_HdcMemSpecial, m_nWidth * 4 / 5, m_nHeight / 2 - 3, 9);

		DeleteObject(hGrayPen);
		DeleteObject(hWhitePen);
	}	
	ReleaseDC(m_Hwnd, hdc);
		
	//提示信息
	SetWindowPos(m_Hwnd, NULL, m_reClient.left, m_reClient.top, m_reClient.right - m_reClient.left, m_reClient.bottom - m_reClient.top, SWP_SHOWWINDOW);
}

void CxSubButton::OnSize(WPARAM wParam, LPARAM lParam)
{
	GetClientRect(m_Hwnd, &m_reClient);
}
void CxSubButton::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	m_nState = BTN_S_FOCUS;
	InvalidateRect(m_Hwnd, NULL, FALSE);
}

void CxSubButton::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	m_nState = BTN_S_NORMAL;
	InvalidateRect(m_Hwnd, NULL, FALSE);
	int id = GetWindowLong(m_Hwnd, GWL_ID);
	SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(id, BN_KILLFOCUS), 0);
}

void CxSubButton::OnDestory(WPARAM wParam, LPARAM lParam)
{
	DeleteDC(m_HdcMemNormal);
	DeleteDC(m_HdcMemSpecial);
	DeleteDC(m_HdcMemInvalid);
	DeleteDC(m_hdctemp);

	DeleteObject(m_hBitmapNormal);
	DeleteObject(m_hBitmapInvalid);
	DeleteObject(m_hBitmapSpecial);
	DeleteObject(m_hbmptemp);
}
void CxSubButton::OnPaint(WPARAM wParam, LPARAM lParam)
{
	HBRUSH hFocusBrush = CreateSolidBrush(m_ColorEdge);		//边框画刷
	HDC			hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(m_Hwnd, &ps);

	RECT itemRect = m_reClient;
	SetStretchBltMode(m_hdctemp, HALFTONE);//背景
		
	if (!IsWindowEnabled(m_Hwnd))
	{
		StretchBlt(m_hdctemp, 0, 0, m_nWidth, m_nHeight, m_HdcMemInvalid, 0, 0, m_bBitmapInfoNormal.bmWidth, m_bBitmapInfoNormal.bmHeight, SRCCOPY);
		SetTextColor(m_hdctemp, g_ColorRefData.GetColorBlack());
	}
	else if (m_nState != BTN_S_NORMAL)
	{
		StretchBlt(m_hdctemp, 0, 0, m_nWidth, m_nHeight, m_HdcMemSpecial, 0, 0, m_bBitmapInfoSpecial.bmWidth, m_bBitmapInfoSpecial.bmHeight, SRCCOPY);
		SetTextColor(m_hdctemp, g_ColorRefData.GetColorWhite());
	}
	else
	{
		StretchBlt(m_hdctemp, 0, 0, m_nWidth, m_nHeight, m_HdcMemNormal, 0, 0, m_bBitmapInfoNormal.bmWidth, m_bBitmapInfoNormal.bmHeight, SRCCOPY);
		SetTextColor(m_hdctemp, g_ColorRefData.GetColorBlack());
	}
	::FrameRect(m_hdctemp, &itemRect, hFocusBrush);
	switch (m_nState)//线条
	{
	case BTN_S_DOWN:
		InflateRect(&itemRect, -1, -1);
		::FrameRect(m_hdctemp, &itemRect, hFocusBrush);
		break;
	default:
		break;
	}
	OnDrawItemText(m_hdctemp, itemRect);
		
	BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_hdctemp, 0, 0, SRCCOPY);

	EndPaint(m_Hwnd, &ps);

	DeleteObject(hFocusBrush);
}
void CxSubButton::DrawTriangle(HDC &hdc, int nStartx, int nStarty, int nWidth, bool bFull)//宽度奇数
{
	if (nWidth % 2 == 0)
		return;
	if (bFull)
	{
		for (int i = 0; i < nWidth / 2 + 1; i++)
		{
			MoveToEx(hdc, nStartx + i, nStarty + i, NULL);
			LineTo(hdc, nStartx + nWidth - i, nStarty + i);
		}
		return;
	}
	else
	{
		int nMidx = nStartx + nWidth / 2 ;
		int nMidy = nStarty + nWidth / 2 ;
		MoveToEx(hdc, nStartx, nStarty, NULL);
		LineTo(hdc, nMidx, nMidy);

		int nEndx = nStartx + nWidth-1;
		int nEndy = nStarty;
		LineTo(hdc, nEndx+1, nEndy-1);
	}
}