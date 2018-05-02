#include "..\PreInclude.h"
//色彩
extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;
extern IStrategyTradeAPI *g_pStrategyApi;
CPopWinTip::CPopWinTip() 
: m_nWidth(0)
, m_nHeight(0)
, m_bHover(false)
, m_bMouseTrack(TRUE)
{

}

CPopWinTip::~CPopWinTip()
{
	if (IsWindow(m_Hwnd))
		DestroyWindow(m_Hwnd);
}

void CPopWinTip::CreatePopWin(LPCTSTR classname, HWND parent, DWORD style, DWORD exstyle)
{
	m_string = LoadResWchar_t(IDS_PopTip);
	CreateFrm(classname, parent, style, exstyle);
}

LRESULT CPopWinTip::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(m_Hwnd);	 
			//////////////////////////////////////////////////////////////////////////
			m_HdcMem = CreateCompatibleDC(hdc);
			//////////////////////////////////////////////////////////////////////////
			ReleaseDC(m_Hwnd, hdc);
		}
		break;
	case WM_SIZE:
		GetClientRect(m_Hwnd, &m_reClient);
		m_nWidth = m_reClient.right - m_reClient.left;
		m_nHeight = m_reClient.bottom - m_reClient.top;
		//////////////////////////////////////////////////////////////////////////
		hdc = GetDC(m_Hwnd);

		if (m_hBKBitmap)
			DeleteObject(m_hBKBitmap);

		m_hBKBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
		SelectObject(m_HdcMem, m_hBKBitmap);

		ReleaseDC(m_Hwnd, hdc);
		//////////////////////////////////////////////////////////////////////////
		break;
	case WM_PAINT:
	{
		COLORREF ref;
		if (m_bHover)
			ref = RGB(0, 150, 200); //RGB(240, 201, 107); //
		else
			ref = RGB(0, 91, 174);//RGB(248, 151, 49); 
		HBRUSH  hFGBrush = CreateSolidBrush(ref);	//前景画刷 单元
		
		hdc = BeginPaint(m_Hwnd, &ps);

		SelectObject(m_HdcMem, g_FontData.GetFontWord15());
		SetTextColor(m_HdcMem, RGB(255, 255, 255));
		SetBkMode(m_HdcMem, TRANSPARENT);
		FillRect(m_HdcMem, &m_reClient, hFGBrush);
		
		RECT rect = m_reClient;
		DrawText(m_HdcMem, m_string.c_str(), m_string.size(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			
		BitBlt(hdc, 0, 0, m_nWidth, m_nHeight, m_HdcMem, 0, 0, SRCCOPY);

		EndPaint(m_Hwnd, &ps);

		DeleteObject(hFGBrush);
	}	
		break;
	case WM_MOUSEMOVE:
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
		break;
	case WM_MOUSEHOVER:	
		m_bHover = true;
		InvalidateRect(m_Hwnd, NULL, FALSE);
		break;
	case WM_MOUSELEAVE:	
		m_bMouseTrack = TRUE;
		m_bHover = false;
		InvalidateRect(m_Hwnd, NULL, FALSE);
		break;
	case WM_LBUTTONUP:
	{
		POINT pointB;
		pointB.x = LOWORD(lParam);
		pointB.y = HIWORD(lParam);
		if (g_pStrategyApi)
			g_pStrategyApi->StopLossMakeSure(m_Hwnd);
		ShowWindow(m_Hwnd, SW_HIDE);
	}
		break;
	case WM_DESTROY:
		DeleteObject(m_hBKBitmap);
		DeleteDC(m_HdcMem);
		break;
	case WM_ERASEBKGND:
		break;
	default:                      /* 不感兴趣的消息交给DefWindowProc()处理 */
		return NOT_PROCESSED;
	}

	return PROCESSED;
}