#ifndef _DUI_TYPE_
#define _DUI_TYPE_

#ifndef _STAR_DUI_LIB_
#define _STAR_DUI_CLASS_ __declspec(dllimport)
#else
#define _STAR_DUI_CLASS_ __declspec(dllexport)
#endif

//base
enum TDuiDirection 
{
	D_LEFT,
	D_RIGHT,
	D_UP,
	D_DOWN
};

//button
enum TDuiButtonState
{
	STATE_NORMAL,
	STATE_HOVER,
	STATE_DOWN
};

//popmenu
struct TDuiPopMenuItem
{
	int										Index;						//自定义索引号
	wchar_t									Text[51];					//左边文字
	wchar_t									RightText[21];				//右边文字
	bool									Begin;						//分组开始，会有一条分割线
	bool									IsChecked;					//是否选中
	void*									Value;
};
typedef std::vector<TDuiPopMenuItem> TDuiPopMenuItemVectorType;
typedef std::multimap<int, TDuiPopMenuItem> TDuiPopMenuSubItemMapType;

//listview
typedef int				TDuiListViewIndexType;
typedef wchar_t			TDuiListViewTextType[51];
typedef unsigned char	TDuiListViewAlignType;
typedef size_t			TDuiListViewSizeType;
typedef short			TDuiListViewColWidthType;
typedef bool			TDuiListViewVisibleType;

enum TDuiListViewColType
{
	DUI_LISTVIEW_INT,
	DUI_LISTVIEW_DOUBLE,
	DUI_LISTVIEW_STR,
	DUI_LISTVIEW_WSTR,
	DUI_LISTVIEW_CHECK,						//单选			有鼠标浮动效果
	DUI_LISTVIEW_COMBO,						//下拉选择		有鼠标浮动效果
	DUI_LISTVIEW_VOID,						//指针值
};
const wchar_t DUI_LISTVIEW_LIST_SPLIT = L'|';

struct TDuiListViewCol						
{
	TDuiListViewIndexType		Index;			//索引											addcol时自动计算
	TDuiListViewColType			Type;			//类型
	TDuiListViewAlignType		Align;			//对齐方式
	TDuiListViewTextType		Text;			//标题
	TDuiListViewColWidthType	PixelLeft;		//左边界										addcol时自动计算
	TDuiListViewColWidthType	PixelWidth;		//列宽度
	
	TDuiListViewSizeType		ValuePos;		//行开始写值位置								addcol时自动计算		
	TDuiListViewSizeType		ValueSize;		//值占内存大小, 一般仅在使用wstr类型时使用		addcol时自动计算	
	TDuiListViewTextType		ComboValue;		//列表类型时使用								DUI_LISTVIEW_LIST_SPLIT分割
	TDuiListViewVisibleType		Visible;		//是否显示列，计算数据，不计算宽度
};
typedef std::vector<TDuiListViewCol> TDuiListViewColVectorType;

//初始化函数------------------------------------------------------------------------------------------------------------------------------
void _STAR_DUI_CLASS_ InitDuiRes(bool IsEnu = false);

//判断字体是否存在----------------------------------------------------------------------------------------------------------------------------
bool _STAR_DUI_CLASS_ IsFontExit(LPCWSTR lptext);

#endif