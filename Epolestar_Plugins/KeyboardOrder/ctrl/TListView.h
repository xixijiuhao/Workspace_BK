#ifndef _T_LISTVIEW_H_
#define _T_LISTVIEW_H_

class TListView
{
public:
	TListView();
	~TListView();
	void CreateList(HWND hParent, UINT style, int itemHeight = 20,int headerHeight = 22, UINT exStyle = 0, int index = 0);
	void MoveWindow(int x, int y, int cx, int cy);
	void AddColumn(const TCHAR *pszText, int width = 100, int index = -1);
	void AddItem(const char *pszText, int iRow, int iCol, UINT text_format = 0,COLORREF textColor = -1);
	void AddItem(const TCHAR *pszText, int iRow, int iCol, UINT text_format = 0, COLORREF textColor = -1);
    void ModifyItem(const char* pszText, int iRow, int iCol, UINT textFormat = 0, COLORREF textColor = -1);

	void AddItem(double data, int iRow, int iCol, UINT text_format = 0, COLORREF textColor = -1);
	void AddItem(double data, int iRow, int iCol,const char* priFormat,  UINT text_format = 0, COLORREF textColor = -1);
	void AddItem(int data, int iRow, int iCol, UINT text_format = 0, COLORREF textColor = -1);
	void DeleteItem(int iItem);
	void DeleteAllItems();
//获取ListView 信息
public:
	int GetItemCount();
	HWND GetListHwnd(){ return m_hwndList; }
	int GetSelectItem(char* ptext, int nlength, int iSubitem);
	int GetSelectItem();
//设置ListView 属性
public:
	void SetColor(COLORREF selectColor, COLORREF unSelectColor, COLORREF gridColor);
	void SetTxtColor(COLORREF textColor);
	void SetItemState(int iRow, int iCol, UINT textFormat, COLORREF textColor);
	void SelectItem(int index);
	void SetItemCount(int count);
//ListView 自绘
public:
	void OnDrawItem(LPARAM lParam);
	void OnMeasureItem(UINT &height);
	void OnGetDispInfo(LPARAM lParam);
//Header 重绘处理
private:
	void OnHeaderPaint();
	void OnHeaderLayout(WPARAM wParam, LPARAM lParam);
	void FindBeginEnd(int left, int right, int &iBegin, int &iEnd);
	BOOL OnEraseBK(WPARAM wParam, LPARAM lParam);
	void AddItemVM(const char *_pchar, int iRow, int iCols);//VM means Vritual Mode
	void AddItemVM(const TCHAR *_pchar, int iRow, int iCols);
private:
	static LRESULT CALLBACK NewHeaderProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data); //重绘Header
	static LRESULT CALLBACK NewListProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data);
private:
	HWND m_hwndList;
	HWND m_hwndHeader;
	int  m_iHeaderHeight;
	int  m_iItemHeight;
	HBRUSH m_hbr[3];   //Selected,unSelected,Grid
	vector<COLORREF> m_vItemColor; //必须有一个元素，默认item字体颜色
	vector<vector<unsigned short>> m_vItemState;

	vector<vector<std::wstring>>  m_vecRowData;
	bool   m_bVirtualMode;
};

#endif