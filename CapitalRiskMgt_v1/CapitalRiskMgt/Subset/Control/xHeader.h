#pragma once

enum HeaderDrawInfoItem
{
	hdiFixCol			= 1,
	hdiTextColor		= 1 << 1,
	hdiSelTextColor		= 1 << 2,
	hdiBackColor		= 1 << 3,
	hdiSelBackColor		= 1 << 4,

	hdiLineColor		= 1 << 5,
	hdiShowHorzLine		= 1 << 6,
	hdiShowVertLine		= 1 << 7,
	hdiShowLeftLine		= 1 << 8,
	hdiShowTopLine		= 1 << 9,
	hdiVertLineMargin	= 1 << 10,
	hdiHeight			= 1 << 11,

	hdiAll				= 0xfff
};

//字段类型
enum FieldType
{
	ftString,
	ftNumeric,
	ftDate
};

//字段显示类型
enum FieldShowType
{
	fstHide,	//隐藏
	fstShow,	//显示
	fstFrozen	//雪藏，不显示也不可设制出来
};

#pragma pack(push, 1)
//listctrl 字段结构配置
struct HeadField
{
	int				id;             //字段编号，对应的ColumnHeadField里的内容
	int				width;          //显示宽度(像素数)
	char			text[32];		//字段的标题
	int				align;			//对齐方式，LVCFMT_LEFT：左对齐，LVCFMT_CENTER：居中，LVCFMT_RIGHT：右对齐
	FieldType		type;			//字段的类型也可以配置上，例如是字符串啊，还是数值，还是日期等等
	FieldShowType	ShowType;		//显示类型
	int				iSortType;		//1升序，-1降序，0无序
	bool			bSorted;		//当前排序字段
};
#pragma pack(pop)

struct HeaderDrawInfo
{
	UINT	 nSize;					//结构体大小
	UINT	 nMask;					//参数掩码，可以是多个HeaderDrawInfoItem

	int		 nFixCol;				//锁定列
	COLORREF clrBack;				//表格背景
	COLORREF clrSelBack;			//选中项背景
	COLORREF clrText;				//表格文字颜色
	COLORREF clrSelText;			//选中项文字颜色

	COLORREF clrLine;				//两列中间分割线的颜色
	BOOL	 bShowHorzLine;
	BOOL	 bShowVertLine;
	UINT	 nVertLineMargin;		//两列中间分割线距上下边的距离
	BOOL	 bShowLeftLine;			//绘制左边线，bShowVertLine为True时生效
	BOOL	 bShowTopLine;			//绘制顶边线，bShowHorzLine为True时生效
	UINT	 nHeight;				//高度
public:
	HeaderDrawInfo(){ memset(this, 0, sizeof(*this)); nSize = sizeof(*this); }
};

class CxHeader : public CHeaderCtrl
{
public:
	CxHeader();
protected:
	HeaderDrawInfo m_DrawInfo;
	char	m_lpBuffer[MAX_PATH];
	CFont	m_OutFont;				//仅为GetFont只用
	UINT	m_Gap;

	typedef POINT TSortPoints[3];
	int		m_SortItem;
	int		m_SortType;
	POINT	m_SortPoints[4];
	void GetSortIconPoints(CRect & rect, int topGap = 2, int iconHeight = 4);
public:
	LOGFONT m_LogFont;
	void SetFont(CFont * cFont, BOOL bRedraw = TRUE);
	CFont * GetFont();
	bool SetDrawInfo(HeaderDrawInfo & Info);
	void GetDrawInfo(HeaderDrawInfo & Info);
	int	 GetItemIndex(POINT pt);	
	void SetSortIcon(int Column, int Sort);
	void SetGap(UINT value = 4);
protected:
	DECLARE_MESSAGE_MAP();
	virtual BOOL OnChildNotify(UINT msg, WPARAM wp, LPARAM lp, LRESULT* pRes);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();	
	void DoOnPaint(CDC & dc);
};
