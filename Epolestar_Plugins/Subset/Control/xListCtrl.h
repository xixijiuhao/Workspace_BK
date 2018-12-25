#pragma once

#include "ITreeNode.h"
#include "xHeader.h"

//单元格字符串最大长度/////////////////////////////////////////////////////////////////////////////////
#define MAX_LIST_FIELD_BUF				MAX_PATH	//MAX_LIST_FIELD_BUF必须小于527，否则自适应列宽出错


class CxListCtrl : public CListCtrl, public IGridRef
{
	DECLARE_DYNAMIC(CxListCtrl)
public:
	CxListCtrl(bool UseSystemHeader = false);
	virtual ~CxListCtrl();
protected:
	DECLARE_MESSAGE_MAP()
		
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	
	void DoDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct, CDC * pDC);
private:
	UINT		m_nCXSMIcon;
	UINT		m_nCYSMIcon;
	UINT		m_nItemHeight;
	UINT		m_nLvirIcon;				//图标宽度
	bool		m_bShowCheckBox;
	bool		m_bShowTreeBtn;
	bool		m_bUseSystemHeader;
	CImageList	m_CheckImgList;
	CImageList	m_TreeImgList;
	TCHAR		m_lpBuffer[MAX_LIST_FIELD_BUF];
	CFont		m_OutFont;				   //仅为GetFont只用

	UINT		m_Gap;
public:
	LOGFONT		m_LogFont;	
	COLORREF	m_BackgroundColor;			//空白区背景
	COLORREF	m_BackColor;				//当前行背景
	COLORREF	m_SelBackColor;		
	COLORREF	m_TextColor;
	COLORREF	m_SelTextColor;
	COLORREF	m_LineColor;				//网格线颜色
	BOOL		m_bShowHorzLine;			//横向显示网格线
	BOOL		m_bShowVertLine;			//竖向向显示网格线
	BOOL		m_bDrawSelFrame;			//显示选中框
	BOOL		m_bShowLeftLine;			//显示左边线，m_bShowVertLine为True时生效


	int GetFirstSelIndex();
	void SetHeaderFont(CFont * hFont, BOOL bRedraw = TRUE);
	CFont * GetHeaderFont();
	void SetFont(CFont * cFont, BOOL bRedraw = TRUE);
	CFont * GetFont();
	void SetItemHeight(UINT nHeight);
	UINT GetItemHeight();
	UINT GetStateImageMask_0(bool Value);	//获得TreeBtn的对应状态图标
	UINT GetStateImageMask_1(bool Value);	//获得CheckBox的对应状态图标
	void ShowCheckBox(bool ShowCheck=false);//显示CheckBox
	bool ItemChecked(int iItem);			//返回CheckBox的选中状态
	bool InCheckBox(int iItem);				//判断当前鼠标是否在CheckBox上
	void ShowTreeBtn(bool ShowTree=false);	//显示TreeBtn	
	bool InTreeBtn(int iItem);				//判断当前鼠标是否在TreeBtn上
	UINT GetColWidthDx(int iSubItem);		//获得该列的宽度增量（在标准宽度基础上的增量）	
	void AutoColumnWidth(int Column = -1, bool CurrPage = true);	//自适应列宽Column>=0则调整指定列，否则调整全部列
	void SetSortIcon(int Column, int Sort);	//设置排序列,用于绘制列头图标
protected:	
	//获得图标矩形区域，iOrder指出要获得图标在单元格里的排列顺序，从零开始；iIndent
	bool GetIconRect(CRect & rcIcon, int iOrder, int iItem, int iSubItem = 0);	
	bool InIcon(int iOrder, int iItem, int iSubItem = 0);

	void DoSetHeight();
public:	
/******************************************IGridRef***********************************************/
	virtual void set_draw(bool draw);
	virtual void redraw();
	virtual void redraw(size_t first_row, size_t last_row, size_t first_col, size_t last_col);
	virtual void redraw_row(size_t first, size_t last);
	virtual void redraw_col(size_t first, size_t last);
	virtual void set_row_count(size_t count);
	virtual size_t get_row_count() ;
	virtual size_t get_col_count();
	virtual void set_col_count(size_t count);
private:
/*************************************************************************************************/
	CxHeader m_Header;
public:
	virtual void PreSubclassWindow();
	bool SetHeaderDrawInfo(HeaderDrawInfo & Info);
	void GetHeaderDrawInfo(HeaderDrawInfo & Info);
};



//LV_ITEM.state字段操作宏////////////////////////////////////////////////////////////////////////////
//因为LV_ITEM的State字段从第二个字节开始就表示图片索引了
//这个控件里所有图标都是自绘实现的，所以State字段从第二个字节开始就可以自定义使用了，类似的iImage其实也可以自定义使用
#define IMAGE_BIT_COUNT					4	//每种组图标索引所占bit数，这个值不一样，Item前最大可显示的图标数也不一样(4个bit可以包含16个图片索引（图标组可以是覆盖图标、复选框图标、树的展开与合并图标等，自由定制，最大支持6组图标，再加上iImage这样一个Item前最多可以显示7组图标）
#define FIRST_IMAGE_BIT					8	//第一个图标索引在state字段中的bit位置

#define IMAGE_BIT(n)					(FIRST_IMAGE_BIT+(n)*IMAGE_BIT_COUNT)			//第n组图标存储位置的起始bit位，n从0开始
#define IMAGE_MASK(n)					(((1<<IMAGE_BIT_COUNT) -1) << IMAGE_BIT(n))		//第n组图标的存取掩码

#define SET_STATEIMAGE_INDEX(i, n)		(((i) + 1) << IMAGE_BIT(n))						//设置第n组图标索引
#define GET_STATEIMAGE_INDEX(s, n)		((((s) & IMAGE_MASK(n)) >> IMAGE_BIT(n)) - 1)	//取得第n组图标索引


//0组 等同于OverlayIndex
#define LVIS_STATEIMAGEMASK_0			IMAGE_MASK(0)									//Index存取掩码
#define INDEX_TO_STATEIMAGEMASK_0(i)	SET_STATEIMAGE_INDEX(i, 0)						//将Index转为状态值
#define STATEIMAGEMASK_TO_INDEX_0(s)	GET_STATEIMAGE_INDEX(s, 0)						//将状态值转为Index

//1组 等同于StateIndex
#define LVIS_STATEIMAGEMASK_1			IMAGE_MASK(1)
#define INDEX_TO_STATEIMAGEMASK_1(i)	SET_STATEIMAGE_INDEX(i, 1)
#define STATEIMAGEMASK_TO_INDEX_1(s)	GET_STATEIMAGE_INDEX(s, 1)

//2组 扩展的图标类型
#define LVIS_STATEIMAGEMASK_2			IMAGE_MASK(2)
#define INDEX_TO_STATEIMAGEMASK_2(i)	SET_STATEIMAGE_INDEX(i, 2)
#define STATEIMAGEMASK_TO_INDEX_2(s)	GET_STATEIMAGE_INDEX(s, 2)

//3组 扩展的图标类型......
//4组 扩展的图标类型......
//5组 扩展的图标类型......

///////////////////////////////////////////////////////////////////////////////////////////////////