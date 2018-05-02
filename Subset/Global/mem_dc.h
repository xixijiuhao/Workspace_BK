#pragma once

#include "windows.h"
#include "crtdbg.h"


class MemDC
{
private:
	HBITMAP		m_bitmap;
	HGDIOBJ		m_old_bitmap;
	HWND		m_hwnd;
	HDC			m_dc;
	HDC			m_mem_dc;
	RECT		m_rect;
	bool		m_vert_reverse;

	inline int width()
	{
		return m_rect.right - m_rect.left;
	}
	inline int height()
	{
		return m_rect.bottom - m_rect.top;
	}
public:
	MemDC(HWND hwnd, RECT * rect = nullptr, bool vert_reverse = false, bool use_alpha = false)
	{
		_ASSERT(hwnd);
		m_hwnd = hwnd;
		m_dc = ::GetDC(m_hwnd);
		m_vert_reverse = vert_reverse;

		if (rect)
			m_rect = *rect;
		else
			::GetClipBox(m_dc, &m_rect);

		if (use_alpha)
		{
			BITMAPINFO  dib;
			memset(&dib, 0, sizeof(dib));
			dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			dib.bmiHeader.biWidth = width();
			dib.bmiHeader.biHeight = -height();
			dib.bmiHeader.biPlanes = 1;
			dib.bmiHeader.biBitCount = 32;
			dib.bmiHeader.biCompression = BI_RGB;
			m_bitmap = ::CreateDIBSection(m_dc, &dib, DIB_RGB_COLORS, nullptr, nullptr, 0);
		}
		else
			m_bitmap = ::CreateCompatibleBitmap(m_dc, width(), height());

		m_mem_dc = ::CreateCompatibleDC(m_dc);
		m_old_bitmap = ::SelectObject(m_mem_dc, m_bitmap);

		POINT point;
		::SetWindowOrgEx(m_mem_dc, m_rect.left, m_rect.top, &point);
	}

	~MemDC()
	{
		if (m_vert_reverse)
			::StretchBlt(m_dc, m_rect.left, m_rect.top, width(), height(), m_mem_dc, m_rect.left, height() + m_rect.top - 1, width(), -height(), SRCCOPY);
		else
			::BitBlt(m_dc, m_rect.left, m_rect.top, width(), height(), m_mem_dc, m_rect.left, m_rect.top, SRCCOPY);

		::SelectObject(m_mem_dc, m_old_bitmap);

		::DeleteObject(m_bitmap);
		::DeleteDC(m_mem_dc);

		::ReleaseDC(m_hwnd, m_dc);
	}

	HDC dc()  { return m_mem_dc; }
};