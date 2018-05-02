#pragma once

// CxEdit
#include "winconsts.h"

typedef void (CWnd::*MsgProc)(CWnd *, UINT, WPARAM, LPARAM);                  //��Ա����ָ������

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
	TMethod KeyDownProc;                                                        //��Ҫ�ص���������ĺ���
	
	bool m_bNoPasteCut;															//��ֹճ��������
	bool m_bNoPopupMenu;														//��ֹ�Ҽ��˵�
	bool m_bNoUndo;																//��ֹ����
	bool m_bNoDelAndBack;														//����ɾ���ͻ���

	bool m_bTimerEnabled;														//��갴��ʱ����˫�����ʱ��		
	bool m_bMouseClicked;														//��굯��ʱ��ɵ����¼�
	MouseClick MouseClk;														//��굯��ʱ��λ�úͰ�����Ϣ
	void SetIsClick(bool Value);												//�����ֹͣ�����ж�
	void DoLBtnClick(UINT nFlags, CPoint point);								//���͵�����Ϣ
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

	void SetKeyDownProc(CWnd * Owner, MsgProc Value);	//����ʱ���﷨���Դ���COrderDlg�е�edPrice�ؼ�Ϊ����edPrice.SetKeyDownProc(this, static_cast<MsgProc>(&COrderDlg::OnedPriceKeyDown));
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
	//���뱣���� ֻ��ͨ���������������ܻ�����룬�ô����Update()��ȡ����ֵҲ��Ч
	int GetWindowText(_Out_z_cap_post_count_(nMaxCount, return + 1) LPTSTR lpszStringBuf, _In_ int nMaxCount) const;
	void GetWindowText(CString& rString) const;
};


