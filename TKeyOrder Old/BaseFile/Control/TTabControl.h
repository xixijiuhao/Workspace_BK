#ifndef T_TABS_H
#define T_TABS_H


#define TABS_NORMAL    0x001
#define TABS_SELECTED  0x002
#define TABS_HOVER     0x004
typedef struct TabContrl
{
	int		iIndex;//从0开始
	std::string	sztext;//标题
	int		iState;//状态
	bool	bEnable;//可用
	bool	bShow;//显示
	RECT    tabRect;//位置

	TabContrl()
	{
		iState = TABS_NORMAL;
		memset(&tabRect, 0, sizeof(RECT));
		iIndex = -1;
		bShow = true;
		bEnable = true;
	}
	bool operator==(const TabContrl& node)
	{
		return(node.iIndex == iIndex);
	}
}TabNode;


class DrawDownTabs;
class TTabs :public TIxFrm
{
public:
	TTabs();
	~TTabs();

	bool Create(HWND hParent);
	void MoveWindow(const int& x, const int& y, const int& cx, const int& cy);
	int GetSelIndex(){ return m_iSelIndex; }
	const TabContrl* GetSelItem();
	void CalculateTabRect();
	void SetData(const std::vector<std::string>&tabstrVec);
protected:
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void	OnPaint();
	virtual void    DrawTab(TMemDC *pmemdc);
	virtual void    DrawButton(TMemDC *pmemdc);
	virtual void	OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void    OnDrawButtonDown();
	void	ShowHideIndex();
	void	OnComboxSelect(WPARAM wParam, LPARAM lParam);
private:
	int		m_iHeight;
	int		m_iWidth;
	int		m_iTabWidth;
	int		m_iTabHeight;
	int		m_iEdge;//梯形的斜边宽的一半
	int		m_iSelIndex;//选中的index
	int     m_iMaxDisplay;//最多显示几个
	int     m_iBeginIndex;
	int		m_iEndIndex;
	HFONT	m_hfont;
	std::vector<TabContrl>		m_TabVec;
	//std::vector<TabContrl>		m_DrawMenuVec;//存放下拉菜单中的结构体？感觉没必要

	int		m_iButtonWidth;//下拉键宽度
	bool	m_bButton;//是否显示下拉键
	bool    m_bButtonHover;//是否经过下拉键
	RECT	m_rcButton;

	TComboxList *m_pcomlist;

	COLORREF m_clrNormal;
	COLORREF m_clrSelect;
	COLORREF m_clrFrmHover;
	COLORREF m_clrFrame;
};



class DrawDownTabs
{

};

#endif