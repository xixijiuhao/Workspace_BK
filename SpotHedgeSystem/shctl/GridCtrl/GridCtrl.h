#ifndef _GRID_CTRL_H_
#define _GRID_CTRL_H_

typedef std::map<int, bool>	MCheck;
/*此控件支持列表显示，暂支持一层叶节点显示*/
class TGridCtrl : public TIxFrm
{
public:
	TGridCtrl();
	virtual ~TGridCtrl();
public:
	bool									Create(HWND hparent);
	void									MoveWindow(int x, int y, int cx, int cy);
	void									GetValidRect(RECT& rect);
	void									RefreshGrid() { if(!m_blockupdate)InvalidateRect(m_Hwnd, 0, false); }
public:
	void									EnableExpandHorz(bool bexpand){ m_bexpand = bexpand; }
	void									EnableMultiSelect(bool benable) { m_bmulselect = benable; }
	void									EnableResizeColumn(bool benable) { m_bresizecol = benable; }
	void									EnableSwapColumn(bool benable) { m_bswapcol = benable; }
/********************************滚动条信息**************************************************************/
public:
	void									SetHorzInfo(SCROLLINFO& stinfo);
	void									GetHorzInfo(LPSCROLLINFO lpinfo);
	_uint									GetVertPos();
	void									SetMoveVertPos(_uint nrange);
/********************************窗口函数**************************************************************/
protected://
	virtual				LRESULT				WndProc(UINT message, WPARAM wParam, LPARAM lParam);
protected://msg
	void									OnPaint();
	void									OnDraw(TMemDC* pmemdc);
	void									OnSize();
	void									OnLButtonDown(WPARAM wparam, LPARAM lparam);
	void									OnMouseMove(WPARAM wparam, LPARAM lparam);
	void									OnMouseLeave();
	void									OnLButtonUp(WPARAM wparam, LPARAM lparam);
	void									OnRButtonDown(WPARAM wparam, LPARAM lparam);
	void									OnDbClickDown(WPARAM wParam, LPARAM lParam);
	void									OnSetCursor();
	void									OnKeyDown(WPARAM wparam, LPARAM lparam);
	void									OnMouseWheel(WPARAM wparam, LPARAM lparam);
	void									OnComboxSel(WPARAM wparam, LPARAM lparam);
/********************************绘制相关**************************************************************/
public:
	void									SetColumnCount(int ncolcount);
	void									SetRowCount(int nrowcount);
	
	void									SetRowHeight(int nheight) { m_nrowheight = nheight; }
	void									SetColWidth(int ncol, int nwidth);

	_uint									GetRowCount() { return m_nrowcount; }
	_uint									GetColumnCount() { return m_ncolumncount; }

	void									SetBkColor(COLORREF clrbk);
	void									SetBdColor(COLORREF clrbd);

