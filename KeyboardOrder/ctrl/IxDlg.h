#pragma once

class IxDlg
{
public:
	IxDlg();
	~IxDlg();
public:
	inline HWND GetHwnd() const { return m_hDlg; }
	inline HWND GetParent() const { return m_hParent; }
protected:
	HWND m_hDlg;
	HWND m_hParent;
	//自己处理的消息返回1 ，不处理的消息返回false 。
	virtual INT_PTR DlgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	//创建非模态对话框
	HWND CreateDlg(HINSTANCE hInstance, int idDlg, HWND hWndParent);
	//创建模态对话框
	INT_PTR DlgBox(HINSTANCE hInstance, int idDlg, HWND hWndParent);
private:
	//对话框处理程序
	static INT_PTR CALLBACK IxDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

