// TabBeam.cpp : 实现文件
//

#include "stdafx.h"
#include "TabBeam.h"
#include "string_ex.h"


// CTabBeam


#define GX_MOVERECORD_FIRST 0
#define GX_MOVERECORD_PREV  1
#define GX_MOVERECORD_NEXT  2
#define GX_MOVERECORD_LAST  3
#define GX_MOVERECORD_SEEK  4

// Scrolling
const int GX_SCRLMOSTLEFT  = 0;
const int GX_SCRLLEFT  = 1;
const int GX_SCRLRIGHT = 2;
const int GX_SCRLMOSTRIGHT = 3;

// Colors for Arrows

/////////////////////////////////////////////////////////////////////////////
// CGXTabInfo

CGXTabInfo::CGXTabInfo()
{
	pExtra = 0;
	bSel = FALSE;
	nWidth = 60;
	bDeleteExtra = FALSE;

	// Scrollbar Info
	memset(&sbInfoVert, 0, sizeof(SCROLLINFO));
	memset(&sbInfoHorz, 0, sizeof(SCROLLINFO));
}

CGXTabInfo::~CGXTabInfo()
{
	if (bDeleteExtra)
		delete pExtra;
}


IMPLEMENT_DYNAMIC(CTabBeam, CWnd)
/////////////////////////////////////////////////////////////////////////////
// CTabBeam

BEGIN_MESSAGE_MAP(CTabBeam, CWnd)
	//{{AFX_MSG_MAP(CTabBeam)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_WININICHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTabBeam::CTabBeam()
	: m_nHeight(GetSystemMetrics(SM_CYHSCROLL))
	, m_nButtonWidth(m_nHeight)
	, m_nWidthEx(8)

	, m_nTabs(0)
	, m_nCurrent(-1)
	, m_nFirstTab(0)
	, m_pNotifyWnd(nullptr)
	, m_hcurArrow(::LoadCursor(nullptr, IDC_ARROW))

	, m_BtnStyle(bsAuto)
	, m_BtnAlient(baLeft)
	, m_dwStyle(tsTrapezoid)

	, m_nHightBarHeight(0)
	, m_rgbHightBar(RGB(0,0,255))
	, m_rgbTextCurrent(::GetSysColor(COLOR_BTNTEXT))
	, m_rgbTextNormal(::GetSysColor(COLOR_BTNTEXT))
	, m_rgbFaceNormal(::GetSysColor(COLOR_WINDOW))
	, m_rgbFaceSel(::GetSysColor(COLOR_BTNFACE))
	, m_rgbBackground(::GetSysColor(COLOR_BTNFACE))	//非标签部分的填充
	, m_rgbTabBorder(RGB(130,135,144))				//标签边线
	, m_rgbDisableBtn(RGB(96,96,96))				//无效按钮的图标颜色
	, m_nDockLineWidth(1)
	, m_rgbDockLine(RGB(130,135,144))
	, m_nSpace(5)
	//, m_pFontNormal(nullptr)
	//, m_pFontCurrent(nullptr)
{
	GetDefaultFont();
}

