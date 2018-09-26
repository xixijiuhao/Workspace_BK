#ifndef _GRID_TREE_CELL_H_
#define _GRID_TREE_CELL_H_
/************************************************************************/
/* 目前只提供一级子项,可扩展(需填充数据支持)                            */
/************************************************************************/
class TGridTreeCell : public TGridCellBase
{
public:
	TGridTreeCell(){}
	~TGridTreeCell(){}
public:
	virtual			bool		ClickCell(const RECT& rect,const POINT& pt, HWND hwnd = NULL);
protected:
	virtual			void		DrawMain(TMemDC* pmemdc, const RECT& rect);
	void						InitExpand(const RECT& rect, RECT& rcexpand);
private:
	bool						IsExpand();
	bool						IsLeaf();
	int							GetLeafCount();
private:
	const			_uint		m_leftgap = 12;
	const			_uint		m_treebtn = 9;
};
#endif
