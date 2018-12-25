// THeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "xHeaderCtrl.h"


THeaderCtrl::THeaderCtrl()
{
	m_bUseHideCursor = FALSE;
}

THeaderCtrl::~THeaderCtrl()
{
	m_aColum.RemoveAll();
}


BEGIN_MESSAGE_MAP(THeaderCtrl, CHeaderCtrl)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


BOOL THeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return m_bUseHideCursor ? 
		TRUE : CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
}

void THeaderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	HD_HITTESTINFO hd_hittestinfo;
	hd_hittestinfo.pt = point;
	BOOL bFind = FALSE;
	SendMessage(HDM_HITTEST, 0, (LPARAM)(&hd_hittestinfo));
	if(hd_hittestinfo.flags == HHT_ONDIVOPEN) {
		TRACE("Divider zero! item index=%d(x:%d, y:%d)\n", hd_hittestinfo.iItem, point.x, point.y);
	}
	if( hd_hittestinfo.iItem>0 && hd_hittestinfo.iItem<GetItemCount() && 
		m_aColum[hd_hittestinfo.iItem].bHide && hd_hittestinfo.flags == HHT_ONDIVOPEN )
	{
		bFind = TRUE;
		TRACE("Divider zero(this item)! item index=%d(x:%d, y:%d)\n", hd_hittestinfo.iItem, point.x, point.y);
	}
	m_bUseHideCursor = bFind;
	CHeaderCtrl::OnMouseMove(nFlags, point);
}


BOOL THeaderCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	NMHDR* pnmh = (NMHDR*)lParam;
	//TRACE("code=%d\n", pnmh->code);
	//注意：下面不能使用 pnmh->code==HDN_BEGINTRACK
	if (pnmh->code==HDN_BEGINTRACKA || pnmh->code==HDN_BEGINTRACKW) {
		if(m_bUseHideCursor) return *pLResult=TRUE; 
	}

	return CHeaderCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