BOOL CTabBeam::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, TabStyle tabStyle, TBtnStyle btnStyle, TBtnAlient btnAlient)
{
	m_dwStyle= tabStyle;
	if ( CWnd::Create(nullptr, nullptr, dwStyle | WS_CHILD, rect, pParentWnd, nID) )
	{
		SetWindowPos(nullptr, 0, 0, 1, m_nHeight,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

		if (tabStyle==tsRectangle)
		{
			m_nWidthEx = 6;
			m_nHightBarHeight = 2;

			m_lfontNormal.lfHeight  = m_nHeight-4;
			m_lfontCurrent.lfHeight = m_nHeight-4;
			m_lfontCurrent.lfWeight = FW_NORMAL;
		}
		else
		{
			m_rgbTextCurrent = RGB(255,0,0);
			m_rgbHightBar = m_rgbTabBorder;
		}

		DisplayScrollBtns(btnStyle, btnAlient);

		return TRUE;
	}

	return FALSE;
}

CTabBeam::~CTabBeam()
{
	for (int i = 0; i < m_nTabs; i++)
		delete m_TabInfo[i];
}

void CTabBeam::GetDefaultFont()
{
	// use "Arial" as default font
	memset(&m_lfontNormal, 0, sizeof(LOGFONT));
	m_lfontNormal.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	m_lfontNormal.lfQuality = DEFAULT_QUALITY;
	m_lfontNormal.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;
	strncpy_s(m_lfontNormal.lfFaceName, _T("Arial"));

#if _MFC_VER >= 0x0400
	if (GetSystemMetrics(SM_DBCSENABLED))
	{
		// Win95/4.0 only supports DEFAULT_GUI_FONT.  Using this
		// font makes it more easy to port the app to
		// international markets
		// Load default system font
		CFont font;
		font.CreateStockObject(DEFAULT_GUI_FONT);
		if (font.GetSafeHandle() == 0)
			font.CreateStockObject(SYSTEM_FONT);

		// Retrieve logical font information
		if (font.GetSafeHandle())
			font.GetObject(sizeof(m_lfontNormal), &m_lfontNormal);
	}
#endif

	m_lfontNormal.lfCharSet = DEFAULT_CHARSET;//GXGetFontState()->GetFontCharset(GXGetFontState()->LookupFontInfo(m_lfontNormal.lfFaceName));//xie
	m_lfontNormal.lfItalic = 0;
	m_lfontNormal.lfUnderline = 0;
	m_lfontNormal.lfHeight = m_nHeight-2;	// font size based on tab beam height

	// Create the normal font
	m_lfontNormal.lfWeight = FW_NORMAL;

	m_lfontCurrent = m_lfontNormal;
	m_lfontCurrent.lfWeight = FW_BOLD;
}

void CTabBeam::CreateFonts()
{
	if (m_fontNormal.GetSafeHandle())
		m_fontNormal.DeleteObject();
	m_fontNormal.CreateFontIndirect(&m_lfontNormal);

	if (m_fontCurrent.GetSafeHandle())
		m_fontCurrent.DeleteObject();
	m_fontCurrent.CreateFontIndirect(&m_lfontCurrent);
}

void CTabBeam::SetTabBkColor(int nTab, COLORREF cr)
{
	CGXTabInfo *pInfo = &GetTab(nTab);

	pInfo->rgbFaceNormal = cr;
	pInfo->brFaceNormal.DeleteObject();
	pInfo->brFaceNormal.CreateSolidBrush(cr);

	// set flagUseOwnColorFaceNormal to be true
	pInfo->flagUseOwnColorFaceNormal = 1;
}

void CTabBeam::SetTabSelBkColor(int nTab, COLORREF cr)
{
	CGXTabInfo *pInfo = &GetTab(nTab);

	pInfo->rgbFaceSel = cr;
	pInfo->brFaceSel.DeleteObject();
	pInfo->brFaceSel.CreateSolidBrush(cr);

	// set flagUseOwnColorFaceSel to be true
	pInfo->flagUseOwnColorFaceSel = 1;
}

void CTabBeam::SetTabMultiOption(int nTab, BOOL bMul)
{
	CGXTabInfo *pInfo = &GetTab(nTab);
	pInfo->bMul = bMul;
}

BOOL CTabBeam::GetTabMultiOption(int nTab)
{
	CGXTabInfo *pInfo = &GetTab(nTab);
	return pInfo->bMul;
}

void CTabBeam::DisplayScrollBtns(TBtnStyle btnStyle, TBtnAlient btnAlient)
{
	m_BtnStyle	= btnStyle;
	m_BtnAlient	= btnAlient;

	m_nMargin	= GetBtnCount() * m_nButtonWidth;
	Invalidate(FALSE);
}

void CTabBeam::SetNotifyWnd(CWnd* pNotify)
{
	m_pNotifyWnd = pNotify;
}

// CTabBeam Tabs

// Insert Tab or append Tab if bInsert is FALSE
// pExtra can be used for CWnd-Object
// bDelExtra must not be TRUE for a CWnd, because a CWnd normally gets destroyed itself
// (bDelExtra should be TRUE for buffers etc., which should get
// destroyed when the Tab gets destroyed)

void CTabBeam::InsertTab(BOOL bInsert, LPCTSTR szLabel, CObject* pExtra, BOOL bDelExtra, int nWidth, HMENU hMenu, BOOL bMul)
{
	CGXTabInfo* pInfo = new CGXTabInfo;
	pInfo->sLabel = szLabel;
	pInfo->nWidth = nWidth;
	pInfo->pExtra = pExtra;
	pInfo->bDeleteExtra = bDelExtra;
	pInfo->hMenu = hMenu;
	pInfo->bMul = bMul;
	// Use the CTabBeam's default color and brush
	pInfo->flagUseOwnColorFaceNormal= 0;
	pInfo->flagUseOwnColorFaceSel=  0;

	// Initialize the color and brush anyway though
	// they are not used. This is to prevent some user go to
	// set the color flags to true in CGXTabInfo directly
	// and get unexpected results.
	pInfo->rgbFaceNormal= m_rgbFaceNormal;
	pInfo->rgbFaceSel= m_rgbFaceSel;
	pInfo->brFaceNormal.CreateSolidBrush(pInfo->rgbFaceNormal);
	pInfo->brFaceSel.CreateSolidBrush(pInfo->rgbFaceSel);

	// Add info and adjust the tab size

	if (m_nCurrent == -1 || !bInsert)
	{
		m_TabInfo.SetAtGrow(m_nTabs, pInfo);
		if (nWidth < 0)
			AdjustTabSize(m_nTabs);
	}
	else
	{
		m_TabInfo.InsertAt(m_nCurrent, pInfo, 1);
		if (nWidth < 0)
			AdjustTabSize(m_nCurrent);
	}
	m_nTabs++;

	Invalidate();

	// Make sure tab is visible

	if (m_nCurrent == -1)
	{
		m_nCurrent = m_nTabs-1;
		ScrollTabInView(m_nCurrent);
	}
}

void CTabBeam::DeleteTab(int nTab)
{
	if (m_nCurrent == nTab)
	{
		m_nCurrent = 0;
		m_nFirstTab = 0;
	}

	delete m_TabInfo[nTab];
	m_TabInfo.RemoveAt(nTab);
	m_nTabs--;

	if (m_nFirstTab >= m_nTabs)
		m_nFirstTab = max(0, m_nTabs-1);
	if (m_nCurrent>m_nTabs)
		m_nCurrent=m_nTabs-1;

	Invalidate();
}

CGXTabInfo& CTabBeam::GetTab(int nTab)
{
	ASSERT(nTab >= 0 && nTab < m_TabInfo.GetSize());
	return *((CGXTabInfo*) m_TabInfo[nTab]);
}

int CTabBeam::GetBtnCount()
{
	switch(m_BtnStyle)
	{
	case bsShortBtn:	return 2;
	case bsAllBtn:		return 4;
	case bsAuto:
		{
			if (m_hWnd==0)
				return 0;

			CRect rect;
			GetClientRect(rect);
			int nOffset = rect.right;
			if (m_BtnAlient==baRight)
				nOffset = GetBtnOffset();

			if (m_nFirstTab == 0)
			{
				int offset = 5;
				if (m_BtnAlient==baLeft)
					offset += m_nMargin+1;

				int nTab = m_nFirstTab;
				for (;nTab < m_nTabs && offset < nOffset;
					offset += GetTab(nTab++).nWidth + m_nWidthEx)
					;

				if (nTab < m_nTabs || offset > nOffset+5)
					return 2;
				else
					return 0;
			}
			else
				return 2;
		}
	default:	return 0;
	}
}

bool CTabBeam::IsShowBtns()
{
	return GetBtnCount()>0;
}

int CTabBeam::GetBtnOffset()
{
	CRect rect;
	GetClientRect(rect);

	if (m_BtnAlient==baLeft)
		return rect.left;
	else
		return rect.right - m_nMargin;
}
//Btn=-1为整个Btn栏 0，1，2，3
void CTabBeam::GetBtnRect(CRect &rect, int Btn)
{
	if (GetBtnCount()==0 || 
		GetBtnCount()==2 && (Btn==0 || Btn==3))
	{
		rect.left = rect.right = -1;
		return;
	}

	rect.left = GetBtnOffset();
	if (Btn==-1)
		rect.right= rect.left + m_nMargin;
	else
	{
		if (GetBtnCount()==2)
			Btn--;
		rect.left +=  m_nButtonWidth*Btn;
		rect.right= rect.left + m_nButtonWidth;
	}
	rect.top=0;
	rect.bottom=m_nButtonWidth;

	if (m_dwStyle==tsRectangle)
		rect.OffsetRect(0, 1);
	rect.DeflateRect(1, 1);
}
int CTabBeam::GetDrawTabLeft()//tab绘图区的最左边
{
	int mLeft = m_nSpace;
	if (IsShowBtns() && m_BtnAlient==baLeft)
		mLeft = m_nMargin + 1;
	return mLeft;
}
void CTabBeam::CaclTabsRect(CRect &rect)//绘制所有标签所需矩形的大小
{
	rect = CRect(0, 0, 5, m_nHeight);
	for (int i=0; i<GetCount(); i++)
		rect.right += GetTab(i).nWidth + m_nWidthEx;
	if (GetBtnCount()>0)
		rect.right += m_nMargin;
	rect.right += 5;//左右对称留空
}
// Calculate the left x-coordinate of the tab
int CTabBeam::CalcTabOffset(int nTab)
{
	int offset = GetDrawTabLeft();
	for (int i = m_nFirstTab; i < nTab; i++)
		offset += GetTab(i).nWidth + m_nWidthEx;
	if (nTab < m_nFirstTab)
	{
		for (int i = nTab; i < m_nFirstTab; i++)
			offset -= GetTab(i).nWidth + m_nWidthEx;
	}
	return offset;
}

// Get the tab for offset (x-coordinate)
int CTabBeam::CalcTabFromOffset(int nOffset)
{
	int offset = GetDrawTabLeft();
	int nTab = m_nFirstTab;
	for (;
		nTab < m_nTabs && offset < nOffset;
		offset += GetTab(nTab++).nWidth + m_nWidthEx)
		;

	return nTab < m_nTabs || offset > nOffset ? nTab-1 : -1;
}

int CTabBeam::CalcLastVisibleTab()
{
	CRect rect;
	GetClientRect(&rect);

	int offset = GetDrawTabLeft();
	if (m_dwStyle==tsTrapezoid)
		offset += GetTab(m_nFirstTab).nWidth + 2*m_nWidthEx;
	else
		offset += GetTab(m_nFirstTab).nWidth + m_nWidthEx;
	int i;
	for (i = m_nFirstTab+1; i < m_nTabs && offset < rect.right; i++)
		offset += GetTab(i).nWidth + m_nWidthEx;

	//if (offset >= rect.right)
		//return max(m_nFirstTab, i-2);

	//return m_nTabs-1;

	if (offset > rect.right)
		return max(m_nFirstTab, i-2);
	else if (offset == rect.right)
		return max(m_nFirstTab, i-1);
	else
		return m_nTabs-1;

}

// Tab has been pressed
void CTabBeam::SwitchToTab(CPoint point)
{
	int n = 0;
	if (m_BtnAlient==baLeft && point.x < GetBtnOffset() ||
		m_BtnAlient==baRight && point.x > GetBtnOffset())
		return;

	if (GetBtnCount() > 0 && m_BtnAlient==baLeft && point.x - GetBtnOffset() < m_nWidthEx)
		n = max(0, m_nFirstTab - 1);
	else if (GetBtnCount() > 0 && m_BtnAlient==baRight &&  GetBtnOffset() - point.x < m_nWidthEx)
		n = min(GetCount()-1, CalcLastVisibleTab());
	else
		n = CalcTabFromOffset(point.x);

	if (n == -1)
		return;

	BOOL bCanSwitch;
	if (m_pNotifyWnd)
		bCanSwitch = !m_pNotifyWnd->SendMessage(WM_GX_CANTABSWITCH, (WPARAM) n, 0);
	else
		bCanSwitch = !GetParent()->SendMessage(WM_GX_CANTABSWITCH, (WPARAM) n, 0);
	if (!bCanSwitch)  
		return;

	BOOL bSwitch = (n != m_nCurrent);
	if(bSwitch)
	{
		SetCurSel(n);
		if (m_pNotifyWnd)
			m_pNotifyWnd->PostMessage(WM_GX_TABSWITCH, (WPARAM) m_nCurrent, 0);
		else
			GetParent()->PostMessage(WM_GX_TABSWITCH, (WPARAM) m_nCurrent, 0);
	}
}

void CTabBeam::ScrollTabInView(int nTab)
{
	if (nTab < m_nFirstTab)
	{
		Invalidate();
		m_nFirstTab = nTab;
	}
	else if (nTab > CalcLastVisibleTab())
	{
		Invalidate();
		do {
			m_nFirstTab++;
		} while (nTab > CalcLastVisibleTab());
	}
}

// Double Clicked a tab
void CTabBeam::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (point.x < m_nMargin || CalcTabFromOffset(point.x) == -1)
	{
		OnLButtonDown(nFlags, point);
		return;
	}

	int n = m_nCurrent;
	if (n < m_nFirstTab)
	{
		OnLButtonDown(nFlags, point);
		return;
	}

	if (m_pNotifyWnd)
		;//m_pNotifyWnd->PostMessage(WM_GX_TABDBLCLK, (WPARAM) n, 0);//xie
	else
		;//GetParent()->PostMessage(WM_GX_TABDBLCLK, (WPARAM) n, 0);//xie
}

