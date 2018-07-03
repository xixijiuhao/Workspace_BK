#pragma once

class TListBox 
{
public:
	TListBox();
	~TListBox();
	void Create(HWND hParent, UINT style = 0, int id = 0);
	void AddItem(const WCHAR* pszText);
	void AddItem(const char* pszText);
	void DeleteItem(int Index);
	HWND GetHwnd() { return m_hListBox; }
	void OnDrawItem(WPARAM wParam, LPARAM lParam);
	void OnMeasureItem(WPARAM wParam, LPARAM lParam);
	void Clear();
private:
	static LRESULT CALLBACK	ListBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
private:
	HWND  m_hListBox;
	int   m_iItemHeight;
};