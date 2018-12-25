/*
//	�Ľ�����:
//	ȥ��CheckBox�� TreeBtn ֮��ĺ���������״̬ͼ����ʾ˳������ⲿ��������ĸ�˳����CheckBox�ĸ���TreeBtn
//	ȥ��m_bShowTreeBtn��m_bShowCheckBox����Ϊһ������Ԫ�ص�����IconShows����ʾ��Ӧ˳���ͼ���Ƿ���ʾ
//	����ͼ������ָ����ǰҪ��ʾ��ͼ������ĿnIconCount��nIconCount���Կ�����iImage�����3��bit��ȡ������ÿ��item��ʾ��ͼ�������Բ�һ�������ڵĴ���
			if(m_bShowTreeBtn)
			{	
				StateImageIndex = STATEIMAGEMASK_TO_INDEX_0(lvi.state);
				pDC->FillSolidRect(rcIcon, m_BackColor);
				m_TreeImgList.Draw(pDC, StateImageIndex, CPoint(rcIcon.left, rcIcon.top + (rcIcon.Height() - m_nCYSMIcon) / 2), ILD_TRANSPARENT);
				
				rcIcon.OffsetRect(m_nCXSMIcon, 0);
			}			
			if (m_bShowCheckBox)...
			�Ϳ��Ի���ͨ�õ�
			for (int i = 0; i < min(nIconCount, 5); i++)
				if(IconShows[i])
				{	
					StateImgIndex = GET_STATEIMAGE_INDEX(lvi.state, i);
					pDC->FillSolidRect(rcIcon, m_BackColor);
					m_StateImgList.Draw(pDC, StateImgIndex, CPoint(rcIcon.left, rcIcon.top + (rcIcon.Height() - m_nCYSMIcon) / 2), ILD_TRANSPARENT);
				
					rcIcon.OffsetRect(m_nCXSMIcon, 0);
				}			
//	�Ժ�Ĺ��ܼƻ���
//	����ѡ�и��ڵ��ʱ���Զ�ѡ���ӽڵ㣬ͳһ��ѡ���б���ɫ����ʾһ����߿�(��ʾ�߿�Ļ�)
//	ѡ���п��Լ������⣬��ͼƬ��ˢ��Ź���ͼƬ���ѡ���б���
//	��Ԫ�����ͷ�Ķ�����ʾ
//	��ͷ��Ŀ����ʾ������
//	�й��ˣ�֧�ֶ�ѡ��֧���������룬��Щ����ֱ������ͷ������㰴ť���������������˱�־����ͼ�����ʽ��ʾ����ͷ��
//	�Զ�������ֶΣ������Ǳ���������ֶε���������Ҳ���Ը�ʽ���ķ�ʽ���
//														2013-02-08	������
*/

#include "stdafx.h"
#include "xListCtrl.h"
#include "MemDC.h"

#include "global.h"
#include "globalmfc.h"



LOGFONT BaseFont = { -14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("Tahoma") };