// Adjust tab size to content
void CTabBeam::AdjustTabSize(int nTab)
{
	CClientDC dc(this);

	CFont* pOldFont = dc.SelectObject(&m_fontCurrent);

	CSize size = dc.GetTextExtent(
		GetTab(nTab).sLabel,
		GetTab(nTab).sLabel.GetLength());

	dc.SelectObject(pOldFont);

	GetTab(nTab).nWidth = size.cx + m_nWidthEx;
	Invalidate();
}

int CTabBeam::FindTab(const CObject *pObject)
{
	for (int i = 0; i < m_nTabs; i++)
		if (GetTab(i).pExtra == pObject)
			return i;
	return -1;
}

// Current Selection, TabCount inlines

int CTabBeam::GetCurSel()
{ 
	return m_nCurrent; 
}

int CTabBeam::GetCount()
{ 
	return m_nTabs; 
}


/////////////////////////////////////////////////////////////////////////////
// CTabBeam Painting

void CTabBeam::OnPaint()
{
	UpdateColors();
	CPaintDC dc(this); // device context for painting

	int i;
	CRect rectPaint;
	dc.GetClipBox(&rectPaint);

	CDC dcMem;
	CBitmap bmPaint;
	CBitmap* pbmOld;

	GetClientRect(rectPaint);
	dcMem.CreateCompatibleDC(&dc);
	bmPaint.CreateCompatibleBitmap(&dc, rectPaint.Width(), rectPaint.Height());
	pbmOld = (CBitmap*) dcMem.SelectObject(&bmPaint);

	dcMem.OffsetViewportOrg(-rectPaint.left, -rectPaint.top);
	dcMem.SetBrushOrg(rectPaint.left % 8, rectPaint.top % 8);   // align dcMem with grid's dc

	// default font
	CFont* pOldFont = dcMem.SelectObject(&m_fontNormal);

	CRect rect;
	dcMem.GetClipBox(&rect);

	// Scroll Buttons
	dcMem.SelectClipRgn(nullptr);
	
	// Black Border
	PatB(&dcMem, rect.left, 0, rect.Width(), m_nDockLineWidth, m_rgbDockLine);
	// Grey Beam
	PatB(&dcMem, rect.left, m_nDockLineWidth, rect.Width(), rect.Height(), m_rgbBackground);

	// Tabs
	{
		int offset = GetDrawTabLeft();
		rect.left = max(rect.left, offset);
		dcMem.IntersectClipRect(rect);

		// Which tabs need painting?
		int nLast = CalcTabFromOffset(rect.right+m_nWidthEx);
		if (nLast == -1)
			nLast = m_nTabs-1;

		int nFirst = max(CalcTabFromOffset(rect.left)-1, 0);

		// Draw tabs
		for (i = nLast; i >= nFirst; i--)
			if (i != m_nCurrent)
				DrawTab(&dcMem, i);

		// Current tab
		if (m_nCurrent >= nFirst && m_nCurrent <= nLast)
			DrawTab(&dcMem, m_nCurrent);
	}

	// Buttons
	if (IsShowBtns())
	{
		for (i = 0; i < GetBtnCount(); i++)
			DrawButton(&dcMem, i + ((GetBtnCount()==2)?1:0), FALSE);
	}

	// Clean up
	dcMem.SelectObject(pOldFont);

	dc.BitBlt(rectPaint.left, rectPaint.top, rectPaint.Width(), rectPaint.Height(), &dcMem,
			rectPaint.left, rectPaint.top, SRCCOPY);

	// clean-up for memory DC
	dcMem.SelectObject(pbmOld);
}

