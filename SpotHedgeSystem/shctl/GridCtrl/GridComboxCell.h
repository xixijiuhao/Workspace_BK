#ifndef _GRID_COMBOX_CELL_H_
#define _GRID_COMBOX_CELL_H_

class TGridComboxCell : public TGridCellBase
{
public:
	TGridComboxCell(TFilterList* plist);
	~TGridComboxCell();
public:
	virtual			bool				ClickCell(const RECT& rect, const POINT& pt, HWND hwnd = NULL);
protected:
	virtual			void				DrawMain(TMemDC* pmemdc, const RECT& rcmain);
private:
	TFilterList*						m_plist;
};
#endif
