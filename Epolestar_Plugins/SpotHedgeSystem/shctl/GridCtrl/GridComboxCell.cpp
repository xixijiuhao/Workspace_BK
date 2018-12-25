#include "PreInclude.h"

TGridComboxCell::TGridComboxCell(TFilterList* plist)
{
	m_plist = plist;
}

TGridComboxCell::~TGridComboxCell()
{

}

bool TGridComboxCell::ClickCell(const RECT& rect, const POINT& pt, HWND hwnd/* = NULL*/)
{
	bool bret = false;

	if (!m_pitem->exdata) return bret;

	if (PtInRect(&rect, pt))
	{
		//µ¯³öÇøÓò
		POINT ptscreen;
		ptscreen.x = rect.left; ptscreen.y = rect.bottom;
		ClientToScreen(hwnd, &ptscreen);

		m_plist->SetData(m_pitem->exdata, m_pitem->cellid, TFilterList::Filter_Combox);
		m_plist->_MoveWindow(ptscreen.x, ptscreen.y, TFilterList::ListPos_LeftTop);
		m_plist->ShowWindow(SW_SHOW);
		bret = true;
	}

	return bret;
}

void TGridComboxCell::DrawMain(TMemDC* pmemdc, const RECT& rcmain)
{
	__super::DrawMain(pmemdc, rcmain);
}