void CTabBeam::DrawTab(CDC* pDC, int nTab)
{
	switch(m_dwStyle)
	{
		case tsRectangle: DrawRectangleTab(pDC, nTab);	break;
		case tsTrapezoid: DrawTrapezoidTab(pDC, nTab);	break;
		default:		  DrawTrapezoidTab(pDC, nTab);	break;
	}
}

void CTabBeam::DrawTrapezoidTab(CDC* pDC, int nTab)//画梯形tab
{
	int x = CalcTabOffset(nTab);
	BOOL bSel = (GetTab(nTab).bSel);
	BOOL bCur = (nTab == m_nCurrent);

	// Tab Polygon
	CRgn rgn;
	POINT pts[4];
	pts[0].x = x;                                   pts[0].y = 0;
	pts[1].x = x+GetTab(nTab).nWidth+2*m_nWidthEx;  pts[1].y = 0;
	pts[2].x = x+GetTab(nTab).nWidth+m_nWidthEx;    pts[2].y = m_nHeight;
	pts[3].x = x+m_nWidthEx;                        pts[3].y = m_nHeight;
	
	rgn.CreatePolygonRgn(pts, 4, ALTERNATE);

	// Inside rect
	CRect rect(pts[0].x, pts[1].y, pts[1].x, pts[2].y-m_nHightBarHeight);

	if (bSel || bCur)
	{
		if (!GetTab(nTab).flagUseOwnColorFaceSel)
			pDC->FillRgn(&rgn, &m_brFaceSel);
		else
			pDC->FillRgn(&rgn, &GetTab(nTab).brFaceSel);
	}
	else
	{
		if (!GetTab(nTab).flagUseOwnColorFaceNormal)
			pDC->FillRgn(&rgn, &m_brFaceNormal);
		else
			pDC->FillRgn(&rgn, &GetTab(nTab).brFaceNormal);
		PatB(pDC, pts[0].x, 0, pts[1].x, 1, m_rgbTabBorder);
	}

	// Frame
	pDC->FrameRgn(&rgn, &m_brBlack, 1, 1);
	if (bCur)
	{
		PatB(pDC, pts[0].x+2, 0, pts[1].x-pts[0].x-3, 1, m_rgbFaceSel);
		if (m_nHightBarHeight>0)
			PatB(pDC, pts[3].x, pts[3].y-m_nHightBarHeight, pts[2].x-pts[3].x, pts[3].y, m_rgbHightBar);
		rect.bottom -= m_nHightBarHeight;
	}
	
	// Draw Text
	COLORREF rgbText = bCur ? m_rgbTextCurrent : m_rgbTextNormal;
	COLORREF rgbBack = (bSel||bCur) ? m_rgbFaceSel : m_rgbFaceNormal;

	pDC->SetTextColor(rgbText);
	pDC->SetBkColor(rgbBack);
	pDC->SetBkMode(TRANSPARENT);

	// check if current tab
	CFont* pOldFont = nullptr;
	if (bCur)
		pOldFont = pDC->SelectObject(&m_fontCurrent);

	// center vertically
	CString strLabel = GetTab(nTab).sLabel;
	pDC->DrawText(strLabel, -1, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	if(GetTab(nTab).bMul)
	{
		rect.top = rect.top+2;
		rect.bottom =  rect.top+6;
		rect.left = x+GetTab(nTab).nWidth;
		rect.right = x+GetTab(nTab).nWidth+m_nWidthEx*3/2;
		strLabel="^";
		pDC->DrawText(strLabel, -1, rect, DT_TOP | DT_CENTER | DT_SINGLELINE);
	}
	// clean up
	if (bCur)
		pDC->SelectObject(pOldFont);
}
void CTabBeam::DrawRectangleTab(CDC* pDC, int nTab)//画矩形tab
{
	int x = CalcTabOffset(nTab);
	BOOL bSel = (GetTab(nTab).bSel);
	BOOL bCur = (nTab == m_nCurrent);

	// Tab Polygon
	CRgn rgn;	
	POINT pts[4];
	pts[0].x = x;									pts[0].y = m_nDockLineWidth;
	pts[1].x = x+GetTab(nTab).nWidth+m_nWidthEx;	pts[1].y = m_nDockLineWidth;
	pts[2].x = x+GetTab(nTab).nWidth+m_nWidthEx;	pts[2].y = m_nHeight+2;
	pts[3].x = x;									pts[3].y = m_nHeight+2;
	
	rgn.CreatePolygonRgn(pts, 4, ALTERNATE);

	// Inside rect
	CRect rect(pts[0].x, pts[1].y, pts[1].x, pts[2].y);

	if (bSel || bCur)
	{
		PatB(pDC, pts[0].x, 0, pts[1].x-pts[0].x, m_nDockLineWidth+1, m_rgbFaceSel);
		if (!GetTab(nTab).flagUseOwnColorFaceSel)
			pDC->FillRgn(&rgn, &m_brFaceSel);
		else
			pDC->FillRgn(&rgn, &GetTab(nTab).brFaceSel);
	}
	else
	{
		if (!GetTab(nTab).flagUseOwnColorFaceNormal)
			pDC->FillRgn(&rgn, &m_brFaceNormal);
		else
			pDC->FillRgn(&rgn, &GetTab(nTab).brFaceNormal);
	}

	if (bCur)
	{
		PatB(pDC, pts[0].x+1, 0, pts[1].x-pts[0].x-2, 1, m_rgbFaceSel);
		if (m_nHightBarHeight>0)
			PatB(pDC, pts[3].x, pts[3].y-m_nHightBarHeight, pts[2].x-pts[3].x, pts[3].y, m_rgbHightBar);
		rect.bottom -= m_nHightBarHeight;
	}
	else			
	{
		int iBorder = 3;
		PatB(pDC, pts[1].x, iBorder, 1, pts[2].y-pts[1].y-2*iBorder, m_rgbTabBorder);//标签间的分割线
	}
	
	// Draw Text
	COLORREF rgbText = bCur ? m_rgbTextCurrent : m_rgbTextNormal;
	COLORREF rgbBack = (bSel||bCur) ? m_rgbFaceSel : m_rgbFaceNormal;

	pDC->SetTextColor(rgbText);
	pDC->SetBkColor(rgbBack);
	pDC->SetBkMode(TRANSPARENT);
	
	// check if current tab
	CFont* pOldFont = nullptr;
	if (bCur)
		pOldFont = pDC->SelectObject(&m_fontCurrent);

	// center vertically
	CString strLabel = GetTab(nTab).sLabel;
	pDC->DrawText(strLabel, -1, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	if(GetTab(nTab).bMul)
	{
		rect.top = rect.top+2;
		rect.left = x+GetTab(nTab).nWidth;
		rect.right = x+GetTab(nTab).nWidth+m_nWidthEx*3/2;
		strLabel="^";
		pDC->DrawText(strLabel, -1, rect, DT_TOP | DT_CENTER | DT_SINGLELINE);
	}
	// clean up
	if (bCur)
		pDC->SelectObject(pOldFont);
}

void CTabBeam::DrawButton(CDC* pDC, int nButton, BOOL bPressed)
{
	int i = nButton;
	CRect rect;
	GetBtnRect(rect, i);
	pDC->FillSolidRect(rect, m_rgbBackground);
	pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNSHADOW));

	// Draw arrows
	int x, y;

	int cx = m_nHeight > 19 ? 6 : 5;
	cx += 2;

	int cy = m_nHeight > 19 ? 9 : 7;

	// center the 9-pixel high arrow vertically
	y = rect.top + ((rect.Height() - 9) / 2);
	int y2 = rect.top + ((rect.Height() - cy) / 2);

	// if pressed, image gets moved
	if (bPressed)
		y++;

	COLORREF rgb = m_rgbTextNormal;

	CRect rectClient;
	GetClientRect(rectClient);

	if (GetCount() <= 1 || i <= 1 && m_nFirstTab == 0 || i >= 2 && CalcTabFromOffset(rectClient.right) == -1)
		rgb = m_rgbDisableBtn;

	switch(i)
	{
	case GX_MOVERECORD_FIRST:
		// Most left
		x = rect.left + ((rect.Width() - cx) / 2) - 1;
		if (bPressed)
			x++;

		PatB(pDC, x, y2, 1, cy, rgb);

	case GX_MOVERECORD_PREV:
		// Left Arrow
		x = rect.left + ((rect.Width() - cx) / 2) - 1;
		if (bPressed)
			x++;

		PatB(pDC, x+2, y+4, 1, 1, rgb);
		PatB(pDC, x+3, y+3, 1, 3, rgb);
		PatB(pDC, x+4, y+2, 1, 5, rgb);
		PatB(pDC, x+5, y+1, 1, 7, rgb);

		if (m_nHeight > 19)
			PatB(pDC, x+6,   y, 1, 9, rgb);
		break;

	case GX_MOVERECORD_LAST:
		// Most Right
		x = rect.right - ((rect.Width() - cx) / 2);
		if (bPressed)
			x++;

		PatB(pDC, x, y2, 1, cy, rgb);

	case GX_MOVERECORD_NEXT:
		// Right Arrow
		x = rect.right - ((rect.Width() - cx) / 2);
		if (bPressed)
			x++;

		PatB(pDC, x-2, y+4, 1, 1, rgb);
		PatB(pDC, x-3, y+3, 1, 3, rgb);
		PatB(pDC, x-4, y+2, 1, 5, rgb);
		PatB(pDC, x-5, y+1, 1, 7, rgb);
		if (m_nHeight > 19)
			PatB(pDC, x-6,   y, 1, 9, rgb);
		break;
	}
}


