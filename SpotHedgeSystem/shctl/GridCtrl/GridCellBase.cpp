#include "PreInclude.h"

TGridCellBase::TGridCellBase()
{
	m_pitem = NULL;
	m_clrhigh = RGB(71, 70, 70)/*(87, 168, 250)*/;
}

TGridCellBase::~TGridCellBase()
{

}

//
void TGridCellBase::Draw(TMemDC* pmemdc, const RECT& rect)
{
	if (!m_pitem) return;
	
	//绘制区域
	RECT rcarea = rect;

	if (_Width(rcarea) <= 0) return;

	//资源创建
	HPEN linepen = CreatePen(PS_SOLID, 1, RGB(165, 165, 165));
	HBRUSH bkbrush = CreateSolidBrush(m_pitem->clrbk);

	//绘制表格线
	SelectObject(pmemdc->GetHdc(), linepen);
	rcarea.bottom -= 1;
	FillRect(pmemdc->GetHdc(), &rcarea, bkbrush);
	rcarea.top += 1;

	if (m_pitem->b_top)
	{
		MoveToEx(pmemdc->GetHdc(), rcarea.left, rcarea.top - 2, NULL);
		LineTo(pmemdc->GetHdc(), rcarea.right, rcarea.top - 2);
	}
	if (m_pitem->b_bottom)
	{
		MoveToEx(pmemdc->GetHdc(), rcarea.left, rcarea.bottom, NULL);
		LineTo(pmemdc->GetHdc(), rcarea.right, rcarea.bottom);
	}
	if (m_pitem->b_left)
	{
		MoveToEx(pmemdc->GetHdc(), rcarea.left, rcarea.top, NULL);
		LineTo(pmemdc->GetHdc(), rcarea.left, rcarea.bottom);
	}
	if (m_pitem->b_right)
	{
		MoveToEx(pmemdc->GetHdc(), rcarea.right - 1, rcarea.bottom, NULL);
		LineTo(pmemdc->GetHdc(), rcarea.right - 1, rcarea.top - 2);
	}


	rcarea.top -= 1;
	//绘制Cell 区域内容
	DrawMain(pmemdc, rcarea);

	//释放资源
	DeleteObject(linepen);
	DeleteObject(bkbrush);
}

void TGridCellBase::DrawMain(TMemDC* pmemdc,const RECT& rcmain)
{
	RECT rctemp = rcmain;
	//FONT
	HFONT hfont = CreateFontIndirect(GetFont());
	SelectObject(pmemdc->GetHdc(), hfont);

	if (GetMask()&GVIF_FGCLR)
		SetTextColor(pmemdc->GetHdc(), GetFgColor());

	//绘制选中行
	if (GetState()&GVIS_SELECTED)
	{
		HBRUSH hbr = CreateSolidBrush(m_clrhigh);
		FillRect(pmemdc->GetHdc(), &rcmain, hbr);
		SetTextColor(pmemdc->GetHdc(), RGB(220, 220, 220));
		DeleteObject(hbr);
	}

	//绘制文字
	SetBkMode(pmemdc->GetHdc(), TRANSPARENT);
	InflateRect(&rctemp, -1, 0);
	DWORD dwformat = DT_VCENTER | DT_SINGLELINE;
	if (GetFormat()&DT_CENTER)
		dwformat |= DT_CENTER;
	else if (GetFormat()&DT_RIGHT)
	{
		dwformat |= DT_RIGHT;
		rctemp.right -= 4;
	}
	else
	{
		dwformat |= DT_LEFT;
		rctemp.left += 4;
	}

	DrawWideText(pmemdc->GetHdc(), GetText(), rctemp, dwformat);
//	DrawTextA(pmemdc->GetHdc(), GetText(), strlen(GetText()), &rctemp, dwformat);
	DeleteObject(hfont);
}
