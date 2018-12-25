#ifndef _GRID_CHECK_CELL_H_
#define _GRID_CHECK_CELL_H_

class TGridCheckCell : public TGridCellBase
{
public:
	TGridCheckCell();
	~TGridCheckCell();
public:
	virtual			bool				ClickCell(const RECT& rect, const POINT& pt, HWND hwnd = NULL);
protected:
	virtual			void				DrawMain(TMemDC* pmemdc, const RECT& rcmain);
private:
	void								DrawFix(TMemDC* pmemdc, const RECT& rect);
};
#endif
