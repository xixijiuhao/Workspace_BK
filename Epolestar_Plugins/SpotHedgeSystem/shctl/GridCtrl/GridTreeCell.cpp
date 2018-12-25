#include "PreInclude.h"


bool TGridTreeCell::ClickCell(const RECT& rect, const POINT& pt, HWND hwnd/* = NULL*/)
{
	if (IsLeaf()) return false;

	RECT rcexpand; 
	InitExpand(rect, rcexpand);

	//·ÇÒ¶½Úµã
	GridExpand*	pexdata = (GridExpand*)m_pitem->exdata;
	if (PtInRect(&rcexpand, pt) && pexdata)
	{
		pexdata->bexpand = !pexdata->bexpand;
		::PostMessage(GetParent(hwnd), SSWM_TREE_EXPAND, 0, 0);
	}
	return true;
}

void TGridTreeCell::DrawMain(TMemDC* pmemdc, const RECT& rect)
{
 	RECT rctree, rctxt;
	InitExpand(rect,rctree);
	rctxt = rect;

	bool bleaf = IsLeaf();
	if (!bleaf&&GetLeafCount() > 0)
	{
		SelectObject(pmemdc->GetHdc(), (HPEN)GetStockObject(BLACK_PEN));
		MoveToEx(pmemdc->GetHdc(), rctree.left + 2, rctree.top + m_treebtn / 2, NULL);
		LineTo(pmemdc->GetHdc(), rctree.right - 2, rctree.top + m_treebtn / 2);
		if (!IsExpand())
		{
 			MoveToEx(pmemdc->GetHdc(), rctree.left + m_treebtn / 2, rctree.top + 2,NULL);
 			LineTo(pmemdc->GetHdc(), rctree.left + m_treebtn / 2, rctree.bottom - 2);
		}

		FrameRect(pmemdc->GetHdc(), &rctree, (HBRUSH)GetStockObject(BLACK_BRUSH));
		rctxt.left = rctree.right + 3;
	}
// 	else if(bleaf)
// 	{
// 		rctree.left += m_leftgap;
// 		rctree.right = rctree.left + m_treebtn+4;
// 		rctxt.left = rctree.right + 3;
// 	}

	__super::DrawMain(pmemdc, rctxt);
}

void TGridTreeCell::InitExpand(const RECT& rect,RECT& rcexpand)
{
	rcexpand.left = rect.left + m_leftgap;
	rcexpand.right = rcexpand.left + m_treebtn;
	rcexpand.top = rect.top + (_Height(rect) - m_treebtn) / 2;
	rcexpand.bottom = rcexpand.top + m_treebtn;
}

bool TGridTreeCell::IsExpand()
{
	GridExpand*	pexdata = (GridExpand*)m_pitem->exdata;
	if (!pexdata) return false;
	return pexdata->bexpand;
}

bool TGridTreeCell::IsLeaf()
{
	GridExpand*	pexdata = (GridExpand*)m_pitem->exdata;
	if (!pexdata) return true;
	return pexdata->bleaf;
}

int TGridTreeCell::GetLeafCount()
{
	GridExpand*	pexdata = (GridExpand*)m_pitem->exdata;
	if (!pexdata) return 0;
	return pexdata->ncount;
}