	void									SetGridSort(int ncol, GridSort emsort);
	void									LockUpdate() { m_blockupdate = true; }
	void									UnlockUpdate() { m_blockupdate = false;}
private:
	COLORREF								m_clrbk;
	COLORREF								m_clrbd;
	bool									m_bexpand;//是否采用外部扩展的滚动条
	bool									m_bdragging = true;
	bool									m_blockupdate;
	bool									m_btrack;
	GScrollBar								m_dragbar;
protected:
	void									DrawMain(TMemDC* pmemdc,RECT& rect);
	void									DrawRow(TMemDC* pmemdc, RECT& rcrow,int nrow);
	void									DrawCell(TMemDC* pmemdc, RECT&rccell,const TCell& cellid);
	void									DrawExpand(TMemDC* pmemdc, RECT rect,int nvisablerow);
	void									DrawCheck(TMemDC* pmdc, RECT& rect, bool bvalue);
protected:
	void									StandardScrollInfo(GScrollBar embar);
	void									RefreshScrollBar();
	void									ResizeScrollPage();
	void									RefreshScrollInfo();
	void									RefreshScrollRect();
	void									ScrollThumb(POINT pt,GScrollBar embar);
protected:
	_uint									GetColWidth(int ncol);
	_uint									GetVirtualWidth(int ncolcount);
	_uint									GetVirtualHeight(int nrowcount);
	_uint									GetPageCount();
	TCell									GetTopLeftNonFixCell();
	_uint									GetMaxVisableCount();
private:
 	_uint									m_ncolumncount;
 	_uint									m_nrowcount;
	_uint									m_nvirtualwidth;
	_uint									m_npagecount;
private://判断
	bool									IsVertVisable();
	bool									IsHorzVisable();
private:
	bool									m_bvertvisable;
	bool									m_bhorzvisable;
	GridScroll								m_scrollshow;
	POINT									m_dragoffset;
private:
	SCROLLINFO								m_vertscroll;
	SCROLLINFO								m_horzscroll;
	VInt									m_vcolwidth;
	_uint									m_nrowheight	=	25;//行高
	_uint									m_horzbordergap =	 5;
/*********************************************固定行固定列********************************************************/
public:
	void									SetFixRow(int nfix);
	void									SetFixColumn(int nfix);
	int										GetFixColumn() { return m_nfixcolumn; }
protected:
	_uint									GetFixColumnWidth();
	_uint									m_nfixrow;
	_uint									m_nfixcolumn;
/*********************************************MouseMode相关*******************************************************/
protected:
	TCell									GetCellFromPt(const POINT& pt);
	_uint									GetColFromPt(const POINT& pt);
	bool									GetCellOriginPt(const TCell& cellid, POINT& pt);
	void									GetCellRect(const TCell& cellid, RECT& rect);
	bool									IsValidCell(TCell& cellid);
/*调整列宽(固定列不参与调整)*/
	bool									GetResizeColumn(const POINT& pt, int& nresizingcol);
	void									ModifyColumnWidth(const _uint ncol, const _uint nwidth);
/*交换列(固定列不参与调整)*/
	HBITMAP									CreateAlphaCursor(TCell cellid);
	void									SwapColumn(_uint nsrc, _uint ndes);
private:
	TCell									m_hovercell;
	TCell									m_lastclickcell;
	TCell									m_swapsrcclick;
	POINT									m_lastmousept;
	bool									m_blbtndown;
	bool									m_bresizecol;
	bool									m_bswapcol;
	MouseMode								m_stmousemode;
	TGridCursor								m_swapcursor;
/**********************************************选中行相关*********************************************************/
public:
	void									SelectAll();
	void									SetSelectRow(int nselect);
	void									AddSelectRow(int nselect);
	void									RefreshSelectRow(int nrange);
	int										GetSelectRow() { return m_nselectrow; }
	void									GetSelectRows(VInt& stlselect) { stlselect = m_stlselect; }
	bool									IsSelectRow(int nrow);
	void									SetHighLight(COLORREF clrlight) { m_clrhighlight = clrlight; }
protected:
	bool									m_bmulselect;
	int										m_nselectrow;
	VInt									m_stlselect;
	COLORREF								m_clrhighlight;
/**********************************************数据相关*******************************************************/
protected:
	void									NotifyDispData();
public:
	void									SetItem(GV_ITEM* pitem);
	GV_ITEM*								GetGridCell(const TCell& cellid);
	void									GetNonfixDataRange(DataRange& strange, bool bresetpos = false);
	void									ModifyItemExdata(int ncol, int nrow, GridCellType sttype, void* pdata);
	void									SetGridFont(LOGFONT lf);
private:
	TGridData								m_stgriddata;
/**********************************************特殊GridCell****************************************************/
protected:
	TFilterList*							m_pfilterlist;
	HFONT									m_font;
	GridSort								m_emstate;
	int										m_nsortcol;
	int										m_nprecol;

	CellEdit								m_miniedit;
/*******************************************为兼容新需求添加临时Expand部分***************************************/
protected:
	const			_uint					EXPAND_WIDTH = 40;
	const			_uint					EXPAND_WIDTH_EX = 60;
	const			_uint					m_checksize = 14;
	bool									m_bexpandfixcol;
	int										m_nid;
	bool									m_bheadershowid;
	bool									m_bexpandcheck;
	RECT									m_rcexpandfixcol;
	MCheck									m_stlcheck;
	bool									IsShowExpandCol() { return m_bexpandfixcol; }
	void									GetExpandColRect(POINT& pt, RECT& rcinfo);
	void									GetExpandCheck(POINT& pt, RECT& rccheck);
	void									GetExpandCheck(RECT rect, RECT& rccheck);
	_uint									GetExpandWidth();
	void									MoveOverExpandCol(POINT pt);
	void									ClickExpandCol(POINT pt);
public:
	void									SetExpandCheck(int nrow, bool bcheck);
	void									SetExpandCol(bool bexpand, int nid = -1,bool bheader = true,bool bcheck = false);
	/**********************************************************************************************************/
};
#endif