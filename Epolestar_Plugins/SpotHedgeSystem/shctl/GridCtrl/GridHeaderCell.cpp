#include "PreInclude.h"

TGridHeaderCell::TGridHeaderCell(TFilterList* plist)
{
	m_plist = plist;
}

TGridHeaderCell::~TGridHeaderCell()
{

}

//
bool TGridHeaderCell::ClickCell(const RECT& rect, const POINT& pt, HWND hwnd/* = NULL*/)
{
	bool bret = false;

	if (!m_pitem->exdata) return bret;
		
	RECT rcfilter;
	InitFilter(rect, rcfilter);
	if (PtInRect(&rcfilter, pt))
	{
		//弹出区域
		POINT ptscreen;
		ptscreen.x = rect.left; ptscreen.y = rect.bottom;
		ClientToScreen(hwnd, &ptscreen);

		int ntype = TFilterList::Filter_List;
		if (m_pitem->emtype == CellType_Edit)
			ntype = TFilterList::Filter_Edit;
		else if (m_pitem->emtype == CellType_EditFilter)
			ntype = TFilterList::Filter_EditList;
		else  if (m_pitem->emtype == CellType_EditFilter1)
			ntype = TFilterList::Filter_EditList_Single;
		//
		if(hwnd)
			SendMessage(GetParent(hwnd), SSWM_SORT_FILTER_DATA, 0, m_pitem->cellid.ncol);
		m_plist->SetData(m_pitem->exdata, m_pitem->cellid,ntype);
		m_plist->_MoveWindow(ptscreen.x, ptscreen.y, TFilterList::ListPos_LeftTop);
		m_plist->ShowWindow(SW_SHOW);
		bret = true;
	}

	return bret;
}

