#include "BaseInclude.h"

TMemDC::TMemDC(HWND hwnd)
{
	m_hwnd = hwnd;
	m_hdc = BeginPaint(hwnd, &m_ps);
	SetBkMode(m_hdc, TRANSPARENT);
	CreateMemDC();
}

TMemDC::~TMemDC()
{
	SIZE size;
	GetSize(size);
	BitBlt(m_hdc, 0, 0, size.cx, size.cy, m_mdc, 0, 0, SRCCOPY);
	DeleteDC(m_mdc);
	DeleteObject(m_bmp);
	EndPaint(m_hwnd, &m_ps);
}

void TMemDC::CreateMemDC()
{
	SIZE size;
	GetSize(size);
	m_mdc = CreateCompatibleDC(m_hdc);
	m_bmp = CreateCompatibleBitmap(m_hdc, size.cx, size.cy);
	SelectObject(m_mdc, m_bmp);
}

void TMemDC::GetSize(SIZE& size)
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);
	size.cx = rect.right - rect.left;
	size.cy = rect.bottom - rect.top;

	return;
}
