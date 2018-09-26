#ifndef _GRID_EDIT_CELL_H_
#define _GRID_EDIT_CELL_H_

class TGridEditCell : public TGridCellBase
{
public:
	TGridEditCell(CellEdit* pedit);
	~TGridEditCell();
public:
	virtual			bool				ClickCell(const RECT& rect, const POINT& pt, HWND hwnd = NULL);
protected:
	virtual			void				DrawMain(TMemDC* pmemdc, const RECT& rcmain);
protected:
	CellEdit *							m_edit;
public:
	int									m_nid;
	WNDPROC								m_ownerProc;
	static		LRESULT	CALLBACK		EditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};
#endif