void TGridHeaderCell::DrawMain(TMemDC* pmemdc, const RECT& rcmain)
{
	//资源
	HPEN hpdrop = NULL;
	HBRUSH hbrdrop = NULL;

	//
	RECT rcsort,rcfilter;
	memset(&rcsort, 0, sizeof(RECT));
	InitFilter(rcmain, rcsort);
	rcfilter = rcsort;
	rcsort.top = rcsort.bottom - 13;

	GridSort emsort = m_pitem->emsort;

	//筛选
	if (m_pitem->exdata)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(pmemdc->GetDevice(), &pt);

		BOOL bhoverfilter = PtInRect(&rcfilter, pt);
		if (bhoverfilter)
		{
			HBRUSH hbr = CreateSolidBrush(RGB(140, 143, 150));
			FillRect(pmemdc->GetHdc(), &rcfilter, hbr);
			DeleteObject(hbr);
		}

		bool bhavefilter = IsSetFilter();
		if (bhavefilter)
			hpdrop = CreatePen(PS_SOLID, 1, RGB(150, 171, 204));
		else
		{
			LOGBRUSH logbrush;
			logbrush.lbColor = RGB(150, 171, 204);
			logbrush.lbStyle = PS_SOLID;
			hpdrop = ExtCreatePen(PS_GEOMETRIC | PS_DASHDOT, 1, &logbrush, 0, NULL);
		}
		SelectObject(pmemdc->GetHdc(), hpdrop);
		int nleft, ntop = 0;
		nleft = (_Width(rcsort) - 7) / 2 + rcsort.left - 1;
		if (emsort == Header_Down)
			ntop = rcsort.top + 4;
		else
			ntop = rcsort.top - 4;
		MoveToEx(pmemdc->GetHdc(), nleft, ntop, NULL);
		LineTo(pmemdc->GetHdc(), nleft + 9, ntop);
		DeleteObject(hpdrop);
	} 	
	//排序
	POINT gpt[3];

	if (emsort != Header_Null)
	{
		if (emsort == Header_Down/*Header_Upper*/)
		{
			hbrdrop = CreateSolidBrush(RGB(150, 171, 204));
			SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_PEN));
			SelectObject(pmemdc->GetHdc(), hbrdrop);

			gpt[0].x = (_Width(rcsort) - 7) / 2 + rcsort.left - 2;
			gpt[1].x = rcsort.right - (_Width(rcsort) - 7) / 2+2;
			gpt[2].x = (gpt[0].x + gpt[1].x) / 2;
			gpt[2].y = rcsort.top - 4;
			gpt[1].y = rcsort.top + 2;
			gpt[0].y = rcsort.top + 2;
		}
		else
		{
			hpdrop = CreatePen(PS_SOLID, 1, RGB(150, 171, 204));
			SelectObject(pmemdc->GetHdc(), hpdrop);

			//边框
			if(emsort == Header_Normal)
				SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_BRUSH));
			else
			{
				hbrdrop = CreateSolidBrush(RGB(150, 171, 204));
				SelectObject(pmemdc->GetHdc(), hbrdrop);
			}
			gpt[0].x = (_Width(rcsort) - 7) / 2 + rcsort.left - 1;
			gpt[2].x = rcsort.right - (_Width(rcsort) - 7) / 2;
			gpt[1].x = (gpt[0].x + gpt[2].x) / 2;
			gpt[0].y = rcsort.top - 1;
			gpt[2].y = gpt[0].y;
			gpt[1].y = gpt[0].y + 4;
		}

		Polygon(pmemdc->GetHdc(), gpt, sizeof(gpt) / sizeof(POINT));
	}

	//释放资源
	DeleteObject(hpdrop);
	DeleteObject(hbrdrop);

	RECT rctxt = rcmain;
	if(emsort != Header_Null)
		rctxt.right = rcmain.right - _Width(rcsort);

	__super::DrawMain(pmemdc, rctxt);
}

 //void TGridHeaderCell::DrawMain(TMemDC* pmemdc, const RECT& rcmain)
 //{
 //	RECT rcfilter;
 //	memset(&rcfilter, 0, sizeof(RECT));
 //	if (m_pitem->exdata)
 //	{
 //		InitFilter(rcmain, rcfilter);
 //
 //		POINT pt;
 //		GetCursorPos(&pt);
 //		ScreenToClient(pmemdc->GetDevice(), &pt);
 //
 //		bool bhavefilter = IsSetFilter();
 //		bool bhoverfilter = (bool)PtInRect(&rcfilter, pt);
 //		if (bhoverfilter)
 //		{
 //			HBRUSH hbr = CreateSolidBrush(RGB(140, 143, 150));
 //			FillRect(pmemdc->GetHdc(), &rcfilter, hbr);
 //			DeleteObject(hbr);
 //		}
 //
 //		if (bhavefilter)
 //		{
 //			HBRUSH hbrdrop = CreateSolidBrush(RGB(0, 0, 0));
 //			SelectObject(pmemdc->GetHdc(), hbrdrop);
 //
 //			POINT gpt[3];
 //			gpt[0].x = rcfilter.left + 5;
 //			gpt[0].y = rcfilter.bottom - 11/*rcfilter.top + (rcfilter.bottom - rcfilter.top - 4) / 2*/;
 //			SelectObject(pmemdc->GetHdc(), GetStockObject(BLACK_PEN));
 //			MoveToEx(pmemdc->GetHdc(), gpt[0].x, gpt[0].y, NULL);
 //			LineTo(pmemdc->GetHdc(), rcfilter.right - 5, gpt[0].y);
 //			gpt[0].y += 3;
 //
 //			gpt[2].x = rcfilter.right - 5;
 //			gpt[2].y = gpt[0].y;
 //			gpt[1].x = (gpt[0].x + gpt[2].x) / 2;
 //			gpt[1].y = gpt[0].y + 4;
 //			SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_PEN));
 //			Polygon(pmemdc->GetHdc(), gpt, sizeof(gpt) / sizeof(POINT));
 //			DeleteObject(hbrdrop);
 //		}
 //		else/* if(bhoverfilter)*/
 //		{
 //			HBRUSH hbrdrop = NULL;
 //			if (bhoverfilter)
 //				hbrdrop = CreateSolidBrush(RGB(0, 0, 0));
 //			else
 //				hbrdrop = CreateSolidBrush(RGB(200, 200, 200));
 //			SelectObject(pmemdc->GetHdc(), hbrdrop);
 //			SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_PEN));
 //
 //			POINT gpt[3];
 //			gpt[0].x = rcfilter.left + 5;
 //			gpt[0].y = rcfilter.top + (rcfilter.bottom - rcfilter.top - 4) / 2;
 //			gpt[2].x = rcfilter.right - 5;
 //			gpt[2].y = gpt[0].y;
 //			gpt[1].x = (gpt[0].x + gpt[2].x) / 2;
 //			gpt[1].y = gpt[0].y + 4;
 //			Polygon(pmemdc->GetHdc(), gpt, sizeof(gpt) / sizeof(POINT));
 //			DeleteObject(hbrdrop);
 //		}
 //	}
 //	
 //	RECT rctxt = rcmain;
 //	rctxt.right = rcmain.right - _Width(rcfilter);
 //
 //	//排序
 //	GridSort emsort = m_pitem->emsort;
 //	if (emsort != Header_Normal&&emsort != Header_Null)
 //	{
 //		HBRUSH hbrdrop = NULL;
 //		hbrdrop = CreateSolidBrush(RGB(150, 171, 204));
 //		SelectObject(pmemdc->GetHdc(), hbrdrop);
 //		SelectObject(pmemdc->GetHdc(), GetStockObject(NULL_PEN));
 //
 //		POINT gpt[3];
 //		gpt[0].x = (_Width(rctxt) - 7)/2 + rctxt.left;
 //		if (emsort == Header_Down)
 //		{
 //			gpt[2].x = rctxt.right - (_Width(rctxt) - 7) / 2;
 //			gpt[1].x = (gpt[0].x + gpt[2].x) / 2;
 //			gpt[0].y = rctxt.top + 1;
 //			gpt[2].y = gpt[0].y;
 //			gpt[1].y = gpt[0].y + 4;
 //		}
 //		else
 //		{
 //			gpt[0].x -= 1;
 //			gpt[1].x = rctxt.right - (_Width(rctxt) - 7) / 2 + 1;
 //			gpt[2].x = (gpt[0].x + gpt[1].x) / 2;
 //			gpt[2].y = rctxt.top ;
 //			gpt[1].y = rctxt.top + 5;
 //			gpt[0].y = rctxt.top + 5;
 //		}
 //
 //		rctxt.top += 4;
 //		Polygon(pmemdc->GetHdc(), gpt, sizeof(gpt) / sizeof(POINT));
 //		DeleteObject(hbrdrop);
 //	}
 //
 //	__super::DrawMain(pmemdc, rctxt);
 //}

void TGridHeaderCell::InitFilter(const RECT& rect, RECT& rcfilter)
{
	rcfilter = rect;
	rcfilter.left = rcfilter.right - m_filterwidth;
}

bool TGridHeaderCell::IsSetFilter()
{
	int ntype = TFilterList::Filter_List;
	if (m_pitem->emtype == CellType_Edit)
		ntype = TFilterList::Filter_Edit;
	else if (m_pitem->emtype == CellType_EditFilter)
		ntype = TFilterList::Filter_EditList;
	else  if (m_pitem->emtype == CellType_EditFilter1)
		ntype = TFilterList::Filter_EditList_Single;
	if (m_pitem->emtype != CellType_Edit)
	{
		VFilter* stlfilter = (VFilter*)m_pitem->exdata;
		for (_uint nindex = 0; nindex < stlfilter->size(); nindex++)
		{
			GFilter stfilter = stlfilter->at(nindex);
			if (stfilter.bcheck)
				return true;
		}
	}
	else
	{
		FilterEdit* stlfilter = (FilterEdit*)m_pitem->exdata;
		if (stlfilter&&wcslen(stlfilter->wsztext) > 0)
			return true;
	}
	
	return false;
}