
#include "stdafx.h"
#include "DwmObject.h"
#include "Global.h"
#include "CharCodes.h"

CDwmObject::CDwmObject():m_clTransparency(0){}

CDwmObject::~CDwmObject()
{
	UnBindWindow(m_dlgHwnd);
	if (m_brBackGnd)
		::DeleteObject(m_brBackGnd);
}

bool CDwmObject::Init(HWND dlgHwnd, MARGINS margins)
{ 
	if (!::IsWindow(dlgHwnd))
		return false;

	m_Margins			= margins;
	m_brBackGnd			= ::CreateSolidBrush(m_clTransparency);
	m_rgnDwmFream		= GetDmwFreamRGN(dlgHwnd, m_Margins);

	if (m_dlgHwnd != dlgHwnd)
	{
		//�������˳���ܵ���
		UnBindWindow(m_dlgHwnd);
		m_dlgHwnd = dlgHwnd;
		BindWindow(m_dlgHwnd);
	}

	ExtendFrameIntoClientArea();

	return true;
}

bool CDwmObject::IsCompositionEnabled()
{
	HRESULT hr;
	BOOL bEnabled;
	hr = ::DwmIsCompositionEnabled(&bEnabled);
	return SUCCEEDED(hr) && bEnabled==TRUE;
}
//��ô��ڿͻ�����
HRGN CDwmObject::GetDmwFreamRGN(HWND hwnd, MARGINS margins)					
{
	CRect mRect;
	::GetClientRect(hwnd, mRect);
	HRGN cRgn = ::CreateRectRgn(mRect.left, mRect.top, mRect.right, mRect.bottom);

	//���͸������
	if (margins.cxLeftWidth>=0 || margins.cyTopHeight>=0 || margins.cxRightWidth>=0 || margins.cyBottomHeight>=0)
	{
		mRect.DeflateRect(margins.cxLeftWidth, margins.cyTopHeight,	margins.cxRightWidth, margins.cyBottomHeight);
	
		HRGN mRgn = ::CreateRectRgn(mRect.left, mRect.top, mRect.right, mRect.bottom);
		::CombineRgn(cRgn, cRgn, mRgn, RGN_DIFF);
	}

	return cRgn;
}
bool CDwmObject::CtrlInDwmFream(HWND parent, HWND child)
{
	CRect mRect;

	::GetWindowRect(child, &mRect);
	if (parent!=NULL)
		::MapWindowPoints(::GetDesktopWindow(), parent, (CPoint*)&mRect, 2);

	return ::RectInRegion(m_rgnDwmFream, &mRect)!=0;
}

void CDwmObject::BindWindow(HWND hWnd)				//�ض��򴰿ڵ�WndProc��CDwmFun::DlgWndProc
{
	if (hWnd == NULL)
		return;

	m_dlgWndProcOld = (WNDPROC) ::GetWindowLong(hWnd, GWL_WNDPROC);	//�����ԭʼ������			
	m_dlgWndProc.Assign(this, &CDwmObject::DlgWndProc);				//ת��Ա����dlgWndProcΪ�ص�����			
	::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(WNDPROC)m_dlgWndProc);//���ô��ڵĻص�����ΪDlgWndProc
}
void CDwmObject::UnBindWindow(HWND hWnd)
{
	if (hWnd != NULL)
		::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(WNDPROC)m_dlgWndProcOld);//��ԭ���ڵĻص�����Ϊm_dlgWndProcOld
}

LOGFONT CDwmObject::GetSysFont()
{
	LOGFONT lFont = {0};
			
	HTHEME hTheme = ::GetWindowTheme(m_dlgHwnd);
	if (hTheme!=NULL)//!::IsThemeActive())
	{
		LOGFONTW mlFont = {0};
		::GetThemeSysFont(hTheme, TMT_MSGBOXFONT, &mlFont);
		memcpy_s(&lFont, sizeof(lFont)-sizeof(lFont.lfFaceName), &mlFont, sizeof(mlFont)-sizeof(mlFont.lfFaceName));
		strcpy_s(lFont.lfFaceName, CCodes::UnicodeToANSI(mlFont.lfFaceName).c_str());
	}
	else
	{
		NONCLIENTMETRICS ncm = {sizeof(NONCLIENTMETRICS)};
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false);
		lFont = ncm.lfMessageFont;
	}

	return lFont;
}	
CFont * CDwmObject::GetCurrFont(HWND hWnd)
{
	CWnd * pWnd = CWnd::FromHandlePermanent(hWnd);
	return pWnd->GetFont();
}

//���ڴ����WM_CREARE��OnInitDialog�¼���
HRESULT CDwmObject::ExtendFrameIntoClientArea()							
{
	HRESULT hr = S_OK;
	if (!IsCompositionEnabled())
		return hr;
	
	//����͸������
	hr = ::DwmExtendFrameIntoClientArea(m_dlgHwnd, &m_Margins);

	//����͸�������ڵ��Ի�����
	HWND hWnd = ::GetTopWindow(m_dlgHwnd);
	while (hWnd)
	{
		LONG lStyle = GetWindowLong(hWnd, GWL_STYLE );
		//lStyle &= ~(WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE);
		if (CtrlInDwmFream(m_dlgHwnd, hWnd)) 
			lStyle = lStyle | SS_OWNERDRAW;
		else
			lStyle = lStyle & ~SS_OWNERDRAW;

		SetWindowLong(hWnd, GWL_STYLE, lStyle);

		hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
	}

	//ʹ�õ�ǰ���������Ʊ���
	::EnableThemeDialogTexture(m_dlgHwnd, true);

	return hr;
}

