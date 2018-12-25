#ifndef _TFILTER_LIST_H_
#define _TFILTER_LIST_H_

//Edit
typedef struct tagFilterEdit
{
	wchar_t			wsztext[49];
}FilterEdit;
//EditList、list
typedef struct tagFilterList
{
	char		sztext[50 + 1];//条件名称
	char		szvalue[50 + 1];//名称对应的含义
	char		szvalue2[50 + 1];
	bool		bcheck;
	bool		bshowvalue;

	tagFilterList()
	{
		memset(this, 0, sizeof(tagFilterList));
	}
	bool operator==(const char* ptext)
	{
		if (strcmp(ptext, sztext) == 0)
			return true;
		return false;
	}
}GFilter;

typedef vector<GFilter>		VFilter;
void VFilter_Clear(VFilter* stfilter);
void VFilter_Add(VFilter* stfilter, const GFilter& sfilter);
void VFilter_SelectItem(VFilter* stfilter, const char* ptext,bool bsingle = false);
int VFilter_GetSelectItem(VFilter* stfilter, string& szstring,bool bhaveall = true,VInt* stlint = NULL);
class TFilterList : public TPopupWnd
{
public:
	TFilterList();
	~TFilterList();
	enum{Filter_List = 0,Filter_Edit,Filter_EditList, Filter_Combox,Filter_EditList_Single};
	enum{ListPos_LeftTop = 0,ListPos_LeftBottom,ListPos_RightTop};
public:
	bool							_Create(HWND hparent);
	void							_MoveWindow(int xpos, int ypos, int nalign = ListPos_LeftBottom);
	virtual			void			ShowWindow(int nCmdShow);
public:
	void							SetFont(HFONT hfont) { m_font = hfont; m_edit.SetFont(hfont);}
	void							SetData(void* pdata, TCell cell,int ntype = Filter_EditList);
protected:
	virtual			void			OnPaint();
	virtual			void			OnArrowUpDown(WPARAM wparam, LPARAM lParam);
	virtual			void			OnMouseWheel(WPARAM wParam, LPARAM lParam);
	virtual			void			OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual			void			OnMouseLeave();
	virtual			LRESULT			WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual			void			OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual			void			OnLButtonUp(WPARAM wparam, LPARAM lparam);
	virtual			void			OnArrowReturn();
	virtual			void			OnEditFilterReturn();
protected:
	void							DrawMain(TMemDC* pmemdc,RECT rect);
	void							DrawCheck(TMemDC* pmemdc, RECT& rect, bool bvalue,bool bselect,bool bpartcheck);
protected:
	HFONT							m_font;
	VFilter*						m_stldata;
	FilterEdit*						m_cpfilter;
	TCell							m_cell;
	int								m_nselect;
	int								m_nbeginrow;
	int								m_nlistwidth = 145/*152*/;
	int								m_nlistheight = 177;
	int								m_nwndheight = 177;
	int								m_ntype;
	int								m_nvisablerow = 7;
public:
	const			int				m_nrowheight = 25;
	const			int				m_leftmargin = 7;
	const			int				m_rightmargin = 7;
	const			int				m_nvertwidth = 7;
	const			int				m_checksize = 14;
protected:
	int								m_nvertheight;
	RECT							m_rcthumb;
	bool							m_bdragging;
	POINT							m_dragoffset;
protected:
	void							Resize();
	void							ResetParam();
	void							ScrollTo(POINT point);
	_uint							GetFilterSize();
	int								GetFilterData(const _uint nindex,GFilter& stdata);
	void							ModifyFilterCheck(const _uint nindex);
	void							ModifyAllCheck();
	void							CalFilterWidth(int& nwidth);
	bool							IsPartCheck();
/////////////////////////////////嵌入Edit//////////////////////////////////////////////////////////////////
protected:
	bool							m_bshowedit;
	MiniEdit						m_edit;
private://筛选
	VInt							m_subdata;
	wchar_t							m_wszpretext[49];
	void							SubFilterProc(int nkey = 0);
	bool							_wcsstr(const wchar_t* wscdes, const wchar_t* wscsrc);
	bool							_strstr(const char* scdes, const char* scsrc);
};
#endif