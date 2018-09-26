#ifndef _GRID_DATA_H_
#define _GRID_DATA_H_

#define _Width(rect)		(rect.right - rect.left)
#define _Height(rect)		(rect.bottom - rect.top)

#define VertScroll_Width		17
#define HorzScroll_Height		17

//GridCellType
typedef enum tagGridCellType
{
	CellType_Base = 0,
	CellType_Header,
	CellType_Commbox,
	CellType_Edit,
	CellType_Tree,
	CellType_Filter,
	CellType_EditFilter,
	CellType_CheckBox,
	CellType_EditFilter1,
	CellType_CellEdit
}GridCellType;

//GridHeader 
typedef enum tagGridSort
{ 
	Header_Null = 0,
	Header_Normal, 
	Header_Upper, 
	Header_Down 
}GridSort;

//滚动条类型
typedef enum tagGScrollBar
{
	GSB_NULL = 0,
	GSB_VERT,
	GSB_HORZ,
	GSB_BOTH
}GScrollBar;
//鼠标模式
typedef enum tagMouseMode
{
	MouseMode_Normal = 0,
	MouseMode_Resizing,
	MouseMode_ResizeCol,
	MouseMode_SwapCol
}MouseMode;

//鼠标action
typedef enum tagGMouseAction
{
	GVA_LCLK = 0,
	GVA_RCLK,
	GVA_DBCLK
}GMouseAction;

//通知消息
typedef enum tagGNotify
{
	GN_DISPINFO = 0,
	GN_TREEXPAND
}GNotify;

//定义表格消息
#define SSWM_GRID_BEGIN						WM_USER + 600
#define	SSWM_GRID_CTRL_ACTION				SSWM_GRID_BEGIN + 1
#define SSWM_FILTER_LIST_UPDOWN				SSWM_GRID_BEGIN + 2
#define SSWM_FILTER_LIST_WHEEL				SSWM_GRID_BEGIN + 3
#define SSWM_FILTER_EDIT_RETURN				SSWM_GRID_BEGIN + 4
#define SSWM_FILTER_LIST_SELECT				SSWM_GRID_BEGIN + 5
#define SSWM_FILTER_COMBOX_SELECT			SSWM_GRID_BEGIN + 6
#define SSWM_TREE_EXPAND					SSWM_GRID_BEGIN + 7
#define SSWM_MINIEDIT_CHANGE				SSWM_GRID_BEGIN + 8
#define SSWM_FILTER_CHANGE					SSWM_GRID_BEGIN + 9
#define SSWM_SORT_FILTER_DATA				SSWM_GRID_BEGIN + 10
//定义表格状态
#define GVIS_FOCUSED						0x00000000L
#define GVIS_SELECTED						0x20000000L
#define GVIS_GRIDLINE						0x40000000L
//mask
#define GVIF_TEXT							0x00000001
#define GVIF_STATE							0x00000008
#define GVIF_BKCLR							(GVIF_STATE<<1)
#define GVIF_FGCLR							(GVIF_STATE<<2)
#define GVIF_FONT							(GVIF_STATE<<3)
#define GVIF_VMARGIN						(GVIF_STATE<<4)
#define GVIF_HMARGIN						(GVIF_STATE<<5)
#define GVIF_FORMAT							(GVIF_STATE<<6)
#define GVIF_FGCLR2							(GVIF_STATE<<7)
#define GVIF_INT							(GVIF_STATE<<8)
#define	GVIF_REAL							(GVIF_STATE<<9)
#define GVIF_ALL							(GVIF_TEXT|GVIF_STATE|GVIF_BKCLR|GVIF_FGCLR|GVIF_FONT|GVIF_VMARGIN|GVIF_HMARGIN|GVIF_FORMAT)

typedef unsigned int			_uint;
//滚动条展示信息
typedef struct tagGridScroll
{
	//竖向
	RECT			rcvert;//滚动区域
	_uint			vertthumb;
	_uint			vertbegin;
	bool			bvertover;
	//横向
	RECT			rchorz;
	_uint			horzthumb;
	_uint			horzbegin;
	bool			bhorzover;

	RECT			rcunion;
	//颜色
	COLORREF		clrbk;
	COLORREF		clrthumb;
}GridScroll;

//
struct TCell
{
	int nrow;
	int ncol;
	
	TCell()
	{
		nrow = ncol = -1;
	}
};

typedef struct tagDataRange
{
	_uint			nbeginrow;
	_uint			nendrow;
	_uint			nbegincol;
	_uint			nendcol;
	tagDataRange()
	{
		nbeginrow = nendrow = 0;
		nbegincol = nendcol = 0;
	}
}DataRange;
/******************************GridCell 数据结构*************************/
/* 结构中除cellid，emtype,exdata外，其余字段赋值都需dwmask中支持          */
/************************************************************************/

//TreeGridCell扩展结构
typedef struct tagGridExpand
{
	bool		bexpand;
	bool		bleaf;
	_uint		ncount;
	tagGridExpand()
	{
		memset(this, 0, sizeof(tagGridExpand));
	}
}GridExpand;

//GridFilterCell扩展结构