HBRUSH CDwmObject::GetCtrlBrush(HWND hWnd, HDC hDC, HBRUSH brDefault)//���ڴ����OnCtrlColor�¼���
{
	if (CtrlInDwmFream(m_dlgHwnd, hWnd))
	{
		::SetBkMode(hDC, TRANSPARENT); 
		return m_brBackGnd;
	}
	else
		return brDefault;
}

void CDwmObject::DrawTransparencyBack()							//���ڴ����OnParint�¼���
{
	if (!IsCompositionEnabled())
		return;

	HDC hDC = ::GetDC(m_dlgHwnd);
	
	//���͸������
	HGDIOBJ hOldBrush = ::SelectObject(hDC, (HGDIOBJ)m_brBackGnd);
	::SetBkMode(hDC, TRANSPARENT); 
	::FillRgn(hDC, m_rgnDwmFream, m_brBackGnd);

	::SelectObject(hDC, hOldBrush);
	::ReleaseDC(m_dlgHwnd, hDC);
	
	//�ػ�͸������Ŀؼ�
	::InvalidateRgn(m_dlgHwnd, m_rgnDwmFream, FALSE);
}

bool CDwmObject::DrawItem(HWND itemHwnd, HDC itemDC)
{
	LONG lStyle = GetWindowLong(itemHwnd, GWL_STYLE);
	if ((lStyle & SS_OWNERDRAW) != SS_OWNERDRAW) 
		return false;
	if (!CtrlInDwmFream(m_dlgHwnd, itemHwnd))
		return false;

	CApiMemDC mDC(itemDC, TRUE);
	HDC hDC = mDC.GetDC();

	CRect mRect;
	::GetWindowRect(itemHwnd, &mRect);

	int sLength = ::GetWindowTextLength(itemHwnd);
	char * pText = (char *)malloc(sLength + 1);
	::GetWindowText(itemHwnd, pText, sLength);
	
	mRect -= mRect.TopLeft(); //��ͬ��rect�������Ͻ�Ϊ(0,0)
	CFont * pFont = GetCurrFont(itemHwnd);
	HGDIOBJ hOldObj = ::SelectObject(hDC, pFont->GetSafeHandle());

	HTHEME htheme = OpenThemeData(m_dlgHwnd,L"globals");
	typedef long (__stdcall * PDrawThemeTextEx)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, _In_reads_(cchText) LPCWSTR pszText, int cchText, DWORD dwTextFlags, _Inout_ LPRECT pRect, _In_opt_ const DTTOPTS *pOptions);
	PDrawThemeTextEx pDrawThemeTextEx = 0;
	if (htheme!=NULL && IsCompositionEnabled())
	{
		HINSTANCE hDll = LoadLibrary("UxTheme.dll");
		if (hDll!=0)
			pDrawThemeTextEx = (PDrawThemeTextEx)GetProcAddress(hDll, "DrawThemeTextEx");
	}

	//�����ı�
	if (pDrawThemeTextEx!=0)
	{
		const UINT uFormat = DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX;

		DTTOPTS dto = { sizeof(DTTOPTS) };
		dto.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE | DTT_TEXTCOLOR;
		dto.iGlowSize = 10;

		string sText = pText; 
		wstring wText = CCodes::ANSIToUnicode(sText);
		pDrawThemeTextEx(htheme, hDC, 9, 0, wText.c_str(), -1,	uFormat, mRect, &dto);
		//::DrawThemeTextEx(htheme, hDC, TEXT_CONTROLLABEL, 0, wText.c_str(), -1,	uFormat, mRect, &dto);
	} 
	else
	{
		const UINT uFormat = DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX;
			
		CFont * pFont = GetCurrFont(itemHwnd);
		HGDIOBJ hOldObj = ::SelectObject(hDC, pFont->GetSafeHandle());
		::SetBkMode(hDC, TRANSPARENT);
		::SetTextColor (hDC, RGB(255,255,255));

		::DrawText(hDC, pText, -1, mRect, uFormat);

		::SelectObject(hDC, hOldObj);
	}

	::EnableThemeDialogTexture(itemHwnd, true);
	
	free(pText);

	return true; 
}

LRESULT CDwmObject::DlgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	//case WM_ERASEBKGND:
	case WM_PAINT:
		DrawTransparencyBack();
		break;
	//case WM_DWMCOMPOSITIONCHANGED:
	//	ExtendFrameIntoClientArea();
	//	break;
	case WM_DRAWITEM://�Ի�ؼ�
		{
			DRAWITEMSTRUCT * pDr = (DRAWITEMSTRUCT *)lParam;
			if (CtrlInDwmFream(m_dlgHwnd, pDr->hwndItem))
				return DrawItem(pDr->hwndItem, pDr->hDC);
		}
		break; 
	}

	//����ԭʼ�Ĵ�����
	LRESULT Result = CallWindowProc(m_dlgWndProcOld, hWnd, message, wParam, lParam);
		
	switch (message)
	{
	case WM_SIZE				:
		{
			m_rgnDwmFream = GetDmwFreamRGN(m_dlgHwnd, m_Margins);			
			ExtendFrameIntoClientArea();
		}
		break;
	//case WM_CTLCOLORDLG			:
	//case WM_CTLCOLORMSGBOX		:
	//case WM_CTLCOLOREDIT		: 
	//case WM_CTLCOLORLISTBOX		:
	//case WM_CTLCOLORBTN			:
	//case WM_CTLCOLORSCROLLBAR	:
	//case WM_CTLCOLORSTATIC:
	//	Result = (LRESULT)GetCtrlBrush((HWND)lParam, (HDC)wParam, (HBRUSH)Result);
	//	break;
	}

	return Result;
}