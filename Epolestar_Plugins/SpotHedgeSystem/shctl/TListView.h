#ifndef _T_LISTVIEW_H_
#define _T_LISTVIEW_H_
#include "UIDesign.h"
#include <vector>
using namespace std;

class TListView
{
public:
	TListView();
	~TListView();
	void CreateList(HWND hParent, UINT style, int itemHeight = 20,int headerHeight = 22, UINT exStyle = 0, int index = 0);
	void MoveWindow(int x, int y, int cx, int cy);
	void AddColumn(const TCHAR *pszText, int width = 100, int index = -1);
	void SetColumnText(const TCHAR *pszText, int index);
	void AddItem(const char *pszText, int iRow, int iCol, UINT text_format = 0,COLORREF textColor = -1);
	void AddItem(double data, int iRow, int iCol, int iPresicion = 4, UINT text_format = 0, COLORREF textColor = -1);
	void AddItem(int data, int iRow, int iCol, UINT text_format = 0, COLORREF textColor = -1);
	void DeleteItem(int iItem);
	void DeleteAllItems();
//获取ListView 信息
public:
	int GetItemCount();
	HWND GetListHwnd(){ return m_hwndList; }
	HWND GetHeaderHwnd(){ return m_hwndHeader; }
	int GetSelectItem(char* ptext, int nlength, int iSubitem);
	int GetSelectItem();
	void GetSubItem(int iRow, int iSubItem, char*pText, int length);
//设置ListView 属性
public:
	void SetColor(COLORREF selectColor, COLORREF unSelectColor, COLORREF gridColor);
	void SetTxtColor(COLORREF textColor);
	void SetItemState(int iRow, int iCol, UINT textFormat, COLORREF textColor);
	void SelectItem(int index);
	
//ListView 自绘
public:
	void OnDrawItem(LPARAM lParam);
	void OnMeasureItem(UINT &height);
//Header 重绘处理
private:
	void OnHeaderPaint();
	void OnHeaderLayout(WPARAM wParam, LPARAM lParam);
	void FindBeginEnd(int left, int right, int &iBegin, int &iEnd);
private:
	static LRESULT CALLBACK NewHeaderProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data); //重绘Header
	static LRESULT CALLBACK NewListProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR data);
	BOOL OnEraseBK(WPARAM wParam, LPARAM lParam);
private:
	UINT m_id;
	HWND m_hwndList;
	HWND m_hwndHeader;
	int  m_iHeaderHeight;
	int  m_iItemHeight;
	HBRUSH m_hbr[3];   //Selected,unSelected,Grid
	vector<COLORREF> m_vItemColor; //必须有一个元素，默认item字体颜色
	vector<vector<unsigned short>> m_vItemState;
};

#endif