#ifndef _TMEMDC_H
#define _TMEMDC_H
#include <Windows.h>

class TMemDC
{
public:
	TMemDC(HWND hwnd);
	virtual ~TMemDC();
public:
	HDC			GetHdc(){ return m_mdc; }
	HDC			GetDVdc() { return m_hdc; }
	HWND		GetDevice() { return m_hwnd; }
	void			GetSize(SIZE& size);
public:
	void			DotRectangle(const RECT & rect,COLORREF color,DWORD dotwidth);
protected:
	void			CreateMemDC();
private:
	HDC			m_mdc;
	HDC			m_hdc;
	HBITMAP		m_bmp;
private:
	HWND			m_hwnd;
	PAINTSTRUCT	m_ps;
};
#endif