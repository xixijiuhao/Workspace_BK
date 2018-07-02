#ifndef _CX_SPIN_EDIT_H_
#define _CX_SPIN_EDIT_H_

class CxSpinEdit
{
public:
	void Create(HWND hParent, UINT editStyle = 0, UINT editID = 0, UINT spinID = 0, UINT style = CxEdit::intStyle, bool bSetBuddy=true);
	void SetRange(int lower, int upper);
	void SetLimitText(int length);
	void SetFont(HFONT font);
	void MoveWindow(int x, int y, int cx, int cy);
	void SetWindowNumber(int number);
	int  GetWindowNumber();
	HWND GetEditHwnd(){ return m_edit.GetHwnd(); }
	HWND GetSpinHwnd(){ return m_spin.GetHwnd(); }
	void SetText(const char *pszText){ m_edit.SetText(pszText); }
	void GetText(char *pszText, int length){ m_edit.GetText(pszText, length); }

	void SetText(const wchar_t *pszText){ m_edit.SetText(pszText); }
	void GetText(wchar_t *pszText, int length){ m_edit.GetText(pszText, length); }
	void Show(bool bShow);
	double GetDoubleData(){ return m_edit.GetDoubleData(); }
	bool IsText();
	void Enable(bool bEnable =true);
private:
	CxSpin     m_spin;
	CxEdit     m_edit;
};

#endif