BOOL CTabBeam::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CTabBeam Mouse Actions

// Replacement for SetCapture
// Sends WM_MOUSEMOVE Messages, when a Button is pressed
void CTabBeam::AutoMove(UINT nRefMessage)
{
	MSG   msgLoop;          // dummmy message structure to process incoming
							// messages while autoscrolling.
	CPoint ptCursorPos;

	msgLoop.message = 0;    // forces at least one loop.

	SetCapture();

	DWORD nLastTicks = GetTickCount();
	DWORD nTicksNeeded = 200;

	// Process all messages until the relevant mouse button
	// has been released.
	while (msgLoop.message != nRefMessage) // nRefMessage depends on which button
										// has been used to trigger autoscrolling.
	{
		// Process incoming messages until autoscroll button is released
		if (PeekMessage(&msgLoop, 0, 0, 0, PM_REMOVE))
		{
			if (msgLoop.message != WM_MOUSEMOVE)
			{
				TranslateMessage(&msgLoop);
				DispatchMessage(&msgLoop);
			}
			continue;
		}

		GetCursorPos(&ptCursorPos);
		ScreenToClient(&ptCursorPos);

		if (GetTickCount() - nLastTicks > nTicksNeeded)
		{
			// peridically do OnMouseMove
			OnMouseMove(0, ptCursorPos);
			if (nTicksNeeded > 100)
				nTicksNeeded -= 20;
		}
	}

	ReleaseCapture();
}

