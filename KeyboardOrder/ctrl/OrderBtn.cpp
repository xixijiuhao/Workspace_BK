#include "..\PreInclude.h"

extern HINSTANCE g_hinst;
extern wstring LoadResString(int strID);

extern G_COLOREF g_ColorRefData;
extern G_UIFONT g_FontData;

COrderBtn::COrderBtn(COFInputDlg&ref) :m_RefDlg(ref)
{
	m_bEnterBtn = false;
	m_bFocus = false;
	m_iTotalNum = 0;
	m_iCoverNum = 0;
	m_iOpenNum = 0;
	m_iPrecision = 2;
}
COrderBtn::~COrderBtn()
{

}
void COrderBtn::InitButton(HWND hwnd)
{
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | BS_OWNERDRAW);
	Init(hwnd);
}
LRESULT CALLBACK COrderBtn::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_SETFOCUS:
			m_bFocus = true;
			InvalidateRect(GetHwnd(), NULL, TRUE);
			break;
		case WM_KILLFOCUS:
			m_bFocus = false;
			InvalidateRect(GetHwnd(), NULL, TRUE);
			break;
		case WM_MOUSEMOVE:
			{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 1;
			_TrackMouseEvent(&tme);
			}
			break;
		case WM_MOUSELEAVE:
			m_bEnterBtn = false;
			InvalidateRect(GetHwnd(), NULL, TRUE);
			break;
		case WM_MOUSEHOVER:
			if (false == m_bEnterBtn)
			{
				m_bEnterBtn = true;
				InvalidateRect(GetHwnd(), NULL, TRUE);
			}
			break;
		case WM_ERASEBKGND:
			return TRUE;
		case WM_KEYDOWN:
		{
			if (::IsWindow(m_RefDlg.GetHwnd()) && m_RefDlg.DoLeftRightUpDownKey(wParam))
			return 0;
			if (VK_TAB == wParam)
			{
				bool bShiftDown = (0x8000 & GetKeyState(VK_SHIFT)) != 0;
				if (::IsWindow(m_RefDlg.GetHwnd()))
				{
					if (bShiftDown)
						m_RefDlg.SetIndexFocus(m_RefDlg.GetLastFocusIndex(GetDlgCtrlID(GetHwnd())), true, false);
					else
						m_RefDlg.SetIndexFocus(m_RefDlg.GetNextFocusIndex(GetDlgCtrlID(GetHwnd())),true);
					return 0;
				}
			}
			else if (VK_RETURN == wParam)
			{
				if (::IsWindow(m_RefDlg.GetHwnd()))
					m_RefDlg.DisposeEnterMsg(wParam);
				return 0;
			}
		}
		break;
		default:
			break;
	}
	return CallWindowProc((WNDPROC)m_OldProc, m_hWnd, message, wParam, lParam); //默认回调过程
}
void COrderBtn::SetBitmapID(const wchar_t *pszTitle)
{
	m_strTitle = pszTitle;
}
void COrderBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC hDC = lpDrawItemStruct->hDC;
	RECT rect = lpDrawItemStruct->rcItem;
	UINT state = lpDrawItemStruct->itemState;
	//是买入按钮吗
	bool bBuy = (LoadResString(IDS_STR_Buy) == m_strTitle);

	HDC memdc;
	memdc = CreateCompatibleDC(hDC);
	HBITMAP bitmapOrgin = CreateCompatibleBitmap(hDC, rect.right - rect.left, rect.bottom - rect.top);
	SelectObject(memdc,bitmapOrgin);
	SelectObject(memdc,GetStockObject(NULL_BRUSH));

	if (state&ODS_SELECTED)
		DrawHelper::FillSolidRect(memdc, 3, 3, rect.right - rect.left - 6, rect.bottom - rect.top - 6, g_ColorRefData.GetColorBackground() );
	else
		DrawHelper::FillSolidRect(memdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, g_ColorRefData.GetColorBackground());
	if ((m_bEnterBtn || m_bFocus) && !(state&ODS_SELECTED))
	{
		HPEN pen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorLightBlue());
		SelectObject(memdc, pen);
		Rectangle(memdc, 0, 0, rect.right, rect.bottom);
		DeleteObject(pen);
	}
	else
	{
		HPEN pen = CreatePen(PS_SOLID, 1, g_ColorRefData.GetColorRGB150());
		SelectObject(memdc, pen);
		Rectangle(memdc, 0, 0, rect.right, rect.bottom);
		DeleteObject(pen);
	}

	//画Buy Sell Text
	SelectObject(memdc, g_FontData.GetFontWord19());

	SetBkMode(memdc,TRANSPARENT);
	if (bBuy)
		SetTextColor(memdc, g_ColorRefData.GetColorTextRed());
	else
		SetTextColor(memdc, g_ColorRefData.GetColorTextGreen());

	RECT rectTitle;
	if (m_bShowPrice)
	{
		rectTitle.left = 5;
		rectTitle.top = 0;
		rectTitle.right = rect.right-rect.left - 5;
		rectTitle.bottom = (rect.bottom - rect.top) / 2;
		DrawText(memdc, m_strTitle.c_str(), m_strTitle.length(), &rectTitle, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		SelectObject(memdc, g_FontData.GetFontNum15());
		rectTitle.left = 0;
		rectTitle.top = (rect.bottom - rect.top)/2;
		rectTitle.right = rect.right - rect.left ;
		rectTitle.bottom = (rect.bottom - rect.top) ;
		DrawText(memdc, m_strPrice.c_str(), m_strPrice.length(), &rectTitle, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	else
		DrawText(memdc, m_strTitle.c_str(), m_strTitle.length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	//画线
	if (m_bShowPrice)
	{
		COLORREF ref;
		if (bBuy)
			ref = g_ColorRefData.GetColorTextRed();
		else
			ref = g_ColorRefData.GetColorTextGreen();
		HPEN pen = CreatePen(PS_SOLID, 1, ref);
		SelectObject(memdc,pen);
		::MoveToEx(memdc, 5, (rect.bottom - rect.top) / 2,NULL);
		LineTo(memdc, rect.right - rect.left - 5, (rect.bottom - rect.top) / 2);
		DeleteObject(pen);
	}

	DeleteObject(bitmapOrgin);
	
	StretchBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SRCCOPY);
	DeleteDC(memdc);
}