IMPLEMENT_DYNAMIC(CxListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CxListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CxListCtrl::CxListCtrl(bool UseSystemHeader) :
	m_BackgroundColor(0x1e1e1e),
	m_BackColor(0x1e1e1e),
	m_SelBackColor(0xcc7a00),//0x0f0f0f),
	m_TextColor(0xb1b1b1),
	m_SelTextColor(0xb1b1b1),
	m_LineColor(0x101010),
	m_bShowHorzLine(FALSE),
	m_bShowVertLine(TRUE),
	m_bDrawSelFrame(FALSE),
	m_bShowLeftLine(FALSE),
	m_LogFont(BaseFont),
	m_nItemHeight(20),
	m_bShowCheckBox(false),
	m_bShowTreeBtn(false),
	m_nCXSMIcon(16),
	m_nCYSMIcon(16),
	m_nLvirIcon(20),
	m_bUseSystemHeader(UseSystemHeader),
	m_Gap(4)
{
	m_nCXSMIcon = GetSystemMetrics(SM_CXSMICON);
	m_nCYSMIcon = GetSystemMetrics(SM_CYSMICON);
}
CxListCtrl::~CxListCtrl()
{
	if (m_CheckImgList.m_hImageList != nullptr)
		m_CheckImgList.DeleteImageList();	
	if (m_TreeImgList.m_hImageList != nullptr)
		m_TreeImgList.DeleteImageList();
}
/****************************************** StateImage ***********************************************/
void CxListCtrl::ShowCheckBox(bool ShowCheck)
{
	if (m_bShowCheckBox == ShowCheck)
		return;

	m_bShowCheckBox = ShowCheck;
	if (m_bShowCheckBox && m_CheckImgList.m_hImageList == nullptr)
		CDraw::img_list_init_form_png(IDB_CHECKBOX, 13, 13, -1, m_CheckImgList);
}

void CxListCtrl::ShowTreeBtn(bool ShowTreeBtn)
{
	if (m_bShowTreeBtn == ShowTreeBtn)
		return;

	m_bShowTreeBtn = ShowTreeBtn;
	if (m_bShowTreeBtn && m_TreeImgList.m_hImageList == nullptr)
		CDraw::img_list_init_form_png(IDB_TREEBTN, 12, 12, -1, m_TreeImgList);
}

bool CxListCtrl::ItemChecked(int iItem)
{
	TCHAR lpBuffer[MAX_LIST_FIELD_BUF];
	LV_ITEM lvi;
	::ZeroMemory(&lvi, sizeof(lvi));
	lvi.iItem = iItem;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.stateMask = LVIS_STATEIMAGEMASK_1;
	lvi.pszText = lpBuffer;
	lvi.cchTextMax = sizeof(lpBuffer);
	lvi.iSubItem = 0;
	VERIFY(GetItem(&lvi));

	return STATEIMAGEMASK_TO_INDEX_1(lvi.state) >= 3;
}

UINT CxListCtrl::GetStateImageMask_0(bool Value)
{
	if (Value)
		return INDEX_TO_STATEIMAGEMASK_0(2);
	else
		return INDEX_TO_STATEIMAGEMASK_0(0);
}

UINT CxListCtrl::GetStateImageMask_1(bool Value)
{
	if (Value)
		return INDEX_TO_STATEIMAGEMASK_1(4);
	else
		return INDEX_TO_STATEIMAGEMASK_1(1);
}

bool CxListCtrl::GetIconRect(CRect & rcIcon, int iOrder, int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem < 0 || iItem >= GetItemCount() || iSubItem >= GetHeaderCtrl()->GetItemCount())
		return false;

	LV_ITEM lvi;
	::ZeroMemory(&lvi, sizeof(lvi));
	lvi.iItem = iItem;
	lvi.iSubItem = iSubItem;
	lvi.mask = LVIF_INDENT;
	VERIFY(GetItem(&lvi));

	GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, rcIcon);
	int _left = rcIcon.left;
	rcIcon.right = rcIcon.left + m_nLvirIcon;

	rcIcon.OffsetRect(m_nCXSMIcon * lvi.iIndent, 0);

	//TreeBtn
	if (m_bShowTreeBtn)
	{
		if (iOrder == 0)
			return true;
		rcIcon.OffsetRect(m_nCXSMIcon, 0);
	}
	//CheckBox
	if (m_bShowCheckBox)
	{
		if (iOrder == 1)
			return true;
		rcIcon.OffsetRect(m_nCXSMIcon, 0);
	}
	//����ʾ�κ�ͼƬ
	rcIcon.right = rcIcon.left = _left;

	return rcIcon.Width() > 0;
}
bool CxListCtrl::InIcon(int iOrder, int iItem, int iSubItem)
{
	POINT Point;
	GetCursorPos(&Point);
	ScreenToClient(&Point);

	CRect rcTreeIcon;
	if (GetIconRect(rcTreeIcon, iOrder, iItem, iSubItem))
		return PtInRect(&rcTreeIcon, Point) == TRUE;

	return false;
}
bool CxListCtrl::InTreeBtn(int iItem)
{
	return InIcon(0, iItem);
}
bool CxListCtrl::InCheckBox(int iItem)
{
	return InIcon(1, iItem);
}

UINT CxListCtrl::GetColWidthDx(int iSubItem)
{
	UINT dx = 0;
	if (iSubItem == 0)
	{
		if (m_bShowCheckBox)
			dx += m_nCXSMIcon;
		if (m_bShowTreeBtn)
			dx += m_nCXSMIcon;
	}

	return dx;
}

/****************************************** Draw ***********************************************/
void CxListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CMemDC MemDC(CDC::FromHandle(lpDrawItemStruct->hDC), FALSE, &lpDrawItemStruct->rcItem);
	DoDrawItem(lpDrawItemStruct, &MemDC);
}

