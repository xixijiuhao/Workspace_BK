#ifndef _TMEMDC_H
#define _TMEMDC_H

class TMemDC
{
public:
	TMemDC(HWND hwnd, bool bauto = true);
	virtual ~TMemDC();
public:
	HDC				GetHdc(){ return m_mdc; }
	void			GetSize(SIZE& size);
	HDC				GetDVdc(){ return m_hdc; }
	HWND			GetDevice() { return m_hwnd; }
public:
	void			DotRectangle(const RECT & rect,COLORREF color,DWORD dotwidth);
	//static   void	DrawPng(Graphics * gc, UINT nid, int nxpos, int nypos);
	static	 void	DrawMultLineText(HDC pDC, RECT rect, int nRowDis, UINT nFromat, wchar_t* sztext);
protected:
	void			CreateMemDC();
private:
	HDC			m_mdc;
	HDC			m_hdc;
	HBITMAP		m_bmp;
private:
	HWND		m_hwnd;
	bool		m_bauto;
	PAINTSTRUCT	m_ps;
};
#endif