// Button is hit
// 0 = most left, 1 = scroll left, 2 = scroll right, 3 = most right
// if nButton is not -1, it is checked if the mouse has left
// the pressed button, (the button will get unpressed in OnMouseMove)
int CTabBeam::ScrollBtnHit(CPoint point, int nButton /*= -1*/)
{
	CRect rect;
	GetBtnRect(rect, -1);
	if (!rect.PtInRect(point))
		return -1;

	// get Button
	int nBtn = (point.x - rect.left) / m_nButtonWidth;

	if (GetBtnCount() == 2)
		nBtn++;

	CRect rectClient;
	GetClientRect(rectClient);

	if (nBtn <= 1 && m_nFirstTab == 0 || nBtn >= 2 && CalcTabFromOffset(rectClient.right) == -1)
		return -1;

	if (nButton != -1 && nBtn != nButton)
		return -1;

	// Button is pressed
	Scroll(nBtn);

	return nBtn;
}

void CTabBeam::Scroll(int nDirection)
{
	// ensure that edit-popup is not visible
	//GetParent()->SendMessage(WM_GX_EDITCANCEL, 0, 0);//xie

	int n = m_nFirstTab;
	switch (nDirection)
	{
	case GX_SCRLMOSTLEFT:  n = 0; break;
	case GX_SCRLLEFT:  n = m_nFirstTab-1; break;
	case GX_SCRLRIGHT: n = m_nFirstTab+1; break;
	case GX_SCRLMOSTRIGHT: n = m_nTabs-1; break;
	default:
		return;
	}

	// Calculate last visible tabs
	CRect rect;
	GetClientRect(rect);
	rect.left = GetDrawTabLeft();//m_nMargin+1;

	int nLast = m_nTabs-1;
	int dx = GetTab(nLast).nWidth+2*m_nWidthEx;
	while (nLast > m_nFirstTab && dx+GetTab(nLast-1).nWidth+m_nWidthEx < rect.Width())
		dx += GetTab(--nLast).nWidth+m_nWidthEx;

	// Adjust n (this the new value for m_nFirstTab)
	n = max(0, min(nLast, n));

	// Check if n is different
	if (n != m_nFirstTab)
	{
		// Scroll
		if (n > m_nFirstTab)
		{
			int scrolled = 0;
			for (int i = m_nFirstTab; i < n; i++)
				scrolled += GetTab(i).nWidth+m_nWidthEx;
			ScrollWindow(-scrolled, 0, rect, rect);
		}
		else
		{
			int scrolled = 0;
			for (int i = n; i < m_nFirstTab; i++)
				scrolled += GetTab(i).nWidth+m_nWidthEx;
			ScrollWindow(scrolled, 0, rect, rect);
			rect.right = rect.left+scrolled+m_nWidthEx;
		}
		InvalidateRect(rect, FALSE);
		m_nFirstTab = n;
	}
}

