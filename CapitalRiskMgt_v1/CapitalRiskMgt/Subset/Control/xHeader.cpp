#include "stdafx.h"
#include "xHeader.h"
#include "math.h"
#include "MemDC.h"

BEGIN_MESSAGE_MAP(CxHeader, CHeaderCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()


LOGFONT HeaderFont = {-14, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET, 0,0,0,0, _T("Tahoma")};

CxHeader::CxHeader() 
	: m_LogFont(HeaderFont)
	, m_SortItem(0)
	, m_SortType(0)
	, m_Gap(4)
{	
	m_DrawInfo.nFixCol			= 0;
	m_DrawInfo.clrBack			= 0x333333;
	m_DrawInfo.clrSelBack		= 0x333333;
	m_DrawInfo.clrText			= 0xc1c1c1;
	m_DrawInfo.clrSelText		= 0xc1c1c1;
	m_DrawInfo.clrLine			= 0x111111;
	m_DrawInfo.bShowHorzLine	= FALSE;
	m_DrawInfo.bShowVertLine	= TRUE;
	m_DrawInfo.bShowLeftLine	= FALSE;
	m_DrawInfo.bShowTopLine		= FALSE;
	m_DrawInfo.nVertLineMargin	= 2;
	m_DrawInfo.nHeight			= 24;
}

void CxHeader::SetFont(CFont * cFont, BOOL bRedraw)
{
	if (!cFont)
		return;

	cFont->GetLogFont(&m_LogFont);
	CHeaderCtrl::SetFont(cFont, bRedraw);
}

CFont * CxHeader::GetFont()
{
	m_OutFont.DeleteObject();
	m_OutFont.CreateFontIndirect(&m_LogFont);
	return &m_OutFont;
}

void CxHeader::SetSortIcon(int Column, int Sort)
{
	m_SortItem = Column;
	m_SortType = Sort;

	Invalidate();
}

void CxHeader::SetGap(UINT value)
{
	if (m_Gap==value)
		return;
	m_Gap = value;
	
	Invalidate();
}

int CxHeader::GetItemIndex(POINT pt)
{
	LVHITTESTINFO ht;
	ht.pt = pt;
	ht.flags =   LVHT_ABOVE;
	((CListCtrl *)GetParent())->SubItemHitTest(&ht);
	return ht.iSubItem;
}

BOOL CxHeader::OnChildNotify(UINT msg, WPARAM wp, LPARAM lp, LRESULT* pRes)
{
	//锁定前m_FixCol列的位置		//NMHDR& nmh = *(NMHDR*)lp;
	LPNMHEADER p =(LPNMHEADER)lp;
	if (m_DrawInfo.nFixCol>0)				
	{		
		if ((p->hdr.code == HDN_BEGINDRAG || p->hdr.code == HDN_BEGINTRACK  || p->hdr.code == HDN_ENDTRACK || p->hdr.code == HDN_ENDDRAG) && p->iItem < m_DrawInfo.nFixCol)
			return *pRes = TRUE;
		else if (p->hdr.code == HDN_ENDDRAG && p->pitem->iOrder < m_DrawInfo.nFixCol)
			p->pitem->iOrder = m_DrawInfo.nFixCol;
	}

	return CHeaderCtrl::OnChildNotify(msg, wp, lp, pRes);
}

BOOL CxHeader::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CxHeader::OnPaint()
{	
	CPaintDC dc(this);
	CMemDC MemDC(&dc);
	DoOnPaint(MemDC);
}
void CxHeader::DoOnPaint(CDC & dc)
{	
	CRect rect;
	dc.GetClipBox(&rect);
	dc.FillSolidRect(rect, m_DrawInfo.clrBack);

	HDITEM hdi;
	CRect rcItem;
	CRect rcTemp;

	CFont font;
	font.CreateFontIndirect(&m_LogFont);
	CFont * pOldFont = dc.SelectObject(&font);
		
	CPen penLine(PS_SOLID, 1, m_DrawInfo.clrLine);
	CPen* pOldPen	= dc.SelectObject(&penLine);

	dc.SetBkMode(TRANSPARENT);

	dc.SetTextColor(m_DrawInfo.clrText);
	hdi.mask = HDI_FORMAT;
	for (int i=0; i<GetItemCount(); i++)
	{		
		GetItemRect(i, &rcItem);
		if (rcItem.right < 0 || rcItem.left >= rect.right)
			continue;

		//获得字符串和对齐方式
		hdi.mask = HDI_TEXT | HDI_FORMAT;
		hdi.pszText = m_lpBuffer;
		hdi.cchTextMax = sizeof(m_lpBuffer);
		GetItem(i, &hdi);
		
		UINT UFmt = DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER;// | DT_END_ELLIPSIS //文字默认显示格式
		switch(hdi.fmt & HDF_JUSTIFYMASK)
		{
		case HDF_CENTER	:	UFmt |= DT_CENTER	; break;
		case HDF_LEFT	:	UFmt |= DT_LEFT		; break;
		default			:	UFmt |= DT_RIGHT	; break;
		}
		rcItem.DeflateRect(m_Gap, 0);						//文字离边框的左右边距
		DrawText(dc.m_hDC, m_lpBuffer, strlen(m_lpBuffer), &rcItem, UFmt);
		rcItem.InflateRect(m_Gap, 0);

		//横线
		if (i>=m_DrawInfo.nFixCol && m_DrawInfo.bShowHorzLine)
		{
			rcTemp = rcItem;
			rcTemp.DeflateRect(0,0,0,1);
			dc.MoveTo(rcTemp.left, rcTemp.bottom);
			dc.LineTo(rcTemp.BottomRight());

			if (m_DrawInfo.bShowTopLine)
			{
				dc.MoveTo(rcTemp.TopLeft());
				dc.LineTo(rcTemp.right, rcTemp.top);
			}
		}

		//竖线
		if (i>=m_DrawInfo.nFixCol && m_DrawInfo.bShowVertLine)
		{
			rcTemp = rcItem;
			rcTemp.DeflateRect(0,m_DrawInfo.nVertLineMargin,1,0);
			dc.MoveTo(rcTemp.right, rcTemp.top);
			dc.LineTo(rcTemp.BottomRight());

			if (i==0 && m_DrawInfo.bShowLeftLine)
			{
				dc.MoveTo(rcTemp.TopLeft());
				dc.LineTo(rcTemp.left, rcTemp.bottom);
			}
		}

		//排序图标
		if (i==m_SortItem && m_SortType!=0)
		{
			CPen penSortIcon(PS_SOLID, 1, m_DrawInfo.clrText);
			CPen * pPen = dc.SelectObject(&penSortIcon);

			GetSortIconPoints(rcItem);
			dc.Polyline(m_SortPoints, 4);

			dc.SelectObject(pPen);
		}
	}	
	
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);

	CHeaderCtrl::OnPaint();
}

