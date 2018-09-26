#include "PreInclude.h"

TMemDC::TMemDC(HWND hwnd, bool bauto/* = true*/)
{
	m_hwnd = hwnd;
	m_bauto = bauto;
	m_hdc = BeginPaint(hwnd, &m_ps);
	SetBkMode(m_hdc, TRANSPARENT);
	CreateMemDC();
}

TMemDC::~TMemDC()
{
	if (m_bauto)
	{
		SIZE size;
		GetSize(size);
		BitBlt(m_hdc, 0, 0, size.cx, size.cy, m_mdc, 0, 0, SRCCOPY);
	}
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

void TMemDC::DotRectangle(const RECT & rect, COLORREF color, DWORD dotwidth)
{
	LOGBRUSH logbrush;
	logbrush.lbColor = color;
	logbrush.lbStyle = PS_SOLID;
	HPEN hpen = ExtCreatePen(PS_GEOMETRIC | PS_DOT, dotwidth, &logbrush, 0, NULL);
	HPEN hold = (HPEN)SelectObject(m_mdc, hpen);
	HBRUSH hbr = (HBRUSH)SelectObject(m_mdc, GetStockObject(NULL_BRUSH));
	Rectangle(m_mdc, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(m_mdc, hold);
	SelectObject(m_mdc, hbr);
	DeleteObject(hpen);
}

//void TMemDC::DrawPng(Graphics* gc, UINT nid, int nxpos, int nypos)
//{
//	HMODULE hmodule = GetModuleHandle(L"TradeDisplay.dll");
//	if (!hmodule) return;
//	HRSRC hRsrc = ::FindResource(hmodule, MAKEINTRESOURCE(nid), L"PNG");
//	if (hRsrc == NULL)
//		return;
//	DWORD dwLen = SizeofResource(hmodule, hRsrc);
//	BYTE* lpRsrc = (BYTE*)LoadResource(hmodule, hRsrc);
//	if (!lpRsrc)
//		return;;
//	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, dwLen);
//	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
//	memcpy(pmem, lpRsrc, dwLen);
//	IStream* pstm;
//	CreateStreamOnHGlobal(m_hMem, FALSE, &pstm);
//	Image img(pstm);
//
//	gc->DrawImage(&img, nxpos, nypos, img.GetWidth(), img.GetHeight());
//
//	GlobalUnlock(m_hMem);
//	pstm->Release();
//	GlobalFree(m_hMem);
//	FreeResource(lpRsrc);
//}

void TMemDC::DrawMultLineText(HDC pDC, RECT rect, int nRowDis, UINT nFromat, wchar_t* sztext)
{
	if (wcslen(sztext) <= 0)
		return;

	WCHAR* pText = sztext;
	int nCount = wcslen(sztext);
	RECT rtChar;
	SIZE size;
	GetTextExtentPoint32(pDC, pText, 1, &size);
	int nRowHeight = size.cy + nRowDis;
	rtChar.top = rect.top;
	rtChar.left = rect.left;
	rtChar.bottom = rtChar.top + nRowDis + size.cy;
	rtChar.right = rtChar.left + size.cx;
	std::wstring strChar;
	for (int nCharIndex = 0; nCharIndex < nCount; nCharIndex++)
	{
		if (rtChar.right > rect.right || strChar == std::wstring(L"\n"))
		{
			rtChar.top = rtChar.bottom;
			rtChar.bottom += nRowHeight;

			GetTextExtentPoint32(pDC, pText + nCharIndex, 1, &size);
			rtChar.left = rect.left;
			rtChar.right = rtChar.left + size.cx;
			if (rtChar.bottom > rect.bottom)
				break;
		}
		strChar = pText[nCharIndex];

		DrawText(pDC, strChar.c_str(), strChar.length(), &rtChar, nFromat);
		//pDC->DrawText(strChar, rtChar, nFromat);

		GetTextExtentPoint32(pDC, pText + nCharIndex + 1, 1, &size);
		rtChar.left = rtChar.right;
		rtChar.right += size.cx;
	}
}