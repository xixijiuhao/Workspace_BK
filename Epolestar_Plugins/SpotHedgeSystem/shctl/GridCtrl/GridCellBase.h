#ifndef _GRID_CELL_BASE_H_
#define _GRID_CELL_BASE_H_

class TGridCellBase
{
public:
	TGridCellBase();
	~TGridCellBase();
public:
	void							SetState(DWORD dwstate) { m_dwstate = dwstate; }
	void							SetHighColor(COLORREF clrhigh) { m_clrhigh = clrhigh; }
	void							SetItem(GV_ITEM* pitem) { m_pitem = pitem; }
	void							SetRect(const RECT& rect) { m_rect = rect; }
public:
	void							Draw(TMemDC* pmemdc, const RECT& rect);
	//返回值： 当返回值为false时 代表ClickCell没有起到作用可继续执行点击效果操作
	virtual			bool			ClickCell(const RECT& rect, const POINT& pt, HWND hwnd = NULL) { return false; }
protected:
	virtual			void			DrawMain(TMemDC* pmemdc, const RECT& rcmain);
protected:
	char*							GetText(){ return m_pitem->szparam; }
	COLORREF						GetFgColor(){ return m_pitem->clrfg; }
	COLORREF						GetFg2Color(){ return m_pitem->clrfg2; }
	COLORREF						GetBkColor(){ return m_pitem->clrbk; }
	LOGFONT*						GetFont(){ return &m_pitem->lgfont; }
	int								GetVMargin(){ return m_pitem->nvmargin; }
	int								GetHMargin(){ return m_pitem->nhmargin; }
	DWORD							GetMask(){ return m_pitem->dwmask; }
	DWORD							GetFormat(){ return m_pitem->dwformat; }
	DWORD							GetState() { return m_dwstate; }
protected:
	GV_ITEM*						m_pitem;
	RECT							m_rect;
	DWORD							m_dwstate;
	COLORREF						m_clrhigh;
};
#endif