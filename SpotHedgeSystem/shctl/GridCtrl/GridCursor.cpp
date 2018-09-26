#include "PreInclude.h"

TGridCursor::TGridCursor()
{

}

TGridCursor::~TGridCursor()
{
	DeleteObject(m_hbitmap);
}

bool TGridCursor::Create(HWND hparent)
{
	__super::Create(hparent);
	SetWindowLong(m_Hwnd, GWL_EXSTYLE, GetWindowLong(m_Hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_Hwnd, 0, 175, LWA_ALPHA);
	return true;
}
//Msg
void TGridCursor::SetBitmap(HBITMAP hbmp, POINT* pt)
{
	if (hbmp)
	{
		DeleteObject(m_hbitmap);
		m_hbitmap = hbmp;

		BITMAP bm;
		GetObject(hbmp, sizeof(bm), &bm);
		m_size.cx = bm.bmWidth;
		m_size.cy = bm.bmHeight;
	}
	else if (pt)
		MoveWindow(pt->x, pt->y, m_size.cx, m_size.cy);
}

//Property

void TGridCursor::DrawMain(TMemDC* pmemdc)
{
	SIZE size;
	pmemdc->GetSize(size);
	HDC hdc = CreateCompatibleDC(pmemdc->GetDVdc());

	SelectObject(hdc, m_hbitmap);
	BitBlt(pmemdc->GetHdc(), 0, 0, size.cx, size.cy, hdc, 0, 0, SRCCOPY);
	DeleteDC(hdc);
}