void CxListCtrl::DoDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct, CDC * pDC)
{
	CRect rcWinRect;
	GetParent()->GetWindowRect(rcWinRect);

	CRect rcClient;
	GetClientRect(&rcClient);

	//���塢��ɫ
	CFont currFont;
	currFont.CreateFontIndirect(&m_LogFont);
	CFont * pOldFont = pDC->SelectObject(&currFont);

	CPen penLine(PS_SOLID, 1, m_LineColor);
	CPen* pOldPen = pDC->SelectObject(&penLine);
	CPen penFream(PS_SOLID, 1, m_SelBackColor);

	LV_ITEM lvi;
	::ZeroMemory(&lvi, sizeof(lvi));
	lvi.iItem = lpDrawItemStruct->itemID;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT;
	lvi.stateMask = LVIS_STATEIMAGEMASK_0 | LVIS_STATEIMAGEMASK_1;
	lvi.pszText = m_lpBuffer;
	lvi.cchTextMax = sizeof(m_lpBuffer);

	HDITEM hdi;
	hdi.mask = HDI_FORMAT;

	CRect rcBound, rcItem, rcIcon, rcText;
	int StateImageIndex = -1;
	int nFmt;


	int FixCount = 0;
	int nColOrd = -1;
	int nColCount = GetHeaderCtrl()->GetItemCount();
	for (int nCol = FixCount; nCol < nColCount; nCol++)
	{
		GetSubItemRect(lpDrawItemStruct->itemID, nCol, LVIR_LABEL, rcItem);
		//��������ʾ���С�//������в���ʾ��������Ϊfixʱ��Ҫ���⴦�������������л�����ɺ����»���fix�С��Ժ��ǣ�

		if (rcItem.right < 0 || rcItem.left > rcClient.right)
			continue;

		lvi.iSubItem = nCol;
		VERIFY(GetItem(&lvi));
		nColOrd = GetHeaderCtrl()->OrderToIndex(nCol);

		rcText = rcItem;
		//��һ�� ����ͼ��
		if (nColOrd == 0)
		{
			GetSubItemRect(lpDrawItemStruct->itemID, nCol, LVIR_BOUNDS, rcBound);
			rcIcon = rcBound;
			rcIcon.right = rcIcon.left + m_nLvirIcon;

			rcIcon.OffsetRect(m_nCXSMIcon * lvi.iIndent, 0);
			//TreeBtn
			if (m_bShowTreeBtn)
			{
				StateImageIndex = STATEIMAGEMASK_TO_INDEX_0(lvi.state);
				pDC->FillSolidRect(rcIcon, m_BackColor);
				if (m_TreeImgList.m_hImageList)
					m_TreeImgList.Draw(pDC, StateImageIndex, CPoint(rcIcon.left, rcIcon.top + (rcIcon.Height() - m_nCYSMIcon) / 2), ILD_TRANSPARENT);

				rcIcon.OffsetRect(m_nCXSMIcon, 0);
			}
			//CheckBox
			if (m_bShowCheckBox)
			{
				StateImageIndex = STATEIMAGEMASK_TO_INDEX_1(lvi.state);
				pDC->FillSolidRect(rcIcon, m_BackColor);
				if (m_CheckImgList.m_hImageList)
					m_CheckImgList.Draw(pDC, StateImageIndex, CPoint(rcIcon.left, rcIcon.top + (rcIcon.Height() - m_nCYSMIcon) / 2), ILD_TRANSPARENT);

				rcIcon.OffsetRect(m_nCXSMIcon, 0);
			}

			rcText.left = rcIcon.left;
			rcItem.left = min(0, rcItem.left);
		}

		//����ѡ�б���
		if (lpDrawItemStruct->itemState & ODS_SELECTED && !m_bDrawSelFrame)
			pDC->FillSolidRect(&rcText, m_SelBackColor);
		else
			pDC->FillSolidRect(&rcText, m_BackColor);

		//������
		if (m_bShowHorzLine || m_bShowVertLine)
		{
			pDC->SelectObject(&penLine);
			if (m_bShowHorzLine)
			{
				pDC->MoveTo(rcItem.left, rcItem.bottom - 1);
				pDC->LineTo(rcItem.right, rcItem.bottom - 1);
			}
			if (m_bShowVertLine)
			{
				pDC->MoveTo(rcItem.right - 1, rcItem.top);
				pDC->LineTo(rcItem.right - 1, rcItem.bottom);
				if (nColOrd == 0 && m_bShowLeftLine)
				{
					pDC->MoveTo(rcItem.left, rcItem.top);
					pDC->LineTo(rcItem.left, rcItem.bottom);
				}
			}
		}

		//����ѡ�п�
		if (lpDrawItemStruct->itemState & ODS_SELECTED && m_bDrawSelFrame)
		{
			pDC->SelectObject(&penFream);
			if (nColOrd == 0)
			{
				pDC->MoveTo(rcItem.left, rcItem.top);
				pDC->LineTo(rcItem.left, rcItem.bottom - 1);
			}
			else if (nColOrd == nColCount - 1)
			{
				pDC->MoveTo(rcItem.right - 1, rcItem.bottom - 1);
				pDC->LineTo(rcItem.right - 1, rcItem.top);
			}

			pDC->MoveTo(rcItem.left, rcItem.top);
			pDC->LineTo(rcItem.right, rcItem.top);

			pDC->MoveTo(rcItem.left, rcItem.bottom - 1);
			pDC->LineTo(rcItem.right, rcItem.bottom - 1);
		}

		//������ɫ
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
			pDC->SetTextColor(m_SelTextColor);
		else
			pDC->SetTextColor(m_TextColor);

		//��������
		UINT UFmt = DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER; // | DT_END_ELLIPSIS����Ĭ����ʾ��ʽ
		GetHeaderCtrl()->GetItem(nCol, &hdi);
		nFmt = hdi.fmt & HDF_JUSTIFYMASK;
		switch (nFmt)
		{
			case HDF_CENTER:	UFmt |= DT_CENTER; break;
			case HDF_LEFT:	UFmt |= DT_LEFT; break;
			default:	UFmt |= DT_RIGHT; break;
		}
		pDC->SetBkMode(TRANSPARENT);
		rcText.DeflateRect(m_Gap, 0);						//������߿�����ұ߾�
		DrawText(pDC->m_hDC, m_lpBuffer, strlen(m_lpBuffer), &rcText, UFmt);
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
}

BOOL CxListCtrl::OnEraseBkgnd(CDC* pDC)
{
	//ֻ��ʣ�ಿ��
	CRect rcHead(0, 0, 0, 0);
	int count = GetHeaderCtrl()->GetItemCount();
	if (count > 0)
		GetHeaderCtrl()->GetItemRect(count - 1, rcHead);

	CRect rcItem(0, rcHead.bottom, rcHead.right, rcHead.bottom);
	count = GetItemCount();
	if (count > 0)
		GetSubItemRect(count - 1, 0, LVIR_LABEL, rcItem);

	CRect rcClient;
	GetClientRect(rcClient);

	CRect rcBottom(rcClient.left, rcItem.bottom, rcClient.right, rcClient.bottom);
	CRect rcRight(rcHead.right, rcHead.bottom, rcClient.right, rcItem.bottom);
	
	if (rcBottom.Height() > 0 && rcBottom.Width() > 0)
	{
		CMemDC mdc(pDC, FALSE, rcBottom);
		mdc.FillSolidRect(rcBottom, m_BackgroundColor);
	}
	if (rcRight.Width() > 0 && rcRight.Height() > 0)
	{
		CMemDC mdc(pDC, FALSE, rcRight);
		mdc.FillSolidRect(rcRight, m_BackgroundColor);
	}

	return TRUE;
}

/****************************************** Font ***********************************************/
//������ͷ����
void CxListCtrl::SetHeaderFont(CFont * hFont, BOOL bRedraw)
{
	if (!hFont)
		return;

	SetRedraw(FALSE);

	m_Header.SetFont(hFont, bRedraw);

	SetRedraw(TRUE);
}
CFont * CxListCtrl::GetHeaderFont()
{
	return m_Header.GetFont();
}
//������������
void CxListCtrl::SetFont(CFont * cFont, BOOL bRedraw)
{
	if (!cFont)
		return;

	SetRedraw(FALSE);

	cFont->GetLogFont(&m_LogFont);
	CListCtrl::SetFont(cFont, bRedraw);

	DoSetHeight();

	SetRedraw(TRUE);
}
CFont * CxListCtrl::GetFont()
{
	m_OutFont.DeleteObject();
	m_OutFont.CreateFontIndirect(&m_LogFont);
	return &m_OutFont;
}

void CxListCtrl::AutoColumnWidth(int Column, bool CurrPage)
{
	if (Column > 0 && Column >= m_Header.GetItemCount())
		return;

	int ret = 0;
	int i = Column < 0 ? 0 : Column;
	int count = Column < 0 ? m_Header.GetItemCount() : Column + 1;

	char text[256];
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_TEXT;
	lvColumn.pszText = text;
	lvColumn.cchTextMax = sizeof(text);

	SetRedraw(FALSE);

	CDC * headDC = m_Header.GetDC();
	CDC * listDC = GetDC();
	CFont * headFont = headDC->SelectObject(GetHeaderFont());
	CFont * listFont = listDC->SelectObject(GetFont());

	for (; i < count; i++)
	{
		GetColumn(i, &lvColumn);
		ret = headDC->GetTextExtent(text).cx;

		int beginIndex = 0;
		int endIndex = GetItemCount() - 1;
		if (CurrPage)
		{
			beginIndex = GetTopIndex();
			//һ������ʾ������
			endIndex = min(GetTopIndex() + max(this->GetCountPerPage(), GetSystemMetrics(SM_CYSCREEN) / (int)GetItemHeight()), GetItemCount());
		}
		for (int j = beginIndex; j < endIndex; j++)
			ret = max(ret, listDC->GetTextExtent(GetItemText(j, i)).cx);

		if (ret > 0)
			SetColumnWidth(i, ret + GetColWidthDx(i) + 2 * m_Gap);
	}

	headDC->SelectObject(headFont);
	listDC->SelectObject(listFont);

	m_Header.ReleaseDC(headDC);
	ReleaseDC(listDC);

	SetRedraw(TRUE);
}

void CxListCtrl::SetSortIcon(int Column, int Sort)
{
	m_Header.SetSortIcon(Column, Sort);
}
/******************************************ItemHeight***********************************************/
void CxListCtrl::SetItemHeight(UINT nHeight)
{
	if (m_nItemHeight == nHeight)
		return;
	m_nItemHeight = nHeight;

	DoSetHeight();
}
//�����иߣ�����WM_WINDOWPOSCHANGED��Ϣ���Ե��ø����ڻ����OnMeasureItem��Ϣ����������ɲ���
void CxListCtrl::DoSetHeight()
{
	CRect rc;
	GetWindowRect(&rc);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;

	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}
void CxListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CFont currFont;
	currFont.CreateFontIndirect(&m_LogFont);

	CDC * pDC = GetDC();
	CFont * pOldFont = pDC->SelectObject(&currFont);
	LONG fontHeight = pDC->GetTextExtent("Font2018").cy;
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	m_nItemHeight = max((UINT)fontHeight, m_nItemHeight);
	lpMeasureItemStruct->itemHeight = m_nItemHeight;
}