/**/
typedef struct tagGV_ITEM
{
	TCell							cellid;
	DWORD							dwmask;
	DWORD							dwformat;
	_uint							nvmargin;
	_uint							nhmargin;

	LOGFONT							lgfont;
	COLORREF						clrbk;
	COLORREF						clrfg;
	COLORREF						clrfg2;
	GridSort						emsort;
	GridCellType					emtype;
	char							szparam[MAX_PATH];

	bool							b_top;
	bool							b_bottom;
	bool							b_left;
	bool							b_right;

	//扩展外部数据(供特殊表格类型使用,内存在需要时申请)
	void*							exdata;
	tagGV_ITEM()
	{
		memset(this, 0, sizeof(tagGV_ITEM));
		emtype = CellType_Base;
		emsort = Header_Null;
		clrbk = RGB(243, 243, 243);
		b_top = false;
		b_bottom = true;
		b_left = false;
		b_right = true;
	}
}GV_ITEM;

//避免内存泄露(释放GV_ITEM中的void*)
void deletevoidpointer(GV_ITEM* pitem);

typedef struct tagGV_CLKINFO
{
	TCell				cellid;
	GMouseAction		naction;
}GV_CLKINFO;

typedef vector<_uint>			VInt;
typedef vector<GV_ITEM*>		MRowData;
typedef vector<MRowData>		MGridData;

class TGridData
{
public:
	TGridData();
	~TGridData();
public:
	void				InitGridData(const _uint& ncolcount,const _uint& nrowcount);
	GV_ITEM*			GetGridCell(const _uint& ncolindex,const _uint& nrowindex);
	MGridData*			GetGridData() { return &m_stdata; }
private:
	void				AddGridRow(const MRowData& stdata);
	void				Clear();
private:
	MGridData			m_stdata;
};

class CellEdit;
struct GridEditParam
{
	int			nrow;
	int			ncol;
	int			nsize;
	CellEdit*	pedit;

	GridEditParam()
	{
		memset(this, 0, sizeof(GridEditParam));
	}
};
//////////////////////////////////////////////////////////////////////////
//FilterList MiniEdit
#define Caret_XGap			 1
#define Begin_Code			32
#define EditText_Begin		 3
#define EditText_End		 3
#define EditBorder			 1
#define TimerLeft_ID		 1
#define TimerRight_ID		 2

/////////////////////////////////////////////////////////////////////////
//MiniEdit

#define MAX_FE_BUFFER 256

// Macros
#define CURSOR_POS (m_TupleData[m_nCurrentTuple].nCursorPos)
#define BUFFER(a) (m_TupleData[a].szBuffer)
#define CURRENT_BUFFER BUFFER(m_nCurrentTuple)
#define CURRENT_TUPLE (m_TupleData[m_nCurrentTuple])

#define SELECTION_EQUAL(sel1, sel2) ( (sel1.nTupleIndex == sel2.nTupleIndex) \
										&& (sel1.nPosition == sel2.nPosition) )

// Window Style Flags
#define FES_NUMERIC			 1  // Allows only the input of numbers.
#define FES_UPPERCASE		 2  // Converts lowercase characters to uppercase.
#define FES_LOWERCASE		 4  // Converts uppercase characters to lowercase.
#define FES_DISABLESPACE	 8  // Space character is ignored.
#define FES_CENTERTEXT		16  // The text is centered in the view.
#define FES_READONLY        32

// Text styles
#define TEXTFORMAT_BOLD      1
#define TEXTFORMAT_ITALIC    2
#define TEXTFORMAT_UNDERLINE 4

// Popup Menu ID's
#define IDM_CUT            1
#define IDM_COPY           2
#define IDM_PASTE		   3
#define IDM_CLEARTUPLE     4
#define IDM_CLEARALLTUPLES 5
#define IDM_SELECTALL      6

class MiniEdit;

typedef struct _tagWindowListEntry
{
	HWND hWnd;
	MiniEdit *pEdit;

} WINDOWLISTENTRY, *LPWINDOWLISTENTRY;

typedef std::list<WINDOWLISTENTRY>  WindowList;

typedef struct _tagTupleData
{
	int    nIndex;
	TCHAR *szBuffer;
	int    nCursorPos;
	int    nTextLimit;

} TUPLEDATA, *LPTUPLEDATA;

typedef std::vector<TUPLEDATA> TupleData;
typedef struct _tagFormattedTextBlock
{
#ifndef UNICODE
	std::string  strBlock;
#else
	std::wstring strBlock;
#endif
	COLORREF     clColor;
	DWORD        dwStyle;

} FORMATTEDTEXTBLOCK, *LPFORMATTEDTEXTBLOCK;

typedef std::vector<FORMATTEDTEXTBLOCK> FormattedText;
typedef LRESULT(CALLBACK *FormatTextProc)(int, const TCHAR*, FormattedText&);
// Version 1.1
typedef struct _tagTuplePos
{
	int nTupleIndex;
	int nPosition;
} TUPLEPOS, *LPTUPLEPOS;

typedef struct _tagSelection
{
	TUPLEPOS start;
	TUPLEPOS end;
} SELECTION, *LPSELECTION;

#endif