void CTabBeam::OnRButtonUp(UINT nFlags, CPoint point) 
{
	int n = 0;
	if (m_BtnAlient==baLeft && point.x < GetBtnOffset() ||
		m_BtnAlient==baRight && point.x > GetBtnOffset())
		return;

	if (m_BtnAlient==baLeft && point.x - GetBtnOffset() < m_nWidthEx)
		n = max(0, m_nFirstTab - 1);
	else if (m_BtnAlient==baRight &&  GetBtnOffset() - point.x < m_nWidthEx)
		n = min(GetCount()-1, CalcLastVisibleTab());
	else
		n = CalcTabFromOffset(point.x);

	if (n == -1)
		return;
	ClientToScreen(&point);
	if (m_pNotifyWnd)
		m_pNotifyWnd->SendMessage(WM_PAGEPOPUP,n,POINTTOPOINTS(point));

}

void CTabBeam::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((m_nBtnDown = ScrollBtnHit(point)) != -1)
	{
		// Scrolling
		CClientDC dc(this);
		DrawButton(&dc, m_nBtnDown, m_bBtnDown = TRUE);

		m_bMouseCapture = TRUE;
		AutoMove(WM_LBUTTONUP); // (SetCapture until WM_LBUTTONUP)
	}
	else if (nFlags & MK_CONTROL || nFlags & MK_SHIFT)
	{
		// Select Tabs
		CRect rect;
		GetClientRect(rect);
		rect.left = GetDrawTabLeft();
		int n = CalcTabFromOffset(point.x);
		if (n == -1)
			return;
		if (nFlags & MK_CONTROL)
		{
			GetTab(n).bSel = !GetTab(n).bSel;
			InvalidateRect(rect, FALSE);
		}
		else if (m_nCurrent != -1)
		{
			while (n != m_nCurrent)
			{
				GetTab(n).bSel = 1;
				n = (n > m_nCurrent)?n-1:n+1;
			}
			InvalidateRect(rect, FALSE);
		}
	}
	else
		SwitchToTab(point);

	CWnd::OnLButtonDown(nFlags, point);
}

