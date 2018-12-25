#pragma once

// CxEdit
#include "winconsts.h"

typedef void (CWnd::*MsgProc)(CWnd *, UINT, WPARAM, LPARAM);                  //成员函数指针类型

struct TMethod
{
	CWnd * Data;
	MsgProc Code;
};

class CxEdit : public CEdit
{
	DECLARE_DYNAMIC(CxEdit)

public:
	CxEdit();
	virtual ~CxEdit();
protected:
	DECLARE_MESSAGE_MAP()
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();

private:
	TMethod KeyDownProc;                                                        //需要回调的其他类的函数
	
	bool m_bNoPasteCut;															//禁止粘贴、剪贴
	bool m_bNoPopupMenu;														//禁止右键菜单
	bool m_bNoUndo;																//禁止撤销
	bool m_bNoDelAndBack;														//不能删除和回退

	bool m_bTimerEnabled;														//鼠标按下时激活双击间隔时钟		
	bool m_bMouseClicked;														//鼠标弹起时完成单击事件
	MouseClick MouseClk;														//鼠标弹起时的位置和按键信息
	void SetIsClick(bool Value);												//激活或停止单击判断
	void DoLBtnClick(UINT nFlags, CPoint point);								//发送单击消息
	bool IsPassword() const;
public:
	bool IsNoPasteCut();
	void NoPasteCut(bool Value=true);
	bool IsNoPopupMenu();
	void NoPopupMenu(bool Value=true);
	bool IsNoUndo();
	void NoUndo(bool Value=true);
	bool IsNoDelAndBack();
	void NoDelAndBack(bool Value=true);

	void SetKeyDownProc(CWnd * Owner, MsgProc Value);	//调用时的语法：以窗口COrderDlg中的edPrice控件为例：edPrice.SetKeyDownProc(this, static_cast<MsgProc>(&COrderDlg::OnedPriceKeyDown));
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
protected:
	COLORREF m_colText;
	COLORREF m_colBack;
	CBrush m_brBackGnd;
public:
	void SetTextColor(COLORREF rgb);
	void SetBackColor(COLORREF rgb);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
public:
	//密码保护， 只有通过这两个函数才能获得密码，用窗体的Update()获取关联值也无效
	int GetWindowText(_Out_z_cap_post_count_(nMaxCount, return + 1) LPTSTR lpszStringBuf, _In_ int nMaxCount) const;
	void GetWindowText(CString& rString) const;
};