UINT CxListCtrl::GetItemHeight()
{
	return m_nItemHeight;
}

/******************************************SelectRow***********************************************/
int CxListCtrl::GetFirstSelIndex()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (!pos)
		return -1;
	else
		return GetNextSelectedItem(pos);
}

/******************************************IGridRef***********************************************/
void CxListCtrl::set_draw(bool draw)
{
	//SetRedraw(draw==true);
	PostMessage(WM_SETREDRAW, draw == true);
}
void CxListCtrl::redraw()
{
	Invalidate();
}
void CxListCtrl::redraw(size_t first_row, size_t last_row, size_t first_col, size_t last_col)
{
	RedrawItems(first_row, last_row);
}
void CxListCtrl::redraw_row(size_t first, size_t last)
{
	RedrawItems(first, last);
}
void CxListCtrl::redraw_col(size_t first, size_t last){};
void CxListCtrl::set_row_count(size_t count)
{
	SetItemCount(count);
}
size_t CxListCtrl::get_row_count()
{
	return GetItemCount();
}
size_t CxListCtrl::get_col_count()
{
	if (GetHeaderCtrl())
		return GetHeaderCtrl()->GetItemCount();
	else
		return 0;
}
void CxListCtrl::set_col_count(size_t count)
{
	while (get_col_count() > count)
		DeleteColumn(get_col_count() - 1);

	while (get_col_count() < count)
		InsertColumn(get_col_count(), "");
}
/*************************************************************************************************/

void CxListCtrl::PreSubclassWindow()
{
	if (!m_bUseSystemHeader)
	{
		CHeaderCtrl* pHeader = GetHeaderCtrl();
		if (pHeader)
			VERIFY(m_Header.SubclassWindow(pHeader->GetSafeHwnd()));
		m_Header.SetGap(m_Gap);
	}

	__super::PreSubclassWindow();
}
/************************************************************************************************/

bool CxListCtrl::SetHeaderDrawInfo(HeaderDrawInfo & Info)
{
	return m_bUseSystemHeader ? false : m_Header.SetDrawInfo(Info);
}
void CxListCtrl::GetHeaderDrawInfo(HeaderDrawInfo & Info)
{
	if (!m_bUseSystemHeader)
		m_Header.GetDrawInfo(Info);
}

