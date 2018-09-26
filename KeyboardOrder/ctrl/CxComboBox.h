#pragma once

static UINT WM_LBUTTONCLK = RegisterWindowMessage(L"WM_LBUTTONCLK");

#pragma pack(push, 1)
struct _ACCallbackOpCodes
{
	unsigned char tag;// CALL 0xE8
	LONG_PTR offset;  // offset (dest - src - 5, 5=sizeof(tag + offset))
	LONG_PTR _this;   // a this pointer
	LONG_PTR _func;   // pointer to real member function address
};
#pragma pack(pop)

static __declspec(naked) int STDACJMPProc()
{
	_asm
	{
		POP ECX
			MOV EAX, DWORD PTR[ECX + 4] // func
			MOV ECX, [ECX]               // this  
			JMP EAX
	}
}

static LONG_PTR CalcJmpOffset(LONG_PTR Src, LONG_PTR Dest)
{
	return Dest - (Src + 5);
}

/*
_TPStdFunc:		a type of function pointer to API or Callbacks, *MUST* be _stdcall
һ��API������ϵͳ�ص�����ָ������ͣ�ת����Ľ������
_TClass:		the class who owns the callback function. The caller should only own the _stdcall function pointer
LIFE TIME: It is important to keep the ACCallback object alive until the CALLBACK is not required!!!
����_TPMemberFunc��ָ�������ࣻ����ת������
_TPMemberFunc:	a type of function pointer to class member function,
*MUST* be the *DEFAULT* calling conversation, *NO* prefix should be added,
that is, using ECX for "this" pointer, pushing parameters from right to left,
and the callee cleans the stack.
һ�����Ա����ָ������ͣ����Ա�����ĵ�ַ
*/
template<typename _TPStdFunc, class _TClass, typename _TPMemberFunc>
class ACCallback
{
public:
	_TClass *m_pThis;
	_TPMemberFunc m_pFunc;

private:
	_TPStdFunc m_pStdFunc;

	void MakeCode()
	{
		if (m_pStdFunc)
			::VirtualFree(m_pStdFunc, 0, MEM_RELEASE);
		m_pStdFunc = (_TPStdFunc)::VirtualAlloc(nullptr, sizeof(_ACCallbackOpCodes), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		_ACCallbackOpCodes *p = (_ACCallbackOpCodes *)m_pStdFunc;
		p->_func = *(LONG_PTR *)&m_pFunc;
		p->_this = (LONG_PTR)m_pThis;
		p->tag = 0xE8;
		p->offset = CalcJmpOffset((LONG_PTR)p, (LONG_PTR)STDACJMPProc);
	}

public:
	ACCallback<_TPStdFunc, _TClass, _TPMemberFunc>()
	{
	}
	ACCallback<_TPStdFunc, _TClass, _TPMemberFunc>(_TClass* pThis, _TPMemberFunc pFunc)
	{
		m_pFunc = pFunc;
		m_pThis = pThis;
		m_pStdFunc = nullptr;
		MakeCode();
	}
	void Assign(_TClass* pThis, _TPMemberFunc pFunc)
	{
		m_pFunc = pFunc;
		m_pThis = pThis;
		m_pStdFunc = nullptr;
		MakeCode();
	}
	~ACCallback<_TPStdFunc, _TClass, _TPMemberFunc>()
	{
		::VirtualFree(m_pStdFunc, 0, MEM_RELEASE);
	}
	operator _TPStdFunc()
	{
		return m_pStdFunc;
	}
};

/*ʾ��
class CTestCallback
{
private:
typedef void (CTestCallback::*CLASSTIMERPROC)(HWND, UINT, UINT_PTR, DWORD);
void OnTimer (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
ACCallback<TIMERPROC, CTestCallback, CLASSTIMERPROC> m_DOnTimer;
}
*/
//����ʱ��ֻҪ����д��
///* ��ʼ���ص��ṹ */
//m_DOnTimer.Assign(this, &CTestCallback::OnTimer);
//m_uid = ::SetTimer( nullptr, 0, 1000, m_DOnTimer);

struct MouseClick
{
	WPARAM wParam;
	LPARAM lParam;
};
class COFInputDlg;
class CxComboBox :public CxSubClassWnd
{
public:
	CxComboBox(COFInputDlg&);
	~CxComboBox();
private:
	//ref
	COFInputDlg &m_ref;
	//�༭�����Ƿ���ʾ�ָ�����������
	bool m_bTextShowValue;														
	//��ֹ�Ҽ��˵�  
	bool m_bNoPopupMenu;														
	//��ֹ����  
	bool m_bNoUndo;     														    
	//��ֹճ��������
	bool m_bNoPasteCut; 														
	//����ɾ���ͻ���
	bool m_bNoDelAndBack;														
	//ֻ��
	bool m_bReadOnly;		
	//���ַָ�����Էָ��Ϊ����ʾ�����ֶ�
	wchar_t m_SplitChar;		
	//�û��Ƿ��������Զ����
	bool m_bAutoComplete;
	//��ǰ״���Ƿ����ִ���Զ����
	bool m_bCanAutoComplete;													
	//
	bool m_bTimerEnabled;
	//
	bool m_bMouseClicked;
	//
	MouseClick MouseClk;
	//
	HWND m_EditHandle;
	WNDPROC m_OldEditWndProc;
	typedef LRESULT(CxComboBox::*CLASSWNDPROC)(HWND, UINT, WPARAM, LPARAM);
	LRESULT EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	ACCallback<WNDPROC, CxComboBox, CLASSWNDPROC> m_dEditWndProc;
private:
	bool m_bHover;
	bool m_bFocus;
	bool m_bTracking;
	HBRUSH m_ClientBrush;
	HBRUSH m_hOrderBrush;
	HBRUSH m_HotOrderBrush1;
	HBRUSH m_HotOrderBrush2;

	CxKeyBoardButton m_btn;
protected:
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void SetIsClick(bool Value);
	void DoLBtnClick(UINT nFlags, POINT point);
	wstring GetValue(int Index);
public:
	void InitCombo(HWND hwnd,bool bShowValue = false);
	int SelectString(int nStartAfter, LPCTSTR lpszString);
	int GetCurSel();
	int SetCurSel(int nSelect);
	void GetLBText(int nIndex, wstring& rString);
	int FindString(_In_ int nStartAfter, _In_z_ LPCTSTR lpszString);
	void ResetContent();
	int AddString(LPCTSTR lpszItem);
	int AddString(wstring Name, wstring Value);
	BOOL SetEditSel(_In_ int nStartChar, _In_ int nEndChar);
public:
	bool IsReadOnly();
	void ReadOnly(bool Value = true);
	wstring GetName(int Index);
	void SetSplitChar(char Value = (' '));
	//ʹValue������ʾ, AuotListWidth:List����Ӧ�ı����
	void AlighValue(bool AuotListWidth = true);		
};

