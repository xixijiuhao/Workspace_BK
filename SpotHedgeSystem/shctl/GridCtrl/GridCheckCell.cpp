#include "PreInclude.h"

#define CHECKBOX_WIDTH			15
#define CHECKBOX_LEFTGAP		5

extern G_COLOREF			g_ColorRefData;				//É«²Ê

TGridCheckCell::TGridCheckCell()
{

}

TGridCheckCell::~TGridCheckCell()
{

}

bool TGridCheckCell::ClickCell(const RECT& rect, const POINT& pt, HWND hwnd /*= NULL*/)
{
	RECT rctemp = rect;
	rctemp.left += CHECKBOX_LEFTGAP;
	rctemp.top = rect.top + (rect.bottom - rect.top - CHECKBOX_WIDTH) / 2;
	rctemp.bottom = rctemp.top + CHECKBOX_WIDTH;
	rctemp.right = rctemp.left + CHECKBOX_WIDTH;

	ConfigData* pdata = (ConfigData*)m_pitem->exdata;
	if (pdata&&PtInRect(&rctemp, pt))
	{
		pdata->bshow = !pdata->bshow;
		return true;
	}

	return false;
}

void TGridCheckCell::DrawMain(TMemDC* pmemdc, const RECT& rcmain)
{
	RECT rect = rcmain;
//	__super::DrawMain(pmemdc, rect);
	if (rect.right - rect.left <= 0) return;

//	DrawFix(pmemdc, rect);

	HBRUSH holdbr = (HBRUSH)SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_BRUSH));
	HPEN hpen = CreatePen(PS_SOLID, 2, g_ColorRefData.GetColorGray());
	SelectObject(pmemdc->GetHdc(), hpen);

	RECT rctemp = rect;
	rctemp.left += CHECKBOX_LEFTGAP;
	rctemp.top = rect.top + (rect.bottom - rect.top - CHECKBOX_WIDTH) / 2;
	rctemp.bottom = rctemp.top + CHECKBOX_WIDTH;
	rctemp.right = rctemp.left + CHECKBOX_WIDTH;

	Rectangle(pmemdc->GetHdc(), rctemp.left, rctemp.top, rctemp.right, rctemp.bottom);

	
	ConfigData* pdata = (ConfigData*)m_pitem->exdata;
	if (pdata&&pdata->bshow)
	{
		InflateRect(&rctemp, -2, -2);

		POINT pp[3];
		pp[0].x = rctemp.left + 1;
		pp[0].y = rctemp.top + (rctemp.bottom - rctemp.top) / 2;
		pp[1].x = pp[0].x + 2;
		pp[1].y = rctemp.bottom - 2;
		pp[2].x = rctemp.right - 1;
		pp[2].y = rctemp.top + 2;

		MoveToEx(pmemdc->GetHdc(), pp[0].x, pp[0].y, NULL);
		LineTo(pmemdc->GetHdc(), pp[1].x, pp[1].y);
		MoveToEx(pmemdc->GetHdc(), pp[1].x, pp[1].y, NULL);
		LineTo(pmemdc->GetHdc(), pp[2].x, pp[2].y);

		InflateRect(&rctemp, 2, 2);
	}

	SelectObject(pmemdc->GetHdc(), holdbr);
	DeleteObject(hpen);
}

void TGridCheckCell::DrawFix(TMemDC* pmemdc, const RECT& rect)
{
	COLORREF clr = RGB(70, 70, 70);
	HPEN hpen = CreatePen(PS_SOLID, 1, clr);
	SelectObject(pmemdc->GetHdc(), hpen);

	HBRUSH hbr = CreateSolidBrush(GetBkColor());

	RECT rctemp = rect;
	rctemp.right = rctemp.left + CHECKBOX_LEFTGAP + CHECKBOX_WIDTH + 2;
	FillRect(pmemdc->GetHdc(), &rctemp, hbr);

	if (1)
	{
		RECT rctemp = rect;
		rctemp.right += 1;
		rctemp.bottom += 1;
		HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0)/*m_clrline*/);
		FrameRect(pmemdc->GetHdc(), &rctemp, hbr);
		DeleteObject(hbr);
	}

	DeleteObject(hpen);
	DeleteObject(hbr);
}