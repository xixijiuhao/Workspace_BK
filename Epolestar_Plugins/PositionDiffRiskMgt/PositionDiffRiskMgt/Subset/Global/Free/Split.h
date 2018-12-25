#pragma once

#include "afxext.h"

class TSplitterWnd: public CSplitterWnd
{
public:
	TSplitterWnd()
	{
		CSplitterWnd::CSplitterWnd();

		// default splitter box/bar sizes (includes borders)
		m_cxSplitter = m_cySplitter = 3;
		m_cxBorderShare = m_cyBorderShare = 0;
		m_cxSplitterGap = m_cySplitterGap = 3;
		m_cxBorder = m_cyBorder = 1;
	}

	void OnDrawSplitter(CDC* pDC, ESplitType nType,	const CRect& rectArg)
	{
		// if pDC == NULL, then just invalidate
		if (pDC == NULL)
		{
			RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
			return;
		}
		ASSERT_VALID(pDC);

		// otherwise, actually draw
		CRect rect = rectArg;
		switch (nType)
		{
		case splitBorder:
			if (m_cxBorder>0 && m_cyBorder>0)
			{
				pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNFACE));
				rect.InflateRect(-m_cxBorder, -m_cyBorder);
				pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNFACE));
			}
			return;
		case splitBox:
			if (m_cxBorder>0 && m_cyBorder>0)
			{
				pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNFACE));
				rect.InflateRect(-m_cxBorder, -m_cyBorder);
				pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNFACE));
				rect.InflateRect(-m_cxBorder, -m_cyBorder);
			}
			break;
		case splitIntersection:
		case splitBar:
			break;
		default:
			ASSERT(FALSE);  // unknown splitter type
		}

		COLORREF clr = GetSysColor(COLOR_BTNFACE);
		pDC->FillSolidRect(rect, clr);
	}
};