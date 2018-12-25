#ifndef _GRID_FILTER_CELL_H_
#define _GRID_FILTER_CELL_H_

class TGridHeaderCell : public TGridCellBase
{
public:
	TGridHeaderCell(TFilterList* plist);
	~TGridHeaderCell();
public:
	virtual			bool				ClickCell(const RECT& rect, const POINT& pt,HWND hwnd = NULL);
protected:
	virtual			void				DrawMain(TMemDC* pmemdc, const RECT& rcmain);
private:
	void								InitFilter(const RECT& rect, RECT& rcfilter);
	bool								IsSetFilter();
private:
	//	GridSort						m_emstate;
	TFilterList*						m_plist;
	const			_uint				m_filterwidth = 17;
};
#endif
