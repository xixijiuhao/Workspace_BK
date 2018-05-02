#pragma once

#include "ACCallback.h"
#include "winconsts.h"
// CxComboBox

#define CBM_FIRST			0x1700
#define CB_SETMINVISIBLE	(CBM_FIRST	+ 1) //设置下拉类表显示的Item数目
#define CB_GETMINVISIBLE	(CBM_FIRST	+ 2) //获得下拉类表显示的Item数目
#define CB_SETCUEBANNER     (CBM_FIRST	+ 3) //设置空值提示
#define CB_GETCUEBANNER     (CBM_FIRST	+ 4) //设置空值提示

//void AFXAPI DDX_TextX(CDataExchange* pDX, int nIDC, CString& value);

class CxComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CxComboBox)
public:
	CxComboBox();
protected:
	DECLARE_MESSAGE_MAP()

	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	HWND m_EditHandle;
	WNDPROC m_OldEditWndProc;

	bool m_bReadOnly;															//只读
	bool m_bNoPasteCut; 														//禁止粘贴、剪贴
	bool m_bNoPopupMenu;														//禁止右键菜单  
	bool m_bNoUndo;     														//禁止撤销      
	bool m_bNoDelAndBack;														//不能删除和回退

	bool m_bAutoComplete;														//用户是否设置了自动完成
	bool m_bCanAutoComplete;													//当前状体是否可以执行自动完成

	CCHAR m_SplitChar;															//文字分割符，以分格符为界显示两个字段
	bool m_bTextShowValue;														//编辑框里是否显示分割符后面的内容

	typedef LRESULT (CxComboBox::*CLASSWNDPROC)(HWND, UINT, WPARAM, LPARAM);
	ACCallback<WNDPROC, CxComboBox, CLASSWNDPROC> m_dEditWndProc;
	LRESULT EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool m_bTimerEnabled;
	bool m_bMouseClicked;
	MouseClick MouseClk;
	void SetIsClick(bool Value);
	void DoLBtnClick(UINT nFlags, CPoint point);
public:
	bool IsReadOnly();
	void ReadOnly(bool Value=true);
	bool IsNoPasteCut();
	void NoPasteCut(bool Value=true);
	bool IsNoPopupMenu();
	void NoPopupMenu(bool Value=true);
	bool IsNoUndo();
	void NoUndo(bool Value=true);
	bool IsNoDelAndBack();
	void NoDelAndBack(bool Value=true);
	bool IsTextShowValue();
	void TextShowValue(bool Value=true);

	void AutoComplete(bool Value=true);
	void SetSplitChar(char Value=_T(' '));

	int AddString(LPCTSTR lpszString);
	int InsertString(int nIndex, LPCTSTR lpszString);
	int  AddString(CString Name, CString Value);
	int  InsertString(int Index, CString Name, CString Value);
	int  AddString(char * Name, char * Value);
	int  InsertString(int Index, char * Name, char * Value);

	int GetWindowText(_Out_z_cap_post_count_(nMaxCount, return + 1) LPTSTR lpszStringBuf, _In_ int nMaxCount) const;
	void GetWindowText(CString& rString) const;

	void AlighValue(bool AuotListWidth=true);		//使Value对齐显示, AuotListWidth:List自适应文本宽度

	int GetCurrIndex() const;
	int GetIndexByName(const char * Name) const;
	CString GetCurrValue() const;					//获得当前值，如果m_SplitChar不为0则返回m_SplitChar后的部分，否则返回""
	CString GetCurrName() const;					
	CString GetName(int Index) const;
	CString GetValue(int Index) const;

	int GetMinVisible();							//获得下拉列表显示行数
	BOOL SetMinVisible(int Value);					//设置下拉列表显示行数

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditupdate();
};