void CxHeader::GetSortIconPoints(CRect & rect, int topGap, int iconHeight)
{
	if (m_SortType == 0)
		return;

	if (m_SortType > 0)
	{
		m_SortPoints[0].x = rect.CenterPoint().x;		
		m_SortPoints[0].y = topGap;		
		m_SortPoints[1].x = rect.CenterPoint().x - iconHeight;		
		m_SortPoints[1].y = topGap + iconHeight;		
		m_SortPoints[2].x = rect.CenterPoint().x + iconHeight;		
		m_SortPoints[2].y = topGap + iconHeight;
	}
	else
	{		
		m_SortPoints[0].x = rect.CenterPoint().x - iconHeight;		
		m_SortPoints[0].y = topGap;		
		m_SortPoints[1].x = rect.CenterPoint().x + iconHeight;		
		m_SortPoints[1].y = topGap;		
		m_SortPoints[2].x = rect.CenterPoint().x;		
		m_SortPoints[2].y = topGap + iconHeight;
	}
	m_SortPoints[3] = m_SortPoints[0];
}

bool CxHeader::SetDrawInfo(HeaderDrawInfo & Info)
{
	UINT n = (UINT)(log(hdiAll + 1.0) / log(2.0));
	for (UINT i = 0; i < n - 1 ; i++)
		switch(Info.nMask & (1 << i))
		{
			case hdiFixCol			: m_DrawInfo.nFixCol		 = Info.nFixCol			; break;
			case hdiTextColor		: m_DrawInfo.clrText		 = Info.clrText			; break;
			case hdiSelTextColor	: m_DrawInfo.clrSelText		 = Info.clrSelText		; break;
			case hdiBackColor		: m_DrawInfo.clrBack		 = Info.clrBack			; break;
			case hdiSelBackColor	: m_DrawInfo.clrSelBack		 = Info.clrSelBack		; break;

			case hdiLineColor		: m_DrawInfo.clrLine		 = Info.clrLine			; break;
			case hdiShowHorzLine	: m_DrawInfo.bShowHorzLine   = Info.bShowHorzLine	; break;
			case hdiShowVertLine	: m_DrawInfo.bShowVertLine	 = Info.bShowVertLine	; break;
			case hdiShowLeftLine	: m_DrawInfo.bShowLeftLine	 = Info.bShowLeftLine	; break;
			case hdiShowTopLine		: m_DrawInfo.bShowTopLine	 = Info.bShowTopLine	; break;
			case hdiVertLineMargin	: m_DrawInfo.nVertLineMargin = Info.nVertLineMargin	; break;
			case hdiHeight			: m_DrawInfo.nHeight		 = Info.nHeight			; break;
		}		 				
	Invalidate();
	return true;
}

void CxHeader::GetDrawInfo(HeaderDrawInfo & Info)
{
	UINT n = (UINT)(log(hdiAll + 1.0) / log(2.0));
	for (UINT i = 0; i < n - 1 ; i++)
		switch(Info.nMask & (1 << i))
		{
			case hdiFixCol			: Info.nFixCol			= m_DrawInfo.nFixCol		 ; break;
			case hdiTextColor		: Info.clrText			= m_DrawInfo.clrText		 ; break;
			case hdiSelTextColor	: Info.clrSelText		= m_DrawInfo.clrSelText		 ; break;
			case hdiBackColor		: Info.clrBack			= m_DrawInfo.clrBack		 ; break;
			case hdiSelBackColor	: Info.clrSelBack		= m_DrawInfo.clrSelBack		 ; break;
				
			case hdiLineColor		: Info.clrLine			= m_DrawInfo.clrLine		; break;
			case hdiShowHorzLine	: Info.bShowHorzLine	= m_DrawInfo.bShowHorzLine	; break;
			case hdiShowVertLine	: Info.bShowVertLine	= m_DrawInfo.bShowVertLine	; break;
			case hdiShowLeftLine	: Info.bShowLeftLine	= m_DrawInfo.bShowLeftLine	; break;
			case hdiShowTopLine		: Info.bShowTopLine		= m_DrawInfo.bShowTopLine	; break;
			case hdiVertLineMargin	: Info.nVertLineMargin	= m_DrawInfo.nVertLineMargin; break;
			case hdiHeight			: Info.nHeight			= m_DrawInfo.nHeight		; break;
		}				
}
