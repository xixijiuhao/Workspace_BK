#pragma once

class CDwmObject
{
private:
	HWND		m_dlgHwnd;				
	MARGINS		m_Margins;
	HBRUSH		m_brBackGnd;
	COLORREF	m_clTransparency;

	HRGN		m_rgnDwmFream;
	
	WNDPROC		m_dlgWndProcOld;
	typedef LRESULT (CDwmObject::*CLASSWNDPROC)(HWND, UINT, WPARAM, LPARAM);
	ACCallback<WNDPROC, CDwmObject, CLASSWNDPROC> m_dlgWndProc;
private:
	bool IsCompositionEnabled();									//系统是否启用毛玻璃效果
	HRGN GetDmwFreamRGN(HWND hwnd, MARGINS margins);				//获得窗口毛玻璃区域
	bool CtrlInDwmFream(HWND parent, HWND child);					//控件是否在毛玻璃区域

	HRESULT ExtendFrameIntoClientArea();							//指定窗口的毛玻璃边界，用在窗体的OnCreate或OnInitDialog事件里
	void DrawTransparencyBack();									//绘制窗口背景，用在窗体的OnParint事件里
	HBRUSH GetCtrlBrush(HWND hWnd, HDC hDC, HBRUSH brDefault);		//绘制控件背景，用在窗体的OnCtrlColor事件里
private:
	void BindWindow(HWND hWnd);										//绑定一个窗口
	void UnBindWindow(HWND hWnd);									//接触对窗口的绑定

	LOGFONT GetSysFont();											//获得当前主题的系统字体
	CFont * GetCurrFont(HWND hWnd);									//获得指定控件的字体

	bool DrawItem(HWND itemHwnd, HDC itemDC);						//绘制窗口内的控件
	LRESULT DlgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//新的窗口过程
public:
	CDwmObject();
	~CDwmObject();
	bool Init(HWND dlgHwnd, MARGINS margins);						//指定窗口和它需要显示玻璃效果的边界
};

/////////////////////////////////// CApiMemDC //////////////////////////////////////////////////////////////////////
class CApiMemDC 
{
private:
    HBITMAP		m_hBitmap;  
	BITMAPINFO	m_DIB;
    HGDIOBJ		m_hOldBitmap;  
    HDC			m_hDC;         
    HDC			m_hMemDC;      
    CRect		m_Rect; 
	BOOL		m_bVReverse;
public:
	CApiMemDC(HDC hDC, BOOL UseAlpha=FALSE, BOOL bVReverse=FALSE)
	{
        ASSERT(hDC != 0);
		m_bVReverse=bVReverse;

		m_hDC = hDC;
        m_hOldBitmap = 0;
		memset(&m_DIB, 0, sizeof(m_DIB));

		::GetClipBox(m_hDC, &m_Rect);
		m_hMemDC = ::CreateCompatibleDC(m_hDC);
		if (UseAlpha)
		{
			m_DIB.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			m_DIB.bmiHeader.biWidth = m_Rect.Width();
			m_DIB.bmiHeader.biHeight = -m_Rect.Height();
			m_DIB.bmiHeader.biPlanes = 1;
			m_DIB.bmiHeader.biBitCount = 32;
			m_DIB.bmiHeader.biCompression = BI_RGB;
			m_hBitmap = ::CreateDIBSection(hDC, &m_DIB, DIB_RGB_COLORS, NULL, NULL, 0);
		}
		else
			m_hBitmap = ::CreateCompatibleBitmap(m_hDC, m_Rect.Width(), m_Rect.Height());

		m_hOldBitmap = ::SelectObject(m_hMemDC, m_hBitmap);
		CPoint point;
        ::SetWindowOrgEx(m_hMemDC, m_Rect.left, m_Rect.top, &point); 
	}
	~CApiMemDC()
	{
		//需要纵向翻转
		if(m_bVReverse)
			::StretchBlt(m_hDC, m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height(), m_hMemDC, 
				m_Rect.left, m_Rect.Height() + m_Rect.top-1, m_Rect.Width(), -m_Rect.Height(), SRCCOPY );
		else
			::BitBlt(m_hDC, m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height(), m_hMemDC, m_Rect.left, m_Rect.top, SRCCOPY );

        ::SelectObject(m_hMemDC, m_hOldBitmap);

		::DeleteDC(m_hMemDC);
		::DeleteObject(m_hBitmap);
	}
	HDC GetDC()
	{
		return m_hMemDC;
	}
	
    // Allow usage as a pointer
    CApiMemDC* operator->() {return this;}
        
    // Allow usage as a pointer
    operator CApiMemDC*() {return this;}
};