void CTabBeam::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bMouseCapture)
	{
		m_bMouseCapture = FALSE;

		if (m_nBtnDown != -1 && m_bBtnDown)
		{
			UpdateWindow();
			CClientDC dc(this);
			for (int i = 0; i < GetBtnCount(); i++)
				DrawButton(&dc, i + ((GetBtnCount()==2)?1:0), FALSE);
		}
	}

	m_bBtnDown = FALSE;

	CWnd::OnLButtonUp(nFlags, point);
}

void CTabBeam::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	SetCursor(m_hcurArrow);

	if (!m_bMouseCapture)
		return;

	if (m_nBtnDown != -1)
	{
		// Check if mouse has left the button,
		// ScrollBtnHit does scrolling
		BOOL bDraw = FALSE;
		if (ScrollBtnHit(point, m_nBtnDown) == -1)
		{
			bDraw = m_bBtnDown;
			m_bBtnDown = FALSE;
		}
		else
		{
			bDraw = !m_bBtnDown;
			m_bBtnDown = TRUE;
		}

		if (bDraw)
		{
			UpdateWindow();
			CClientDC dc(this);
			for (int i = 0; i < GetBtnCount(); i++)
			{
				int n = i + ((GetBtnCount()==2)?1:0);
				DrawButton(&dc, n, n == m_nBtnDown && m_bBtnDown);
			}
		}
	}
}

void CTabBeam::UpdateColors()
{
	m_nMargin = GetBtnCount() * m_nButtonWidth;

	// Brushes
	m_brFaceSel.DeleteObject();
	m_brFaceSel.CreateSolidBrush(m_rgbFaceSel);
	m_brFaceNormal.DeleteObject();
	m_brFaceNormal.CreateSolidBrush(m_rgbFaceNormal);
	m_brBlack.DeleteObject();
	m_brBlack.CreateSolidBrush(m_rgbTabBorder);

	// Pens
	m_penShadow.DeleteObject();
	m_penShadow.CreatePen(PS_SOLID, 1, m_rgbTabBorder);

	// Fonts
	CreateFonts();
}

void CTabBeam::OnSysColorChange()
{
	CWnd::OnSysColorChange();

	//GXGetSysData()->UpdateSysColors();//xie
	Invalidate();

#if _MFC_VER < 0x0300
	// Also check scrollbar size
	if (GXGetAppData()->bWin4)
		OnWinIniChange(nullptr);
#endif
}

void CTabBeam::OnWinIniChange(LPCTSTR lpszSection)
{
	CWnd::OnWinIniChange(lpszSection);

	//GXGetSysData()->UpdateSysSettings();//xie

	int cy = GetSystemMetrics(SM_CYHSCROLL);

	if (m_nHeight != cy)
	{
		// scrollbar size did change

		m_nButtonWidth = m_nHeight = cy;
		m_nMargin = GetBtnCount() * m_nButtonWidth;

		CreateFonts();

		for (int i = 0; i < m_nTabs; i++)
			AdjustTabSize(i);
	}

	Invalidate();
}

// Drawing helpers

void AFXAPI CTabBeam::PatB(CDC* pDC,int x,int y,int dx,int dy, COLORREF rgb)
{
	RECT    rc;

	rc.left   = x;
	rc.top    = y;
	rc.right  = x + dx;
	rc.bottom = (y + dy);

	COLORREF rgbOld = pDC->GetBkColor();
	pDC->SetBkColor(rgb);
	pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,nullptr,0,nullptr);
	pDC->SetBkColor(rgbOld);
}

void CTabBeam::SetCurSel(int nCurrent)
{
	m_nCurrent = nCurrent;
	if (!GetTab(m_nCurrent).bSel)
	{
		for (int i = 0; i < m_nTabs; i++)
			GetTab(i).bSel = 0;
	}

	ScrollTabInView(m_nCurrent);

	Invalidate();
	UpdateWindow();
}

