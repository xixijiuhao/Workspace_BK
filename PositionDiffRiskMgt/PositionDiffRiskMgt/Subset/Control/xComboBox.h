#pragma once

#include "ACCallback.h"
#include "winconsts.h"
// CxComboBox

#define CBM_FIRST			0x1700
#define CB_SETMINVISIBLE	(CBM_FIRST	+ 1) //�������������ʾ��Item��Ŀ
#define CB_GETMINVISIBLE	(CBM_FIRST	+ 2) //������������ʾ��Item��Ŀ
#define CB_SETCUEBANNER     (CBM_FIRST	+ 3) //���ÿ�ֵ��ʾ
#define CB_GETCUEBANNER     (CBM_FIRST	+ 4) //���ÿ�ֵ��ʾ

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

	bool m_bReadOnly;															//ֻ��
	bool m_bNoPasteCut; 														//��ֹճ��������
	bool m_bNoPopupMenu;														//��ֹ�Ҽ��˵�  
	bool m_bNoUndo;     														//��ֹ����      
	bool m_bNoDelAndBack;														//����ɾ���ͻ���

	bool m_bAutoComplete;														//�û��Ƿ��������Զ����
	bool m_bCanAutoComplete;													//��ǰ״���Ƿ����ִ���Զ����

	CCHAR m_SplitChar;															//���ַָ�����Էָ��Ϊ����ʾ�����ֶ�
	bool m_bTextShowValue;														//�༭�����Ƿ���ʾ�ָ�����������

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

	void AlighValue(bool AuotListWidth=true);		//ʹValue������ʾ, AuotListWidth:List����Ӧ�ı����

	int GetCurrIndex() const;
	int GetIndexByName(const char * Name) const;
	CString GetCurrValue() const;					//��õ�ǰֵ�����m_SplitChar��Ϊ0�򷵻�m_SplitChar��Ĳ��֣����򷵻�""
	CString GetCurrName() const;					
	CString GetName(int Index) const;
	CString GetValue(int Index) const;

	int GetMinVisible();							//��������б���ʾ����
	BOOL SetMinVisible(int Value);					//���������б���ʾ����

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditupdate();
};



