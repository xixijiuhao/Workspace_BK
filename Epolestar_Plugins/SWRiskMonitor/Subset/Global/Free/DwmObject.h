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
	bool IsCompositionEnabled();									//ϵͳ�Ƿ�����ë����Ч��
	HRGN GetDmwFreamRGN(HWND hwnd, MARGINS margins);				//��ô���ë��������
	bool CtrlInDwmFream(HWND parent, HWND child);					//�ؼ��Ƿ���ë��������

	HRESULT ExtendFrameIntoClientArea();							//ָ�����ڵ�ë�����߽磬���ڴ����OnCreate��OnInitDialog�¼���
	void DrawTransparencyBack();									//���ƴ��ڱ��������ڴ����OnParint�¼���
	HBRUSH GetCtrlBrush(HWND hWnd, HDC hDC, HBRUSH brDefault);		//���ƿؼ����������ڴ����OnCtrlColor�¼���
private:
	void BindWindow(HWND hWnd);										//��һ������
	void UnBindWindow(HWND hWnd);									//�Ӵ��Դ��ڵİ�

	LOGFONT GetSysFont();											//��õ�ǰ�����ϵͳ����
	CFont * GetCurrFont(HWND hWnd);									//���ָ���ؼ�������

	bool DrawItem(HWND itemHwnd, HDC itemDC);						//���ƴ����ڵĿؼ�
	LRESULT DlgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//�µĴ��ڹ���
public:
	CDwmObject();
	~CDwmObject();
	bool Init(HWND dlgHwnd, MARGINS margins);						//ָ�����ں�����Ҫ��ʾ����Ч���ı߽�
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
		//��Ҫ����ת
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
